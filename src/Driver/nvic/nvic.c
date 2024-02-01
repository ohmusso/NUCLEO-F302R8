#include "nvic.h"

/*  Cortex-M4 Register. */
#define NVIC_ISERx_BASE_ADDRESS 0xE000E100
#define NVIC_ICPRx_BASE_ADDRESS 0xE000E280
#define NVIC_IPRx_BASE_ADDRESS 0xE000E400
#define AIRCR_BASE_ADDRESS 0xE000ED0C
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

#define NVIC_ISER0_EXTI03_Enabled (NVIC_ISER_Enabled << 9)
#define NVIC_ISER0_ADC_Enabled (NVIC_ISER_Enabled << 18)
#define NVIC_ISER0_TIM1_UP_Enabled (NVIC_ISER_Enabled << 25)
#define NVIC_ISER0_TIM1_CC_Enabled (NVIC_ISER_Enabled << 27)
#define Init_NVIC_ISER0                                    \
    (NVIC_ISER0_TIM1_CC_Enabled | NVIC_ISER0_ADC_Enabled | \
     NVIC_ISER0_EXTI03_Enabled)
// #define Init_NVIC_ISER0 (NVIC_ISER0_TIM1_UP_Enabled |
// NVIC_ISER0_EXTI03_Enabled)

#define NVIC_ISER1_USART2_Enabled (NVIC_ISER_Enabled << 6)
#define NVIC_ISER1_USART3_Enabled (NVIC_ISER_Enabled << 7)
#define NVIC_ISER1_EXTI10_15_Enabled (NVIC_ISER_Enabled << 8)
#define Init_NVIC_ISER1 \
    (NVIC_ISER1_EXTI10_15_Enabled | NVIC_ISER1_USART2_Enabled)

/* ICPRx */
#define NVIC_ICPR_Clear 0x01

/* #define NVIC_ICPR_TIM1_CC (NVIC_ICPR_Clear << 27) */

/* IPR */
#define NVIC_IPR_Num (21u) /* IPR00 - IPR20 */
#define NVIC_IP_Num (NVIC_IPR_Num * 4u)
/* initialize priority. */
/*  - F302R8 implement 4 bit interrupt priority. */
/*    Use bit7 - bit4 in IP[x] */
/*    Highest is 0, lowest is 15. */
/*  - This project use FreeRTOS.FreeRTOS is using intterupt and set priority. */
/*    User defined interrupt priority must below */
/*    configMAX_SYSCALL_INTERRUPT_PRIORITY. */
#define NVIC_IP_Shift (4u)
#define NVIC_IP_Mask (0xF0u)

/* AIRCR */
#define AIRCR_PRIGROUP_SubGroupOnly (3u)
#define AIRCR_PRIGROUP_Shift (8u)
#define AIRCR_PRIGROUP_Set_SubGroupOnly \
    (AIRCR_PRIGROUP_SubGroupOnly << AIRCR_PRIGROUP_Shift)

/* pointer to register */
#define stpISER ((StNVIC_ISER*)(NVIC_ISERx_BASE_ADDRESS))
#define stpICPR ((StNVIC_ICPR*)(NVIC_ICPRx_BASE_ADDRESS))
#define stpIP ((uint8*)NVIC_IPRx_BASE_ADDRESS)
#define pAIRCR ((uint32*)AIRCR_BASE_ADDRESS)

void Nvic_Init(void) {
    /* enable interrupt */
    stpISER->ISER0 = Init_NVIC_ISER0;
    stpISER->ISER1 = Init_NVIC_ISER1;
}

void Nvic_SetInterruptGroupPrioriySubGroupOnly(void) {
    *(pAIRCR) |= AIRCR_PRIGROUP_Set_SubGroupOnly;
}

void Nvic_InitInterruptPrioriy(const uint8 priority) {
    uint8 i;
    uint8 _priority = (priority << NVIC_IP_Shift) & NVIC_IP_Mask;
    for (i = 0; i < NVIC_IP_Num; i++) {
        stpIP[i] = _priority;
    }
}

void Nvic_ComEsp32Config(void) {
    /* enable USART3 interrupt */
    stpISER->ISER1 |= NVIC_ISER1_USART3_Enabled;
}
