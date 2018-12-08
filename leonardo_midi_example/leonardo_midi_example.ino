#include "MIDIUSB.h"

void sendMIDI(byte header, byte byte1, byte byte2, byte byte3) {
  midiEventPacket_t midi = {header, byte1, byte2, byte3};
  MidiUSB.sendMIDI(midi);
  MidiUSB.flush();
}
void sendMIDIOn(byte channel, byte pitch, byte velocity) {
  sendMIDI(0x09, 0x90 | channel, pitch, velocity);
}
void sendMIDIOff(byte channel, byte pitch, byte velocity) {
  sendMIDI(0x08, 0x80 | channel, pitch, velocity);
}
void sendMIDIControl(byte channel, byte control, byte value) {
  sendMIDI(0x0B, 0xB0 | channel, control, value);
}

void getMIDI(midiEventPacket_t rx){
  byte comm = rx.header;
  byte sig = rx.byte1;
  byte note = rx.byte2;
  byte velo = rx.byte3;
  
  Serial.print(comm, HEX);
  Serial.print("\t");
  Serial.print(sig, HEX);
  Serial.print("\t");
  Serial.print(note, HEX);
  Serial.print("\t");
  Serial.println(velo, HEX);
  
  //sendMIDI(comm, sig, note, velo);
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0)
      getMIDI(rx);
  } while (rx.header != 0);
}