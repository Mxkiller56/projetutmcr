#include "Arduino_testing.h"
#include "util.h"

char FAKECONTENTFILE[] = "example.ics";
char *onlinecontent = NULL;
char *contentptr = NULL;
bool _connected = true;
WiFiClient _wfclient;

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

bool HTTPClient::begin(char *url){return true;}
int HTTPClient::GET(void){return HTTP_CODE_OK;}
int HTTPClient::getSize(void){}
WiFiClient *HTTPClient::getStreamPtr(void){
  return &_wfclient;
}
uint8_t WiFiClient::connected(void){
  return _connected;
}

