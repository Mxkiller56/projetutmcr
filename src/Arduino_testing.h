#ifndef ARDUINO_TESTING_H
#define ARDUINO_TESTING_H
#include <iostream>
#include <stdint.h>
#include <cstddef>
#include <time.h>

/** Mocks most of the Arduino built-in functions and
 *  libraries used in this project.
 */

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

#define ESP_OK 1

/* functions declared globally */
int esp_wifi_stop(void);
unsigned long millis(void);
void delay(unsigned long ms);
void pinMode(int pinNumber, int mode);
void digitalWrite(int pinNumber, int state);
int digitalRead(int pinNumber);
// time things, took it from real Arduino.h
bool getLocalTime(struct tm * info, uint32_t ms = 5000);
void configTime(long gmtOffset_sec, int daylightOffset_sec, const char* server1, const char* server2 = nullptr, const char* server3 = nullptr);
bool _802_1x_eap_connect(char *ssid, char *identity, char *password); // declared here because there is no sense to include co2Eduroam.h while testing code
void esp_deep_sleep(long long us_time);

// just used for ESP.restart
class fESP {
 public:
  void restart(void);
};
/* mocking preferences API from Espressif */
class Preferences {
 public:
  Preferences();
  bool begin(const char *name, bool readOnly=false);
  size_t putString(const char *key, const char *value);
  /* value is a pointer to the output value. As said in "documentation":
 * All functions expect out_value to be a pointer to an already
 * allocated variable of the given type.*/
  size_t getString(const char *key, char *value, size_t maxlen);
  struct kv {
    char *key;
    char *val;
  };
  struct kv kvs [1];
};

// not integrated into simulation code
// uses Strings, etc, and tons of other APIs
class WebServer2{
public:
	void begin(Preferences *preferences);
	void blocking_run(void);
};

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
  size_t readBytes(uint8_t *buffer, size_t length);
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
  void setCursor(int,int);
  void scrollDisplayLeft();
  void scrollDisplayRight();
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
extern fESP ESP;
#endif
