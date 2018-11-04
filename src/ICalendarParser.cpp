/* Trying to follow arduino API style recommandations
   here. I'm not very okay with references, so I may
   change functions to use refs later if needed */
#include "ICalendarParser.h"
#define MKTIME mktime
#ifdef TESTING
#include <iostream>
#undef MKTIME
#define MKTIME timegm
#endif

#define c_array_len(array) sizeof(array)/sizeof(array[0])

/* **************** ICVevent ****************** */
ICVevent::ICVevent(void){}
time_t ICVevent::getDtstart(void) {return this->dtstart;}
time_t ICVevent::getDtend(void) {return this->dtend;}
void ICVevent::setDtstart(time_t start){this->dtstart = start;}
void ICVevent::setDtend(time_t end){this->dtend = end;}
void ICVevent::setSummary(const char *summary){
  strncpy(this->summary,summary,sizeof(this->summary));
  this->location[c_array_len(this->location)-1] = '\0';
}
void ICVevent::setLocation(const char *location){
  strncpy(this->location,location,sizeof(this->location)); //copy sizeof bytes
  this->location[c_array_len(this->location)-1] = '\0'; //terminate
}
char *ICVevent::getSummary(void){return this->summary;}
char *ICVevent::getLocation(void){return this->location;}

/* **************** GenericICalParser ************ */
GenericICalParser::GenericICalParser(void){}
/* can only return vevents for now */
ICVevent *GenericICalParser::getNext(void){
  char ic_begin[] = "BEGIN";
  char ic_end[] = "END";
  char ic_vevent[] = "VEVENT";
  char ic_dtstart[] = "DTSTART";
  char ic_dtend[] = "DTEND";
  char ic_summary[] = "SUMMARY";
  char ic_location[] = "LOCATION";
  ICline ic_curline = ICline(); // structured representation of this->curline

  while (this->readNextLine() != NULL){
    ic_curline.setFromICString(this->curline); // convert into structured
    if (ic_curline.nameIs(ic_begin) && ic_curline.valueIs(ic_vevent)){
      while(this->readNextLine() != NULL){
	ic_curline.setFromICString(this->curline);
	if(ic_curline.nameIs(ic_end) && ic_curline.valueIs(ic_vevent))
	  return &this->curr_vevent;
	else if (ic_curline.nameIs(ic_dtstart))
	  this->curr_vevent.setDtstart(ICDate::setFromICString(ic_curline.getValue()));
	else if (ic_curline.nameIs(ic_dtend))
	  this->curr_vevent.setDtend(ICDate::setFromICString(ic_curline.getValue()));
	else if (ic_curline.nameIs(ic_summary))
	  this->curr_vevent.setSummary(ic_curline.getValue());
	else if (ic_curline.nameIs(ic_location))
	  this->curr_vevent.setLocation(ic_curline.getValue());
      }
    } // end vevent while
  } // end buf while
  return NULL;
}

/* **************** ICalBufferParser *********** */
ICalBufferParser::ICalBufferParser(void){}
bool ICalBufferParser::begin(const char *icsbuf){this->icsbuf = icsbuf; return true;}
char *ICalBufferParser::readNextLine(void){
  static int icsbuf_off=0;
  static int skip=0;
  int linebuf_off=0;

  while(this->icsbuf[icsbuf_off] != '\0'){
    if(skip >= 0) // skipping
      skip--;
    else{ // not skipping
      if(icsbuf_off >= 3){ // negative outbound read
	this->curline[linebuf_off] = this->icsbuf[icsbuf_off-3]; // trail
	if (linebuf_off < c_array_len(this->curline)-1) // continue filling if ok.
	  linebuf_off++;
	if(this->icsbuf[icsbuf_off-2]=='\r' && this->icsbuf[icsbuf_off-1]=='\n'){// CRLF
	  if(this->icsbuf[icsbuf_off]==' ')// CRLFSPC -> folded line. skip & continue.
	    // trail is at xCRLFSPCa
	    skip = 2;
	  else { //CRLFa a is any char.
	    skip = 2;
	    this->curline[linebuf_off]='\0';
	    return this->curline;
	  }
	}
      } // negative outbound read end
    } // skip end
    icsbuf_off++;
  } // while end. Close last line and return.
  if (this->icsbuf[icsbuf_off-2]!='\r' && this->icsbuf[icsbuf_off-1]!='\r'
      && linebuf_off < c_array_len(this->curline)-4){ // not a CRLFEOF file end & enough room to finish copy
    this->curline[linebuf_off++] = this->icsbuf[icsbuf_off-3]; // axxEOF
    this->curline[linebuf_off++] = this->icsbuf[icsbuf_off-2]; // xaxEOF
    this->curline[linebuf_off++] = this->icsbuf[icsbuf_off-1]; // xxaEOF
  }
  else if (linebuf_off < c_array_len(this->curline)-2) // it's a CRLFEOF file end
    this->curline[linebuf_off++] = this->icsbuf[icsbuf_off-3]; // aCRLFEOF
  // in any case, close line correctly !
  this->curline[linebuf_off] = '\0';
  icsbuf_off = 0; // "rewind" in buffer to reparse after if needed
  return NULL;
}

