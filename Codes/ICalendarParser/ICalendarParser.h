#ifndef ICALENDARPARSER_H
#define ICALENDARPARSER_H

#define IC_LINELEN 256

class ICDate {
 public:
  ICDate();
  void set(int year=1970, int month=01, int day=01, int hours=0,
      int minutes=0, int seconds=0);
  void parse(char *datevalue);
 private:
  unsigned int year, month, day, hours, minutes, seconds;
};

class ICline {
 public:
  ICline();
  char *getName();
  char *getValue();
 private:
  char name[IC_LINELEN];
  char value[IC_LINELEN];
};

class ICObject {
 public:
  virtual ICDate getDtstart(void)=0;
  virtual ICDate getDtend(void)=0;
};

class ICVevent: public ICObject {
 public:
  virtual ICDate getDtstart(void);
  virtual ICDate getDtend(void);
 private:
  ICDate _dtstart;
  ICDate _dtend;
};

class ICalendarParser {
 public:
  ICalendarParser(); // constructor
  ICObject &getNext(void); // get_ic_events
  bool begin(char *icsbuf);
 private:
  char *readNextLine(); // _get_next_line
  ICline getNextLine(char *contentline); // _ic_analyze_contentline
};

#endif
