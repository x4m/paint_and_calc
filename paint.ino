#include <SPFD5408_Adafruit_GFX.h> // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>

#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9 // can be a digital pin
#define XP 8 // can be a digital pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define WHITE 0x0000 //Black->White
#define YELLOW 0x001F //Blue->Yellow
#define CYAN 0xF800 //Red->Cyan
#define PINK 0x07E0 //Green-> Pink
#define RED 0x07FF //Cyan -> Red
#define GREEN 0xF81F //Pink -> Green
#define BLUE 0xFFE0 //Yellow->Blue
#define BLACK 0xFFFF //White-> Black
#define MINPRESSURE 10
#define MAXPRESSURE 1000

/*____Calibrate TFT LCD_____*/

#define TS_MINY 109
#define TS_MINX 933
#define TS_MAXY 940
#define TS_MAXX 122
/*______End of Calibration______*/ 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //300 is the sensitivity
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //Start communication with LCD 

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
  Serial.begin(9600); //Use serial monitor for debugging
  tft.reset(); //Always reset at start
  tft.begin(0x9341); // My LCD uses LIL9341 Interface driver IC
  tft.setRotation(0); // I just roated so that the power jack faces up - optional
  tft.fillScreen(WHITE); 
  IntroScreen();
}

int mode = 0;

// 0 - Selection mode
// 1 - Paint mode
// 2 - Calculator mode


void loop() {
    if (mode == 0)
    {
      DrawSelection();
    }
    else {
      DrawPaint();
    }
}

void DrawSelection() {
  tft.fillCircle(120, 80, 30, PINK);
  tft.fillCircle(120, 240, 30, PINK);
  TSPoint p;
  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
    if (p.y<160) {
      mode = 1;
      tft.fillScreen(WHITE); 
      for (int x = 0; x < ncolors; x ++) {
        tft.fillRect(x*30,0,30,30,colors[x]);
      }
    } else {

    }
  }
  delay(200);
}

uint16_t current_color = 0;

int drawing = 0;
TSPoint prev_point = {0,0,0};
TSPoint DrawPaint() {
  TSPoint p;
  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
      Serial.print(p.x); Serial.print(','); Serial.println(p.y);

      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
      //p.x = (p.x / 5) * 5;
      //p.y = (p.y / 5) * 5;

      if (p.y<30)
        current_color = colors[p.x/30];
      else {
        if (drawing == 0) {
          //tft.fillRect(p.x-2,p.y-2,5,5,current_color);
        } else
        {
          tft.drawLine(prev_point.x, prev_point.y, p.x, p.y, current_color);
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


void IntroScreen()
{
  tft.setCursor (50, 115);
  tft.setTextSize (4);
  tft.setTextColor(BLUE);
  tft.setTextColor(GREEN);
  tft.println("Matvei");
  delay(1000);
  tft.fillScreen(WHITE); 
  DrawColorButtons();
}

void DrawColorButtons() {
  // tft.fillRect(0,0,5,5,BLACK);
  // tft.fillRect(235,0,5,5,BLACK);
  // tft.fillRect(235,315,5,5,BLACK);
  // tft.fillRect(0,315,5,5,BLACK);
  /*for (int x = 0; x < 240; x += 10) {
    for (int y = 0; y < 320; y += 10) {
      int color = GREEN;
      if ((x+y)%20 == 0)
      {
        color = RED;
      }
      else
      {
        color = GREEN;
      }
      tft.fillRect(x,y,10,10,color);
    }
  }*/
}

