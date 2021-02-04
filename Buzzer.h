#pragma once

class Buzzer {
  public:
    int MELODY_COUNT = 4;
    Buzzer(int pin);

    void play(int melody);
    void stop();

  private:
    int pin;
    unsigned int tonePointer = 0;
    unsigned long nextToneAt = 0;
    unsigned int currentMelody = 0;
    unsigned int toneCount;
    unsigned int *melodyTones;
    unsigned int *melodyDelays;

    void updateMelody(int melody);
};
