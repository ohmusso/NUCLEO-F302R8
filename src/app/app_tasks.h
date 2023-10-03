#ifndef APP_TASK_H
#define APP_TASK_H

/* FreeRTOS */
// #include "FreeRTOS.h"
// #include "task.h"

/* app tasks */
extern void taskAppLedBlink();
extern void taskAppLow();
extern void taskAppMid();
extern void taskAppHigh();
extern void taskAppUartTx();

#endif /* APP_TASK_H */
