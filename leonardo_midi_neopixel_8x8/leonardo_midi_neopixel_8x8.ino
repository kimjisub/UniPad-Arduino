#include <Adafruit_NeoPixel.h>
#include "MIDIUSB.h"

#define PIN            10
#define NUMPIXELS      64

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const unsigned long colors[] = {
  0x000000,0xfafafa,0xfafafa,0xfafafa,0xf8bbd0,0xef5350,0xe57373,0xef9a9a,
  0xfff3e0,0xffa726,0xffb960,0xffcc80,0xffe0b2,0xffee58,0xfff59d,0xfff9c4,
  0xdcedc8,0x8bc34a,0xaed581,0xbfdf9f,0x5ee2b0,0x00ce3c,0x00ba43,0x119c3f,
  0x57ecc1,0x00e864,0x00e05c,0x00d545,0x7afddd,0x00e4c5,0x00e0b2,0x01eec6,
  0x49efef,0x00e7d8,0x00e5d1,0x01efde,0x6addff,0x00dafe,0x01d6ff,0x08acdc,
  0x73cefe,0x0d9bf7,0x148de4,0x2a77c9,0x8693ff,0x2196f3,0x4668f6,0x4153dc,
  0xb095ff,0x8453fd,0x634acd,0x5749c5,0xffb7ff,0xe863fb,0xd655ed,0xd14fe9,
  0xfc99e3,0xe736c2,0xe52fbe,0xe334b6,0xed353e,0xffa726,0xf4df0b,0x8bc34a,
  0x5cd100,0x00d29e,0x2388ff,0x3669fd,0x00b4d0,0x475cdc,0xfafafa,0xfafafa,
  0xf72737,0xd2ea7b,0xc8df10,0x7fe422,0x00c931,0x00d7a6,0x00d8fc,0x0b9bfc,
  0x585cf5,0xac59f0,0xd980dc,0xb8814a,0xff9800,0xabdf22,0x9ee154,0x66bb6a,
  0x3bda47,0x6fdeb9,0x27dbda,0x9cc8fd,0x79b8f7,0xafafef,0xd580eb,0xf74fca,
  0xea8a1f,0xdbdb08,0x9cd60d,0xf3d335,0xc8af41,0x00ca69,0x24d2b0,0x757ebe,
  0x5388db,0xe5c5a6,0xe93b3b,0xf9a2a1,0xed9c65,0xe1ca72,0xb8da78,0x98d52c,
  0x626cbd,0xcac8a0,0x90d4c2,0xceddfe,0xbeccf7,0xfafafa,0xfafafa,0xfafafa,
  0xfe1624,0xcd2724,0x9ccc65,0x009c1b,0xffff00,0xbeb212,0xf5d01d,0xe37829
};

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
  
  /*Serial.print(comm);//, HEX);
  Serial.print("\t");
  Serial.print(sig);//, HEX);
  Serial.print("\t");
  Serial.print(note);//, HEX);
  Serial.print("\t");
  Serial.println(velo);//, HEX);*/
  
  if(comm == 4 || comm == 7)
    mode = true;
  
  if(!mode) {
    if (comm == 9) {
      int x = 9 - (note / 10);
			int y = note % 10;
			
			if (y >= 1 && y <= 8)
				pad(x - 1, y - 1, velo);
    } else if (comm == 11) {
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
    if(comm == 9 || comm == 8){
      velo = comm==8 ? 0:velo;
      if(36<=note && note<=67)
        pad((67-note)/4, 4-(67-note)%4-1, velo);
      else if(68<=note && note<=99)
        pad((99-note)/4, 8-(99-note)%4-1, velo);
      else if(100<=note && note<=107)
        chain(note-100+8, velo);
      else if(116<=note && note<=123)
        chain(7-(note-116)+16, velo);
      else if(108<=note && note<=115)
        chain(7-(note-108)+24, velo);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pixels.begin();
  pixels.setBrightness(3);//0~255
  pixels.show();
  delay(3000);
  
  do {
    for(int i=0;i<127;i++){
      led(i%NUMPIXELS, i);
      delay(10);
    }
    for(int i=0;i<NUMPIXELS;i++){
      led(i, 0);
      delay(10);
    }
  } while(0);
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0)
      getMIDI(rx);
  } while (rx.header != 0);
  pixels.show();
}

void pad(int x, int y, int velo){
  if(0<=x&&x<=7 && 0<=y&&y<=7)
    led(x*8 + y, velo);
}
void chain(int x, int velo){
}
void led(int i, int velo){
  pixels.setPixelColor(i, colors[velo]);
}
