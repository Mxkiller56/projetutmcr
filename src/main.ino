/* -*- mode: c++; -*- */
#ifndef TESTING
#include "co2Eduroam.h"
#elif
#include "Arduino_testing.h"
#endif
#include "secrets.h"
#include "CalConnector.h"
#include "ICalendarParser.h"
#include "LedsMng.h"

char onlineresource[] = "wget https://planning.univ-rennes1.fr/jsp/custom/modules/plannings/cal.jsp?data=8241fc3873200214080677ae6bf34798e0fa50826f0818af576889429f500664906f45af276f59ae8fac93f781e861526c6c7672adf13bbdf6273afaeb8260a8c2973627c2eb073b16351c4cc23ce65f8d3f4109b6629391";
char ourlocation[] = "TD3";
ICVevent mcr_icvevs[8];
int n_events_today=0;
CourseSlot mcr_slots[] = {
  CourseSlot(8,0,    10,0,  1), // our "timezone" is UTC+1hour
  CourseSlot(10,15,  12,15, 1), 
  CourseSlot(13,45,  15,45, 1),
  CourseSlot(16,0,   18,0,  1)
};


void setup() {
  Serial.begin(115200);
}

void loop() {
}

void settings_mode (){
  // dummy for now
}

void wifico (){
  if(!_802_1x_eap_connect(_MCR_SSID, _MCR_EAP_IDENTITY, _MCR_EAP_PASSWORD))
    e_wifico();
  else
    return;
}

void e_wifico(){
  // print some message on the lcd, connexion failed
}

bool get_time (){
  // dummy for now
}

void e_get_time(){
}

bool get_ics(){
  time_t prev_midnight, next_midnight; // TODO init
  n_events_today = CalCo::events4loc_from_url(onlineresource,ourlocation, prev_midnight, next_midnight, mcr_icvevs, c_array_len(mcr_icvevs));
   // negative return code, we got an error
  if (n_events_today < 0)
    e_get_ics();
  else
    return true;
}

void e_get_ics(){
  // print some message on the lcd with http error code
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

