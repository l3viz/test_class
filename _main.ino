#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <EEPROM.h>
#include "Module.h"

int ledFeedbackAmount = 5;            // Amount of feedback LEDS
int maxScore = ledFeedbackAmount;

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
uint32_t aqua      = ledStripFeedback.Color(0, 125, 255);
uint32_t green     = ledStripFeedback.Color(0, 255, 0);
uint32_t blue      = ledStripFeedback.Color(0, 0, 255);
uint32_t magenta   = ledStripFeedback.Color(255, 0, 255);
uint32_t ledOff    = ledStripFeedback.Color(0, 0, 0);

//Module moduleName(Adafruit_NeoPixel led, uint32_t color, unsigned long timeLedNeedsToStayOn, boolean ledState, int hallBaseValue, long baseTime) {
Module moduleLeft(ledStripLeft, red, 0, 0, analogRead(hallLeft), millis());
Module moduleMid(ledStripMid, green, 0, 0, analogRead(hallMid), millis());
Module moduleRight(ledStripRight, blue, 0, 0, analogRead(hallRight), millis());

// States
bool stateLeft = 0;

//timers
long baseTimeLedStaysOn   = 5500;
long baseTimeBetweenLeds  = 5500;

// Random
int randScore;
int hallMarge             = 5;    //De marge die hall heeft voor hij getriggered wordt. als hij normaal 500 is zal hij dus pas op 505 triggeren
float brightness          = 0;
float directionn          = 1;
int mainMenu = 0;
uint32_t mainMenuArray[]  = {white, red, blue};
int subMenu               = 0;
int maxMenuChoise         = 2;
int level;
int sublevel;
uint32_t levelColors[]  = {white, yellow, orange, red, pink, aqua, green, blue, magenta};
int score;


void setup(){
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  Serial.begin(9600);
  Serial.println("Start initializing parameters");
  pinMode(hallLeft,   INPUT);
  pinMode(hallMid,    INPUT); 
  pinMode(hallRight,  INPUT); 
  ledStripLeft.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)  
  ledStripMid.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)  
  ledStripRight.begin();          // INITIALIZE NeoPixel strip object (REQUIRED)
  ledStripFeedback.begin();       // INITIALIZE NeoPixel strip object (REQUIRED)
  randomSeed(analogRead(3));
  TurnAllLedsOff();
  moduleLeft.SetHallBaseValue(analogRead(hallLeft));
  moduleMid.SetHallBaseValue(analogRead(hallMid));
  moduleRight.SetHallBaseValue(analogRead(hallRight));
  Serial.println("END initializing parameters");
  delay(1000);
}

void loop(){    
  Serial.println("Loop menu");
  Serial.println(random(-100,100));
  //LaunchMenu();
  //Get level from bluetooth app.
  GetLevel();
  Countdown(2000); // how much milliseconds it takes to prepare for game
  moduleLeft.SetHallBaseValue(analogRead(hallLeft));
  moduleMid.SetHallBaseValue(analogRead(hallMid));
  moduleRight.SetHallBaseValue(analogRead(hallRight));

  StartGame(level, sublevel);
  
}

void LaunchMenu() {
  // Start de keuzeleds met een kleur
  ledStripLeft.setPixelColor(0, orange);
  ledStripLeft.setBrightness(100);
  ledStripLeft.show();
  ledStripRight.setPixelColor(0, orange);
  ledStripRight.setBrightness(100);
  ledStripRight.show();
  ledStripMid.setPixelColor(0, green);
  ledStripMid.setBrightness(100);
  ledStripMid.show();
  
  int x = AdjustLeds(white, millis());
  
  switch (x) {
    Serial.print("x: ");
    Serial.println(x);
    case 1:
      Serial.println("Left");
      Serial.print("mainMenu: ");
      Serial.println(mainMenu);
      
      if(mainMenu == 0) {
        mainMenu = maxMenuChoise;
        Serial.print("if");
        Serial.println(mainMenu);
        AdjustLeds(mainMenuArray[mainMenu], millis());
      }
      else{
        mainMenu += -1;
        Serial.print("else");
        Serial.println(mainMenu);
        AdjustLeds(mainMenuArray[mainMenu], millis());
      }
      break;
    case 2:
      Serial.println("Enter");
      break;
    case 3:
      Serial.println("Right");
      if(mainMenu > maxMenuChoise) {
        mainMenu = 0;
        AdjustLeds(mainMenuArray[mainMenu], millis());
      }
      mainMenu += 1;
      AdjustLeds(mainMenuArray[mainMenu], millis());
      break;
  }
  Serial.print("KEUZE GEMAAKT x = ");
  Serial.println(x);
}

int AdjustLeds(uint32_t c, long t) {
  int y = 0;
  while(y == 0) {
    if(t + 250 < millis()) { //Check for 250ms what module is hit.
      Serial.println("CheckWhatModuleIsHit");
      y = CheckWhatModuleIsHit();
    }
    int midLed = floor(ledFeedbackAmount/2);
    ledStripFeedback.setPixelColor(midLed, c);
    ledStripFeedback.setBrightness(brightness);
    ledStripFeedback.show();
    
    if (brightness < 0 || brightness >= 255) {
      directionn *= -1;
    }
    brightness += directionn/4;
  }
  Serial.println("Uit while");
  brightness = 0;
  ledStripFeedback.setBrightness(brightness);
  ledStripFeedback.show();
  return y;
}

