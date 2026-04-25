// board: esp32
//  methode: usb or software serial

#include <SPI.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include "tb_display.h"
#include "indonesian_map.h"
#include "ina_e.h"
#include "ina_w.h"
#include <SoftwareSerial.h>
// #include <Tone32.h>

#include <TimeLib.h>
#include "SD.h"
#include "note.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <ArduinoJson.h>
// #include "Adafruit_MMA8451.h"
// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
#include "SparkFun_MMA8452Q.h" // Click here to get the library: http://librarymanager/All#SparkFun_MMA8452Q

MMA8452Q accel; // create instance of the MMA8452 class

#include <Wire.h>

// Adafruit_MPU6050 mpu;
// Adafruit_MMA8451 mma = Adafruit_MMA8451();
#include <Adafruit_NeoPixel.h>
#define NEOPIN 12 // PIN_D3
#define PIN_BACKLIGHT 4
Adafruit_NeoPixel NEO = Adafruit_NeoPixel(1, NEOPIN, NEO_GRB + NEO_KHZ800);
SPIClass SPI_EXT;
enum
{
  spi_sck = 18,
  spi_miso = 26,
  spi_mosi = 27,
  spi_ss = 23
};
StaticJsonDocument<1524> doc;
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS 33
#define TFT_RST 14 // you can also connect this to the Arduino reset
// in which case, set this #define pin to -1!
#define TFT_DC 26
int tmpNOTE = 1123;
auto mpuReady = true;
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

#define BUZZER_PIN 27
#define BUZZER_CHANNEL 0

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
String sfpd_r56 = "SFProDisplay-Regular-56";
String sfpd_r84 = "SFProDisplay-Regular-84";

String scores = "", old_score = "";
String homescore = "";
String awayscore = "";
String hometeam = "";
String awayteam = "";
String matchtime = "";
int ihscore = 0; // home score in integer
int iascore = 0; // away score in integer
bool teamcolored = false;
uint32_t homecolor = TFT_WHITE;
uint32_t awaycolor = TFT_WHITE;
String MATCHTIME = "";
const struct site_tc
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

SoftwareSerial serial(18, 19);

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
int dmode = 3;

bool usingLightSensor = false;
bool animation = false, noanim = false;
int ANIMATIONSPEED = 20;
int count10 = 0, count2 = 0;
auto newScore = false;
uint16_t backlight = 128, prevbacklight = 0;
uint16_t screenOrientation = 1, prevscreenOrientation = 4;

int displaylivescore = 0;
void setup(void)
{
  Serial.begin(115200);
  serial.begin(9600);
  tft.init();
  Wire.begin();
  setupMMA();
  delay(20);
  tft.setRotation(mmaOrientation());
  Serial.print("Hello! ST77xx TFT Test");
  // setupMPU();
  pinMode(25, OUTPUT);
  pinMode(16, OUTPUT);
  tb_display_init(mmaOrientation());
  // tft.setRotation(1);
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }
  NEO.begin();
  NEO.show();
  // NEO.setPixelColor(0, 0);
  NEO.setPixelColor(0, NEO.Color(170, 0, 0));
  // NEO.show();
  NEO.show();
  setBrightness(128);
  // SDカード初期化
  if (!SD.begin(33))
  {
    tft.println("Card Mount Failed");
    Serial.println("Card Mount Failed");
    // return;
  }
  else
  {
    tft.println("SD Card Mount Success");
    Serial.println("SD Card Mount Success");
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE)
  {
    tft.println("No SD card attached");
    // return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.println("\r\nInitialisation done.");
  // pinMode(PIN_BACKLIGHT, OUTPUT);
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

  // getMpuData();
  // large block of text
  tft.fillScreen(TFT_BLACK);
  // testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", TFT_WHITE);
  testdrawtext("USB serial screen fo pi-radio, baud = 115200\nWaiting for incoming signal...", TFT_WHITE);
  //  testdrawtext(usbbaud, TFT_WHITE);
  //  testdrawtext("USB serial screen fo pi-radio, baud = 115200\nWaiting for incoming signal...", TFT_WHITE);
  delay(1000);

  NEO.setPixelColor(0, 0);
  NEO.show();
  digitalWrite(25, LOW);
  tft.fillScreen(TFT_BLACK);
  testdrawtext("waiting for incoming data", COLOR_MEDIUM[random(10)]);

  ledcSetup(BUZZER_CHANNEL, 1000, 8);        // Configure PWM
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL); // Attach the pin to the PWM channel
  ledcSetup(1, 1000, 8);                     // Configure PWM for backlight
  ledcAttachPin(PIN_BACKLIGHT, 1);           // Attach the pin to the PWM channel for backlight
  startUpMelody2();
  // noTone(BUZZER_PIN);
} // void setup
void startUpMelody2()
{
  int melody[] = {2093, 2637, 3136};
  int noteDurations[] = {200, 200, 200};
  for (int thisNote = 0; thisNote < 3; thisNote++)
  {
    // int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDurations[thisNote]);
    // digitalWrite(8, LOW);
    // delay(noteDuration * 1.30);
    // noTone(BUZZER_PIN);
    // digitalWrite(8, HIGH);
    // delay(50);
  }
  noTone(BUZZER_PIN);
}
void setupMMA()
{

  if (accel.begin() == false)
  {
    Serial.println("Not Connected. Please check connections and read the hookup guide.");
    mpuReady = false;
    // while (1);
    return;
  }
}
// void setupMPU()
// {
//   // Try to initialize!
//   if (!mpu.begin())
//   {
//     Serial.println("Failed to find MPU6050 chip");
//     mpuReady = false;
//     return;
//     // while (1)
//     // {
//     //   delay(10);
//     // }
//   }
//   Serial.println("MPU6050 Found!");

