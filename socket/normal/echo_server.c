#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "util.h"
#include "sig.h"

#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
	int server_fd, client_fd;
	struct sockaddr_in server_addr;
	char buffer[1024];
	int tmp;
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	_printf("server socket created: %d\n", server_fd);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9999);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	_printf("binded to 0.0.0.0:9999\n");
	
	listen(server_fd, 16);
	_printf("listening with backlog=%d\n", 16);

	signal(SIGCHLD, sig_child);
	_printf("sig_child listening on signal SIGCHLD\n");
	
	while(client_fd = accept(server_fd, NULL, NULL))
	{
		_printf("accepted new connection: %d\n", client_fd);
		if(fork() == 0)
		{
			for(;;)
			{
				while((tmp = read(client_fd, buffer, BUFFER_SIZE)) > 0) 
				{
					buffer[tmp] = '\0';
					_printf("read string[%d] from client[%d]: %s\n", tmp, client_fd, buffer);
					write(client_fd, buffer, tmp);
					_printf("sent string back to client[%d]: %s\n", client_fd, buffer);
				}
				if(tmp == 0)
				{
					_printf("received EOF from client, exit normally.\n");
					break;	
				}
				else if(tmp < 0 && errno != EINTR)
				{
					_printf("read error!!\n");
				}
			}
			close(client_fd);
			_printf("close client connection: %d\n", client_fd);
			exit(0);
		}
		close(client_fd);
		_printf("close client connection: %d\n", client_fd);
	}

	close(server_fd);
	_printf("close server_fd: %d\n", server_fd);
	
	return 0;
}
