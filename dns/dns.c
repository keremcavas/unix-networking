#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "dns.h"

// Internal definitions
#define LIST_SIZE	10
#define LINE_SIZE	100
#define BUFF_SIZE	65536
#define RSP_SIZE	16
#define DNS_PORT	53

// Internal global variables
// char dns_servers[LIST_SIZE][LINE_SIZE] = { "208.67.222.222", "208.67.220.220", "13.239.157.177", "8.26.56.26"};
char dns_servers[LIST_SIZE][LINE_SIZE] = { "208.67.222.222", "208.67.220.220", "13.239.157.177", "8.8.8.8"};
int dns_server_count = 4;
int dst_dns_server = 3;

// Internal function definitions
void fillQueryHeader(struct DNS_HEADER *);
void fillInverseQueryHeader(struct DNS_HEADER *);
void printResponseHeaderInfo(struct DNS_HEADER*);

void readAnswers(unsigned char  *, unsigned char  *, struct RES_RECORD *, int, int *);
void readAuthorities(unsigned char  *, unsigned char  *, struct RES_RECORD *, int, int *);
void readAdditional(unsigned char  *, unsigned char  *, struct RES_RECORD *, int, int *);

void printAnswers(struct RES_RECORD *, int);
void printAuthorities(struct RES_RECORD *, int);
void printAddRR(struct RES_RECORD *, int);


/*
 * Perform a standard DNS query by sending a packet
 * 
 */
void sendDNSQuery(unsigned char *host , int query_type)
{
    unsigned char buf[BUFF_SIZE], *qname, *reader;
    int stop, s, sockSize;

    struct sockaddr_in dest;
 
    struct RES_RECORD answers[RSP_SIZE], auth[RSP_SIZE], addit[RSP_SIZE]; // possible replies from the DNS server
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    // Creating socket and filling socket structure 
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); 
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DNS_PORT);
    dest.sin_addr.s_addr = inet_addr(dns_servers[dst_dns_server]); 
    sockSize = sizeof(dest);
 
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;
    fillQueryHeader(dns);

    // jump to query section
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
 
    // convert hostname to dns style
    // replace dots with length of the following string
    dotted2DNS(qname, host);

    // Set query type (User supplied, A) and class (Internet)
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it 
    qinfo->qtype = htons( query_type );	
    qinfo->qclass = htons(1); 
 
    printf("\nSending Packet to %s...", dns_servers[dst_dns_server]);
    if( sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sockSize) < 0)  {
        perror("sendto failed");
    }
    printf("Done");

    // Receive the answer
    printf("\nReceiving answer...");
    if(recvfrom (s, (char*)buf, BUFF_SIZE, 0, (struct sockaddr*)&dest, (socklen_t*)&sockSize ) < 0) {
        perror("recvfrom failed");
    }
    printf("Done");
 
    dns = (struct DNS_HEADER*) buf;
    printResponseHeaderInfo(dns);  
 
    // jump to answer section ahead of the dns header and the query field
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
    stop=0;

    //Start reading answers
    readAnswers(reader, buf, answers, ntohs(dns->ans_count), &stop);
 
    // read authorities
    readAuthorities(reader, buf, auth, ntohs(dns->auth_count), &stop);

    //read additional
    readAdditional(reader, buf, addit, ntohs(dns->add_count), &stop);

    //print answers
    printAnswers(answers, ntohs(dns->ans_count));

    //print authorities
    printAuthorities(auth, ntohs(dns->auth_count));

    //print additional resource records
    printAddRR(addit, ntohs(dns->add_count));

    return;
}

void sendInverseQuery(unsigned char *addr , int query_type)
{
    unsigned char buf[BUFF_SIZE], *qname, *reader;
    int stop, s, sockSize;

    struct sockaddr_in dest;
 
    struct RES_RECORD answers[RSP_SIZE], auth[RSP_SIZE], addit[RSP_SIZE]; // possible replies from the DNS server
    struct DNS_HEADER *dns = NULL;
    // struct QUESTION *qinfo = NULL;
    struct RES_RECORD *inverse = NULL;

    // Creating socket and filling socket structure 
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); 
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DNS_PORT);
    dest.sin_addr.s_addr = inet_addr(dns_servers[dst_dns_server]); 
    sockSize = sizeof(dest);
 
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;
    fillInverseQueryHeader(dns);

    // jump to ANSWER section
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
 
	/*
    // convert hostname to dns style
    // replace dots with length of the following string
    dotted2DNS(qname, host);

    // Set query type (User supplied, A) and class (Internet)
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it 
    qinfo->qtype = htons( query_type );	
    qinfo->qclass = htons(1); 
    * */
    
    inverse = (struct RES_RECORD *) &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
    inverse->name = "asd.qwe";
    // inverse->rdata = inet_pton(addr);
    char ip[4];
    inet_pton(AF_INET, addr, ip);
    inverse->rdata = ip;
    
    struct R_DATA *inverse_resource = (struct R_DATA *) malloc(sizeof(struct R_DATA));
    inverse_resource->type = T_A;
    inverse_resource->_class = 1;
    inverse_resource->ttl = 0;
    inverse_resource->data_len = 4;
    
    inverse->resource = inverse_resource;
 
    printf("\nSending Packet to %s...", dns_servers[dst_dns_server]);
    if( sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sockSize) < 0)  {
        perror("sendto failed");
    }
    printf("Done");

    // Receive the answer
    printf("\nReceiving answer...");
    if(recvfrom (s, (char*)buf, BUFF_SIZE, 0, (struct sockaddr*)&dest, (socklen_t*)&sockSize ) < 0) {
        perror("recvfrom failed");
    }
    printf("Done");
 
    dns = (struct DNS_HEADER*) buf;
    printResponseHeaderInfo(dns);  
 
    // jump to QUESTION section ahead of the dns header and the query field
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1)];
    stop=0;

    //Start reading answers
    readAnswers(reader, buf, answers, ntohs(dns->ans_count), &stop);
 
    // read authorities
    readAuthorities(reader, buf, auth, ntohs(dns->auth_count), &stop);

    //read additional
    readAdditional(reader, buf, addit, ntohs(dns->add_count), &stop);

    //print answers
    printAnswers(answers, ntohs(dns->ans_count));

    //print authorities
    printAuthorities(auth, ntohs(dns->auth_count));

    //print additional resource records
    printAddRR(addit, ntohs(dns->add_count));

    return;
}

