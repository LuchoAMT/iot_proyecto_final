#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFiClientSecure.h>
#include <WiFiClient.h>

class WifiManager {
public:
    WifiManager(const char* SSID, const char* PASS);
    void connect();
    void setCertificates(const char *CA_CERT, const char *CLIENT_CERT, const char *CLIENT_KEY);
    bool isConnected();
    WiFiClient& getWifiClient(); // Para acceder al cliente WiFi seguro

private:
    const char* SSID;
    const char* PASS;
    const char* CA_CERT; // Certificado raíz del broker
    const char* CLIENT_CERT;
    const char* CLIENT_KEY;
    WiFiClient insecureClient;
    WiFiClientSecure wifiClient;
};

#endif // WIFIMANAGER_H
