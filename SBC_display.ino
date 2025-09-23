// board: esp32-c3, littleFS, 1.2APP/1.5DATA
// methode: usb or software serial

#include <SPI.h>
// #define FS_NO_GLOBALS
#include <FS.h>
#include "tb_display.h"
#include <SoftwareSerial.h>
// #include <Tone32.h>
#include "note.h"
// #include "pitches.h"
#include "SBC_display.h"
#include <TimeLib.h>
#include "tft_setup.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <ArduinoJson.h>

StaticJsonDocument<200> doc;
// #define SS_DISABLE 0x1062 // black

// #define SS_DISABLE 0 // black

#include <Adafruit_NeoPixel.h>
#define NEOPIN 6 // PIN_D3
Adafruit_NeoPixel NEO = Adafruit_NeoPixel(1, NEOPIN, NEO_GRB + NEO_KHZ800);
int tmpNOTE = 440;
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
uint32_t hcolor = TFT_WHITE;
uint32_t acolor = TFT_WHITE;
bool updateSecondhand = false;
String rssmsg[300];
String siteonread;
bool noanim = false;
bool animation = false;
String MATCHTIME = "";
int ANIMATIONSPEED = 40;
bool gosave = false;
bool forcedrawclock = false;
// int httpGetChar();
#define BUZZER_PIN 6
#define BUZZER_CHANNEL 0

const uint32_t COLOR_MEDIUM[] = {TFT_WHITE, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_GREENYELLOW, TFT_PINK, TFT_ORANGE, TFT_RED, TFT_CYAN, TFT_MAGENTA, TFT_PINK, TFT_SKYBLUE};

const uint32_t COLORS_LIGHT[10] = {
    0xDB5B, 0x97E9, 0x8C7F, 0xFACC, 0xFFED,
    0x4F1F, 0x9ADF, 0xFD0B, 0x5DDF, 0xF9B1};
const uint32_t COLORS_DARK[10] = {
    0x2004, 0x0920, 0x0808, 0x4005, 0x0900,
    0x00E4, 0x280D, 0x20C0, 0x0006, 0x3000};
uint16_t analogClockProps[8] = {0, 0, 0, 0, 0, 0, 0, 0};
String nsb15 = "NotoSansBold15";
String sui14 = "SegoeUI-14";
String sfpt_r14 = "SFProText-Regular-14";
String sfpt_r16 = "SFProText-Regular-16";
String sfpt_r18 = "SFProText-Regular-18";
String sfpd_r20 = "SFProDisplay-Regular-20";
String sfpd_r24 = "SFProDisplay-Regular-24";
String sfpd_r28 = "SFProDisplay-Regular-28";
String sfpd_r56 = "SFProDisplay-Regular-56";
String sfpd_r84 = "SFProDisplay-Regular-84";

String scores = "";
String homescore = "";
String awayscore = "";
String hometeam = "";
String awayteam = "";
String matchtime = "";
int ihscore = 0; // home score in integer
int iascore = 0; // away score in integer

long prevmill = 0;
String oldsdata;
int tryrequest = 0;
bool foundRadio = false;
String data;
char c;
int toScreenSleep = 0;
int maxWait = 20;
String olddata = "";
int clockFace = 3, oldClockFace = 0;
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

// SoftwareSerial serial(21, 19);

