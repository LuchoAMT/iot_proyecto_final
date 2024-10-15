#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

class MQTTClient {
public:
    MQTTClient(const char* SERVER, int PORT, const char* TOPIC);
    void connect(WiFiClientSecure& wifiClient);
    void connect(WiFiClient& wifiClient);
    void publishMessage(const char* message);
    void subscribe();
    bool isConnected();
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    void loop();

private:
    const char* SERVER;
    int PORT;
    const char* TOPIC;
    PubSubClient client;
};

#endif // MQTTCLIENT_H
