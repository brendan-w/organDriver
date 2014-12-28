#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//that old 74HC595N shift register
#define latchPin 12  //pin 12 of chip
#define clockPin 11  //pin 11 of chip
#define dataPin 10  //pin 14 of chip
#define numChips 2  // number of shift registers
#define noteOffset 24  //midi C1 value, start of rank


#define DIP_LSB_PIN 9
#define DIP_MSB_PIN 6


byte chestChannel = 0;  // channel
byte noteStates[numChips];  //number of shift registers

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  

  //set the pinMode for the DIP switch
  pinModeRange(DIP_LSB_PIN, DIP_MSB_PIN, INPUT_PULLUP);

  //get the MIDI channel number from the DIP switch
  chestChannel = readDIP(DIP_LSB_PIN, DIP_MSB_PIN, true);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(chestChannel);

}

//sets a range of pins to a given mode
void pinModeRange(uint8_t a, uint8_t b, int mode)
{
  //handle forwards and backwards ranges
  uint8_t start = (a < b) ? a : b;
  uint8_t stop  = (a < b) ? b : a;

  for(uint8_t pin = start; pin <= stop; a++)
      pinMode(pin, mode);
}


uint8_t readDIP(uint8_t lsb_pin, uint8_t msb_pin, bool invert)
{
  uint8_t value = 0;

  //handle big and little endian pin ranges
  if(lsb_pin < msb_pin)
  {
    for(uint8_t pin = lsb_pin; pin <= msb_pin; pin++)
      bitWrite(value, lsb_pin + pin, digitalRead(pin) ^ invert);
  }
  else if(msb_pin < lsb_pin)
  {
    for(uint8_t pin = lsb_pin; pin >= msb_pin; pin--)
      bitWrite(value, lsb_pin - pin, digitalRead(pin) ^ invert);
  }

  return value;
}

void loop()
{

  MIDI.read();
  shiftShit();

} 

void shiftShit()
{
  digitalWrite(latchPin, LOW);
  for(int i = 0; i < numChips; i++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, noteStates[i]);  
  }
  digitalWrite(latchPin, HIGH);
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  updateNoteStatus(pitch, true);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  updateNoteStatus(pitch, false);
}

void updateNoteStatus(byte pitch, boolean state)
{
  pitch -= noteOffset;
  pitch = (pitch < 0) ? 0 : pitch;
  int chipNum = (pitch/8);
  int pinNum = pitch % 8;
  bitWrite(noteStates[chipNum], pinNum, state);

}







