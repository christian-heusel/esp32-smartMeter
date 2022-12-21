#ifndef MQTT_H
#define MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mqtt_client.h"

const char* const MQTT_LOG_TAG = "MQTT";

esp_mqtt_client_handle_t mqtt_app_start();

#ifdef __cplusplus
}
#endif

#endif /* ifndef MQTT_H */
