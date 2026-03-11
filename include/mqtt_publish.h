#ifndef MQTT_PUBLISH_H
#define MQTT_PUBLISH_H

#include <string>
#include <chrono>
#include <mqtt/async_client.h>

class MQTTClient {
public:
    MQTTClient(const std::string& serverURI, const std::string& clientID);

    void connect();
    void publish(const std::string& topic, const std::string& payload);
    void disconnect();

private:
    mqtt::async_client cli;
};

#endif
