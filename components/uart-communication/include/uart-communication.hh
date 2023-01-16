#ifndef UART_COMMUNICATION_HH
#define UART_COMMUNICATION_HH

#include "driver/uart.h"

#include <vector>

const char* const UART_COMMUNICATION_TAG = "uart comm";

class UARTInterface {
public:
    UARTInterface();
    ~UARTInterface();

    // this driver can currently not be copied
    UARTInterface(const UARTInterface& other) = delete;
    UARTInterface& operator=(const UARTInterface& other) = delete;

    void testEndlessLoop();

    static void uart_event_task(void *pvParameters);

    using data_callback_t = void(*)(void*, const unsigned char* input, size_t size);
    void registerDataCallback(data_callback_t callback, void* callback_1);
    void invokeDataCallbacks(const unsigned char* input, size_t size);

private:
    QueueHandle_t uart_queue;
    std::vector<uint8_t> read_buf;
    std::vector<std::pair<data_callback_t, void*>> data_callbacks;
};

#endif /* ifndef UART_COMMUNICATION_HH */
