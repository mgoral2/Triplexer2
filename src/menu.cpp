#include "menu.h"
#include "settings.h"
#include "LiquidCrystal.h"
#include "DFRkeypad.h"
#include "Bounce2.h"
#include "EEPROM.h"
#include "trip_adc.h"
/* TODO: When switching a param from MIDI to OSC or OSC to MIDI, change the value of selected accordingly
 * so the menu doesn't return to the MIDI specific config menu if param is in OSC mode, or vice-versa
 * */
//#include <string>
extern LiquidCrystal lcd; // lcd object is declared and initialized in main.cpp
unsigned char selected = 1;
unsigned char xCfg = 14; // menu initially thinks all parameters are in MIDI mode. May want to change later
unsigned char yCfg = 26; // these are just initializations
unsigned char totCfg = 38;
tpxSettings * Settings = new tpxSettings; // how do I share this instance with the future web front-end?
const char * modemap[3] = {"MIDI/USB","MIDI/UART","OSC"};
const char * curvemap[3] = {"LINEAR W EDGES", "LINEAR", "LOGARITHMIC"};

const char menu_000[] = "[Main Menu]";        // 0
const char menu_001[] = "Config";             // 1
const char menu_002[] = "Calibration";        // 2
const char menu_003[] = "Presets";            // 3
/* TODO: SAVE DEFAULT INITIAL STATES TO EEPROM FOR EXPO
 * AND LOAD THEM ON STARTUP
*/
const char menu_100[] = "[Config param]";     // 4
const char menu_101[] = "X";                  // 5
const char menu_102[] = "Y";                  // 6                       
const char menu_103[] = "TOT";                // 7

const char menu_110[] = "[Load preset]";     // 8
const char menu_111[] = "Preset 1";          // 9
const char menu_112[] = "Preset 2";          // 10
const char menu_113[] = "Preset 3";          // 11
const char menu_114[] = "Preset 4";          // 12

const char menu_200[] = "[Config X]";         // 13
const char menu_201[] = "MIDI channel";       // 14
const char menu_202[] = "MIDI CC #";          // 15
const char menu_203[] = "INVERT X";           // 16
const char menu_204[] = "MODE";               // 17
const char menu_209[] = "EN/DISABLE X";       // 18
const char menu_299[] = "X RESPONSE CRV";     // 19

const char menu_205[] = "[Config X OSC]";     // 20
const char menu_206[] = "INVERT X";           // 21
const char menu_207[] = "MODE";               // 22
                                              // 23
                                              // 24

const char menu_210[] = "[Config Y]";         // 25
const char menu_211[] = "MIDI channel";       // 26
const char menu_212[] = "MIDI CC #";          // 27
const char menu_213[] = "INVERT Y";           // 28
const char menu_214[] = "MODE";               // 29
const char menu_218[] = "EN/DISABLE Y";       // 30
const char menu_219[] = "Y RESPONSE CRV";     // 31

const char menu_215[] = "[Config Y OSC]";     // 32
const char menu_216[] = "INVERT Y";           // 33
const char menu_217[] = "MODE";               // 34
                                              // 35
                                              // 36

const char menu_220[] = "[Config TOT]";       // 37
const char menu_221[] = "MIDI channel";       // 38
const char menu_222[] = "MIDI CC #";          // 39
const char menu_223[] = "INVERT TOT";         // 40
const char menu_224[] = "MODE";               // 41
const char menu_228[] = "EN/DISABLE TOT";     // 42
const char menu_229[] = "T RESPONSE CRV";     // 43

const char menu_225[] = "[Config TOT OSC]";   // 44
const char menu_226[] = "INVERT TOT";         // 45
const char menu_227[] = "MODE";               // 46
                                              // 47
                                              // 48
//const char* DFRkeypad::sKEY[]=                          { "---",       "Right",   "Up", "Down", "Left", "Select", "???" };
//
static Bounce back = Bounce();
static Bounce up = Bounce();
static Bounce down = Bounce();
static Bounce enter = Bounce();
static Bounce save = Bounce();

