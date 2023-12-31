#include <SPFD5408_Adafruit_GFX.h>     // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h>  // Hardware-specific library
#include <SPFD5408_TouchScreen.h>

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define WHITE 0x0000   //Black->White
#define YELLOW 0x001F  //Blue->Yellow
#define CYAN 0xF800    //Red->Cyan
#define PINK 0x07E0    //Green-> Pink
#define RED 0x07FF     //Cyan -> Red
#define GREEN 0xF81F   //Pink -> Green
#define BLUE 0xFFE0    //Yellow->Blue
#define BLACK 0xFFFF   //White-> Black
#define MINPRESSURE 10
#define MAXPRESSURE 1000

/*____Calibrate TFT LCD_____*/
short TS_MINX = 920;
short TS_MINY = 120;
short TS_MAXX = 130;
short TS_MAXY = 940;
/*______End of Calibration______*/
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);               //300 is the sensitivity
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);  //Start communication with LCD

uint16_t colors[] = {
  WHITE,
  YELLOW,
  CYAN,
  PINK,
  RED,
  GREEN,
  BLUE,
  BLACK,
};
uint16_t ncolors = 8;

#define HOROSHIY_ZVET 0x373F


const int xPin = A14;
const int yPin = A15;
const int buttonPin = 52;
const int powerPin = 53;

int xPosition = 0;
int yPosition = 0;
int buttonState = 0;
bool highX = false, lowX = false, joystickActive = false;

