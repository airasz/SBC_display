int tenSegment[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// Segment activation patterns for numbers 0-9 (A-G segments)
const byte tdigitPatterns[11] = {
    0b1110110111, // 0
    0b0011100011, // 1
    0b0111111110, // 2
    0b0011111111, // 3
    0b1001101011, // 4
    0b1011111101, // 5---
    0b1111111101, // 6
    0b0011110011, // 7
    0b1111111111, // 8---
    0b1011111111, // 9
    0b0001001000, // 10 as "-" sign
};
uint32_t TenSegmenPaternts[11][10] = {
    {1, 1, 1, 0, 1, 1, 0, 1, 1, 1}, // 0
    {0, 0, 1, 1, 1, 0, 0, 0, 1, 1}, // 1
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0}, // 2
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1}, // 3
    {1, 0, 0, 1, 1, 0, 1, 0, 1, 1}, // 4
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1}, // 5
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, // 6
    {0, 0, 1, 1, 1, 1, 0, 0, 1, 1}, // 7
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
    {0, 0, 0, 1, 0, 0, 1, 0, 0, 0}  // 10 as "-" sign
};

// Segment positions (relative to x,y)
const int tsegments[7][4] = {
    {1, 0, 3, 0}, // A (top)
    {4, 1, 4, 3}, // B (upper right)
    {4, 4, 4, 6}, // C (lower right)
    {1, 7, 3, 7}, // D (bottom)
    {0, 4, 0, 6}, // E (lower left)
    {0, 1, 0, 3}, // F (upper left)
    {1, 4, 3, 4}  // G (middle)
};

uint32_t triangularpos[10][3][2] = {
    {{0, 0}, {0, 0}, {0, 0}},  // 0
    {{0, 0}, {0, 0}, {0, 0}},  // 1
    {{0, 0}, {0, 0}, {0, 0}},  // 2
    {{0, 0}, {0, 0}, {0, 0}},  // 3
    {{0, 0}, {0, 0}, {0, 0}},  // 4
    {{0, 0}, {0, 0}, {0, 0}},  // 5
    {{0, 0}, {0, 0}, {0, 0}},  // 6
    {{0, 0}, {0, 0}, {0, 0}},  // 7
    {{0, 0}, {0, 0}, {0, 0}},  // 8
    {{0, 0}, {0, 0}, {0, 0}}}; // 9

