#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET 28

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define YP A1
#define XM A2
#define YM 7
#define XP 6

short TS_MINX=143;
short TS_MINY=147;
short TS_MAXX=991;
short TS_MAXY=968;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXWIDTH 80
#define BOXHEIGHT 99
#define PENRADIUS 3

#define MINPRESSURE 10
#define MAXPRESSURE 1000

bool change = 0;
int clicks = 0;
int steps = 1;
int gamestate = 1;
int *sequence; // declare a pointer to an integer
int sequenceSize = 0;

int randomNumber;
int score = -1;
char scoreChar[10];
int jeuxinit = 1;

char data='e';
char incomingByte;

///carreau
class Block 
{
  public:
  int x;
  int y;
  int width;
  int height;
  int colour;
  int number;
  boolean isActive;
   
      // default constructor
      Block(){}
       
      Block(int xpos, int ypos, int bwidth, int bheight, int bcol, int bnumber)
      {
        x = xpos;
        y = ypos;
        width = bwidth;
        height = bheight;
        colour = bcol;
        number = bnumber;
        isActive = true;
        if(bnumber < 10 && bnumber > 0)
        {
          drawBlock();
        }
        
      }
       
      void fillBlock(int col)
      {
        tft.fillRect(x,y,width,height,col);
      }

      void drawBlock()
      {
        tft.drawRect(x,y,width,height,colour);
      }

};

Block box[9];
Block startBox;
///////////////////////////////////////////////////////////////////////////////
void setup() {

  Serial.begin(19200);
        tft.reset();
        tft.begin(0x9341);
        tft.fillScreen(BLACK);
        lcd.init();
        lcd.init();
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("                ");

      while(score == -1)
      {
        if (Serial.available())
        {     
           String binaryString = Serial.readString();
  
           //Convert the binary string to an integer
           score = strtol(binaryString.c_str(), NULL, 10);    
           //data = Serial.read();
           Serial.println(score);
           if(score!=0) 
          {
             lcd.backlight();
             lcd.setCursor(0,0);
             lcd.print("Vous avez deja");
             lcd.setCursor(0,1);
             lcd.print("                ");
             lcd.setCursor(0,1);
             lcd.print("un score: ");
             lcd.setCursor(11,1);
             lcd.print(score);
             //delay(2000);
           }  
           else if(score == 0)
           {
             lcd.backlight();
             lcd.setCursor(0,0);
             lcd.print("Nouvelle partie");
             lcd.setCursor(0,1);
             lcd.print("                ");

             //delay(2000);
           }
        }      
      }
        pinMode(A5, OUTPUT);
        digitalWrite(A5, LOW);
        
      
        
       
      
        
       //////////random number//////////
        randomSeed(analogRead(0));
        
         tft.fillScreen(BLACK);
        
        tft.reset();
        tft.begin(0x9341);
        tft.fillScreen(WHITE);
        startBox.drawBlock();
        tft.setCursor (55, 110);
        tft.setTextSize (4);
        tft.setTextColor(RED);  
        tft.println("PRESS");
                            
        tft.setCursor (40, 170);
        tft.setTextSize (4);
        tft.setTextColor(RED);
        tft.println("TO PLAY");
                        
        
      
        startBox = Block(0, 0, 240, 320, RED, 0);
        
}

