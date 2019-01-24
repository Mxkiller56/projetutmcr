#ifndef ARDUINO_TESTING_H
#define ARDUINO_TESTING_H
#include <iostream>
#include <stdint.h>
#include <cstddef>
#include <time.h>

// fake values for INPUT OUTPUT
#define OUTPUT 0
#define INPUT 1
// same for HIGH LOW
#define HIGH 1
#define LOW 0

#define TRUE true
#define FALSE false

#define WIFI_STA 0
#define HTTP_CODE_OK 200

/* functions declared globally */
unsigned long millis(void);
void pinMode(int pinNumber, int mode);
void digitalWrite(int pinNumber, int state);
int digitalRead(int pinNumber);
// time things, took it from real Arduino.h
bool getLocalTime(struct tm * info, uint32_t ms = 5000);
void configTime(long gmtOffset_sec, int daylightOffset_sec, const char* server1, const char* server2 = nullptr, const char* server3 = nullptr);
bool _802_1x_eap_connect(char *ssid, char *identity, char *password); // declared here because there is no sense to include co2Eduroam.h while testing code

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

class Print {
 public:
  size_t printf(const char *format, ...);
  size_t print(const char[]);
  size_t println(struct tm * timeinfo, const char * format = NULL);
  size_t println(const char[]);
};

/* Serial is also a Stream */
class fSerial: public Stream, public Print {
 public:
  size_t available(void);
  void begin(int baudrate);
};

class LiquidCrystal_I2C: public Print {
 public:
  LiquidCrystal_I2C(int lcd_addr, int lcd_cols, int lcd_rows);
  void begin(); // must be called before everything else
  void clear(); // remove + home()
  void home(); // print/write op at first position on display
  void init(); // idk
  void backlight();
  void noBacklight();
  bool getBacklight();
 private:
  bool bl_on=true; // backlight is on by default
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

/* global variables declaration (in order to be used in other
   files) */
extern fSerial Serial;
#endif