/* //
//
//    a
//    /\
//   /| \
//  / |  \
// /  |   \
//c --d--- b
//
*/
void fillingTriangular(int x, int y, int size, int gap)
{
    float btp = sqrt((size * size) + ((size / 2) * (size / 2))); // a to d
    float btpg = sqrt((gap * gap) + ((gap / 2) * (gap / 2)));    // gap to gap
    for (int i = 0; i < 10; i++)
    {
        if (i == 0)
        {
            triangularpos[i][0][0] = x;              // x
            triangularpos[i][0][1] = y;              // y
            triangularpos[i][1][0] = x + int(btp);   // x1
            triangularpos[i][1][1] = y + (size / 2); // y1
            triangularpos[i][2][0] = x;              // x2
            triangularpos[i][2][1] = y + size;       // y2
        }
        else if (i == 1)
        {
            triangularpos[i][0][0] = x;                           // x
            triangularpos[i][0][1] = y + size + gap;              // y
            triangularpos[i][1][0] = x + int(btp);                // x1
            triangularpos[i][1][1] = y + (size / 2) + size + gap; // y1
            triangularpos[i][2][0] = x;                           // x2
            triangularpos[i][2][1] = y + size + size + gap;       // y2
        }
        else if (i == 2)
        {
            triangularpos[i][0][0] = x + int(btp) + int(btpg) + gap;       // x
            triangularpos[i][0][1] = y - (size / 2) - (gap / 2);           // y
            triangularpos[i][1][0] = x + (int(btp) * 2) + int(btpg) + gap; // x1
            triangularpos[i][1][1] = y - (gap / 2);                        // y1
            triangularpos[i][2][0] = x + int(btp) + int(btpg) + gap;       // x2
            triangularpos[i][2][1] = y + (size / 2) - (gap / 2);           // y2
        }
        else if (i == 3)
        {
            triangularpos[i][0][0] = x + int(btp) + int(btpg) + gap;       // x
            triangularpos[i][0][1] = y + (size / 2) + (gap / 2);           // y
            triangularpos[i][1][0] = x + (int(btp) * 2) + int(btpg) + gap; // x1========V
            triangularpos[i][1][1] = y + size + (gap / 2);                 // y1
            triangularpos[i][2][0] = x + int(btp) + int(btpg) + gap;       // x2
            triangularpos[i][2][1] = y + ((size / 2) + (gap / 2)) + size;  // y2
        }
        else if (i == 4)
        {
            triangularpos[i][0][0] = x + int(btp) + int(btpg) + gap;                // x
            triangularpos[i][0][1] = y + size + gap + (size / 2) + (gap / 2);       // y
            triangularpos[i][1][0] = x + (int(btp) * 2) + int(btpg) + gap;          // x1
            triangularpos[i][1][1] = y + (size * 2) + gap + (gap / 2);              // y1
            triangularpos[i][2][0] = x + int(btp) + int(btpg) + gap;                // x2
            triangularpos[i][2][1] = y + (size * 2) + gap + (gap / 2 + (size / 2)); // y2
        }
        else if (i == 5)
        {
            triangularpos[i][0][0] = x + int(btpg);                // x
            triangularpos[i][0][1] = y - (gap / 2);                // y
            triangularpos[i][1][0] = x + int(btpg) + int(btp);     // x1
            triangularpos[i][1][1] = y - (size / 2) - (gap / 2);   // y1
            triangularpos[i][2][0] = x + int(btpg) + int(btp);     // x2
            triangularpos[i][2][1] = y + (size / 2) - (gap / gap); // y2
        }
        else if (i == 6)
        {
            triangularpos[i][0][0] = x + int(btpg);                       // x
            triangularpos[i][0][1] = y + size + (gap / 2);                // y
            triangularpos[i][1][0] = x + int(btpg) + int(btp);            // x1
            triangularpos[i][1][1] = y + (size / 2) + (gap / 2);          // y1
            triangularpos[i][2][0] = x + int(btpg) + int(btp);            // x2
            triangularpos[i][2][1] = y + ((size / 2) + (gap / 2)) + size; // y2
        }
        else if (i == 7)
        {
            triangularpos[i][0][0] = x + int(btpg);                                 // x
            triangularpos[i][0][1] = y + (size * 2) + gap + (gap / 2);              // y
            triangularpos[i][1][0] = x + int(btpg) + int(btp);                      // x1
            triangularpos[i][1][1] = y + size + gap + (size / 2) + (gap / 2);       // y1
            triangularpos[i][2][0] = x + int(btpg) + int(btp);                      // x2
            triangularpos[i][2][1] = y + (size * 2) + gap + (gap / 2 + (size / 2)); // y2
        }
        else if (i == 8)
        {
            triangularpos[i][0][0] = x + int(btp) + (int(btpg * 2)) + gap;       // x
            triangularpos[i][0][1] = y + (size / 2);                             // y
            triangularpos[i][1][0] = x + (int(btpg) * 2) + (int(btp) * 2) + gap; // x1
            triangularpos[i][1][1] = y;                                          // y1
            triangularpos[i][2][0] = x + (int(btpg) * 2) + (int(btp) * 2) + gap; // x2
            triangularpos[i][2][1] = y + size;                                   // y2
        }
        else if (i == 9)
        {
            triangularpos[i][0][0] = x + int(btp) + (int(btpg * 2)) + gap;       // x
            triangularpos[i][0][1] = y + (size / 2) + size + gap;                // y
            triangularpos[i][1][0] = x + (int(btpg) * 2) + (int(btp) * 2) + gap; // x1
            triangularpos[i][1][1] = y + size + gap;                             // y1
            triangularpos[i][2][0] = x + (int(btpg) * 2) + (int(btp) * 2) + gap; // x2
            triangularpos[i][2][1] = y + size + size + gap;                      // y2
        }
    }
}
#define SS_DISABLE 0x18c3
void drawTSegment(int x, int y, int segment, bool state, int size, uint32_t col)
{
    int x0, y0, x1, y1, x2, y2;

    x0 = (x + triangularpos[segment][0][0]);
    y0 = (y + triangularpos[segment][0][1]);
    x1 = (x + triangularpos[segment][1][0]);
    y1 = (y + triangularpos[segment][1][1]);
    x2 = (x + triangularpos[segment][2][0]);
    y2 = (y + triangularpos[segment][2][1]);

    if (state)
    {
        tft.fillTriangle(x0, y0, x1, y1, x2, y2, col);
    }
    else
    {
        tft.drawTriangle(x0, y0, x1, y1, x2, y2, SS_DISABLE);
    }
}

