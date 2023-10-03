#include <stdint.h>

/* FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"

/* driver */
#include "driver/port/port.h"
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

void taskAppLow() {
    for (;;) {
        uartSendChar = '1';
    }
}

void taskAppMid() {
    const int32_t durationTx = 500;
    int32_t preSysTickCount;

    for (;;) {
        preSysTickCount = systickCount;
        while ((systickCount - preSysTickCount) <= 500) {
            uartSendChar = '2';
        }
        vTaskDelay(durationTx);
    }
}

void taskAppHigh() {
    const int32_t durationTx = 800;
    int32_t preSysTickCount;
    uint8_t portValue = Port_Off;

    for (;;) {
        Port_Write(portValue);
        portValue++;
        preSysTickCount = systickCount;
        while ((systickCount - preSysTickCount) <= 200) {
            uartSendChar = '3';
        }
        vTaskDelay(durationTx);
    }
}

void taskAppUartTx() {
    const int32_t durationTx = 100;

    for (;;) {
        Usart2_Transmit(uartSendChar);
        vTaskDelay(durationTx);
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
void vApplicationTickHook(void) { systickCount++; }