void loop() {

  while(gamestate == 0)
  {
          clicks=0;
      
      sequenceSize++;
      sequence = (int *)realloc(sequence, sequenceSize * sizeof(int)); // increase the size of the array to 10 integers
    
         int randomNumber = random(9)+1;
      do
      {
        randomNumber = random(9)+1;
      }while(sequence[sequenceSize-2] == randomNumber);
      
        sequence[sequenceSize-1] = randomNumber;

      ///////////////////////////////////////////
      for(int i=0; i < sequenceSize; i++)
      {
        for(int y=0; y < 9; y++)
        {
          if(sequence[i]==box[y].number)
          {
            
            box[y].fillBlock(BLUE);
            delay(700);
            box[y].fillBlock(BLACK);
            box[y].drawBlock();
           }
        }
      }
      for(int i=1; i < sequenceSize+1; i++)
       {
        while(clicks<i && gamestate == 0)
        {
          /////////////Touch detection/////////////
          TSPoint p = ts.getPoint();
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
          {
              int temp = p.x;
              p.x = p.y;
              p.y = temp;
              
              p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
              p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());
            
            
              for(int y=0; y < 9; y++)
              {
                if(sequence[i-1]==box[y].number)
                {
                  if(p.x > box[y].x && p.x < (box[y].x+box[y].width))
                  {
                      
                    if(p.y > box[y].y && p.y < (box[y].y+box[y].height))
                    {
                      
                      box[y].fillBlock(GREEN);
                      delay(500);
                      box[y].fillBlock(BLACK);
                      box[y].drawBlock();
                      
                      clicks++;
                    }
                    
                  }else
                  {
                      tft.setCursor (55, 110);
                      tft.setTextSize (7);
                      tft.setTextColor(RED);  
                      tft.println("YOU");
                      
                      tft.setCursor (40, 170);
                      tft.setTextSize (7);
                      tft.setTextColor(RED);
                      tft.println("LOSE");
                      Serial.flush();
                      
                      
                      gamestate = 1;
                      data='e';
                      delay(2000);

                      lcd.backlight();
                      lcd.setCursor(0,0);
                      lcd.print("Votre score est :");
                      lcd.setCursor(0,1);
                      lcd.print("                ");
                      
                      
                      delay(100);
                      itoa(score,scoreChar,10);
                      Serial.write(scoreChar);
                      delay(100);

                      //String binaryScore = Serial.readString();                    
                      //score = strtol(binaryScore.c_str(), NULL, 10);   
                      lcd.setCursor(7,1);
                      lcd.print(score);

                      score = 0;

                      tft.reset();
                      tft.begin(0x9341);
                      
                      tft.fillScreen(WHITE);
                      startBox.drawBlock();
                      tft.setCursor (55, 110);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);  
                      tft.println("PRESS");
                      
                      tft.setCursor (40, 170);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);
                      tft.println("TO PLAY");
                  }
                }
              }
            }
            if(clicks==sequenceSize)
            {
              tft.setCursor (10, 300);
              tft.setTextSize (2);
              tft.setTextColor(BLACK);
              tft.print("Score: "); tft.print(score);
                      
              score = score + 50;
              tft.setCursor (10, 300);
              tft.setTextSize (2);
              tft.setTextColor(WHITE);
              tft.print("Score: "); tft.print(score);
              delay(1000);
            }
            if(score == 1000)
            {
                      tft.setCursor (55, 110);
                      tft.setTextSize (4);
                      tft.setTextColor(GREEN);  
                      tft.println("YOU WIN");
                      itoa(score,scoreChar,10);
                      Serial.write(scoreChar);
                      delay(2000);

                      lcd.backlight();
                      lcd.setCursor(0,0);
                      lcd.print("Votre score est :");
                      lcd.setCursor(0,1);
                      lcd.print("     1000       ");
                      
                      lcd.backlight();
                      lcd.setCursor(0,0);
                      lcd.print("                ");
                      lcd.setCursor(7,0);
                      lcd.print("YOU");
                      lcd.setCursor(0,1);
                      lcd.print("                ");
                      lcd.setCursor(7,0);
                      lcd.print("WIN");

                      tft.reset();
                      tft.begin(0x9341);
                      
                      tft.fillScreen(WHITE);
                      startBox.drawBlock();
                      tft.setCursor (55, 110);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);  
                      tft.println("PRESS");
                      
                      tft.setCursor (40, 170);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);
                      tft.println("TO PLAY");
                      gamestate=1;
                                                  
            }
            
          }
        
      }
  }
  
  while(gamestate == 1)
  {
    
    TSPoint p = ts.getPoint();
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);

       
        if (Serial.available())
        {     
           String binaryString = Serial.readString();
  
          // Convert the binary string to an integer
          score = strtol(binaryString.c_str(), NULL, 10);    
           //data = Serial.read();
           if(score!=0) 
          {
             lcd.backlight();
             lcd.setCursor(0,0);
             lcd.print("Vous avez deja1");
             lcd.setCursor(0,1);
             lcd.print("un score: ");
             lcd.setCursor(11,1);
             lcd.print(score);
             tft.reset();
             tft.begin(0x9341);
             //delay(2000);
                      tft.fillScreen(WHITE);
                      startBox.drawBlock();
                      tft.setCursor (55, 110);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);  
                      tft.println("PRESS");
                      
                      tft.setCursor (40, 170);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);
                      tft.println("TO PLAY");             
           }  
           else
           {
             lcd.backlight();
             lcd.setCursor(0,0);
             lcd.print("Nouvelle parti1");
             lcd.setCursor(0,1);
             lcd.print("                ");
             tft.reset();
             tft.begin(0x9341);
             delay(2000);
                      tft.fillScreen(WHITE);
                      startBox.drawBlock();
                      tft.setCursor (55, 110);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);  
                      tft.println("PRESS");
                      
                      tft.setCursor (40, 170);
                      tft.setTextSize (4);
                      tft.setTextColor(RED);
                      tft.println("TO PLAY");             
           }

        }      
                      
          if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
          {
            gamestate = 0;
            score = 0;
            randomSeed(analogRead(0)); //init sequence de nombre aleat
            tft.fillScreen(BLACK);
            box[0] = Block(0, 0, BOXWIDTH, BOXHEIGHT, WHITE, 1);
            box[1] = Block(BOXWIDTH, 0, BOXWIDTH, BOXHEIGHT, WHITE, 2);
            box[2] = Block(BOXWIDTH*2, 0, BOXWIDTH, BOXHEIGHT, WHITE, 3);
            box[3] = Block(0, BOXHEIGHT, BOXWIDTH, BOXHEIGHT, WHITE, 4);
            box[4] = Block(BOXWIDTH, BOXHEIGHT, BOXWIDTH, BOXHEIGHT, WHITE, 5);
            box[5] = Block(BOXWIDTH*2, BOXHEIGHT, BOXWIDTH, BOXHEIGHT, WHITE, 6);
            box[6] = Block(0, BOXHEIGHT*2, BOXWIDTH, BOXHEIGHT, WHITE, 7);
            box[7] = Block(BOXWIDTH, BOXHEIGHT*2, BOXWIDTH, BOXHEIGHT, WHITE, 8);
            box[8] = Block(BOXWIDTH*2, BOXHEIGHT*2, BOXWIDTH, BOXHEIGHT, WHITE, 9);
            sequenceSize = 0;
              sequence = (int *)realloc(sequence, sequenceSize * sizeof(int));
          }
   }
  }
