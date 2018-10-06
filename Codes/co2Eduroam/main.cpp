//Sketch edited by: martinius96 (Martin Chlebovec)
//Personal website: https://arduino.php5.sk
#include "esp_wpa2.h"
#include <WiFi.h>
#define EAP_IDENTITY "" //eduroam login --> identity@youruniversity.domain
#define EAP_PASSWORD "" //your Eduroam password
String line; //variable for response
const char* ssid = "eduroam"; // Eduroam SSID
const char* host = "arduino.php5.sk"; //external server domain for HTTP connection after authentification
void connexion(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config to default (fixed for 2018 and Arduino 1.8.5+)
  esp_wifi_sta_wpa2_ent_enable(&config); //set config to enable function (fixed for 2018 and Arduino 1.8.5+)
  WiFi.begin(ssid); //connect to Eduroam function
  WiFi.setHostname("ESP32Name"); //set Hostname for your device - not neccesary
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP
}

void setup() {
  Serial.begin(9600);
  connexion();
  delay(250);
}
void loop() {
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP
  delay(1000);
}
