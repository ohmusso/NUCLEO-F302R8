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
#define APP_TASK_PRIORITY_LOW (tskIDLE_PRIORITY + 1)
#define APP_TASK_APRIORITY_MID (tskIDLE_PRIORITY + 2)
#define APP_TASK_APRIORITY_HIGH (tskIDLE_PRIORITY + 3)
#define APP_TASK_APRIORITY_UART (tskIDLE_PRIORITY + 4)

int main() {
    Clock_Init();
    Port_Init();
    Uart_Init();

    /* create app task*/
    // xTaskCreate(taskAppLedBlink, "LedBlink", configMINIMAL_STACK_SIZE,
    //             (void *)NULL, APP_TASK_PRIORITY_LOW, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppLow, "Low", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_PRIORITY_LOW, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppMid, "Mid", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY_MID, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppHigh, "High", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY_HIGH, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppUartTx, "UartTx", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY_UART, (TaskHandle_t *)NULL);

    /* start FreeRTOS */
    vTaskStartScheduler();

    while (1) {
        /* Usually never executed */
    }
}
