// purpose: display for all display
// board m5stickc
// methode: usb serial / soft serial
void setup()
{
  // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
}
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <M5StickC.h>
#include <ArduinoOTA.h>
#include "tb_display.h"
bool newline = false;
// #include  <ESP8266WiFi.h>
bool screenOR = true;
const int BUTTON_HOME = GPIO_NUM_37;
const int BUTTON_PIN = GPIO_NUM_39;
String oldstr;
int runmode = 0, titlex = 0;
long previousMillis = 0;
int interval = 40;
const char *ssid = "RMN20";
const char *password = "air46664";
String rssmsg[300];
String siteonread;

const byte numChars = 60;
char receivedChars[numChars];
bool newData = false;

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
void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  serial.begin(9600);
  M5.begin();
  Serial.println("starting piradio reader");
  M5.IMU.Init();
  pinMode(BUTTON_HOME, INPUT | PULLUP);
  pinMode(BUTTON_PIN, INPUT | PULLUP);
  //  attachInterrupt(digitalPinToInterrupt(BUTTON_HOME), resett, RISING);
  // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), resett, RISING);

  // M5.Lcd.SMOOTH_FONT();
  M5.Axp.ScreenBreath(50); // 0-100

  // M5.Lcd.setRotation(1);
  tb_display_init(1);
  M5.Lcd.fillScreen(BLACK);
  cekIMU();
  M5.Lcd.setCursor(0, 5);
  M5.Lcd.setTextSize(1);
  // M5.Lcd.setTextColor(0x00aadd00, 0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.println("rss reader");
  pinMode(BUTTON_HOME, INPUT | PULLUP);
  pinMode(BUTTON_PIN, INPUT | PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_HOME), resett, RISING);
}

void resett()
{
  ESP.restart();
}
String sdata;
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
  //       // tft.fillScreen(TFT_BLACK);
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
  while (serial.available() > 0)
  {
    delay(10);
    c = serial.read();
    data += c;
  }
  if (data.length() > 0)
  {
    Serial.println(data);
    tb_display_print_String(data.c_str(), 20);
    data = "";
  }

  rwnm();
  // showNewData();
}
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