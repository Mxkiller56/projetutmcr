#include "Arduino_testing.h"
#include "util.h"

char FAKECONTENTFILE[] = "example.ics";
char *onlinecontent = NULL;

bool WiFi::disconnect(bool disco){/* french pun :)*/
  return true;
}
void WiFi::mode(int mode){}
bool WiFi::begin(char *ssid){// must be called first
  onlinecontent = _file2mem(FAKECONTENTFILE);
  return true;
}
void WiFi::setHostname(char *hostname){}

size_t WiFiClient::available(void){}
int WiFiClient::readBytes(uint8_t *buf, size_t toread){}

bool HTTPClient::begin(char *url){return true;}
int HTTPClient::GET(void){return HTTP_CODE_OK;}
int HTTPClient::getSize(void){}
WiFiClient *HTTPClient::getStreamPtr(void){}
bool connected(void){return true;}
