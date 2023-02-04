// ESP32-SmartMeter
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
#include <memory>

#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "wifi.h"
#include "uart-communication.hh"
#include "sml-parser.hh"
#include "mqtt.hh"

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

    auto mqtt_client = std::make_unique<MQTTClient>("esp32/smartMeter/");
    auto sml_parser = std::make_unique<SMLParser>(mqtt_client.get());
    auto uart_interface = std::make_unique<UARTInterface>();

    uart_interface->registerDataCallback(&SMLParser::readBufferCallbackWrapper, sml_parser.get());

    // Create a task to handler UART event from ISR
    // The UART interface has to be allocated dynamically as the tasks cannot reference stack vars
    // see https://www.freertos.org/a00125.html at "pvParameters"
    xTaskCreate(UARTInterface::uart_event_task, "uart_event_task", 4096, uart_interface.get(), 12, NULL);

    for (;;) {
        ESP_LOGI("MAIN", "tick");
        char buffer[50];
	int64_t uptime_seconds = esp_timer_get_time() / 1E6;
        auto size = sprintf(buffer, "%lld", uptime_seconds);
    	auto str = std::string_view(buffer, size);
        ESP_LOGI("MAIN", "uptime: %s,%d", buffer, size);
    	mqtt_client->publish("uptime", str);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
