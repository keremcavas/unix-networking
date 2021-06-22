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
#define SRV_IP "127.0.0.1"
#define NPACK 10

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
	
	struct sockadrr_in si_other;
	int s, i, slen = sizeof(si_other);
	char buf[BUFLEN];
	
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		diep("socket");
	}
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	if (inet_aton(SRV_IP, &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton failed\n");
		exit(1);
	}
	
	for (i = 0; i < NPACK; i++) {
		printf("Sending packet (%d)\n", i);
		sprintf(buf, "Packetttt %d\n", i);
		if (sendto(s, buf, BUFLEN, 0, (const struct sockadrr *) &si_other, slen) == -1) {
			diep("sendto");
		}
	}
	
	close(s);
	
	return 0;
}

