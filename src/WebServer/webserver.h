#ifndef WEBSERVER_H
#include <WiFi.h>
#include <Preferences.h>
#include <Arduino.h>
#define WEBSERVER_H


class WebServer2{
public:
	void begin(void);
	void blocking_run(void);
private:
  Preferences preferences;
	WiFiServer server;
	String header;
	String salle;
	String salle2;	
};
#endif
