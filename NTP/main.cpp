#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);
void setup() {
  Serial.begin(9600);
  Serial.println("Setup beginned");
  WiFi.begin("","");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    }
  Serial.println("Conneted !");
  timeClient.begin();
  Serial.println(timeClient.getFormattedTime()); //affichage de la date avant la mise a jour de l'heure
}

void loop() {
  timeClient.update(); //mise à jour de l'heure
  Serial.println(timeClient.getFormattedTime());
  delay(1000);
}
