#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTClient.h"
#include "SensorManager.h"
#include "LCDManager.h"
#include <ArduinoJson.h>

#define DHTPIN 4        // Pin del DHT11
#define DHTTYPE DHT11   // Tipo de sensor DHT11
#define BUZZERPIN 5

DHT dht(DHTPIN, DHTTYPE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// const char* SSID = "Huawei P50 Pro";
// const char* PASS = "nqyct6y9ypt7xic";

const char* SSID = "Menacho-8Mbps";
const char* PASS = "47Lu1sT0no43";

// Información del broker MQTT de AWS
const char* SERVER = "a255q5ixlivm2d-ats.iot.us-east-2.amazonaws.com";  // Obtén el endpoint de AWS IoT
const int PORT = 8883; // Puerto para conexión segura
const char* TOPIC = "sensores/temperaturas";
const char* UPDATE_TOPIC = "$aws/things/coso_esp_temperatura/shadow/update";              // publish
const char* UPDATE_DELTA_TOPIC = "$aws/things/coso_esp_temperatura/shadow/update/delta";  // subscribe

WifiManager wifiManager(SSID, PASS);
MQTTClient mqttClient(SERVER, PORT, UPDATE_TOPIC);
SensorManager sensorManager(dht, mlx);
LCDManager lcd(0x27,16,2);

float lastTempDHT = -100.0;
float lastTempMLX = -100.0;
const float TEMP_TRESH = 5.0;

unsigned long lastSentTime = 0;
const unsigned long SEND_INTERVAL = 50000;

StaticJsonDocument<256> shadowDoc;
char outputBuffer[256];

byte buzzer = 0;
byte prevBuzzerState = 0;

// Certificados y clave privada
const char* CA_CERT = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"\
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"\
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"\
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"\
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"\
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"\
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"\
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"\
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"\
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"\
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"\
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"\
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"\
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"\
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"\
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"\
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"\
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"\
"-----END CERTIFICATE-----\n";

const char* CLIENT_CERT = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUYwDoGg9Bw8h9wYExNLCOFj6vNMMwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTAxMjEzNTQ0\n"\
"N1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANG2KFRm5xR6li9GkF07\n"\
"LaulxTuOUqAvlsH9hzblVpBGUMsSFJC5oUZKx+QTMpNSd+ZMqD1Q8iNwVyAECzQL\n"\
"QSp1vEH/R4ri4ISbc7rJOVNoyZ3pQeC0sq9u2YZWBXh32Sxx8sRrK/JX7vxVrQAy\n"\
"IT7sTwMtC8AB4kdLI4UIpD+Y9SgcGBoXj+0ouJRWRwFLJlkci+FNc7vJhMTVurUL\n"\
"HsH9qAut4YCgpUIyf2xqKMWBAqoamA2QblAzzsADuCKkX22zi0H7eS1b4nL2uaQ0\n"\
"U3Upy8Sm9SEAD2I3RLpgl+bJnLv8A3o2M2B17xNyIMe3exWbYOyjXfUhjNntKdr2\n"\
"6R0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUmXIDURMXEgyJh94oaBzxXqKApRQwHQYD\n"\
"VR0OBBYEFG+zy5+pDD3k8pQiXkKuqloQwoRBMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAIuN/UvR6otUT2hsXnoE8i1Fun\n"\
"/wwiLqMd42YAtheA/yYqI9GuZnl1EEpKre4HPdQ1wsAg1trpjJQh9SbojObPuENx\n"\
"6VL8sLIgUndCF7QfPwLB0O9p6XPK8Am/zJ1LhshMKRhetvTwkqVL7uYt+W1tbuf6\n"\
"DS4qKsAPgNUJ4a/f0jiFIHvANqQC1I54z1yltVCm4FzhT7RtKCqPoqRm51vgVLrs\n"\
"RdIr4ig93phd7fW2AufnoZLjFcKB4C/6/25xjTAVqwyUVvOzBZQapkXyV6WgI9bM\n"\
"nSlZgMedBkcOK+Pp/VSegRXVD5DqAzXnQka+tY4Rwoz3pjXbPiy7omoN4M2y\n"\
"-----END CERTIFICATE-----\n";

const char* CLIENT_KEY = \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpAIBAAKCAQEA0bYoVGbnFHqWL0aQXTstq6XFO45SoC+Wwf2HNuVWkEZQyxIU\n"\
"kLmhRkrH5BMyk1J35kyoPVDyI3BXIAQLNAtBKnW8Qf9HiuLghJtzusk5U2jJnelB\n"\
"4LSyr27ZhlYFeHfZLHHyxGsr8lfu/FWtADIhPuxPAy0LwAHiR0sjhQikP5j1KBwY\n"\
"GheP7Si4lFZHAUsmWRyL4U1zu8mExNW6tQsewf2oC63hgKClQjJ/bGooxYECqhqY\n"\
"DZBuUDPOwAO4IqRfbbOLQft5LVvicva5pDRTdSnLxKb1IQAPYjdEumCX5smcu/wD\n"\
"ejYzYHXvE3Igx7d7FZtg7KNd9SGM2e0p2vbpHQIDAQABAoIBAFDiPgnXqpbGdTiE\n"\
"sdeTpJjIlhrTWOD5md72ZvlwWQPKawz3y0DKp/5Rh2lbUi1qKF6GAHmtx8aNZL0v\n"\
"cha/pIG46URXLoPN36SlxGwtglO+XczvE36UkzqAuZtLgE6ngvbWeDjz0P5mlgFz\n"\
"Fge2TgZT5GGoNRj4YQr4WxnLbHYuWNdkLoJ5Kh4B4MwV3MfLQmUKRvy15ZTvyVUe\n"\
"FuSf1ulgXfgECZnAi8qFcrNPrztxBxhr8KDDU/rYIeVxaY7tfspFBnJVjLERa6d2\n"\
"bAQ4KNFcxBLxg1eP1eyW2fv1Mj5qAvpyi6ecXwu1e9rkrtBm1gnhm2sStMXCtB3I\n"\
"5c59bo0CgYEA6xDdw0c6PbE4xi9bkVLATpBNGu9QXXVuA+7g1SqCVPUDw+Fi7Tdw\n"\
"v64d7KwVKhqmIWnKCtjOsgjWYHKZTeDEAdF7BFzChkkYBZQWL5fUncM9UBu8xSLz\n"\
"gtwc0uzF2/dQ8J3RsCDhST2FDgQ2oZE1EmVF53WfdVIinuP9wDB9XDMCgYEA5GNA\n"\
"lKmgo8LUTMugvF6fpLorOqFNPtaajl4R3KwPXGC/cLmPc8v8ZtGDWUS38A/WJQ/0\n"\
"Qa+rAuXndw6ey8OiD/kYLoGvxx+NT/GVFRioNtRARAp9c+NmqTi/4t8k2DyFXly4\n"\
"J27sgsBSQx99/7k43dwpD6SWyyZmJJ2KLfgFVW8CgYEAh6Iww3e65FP49WnoBP9h\n"\
"lTNwOiVyEqsqcryIIJepoknKBWFsioHd3cwi2iglAilkSI1ZFk8pXNL6l6YoTSZv\n"\
"ErmDqO25FHDimn1rG8UY2nsISsvmbT2QkqZdP1t9g0G+A0ic8GrYc0E7H+5Qc4Ns\n"\
"wxOKdVaKTtC4x0O4bAfOnb8CgYEAzKfgDBeA2p5Qrcxe6c2FExC8ghpe5Otbri2/\n"\
"pdYNIcOUaUfus15cfCpMoGbCyPz38Vg9kcr/3GX2J6rTQ1a4V8XoFGyj/jRY5gVe\n"\
"iXwB6LgEKrSXvIxOx18SHBPHqXl6LZe6caOljnkWfg2vU8M/oReTN/BOLBbpuu/t\n"\
"DcpNxUMCgYAYhCehfJ/ZqzUFqX9RklqkOzEKJ4f2MVaccMi1EB3/wAhnfan6/hIB\n"\
"Pew9gyrAZF1vcKjkzbTUw71uSG1EWLEHcjGuX1u6sfmwskMFMMhBGTDXyv+nAyvH\n"\
"KGUlNfKPV+6Cf5q/7IgjQjPQSR+XCKUcp26d7+k0Nbf4Eq7RfQprNw==\n"\
"-----END RSA PRIVATE KEY-----\n";

const int BUZZERINTERVAL = 10;
bool buzzerActive = false; 
int beepCount = 0; 
unsigned long previousMillis = 0;

void triggerBuzzer(){ //se optó por usar delay en lugar de millis ya que con millis la duración del beep nunca variaba aun cambiando el dato de forma manual.
  for (int i = 0; i < 2; i++) {    // Dos beeps
    digitalWrite(BUZZERPIN, HIGH); // Enciende el buzzer
    delay(50);                     // Duración del beep (ajústalo a tu gusto)
    digitalWrite(BUZZERPIN, LOW);  // Apaga el buzzer
    delay(50);                     // Pausa entre beeps
  }
}

// Función callback para manejar los mensajes recibidos por MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++){
    message += (char)payload[i];
  }

  Serial.print("Mensaje recibido: ");
  Serial.println(message);
  lcd.clear();
  lcd.print(message.c_str());  
}

