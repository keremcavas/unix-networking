#include <stdio.h> 	

#include "dns.h"

// Definition of menu items
#define LST_LCL_DNS	1 
#define LST_ALL_DNS	2
#define RFH_LCL_DNS	3
#define SLT_DST_DNS	4
#define SND_STD_QRY	5
#define SND_RVS_QRY	6 

int printMenu(void);

int main( int argc , char *argv[])  {

    int rc=1;
    unsigned char hostname[100];
 
    //Get the DNS servers from the resolv.conf file
    // get_dns_servers();

    while(rc)  {
 
        rc = printMenu();
	printf("rc is %d\n", rc);
        switch(rc)  {

        case LST_LCL_DNS : 
		getLocalDnsServers();
        break;

        case LST_ALL_DNS : 
		printAllDnsServers();
        break;

        case RFH_LCL_DNS : 
        break;

        case SLT_DST_DNS :
        break;

        case SND_STD_QRY : 
            // Get the hostname from the terminal
            printf("Enter Hostname to Lookup : ");
            scanf("%s" , hostname);
     
            // Now get the ip of this hostname , A record
            sendDNSQuery(hostname , T_A);
        break;

        case SND_RVS_QRY :
			printf("Enter IP to Lookup : ");
			scanf("%s" , hostname);
			sendInverseQuery(hostname, T_A);
        break;
	
	default : 
		printf("wrong selection!\n");
	break;
        }

    }

     
 
    return 0;
}


int printMenu(void)  {

	int selection;
	printf("\n-------- MAIN MENU --------\n");
	printf("%d - List local DNS servers\n", LST_LCL_DNS);
	printf("%d - List all DNS servers\n", LST_ALL_DNS);
	printf("%d - Refresh local DNS servers\n", RFH_LCL_DNS);
	printf("%d - Select target DNS server\n", SLT_DST_DNS);
	printf("%d - Send a standard query\n", SND_STD_QRY);
	printf("%d - Send an inverse query\n", SND_RVS_QRY);
	printf("0 - Exit");
	printf("\n\n>");
	scanf("%d", &selection);

	return selection;
}
 
