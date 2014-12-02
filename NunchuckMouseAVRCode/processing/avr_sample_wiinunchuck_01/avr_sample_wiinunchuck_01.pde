/*
test sketch for wiinunchuck
version 0.1

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

import processing.opengl.*;
import processing.serial.*;

//crc8 lookup for poly X^8 + X^7 + X^2 + 1
int[] crc8table =
{
  0x00,0x85,0x8F,0x0A,0x9B,0x1E,0x14,0x91,0xB3,0x36,
  0x3C,0xB9,0x28,0xAD,0xA7,0x22,0xE3,0x66,0x6C,0xE9,
  0x78,0xFD,0xF7,0x72,0x50,0xD5,0xDF,0x5A,0xCB,0x4E,
  0x44,0xC1,0x43,0xC6,0xCC,0x49,0xD8,0x5D,0x57,0xD2,
  0xF0,0x75,0x7F,0xFA,0x6B,0xEE,0xE4,0x61,0xA0,0x25,
  0x2F,0xAA,0x3B,0xBE,0xB4,0x31,0x13,0x96,0x9C,0x19,
  0x88,0x0D,0x07,0x82,0x86,0x03,0x09,0x8C,0x1D,0x98,
  0x92,0x17,0x35,0xB0,0xBA,0x3F,0xAE,0x2B,0x21,0xA4,
  0x65,0xE0,0xEA,0x6F,0xFE,0x7B,0x71,0xF4,0xD6,0x53,
  0x59,0xDC,0x4D,0xC8,0xC2,0x47,0xC5,0x40,0x4A,0xCF,
  0x5E,0xDB,0xD1,0x54,0x76,0xF3,0xF9,0x7C,0xED,0x68,
  0x62,0xE7,0x26,0xA3,0xA9,0x2C,0xBD,0x38,0x32,0xB7,
  0x95,0x10,0x1A,0x9F,0x0E,0x8B,0x81,0x04,0x89,0x0C,
  0x06,0x83,0x12,0x97,0x9D,0x18,0x3A,0xBF,0xB5,0x30,
  0xA1,0x24,0x2E,0xAB,0x6A,0xEF,0xE5,0x60,0xF1,0x74,
  0x7E,0xFB,0xD9,0x5C,0x56,0xD3,0x42,0xC7,0xCD,0x48,
  0xCA,0x4F,0x45,0xC0,0x51,0xD4,0xDE,0x5B,0x79,0xFC,
  0xF6,0x73,0xE2,0x67,0x6D,0xE8,0x29,0xAC,0xA6,0x23,
  0xB2,0x37,0x3D,0xB8,0x9A,0x1F,0x15,0x90,0x01,0x84,
  0x8E,0x0B,0x0F,0x8A,0x80,0x05,0x94,0x11,0x1B,0x9E,
  0xBC,0x39,0x33,0xB6,0x27,0xA2,0xA8,0x2D,0xEC,0x69,
  0x63,0xE6,0x77,0xF2,0xF8,0x7D,0x5F,0xDA,0xD0,0x55,
  0xC4,0x41,0x4B,0xCE,0x4C,0xC9,0xC3,0x46,0xD7,0x52,
  0x58,0xDD,0xFF,0x7A,0x70,0xF5,0x64,0xE1,0xEB,0x6E,
  0xAF,0x2A,0x20,0xA5,0x34,0xB1,0xBB,0x3E,0x1C,0x99,
  0x93,0x16,0x87,0x02,0x08,0x8D
};

int[] buttons = {0,0};
int[] joys = {0,0};
short[] accels = {0,0,0};

float pitch = 0;
float roll = 0;
float yaw = 0;

PVector accelsv;

void setup() {
  size(400, 450, P3D);
  //println(Serial.list());
  new Serial(this, "COM33",  9600); 
  noLoop();
  PFont font = loadFont("ArialUnicodeMS-48.vlw"); 
  textFont(font, 14);
}

void draw() {
  int transX = 0;
  int transY = 0;
  
  background(#000000);
  fill(#ffffff);
  
  //accellerometer normalized vector
  accelsv = new PVector((float)accels[0],(float)accels[1],(float)accels[2]);
  accelsv.normalize();
  
  //print out data
  text("Button:\n  Z: " + buttons[0] + "\n  C: " + buttons[1], 10, 20);
  text("Joy:\n  X: " + joys[0] + "\n  Y: " + joys[1], 70, 20); 
  text("Accell:\n  X: " + accels[0] + "\n  Y: " + accels[1] + "\n  Z: " + accels[2], 130, 20);
  text("Accell Norm (g):\n  X: " + accelsv.x + "\n  Y: " + accelsv.y + "\n  Z: " + accelsv.z, 210, 20);
  
  //estimation of pitch and roll
  roll = atan2(accelsv.y, accelsv.z);
  pitch = atan2(accelsv.x, sqrt(pow(accelsv.y,2) + pow(accelsv.z, 2)));
  
  /*float croll = atan(accelsv.y/accelsv.z);
  float cpitch = atan(accelsv.x/sqrt(pow(accelsv.y,2) + pow(accelsv.z, 2)));
  if(croll > -radians(80) && croll < radians(80)) {
    roll = croll;
    pitch = cpitch;
  }*/
  
  //draw rotating cube
  transX = 270;
  transY = 310;
  translate(transX, transY);
  pushMatrix();
  draw_rotocube();
  popMatrix();
  translate(-transX, -transY);
  
  //draw pitch and roll
  transX = 190;
  transY = 145;
  translate(transX, transY);
  pushMatrix();
  draw_pitchroll();
  popMatrix();
  translate(-transX, -transY);
  
  //draw buttons
  transX = 50;
  transY = 120;
  translate(transX, transY);
  pushMatrix();
  draw_buttons();
  popMatrix();
  translate(-transX, -transY);
  
  //draw joys
  transX = 80;
  transY = 240;
  translate(transX, transY);
  pushMatrix();
  draw_joys();
  popMatrix();
  translate(-transX, -transY);
}