long prevmill2 = 0;
long prevmill3 = 0;
int angka = 0;
bool blinking = false;
int nblinking = 6;
bool blinkstate = false;
int blinkval = 255;
int countblink = 22;
int blinkduration = 9; // by point (1=9)
int endmatch = 0;
int startblink = 0;
int endblink = 0;
#define usbbaud 115200
int dmode = 10;
void setup(void)
{
  Serial.begin(115200);
  // serial.begin(96050);
  // tone(BUZZER_PIN, tmpNOTE);
  // noTone(BUZZER_PIN);
  NEO.begin();
  NEO.show();
  // NEO.setPixelColor(0, 0);
  NEO.setPixelColor(0, NEO.Color(170, 0, 0));
  // NEO.show();
  NEO.show();
  Serial.print("Hello! ST77xx TFT Test");
  // pinMode(25, OUTPUT);
  pinMode(8, OUTPUT);

  // tone(BUZZER_PIN, tmpNOTE);
  Serial.print("tb init");
  tb_display_init(1);
  Serial.print("after tb init");
  tft.init();
  tft.setRotation(0);
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }

  // listFiles(); // Lists the files so you can see what is in the SPIFFS
  Serial.println("\r\nInitialisation done.");

  EEPROM.begin(EEPROM_SIZE);
  EEPROM_readAnything(0, config); // get saved settings
  if (config.magic_number != CONFIG_REVISION)
  { // this will set it up for very first use

    Serial.printf("magic wrong, was %ld, should be %ld\n", config.magic_number, CONFIG_REVISION);
    config.magic_number = CONFIG_REVISION;

    config.dmode = 0;
    config.animation = false;
    config.note = 2093;
    config.clockFace = 3; // default clock face
    config.aspeed = 40;   // default animation speed

    EEPROM_writeAnything(0, config);
    int sz = sizeof(config);
    Serial.print("config size used");
    Serial.println(sz);
    Serial.print("config size alocated");
    Serial.println(EEPROM_SIZE);
    EEPROM.commit();
  }
  tmpNOTE = config.note;
  // setNote(config.note);
  clockFace = config.clockFace;
  animation = config.animation;
  ANIMATIONSPEED = config.aspeed;
  ANIMATIONSPEED = constrain(ANIMATIONSPEED, 4, 60);
  dmode = config.dmode;
  Serial.printf("dmode = %d\n", dmode);

  tb_display_print_String("\nConnecting to WiFi...", 2);
  WiFi.begin("ASUS", "air46664");
  // WiFi.begin("RMN20", "air46664");
  // WiFi.begin("OFFLINE", "terbaik2025");
  // wifiMulti.addAP("OFFLINE", "terbaik2025");
  // wifiMulti.addAP("ASUS", "air46664");
  // wifiMulti.addAP("RMN20", "air46664");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    tb_display_print_String(".", 2);
    delay(200);
  }

  tb_display_print_String("\nsyncing to internet time", 2);
  tft.loadFont(sfpt_r14, LittleFS);
  Serial.println("Initialized");

  // tft.fillScreen(TFT_BLACK);
  syncTime();
  if (dmode == 10)
  {
    forcedrawclock = true;
    drawClockFace();
    Serial.println("draw clock");
  }
  else
  {
    testdrawtext("USB serial screen for pi-radio, \nbaud = 115200\nWaiting for incoming signal...", COLOR_MEDIUM[random(10)]);
  }
  uint16_t time = millis();
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  delay(1000);
  fillacf();
  delay(200);
  // noTone(BUZZER_PIN);c
  ledcSetup(BUZZER_CHANNEL, 1000, 8);        // Configure PWM
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL); // Attach the pin to the PWM channel
  beep();
}

