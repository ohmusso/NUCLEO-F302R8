#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H

#include "Platform_Types.h"

extern void Timer_Init();

extern uint16 tim1GetCnt();
extern uint8 tim1IsUIFSet();
extern uint8 tim1ClearUIF();
extern uint8 tim1IsCC1IFSet();
extern uint8 tim1ClearCC1IF();

extern void tim1Start3PhasePwm();
extern void tim1Flip3PhasePwm();

#endif /* DRIVER_TIMER_H */