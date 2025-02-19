
// Segment activation patterns for numbers 0-9 (A-G segments)
const byte tdigitPatterns[11] = {
  0b1111110,  // 0
  0b0110000,  // 1
  0b1101101,  // 2
  0b1111001,  // 3
  0b0110011,  // 4
  0b1011011,  // 5
  0b1011111,  // 6
  0b1110000,  // 7
  0b1111111,  // 8
  0b1111011,  // 9
  0b0000001,  // 10 as "-" sign
};

// Segment positions (relative to x,y)
const int tsegments[7][4] = {
  { 1, 0, 3, 0 },  // A (top)
  { 4, 1, 4, 3 },  // B (upper right)
  { 4, 4, 4, 6 },  // C (lower right)
  { 1, 7, 3, 7 },  // D (bottom)
  { 0, 4, 0, 6 },  // E (lower left)
  { 0, 1, 0, 3 },  // F (upper left)
  { 1, 4, 3, 4 }   // G (middle)
};
uint32_t triangularpos[10][3][2] = {
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 0
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 1
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 2
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 3
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 4
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 5
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 6
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 7
  { { 0, 0 }, { 0, 0 }, { 0, 0 } },  // 8
  { { 0, 0 }, { 0, 0 }, { 0, 0 } }
};  // 9
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
void fillingTriangular(int x, int y, int size, int gap) {
  float btp = sqrt((size * size) + ((size / 2) * (size / 2)));  // a to d
  float btpg = sqrt((gap * gap) + ((gap / 2) * (gap / 2)));     //gap to gap
  for (int i = 0; i < 10; i++) {
    if (i == 0) {
      triangularpos[i][0][0] = x;               //x
      triangularpos[i][0][1] = y;               //y
      triangularpos[i][1][0] = int(btp);        //x1
      triangularpos[i][1][1] = y + (size / 2);  //y1
      triangularpos[i][2][0] = x;               //x2
      triangularpos[i][2][1] = y + size;        //y2
    } else if (i == 1) {
      triangularpos[i][0][0] = x + ;                         //x
      triangularpos[i][0][1] = y + size + gap;               //y
      triangularpos[i][1][0] = int(btp);                     //x1
      triangularpos[i][1][1] = y + (size / 2) + size + gap;  //y1
      triangularpos[i][2][0] = x;                            //x2
      triangularpos[i][2][1] = y + size + size + gap;        //y2
    } else if (i == 2) {
      triangularpos[i][0][0] = x + int(btp) + int(btpg) + gap;     //x
      triangularpos[i][0][1] = y - ((size / 2) - (gap / 2));       //y
      triangularpos[i][1][0] = triangularpos[i][0][0] + int(btp);  //x1
      triangularpos[i][1][1] = y + (gap / 2);                      //y1
      triangularpos[i][2][0] = x + int(btp) + int(btpg) + gap;     //x2
      triangularpos[i][2][1] = y + (size / 2) - (gap / 2);         //y2
    } else if (i == 3) {
      triangularpos[i][0][0] = x + int(btp) + int(btpg) + gap;  //x
      triangularpos[i][0][1] = y + ((size / 2) - (gap / 2));    //y
      triangularpos[i][1][0] = int(btp);                        //x1========V
      triangularpos[i][1][1] = y + (size / 2) + size + gap;     //y1
      triangularpos[i][2][0] = x;                               //x2
      triangularpos[i][2][1] = y + size + size;                 //y2
    } else if (i == 4) {
      triangularpos[i][0][0] = x + int(btp) + gap;             //x
      triangularpos[i][0][1] = y + size + gap;                 //y
      triangularpos[i][1][0] = x + int(btp) + gap + int(btp);  //x1
            triangularpos[i][1][1] = y + (size /
    }
  }