//DFRobot.com
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
const int lVerte = 27;
const int lRouge = 25;
const int lOrange = 26;
const int lBlanche = 33;
void setup()
{
	pinmode(lVerte, OUTPUT);
	pinmode(lRouge, OUTPUT);
	pinmode(lOrange, OUTPUT);
	pinmode(lBlanche, OUTPUT);
	digitalWrite(lVerte, HIGH);
	digitalWrite(lRouge, HIGH);
	digitalWrite(lOrange, HIGH);
	digitalWrite(lBlanche, HIGH);
  lcd.init();
  lcd.backlight();
  lcd.print("Hello, world!");
}

void loop()
{
}