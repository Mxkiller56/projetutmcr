#include "ICalendarParser.cpp"
#include <string>
#include <fstream>
#include <iostream>
#include <string.h>
#include <unistd.h>
#define ICS_FILE "example.ics"

int main (void){
  /* test ICDate functionnality */
  // 1. Test icdate
  char testdate[] = "20180927T120512Z";
  ICDate icdate = ICDate();
  icdate.setFromICString(testdate);
  if(icdate.getYear()    == 2018 &&
     icdate.getMonth()   == 9    &&
     icdate.getDay()     == 27   &&
     icdate.getHours()   == 12   &&
     icdate.getMinutes() == 5    &&
     icdate.getSeconds() == 12)
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
  icdate.set(2018,10,13);
  vevent.setDtstart(icdate);
  icdate.set(icdate.getYear()+1,10,13);
  vevent.setDtend(icdate);
  vevent.setLocation(testlocation);
  vevent.setSummary(testsummary);
  if (vevent.getDtstart().getYear()  == 2018 &&
      vevent.getDtstart().getMonth() == 10 &&
      vevent.getDtstart().getDay()   == 13 &&
      vevent.getDtend().getYear()    == 2019 &&
      vevent.getDtend().getMonth()   == 10 &&
      vevent.getDtend().getDay()     == 13 &&
      strcmp(vevent.getLocation(),testlocation) == 0 &&
      strcmp(vevent.getSummary(),testsummary) == 0)
    std::cout << "ICVevent test  passed\n";
  else
    std::cout << "ICVevent test  FAILED\n";
  // 4. Test ICalendarParser
  ICalendarParser icparser = ICalendarParser();
  // c++ trickery found on SO. dev quick & dirty
  std::ifstream in(ICS_FILE);
  std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  const char *icsbuf = contents.c_str();
}
