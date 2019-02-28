#include "LedsMng.h"
#include "ICalendarParser.h"
#include "CalConnector.h"
#include "Arduino_testing.h"
#include "TaskMgr.h"
#include "util.h"
#include <stdint.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
char ICS_FILE[] = "example.ics";

void emptyaction(void){}

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
  #ifdef WFCLIENTDMO
  // Client/WiFiClient demo run
  WiFi::begin("SSID");
  HTTPClient http;
  WiFiClient *stream = http.getStreamPtr();
  ICalClientParser icc_parser;
  ICVevent *icvev;
  time_t tmptime2;
  icc_parser.begin(stream);
  std::cout << "Now simulating network interaction ...\n";
  while((icvev = icc_parser.getNext())!=NULL){
    std::cout << "summary:" << icvev->getSummary() << "\n";
    std::cout << "location:" << icvev->getLocation() << "\n";
    tmptime2 = icvev->getDtstart();
    std::cout << "dtstart:" << asctime(localtime(&tmptime2));
    tmptime2 = icvev->getDtend();
    std::cout << "dtend:" << asctime(localtime(&tmptime2)) << "\n";
  }
  #endif
  #ifdef ICALBUFPARSDMO
  // ICalBufferParser demo run
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
  free(icsbuf);
  #endif
  #ifndef ICALBUFPARSDMO
  #ifndef WFCLIENTDMO
  WiFi::begin("SSID");
  time_t prev_midnight = ICDate::setFromICString("20180122T000000Z");
  time_t next_midnight = ICDate::setFromICString("20180123T000000Z");
  ICVevent icvevs[8];
  char onlineresource[] = "http://example.com/example.ics";
  char ourlocation[] = "Salle Télécom. 1";
  int eventsnum =
  CalCo::events4loc_from_url(onlineresource,ourlocation, prev_midnight, next_midnight, icvevs, c_array_len(icvevs));
  std::cout << "found " << eventsnum << " event(s) between " << ctime(&prev_midnight) <<  " and " << ctime(&next_midnight) << " in " << ourlocation << "\n";
  for (int i=0; i<=eventsnum-1; i++){
    std::cout << icvevs[i].getSummary() << " in " << icvevs[i].getLocation() << "\n";
  }
  #else
  #warning "events filter testing disabled because of demo(s) !"
  #endif
  #else
  #warning "events filter testing disabled because of demo(s) !"
  #endif
  // testing LedMng features
  /* ICVevents have their begin and end expressed in UTC
     while LED slots have their begin and end expressed in localtime 
     (you must set their TZ offset before) */
  CourseSlot ledmng_test_slots[] = {
    CourseSlot(8,0,    10,0,  1), // our "timezone" is UTC+1hour
    CourseSlot(10,15,  12,15, 1), 
    CourseSlot(13,45,  15,45, 1),
    CourseSlot(16,0,   18,0,  1)
  };
  ICVevent ledmng_vevents[4]; // no init by constructor (Arduino-style, why do I follow this ?)
  // sorry for this very long initialisation
  ledmng_vevents[0].setDtstart(ICDate::setFromICString("20181130T070000Z"));
  ledmng_vevents[0].setDtend(  ICDate::setFromICString("20181130T090000Z"));
  ledmng_vevents[0].setLocation("TD0");
  ledmng_vevents[0].setSummary("Summary1");
  ledmng_vevents[1].setDtstart(ICDate::setFromICString("20181130T091500Z"));
  ledmng_vevents[1].setDtend(  ICDate::setFromICString("20181130T111500Z"));
  ledmng_vevents[1].setLocation("TD0");
  ledmng_vevents[1].setSummary("Summary2");
  ledmng_vevents[2].setDtstart(ICDate::setFromICString("20181130T124500Z"));
  ledmng_vevents[2].setDtend(  ICDate::setFromICString("20181130T144500Z"));
  ledmng_vevents[2].setLocation("TD9"); // note that association is made
  // regardless of the location (should be filtered first with CalConnector)
  ledmng_vevents[2].setSummary("Summary3");
  // shouldn't fit, out of range
  ledmng_vevents[3].setDtstart(ICDate::setFromICString("20181130T190000Z"));
  ledmng_vevents[3].setDtend(  ICDate::setFromICString("20181130T200000Z"));
  ledmng_vevents[3].setLocation("TD0");
  ledmng_vevents[3].setSummary("Summary4");
  time_t ledmng_now = ICDate::setFromICString("20181130T093253Z");
  struct tm *ledmng_utcnow = gmtime(&ledmng_now);
  // standard way of associating. Should be used like this
  // in final program also. ivev limit is dynamic (depends of the
  // number of matching vevents)
  for (int ivev = 0; ivev <= c_array_len(ledmng_vevents)-1; ivev++){
    for (int islot = 0; islot <= c_array_len(ledmng_test_slots)-1; islot++){
      ledmng_test_slots[islot].associateVevent(&ledmng_vevents[ivev],ledmng_utcnow);
    }
  }
  if (ledmng_test_slots[0].whichState() == CourseState::PLANNED &&
      ledmng_test_slots[1].whichState() == CourseState::PLANNED &&
      ledmng_test_slots[2].whichState() == CourseState::PLANNED &&
      ledmng_test_slots[3].whichState() == CourseState::NOCOURSE_NOACT &&
      ledmng_test_slots[0].isActiveNow(ledmng_utcnow) == false  &&
      ledmng_test_slots[1].isActiveNow(ledmng_utcnow) == true   &&
      ledmng_test_slots[2].isActiveNow(ledmng_utcnow) == false  &&
      ledmng_test_slots[3].isActiveNow(ledmng_utcnow) == false)
    std::cout << "LedMng test passed\n";
  else
    std::cout << "LedMng test FAILED\n";

  // testing schedEvt
  bool sched_evt_ok = true;
  // chain feature
  schedEvt ev1, ev2, ev3;
  int a = 5; // for non-linearity
  schedEvt ev4;
  ev1.setNext(&ev2);
  ev2.setNext(&ev3);
  // before ev1 ev2 ev3 (remove MIDDLE)
  ev2.unlink_myself(&ev1); // after ev1 ev3
  if (ev1.getNext() != &ev3)
    sched_evt_ok = false;
  // before ev1 ev3 (remove LAST)
  ev3.unlink_myself(&ev1); // after ev1
  if (ev1.getNext() != NULL)
    sched_evt_ok = false;
  // before ev1
  ev1.unlink_myself(&ev1); // after ev1 (testing corner-cases...)
  if (ev1.getNext() != NULL) 
    sched_evt_ok = false;
  ev1.setNext(&ev2); // before ev1 ev2
  ev1.unlink_myself(&ev1); // remove FIRST, after ev1
  if (ev1.getNext() != NULL)
    sched_evt_ok = false;
  // action exec feature
  time_t tmgr_time_now = time(NULL);
  ev1.setWhen(tmgr_time_now);
  ev1.setAction(&emptyaction);
  if (ev1.execAction(tmgr_time_now - 10) == !false || /* trying to exec *before* */
      ev1.execAction(tmgr_time_now) == !true || /* trying to exec now, should be OK */
      ev1.execAction(tmgr_time_now) == !false) /* trying to exec more than once */
    sched_evt_ok = false;
  ev1.setWhen(tmgr_time_now-5L*60L); // 5 minutes before
  if (ev1.execAction(tmgr_time_now) != true) // should exec *again* because we changed time
    sched_evt_ok = false;
  // now testing generic algorithm
  ev1.setNext(&ev2); // ev1 is root
  ev2.setNext(&ev3);
  schedEvt *evptr = &ev1;
  //  ev1.begin(&emptyaction,tmgr_time_now);
  //  while ((evptr = evptr->getNext()) != NULL)
  
  if (sched_evt_ok)
    std::cout << "TaskMgr test passed\n";
  else
    std::cout << "TaskMgr test FAILED\n";
}
