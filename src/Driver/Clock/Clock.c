#include "clock.h"

/* referenced by FreeRTOS*/
uint32 SystemCoreClock = ((uint32)8000000); /* 8MHz */

#define RCC_BASE_ADDRESS 0x40021000 /*  Reset and clock control RCC  */
/*  SysTick Control and Status Register */
/*  Cortex-M4 Register.                 */
#define SYST_BASE_ADDRESS 0xE000E010

/* RCC Register Map*/
typedef struct {
    uint32 notUsedCR;
    uint32 notUsedCFGR;
    uint32 notUsedCIR;
    uint32 notUsedAPB2RSTR;
    uint32 notUsedAPB1RSTR;
    uint32 AHBENR;
    uint32 APB2ENR;
    uint32 APB1ENR;
    uint32 notUsedBDCR;
    uint32 notUsedCSR;
    uint32 notUsedAHBRSTR;
    uint32 notUsedCFGR2;
    uint32 CFGR3;
} StRCC;

/* AHBENR */
#define RCC_AHBENR_Disabled 0x00
#define RCC_AHBENR_Enabled 0x01

#define RCC_AHAENR_IOPAEN (RCC_AHBENR_Enabled << 17)
#define RCC_AHBENR_IOPBEN (RCC_AHBENR_Enabled << 18)
#define RCC_AHBENR_IOPCEN (RCC_AHBENR_Enabled << 19)
#define RCC_AHBENR_ADC12EN (RCC_AHBENR_Enabled << 28)
#define Init_RCC_AHBENR                                           \
    (RCC_AHBENR_ADC12EN | RCC_AHBENR_IOPCEN | RCC_AHBENR_IOPBEN | \
     RCC_AHAENR_IOPAEN)

/* APB1ENR */
#define RCC_APB1ENR_Disabled 0x00
#define RCC_APB1ENR_Enabled 0x01

#define RCC_APB1ENR_USERT2 (RCC_APB1ENR_Enabled << 17)
#define Init_RCC_APB1ENR (RCC_APB1ENR_USERT2)

/* APB2ENR */
#define RCC_APB2ENR_Disabled 0x00
#define RCC_APB2ENR_Enabled 0x01

#define RCC_APB2ENR_TIM1 (RCC_APB2ENR_Enabled << 11)
#define RCC_APB2ENR_SYSCFG (RCC_APB2ENR_Enabled << 0)
#define Init_RCC_APB2ENR (RCC_APB2ENR_TIM1 | RCC_APB2ENR_SYSCFG)

/* CFGR3 */
#define RCC_CFGR3_TIM1SW_PCLK2 0x00

#define RCC_CFGR3_TIM1SW (RCC_CFGR3_TIM1SW_PCLK2 << 8)
#define Init_RCC_CFGR3 (RCC_CFGR3_TIM1SW)

/* pointer to RCC register */
#define stpRCC ((StRCC*)(RCC_BASE_ADDRESS))

/* SYST Register Map */
typedef struct {
    uint32 CSR;
    uint32 RVR;
    uint32 CVR;
    uint32 notUsedCALIB;
} StSYST;

#define SYST_CSR_ENABLE_disable ((uint32)(0 << 0))
#define SYST_CSR_ENABLE_enable ((uint32)(1 << 0))
#define SYST_CSR_TICKINT_disable ((uint32)(0 << 1))
#define SYST_CSR_TICKINT_enable ((uint32)(1 << 1))
#define SYST_CSR_CLOCKSOURCE_external ((uint32)(0 << 2))
#define SYST_CSR_CLOCKSOURCE_processor ((uint32)(1 << 2))
#define SYST_RVR_1msTick (1000)
#define SYST_CVR_Reset (0x00000000)

#define Init_SYST_CSR                                          \
    (SYST_CSR_CLOCKSOURCE_external | SYST_CSR_TICKINT_enable | \
     SYST_CSR_ENABLE_enable)
#define Init_SYST_RVR ((uint32)SYST_RVR_1msTick)
#define Init_SYST_CVR SYST_CVR_Reset

#define stpSYST ((StSYST*)(SYST_BASE_ADDRESS))

/*  SYSCLK(source clock)                        */
/*   - HSI(High Speed Internal) 8MHz            */
/*  HCLK(bus clock)                             */
/*   - SYSCLK / HPRE                            */
/*   - HPRE: default is 1                       */
/*  cortex System Timer: external clock         */
/*   - HCLK / 8 = 1MHz                          */
/*  Systick                                     */
/*   - cortex System Timer * SYST_RVR           */
void Clock_Init() {
    stpRCC->AHBENR = Init_RCC_AHBENR;
    stpRCC->APB2ENR = Init_RCC_APB2ENR;
    stpRCC->APB1ENR = Init_RCC_APB1ENR;
    stpRCC->CFGR3 = Init_RCC_CFGR3;
    /* FreeRTOS initialize and use Systick_Handler. */
    /* if enable following codes */
    /* xPortSysTickHandler is exceptionally called and infinite loop */
    /* stpSYST->CSR = Init_SYST_CSR; */
    /* stpSYST->RVR = Init_SYST_RVR; */
    /* stpSYST->CVR = Init_SYST_CVR; */
}
