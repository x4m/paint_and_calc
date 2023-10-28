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
  tft.fillCircle(120, 80, 60, random(0xFFFF));
  tft.fillCircle(120, 240, 60, random(0xFFFF));
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
    else if (mode == 2)
    {
      DrawCalc();
    }
    else {
      DrawPaint();
    }
}

void DrawSelection() {

  TSPoint p;
  p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  if (p.z > MINPRESSURE) 
  {
    if (p.y<160) {
      mode = 1;
      tft.fillScreen(WHITE); 
      for (int x = 0; x < ncolors; x ++) {
        tft.fillRect(x*30,0,30,30,colors[x]);
      }
    } else {
      mode = 2;
      draw_BoxNButtons();
    }
  }
  delay(10);
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
}


String symbol[4][4] = {
{ "7", "8", "9", "/" },
{ "4", "5", "6", "*" },
{ "1", "2", "3", "-" },
{ "C", "0", "=", "+" }
};
int X,Y;
long Num1,Num2,Number;
char action;
boolean result = false; 


void DrawCalc() {
TSPoint p = waitTouch();
X = p.y; Y = p.x;
DetectButtons(); 
if (result==true)
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
} while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));

 Serial.print(p.x); Serial.print(','); Serial.println(p.y);
p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
return p;
} 
void DetectButtons()
{ 
if (Y<50 && Y>0) //Detecting Buttons on Column 1
{
if (X>269 && X<300) //If cancel Button is pressed
{Serial.println ("Button Cancel"); Number=Num1=Num2=0; result=false;} 
if (X>210 && X<260) //If Button 1 is pressed
{Serial.println ("Button 1");
if (Number==0)
Number=1;
else
Number = (Number*10) + 1; //Pressed twice
} 
if (X>160 && X<205) //If Button 4 is pressed
{Serial.println ("Button 4");
if (Number==0)
Number=4;
else
Number = (Number*10) + 4; //Pressed twice
} 
if (X>108 && X<156) //If Button 7 is pressed
{Serial.println ("Button 7");
if (Number==0)
Number=7;
else
Number = (Number*10) + 7; //Pressed twice
}
} 
if (Y<100 && Y>55) //Detecting Buttons on Column 2
{
if (X>269 && X<300)
{Serial.println ("Button 0"); //Button 0 is Pressed
if (Number==0)
Number=0;
else
Number = (Number*10) + 0; //Pressed twice
} 
if (X>214 && X<256)
{Serial.println ("Button 2");
if (Number==0)
Number=2;
else
Number = (Number*10) + 2; //Pressed twice
} 
if (X>160 && X<205)
{Serial.println ("Button 5");
if (Number==0)
Number=5;
else
Number = (Number*10) + 5; //Pressed twic
} 
if (X>108 && X<156)
{Serial.println ("Button 8");
if (Number==0)
Number=8;
else
Number = (Number*10) + 8; //Pressed twic
}
} 
if (Y<175 && Y>115) //Detecting Buttons on Column 3
{
if (X>269 && X<300)
{Serial.println ("Button Equal");
Num2=Number;
result = true;
}
if (X>210 && X<256)
{Serial.println ("Button 3");
if (Number==0)
Number=3;
else
Number = (Number*10) + 3; //Pressed twice
}
if (X>160 && X<205)
{Serial.println ("Button 6");
if (Number==0)
Number=6;
else
Number = (Number*10) + 6; //Pressed twice
} 
if (X>108 && X<156)
{Serial.println ("Button 9");
if (Number==0)
Number=9;
else
Number = (Number*10) + 9; //Pressed twice
}
} 
if (Y<236 && Y>182) //Detecting Buttons on Column 3
{
Num1 = Number;
Number =0;
tft.setCursor(200, 20);
tft.setTextColor(RED);
if (X>269 && X<300)
{Serial.println ("Addition"); action = 1; tft.println('+');}
if (X>214 && X<256)
{Serial.println ("Subtraction"); action = 2; tft.println('-');}
if (X>160 && X<205)
{Serial.println ("Multiplication"); action = 3; tft.println('*');}
if (X>108 && X<156)
{Serial.println ("Devesion"); action = 4; tft.println('/');} 
delay(300);
}
} 
void CalculateResult()
{
if (action==1)
Number = Num1+Num2; 
if (action==2)
Number = Num1-Num2; 
if (action==3)
Number = Num1*Num2; 
if (action==4)
Number = Num1/Num2;
} 



int32_t circle_x = 10;
int32_t circle_y = 20;
int32_t dx = 4;
int32_t dy = 3;
void DisplayResult()
{
tft.fillRect(0, 0, 240, 80, CYAN); //clear result box
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
tft.println(Number); //update new value
} 
void draw_BoxNButtons()
{
//Draw the Result Box
tft.fillRect(0, 0, 240, 80, CYAN); 
//Draw First Column
tft.fillRect (0,260,60,60,RED);
tft.fillRect (0,200,60,60,BLACK);
tft.fillRect (0,140,60,60,BLACK);
tft.fillRect (0,80,60,60,BLACK); 
//Draw Third Column
tft.fillRect (120,260,60,60,GREEN);
tft.fillRect (120,200,60,60,BLACK);
tft.fillRect (120,140,60,60,BLACK);
tft.fillRect (120,80,60,60,BLACK); 
//Draw Secound & Fourth Column
for (int b=260; b>=80; b-=60)
{ tft.fillRect (180,b,60,60,BLUE);
tft.fillRect (60,b,60,60,BLACK);} 
//Draw Horizontal Lines
for (int h=80; h<=320; h+=60)
tft.drawFastHLine(0, h, 240, WHITE); 
//Draw Vertical Lines
for (int v=0; v<=240; v+=60)
tft.drawFastVLine(v, 80, 240, WHITE); 
//Display keypad lables
for (int j=0;j<4;j++) {
for (int i=0;i<4;i++) {
tft.setCursor(22 + (60*i), 100 + (60*j));
tft.setTextSize(3);
tft.setTextColor(WHITE);
tft.println(symbol[j][i]);
}
}
}