/* **************** ICalClientParser ********** */
ICalClientParser::ICalClientParser(void){}
bool ICalClientParser::begin(Client *client){
  this->client = client;
  return true;
}
char *ICalClientParser::readNextLine(void){
  static int skip=0, data_off = 0;
  int linebuf_off = 0;
  int i;
  
  while(this->client->available()){ // there is still data to read
    /* cache4 moving */
    for (i=c_array_len(this->cache4)-1; i > 0; i--)
      this->cache4[i] = this->cache4[i-1];
    this->cache4[0] = this->client->read(); // 3 is trail 0 is head
    data_off++;

    if(skip > 0) // skipping. Oops, border effect
      skip--;
    else{ // not skipping
      if(data_off > 3){
	this->curline[linebuf_off] = this->cache4[3]; // copy trail
	if (linebuf_off < c_array_len(this->curline)-1) // continue filling if ok
	  linebuf_off++;
	if(this->cache4[2]=='\r' && this->cache4[1]=='\n'){// CRLF
	  if(this->cache4[0]==' ')// CRLFSPC -> folded line. skip & continue.
	    // trail is at xCRLFSPCa
	    skip = 3;
	  else { //CRLFa a is any char.
	    skip = 2;
	    this->curline[linebuf_off]='\0';
	    return this->curline;
	  }
	}
      } // negative outbound read end
    } // skip end
  } // while end (no more data available)
  return NULL;
}

/* **************** ICDate ******************** */
/* getters. returns are in Universal Coordinated Time */
int ICDate::getUtcYear(time_t t){return 1900+gmtime(&t)->tm_year;} // this one is a bit special, since tm_year is years since 1900
int ICDate::getUtcYearsSince1900(time_t t){return gmtime(&t)->tm_year;}
int ICDate::getUtcMonth(time_t t){return gmtime(&t)->tm_mon+1;} // months are numbered starting from 0
int ICDate::getUtcDay(time_t t){return gmtime(&t)->tm_mday;}
int ICDate::getUtcHours(time_t t){return gmtime(&t)->tm_hour;}
int ICDate::getUtcMinutes(time_t t){return gmtime(&t)->tm_min;}
int ICDate::getUtcSeconds(time_t t){return gmtime(&t)->tm_sec;}
/** setter from ICalendar-formatted date-time
    https://icalendar.org/iCalendar-RFC-5545/3-3-5-date-time.html
    this version only implements UTC time
 */
