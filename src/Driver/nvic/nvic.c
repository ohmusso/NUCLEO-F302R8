#include "nvic.h"

/*  Cortex-M4 Register. */
#define NVIC_ISERx_BASE_ADDRESS 0xE000E100
#define NVIC_ICPRx_BASE_ADDRESS 0xE000E280

/* ISER Register Map*/
typedef struct {
    uint32 ISER0;
    uint32 ISER1;
    uint32 ISER2;
} StNVIC_ISER;

typedef struct {
    uint32 ICPR0;
    uint32 ICPR1;
    uint32 ICPR2;
} StNVIC_ICPR;

/* ISERx */
#define NVIC_ISER_Disabled 0x00
#define NVIC_ISER_Enabled 0x01

#define NVIC_ISER_TIM1_CC_Enabled (NVIC_ISER_Enabled << 27)
#define Init_NVIC_ISER0 (NVIC_ISER_TIM1_CC_Enabled)

/* ICPRx */
#define NVIC_ICPR_Clear 0x01

#define NVIC_ICPR_TIM1_CC (NVIC_ICPR_Clear << 27)

/* pointer to register */
#define stpISER ((StNVIC_ISER*)(NVIC_ISERx_BASE_ADDRESS))
#define stpICPR ((StNVIC_ICPR*)(NVIC_ICPRx_BASE_ADDRESS))

void Nvic_Init() { stpISER->ISER0 = Init_NVIC_ISER0; }

inline void Nvic_Clear() { stpICPR->ICPR0 |= NVIC_ICPR_TIM1_CC; }
