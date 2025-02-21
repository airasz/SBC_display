
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
    String scores = data.substring(data.indexOf(">"));
    String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
    String awayscore = scores.substring(scores.indexOf("-") + 1);
    String hometeam = data.substring(0, data.indexOf("vs"));
    String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));
    String matchtime = awayteam.substring(data.indexOf("\n"));

    int number = homescore.toInt();
    int ascr = awayscore.toInt();
    int sz = 10; // segment size
    int x = 10, y = 30;
    if (score != old_score)
    {
        tft.fillScreen(TFT_BLACK);
        cx = 0, cy = 0;
        drawtext(hometeam, COLOR_MEDIUM[random(10)]);
        // tft.setCursor(0, 82);
        // tft.print(awayteam);
        cx = 0, cy = 112;
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);
        int sssize = sizeof(sevensegment) / sizeof(sevensegment[0]);
        old_score = score;
    }
    else
    {
        // tft.fillScreen(TFT_BLACK);
        cx = 0, cy = 0;
        drawtext(hometeam, TFT_BLACK);
        drawtext(hometeam, COLOR_MEDIUM[random(10)]);
        // tft.setCursor(0, 82);
        // tft.print(awayteam);
        cx = 0, cy = 112;
        drawtext(awayteam, TFT_BLACK);
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);

        int sssize = sizeof(sevensegment) / sizeof(sevensegment[0]);
        draw7Segment(x, y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 5), y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10
        shuffleArray(sevensegment, sssize);
        draw7Segment(x + (sz * 10), y, 8, sz, TFT_BLACK); // Position at (20,20) with size 10

        // drawDigit(60, 10, 10, number, COLOR_MEDIUM[random(12)]);
    }
    shuffleArray(sevensegment, sssize);
    draw7Segment(x, y, number, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
    shuffleArray(sevensegment, sssize);
    draw7Segment(x + (sz * 5), y, 10, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
    shuffleArray(sevensegment, sssize);
    draw7Segment(x + (sz * 10), y, ascr, sz, COLOR_MEDIUM[random(12)]); // Position at (20,20) with size 10
}

//=================5x3 digit font=======================
int sizee = 10;    // size of each box
byte space = 2;    // space between boxes
int fromTop = 14;  // positon x
int fromLeft = 10; // position y
int Round = 0;

short number = 0;
int digits = 0;

short colors[2] = {TFT_BLACK, TFT_GREEN}; // first colour is color of background , second is color of digit

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
    String scores = data.substring(data.indexOf(">"));
    String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
    String awayscore = scores.substring(scores.indexOf("-") + 1);
    String hometeam = data.substring(0, data.indexOf("vs"));
    String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));
    String matchtime = awayteam.substring(data.indexOf("\n"));

    int number = homescore.toInt();
    int ascr = awayscore.toInt();
    int sz = 10; // segment size
    int x = 10, y = 30;
    int digit = (number * 100) + ascr;
    if (score != old_score)
    {
        tft.fillScreen(TFT_BLACK);
        cx = 0, cy = 0;
        drawtext(hometeam, COLOR_MEDIUM[random(10)]);
        // tft.setCursor(0, 82);
        // tft.print(awayteam);
        cx = 0, cy = 98;
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);
        drawAnimatedDigit(20, 20, 10, digit, COLOR_MEDIUM[random(12)]);
        // drawAnimatedDigit(20 + (10 * 3) + (2 * 3), 20, 10, 0, COLOR_MEDIUM[random(12)]);
        // drawAnimatedDigit(20 + (10 * 3) + (2 * 3) + (10 * 3) + (2 * 3), 20, 10, ascr, COLOR_MEDIUM[random(12)]);
        old_score = score;
    }
    else
    {
        // tft.fillScreen(TFT_BLACK);
        cx = 0, cy = 0;
        drawtext(hometeam, TFT_BLACK);
        drawtext(hometeam, COLOR_MEDIUM[random(10)]);
        // tft.setCursor(0, 82);
        // tft.print(awayteam);
        cx = 0, cy = 112;
        drawtext(awayteam, TFT_BLACK);
        drawtext(awayteam, COLOR_MEDIUM[random(10)]);

        // drawAnimatedDigit(20, 20, 10, digit, COLOR_MEDIUM[random(12)]);
        drawAnimatedDigit(20, 20, 10, number, COLOR_MEDIUM[random(12)]);
        drawAnimatedDigit(20 + (10 * 3) + (2 * 3), 20, 10, 10, COLOR_MEDIUM[random(12)]);
        drawAnimatedDigit(20 + (10 * 3) + (2 * 3) + (10 * 3) + (2 * 3), 20, 10, ascr, COLOR_MEDIUM[random(12)]);
    }
}

