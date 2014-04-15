#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
	int sock_fd;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr;
	int len;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	_printf("client socket created: %d\n", sock_fd);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	_printf("connect to server successfully.\n");
	
	while(scanf("%s", buffer))
	{
		send(sock_fd, buffer, strlen(buffer), 0);
		
		if((len = recv(sock_fd, buffer, BUFFER_SIZE, 0)) == 0)
		{
			_printf("error: server terminated!\n");
			break;
		}
		_printf("receive from server: %s\n", buffer);
	}
	
	// close(sock_fd);
	exit(0);
	return 0;
}
