#pragma once
#include <string>
#include <vector>
#include <mqtt/async_client.h>

class MqttClient {
public:
    MqttClient(const std::string& broker,
               const std::string& client_id,
               const std::string& topic);

    void connect();
    void disconnect();

    void publishChunks(const std::vector<double>& xs,
                       const std::vector<double>& ys,
                       const std::vector<double>& zs);

    void publishFFT(const std::vector<double>& frequencies,
                    const std::vector<double>& magTotal,
                    const std::vector<double>& magX,
                    const std::vector<double>& magY,
                    const std::vector<double>& magZ);

private:
    mqtt::async_client client_;
    std::string topic_;

    std::string createJson(size_t start, size_t end,
                           const std::vector<double>& xs,
                           const std::vector<double>& ys,
                           const std::vector<double>& zs);
};