/*
 * Filling header structure of the request 
 * 
 * 
 */

void fillQueryHeader(struct DNS_HEADER *reqHeader) {

    // Filling the header structure
    reqHeader->id = (unsigned short) htons(getpid());	// resolvers PID
    reqHeader->qr = 0; 					// This is a query
    reqHeader->opcode = 0; 				// This is a standard query
    reqHeader->aa = 0; 					// Not Authoritative
    reqHeader->tc = 0; 					// This message is not truncated
    reqHeader->rd = 1; 					// Recursion Desired
    reqHeader->ra = 0; 					// Not possible in a request so Recursion not available!
    reqHeader->z = 0;
    reqHeader->ad = 0;
    reqHeader->cd = 0;
    reqHeader->rcode = 0;
    reqHeader->q_count = htons(1); 			// Just one question !
    reqHeader->ans_count = 0;
    reqHeader->auth_count = 0;
    reqHeader->add_count = 0;

}

void fillInverseQueryHeader(struct DNS_HEADER *reqHeader) {

    // Filling the header structure
    reqHeader->id = 997; // (unsigned short) htons(getpid());	// resolvers PID
    reqHeader->qr = 0; 					// This is a query
    reqHeader->opcode = 1; 				// This is a standard query
    reqHeader->aa = 0; 					// Not Authoritative
    reqHeader->tc = 0; 					// This message is not truncated
    reqHeader->rd = 1; 					// Recursion Desired
    reqHeader->ra = 0; 					// Not possible in a request so Recursion not available!
    reqHeader->z = 0;
    reqHeader->ad = 0;
    reqHeader->cd = 0;
    reqHeader->rcode = 0;
    reqHeader->q_count = htons(1); 			// Just one question !
    reqHeader->ans_count = 0;
    reqHeader->auth_count = 0;
    reqHeader->add_count = 0;

}


/*
 * Print Header Info
 * 
 * 
 */ 

void printResponseHeaderInfo(struct DNS_HEADER *rspHeader)  {

    printf("\nThe response contains : ");
    printf("\n %d Questions.",ntohs(rspHeader->q_count));
    printf("\n %d Answers.",ntohs(rspHeader->ans_count));
    printf("\n %d Authoritative Servers.",ntohs(rspHeader->auth_count));
    printf("\n %d Additional records.\n\n",ntohs(rspHeader->add_count));

}

/* 
 * Read Answers
 *
 *
 */
void readAnswers(unsigned char  *reader, unsigned char  *buf, struct RES_RECORD *answers, int cnt, int *stop)  {

    int i, j;

    for(i = 0;i < cnt;i++)  {
        answers[i].name=DNS2dotted(reader, buf, stop);
        reader = reader + *stop;
 
        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);
 
	// check whether its an ipv4 address
        if(ntohs(answers[i].resource->type) == T_A)  {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
 
            for(j = 0 ; j < ntohs(answers[i].resource->data_len); j++)  {
                answers[i].rdata[j]=reader[j];
            }
 
            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
            reader = reader + ntohs(answers[i].resource->data_len);

        }  else  {
            answers[i].rdata = DNS2dotted(reader, buf, stop);
            reader = reader + *stop;
        }
    }

}

/* 
 * Read Authorities 
 *
 *
 */
void readAuthorities(unsigned char  *reader, unsigned char  *buf, struct RES_RECORD *auth, int cnt, int *stop)  {

    int i;

    for(i=0;i< cnt;i++) {
        auth[i].name=DNS2dotted(reader, buf, stop);
        reader+=*stop;
 
        auth[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);
 
        auth[i].rdata=DNS2dotted(reader, buf, stop);
        reader+=*stop;
    }

}