void setup() {
  Serial.begin(9600);  //Use serial monitor for debugging

  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  pinMode(yPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  tft.reset();         //Always reset at start
  tft.begin(0x9341);   // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(0);  // I just roated so that the power jack faces up - optional
  tft.fillScreen(WHITE);
  IntroScreen();
  StartScreen();
}
int mode = 0;
void StartScreen() {
  tft.fillCircle(60, 40, 35, random(0xFFFF));
  tft.fillCircle(60, 120, 35, random(0xFFFF));
  tft.fillCircle(60, 200, 35, random(0xFFFF));
  tft.fillCircle(60, 280, 35, random(0xFFFF));
  tft.fillCircle(180, 40, 35, random(0xFFFF));
  random(0xFFFF);
  tft.fillCircle(180, 120, 35, random(0xFFFF));
  tft.fillCircle(180, 200, 35, random(0xFFFF));
  tft.fillCircle(180, 280, 35, random(0xFFFF));
  mode = 0;
}


// 0 - Selection mode
// 1 - Paint mode
// 2 - Calculator mode
// 3 - Arkanoid mode
// 4 - Snake
// 5 - 2048
// 6 - Pipes
// 7 - Tic Tac Toe
// 8 - Minesweeper

void loop() {
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  buttonState = digitalRead(buttonPin);
  if (xPosition < 100 || yPosition < 100)
    lowX = true;
  if (xPosition > 900 || yPosition > 900)
    highX = true;
  if (highX || lowX)
    joystickActive = true;

  if (mode == 0) {
    DrawSelection();
  } else if (mode == 2) {
    DrawCalc();
  } else if (mode == 3) {
    DrawArkanoid();
  } else if (mode == 4) {
    DrawSnake();
  } else if (mode == 5) {
    Draw2048();
  } else if (mode == 6) {
    DrawPipes();
  } else if (mode == 7) {
    DrawTT();
  } else if (mode == 8) {
    DrawMS();
  } else {
    DrawPaint();
  }
}

bool dig;

bool open[6][7];
bool mine[6][7];
bool mark[6][7];
int mines;

void StartMS() {
  dig = true;
  memset(&open, 0, sizeof(open));
  memset(&mine, 0, sizeof(mine));
  memset(&mark, 0, sizeof(mark));
  mines = 0;
  while (mines < 8) {
    int x = random(6);
    int y = random(7);
    if (mine[x][y])
      continue;
    mine[x][y] = true;
    mines++;
  }
}

int CountMines(int x, int y) {
  int r = 0;
  if ((x > 0) && mine[x - 1][y])
    r++;
  if ((y > 0) && mine[x][y - 1])
    r++;
  if ((y > 0) && (x > 0) && mine[x - 1][y - 1])
    r++;
  if ((x < 5) && mine[x + 1][y])
    r++;
  if ((y < 6) && mine[x][y + 1])
    r++;
  if ((y < 6) && (x < 5) && mine[x + 1][y + 1])
    r++;
  if ((y > 0) && (x < 5) && mine[x + 1][y - 1])
    r++;
  if ((y < 6) && (x > 0) && mine[x - 1][y + 1])
    r++;
  return r;
}

void ShowBombs() {
  for (int i = 0; i < 6; i++)
    for (int o = 0; o < 7; o++) {
      if (!mine[i][o] || open[i][o])
        continue;
      tft.fillRect(i * 40 + 1, 40 + o * 40 + 1, 39, 39, ~GREEN);
      tft.setCursor(i * 40 + 10, 40 + o * 40 + 6);
      tft.setTextSize(4);
      tft.setTextColor(~CYAN);
      tft.print("B");
    }
}

void DrawMS() {
  tft.fillRect(2, 2, 120 - 4, 40 - 4, ~WHITE);
  tft.setCursor(10, 10);
  tft.setTextSize(dig ? 4 : 3);
  tft.setTextColor(dig ? ~RED : ~GREEN);
  tft.print("DIG");

  tft.fillRect(2 + 120, 2, 120 - 4, 40 - 4, ~WHITE);
  tft.setCursor(10 + 120, 10);
  tft.setTextSize(dig ? 3 : 4);
  tft.setTextColor(dig ? ~GREEN : ~RED);
  tft.print("MARK");

  bool show_bobms = false;
  bool win = true;
  for (int i = 0; i < 6; i++)
    for (int o = 0; o < 7; o++) {
      if (!open[i][o] && !mine[i][o])
        win = false;
      if (mark[i][o]) {
        tft.fillRect(i * 40 + 1, 40 + o * 40 + 1, 39, 39, ~RED);
        continue;
      }
      if (!open[i][o]) {
        tft.fillRect(i * 40 + 1, 40 + o * 40 + 1, 39, 39, ~GREEN);
      } else {
        if (mine[i][o]) {
          show_bobms = true;
          tft.fillRect(i * 40 + 1, 40 + o * 40 + 1, 39, 39, ~BLACK);
          tft.setCursor(i * 40 + 10, 40 + o * 40 + 6);
          tft.setTextSize(4);
          tft.setTextColor(~RED);
          tft.print("B");
        } else {
          int c = CountMines(i, o);
          tft.fillRect(i * 40 + 1, 40 + o * 40 + 1, 39, 39, ~WHITE);
          if (c != 0) {
            tft.setCursor(i * 40 + 10, 50 + o * 40);
            tft.setTextSize(4);
            tft.setTextColor(~BLUE);
            tft.print(c);
          }
        }
      }
    }
  if (show_bobms || win) {
    ShowBombs();
    delay(3000);
    if (win) {
      WinScreen();
      delay(2000);
    }
    StartScreen();
  }

  TSPoint p;
  p.z = 0;
  while (p.z < MINPRESSURE)
    p = ReadPoint();

  if (p.y < 40) {
    dig = p.x < 120;
    return;
  }
  int x = p.x / 40;
  int y = p.y / 40 - 1;
  click(x, y);
}

void click(int x, int y) {
  if (dig) {
    if (!mark[x][y]) {
      open[x][y] = true;
      if (!mine[x][y] && (CountMines(x, y) == 0)) {
        for (int i = max(x - 1, 0); i <= min(x + 1, 5); i++)
          for (int o = max(y - 1, 0); o <= min(y + 1, 6); o++) {
            if (x != i || y != o) {
              if (!open[i][o])
                click(i, o);
            }
          }
      }
    }
  } else {
    if (!open[x][y])
      mark[x][y] = !mark[x][y];
  }
}

bool nextCross;

int tt[3][3];
void StartTT() {
  tft.fillRect(78, 40, 4, 240, ~RED);
  tft.fillRect(78 + 80, 40, 4, 240, ~RED);
  tft.fillRect(0, 40 + 78, 240, 4, ~RED);
  tft.fillRect(0, 40 + 78 + 80, 240, 4, ~RED);

  tft.fillRect(0, 38, 240, 4, ~RED);
  tft.fillRect(0, 278, 240, 4, ~RED);
  nextCross = true;
  memset(&tt, 0, sizeof(tt));
}


void DrawTT() {
  TSPoint p;
  p.z = 0;
  while (p.z < MINPRESSURE)
    p = ReadPoint();

  if (p.y < 40 || p.y > 240)
    return;
  int cx = p.x / 80;
  int cy = (p.y - 40) / 80;
  p.x = (cx)*80;
  p.y = (cy)*80 + 40;
  if (tt[cx][cy] != 0)
    return;
  tft.setTextSize(6);
  if (nextCross) {
    tt[cx][cy] = 1;
    tft.setTextColor(~RED);
    tft.setCursor(p.x + 20, p.y + 20);
    tft.print("X");
  } else {
    tt[cx][cy] = 2;
    tft.setTextColor(~GREEN);
    tft.setCursor(p.x + 20, p.y + 20);
    tft.print("O");
  }

  for (int i = 0; i < 3; i++) {
    if (tt[0][i] != 0 && tt[0][i] == tt[1][i] && tt[0][i] == tt[2][i]) {
      tft.fillRect(0, 38 + 40 + i * 80, 240, 4, ~BLACK);
      delay(3000);
      StartScreen();
      return;
    }
    if (tt[i][0] != 0 && tt[i][0] == tt[i][1] && tt[i][0] == tt[i][2]) {
      tft.fillRect(38 + i * 80, 40, 4, 240, ~BLACK);
      delay(3000);
      StartScreen();
      return;
    }
  }
  if (tt[0][0] != 0 && tt[0][0] == tt[1][1] && tt[0][0] == tt[2][2]) {
    tft.drawLine(0, 40, 240, 240 + 40, ~BLACK);
    delay(3000);
    StartScreen();
    return;
  }

  if (tt[2][0] != 0 && tt[2][0] == tt[1][1] && tt[2][0] == tt[0][2]) {
    tft.drawLine(240, 40, 0, 240 + 40, ~BLACK);
    delay(3000);
    StartScreen();
    return;
  }

  delay(1000);

  nextCross = !nextCross;
}

#define PX 4
#define PY 5
#define PXR (240 / PX)
#define PYR (320 / PY)

struct Pipe {
  bool u;
  bool d;
  bool l;
  bool r;
  bool s;
  bool v;
  bool p;
};

Pipe pfld[PX][PY];
int psx;
int psy;

void DrawPipes() {
  ResetVisited();
  PowerRecurse(psx, psy);
  for (int i = 0; i < PX; i++)
    for (int o = 0; o < PY; o++) {
      if (i == psx && o == psy)
        tft.fillRect(PXR * i + 1, PYR * o + 1, PXR - 2, PYR - 2, ~WHITE);
      else
        tft.fillRect(PXR * i + 1, PYR * o + 1, PXR - 2, PYR - 2, ~RED);

      uint16_t color = ~BLACK;
      if (pfld[i][o].p)
        color = ~BLUE;

      if (pfld[i][o].u) {
        tft.fillRect(PXR * i + PXR / 2 - 2, PYR * o, 4, PYR / 2, color);
      }
      if (pfld[i][o].d) {
        tft.fillRect(PXR * i + PXR / 2 - 2, PYR * o + PYR / 2, 4, PYR / 2, color);
      }
      if (pfld[i][o].l) {
        tft.fillRect(PXR * i, PYR * o + PYR / 2 - 2, PXR / 2, 4, color);
      }
      if (pfld[i][o].r) {
        tft.fillRect(PXR * i + PXR / 2, PYR * o + PYR / 2 - 2, PXR / 2, 4, color);
      }
    }
  if (PipesWin()) {
    WinScreen();
    StartScreen();
    return;
  }

  TSPoint p;
  p.z = 0;
  while (p.z < MINPRESSURE)
    p = ReadPoint();

  PipeRotate(p.x / PXR, p.y / PYR);
}

void PowerRecurse(int x, int y) {
  pfld[x][y].p = true;
  pfld[x][y].v = true;
  if ((x > 0) && (!pfld[x - 1][y].v)) {
    if (pfld[x - 1][y].r && pfld[x][y].l)
      PowerRecurse(x - 1, y);
  }
  if ((y > 0) && (!pfld[x][y - 1].v)) {
    if (pfld[x][y - 1].d && pfld[x][y].u)
      PowerRecurse(x, y - 1);
  }
  if ((x < PX - 1) && (!pfld[x + 1][y].v)) {
    if (pfld[x + 1][y].l && pfld[x][y].r)
      PowerRecurse(x + 1, y);
  }
  if ((y < PY - 1) && (!pfld[x][y + 1].v)) {
    if (pfld[x][y + 1].u && pfld[x][y].d)
      PowerRecurse(x, y + 1);
  }
}

void ResetVisited() {
  for (int i = 0; i < PX; i++)
    for (int o = 0; o < PY; o++) {
      pfld[i][o].v = false;
      pfld[i][o].p = false;
    }
}
bool PipesWin() {
  bool win = true;
  for (int i = 0; i < PX; i++)
    for (int o = 0; o < PY; o++) {
      if (pfld[i][o].d || pfld[i][o].r || pfld[i][o].u || pfld[i][o].l)
        if (!pfld[i][o].p)
          win = false;
    }
  return win;
}

bool CanRecurse(int x, int y) {
  if (x > 0 && (!pfld[x - 1][y].v))
    return true;
  if (y > 0 && (!pfld[x][y - 1].v))
    return true;
  if (x < PX - 1 && (!pfld[x + 1][y].v))
    return true;
  if (y < PY - 1 && (!pfld[x][y + 1].v))
    return true;
  return false;
}


void TryRecurse(int x, int y, int direction, int depth) {
  if ((direction == 0) && (x > 0) && (!pfld[x - 1][y].v)) {
    pfld[x][y].l = true;
    pfld[x - 1][y].r = true;
    pfld[x - 1][y].v = true;
    SeedRecurse(x - 1, y, depth);
  }
  if ((direction == 1) && (y > 0) && (!pfld[x][y - 1].v)) {
    pfld[x][y].u = true;
    pfld[x][y - 1].d = true;
    pfld[x][y - 1].v = true;
    SeedRecurse(x, y - 1, depth);
  }
  if ((direction == 2) && (x < PX - 1) && (!pfld[x + 1][y].v)) {
    pfld[x][y].r = true;
    pfld[x + 1][y].l = true;
    pfld[x + 1][y].v = true;
    SeedRecurse(x + 1, y, depth);
  }
  if ((direction == 3) && (y < PY - 1) && (!pfld[x][y + 1].v)) {
    pfld[x][y].d = true;
    pfld[x][y + 1].u = true;
    pfld[x][y + 1].v = true;
    SeedRecurse(x, y + 1, depth);
  }
}

void SeedRecurse(int x, int y, int depth) {
  if (depth > 10)
    return;

  while (CanRecurse(x, y))
    TryRecurse(x, y, random(4), depth + 1);
}

void PipeRotate(int x, int y) {
  bool t = pfld[x][y].u;
  pfld[x][y].u = pfld[x][y].l;
  pfld[x][y].l = pfld[x][y].d;
  pfld[x][y].d = pfld[x][y].r;
  pfld[x][y].r = t;
}

void SeedPipes() {
  memset(&pfld, 0, sizeof(pfld));
  psx = random(PX);
  psy = random(PY);
  pfld[psx][psy].s = true;
  pfld[psx][psy].v = true;
  SeedRecurse(psx, psy, 0);


  for (int i = 0; i < PX; i++)
    for (int o = 0; o < PY; o++) {
      int r = random(4);
      for (int x = 0; x < r; x++)
        PipeRotate(i, o);
    }
}

#define N2 5

int field[N2][N2];
void SeedField() {
  for (int i = 0; i < N2; i++)
    for (int o = 0; o < N2; o++) {
      if (random(4) != 2)
        continue;
      int p = random(5);
      if (p <= 1)
        field[i][o] = 0;
      else
        field[i][o] = 1 << (p - 1);
    }
}

void DrawField() {
  tft.setTextColor(BLUE);
  int rsize = 240 / N2;
  int shift = N2 > 4 ? 10 : 2;
  for (int i = 0; i < N2; i++)
    for (int o = 0; o < N2; o++) {
      tft.fillRect(rsize * i + 1, rsize * o + 40 + 1, rsize - 2, rsize - 2, ~RED);
      tft.setCursor(rsize * i + shift, rsize * o + 40 + shift);
      if (field[i][o]) {
        if (field[i][o] > 512 || N2 > 5)
          tft.setTextSize(1);
        else if (field[i][o] > 64)
          tft.setTextSize(2);
        else
          tft.setTextSize(N2 > 4 ? 3 : 4);
        tft.print(field[i][o]);
      }
    }
}

bool ShiftUp() {
  bool result = false;
  for (int i = 0; i < N2; i++) {
    for (int o = 0; o < N2; o++) {
      if (field[i][o] == 0) {
        int z = o + 1;
        for (; z < N2; z++) {
          if (field[i][z] != 0) {
            field[i][o] = field[i][z];
            field[i][z] = 0;
            o--;
            result = true;
            break;
          }
        }
      } else {
        if (o > 0 && field[i][o] == field[i][o - 1]) {
          field[i][o - 1] *= 2;
          field[i][o] = 0;
          result = true;
          o--;
        }
      }
    }
  }
  return result;
}
bool ShiftLeft() {
  bool result = false;
  for (int i = 0; i < N2; i++) {
    for (int o = 0; o < N2; o++) {
      if (field[o][i] == 0) {
        int z = o + 1;
        for (; z < N2; z++) {
          if (field[z][i] != 0) {
            field[o][i] = field[z][i];
            field[z][i] = 0;
            result = true;
            o--;
            break;
          }
        }
      } else {
        if (o > 0 && field[o][i] == field[o - 1][i]) {
          field[o - 1][i] *= 2;
          field[o][i] = 0;
          result = true;
          o--;
        }
      }
    }
  }
  return result;
}
bool ShiftDown() {
  bool result = false;
  for (int i = 0; i < N2; i++) {
    for (int o = 0; o < N2; o++) {
      if (field[i][N2 - 1 - o] == 0) {
        int z = o + 1;
        for (; z < N2; z++) {
          if (field[i][N2 - 1 - z] != 0) {
            field[i][N2 - 1 - o] = field[i][N2 - 1 - z];
            field[i][N2 - 1 - z] = 0;
            o--;
            result = true;
            break;
          }
        }
      } else {
        if (o > 0 && field[i][N2 - 1 - o] == field[i][N2 - 1 - (o - 1)]) {
          field[i][N2 - 1 - (o - 1)] *= 2;
          field[i][N2 - 1 - o] = 0;
          result = true;
          o--;
        }
      }
    }
  }
  return result;
}


bool ShiftRight() {
  bool result = false;
  for (int i = 0; i < N2; i++) {
    for (int o = 0; o < N2; o++) {
      if (field[N2 - 1 - o][i] == 0) {
        int z = o + 1;
        for (; z < N2; z++) {
          if (field[N2 - 1 - z][i] != 0) {
            field[N2 - 1 - o][i] = field[N2 - 1 - z][i];
            field[N2 - 1 - z][i] = 0;
            o--;
            result = true;
            break;
          }
        }
      } else {
        if (o > 0 && field[N2 - 1 - o][i] == field[N2 - 1 - (o - 1)][i]) {
          field[N2 - 1 - (o - 1)][i] *= 2;
          field[N2 - 1 - o][i] = 0;
          result = true;
          o--;
        }
      }
    }
  }
  return result;
}

void Draw2048() {
  DrawField();
  TSPoint p;
  p.z = 0;
  bool nonzero = false;
  bool win = false;
  for (int i = 0; i < N2; i++)
    for (int o = 0; o < N2; o++) {
      if (field[i][o] >= 2048)
        win = true;
      if (field[i][o] == 0)
        nonzero = true;
    }
  if (win) {
    WinScreen();
    StartScreen();
    return;
  }
  if (!nonzero) {
    for (int i = 1; i < N2; i++)
      for (int o = 1; o < N2; o++) {
        if (field[i][o] == field[i - 1][o] || field[i][o] == field[i][o - 1])
          nonzero = true;
      }
    if (field[0][0] == field[1][0] || field[0][0] == field[0][1])
      nonzero = true;
  }
  if (!nonzero) {
    delay(1000);
    mode = 0;
    StartScreen();
    return;
  }
  while (p.z < MINPRESSURE)
    p = ReadPoint();

  if (p.x > (6 * p.y) / 8) {
    if (p.x < 240 - ((6 * p.y) / 8)) {
      if (!ShiftUp())
        return;
    } else {
      if (!ShiftRight())
        return;
    }
  } else {
    if (p.x < 240 - ((6 * p.y) / 8)) {
      if (!ShiftLeft())
        return;
    } else {
      if (!ShiftDown())
        return;
    }
  }
  while (true) {
    int i = random(N2);
    int o = random(N2);
    if (field[i][o] == 0) {
      field[i][o] = 2;
      break;
    }
  }
}

struct SnakePoint {
  uint16_t x;
  uint16_t y;
};

SnakePoint snake[100];
int s_l;
int s_x;
int s_y;
int snake_frame;
SnakePoint food;

void GenerateFood() {
  while (true) {
    int x = random(24);
    int y = random(24);
    for (int i = 0; i < s_l; i++) {
      if (snake[i].x == x && snake[i].y == y)
        continue;
    }
    food.x = x;
    food.y = y;
    FoodPixel(x, y);
    return;
  }
}

void StartSnake() {
  for (int x = 0; x < 24; x += 1)
    for (int y = 0; y < 32; y += 1) {
      SnakePixel(x, y, false);
    }

  s_x = 0;
  s_y = 1;
  s_l = 0;
  for (int i = 0; i < 4; i++) {
    snake[i].x = 12;
    snake[i].y = 12 - i;
    SnakePixel(snake[i].x, snake[i].y, true);
    s_l++;
  }
  snake_frame = 0;
  GenerateFood();
}

void SnakePixel(int x, int y, bool snake) {
  uint16_t color = PINK;
  if ((x + y) % 2 == 0)
    color = CYAN;
  if (snake)
    color = HOROSHIY_ZVET;
  tft.fillRect(x * 10, y * 10, 10, 10, color);
}
void FoodPixel(int x, int y) {
  tft.fillRect(x * 10, y * 10, 10, 10, HOROSHIY_ZVET);
}

void SnakeDetect() {
  TSPoint p = ReadPoint();
  int np = p.x;

  if (joystickActive) {
    p.x = 24 * xPosition / 102;
    p.y = 32 * yPosition / 102;
    if (abs(p.x - 120) > 30 || abs(p.y - 160) > 30)
      p.z = MAXPRESSURE;
    else
      p.z = MINPRESSURE;
  }

  if (p.z > MINPRESSURE) {
    int o_x = s_x;
    int o_y = s_y;
    if (p.x > (6 * p.y) / 8) {
      if (p.x < 240 - ((6 * p.y) / 8)) {
        s_x = 0;
        s_y = -1;
      } else {
        s_x = 1;
        s_y = 0;
      }
    } else {
      if (p.x < 240 - ((6 * p.y) / 8)) {
        s_x = -1;
        s_y = 0;
      } else {
        s_x = 0;
        s_y = 1;
      }
    }
    SnakePoint next = { snake[0].x + s_x, snake[0].y + s_y };
    if (next.x == snake[1].x && next.y == snake[1].y) {
      s_x = o_x;
      s_y = o_y;
    }
  }
}

void DrawSnake() {
  snake_frame++;
  SnakeDetect();
  delay(10);
  if (snake_frame % 32 == 0) {
    SnakePoint next;
    bool regen = false;
    next.x = snake[0].x + s_x;
    next.y = snake[0].y + s_y;

    for (int i = 1; i < s_l; i++) {
      if (snake[i].x == next.x && snake[i].y == next.y) {
        tft.fillScreen(WHITE);
        StartScreen();
        return;
      }
    }

    if (food.x == next.x && food.y == next.y) {
      s_l++;
      regen = true;
    } else {
      SnakePixel(snake[s_l - 1].x, snake[s_l - 1].y, false);
    }

    if (next.x < 0 || next.y < 0 || next.x >= 24 || next.y >= 32) {
      tft.fillScreen(WHITE);
      StartScreen();
      return;
    }

    SnakePixel(next.x, next.y, true);
    for (int i = s_l - 1; i > 0; i--) {
      snake[i] = snake[i - 1];
    }
    snake[0] = next;
    if (regen)
      GenerateFood();
    else
      FoodPixel(food.x, food.y);  // Redraw for debug purposes
  }
}

int platform = 240 / 2;
int pl_wh = 240 / 8;

int b_x = 240 / 2;
int b_y = 240;
int v_x = -2 + random(5);
int v_y = -2 + random(6);
uint16_t b_c = BLUE;  //random(0xffff);
int b_r = 10;
int numbricks;

void DrawArkanoid() {
  if (numbricks == 0) {
    WinScreen();
    b_x = 240 / 2;
    b_y = 240;
    v_x = -2 + random(5);
    v_y = -2 + random(6);
    StartScreen();
  }
  TSPoint p = ReadPoint();
  if (joystickActive) {
    p.x = 24 * xPosition / 100;
    p.z = MAXPRESSURE;
  }
  int np = p.x;
  if (p.z > MINPRESSURE && np != platform) {
    if (np < pl_wh)
      np = pl_wh;
    if (np >= 240 - pl_wh)
      np = 239 - pl_wh;

    tft.fillRect(platform - pl_wh, 310, 2 * pl_wh, b_r, WHITE);
    tft.fillRect(np - pl_wh, 310, 2 * pl_wh, b_r, PINK);

    platform = np;
  }

  while (v_y == 0) {
    v_y = -2 + random(5);
  }

  tft.drawCircle(b_x, b_y, 10, WHITE);
  if (b_x + v_x < b_r) {
    v_x = -v_x;
  }
  if (b_y + v_y < b_r) {
    v_y = -v_y;
  }
  if (b_x + v_x > 240 - b_r) {
    v_x = -v_x;
  }

  if (((b_y + v_y) >= 300) && (abs(b_x + v_x - platform) <= pl_wh)) {
    v_y = -1 - random(3);
    do {
      v_x = -2 + random(4);
    } while (v_x == 0);
  }

  if (b_y + v_y > 320 - b_r) {
    b_x = 240 / 2;
    b_y = 240;
    v_x = -2 + random(5);
    v_y = -2 + random(6);
    StartScreen();
    return;
  }

  int collision = TestCollision(b_x, b_y, v_x, v_y);

  if (collision == 1) {
    v_x = -v_x;
  } else if (collision == 2) {
    v_y = -v_y;
  }

  b_x += v_x;
  b_y += v_y;

  tft.drawCircle(b_x, b_y, 10, b_c);

  delay(1);
}

bool bricks[5][4];

void ClearBricks() {
  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 4; y++) {
      bricks[x][y] = true;
    }
  }
  numbricks = 5 * 4;
}

