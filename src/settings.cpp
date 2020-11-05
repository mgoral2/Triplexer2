#include "settings.h"
#include "WProgram.h"

// modes defines for param class member mode

// TODO: parameter status menu screen OR persistent symbol in corner of LCD
// referring to which modes are active
// see defines in settings.h... MIDIUSB = 0, MIDIUART = 1, OSC = 2
//
// If we switch from either MIDIUSB or MIDIUART to OSC, we shouldn't have to erase it IFF
// OSC options are stored at different indices from the MIDI options.
// How much control do we give the user wrt OSC here?
// Couldn't there be any number of possible options, given the open nature of OSC?
   void param::update(uint8_t option, uint8_t value)
   {
      settings[option] = value; // menu.cpp already takes care of ensuring all MIDI settings
      // fall in valid range
   }
   uint8_t param::read(uint8_t option) // read current menu option value
   {
      return settings[option];
   }


   bool tpxSettings::isValid(uint8_t XMode, uint8_t YMode, uint8_t TOTMode) // check if valid combination of modes is assigned
      // to parameters (X, Y, TOT)
      // most likely done after user hits enter on mode selection screen
   {
      uint8_t temp[3] = {XMode, YMode, TOTMode};
      bool OSCflag = false;
      bool MIDIUSBflag = false;
      for (int i = 0; i < 3; ++i) {
         if (temp[i] == MIDIUSB) {
            MIDIUSBflag = true;
         }
         else if (temp[i] == OSC) {
            OSCflag = true;
         }
         else continue;
      }
      if (MIDIUSBflag && OSCflag) return false;
      else return true;
   }

      // These functions must report to the LCD menu logic if a mode combination is invalid. 
      // Try catch?
   uint8_t tpxSettings::setParamMode(char param, uint8_t modeToSet)
   {
      switch (param) {
         case 'X':
            // test for valid combination of modes with prospective mode change
            if (isValid(modeToSet, Y.mode, TOT.mode)) {
               X.mode = modeToSet;
               return 1;
            }
            else {
               Serial.println("ERROR: MIDI/USB and OSC cannot be on at the same time");
               return -1;
            }
        case 'Y':
            if (isValid(X.mode, modeToSet, TOT.mode)) {
               Y.mode = modeToSet;
               return 1;
            }
            else {
               Serial.println("ERROR: MIDI/USB and OSC cannot be on at the same time");
               return -1;
            }
        case 'T':
                      
            if (isValid(X.mode, Y.mode, modeToSet)) {
               TOT.mode = modeToSet;
               return 1;
            }
            else {
               Serial.println("ERROR: MIDI/USB and OSC cannot be on at the same time");
              return -1;
            }
        default:
            return -2; // error, invalid parameter
      }
   }
   
   uint8_t tpxSettings::getParamMode(char param)
   {
      switch (param) {
         case 'X':
            if (X.mode == MIDIUSB) {
               return MIDIUSB;
            }
            else if (X.mode == MIDIUART) {
               return MIDIUART;
            }
            else if (X.mode == OSC) {
               return OSC;
            }
            else return -1; // error
         case 'Y':
            if (Y.mode == MIDIUSB) {
               return MIDIUSB;
            }
            else if (Y.mode == MIDIUART) {
               return MIDIUART;
            }
            else if (Y.mode == OSC) {
               return OSC;
            }
            else return -1; // error
         case 'T':
            if (TOT.mode == MIDIUSB) {
               return MIDIUSB;
            }
            else if (TOT.mode == MIDIUART) {
               return MIDIUART;
            }
            else if (TOT.mode == OSC) {
               return OSC;
            }
            else return -1; // error

         default:
            return -1; // invalid parameter passed to function
      }
   }
   uint8_t tpxSettings::getParamSetting(char param, uint8_t key)
   {
      switch (param) {
         case 'X':
            return X.read(key);

         case 'Y':
            return Y.read(key);

         case 'T':
            return TOT.read(key);

         default:
            return -1; // invalid parameter passed to function

      }
   }
      // unfortunately if we want to make a generic interface that
      // the web front-end can work with, we cannot use "selected" from
      // the menu logic to infer which parameter we are configuring.
      // this means we need three separate functions to update individual
      // options -- one for each parameter
      // For error handling purposes, our very simple LCD screen/buttons
      // doesn't allow the user to input invalid values for ex: a CC number.
      // Problem is the web front-end will. LUCKILY, IF WE'RE GOING TO DO IT RIGHT,
      // error checking can happen on the front-end itself!! Not in our program here!
   uint8_t tpxSettings::setParamOption(char param, uint8_t option, uint8_t value) 
   {
      switch (param) {
         case 'X':
            X.update(option, value);
            break;
         case 'Y':
            Y.update(option, value);
            break;
         case 'T':
            TOT.update(option, value);
            break;
         default:
            return -1; // error
      }
      return 1;
   }
   uint8_t tpxSettings::enOrDisableParam(char param, bool on)
   {
      switch (param) {
         case 'X':
            if (on == true) {
               X.enabled = true;
            }
            else {
               X.enabled = false;
            }
            break;
         case 'Y':
            if (on == true) {
               Y.enabled = true;
            }
            else {
               Y.enabled = false;
            }
            break;
         case 'T':
            if (on == true) {
               TOT.enabled = true;
            }
            else {
               TOT.enabled = false;
            }
            break;
      }
      return 1;
       // need to add error handling
   }
   bool tpxSettings::isParamEnabled(char param)
   {
      switch (param) {
         case 'X':
            return X.enabled;
            break;
         case 'Y':
            return Y.enabled;
            break;
         case 'T':
            return TOT.enabled;
            break;
       
         default: // error
            return false;
      }
   }