void syncTime()
{
  if (Ping.ping(remote_host))
  {
    Serial.println("has internet");
    timeClient.begin();
    timeClient.setTimeOffset(3600 * 7);
    timeClient.update();
    // setSyncProvider(timeClient.getEpochTime());
    // setTime(int hr, int min, int sec, int dy, int mnth, int yr)
    Serial.println("has internet, sync device with internet time");
    setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(),
            timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());
  }
  else
  {
    Serial.println("ping failed");
  }
}
void fillacf()
{
  for (int i = 0; i < 8; i++)
  {
    analogClockProps[i] = COLOR_MEDIUM[random(10)];
    if (i == 1)
      analogClockProps[i] = COLORS_DARK[random(10)];
  }
}
int oldss = 0;
void loop()
{

  // while (serial.available() > 0)
  // {
  //   delay(10);
  //   c = serial.read();
  //   data += c;
  // }
  while (Serial.available() > 0)
  {
    delay(10);
    c = Serial.read();
    data += c;
  }
  if (data.length() > 0)
  {
    if (data.startsWith("#"))
      proccesCMD(data);
    else if (data.startsWith("$"))
      proccesJsonData(data);
    else
      proccesData(data);
    data = "";
  }
  if (millis() > prevmill + 100)
  {
    if (dmode == 10)
    {
      // Serial.println("analog clock");
      if (oldss != second())
      {
        oldss = second();
        if (oldss % 5 == 0)
          if (year() == 1970)
            syncTime();
          else if (WiFi.status() == WL_CONNECTED)
            WiFi.mode(WIFI_OFF);
        if (minute() % 5 == 0 && second() == 0)
        {
          clockFace = random(5);
          tft.fillScreen(TFT_BLACK);
        }
        // analogClock(0);
        // digitalClock(1);
        drawClockFace();
      }
      // analogClock(0);
    }
    toScreenSleep++;
    if (toScreenSleep > maxWait)
      toScreenSleep = 0;
    prevmill = millis();
  }
  beepnblink(); // beepnblink.ino
} // end loop
int displaylivescore = 0;
void proccesJsonData(String data)
{
  printtextcs(0, 129, data, TFT_WHITE, 16);
  data.replace("$", "");
  DeserializationError error = deserializeJson(doc, data);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  if (doc.containsKey("dmode"))
  {
    dmode = doc["dmode"];
  }
  if (doc.containsKey("save"))
  {
    String vdata = doc["save"];
    Serial.println(vdata);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.printf("save> %s\n", vdata.c_str());
    if (vdata.startsWith("dmode"))
    {
      Serial.print("save>");

      int idata = vdata.substring(6).toInt();
      Serial.printf("dmode = %d\n", idata);
      if (idata >= 0 && idata <= 10)
      {
        savepref();
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 230);
        tft.setTextColor(TFT_GREENYELLOW);
        printWordWrap("dmode set to " + String(dmode), COLOR_MEDIUM[random(12)]);
        beep();
      }
    }
  }
  if (doc.containsKey("animation"))
  {
    animation = doc["animation"];
    // savepref();
  }
  if (doc.containsKey("note"))
  {
    setNote(doc["note"]);
    // savepref();
  }
  if (doc.containsKey("rotation"))
  {
    int sr = doc["rotation"];
    tft.setRotation(sr);
  }
  if (doc.containsKey("clockface"))
  {
    clockFace = doc["clockface"];
    // savepref();
  }
  if (doc.containsKey("aspeed"))
  {
    if (doc["aspeed"] > 0)
      ANIMATIONSPEED = doc["aspeed"];
    ANIMATIONSPEED = constrain(ANIMATIONSPEED, 4, 100);
    // else
    // ANIMATIONSPEED = doc["anspeed"];
  }
  if (doc.containsKey("time"))
  {
    String stime = doc["time"];
    int h = stime.substring(0, 2).toInt();
    int m = stime.substring(3, 5).toInt();
    int s = stime.substring(6, 8).toInt();
    setTime(h, m, s, 2, 7, 2021);
    // setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(),
    //             timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());
  }

  // dmode = doc["dmode"];
  // animation = doc["animation"];
  // tmpNOTE = doc["note"];
  // tft.setRotation(doc["rotation"]);
}
void proccesCMD(String data)
{
  data.replace("#", "");
  if (data.length() > 3)
  {

    toScreenSleep = 0;
    maxWait = (data.length() > 18) ? data.length() / 6 : 40;
    // tft.printf("maxwait = %d\n", maxWait);
    if (data.startsWith("rotation"))
    {
      int sr = data.substring(9).toInt();
      tft.setRotation(sr);
      // testdrawtext("rotated", COLOR_MEDIUM[random(12)]);
      printWordWrap("rotated", COLOR_MEDIUM[random(12)]);
      data = "";
      return;
    }
    else if (data.startsWith("save"))
    {
      String sdata = data.substring(5);
      Serial.print("save> ");
      Serial.println(sdata);
      if (sdata.startsWith("dmode"))
      {
        int idata = data.substring(11).toInt();
        Serial.printf("dmode = %d\n", idata);
        if (idata >= 0 && idata <= 10)
        {
          dmode = idata;
          savepref();
          snackBar("dmode : " + String(dmode));
          beep();
        }
      }
      else if (sdata.startsWith("animation"))
      {
        String sdata = data.substring(15);
        Serial.printf("sdata  : %s \n", sdata.c_str());
        if (sdata == "1")
          animation = true;
        else
          animation = false;
        snackBar("enable animation : " + String(animation));
        beep();
      }
      else if (sdata.startsWith("note"))
      {
        setNote(sdata.substring(5));
        // savepref();
      }
      else if (sdata.startsWith("clockface"))
      {
        int idata = sdata.substring(10).toInt();
        if (idata < 5)
        {
          if (idata != clockFace)
          {

            tft.fillScreen(TFT_BLACK);
            clockFace = idata;
            // savepref();
            snackBar("clock face : " + String(clockFace));
            beep();
            Serial.println("startblinking");
          }
        }
      }
      else if (sdata.startsWith("aspeed"))
      {
        int idata = sdata.substring(7).toInt();
        if (idata > 0)
        {
          ANIMATIONSPEED = idata;
          snackBar("animation speed  : " + String(ANIMATIONSPEED));
          beep();
        }
      }
      writePref();
    } //============= end save================
    else if (data.startsWith("resetscreen"))
    {
      tft.fillScreen(TFT_BLACK);
      data = "";
      return;
    }
    else if (data.startsWith("setnote"))
    {
      // tft.fillScreen(TFT_BLACK);
      data = data.substring(8);
      // data.replace("\n", "");
      // data.replace("\r", "");
      // data.replace("\0", "");
      for (int i = 0; i < sizeof(notes) / sizeof(struct Note); i++)
      {
        // Serial.printf("note : -%s- -%s-\n", data, notes[i].name);

        if (data.startsWith(notes[i].name))
        {
          tmpNOTE = notes[i].frequency;
          beep();
          Serial.println("start beeping " + notes[i].name);
          data = "";
          tft.fillRect(0, 230, 240, 10, TFT_BLACK);
          tft.setCursor(0, 230);
          tft.setTextColor(TFT_GREENYELLOW);
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
    else if (data.startsWith("aspeed"))
    {
      int idata = data.substring(7).toInt();
      if (idata > 0)
      {
        ANIMATIONSPEED = idata;
        snackBar("animation speed  : " + String(ANIMATIONSPEED));
        beep();
      }
      data = "";
    }
    else if (data.startsWith("clockface"))
    {
      int idata = data.substring(10).toInt();
      if (idata < 5)
      {
        if (idata != clockFace)
        {

          tft.fillScreen(TFT_BLACK);
          clockFace = idata;
          beep();
          Serial.println("startblinking");
        }
      }
      data = "";
    }
    else if (data.startsWith("shakeface"))
    {

      tft.fillScreen(TFT_BLACK);
      clockFace = random(5);
      beep();
      Serial.println("startblinking");
      data = "";
    }
    else if (data.startsWith("settime"))
    {
      int h = data.substring(8, 10).toInt();
      int m = data.substring(11, 13).toInt();
      int s = data.substring(14, 16).toInt();
      updateSecondhand = false;
      beep();
      Serial.println("startblinking");
      setTime(h, m, s, 2, 7, 2021);
      data = "";

      // tft.fillRect(0, 230, 240, 10, TFT_BLACK);
      // tft.setCursor(0, 230);
      // tft.setTextColor(TFT_GREENYELLOW);
      // tft.printf("time set to %02d:%02d:%02d \n", h, m, s);
      char info[40];
      sprintf(info, "time set to %02d:%02d:%02d", h, m, s);
      snackBar(info);
      // printWordWrap("time set to " + String(h) + ":" + String(m) + ":" + String(s), COLOR_MEDIUM[random(12)]);
      // setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(),
      //             timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());
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
    else if (data.startsWith("animation"))
    {
      String sdata = data.substring(10, 11);
      Serial.printf("sdata  : %s \n", sdata.c_str());
      if (sdata == "1")
        animation = true;
      else
        animation = false;
      char info[40];
      sprintf(info, "animation  : %s \n", (animation) ? "true" : "false");
      snackBar(info);

      data = "";
      beep();
      return;
    }
    else if (data.startsWith("dmode"))
    {
      if (data == "dmode")
      {
        tft.fillScreen(TFT_BLACK);
        tft.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode\n10 clock", dmode);

        return;
      }
      else
      {
        int dmod = data.substring(6).toInt();
        if (dmod == 10 && dmode != 10)
          tft.fillScreen(TFT_BLACK);
        if (dmod < 3 || dmod == 10)
          dmode = dmod;
        // Serial.println("startblinking");
        tft.setCursor(0, 0);
        Serial.printf("change display mode to : %d \n", dmode);
        tft.fillScreen(TFT_BLACK);
        tft.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode\n2 clock", dmode);

        drawClockFace();
        data = "";
        return;
      }
    }

    data = "";
  }
}

void beep()
{

  tone(BUZZER_PIN, tmpNOTE);
  NEO.setPixelColor(0, NEO.Color(0, 0, 255));
  NEO.show();
  delay(100);
  noTone(BUZZER_PIN);
  NEO.setPixelColor(0, NEO.Color(0, 0, 0));
  NEO.show();
}

void proccesData(String data)
{

  if (data.length() > 4)
  {
    if (data.startsWith("noanim"))
    {
      noanim = true;
      data = data.substring(6);
    }
    toScreenSleep = 0;
    maxWait = (data.length() > 18) ? data.length() / 6 : 40;
    // tft.printf("maxwait = %d\n", maxWait);
    if (data.startsWith("testnum"))
    {

      int sr = data.substring(7).toInt();
      tft.fillScreen(TFT_BLACK);
      testDTSegment(sr);
      data = "";
      return;
    }
    else if (data.startsWith("play pos"))
    {
      printtextbig(data, COLOR_MEDIUM[random(12)]);
      data = "";
      return;
    }
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

        scores = data.substring(data.indexOf(">"));
        homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
        awayscore = scores.substring(scores.indexOf("-") + 1);
        hometeam = data.substring(0, data.indexOf("vs"));
        awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf("["));
        matchtime = awayteam.substring(data.indexOf("\n"));
        ihscore = homescore.toInt();
        iascore = awayscore.toInt();
        MATCHTIME = data.substring(data.indexOf("[") + 1, data.indexOf("]"));
        // scores.replace(" ", "");
        Serial.printf(" cx : %d \n", 120 - (50 * (scores.length() / 2)));
        if (ihscore > 9 || iascore > 9)
        {
          if (scores != old_score)
          {
            tft.fillScreen(TFT_BLACK);
            old_score = scores;
          }
          int cx = 0, cy = 0;
          printtextcs(cx, cy, hometeam, COLOR_MEDIUM[random(12)], 16);
          cx = 0, cy = 130;
          printtextcs(cx, cy, awayteam, COLOR_MEDIUM[random(12)], 16);
          cx = 120 - ((50 * MATCHTIME.length()) / 2), cy = 160;
          printtextcs(cx, cy, MATCHTIME, COLOR_MEDIUM[random(12)], 84);

          cx = 120 - ((50 * scores.length()) / 2), cy = 40;
          printtextcs(cx, cy, scores, COLOR_MEDIUM[random(12)], 84);
        }
        else
        {
          if (displaylivescore == 0)
            ssgmnt(homescore);
          else if (displaylivescore == 1)
            displayscore(homescore);
          else if (displaylivescore == 2)
            tsgmnt(homescore);
          else if (displaylivescore == 3)
            drawDigitLivescore(homescore);
        }
        if (noanim)
          noanim = false;
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
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);
  tft.setTextWrap(true);
  tft.print(text);
  oldsdata = text;
}
void drawtext(String text, uint16_t color)
{
  tft.setCursor(cx, cy);
  tft.setTextWrap(true);
  tft.setTextColor(color, TFT_BLACK);
  tft.print(text);
  oldsdata = text;
}

void printWordWrap(String text, uint16_t color)
{
  tft.setTextWrap(true, false);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);
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
  tft.loadFont(sfpd_r28, LittleFS);
  tft.setTextWrap(true);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);
  tft.print(text);
  oldsdata = text;
  tft.unloadFont();
  delay(25);
  tft.loadFont(sfpt_r14, LittleFS);
}
void printtextcs(int x, int y,
                 String text, uint16_t color, uint8_t fsize)
{

  tft.unloadFont();
  delay(25);

  if (fsize == 16)
    tft.loadFont(sfpt_r16, LittleFS);
  else if (fsize == 18)
    tft.loadFont(sfpt_r18, LittleFS);
  else if (fsize == 20)
    tft.loadFont(sfpd_r20, LittleFS);
  else if (fsize == 24)
    tft.loadFont(sfpd_r24, LittleFS);
  else if (fsize == 28)
    tft.loadFont(sfpd_r28, LittleFS);
  else if (fsize == 56)
    tft.loadFont(sfpd_r56, LittleFS);
  else if (fsize == 84)
    tft.loadFont(sfpd_r84, LittleFS);
  else
    tft.loadFont(sfpt_r16, LittleFS);

  tft.setCursor(x, y);
  tft.setTextWrap(true);
  tft.setTextColor(color, TFT_BLACK);
  tft.print(text);
  oldsdata = text;

  tft.unloadFont();
  delay(25);
  tft.loadFont(sfpt_r14, LittleFS);
}

void printtextcs(
    String text, uint16_t color, uint8_t fsize)
{

  tft.unloadFont();
  delay(25);
  if (fsize == 16)
    tft.loadFont(sfpt_r16, LittleFS);
  else if (fsize == 18)
    tft.loadFont(sfpt_r18, LittleFS);
  else if (fsize == 20)
    tft.loadFont(sfpd_r20, LittleFS);
  else if (fsize == 24)
    tft.loadFont(sfpd_r24, LittleFS);
  else
    tft.loadFont(sfpt_r16, LittleFS);
  tft.setCursor(cx, cy);
  tft.setTextWrap(true);
  tft.fillScreen(TFT_BLACK);
  int tl = text.length();
  int cymr = map(tl, 10, 100, 80, 15);
  tft.setCursor(cx, random(1, cymr));
  tft.setTextColor(color, TFT_BLACK);
  tft.print(text);
  oldsdata = text;
  tft.unloadFont();
  delay(25);
  tft.loadFont(sfpt_r14, LittleFS);
}

void drawClockFace()
{
  if (clockFace == 0)
  {
    analogClock(clockFace);
  }
  else
  {
    if (second() % 30 == 0 || forcedrawclock)
    {
      digitFace(clockFace - 1);
      forcedrawclock = false;
    }
  }
}

void setNote(String note)
{
  for (int i = 0; i < sizeof(notes) / sizeof(struct Note); i++)
  {
    // Serial.printf("note : -%s- -%s-\n", data, notes[i].name);
    tmpNOTE = 440; // default note
    // if (note.startsWith(notes[i].name))
    if (note.startsWith(notes[i].name))
    {
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
      // printWordWrap("note set to " + notes[i].name, COLOR_MEDIUM[random(12)]);
      snackBar("note set to " + notes[i].name);
      break;
    }
  }
}

void savepref()
{
  if (gosave)
  {
    gosave = false;
  }
  if (config.dmode != dmode || config.animation != animation || config.note != tmpNOTE || config.clockFace != clockFace || config.aspeed != ANIMATIONSPEED)
  {
    config.dmode = dmode;
    config.animation = animation;
    config.note = tmpNOTE;
    config.clockFace = clockFace;
    config.aspeed = ANIMATIONSPEED;
    writePref();
    snackBar("saved preferences");
    Serial.println("saved preferences");
  }
}

void writePref()
{
  EEPROM_writeAnything(0, config);
  EEPROM.commit();
} // end of writePref

// function for readPref
void readPref()
{
  EEPROM_readAnything(0, config); // get saved settings
} // end of readPref
// bottom info
void snackBar(String text)
{
  tft.fillRect(0, 230, 240, 10, TFT_BLACK);
  tft.setCursor(0, 230);
  tft.setTextColor(TFT_GREENYELLOW);
  tft.printf("%s\n", text.c_str());
}