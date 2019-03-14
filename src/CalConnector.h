#ifndef CALCONNECTOR_H
#define CALCONNECTOR_H
#ifndef TESTING
#include <HTTPClient.h>
#endif
#include "LedsMng.h"
#include "ICalendarParser.h"
#include <time.h>
#include <string.h>

/**
 * Connector between parser and main program.
 * Sort of intermediate layer, groups
 * mostly utilitary functions.
*/
namespace CalCo {
  /**
   * Checks if a Vevent is in a date range.
   * @param beg UNIX UTC time range start
   * @param end UNIX UTC time range stop
   * @return true if the Vevent is in the date range, false else.
   */
  bool is_in_date_range (ICVevent*, time_t beg, time_t end);
  /** Checks if a Vevent matches a LOCATION property. */
  bool is_in_location (ICVevent*, char *location);
  /** Checks if a Vevent overlaps a date range. */
  bool over_date_range(ICVevent *event, time_t beg, time_t end);
  /**
   * Retrieve Vevents.
   * For now URL scheme can only be http(s)
   * @param icvevs array where retrieved vevents will be written
   * @param vevcount number of vevents in the icvevs array (size)
   * @param start events before this point in time are ignored
   * @param stop events after this point in time are ignored
   * @param loc C string to be matched with the ICalendar LOCATION. Case-insensitive.
   * @param url url of the icalendar file containing the vevents
   * @return number of events collected or negative value if error */
  int events4loc_from_url (char *url, char *loc, time_t start, time_t stop, ICVevent *icvevs, int vevcount);

};


#endif
