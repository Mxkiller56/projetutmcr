#include <Arduino.h>
#define detect 33 //pin du détecteur
#define LED 17
void setup() {
    pinMode(detect,INPUT); //le détecteur renvoie une valeur binaire 0 ou 1
    pinMode(LED, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    Serial.println(digitalRead(detect));
    digitalWrite(LED,digitalRead(detect)); //pour afficher si le détecteur voit quelque chose
    delay(50);
}
