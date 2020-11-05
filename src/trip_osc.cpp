#include "OSCMessage.h"
#include "OSCBundle.h"
#include "trip_osc.h"
#include "SLIPEncodedUSBSerial.h"
#include "SLIPEncodedSerial.h"
#include "WProgram.h"
#include "settings.h"
#include "menu.h"
#include "trip_adc.h"


#define OSCPARAMINDEX 8
#define OSCMAXADDR 16

//this is the current system settings object
extern tpxSettings * Settings;



SLIPEncodedUSBSerial SLIPSerial( Serial ); //global SLIP object


static OSCBundle globalBundle;


void oscadd1(char * target, int value){
   globalBundle.add(target).add((int32_t)value);
}

void oscsend(){
    SLIPSerial.beginPacket();
    globalBundle.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket();        // mark the end of the OSC Packet
    globalBundle.empty();          // empty the bundle to free room for a new one
}

void oscinit() {
  SLIPSerial.begin(9600);   // set this as high as you can reliably run on your platform
}


void oscsend1(char * target, int value){
  //the message wants an OSC address as first argument
  OSCMessage msg(target);
  msg.add(value);
  SLIPSerial.beginPacket();  
    msg.send(SLIPSerial); // send the bytes to the SLIP stream
  SLIPSerial.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
}


void oscsend3(unsigned short x, unsigned short y, unsigned short t){
    OSCBundle myBundle;
    //OSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
    myBundle.add("/teensy/x").add((int32_t)x);
    myBundle.add("/teensy/y").add((int32_t)y);
    myBundle.add("/teensy/t").add((int32_t)t);

    SLIPSerial.beginPacket();
        myBundle.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    myBundle.empty(); // empty the bundle to free room for a new one
}


void oscsend4(unsigned short ul, unsigned short ur, unsigned short ll, unsigned short lr){
    OSCBundle myBundle;
    //BOSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
    myBundle.add("/teensy/ul").add((int32_t)ul);
    myBundle.add("/teensy/ur").add((int32_t)ur);
    myBundle.add("/teensy/ll").add((int32_t)ll);
    myBundle.add("/teensy/lr").add((int32_t)lr);

    SLIPSerial.beginPacket();
        myBundle.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    myBundle.empty(); // empty the bundle to free room for a new one
};

//Functions for receive OSC, and doing stuff with it

//TODO once program change is implemented, add the function to do that here
void osc_pgm(OSCMessage &msg){
   int pgm;
   pgm=msg.getInt(0);
   //Serial.print("got pgm: ");
   //Serial.println(pgm);
}


void strcopy(char * dest, char * source, int n){
   int i = 0;
  while(i<n-1 && source[i]){
   dest[i]=source[i];
   i++;
   //Serial.println("copying a string\r");
   }
   dest[i] = 0;
}

//TODO implement send
void osc_send(OSCMessage &msg){
   uint8_t setting_ret;
   char parambuf[2];
   char param, paramCAPS;
   char target[OSCMAXADDR];
   msg.getString(0, (char*)parambuf, 2);
   //Serial.print("got send: ");
   //Serial.println(parambuf[0]);
   param = parambuf[0];
   paramCAPS=param+('A'-'a');
   //send cc, ch, inv, mode, en
   setting_ret=Settings->getParamSetting(paramCAPS, MIDICC);
   strcopy(target, "/teensy/pcc", OSCMAXADDR);
   target[OSCPARAMINDEX] = param;
   oscsend1((char *)target, setting_ret);
   //ch
   setting_ret=Settings->getParamSetting(paramCAPS, MIDICHNL);
   strcopy(target, "/teensy/pch", OSCMAXADDR);
   target[OSCPARAMINDEX] = param;
   oscsend1((char *)target, setting_ret);
   //inv
   setting_ret=Settings->getParamSetting(paramCAPS, INV);
   strcopy(target, "/teensy/pinv", OSCMAXADDR);
   target[OSCPARAMINDEX] = param;
   oscsend1((char *)target, setting_ret);
   //mode
   setting_ret=Settings->getParamMode(paramCAPS);
   strcopy(target, "/teensy/pmode", OSCMAXADDR);
   target[OSCPARAMINDEX] = param;
   oscsend1((char *)target, setting_ret);
   //enable
   setting_ret=(uint8_t)Settings->isParamEnabled(paramCAPS);
   strcopy(target, "/teensy/pen", OSCMAXADDR);
   target[OSCPARAMINDEX] = param;
   oscsend1((char *)target, setting_ret);
}