int TestCollision(int x, int y, int dx, int dy) {
  int nx = 5;
  int wx = 240 / nx;
  int ny = 4;
  int wy = 150 / ny;
  int wp = 30;

  int tx = dx + x;
  int ty = dy + y;
  int lineno = (ty - wp);
  if (lineno < 0)
    return 0;
  lineno /= wy;
  int colno = tx / wx;
  if (lineno > 3 || colno > 4)
    return 0;
  if (!bricks[colno][lineno])
    return 0;
  bricks[colno][lineno] = false;  // We break the brick
  tft.drawRect(colno * wx + 2, wp + lineno * wy + 2, wx - 4, wy - 4, WHITE);
  numbricks--;
  int colx = colno * wx;
  if (colx > x || colx + wx < x)
    return 1;  // Side reflection
  return 2;    // Horizontal reflection
}

void StartArkanoid() {
  ClearBricks();
  tft.fillRect(platform - pl_wh, 310, 2 * pl_wh, b_r, PINK);

  int nx = 5;
  int wx = 240 / nx;
  int ny = 4;
  int wy = 150 / ny;
  int wp = 30;

  for (int x = 0; x < nx; x++) {
    for (int y = 0; y < ny; y++) {
      tft.drawRect(x * wx + 2, wp + y * wy + 2, wx - 4, wy - 4, PINK);
    }
  }
}

