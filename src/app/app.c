#include <stdint.h>

/* FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"

/* driver */
#include "driver/adc/adc.h"
#include "driver/port/port.h"
#include "driver/timer/timer.h"
#include "driver/uart/uart.h"

/* app */
#include "app_tasks.h"

int32_t systickCount = 0; /* unit: 1ms */

/* app tasks */
void taskAppLedBlink(void* pvParameters) {
    const int32_t durationBlink = 500;

    for (;;) {
        Port_Write(Port_Off);
        vTaskDelay(durationBlink);
        Port_Write(Port_On);
        vTaskDelay(durationBlink);
    }
}

typedef struct {
    uint16 width;
    uint16 duty;
    TickType_t phaseTime;
} Tim13PhasePwmCfg_t;

/* Motor spec */
/* 3Phase u, v, w */
/* Poles number: 7 */
/* 1Phase electrical angle: 17.1[deg] */
#define mApp3phaseCfgPwmWitdh \
    ((uint16)25) /* pwm center aligned mode: width = ARR*2 = 50[us] */

#define mAppMotorSpeedMax 4
static const Tim13PhasePwmCfg_t pwmSetting[mAppMotorSpeedMax] = {
    /* stop */
    {mApp3phaseCfgPwmWitdh, 0, 0},
    /* 1phase: 48[ms], 60[rpm] */
    {mApp3phaseCfgPwmWitdh, mApp3phaseCfgPwmWitdh / 12, 48},
    /* 1phase: 48[ms], 120[rpm] */
    {mApp3phaseCfgPwmWitdh, mApp3phaseCfgPwmWitdh / 8, 24},
    /* 1phase: 24[ms], 240[rpm] */
    {mApp3phaseCfgPwmWitdh, mApp3phaseCfgPwmWitdh / 4, 6}};

static uint8_t motorSpdLvlRef = 0;

void taskAppMotor(void* pvParameters) {
    TickType_t duration = 0;

    tim1Start3PhasePwm();

    for (;;) {
        if (motorSpdLvlRef == 0) {
            /* stop motor */
            Port_SetMotorDriverDisable();
            vTaskDelay(100);
            continue;
        }

        if (motorSpdLvlRef >= mAppMotorSpeedMax) {
            /* invalid motor speed ref, stop motor */
            Port_SetMotorDriverDisable();
            vTaskDelay(100);
            continue;
        }

        /* set duration from motor speed ref */
        tim1Set3PhasePwmCfg(pwmSetting[motorSpdLvlRef].width,
                            pwmSetting[motorSpdLvlRef].duty);
        duration = pwmSetting[motorSpdLvlRef].phaseTime;

        /* phase uv:  u: high, v: high, w: high-z */
        timSet6StepMotorPhaseU();
        Port_SetMotorDriverEnUV();
        vTaskDelay(duration / 2);
        Port_SetMotorDriverEnWU();
        vTaskDelay(duration / 2);
        /* phase v:  u: high-z, v: high, w: low */
        timSet6StepMotorPhaseV();
        Port_SetMotorDriverEnVW();
        vTaskDelay(duration / 2);
        Port_SetMotorDriverEnUV();
        vTaskDelay(duration / 2);
        /* phase w:  u: low, v: high-z, w: high */
        timSet6StepMotorPhaseW();
        Port_SetMotorDriverEnWU();
        vTaskDelay(duration / 2);
        Port_SetMotorDriverEnVW();
        vTaskDelay(duration / 2);
    }
}

void taskAppUart(void* pvParameters) {
    const TickType_t durationTx = 100;
    RxDataType rxData = '\0';

    for (;;) {
        if (Uart2_ReadData(&rxData) == UartRetFetchData) {
            uint8_t ref = motorSpdLvlRef;

            /* update motor speed reference */
            if (rxData == '+') {
                ref++;
                if (ref >= mAppMotorSpeedMax) {
                    ref--;
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

#define adcBemfPhaseU 0
#define adcBemfPhaseV 1
#define adcBemfPhaseW 2
void taskAppAdcBemf(void* pvParameters) {
    const TickType_t durationTx = 1; /* 1ms */
    int16_t adcValue = 0;
    char_t uartTxData[6];

    for (;;) {
        adcValue = vAdcConvertADC1IN9();
        /* convert to ascii code */
        uartTxData[0] = (adcValue % 1000) + '0';
        uartTxData[1] = (adcValue / 100 % 10) + '0';
        uartTxData[2] = (adcValue / 10 % 10) + '0';
        uartTxData[3] = (adcValue % 10) + '0';
        uartTxData[4] = '\n';
        uartTxData[5] = '\0';
        /* tx uart */
        Usart2_TransmitBytes(uartTxData);

        vTaskDelay(durationTx);
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
void vApplicationTickHook(void) { systickCount++; }
