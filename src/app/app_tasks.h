#ifndef APP_TASK_H
#define APP_TASK_H

/* FreeRTOS */
#include "Queue.h"

/* app tasks */
extern void taskAppLedBlink(void* pvParameters);
extern void taskAppMotor(void* pvParameters);
extern void taskAppUart(void* pvParameters);
extern void taskAppAdcBemf(void* pvParameters);

#endif /* APP_TASK_H */
