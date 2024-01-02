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
TaskHandle_t xTaskHandleAppMotor = 0;

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
    uint16_t pwmWidth;
    uint32_t stepTime;
    int32_t acceptErrorStepTime;
} SixStepMotorCtrlCfg_t;

#define mApp3PhasePwmWitdh \
    ((uint16_t)25) /* pwm center aligned mode: width = ARR*2 = 50[us] */
#define mApp3PhasePwmMaxDuty ((uint16_t)20)
#define mApp3PhasePwmMinDuty ((uint16_t)3)

/* Motor spec */
/* 3Phase u, v, w */
/* Poles number: 14 */
/* 1Phase electrical angle: 17.1[deg] */

/* 6step motor control */
/* 1step electrical angle: 8.56[deg] */
#define mAppMotorSpeedMax 5
#define mAppMotorStop 0
static const SixStepMotorCtrlCfg_t sixStepCtrlCfg[mAppMotorSpeedMax] = {
    /* stop */
    {mApp3PhasePwmWitdh, 0, 0},
    /* 1step: 5[ms], xx [rpm] */
    {mApp3PhasePwmWitdh, 1000, 100},
    /* 1step: 1[ms], xx[rpm] */
    {mApp3PhasePwmWitdh, 800, 50},
    /* 1step: 500[us], xx[rpm] */
    {mApp3PhasePwmWitdh, 500, 50},
    /* 1step: 250[us], xx[rpm] */
    {mApp3PhasePwmWitdh, 250, 50}};

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

