#include "ICalendarParser.cpp"
#include <string>
#include <fstream>
#include <iostream>
#include <string.h>
#define ICS_FILE "example.ics"

int main (void){
  /* test ICDate functionnality */
  // 1. Test icdate
  char testdate[] = "20180927T120512Z";
  ICDate icdate = ICDate();
  icdate.setFromIC(testdate);
  if(icdate.getYear()    == 2018 &&
     icdate.getMonth()   == 9    &&
     icdate.getDay()     == 27   &&
     icdate.getHours()   == 12   &&
     icdate.getMinutes() == 5    &&
     icdate.getSeconds() == 12)
    std::cout << "ICDate setFromIC test passed\n";
  else
    std::cerr << "ICDate setFromIC test failed\n";
  // 2. Test ICline
  ICline icline;
  char dtstart[] = "DTSTART";
  icline.setName(dtstart);
  icline.setValue(testdate);
  if (strcmp(icline.getName(),dtstart) == 0 &&
      strcmp(icline.getValue(), testdate) == 0)
    std::cout << "ICLine test passed\n";
  else
    std::cerr << "ICLine test failed\n";
}
