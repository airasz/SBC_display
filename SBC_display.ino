// board m5stickc
// methode: usb serial / soft serial
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <M5StickC.h>
#include <ArduinoOTA.h>
#include "tb_display.h"

#include "note.h"
#include "res.h"

bool newline = false;
// #include  <ESP8266WiFi.h>
bool screenOR = true;
const int BUTTON_HOME = GPIO_NUM_37;
const int BUTTON_PIN = GPIO_NUM_39;
String oldstr;
int runmode = 0, titlex = 0;
long previousMillis = 0;
long prevmill2 = 0;
int interval = 40;
const char *ssid = "RMN20";
const char *password = "air46664";
String rssmsg[300];
String siteonread;

const byte numChars = 60;
char receivedChars[numChars];
bool newData = false;
int angka = 0;
bool blinking = true;
int nblinking = 6;
bool blinkstate = false;
int blinkval = 255;
int countblink = 16;

const uint32_t COLORS_LIGHT[10] = {
    0xDB5B, 0x97E9, 0x8C7F, 0xFACC, 0xFFED,
    0x4F1F, 0x9ADF, 0xFD0B, 0x5DDF, 0xF9B1};

const uint32_t COLORS_DARK[10] = {
    0x2004, 0x0920, 0x0808, 0x4005, 0x0900,
    0x00E4, 0x280D, 0x20C0, 0x0006, 0x3000};

// const uint32_t COLOR_MEDIUM[] = {
//     PURPLE, BLUE, GREEN, YELLOW, GREENYELLOW,
//     PINK, ORANGE, RED, CYAN, MAGENTA};

const uint32_t COLOR_MED[] = {
    0x4810, BLUE, GREEN, 0x7380,
    PINK, 0x7380, RED, 0x032D, MAGENTA};
// String rssmsg[] = {
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
// };
int rssmsgidx = 0;
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
int delayPerCharacter = 200;
const int delayPerArticle = 4000;
int delayPerRSS = 100;
const char label = 0xfc;

int itemDepth = 0;
int lastTagMatches = 0;
char *contentsToDisplay;

int httpGetChar();
SoftwareSerial serial(36, 26);
String data;
char c;

#define BUZZER_PIN 26
#define BUZZER_CHANNEL 0

// long prevmill2 = 0;
long prevmill3 = 0;
// int angka = 0;
// bool blinking = false;
// int nblinking = 6;
// bool blinkstate = false;
// int blinkval = 255;
// int countblink = 22;
int blinkduration = 9; // by point (1=9)
int endmatch = 0;
int startblink = 0;
int endblink = 0;

int tmpNOTE = 1123;
#define usbbaud 115200
// int dmode = 3;
void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  serial.begin(115200);
  M5.begin();
  Serial.println("starting piradio reader");
  M5.IMU.Init();
  pinMode(BUTTON_HOME, INPUT | PULLUP);
  pinMode(BUTTON_PIN, INPUT | PULLUP);
  pinMode(10, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), mode, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_HOME), resett, RISING);

  // M5.Lcd.SMOOTH_FONT();
  M5.Axp.ScreenBreath(50); // 0-100

  // M5.Lcd.setRotation(1);
  tb_display_init(1);
  M5.Lcd.fillScreen(BLACK);
  cekIMU();
  M5.Lcd.setCursor(0, 5);
  M5.Lcd.setTextSize(2);
  // M5.Lcd.setTextColor(0x00aadd00, 0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.println("0 livescore\n1 typing mode\n2 statis mode");

  M5.Lcd.setTextSize(1);

  ledcSetup(BUZZER_CHANNEL, 1000, 10);       // Configure PWM
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL); // Attach the pin to the PWM channel
}

void resett()
{
  ESP.restart();
}
int dmode = 1;
void mode()
{
  dmode++;
  if (dmode > 2)
  {
    dmode = 0;
  }
  // M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
  // M5.Lcd.setTextSize(1);
}

