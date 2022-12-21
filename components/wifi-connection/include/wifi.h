#ifndef WIFI_H
#define WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_event.h"

const char* const WIFI_LOG_TAG = "wifi station";

void event_handler(void* arg, esp_event_base_t event_base,
                   int32_t event_id, void* event_data);

void wifi_init_station_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef WIFI_H */
