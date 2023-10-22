#ifndef APP_TASK_H
#define APP_TASK_H

/* FreeRTOS */
#include "Queue.h"

/* app tasks */
extern void taskAppLedBlink();
extern void taskAppMotor();
extern void taskAppUart();

#endif /* APP_TASK_H */
