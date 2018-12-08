#include "MIDIUSB.h"

int pins[17]= {-1, A0, A1, A2, A3, 2, 3, 4, 5, 13,12, 11, 10, 9, 8, 7, 6};
int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[06],pins[11], pins[15], pins[16]};
int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1],pins[7], pins[2], pins[5]};

byte col = 0;
byte leds[8][8];
boolean mode = false;

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
  
  if(comm == 4 || comm == 7)
    mode = true;
  
  if(!mode) {
    if(comm == 9) {
      int x = 9 - (note / 10);
			int y = note % 10;
			
			if (y >= 1 && y <= 8)
				pad(x - 1, y - 1, velo);
    } else if(comm == 11 && sig == -80) {
      if (91 <= note && note <= 98)
				chain(note - 91, velo);
			if (19 <= note && note <= 89 && note % 10 == 9)
				chain(9 - (note / 10) - 1 + 8, velo);
			if (1 <= note && note <= 8)
				chain(8 - note + 16, velo);
			if (10 <= note && note <= 80 && note % 10 == 0)
				chain((note / 10) - 1 + 24, velo);
    }
  } else {
    
  }
}

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 8; i++){
    pinMode(rows[i], OUTPUT);
    pinMode(cols[i], OUTPUT); 
    digitalWrite(cols[i],HIGH);
  }
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      getMIDI(rx);
      display();
    }
  } while (rx.header != 0);
  display();
}

void display() {
  digitalWrite(cols[col], LOW);
  col++;
  if (col == 8)
    col = 0;
 
  for (int row = 0; row < 8; row++) {
    if (leds[col][7 - row] == 1)
      digitalWrite(rows[row], LOW);
    else 
      digitalWrite(rows[row], HIGH);
  }
  digitalWrite(cols[col], HIGH);
}

void pad(int x, int y, int velo){
  if(0<=x&&x<=7 && 0<=y&&y<=7){
    if(velo != 0)
      leds[x][y] = 1;
    else
      leds[x][y] = 0;
  }
}

void chain(int x, int velo){
}