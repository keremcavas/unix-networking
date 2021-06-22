#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * ziyacihantaysi@gmail.com
 * BLM4900 - DNS
 * BLM4900 - RSH
 * no, ad, soyad 
 * adece kod dosyalarınız
 * 
 * */

#define PORT 3490
#define BACKLOG 10 // pending connection queue

struct sockadrr_in {
	short int sin_family;
	unsigned short int sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero[8];
};

int main(int argc, char **argv)
{
	
	int sockfd, new_fd;
	struct sockadrr_in my_addr;
	struct sockadrr_in their_addr; // client
	socklen_t sin_size;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);
	
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}
	
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	
	while (1) {
		sin_size = sizeof(struct sockadrr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size)) == -1) {
			perror("accept");
			exit(1);
		}
		
		if (fork() == 0) {
			
			printf("[server] got connection from: %s\n", inet_ntoa(their_addr.sin_addr));
			
			if (send(new_fd, "Hello world!\n", 14, 0) == -1) {
				perror("send");
			}
			
			close(new_fd);
			exit(0);
		}
	}
	
	return 0;
}

