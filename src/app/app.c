#include <stdint.h>

#include "driver/port/port.h"
#include "driver/uart/uart.h"

/* FreeRTOS API */
/* interupt cycle: 1ms */
int32_t timer = 0; /* unit: 1ms */
int8_t uartSendChar = 'a';
void vApplicationTickHook(void) {
    const int32_t blinkTime = 500;

    if (timer <= blinkTime) {
        Port_Write(Port_On);
    } else if (timer <= (blinkTime * 2)) {
        Port_Write(Port_Off);
    } else {
        Port_Write(Port_On);
        Usart2_Transmit(uartSendChar);
        uartSendChar++;
        if (uartSendChar > 'c') {
            uartSendChar = 'a';
        }
        timer = 0;
    }

    timer++;
}
