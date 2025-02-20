
int sevensegment[7] = {0, 1, 2, 3, 4, 5, 6};
void shuffleArray(int *array, int size)
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = random(0, i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

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
    // for (int i = 0; i < 7; i++)
    // {
    //     // bool state = bitRead(pattern, 6 - i); // Bits are A to G (MSB to LSB)
    //     bool state = bitRead(pattern, 6 - sevensegment[i]); // Bits are A to G (MSB to LSB)
    //     drawSegment(x, y, sevensegment[i], true, size, SS_DISABLE);
    //     delay(50);
    // }
    for (int i = 0; i < 7; i++)
    {
        bool state = bitRead(pattern, 6 - sevensegment[i]); // Bits are A to G (MSB to LSB)
        drawSegment(x, y, sevensegment[i], state, size, col);
        delay(75);
    }
}
String old_score;
void ssgmnt(String score)
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
        shuffleArray(sevensegment, sssize);
        draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
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
        draw7Segment(x, y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 5), y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 10), y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10

        // drawDigit(60, 10, 10, number, COLOR_MEDIUM[random(12)]);
    }
}
int sizee = 10;    // size of each box
byte space = 2;    // space between boxes
int fromTop = 14;  // positon x
int fromLeft = 10; // position y
int Round = 0;

short number = 0;
int digits = 0;

short colors[2] = {TFT_BLACK, TFT_GREEN}; // first colour is color of background , second is color of digit
byte arr[11][5][3] =
    {
        {{1, 1, 1}, // 0
         {1, 0, 1},
         {1, 0, 1},
         {1, 0, 1},
         {1, 1, 1}

        },
        {{0, 1, 0}, // 1
         {0, 1, 0},
         {0, 1, 0},
         {0, 1, 0},
         {0, 1, 0}

        },
        {{1, 1, 1}, // 2
         {0, 0, 1},
         {1, 1, 1},
         {1, 0, 0},
         {1, 1, 1}

        },
        {{1, 1, 1}, // 3
         {0, 0, 1},
         {1, 1, 1},
         {0, 0, 1},
         {1, 1, 1}

        },
        {{1, 0, 1}, // 4
         {1, 0, 1},
         {1, 1, 1},
         {0, 0, 1},
         {0, 0, 1}

        },
        {{1, 1, 1}, // 5
         {1, 0, 0},
         {1, 1, 1},
         {0, 0, 1},
         {1, 1, 1}

        },
        {{1, 1, 1}, // 6
         {1, 0, 0},
         {1, 1, 1},
         {1, 0, 1},
         {1, 1, 1}

        },
        {{1, 1, 1}, // 7
         {0, 0, 1},
         {0, 0, 1},
         {0, 0, 1},
         {0, 0, 1}

        },
        {{1, 1, 1}, // 8
         {1, 0, 1},
         {1, 1, 1},
         {1, 0, 1},
         {1, 1, 1}

        },
        {{1, 1, 1}, // 9
         {1, 0, 1},
         {1, 1, 1},
         {0, 0, 1},
         {1, 1, 1}

        },
        {{0, 0, 0}, // 10 for -
         {0, 0, 0},
         {0, 1, 0},
         {0, 0, 0},
         {0, 0, 0}

        }};

int oldarr[5][3] =
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1}

};
void drawDigit(int x, int y, int fsize, int digit, uint16_t color)
{
    sizee = fsize;
    fromTop = y;
    fromLeft = x;
    colors[1] = color;
    String n = String(digit);
    for (int z = 0; z < n.length(); z++)
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 3; j++)
            {
                String c = n.substring(z, z + 1);
                int b = c.toInt();
                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[b][i][j]]);
                // delay(20);
            }
}

void drawAnimatedDigit(int x, int y, int fsize, int digit, uint16_t color)
{
    sizee = fsize;
    fromTop = y;
    fromLeft = x;
    colors[1] = color;
    String n = String(digit);
    bool animate = true;

    int anp = 32;
    int f = 0;
    int z = 0;

    String c = n.substring(z, z + 1);
    int b = c.toInt();

    for (int m = 1; m < 21; m++)
    {
        anp--;
        if (m % 5 == 0)
            f++;
        int rnda = random(0, anp);
        for (int i = 0; i < 5; i++)
        {
            if (i <= (f - 1))
            {
                for (int j = 0; j < 3; j++)
                {
                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, (digit == 10) ? colors[arr[10][i][j]] : colors[arr[b][i][j]]);
                    // delay(20);
                }
            }
            else
            {
                for (int j = 0; j < 3; j++)
                {
                    int rnd = random(2);
                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[rnd]);
                }
            }
            if (i == 4 && f == 4)
            {
                for (int j = 0; j < 3; j++)
                {
                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, (digit == 10) ? colors[arr[10][i][j]] : colors[arr[b][i][j]]);
                    // delay(20);
                }
            }
        }
        delay(60);
    }
}

