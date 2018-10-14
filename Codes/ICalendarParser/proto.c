/* For more info on iCalendar, see RFC 5545
 * Doc here also: https://www.kanzaki.com/docs/ical/
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define _IC_LEN_LOGICAL 1024
#define _IC_LEN_ELT _IC_LEN_LOGICAL/2 // approx
#define _IC_LEN_NAME 20
#define ICS_FILE "example.ics"
/* shorthand */
#define array_len(array) sizeof(array)/sizeof(array[0])

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

struct icDate {
  /* we do not use standard C 'time'/epoch for compatibility with
     arduino-ide */
  int year; 
  int month;
  int day;
  int hours;
  int minutes;
  int seconds;
  /* may be extended later */
};

struct icVevent {
  char summary[_IC_LEN_ELT];
  char description[_IC_LEN_ELT];
  char location[_IC_LEN_ELT];
  struct icDate dtstamp;
  struct icDate dtstart;
  struct icDate dtend;
};
  
/* general variables */
static char curline[_IC_LEN_LOGICAL]; // used to store iCalendar logical lines, we
		      // assume that no line will be longer than
		      // _IC_LEN_LOGICAL
static const int true = 1, false = 0;

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

/* parse a date in the iCalendar format, returning a icDate
   struct. iCalendar date format is YYYYMMDDThhmmssZ*/
struct icDate
_ic_parse_date (char *datevalue)
{
  struct icDate current;
  char tmp[] = "\0\0\0\0";
  int i;
  struct _parser {
    char *start;
    size_t cpysize;
    int *storto;
  };
  struct _parser dv_parser [] = {
    {&datevalue[array_len("YYYYMMDDThhmm")-1],sizeof("ss")-1,  &current.seconds},
    {&datevalue[array_len("YYYYMMDDThh")-1],  sizeof("mm")-1,  &current.minutes},
    {&datevalue[array_len("YYYYMMDDT")-1],    sizeof("hh")-1,  &current.hours},
    {&datevalue[array_len("YYYYMM")-1],       sizeof("DD")-1,  &current.day},
    {&datevalue[array_len("YYYY")-1],         sizeof("MM")-1,  &current.month},
    {&datevalue[0],                           sizeof("YYYY")-1,&current.year}
  };
    
  /* do some format-validation. not checking for outbounds read, sorry */
  if (datevalue[array_len("YYYYMMDD")-1] == 'T'
      && datevalue[array_len("YYYYMMDDThhmmss")-1] == 'Z'){
    for (i=0; i<=array_len(dv_parser)-1; i++){
      memset(tmp,'\0',sizeof(tmp));
      memcpy(tmp, dv_parser[i].start, dv_parser[i].cpysize);
      *(dv_parser[i].storto) = atoi(tmp);
    }
  }
    
  return current;
}

void /* for now */ get_ic_events(char *icsbuf){
  char *ic_line; // pointer to global _currentline
  struct icContline linecontents;
  struct icVevent vevent;
  int in_vevent = false;
  

  while ((ic_line = _get_next_line(icsbuf)) != NULL){
    /* Analyze the iCalendar buffer, searching for iCalendar
     * objects. We're not covering the whole iCalendar standard here,
     * but only Vevents mostly */
    // TODO: Nested objects
    linecontents = ic_analyze_contentline(ic_line); // get it
    if(strcmp(linecontents.name,"BEGIN") == 0 &&
       strcmp(linecontents.value,"VEVENT") == 0) /* open new event */
      in_vevent = true;
    else if(strcmp(linecontents.name,"END") == 0 &&
	    strcmp(linecontents.value,"VEVENT") == 0)/* close event */
      in_vevent = false;
    if(in_vevent){
      if(strcmp(linecontents.name,"DTSTART") == 0){
	vevent.dtstart = _ic_parse_date(linecontents.value);
	/* Houston, this is a test */
	printf("year:%d month:%d day:%d hours:%d minutes:%d seconds:%d\n",vevent.dtstart.year,
	       vevent.dtstart.month,
	       vevent.dtstart.day,
	       vevent.dtstart.hours,
	       vevent.dtstart.minutes,
	       vevent.dtstart.seconds);
      }
    }
  }
}


/* reads a contentline in iCalendar format,
 * return a struct with its fields. contentline
 * must be '\0'-terminated and syntaxically correct,
 * no sanity check is performed ! (bad) */
struct icContline
ic_analyze_contentline(char *contentline)
{
  int offset = 0;
  int value_off = 0, name_off = 0;
  int column_cnt = 0, semicolumn_cnt = 0, eq_cnt = 0, comma_cnt = 0, dblquote_cnt = 0;
  char prev_char = '*'; // not '\\'
  int skip_char;
  struct icContline analysis;
  
  /* contentline   = name *(";" param ) ":" value CRLF */
  /* param         = param-name "=" param-value *("," param-value) */

  /* init struct members */
  memset(analysis.name,'\0',sizeof(analysis.name));
  memset(analysis.value,'\0',sizeof(analysis.value));
    
  while (contentline[offset] != '\0'){
    /* string & delimiters tester */
    if (contentline[offset] == '"' && prev_char != '\\')
      ++dblquote_cnt;
    else if (dblquote_cnt%2 == 0){ // we're not in a string and current char is not '"'
      /* delimiters from left to right,
       * determines if we have hit a delimiter, count it and skip it
       * (we do not want to copy it) */
      skip_char = false;
      switch (contentline[offset]){
      case ';': /* param delimiter */           ++semicolumn_cnt; skip_char=true; break;
      case ':': /* name and params delimiter */ ++column_cnt;     skip_char=true; break;
      case '=': /* param name delimiter */      ++eq_cnt; /* skip ? */            break;
      case ',': /* params value delimiter */    ++comma_cnt; /* skip ? */         break;
      }
    }
    prev_char = contentline[offset];
    /* copy correct parts in struct */
    if (skip_char == false){
      if(semicolumn_cnt == 0 && column_cnt == 0){
	/* still in name part, copy name */
	analysis.name[name_off++] = contentline[offset];
      }else if (semicolumn_cnt > 0 && column_cnt == 0){
	/* param part, undefined behaviour (not implemented) */
      }else if (semicolumn_cnt > 0 || column_cnt > 0){
	/* parameter is optional, so semicolumn_cnt may be == 0 */
	/* value part, copy value */
	analysis.value[value_off++] = contentline[offset];
      }
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
