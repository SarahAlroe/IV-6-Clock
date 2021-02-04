#include "Buzzer.h"
#include "Arduino.h"
#include "Structure.h"

Buzzer::Buzzer(int pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Buzzer::play(int melody) {
  if (currentMelody != melody) {
    updateMelody(melody);
  }

  if (nextToneAt < millis()) {
    if (pgm_read_word_near(melodyTones + tonePointer) != 0) {
      tone(pin, pgm_read_word_near(melodyTones + tonePointer), pgm_read_word_near(melodyDelays + tonePointer) );
    } else {
      noTone(pin);
    }
    nextToneAt = millis() + pgm_read_word_near(melodyDelays + tonePointer);
    tonePointer = (tonePointer + 1) % toneCount;
  }
}

void Buzzer::stop() {
  currentMelody = 0;
  int tonePointer = 0;
  noTone(pin);
}

void Buzzer::updateMelody(int melody) {
  currentMelody = melody;
  int tonePointer = 0;
  noTone(pin);
  nextToneAt = millis();
  switch (melody) {
    case 1:
      toneCount = MEL_1_COUNT;
      melodyTones = MEL_1_TONE_ARR;
      melodyDelays = MEL_1_DELAY_ARR;
      break;
    case 2:
      toneCount = MEL_2_COUNT;
      melodyTones = MEL_2_TONE_ARR;
      melodyDelays = MEL_2_DELAY_ARR;
      break;
    case 3:
      toneCount = MEL_3_COUNT;
      melodyTones = MEL_3_TONE_ARR;
      melodyDelays = MEL_3_DELAY_ARR;
      break;
    case 4:
      toneCount = MEL_4_COUNT;
      melodyTones = MEL_4_TONE_ARR;
      melodyDelays = MEL_4_DELAY_ARR;
      break;
  }
}
