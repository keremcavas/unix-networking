#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"


void error(char *msg) {
  perror(msg);
  exit(1);
}


//
//This function reads the linux command form the buffer and parse it
//
int readAndParseCmdLine(char *buffer, char *prog, char *args[]){

    int i=0;
    char *tok;

 printf("buffer : %s\n", buffer);

    if(strcmp(buffer,"exit") == 0)  { 
        return 0;
    } //end program if user type exit.

    tok = strtok(buffer," \n");
    strcpy(prog,tok);

printf("prog : %s \n", prog);

    while(tok != NULL){
        strcpy(args[i], tok);
printf("i : %d - %s\n", i, args[i]);
	i++;
	tok = strtok(NULL, " \n");
    }

    args[i]=NULL;
    return 1;
}
