/* ICalendar parsing lib, Arduino-style (somewhat)
 * For more info on iCalendar, see RFC 5545
 * Doc here also: https://www.kanzaki.com/docs/ical/
 */

#ifndef ICALENDARPARSER_H
#define ICALENDARPARSER_H

#include <stdlib.h>
#include <string.h>
#ifdef TESTING
#include "Arduino_testing.h"
#else
#include "Arduino.h"
#endif

#define IC_ELMTLEN 128
#define IC_LEN_LOGICAL 2*IC_ELMTLEN

class ICDate {
 public:
  ICDate();
  void set(int year=1970, int month=01, int day=01, int hours=0,
      int minutes=0, int seconds=0);
  void setFromICString(char *datevalue);
  unsigned int getYear(void);
  unsigned int getMonth(void);
  unsigned int getDay(void);
  unsigned int getHours(void);
  unsigned int getMinutes(void);
  unsigned int getSeconds(void);
 private:
  unsigned int year, month, day, hours, minutes, seconds;
  void check(void);
};

class ICline {
 public:
  ICline();
  char *getName();
  char *getValue();
  void setName(char *name);
  void setValue(char *value);
  void setFromICString(char *icstr); // _ic_analyze_contentline
 private:
  char name[IC_ELMTLEN];
  char value[IC_ELMTLEN];
};

class ICObject {
  /* empty, just an ancestor */
};

class ICVevent: public ICObject {
 public:
  ICVevent();
  ICDate getDtstart(void);
  ICDate getDtend(void);
  void setDtstart(ICDate);
  void setDtend(ICDate);
  void setSummary(const char *summary);
  char *getSummary(void);
  void setLocation(const char *location);
  char *getLocation(void);
 private:
  ICDate dtstart;
  ICDate dtend;
  char location[IC_ELMTLEN];
  char summary[IC_ELMTLEN];
};

class ICalendarParser {
 public:
  ICalendarParser(); // constructor
  ICObject &getNext(void); // get_ic_events
  bool begin(const char *icsbuf);
 private:
  char *readNextLine(); // _get_next_line
  const char *icsbuf;
  char curline[IC_LEN_LOGICAL];
};

#endif
