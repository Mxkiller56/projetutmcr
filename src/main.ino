/* -*- mode: c++; -*- */
#ifndef TESTING
#include "co2Eduroam.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "webserver.h"
#include "esp_sleep.h"
#else
#include "Arduino_testing.h"
#endif
#include "TaskMgr.h"
#include "secrets.h"
#include "CalConnector.h"
#include "ICalendarParser.h"
#include "LedsMng.h"

/* functions prototypes */
void checkPresence(void);
void scheduleTasks(void);
void deepsleep_s(void);
void reboot(void);
void wifico(void);
void e_wifico(void);
void get_time(void);
void get_time(struct tm *);
void e_get_time(void);
bool get_ics(void);
void e_get_ics(void);
void showSlot(void);
void showSlot(CourseSlot *);
CourseSlot *getActiveSlot(void);
void screeninfo(CourseSlot *);
void ledinfo(CourseSlot *);
void movestr(char *dest, char *src);
int cleanstr(char *str, char def);

/* global variables or consts */
char onlineresource[] = "https://planning.univ-rennes1.fr/jsp/custom/modules/plannings/cal.jsp?data=8241fc3873200214080677ae6bf34798e0fa50826f0818af576889429f500664906f45af276f59ae8fac93f781e861526c6c7672adf13bbdf6273afaeb8260a8c2973627c2eb073b16351c4cc23ce65f8d3f4109b6629391";
Preferences pref; // Preferences API object
char ourlocation[25] = "TD1"; // default location. Location size is 25 chars max (overwritten by preferences)
ICVevent mcr_icvevs[8];
int n_events_today=0;
CourseSlot mcr_slots[] = {
  CourseSlot(8,0,    10,0,  1), // our "timezone" is UTC+1hour
  CourseSlot(10,15,  12,15, 1), 
  CourseSlot(13,45,  15,45, 1),
  CourseSlot(16,0,   18,0,  1)
};
unsigned int btn_presses = 0;
long int remaining_show_ms = 0; // miliseconds remaining to display
long int show_start_ms = 0; // when we started to display
TaskMgr tmgr; // our task manager
// refreshes are called at the beginning of each time slot
schedEvt refreshes[c_array_len(mcr_slots)],
  scans5[c_array_len(mcr_slots)],
  scans10[c_array_len(mcr_slots)],
  scans30[c_array_len(mcr_slots)],
  last_sleep; // deepsleep from older CourseSlot stop until the morning, and you'll be freshly reset
int deepsleep_duration = 0; // global variable again, I'm sorry
// trucs "physiques"
LiquidCrystal_I2C lcd(0x27,16,2);  // déclaration d'un écran LCD de 16x2 caractères avec l'adresse 0x27 sur le bus i2c
const int lVerte = 27;
const int lRouge = 25;
const int lOrange = 26;
const int lBlanche = 33;
// TODO à changer
const int pDetect = 18;
const int pBouton = 17;

void reboot(void){
  ESP.restart();
}

/** sort of asciiz memmove */
void movestr(char *dest, char *src){
  while (*dest != '\0' && *src != '\0') // check it's not the end
    *(dest++) = *(src++);
  // shrinking ("foobar" can become "fobar" but not "fobarr" !)
  *(src-1) = '\0';
}

/** rewrites string str to fit in ascii char range */
int cleanstr(char *str, char def){
  int i,replaced=0;
  struct replace {
    const char *detect;
    const char *replace;
  };
  struct replace replacements [] = {{"é","e"},
				    {"è","e"},
				    {"É","E"},
				    {"à","a"},
				    {"À","A"}};
  while (*str != '\0'){
    // trying to replace current char with known
    // replacements
    for (i=0; i<=c_array_len(replacements)-1; i++){
      if (str == strstr(str,replacements[i].detect)){
	*str = *(replacements[i].replace);
	movestr(str+1,str+2); // the other byte is invalid then
	replaced++;
	break;
      }
    }
    // char not in "ascii printable" (see man ascii)
    // and has no known replacement, replace by default char
    if (!(*str >= 0x20 && *str <= 0x7E)){
	*str = def;
	movestr(str+1,str+2); // same thing than before
	replaced++;
    }
    str++;
  }
  return replaced;
}

/** affiche les informations du slot passé
    en paramètre sur l'écran LCD */
void screeninfo(CourseSlot *slotinfo){
  lcd.clear();
  lcd.printf("%d:%d-%d:%d, %s",slotinfo->local_tm_begin_hour,
	     slotinfo->local_tm_begin_min, slotinfo->local_tm_end_hour,
	     slotinfo->local_tm_end_min, ourlocation);
  lcd.setCursor(0,1); // next line
  if (slotinfo->getAssociatedVevent() != NULL){
    // overwriting summary via cleanstr is OK (not critical data for our program)
    cleanstr(slotinfo->getAssociatedVevent()->getSummary(),'?');
    lcd.print(slotinfo->getAssociatedVevent()->getSummary());
  }
  else {
    // no associated vevent
    lcd.print("Pas de cours");
  }
}
/** change l'état des leds en fonction
    du courseslot passé en paramètre */
