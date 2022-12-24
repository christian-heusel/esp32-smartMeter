#ifndef UART_COMMUNICATION_HH
#define UART_COMMUNICATION_HH

#include "driver/uart.h"

#include <vector>

const char* const UART_COMMUNICATION_TAG = "uart comm";

class UARTInterface {
public:
    UARTInterface();
    ~UARTInterface();

    void testEndlessLoop();
    static void uart_event_task(void *pvParameters);

private:
    QueueHandle_t uart_queue;
    std::vector<uint8_t> read_buf;
};

#endif /* ifndef UART_COMMUNICATION_HH */
