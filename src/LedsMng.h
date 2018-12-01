/* Provides some abstraction and utilities to manage the
   state of the LEDs
*/

#ifndef LEDSMNG_H
#define LEDSMNG_H
#include "ICalendarParser.h"
#include "CalConnector.h"
#include "time.h"
// took it from Time.h
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
  
/* possible states:
   - course planified, vevent is associated. room isn't free.
   - no course planified, activity detected. room may be free.
   - no course planified, no activity. room free.
*/
enum class CourseState {PLANNED, NOCOURSE_ACT, NOCOURSE_NOACT};
enum class LedColor {RED, ORANGE, GREEN, WHITE}; // here colors are matching states (e.g. PLANNED = RED)

class CourseSlot {
 public:
  CourseSlot(int beg_h=0, int beg_m=0, int end_h=0, int end_m=0, int tzoff_hours=0, int tzoff_min=0);
  LedColor whichColor(void);
  CourseState whichState(void);
  time_t end2UTC(struct tm *UTCnow);
  time_t beg2UTC(struct tm *UTCnow);
  bool associateVevent(ICVevent *, struct tm *UTCnow);
  bool isActiveNow(struct tm *UTCnow);
  int local_tm_begin_hour;
  int local_tm_begin_min;
  int local_tm_end_hour;
  int local_tm_end_min;
  bool activity_detected; // if there was activity between pit beg and pit end, set to true
 protected:
  int tzoff_hours;
  int tzoff_min;
  ICVevent *assoc_vev; // matching vevent. NULL if none.
  time_t smth2UTC(int local_hour, int local_min, struct tm *UTCnow);
};

#endif
