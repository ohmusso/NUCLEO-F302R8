/* Driver */
#include "./driver/clock/clock.h"
#include "./driver/port/port.h"
#include "./driver/uart/uart.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* app */
#include "./app/app_tasks.h"

/* Priorities for tasks. */
#define APP_TASK_PRIORITY (tskIDLE_PRIORITY)
#define APP_TASK_APRIORITY (tskIDLE_PRIORITY + 1)

int main() {
    Clock_Init();
    Port_Init();
    Uart_Init();

    if (sizeof(uint32) == 4) {
        Port_Write(Port_On);
    }

    /* create app task*/
    xTaskCreate(taskAppLedBlink, "LedBlink", configMINIMAL_STACK_SIZE,
                (void *)NULL, APP_TASK_PRIORITY, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppUartTx, "UartTx", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY, (TaskHandle_t *)NULL);

    /* start FreeRTOS */
    vTaskStartScheduler();

    while (1) {
        /* Usually never executed */
    }
}