void setup() {
  Serial.begin(115200);
  wifiManager.setCertificates(CA_CERT,CLIENT_CERT, CLIENT_KEY);
  wifiManager.connect();

  mqttClient.connect(wifiManager.getWifiClient());
  mqttClient.setCallback(mqttCallback);

  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN, LOW);
  
  Wire.begin(26,25);
  sensorManager.initialize();

  lcd.begin();
  lcd.print("Iniciando...");
  Serial.println("Iniciando...");
  delay(1000);
}

void loop(){
  mqttClient.loop();
  float tempDHT, tempMLX;
  sensorManager.readSensors(tempDHT, tempMLX);

  if (tempMLX >= 37){
    triggerBuzzer();
  }

  bool significantChangeDHT = abs(tempDHT - lastTempDHT) >= TEMP_TRESH;
  bool significantChangeMLX = abs(tempMLX - lastTempMLX) >= TEMP_TRESH;
  bool sentIntervalDeath = millis() - lastSentTime >= SEND_INTERVAL;

  if (significantChangeDHT || significantChangeMLX || sentIntervalDeath){
    shadowDoc["state"]["reported"]["tempDHT"] = tempDHT;
    shadowDoc["state"]["reported"]["tempMLX"] = tempMLX;
    serializeJson(shadowDoc, outputBuffer);

    mqttClient.publishMessage(outputBuffer);
    Serial.println("Reported to shadow: ");
    Serial.println(outputBuffer);

    lastTempDHT = tempDHT;
    lastTempMLX = tempMLX;
    lastSentTime = millis();
  }
  if (tempMLX >= 37){
    triggerBuzzer();
  }
  
  lcd.displayTemperatures(tempDHT, tempMLX);
  delay(2000);
}