void debounceInit() {

   back.attach(PIN_BACK);
   back.interval(50);

   up.attach(PIN_UP);
   up.interval(50);

   down.attach(PIN_DOWN);
   down.interval(50);

   enter.attach(PIN_ENTER);
   enter.interval(50); 

   save.attach(PIN_SAVE);
   save.interval(50);
   return;
}

int getButtonPress() {
   enter.update();
   up.update();
   down.update();
   back.update();
   save.update();

   if (enter.fell()){
      return ENTER;
   }
   else if (up.fell()) {
      return UP;
   }
   else if (down.fell()) {
      return DOWN;
   }
   else if (back.fell()) {
      return BACK;
   }
   else if (save.fell()) {
      return SAVE;
   }
   else return -1; // no button press detected
}
// we can deduce which parameter is being configured based on 
// the current value of 'selected'. All other config menu functions
// use this same logic 
void configMIDICC() {
   char param = 'A'; // initialization
   uint8_t CCnum = 0;
   int keypress = 0;
   if (selected == (xCfg+1)) {
        param = 'X';
        CCnum = Settings->getParamSetting('X', MIDICC);
   }
   else if (selected == (yCfg+1)) {
        param = 'Y';
        CCnum = Settings->getParamSetting('Y', MIDICC);
   }
   else if (selected == (totCfg+1)) {
        param = 'T';
        CCnum = Settings->getParamSetting('T', MIDICC);
   }
   else {
      lcd.clear();
      lcd.print("ERROR: invalid");
   }
   
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[MIDI CC #]");
   lcd.setCursor(14,0);
   lcd.print(param);
   do {
      keypress = getButtonPress();
      lcd.setCursor(1,1);
      lcd.print(CCnum);
      if (keypress == UP) {
         if (CCnum == 127) { 
            CCnum = 1;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[MIDI CC #]");
            lcd.setCursor(14,0);
            lcd.print(param);
         }
         else {
            ++CCnum;
         }
      }
      else if (keypress == DOWN) { 
         if (CCnum == 1) {
            CCnum = 127;
         }
         else if (CCnum == 100 || CCnum == 10) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[MIDI CC #]");
            lcd.setCursor(14, 0);
            lcd.print(param);
            --CCnum;
         }
         else {
            --CCnum;
         }
      }
   } while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back

   if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamOption('X', MIDICC, CCnum) != 1) {
            lcd.clear();
            lcd.print("Error setting CC!");
            Serial.println("Error setting X CC num");
         }
         break;
      case 'Y':
         if (Settings->setParamOption('Y', MIDICC, CCnum) != 1) {
            lcd.clear();
            lcd.print("Error setting CC!");
            Serial.println("Error setting Y CC num");
         }
         break;
      case 'T':
         if (Settings->setParamOption('T', MIDICC, CCnum) != 1) {
            lcd.clear();
            lcd.print("Error setting CC!");
            Serial.println("Error setting TOT CC num");
         }
         break;
   }
   lcd.setCursor(1,1);
   lcd.print("CC # ");
   lcd.print(CCnum);
   lcd.print(" OK");
   delay(1000); // allow user time to see confirmation 
   // of midi CC number
   return;
}

