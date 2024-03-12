#include "uart-communication.hh"

#include <memory>
#include <string>
#include <functional>
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

#define UART_COMMUNICATION_PORT_NUM      (uart_port_t)(CONFIG_UART_PORT_NUM)
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
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_COMMUNICATION_PORT_NUM, READ_BUF_SIZE * 2, 0, 10, &uart_queue, intr_alloc_flags));
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
        ESP_LOGI(UART_COMMUNICATION_TAG, "Send str (testEndlessLoop): %s", (char *) teststring.data());
        uart_write_bytes(UART_COMMUNICATION_PORT_NUM, teststring.data(), teststring.size());

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void UARTInterface::registerDataCallback(data_callback_t callback, void* callback_1) {
    data_callbacks.push_back({callback, callback_1});
}

void UARTInterface::invokeDataCallbacks(const unsigned char* input, size_t size) {
    for (auto&& [callback, obj] : data_callbacks) {
        callback(obj, input, size);
    }
}

void UARTInterface::uart_event_task(void* pvParameters) {
    UARTInterface* uart_interface_ptr = static_cast<UARTInterface*>(pvParameters);
    uart_event_t event;
    size_t buffered_size;

    ESP_LOGI(UART_COMMUNICATION_TAG, "uart interface ptr: %p", uart_interface_ptr);
    ESP_LOGI(UART_COMMUNICATION_TAG, "uart interface readbuf size %d", uart_interface_ptr->read_buf.size());

    for(;;) {
        // Waiting for UART event.
        if(xQueueReceive(uart_interface_ptr->uart_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(uart_interface_ptr->read_buf.data(), READ_BUF_SIZE);
            // ESP_LOGI(UART_COMMUNICATION_TAG, "uart[%d] event:", UART_COMMUNICATION_PORT_NUM);
            switch(event.type) {
                // Event of UART receving data
                /* We'd better handler data event fast, there would be much more data events than
                   other types of events. If we take too much time on data event, the queue might
                   be full. */
                case UART_DATA:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(UART_COMMUNICATION_PORT_NUM, uart_interface_ptr->read_buf.data(), event.size, portMAX_DELAY);
                    // ESP_LOGI(UART_COMMUNICATION_TAG, "[DATA EVT]: %s", (const char*) uart_interface_ptr->read_buf.data());

                    uart_interface_ptr->invokeDataCallbacks(uart_interface_ptr->read_buf.data(), event.size);
                    break;
                }
                // Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(UART_COMMUNICATION_PORT_NUM);
                    xQueueReset(uart_interface_ptr->uart_queue);
                    break;
                }
                // Event of UART ring buffer full
                case UART_BUFFER_FULL:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "ring buffer full");
                    // If buffer full happened, you should consider increasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(UART_COMMUNICATION_PORT_NUM);
                    xQueueReset(uart_interface_ptr->uart_queue);
                    break;
                }
                // Event of UART RX break detected
                case UART_BREAK:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "uart rx break");
                    break;
                }
                // Event of UART parity check error
                case UART_PARITY_ERR:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "uart parity error");
                    break;
                }
                // Event of UART frame error
                case UART_FRAME_ERR:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "uart frame error");
                    break;
                }
                // UART_PATTERN_DET
                case UART_PATTERN_DET:
                {
                    uart_get_buffered_data_len(UART_COMMUNICATION_PORT_NUM, &buffered_size);
                    int pos = uart_pattern_pop_pos(UART_COMMUNICATION_PORT_NUM);
                    ESP_LOGI(UART_COMMUNICATION_TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    break;
                }
                // Others
                default:
                {
                    ESP_LOGI(UART_COMMUNICATION_TAG, "uart event type: %d", event.type);
                    break;
                }
            }
        }
    }

    // this removes the task from the RTOS queues and is usually not reached
    vTaskDelete(NULL);
}

