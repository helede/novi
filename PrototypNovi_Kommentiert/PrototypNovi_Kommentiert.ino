///// DEMO MIT BLUETOOTH OHNE TOUCH - novi /////

///// Libaries /////
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Library für das LCD-Display
#include <SD.h> // Libraries für die SD-Karte in dem Dispaly
#include <SPI.h>

///// LCD Pins /////
#define LCD_CS A3 // Chip Select LCD
#define LCD_CD A2 // Command/Data LCD (LCD_RS auf dem Shield)
#define LCD_WR A1 // LCD Write 
#define LCD_RD A0 // LCD Read

///// TFT Display Pins  für Arduino Mega /////
//   D0 - pin 8  
//   D1 - pin 9  
//   D2 - pin 2
//   D3 - pin 3
//   D4 - pin 4
//   D5 - pin 5
//   D6 - pin 6
//   D7 - pin 7

#define SD_CS 53 // SD Chip Select Pin
// BMP-Files für die SD-Karte: 24 bit color BMP files

///// TFT Display wiring
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);

///// Eingabe-Variablen zur Bluetooth-Steuerung /////
char junk;
String inputString="";

///// Licht und Vibration /////
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


///// Variablen /////
String select = "0"; // ausgewählte Route
String screen = "1.bmp"; //bmpbezeichnung
String number = "0"; //Nummer des bmps in Routenabfolge
String alternative = ""; //Alternative a oder b
boolean choosed = false; //noch keine Route ausgewählt
int z = 0; // Index des bmp-Arrays
char slide[10] = "123456789"; //bmp-Nummer Array


void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);  // Serial1 TX1 und RX1 auf dem Mega, damit TX und RX nicht mehr bei jedem Codeupload abgezogen werden müssen
  tft.reset();
  uint16_t identifier = tft.readID();
  
  ///// Chip Select SD Setup /////
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);

  ///// Light und Vibration/////
  pinMode(ledBlink, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(buttonU, INPUT);
  pinMode(buttonD, INPUT);
  pinMode(buttonY, INPUT);

  tft.begin(identifier);

  ///// SD - Initialisierung /////
  progmemPrint(PSTR("Initializing SD card..."));
    if (!SD.begin(SD_CS)) {
      progmemPrintln(PSTR("failed!"));
      return;
    }
  progmemPrintln(PSTR("OK!"));

  bmpDraw(screen, 0, 0); //Startbildschirm = Uhr
}


///// DEMO-LOGIK /////
void loop()
{
   if (choosed == false) {
    destinations(); //Funktion, zur Anzeige und Auswahl der einzelnen Routen
  }
  else {
    route(); //Funktion zum Ablauf der ausgewählten Route
  }
if(Serial1.available()){
  while(Serial1.available())
    {
      char inChar = (char)Serial1.read(); //liest den Buchstaben-input
      inputString += inChar;        //String aus Eingabe-Buchstaben wird generiert
    }
    Serial.println(inputString); 
    
    while (Serial1.available() > 0)  
    { junk = Serial1.read() ; }          // clear the Serial1 buffer BT
      if (choosed == false) {
        destinations(); //Funktion, zur Anzeige und Auswahl der einzelnen Routen
      }
      else {
        route(); //Funktion zum Ablauf der ausgewählten Route
      }
    }
}

///// DEMO-LOGIK /////


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
          if((digitalRead(buttonU) == HIGH) || inputString == "u"){ // up
            select = "ffm";
            drawn = 0;
          }
          else if ((digitalRead(buttonD) == HIGH) || inputString == "d") { // down
            select = "wda";
            drawn = 0;
          }  
        }

        else if(select == "wda"){    
          if((digitalRead(buttonU) == HIGH) || inputString == "u"){ // up
            select = "hda";
            drawn = 0;
          }
          else if ((digitalRead(buttonD) == HIGH) || inputString == "d") { // down
            select = "ffm";
            drawn = 0;
          }  
        }
        
        else if(select == "ffm"){    
          if((digitalRead(buttonU) == HIGH) || inputString == "u"){ // up
            select = "wda";
            drawn = 0;
          }
          else if ((digitalRead(buttonD) == HIGH) || inputString == "d") { // down
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
  if (inputString == "n") { //next
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
          }  
          if (alternative == "b" && (z == 2 || (select == "hda" && z == 4))) {
            screen = "" + select + "_" + alternative + number + "_1" + ".bmp"; 
            String  screen2 = "" + select + "_" + alternative + number + "_2" + ".bmp"; 
            bmpDraw(screen, 0, 0);
            if (z == 2) {
              ledFade(led, ledDelay, side); // licht und vibration
            }
            delay(2000);
            bmpDraw(screen2, 0, 0); 

          }
          else {
            screen = "" + select + "_" + alternative + number + ".bmp";
            bmpDraw(screen, 0, 0);
            ledFade(led, ledDelay, side); // licht und vibration
          }
          inputString = "";
          z = z + 1;
}
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 60
void bmpDraw(String filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = false;        // BMP is stored bottom-to-top
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
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    progmemPrintln(PSTR("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    progmemPrint(PSTR("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    progmemPrint(PSTR("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        progmemPrint(PSTR("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
        progmemPrint(PSTR("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to Serial1 port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}

