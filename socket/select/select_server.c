#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 7777

#define TRUE 1
#define FALSE 0
int main(int argc, char* argv[])
{
	
	int listen_sd, /*监听fd*/
		max_sd, /*最多需要查询的fd数量*/
		shutdown = FALSE, /*服务器是否关闭*/
		count, /*select()返回值*/
		len,
		newconn_sd = -1, /*新连接fd*/
		close_conn = FALSE, /*是否关闭客户端连接*/
		i, rc, on = 1; /*中间结果*/
	char buffer[256]; /*缓冲区*/
	struct sockaddr_in addr; /*服务端地址*/
	fd_set master_set, working_set;
	struct timeval timeout; /*服务端监听请求超时时间*/

	/*创建监听fd*/
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sd < 0)
	{
		perror("socket() failed");
		exit(-1);
	}

	/*设置重用？？*/
	rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	if(rc < 0) 
	{
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}

	/*设置为非阻塞模式*/
	rc = ioctl(listen_sd, FIONBIO, (char*)&on);
	if(rc < 0)
	{
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}

	/*绑定服务器端口*/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);
	rc = bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr));
	if(rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}

	/*开始监听*/
	rc = listen(listen_sd, 32);
	if(rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}

	FD_ZERO(&master_set);
	max_sd = listen_sd;
	// 注册监听事件
	FD_SET(listen_sd, &master_set);

	/*超时时间为1分钟*/
	timeout.tv_sec = 1 * 60;
	timeout.tv_usec = 0;

	do {
		memcpy(&working_set, &master_set, sizeof(master_set));
		printf("blocking on select()...\n");
		count = select(max_sd + 1, &working_set, NULL, NULL, &timeout);
		if(count < 0)
		{
			perror("select() failed()");
			break;
		}
		else if(count == 0)
		{
			// timeout
			perror("select() timeout ");
			break;
		}
		
		for(i = 0;i <= max_sd && count > 0;i++)
		{
			if(FD_ISSET(i, &working_set))
			{
				count--;
				if(i == listen_sd)
				{
					// 是监听fd的事件，即有新连接
					printf("listen_sd is readable now\n");
					do {
						newconn_sd = accept(listen_sd, NULL, NULL);
						if(newconn_sd < 0)
						{
							if(errno != EWOULDBLOCK)
							{
								perror("accept() failed");
								shutdown = TRUE;
							}
							break;
						}
						printf("new incoming connection: %d\n", newconn_sd);

						// FreeBSD会继承listen_sd的是否阻塞，但不通用。这里还需要重新设置！
						// http://comments.gmane.org/gmane.os.freebsd.devel.hackers/44063
						rc = ioctl(newconn_sd, FIONBIO, (char*)&on);
						if(rc < 0)
						{
							perror("ioctl() failed");
							close(newconn_sd);
							break;
						}

						// 注册新连接的读请求事件
						FD_SET(newconn_sd, &master_set);
						if(newconn_sd > max_sd)
						{
							max_sd = newconn_sd;
						}
					} while(newconn_sd != -1);
				}
				else 
				{
					// 有客户端发来的数据
					printf("connection %d is readable now\n", i);
					close_conn = FALSE;
					do {
						printf("receiving data from connection %d\n", i);
						rc = recv(i, buffer, sizeof(buffer), 0);
						printf("after receiving data from connection %d\n", i);
						if(rc < 0)
						{
							if(errno != EWOULDBLOCK)
							{
								perror("recv() failed");
								close_conn = TRUE;
							}
							break;
						}

						if(rc == 0)
						{
							printf("closing connection %d\n", i);
							close_conn = TRUE;
							break;
						}
						
						len = rc;
						printf("received %d bytes from connection %d\n", len, i);
						
						rc = send(i, buffer, len, 0);
						if(rc < 0)
						{
							perror("send() failed");
							close_conn = TRUE;
							break;
						}
					} while(TRUE);
					if(close_conn)
					{
						close(i);
						FD_CLR(i, &master_set);
						if(i == max_sd)
						{
							while(FD_ISSET(max_sd, &master_set) == FALSE)
							{
								max_sd--;
							}
						}
					}
				}
			}
		}
	} while(shutdown == FALSE);
	
	for(i = 0;i< max_sd;i++)
	{
		if(FD_ISSET(i, &master_set))
		{
			close(i);
		}
	}
	return 0;
}
