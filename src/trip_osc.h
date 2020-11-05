#include <OSCMessage.h>
#include <SLIPEncodedUSBSerial.h>
#include <SLIPEncodedUSBSerial.h>

/*
Make an OSC message and send it over serial
 */



void oscinit();
void oscadd1(char * target, int value);
void oscsend();
void oscsend1(char * target, int value);
void oscsend3(unsigned short x, unsigned short y, unsigned short t);
void oscsend4(unsigned short ul, unsigned short ur, unsigned short ll, unsigned short lr);
void checkosc();
