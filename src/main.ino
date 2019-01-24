/* -*- mode: c++; -*- */
#ifndef TESTING
#include "co2Eduroam.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#else
#include "Arduino_testing.h"
#endif
#include "secrets.h"
#include "CalConnector.h"
#include "ICalendarParser.h"
#include "LedsMng.h"

/* functions */
void wifico(void);
void e_wifico(void);
void get_time(void);
void e_get_time(void);


/* global variables or consts */
char onlineresource[] = "https://planning.univ-rennes1.fr/jsp/custom/modules/plannings/cal.jsp?data=8241fc3873200214080677ae6bf34798e0fa50826f0818af576889429f500664906f45af276f59ae8fac93f781e861526c6c7672adf13bbdf6273afaeb8260a8c2973627c2eb073b16351c4cc23ce65f8d3f4109b6629391";
// TODO: use Preferences API for location
char ourlocation[] = "TD3";
ICVevent mcr_icvevs[8];
int n_events_today=0;
CourseSlot mcr_slots[] = {
  CourseSlot(8,0,    10,0,  1), // our "timezone" is UTC+1hour
  CourseSlot(10,15,  12,15, 1), 
  CourseSlot(13,45,  15,45, 1),
  CourseSlot(16,0,   18,0,  1)
};
// trucs "physiques"
LiquidCrystal_I2C lcd(0x27,16,2);  // déclaration d'un écran LCD de 16x2 caractères avec l'adresse 0x27 sur le bus i2c
const int lVerte = 27;
const int lRouge = 25;
const int lOrange = 26;
const int lBlanche = 33;
// TODO à changer
const int pDetect = 34;
const int pBouton = 35;

// exécuté une fois au démarrage
void setup() {
  Serial.begin(115200);
  /* pins des leds en mode OUTPUT */
  pinMode(lVerte, OUTPUT);
  pinMode(lRouge, OUTPUT);
  pinMode(lOrange, OUTPUT);
  pinMode(lBlanche, OUTPUT);
  /* pins du detecteur de mouvement et du bouton en INPUT */
  pinMode(pDetect, INPUT);
  pinMode(pBouton, INPUT);
  /* allumage des leds */
  digitalWrite(lVerte, HIGH);
  digitalWrite(lRouge, HIGH);
  digitalWrite(lOrange, HIGH);
  digitalWrite(lBlanche, HIGH);
  /* initialisation du LCD */
  lcd.init();
  lcd.backlight();
  lcd.print("MCR now booting");
  /* connexion à eduroam */
  wifico();
  /* récupération du temps */
  get_time();
}

void loop() {
}

void settings_mode (){
  // dummy for now
}

void e_wifico(){
  Serial.println("[debug] failed to connect to WiFi");
  lcd.print("WiFi error !");
  for(;;){} // TODO change behaviour (AP mode ?)
}

void wifico (){
  // ces constantes sont dans secret.h
  if(!_802_1x_eap_connect(_MCR_SSID, _MCR_EAP_IDENTITY, _MCR_EAP_PASSWORD))
    e_wifico();
  else
    return; // tout s'est bien passé, nous sommes connectés
}

void e_get_time(){
  lcd.print("NTP error !");
  for(;;){} // TODO change behaviour (config mode ?)
}

void get_time (){
  struct tm tmi;
  configTime(0.0, 0, "pool.ntp.org");
  if(!getLocalTime(&tmi)){
    Serial.println("[debug] failed to obtain time");
    e_get_time();
  }
  else{
   Serial.println(&tmi, "[debug] obtained time: %A, %B %d %Y %H:%M:%S");
  }
}

void e_get_ics(int ecode){
  lcd.printf("HTTP error %d",ecode);
}

bool get_ics(){
  time_t prev_midnight, next_midnight; // TODO init
  n_events_today = CalCo::events4loc_from_url(onlineresource,ourlocation, prev_midnight, next_midnight, mcr_icvevs, c_array_len(mcr_icvevs));
   // negative return code, we got an error
  if (n_events_today < 0)
    e_get_ics(-n_events_today);
  else
    return true;
}

/* actualizes screen & leds */
void showtime(){
  struct tm *utcnow; // TODO init
  for (int ivev = 0; ivev <= c_array_len(mcr_icvevs)-1; ivev++){
    for (int islot = 0; islot <= c_array_len(mcr_slots)-1; islot++){
      mcr_slots[islot].associateVevent(&mcr_icvevs[ivev],utcnow);
    }
  }
}