void displayscore(String score)
{

    tft.fillScreen(TFT_BLACK);
    String scores = data.substring(data.indexOf(">"));
    String homescore = scores.substring(scores.indexOf(">") + 2, scores.indexOf("-"));
    String awayscore = scores.substring(scores.indexOf("-") + 1);
    String hometeam = data.substring(0, data.indexOf("vs"));
    String awayteam = data.substring(data.indexOf("vs") + 2, data.indexOf(">"));

    // tft.setTextColor(TFT_GREENYELLOW);
    tft.setCursor(0, 0);
    cx = 0, cy = 0;
    drawtext(hometeam, COLOR_MEDIUM[random(10)]);
    cx = 0, cy = 98;
    drawtext(awayteam, COLOR_MEDIUM[random(10)]);
    // printWordWrap(hometeam, COLOR_MEDIUM[random(12)]);
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
    int y_start = 36;
    // for (int pos = 1; pos < 4; pos++)
    // {
    //     uint8_t curr_digit = 0;
    //     if (pos == 0)
    //     {
    //         curr_digit = count / 1000;
    //         if (curr_digit == 0)
    //             curr_digit = 11; // make it blank. see on res.h
    //     }
    //     else if (pos == 1)
    //     {
    //         curr_digit = count / 100 % 10;
    //     }
    //     else if (pos == 2)
    //     {
    //         // curr_digit = count / 10 % 10;
    //         curr_digit = 10; // make it (-). see on res.h
    //     }
    //     else if (pos == 3)
    //     {
    //         curr_digit = count % 10;
    //     }
    //     int rnd = random(10);
    //     for (int row = 0; row < 7; row++)
    //     {
    //         for (int col = 0; col < 5; col++)
    //         {
    //             uint32_t color = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[curr_digit] : COLORS_DARK[curr_digit];
    //             uint32_t colorrnd = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[rnd] : COLORS_DARK[rnd];
    //             if (DIGITS[curr_digit][row][col] == 1)
    //             {
    //                 if (clock_style == 0)
    //                 {
    //                     tft.fillCircle(x_start + col * 7 - 1 + random(4), y_start + row * 7 - 1 + random(4), r, colorrnd);
    //                     // tft.fillCircle(x_start + col * 7 -1+random(4), y_start + row * 7-1+random(4), 3, BLACK);
    //                     // tft.fillCircle(x_start + col * 7 -1+random(4), y_start + row * 7-1+random(4), r, colorrnd);
    //                 }
    //                 else if (clock_style == 1)
    //                 {
    //                     tft.fillRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
    //                 }
    //             }
    //             else
    //             {
    //                 if (clock_style == 1)
    //                 {
    //                     tft.drawRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
    //                 }
    //                 else if (clock_style == 0)
    //                 {
    //                     // tft.drawCircle(x_start + col * 7, y_start + row * 7, r, colorrnd);
    //                 }
    //             }
    //         }
    //     }
    //     x_start += 39;
    // }
    if (clock_style == 0)
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
            int startrow = 0;
            for (int ani = 0; ani < 20; ani++)
            {
                if (ani % 2 == 0)
                    startrow++;
                if (startrow > 6)
                    startrow = 6;
            }
            for (int row = startrow; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    uint32_t color = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[curr_digit] : COLORS_DARK[curr_digit];
                    uint32_t colorrnd = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[rnd] : COLORS_DARK[rnd];
                    // if (DIGITS[curr_digit][row][col] == 1)
                    bool state = DIGITS[curr_digit][row][col];
                    tft.fillCircle(x_start + col * 7 - 1 + random(4), y_start + row * 7 - 1 + random(4), r, (state) ? colorrnd : TFT_BLACK);
                }
                delay(20);
            }
            x_start += 39;
        }
    else if (clock_style == 1)
    {
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
            for (int ani = 0; ani < 20; ani++)
            {
            }

            for (int row = 0; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    uint32_t color = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[curr_digit] : COLORS_DARK[curr_digit];
                    uint32_t colorrnd = DIGITS[curr_digit][row][col] ? COLORS_LIGHT[rnd] : COLORS_DARK[rnd];
                    if (DIGITS[curr_digit][row][col] == 1)
                        if (clock_style == 1)
                            tft.fillRoundRect(x_start - r + col * 7, y_start - r + row * 7, r * 2, r * 2, 0, colorrnd);
                }
            }
            x_start += 39;
        }
    }
}