void drawT7Segment(int x, int y, int number, int size, uint32_t col)
{
    fillingTriangular(x, y, size, 4);
    byte pattern = tdigitPatterns[number];
    // for (int i = 0; i < 7; i++)
    // {
    //     // bool state = bitRead(pattern, 6 - i); // Bits are A to G (MSB to LSB)
    //     bool state = bitRead(pattern, 6 - sevensegment[i]); // Bits are A to G (MSB to LSB)
    //     drawSegment(x, y, sevensegment[i], true, size, SS_DISABLE);
    //     delay(50);
    // }
    Serial.print("Pattern: ");
    Serial.println(pattern, BIN);
    for (int i = 0; i < 10; i++)
    {

        // bool state = bitRead(pattern, 9 - tenSegment[i]); // Bits are A to G (MSB to LSB)
        bool state = TenSegmenPaternts[number][i];
        Serial.print(state);
        drawTSegment(x, y, tenSegment[i], state, size, col);
        delay(75);
    }
}
// String old_score;
void tsgmnt(String score)
{
    if (score != old_score)
    {
        String scores = data.substring(data.indexOf(">"));
        String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
        String awayscore = scores.substring(scores.indexOf("-") + 1);
        String hometeam = data.substring(0, data.indexOf("vs"));
        String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));
        String matchtime = awayteam.substring(data.indexOf("\n"));

        tft.fillScreen(TFT_BLACK);
        cx = 0, cy = 0;
        drawtext(hometeam, COLOR_MEDIUM[random(10)]);
        // tft.setCursor(0, 82);
        // tft.print(awayteam);
        cx = 0, cy = 112;
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);
        int number = homescore.toInt();
        int ascr = awayscore.toInt();
        int sz = 10; // segment size
        int x = 10, y = 30;
        int sssize = sizeof(sevensegment) / sizeof(sevensegment[0]);
        // shuffleArray(sevensegment, sssize);
        drawT7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        old_score = score;
    }
    else
    {
        String scores = data.substring(data.indexOf(">"));
        String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
        String awayscore = scores.substring(scores.indexOf("-") + 1);
        String hometeam = data.substring(0, data.indexOf("vs"));
        String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));

        // tft.fillScreen(TFT_BLACK);
        cx = 0, cy = 0;
        drawtext(hometeam, TFT_BLACK);
        drawtext(hometeam, COLOR_MEDIUM[random(10)]);
        // tft.setCursor(0, 82);
        // tft.print(awayteam);
        cx = 0, cy = 112;
        drawtext(awayteam, TFT_BLACK);
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);
        int number = homescore.toInt();
        int ascr = awayscore.toInt();
        int sz = 10; // segment size
        int x = 10, y = 30;

        int sssize = sizeof(sevensegment) / sizeof(sevensegment[0]);

        drawT7Segment(x, y, number, sz, TFT_BLACK);                // Position at (20,20) with size 10
        drawT7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // draw7Segment(x, y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 5), y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 10), y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10

        // drawDigit(60, 10, 10, number, COLOR_MEDIUM[random(12)]);
    }
}
void testDTSegment(int number)
{

    // drawT7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
    drawT7Segment(10, 10, number, 16, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
}