/*
 * MODULE CLASS DECLARATION
 */
#include <Adafruit_NeoPixel.h>

class Module {
  private:
    Adafruit_NeoPixel &led;
    boolean ledState;
    unsigned long timeLedNeedsToStayOn;
    uint32_t color;
    int hallBaseValue;

  public:
    Module(Adafruit_NeoPixel &led, uint32_t color, unsigned long timeLedNeedsToStayOn, boolean ledState, int hallBaseValue) {
      SetLed(led);
      SetColor(color);
      SetTimeLedOn(timeLedNeedsToStayOn);
      SetLedState(ledState);
      SetHallBaseValue(hallBaseValue);
    }


//------------  Setters ----------------
    void SetLed(Adafruit_NeoPixel &l) {
      led = l;
    }
    void SetColor(uint32_t c) {
      color = c;
    }
    void SetTimeLedOn(long t) {
      timeLedNeedsToStayOn = t;
    }
     void SetLedState(bool ls) {
      ledState = ls;
    }
    void SetHallBaseValue(int h) {
      hallBaseValue = h;
    }

//------------  Getters ----------------
    Adafruit_NeoPixel GetLed() {
      return led;
    }    
    uint32_t GetColor() {
      return color;
    }
    long GetTimeLedOn() {
      return timeLedNeedsToStayOn;
    }    
    bool GetLedState() {
      return ledState;
    }
    int GetHallBaseValue() {
      return hallBaseValue;
    }
    

//------------  Setters ----------------
    // Checks whether it is time to turn on or off the LED.
    void check() {
      led.setPixelColor(0, color);
      led.show();
    }

    void LedOff() {
      Serial.println("Led OFF");
      led.setPixelColor(0, 0, 0, 0);
      led.show();
    }
};
