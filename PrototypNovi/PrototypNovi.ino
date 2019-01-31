///// DEMO MIT BLUETOOTH OHNE TOUCH - novi /////

#include <SPFD5408_Adafruit_GFX.h>    
#include <SPFD5408_Adafruit_TFTLCD.h> 
#include <SD.h>
#include <SPI.h>

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1  
#define LCD_RD A0 

#define SD_CS 53 

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);

char junk;
String inputString="";
const int motorPin = 10;
const int ledBlink = 12;
int brightness = 0;
int fadeAmount = 5;
int led = 0;
int ledDelay = 10;
String side = "m";
const int buttonU = 22;
const int buttonD = 23;
const int buttonY = 24;
int drawn = 0;
String select = "0"; 
String screen = "1.bmp"; 
String number = "0"; 
String alternative = ""; 
boolean choosed = false; 
int z = 1; 
char slide[10] = "123456789"; 


void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);  
  tft.reset();
  uint16_t identifier = tft.readID();
  
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);
  pinMode(ledBlink, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(buttonU, INPUT);
  pinMode(buttonD, INPUT);
  pinMode(buttonY, INPUT);

  tft.begin(identifier);
  progmemPrint(PSTR("Initializing SD card..."));
    if (!SD.begin(SD_CS)) {
      progmemPrintln(PSTR("failed!"));
      return;
    }
  progmemPrintln(PSTR("OK!"));

  bmpDraw(screen, 0, 0); 
}

void loop()
{
   if (choosed == false) {
    destinations(); 
  }
  else {
    route(); 
  }
if(Serial1.available()){
  while(Serial1.available())
    {
      char inChar = (char)Serial1.read(); 
      inputString += inChar;       
    }
    Serial.println(inputString); 
    
    while (Serial1.available() > 0)  
    { junk = Serial1.read() ; }         
      if (choosed == false) {
        destinations(); 
      }
      else {
        route(); 
      }
    }
}


void ledFade (int i, int fadeDelay, String side) {
  Serial.println("blinking");
  for (int k = 0; k < i; k++) {
    for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
      if (side == "m") {
        analogWrite(ledBlink, fadeValue);      
      }
      else if (side == "r") {
        analogWrite(ledBlink-1, fadeValue);      
      }
      else if (side == "l") {
        analogWrite(ledBlink+1, fadeValue);      
      }
      analogWrite(motorPin, fadeValue);
      delay(fadeDelay);
    }
    for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 15) {
      if (side == "m") {
        analogWrite(ledBlink, fadeValue);      
      }
      else if (side == "r") {
        analogWrite(ledBlink-1, fadeValue);      
      }
      else if (side == "l") {
        analogWrite(ledBlink+1, fadeValue);      
      }
      analogWrite(motorPin, fadeValue);
      delay(fadeDelay);
    }  
  }
}

void destinations () {
        if ((digitalRead(buttonY) == HIGH) || inputString == "y") { 
          if(select == "0") { 
            select = "hda";
            Serial.println(select);
            Serial.println("up");
            drawn = 0;
          }
          else {
            choosed = true; 
            Serial.println(select);
            Serial.println(choosed);     
            ledFade(1, 20, "m");
            number = "0";
            alternative = "a";
            z = 0;
            drawn = 0;
          }
        }
        
        else if(select == "hda"){ 
          if((digitalRead(buttonU) == HIGH) || inputString == "u"){ 
            select = "ffm";
            drawn = 0;
          }
          else if ((digitalRead(buttonD) == HIGH) || inputString == "d") { 
            select = "wda";
            drawn = 0;
          }  
        }

        else if(select == "wda"){    
          if((digitalRead(buttonU) == HIGH) || inputString == "u"){ 
            select = "hda";
            drawn = 0;
          }
          else if ((digitalRead(buttonD) == HIGH) || inputString == "d") {
            select = "ffm";
            drawn = 0;
          }  
        }
        
        else if(select == "ffm"){    
          if((digitalRead(buttonU) == HIGH) || inputString == "u"){ 
            select = "wda";
            drawn = 0;
          }
          else if ((digitalRead(buttonD) == HIGH) || inputString == "d") { 
            select = "hda";
            drawn = 0;
          }   
        }
        if (drawn == 0) {
          if (choosed == false) {
             if (select == "0") {
              screen = "" + select + ".bmp";
            }
            else { 
            screen = "" + select + "_" + alternative + number + ".bmp";
            bmpDraw(screen, 0, 0);
            }
          }
          else if (choosed == true){
            screen = "" + select + "_" + alternative + number + "_1" + ".bmp"; 
            String  screen2 = "" + select + "_" + alternative + number + "_2" + ".bmp"; 
            bmpDraw(screen, 0, 0);
            delay(1000);
            bmpDraw(screen2, 0, 0); 
          }
          Serial.println(select); 
          inputString = "";
          drawn = 1;
        } 
}

