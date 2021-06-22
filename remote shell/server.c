#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "misc.h"

#define LINE_SIZE 	255
#define SCREEN_SIZE	1024
#define PRM_LEN		32
#define PRM_NUM		100
#define BACK_LOG	5

int main(int argc, char *argv[])  {

    int i, n, child_pid;
    int opt, argCnt=0, processCnt;
    int sockfd, newsockfd, portno, clilen; 	// int variable that is used later
    struct sockaddr_in serv_addr, cli_addr; 	//calling the library struct
    char buffer[LINE_SIZE]; 			//buffer of size created
    char **args;
    char prog[32];


    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
           switch (opt) {
               case 'n':
                   processCnt = atoi(optarg);
		   argCnt++;
                   break;
               case 'p':
                   portno = atoi(optarg);
		   argCnt++;
                   break;
               default: /* '?' */
                   fprintf(stderr, "Usage: %s [-a ip_address] [-p port_number] \n", argv[0]);
               exit(EXIT_FAILURE);
          }
    }

    if(argCnt != 2 )  {
       error("ERR>> missing arguments");
    }

    //1st IP Address 2nd TCP Concept 3rd Socket
    sockfd= socket(AF_INET, SOCK_STREAM,0);
    if(sockfd == -1)  {
      error("ERR>> socket creation failed");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // convert and use port number 
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)  {
      error("ERR>> can not bind to socket");
    }

    listen(sockfd, BACK_LOG);
    clilen = sizeof(cli_addr);

	int total_process = 0;

    //
    //The new socket for the client informations
    //
    while (total_process <= processCnt) {
    
		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
		if (newsockfd < 0)  {
			  error("ERR>> can not accept");
		}
		
		total_process++;
		
		int fork_pid = fork();
		
		if (fork_pid == 0) {

			while (1) {
				bzero(buffer, LINE_SIZE); //Clears the buffer
				n = recv(newsockfd, buffer, LINE_SIZE, 0);
				if (n < 0)  {
					error("ERR>> can not read from socket");
				}
				
				if (buffer[0] == '\0') {
					continue;
				}

				//Buffer Stores the msg sent by the client
				printf("Here is the entered bash command: %s\n",buffer);

				/*
				n = send(newsockfd, "I got your message", 18, 0);
				if (n < 0)  {
					error("ERROR writing to socket");
				}
				* */

				args = malloc(PRM_NUM * sizeof(char *));
				for(i = 0;i < PRM_NUM; i++)
				args[i]=malloc(PRM_LEN *sizeof(char));

				// buffer backup
				char buffer_backup[5000];
				strcpy(buffer_backup, buffer);

				//Running the Bash Commands
				if(readAndParseCmdLine(buffer, prog, args)) {
					/*
					int link[2];
					char output[2000];
					
					pipe(link);
					
					child_pid =fork();
					

					if(child_pid == 0){ //child part
						execvp(prog, args);  // create and run the new process and close the child process
						printf("Error in excuting the command- please make sure you type the right syntax.\n");
					} else{ //parent part
						int n = read(link[0], output, sizeof(output));
						printf("Output: (%.*s)\n", n, output);
						wait(&child_pid);
					}
					* */
					
					char output[5000];
					char line[1000];
					
					bzero(output, 4990);
					bzero(line, 990);
					
					FILE *fp = popen(buffer_backup, "r");
					if (fp == NULL) {
						printf("failed to run\n");
					} else {
						while (fgets(line, sizeof(line), fp) != NULL) {
							strcat(output, line);
						}
						printf("Output: (%s)\n", output);
					}
					
					if (strlen(output) > 0) {
						n = send(newsockfd, output, strlen(output), 0);
						if (n < 0)  {
							error("ERROR writing to socket");
						}
					} else {
						n = send(newsockfd, "No output", 10, 0);
						if (n < 0)  {
							error("ERROR writing to socket");
						}
					}
					
				}
			}
		}
	}
	
	printf ("Maximum process count exceeded...\n");
	close(newsockfd);

}


