#include <WiFi.h>
#include <Preferences.h>
#include <Arduino.h>
#include "webserver.h"

const char* ssid     = "Honor 6X";
const char* password = "titou2000";
WebServer2 serv;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  serv.begin();
}

void loop() {
  serv.blocking_run();
}
