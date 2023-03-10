#ifndef MQTT_HH
#define MQTT_HH

#include <string_view>
#include <string>

#include "mqtt_client.h"

class MQTTClient {
public:
    MQTTClient(std::string_view base_topic);
    ~MQTTClient();

    // it currently makes no sense to copy or assign as the handle gets invalid on destruction
    // for this to work the handle would have to be lifetime extended
    MQTTClient(const MQTTClient& other) = delete;
    MQTTClient& operator=(const MQTTClient& other) = delete;

    int publish(std::string_view topic_suff, std::string_view message);

private:
    esp_mqtt_client_handle_t client_handle;
    std::string base_topic;
};

#endif /* ifndef MQTT_HH */