void ledinfo(CourseSlot *slotinfo){
  LedColor lcolor = slotinfo->whichColor();
  struct tm utcnow;
  
  // set all leds to off first
  digitalWrite(lVerte, LOW);
  digitalWrite(lRouge, LOW);
  digitalWrite(lOrange, LOW);
  digitalWrite(lBlanche, LOW);
  switch(lcolor){
  case LedColor::RED:
    Serial.println("[debug] red led on");
    digitalWrite(lRouge, HIGH);
    break;
  case LedColor::ORANGE:
    Serial.println("[debug] orange led on");
    digitalWrite(lOrange, HIGH);
    break;
  case LedColor::GREEN:
    Serial.println("[debug] green led on");
    digitalWrite(lVerte, HIGH);
    break;
  }
  get_time(&utcnow);
  if (slotinfo->isActiveNow(&utcnow)){
    Serial.println("[debug] selected slot active, white led on");
    digitalWrite(lBlanche, HIGH);
  }
}

// exécuté une fois au démarrage
void setup() {
  Serial.begin(115200);
  Serial.println("[debug] MCR now booting");
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
  /* initialisation de l'API preferences */
  pref.begin("my-app", false);
  pref.getString("salle", ourlocation, c_array_len(ourlocation)); // may warn if "salle" not initalised. Using default val then.
  /* connexion à eduroam */
  wifico();
  /* paramétrage serveur de temps et offset */
  configTime(0.0, 0, "pool.ntp.org");  // UTC
  /* récupération du temps */
  get_time();
  /* récupération et parsing du fichier iCalendar */
  get_ics();
  /* programmation des tâches à venir */
  scheduleTasks();
  /* afficher le slot en cours */
  showSlot();
}

void loop() {
  // exec tasks first
  struct tm tm_loop_now = {};
  get_time(&tm_loop_now);
  tmgr.execTasks(mktime(&tm_loop_now));
  // button has been pressed
  // remaining seconds showing are decreasing
  // scroll display every 250ms
  // button has been pressed again
  // loop in slots
  if (digitalRead(pBouton) && remaining_show_ms <= 0){
    remaining_show_ms = 250*100;
    show_start_ms = millis();
    showSlot(); // show current slot
  }
  else if (digitalRead(pBouton) && remaining_show_ms > 0) {
    // change displayed slot
    showSlot(&(mcr_slots[btn_presses % c_array_len(mcr_slots)]));
    btn_presses++;
  } else if (!digitalRead(pBouton) && remaining_show_ms > 0){
    // scroll display
    lcd.scrollDisplayLeft();
    remaining_show_ms -= millis() - show_start_ms;
  }
}

void deepsleep_s(void){
  Serial.printf("[debug] entering in deep sleep for %d seconds", deepsleep_duration);
  // TODO: DISABLE ALL RADIOS !
  esp_deep_sleep(1000000LL * deepsleep_duration);
}

void checkPresence(void){
  // TODO
  showSlot();
}

/** schedules and inserts the tasks that the object will
 * have to execute */