long prevmill = 0;
String oldsdata;
int tryrequest = 0;
bool foundRadio = false;
// String data;
// char c;
int toScreenSleep = 0;
int maxWait = 20;
String olddata = "";
void loop()
{
  // put your main code here, to run repeatedly:

  ArduinoOTA.handle();
  M5.update();
  if (M5.BtnA.wasPressed())
  {
    screenOR = !screenOR;

    M5.Lcd.setRotation((screenOR) ? 1 : 3);
    // tb_display_init(screen_orientation);
  }
  // for (int i = 0; i < sizeof(sites) / sizeof(struct site_t); i++) {
  //   Serial.print("size of site : ");
  //   Serial.println(sizeof(sites) / sizeof(struct site_t));
  //   // Serial.println("wifi status = ");
  //   // Serial.println(wifiMulti.run());
  //   if ((wifimulti.run() == WL_CONNECTED)) {
  //     Serial.println("wifi connected");
  //     itemDepth = 0;
  //     lastTagMatches = 0;

  //     // M5.Lcd.fillScreen(BLACK);
  //     // M5.Lcd.setCursor(0, 0);
  //     // M5.Lcd.println(sites[i].title);
  //     contentsToDisplay = sites[i].contentsToDisplay;
  //     //      http.begin(sites[i].url);

  //     http.begin(url);
  //     int httpCode = http.GET();
  //     Serial.printf(" get feed from site %d : %s \n", i, sites[i]);
  //     Serial.printf(" respond code : %d \n", httpCode);
  //     if (httpCode > 0) {
  //       if (httpCode == HTTP_CODE_OK) {

  //         cekIMU();
  //         sdata = http.getString();
  //         sdata.replace("<br>", "");
  //         sdata.replace("[", "\n[");
  //         sdata.replace("%", "%\n");
  //         sdata = ">" + sdata;
  //         //          stream = http.getStreamPtr();

  //         tb_display_print_String(sdata.c_str(), 100);
  //         // siteonread = sites[i];
  //       }
  //     } else {
  //       // testdrawtext("no respon", TFT_WHITE);
  //       // M5.Lcd.fillScreen(TFT_BLACK);
  //       if (trdip() == 1) {
  //         url = sites[3].url;
  //       } else {
  //         url = sites[2].url;
  //       }
  //     }
  //     http.end();

  //     M5.Lcd.setCursor(0, 8);
  //     delay(delayPerRSS);
  //     // cekIMU();
  //   }
  //   if (millis() - previousMillis == 1000) {
  //     // cekIMU();
  //     previousMillis = millis();
  //   }
  //   // else
  //   // {
  //   //   WiFi.disconnect();
  //   //   WiFi.reconnect();
  //   // }
  // }
  if (millis() - previousMillis > 1000)
  {
    cekIMU();
    previousMillis = millis();
  }
  if (millis() > prevmill2 + 100)
  {
    angka++;
    if (angka == 9)
    {
      if (blinking)
      {
        countblink++;
        digitalWrite(10, LOW);
        if (countblink > nblinking)
        {
          blinking = false;
          digitalWrite(10, HIGH);
          countblink = 0;
        }
      }
    }
    if (angka > 9)
    {
      if (blinking)
        digitalWrite(10, HIGH);
      angka = 0;
    }
    prevmill2 = millis();
  }

  while (serial.available() > 0)
  {
    delay(10);
    c = serial.read();
    data += c;
  }
  while (Serial.available() > 0)
  {
    // delay(10);
    c = Serial.read();
    data += c;
  }

  if (data.length() > 0)
  {
    if (!data.startsWith("#"))
      proccesData(data);
    else
      proccesCMD(data.substring(1));
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
  beepnblink(); // beepnblink.ino
  rwnm();
  // showNewData();
}

void displayinfo(String Header, String body)
{
  M5.Lcd.fillScreen(BLACK);
  // M5.Lcd.setFont();
  M5.Lcd.setTextColor(0xAA00FF00);
  // M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.print(Header);
  M5.Lcd.setTextColor(WHITE);
  // M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 9);
  // Serial.print("msgg = ");
  // Serial.println(msgg[countindex-1]);
  M5.Lcd.setTextWrap(true);
  M5.Lcd.print(body);
  // display.display();
}

void displayinfo(String msg)
{
  M5.Lcd.fillScreen(BLACK);
  // M5.Lcd.setFont();
  M5.Lcd.setTextColor(WHITE);
  // M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.print(msg);
  // display.display();
}

// void displayscore(String score)
// {

//   M5.Lcd.fillScreen(BLACK);
//   String scores = data.substring(data.indexOf(">"));
//   String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
//   String awayscore = scores.substring(scores.indexOf("-") + 1);
//   String hometeam = data.substring(0, data.indexOf("vs"));
//   String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));

//   score.replace("-", "");
//   score.replace(" ", "");
//   int clock_style = random(2);
//   int count = 0;
//   count = (homescore.toInt() * 100) + awayscore.toInt();
//   // count = score.toInt();
//   Serial.printf("count  : %d \n", count);
//   int x_start = 25;
//   int x_delta = 6;
//   int r = 5;
//   int rd = random(2);
//   for (int n = 0; n < 10; n++)
//   {
//     // canvas.fillCircle(x_start + x_delta * n, 4, r, COLORS_LIGHT[n]);
//   }

//   int y2 = 11;
//   for (int n = 0; n < 10; n++)
//   {
//     // canvas.fillCircle(x_start + x_delta * n, y2, r, COLORS_DARK[n]);
//   }
//   // int testgigit=DIGITS[0][0][4];
//   // Serial.printf("testgigit %d\n",testgigit);
//   int y_start = 16;
//   for (int pos = 1; pos < 4; pos++)
//   {
//     uint8_t curr_digit = 0;
//     if (pos == 0)
//     {
//       curr_digit = count / 1000;
//       if (curr_digit == 0)
//         curr_digit = 11; // make it blank. see on res.h
//     }
//     else if (pos == 1)
//     {
//       curr_digit = count / 100 % 10;
//     }
//     else if (pos == 2)
//     {
//       // curr_digit = count / 10 % 10;
//       curr_digit = 10; // make it (-). see on res.h
//     }
//     else if (pos == 3)
//     {
//       curr_digit = count % 10;
//     }
//     int rnd = random(10);
//     for (int row = 0; row < 7; row++)
//     {
//       for (int col = 0; col < 5; col++)
//       {
//         uint32_t color = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[curr_digit] : COLORS_DARK[curr_digit];
//         uint32_t colorrnd = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[rnd] : COLORS_DARK[rnd];
//         if (DIGITS[curr_digit][row][col] == 1)
//         {
//           if (clock_style == 0)
//           {
//             M5.Lcd.fillCircle(x_start + col * 7 - 1 + random(4), y_start + row * 7 - 1 + random(4), r, colorrnd);
//             // M5.Lcd.fillCircle(x_start + col * 7 -1+random(4), y_start + row * 7-1+random(4), 3, BLACK);
//             // M5.Lcd.fillCircle(x_start + col * 7 -1+random(4), y_start + row * 7-1+random(4), r, colorrnd);
//           }
//           else if (clock_style == 1)
//           {
//             M5.Lcd.fillRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
//           }
//         }
//         else
//         {
//           if (clock_style == 1)
//           {
//             M5.Lcd.drawRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
//           }
//           else if (clock_style == 0)
//           {
//             // M5.Lcd.drawCircle(x_start + col * 7, y_start + row * 7, r, colorrnd);
//           }
//         }
//         // M5.Lcd.fillRect (x_start + col * 7, y_start + row * 7, r, colorrnd);
//         // RGB565 = (((RGB888&0xf80000)>>8) + ((RGB888&0xfc00)>>5) + ((RGB888&0xf8)>>3));
//       }
//     }
//     x_start += 39;
//   }

//   M5.Lcd.setTextColor(WHITE);
//   M5.Lcd.setCursor(0, 0);
//   M5.Lcd.println(hometeam);
//   M5.Lcd.setCursor(0, 72);
//   M5.Lcd.println(awayteam);
// }
void rwnm()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
}