int CheckWhatModuleIsHit(){
  Serial.println("Check if there is a hit");
  long x = millis();
  
  if (analogRead(hallLeft) > (moduleLeft.GetHallBaseValue() + hallMarge)) {
    int max = moduleLeft.GetHallBaseValue();
    int min = moduleLeft.GetHallBaseValue();
    while(millis() < x + 50) {
      if( analogRead(hallLeft) > max){
        max = analogRead(hallLeft);
      } else  if( analogRead(hallLeft) < min){
        min = analogRead(hallLeft);
      }
    }
    if(moduleLeft.GetLedState() == 0){
      CalculateScore(-1);
    }
    else{
      CalculateScore(1);
    }
    return 1;
  }
  if (analogRead(hallMid) > (moduleMid.GetHallBaseValue() + hallMarge)) {
    return 2;
  }
  if (analogRead(hallRight) > (moduleRight.GetHallBaseValue() + hallMarge)) {
    return 3;
  }
  return 0;
}

void TurnAllLedsOff() {
  ledStripFeedback.setPixelColor(0, 0, 0, 0);
  ledStripFeedback.show();
  ledStripLeft.setPixelColor(0, 0, 0, 0);
  ledStripLeft.show();  
  ledStripRight.setPixelColor(0, 0, 0, 0);
  ledStripRight.show();  
  ledStripMid.setPixelColor(0, 0, 0, 0);
  ledStripMid.show();
}


void LaunchMenu2() {
  Serial.println("Inside Start keuzemenu");
  
  while(CheckWhatModuleIsHit() == 0) {
    int midLed = floor(ledFeedbackAmount/2);
    int brightness = 0;

    for(brightness; brightness <= 255; brightness++){
      if(CheckWhatModuleIsHit() != 0)
        break;
      ledStripFeedback.setPixelColor(midLed, white);
      ledStripFeedback.setBrightness(brightness);
      ledStripFeedback.show();
      delay(5);
    }
    for(brightness = 255; brightness >= 0; brightness--){
      if(CheckWhatModuleIsHit() != 0)
        break;
      ledStripFeedback.setPixelColor(midLed, white);
      ledStripFeedback.setBrightness(brightness);
      ledStripFeedback.show();
      delay(5);
    }
  }

  Serial.println("KEUZE GEMAAKT");
  delay(5000);
}

void GetLevel() {
  level = 5;
  sublevel = 3;
}

void Countdown(long x) {
  x = millis() + x;
  while(x > millis()) {
    ledStripFeedback.fill(orange, 0, 2); // kleur, eerste led, hoeveel leds
    ledStripFeedback.setBrightness(20);
    ledStripFeedback.show();
  }
    ledStripFeedback.fill();
    ledStripFeedback.show();
    delay(500);
    ledStripFeedback.fill(orange, 0, 2); // kleur, eerste led, hoeveel leds
    ledStripFeedback.setBrightness(250);
    ledStripFeedback.show();
    delay(100);
    ledStripFeedback.fill();
    ledStripFeedback.show();
    delay(400);
}

void StartGame(int x, int y) {
  //level = 5    sublevel = 3
  Serial.println("Start Game");
  Serial.println(x);
  Serial.println(y);

  long TimeBetweenLeds = baseTimeBetweenLeds-((x*500)+(y*25));
  
  int score = y;
  // Light correct leds
  //First led has color of previous level
  ledStripFeedback.setPixelColor(0, levelColors[x-2]);  

  //Last led has color of next level
  ledStripFeedback.setPixelColor(ledFeedbackAmount-1, levelColors[x]);

  //Fill other leds till current sublevel
  ledStripFeedback.setPixelColor(y-1, levelColors[x-1]);
  ledStripFeedback.setBrightness(40);
  ledStripFeedback.show();
  delay(5000);

  // Run programas long as score is not 0 or score is not end of leds
  while(score < 0 || score <  ledFeedbackAmount) {
    Serial.println("make New Module");
    Module q;
    int r = random(1,100);
    if(r<33){q = moduleLeft;}
      else if(r<66){q = moduleMid;}
        else{q = moduleRight;}
    
    Serial.print("Random: ");
    Serial.println(r);
    delay(5000);
    if(r > 95) {
      // led, color, timeLedNeedsToStayOn, ledState, hallBaseValue, baseTime
      q.SetColor(magenta);
    } else if (r < 10) {
      q.SetColor(red);
    } else {
      q.SetColor(green);
    }
      q.SetTimeLedOn(2000);
      q.SetLedState(1);
      q.SetHallBaseValue(analogRead(hallLeft));
      q.SetBaseTime(millis());
        
    //If we didnt pass the point of making new module, just check if we got a hit
    while(millis() < TimeBetweenLeds + millis() ) {
      int hit = CheckWhatModuleIsHit();
      Serial.println(hit);
      if(hit){
      }
    }
    
    //MakeRandomModule();
    Serial.print("MakeRandomModule(");
    Serial.print(TimeBetweenLeds+random(-125,125));
    Serial.println(")");
  }
    
}

void CalculateScore(int x) { 
  if(x == 1){
    if(score < maxScore){
      ++score;
    }
    else {
      Serial.println("****************** Max score bereikt, ga naar volgende level ******************");
    }
  }
  if(x == -1){
    if(score > 0){
      --score;
    }
    else {
      Serial.println("****************** Score 0, ga een level terug  ******************");
    }
  }
}


