/* ICalendar parsing lib, Arduino-style (somewhat)
 * For more info on iCalendar, see RFC 5545
 * Doc here also: https://www.kanzaki.com/docs/ical/
 */

#ifndef ICALENDARPARSER_H
#define ICALENDARPARSER_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef TESTING
#include "Arduino_testing.h"
#else
#include "Arduino.h"
#include "Client.h"
#endif

// this breaks readNextLine(). Investigate.
// doesn't break if defined to 256
// strange break on 235
#define IC_NAMELEN (20)
#define IC_VALUELEN (150)
#define IC_LOCATIONLEN (54)
#define IC_SUMMARYLEN (150)
// not used
#define IC_PROPSLEN
#define IC_LEN_LOGICAL (IC_NAMELEN+IC_VALUELEN)

class ICDate {
 public:
  static time_t setFromICString(char *datevalue);
  static int getUtcYear(time_t);
  static int getUtcYearsSince1900(time_t);
  static int getUtcMonth(time_t);
  static int getUtcDay(time_t);
  static int getUtcHours(time_t);
  static int getUtcMinutes(time_t);
  static int getUtcSeconds(time_t);
};

class ICline {
 public:
  ICline();
  char *getName();
  char *getValue();
  bool nameIs(char *);
  bool valueIs(char *);
  void setName(char *name);
  void setValue(char *value);
  void setFromICString(char *icstr); // _ic_analyze_contentline
 private:
  char name[IC_NAMELEN];
  char value[IC_VALUELEN];
};

class ICObject {
  /* empty, just an ancestor */
};

class ICVevent: public ICObject {
 public:
  ICVevent();
  time_t getDtstart(void);
  time_t getDtend(void);
  void setDtstart(time_t);
  void setDtend(time_t);
  void setSummary(const char *summary);
  char *getSummary(void);
  void setLocation(const char *location);
  char *getLocation(void);
 private:
  time_t dtstart;
  time_t dtend;
  char location[IC_LOCATIONLEN];
  char summary[IC_SUMMARYLEN];
};

/* parsers */
class GenericICalParser {
 public:
  GenericICalParser(); // empty constructor
  ICVevent *getNext(); // common, uses readNextLine, curline and ICline object.setFromICString(curline) on stack. get_ic_events in proto.c
 protected:
  ICVevent curr_vevent;
  virtual char *readNextLine() = 0;
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
 * Note that Client must have already been initialized.
 * Incoming data is discarded when no longer needed in
 * order to free memory. */
class ICalClientParser: public GenericICalParser {
 public:
  ICalClientParser(); // empty constructor
  bool begin(Client *client);
 private:
  Client *client;
  char cache4[4] = {0};
  // specific ICalClientParser version
  virtual char *readNextLine(); // _get_next_line using Client stream
};


#endif