TSPoint ReadPoint() {
  TSPoint p;
  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  return p;
}

void DrawSelection() {
  TSPoint p = ReadPoint();

  if (p.z > MINPRESSURE) {
    if (p.x <= 120) {
      if (p.y < 320 / 4) {
        mode = 1;
        tft.fillScreen(WHITE);
        for (int x = 0; x < ncolors; x++) {
          tft.fillRect(x * 30, 0, 30, 30, colors[x]);
        }
      } else if (p.y < 2 * 320 / 4) {
        mode = 2;
        draw_BoxNButtons();
      } else if (p.y < 3 * 320 / 4) {
        mode = 3;
        tft.fillScreen(WHITE);
        StartArkanoid();
      } else {
        mode = 4;
        StartSnake();
      }
    } else {
      if (p.y < 320 / 4) {
        mode = 5;
        SeedField();
        tft.fillScreen(WHITE);
      } else if (p.y < 2 * 320 / 4) {
        mode = 6;
        SeedPipes();
        tft.fillScreen(WHITE);
      } else if (p.y < 3 * 320 / 4) {
        mode = 7;
        tft.fillScreen(HOROSHIY_ZVET);
        StartTT();
      } else {
        mode = 8;
        tft.fillScreen(HOROSHIY_ZVET);
        StartMS();
      }
    }
  }
  random();  // hack to get entropy
  delay(10);
}

