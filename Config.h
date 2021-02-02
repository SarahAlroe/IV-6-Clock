#pragma once 

class Config {
  public:
    static Config *getInstance();

    void load();
    void save();
    void toggleSummerTime();
    void toggleTempUnit();
    void setSnoozeTime(int mins);

    bool isSummerTime();
    char getTempUnit();
    int getSnoozeTime();

  private:
    const int SUMMER_TIME_ADDR = 4;
    const int TEMP_UNIT_ADDR = 6;
    const int SNOOZE_TIME_ADDR = 8;
    Config();
    static Config *instance;
    bool summerTime = false;
    char tempUnit = 'C';
    unsigned int snoozeTime = 5;
};