time_t ICDate::setFromICString(char *datevalue){
  struct tm convtm = {};
  time_t convtime;
  char tmp[] = "\0\0\0\0";
  int i=0;
  struct _parser {
    char *start;
    int cpysize;
    int *storto;
  };
  // cpysize is -1 because sizeof counts '\0' because it's a "string"
  struct _parser dv_parser [] = {
    /* start                                    cpysize            storto        */
    {&datevalue[c_array_len("YYYYMMDDThhmm")-1],sizeof("ss")-1,    &convtm.tm_sec}, // from 0 to 59 (60 for leap second)
    {&datevalue[c_array_len("YYYYMMDDThh")-1],  sizeof("mm")-1,    &convtm.tm_min}, // from 0 to 59
    {&datevalue[c_array_len("YYYYMMDDT")-1],    sizeof("hh")-1,    &convtm.tm_hour},// from 0 to 23
    {&datevalue[c_array_len("YYYYMM")-1],       sizeof("DD")-1,    &convtm.tm_mday},// from 1 to 31
    {&datevalue[c_array_len("YYYY")-1],         sizeof("MM")-1,    &convtm.tm_mon}, // from 0 to 11
    {&datevalue[0],                             sizeof("YYYY")-1,  &convtm.tm_year} // years elapsed since 1900
  };
  /* do some format-validation. */
  while (datevalue[i]!='\0')
    i++;
  if (i < c_array_len("YYYYMMDDTHHMMSSZ")-1)
    return 0;  // quit now. too small to be parsed.
  if (datevalue[c_array_len("YYYYMMDD")-1] == 'T'
      && datevalue[c_array_len("YYYYMMDDThhmmss")-1] == 'Z'){
    for (i=0; i<=c_array_len(dv_parser)-1; i++){
      memset(tmp,'\0',sizeof(tmp));
      memcpy(tmp, dv_parser[i].start, dv_parser[i].cpysize);
      *(dv_parser[i].storto) = atoi(tmp);
    }
  }
  /* now some adjustments */
  convtm.tm_mon -= 1; // correct month numbering
  convtm.tm_year -= 1900; // year is elapsed years since 1900
  convtime = MKTIME(&convtm); // convert it in time_t format
  if (convtime != -1) // input was correct
    return convtime;
  else // return default value (epoch start)
    return 0;
}

/* ******************** ICline *************** */
ICline::ICline(void){ // in case nothing is set
    memset(&(this->name),'\0',sizeof(this->name));
    memset(&(this->value),'\0',sizeof(this->value));
}
char *ICline::getName(void){return this->name;}
char *ICline::getValue(void){return this->value;}
bool ICline::nameIs(char *comp){
  if(strncmp(this->name,comp,sizeof(this->name)) == 0)
    return true;
  else
    return false;
}
bool ICline::valueIs(char *comp){
  if(strncmp(this->value,comp,sizeof(this->value)) == 0)
    return true;
  else
    return false;
}
void ICline::setName(char *name){
  strncpy(this->name,name,sizeof(this->name)); // copy sizeof bytes
  this->name[c_array_len(this->name)-1] = '\0'; // terminate anyway
}
void ICline::setValue(char *value){
  strncpy(this->value,value,sizeof(this->value)); // copy sizeof bytes
  this->value[c_array_len(this->value)-1] = '\0'; // terminate anyway
}
/* icstr must be null-terminated */
void ICline::setFromICString(char *icstr){
  int offset = 0;
  int value_off = 0, name_off = 0;
  int column_cnt = 0, semicolumn_cnt = 0, eq_cnt = 0, comma_cnt = 0, dblquote_cnt = 0;
  char prev_char = '*'; // not '\\'
  bool skip_char;
  
  /* icstr   = name *(";" param ) ":" value CRLF */
  /* param         = param-name "=" param-value *("," param-value) */

  /* init struct members */
  memset(&(this->name),'\0',sizeof(this->name));
  memset(&(this->value),'\0',sizeof(this->value));
    
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
	/* still in name part, copy name and check if not 
	   writing out-of-bounds */
	this->name[name_off] = icstr[offset];
	if(name_off < c_array_len(this->name)-1)
	  name_off++;
      }else if (semicolumn_cnt > 0 && column_cnt == 0){
	/* param part, undefined behaviour (not implemented) */
      }else if (semicolumn_cnt > 0 || column_cnt > 0){
	/* parameter is optional, so semicolumn_cnt may be == 0
	 * value part, copy value and check if not writing out
	 * of bounds */
	this->value[value_off] = icstr[offset];
	if(value_off < c_array_len(this->value)-1)
	  value_off++;
      }
    }
    ++offset;
  }
}