uint16_t current_color = 0;

int drawing = 0;
TSPoint prev_point = { 0, 0, 0 };
TSPoint DrawPaint() {
  TSPoint p;
  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    Serial.print(p.x);
    Serial.print(',');
    Serial.println(p.y);

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    if (p.y < 30)
      current_color = colors[p.x / 30];
    else {
      if (drawing == 0) {
        //tft.fillRect(p.x-2,p.y-2,5,5,current_color);
      } else {
        TSPoint p1 = prev_point;
        TSPoint p2 = p;
        tft.drawLine(p1.x, p1.y, p2.x, p2.y, current_color);
        p1.x += 1;
        p2.x += 1;
        tft.drawLine(p1.x, p1.y, p2.x, p2.y, current_color);
        p1.y -= 1;
        p2.y -= 1;
        tft.drawLine(p1.x, p1.y, p2.x, p2.y, current_color);
      }
      drawing = 25;
      prev_point = p;
    }
  } else {
    if (drawing == 1) {
      //tft.drawCircle(prev_point.x,prev_point.y,3,current_color);
    }
    if (drawing > 0)
      drawing--;
  }
  delay(10);
}


void WinScreen() {
  tft.setCursor(50, 115);
  tft.setTextSize(4);
  tft.setCursor(80, 145);
  tft.setTextColor(BLUE);
  tft.println("WIN");
  tft.setCursor(50, 185);

  delay(1000);
  tft.fillScreen(WHITE);
}