void scheduleTasks(void){
  struct tm utcnow;
  get_time(&utcnow);
  time_t utcnow_t = mktime(&utcnow);
  struct tm tm_prev_midnight = utcnow;
  tm_prev_midnight.tm_sec = 0;
  tm_prev_midnight.tm_min = 0;
  tm_prev_midnight.tm_hour = 0;
  time_t prev_midnight = mktime(&tm_prev_midnight);
  time_t next_midnight = prev_midnight + 24*SECS_PER_HOUR;
  time_t last_slot_end = utcnow_t;
  time_t first_slot_beg = next_midnight + 24*SECS_PER_HOUR;
  
  // setting presence check dates and refreshes, find the oldest slot
  for (int slot=0; slot<=c_array_len(mcr_slots)-1; slot++){
    // don't schedule something in the past, would be useless
    if (mcr_slots[slot].beg2UTC(&utcnow) > utcnow_t){
      refreshes[slot].setWhen(mcr_slots[slot].beg2UTC(&utcnow)+1);
      refreshes[slot].setAction(&showSlot); // show slot 1 second after changing
      scans5[slot].setWhen(mcr_slots[slot].beg2UTC(&utcnow) + 60*5);
      scans5[slot].setAction(&checkPresence); // 5 minutes after beg
      scans10[slot].setWhen(mcr_slots[slot].beg2UTC(&utcnow) + 60*10);
      scans10[slot].setAction(&checkPresence); // 10 minutes after beg
      scans30[slot].setWhen(mcr_slots[slot].beg2UTC(&utcnow) + 60*30);
      scans30[slot].setAction(&checkPresence); // 30 minutes after beg
      // adding tasks to the task scheduler
      tmgr.addTask(&refreshes[slot]);
      tmgr.addTask(&scans5[slot]);
      tmgr.addTask(&scans10[slot]);
      tmgr.addTask(&scans30[slot]);
    }
    if (last_slot_end < mcr_slots[slot].end2UTC(&utcnow))
      last_slot_end = mcr_slots[slot].end2UTC(&utcnow); // we found something older
    if (first_slot_beg > mcr_slots[slot].beg2UTC(&utcnow) + 24*SECS_PER_HOUR) // searching first slot beginning tomorrow
      first_slot_beg = mcr_slots[slot].beg2UTC(&utcnow) + 24*SECS_PER_HOUR;
  }
  /* then set last sleep (sleeping from oldest courseslot today
   * to first courseslot tomorrow) */
  deepsleep_duration = next_midnight - last_slot_end + first_slot_beg - next_midnight;
  Serial.printf("[debug] will deepsleep when reaching %d, for %d seconds",last_slot_end+1, deepsleep_duration);
  // enter in deep sleep at the end of the last slot
  last_sleep.setWhen(last_slot_end + 1);
  last_sleep.setAction(&deepsleep_s);
  tmgr.addTask(&last_sleep);
}

void settings_mode (){
  WebServer2 ws;
  ws.begin(&pref);
  ws.blocking_run();
}

void e_wifico(){
  lcd.clear();
  Serial.println("[debug] failed to connect to WiFi");
  lcd.print("WiFi error !");
  delay(2000);
  reboot();
}

void wifico (){
  // ces constantes sont dans secret.h
  if(!_802_1x_eap_connect(_MCR_SSID, _MCR_EAP_IDENTITY, _MCR_EAP_PASSWORD))
    e_wifico();
  else
    return; // tout s'est bien passé, nous sommes connectés
}

void e_get_time(){
  lcd.clear();
  lcd.print("NTP/time error !");
  delay(2000);
  reboot();
}

/** tests if we can obtain time with NTP */
void get_time (){
  struct tm test;
  get_time(&test);
}

/** get time by modifying tmi
    main.ino funs should use this
 */
void get_time(struct tm *tmi){
  if(!getLocalTime(tmi)){
    Serial.println("[debug] failed to obtain time");
    e_get_time();
  }
  else{
   Serial.println(tmi, "[debug] obtained time: %A, %B %d %Y %H:%M:%S");
  }
}

void e_get_ics(int ecode){
  lcd.clear();
  lcd.printf("HTTP error %d",ecode);
  delay(2000);
  reboot();
}

bool get_ics(){
  struct tm tm_prev_midnight = {};
  time_t prev_midnight;
  time_t next_midnight;
  struct tm utcnow = {};
  get_time(&utcnow);
  tm_prev_midnight = utcnow;
  tm_prev_midnight.tm_sec = 0;
  tm_prev_midnight.tm_min = 0;
  tm_prev_midnight.tm_hour = 0;
  prev_midnight = mktime(&tm_prev_midnight);
  next_midnight = prev_midnight + 24*SECS_PER_HOUR;
  Serial.println(&tm_prev_midnight, "[debug] prev_midnight: %A, %B %d %Y %H:%M:%S");
  
  n_events_today = CalCo::events4loc_from_url(onlineresource,ourlocation, prev_midnight, next_midnight, mcr_icvevs, c_array_len(mcr_icvevs));
   // negative return code, we got an error
  if (n_events_today < 0)
    e_get_ics(-n_events_today); // abort
  else{ // continue and associate vevents to slots
    for (int ivev = 0; ivev <= c_array_len(mcr_icvevs)-1; ivev++){
      for (int islot = 0; islot <= c_array_len(mcr_slots)-1; islot++){
	mcr_slots[islot].associateVevent(&mcr_icvevs[ivev],&utcnow);
      }
    }
    return true;
  }
}

void showSlot(CourseSlot *cslot){
  screeninfo(cslot);
  ledinfo(cslot);
}

void showSlot(void){
  if (getActiveSlot() != NULL)
    showSlot(getActiveSlot());
}

CourseSlot *getActiveSlot(void){
  struct tm utcnow;
  get_time(&utcnow);

  for (int slot=0; slot<=c_array_len(mcr_slots)-1; slot++){
    if(mcr_slots[slot].isActiveNow(&utcnow)){
      return &mcr_slots[slot]; // found active slot
    }
  }
  return NULL; // no slot is active
}
