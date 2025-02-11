

// Segment activation patterns for numbers 0-9 (A-G segments)
const byte digitPatterns[11] = {
    0b1111110, // 0
    0b0110000, // 1
    0b1101101, // 2
    0b1111001, // 3
    0b0110011, // 4
    0b1011011, // 5
    0b1011111, // 6
    0b1110000, // 7
    0b1111111, // 8
    0b1111011, // 9
    0b0000001, // 10 as "-" sign
};

// Segment positions (relative to x,y)
const int segments[7][4] = {
    {1, 0, 3, 0}, // A (top)
    {4, 1, 4, 3}, // B (upper right)
    {4, 4, 4, 6}, // C (lower right)
    {1, 7, 3, 7}, // D (bottom)
    {0, 4, 0, 6}, // E (lower left)
    {0, 1, 0, 3}, // F (upper left)
    {1, 4, 3, 4}  // G (middle)
};

// {1, 0, 3, 0}, // A (top)
// {4, 1, 4, 3}, // B (upper right)
// {4, 4, 4, 6}, // C (lower right)
// {1, 7, 3, 7}, // D (bottom)
// {0, 4, 0, 6}, // E (lower left)
// {0, 1, 0, 3}, // F (upper left)
// {1, 3, 3, 3}  // G (middle)
#define SS_DISABLE 0x18c3
void drawSegment(int x, int y, int segment, bool state, int size, uint32_t col)
{
    int x0, y0, x1, y1;
    if (segment == 0)
    {
        x0 = (x + segments[segment][0] * size);
        y0 = (y + segments[segment][1] * size) + (size / 2);
        x1 = (x + segments[segment][2] * size);
        y1 = (y + segments[segment][3] * size) + (size / 2);
    }
    else if (segment == 3)
    {
        x0 = (x + segments[segment][0] * size);
        y0 = (y + segments[segment][1] * size) - (size / 2);
        x1 = (x + segments[segment][2] * size);
        y1 = (y + segments[segment][3] * size) - (size / 2);
    }
    else if (segment == 6)
    {
        x0 = (x + segments[segment][0] * size);
        y0 = (y + segments[segment][1] * size) - (size / 2);
        x1 = (x + segments[segment][2] * size);
        y1 = (y + segments[segment][3] * size) - (size / 2);
    }
    else
    {
        x0 = (x + segments[segment][0] * size);
        y0 = y + segments[segment][1] * size;
        x1 = (x + segments[segment][2] * size);
        y1 = y + segments[segment][3] * size;
    }

    // tft.drawLine(x0, y0, x1, y1, state ? TFT_RED : TFT_BLACK);
    if (x0 == x1)
    { // Vertical segment
        tft.drawWideLine(x0, y0, x1, y1, 10, state ? col : SS_DISABLE, TFT_BLACK);
        // tft.drawWideLine(x0 - 1, y0, x1 - 1, y1, 10, state ? TFT_RED : TFT_BLACK, TFT_BLACK);
        // tft.drawWideLine(x0 + 1, y0, x1 + 1, y1, 10, state ? TFT_RED : TFT_BLACK, TFT_BLACK);
        // tft.drawLine(x0 - 1, y0, x1 - 1, y1, state ? TFT_RED : TFT_BLACK);
        // tft.drawLine(x0 + 1, y0, x1 + 1, y1, state ? TFT_RED : TFT_BLACK);
    }
    else
    { // Horizontal segment
        tft.drawWideLine(x0, y0, x1, y1, 10, state ? col : SS_DISABLE, TFT_BLACK);
        // tft.drawWideLine(x0, y0 - 1, x1, y1 - 1, 10, state ? TFT_RED : TFT_BLACK, TFT_BLACK);
        // tft.drawWideLine(x0, y0 + 1, x1, y1 + 1, 10, state ? TFT_RED : TFT_BLACK, TFT_BLACK);
        // tft.drawLine(x0, y0 - 1, x1, y1 - 1, state ? TFT_RED : TFT_BLACK);
        // tft.drawLine(x0, y0 + 1, x1, y1 + 1, state ? TFT_RED : TFT_BLACK);
    }
}

void draw7Segment(int x, int y, int number, int size, uint32_t col)
{
    byte pattern = digitPatterns[number];
    for (int i = 0; i < 7; i++)
    {
        bool state = bitRead(pattern, 6 - i); // Bits are A to G (MSB to LSB)
        drawSegment(x, y, i, state, size, col);
    }
}
void ssgmnt(String score)
{
    String scores = data.substring(data.indexOf(">"));
    String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
    String awayscore = scores.substring(scores.indexOf("-") + 1);
    String hometeam = data.substring(0, data.indexOf("vs"));
    String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));

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
    draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]);           // Position at (20,20) with size 10
    draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]);    // Position at (20,20) with size 10
    draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
}
// void setup()
// {
//     tft.init();
//     tft.setRotation(1);
//     tft.fillScreen(TFT_BLACK);
// }

// void loop()
// {
//     for (int num = 0; num < 10; num++)
//     {
//         tft.fillScreen(TFT_BLACK);
//         draw7Segment(20, 20, num, 10); // Position at (20,20) with size 10
//         delay(1000);
//     }
// }