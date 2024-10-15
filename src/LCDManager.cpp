#include "LCDManager.h"

LCDManager::LCDManager(uint8_t address, int columns, int rows) 
: lcd(address, columns, rows) {}

void LCDManager::begin(){
  lcd.begin(16,2);
  lcd.backlight();
  lcd.clear();
}

void LCDManager::clear(){
  lcd.clear();
}

void LCDManager::print(const char *message){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  if (strlen(message) > 16) {
    lcd.setCursor(0, 1);  // Mover el cursor a la segunda fila
    for (int i = 16; i < 32 && message[i] != '\0'; i++) {
      lcd.print(message[i]);
    }
  }
}

void LCDManager::displayTemperatures(float tempDHT, float tempMLX) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DHT11: ");
  lcd.print(tempDHT);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("MLX90614: ");
  lcd.print(tempMLX);
  lcd.print(" C");
}