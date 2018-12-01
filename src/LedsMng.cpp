#include "LedsMng.h"
// MKTIME hack (TZ is not set on ESP32)
#define MKTIME mktime
#ifdef TESTING
#undef MKTIME
#define MKTIME timegm
#endif


/** init new object status */
CourseSlot::CourseSlot(int beg_h, int beg_m, int end_h, int end_m, int tzoff_hours, int tzoff_min){
  assoc_vev = NULL; // no associated vevent
  local_tm_begin_hour = beg_h%23;
  local_tm_begin_min = beg_m%60;
  local_tm_end_hour = end_h%23;
  local_tm_end_min = end_m%60;
  // set internal timezone offset
  this->tzoff_hours = tzoff_hours%13;
  this->tzoff_min = tzoff_min%60;
  activity_detected = false;
}
/** @return the state of the slot */
CourseState CourseSlot::whichState(void){
  if (assoc_vev != NULL) // there is an associated vevent
    return CourseState::PLANNED;
  else if (activity_detected) // no assoc vevent and activity
    return CourseState::NOCOURSE_ACT;
  else // nothing is hapenning
    return CourseState::NOCOURSE_NOACT;
}

LedColor CourseSlot::whichColor(void){
  switch(this->whichState()){
  case CourseState::PLANNED:
    return LedColor::RED;
    break;
  case CourseState::NOCOURSE_ACT:
    return LedColor::ORANGE;
    break;
  case CourseState::NOCOURSE_NOACT:
    return LedColor::GREEN;
    break;
  }
  return LedColor::RED;
}

bool CourseSlot::associateVevent(ICVevent *icvev, struct tm *UTCnow){
  // 1. convert CourseSlot beg and end to UTC time
  time_t beg = this->beg2UTC(UTCnow);
  time_t end = this->end2UTC(UTCnow);
  // 2. link vevent if matching
  if (CalCo::over_date_range(icvev,beg,end)){
    this->assoc_vev = icvev;
    return true;
  }
  else{
    // don't set assoc_vev to NULL as we may overwrite
    // previous value. Just return false.
    return false;
  }
}

time_t CourseSlot::smth2UTC(int local_hour, int local_min, struct tm *UTCnow){
  time_t rval = 0;
  struct tm smth_time = *UTCnow; // copy now, we don't wan't to modify UTCnow as it may be reused
  smth_time.tm_sec = 0; // granularity in CourseSlot is limited to minutes
  smth_time.tm_min = local_min; // seems crazy but wait
  smth_time.tm_hour = local_hour; // as an example, local_hour is +1H from UTC
  rval = MKTIME(&smth_time);
  // converting to UTC. local_hour was +1H from UTC, and our tzoff_hours is +1H
  // so rval was localtime (aka UTC+1H), then it's UTC+1H-1H = UTC
  rval -= SECS_PER_HOUR * this->tzoff_hours; // note that tzoff_hours can be negative.
  rval -= SECS_PER_MIN * this->tzoff_min;
  return rval;
}
/** return when is the end of this slot, in UTC time.
    @param UTCnow tm struct representing the actual time
    @param tzoff_hours your timezone offset in hours
*/
time_t CourseSlot::end2UTC(struct tm *UTCnow){
  return smth2UTC(this->local_tm_end_hour,
		  this->local_tm_end_min,
		  UTCnow);
}
/** return when is the beginning of this slot, in UTC time.
    @param UTCnow tm struct representing the actual time
    @param tzoff_hours your timezone offset in hours
*/
time_t CourseSlot::beg2UTC(struct tm *UTCnow){
  return smth2UTC(this->local_tm_begin_hour,
		  this->local_tm_begin_min,
		  UTCnow);
}
