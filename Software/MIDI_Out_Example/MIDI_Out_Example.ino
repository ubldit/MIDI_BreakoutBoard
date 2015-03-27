#include <MIDI.h>
// A very simple MIDI Out demo by ubld.it
// Connect power from Arduino to MIDI Breakout
// Connect ground from Arduino to MIDI Breakout
// Connect rx from Arduino to MIDI Breakout
// Connect tx from Arduino to MIDI Breakout
//
// Simple tutorial for sending midi messages via MIDI Breakout
// Will send a few notes and blink the led each loop(), nothing fancy but demonstrates sending messages
// Can easily be adapted to using physical buttons or something like an accelerometer
//

#define LED 13   		    // LED pin on Arduino Uno

void setup()
{
   pinMode(LED, OUTPUT);
   MIDI.begin(1);
}

void loop()
{
        MIDI.sendNoteOn(52,127,1);  // Send a Note (pitch 52, velo 127 on channel 1)
        digitalWrite(LED,HIGH);     // LED on
        delay(1000);		    // Wait for a second
 
		digitalWrite(LED,LOW);      // LED off
        MIDI.sendNoteOn(40,127,1);  // Send a Note (pitch 40, velo 127 on channel 1)
        delay(100);                 // Wait

        MIDI.sendNoteOff(40,127,1); // Stop the note
        delay(100);                 // Wait

        MIDI.sendNoteOn(40,127,1);  // Send a Note (pitch 40, velo 127 on channel 1)
        delay(100);		    // Wait

        MIDI.sendNoteOff(40,127,1); // Stop the note
        MIDI.sendNoteOff(52,0,1);   // Stop the note
}
