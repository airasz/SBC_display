
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ESP32Ping.h>
// #include <ESP8266WiFi.h>#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

#include <EEPROM.h>
#include "EEPROM_rw_anything.h"
#include "LittleFS.h"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char *remote_host = "www.google.co.id";

WiFiMulti wifiMulti;
#define EEPROM_SIZE 256
#define CONFIG_REVISION 12349L
typedef struct config_t
{
    long magic_number;
    uint8_t dmode;
    bool animation;
    uint16_t note;
    uint8_t clockFace;
    uint8_t aspeed; // animation speed
} CONFIGGEN;
CONFIGGEN config;
int sevensegment[7] = {0, 1, 2, 3, 4, 5, 6};
int tmpRow[7] = {0, 1, 2, 3, 4, 5, 6};
int tmpRowSize = sizeof(tmpRow) / sizeof(tmpRow[0]);
int tmpCol[5] = {0, 1, 2, 3, 4};
int tmpColSize = sizeof(tmpCol) / sizeof(tmpCol[0]);
int patternSequences[20] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20};
int patternSequencesSize = sizeof(patternSequences) / sizeof(patternSequences[0]);
void shuffleArray(int *array, int size);

uint32_t tmpDigit[7][5][2] = {
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}}};

uint32_t oldDigit_0[7][5][2] = {
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}}};

uint32_t oldDigit_1[7][5][2] = {
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}}};

uint32_t oldDigit_2[7][5][2] = {
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}}};

uint32_t oldDigit_3[7][5][2] = {
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}},
    {{0, 0},
     {0, 0},
     {0, 0},
     {0, 0},
     {0, 0}}};
uint16_t blockColor[4] = {0, 0, 0, 0};
int digitOrder[4] = {0, 1, 2, 3};
int digitOrderSize = sizeof(digitOrder) / sizeof(digitOrder[0]);
uint8_t digitOrderIndex = 0;

String old_score;