#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H

#include "Platform_Types.h"

extern void ADC1_Init(void);
extern void ADC1_StartConv(void);
extern void ADC1_StartConvBemf1(void);
extern void ADC1_StartConvBemf2(void);
extern void ADC1_StartConvBemf3(void);
extern void ADC1_StopConv(void);
extern uint16 vAdcConvertADC1IN9(void);
extern uint16 vAdcRead(void);
extern void ADC1_SetSequenceBemf1(void);
extern void ADC1_SetSequenceBemf2(void);
extern void ADC1_SetSequenceBemf3(void);

#endif /* DRIVER_ADC_H */