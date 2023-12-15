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
    uint16 pwmWidth;
    uint16 pwmDuty;
    TickType_t stepTime;
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
    /* 1step: 1000[ms], - [rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 12, 1000},
    /* 1step: 48[ms], 30[rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 12, 48},
    /* 1step: 24[ms], 60[rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 8, 24},
    /* 1step: 6[ms], 240[rpm] */
    {mApp3PhasePwmWitdh, mApp3PhasePwmWitdh / 4, 6}};

static uint8_t motorSpdLvlRef = 0;
uint16_t adcResult = 0;
void taskAppMotor(void* pvParameters) {
    TickType_t duration = 0;

    tim1Start3PhasePwm();

    for (;;) {
        /* set duration from motor speed ref */
        tim1Set3PhasePwmCfg(sixStepCtrlCfg[motorSpdLvlRef].pwmWidth,
                            sixStepCtrlCfg[motorSpdLvlRef].pwmDuty);
        duration = sixStepCtrlCfg[motorSpdLvlRef].stepTime;

        if ((motorSpdLvlRef == 0) || (motorSpdLvlRef >= mAppMotorSpeedMax)) {
            /* stop motor */
            Port_SetMotorDriverDisable();
            vTaskDelay(100);
            continue;
        }

        /* phase uv:  u: high, v: low, w: high-z */
        timSet6StepMotorPhaseU();
        Port_SetMotorDriverEnUV();
        ADC1_SetSequenceBemf3();
        ADC1_StartConv();
        vTaskDelay(duration);
        /* phase uw:  u: high, v: high-z, w: low */
        ADC1_StopConv();
        adcResult = 0;
        Port_SetMotorDriverDisable();
        vTaskDelay(1000);
        // Port_SetMotorDriverEnWU();
        // vTaskDelay(duration);
        // /* phase vw:  u: high-z, v: high, w: low */
        // timSet6StepMotorPhaseV();
        // Port_SetMotorDriverEnVW();
        // vTaskDelay(duration);
        // /* phase vu:  u: low, v: high, w: high-z */
        // Port_SetMotorDriverEnUV();
        // vTaskDelay(duration);
        // /* phase wu:  u: low, v: high-z, w: high */
        // timSet6StepMotorPhaseW();
        // Port_SetMotorDriverEnWU();
        // vTaskDelay(duration);
        // /* phase wv:  u: high-z, v: low, w: high */
        // Port_SetMotorDriverEnVW();
        // vTaskDelay(duration);
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
