#ifndef MQTT_HH
#define MQTT_HH

#include <string_view>

#include "mqtt_client.h"

class mqttClient {
public:
    mqttClient();
    ~mqttClient();

    // it currently makes no sense to copy or assign as the handle gets invalid on destruction
    // for this to work the handle would have to be lifetime extended
    mqttClient(const mqttClient& other) = delete;
    mqttClient& operator=(const mqttClient& other) = delete;

    int publish(std::string_view topic, std::string_view message);

private:
    esp_mqtt_client_handle_t client_handle;
};

#endif /* ifndef MQTT_HH */