//   mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//   Serial.print("Accelerometer range set to: ");
//   switch (mpu.getAccelerometerRange())
//   {
//   case MPU6050_RANGE_2_G:
//     Serial.println("+-2G");
//     break;
//   case MPU6050_RANGE_4_G:
//     Serial.println("+-4G");
//     break;
//   case MPU6050_RANGE_8_G:
//     Serial.println("+-8G");
//     break;
//   case MPU6050_RANGE_16_G:
//     Serial.println("+-16G");
//     break;
//   }
//   mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//   Serial.print("Gyro range set to: ");
//   switch (mpu.getGyroRange())
//   {
//   case MPU6050_RANGE_250_DEG:
//     Serial.println("+- 250 deg/s");
//     break;
//   case MPU6050_RANGE_500_DEG:
//     Serial.println("+- 500 deg/s");
//     break;
//   case MPU6050_RANGE_1000_DEG:
//     Serial.println("+- 1000 deg/s");
//     break;
//   case MPU6050_RANGE_2000_DEG:
//     Serial.println("+- 2000 deg/s");
//     break;
//   }

//   mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
//   Serial.print("Filter bandwidth set to: ");
//   switch (mpu.getFilterBandwidth())
//   {
//   case MPU6050_BAND_260_HZ:
//     Serial.println("260 Hz");
//     break;
//   case MPU6050_BAND_184_HZ:
//     Serial.println("184 Hz");
//     break;
//   case MPU6050_BAND_94_HZ:
//     Serial.println("94 Hz");
//     break;
//   case MPU6050_BAND_44_HZ:
//     Serial.println("44 Hz");
//     break;
//   case MPU6050_BAND_21_HZ:
//     Serial.println("21 Hz");
//     break;
//   case MPU6050_BAND_10_HZ:
//     Serial.println("10 Hz");
//     break;
//   case MPU6050_BAND_5_HZ:
//     Serial.println("5 Hz");
//     break;
//   }

