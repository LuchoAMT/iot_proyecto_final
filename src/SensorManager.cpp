#include "SensorManager.h"

SensorManager::SensorManager(DHT& dht, Adafruit_MLX90614& mlx) : dht(dht), mlx(mlx) {}

void SensorManager::initialize() {
    dht.begin(); // Inicializa el sensor DHT
    mlx.begin(); // Inicializa el sensor MLX90614
}

void SensorManager::readSensors(float& tempDHT, float& tempMLX) {
    tempDHT = dht.readTemperature();
    tempMLX = mlx.readObjectTempC();

    // Manejo de errores para el sensor DHT
    if (isnan(tempDHT)) {
        Serial.println("Error leyendo el sensor DHT!");
        tempDHT = -99.0; // O cualquier valor por defecto que desees
    }

    // Manejo de errores para el sensor MLX90614
    if (isnan(tempMLX)) {
        Serial.println("Error leyendo el sensor MLX90614!");
        tempMLX = -99.0; // O cualquier valor por defecto que desees
    }
}