void IntroScreen() {
  tft.setCursor(50, 115);
  tft.setTextSize(4);
  tft.setTextColor(GREEN);
  tft.println("Matvei");
  tft.setCursor(80, 145);
  tft.setTextColor(BLUE);
  tft.println("and");
  tft.setCursor(50, 185);
  tft.setTextColor(CYAN);
  tft.println("Andrey");

  delay(1000);
  tft.fillScreen(WHITE);
}


String symbol[4][4] = {
  { "7", "8", "9", "/" },
  { "4", "5", "6", "*" },
  { "1", "2", "3", "-" },
  { "C", "0", "=", "+" }
};
int X, Y;
long Num1, Num2, Number;
char action;
boolean result = false;


void DrawCalc() {
  TSPoint p = waitTouch();
  X = p.y;
  Y = p.x;
  DetectButtons();
  if (result == true)
    CalculateResult();
  DisplayResult();
  delay(300);
}
TSPoint waitTouch() {
  TSPoint p;
  int32_t current_time = 0;
  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE) || (p.z > MAXPRESSURE));

  Serial.print(p.x);
  Serial.print(',');
  Serial.println(p.y);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  return p;
}
void DetectButtons() {
  if (Y < 50 && Y > 0)  //Detecting Buttons on Column 1
  {
    if (X > 269 && X < 300)  //If cancel Button is pressed
    {
      Serial.println("Button Cancel");
      Number = Num1 = Num2 = 0;
      result = false;
    }
    if (X > 210 && X < 260)  //If Button 1 is pressed
    {
      Serial.println("Button 1");
      if (Number == 0)
        Number = 1;
      else
        Number = (Number * 10) + 1;  //Pressed twice
    }
    if (X > 160 && X < 205)  //If Button 4 is pressed
    {
      Serial.println("Button 4");
      if (Number == 0)
        Number = 4;
      else
        Number = (Number * 10) + 4;  //Pressed twice
    }
    if (X > 108 && X < 156)  //If Button 7 is pressed
    {
      Serial.println("Button 7");
      if (Number == 0)
        Number = 7;
      else
        Number = (Number * 10) + 7;  //Pressed twice
    }
  }
  if (Y < 100 && Y > 55)  //Detecting Buttons on Column 2
  {
    if (X > 269 && X < 300) {
      Serial.println("Button 0");  //Button 0 is Pressed
      if (Number == 0)
        Number = 0;
      else
        Number = (Number * 10) + 0;  //Pressed twice
    }
    if (X > 214 && X < 256) {
      Serial.println("Button 2");
      if (Number == 0)
        Number = 2;
      else
        Number = (Number * 10) + 2;  //Pressed twice
    }
    if (X > 160 && X < 205) {
      Serial.println("Button 5");
      if (Number == 0)
        Number = 5;
      else
        Number = (Number * 10) + 5;  //Pressed twic
    }
    if (X > 108 && X < 156) {
      Serial.println("Button 8");
      if (Number == 0)
        Number = 8;
      else
        Number = (Number * 10) + 8;  //Pressed twic
    }
  }
  if (Y < 175 && Y > 115)  //Detecting Buttons on Column 3
  {
    if (X > 269 && X < 300) {
      Serial.println("Button Equal");
      Num2 = Number;
      result = true;
    }
    if (X > 210 && X < 256) {
      Serial.println("Button 3");
      if (Number == 0)
        Number = 3;
      else
        Number = (Number * 10) + 3;  //Pressed twice
    }
    if (X > 160 && X < 205) {
      Serial.println("Button 6");
      if (Number == 0)
        Number = 6;
      else
        Number = (Number * 10) + 6;  //Pressed twice
    }
    if (X > 108 && X < 156) {
      Serial.println("Button 9");
      if (Number == 0)
        Number = 9;
      else
        Number = (Number * 10) + 9;  //Pressed twice
    }
  }
  if (Y < 236 && Y > 182)  //Detecting Buttons on Column 3
  {
    Num1 = Number;
    Number = 0;
    tft.setCursor(200, 20);
    tft.setTextColor(RED);
    if (X > 269 && X < 300) {
      Serial.println("Addition");
      action = 1;
      tft.println('+');
    }
    if (X > 214 && X < 256) {
      Serial.println("Subtraction");
      action = 2;
      tft.println('-');
    }
    if (X > 160 && X < 205) {
      Serial.println("Multiplication");
      action = 3;
      tft.println('*');
    }
    if (X > 108 && X < 156) {
      Serial.println("Devesion");
      action = 4;
      tft.println('/');
    }
    delay(300);
  }
}
void CalculateResult() {
  if (action == 1)
    Number = Num1 + Num2;
  if (action == 2)
    Number = Num1 - Num2;
  if (action == 3)
    Number = Num1 * Num2;
  if (action == 4)
    Number = Num1 / Num2;
}



