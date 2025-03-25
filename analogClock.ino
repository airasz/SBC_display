

// #include "aWatch.h"

// convert html rgb to rgb565
int rgbToHex(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (long)b;
  uint16_t rgb565 = (((rgb & 0xf80000) >> 8) | ((rgb & 0xfc00) >> 5) | ((rgb & 0xf8) >> 3));
  return rgb565;
}

// void updateClock();
//=========================================================================
int sdeg, mdeg, hdeg, hh, mm, ss;
void analogClock(int v) {

  int wipetracelong, px, dgree, old_rnddot;
  long dmCover;  // daily distance covered
                 //   tnow = ttgo->rtc->getDateTime();
                 //   int nday = day,
                 //       nmonth = month,
                 //       nyear = year, g = 250;
  String const dw[] = { "MGG", "SEN", "SEL", "RBU", "KMS", "JMT", "SBT" };
  //   int dow = ttgo->rtc->getDayOfWeek(nday, nmonth, nyear);
  // Serial.printf(" day of week : %d \n", dow);
  hh = hour();
  mm = minute();
  ss = second();
  //   v -= 6;
  int hh12 = hh >= 12 ? hh - 12 : hh;  // hh += hh > 12 ? (-12) : 0;
  int hq;
  hq = getQuadrant(hh);
  sdeg = ss * 6;                      // 0-59 -> 0-354   Pre-compute
  mdeg = mm * 6 + sdeg * 0.01666667;  // 0-59 -> 0-360 - includes seconds
  hdeg = hh * 30 + mdeg * 0.0833333;  // 0-11 -> 0-360 - inc min and seconds
                                      // face=6> standart analog
                                      //   Serial.println("analog clock ?");
                                      //   Serial.printf("clock v  : %d \n", v);
  if (v == 0) {

    //     Serial.println("analog clock 0");
    // tft.fillScreen(TFT_BLACK); // CLEAR DISPLAY
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // int o_colcircle;
    int o_colcircle = COLOR_MEDIUM[random(10)];
    // int i_colcircle;
    int i_colcircle = COLORS_DARK[random(10)];
    tft.drawSmoothCircle(120, 120, 82, o_colcircle, TFT_BLACK);
    tft.fillSmoothCircle(120, 120, 80, i_colcircle, o_colcircle);
    // 12 LINES
    // rainbow color for every multiple 5  minute line mark
    for (int i = 0; i < 360; i += 30)
      tft.drawLine(posX(80, i), posY(80, i), posX(72, i), posY(72, i), COLOR_MEDIUM[random(10)]);  // DRAW 12 LINES

    // 60 DOTS
    for (int i = 0; i < 360; i += 6) {
      tft.drawPixel(posX(78, i), posY(79, i), COLOR_MEDIUM[random(10)]);  // MINUTE DOT
      if (i % 90 == 0)
        tft.fillCircle(posX(78, i), posY(79, i), 3, TFT_WHITE);  // DOTS
    }
    // middle dot
    tft.fillCircle(120, 120, 3, TFT_RED);
    // Serial.printf("hh=%01d, mm=%02d, ss=%03d\n",hh,mm,ss);

    tft.drawWideLine(posX(50, hdeg), posY(50, hdeg), 120, 120, 6, COLOR_MEDIUM[random(10)], i_colcircle);  // DRAW  HOUR
    tft.drawWideLine(posX(61, mdeg), posY(61, mdeg), 120, 120, 6, COLOR_MEDIUM[random(10)], i_colcircle);  // DRAW  MINUTE
    tft.drawWideLine(posX(71, sdeg), posY(71, sdeg), 120, 120, 6, COLOR_MEDIUM[random(10)], i_colcircle);  // DRAW  MINUTE
    tft.fillSmoothCircle(120, 120, 6, TFT_RED, i_colcircle);
  }
  // cf 13 >chaotic pendulum
  else if (v == 1) {
    tft.fillScreen(TFT_BLACK);            // CLEAR DISPLAY
    hdeg = hh12 * 30 + mdeg * 0.0833333;  // 0-11 -> 0-360 - inc min and seconds
    hq = getQuadrant(hh12);
    hq = (hh12 / 3) + 1;  // get quadrant
    int xx2 = 120;
    int yy2 = 120;
    int xx, yy, mx, my;
    // int xx = posX(50, hdeg, xx2);
    // int yy = posY(50, hdeg, yy2);
    // int mx = posX(80, mdeg, xx);
    // int my = posY(80, mdeg, yy);
    if (mx > 180) {
      xx2 = xx2 - (mx - 180);
    }
    if (mx < 60) {
      xx2 = xx2 + (60 - mx);
    }

    if (my > 180) {
      yy2 = yy2 - (my - 180);
    }
    if (my < 60) {
      yy2 = yy2 + (60 - my);
    }

    xx = posX(50, hdeg, xx2);
    mx = posX(80, mdeg, xx);
    yy = posY(50, hdeg, yy2);
    my = posY(80, mdeg, yy);
    // int hcol, mcol;
    int hcol = COLOR_MEDIUM[random(10)];
    int mcol = COLOR_MEDIUM[random(10)];
    // draw a minute line guide
    for (int i = 0; i < 720; i += 6)
      if (i + 1 > mdeg - 50 && i + 1 < mdeg + 50 || i + 1 > (360 + mdeg) - 50 && i + 1 < (mdeg + 360) + 50)
        tft.drawWideLine(posX(90, i, xx), posY(90, i, yy), posX(90 - ((i % 15 == 0) ? 20 : 10), i, xx), posY(90 - ((i % 15 == 0) ? 20 : 10), i, yy), (i % 15 == 0) ? 2 : 1, ((i % 15 == 0) ? rgbToHex(200, 80, 0) : TFT_YELLOW), TFT_BLACK);  // DRAW  MINUTE

    tft.drawWideLine(mx, my, xx, yy, 6, mcol, TFT_BLACK);  // DRAW  MINUTE
    tft.fillSmoothCircle(xx, yy, 6, TFT_WHITE, TFT_BLACK);
    // draw pie hour line guide
    for (int i = 0; i < 720; i += 30)
      if (i + 1 > hdeg - 60 && i + 1 < hdeg + 60 || i + 1 > (360 + hdeg) - 60 && i + 1 < (hdeg + 360) + 60)
        tft.drawWideLine(posX(60, i, xx2), posY(60, i, yy2), posX(60 - ((i % 90 == 0) ? 20 : 10), i, xx2), posY(60 - ((i % 90 == 0) ? 20 : 10), i, yy2), (i % 90 == 0) ? 3 : 2, TFT_WHITE, TFT_BLACK);  // DRAW  MINUTE

    tft.drawWideLine(xx, yy, xx2, yy2, 6, hcol, TFT_BLACK);  // DRAW  HOUR HAND
    tft.fillSmoothCircle(xx2, yy2, 6, TFT_WHITE, TFT_BLACK);
  }
  //   else if (v == 7)
  //   {
  //     // CF=11
  //     javaneseClock(hh, mm); // clock_face_text.ino

  //     if (enableSS)
  //       screenServer();
  //   }
  //   else if (v == 8)
  //   {
  //     // CF=12
  //     mathFace(hh, mm); // clock_face_text.ino

  //     if (enableSS)
  //       screenServer();
  //   }
}
// tft.drawLine(*(pos(hh, mm) + 0), *(pos(hh, mm) + 1), 120, 120, 0x3186); //

float getCos(int degree) {
  float r = cos((degree - 90) * 0.0174532925);
  return r;
}
float getSin(int degree) {
  float r = sin((degree - 90) * 0.0174532925);
  return r;
}
int posX(int radius, int degree) {
  int r = getCos(degree) * radius + 120;
  return r;
}
int posY(int radius, int degree) {
  int r = getSin(degree) * radius + 120;
  return r;
}

int posX(int radius, int degree, int pivot) {
  int r = getCos(degree) * radius + pivot;
  return r;
}
int posY(int radius, int degree, int pivot) {
  int r = getSin(degree) * radius + pivot;
  return r;
}
int getQuadrant(int hh) {
  // if (hh < 3)
  //   return 1;
  // if (hh >= 3 && hh < 6)
  //   return 2;
  // if (hh >= 6 && hh < 9)
  //   return 3;
  // if (hh >= 9 && hh < 12)
  //   return 4;

  return (hh / 3) + 1;
}
void assignDegree() {
}