#include "../include/mqtt_publish.h"

MQTTClient::MQTTClient(const std::string& serverURI, const std::string& clientID)
    : cli(serverURI, clientID) {}

void MQTTClient::connect() {
    mqtt::connect_options connOpts;
    cli.connect(connOpts)->wait();
}

void MQTTClient::publish(const std::string& topic, const std::string& payload) {
    mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
    pubmsg->set_qos(1);
    cli.publish(pubmsg)->wait_for(std::chrono::seconds(10));
}

void MQTTClient::disconnect() {
    cli.disconnect()->wait();
}
