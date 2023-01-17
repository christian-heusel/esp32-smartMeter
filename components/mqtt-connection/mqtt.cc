
#include "esp_log.h"
#include "mqtt_client.h"

#include "mqtt.hh"
#include "mqtt.h"

MQTTClient::MQTTClient(std::string_view base_topic_input) :
    client_handle(mqtt_app_start()),
    base_topic(base_topic_input)
{};

MQTTClient::~MQTTClient() {
    // Cleanup the mqtt client
    ESP_ERROR_CHECK(esp_mqtt_client_destroy(client_handle));
}

int MQTTClient::publish(std::string_view topic_suff, std::string_view message) {
    std::string topic = std::string(base_topic) + std::string(topic_suff);
    int msg_id = esp_mqtt_client_publish(client_handle, topic.data(), message.data(), 0, 1, 0);
    // ESP_LOGI(MQTT_LOG_TAG, "sent publish successful, msg_id=%d", msg_id);
    return msg_id;
}

