#ifndef APP_TASK_H
#define APP_TASK_H

/* FreeRTOS */
#include "Queue.h"

/* task handle */
extern TaskHandle_t xTaskHandleAppMotor;

/* app tasks */
extern void taskAppLedBlink(void* pvParameters);
extern void taskAppMotor(void* pvParameters);
extern void taskAppUart(void* pvParameters);
extern void taskAppAdcBemf(void* pvParameters);

/* ISR Handler */
void taskAppIsrHandlerAdc(void);

#endif /* APP_TASK_H */
