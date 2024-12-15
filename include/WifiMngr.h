#ifndef WIFIMANAGER2_H
#define WIFIMANAGER2_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

class WifiManager {
public:
    WifiManager();
    void connect();
    void setCertificates(const char *CA_CERT, const char *CLIENT_CERT, const char *CLIENT_KEY);
    bool isConnected();
    WiFiClientSecure& getWifiClient(); // Para acceder al cliente WiFi seguro

private:
    const char* CA_CERT; // Certificado raíz del broker
    const char* CLIENT_CERT;
    const char* CLIENT_KEY;
    WiFiClientSecure wifiClient;
    WiFiManager wm;
};

#endif // WIFIMANAGER_H
