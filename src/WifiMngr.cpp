#include "WifiMngr.h"

WifiManager::WifiManager() : CA_CERT(nullptr), CLIENT_CERT(nullptr), CLIENT_KEY(nullptr), wm(){}

void WifiManager::setCertificates(const char *CA_CERT, const char *CLIENT_CERT, const char *CLIENT_KEY)
{
  this->CA_CERT = CA_CERT;
  this->CLIENT_CERT = CLIENT_CERT;
  this->CLIENT_KEY = CLIENT_KEY;

  wifiClient.setCACert(CA_CERT);
  wifiClient.setCertificate(CLIENT_CERT);
  wifiClient.setPrivateKey(CLIENT_KEY);
}

void WifiManager::connect()
{
  if (!wm.autoConnect("ESP32-Config", "12345678")) {  // Portal de configuración
        Serial.println("Falló la conexión, reiniciando...");
        delay(3000);
        ESP.restart();
    }

    Serial.println("WiFi conectado!");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());

}

bool WifiManager::isConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

WiFiClientSecure &WifiManager::getWifiClient()
{
  //return insecureClient;
  return wifiClient;
}
