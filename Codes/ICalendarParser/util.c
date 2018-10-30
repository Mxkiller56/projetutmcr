#include "util.h"

/* returns a buffer with file contents (dégoulasse) */
char *_file2mem (char *filename){
  FILE *icsfile;
  char buffer[1024];
  int read_return,i,offset=0;
  char *icsbuf;
    
  if ((icsfile = fopen(filename, "r")) != NULL){
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
      if((icsbuf = realloc(icsbuf,offset+sizeof(buffer))) == NULL){/* alloc error happened */
      printf("an error has occured while allocating memory");
      return NULL;
    }
    }
    /* file exhausted, icsbuf filled with file data */
    return icsbuf;
  }
  else{ /* catch error */
    printf("an error has occured on %s opening: %s\n", filename, strerror(errno));
    return NULL;
  }
}
