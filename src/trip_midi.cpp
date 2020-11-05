#include "WProgram.h"
#include "MIDI.h"
#include "settings.h"
extern tpxSettings * Settings;

void checkmidi(void){
   if(MIDI.read()){
     if (MIDI.getType() == NoteOn){
       

     }

     else if(MIDI.getType() == ControlChange){
         //usbMIDI.sendControlChange(Settings->getParamSetting('X', MIDICC), (char)(x>>9), Settings->getParamSetting('X', MIDICHNL));

     }

   }


}
