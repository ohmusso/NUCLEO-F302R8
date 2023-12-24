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

static void motor6stepControll(void);

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
    uint16 pwmWidth;
    uint16 pwmDuty;
    uint16 bemfThresh;
    uint32_t stepTime;
} SixStepMotorCtrlCfg_t;

#define mApp3PhasePwmWitdh \
    ((uint16)100) /* pwm center aligned mode: width = ARR*2 = 200[us] */

/* Motor spec */
/* 3Phase u, v, w */
/* Poles number: 14 */
/* 1Phase electrical angle: 17.1[deg] */

/* 6step motor control */
/* 1step electrical angle: 8.56[deg] */
#define mAppMotorSpeedMax 5
static const SixStepMotorCtrlCfg_t sixStepCtrlCfg[mAppMotorSpeedMax] = {
    /* stop */
    {mApp3PhasePwmWitdh, 0, 0},
    /* 1step: 5[ms], xx [rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 12, 50, 5000},
    /* 1step: 1[ms], xx[rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 8, 80, 1000},
    /* 1step: 500[us], xx[rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 4, 90, 500},
    /* 1step: 250[us], xx[rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 2, 100, 250}};

static uint8_t motorSpdLvlRef = 0;
void taskAppMotor(void* pvParameters) {
    tim1Start3PhasePwm();

    for (;;) {
        if ((motorSpdLvlRef == 0) || (motorSpdLvlRef >= mAppMotorSpeedMax)) {
            /* stop motor */
            Port_SetMotorDriverDisable();
            vTaskDelay(100);
            continue;
        }

        motor6stepControll();
    }
}

#define enterClearNotifiedVal ((uint32_t)0xFFFFFFFF)
uint16_t adcResult = 0;
uint16_t bemfThreshold = 50;
static uint32_t motorStep;
static uint16_t pwmWidth;
static uint16_t pwmDuty;
static uint32_t stepTime = 0;
static void motor6stepControll(void) {
    const uint32_t initialWait = 10; /* 10[ms] */
    // uint32_t refStepTime = 0;
    uint8_t motorSpdLvl = 0;
    uint32_t bemfVal = 0;

    motorSpdLvl = motorSpdLvlRef;
    // refStepTime = sixStepCtrlCfg[motorSpdLvl].stepTime;
    pwmWidth = sixStepCtrlCfg[motorSpdLvl].pwmWidth;
    pwmDuty = sixStepCtrlCfg[motorSpdLvl].pwmDuty;
    bemfThreshold = sixStepCtrlCfg[motorSpdLvl].bemfThresh;

    for (;;) {
        if ((motorSpdLvlRef == 0) || (motorSpdLvl != motorSpdLvlRef)) {
            break;
        }

        /* set pwm */
        tim1Set3PhasePwmCfg(pwmWidth, pwmDuty);

        /* cleaer timer */
        tim2clearCnt();

        /* phase uv:  u: high, v: low, w: high-z */
        motorStep = 1;
        timSet6StepMotorPhaseU();
        Port_SetMotorDriverEnUV();
        ADC1_StartConvBemf3();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfVal,
                        initialWait); /* clear notified value */
        ADC1_StopConv();
        /* phase uw:  u: high, v: high-z, w: low */
        motorStep = 2;
        Port_SetMotorDriverEnWU();
        ADC1_StartConvBemf2();
        xTaskNotifyWait(0, 0, &bemfVal, initialWait);
        ADC1_StopConv();
        /* phase vw:  u: high-z, v: high, w: low */
        motorStep = 3;
        timSet6StepMotorPhaseV();
        Port_SetMotorDriverEnVW();
        ADC1_StartConvBemf1();
        xTaskNotifyWait(0, 0, &bemfVal, initialWait);
        ADC1_StopConv();
        /* phase vu:  u: low, v: high, w: high-z */
        motorStep = 4;
        Port_SetMotorDriverEnUV();
        ADC1_StartConvBemf3();
        xTaskNotifyWait(0, 0, &bemfVal, initialWait);
        ADC1_StopConv();
        /* phase wu:  u: low, v: high-z, w: high */
        motorStep = 5;
        timSet6StepMotorPhaseW();
        Port_SetMotorDriverEnWU();
        ADC1_StartConvBemf2();
        xTaskNotifyWait(0, 0, &bemfVal, initialWait);
        ADC1_StopConv();
        /* phase wv:  u: high-z, v: low, w: high */
        motorStep = 6;
        Port_SetMotorDriverEnVW();
        ADC1_StartConvBemf1();
        xTaskNotifyWait(0, 0, &bemfVal, initialWait);
        ADC1_StopConv();
        /* 6step end */
        stepTime = tim2GetCnt() / 8; /* 0.125us => 1us */
        stepTime = stepTime / 6;     /* 6step time => avg 1step time*/
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
            // Usart2_Transmit(rxData);
        }
        vTaskDelay(durationTx);
    }
}

#define adcBemfPhaseU 0
#define adcBemfPhaseV 1
#define adcBemfPhaseW 2
void taskAppAdcBemf(void* pvParameters) {
    const TickType_t durationTx = 10; /* 10ms */
    uint16_t adcValue = 0;
    const char_t uartTxDataSize = 8;
    char_t uartTxData[uartTxDataSize];

    for (;;) {
        /* adc start */
        adcValue = vAdcConvertADC1IN9();
        /* convert to ascii code */
        uartTxData[0] = ((adcValue >> 12) & 0x000F); /* 1000 place of 4 hex */
        uartTxData[1] = ((adcValue >> 8) & 0x000F);  /* 100 place */
        uartTxData[2] = ((adcValue >> 4) & 0x000F);  /* 10 place */
        uartTxData[3] = (adcValue & 0x000F);         /* 1 place */
        /* tx uart */
        Usart2_TransmitHexDatas(uartTxData, 4);

        vTaskDelay(durationTx);
    }
}

/* FreeRTOS API */
/* interupt cycle: 1ms */
void vApplicationTickHook(void) { systickCount++; }
