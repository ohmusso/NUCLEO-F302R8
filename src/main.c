/* Driver */
#include "./driver/adc/adc.h"
#include "./driver/clock/clock.h"
#include "./driver/exti/exti.h"
#include "./driver/nvic/nvic.h"
#include "./driver/port/port.h"
#include "./driver/syscfg/syscfg.h"
#include "./driver/timer/timer.h"
#include "./driver/uart/uart.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
/* FreeRTOS plus TCP */
#include "FreeRTOS_Routing.h"

/* app */
#include "./app/app_tasks.h"

/* Priorities for tasks. */
#define APP_TASK_PRIORITY_LOW (tskIDLE_PRIORITY + 1)
#define APP_TASK_APRIORITY_MID (tskIDLE_PRIORITY + 2)
#define APP_TASK_APRIORITY_HIGH (tskIDLE_PRIORITY + 3)
#define APP_TASK_APRIORITY_UART (tskIDLE_PRIORITY + 4)

/* NetworkInterface */
static NetworkInterface_t xNetInterface;
extern NetworkInterface_t *pxESP32_Wifi_Via_Uart_FillInterfaceDescriptor(
    BaseType_t xEMACIndex, NetworkInterface_t *pxInterface);
static void vInitNetwork(void);

int main() {
    Clock_Init();
    Syscfg_Init();
    Exti_Init();
    Port_Init();
    ADC1_Init();
    Timer_Init();
    Uart_Init();

    Nvic_Init();
    Nvic_SetInterruptGroupPrioriySubGroupOnly();
    Nvic_InitInterruptPrioriy(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);

    vInitNetwork();

    /* create app task */
    xTaskCreate(taskAppLedBlink, "LedBlink", configMINIMAL_STACK_SIZE,
                (void *)NULL, APP_TASK_PRIORITY_LOW, (TaskHandle_t *)NULL);
    // xTaskCreate(taskAppUart, "Uart", configMINIMAL_STACK_SIZE, (void *)NULL,
    //             APP_TASK_APRIORITY_MID, (TaskHandle_t *)NULL);

    /* start FreeRTOS */
    vTaskStartScheduler();

    while (1) {
        /* Usually never executed */
    }
}

static void vInitNetwork(void) {
    pxESP32_Wifi_Via_Uart_FillInterfaceDescriptor(0, &xNetInterface);

    /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network */
    /* are created in the vApplicationIPNetworkEventHook() hook function */
    /* below.  The hook function is called when the network connects. */
    FreeRTOS_IPInit_Multi();
}

/* interupt handler */
void IRQ_TIM1_UP_Handler() __attribute__((interrupt("IRQ")));
void IRQ_TIM1_UP_Handler() {
    // tim1Flip3PhasePwm();
    // tim1ClearUIF();
}
extern uint16 adcResult;
void IRQ_TIM1_CC_Handler() __attribute__((interrupt("IRQ")));
void IRQ_TIM1_CC_Handler() {
    // tim1Flip3PhasePwm();
    tim1ClearCC1IF();
}

void IRQ_EXTI3_Handler() __attribute__((interrupt("IRQ")));
void IRQ_EXTI3_Handler() { Exti_ClearExti3(); }

void IRQ_UART2_Handler() __attribute__((interrupt("IRQ")));
void IRQ_UART2_Handler() { Usart2_RxIndication(); }

extern void taskAppWifiViaUartIsrHandlerUart3Rx(void);
void IRQ_UART3_Handler() __attribute__((interrupt("IRQ")));
void IRQ_UART3_Handler() { taskAppWifiViaUartIsrHandlerUart3Rx(); }

void IRQ_EXTI15_10_Handler() __attribute__((interrupt("IRQ")));
void IRQ_EXTI15_10_Handler() { Exti_ClearExti15_10(); }

extern uint16_t bemfThreshold;
void IRQ_ADC_Handler() __attribute__((interrupt("IRQ")));
void IRQ_ADC_Handler() { taskAppIsrHandlerAdc(); }
