// board m5stickc
// methode: usb serial / soft serial
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <M5StickC.h>
#include <ArduinoOTA.h>
#include "tb_display.h"

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
  if (data.length() > 4)
  {
    if (data.startsWith("blink"))
    {
      nblinking = data.substring(5).toInt();
      blinking = true;
      angka = 7;
      // Serial.println("startblinking");
      Serial.printf("start blinking  : %d times\n", nblinking);
    }
    else if (data.startsWith("dmode"))
    {
      int dmod = data.substring(5).toInt();
      if (dmod < 3)
        dmode = dmod;
      // Serial.println("startblinking");
      M5.Lcd.setCursor(0, 0);
      // M5.Lcd.setTextSize(2);
      Serial.printf("change display mode to : %d \n", dmode);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.printf("dmode=%d\n0 livescore\n1 typing mode\n2 statis mode", dmode);
      // M5.Lcd.setTextSize(1);
      data = "";
    }
    Serial.println(data);
    String homescore = data.substring(data.indexOf(">") + 2);
    // displayinfo(homescore);
    if (data.length() > 4)
      if (dmode == 0)
      {
        displayscore(homescore);
      }
      else if (dmode == 1)
      {
        data = data + "%";
        tb_display_print_String(data.c_str(), 20);
      }
      else if (dmode == 2)
      {
        displayinfo(data);
      }

    data = "";
  }

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

void displayscore(String score)
{

  M5.Lcd.fillScreen(BLACK);
  String scores = data.substring(data.indexOf(">"));
  String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
  String awayscore = scores.substring(scores.indexOf("-") + 1);
  String hometeam = data.substring(0, data.indexOf("vs"));
  String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));

  score.replace("-", "");
  score.replace(" ", "");
  int clock_style = random(2);
  int count = 0;
  count = (homescore.toInt() * 100) + awayscore.toInt();
  // count = score.toInt();
  Serial.printf("count  : %d \n", count);
  int x_start = 25;
  int x_delta = 6;
  int r = 5;
  int rd = random(2);
  for (int n = 0; n < 10; n++)
  {
    // canvas.fillCircle(x_start + x_delta * n, 4, r, COLORS_LIGHT[n]);
  }

  int y2 = 11;
  for (int n = 0; n < 10; n++)
  {
    // canvas.fillCircle(x_start + x_delta * n, y2, r, COLORS_DARK[n]);
  }
  // int testgigit=DIGITS[0][0][4];
  // Serial.printf("testgigit %d\n",testgigit);
  int y_start = 16;
  for (int pos = 1; pos < 4; pos++)
  {
    uint8_t curr_digit = 0;
    if (pos == 0)
    {
      curr_digit = count / 1000;
      if (curr_digit == 0)
        curr_digit = 11; // make it blank. see on res.h
    }
    else if (pos == 1)
    {
      curr_digit = count / 100 % 10;
    }
    else if (pos == 2)
    {
      // curr_digit = count / 10 % 10;
      curr_digit = 10; // make it (-). see on res.h
    }
    else if (pos == 3)
    {
      curr_digit = count % 10;
    }
    int rnd = random(10);
    for (int row = 0; row < 7; row++)
    {
      for (int col = 0; col < 5; col++)
      {
        uint32_t color = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[curr_digit] : COLORS_DARK[curr_digit];
        uint32_t colorrnd = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[rnd] : COLORS_DARK[rnd];
        if (DIGITS[curr_digit][row][col] == 1)
        {
          if (clock_style == 0)
          {
            M5.Lcd.fillCircle(x_start + col * 7 - 1 + random(4), y_start + row * 7 - 1 + random(4), r, colorrnd);
            // M5.Lcd.fillCircle(x_start + col * 7 -1+random(4), y_start + row * 7-1+random(4), 3, BLACK);
            // M5.Lcd.fillCircle(x_start + col * 7 -1+random(4), y_start + row * 7-1+random(4), r, colorrnd);
          }
          else if (clock_style == 1)
          {
            M5.Lcd.fillRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
          }
        }
        else
        {
          if (clock_style == 1)
          {
            M5.Lcd.drawRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
          }
          else if (clock_style == 0)
          {
            // M5.Lcd.drawCircle(x_start + col * 7, y_start + row * 7, r, colorrnd);
          }
        }
        // M5.Lcd.fillRect (x_start + col * 7, y_start + row * 7, r, colorrnd);
        // RGB565 = (((RGB888&0xf80000)>>8) + ((RGB888&0xfc00)>>5) + ((RGB888&0xf8)>>3));
      }
    }
    x_start += 39;
  }

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(hometeam);
  M5.Lcd.setCursor(0, 72);
  M5.Lcd.println(awayteam);
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