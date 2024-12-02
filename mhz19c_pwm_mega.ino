#include "MHZCO2.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c //0x76 //0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//#define pwmPin D5//3
#define pwmPin 3
#define LedPin 13

long prevVal = LOW;
long th, tl, h, l, ppm;

//  adjust to calibrate.
const float MAX_CONCENTRATION = 2000.0;

unsigned long startMillis; // Stores the starting time
unsigned long elapsedTime; // Stores the elapsed time
const unsigned long oneHour = 3600000; // 1 hour in milliseconds (60*60*1000)

volatile uint16_t width;

void showCo2Ppm(int co2);

void IRQ()
{
  static uint32_t start = 0;
  int v = digitalRead(pwmPin);
  if (v == HIGH) start = millis();
  else width = millis() - start;
}

uint16_t PWM_concentration()
{
  noInterrupts();
  uint16_t TimeHigh = width;  //  milliseconds
  interrupts();

  uint16_t concentration = round(((TimeHigh - 2) * MAX_CONCENTRATION) * 0.001);
  return concentration;
}

void setup() {
  Serial.begin(115200);
  pinMode(pwmPin, INPUT);
  pinMode(LedPin, OUTPUT);

  startMillis = millis(); // Record the start time

  /**
   * pin 7 used for regular reset due to OLED display gets blunk
   */
  pinMode(7, OUTPUT); // Set pin 7 as an output
  digitalWrite(7, HIGH); // Ensure it's high

  Serial.println(__FILE__);
  Serial.print("MHZCO2_LIB_VERSION: ");
  Serial.println(MHZCO2_LIB_VERSION);
  
  attachInterrupt(digitalPinToInterrupt(pwmPin), IRQ, CHANGE);

  

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

long tt_show = 0;

void loop() {

  long tt = millis();
  elapsedTime = millis() - startMillis; // Calculate elapsed time
  
  int myVal = digitalRead(pwmPin);

  //Если обнаружили изменение
  if (myVal == HIGH) {
    digitalWrite(LedPin, HIGH);
    if (myVal != prevVal) {
      h = tt;
      tl = h - l;
      prevVal = myVal;
    }
  }  else {
    digitalWrite(LedPin, LOW);
    if (myVal != prevVal) {
      l = tt;
      th = l - h;
      prevVal = myVal;
      ppm = 5000 * (th - 2) / (th + tl - 4);
      Serial.println("PPM = " + String(ppm));
      if (tt - tt_show > 3000) {
        tt_show = tt;
        //showCo2Ppm(ppm - 3000);
        showCo2Ppm(ppm);
      }
    }
  }
  //delay(1000);
      if (elapsedTime >= oneHour) {
        Serial.println("1 hour has passed!");
        // Reset the timer or perform your desired action
        startMillis = millis(); // Reset the timer if needed
        resetMega();
    } 
}

void __loop()
{
  ppm = PWM_concentration();
  Serial.println(ppm);
  showCo2Ppm(ppm);
  delay(1000);
}



void showCo2Ppm(int co2) {

  display.clearDisplay();
  display.setTextSize(6);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
//  display.print("CO2");
//  display.display();
  //delay(500);
/*
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("PPM");
  display.display();
  //delay(500);
*/
  display.clearDisplay();
  (co2 > 1000) ? display.setTextSize(4) : display.setTextSize(6);
  display.setCursor(0, 10);
  display.print(co2);
  display.display();
  //delay(2000);
}

void resetMega(void) {
    Serial.println("Resetting...");
    digitalWrite(7, LOW); // Pull reset pin low
    delay(100); // Hold reset pin low for 100ms
    digitalWrite(7, HIGH); // Release reset pin
}