void osc_cc(OSCMessage &msg){
   int cc;
   char param;
   char buffer[OSCMAXADDR];
   cc=msg.getInt(0);
   //Serial.print("got cc: ");
   //Serial.println(cc);
   if(msg.getAddress((char*)buffer, 0)  >= 9){
     param = buffer[OSCPARAMINDEX];
   }
   param+=('A'-'a');
   //Serial.print("param = ");
   //Serial.println(param);
   Settings->setParamOption(param, MIDICC, cc);
}

void osc_ch(OSCMessage &msg){
   int ch;
   char param;
   char buffer[OSCMAXADDR];
   ch=msg.getInt(0);
   //Serial.print("got ch: ");
   //Serial.println(ch);
   if(msg.getAddress((char*)buffer, 0)  >= 9){
     param = buffer[OSCPARAMINDEX];
   }
   param+=('A'-'a');
   Settings->setParamOption(param, MIDICHNL, ch);
}

void osc_inv(OSCMessage &msg){
   int inv;
   char param;
   char buffer[OSCMAXADDR];
   inv=msg.getInt(0);
   //Serial.print("got inv: ");
   //Serial.println(inv);
   if(msg.getAddress((char*)buffer, 0)  >= 9){
     param = buffer[OSCPARAMINDEX];
   } else return;
   param+=('A'-'a');
   Settings->setParamOption(param, INV, inv);
}

void osc_mode(OSCMessage &msg){
   int mode;
   char param;
   char buffer[OSCMAXADDR];
   mode=msg.getInt(0);
   //Serial.print("got mode: ");
   //Serial.println(mode);
   if(msg.getAddress((char*)buffer, 0)  >= 9){
     param = buffer[OSCPARAMINDEX];
   } else 
     return;
   param+=('A'-'a');
   Settings->setParamMode(param, mode);  //mode 0 = usb midi, mode 1 = midi, mode 2 = OSC
}

void osc_en(OSCMessage &msg){
   int en;
   char param;
   char buffer[OSCMAXADDR];
   en=msg.getInt(0);
   //Serial.print("got en: ");
   //Serial.println(en);
   if(msg.getAddress((char*)buffer, 0)  >= 9){
     param = buffer[OSCPARAMINDEX];
   } else return;
   param+=('A'-'a');
   Settings->enOrDisableParam(param, (bool)en);
}


void osc_save(OSCMessage &msg){
   bool save;
   save=(bool)msg.getInt(0);
   if(save){
      savePreset();//TODO this function will require user to use the buttons to choose where to save it. Could change that.
   }
}

void oscreceive(){
   static OSCMessage inMessage;
   int size;
   disableInterrupts();
   if(!SLIPSerial.endofPacket()){  	//replaced this while loop with an if. This block will run repeatedly from main, 
 					//and now it won't block while waiting for the rest of an OSC packet
      if( (size = SLIPSerial.available() ) > 0)
      {
        while(size--){
          inMessage.fill(SLIPSerial.read());
        }
      }
      enableInterrupts();
      return;
  }
  else if(inMessage.hasError()){
  inMessage.empty();
  enableInterrupts();
  return;
  }
  else 
  {
   if(inMessage.dispatch("/teensy/pgm", osc_pgm)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/send", osc_send)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/xcc", osc_cc)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/ycc", osc_cc)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/tcc", osc_cc)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/xch", osc_ch)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/ych", osc_ch)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/tch", osc_ch)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/xinv", osc_inv)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/yinv", osc_inv)){ 
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/tinv", osc_inv)){
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/xmode", osc_mode)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/ymode", osc_mode)){ 
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/tmode", osc_mode)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/xen", osc_en)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/yen", osc_en)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/ten", osc_en)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else if(inMessage.dispatch("/teensy/save", osc_save)){
      inMessage.empty();
      enableInterrupts();
      return;
      }
   else{
      inMessage.empty();
      //Serial.println("got an unkown OSC input");
      enableInterrupts();
      }
  }
}

void checkosc(){
   oscreceive();
}