uint16_t adcResult = 0;
uint16_t bemfThreshold;
static uint32_t bemfValStep1 = 0;
static uint32_t bemfValStep2 = 0;
static uint32_t bemfValStep3 = 0;
static uint32_t bemfValStep4 = 0;
static uint32_t bemfValStep5 = 0;
static uint32_t bemfValStep6 = 0;
static uint32_t motorStep;
static uint16_t pwmWidth;
static uint16_t pwmDuty;
static int32_t stepTime;
static int32_t stepTimeAvg;
static int32_t errStepTime;
static void motor6stepControll(void) {
    const TickType_t forceCtrlWait = 5; /* 5[ms] */
    /* feedback controll */
    const uint32_t enterClearNotifiedVal = 0xFFFFFFFF;
    const int32_t ctrlCycle = 10;
    const int32_t ctrlSyncCycle = 50;
    const uint8_t motorCtrlStateOK = 0;
    const uint8_t motorCtrlStateDutyChangeCnt = 30;
    const uint16_t bemfThresholdInit = 0;
    const uint16_t bemfThresholdMax = 100;
    const uint16_t bemfThresholdMin = 5;
    int32_t cycleCnt = 0;
    int32_t cycleSyncCnt = 0;
    int32_t refStepTime = 0;
    int32_t stepTimeSum = 0;
    uint8_t motorSpdLvl = 0;
    uint8_t motorCtrlState = motorCtrlStateOK;

    cycleCnt = 0;
    stepTime = 0;
    errStepTime = 0;
    pwmWidth = sixStepCtrlCfg[mAppMotorStop].pwmWidth;
    pwmDuty = mApp3PhasePwmMinDuty;
    bemfThreshold = bemfThresholdInit;

    for (;;) {
        if (motorSpdLvlRef == 0) {
            break;
        }
        motorSpdLvl = motorSpdLvlRef;
        refStepTime = (int32_t)sixStepCtrlCfg[motorSpdLvl].stepTime;

        /* set pwm */
        tim1Set3PhasePwmCfg(pwmWidth, pwmDuty);

        /* cleaer timer */
        tim2clearCnt();

        /* phase uv:  u: high, v: low, w: high-z */
        motorStep = 1;
        timSet6StepMotorPhaseU();
        Port_SetMotorDriverEnUV();
        ADC1_StartConvBemf3();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfValStep1,
                        forceCtrlWait); /* clear notified value */
        /* phase uw:  u: high, v: high-z, w: low */
        motorStep = 2;
        Port_SetMotorDriverEnWU();
        ADC1_StartConvBemf2();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfValStep2, forceCtrlWait);
        /* phase vw:  u: high-z, v: high, w: low */
        motorStep = 3;
        timSet6StepMotorPhaseV();
        Port_SetMotorDriverEnVW();
        ADC1_StartConvBemf1();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfValStep3, forceCtrlWait);
        /* phase vu:  u: low, v: high, w: high-z */
        motorStep = 4;
        Port_SetMotorDriverEnUV();
        ADC1_StartConvBemf3();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfValStep4, forceCtrlWait);
        /* phase wu:  u: low, v: high-z, w: high */
        motorStep = 5;
        timSet6StepMotorPhaseW();
        Port_SetMotorDriverEnWU();
        ADC1_StartConvBemf2();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfValStep5, forceCtrlWait);
        /* phase wv:  u: high-z, v: low, w: high */
        motorStep = 6;
        Port_SetMotorDriverEnVW();
        ADC1_StartConvBemf1();
        xTaskNotifyWait(enterClearNotifiedVal, 0, &bemfValStep6, forceCtrlWait);
        /* 6step end */
        stepTime = tim2GetCnt() / 8; /* 0.125us => 1us */
        stepTime = stepTime / 6;     /* 6step time => avg 1step time*/

        /* speed controll */
        cycleCnt++;
        stepTimeSum += stepTime;
        if (cycleCnt >= ctrlCycle) {
            stepTimeAvg = stepTimeSum / cycleCnt;
            errStepTime = stepTimeAvg - refStepTime;
            if (cycleSyncCnt <= ctrlSyncCycle) {
                cycleSyncCnt++;
            } else {
                if (motorCtrlState <= motorCtrlStateDutyChangeCnt) {
                    if (errStepTime >
                        sixStepCtrlCfg[motorSpdLvl].acceptErrorStepTime) {
                        if (bemfThreshold < bemfThresholdMax) {
                            bemfThreshold++;
                        }
                        motorCtrlState++;
                    } else if (errStepTime < -(sixStepCtrlCfg[motorSpdLvl]
                                                   .acceptErrorStepTime)) {
                        if (bemfThreshold > bemfThresholdMin) {
                            bemfThreshold--;
                        }
                        motorCtrlState++;
                    } else {
                        /* speed is approximately same */
                        motorCtrlState = motorCtrlStateOK;
                    }
                } else if (motorCtrlState >= motorCtrlStateDutyChangeCnt) {
                    if (errStepTime >
                        sixStepCtrlCfg[motorSpdLvl].acceptErrorStepTime) {
                        pwmDuty++;
                        motorCtrlState = motorCtrlStateOK;
                        cycleSyncCnt = 0;
                    } else if (errStepTime < -(sixStepCtrlCfg[motorSpdLvl]
                                                   .acceptErrorStepTime)) {
                        pwmDuty--;
                        motorCtrlState = motorCtrlStateOK;
                        cycleSyncCnt = 0;
                    } else {
                        motorCtrlState = motorCtrlStateOK;
                    }
                }
            }
            /* reset */
            cycleCnt = 0;
            stepTimeSum = 0;
        }
        /* set duty */
        /* - clip */
        if (pwmDuty > mApp3PhasePwmMaxDuty) {
            pwmDuty = mApp3PhasePwmMaxDuty;
        } else if ((pwmDuty < mApp3PhasePwmMinDuty)) {
            pwmDuty = mApp3PhasePwmMinDuty;
        } else {
            /* nop */
        }
    }
}

void taskAppIsrHandlerAdc(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    adcResult = vAdcRead();
    if (adcResult > bemfThreshold) {
        ADC1_StopConv();
        xTaskNotifyFromISR(xTaskHandleAppMotor, (uint32_t)adcResult,
                           eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
