#include "timer.h"

#define TIM1_BASE_ADDRESS 0x40012C00 /* TIM1 */

typedef struct {
    uint32 CR1;
    uint32 CR2;
    uint32 notUsedSMCR;
    uint32 DIER;
    uint32 SR;
    uint32 EGR;
    uint32 CCMR1;
    uint32 CCMR2;
    uint32 CCER;
    uint16 CNT;
    uint16 notUsedCNT_H;
    uint16 PSC;
    uint16 notUsedPSC_H;
    uint16 ARR;
    uint16 notUsedARR_H;
    uint16 RCR;
    uint16 notUsedRCR_H;
    uint16 CCR1;
    uint16 notUsedCCR1_H;
    uint16 CCR2;
    uint16 notUsedCCR2_H;
    uint16 CCR3;
    uint16 notUsedCCR3_H;
    uint16 CCR4;
    uint16 notUsedCCR4_H;
    uint32 BDTR;
    uint32 DCR;
    uint32 notUsedDMAR;
    uint32 notUsedOR;
    uint32 CCMR3;
    uint32 CCR5;
    uint32 notUsedCCR6;
} StTIM1;

/* CR1 */
#define TIM1_CR1_CKD_CkInt 0x00
#define TIM1_CR1_CKD (TIM1_CR1_CKD_CkInt << 8)

#define TIM1_CR1_ARPE_Enable (0x01)
#define TIM1_CR1_ARPE (TIM1_CR1_ARPE_Enable << 7)

#define TIM1_CR1_CMS_CenterModeIntFlagSetWhenDown (0x01)
#define TIM1_CR1_CMS_CenterModeIntFlagSetWhen (0x02)
#define TIM1_CR1_CMS (TIM1_CR1_CMS_CenterModeIntFlagSetWhenDown << 5)

#define TIM1_CR1_DIR_Upcounter (0x00)
#define TIM1_CR1_DIR (TIM1_CR1_DIR_Upcounter << 4)

#define TIM1_CR1_OPM_DisableOnePulse (0x00)
#define TIM1_CR1_OPM (TIM1_CR1_OPM_DisableOnePulse << 3)

#define TIM1_CR1_CEN_CounterDisable (0x00)
#define TIM1_CR1_CEN_CounterEnable (0x01)
#define TIM1_CR1_CEN (TIM1_CR1_CEN_CounterEnable << 0)

#define Init_TIM1_CR1 \
    (TIM1_CR1_CKD | TIM1_CR1_ARPE | TIM1_CR1_CMS | TIM1_CR1_DIR | TIM1_CR1_OPM)

/* CR2 */
/* MMS1: TRGO1: Trigger for Slave Timer */
#define TIM1_CR2_MMS_Update (2u)
#define TIM1_CR2_MMS_ComparePulse (3u)
#define TIM1_CR2_MMS_OC1REFC (6u)
#define TIM1_CR2_MMS_SHIFT (4u)
#define TIM1_CR2_MMS_Set_OC1REFC (TIM1_CR2_MMS_OC1REFC << TIM1_CR2_MMS_SHIFT)
/* MMS2: TRGO2: Trigger for ADC */
#define TIM1_CR2_MMS2_ComparePulse (3u)
#define TIM1_CR2_MMS2_OC1REFC (4u)
#define TIM1_CR2_MMS2_SHIFT (20u)
#define TIM1_CR2_MMS2_Set_OC1REFC (TIM1_CR2_MMS2_OC1REFC << TIM1_CR2_MMS2_SHIFT)
#define TIM1_CR2_MMS2_Set_ComparePulse \
    (TIM1_CR2_MMS2_ComparePulse << TIM1_CR2_MMS2_SHIFT)

/* DIER */
#define TIM1_DIER_CCXIE_Enable (0x01)
#define TIM1_DIER_CC1IE (TIM1_DIER_CCXIE_Enable << 1)

#define TIM1_DIER_UIE_Enable (0x01)
#define TIM1_DIER_UIE (TIM1_DIER_UIE_Enable << 0)

// #define Init_TIM1_DIER (TIM1_DIER_UIE_Enable)
// #define Init_TIM1_DIER (TIM1_DIER_CC1IE)
#define Init_TIM1_DIER (0)

/* SR */
#define TIM1_SR_CCXIF_InterruptNotOcuure (0x00)
#define TIM1_SR_CCXIF_InterruptOcuure (0x01)
#define TIM1_SR_UIF_BIT (1 << 0)
#define TIM1_SR_UIF_SHIFT (0)
#define TIM1_SR_CC1IF_BIT (1 << 1)
#define TIM1_SR_CC1IF_SHIFT (1)

/* EGR */
#define TIM1_EGR_CCXG_GenarateEvent (1u)
#define TIM1_EGR_CC1G_SHIFT (1u)
#define TIM1_EGR_Set_CC1G (TIM1_EGR_CCXG_GenarateEvent << TIM1_EGR_CC1G_SHIFT)

