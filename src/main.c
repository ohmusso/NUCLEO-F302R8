/* Driver */
#include "./driver/clock/clock.h"
#include "./driver/nvic/nvic.h"
#include "./driver/port/port.h"
#include "./driver/timer/timer.h"
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

extern uint32 tim1IntCnt;
int main() {
    Clock_Init();
    Port_Init();
    Timer_Init();
    Uart_Init();
    Nvic_Init();

    tim1Start();

    /* create app task*/
    xTaskCreate(taskAppLedBlink, "LedBlink", configMINIMAL_STACK_SIZE,
                (void *)NULL, APP_TASK_PRIORITY_LOW, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppMid, "Mid", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY_MID, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppUartTx, "UartTx", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY_UART, (TaskHandle_t *)NULL);

    /* start FreeRTOS */
    vTaskStartScheduler();

    while (1) {
        /* Usually never executed */
    }
}

/* interupt handler */
uint32 tim1IntCnt = 0;
void IRQ_TIM1_CC_Handler() __attribute__((interrupt("IRQ")));
void IRQ_TIM1_CC_Handler() {
    tim1IntCnt++;
    tim1ClearCC1IF();
}
