#ifndef SensorManager_h
#define SensorManager_h

#include <DHT.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h> // Necesario para la comunicación I2C

class SensorManager {
public:
    SensorManager(DHT& dht, Adafruit_MLX90614& mlx);
    void initialize(); // Método para inicializar sensores
    void readSensors(float& tempDHT, float& tempMLX);

private:
    DHT& dht;
    Adafruit_MLX90614& mlx;
};

#endif  // SensorManager_h