/* 
 * Read Additional
 *
 *
 */
void readAdditional(unsigned char  *reader, unsigned char  *buf, struct RES_RECORD *addit, int cnt, int *stop)  {

    int i, j;

    for(i = 0; i < cnt; i++)  {

        addit[i].name=DNS2dotted(reader, buf, stop);
        reader+=*stop;
 
        addit[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);
 
        if(ntohs(addit[i].resource->type)==1)  {
            addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
            for(j = 0; j < ntohs(addit[i].resource->data_len); j++)
            	addit[i].rdata[j]=reader[j];
 
            addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
            reader+=ntohs(addit[i].resource->data_len);
        } else  {
            addit[i].rdata=DNS2dotted(reader, buf, stop);
            reader+=*stop;
        }
    }
}

/* 
 * Print answers
 *
 *
 */

void printAnswers(struct RES_RECORD *answers, int cnt)  {

    int i;
    long *p;
    struct sockaddr_in tmpSock;

    printf("\nAnswer Records : %d \n" , cnt);
    for(i=0 ; i < cnt ; i++)  {

        printf("Name : %s ",answers[i].name);
        if( ntohs(answers[i].resource->type) == T_A)  {  		// IPv4 address
            p=(long*)answers[i].rdata;
            tmpSock.sin_addr.s_addr=(*p); 				// working without ntohl
            printf("has IPv4 address : %s",inet_ntoa(tmpSock.sin_addr));
        }
         
        if(ntohs(answers[i].resource->type)== T_CNAME)  {			// Canonical name for an alias
            printf("has alias name : %s",answers[i].rdata);
        }
 
        printf("\n");
    }

}


/* 
 * Print authorities
 *
 *
 */
void printAuthorities(struct RES_RECORD *auth, int cnt)  {

    int i;

    printf("\nAuthoritive Records : %d \n",  cnt);
    for( i=0 ; i < cnt; i++)  {
        printf("Name : %s ",auth[i].name);
        if(ntohs(auth[i].resource->type)==2)  {
            printf("has nameserver : %s",auth[i].rdata);
        }
        printf("\n");
    }
}


/* 
 * Print additional RR
 *
 *
 */
void printAddRR(struct RES_RECORD *addit, int cnt)  {

    int i;
    long *p;
    struct sockaddr_in tmpSock;

    printf("\nAdditional Records : %d \n" ,  cnt);
    for(i=0; i < cnt ; i++) {
        printf("Name : %s ",addit[i].name);
        if(ntohs(addit[i].resource->type)==1)  { 
            p=(long*)addit[i].rdata;
            tmpSock.sin_addr.s_addr=(*p);
            printf("has IPv4 address : %s",inet_ntoa(tmpSock.sin_addr));
        }
        printf("\n");
    }


} 

 
/*
 * This will convert www.google.com to 3www6google3com 
 * 
 */

void dotted2DNS(unsigned char *dns, unsigned char *host)  {
    int lock = 0 , i;
    strcat((char*)host,".");
   
    for(i = 0 ; i < strlen((char*)host) ; i++)  {
        if(host[i]=='.')  {

            *dns++ = i-lock;
            for(;lock<i;lock++)  {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}


/*
 * This will convert 3www6google3com to www.google.com
 * 
 */
unsigned char *DNS2dotted(unsigned char *reader, unsigned char *buffer, int *count)  {

    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i, j;
 
    *count = 1;
    name = (unsigned char*)malloc(256);
    name[0]='\0';
 
    // read the names in 3www6google3com format
    while(*reader!=0)  {
        if(*reader>=192)  {
            offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        } else  {
            name[p++]=*reader;
        }
 
        reader = reader+1;

        if(jumped==0)  {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }
    name[p]='\0'; //string complete

    if(jumped==1)  {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }
 
    // now convert 3www6google3com0 to www.google.com
    for(i=0;i<(int)strlen((const char*)name);i++)  {
        p=name[i];
        for(j=0;j<(int)p;j++)  {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0'; //remove the last dot
    return name;
}
 


/*
 * Get the DNS servers from /etc/resolv.conf file on Linux
 * 
 */
void getLocalDnsServers() {
    FILE *fp;
    char line[LINE_SIZE] , *p;

    if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)  {
        printf("Failed opening /etc/resolv.conf file... \n");
    }
     
    while(fgets(line, LINE_SIZE, fp))  {
        if(line[0] != '#') {
            if(strncmp(line , "nameserver" , 10) == 0) {
                p = strtok(line, " ");
                p = strtok(NULL, "\n");

     	        printf("found a nameserver from resolv.conf file : %s\n", p);
   	        strcpy(dns_servers[dns_server_count], p);
	        dns_server_count++;
             }
        }

    }
     
}

/*
 * List all DNS servers on the array
 *
 *
 */ 
void printAllDnsServers(void)  {

	int i;

	for(i = 0; i < dns_server_count; i++)  {

		printf("%d - %s\n", i, dns_servers[i]);

	}

}


