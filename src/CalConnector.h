#ifndef CALCONNECTOR_H
#define CALCONNECTOR_H
#include "ICalendarParser.h"
#include <time.h>

namespace CalCo {
  /* filters */
  bool matches_date_range (ICVevent*, time_t beg, time_t end);
  bool matches_location (ICVevent*, char *location);
  /** @param url c_str, where iCalendar file is stored (http(s) only urls)
      @param loc c_str, location events must match (not case-sensitive)
      @param icvevs array of vevents
      @param vevcount length of the vevents array
      @return negative if error, 0 if ok
      Get events from url and filter them according
      to their location and the day
  */
  int events4loc_from_url (char *url, char *loc, time_t start, time_t stop, ICVevent *icvevs, int vevcount);
};

#endif
