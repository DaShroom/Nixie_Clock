#include <Wire.h>
#include <NixieTube.h>
#include "RTClib.h"

RTC_DS1307 RTC;

NixieTube tube(11,12,13,10, 1);  // NixieTube(DIN,ST,SH,OE,NUM)
//  DIN(D11) - PIN1
//  ST(D12)  - PIN2
//  SH(D13)  - PIN3
//  OE(D10)  - PIN4
//  GND      - PIN5
//  5v       - PIN6  
//  The pin number of the module is defined in the datasheet


unsigned long last;
byte d[4];
byte h,m;

//RTC_DS1307 RTC;

void setup(){
  Wire.begin();
  RTC.begin();
}

unsigned long backgroundCtrl_Timer = 0;  // the timer for managing backlight
unsigned long numCtrl_Timer = 0;         // the timer for managing number displayed

void loop(){

  static int dispVal = 0;
  static boolean dir = true;
  static byte brightness = 0;
  static int colorNum = 0;    // the number of color in array of the library 

  if(millis() >= (last + 30000)) {
    DateTime now = RTC.now();
    h = now.hour();
    m = now.minute();
    d[1] = (h/10);
    d[2] = (h-(d[1]*10));
    d[3] = (m/10);
    d[4] = (m-(d[3]*10));
    showtime();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(millis() - backgroundCtrl_Timer > 5){ // Manage the backlight controlling flash rate

    if(dir)  tube.setBrightness(++brightness);    // Set the brightness of the background
    else tube.setBrightness(--brightness);

    if(brightness > 250)  dir = false;
    else if(brightness < 5)  dir = true,colorNum++;

    if(colorNum > 12) colorNum = 0;
    tube.setBackgroundColor((Color)7);
    backgroundCtrl_Timer = millis();
  }

  if(millis() - numCtrl_Timer > 100){ // Manage the number controlling flash rate
    tube.setNumber(++dispVal);     // set the number to display
    tube.display();                   // Show the number
    numCtrl_Timer = millis();
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////

void showtime() {
  unsigned long timer;
  byte fade = 1;
  byte bright = 6;
  tube.setBrightness(bright);
  tube.display();
  for(byte j = 0; j<2; j++) {
    for(byte i = 0; i<4; i++) {
      byte bflag = 1;
      while (bflag) {

        if(fade)  tube.setBrightness(++bright);
        else  tube.setBrightness(--bright);
        if(bright > 250)  fade = 0;
        else if(bright < 5) {
          fade = 1;
          bflag = 0;
        }
        tube.setBackgroundColor((Color)(i+1));
        tube.setNumber(d[(i+1)]);
        tube.display();
        delay(10);
      }
      last = millis();
    }
  }
}

