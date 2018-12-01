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
  CourseSlot(int beg_h=0, int beg_m=0, int end_h=0, int end_m=0);
  LedColor whichColor(void);
  CourseState whichState(void);
  time_t end2UTC(struct tm *UTCnow, int tzoff_hours);
  time_t beg2UTC(struct tm *UTCnow, int tzoff_hours);
  void associateVevent(ICVevent *);
  int local_tm_begin_hour;
  int local_tm_begin_min;
  int local_tm_end_hour;
  int local_tm_end_min;
  bool activity_detected; // if there was activity between pit beg and pit end, set to true
 protected:
  ICVevent *assoc_vev; // matching vevent. NULL if none.
  time_t smth2UTC(int local_hour, int local_min, struct tm *UTCnow, int tzoff_hours);
};

void cslots_set (CourseSlot* cslots, int slotcount, ICVevent *icvevs, int vevcount, struct tm *UTCnow, int tzoff_hours);

/* example: time slots used by MCR
CourseSlot mcr_slots[] = {
  CourseSlot(8,0,   10,0),
  CourseSlot(0,15,  12,15),
  CourseSlot(13,45, 15,45),
  CourseSlot(16,0,  18,0)
};
*/

#endif
