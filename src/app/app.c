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

extern uint32_t tim1IntCnt;

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
    uint16_t tim1IntCntPre = 0;

    for (;;) {
        if (tim1IntCntPre != tim1IntCnt) {
            uartSendChar++;
            tim1IntCntPre = tim1IntCnt;
        }

        if (uartSendChar > 'Z') {
            uartSendChar = '-';
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
