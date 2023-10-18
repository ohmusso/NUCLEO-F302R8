#include <stdint.h>

/* FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"

/* driver */
#include "driver/port/port.h"
#include "driver/timer/timer.h"
#include "driver/uart/uart.h"

/* app */
#include "app_tasks.h"

int32_t systickCount = 0; /* unit: 1ms */

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

uint8_t uartSendChar = '-';

void taskAppMid() {
    const int32_t durationTx = 500;

    for (;;) {
        vTaskDelay(durationTx);
    }
}

void taskAppUart() {
    const int32_t durationTx = 100;
    RxDataType rxData = '-';

    for (;;) {
        if (Uart2_ReadData(&rxData) == UartRetFetchData) {
            /* echo back */
            Usart2_Transmit(rxData);
        }
        vTaskDelay(durationTx);
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
void vApplicationTickHook(void) { systickCount++; }
