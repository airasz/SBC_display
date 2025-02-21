// board: esp32
//  methode: usb or software serial

#include <SPI.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include "tb_display.h"
#include <SoftwareSerial.h>

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS 33
#define TFT_RST 14 // you can also connect this to the Arduino reset
// in which case, set this #define pin to -1!
#define TFT_DC 26

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)

// For 1.44" and 1.8" TFT with ST7735 use

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h
// For 1.54" TFT with ST7789
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
// #define TFT_SCLK 13   // set these to be whatever pins you like!
// #define TFT_MOSI 11   // set these to be whatever pins you like!
// Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#include "res.h"
float p = 3.1415926;

String rssmsg[300];
String siteonread;

int httpGetChar();
const uint32_t COLOR_MEDIUM[] = {TFT_WHITE, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_GREENYELLOW, TFT_PINK, TFT_ORANGE, TFT_RED, TFT_CYAN, TFT_MAGENTA, TFT_PINK, TFT_SKYBLUE};

const uint32_t COLORS_LIGHT[10] = {
    0xDB5B, 0x97E9, 0x8C7F, 0xFACC, 0xFFED,
    0x4F1F, 0x9ADF, 0xFD0B, 0x5DDF, 0xF9B1};
const uint32_t COLORS_DARK[10] = {
    0x2004, 0x0920, 0x0808, 0x4005, 0x0900,
    0x00E4, 0x280D, 0x20C0, 0x0006, 0x3000};
String nsb15 = "NotoSansBold15";
String sui14 = "SegoeUI-14";
String sfpt_r14 = "SFProText-Regular-14";
String sfpt_r16 = "SFProText-Regular-16";
String sfpt_r18 = "SFProText-Regular-18";
String sfpd_r20 = "SFProDisplay-Regular-20";
String sfpd_r24 = "SFProDisplay-Regular-24";
String sfpd_r28 = "SFProDisplay-Regular-28";
const struct site_t
{
  char *title;
  char *url;
  char *contentsToDisplay;
} sites[] = {
    {"bananab", "http://192.168.10.232/radio/oradio.php?cmd=status", "title"},
    {"bananay", "http://192.168.1.152/radio/oradio.php?cmd=status", "title"},
    {"orangeb", "http://192.168.10.200/radio/oradio.php?cmd=status", "title"},
    {"orangey", "http://192.168.1.120/radio/oradio.php?cmd=status", "title"}};
char *url = "http://192.168.10.232/radio/oradio.php?cmd=status";
String sdata;

SoftwareSerial serial(21, 19);

