#pragma once
class Display {
  public:
    Display(int dataPin, int clockPin, int latchPin, int ldrPin);
    void display(DisplayData);
  private:
    const int RUNNING_AVG_N = 8; // N factor of running average on LDR readings
    const int TOTAL_DELAY_INTERVAL = 10; // Total interval of screen on/off cycle
    const int MIN_BRIGHT_DELAY = 1; // Lowest possible delay for 
    const int LDR_BRIGHT = 900; // Analog value for a bright room. Experimentally chosen
    const int LDR_DARK = 200; // Analog value for a dark room. Experimentally chosen
    const struct DisplayData EMPTY_DISPLAY = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; // A completely empty display
    
    int dataPin, clockPin, latchPin, ldrPin;
    int ambientBrightness;
    
    int getBrightnessDelay();
    void pushToDisplay(DisplayData);
};

Display::Display(int dataPin, int clockPin, int latchPin, int ldrPin){
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->latchPin = latchPin;
  this->ldrPin = ldrPin;

  //Init pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(ldrPin, INPUT);

  //Seed average
  ambientBrightness = analogRead(ldrPin);
}

void Display::display(DisplayData displayData){
  int brightnessDelay = getBrightnessDelay();
  
  pushToDisplay(EMPTY_DISPLAY);
  delay(TOTAL_DELAY_INTERVAL - brightnessDelay);
  
  pushToDisplay(displayData);
  delay(brightnessDelay);
}

int Display::getBrightnessDelay(){
  ambientBrightness = (ambientBrightness * (RUNNING_AVG_N-1) + analogRead(ldrPin)) / RUNNING_AVG_N;
  return min(TOTAL_DELAY_INTERVAL, max(MIN_BRIGHT_DELAY, map(ambientBrightness, LDR_DARK, LDR_BRIGHT, MIN_BRIGHT_DELAY, TOTAL_DELAY_INTERVAL)));
}


void Display::pushToDisplay(DisplayData displayData) {
  // Pushes 6 display bytes to the display
  // Pull latch low to prepare for new data
  digitalWrite(latchPin, LOW);
  for (int i = 0; i < 6; i++) {
    // Each byte for display is pushed inverted by the ~ operator, least significant byte first
    shiftOut(dataPin, clockPin, LSBFIRST, ~ displayData.d[5 - i]);
  }
  // Unlatch to show new data
  digitalWrite(latchPin, HIGH);
}
