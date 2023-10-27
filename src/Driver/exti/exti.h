#ifndef DRIVER_EXTI_H
#define DRIVER_EXTI_H

#include "Platform_Types.h"

extern void Exti_Init();

extern void Exti_ClearExti3();
extern uint8 Exti_ClearExti15_10();

#endif /* DRIVER_EXTI_H */