#include <DS3232RTC.h>
#include "Structure.h"
#include "Button.h"
#include "Alarm.h"
#include "Alarms.h"
#include "Display.h"
#include "ScreenManager.h"
#include "Screen.h"
#include "Buzzer.h"

// Pin configuration
const int LATCH_PIN = A2;  // ST_CP / rclk on shift register
const int CLOCK_PIN = A1;  // SH_CP / srclk on shift register
const int DATA_PIN = A3;   // DS / data on shift register
const int LDR_PIN = A0;    // LDR pin
const int BUZZER_PIN = 13; // Buzzer pin
const int L_BUTTON_PIN = 2; // Left button pin
const int M_BUTTON_PIN = 3; // Middle button pin
const int R_BUTTON_PIN = 4; // Right button pin

// Display
Display display(DATA_PIN, CLOCK_PIN, LATCH_PIN, LDR_PIN);

// Buzzer
Buzzer buzzer(BUZZER_PIN);

// Buttons
Button lButton(L_BUTTON_PIN);
Button mButton(M_BUTTON_PIN);
Button rButton(R_BUTTON_PIN);

// Screens
ScreenManager screenManager;

// Alarms
Alarms * alarms;

void setup() {
  // Init clock
  setSyncProvider(RTC.get);

  // Init serial
  Serial.begin(9600);

  // Init alarms
  alarms = alarms->getInstance();
}

void loop() {
  // Check alarms
  if (alarms -> isActive()) {
    buzzer.playAlarm();
    if (mButton.pressed()) {
      alarms -> stop();
      buzzer.stopAlarm();
    }
    else if (lButton.pressed() || rButton.pressed()) {
      alarms -> snooze();
    }
  }

  // Get screen to process input and be displayed
  Screen *currentScreen = screenManager.getCurrentScreen();

  // Handle input
  if (lButton.pressed()) {
    currentScreen->leftButtonPressed();
  }
  if (rButton.pressed()) {
    currentScreen->rightButtonPressed();
  }
  if (mButton.pressed()) {
    currentScreen->middleButtonPressed();
  }

  // Update display
  display.display(currentScreen->getDisplay());

  // Check if there's anything on the serial port
  checkSerialUpdate();
}

void checkSerialUpdate() {
  // Check if new time has been pushed over serial (in format yy,m,d,h,m,s)
  if (Serial.available() != 0) {
    time_t t;
    tmElements_t tm;

    // Parse new datetime. Convert year from double digit to since 1970
    int y = Serial.parseInt();
    tm.Year = y2kYearToTm(y);
    tm.Month = Serial.parseInt();
    tm.Day = Serial.parseInt();
    tm.Hour = Serial.parseInt();
    tm.Minute = Serial.parseInt();
    tm.Second = Serial.parseInt();

    // Convert to time_t object
    t = makeTime(tm);

    // Set RTC and locally kept time
    RTC.set(t);
    setTime(t);

    //Clear any further serial data
    while (Serial.available() > 0) Serial.read();
  }
}