/* CCMR1 */
#define TIM1_CCMRX_CCXS_Output 0x00
#define TIM1_CCMR1_CC1S (TIM1_CCMRX_CCXS_Output << 0)
#define TIM1_CCMR1_CC2S (TIM1_CCMRX_CCXS_Output << 8)

#define TIM1_CCMRX_OCXPE_PreloadEnable 0x01
#define TIM1_CCMR1_OC1PE (TIM1_CCMRX_OCXPE_PreloadEnable << 3)
#define TIM1_CCMR1_OC2PE (TIM1_CCMRX_OCXPE_PreloadEnable << 11)

#define TIM1_CCMRX_OCXM_PwmMode1 0x06
#define TIM1_CCMR1_OC1M (TIM1_CCMRX_OCXM_PwmMode1 << 4)
#define TIM1_CCMR1_OC2M (TIM1_CCMRX_OCXM_PwmMode1 << 12)

#define Init_TIM1_CCMR1                                                       \
    (TIM1_CCMR1_OC2M | TIM1_CCMR1_OC2PE | TIM1_CCMR1_CC2S | TIM1_CCMR1_OC1M | \
     TIM1_CCMR1_OC1PE | TIM1_CCMR1_CC1S)

/* CCMR2 */
#define TIM1_CCMR2_CC3S (TIM1_CCMRX_CCXS_Output << 0)
#define TIM1_CCMR2_OC3PE (TIM1_CCMRX_OCXPE_PreloadEnable << 3)
#define TIM1_CCMR2_OC3M (TIM1_CCMRX_OCXM_PwmMode1 << 4)

#define Init_TIM1_CCMR2 (TIM1_CCMR2_OC3M | TIM1_CCMR2_OC3PE | TIM1_CCMR2_CC3S)

/* CCMR3 */
#define TIM1_CCMR3_OC5PE (TIM1_CCMRX_OCXPE_PreloadEnable << 3)
#define TIM1_CCMR3_OC5M (TIM1_CCMRX_OCXM_PwmMode1 << 4)

#define Init_TIM1_CCMR3 (TIM1_CCMR3_OC5M | TIM1_CCMR3_OC5PE)

/* CCER */
#define TIM1_CCER_CCXE_Enable (0x01)
#define TIM1_CCER_CC1E (TIM1_CCER_CCXE_Enable << 0)
#define TIM1_CCER_CC2E (TIM1_CCER_CCXE_Enable << 4)
#define TIM1_CCER_CC3E (TIM1_CCER_CCXE_Enable << 8)

#define Init_TIM1_CCER (TIM1_CCER_CC3E | TIM1_CCER_CC2E | TIM1_CCER_CC1E)

/* PWM  */
/* - unit */
/* - width */
/* - duty */
/* unit = CK_INT / (PSC + 1) = 8MHz / 8 = 1MHz  */
#define Init_TIM1_PSC (7)
/* width =  ARR * 2 / unit =  500 * 2 * 1us = 1000us */
/*          CMS is Center-aligned mode, so double ARR  */
#define Init_TIM1_ARR (500)
/* duty =  CCRX / width = 500us / 1000us = 0.5  */
#define Init_TIM1_CCR1 (Init_TIM1_ARR / 2)
#define Init_TIM1_CCR2 (Init_TIM1_CCR1)
#define Init_TIM1_CCR3 (Init_TIM1_CCR1)

/* CCR5 */
#define TIM1_CCR5_GC5C3_BIT (1 << 31)
#define TIM1_CCR5_GC5C2_BIT (1 << 30)
#define TIM1_CCR5_GC5C1_BIT (1 << 29)
#define TIM1_CCR5_GC5_MASK \
    (~(TIM1_CCR5_GC5C3_BIT | TIM1_CCR5_GC5C2_BIT | TIM1_CCR5_GC5C1_BIT))
#define Init_TIM1_CCR5 \
    (TIM1_CCR5_GC5C3_BIT | TIM1_CCR5_GC5C2_BIT) /* mask pwm v, w */

/* BDTR */
#define TIM1_BDTR_MOE_Enable (0x01)
#define TIM1_BDTR_MOE (TIM1_BDTR_MOE_Enable << 15)

#define Init_TIM1_BDTR (TIM1_BDTR_MOE)

/* pointer to TIM1 register */
#define stpTIM1 ((StTIM1*)(TIM1_BASE_ADDRESS))

/* macro */
/* SR */
#define mIsTim1UIFSet() \
    ((((stpTIM1->SR & (~TIM1_SR_UIF_BIT)) >> TIM1_SR_UIF_SHIFT)) != 0)
#define mClearTim1UIF() (stpTIM1->SR &= (~TIM1_SR_UIF_BIT))

#define mIsTim1CC1IFSet() \
    ((((stpTIM1->SR & (~TIM1_SR_CC1IF_BIT)) >> TIM1_SR_CC1IF_SHIFT)) != 0)
