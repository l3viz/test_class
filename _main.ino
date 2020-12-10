#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <EEPROM.h>

int ledFeedbackAmount = 5;            // Amount of feedback LEDS

// Leds                               (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800)
Adafruit_NeoPixel ledStripLeft        (1, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledStripMid         (1, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledStripRight       (1, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledStripFeedback    (ledFeedbackAmount, 7, NEO_GRB + NEO_KHZ800);

// Halls
int hallLeft  = A2;
int hallMid   = A0;
int hallRight = A1;

// Colors
uint32_t white     = ledStripFeedback.Color(255, 255, 255);
uint32_t yellow    = ledStripFeedback.Color(255, 255, 0);
uint32_t orange    = ledStripFeedback.Color(255, 60, 0);
uint32_t red       = ledStripFeedback.Color(255, 0, 0);
uint32_t pink      = ledStripFeedback.Color(255, 0, 60);
uint32_t lightblue = ledStripFeedback.Color(0, 125, 255);
uint32_t green     = ledStripFeedback.Color(0, 255, 0);
uint32_t blue      = ledStripFeedback.Color(0, 0, 255);
uint32_t magenta   = ledStripFeedback.Color(255, 0, 255);
uint32_t ledOff    = ledStripFeedback.Color(0, 0, 0);

//Module moduleName(Adafruit_NeoPixel led, uint32_t color, unsigned long timeLedNeedsToStayOn, boolean ledState, int hallBaseValue) {
Module moduleLeft(ledStripLeft, red, 2000, 1, analogRead(hallLeft));

//States
bool stateLeft = 0;

void setup()
{
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  Serial.begin(9600);
  pinMode(hallLeft,   INPUT);
  pinMode(hallMid,    INPUT); 
  pinMode(hallRight,  INPUT); 
  ledStripLeft.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)  
  ledStripMid.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)  
  ledStripRight.begin();          // INITIALIZE NeoPixel strip object (REQUIRED)
  ledStripFeedback.begin();       // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop(){
    Serial.println("LED PINK ON");
    ledStripLeft.setPixelColor(0, pink);
    ledStripLeft.show();

    delay(2000);
    
    moduleLeft.check();
    delay(5000);
    moduleLeft.LedOff();
    delay(3000);
}

void LaunchMenu() {
  while(millis()<10000) {
    
    int midLed = floor(ledFeedbackAmount/2);
    int brightness = 0;

    for(brightness; brightness <= 255; brightness++){
      ledStripFeedback.setPixelColor(midLed, white);
      ledStripFeedback.setBrightness(brightness);
      ledStripFeedback.show();
      delay(5);
    }
    for(brightness = 255; brightness >= 0; brightness--){
      ledStripFeedback.setPixelColor(midLed, white);
      ledStripFeedback.setBrightness(brightness);
      ledStripFeedback.show();
      delay(5);
    }
  }
}
