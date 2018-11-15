#include "CalConnector.h"

namespace CalCo {

/* if a vevent fits in a date range */
bool is_in_date_range (ICVevent *event, time_t beg, time_t end){
  if (beg <= event->getDtstart() &&
      event->getDtend() <= end)
    return true;
  else
    return false;
}
/** if a vevent is over a date range
    Examples returning true:
1  | vevent duration ... |
        | date range |
   *******************
2         | vevent duration ... |
     | date range |
   *******************
3  | vevent duration ...|
               | date range |
4  | date range .......   |
     | vevent duration |
*/	     
bool over_date_range(ICVevent *event, time_t beg, time_t end){
  // remark: case 4 is is_in_date_range
  if ((event->getDtstart() <= beg && event->getDtend() >= end) || // case 1
      (event->getDtstart() >= beg && event->getDtend() >= end) || // case 2
      (event->getDtstart() <= beg && event->getDtend() <= end) || // case 3
      (event->getDtstart() >= beg && event->getDtend() <= end))   // case 4
    return true;
  else
    return false;
}
/* if a vevent is in a location */
bool is_in_location (ICVevent *event, char *location){
  /* we found the searched location in the event */
  if (strcasestr(event->getLocation(), location) != NULL)
    return true;
  else
    return false;
}

/* returns the number of events collected or negative value if error */
  int events4loc_from_url (char *url, char *loc, time_t start, time_t stop, ICVevent *icvevs, int vevcount){
  ICVevent *curr_icvev;
  ICalClientParser icparser = ICalClientParser();
  HTTPClient http = HTTPClient();
  int httpCode = 0;
  int icvevs_offset = 0;
  
  http.begin(url);
  httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode = HTTP_CODE_OK) {
      icparser.begin(http.getStreamPtr());
      while ((curr_icvev = icparser.getNext()) != NULL &&
	     icvevs_offset <= vevcount-1){
	if (is_in_date_range(curr_icvev, start, stop) &&
	    is_in_location(curr_icvev, loc))
	  icvevs[icvevs_offset++] = *curr_icvev;
      }
      return icvevs_offset; // number of events read
    }
  } else {
    return -httpCode;
  }
}


} // namespace end