int trdip()
{
  int trdip;
  String LIP = WiFi.localIP().toString();
  int idx = LIP.lastIndexOf(".");
  trdip = LIP.substring(8, idx).toInt();

  Serial.print("trdip  = ");
  Serial.println(trdip);
  return trdip;
}

int displaylivescore = 0;
void proccesCMD(String data)
{
  if (data.length() > 4)
  {
    Serial.printf("data = %s\n", data.c_str());
    // tb_display_print_String(data.c_str(), 20);
    toScreenSleep = 0;
    // if (data.length() > 10)
    //   maxWait = data.length() / 10
    // else
    maxWait = (data.length() > 18) ? data.length() / 6 : 40;
    // M5.Lcd.printf("maxwait = %d\n", maxWait);
    if (data.startsWith("rotation"))
    {
      int sr = data.substring(9).toInt();
      M5.Lcd.setRotation(sr);
      // testdrawtext("rotated", COLOR_MEDIUM[random(12)]);
      printWordWrap("rotated", COLOR_MEDIUM[random(12)]);
      data = "";
      return;
    }
    else if (data.startsWith("resetscreen"))
    {
      M5.Lcd.fillScreen(TFT_BLACK);
      data = "";
      return;
    }
    else if (data.startsWith("setnote"))
    {
      M5.Lcd.fillScreen(TFT_BLACK);
      data = data.substring(8);
      // data.replace("\n", "");
      // data.replace("\r", "");
      // data.replace("\0", "");
      for (int i = 0; i < sizeof(notes) / sizeof(struct Note); i++)
      {
        // Serial.printf("note : -%s- -%s-\n", data, notes[i].name);

        if (data.startsWith(notes[i].name))
        {
          // tone(BUZZER_PIN, notes[i].note, 500, BUZZER_CHANNEL);
          // ledcWriteTone(BUZZER_CHANNEL, notes[i].note);

          tmpNOTE = notes[i].frequency;
          nblinking = 1;
          blinking = true;
          blinkduration = 9;
          startblink = 2;
          endblink = 8;
          angka = 0;
          countblink = 0;
          Serial.println("start beeping " + notes[i].name);
          data = "";
          prevmill2 = millis();
          printWordWrap("note set to " + notes[i].name, COLOR_MEDIUM[random(12)]);
          break;
        }
      }

      printWordWrap(data.substring(7), COLOR_MEDIUM[random(12)]);
    }

    else if (data.startsWith("blink"))
    {
      nblinking = data.substring(6).toInt();
      blinking = true;
      blinkduration = 9;
      startblink = 9;
      endblink = 10;
      countblink = 0;
      angka = 7;
      Serial.println("startblinking");
      data = "";
      prevmill2 = millis();
      return;
    }
    else if (data.startsWith("longbeep"))
    {
      nblinking = 1;
      blinking = true;
      blinkduration = 9;
      startblink = 2;
      endblink = 8;
      angka = 0;
      countblink = 0;
      Serial.println("start beeping");
      data = "";
      prevmill2 = millis();
      return;
    }
    else if (data.startsWith("endmatch"))
    {
      nblinking = 2;
      blinking = true;
      blinkduration = 19;
      endmatch = 1;
      startblink = 9;
      endblink = 10;
      angka = 7;
      countblink = 0;
      Serial.println("start beeping");
      data = "";
      prevmill2 = millis();
      return;
    }
    else if (data.startsWith("dmode"))
    {
      if (data == "dmode")
      {
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
        return;
      }
      else
      {
        int dmod = data.substring(6).toInt();
        if (dmod < 3)
          dmode = dmod;
        // Serial.println("startblinking");
        M5.Lcd.setCursor(0, 0);
        // M5.Lcd.setTextSize(2);
        Serial.printf("change display mode to : %d \n", dmode);
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
        // M5.Lcd.setTextSize(1);
        data = "";
        return;
      }
    }

    data = "";
  }
}

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
    // M5.Lcd.printf("maxwait = %d\n", maxWait);
    if (data.startsWith("testnum"))
    {

      int sr = data.substring(7).toInt();
      M5.Lcd.fillScreen(TFT_BLACK);
      testDTSegment(sr);
      data = "";
      return;
    }
    else if (data.startsWith("play pos"))
    {
      // printtextbig(data, COLOR_MEDIUM[random(12)]);
      testdrawtext(data, COLOR_MEDIUM[random(12)]);
      data = "";
      return;
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
          M5.Lcd.fillScreen(TFT_BLACK);
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
      else if (dmode == 3)
      {
        // displayinfo(data);
        tb_display_print_String(data.c_str(), 2);
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
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(cx, cy);
  int tl = sizeof(text);
  int cymr = map(tl, 10, 100, 80, 15);
  M5.Lcd.setCursor(cx, random(1, cymr));
  M5.Lcd.setTextColor(color, TFT_BLACK);
  // M5.Lcd.setTextWrap(true);
  M5.Lcd.print(text);
}
void testdrawtext(String text, uint16_t color)
{

  M5.Lcd.setCursor(cx, cy);
  // M5.Lcd.setTextWrap(true);
  // M5.Lcd.setTextColor(TFT_BLACK, TFT_BLACK);
  // M5.Lcd.print(oldsdata);
  M5.Lcd.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  M5.Lcd.setCursor(cx, random(1, cymr));
  M5.Lcd.setTextColor(color, TFT_BLACK);

  // M5.Lcd.print(tl);
  // M5.Lcd.setTextWrap(true);
  M5.Lcd.print(text);
  oldsdata = text;

  // M5.Lcd.unloadFont();
}
void drawtext(String text, uint16_t color)
{

  M5.Lcd.setCursor(cx, cy);
  // M5.Lcd.setTextWrap(true);
  // M5.Lcd.setTextColor(TFT_BLACK, TFT_BLACK);
  // M5.Lcd.print(oldsdata);
  // M5.Lcd.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  // M5.Lcd.setCursor(cx, random(1, cymr));
  M5.Lcd.setTextColor(color, TFT_BLACK);

  // M5.Lcd.print(tl);
  // M5.Lcd.setTextWrap(true);
  M5.Lcd.print(text);
  oldsdata = text;

  // M5.Lcd.unloadFont();
}
void printWordWrap(String text, uint16_t color)
{

  M5.Lcd.setCursor(cx, cy);
  // M5.Lcd.printf("maxwait = %d\n", maxWait);
  // M5.Lcd.setTextWrap(true, false);
  // M5.Lcd.setTextColor(TFT_BLACK, TFT_BLACK);
  // M5.Lcd.print(oldsdata);
  M5.Lcd.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  M5.Lcd.setCursor(cx, random(1, cymr));
  M5.Lcd.setTextColor(color, TFT_BLACK);

  // M5.Lcd.print(tl);
  printSplitString(text, color);
}
void printSplitString(String text, uint16_t color)
{
  int wordStart = 0;
  int wordEnd = 0;
  while ((text.indexOf(' ', wordStart) >= 0) && (wordStart <= text.length()))
  {
    wordEnd = text.indexOf(' ', wordStart + 1);
    uint16_t len = M5.Lcd.textWidth(text.substring(wordStart, wordEnd));
    if (M5.Lcd.getCursorX() + len >= M5.Lcd.width())
    {
      M5.Lcd.println();
      if (wordStart > 0)
        wordStart++;
    }
    M5.Lcd.print(text.substring(wordStart, wordEnd));
    wordStart = wordEnd;
  }
}

// void printtextbig(
//     String text, uint16_t color)
// {

//   M5.Lcd.unloadFont();
//   delay(25);
//   M5.Lcd.loadFont(sfpd_r28);
//   M5.Lcd.setCursor(cx, cy);
//   M5.Lcd.setTextWrap(true);
//   // M5.Lcd.setTextColor(TFT_BLACK, TFT_BLACK);
//   // M5.Lcd.print(oldsdata);
//   M5.Lcd.fillScreen(TFT_BLACK);
//   int tl = text.length();
//   int cymr = map(tl, 10, 100, 80, 15);
//   M5.Lcd.setCursor(cx, random(1, cymr));
//   M5.Lcd.setTextColor(color, TFT_BLACK);

//   // M5.Lcd.print(tl);
//   M5.Lcd.print(text);
//   oldsdata = text;

//   M5.Lcd.unloadFont();
//   delay(25);
//   M5.Lcd.loadFont(sfpt_r14);
// }

// void printtextcs(
//     String text, uint16_t color, uint8_t fsize)
// {

//   M5.Lcd.unloadFont();
//   delay(25);
//   if (fsize == 16)
//   {
//     M5.Lcd.loadFont(sfpt_r16);
//   }
//   else if (fsize == 18)
//   {
//     M5.Lcd.loadFont(sfpt_r18);
//   }
//   else if (fsize == 20)
//   {
//     M5.Lcd.loadFont(sfpd_r20);
//   }
//   else if (fsize == 24)
//   {
//     M5.Lcd.loadFont(sfpd_r24);
//   }
//   else
//   {
//     M5.Lcd.loadFont(sfpt_r16);
//   }
//   M5.Lcd.setCursor(cx, cy);
//   M5.Lcd.setTextWrap(true);
//   // M5.Lcd.setTextColor(TFT_BLACK, TFT_BLACK);
//   // M5.Lcd.print(oldsdata);
//   M5.Lcd.fillScreen(TFT_BLACK);
//   int tl = text.length();
//   int cymr = map(tl, 10, 100, 80, 15);
//   M5.Lcd.setCursor(cx, random(1, cymr));
//   M5.Lcd.setTextColor(color, TFT_BLACK);

//   // M5.Lcd.print(tl);
//   M5.Lcd.print(text);
//   oldsdata = text;

//   M5.Lcd.unloadFont();
//   delay(25);
//   M5.Lcd.loadFont(sfpt_r14);
// }
float accX = 0;
float accY = 0;
float accZ = 0;
float ACCx1 = 0.31;
float ACCx2 = 0.11; //(-0.11);
// 1.20
float ACCy1 = 1.20;
int orient = 0, lastOrient = 0;

void cekIMU()
{
  // Serial.println("IMU checked");
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  if (accX < ACCx2)
  {
    orient = 3;
  }
  else if (accX > ACCx1)
  {
    orient = 1;
  }
  if (lastOrient != orient)
  {
    lastOrient = orient;
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setRotation(lastOrient);
  }
}