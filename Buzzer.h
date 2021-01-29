class Buzzer {
  public:
    Buzzer(int pin);

    void playAlarm();
    void stopAlarm();

  private:
    int pin;
    int lastSecond = 0;
};

Buzzer::Buzzer(int pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Buzzer::playAlarm() {
  if (lastSecond != second()) {
    lastSecond = second();
    tone(pin, 440, 500);
  }
}

void Buzzer::stopAlarm() {
  noTone(pin);
}
