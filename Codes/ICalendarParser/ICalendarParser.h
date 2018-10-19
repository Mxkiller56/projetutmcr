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

// this breaks readNextLine(). Investigate.
// doesn't break if defined to 256
// strange break on 235
#define IC_ELMTLEN (20)
#define IC_LEN_LOGICAL (2*IC_ELMTLEN)

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

/* parsers */
class GenericICalParser {
 public:
  GenericICalParser(); // empty constructor
  ICObject &getNext(); // common, uses readNextLine, curline and ICline object.setFromICString(curline) on stack. get_ic_events in proto.c
 protected:
  virtual char *readNextLine();
  ICObject curr_ic_object;
  char curline [IC_LEN_LOGICAL]; // common, used to store current "logical" line in both readNextLine methods
};

/* 1st case: large on memory, you have a big buffer
 * with ALL ICal data in and want to parse it */
class ICalBufferParser: public GenericICalParser {
 public:
  ICalBufferParser(); // empty constructor
  bool begin(const char *icsbuf);
 private:
  const char *icsbuf;
  /* specific ICalBufferParser version */
  virtual char *readNextLine(); // _get_next_line using icsbuf
};

/* 2nd case: less large on memory, you have a connection
 * to an ICal resource and you pass it to the parser,
 * which uses it to read data from chunk to chunk.
 * Incoming data is discarded when no longer needed in
 * order to free memory */
class ICalStreamParser: public GenericICalParser {
 public:
  ICalStreamParser(); // empty constructor
  bool begin(WifiClient *client);
 private:
  WifiClient *client;
  // specific ICalStreamParser version
  virtual char *readNextLine(); // _get_next_line using WifiClient stream
};


#endif
