#ifndef CALCONNECTOR_H
#define CALCONNECTOR_H
#ifndef TESTING
#include <HTTPClient.h>
#endif
#include "LedsMng.h"
#include "ICalendarParser.h"
#include <time.h>
#include <string.h>

namespace CalCo {
  /* filters */
  bool is_in_date_range (ICVevent*, time_t beg, time_t end);
  bool is_in_location (ICVevent*, char *location);
  bool over_date_range(ICVevent *event, time_t beg, time_t end);
  /** Get IC Vevents from an URL for a location between start and stop in time,
      and store them in the icvevs array
      For now URL scheme can only be http(s)
      @param icvevs array of vevents
      @param vevcount number of vevents in the icvevs array
      @param start events before this pit are ignored
      @param end events after this pit are ignored
      @param loc C string to be matched with the ICalendar LOCATION. Case-insensitive.
      @param url url of the icalendar file containing the vevents
      @return number of events collected or negative value if error */
  int events4loc_from_url (char *url, char *loc, time_t start, time_t stop, ICVevent *icvevs, int vevcount);

};


#endif
