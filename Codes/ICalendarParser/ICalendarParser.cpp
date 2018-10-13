/* Trying to follow arduino API style reccomandations
   here. I'm not very okay with references, so I may
   change functions to use refs later if needed */
#include "ICalendarParser.h"

#define c_array_len(array) sizeof(array)/sizeof(array[0])

/* **************** ICVevent ****************** */
ICVevent::ICVevent(void){}
ICDate ICVevent::getDtstart(void) {return this->dtstart;}
ICDate ICVevent::getDtend(void) {return this->dtend;}
void ICVevent::setDtstart(ICDate start){this->dtstart = start;}
void ICVevent::setDtend(ICDate end){this->dtend = end;}
void ICVevent::setSummary(const char *summary){strncpy(this->summary,summary,sizeof(this->summary)-1);}
void ICVevent::setLocation(const char *location){strncpy(this->location,location,sizeof(this->summary)-1);}
char *ICVevent::getSummary(void){return this->summary;}
char *ICVevent::getLocation(void){return this->location;}
/* **************** ICalendarParser *********** */
ICalendarParser::ICalendarParser(void){}
bool ICalendarParser::begin(const char *icsbuf){this->icsbuf = icsbuf;}
char *ICalendarParser::readNextLine(void){
  int linebuf_off;
  static int buf_off=0; // save buffer offset between each call

  /* read everything */
  linebuf_off=0; // init offset in curline
  for(/* buf_off hasn't to be reinitialized */; icsbuf[buf_off]!='\0'; buf_off++){
    if(icsbuf[buf_off]!='\r' /* CR */){
      /* we're in a line, so copy */
      this->curline[linebuf_off++]=this->icsbuf[buf_off];
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
      if (this->icsbuf[buf_off+2]==' '/* length of '\n' + 'SPC' */){
	// line continues
	buf_off+=2; // discard '\n' and ' ' (don't read), continue
		    // reading
      }else{ // wasn't a ' '
	// so we assume that both logical and physical line ended
	/* 1 finish up */
	this->curline[linebuf_off] = '\0'; // current is '\r'
	/* 2 reset and prepare for next line */
	linebuf_off = 0;
	/* we had '\r\n?' with '?' continuation or spc, we were at
	 * '\r', discard '\n' anyway */
	buf_off+=2; // 1 to discard '\n', another 1 because 3rd for
		    // loop statement won't be executed after re-entry
		    // in this function
	/* return line @ */
	return this->curline;
      }
    }
  }
  /* end of buffer reached, NULL return and reset for rereading */
  buf_off = 0;
  return NULL;
}
ICObject &ICalendarParser::getNext(void){}

/* **************** ICDate ******************** */
ICDate::ICDate(void){}
/** "manual" setter */
void ICDate::set(int year, int month, int day, int hours, int minutes, int seconds){
  // just copying 1st
  this->year = year; this->month = month; this->day = day; this->hours = hours; this->minutes = minutes; this->seconds = seconds;
  (this->check)(); // then check
}
/** checks and corrects state if needed */
void ICDate::check(void){
  // no need to check anything for year
  if (this->month > 12){this->month = this->month%12;}
  if (this->day > 31){this->day = this->day%31;} // basic
  if (this->hours > 24){this->hours = this->hours%24;}
  if (this->minutes > 60){this->minutes = this->minutes%60;}
  if (this->seconds > 60){this->seconds = this->seconds%60;}
}
/* getters */
unsigned int ICDate::getYear(void){return this->year;}
unsigned int ICDate::getMonth(void){return this->month;}
unsigned int ICDate::getDay(void){return this->day;}
unsigned int ICDate::getHours(void){return this->hours;}
unsigned int ICDate::getMinutes(void){return this->minutes;}
unsigned int ICDate::getSeconds(void){return this->seconds;}
/** setter from ICalendar-formatted date */
void ICDate::setFromICString(char *datevalue){
  char tmp[] = "\0\0\0\0";
  int i;
  struct _parser {
    char *start;
    int cpysize;
    unsigned int *storto;
  };
  struct _parser dv_parser [] = {
    {&datevalue[c_array_len("YYYYMMDDThhmm")-1],sizeof("ss")-1,  &this->seconds},
    {&datevalue[c_array_len("YYYYMMDDThh")-1],  sizeof("mm")-1,  &this->minutes},
    {&datevalue[c_array_len("YYYYMMDDT")-1],    sizeof("hh")-1,  &this->hours},
    {&datevalue[c_array_len("YYYYMM")-1],       sizeof("DD")-1,  &this->day},
    {&datevalue[c_array_len("YYYY")-1],         sizeof("MM")-1,  &this->month},
    {&datevalue[0],                           sizeof("YYYY")-1,&this->year}
  };
  /* do some format-validation. not checking for outbounds read, sorry */
  if (datevalue[c_array_len("YYYYMMDD")-1] == 'T'
      && datevalue[c_array_len("YYYYMMDDThhmmss")-1] == 'Z'){
    for (i=0; i<=c_array_len(dv_parser)-1; i++){
      memset(tmp,'\0',sizeof(tmp));
      memcpy(tmp, dv_parser[i].start, dv_parser[i].cpysize);
      *(dv_parser[i].storto) = atoi(tmp);
    }
  }
  (this->check)();
}

/* ******************** ICline *************** */
ICline::ICline(void){}
char *ICline::getName(void){return this->name;}
char *ICline::getValue(void){return this->value;}
void ICline::setName(char *name){strncpy(this->name,name,sizeof(this->name)-1);}
void ICline::setValue(char *value){strncpy(this->value,value,sizeof(this->value)-1);}
void ICline::setFromICString(char *icstr){
  int offset = 0;
  int value_off = 0, name_off = 0;
  int column_cnt = 0, semicolumn_cnt = 0, eq_cnt = 0, comma_cnt = 0, dblquote_cnt = 0;
  char prev_char = '*'; // not '\\'
  bool skip_char;
  
  /* icstr   = name *(";" param ) ":" value CRLF */
  /* param         = param-name "=" param-value *("," param-value) */

  /* init struct members */
  memset(this->name,'\0',sizeof(this->name));
  memset(this->value,'\0',sizeof(this->value));
    
  while (icstr[offset] != '\0'){
    /* string & delimiters tester */
    if (icstr[offset] == '"' && prev_char != '\\')
      ++dblquote_cnt;
    else if (dblquote_cnt%2 == 0){ // we're not in a string and current char is not '"'
      /* delimiters from left to right,
       * determines if we have hit a delimiter, count it and skip it
       * (we do not want to copy it) */
      skip_char = false;
      switch (icstr[offset]){
      case ';': /* param delimiter */           ++semicolumn_cnt; skip_char=true; break;
      case ':': /* name and params delimiter */ ++column_cnt;     skip_char=true; break;
      case '=': /* param name delimiter */      ++eq_cnt; /* skip ? */            break;
      case ',': /* params value delimiter */    ++comma_cnt; /* skip ? */         break;
      }
    }
    prev_char = icstr[offset];
    /* copy correct parts in struct */
    if (skip_char == false){
      if(semicolumn_cnt == 0 && column_cnt == 0){
	/* still in name part, copy name */
	this->name[name_off++] = icstr[offset];
      }else if (semicolumn_cnt > 0 && column_cnt == 0){
	/* param part, undefined behaviour (not implemented) */
      }else if (semicolumn_cnt > 0 || column_cnt > 0){
	/* parameter is optional, so semicolumn_cnt may be == 0 */
	/* value part, copy value */
	this->value[value_off++] = icstr[offset];
      }
    }
    ++offset;
  }
}
