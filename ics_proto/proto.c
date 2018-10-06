#include "proto.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define _IC_LEN_LOGICAL 1024
#define _IC_LEN_ELT _IC_LEN_LOGICAL/2 // approx
#define ICS_FILE "example.ics"

/* prototypes (will be included in proto.h */
/* private */
char *_file2mem(char *filename);
char *_get_next_line(char *icsbuf);
/* public */
struct icContline ic_analyze_contentline(char *contentline);
void get_ic_events (char *icsbuf);

/* structs definitions */
/* public */
struct icContline {
  char name[_IC_LEN_ELT];
  char **params;
  char value[_IC_LEN_ELT];
};
  
/* general variables */
char curline[_IC_LEN_LOGICAL]; // used to store iCalendar logical lines, we
		      // assume that no line will be longer than
		      // _IC_LEN_LOGICAL 

/* main fun */
int main (int arc, int **argv){
  char *icsbuf; // ics buf ptr
  
  if ((icsbuf = _file2mem(ICS_FILE)) != NULL){ // returned ok (_file2mem
					       // allocates
    /* icsbuf ready to use and filled */
    get_ic_events(icsbuf);
    /* buffer no longer used */
    free(icsbuf);
  }else{exit(-1);/* not ok, terminate */}
  
}

void /* for now */ get_ic_events(char *icsbuf){
  char *icsline; // pointer to global _currentline
  struct icContline linecontents;
  
  while ((icsline = _get_next_line(icsbuf)) != NULL){
    /* Analyze the line. We're
     * not covering the whole iCalendar standard here, but only
     * "BEGIN", "END", "CALSCALE", "DTSTART", "DTEND", "SUMMARY",
     * "LOCATION", "DESCRIPTION" names (most useful) */
    linecontents = ic_analyze_contentline(icsline);
  }
}


/* reads a contentline in iCalendar format,
 * return a struct with its fields. contentline
 * must be '\0'-terminated and syntaxically correct,
 * no sanity check is performed ! (bad) */
struct icContline ic_analyze_contentline(char *contentline){
  int offset = 0;
  int value_off = 0, name_off = 0;
  char dbl_point_cnt = 0, pt_comma_cnt = 0, eq_cnt = 0, comma_cnt = 0;
  struct icContline analysis;
  
  /* contentline   = name *(";" param ) ":" value CRLF */
  /* param         = param-name "=" param-value *("," param-value) */

  /* init struct members */
  memset(analysis.name,'\0',sizeof(analysis.name));
  memset(analysis.value,'\0',sizeof(analysis.value));
    
  while (contentline[offset] != '\0'){
    /* delimiters from left to right,
     * determines if we have hit a delimiter, count it and skip it
     * (we do not want to copy it) */
    switch (contentline[offset]){
    case ';': /* param delimiter */            ++pt_comma_cnt;  ++offset; break;
    case ':': /* name and params delimiter */  ++dbl_point_cnt; ++offset; break;
    case '=': /* param name delimiter */       ++eq_cnt;        ++offset; break;
    case ',': /* params value delimiter */     ++comma_cnt;     ++offset; break;
    }
    if(pt_comma_cnt == 0 && dbl_point_cnt == 0){
      /* still in name part, copy name */
      analysis.name[name_off++] = contentline[offset];
    }else if (pt_comma_cnt > 0 && dbl_point_cnt == 0){
      /* param part, undefined behaviour (not implemented) */
    }else if (pt_comma_cnt > 0 || dbl_point_cnt > 0){
      /* parameter is optional, so pt_comma_cnt may be == 0 */
      /* value part, copy value */
      analysis.value[value_off++] = contentline[offset];
    }
    ++offset;
  }
  return analysis;
}

/* reads a buffer in iCalendar format, unwrap
 * and returns each line */
char *_get_next_line(char *icsbuf){
  int linebuf_off;
  static int buf_off=0; // save buffer offset between each call

  /* read everything */
  linebuf_off=0; // init offset in curline
  for(/* buf_off hasn't to be reinitialized */; icsbuf[buf_off]!='\0'; buf_off++){
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
	buf_off+=2; // discard '\n' and ' ' (don't read), continue
		    // reading
      }else{ // wasn't a ' '
	// so we assume that both logical and physical line ended
	/* 1 finish up */
	curline[linebuf_off] = '\0'; // current is '\r'
	/* 2 reset and prepare for next line */
	linebuf_off = 0;
	/* we had '\r\n?' with '?' continuation or spc, we were at
	 * '\r', discard '\n' anyway */
	buf_off+=2; // 1 to discard '\n', another 1 because 3rd for
		    // loop statement won't be executed after re-entry
		    // in this function
	/* return line @ */
	return &curline[0];
      }
    }
  }
  /* end of buffer reached */
  return NULL;
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
