#include "proto.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLEN 1024
#define ICS_FILE "example.ics"

int main (int argc, int **argv){
  FILE *icsfile;
  char buffer[1024];
  int read_return,i,offset=0;
  char *icsbuf;
    
  if ((icsfile = fopen(ICS_FILE, "r")) != NULL){
    /* file opened successfully */
    /* start to allocate buffer for copying (bourrin mode) */
    icsbuf = malloc(sizeof(buffer));
    /* clear buffer before reading */
    memset(buffer,'\0',sizeof(buffer));
    while ((read_return = read(fileno(icsfile),&buffer,sizeof(buffer))) != 0 && read_return != -1){
      /* buffer may contain less than sizeof(buffer) */
      for(i=0; i<=sizeof(buffer) && buffer[i]!='\0'; i++){
	icsbuf[offset+i] = buffer[i];
      }
      /* clear buffer after reading */
      memset(buffer, '\0', sizeof(buffer));
      offset+=sizeof(buffer);
      if(realloc(icsbuf,offset+sizeof(buffer)) == NULL){/* alloc error happened */
      printf("an error as occured while allocating memory");
      exit(-1);
    }
    }
    /* file exhausted, icsbuf filled with file data */
    printf("%s",icsbuf);
    /* no longer need file data, memory can be freed */
    free(icsbuf);
  }
  else{ /* catch error */
    printf("an error has occured on %s opening: %s\n", ICS_FILE, strerror(errno));
    exit(-1);
  }
}
