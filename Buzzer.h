class Buzzer {
  public:
    Buzzer(int pin);
    void playAlarm();
    void stopAlarm();
  private:
    int lastSecond = 0;
    int pin;
};

Buzzer :: Buzzer(int pin) {
  this->pin = pin;
}

void Buzzer :: playAlarm() {
  if (lastSecond != second()) {
    lastSecond = second();
    tone(pin, 440, 500);
  }
}

void Buzzer :: stopAlarm() {
  noTone(pin);
}