#define mClearTim1CC1IF() (stpTIM1->SR &= (~TIM1_SR_CC1IF_BIT))

/* Clock must initialized before Port initialize */
void Timer_Init() {
    stpTIM1->CR1 = Init_TIM1_CR1;
    stpTIM1->CR2 = TIM1_CR2_MMS2_Set_ComparePulse;
    stpTIM1->DIER = Init_TIM1_DIER;
    stpTIM1->CCMR1 = Init_TIM1_CCMR1;
    stpTIM1->CCMR2 = Init_TIM1_CCMR2;
    stpTIM1->CCER = Init_TIM1_CCER;
    stpTIM1->CNT = 0;
    stpTIM1->ARR = Init_TIM1_ARR;
    stpTIM1->PSC = Init_TIM1_PSC;
    stpTIM1->CCR1 = Init_TIM1_CCR1;
    stpTIM1->CCR2 = Init_TIM1_CCR2;
    stpTIM1->CCR3 = Init_TIM1_CCR3;
    stpTIM1->CCMR3 = Init_TIM1_CCMR3;
    stpTIM1->CCR5 = Init_TIM1_CCR5;
}

uint16 tim1GetCnt() { return stpTIM1->CNT; }

uint8 tim1IsCC1IFSet() { return mIsTim1CC1IFSet(); }
void tim1ClearCC1IF(void) { mClearTim1CC1IF(); }

uint8 tim1IsUIFSet() { return mIsTim1UIFSet(); }
uint8 tim1ClearUIF() { return mClearTim1UIF(); }

void tim1Start3PhasePwm() {
    stpTIM1->BDTR = Init_TIM1_BDTR;
    stpTIM1->CR1 |= TIM1_CR1_CEN;
}

void tim1Set3PhasePwmCfg(uint16 width, uint16 duty) {
    stpTIM1->ARR = width;
    stpTIM1->CCR1 = duty;
    stpTIM1->CCR2 = duty;
    stpTIM1->CCR3 = duty;
}

static uint32 tim13PhasePwmCurrentPhase = 0; /* 0: u, 1: v, 2: w*/
void tim1Flip3PhasePwm() {
    uint32 valueRegCCR5 = stpTIM1->CCR5;

    /* clear GC5 */
    valueRegCCR5 &= TIM1_CCR5_GC5_MASK;

    if (tim13PhasePwmCurrentPhase == 0) {
        /* pahse u. mask v, w pwm */
        valueRegCCR5 |= (TIM1_CCR5_GC5C3_BIT | TIM1_CCR5_GC5C2_BIT);
    } else if (tim13PhasePwmCurrentPhase == 1) {
        /* pahse v. mask w, u pwm */
        valueRegCCR5 |= (TIM1_CCR5_GC5C3_BIT | TIM1_CCR5_GC5C1_BIT);
    } else {
        /* pahse w. mask u, v pwm */
        valueRegCCR5 |= (TIM1_CCR5_GC5C2_BIT | TIM1_CCR5_GC5C1_BIT);
    }

    /* set GC5  */
    stpTIM1->CCR5 = valueRegCCR5;

    /* increment pahse */
    tim13PhasePwmCurrentPhase++;
    if (tim13PhasePwmCurrentPhase > 2) {
        tim13PhasePwmCurrentPhase = 0;
    }
}

void timSet6StepMotorPhaseU(void) {
    uint32 valueRegCCR5 = stpTIM1->CCR5;

    /* clear GC5 */
    valueRegCCR5 &= TIM1_CCR5_GC5_MASK;

    /* phase u:  u: high, v: low, w: low */
    valueRegCCR5 |= (TIM1_CCR5_GC5C3_BIT | TIM1_CCR5_GC5C2_BIT);

    /* set GC5  */
    stpTIM1->CCR5 = valueRegCCR5;
}

void timSet6StepMotorPhaseV(void) {
    uint32 valueRegCCR5 = stpTIM1->CCR5;

    /* clear GC5 */
    valueRegCCR5 &= TIM1_CCR5_GC5_MASK;

    /* phase v:  u: low, v: high, w: low */
    valueRegCCR5 |= (TIM1_CCR5_GC5C3_BIT | TIM1_CCR5_GC5C1_BIT);

    /* set GC5  */
    stpTIM1->CCR5 = valueRegCCR5;
}

void timSet6StepMotorPhaseW(void) {
    uint32 valueRegCCR5 = stpTIM1->CCR5;

    /* clear GC5 */
    valueRegCCR5 &= TIM1_CCR5_GC5_MASK;

    /* phase w:  u: low, v: low, w: high */
    valueRegCCR5 |= (TIM1_CCR5_GC5C2_BIT | TIM1_CCR5_GC5C1_BIT);

    /* set GC5  */
    stpTIM1->CCR5 = valueRegCCR5;
}
