/* For more info on iCalendar, see RFC 5545
 * iCalendar Stream
 * iCalendar objects
 * Section 3.1 content line format
 * Section 3.1 property paramater format
 * Section 3.3 data types for property values
 */

/* content lines should not be larger than 75 octets, excluding line break */
#define _IC_CLIN_SIZ 75
/* newlines are carriage return + line feed 
 * may not use ?
 */
#define _IC_NEWLIN '\r\n'
/* line continues when you have
 * CRLF+\t or CRLF+' '. Those chars are not taken in account. So you
 * need first to have an unfold function() */
/* les valeurs sont séparées par un ',' */
#define _IC_SIMPLVALSEP ','
#define _IC_MULTIVALSEP ';'

struct _contentline {
  /* content = name *(";" param ) ":" value CRLF */
  char *name;
  char **params; /* may have more than 1 parameter */
  char *value;
};

