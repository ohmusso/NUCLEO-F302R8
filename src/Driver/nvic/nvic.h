#ifndef DRIVER_NVIC_H
#define DRIVER_NVIC_H

#include "Platform_Types.h"

extern void Nvic_Init();
extern void Nvic_Clear();
extern void Nvic_SetInterruptGroupPrioriySubGroupOnly(void);
extern void Nvic_InitInterruptPrioriy(uint8 priority);
extern void Nvic_ComEsp32Config(void);

#endif /* DRIVER_NVIC_H */