//   Serial.println("");
//   mpuReady = true;
//   // delay(100);
// }
long prevmill = 0;
String oldsdata;
int tryrequest = 0;
bool foundRadio = false;
String data;
char c;
int toScreenSleep = 0;
auto isSleep = false;
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
    if (isSleep)
    {
      setBrightness(backlight);
      isSleep = false;
    }
    if (data.startsWith("#"))
      proccesCMD(data);
    else if (data.startsWith("$"))
      proccesJsonData(data);
    else if (data.startsWith("*"))
      proccesLiveScore(data);
    else
      proccesData(data);
    data = "";
  }
  beepnblink(); // beepnblink.ino
  if (millis() > prevmill + 1000)
  {
    if (count10++ > 10)
    {
      if (dmode == 10)
        displayClock;
      // count10 = 0;
    }
    // getMpuData();
    if (count2++ > 2)
    {
      if (mpuReady)
        getMMAData();
      // getMpuData();
      count2 = 0;
    }
    if (hour() > 18 || hour() < 6)
    {
      // night time, dim the backlight
      if (!isSleep && usingLightSensor)
      {
        backlight = 20;
        if (prevbacklight != backlight)
        {

          setBrightness(backlight);
          prevbacklight = backlight;
        }
      }
      // analogWrite(BACKLIGHT_PIN, 40);
    }
    else
    {
      // day time, brighten the backlight
      if (!isSleep && usingLightSensor)
      {
        backlight = 220;
        if (prevbacklight != backlight)
        {
          setBrightness(backlight);
          prevbacklight = backlight;
        }
      }
      // analogWrite(BACKLIGHT_PIN, 200);
    }
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
      tft.fillScreen(TFT_BLACK);
      setBrightness(0);
      isSleep = true;
      // testdrawtext("waiting for incoming data", COLOR_MEDIUM[random(12)]);
      // printWordWrap("waiting for incoming data", COLOR_MEDIUM[random(12)]);
    }
    prevmill = millis();
  }
} // end loop
// void getMpuData()
// {
//   /* Get new sensor events with the readings */
//   sensors_event_t a, g, temp;
//   mpu.getEvent(&a, &g, &temp);
//   Serial.println("get mpu data");
//   // Print acceleration and gyro concisely

//   // Serial.printf("Acc: X=%.2f Y=%.2f Z=%.2f m/s^2\nGyro: X=%.2f Y=%.2f Z=%.2f rad/s\n",
//   //               a.acceleration.x, a.acceleration.y, a.acceleration.z,
//   //               g.gyro.x, g.gyro.y, g.gyro.z);
//   // Serial.printf("Temp: %.2f degC\n", temp.temperature);

//   // Serial.println("");
//   /* Print the same values to the TFT */
//   // Clear a small area at the top to avoid flicker of whole screen
//   // uint16_t areaH = 120;
//   // tft.fillRect(0, 0, tft.width(), areaH, TFT_BLACK);

//   // Print acceleration, gyro and temperature (2 decimal places)
//   if (dmode == 11)
//   {
//     tft.fillScreen(TFT_BLACK);

