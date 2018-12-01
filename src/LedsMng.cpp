#include "LedsMng.h"
// MKTIME hack (TZ is not set on ESP32)
#define MKTIME mktime
#ifdef TESTING
#undef MKTIME
#define MKTIME timegm
#endif


/** init new object status */
CourseSlot::CourseSlot(int beg_h, int beg_m, int end_h, int end_m){
  assoc_vev = NULL; // no associated vevent
  local_tm_begin_hour = beg_h%23;
  local_tm_begin_min = beg_m%60;
  local_tm_end_hour = end_h%23;
  local_tm_end_min = end_m%60;
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

void CourseSlot::associateVevent(ICVevent *icvev){
  if (icvev != NULL)
    this->assoc_vev = icvev;
  else
    this->assoc_vev = NULL;
}


/** Associate LED slots to ICVevents.
    ICVevents have their begin and end expressed in UTC
    while LED slots have their begin and end expressed in localtime
    that's why you need to pass timezone offset
    @param cslots array of CourseSlots
    @param slotcount number of CourseSlots in cslots
    @param icvevs array of ICVevents
    @param vevcount size of ICVevents array
    @param tzoff_hours your timezone is plus or minus x hours compared to UTC
*/
void cslots_set (CourseSlot* cslots, int slotcount, ICVevent *icvevs, int vevcount, struct tm *UTCnow, int tzoff_hours){
  time_t beg = 0;
  time_t end = 0;
  int ivev, islot;

  for (ivev = 0; ivev <= vevcount-1; ivev++){
    for (islot = 0; islot <= slotcount-1; islot++){
      // 1. convert CourseSlot beg and end to UTC time
      beg = cslots[islot].beg2UTC(UTCnow, tzoff_hours);
      end = cslots[islot].end2UTC(UTCnow, tzoff_hours);
      // 2. link vevent if matching
      if (CalCo::over_date_range(&icvevs[ivev],beg,end))
	cslots[islot].associateVevent(&icvevs[ivev]);
    }
  }  
}

time_t CourseSlot::smth2UTC(int local_hour, int local_min, struct tm *UTCnow, int tzoff_hours){
  time_t rval = 0;
  struct tm smth_time = *UTCnow; // copy now, we don't wan't to modify UTCnow as it may be reused
  smth_time.tm_sec = 0; // granularity in CourseSlot is limited to minutes
  smth_time.tm_min = local_min; // seems crazy but wait
  smth_time.tm_hour = local_hour; // as an example, local_hour is +1H from UTC
  rval = MKTIME(&smth_time);
  // converting to UTC. local_hour was +1H from UTC, and our tzoff_hours is +1H
  // so rval was localtime (aka UTC+1H), then it's UTC+1H-1H = UTC
  rval -= SECS_PER_HOUR * tzoff_hours; // note that tzoff_hours can be negative.
  return rval;
}
/** return when is the end of this slot, in UTC time.
    @param UTCnow tm struct representing the actual time
    @param tzoff_hours your timezone offset in hours
*/
time_t CourseSlot::end2UTC(struct tm *UTCnow, int tzoff_hours){
  return smth2UTC(this->local_tm_end_hour,
		  this->local_tm_end_min,
		  UTCnow, tzoff_hours);
}
/** return when is the beginning of this slot, in UTC time.
    @param UTCnow tm struct representing the actual time
    @param tzoff_hours your timezone offset in hours
*/
time_t CourseSlot::beg2UTC(struct tm *UTCnow, int tzoff_hours){
  return smth2UTC(this->local_tm_begin_hour,
		  this->local_tm_begin_min,
		  UTCnow, tzoff_hours);
}
