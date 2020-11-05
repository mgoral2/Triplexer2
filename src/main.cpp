#include "WProgram.h"
#include "menu.h"
#include "ADC.h"
#include "IntervalTimer.h"
#include "LiquidCrystal.h"
#include "trip_osc.h"
#include "trip_adc.h"
#include "MIDI.h"
#include "trip_midi.h"



//static int * raw;
const int channel = 1;

//display macros
               // RS, E, D4, D5, D6, D7
LiquidCrystal lcd(18, 19, 20, 21, 22, 23);

//DFRkeypad button defines
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5  


void  init() {
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   //pinMode(0, INPUT);
   digitalWriteFast(13, LOW);
   pinMode(PIN_ENTER, INPUT_PULLUP); // menu buttons
   pinMode(PIN_UP, INPUT_PULLUP);
   pinMode(PIN_DOWN, INPUT_PULLUP);
   pinMode(PIN_BACK, INPUT_PULLUP);
   pinMode(PIN_SAVE, INPUT_PULLUP);
   MIDI.begin(1);
   lcd.begin(16,2);
   debounceInit();

   showMenu();
}

extern "C" int main() 
{
  init();
  adcinit();
  oscinit();
  dispCalibrate();
  //adcCalibrate();
   while (1) {
   browseMenu();
   checkmidi();
   //checkosc(); //this function only works inside a while loop
   } 
}




