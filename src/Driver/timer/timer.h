#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H

#include "Platform_Types.h"

extern void Timer_Init();

extern uint16 tim1GetCnt();
extern uint32 tim2GetCnt(void);
extern void tim2clearCnt(void);

extern uint8 tim1IsUIFSet();
extern uint8 tim1ClearUIF();
extern uint8 tim1IsCC1IFSet();
extern void tim1ClearCC1IF(void);

extern void tim1Start3PhasePwm();
extern void tim1Set3PhasePwmCfg(uint16 width, uint16 duty);
extern void tim1Flip3PhasePwm();

extern void timSet6StepMotorPhaseU(void);
extern void timSet6StepMotorPhaseV(void);
extern void timSet6StepMotorPhaseW(void);

#endif /* DRIVER_TIMER_H */