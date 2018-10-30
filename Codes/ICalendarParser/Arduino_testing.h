#ifndef ARDUINO_TESTING_H
#define ARDUINO_TESTING_H
#include <stdint.h>
#include <cstddef>

#define TRUE true
#define FALSE false

#define WIFI_STA 0
#define HTTP_CODE_OK 200

class WiFi {
 public:
  static bool disconnect(bool);
  static void mode(int);
  static bool begin(char *);
  static void setHostname(char *);
};

class WiFiClient {
 public:
  size_t available(void); // data size available
  int readBytes(uint8_t *buf, size_t toread); /** @param buf buffer to fill with data
						  @param toread size of data to get */
};

class HTTPClient {
 public:
  bool begin(char *url); // url to get
  int GET(void); // GET url, return code is HTTP
  int getSize(void); // document length (-1 if no length provided)
  WiFiClient *getStreamPtr(void);
  bool connected(void); // still connected to server ?
};
#endif
