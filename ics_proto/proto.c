#include "proto.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLEN 1024
#define ICS_FILE "example.ics"

/* prototypes (will be included in proto.h */
char *_file2mem(char *filename);
void _readlines(char *icsbuffer);

/* main fun */
int main (int arc, int **argv){
  char *icsbuf;

  if ((icsbuf = _file2mem(ICS_FILE)) != NULL){
    smth(icsbuf);
    /* buffer no longer used */
    free(icsbuf);
  }else{exit(-1);/* not ok, terminate */}
  
}

void _readlines(char *icsbuffer){
  char curline[MAXLEN]; // we assume that even folded lines wil be no
			// longer than this
  int buf_off, linebuf_off;

  /* read everything */
  linebuf_off=0; // init offset in curline
  for(buf_off=0; icsbuf[buf_off]!='\0'; buf_off++){
    if(icsbuf[buf_off]!='\r' /* CR */){
      /* we're in a line, so copy */
      curline[linebuf_off++]=icsbuf[buf_off];
    }else{
      // physical line ended, let's special-case now
      // now, does logical line ended ?
      /* states :
       * (1) on endline
       * (2) line continues
       * (3) line finished, analyse and newline
       * 1 -> 2
       * 2 -> 1
       * 1 -> 3
       */
      // we're on side. test if (1) or (2). '?' is unknown char
      if (icsbuf[buf_off+2]==' '/* length of '\n' + 'SPC' */){
	// line continues
	buf_off+=2; // discard '\n' and ' ' (don't read)
      }else{ // wasn't a ' '
	// so we assume that both logical and physical line ended
	/* 1 finish up & analyze */
	curline[linebuf_off] = '\0'; // current is '\r'
	// printf is analyze "placeholder" for now
	printf("%s\n",&curline);
	/* 2 reset */
	linebuf_off = 0;
	/* we had '\r\n?' with '?' continuation or spc, we were at
	 * '\r', discard '\n' anyway */
	buf_off++;
      }
    }
  }
}

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
      if(realloc(icsbuf,offset+sizeof(buffer)) == NULL){/* alloc error happened */
      printf("an error as occured while allocating memory");
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
