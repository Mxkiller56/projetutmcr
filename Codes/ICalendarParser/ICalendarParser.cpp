/* Trying to follow arduino API style reccomandations
   here */
#include "ICalendarParser.h"
#define TESTING
#ifdef TESTING
#include <stdlib.h>
#include <string.h>
#endif
#define c_array_len(array) sizeof(array)/sizeof(array[0])

// ICVevent
ICDate ICVevent::getDtstart(void) {
}
ICDate ICVevent::getDtend(void) {
}
// ICalendarParser
ICalendarParser::ICalendarParser(void){}
bool ICalendarParser::begin(char *icsbuf){}
char *ICalendarParser::readNextLine(void){}
ICline ICalendarParser::getNextLine(char *contentline){}
ICObject &ICalendarParser::getNext(void){}

/* ************** ICDate ******************** */
ICDate::ICDate(void){}
/** "manual" setter */
void ICDate::set(int year, int month, int day, int hours, int minutes, int seconds){
  this->year = year;
  this->month = month%12;
  this->day = day%31; // TODO: add more checks
  this->hours = hours%24;
  this->minutes = minutes%60;
  this->seconds = seconds%60;
}
/* getters */
unsigned int ICDate::getYear(void){return this->year;}
unsigned int ICDate::getMonth(void){return this->month;}
unsigned int ICDate::getDay(void){return this->day;}
unsigned int ICDate::getHours(void){return this->hours;}
unsigned int ICDate::getMinutes(void){return this->minutes;}
unsigned int ICDate::getSeconds(void){return this->seconds;}
/** setter from ICalendar-formatted date */
void ICDate::setFromIC(char datevalue[]){
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
}

/* ******************** ICline *************** */
ICline::ICline(void){}
char *ICline::getName(void){return this->name;}
char *ICline::getValue(void){return this->value;}
void ICline::setName(char *name){strncpy(this->name,name,sizeof(this->name)-1);}
void ICline::setValue(char *value){strncpy(this->value,value,sizeof(this->value)-1);}
