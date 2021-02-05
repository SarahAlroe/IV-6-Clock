#pragma once

int displayDelayCounter = 0;
struct DisplayData currentDisplayData;
const struct DisplayData EMPTY_DISPLAY = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; // A completely empty display
int dataPin, clockPin, latchPin;
int currentBrightnessDelay;

class Display {
  public:
    Display(int dataPin, int clockPin, int latchPin, int ldrPin);

    void display(DisplayData);

  private:
    const int RUNNING_AVG_N = 8; // N factor of running average on LDR readings
    const int TOTAL_DELAY_INTERVAL = 10; // Total interval of screen on/off cycle
    const int MIN_BRIGHT_DELAY = 1; // Lowest possible delay for screen to stay on
    const int LDR_BRIGHT = 800; // Analog value for a bright room. Experimentally chosen
    const int LDR_DARK = 20; // Analog value for a dark room. Experimentally chosen

    int ldrPin;
    int ambientBrightness;
    struct DisplayData targetDisplayData;
    unsigned long lastNow;

    int getBrightnessDelay();
    void pushToDisplay(DisplayData);
};

Display::Display(int inDataPin, int inClockPin, int inLatchPin, int ldrPin) {
  dataPin = inDataPin;
  clockPin = inClockPin;
  latchPin = inLatchPin;
  this->ldrPin = ldrPin;

  // Init pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(ldrPin, INPUT);

  // Seed average
  ambientBrightness = analogRead(ldrPin);

  // TIMER 1 for interrupt frequency 1200.1200120012002 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 1200.1200120012002 Hz increments
  OCR1A = 6665; // = 8000000 / (1 * 1200.1200120012002) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts

}

void Display::display(DisplayData displayData) {
  currentBrightnessDelay = getBrightnessDelay();
  targetDisplayData = displayData;
  if (lastNow + 25 < millis()) {
    lastNow = millis();
    for (int i = 0; i < 6; i++) {
      byte currentByte = currentDisplayData.d[i];
      byte targetByte = targetDisplayData.d[i];
      byte mask = random(256);
      byte diff = currentByte ^ targetByte;
      byte maskedDiff = diff & mask;
      currentByte = currentByte ^ maskedDiff;
      currentDisplayData.d[i] = currentByte;
    }
  }
}

int Display::getBrightnessDelay() {
  // Calculate new running average
  ambientBrightness = (ambientBrightness * (RUNNING_AVG_N - 1) + analogRead(ldrPin)) / RUNNING_AVG_N;
  // Map ambient brightness to display brightness delay interval
  return min(TOTAL_DELAY_INTERVAL, max(MIN_BRIGHT_DELAY, map(ambientBrightness, LDR_DARK, LDR_BRIGHT, MIN_BRIGHT_DELAY, TOTAL_DELAY_INTERVAL)));
}


void pushToDisplay(DisplayData displayData) {
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

ISR(TIMER1_COMPA_vect) {
  displayDelayCounter ++;
  displayDelayCounter = displayDelayCounter % 10;
  if (displayDelayCounter == 0) {
    pushToDisplay(currentDisplayData);
  } else if (displayDelayCounter == currentBrightnessDelay) {
    pushToDisplay(EMPTY_DISPLAY);
  }
}
