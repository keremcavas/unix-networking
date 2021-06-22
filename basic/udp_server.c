#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFLEN 512
#define PORT 9930
#define NPACK 10
/*
struct in_addr {
	unsigned long s_addr;
}

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
}
*/

struct sockadrr_in {
	short int sin_family;
	unsigned short int sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero[8];
};

void diep(char *s) {
	perror(s);
	exit(1);
}

int main(int argc, char **argv)
{
	
	struct sockadrr_in si_me, si_other;
	int s, i, slen = sizeof(si_other);
	char buf[BUFLEN];
	
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		diep("socket");
	}
	
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(s, (const struct sockadrr *) &si_me, sizeof(si_me)) == -1) {
		diep("bind");
	}
	
	for (i = 0; i < NPACK; i++) {
		if (recvfrom(s, buf, BUFLEN, 0, (const struct sockadrr *) &si_other, &slen) == -1) {
			diep("recvfrom()");
		}
		printf("Received packet from: %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data:\n%s\n\n", buf);
	}
	
	close(s);
	
	return 0;
}
