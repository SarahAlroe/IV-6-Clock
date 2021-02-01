class Button {
  public:
    Button(int pin);

    bool pressed();

    const static bool PRESSED = LOW;
    const static bool RELEASED = HIGH;

  private:
    const unsigned long READ_DELAY = 50;

    int pin;
    unsigned long readDelayTo;
    bool state = RELEASED;
    bool justChangedState = false;

    bool hasJustChanged();
    bool read();
};

Button::Button(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP);
}

bool Button::pressed() {
  return read() == PRESSED && hasJustChanged();
}

bool Button::read()
{
  if (readDelayTo < millis())
  {
    if (state != digitalRead(pin)) {
      state = ! state;
      readDelayTo = millis() + READ_DELAY;
      justChangedState = true;
    }
  }
  return state;
}

bool Button::hasJustChanged() {
  if (justChangedState) {
    justChangedState = false;
    return true;
  }
  return false;
}