void route () {
  if (inputString == "n") { 
          number = slide[z];
            if (alternative == "a") { 
              if ((select == "wda" || select == "ffm") && z == 0) {
                side = "l";
                led = 5;
                ledDelay = 10;
              }  
              else if (select == "hda" &&  (z == 0 || z == 6)) {
                side = "r";
                led = 5;
                ledDelay = 10;               
              }
              else if (((select == "hda" || select == "wda" || select == "ffm") && (z == 1 || z == 4)) || (select == "wda" && z == 7)) {
                side = "m";
                led = 1;
                ledDelay = 20;
              }
              else if ((select == "hda" || select == "wda" || select == "ffm") && z == 2){
                int ranAlternative = floor(random(0,2));
                if(ranAlternative == 1){
                  side = "m";
                  alternative = "b";
                  led = 3;  
                  ledDelay = 10;          
                }
              }
              else if ((select == "hda" && z == 7) || (select == "wda" && z == 8) || (select == "ffm" && z == 5)){
                side = "m";
                alternative = "end";
                number = "";
                led = 3;
                ledDelay = 10;
              }
              else {
                led = 0;
              }
            }
            else if (alternative == "b") {
              if ((select == "hda" && z == 3) || ((select == "wda" || select == "ffm") && z == 4) || (select == "ffm" && z == 7)) {
                side = "m";
                led = 1;
                ledDelay = 20;
              }
              else if ((select == "hda" && z == 5) || (select == "wda" && z == 5) || (select == "ffm" && z == 8)){
                side = "m";
                alternative = "end";
                number = "";
                led = 3;
                ledDelay = 10;
              }   
              else {
                led = 0;
              }   
            }            
           
          if (alternative == "b" && (z == 2 || (select == "hda" && z == 4))) {
            screen = "" + select + "_" + alternative + number + "_1" + ".bmp"; 
            String  screen2 = "" + select + "_" + alternative + number + "_2" + ".bmp"; 
            bmpDraw(screen, 0, 0);
            if (z == 2) {
              ledFade(led, ledDelay, side); 
            }
            delay(2000);
            bmpDraw(screen2, 0, 0); 

          }
          else {
            screen = "" + select + "_" + alternative + number + ".bmp";
            bmpDraw(screen, 0, 0);
            ledFade(led, ledDelay, side); 
          }
          inputString = "";
          z = z + 1;
  }
}

#define BUFFPIXEL 60
void bmpDraw(String filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   
  uint8_t  bmpDepth;              
  uint32_t bmpImageoffset;        
  uint32_t rowSize;               
  uint8_t  sdbuffer[3*BUFFPIXEL]; 
  uint16_t lcdbuffer[BUFFPIXEL];  
  uint8_t  buffidx = sizeof(sdbuffer); 
  boolean  goodBmp = false;       
  boolean  flip    = false;       
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  progmemPrint(PSTR("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  if ((bmpFile = SD.open(filename)) == NULL) {
    progmemPrintln(PSTR("File not found"));
    return;
  }

  if(read16(bmpFile) == 0x4D42) { 
    progmemPrint(PSTR("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile);
    bmpImageoffset = read32(bmpFile); 
    progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    progmemPrint(PSTR("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { 
      bmpDepth = read16(bmpFile); 
      progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { 

        goodBmp = true; 
        progmemPrint(PSTR("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        rowSize = (bmpWidth * 3 + 3) & ~3;

        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { 
          if(flip) 
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else    
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { 
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); 
          }

          for (col=0; col<w; col++) { 
            if (buffidx >= sizeof(sdbuffer)) { 
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; 
            }

            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } 
        } 
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
        progmemPrint(PSTR("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } 
    }
  }

  bmpFile.close();
  if(!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); 
  ((uint8_t *)&result)[1] = f.read(); 
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); 
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); 
  return result;
}

void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}

