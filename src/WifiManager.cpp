#include "WifiManager.h"
#include <WiFi.h>

WifiManager::WifiManager(const char *SSID, const char *PASS)
    : SSID(SSID), PASS(PASS){}

void WifiManager::setCertificates(const char *CA_CERT, const char *CLIENT_CERT, const char *CLIENT_KEY)
{
  // Cargar el certificado del broker en WiFiClientSecure
  wifiClient.setCACert(CA_CERT);
  wifiClient.setCertificate(CLIENT_CERT);
  wifiClient.setPrivateKey(CLIENT_KEY);
}

void WifiManager::connect()
{
  WiFi.begin(SSID, PASS);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  wifiClient.setInsecure();
}

bool WifiManager::isConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

WiFiClient &WifiManager::getWifiClient()
{
  return insecureClient;
  //return wifiClient;
}
