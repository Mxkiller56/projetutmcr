#ifndef ARDUINO_TESTING_H
#define ARDUINO_TESTING_H
#include <stdint.h>
#include <cstddef>
#include <time.h>

#define TRUE true
#define FALSE false

#define WIFI_STA 0
#define HTTP_CODE_OK 200

unsigned long millis(void);

class WiFi {
 public:
  ~WiFi(void);
  static bool disconnect(bool);
  static void mode(int);
  static bool begin(char *);
  static void setHostname(char *);
};

class Stream {
 public:
  virtual size_t available() = 0;
  size_t readBytes(uint8_t *buf, size_t toread); /** @param buf buffer to fill with data
						  @param toread size of data to get */
  int read(void);
};
/* note: Client is also a Stream */
class Client: public Stream {
 public:
  size_t available(void); // data size available
  virtual uint8_t connected() = 0;
};

class WiFiClient: public Client {
  uint8_t connected(void);
};

class HTTPClient {
 public:
  bool begin(char *url); // url to get
  int GET(void); // GET url, return code is HTTP
  int getSize(void); // document length (-1 if no length provided)
  WiFiClient *getStreamPtr(void);
  bool connected(void); // still connected to server ?
  void useHTTP10(bool); // use HTTP/1.0 instead of HTTP/1.1 (default)
};
#endif
