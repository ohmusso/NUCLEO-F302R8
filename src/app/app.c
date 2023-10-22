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

/* Motor spec */
/* 3Phase u, v, w */
/* Poles number: 7 */
/* 1Phase electrical angle: 17.1[deg] */
#define APP_MOTOR_SPEED_LEVEL_MAX 4
#define APP_3PHASE_PWM_CFG_STOP_FREQ ((uint16)23750) /* 60[rpm] */
#define APP_3PHASE_PWM_CFG_STOP_DUTY (0)
#define APP_3PHASE_PWM_CFG_LEVEL1_FREQ ((uint16)23750) /* 60[rpm] */
#define APP_3PHASE_PWM_CFG_LEVEL1_DUTY (APP_3PHASE_PWM_CFG_LEVEL1_FREQ / 2)
#define APP_3PHASE_PWM_CFG_LEVEL2_FREQ ((uint16)11875) /* 120[rpm] */
#define APP_3PHASE_PWM_CFG_LEVEL2_DUTY (APP_3PHASE_PWM_CFG_LEVEL2_FREQ / 2)
#define APP_3PHASE_PWM_CFG_LEVEL3_FREQ ((uint16)5937) /* 240[rpm] */
#define APP_3PHASE_PWM_CFG_LEVEL3_DUTY (APP_3PHASE_PWM_CFG_LEVEL3_FREQ / 2)

typedef struct {
    uint16 frequency;
    uint16 duty;
} Tim13PhasePwmCfg_t;

static const Tim13PhasePwmCfg_t pwmSetting[APP_MOTOR_SPEED_LEVEL_MAX] = {
    {APP_3PHASE_PWM_CFG_STOP_FREQ, APP_3PHASE_PWM_CFG_STOP_DUTY},
    {APP_3PHASE_PWM_CFG_LEVEL1_FREQ, APP_3PHASE_PWM_CFG_LEVEL1_DUTY},
    {APP_3PHASE_PWM_CFG_LEVEL2_FREQ, APP_3PHASE_PWM_CFG_LEVEL2_DUTY},
    {APP_3PHASE_PWM_CFG_LEVEL3_FREQ, APP_3PHASE_PWM_CFG_LEVEL3_DUTY}};

static uint8_t motorSpdLvlRef = 0;

void taskAppMotor() {
    const int32_t durationTx = 500;
    uint8_t motorSpdLvl = 0;

    tim1Set3PhasePwm(APP_3PHASE_PWM_CFG_STOP_FREQ,
                     APP_3PHASE_PWM_CFG_STOP_DUTY);
    tim1Start3PhasePwm();
    Port_SetMotorDriverEnable();

    for (;;) {
        if (motorSpdLvl != motorSpdLvlRef) {
            motorSpdLvl = motorSpdLvlRef;
            tim1Set3PhasePwm(pwmSetting[motorSpdLvl].frequency,
                             pwmSetting[motorSpdLvl].duty);
        }
        vTaskDelay(durationTx);
    }
}

void taskAppUart() {
    const int32_t durationTx = 100;
    RxDataType rxData = '\0';

    for (;;) {
        if (Uart2_ReadData(&rxData) == UartRetFetchData) {
            uint8_t ref = motorSpdLvlRef;

            /* update motor speed reference */
            if (rxData == '+') {
                if (ref < (APP_MOTOR_SPEED_LEVEL_MAX - 1)) {
                    ref++;
                }
            } else if (rxData == '-') {
                if (ref > 0) {
                    ref--;
                }
            } else {
                /* nop */
            }
            motorSpdLvlRef = ref;

            /* echo back */
            Usart2_Transmit(rxData);
        }
        vTaskDelay(durationTx);
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
void vApplicationTickHook(void) { systickCount++; }
