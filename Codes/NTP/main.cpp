#include <Arduino.h>
#include <WiFi.h>
#include <NTPClientLib.h>

void connexion(); //déclaration de la méthode connexion()

void setup() {
  Serial.begin(9600); //définition du débit du port série
  Serial.println("Setup beginned");
  connexion();
  NTP.begin ("pool.ntp.org", 1, true, 0); // définition du serveur de temps utilisé, 1 correspond au décalage horaire par rapport au serveur, true pour gérer l'heure d'été et l'heure d'hiver, 0 pour le décalage en minute
  NTP.setInterval (3600);
}

void loop() {
  Serial.println (NTP.getTimeDateString ());
  delay(1000);
}
void connexion(){
  WiFi.begin("","");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    }
  Serial.println("Conneted !");
}
