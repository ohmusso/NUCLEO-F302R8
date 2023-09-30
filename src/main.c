/* Driver */
#include "./driver/clock/clock.h"
#include "./driver/port/port.h"
#include "./driver/uart/uart.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

int main() {
    Clock_Init();
    Port_Init();
    Uart_Init();

    /* start FreeRTOS */
    vTaskStartScheduler();

    while (1) {
        /* Usually never executed */
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
uint32 systickCount = 0;
uint8 uartSendChar = 'a';
void vApplicationTickHook(void) {
    const uint32 blinkTime = 1000;

    if (systickCount <= blinkTime) {
        Port_Write(Port_On);
    } else if (systickCount <= (blinkTime * 2)) {
        Port_Write(Port_Off);
    } else {
        Port_Write(Port_On);
        Usart2_Transmit(uartSendChar);
        uartSendChar++;
        if (uartSendChar > 'c') {
            uartSendChar = 'a';
        }
        systickCount = 0;
    }

    systickCount++;
}
