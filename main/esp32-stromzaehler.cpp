// ESP32-Stromzaehler
// Copyright (C) 2022 Christian Heusel
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

#include "wifi.h"
#include "mqtt.h"

extern "C" void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(WIFI_LOG_TAG, "ESP_WIFI_MODE_STA");
    wifi_init_station_mode();

    esp_mqtt_client_handle_t mqtt_client = mqtt_app_start();
    int msg_id = esp_mqtt_client_publish(mqtt_client, "/topic/some_test_topic", "abcde", 0, 1, 0);
    ESP_LOGI(MQTT_LOG_TAG, "sent publish successful, msg_id=%d", msg_id);

    // Cleanup the mqtt client
    ESP_ERROR_CHECK(esp_mqtt_client_destroy(mqtt_client));
}
