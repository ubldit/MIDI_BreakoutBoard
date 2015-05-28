// A very simple MIDI synth.
// originally by Greg Kennedy 2011 (http://forum.arduino.cc/index.php?topic=79326.0)
// modified to demo MIDI Breakout by ubld.it
// 
// Connect positive side of piezo speaker to digital pin 7 (or modify tonePin below)
// Connect negative side of piezo speaker to ground (this possibly works vice versa since the pwm goes high low)
// Connect power from Arduino to MIDI Breakout
// Connect ground from Arduino to MIDI Breakout
// Connect rx from Arduino to MIDI Breakout
// Connect tx from Arduino to MIDI Breakout
// Send data on MIDI channel 0
//
// This demo will play tones using the tone() function when MIDI notes are sent in
//
// **** Note that if you are using the RX and TX on the Arduino, if its an older arduino the RX and TX are shared with
// The programming FTDI chip, you may need to disconnect RX and TX while programming, then reconnect them

#include <avr/pgmspace.h>

#define statusLed 13
#define tonePin 7

// MIDI channel to answer to, 0x00 - 0x0F
#define myChannel 0x00
// set to TRUE and the device will respond to all channels
#define respondAllChannels false

// midi commands
#define MIDI_CMD_NOTE_OFF 0x80
#define MIDI_CMD_NOTE_ON 0x90
#define MIDI_CMD_KEY_PRESSURE 0xA0
#define MIDI_CMD_CONTROLLER_CHANGE 0xB0
#define MIDI_CMD_PROGRAM_CHANGE 0xC0
#define MIDI_CMD_CHANNEL_PRESSURE 0xD0
#define MIDI_CMD_PITCH_BEND 0xE0

// this is a placeholder: there are
//  in fact real midi commands from F0-FF which
//  are not channel specific.
// this simple synth will just ignore those though.
#define MIDI_CMD_SYSEX 0xF0

// a dummy "ignore" state for commands which
//  we wish to ignore.
#define MIDI_IGNORE 0x00

// midi "state" - which data byte we are receiving
#define MIDI_STATE_BYTE1 0x00
#define MIDI_STATE_BYTE2 0x01

// MIDI note to frequency
//  This isn't exact and may sound a bit detuned at lower notes, because
//  the floating point values have been rounded to uint16.
//  Based on A440 tuning.

// I would prefer to use the typedef for this (prog_uint16_t), but alas that triggers a gcc bug
// and does not put anything into the flash memory.

// Also note the limitations of tone() which at 16mhz specifies a minimum frequency of 31hz - in other words, notes below
// B0 will play at the wrong frequency since the timer can't run that slowly!
uint16_t frequency[128] PROGMEM = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 5920, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

//setup: declaring iputs and outputs and begin serial
void setup()
{
  pinMode(statusLed,OUTPUT);   // declare the LED's pin as output

  pinMode(tonePin,OUTPUT);           // setup tone output pin

  //start serial with midi baudrate 31250
  // or 38400 for debugging (eg MIDI over serial from PC)
  Serial.begin(31250);

  // indicate we are ready to receive data!
  digitalWrite(statusLed,HIGH);
  
  tone(tonePin,3000);
  delay(500);
  noTone(tonePin);
  
  digitalWrite(statusLed,LOW);
}

//loop: wait for serial data
void loop()
{
  static byte note;
  static byte lastCommand = MIDI_IGNORE;
  static byte state;
  static byte lastByte;

  while(Serial.available())
  {
    // read the incoming byte:
    byte incomingByte = Serial.read();

    // Command byte?
    if (incomingByte & 0b10000000)
    {
      if (respondAllChannels ||
             (incomingByte & 0x0F) == myChannel) { // See if this is our channel
        lastCommand = incomingByte & 0xF0;
      }
      else
      { // Not our channel.  Ignore command.
        lastCommand = MIDI_IGNORE;
      }
      state = MIDI_STATE_BYTE1; // Reset our state to byte1.
    }
    else if(state == MIDI_STATE_BYTE1)
    { // process first data byte
      if(lastCommand==MIDI_CMD_NOTE_OFF)
      { // if we received a "note off", make sure that is what is currently playing
        if(note == incomingByte)
        {
          noTone(tonePin);
        }
        state = MIDI_STATE_BYTE2; // expect to receive a velocity byte
      }
      else if(lastCommand == MIDI_CMD_NOTE_ON)
      { // if we received a "note on", we wait for the note (databyte)
        lastByte=incomingByte;    // save the current note
        state = MIDI_STATE_BYTE2; // expect to receive a velocity byte
      }
      // implement whatever further commands you want here
    }
    else
    { // process second data byte
      if(lastCommand == MIDI_CMD_NOTE_ON)
      {
        if(incomingByte != 0)
        {
          note = lastByte;
          tone(tonePin,(unsigned int)pgm_read_word(&frequency[note]));
          digitalWrite(statusLed,HIGH);
        }
        else if(note == lastByte)
        {
          noTone(tonePin);
          digitalWrite(statusLed,LOW);
        }
      }
      state = MIDI_STATE_BYTE1; // message data complete
                                 // This should be changed for SysEx
    }
  }
}
