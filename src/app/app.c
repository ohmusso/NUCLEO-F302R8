#include <stdint.h>

/* FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"

/* driver */
#include "driver/port/port.h"
#include "driver/uart/uart.h"

/* app */
#include "app_tasks.h"

/* app tasks */
void taskAppLedBlink() {
    const int32_t durationBlink = 500;

    for (;;) {
        Port_Write(Port_Off);
        vTaskDelay(durationBlink);
        Port_Write(Port_On);

        vTaskDelay(durationBlink);
    }
}

void taskAppUartTx() {
    const int32_t durationTx = 1000;
    int8_t uartSendChar = 'a';

    for (;;) {
        Usart2_Transmit(uartSendChar);

        uartSendChar++;
        if (uartSendChar > 'c') {
            uartSendChar = 'a';
        }

        vTaskDelay(durationTx);
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
int32_t systickCount = 0; /* unit: 1ms */
void vApplicationTickHook(void) { systickCount++; }
