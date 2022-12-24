#include "uart-communication.hh"

#include <memory>
#include <string>
#include <cstring>

#include "esp_err.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_COMMUNICATION_TXD (CONFIG_UART_TXD)
#define UART_COMMUNICATION_RXD (CONFIG_UART_RXD)
#define UART_COMMUNICATION_RTS (UART_PIN_NO_CHANGE)
#define UART_COMMUNICATION_CTS (UART_PIN_NO_CHANGE)

#define UART_COMMUNICATION_PORT_NUM      (CONFIG_UART_PORT_NUM)
#define UART_COMMUNICATION_BAUD_RATE     (CONFIG_UART_BAUD_RATE)

#define READ_BUF_SIZE (1024)

UARTInterface::UARTInterface() {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UART_COMMUNICATION_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_COMMUNICATION_PORT_NUM, READ_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_COMMUNICATION_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_COMMUNICATION_PORT_NUM, UART_COMMUNICATION_TXD, UART_COMMUNICATION_RXD, UART_COMMUNICATION_RTS, UART_COMMUNICATION_CTS));

    read_buf.resize(READ_BUF_SIZE);
}

UARTInterface::~UARTInterface() {
    ESP_ERROR_CHECK(uart_driver_delete(UART_COMMUNICATION_PORT_NUM));
}

void UARTInterface::testEndlessLoop() {
    std::string teststring("ABCDE");

    for (;;) {
        // Write data to the UART
        uart_write_bytes(UART_COMMUNICATION_PORT_NUM, teststring.data(), teststring.size());

        // Read data from the UART
        int len = uart_read_bytes(UART_COMMUNICATION_PORT_NUM, read_buf.data(), (READ_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

        ESP_LOGI(UART_COMMUNICATION_TAG, "Recv str: %s", (char *) read_buf.data());

        // zero out the buffer
        if (len > 0) {
            memset (read_buf.data(), uint8_t{}, READ_BUF_SIZE);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