/*
draw joyspad
*/
void draw_joys() {  
  strokeWeight(2);
  stroke(#64CC70);
  
  fill(#000000);
  rect(-50, -50, 100, 100);
    
  stroke(0, 0, 255);
  fill(#000000, 220);
  rect((joys[0]*100/255)-10, -(joys[1]*100/255)-10, 20, 20);
  
  fill(#ffffff);
  text("Joypad", -50, 70);
}

/*
draw buttons
*/
void draw_buttons() {
  //button C  
  strokeWeight(2);
  stroke(#64CC70);
  if(buttons[1] == 1)
    fill(0, 255, 0);
  else
    fill(#000000);
  ellipse(0, 0, 50, 50);  
  fill(#ffffff);
  text("C", -4, 40);
  
  //button Z
  strokeWeight(2);
  stroke(#64CC70);
  if(buttons[0] == 1)
    fill(255, 0, 0);
  else
    fill(#000000);
  ellipse(75, 0, 50, 50);
  fill(#ffffff);
  text("Z", -4+75, 40);
}

/*
draw a rotating cube
*/
void draw_rotocube() {
  rotateZ(roll);
  rotateX(pitch);
  rotateY(yaw);
  
  //axis
  fill(#000000);
  strokeWeight(2);
  stroke(255, 0, 0);
  line(-100, 0, 0, 100, 0, 0);
  translate(+100, 0, 0);
  box(10,10,10);
  translate(-100, 0, 0);
  stroke(0, 255, 0);
  line(0, -100, 0, 0, 100, 0);
  translate(0, -100, 0);
  box(10,10,10);
  translate(0, +100, 0);
  stroke(0, 0, 255);
  line(0, 0, -100, 0, 0, 100);
  translate(0, 0, +100);
  box(10,10,10);
  translate(0, 0, -100);
  
  //box
  strokeWeight(2);
  stroke(#64CC70);
  fill(#000000, 220);
  box(100,100,100);
}

/*
draw pitch and roll indicators
*/
void draw_pitchroll() {
  //pitch circle
  pushMatrix();
  strokeWeight(2);
  stroke(#64CC70);
  fill(#000000);
  ellipse(25, -25, 50, 50);
  fill(255);
  text("pitch:\n    " + degrees(pitch), 0, 20);
  translate(25, -25);
  strokeWeight(2);
  rotateZ(pitch);
  stroke(0, 255, 0);
  line(-25, 0, 0, 25, 0, 0);
  popMatrix();
  
  translate(100, 0);
  //roll circle
  pushMatrix();
  strokeWeight(2);
  stroke(#64CC70);
  fill(#000000);
  ellipse(25, -25, 50, 50);
  fill(255);
  text("roll:\n    " + degrees(roll), 0, 20);
  translate(25, -25);
  strokeWeight(2);
  rotateZ(roll);
  stroke(255, 0, 0);
  line(-25, 0, 0, 25, 0, 0);
  popMatrix();
}

/*
get serial event
*/
void serialEvent(Serial myPort) {
  //read from serial data, check crc and fill variables
  byte[] buf = new byte[128];
  int bufread = myPort.readBytesUntil((byte)10, buf); 
  if(bufread == 14) {
      int crcr = 0;
      int crcc = 0;
      for(int i=0; i<12; i++)
        crcc = crc8table[crcc ^ (buf[i] & 0xff)];
      crcr = buf[12] & 0xff;
      if(crcr == crcc) {
        buttons[0] = buf[0];
        buttons[1] = buf[1];
        joys[0] = (short)((buf[3] & 0xff)<<8 | (buf[2] & 0xff));
        joys[1] = (short)((buf[5] & 0xff)<<8 | (buf[4] & 0xff));
        accels[1] = (short)((buf[7] & 0xff)<<8 | (buf[6] & 0xff));
        accels[0] = (short)((buf[9] & 0xff)<<8 | (buf[8] & 0xff));
        accels[2] = (short)((buf[11] & 0xff)<<8 | (buf[10] & 0xff));
      }
  }
  redraw();
}

