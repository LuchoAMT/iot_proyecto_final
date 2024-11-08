#include "MQTTClient.h"

MQTTClient::MQTTClient(const char* SERVER, int PORT, const char* TOPIC)
    : SERVER(SERVER), PORT(PORT), TOPIC(TOPIC), client(PubSubClient()) {}

void MQTTClient::connect(WiFiClientSecure& wifiClient) {
    client.setClient(wifiClient);
    client.setServer(SERVER, PORT);
    Serial.println("Connecting to MQTT...");
    while (!client.connected()) {
        Serial.println(".");
        if (client.connect("ESP32Panconquesito")) {
            Serial.println("Connected to MQTT");
            subscribe();
        } else {
            Serial.print("Failed MQTT connection, rc=");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void MQTTClient::publishMessage(const char* message) {
    if (client.connected()) {
        client.publish(TOPIC, message);
    }
}

void MQTTClient::subscribe() {
    client.subscribe(TOPIC);
}

bool MQTTClient::isConnected() {
    return client.connected();
}

void MQTTClient::setCallback(MQTT_CALLBACK_SIGNATURE){
    client.setCallback(callback);
}

void MQTTClient::loop(){
    client.loop();
}