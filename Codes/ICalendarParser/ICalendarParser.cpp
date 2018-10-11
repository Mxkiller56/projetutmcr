#include "ICalendarParser.h"

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
// ICDate
ICDate::ICDate(void){}
void ICDate::set(int year, int month, int day, int hours, int minutes, int seconds){}
void ICDate::parse(char *datevalue){}
// ICline
ICline::ICline(void){}
char *ICline::getName(void){}
char *ICline::getValue(void){}
