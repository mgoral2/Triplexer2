#include <stdint.h>
#include <map>
// convenient defines to pass to setParamMode, get from getParamMode 
#define MIDIUSB 0
#define MIDIUART 1
#define OSC 2
// convenient defines for use with getParamOption, setParamOption, which take uint8's
#define MIDICHNL 0
#define MIDICC 1
#define INV 2
#define RESPCURVE 3

#define LINEAR_W_EDGES 0
#define LINEAR_NO_EDGES 1
#define LOGARITHMIC 2
class param {
   private:
      uint8_t settings[6];
   public:
      bool enabled;
      uint8_t mode;
      param() : enabled(false), mode(MIDIUART) // on instantiation all params are initially disabled
      {
       // initialize settings map! 
         settings[0] = 1; // MIDI Channel
         settings[1] = 1; // MIDI CC number
         settings[2] = 0; // invert
         settings[3] = 0; // response curve, default to linear with edges.
         // 0: linear w/ edges, 1: linear, 2: logarithmic
      }
      // does no error checking. rely on front-end to do it
      // which is exactly what the LCD menu implementation does right now
      void update(uint8_t option, uint8_t value); 
      uint8_t read(uint8_t option);
};

class tpxSettings {
   private:
      param X, Y, TOT;
   public:
      bool isValid(uint8_t XMode, uint8_t YMode, uint8_t TOTMode); 
      bool isParamEnabled(char param);
      uint8_t setParamMode(char param, uint8_t modeToSet);
      uint8_t getParamMode(char param);
      uint8_t getParamSetting(char param, uint8_t key);
      uint8_t setParamOption(char param, uint8_t option, uint8_t value);
      uint8_t enOrDisableParam(char param, bool on);
};
