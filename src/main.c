/* Driver */
#include "./driver/clock/clock.h"
#include "./driver/exit/exit.h"
#include "./driver/nvic/nvic.h"
#include "./driver/port/port.h"
#include "./driver/syscfg/syscfg.h"
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

int main() {
    Clock_Init();
    Syscfg_Init();
    Exit_Init();
    Port_Init();
    Timer_Init();
    Uart_Init();
    Nvic_Init();

    /* create app task*/
    xTaskCreate(taskAppLedBlink, "LedBlink", configMINIMAL_STACK_SIZE,
                (void *)NULL, APP_TASK_PRIORITY_LOW, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppMotor, "Motor", configMINIMAL_STACK_SIZE, (void *)NULL,
                APP_TASK_APRIORITY_MID, (TaskHandle_t *)NULL);
    xTaskCreate(taskAppUart, "Uart", configMINIMAL_STACK_SIZE, (void *)NULL,
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
    tim1Flip3PhasePwm();
    tim1ClearCC1IF();
}

void IRQ_EXIT3_Handler() __attribute__((interrupt("IRQ")));
void IRQ_EXIT3_Handler() { Exit_ClearExit3(); }

void IRQ_UART2_Handler() __attribute__((interrupt("IRQ")));
void IRQ_UART2_Handler() { Usart2_RxIndication(); }

void IRQ_EXIT15_10_Handler() __attribute__((interrupt("IRQ")));
void IRQ_EXIT15_10_Handler() { Exit_ClearExit15_10(); }