int32_t circle_x = 10;
int32_t circle_y = 20;
int32_t dx = 4;
int32_t dy = 3;
void DisplayResult() {
  tft.fillRect(0, 0, 240, 80, CYAN);  //clear result box
  tft.drawCircle(circle_x, circle_y, 2, GREEN);
  if (circle_x > 230)
    dx = -4;
  if (circle_y > 70)
    dy = -3;
  if (circle_x < 10)
    dx = 4;
  if (circle_y < 10)
    dy = 3;
  circle_x += dx;
  circle_y += dy;
  tft.setCursor(10, 20);
  tft.setTextSize(4);
  tft.setTextColor(GREEN);
  tft.println(Number);  //update new value
}
void draw_BoxNButtons() {
  //Draw the Result Box
  tft.fillRect(0, 0, 240, 80, CYAN);
  //Draw First Column
  tft.fillRect(0, 260, 60, 60, RED);
  tft.fillRect(0, 200, 60, 60, BLACK);
  tft.fillRect(0, 140, 60, 60, BLACK);
  tft.fillRect(0, 80, 60, 60, BLACK);
  //Draw Third Column
  tft.fillRect(120, 260, 60, 60, GREEN);
  tft.fillRect(120, 200, 60, 60, BLACK);
  tft.fillRect(120, 140, 60, 60, BLACK);
  tft.fillRect(120, 80, 60, 60, BLACK);
  //Draw Secound & Fourth Column
  for (int b = 260; b >= 80; b -= 60) {
    tft.fillRect(180, b, 60, 60, BLUE);
    tft.fillRect(60, b, 60, 60, BLACK);
  }
  //Draw Horizontal Lines
  for (int h = 80; h <= 320; h += 60)
    tft.drawFastHLine(0, h, 240, WHITE);
  //Draw Vertical Lines
  for (int v = 0; v <= 240; v += 60)
    tft.drawFastVLine(v, 80, 240, WHITE);
  //Display keypad lables
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      tft.setCursor(22 + (60 * i), 100 + (60 * j));
      tft.setTextSize(3);
      tft.setTextColor(WHITE);
      tft.println(symbol[j][i]);
    }
  }
}
