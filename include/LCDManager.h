#ifndef LCDManager_h
#define LCDManager_h

#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

class LCDManager {
public:
  LCDManager(uint8_t address, int columns, int rows);
  void begin();
  void clear();
  void print(const char *message);
  void displayTemperatures(float tempDHT, float tempMLX);

private:
  LiquidCrystal_I2C lcd;
};

#endif  // LCDManager_h