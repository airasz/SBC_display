// display for all display
#include "EasyNextionLibrary.h"

// #include <WiFiClientSecure.h>

#include <Adafruit_NeoPixel.h>
#define NEOPIN 13
Adafruit_NeoPixel NEO = Adafruit_NeoPixel(1, NEOPIN, NEO_GRB + NEO_KHZ800);
bool dbg_result = false;
SoftwareSerial myserial(4, 5);
bool state0 = false, state1 = false, state2 = false, state3 = false;
EasyNex myNex(myserial);
String durls[9];
int steps[] = {0, 12, 20, 32, 36, 42, 54, 66, 78};
int step = 0;
// int page = 0;
bool bstates[9];
bool loadState = true, goSave = false, next = false;
int page = 2;
int vcd2 = 90, vcd3 = 90; // value of minute countdown
long prevmil = 0;
int stoo = 0;

bool cpage = false; // change page

String jsonData = " ";
String bk, bv, bh;
String stas[18][2];
String sstation;
bool dnssplit = false;

long prevmill = 0;
String oldsdata;
int tryrequest = 0;
bool foundRadio = false;
String data;
char c;
int toScreenSleep = 0;

long prevmill2 = 0;
int angka = 0;
bool blinking = true;
int nblinking = 6;
bool blinkstate = false;
int blinkval = 255;
int countblink = 16;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start test nextion");

  NEO.begin();
  NEO.show();
  // NEO.setPixelColor(0, 0);
  NEO.setPixelColor(0, NEO.Color(170, 0, 0));

  page = 0;

  myNex.begin(115200); // Begin the object with a baud rate of 9600
  myNex.writeStr("page page0");
  myNex.writeStr("dim=50");
  int count = 0;

} // setup
int count2 = 0;
int count3 = 0, checkpoint = 10;
bool blink = false;
void loop()
{
  // put your main code here, to run repeatedly:
  myNex.NextionListen();
  // if(blink)

  while (Serial.available() > 0)
  {
    delay(10);
    c = Serial.read();
    data += c;
  }
  if (data.length() > 0)
  {
    //    Serial.println(data);
    // tb_display_print_String(data.c_str(), 20);
    toScreenSleep = 0;
    if (data.startsWith("rotation"))
    {
      myNex.writeStr("t1.txt", data);
    }
    else if (data.startsWith("set volume"))
    {
      myNex.writeStr("t1.txt", data);
    }
    else if (data.startsWith("blink"))
    {
      nblinking = data.substring(5).toInt();
      blinking = true;
      angka = 7;
      Serial.println("startblinking");
    }
    else if (data.startsWith("play pos"))
    {
      myNex.writeStr("t1.txt", data);
    }
    else
    {
      data.replace("\n", "\\r");
      myNex.writeStr("t1.txt", data);
    }
    data = "";
  }
  if (millis() > prevmill + 1000)
  {
    toScreenSleep++;
    // if (toScreenSleep > 10)
    // {
    //   testdrawtext("waiting for incoming data", COLOR_MEDIUM[random(10)]);
    // }
    // else
    if (toScreenSleep > 20)
    {
      toScreenSleep = 20;
      myNex.writeStr("waiting for incoming data");
    }
    prevmill = millis();
  }
  if (millis() > prevmill2 + 100)
  {
    angka++;
    if (angka == 9)
    {
      if (blinking)
      {
        countblink++;
        analogWrite(16, 255);
        if (countblink > nblinking)
        {
          blinking = false;
          analogWrite(16, 0);
          countblink = 0;
        }
      }
    }
    if (angka > 9)
    {
      if (blinking)
        analogWrite(16, 0);
      angka = 0;
    }
    prevmill2 = millis();
  }
}