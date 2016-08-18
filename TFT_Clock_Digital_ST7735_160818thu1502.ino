/*
  An example digital clock using a TFT LCD screen to show the time.
  Demonstrates use of the font printing routines. (Time updates but date does not.)

  For a more accurate clock, it would be better to use the RTClib library.
  But this is just a demo.

  This examples uses the hardware SPI only. Non-hardware SPI
  is just too slow (~8 times slower!)

  Based on clock sketch by Gilchrist 6/2/2014 1.0
  Updated by Bodmer
  A few colour codes:

  code	color
  0x0000	Black
  0xFFFF	White
  0xBDF7	Light Gray
  0x7BEF	Dark Gray
  0xF800	Red
  0xFFE0	Yellow
  0xFBE0	Orange
  0x79E0	Brown
  0x7E0	Green
  0x7FF	Cyan
  0x1F	Blue
  0xF81F	Pink

*/

#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
//#include <NewPing.h>

#define TRIG_PIN 4
#define ECHO_PIN 3
#define MAX_DIST 200

#define LED_PIN 6

//char recvChr; // received char
boolean newData = false;

const byte numChars = 32;
char receivedChars[numChars]; // array to store the data

int stat = 0; // 0 is initiated, 1 is ready to send data


//NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);

TFT_ST7735 tft = TFT_ST7735();  // Invoke library, pins defined in User_Setup.h

uint32_t targetTime = 0;       // for next 1 second timeout

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK); // Note: the new fonts do not draw the background colour

  targetTime = millis() + 1000;

  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);

  stat = 1; // device is ready to send data
  Serial.println("Arduino is ready");
  Serial.println(stat);
}

void loop() {
  if (targetTime < millis()) {

    //    unsigned int uS = sonar.ping();
    //    String distance = F("Ping: ");
    //    int dist = uS / US_ROUNDTRIP_CM;
    //    distance = distance + dist + "cm \n";
    //    String output = output + distance;
    unsigned long duration;
    int distance;
    digitalWrite(TRIG_PIN, HIGH);
    delay(1);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH, 5700); // microsec timeout if dist > 100cm
    distance = duration / 57;
    //Serial.println(distance);
    //Serial.println(stat); // show status instead of sending the data
    Serial.println(receivedChars);

    // indicator LED
    if (distance == 0) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }

    targetTime = millis() + 1000;
    ss++;              // Advance second
    if (ss == 60) {
      ss = 0;
      omm = mm;
      mm++;            // Advance minute
      if (mm > 59) {
        mm = 0;
        hh++;          // Advance hour
        if (hh > 23) {
          hh = 0;
        }
      }
    }

    if (ss == 0 || initial) {
      initial = 0;
      tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
      tft.setCursor (8, 52);
      tft.print(__DATE__); // This uses the standard ADAFruit small font

      tft.setTextColor(ST7735_BLUE, ST7735_BLACK);
      tft.drawCentreString("It is windy", 120, 48, 2); // Next size up font 2
      //tft.setTextColor(0xF81F, ST7735_BLACK); // Pink
      //tft.drawCentreString("12.34",80,100,6); // Large font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 . : a p m
    }

    // Update digital time
    byte xpos = 6;
    byte ypos = 0;
    if (omm != mm) { // Only redraw every minute to minimise flicker
      // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
      tft.setTextColor(0x39C4, ST7735_BLACK);  // Leave a 7 segment ghost image, comment out next line!
      //tft.setTextColor(ST7735_BLACK, ST7735_BLACK); // Set font colour to back to wipe image
      // Font 7 is to show a pseudo 7 segment display.
      // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
      tft.drawString("88:88", xpos, ypos, 7); // Overwrite the text to clear it
      tft.setTextColor(0xFBE0, ST7735_BLACK); // Orange
      omm = mm;

      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 7);
      xpos += tft.drawNumber(hh, xpos, ypos, 7);
      xcolon = xpos;
      xpos += tft.drawChar(':', xpos, ypos, 7);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 7);
      tft.drawNumber(mm, xpos, ypos, 7);
    }

    if (ss % 2) { // Flash the colon
      tft.setTextColor(0x39C4, ST7735_BLACK);
      xpos += tft.drawChar(':', xcolon, ypos, 7);
      tft.setTextColor(0xFBE0, ST7735_BLACK);
    }
    else {
      tft.drawChar(':', xcolon, ypos, 7);
      colour = random(0xFFFF);
      // Erase the text with a rectangle
      tft.fillRect (0, 64, 160, 20, ST7735_BLACK);
      tft.setTextColor(colour);
      tft.drawRightString("Colour", 75, 64, 4); // Right justified string drawing to x position 75
      String scolour = String(colour, HEX);
      scolour.toUpperCase();
      char buffer[20];
      scolour.toCharArray(buffer, 20);
      tft.drawString(buffer, 82, 64, 4);

      tft.fillRect (0, 84, 160, 40, ST7735_BLACK); // x, y, width, height
      tft.setTextColor(colour);
      tft.setCursor (8, 84);
      //      tft.println(output);
      tft.setTextColor(0x7E0);
      String output = "ping: " + String(distance) + " cm";
      tft.println(output);
      //      String input;
      //      if (Serial.available()) {
      //        input = Serial.read();
      //        Serial.println(input);
      //      }
      //      tft.setCursor (8, 92);
      //      tft.println(input);
    }
  }
  recvStartEnd();
  showData();
}

void recvStartEnd() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        digitalWrite(LED_PIN, LOW);
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
      digitalWrite(LED_PIN, HIGH);
    }
  }  // end of while
}

//void recvMark() {
//  static byte ndx = 0;
//  //char endMarker = '\n';
//  char endMarker = '>';
//  char rc;
//
//  while (Serial.available() > 0 && newData == false) {
//    rc = Serial.read();
//
//    if (rc != endMarker) {
//      receivedChars[ndx] = rc;
//      ndx++;
//      if (ndx >= numChars) {
//        ndx = numChars - 1;
//      }
//
//    } else {
//      receivedChars[ndx] = '\0'; // terminate the string
//      ndx = 0;
//      newData = true;
//    }
//  } // end of while
//}

//void recvChar() {
//  if (Serial.available()) {
//    char recvChr = Serial.read();
//    Serial.println(recvChr);
//    newData = true;
//  }
//}

void showData() {
  if (newData == true) {
    Serial.print("input: ");
    //Serial.println(recvChr);
    Serial.println(receivedChars);
    
    tft.setCursor (8, 92);
    tft.setTextColor(0x7E0);
    tft.println(receivedChars);
    newData = false;
  }
}
