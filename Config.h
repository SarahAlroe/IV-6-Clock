class Config {
  public:
    static Config *getInstance();

    void load();
    void save();
    void toggleSummerTime();
    void toggleTempUnit();

    bool isSummerTime();
    char getTempUnit();

  private:
    const int SUMMER_TIME_ADDR = 4;
    const int TEMP_UNIT_ADDR = 6;
    Config();
    static Config *instance;
    bool summerTime = false;
    char tempUnit = 'C';
};

Config *Config::instance = 0;

Config::Config() {
  this->load();
}

Config * Config::getInstance() {
  if (!instance)
    instance = new Config;
  return instance;
}

void Config::load() {
  EEPROM.get(SUMMER_TIME_ADDR, summerTime);
  EEPROM.get(TEMP_UNIT_ADDR, tempUnit);
}

void Config::save() {
  EEPROM.put(SUMMER_TIME_ADDR, summerTime);
  EEPROM.put(TEMP_UNIT_ADDR, tempUnit);
}

void Config::toggleSummerTime() {
  if (isSummerTime()) {
    setTime(now() - 3600);
    RTC.set(now());
  } else {
    setTime(now() + 3600);
    RTC.set(now());
  }
  summerTime = ! summerTime;
}

bool Config::isSummerTime() {
  return summerTime;
}

void Config::toggleTempUnit(){
  switch(tempUnit){
    case 'C':
      tempUnit = 'K';
      break;
    case 'K':
      tempUnit = 'F';
      break;
    case 'F':
      tempUnit = 'C';
      break;
  }
}

char Config::getTempUnit(){
  return tempUnit;
}
