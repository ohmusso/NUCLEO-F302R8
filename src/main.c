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