long prevmill2 = 0;
int angka = 0;
bool blinking = true;
int nblinking = 6;
bool blinkstate = false;
int blinkval = 255;
int countblink = 16;
#define usbbaud 115200
int dmode = 0;
void setup(void)
{
  Serial.begin(115200);
  serial.begin(9600);
  Serial.print("Hello! ST77xx TFT Test");
  pinMode(25, OUTPUT);
  pinMode(16, OUTPUT);
  tb_display_init(1);
  tft.init();
  tft.setRotation(1);
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
  digitalWrite(25, HIGH);
  // Use this initializer (uncomment) if you're using a 1.44" TFT
  // tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 0.96" 180x60 TFT
  // tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display

  // Use this initializer (uncomment) if you're using a 1.54" 240x240 TFT
  // tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

  tft.loadFont(sfpt_r14);
  Serial.println("Initialized");

  uint16_t time = millis();
  tft.fillScreen(TFT_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(TFT_BLACK);
  // testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", TFT_WHITE);
  testdrawtext("USB serial screen fo pi-radio, baud = 115200\nWaiting for incoming signal...", TFT_WHITE);
  //  testdrawtext(usbbaud, TFT_WHITE);
  //  testdrawtext("USB serial screen fo pi-radio, baud = 115200\nWaiting for incoming signal...", TFT_WHITE);
  delay(1000);

  digitalWrite(25, LOW);
  tft.fillScreen(TFT_BLACK);
  testdrawtext("waiting for incoming data", COLOR_MEDIUM[random(10)]);
}
long prevmill = 0;
String oldsdata;
int tryrequest = 0;
bool foundRadio = false;
String data;
char c;
int toScreenSleep = 0;
int maxWait = 20;
String olddata = "";
void loop()
{

  while (serial.available() > 0)
  {
    delay(10);
    c = serial.read();
    data += c;
  }
  while (Serial.available() > 0)
  {
    delay(10);
    c = Serial.read();
    data += c;
  }
  if (data.length() > 0)
  {
    proccesData(data);
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
    // Serial.printf("maxwait = %d", maxWait);
    if (toScreenSleep > maxWait)
    {
      toScreenSleep = 0;
      // testdrawtext("waiting for incoming data", COLOR_MEDIUM[random(12)]);
      // printWordWrap("waiting for incoming data", COLOR_MEDIUM[random(12)]);
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
} // end loop
int displaylivescore = 0;
void proccesData(String data)
{

  if (data.length() > 4)
  {
    //    Serial.println(data);
    // tb_display_print_String(data.c_str(), 20);
    toScreenSleep = 0;
    // if (data.length() > 10)
    //   maxWait = data.length() / 10
    // else
    maxWait = (data.length() > 18) ? data.length() / 6 : 40;
    // tft.printf("maxwait = %d\n", maxWait);
    if (data.startsWith("rotation"))
    {
      int sr = data.substring(9).toInt();
      tft.setRotation(sr);
      // testdrawtext("rotated", COLOR_MEDIUM[random(12)]);
      printWordWrap("rotated", COLOR_MEDIUM[random(12)]);
    }
    else if (data.startsWith("set volume"))
    {
      printtextbig(data, COLOR_MEDIUM[random(12)]);
    }
    else if (data.startsWith("resetscreen"))
    {
      tft.fillScreen(TFT_BLACK);
      data = "";
      return;
    }
    else if (data.startsWith("testnum"))
    {

      int sr = data.substring(7).toInt();
      tft.fillScreen(TFT_BLACK);
      testDTSegment(sr);
      data = "";
      return;
    }

    else if (data.startsWith("blink"))
    {
      nblinking = data.substring(5).toInt();
      blinking = true;
      angka = 7;
      Serial.println("startblinking");
      data = "";
      return;
    }
    else if (data.startsWith("dmode"))
    {
      if (data == "dmode")
      {
        tft.fillScreen(TFT_BLACK);
        tft.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
        return;
      }
      else
      {
        int dmod = data.substring(5).toInt();
        if (dmod < 3)
          dmode = dmod;
        // Serial.println("startblinking");
        tft.setCursor(0, 0);
        // tft.setTextSize(2);
        Serial.printf("change display mode to : %d \n", dmode);
        tft.fillScreen(TFT_BLACK);
        tft.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
        // tft.setTextSize(1);
        data = "";
        return;
      }
    }
    else if (data.startsWith("play pos"))
    {
      printtextbig(data, COLOR_MEDIUM[random(12)]);
    }
    String homescore = data.substring(data.indexOf(">") + 2);
    Serial.printf("dmode=%d\n", dmode);
    if (data.length() > 4)
      if (dmode == 0)
      {
        // displayscore(homescore);
        // drawSegment(10, 20, 0, homescore);

        if (data != olddata)
        {
          olddata = data;
          displaylivescore = random(4);
          tft.fillScreen(TFT_BLACK);
        }
        maxWait = (data.length() > 10) ? data.length() / 2 : 80;
        if (displaylivescore == 0)
          ssgmnt(homescore);
        else if (displaylivescore == 1)
          displayscore(homescore);
        else if (displaylivescore == 2)
          tsgmnt(homescore);
        else if (displaylivescore == 3)
          drawDigitLivescore(homescore);

        // ssgmnt(homescore);

        // drawDigitLivescore(homescore);
        // tsgmnt(homescore);
      }
      else if (dmode == 1)
      {
        // data = data + "%";
        tb_display_print_String(data.c_str(), 20);
      }
      else if (dmode == 2)
      {
        // displayinfo(data);
        printWordWrap(data, COLOR_MEDIUM[random(12)]);
      }
      else
      {
        // testdrawtext(data, COLOR_MEDIUM[random(10)]);
        printWordWrap(data, COLOR_MEDIUM[random(12)]);
      }

    data = "";
  }
}

int cx = 0, cy = 15;
void testdrawtext(char *text, uint16_t color)
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(cx, cy);
  int tl = sizeof(text);
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);
  tft.setTextWrap(true);
  tft.print(text);
}
void testdrawtext(String text, uint16_t color)
{

  tft.setCursor(cx, cy);
  tft.setTextWrap(true);
  // tft.setTextColor(TFT_BLACK, TFT_BLACK);
  // tft.print(oldsdata);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);

  // tft.print(tl);
  tft.setTextWrap(true);
  tft.print(text);
  oldsdata = text;

  // tft.unloadFont();
}
void drawtext(String text, uint16_t color)
{

  tft.setCursor(cx, cy);
  tft.setTextWrap(true);
  // tft.setTextColor(TFT_BLACK, TFT_BLACK);
  // tft.print(oldsdata);
  // tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  // tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);

  // tft.print(tl);
  tft.setTextWrap(true);
  tft.print(text);
  oldsdata = text;

  // tft.unloadFont();
}
void printWordWrap(String text, uint16_t color)
{

  tft.setCursor(cx, cy);
  // tft.printf("maxwait = %d\n", maxWait);
  tft.setTextWrap(true, false);
  // tft.setTextColor(TFT_BLACK, TFT_BLACK);
  // tft.print(oldsdata);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);

  // tft.print(tl);
  printSplitString(text, color);
}
void printSplitString(String text, uint16_t color)
{
  int wordStart = 0;
  int wordEnd = 0;
  while ((text.indexOf(' ', wordStart) >= 0) && (wordStart <= text.length()))
  {
    wordEnd = text.indexOf(' ', wordStart + 1);
    uint16_t len = tft.textWidth(text.substring(wordStart, wordEnd));
    if (tft.getCursorX() + len >= tft.width())
    {
      tft.println();
      if (wordStart > 0)
        wordStart++;
    }
    tft.print(text.substring(wordStart, wordEnd));
    wordStart = wordEnd;
  }
}

void printtextbig(
    String text, uint16_t color)
{

  tft.unloadFont();
  delay(25);
  tft.loadFont(sfpd_r28);
  tft.setCursor(cx, cy);
  tft.setTextWrap(true);
  // tft.setTextColor(TFT_BLACK, TFT_BLACK);
  // tft.print(oldsdata);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);

  // tft.print(tl);
  tft.print(text);
  oldsdata = text;

  tft.unloadFont();
  delay(25);
  tft.loadFont(sfpt_r14);
}
void printtextcs(
    String text, uint16_t color, uint8_t fsize)
{

  tft.unloadFont();
  delay(25);
  if (fsize == 16)
  {
    tft.loadFont(sfpt_r16);
  }
  else if (fsize == 18)
  {
    tft.loadFont(sfpt_r18);
  }
  else if (fsize == 20)
  {
    tft.loadFont(sfpd_r20);
  }
  else if (fsize == 24)
  {
    tft.loadFont(sfpd_r24);
  }
  else
  {
    tft.loadFont(sfpt_r16);
  }
  tft.setCursor(cx, cy);
  tft.setTextWrap(true);
  // tft.setTextColor(TFT_BLACK, TFT_BLACK);
  // tft.print(oldsdata);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);

  // tft.print(tl);
  tft.print(text);
  oldsdata = text;

  tft.unloadFont();
  delay(25);
  tft.loadFont(sfpt_r14);
}