void drawAnimatedDigitd(int x, int y, int fsize, int digit, uint16_t color) // pixelate from top to bottom
{
    sizee = fsize;
    fromTop = y;
    fromLeft = x;
    colors[1] = color;
    String n = String(digit);
    bool animate = true;
    for (int z = 0; z < n.length(); z++)
    {
        int anp = 32;
        // for (int m = 0; m < 20; m++)
        // {
        // anp--;

        int rnda = random(0, anp);
        for (int i = 0; i < 5; i++)
        {
            for (int k = 0; k < 5; k++)
            {

                for (int j = 0; j < 3; j++)
                {

                    String c = n.substring(z, z + 1);
                    int b = c.toInt();
                    int rnd = random(2);
                    if (digit != 10)
                    {
                        // if (rnd == arr[b][i][j])
                        // {

                        if (arr[b][i][j] == 1) // in shape
                        {
                            if (rnda < 2)
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[b][i][j]]);
                            else
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[rnd]);
                        }
                        else
                        {
                            if (rnda < 2)
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[b][i][j]]);
                            else
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[rnd]);
                        }
                        // tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[rnd]);
                    }
                    else
                    {

                        if (arr[b][i][j] == 1) // in shape
                        {
                            if (rnda < 2)
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[10][i][j]]);
                            else
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[rnd]);
                        }
                        else
                        {
                            if (rnda < 2)
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[10][i][j]]);
                            else
                                tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[rnd]);
                        }
                    }
                    // tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[random(2)]);
                    // delay(20);
                }
                delay(80);
            }
            for (int j = 0; j < 3; j++)
            {

                String c = n.substring(z, z + 1);
                int b = c.toInt();
                int rnd = random(2);
                if (digit != 10)
                {

                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[b][i][j]]);
                }
                else
                {
                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[10][i][j]]);
                }
                // delay(20);
            }
        }
        //     delay(80);
        // }
        if (digit != 10)
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 3; j++)
                {
                    String c = n.substring(z, z + 1);
                    int b = c.toInt();
                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[b][i][j]]);
                    // delay(20);
                }
        else
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 3; j++)
                {
                    String c = n.substring(z, z + 1);
                    int b = c.toInt();
                    tft.fillRoundRect((z * (sizee * 4)) + fromLeft + (j * sizee) + (j * space), fromTop + (i * sizee) + (i * space), sizee, sizee, Round, colors[arr[10][i][j]]);
                    // delay(20);
                }
    }
}
void drawDigitLivescore(String score)
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
        cx = 0, cy = 98;
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);
        int number = homescore.toInt();
        int ascr = awayscore.toInt();
        int sz = 10; // segment size
        int x = 10, y = 30;
        // int sssize = sizeof(sevensegment) / sizeof(sevensegment[0]);
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
        // shuffleArray(sevensegment, sssize);
        // draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10

        int digit = (number * 100) + ascr;
        drawAnimatedDigit(20, 20, 10, digit, COLOR_MEDIUM[random(12)]);
        // drawAnimatedDigit(20 + (10 * 3) + (2 * 3), 20, 10, 0, COLOR_MEDIUM[random(12)]);
        // drawAnimatedDigit(20 + (10 * 3) + (2 * 3) + (10 * 3) + (2 * 3), 20, 10, ascr, COLOR_MEDIUM[random(12)]);
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
        int digit = (number * 100) + ascr;

        // drawAnimatedDigit(20, 20, 10, number, TFT_BLACK);
        // drawAnimatedDigit(20 + (10 * 3) + (2 * 3), 20, 10, 0, TFT_BLACK);
        // drawAnimatedDigit(20 + (10 * 3) + (2 * 3) + (10 * 3) + (2 * 3), 20, 10, ascr, TFT_BLACK);

        // drawAnimatedDigit(20, 20, 10, digit, COLOR_MEDIUM[random(12)]);
        drawAnimatedDigit(20, 20, 10, number, COLOR_MEDIUM[random(12)]);
        drawAnimatedDigit(20 + (10 * 3) + (2 * 3), 20, 10, 10, COLOR_MEDIUM[random(12)]);
        drawAnimatedDigit(20 + (10 * 3) + (2 * 3) + (10 * 3) + (2 * 3), 20, 10, ascr, COLOR_MEDIUM[random(12)]);
    }
}