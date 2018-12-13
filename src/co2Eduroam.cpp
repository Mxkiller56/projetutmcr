#include "esp_wpa2.h"
#include <WiFi.h>

bool _802_1x_eap_connect(char *ssid, char *identity, char *password){
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)identity, strlen(identity)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)identity, strlen(identity)); //provide username
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password)); //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config to default (fixed for 2018 and Arduino 1.8.5+)
  esp_wifi_sta_wpa2_ent_enable(&config); //set config to enable function (fixed for 2018 and Arduino 1.8.5+)
  WiFi.begin(ssid); //connect to Eduroam function
  unsigned long next_timeout = millis() + 10000L; // 10 seconds
  while (WiFi.status() != WL_CONNECTED && millis() < next_timeout) {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
  }
  if (WiFi.status() == WL_CONNECTED){
#ifdef DEBUG
    Serial.println("WiFi connected");
    Serial.println("IP address set: ");
    Serial.println(WiFi.localIP()); //print LAN IP
#endif
    return true;
  } else
    return false; // we had a problem connecting
}

