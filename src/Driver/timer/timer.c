#include "timer.h"

#define TIM1_BASE_ADDRESS 0x40012C00 /* TIM1 */

typedef struct {
    uint32 CR1;
    uint32 notUsedCR2;
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
    uint32 notUsedCCR4;
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

/* DIER */
#define TIM1_DIER_CCXIE_Enable (0x01)
#define TIM1_DIER_CC1IE (TIM1_DIER_CCXIE_Enable << 1)

#define Init_TIM1_DIER (TIM1_DIER_CC1IE)

/* SR */
#define TIM1_SR_CCXIF_InterruptNotOcuure (0x00)
#define TIM1_SR_CCXIF_InterruptOcuure (0x01)
#define TIM1_SR_UIF_BIT (1 << 0)
#define TIM1_SR_UIF_SHIFT (0)
#define TIM1_SR_CC1IF_BIT (1 << 1)
#define TIM1_SR_CC1IF_SHIFT (1)

/* EGR */
#define TIM1_EGR_CCXG_GenarateEvent (0x01)
#define TIM1_EGR_CCX1 (TIM1_EGR_CCXG_GenarateEvent << 1)
#define Init_TIM1_EGR (TIM1_EGR_CCX1)

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
#define Init_TIM1_CCR1 \
    (Init_TIM1_ARR / 2) /* duty =  CCRX / width = 500us / 1000us = 0.5  */
#define Init_TIM1_CCR2 (Init_TIM1_CCR1)
#define Init_TIM1_CCR3 (Init_TIM1_CCR1)

/* BDTR */
#define TIM1_BDTR_MOE_Enable (0x01)
#define TIM1_BDTR_MOE (TIM1_BDTR_MOE_Enable << 15)

#define Init_TIM1_BDTR (TIM1_BDTR_MOE)

/* pointer to TIM1 register */
#define stpTIM1 ((StTIM1*)(TIM1_BASE_ADDRESS))

/* macro */
/* SR */
#define mIsTim1UIFSet() \
    ((((stpTIM1->SR && (~TIM1_SR_UIF_BIT)) >> TIM1_SR_UIF_SHIFT)) != 0)
#define mClearTim1UIF() (stpTIM1->SR &= (~TIM1_SR_UIF_BIT))

#define mIsTim1CC1IFSet() \
    ((((stpTIM1->SR && (~TIM1_SR_CC1IF_BIT)) >> TIM1_SR_CC1IF_SHIFT)) != 0)
#define mClearTim1CC1IF() (stpTIM1->SR &= (~TIM1_SR_CC1IF_BIT))

/* Clock must initialized before Port initialize */
void Timer_Init() {
    stpTIM1->CR1 = Init_TIM1_CR1;
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
    stpTIM1->BDTR = Init_TIM1_BDTR;
}

uint16 tim1GetCnt() { return stpTIM1->CNT; }

uint8 tim1IsCC1IFSet() { return mIsTim1CC1IFSet(); }
uint8 tim1ClearCC1IF() { return mClearTim1CC1IF(); }

uint8 tim1IsUIFSet() { return mIsTim1UIFSet(); }
uint8 tim1ClearUIF() { return mClearTim1UIF(); }

void tim1Start() { stpTIM1->CR1 |= TIM1_CR1_CEN; }
