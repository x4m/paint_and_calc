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
void setup() {
  Serial.begin(9600);  //Use serial monitor for debugging
  tft.reset();         //Always reset at start
  tft.begin(0x9341);   // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(0);  // I just roated so that the power jack faces up - optional
  tft.fillScreen(WHITE);
  IntroScreen();
  StartScreen();
}
int mode = 0;
void StartScreen() {
  tft.fillCircle(120, 40, 35, random(0xFFFF));
  tft.fillCircle(120, 120, 35, random(0xFFFF));
  tft.fillCircle(120, 200, 35, random(0xFFFF));
  tft.fillCircle(120, 280, 35, random(0xFFFF));
  mode = 0;
}


// 0 - Selection mode
// 1 - Paint mode
// 2 - Calculator mode
// 3 - Arkanoid mode
// 4 - Flappy bird

void loop() {
  if (mode == 0) {
    DrawSelection();
  } else if (mode == 2) {
    DrawCalc();
  } else if (mode == 3) {
    DrawArkanoid();
  } else if (mode == 4) {
    DrawSnake();
  } else {
    DrawPaint();
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
    FoodPixel(x,y);
    return;
  }
}

void StartSnake() {
  for (int x = 0; x < 24; x += 1)
    for (int y = 0; y < 32; y += 1) {
      SnakePixel(x, y, false);
    }

  s_x = 1;
  s_y = 0;
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

#define HOROSHIY_ZVET 0x373F

void SnakePixel(int x, int y, bool snake) {
  uint16_t color = PINK;
  if ((x + y) % 2 == 0)
    color = CYAN;
  if (snake)
    color = HOROSHIY_ZVET;
  tft.fillRect(x * 10, y * 10, 10, 10, color);
}
void FoodPixel(int x, int y) {
  tft.fillRect(x * 10, y * 10, 10, 10, BLUE);
}

void SnakeDetect() {
  TSPoint p = ReadPoint();
  int np = p.x;
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

    for (int i=1;i<s_l;i++) {
      if (snake[i].x == next.x && snake[i].y==next.y) {
        StartScreen();
        return;
      }
    }

    if (food.x == next.x && food.y == next.y)
    {
      s_l++;
      regen = true;
    }
    else {
      SnakePixel(snake[s_l - 1].x, snake[s_l - 1].y, false);
    }

    if (next.x < 0 || next.y <0 || next.x>=24 || next.y >=32) {
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
      FoodPixel(food.x,food.y); // Redraw for debug purposes
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
  }
  random(); // hack to get entropy
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