void configMIDIChannel() {
  char param = 'A'; // initialization
  uint8_t CHnum = 0;
  int keypress = 0;
  if (selected == xCfg) {
    param = 'X';
    CHnum = Settings->getParamSetting('X', MIDICHNL);
  }
  else if (selected == yCfg) {
    param = 'Y';
    CHnum = Settings->getParamSetting('Y', MIDICHNL);
  }
  else if (selected == totCfg) {
    param = 'T';
    CHnum = Settings->getParamSetting('T', MIDICHNL);
  }
  else {
     lcd.clear();
     lcd.print("ERROR: Invalid");
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[MIDI CHNL #]");
  lcd.setCursor(14,0);
  lcd.print(param);
  do {
     keypress = getButtonPress();
     lcd.setCursor(1,1);
     lcd.print(CHnum);
     if (keypress == UP) { // up
   // accounting for when *CHnum goes from
   // two digits to one digit in length -- second
   // digit persists on LCD unless we redraw the
   // screen entirely. Not elegant, but works for 
   // now
        if (CHnum == 16) {
           CHnum = 1;
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("[MIDI CHNL #]");
           lcd.setCursor(14,0);
           lcd.print(param);
        } 
        else {
          ++CHnum;
        }
     }
     else if (keypress == DOWN) { // down
       if (CHnum == 1) {
         CHnum = 16;
       }
       else if (CHnum == 10) {
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("[MIDI CHNL #]");
           lcd.setCursor(14,0);
           lcd.print(param);
           --CHnum;
       }
       else {
         --CHnum;
       }
     }
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
  if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamOption('X', MIDICHNL, CHnum) != 1) {
            Serial.println("Error setting X channel num");
         }
         break;
      case 'Y':
         if (Settings->setParamOption('Y', MIDICHNL, CHnum) != 1) {
            Serial.println("Error setting Y channel num");
         }
         break;
      case 'T':
         if (Settings->setParamOption('T', MIDICHNL, CHnum) != 1) {
            Serial.println("Error setting TOT channel num");
         }
         break;
   }
  lcd.setCursor(1,1);
  lcd.print("Channel ");
  lcd.print(CHnum);
  lcd.print(" OK");
  delay(1000); // allow user time to see confirmation
 // of MIDI channel 
  return;
}

void configINV()
{
  uint8_t inv = 0;
  char param = 'A';
  int keypress = 0;
  if (selected == (xCfg+2) || selected == (xCfg+7)) {
       param = 'X';
       inv = Settings->getParamSetting('X', INV);
  }
  else if (selected == (yCfg+2) || selected == (yCfg+7)) {
       param = 'Y';
       inv = Settings->getParamSetting('Y', INV);
  }
  else if (selected == (totCfg+2) || selected == (totCfg+7)) {
       param = 'T';
       inv = Settings->getParamSetting('T', INV);
  }
  else {
     lcd.clear();
     lcd.print("ERROR: invalid");
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[INVERT PARAM]");
  lcd.setCursor(15,0);
  lcd.print(param);
  do {
    keypress = getButtonPress();
    lcd.setCursor(1,1);
    if (inv == 0) {
       lcd.print("NORMAL");
    }
    else {
      lcd.print("INVERT");
    }
    if ((keypress == UP || keypress == DOWN) && (inv == 0)) {
      inv = 1;
    }
    else if ((keypress == UP || keypress == DOWN) && (inv == 1)) {
      inv = 0;
    }
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
   if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamOption('X', INV, inv) != 1) {
            Serial.println("Error setting X channel num");
         }
         break;
      case 'Y':
         if (Settings->setParamOption('Y', INV, inv) != 1) {
            Serial.println("Error setting Y channel num");
         }
         break;
      case 'T':
         if (Settings->setParamOption('T', INV, inv) != 1) {
            Serial.println("Error setting TOT channel num");
         }
         break;
   }
  lcd.setCursor(1,1);
  lcd.print(param);
  lcd.print(" ");
  if (inv == 0) {
    lcd.print("NORMAL");
  }
  else {
    lcd.print("INVERT");
  }
  lcd.print(" OK");
  delay(1000); // give user time to see confirmation of invert parameter setting

  return;
}

void configMode()
{
  uint8_t mode = 0;
  char param = 'A';
  int keypress = 0;
  if (selected == xCfg+3 || selected == xCfg+8) {
       param = 'X';
       mode = Settings->getParamMode(param);
       Serial.println("X mode is ");
       Serial.print(mode);
  }
  else if (selected == yCfg+3 || selected == yCfg+8) {
       param = 'Y';
       mode = Settings->getParamMode(param);
       Serial.println("Y mode is ");
       Serial.print(mode);
  }
  else if (selected == totCfg+3 || selected == totCfg+8) {
       param = 'T';
       mode = Settings->getParamMode(param);
       Serial.print(mode);
       Serial.println("T mode is ");
  }
  else {
     lcd.setCursor(0,0);
     lcd.print("ERROR: invalid");
     return;
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[SET MODE FOR]");
  lcd.setCursor(15,0);
  lcd.print(param);
  do {
    keypress = getButtonPress();
    lcd.setCursor(1,1);
    lcd.print(modemap[(int)mode]);

   if (keypress == UP) {
      if (mode == 2) { 
         mode = 0;
      }
      else {
         ++mode;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("[SET MODE FOR]");
      lcd.setCursor(15,0);
      lcd.print(param);
   }
   else if (keypress == DOWN) { 
      if (mode == 0) {
         mode = 2;
      }
      else {
         --mode;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("[SET MODE FOR]");
      lcd.setCursor(15,0);
      lcd.print(param);
   }
   else continue;
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
  if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamMode('X', mode) != 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("USB in use");
            lcd.setCursor(0,1);
            lcd.print("Resetting X");
            delay(2000);
            return;
         }
         else {
            if (mode == OSC) {
               selected = xCfg + 7;
            }
            else {
               selected = xCfg;
            }
         }
         break;
      case 'Y':
         if (Settings->setParamMode('Y', mode) != 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("USB in use");
            lcd.setCursor(0,1);
            lcd.print("Resetting Y");
            delay(2000);
            return;
         }
         else {
            if (mode == OSC) {
               selected = yCfg + 7;
            }
            else {
               selected = yCfg;
            }
         }
         break;
      case 'T':
         if (Settings->setParamMode('T', mode) != 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("USB in use");
            lcd.setCursor(0,1);
            lcd.print("Resetting TOT");
            delay(2000);
            return;
         }
         else {
            if (mode == OSC) {
               selected = totCfg + 7;
            }
            else {
               selected = totCfg;
            }
         }
         break;
   }
   
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(param);
   lcd.print(" mode ");
   lcd.print(modemap[(int)mode]);
   lcd.setCursor(1,1);
   lcd.print(" OK");
   delay(1000);
} 
void toggleOnOff() {
  char param = 'A'; // initialization
  bool on = false;
  int keypress = 0;
  if (selected == xCfg+4 || selected == xCfg+9) {
       param = 'X';
       on = Settings->isParamEnabled(param);
  }
  else if (selected == yCfg+4 || selected == yCfg+9) {
       param = 'Y';
       on = Settings->isParamEnabled(param);
  }
  else if (selected == totCfg+4 || selected == totCfg+9) {
       param = 'T';
       on = Settings->isParamEnabled(param);
  }
  else {
     lcd.setCursor(0,0);
     lcd.print("ERROR: invalid");
     return;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[EN/DISABLE] ");
  lcd.setCursor(15,0);
  lcd.print(param);
  do {
    keypress = getButtonPress();
    lcd.setCursor(1,1);
    if (on == 0) {
       lcd.print("DISABLED");
    }
    else {
      lcd.print(" ENABLED");
    }
    if ((keypress == UP || keypress == DOWN) && (on == false)) {
      on = true;
    }
    else if ((keypress == UP || keypress == DOWN) && (on == true)) {
      on = false;
    }
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
   if (keypress == BACK) return;

  if (Settings->enOrDisableParam(param, on) != 1) {
     lcd.setCursor(1,1);
     lcd.print("ERROR");
     return;
  }

  lcd.setCursor(1,1);
  lcd.print(param);
  lcd.print(" ");
  if (on == false) {
    lcd.print("DISABLED");
  }
  else {
    lcd.print("ENABLED");
  }
  lcd.print(" OK");
  delay(1000); // give user time to see confirmation of invert parameter setting
}

void configResponseCurve()
{
   int keypress = 0;
   char param = 'A'; // initialization
   uint8_t curve = 0;

   if (selected == xCfg + 5 || selected == xCfg + 10) {
      param = 'X';
      curve = Settings->getParamSetting('X', RESPCURVE);
   }
   else if (selected == yCfg + 5 || selected == yCfg + 10) {
      param = 'Y';
      curve = Settings->getParamSetting('Y', RESPCURVE);
   }
   else if (selected == totCfg + 5 || selected == totCfg + 10) {
      param = 'T';
      curve = Settings->getParamSetting('T', RESPCURVE);
   }
   else {
      lcd.clear();
      lcd.print("ERR: INVALID");
      return;
   }
   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("[SET CURVE] ");
    lcd.print(param);
    lcd.setCursor(0,1);
    lcd.print("-");
    do {
       keypress = getButtonPress();
       lcd.setCursor(1,1);
       lcd.print(curvemap[curve]);
       if (keypress == UP) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("[SET CURVE] ");
          lcd.print(param);
          lcd.setCursor(0,1);
          lcd.print("-");
          if (curve == LOGARITHMIC) {
             curve = LINEAR_W_EDGES;
          }
          else ++curve;
       }
       else if (keypress == DOWN) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("[SET CURVE] ");
          lcd.print(param);
          lcd.setCursor(0,1);
          lcd.print("-");
          if (curve == LINEAR_W_EDGES) {
             curve = LOGARITHMIC;
          }
          else --curve;
       }
    } while (keypress != ENTER && keypress != BACK);
    if (keypress == BACK) return;
    if (Settings->setParamOption(param, RESPCURVE, curve) != 1) {
       lcd.clear();
       lcd.print("ERROR setting");
       return;
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(param);
    lcd.print(" RESP CURVE OK");
    lcd.setCursor(0,1);
    lcd.print(curvemap[curve]);
    delay(1000);
    return;
}

/* TODO: SAVE DEFAULT INITIAL STATES TO EEPROM FOR EXPO
 * AND LOAD THEM ON STARTUP
*/
// all the user needs to do at this time is choose a preset to save to EEPROM
void savePreset()
{
   uint8_t choice = 1;
   int keypress = 0;

   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[SAVE TO EEPROM]");
   lcd.setCursor(0,1);
   lcd.print("-#");
   do {
      keypress = getButtonPress();     
      lcd.setCursor(2,1);
      lcd.print(choice);
      if (keypress == UP) {
         if (choice == 4) {
            choice = 1;
         }
         else ++choice;
      }
      else if (keypress == DOWN) {
         if (choice == 1) {
            choice = 4;
         }
         else --choice;
      }
   } while (keypress != BACK && keypress != ENTER);
   if (keypress == BACK) return;
   if (choice >= 1 && choice <= 4) {
      EEPROM.put((choice-1)*sizeof(tpxSettings),*Settings);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SAVED TO");
      lcd.setCursor(0,1);
      lcd.print("PRESET ");
      lcd.print(choice);
      delay(1000);
   }
   else {
      lcd.clear();
      lcd.print("ERROR SAVING");
      delay(1000);
   }
   return;
}
   
void loadPreset()
{
  lcd.clear();
  uint8_t choice = 0;
  if (selected == 9) {
     choice = 1;
  }
  else if (selected == 10) {
     choice = 2;
  }
  else if (selected == 11) {
     choice = 3;
  }
  else if (selected == 12) {
     choice = 4;
  }
  else {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("ERR: invalid");
     delay(1000);
     return;
  }
  EEPROM.get((choice-1)*sizeof(tpxSettings), *Settings);
  lcd.setCursor(0,0);
  lcd.print("Preset ");
  lcd.print(choice);
  lcd.setCursor(1,1);
  lcd.print(" loaded");
  delay(1000);
  return;
 // Serial.println("from EEPROM:");
 // Serial.println("PARAM STATUS:");
 // Serial.println(Settings->isParamEnabled('X'));
 // Serial.println(Settings->isParamEnabled('Y'));
 // Serial.println(Settings->isParamEnabled('T'));
 // Serial.println("XMIDICC: ");
 // Serial.println(Settings->getParamSetting('X', 1));
 // Serial.println("YMIDICC: ");
 // Serial.println(Settings->getParamSetting('Y', 1));
 // Serial.println("TOTMIDICC: ");
 // Serial.println(Settings->getParamSetting('T', 1));
 // Serial.println("X mode: ");
 // Serial.println(Settings->getParamMode('X'));
 // Serial.println("Y mode: ");
 // Serial.println(Settings->getParamMode('Y'));
 // Serial.println("T mode: ");
 // Serial.println(Settings->getParamMode('T'));

}

void dispCalibrate()
{
   disableInterrupts();
   int keypress = 0;
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[Calibrate]");
   lcd.setCursor(0,1);
   lcd.print("Proceed?");
   do {
      keypress = getButtonPress();
   } while (keypress != BACK && keypress != ENTER);
   if (keypress == BACK) return;
   else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Remove your foot");
      delay(1500);
      calibrateZero();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calibrate max");
      lcd.setCursor(0,1);
      lcd.print("Press down");
      delay(1500);
      calibrateMax();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Done!");
      delay(1500);
      enableInterrupts();
   }
}

MenuEntry menu[] =
{
   {menu_000, 4, 0, 0, 0, 0, 0}, // [Main Menu]         0
   {menu_001, 4, 1, 2, 5, 1, 0},                      // 1
   {menu_002, 4, 1, 3, 2, 1, dispCalibrate},          // 2
   {menu_003, 4, 2, 3, 9, 1, 0}, // Presets           // 3
                                                  
   {menu_100, 4, 0, 0, 0, 0, 0}, // [Config param]    // 4
   {menu_101, 4, 5, 6, xCfg, 1, 0},                   // 5
   {menu_102, 4, 5, 7, yCfg, 1, 0},                   // 6 
   {menu_103, 4, 6, 7, totCfg, 1, 0},                 // 7

   {menu_110, 5, 0, 0, 0, 0, 0},                      // 8 [Load Presets]
   {menu_111, 5, 9, 10, 9, 1, loadPreset},            // 9
   {menu_112, 5, 9, 11, 10, 1, loadPreset},           // 10
   {menu_113, 5, 10, 12, 11, 1, loadPreset},          // 11
   {menu_114, 5, 11, 12, 12, 1, loadPreset},          // 12
                                                  
   {menu_200, 7, 0, 0, 0, 0, 0}, // [Config X]        // 13
   {menu_201, 7, 14, 15, 14, 5, configMIDIChannel},   // 14           
   {menu_202, 7, 14, 16, 15,  5, configMIDICC},        // 15
   {menu_203, 7, 15, 17, 16, 5, configINV},           // 16
   {menu_204, 7, 16, 18, 17, 5, configMode},          // 17
   {menu_209, 7, 17, 19, 18, 5, toggleOnOff},         // 18
   {menu_299, 7, 18, 19, 19, 5, configResponseCurve}, // 19

   {menu_205, 5, 0, 0, 0, 0, 0}, // [Config X OSC]    // 20
   {menu_206, 5, 21, 22, 21, 5, configINV},                   // 21
   {menu_207, 5, 21, 23, 22, 5, configMode},          // 22
   {menu_209, 5, 22, 24, 23, 5, toggleOnOff},         // 23
   {menu_299, 5, 23, 24, 24, 5, configResponseCurve}, // 24
                                                 
   {menu_210, 7, 0, 0, 0, 0, 0}, // [Config Y]        // 25
   {menu_211, 7, 26, 27, 26, 5, configMIDIChannel},   // 26
   {menu_212, 7, 26, 28, 27, 5, configMIDICC},        // 27
   {menu_213, 7, 27, 29, 28, 5, configINV},           // 28
   {menu_214, 7, 28, 30, 29, 5, configMode},          // 29
   {menu_218, 7, 29, 31, 30, 5, toggleOnOff},         // 30
   {menu_219, 7, 30, 31, 31, 5, configResponseCurve}, // 31

   {menu_215, 5, 0, 0, 0, 0, 0}, // [Config Y OSC]    // 32
   {menu_216, 5, 33, 34, 33, 5, configINV},                   // 33
   {menu_217, 5, 33, 35, 34, 5, configMode},          // 34
   {menu_218, 5, 34, 36, 35, 5, toggleOnOff},         // 35
   {menu_219, 5, 35, 36, 36, 5, configResponseCurve}, // 36
                                                
   {menu_220, 7, 0, 0, 0, 0, 0},  // [Config TOT]     // 37
   {menu_221, 7, 38, 39, 38, 5, configMIDIChannel},   // 38
   {menu_222, 7, 38, 40, 39, 5, configMIDICC},        // 39
   {menu_223, 7, 39, 41, 40, 5, configINV},           // 40
   {menu_224, 7, 40, 42, 41, 5, configMode},          // 41
   {menu_228, 7, 41, 43, 42, 5, toggleOnOff},         // 42
   {menu_229, 7, 42, 43, 43, 5, configResponseCurve}, // 43

   {menu_225, 5, 0, 0, 0, 0, 0}, // [Config TOT OSC]  // 44
   {menu_226, 5, 45, 46, 45, 5, configINV},                   // 45
   {menu_227, 5, 45, 47, 46, 5, configMode},          // 46
   {menu_228, 5, 46, 48, 47, 5, toggleOnOff},         // 47
   {menu_229, 5, 47, 48, 48, 5, configResponseCurve}, // 48

};
void showMenu() {
   unsigned char from = 0;
   unsigned char to = 0;
   //unsigned char temp = 0;
   unsigned char line = 0;

   while (to <= selected) {
      to += menu[to].num_menupoints;
   }
   from = to - menu[selected].num_menupoints;
   --to;
   
   //temp = from; temp is used for screens with more than two rows
   lcd.clear();
   
   if ( (selected < (from + 2)) ) {
      to = from + 1;

      for (from; from<=to; ++from) {
         lcd.setCursor(1,line);
         lcd.print(menu[from].text);
         ++line;
      }
      lcd.setCursor(0,1);
      lcd.print("-");
   }
   else if ( (selected >= (from + 2)) ) {
      from = selected - 1;
      to = from + 1;

      for (from; from<=to; ++from) {
         lcd.setCursor(1,line);
         lcd.print(menu[from].text);
         ++line;
      }
      lcd.setCursor(0,1);
      lcd.print("-");
   }
   else {
      lcd.setCursor(0,0);
      lcd.print("ERR: Invalid");
      lcd.setCursor(0,1);
      lcd.print("selection!"); 
   }
   delay(50);
}

void menuCfg() {
   if (Settings->getParamMode('X') == OSC) {
      menu[5].enter = 21;
   }
   else {
      menu[5].enter = 14;
   }
   if (Settings->getParamMode('Y') == OSC) {
      menu[6].enter = 33;
   }
   else {
      menu[6].enter = 26;
   }
   if (Settings->getParamMode('T') == OSC) {
      menu[7].enter = 45;
   }
   else {
      menu[7].enter = 38;
   }
}


void browseMenu() {
   menuCfg();

#ifdef DFRKEYPAD
   static char buffer[15];
   uint8_t key = DFRkeypad::GetKey();
   strcpy(buffer, DFRkeypad::KeyName(key));

   if (!strcmp(buffer, DFRkeypad::sKEY[2])) { // up
     selected = menu[selected].up;
   } 
   if (!strcmp(buffer, DFRkeypad::sKEY[3])) { // down
      selected = menu[selected].down;
   }
   if (!strcmp(buffer, DFRkeypad::sKEY[4])) { // back (actually LEFT KEY)
      selected = menu[selected].back;
   }
   if (!strcmp(buffer, DFRkeypad::sKEY[1])) { // enter (actually RIGHT KEY)
      if (menu[selected].fp != 0 ) {
         menu[selected].fp();
      }
      selected = menu[selected].enter;
   }
   if (!strcmp(buffer, DFRkeypad::sKEY[0])) {
      return;
   }
   else {
      showMenu(); // only redraw menu if a keypress is detected
  }
#endif // DFRKEYPAD
//   Serial.print("*  key: ");
//   Serial.print(analogRead(A8));
//   Serial.print(" ");
//   Serial.println(buffer);
   int keypress = getButtonPress();

   if (keypress == BACK) {
      //Serial.println("BACK pressed");
      selected = menu[selected].back;
   }
   if (keypress == UP) {
      //Serial.println("UP pressed");
      selected = menu[selected].up;
   }
   if (keypress == DOWN) {
      //Serial.println("DOWN pressed");
      selected = menu[selected].down;
   }
   if (keypress == ENTER) {
      //Serial.println("ENTER pressed");
      if (menu[selected].fp != 0 ) {
         menu[selected].fp(); // fp takes care of drawing menu
      }  // may want to change this in the future
      selected = menu[selected].enter;
   }
   if (keypress == SAVE) {
      //Serial.println("SAVE pressed");
      savePreset();
      // if SAVE is pressed, when the user is done saving, they return to wherever
      // they were before in the menu
   }
   if (keypress != -1) { // only redraw menu if keypress is detected
      disableInterrupts();
      showMenu();
      enableInterrupts();
   }
}

