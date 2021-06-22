#include <stdio.h>
#include <pcap.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <time.h>

void print_packet_info(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet) {
	
	int size = pkthdr->len;
	
	struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));
	
	if (iph->protocol == 1) {
		
		time_t nowtime;
		struct tm *nowtm;
		char tmbuf[64], buf[64];

		nowtime = pkthdr->ts.tv_sec;
		
		nowtm = localtime(&nowtime);
		strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
		snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, pkthdr->ts.tv_usec);
		
		printf("time : %s\n", buf);
		
		struct in_addr src_addr;
		struct in_addr dst_addr;
		
		src_addr.s_addr = iph->saddr;
		dst_addr.s_addr = iph->daddr;
		
		printf("source ip: %s\n", inet_ntoa(src_addr));
		printf("destination ip: %s\n", inet_ntoa(dst_addr));
			
			
		printf ("type: %d\n\n\n", (char)(pkthdr + 34));
	}
	
}

int main(int argc, char **argv) {
	
	if(argc != 2) {
        printf("usage: %s filename", argv[0]);
        return -1;
    }
	
	pcap_t* descr;
	char error_buffer[1000];
	struct pcap_pkthdr packet_header;
	const u_char *packet;
	
	descr = pcap_open_offline(argv[1], error_buffer);
	
	if (descr == NULL) {
		printf("error when opening file\n\n");
		return -1;
	}
	
	
	pcap_loop(descr, 0, print_packet_info, NULL);
	
	
	/*
	while ((packet = pcap_next(descr, &packet_header))) {
		print_packet_info(packet, packet_header);
	}
	* */
}
