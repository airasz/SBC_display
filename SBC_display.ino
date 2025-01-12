// display for all display
//  board: esp32
//   methode: http GET request

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#define FS_NO_GLOBALS
#include <FS.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>
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

WiFiMulti wifimulti;
float p = 3.1415926;

String rssmsg[300];
String siteonread;

int httpGetChar();
const uint32_t COLOR_MEDIUM[] = {TFT_PURPLE, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_GREENYELLOW, TFT_PINK, TFT_ORANGE, TFT_RED, TFT_CYAN, TFT_MAGENTA};
WiFiMulti wifiMulti;
HTTPClient http;
WiFiClient *stream;
String fileName = "NotoSansBold15";
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
void setup(void)
{
  Serial.begin(9600);
  Serial.print("Hello! ST77xx TFT Test");

  tft.init();
  tft.setRotation(1);
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  // tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 0.96" 180x60 TFT
  // tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display

  // Use this initializer (uncomment) if you're using a 1.54" 240x240 TFT
  // tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

  tft.loadFont(fileName);
  Serial.println("Initialized");

  uint16_t time = millis();
  tft.fillScreen(TFT_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(TFT_BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", TFT_WHITE);
  delay(1000);

  tft.fillScreen(TFT_BLACK);
  wifimulti.addAP("airLink", "air46664");
  // wifimulti.addAP("OFFLINE", "terbaik2025");
  // wifimulti.addAP("rumah", "GIGIBOLONG");
  // wifimulti.addAP("RMN20", "air46664");
  // wifimulti.addAP("S.H.I.E.L.D", "air46664");
  // wifimulti.addAP("HGA_4", "RadenSaleh4");
  wifimulti.addAP("ASUS", "air46664");
  tft.println("conneting to wifi");
  // clearDisplayBuffer();
  int counttorst = 0;
  while (wifimulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    if (counttorst++ > 20)
      ESP.restart();
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected to " + WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (trdip() == 1)
  {
    // url = "http://192.168.1.152/radio/oradio.php?cmd=status";
    url = sites[1].url;
  }
  else
  {
    // url = "http://192.168.10.232/radio/oradio.php?cmd=status";
    url = sites[0].url;
  }

  // url = (trdip() == 1) ? "http://192.168.1.152/radio/oradio.php?cmd=status" : "http://192.168.1.152/radio/oradio.php?cmd=status";

  tft.println("conneted");
  // getrss2();
  ArduinoOTA.setHostname("esp32");

  ArduinoOTA
      .onStart([]()
               {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else  // U_SPIFFS
      type = "filesystem";
    Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed"); });
  ArduinoOTA.begin();

  tft.fillScreen(TFT_BLACK);
}
long prevmill = 0;
String oldsdata;
int tryrequest = 0;
bool foundRadio = false;
void loop()
{
  // tft.invertDisplay(true);
  // delay(500);
  // tft.invertDisplay(false);
  // delay(500);

  ArduinoOTA.handle();
  if (millis() > prevmill + 5000)
  {

    if ((wifimulti.run() == WL_CONNECTED))
    {
      Serial.println("wifi connected");
      //      http.begin(sites[i].url);

      // testdrawtext("get data", TFT_WHITE);
      http.begin(url);
      int httpCode = http.GET();
      Serial.printf(" respond code : %d \n", httpCode);
      if (httpCode > 0)
      {
        if (httpCode == HTTP_CODE_OK)
        {
          if (!foundRadio)
            foundRadio = true;
          tryrequest = 0;
          sdata = http.getString();
          sdata.replace("<br>", "");
          sdata.replace("[", "\n[");
          sdata.replace("%", "%\n");
          // sdata = ">" + sdata;
          //          stream = http.getStreamPtr();

          // tb_display_print_String(sdata.c_str(), 100);
          testdrawtext(sdata, COLOR_MEDIUM[random(10)]);
          // siteonread = sites[i];
        }
        else
        {

          tft.fillScreen(TFT_BLACK);
          testdrawtext("bad respon", TFT_WHITE);
        }
      }

      else
      {
        if (!foundRadio)
        {
          tft.fillScreen(TFT_BLACK);
          testdrawtext("no respon \ntry another IP", TFT_WHITE);
          if (trdip() == 1)
          {
            url = sites[3].url;
          }
          else
          {
            url = sites[2].url;
          }
        }
      }
      http.end();

      // cekIMU();
    }
    prevmill = millis();
  }
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

int cx = 0, cy = 15;
void testdrawtext(char *text, uint16_t color)
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(cx, cy);
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
  tft.print(text);
  oldsdata = text;

  // tft.unloadFont();
}
