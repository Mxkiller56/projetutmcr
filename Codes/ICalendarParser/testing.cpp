#include "ICalendarParser.h"
#include "util.h"
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
char ICS_FILE[] = "example.ics";

int main (void){
  /* test ICDate functionnality */
  // 1. Test icdate
  time_t testdate_time;
  time_t testdate_time2;
  struct tm *testdate_tm;
  char testdate[] = "20180927T120512Z";
  char testdate2short[] = "20180927T120512";
  testdate_time = ICDate::setFromICString(testdate);
  testdate_time2 = ICDate::setFromICString(testdate2short);
  testdate_tm = gmtime(&testdate_time);
  if(testdate_tm->tm_year == 2018 - 1900 &&
     testdate_tm->tm_mon  == 9 - 1 &&
     testdate_tm->tm_mday == 27   &&
     testdate_tm->tm_hour == 12   &&
     testdate_tm->tm_min  == 5    &&
     testdate_tm->tm_sec  == 12   &&
     testdate_time2 == 0)
    std::cout << "ICDate setFromICString test  passed\n";
  else
    std::cerr << "ICDate setFromICString test  FAILED\n";
  // 2. Test ICline
  ICline icline;
  char testdtstart[] = "DTSTART";
  icline.setName(testdtstart);
  icline.setValue(testdate);
  if (strcmp(icline.getName(),testdtstart) == 0 &&
      strcmp(icline.getValue(), testdate) == 0)
    std::cout << "ICline manual set test  passed\n";
  else
    std::cerr << "ICline manual set test  FAILED\n";
  char testicstr[] = "NAME;param1=42,param2=\"\\\"\":VALUE";
  icline.setFromICString(testicstr);
  if (strcmp(icline.getName(),"NAME") == 0 &&
      strcmp(icline.getValue(),"VALUE") == 0)
    std::cout << "ICline setFromICString test  passed\n";
  else
    std::cerr << "ICline setFromICString test  FAILED\n";
  // 3. test ICVevent
  ICVevent vevent = ICVevent();
  char testlocation[] = "TD9";
  char testsummary[] = "Maths-Physique-Electromag TD";
  struct tm testbeg = {0,0,16,26,10 - 1,2018 - 1900,0,0,0};
  struct tm testend = {0,0,18,26,10 - 1,2019 - 1900,0,0,0};
  time_t testbeg_t = timegm(&testbeg);
  time_t testend_t = timegm(&testend);
  vevent.setDtstart(testbeg_t);
  vevent.setDtend(testend_t);
  vevent.setLocation(testlocation);
  vevent.setSummary(testsummary);
  if (ICDate::getUtcYear(vevent.getDtstart())  == 2018 &&
      ICDate::getUtcMonth(vevent.getDtstart()) == 10 &&
      ICDate::getUtcDay(vevent.getDtstart())   == 26 &&
      ICDate::getUtcYear(vevent.getDtend())    == 2019 &&
      ICDate::getUtcMonth(vevent.getDtend())   == 10 &&
      ICDate::getUtcDay(vevent.getDtend())     == 26 &&
      strcmp(vevent.getLocation(),testlocation) == 0 &&
      strcmp(vevent.getSummary(),testsummary) == 0)
    std::cout << "ICVevent test  passed\n";
  else
    std::cout << "ICVevent test  FAILED\n";
  // 4. Test ICalendarParser
  ICalBufferParser icparser = ICalBufferParser();
  char *icsbuf = _file2mem(ICS_FILE);
  icparser.begin(icsbuf);
  ICVevent *icobj;
  time_t tmptime;
  std::cout << "Now parsing " << ICS_FILE << " ...\n";
  while((icobj = icparser.getNext())!=NULL){
    std::cout << "summary:" << icobj->getSummary() << "\n";
    std::cout << "location:" << icobj->getLocation() << "\n";
    tmptime = icobj->getDtstart();
    std::cout << "dtstart:" << asctime(localtime(&tmptime));
    tmptime = icobj->getDtend();
    std::cout << "dtend:" << asctime(localtime(&tmptime)) << "\n";
  }
}
