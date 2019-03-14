#include "Arduino_testing.h"
#include "util.h"


char FAKECONTENTFILE[] = "example.ics";
char *onlinecontent = NULL;
char *contentptr = NULL;
bool _connected = true;
WiFiClient _wfclient;



/* functions with global visibility */
int esp_wifi_stop(void){
  return ESP_OK;
}
bool getLocalTime(struct tm * info, uint32_t ms){
#ifndef FOREVERTODAY
  const time_t time_now = time(NULL);
  *info = *(localtime(&time_now));
#else
  /* 20171208 */
  struct tm today = {};
  today.tm_sec = 23;
  today.tm_min = 10;
  today.tm_hour = 8;
  today.tm_mday = 8;
  today.tm_mon = 12 - 1;
  today.tm_year = 2017 - 1900;
  mktime(&today);
  *info = today;
#endif
  return true;
}
void configTime(long gmtOffset_sec, int daylightOffset_sec, const char* server1, const char* server2, const char* server3){
  // dummy for now
}
unsigned long millis (void){
  static clock_t reference = 0;
  if (reference == 0)
    reference = clock();
  return clock() - reference;
}
void delay(unsigned long ms){ // 10^-3
  // duration of the sleep in microseconds (10^-6)
  usleep(ms*1000);
}
void pinMode(int pinNumber, int mode){/* dummy */}
void digitalWrite(int pinNumber, int state){/* dummy */}
int digitalRead(int pinNumber){
  int a = 1; // might be modified in debugger at run-time
  return a; // always active (for testing purposes)
}
bool _802_1x_eap_connect(char *ssid, char *identity, char *password){
   /* in the magical world of networked software, wifi _always_ work
      ;) */ 
  WiFi::begin("SSID");
  return true;
}
void esp_deep_sleep(long long us_time){
  std::cout << "deepsleeping for " << us_time << " microseconds";
  exit(0);
}

/* class methods */
void fESP::restart(void){
  std::cout << "asked to reboot, exiting";
  exit(0);
}
Preferences::Preferences(){
  // only a small set of keys are defined
  kvs[0] = {"salle","Salle Télécom. 1"};
}
bool Preferences::begin(const char *name, bool readOnly){/* dummy for now */}  
size_t Preferences::putString(const char *key, const char *value){ /* dummy */}
size_t Preferences::getString(const char *key, char *value, size_t maxlen){
  int i,j;
  for (i=0; i<=sizeof(kvs)/sizeof(kvs[0])-1; i++){
    if (strcmp(kvs[i].key,key) == 0)
      for (j=0; kvs[i].val[j] != '\0' && j <= maxlen; j++)
	value[j] = kvs[i].val[j]; // copy until maxlen or end
  }
}

void WebServer2::begin(Preferences *preferences){}
void WebServer2::blocking_run(void){for(;;){/* it never returns */}}


bool WiFi::disconnect(bool disco){/* french pun :)*/
  return true;
}
void WiFi::mode(int mode){}
bool WiFi::begin(char *ssid){// must be called first
  onlinecontent = _file2mem(FAKECONTENTFILE);
  contentptr = onlinecontent;
  return true;
}
WiFi::~WiFi(void){free(onlinecontent);}
void WiFi::setHostname(char *hostname){}

size_t fSerial::available(void){return sizeof(char); /* seriously, idk, and who uses this ? */}
void fSerial::begin(int baudrate){/*dummy*/}
/* print generic functions */
size_t Print::printf(const char *format, ...){/*idk how to do*/}
size_t Print::print(const char msg[]){
  const char *beg = msg;
  while (*msg != '\0')
    std::cout << *(msg++);
  return msg-beg;
}
size_t Print::println(const char msg[]){
  size_t n = print(msg);
  std::cout << '\n';
  return ++n;
}
size_t Print::println(struct tm * timeinfo, const char * format){
  const char * f = format;
    if(!f){
        f = "%c";
    }
    char buf[64];
    size_t written = strftime(buf, 64, f, timeinfo);
    println(buf);
    return written;
}

/* LiquidCrystal_I2C */
LiquidCrystal_I2C::LiquidCrystal_I2C(int lcd_addr, int lcd_cols, int lcd_rows){}
void LiquidCrystal_I2C::begin(){}
void LiquidCrystal_I2C::clear(){}
void LiquidCrystal_I2C::home(){}
void LiquidCrystal_I2C::init(){}
void LiquidCrystal_I2C::backlight(){bl_on=true;}
void LiquidCrystal_I2C::noBacklight(){bl_on=false;}
bool LiquidCrystal_I2C::getBacklight(){return bl_on;}
void LiquidCrystal_I2C::setCursor(int x, int y){}
void LiquidCrystal_I2C::scrollDisplayLeft(){}
void LiquidCrystal_I2C::scrollDisplayRight(){}

size_t Client::available(void){
  return _connected;
}
int Stream::read(void){
  uint8_t data;
  int res;
  
  res = Stream::readBytes(&data, 1L);
  if (res < 0)
    return res;
  else
    return data;
}
size_t Stream::readBytes(uint8_t *buffer, size_t length){
  size_t count = 0;
    while(count < length && *contentptr != '\0') {
        *buffer++ = *contentptr++;
        count++;
    }
    if (count == 0){
      _connected = false;
    }
    return count;
}

void HTTPClient::useHTTP10(bool usehttp10 = true){} // does nothing
bool HTTPClient::begin(char *url){return true;}
int HTTPClient::GET(void){return HTTP_CODE_OK;}
int HTTPClient::getSize(void){}
WiFiClient *HTTPClient::getStreamPtr(void){
  return &_wfclient;
}
uint8_t WiFiClient::connected(void){
  return _connected;
}

/* global variables */
fSerial Serial;
fESP ESP;