//     // Configure text appearance
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextSize(1);
//     tft.setCursor(0, 0);
//     tft.printf("Acc\n X: %.2f Y: %.2f Z: %.2f\n", a.acceleration.x, a.acceleration.y, a.acceleration.z);
//     tft.printf("Gyro\n X: %.2f Y: %.2f Z: %.2f\n", g.gyro.x, g.gyro.y, g.gyro.z);
//     // tft.printf("Temp: %.2f C\n", temp.temperature);
//   }
//   // delay(500);
//   if (a.acceleration.x > 3)
//     tft.setRotation(1);
//   else if (a.acceleration.x < -3)
//     tft.setRotation(3);
// }
void getMMAData()
{
  if (accel.available())
  { // Wait for new data from accelerometer
    // Orientation of board (Right, Left, Down, Up);
    if (accel.isRight() == true)
    {
      Serial.println("Right");
    }
    else if (accel.isLeft() == true)
    {
      Serial.println("Left");
    }
    else if (accel.isUp() == true)
    {
      Serial.println("Up");
      screenOrientation = 1;
      if (prevscreenOrientation != screenOrientation)
      {
        tft.setRotation(1);
        if (dmode == 0)
        {
          tft.fillScreen(TFT_BLACK);
          if (displaylivescore == 0)
            ssgmnt(homescore);
          else if (displaylivescore == 1)
            displayscore(homescore);
          else if (displaylivescore == 2)
            tsgmnt(homescore);
          else if (displaylivescore == 3)
            drawDigitLivescore(homescore);
        }
        prevscreenOrientation = screenOrientation;
      }
    }
    else if (accel.isDown() == true)
    {
      Serial.println("Down");
      screenOrientation = 3;
      if (prevscreenOrientation != screenOrientation)
      {
        tft.setRotation(3);
        if (dmode == 0)
        {
          tft.fillScreen(TFT_BLACK);
          if (displaylivescore == 0)
            ssgmnt(homescore);
          else if (displaylivescore == 1)
            displayscore(homescore);
          else if (displaylivescore == 2)
            tsgmnt(homescore);
          else if (displaylivescore == 3)
            drawDigitLivescore(homescore);
        }
        prevscreenOrientation = screenOrientation;
      }
    }
    else if (accel.isFlat() == true)
    {
      Serial.println("Flat");
    }
  }
}
uint8_t mmaOrientation()
{
  if (accel.isRight() == true)
  {
    return 1;
  }
  else if (accel.isLeft() == true)
  {
    return 2;
  }
  else if (accel.isUp() == true)
  {
    return 3;
  }
  else if (accel.isDown() == true)
  {
    return 4;
  }
  else if (accel.isFlat() == true)
  {
    return 5;
  }
  return 0;
}
void proccesCMD(String data)
{
  data.remove(0, 1); // remove starting #
  if (data.length() > 4)
  {
    Serial.println(data);
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
      data = "";
      return;
    }
    else if (data.startsWith("brightness"))
    {
      int b = data.substring(11).toInt();
      if (b < 10)
        b = 10;
      if (b > 255)
        b = 255;
      backlight = b;
      setBrightness(b);
      if (usingLightSensor)
      {
        Serial.printf("backlight set to %d\n", b);
        usingLightSensor = false; // disable auto backlight when brightness is set manually
      }
      printWordWrap("brightness set to " + String(b), COLOR_MEDIUM[random(12)]);
      data = "";
      return;
    }
    else if (data.startsWith("resetscreen"))
    {
      tft.fillScreen(TFT_BLACK);
      data = "";
      return;
    }
    else if (data.startsWith("aspeed"))
    {
      int idata = data.substring(7).toInt();
      if (idata > 0)
      {
        ANIMATIONSPEED = idata;
        // beep();
      }
      data = "";
    }
    else if (data.startsWith("animation"))
    {
      String sdata = data.substring(10, 11);
      Serial.printf("sdata  : %s \n", sdata.c_str());
      if (sdata == "1")
        animation = true;
      else
        animation = false;
      // char info[40];

      data = "";
      // beep();
      return;
    }
    else if (data.startsWith("setnote"))
    {
      tft.fillScreen(TFT_BLACK);
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
        tft.fillScreen(TFT_BLACK);
        tft.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
        return;
      }
      else
      {
        int dmod = data.substring(6).toInt();
        if (dmod < 3 || dmod == 10 || dmod == 11)
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
    else if (data.startsWith("settime"))
    {
      int h = data.substring(8, 10).toInt();
      int m = data.substring(11, 13).toInt();
      int s = data.substring(14, 16).toInt();
      // beep();
      Serial.println("startblinking");
      setTime(h, m, s, 2, 7, 2025);
      data = "";

      // tft.fillRect(0, 230, 240, 10, TFT_BLACK);
      // tft.setCursor(0, 230);
      // tft.setTextColor(TFT_GREENYELLOW);
      // tft.printf("time set to %02d:%02d:%02d \n", h, m, s);
      // char info[40];
      // sprintf(info, "time set to %02d:%02d:%02d", h, m, s);
      // snackBar(info);
      // printWordWrap("time set to " + String(h) + ":" + String(m) + ":" + String(s), COLOR_MEDIUM[random(12)]);
      // setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(),
      //             timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());
      // prevmill2 = millis();
      return;
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
    // String homescore = data.substring(data.indexOf(">") + 2);
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

        scores = data.substring(data.indexOf(">"));
        homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
        awayscore = scores.substring(scores.indexOf("-") + 1);
        hometeam = data.substring(0, data.indexOf("vs"));
        // awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf("[")); // only awayteam
        awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">")); // include minute progress
        matchtime = awayteam.substring(data.indexOf("\n"));
        ihscore = homescore.toInt();
        iascore = awayscore.toInt();
        MATCHTIME = data.substring(data.indexOf("[") + 1, data.indexOf("]"));

        maxWait = (data.length() > 10) ? data.length() / 2 : 80;

        if (ihscore > 9 || iascore > 9)
        {
          if (scores != old_score)
          {
            tft.fillScreen(TFT_BLACK);
            old_score = scores;
          }
          int cx = 0, cy = 0;
          printtextcs(cx, cy, hometeam, COLOR_MEDIUM[random(12)], 16);
          cx = 0, cy = 110;
          printtextcs(cx, cy, awayteam, COLOR_MEDIUM[random(12)], 16);
          // cx = 120 - ((50 * MATCHTIME.length()) / 2), cy = 160;
          // printtextcs(cx, cy, MATCHTIME, COLOR_MEDIUM[random(12)], 84);

          // cx = 120 - ((34 * scores.length()) / 2), cy = 40;
          cx = 0, cy = 20;
          printtextcs(cx, cy, scores.substring(2), COLOR_MEDIUM[random(12)], 56);
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

void proccesJsonData(String data)
{
  Serial.println("its json data");
  data.replace("$", "");
  if (data != olddata)
  {
    olddata = data;
    displaylivescore = random(4);
    tft.fillScreen(TFT_BLACK);
  }
  DeserializationError error = deserializeJson(doc, data);
  if (error)
  {
    return;
  }
  if (doc.containsKey("dmode"))
  {
    dmode = doc["dmode"].as<int>();
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
  if (doc.containsKey("sleep"))
  {
    tft.fillScreen(TFT_BLACK);
    setBrightness(0);
  }
  if (doc.containsKey("rotation"))
  {
    int sr = doc["rotation"];
    tft.setRotation(sr);
  }
  if (doc.containsKey("aspeed"))
  {
    if (doc["aspeed"] > 0)
      ANIMATIONSPEED = doc["aspeed"];
    ANIMATIONSPEED = constrain(ANIMATIONSPEED, 4, 100);
    // else
    // ANIMATIONSPEED = doc["anspeed"];
  }
  bool displals = false;
  if (doc.containsKey("Hometeam"))
  {
    displals = true;
    hometeam = doc["Hometeam"].as<String>();
  }
  if (doc.containsKey("Awayteam"))
  {
    awayteam = doc["Awayteam"].as<String>();
  }
  if (doc.containsKey("HomeScore"))
  {
    homescore = doc["HomeScore"].as<String>();
  }
  if (doc.containsKey("AwayScore"))
  {
    awayscore = doc["AwayScore"].as<String>();
  }
  if (doc.containsKey("Score"))
  {
    scores = doc["Score"].as<String>();
  }
  if (doc.containsKey("Matchtime"))
  {
    matchtime = doc["Matchtime"].as<String>();
    int cx = 0, cy = 116;
    tft.fillRect(0, cy, tft.width(), 12, TFT_BLACK);
    printtextcs(cx, cy, matchtime, COLOR_MEDIUM[random(12)], 16);
  }
  if (doc.containsKey("newScore"))
  {
    newScore = doc["newScore"].as<bool>();
  }
  if (doc.containsKey("MatchState"))
  {
    int matchState = doc["MatchState"].as<int>();
    if (matchState == 1)
    {
      tone(BUZZER_PIN, tmpNOTE, 800);
    }
    else if (matchState == 2)
    {
      tone(BUZZER_PIN, tmpNOTE, 100);
      noTone(BUZZER_PIN);
      delay(200);
      tone(BUZZER_PIN, tmpNOTE, 600);
    }
  }
  if (doc.containsKey("drawmap"))
  {
    if (doc["drawmap"] == true)
    {
      if (doc["indexmap"] == 0)
      {
        tft.drawBitmap(0, 0, indonesian_western, 160, 128, TFT_WHITE);
      }
      else if (doc["indexmap"] == 1)
      {
        tft.drawBitmap(0, 0, indonesian_eastern, 160, 128, TFT_WHITE);
      }
      int lat = doc["latitude"].as<int>();
      int lon = doc["longitude"].as<int>();
      Serial.printf("lat : %d, lon : %d\n", lat, lon);
      tft.fillCircle(lat, lon, 5, TFT_RED);

      printtextcs(2, 12, "Lokasi Gempa", COLOR_MEDIUM[random(12)], 16);
    }
  }
  if (displals)
    if (displaylivescore == 0)
      ssgmnt(homescore);
    else if (displaylivescore == 1)
      displayscore(homescore);
    else if (displaylivescore == 2)
      tsgmnt(homescore);
    else if (displaylivescore == 3)
      drawDigitLivescore(homescore);
}

void proccesLiveScore(String data)
{
  Serial.println("its json ls data");
  data.replace("*", "");
  DeserializationError error = deserializeJson(doc, data);
  if (error)
  {
    return;
  }
  bool displals = false;
  if (doc.containsKey("team_colored"))
  {
    teamcolored = doc["team_colored"].as<bool>();
    if (teamcolored)
    {
      homecolor = doc["home"]["color"].as<uint32_t>();
      // tft.fillRect(0, 0, 10, 40, homecolor);
      Serial.printf("home color : %lu\n", homecolor);
      Serial.println("home color : " + String(homecolor, HEX));
      awaycolor = doc["away"]["color"].as<uint32_t>();
      // tft.fillRect(tft.width() - 10, 0, 10, 40, awaycolor);
    }
  }

  if (doc.containsKey("home"))
  {
    displals = true;
    hometeam = doc["home"]["name"].as<String>();
    hometeam.replace("-", " ");
    homescore = doc["home"]["score"].as<String>();
    // if (teamcolored)
    // {
    //   String colorhex = doc["home"]["color"].as<String>();
    //   Serial.println("home color : " + colorhex);
    //   uint32_t colorval = strtoul(&colorhex[1], NULL, 16);
    //   Serial.printf("home color val : %lu\n", colorval);
    //   setNeopixelColor(0, colorval);
    // }
  }
  if (doc.containsKey("away"))
  {
    awayteam = doc["away"]["name"].as<String>();
    awayteam.replace("-", " ");
    awayscore = doc["away"]["score"].as<String>();
  }
  if (doc.containsKey("Score"))
  {
    scores = doc["Score"].as<String>();
  }
  if (doc.containsKey("Matchtime"))
  {
    matchtime = doc["Matchtime"].as<String>();
    int cx = 0, cy = 116;
    tft.fillRect(0, cy, tft.width(), 12, TFT_BLACK);
    printtextcs(cx, cy, matchtime, COLOR_MEDIUM[random(12)], 16);
  }
  if (doc.containsKey("newScore"))
  {
    newScore = doc["newScore"].as<bool>();
    if (newScore)
    {
      wristleBeep(2, 4);
    }
  }
  if (doc.containsKey("MatchState"))
  {
    int matchState = doc["MatchState"].as<int>();
    if (matchState == 1)
    {
      // tone(BUZZER_PIN, tmpNOTE, 800);
      wristleBeep(0, 0);
    }
    else if (matchState == 2)
    {
      wristleBeep(1, 0);
      // tone(BUZZER_PIN, tmpNOTE, 100);
      // noTone(BUZZER_PIN);
      // delay(200);
      // tone(BUZZER_PIN, tmpNOTE, 600);
    }
  }

  if (displals)
    if (displaylivescore == 0)
      ssgmnt(homescore);
    else if (displaylivescore == 1)
      displayscore(homescore);
    else if (displaylivescore == 2)
      tsgmnt(homescore);
    else if (displaylivescore == 3)
      drawDigitLivescore(homescore);
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
void printtextcs(int x, int y,
                 String text, uint16_t color, uint8_t fsize)
{

  tft.unloadFont();
  delay(25);

  if (fsize == 16)
    tft.loadFont(sfpt_r16);
  else if (fsize == 18)
    tft.loadFont(sfpt_r18);
  else if (fsize == 20)
    tft.loadFont(sfpd_r20);
  else if (fsize == 24)
    tft.loadFont(sfpd_r24);
  else if (fsize == 28)
    tft.loadFont(sfpd_r28);
  else if (fsize == 56)
    tft.loadFont(sfpd_r56);
  else if (fsize == 84)
    tft.loadFont(sfpd_r84);
  else
    tft.loadFont(sfpt_r16);

  tft.setCursor(x, y);
  tft.setTextWrap(true);
  tft.setTextColor(color, TFT_BLACK);
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
      tft.setCursor(0, 118);
      printWordWrap("note set to " + notes[i].name, COLOR_MEDIUM[random(12)]);
      // snackBar("note set to " + notes[i].name);

      break;
    }
  }
}
void wristleBeep(int mode, int beepvalue)
{
  if (mode == 0) // long beep
  {
    nblinking = 1;
    blinking = true;
    startblink = 2;
    endblink = 8;
    angka = 0;
    countblink = 0;
    Serial.println("start beeping");
    prevmill2 = millis();
  }
  else if (mode == 1) // end match
  {
    nblinking = 2;
    blinking = true;
    endmatch = 1;
    startblink = 9;
    endblink = 10;
    angka = 7;
    countblink = 0;
    Serial.println("start beeping");
    prevmill2 = millis();
  }
  else if (mode == 2) // n beep
  {
    nblinking = beepvalue;
    blinking = true;
    startblink = 5;
    endblink = 6;
    countblink = 0;
    angka = startblink - 2;
    Serial.println("startblinking");
    prevmill2 = millis();
  }
}

void setBrightness(int b)
{
  ledcWrite(1, b);
  // analogWrite(PIN_BACKLIGHT, b);
}