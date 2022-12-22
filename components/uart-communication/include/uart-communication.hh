#ifndef UART_COMMUNICATION_HH
#define UART_COMMUNICATION_HH

#include "driver/uart.h"

const char* const UART_COMMUNICATION_TAG = "uart comm";

class UARTInterface {
public:
    UARTInterface();
    ~UARTInterface();

    void read_inf();

private:
    QueueHandle_t uart_queue;
};

#endif /* ifndef UART_COMMUNICATION_HH */
