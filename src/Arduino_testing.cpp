#include "Arduino_testing.h"
#include "util.h"


char FAKECONTENTFILE[] = "example.ics";
char *onlinecontent = NULL;
char *contentptr = NULL;
bool _connected = true;
WiFiClient _wfclient;



/* functions with global visibility */
bool getLocalTime(struct tm * info, uint32_t ms){
  const time_t time_now = time(NULL);
  *info = *(localtime(&time_now));
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

void pinMode(int pinNumber, int mode){/* dummy */}
void digitalWrite(int pinNumber, int state){/* dummy */}
int digitalRead(int pinNumber){/* dummy, might be extended ? */}
bool _802_1x_eap_connect(char *ssid, char *identity, char *password){return true; /* in the magical world of networked software, wifi _always_ work ;) */}


/* class methods */
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
