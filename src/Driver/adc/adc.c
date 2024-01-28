#include "adc.h"

/*  Cortex-M4 Register. */
#define ADC1_BASE_ADDRESS 0x50000000
#define ADCCOM_BASE_ADDRESS 0x50000300

/* ADC Register Map*/
typedef struct {
    uint32 ISR;
    uint32 IER;
    uint32 CR;
    uint32 CFGR;
    uint32 reserved_10;
    uint32 SMPR1;
    uint32 SMPR2;
    uint32 reserved_1C;
    uint32 TR1;
    uint32 TR2;
    uint32 TR3;
    uint32 reserved_2C;
    uint32 SQR1;
    uint32 SQR2;
    uint32 SQR3;
    uint32 SQR4;
    uint16 DR;
    uint16 reserved_DR;
    uint32 reserved_44[2];
    uint32 JSQR;
    uint32 reserved_50[4];
    uint32 OFR1;
    uint32 OFR2;
    uint32 OFR3;
    uint32 OFR4;
    uint32 reserved_70[4];
    uint32 JDR1;
    uint32 JDR2;
    uint32 JDR3;
    uint32 JDR4;
    uint32 reserved_90[4];
    uint32 AWD2CR;
    uint32 AWD3CR;
    uint32 reserved_A8[2];
    uint32 DIFSEL;
    uint32 CALFACT;
} StADC;

/* ADC Common Register Map*/
typedef struct {
    uint32 CSR;
    uint32 reserved_04;
    uint32 CCR;
    uint32 CDR;
} StADCCOM;

/* ADCx_ISR: ADC interrupt and status register */
#define ADC_ISR_ADRDY_clear (0u)
#define ADC_ISR_ADRDY_Ready (1u)
#define ADC_ISR_ADRDY_SHIFT (0u)
#define ADC_ISR_ADRDY_MASK (1u << ADC_ISR_ADRDY_SHIFT)

#define ADC_ISR_EOC_Set (1u)
#define ADC_ISR_EOC_SHIFT (2u)
#define ADC_ISR_EOC_MASK (1u << ADC_ISR_EOC_SHIFT)

/* ADCx_IER: ADC interrupt enable register */
#define ADC_IER_Enable (1u)
#define ADC_IER_EOC_SHIFT (2u)
#define ADC_IER_EOC_Enable (ADC_IER_Enable << ADC_IER_EOC_SHIFT)

/* ADCx_CR */
#define ADC_CR_ADVREGEN_DisEToEn (0u)
#define ADC_CR_ADVREGEN_Enabled (1u)
#define ADC_CR_ADVREGEN_Disabled (2u)
#define ADC_CR_ADVREGEN_SHIFT (28u)
#define ADC_CR_ADVREGEN_MASK (3u << ADC_CR_ADVREGEN_SHIFT)
#define ADC_CR_ADVREGEN_Set_Enabled \
    (ADC_CR_ADVREGEN_Enabled << ADC_CR_ADVREGEN_SHIFT)

#define ADC_CR_ADCAL_End (0u)
#define ADC_CR_ADCAL_Start (1u)
#define ADC_CR_ADCAL_SHIFT (31u)
#define ADC_CR_ADCAL_MASK (1u << ADC_CR_ADCAL_SHIFT)
#define ADC_CR_ADCAL_Set_Start (ADC_CR_ADCAL_Start << ADC_CR_ADCAL_SHIFT)

#define ADC_CR_ADCALDIF_Singleend (0u)
#define ADC_CR_ADCALDIF_Differential (1u)
#define ADC_CR_ADCALDIF_SHIFT (30u)

#define ADC_CR_ADSTP_Set (1u)
#define ADC_CR_ADSTP_SHIFT (4u)
#define ADC_CR_ADSTP_MASK (1u << ADC_CR_ADSTP_SHIFT)

#define ADC_CR_ADSTART_Set (1u)
#define ADC_CR_ADSTART_SHIFT (2u)
#define ADC_CR_ADSTART_MASK (1u << ADC_CR_ADSTART_SHIFT)

#define ADC_CR_ADEN_Disabled (0u)
#define ADC_CR_ADEN_Enabled (1u)
#define ADC_CR_ADEN_SHIFT (0u)
#define ADC1_CR_ADEN_MASK (1u << ADC_CR_ADEN_SHIFT)
#define ADC_CR_ADEN_SET_Enabled (1u << ADC_CR_ADEN_SHIFT)

/* ADCx_CFGR */
#define ADC_CFGR_EXTEN_HwTrigerRising (1u)
#define ADC_CFGR_EXTEN_HwTrigerFalling (2u)
#define ADC_CFGR_EXTEN_SHIFT (10u)

#define ADC_CFGR_EXTEN_Set_HwTrigerRising \
    (ADC_CFGR_EXTEN_HwTrigerRising << ADC_CFGR_EXTEN_SHIFT)
#define ADC_CFGR_EXTEN_Set_HwTrigerFalling \
    (ADC_CFGR_EXTEN_HwTrigerFalling << ADC_CFGR_EXTEN_SHIFT)

#define ADC_CFGR_EXTSEL_TIM1_CC1_event (0u)
#define ADC_CFGR_EXTSEL_TIM1_TRGO_event (9u)
#define ADC_CFGR_EXTSEL_TIM1_TRGO2_event (10u)
#define ADC_CFGR_EXTSEL_SHIFT (6u)

#define ADC_CFGR_EXTSEL_Set_TIM1_CC1_Event \
    (ADC_CFGR_EXTSEL_TIM1_CC1_event << ADC_CFGR_EXTSEL_SHIFT)
#define ADC_CFGR_EXTSEL_Set_TIM1_TRGO_event \
    (ADC_CFGR_EXTSEL_TIM1_TRGO_event << ADC_CFGR_EXTSEL_SHIFT)
#define ADC_CFGR_EXTSEL_Set_TIM1_TRGO2_event \
    (ADC_CFGR_EXTSEL_TIM1_TRGO2_event << ADC_CFGR_EXTSEL_SHIFT)

#define ADC_CFGR_RES_8bit (2u)
#define ADC_CFGR_RES_SHIFT (3u)
#define ADC_CFGR_RES_Set_8bit (ADC_CFGR_RES_8bit << ADC_CFGR_RES_SHIFT)

#define ADC_CFGR_SET                     \
    (ADC_CFGR_EXTEN_Set_HwTrigerRising | \
     ADC_CFGR_EXTSEL_Set_TIM1_TRGO2_event | ADC_CFGR_RES_Set_8bit)

/* ADCx_SMPR */
#define ADC_SMPR_1_5Clock (0u)  /* 1.5 ADC clock cycles */
#define ADC_SMPR_19_5Clock (4u) /* 19.5 ADC clock cycles */

#define ADC_SMPR_IN8_SHIFT (24u)
#define ADC_SMPR_IN8 (ADC_SMPR_19_5Clock << ADC_SMPR_IN8_SHIFT)

/* ADCx_SQR1 */
#define Init_ADC_SQR_L (0u) /* one conversion */

#define ADC_SQR_SQ1_SHIFT (6u)
#define ADC_SQR_SQ2_SHIFT (12u)
#define ADC_SQR_SQ1_IN8 (8u << ADC_SQR_SQ1_SHIFT)
#define ADC_SQR_SQ1_IN9 (9u << ADC_SQR_SQ1_SHIFT)   /* BEMF1 */
#define ADC_SQR_SQ1_IN11 (11u << ADC_SQR_SQ1_SHIFT) /* BEMF2 */
#define ADC_SQR_SQ1_IN15 (15u << ADC_SQR_SQ1_SHIFT) /* BEMF3 */
#define ADC_SQR_SQ2_IN15 (15u << ADC_SQR_SQ2_SHIFT) /* BEMF3 */
#define ADC_SQR_SQ1_IN18 (18u << ADC_SQR_SQ1_SHIFT) /* VREFINT */

/* ADCx_DIFSEL */
#define ADC1_DIFSEL_Singleend (0u)
#define ADC1_DIFSEL_Differential (1u)

#define Init_ADC1_DIFSEL (0x00000000u)

/* ADCx_DR */
#define ADC1_DR_MASK_12bit (0x1FFFu)

/* ADCx_CCR */
#define ADC_CCR_CKMODE_CK_ADCx (0u)
#define ADC_CCR_CKMODE_HCLK_Div1 (1u)
#define ADC_CCR_CKMODE_HCLK_Div2 (2u)
#define ADC_CCR_CKMODE_HCLK_Div4 (3u)
#define ADC_CCR_CKMODE_SHIFT (16u)

#define ADC_CCR_VREFEN_Enable (1u)
#define ADC_CCR_VREFEN_SHIFT (22u)

/* pointer to register */
#define stpADC1 ((StADC*)(ADC1_BASE_ADDRESS))
#define stpADCComm ((StADCCOM*)(ADCCOM_BASE_ADDRESS))

/* macro */
/* ADSTART */
#define mStartConversion(regCR) \
    ((regCR) |= (ADC_CR_ADSTART_Set << ADC_CR_ADSTART_SHIFT))
#define mIsConverting(regCR)                                      \
    ((((regCR) & ADC_CR_ADSTART_MASK) >> ADC_CR_ADSTART_SHIFT) == \
     ADC_CR_ADSTART_Set)

/* ADSTP */
#define mStopConversion(regCR) \
    ((regCR) |= (ADC_CR_ADSTP_Set << ADC_CR_ADSTP_SHIFT))

/* EOC */
#define mClearEOC(regISR) ((regISR) &= ~ADC_ISR_EOC_MASK)
#define mIsEOCSet(regISR) \
    ((((regISR) & ADC_ISR_EOC_MASK) >> ADC_ISR_EOC_SHIFT) != 0)

/* ADRDY */
#define mClearAdcReady(regISR) ((regISR) &= ~ADC_ISR_ADRDY_MASK)
#define mIsAdcReady(regISR)                                      \
    ((((regISR) & ADC_ISR_ADRDY_MASK) >> ADC_ISR_ADRDY_SHIFT) == \
     ADC_ISR_ADRDY_Ready)

/* ADVREGEN */
#define mResetAdcAdvregen(regCR) (regCR) &= ~ADC_CR_ADVREGEN_MASK
#define mEnableAdcAdvregen(regCR) (regCR) |= ADC_CR_ADVREGEN_Set_Enabled
#define mIsAdcAdvregDisabled(regCR)                                 \
    ((((regCR) & ADC_CR_ADVREGEN_MASK) >> ADC_CR_ADVREGEN_SHIFT) == \
     ADC_CR_ADVREGEN_Disabled)
#define mIsAdcAdvregEnabled(regCR)                                  \
    ((((regCR) & ADC_CR_ADVREGEN_MASK) >> ADC_CR_ADVREGEN_SHIFT) == \
     ADC_CR_ADVREGEN_Enabled)

/* ADCALDIF */
#define mSetADCALDIF(regCR, regVal) \
    (regCR) |= ((regVal) << ADC_CR_ADCALDIF_SHIFT)

/* ADCAL */
#define mAdcStartCalibration(regCR) (regCR) |= ADC_CR_ADCAL_Set_Start
#define mIsAdcCalibrationEnd(regCR) (((regCR) & ADC_CR_ADCAL_MASK) == 0u)

/* ADEN */
#define mEnableAdc(regCR) ((regCR) |= 1u)
#define mDisableAdc(regCR) ((regCR) &= ~ADC1_CR_ADEN_MASK)
#define mIsAdcEnabled(regCR)                                 \
    (((regCR) & (ADC1_CR_ADEN_MASK) >> ADC_CR_ADEN_SHIFT) == \
     ADC_CR_ADEN_Enabled)

/* SMPR */
#define mSetSamplingRate(regSMPR, regInVal) ((regSMPR) = regInVal)

/* SQR */
#define mSetSequence(regSQR, regInVal) \
    ((regSQR) = ((regInVal) | Init_ADC_SQR_L))

/* VREFEN */
#define mEnableVREFEN() \
    stpADCComm->CCR |= (ADC_CCR_VREFEN_Enable << ADC_CCR_VREFEN_SHIFT)

/* CKMODE */
#define mSetCKMODE(regVal) stpADCComm->CCR |= ((regVal) << ADC_CCR_CKMODE_SHIFT)

/* prototype declaration */
static void vAdcStartCalibration(volatile StADC* pReg);
static void vAdcWait(uint32 time);
static void vAdcEnable(volatile StADC* pReg);
static uint16 vAdcStartConvertBit12(void);

extern void Port_Write(uint8 value);
/* function definition */
void ADC1_Init(void) {
    /* select adc clock  */
    mSetCKMODE(ADC_CCR_CKMODE_HCLK_Div2);
    // mEnableVREFEN();

    /* enable adc voltage regulator if regulator is not enable */
    if (mIsAdcAdvregDisabled(stpADC1->CR) == TRUE) {
        mResetAdcAdvregen(stpADC1->CR);
        vAdcWait(3000);
        mEnableAdcAdvregen(stpADC1->CR);
        /* wait for regulator start up time */
        /*   clock: HLCK /2 = 8 / 2 = 4[MHz] : 0.25[us] */
        /*   100tick = 250[us] */
        vAdcWait(3000);
    }

    /* calibration */
    vAdcStartCalibration(stpADC1);

    /* wait for after end of caliblation */
    vAdcWait(3000);

    /* enable adc */
    vAdcEnable(stpADC1);

    /* set adc hw trigger */
    stpADC1->CFGR = ADC_CFGR_SET;

    /* set adc intterupt enable */
    stpADC1->IER = ADC_IER_EOC_Enable;
}

static void vAdcWait(uint32 time) {
    while (time > 0) {
        time--;
    }
}

static void vAdcStartCalibration(volatile StADC* pRegADC) {
    /* check adc disabled */
    if (mIsAdcEnabled(pRegADC->CR) == TRUE) {
        return; /* adc aleady enable */
    }

    /* set calibration mode */
    mSetADCALDIF(pRegADC->CR, ADC_CR_ADCALDIF_Singleend);

    /* start calibration*/
    mAdcStartCalibration(pRegADC->CR);

    /* Wait for end of calibration */
    Port_Write(1);
    while (mIsAdcCalibrationEnd(pRegADC->CR) != TRUE) {
        /* busy loop */
    }
    Port_Write(0);
}

static void vAdcEnable(volatile StADC* pRegADC) {
    /* set ADEN */
    mEnableAdc(pRegADC->CR);

    /* Wait for ADRDY to set */
    while (mIsAdcReady(pRegADC->ISR) != TRUE) {
        /* busy loop */
    }

    /* clear ADRDY */
    mClearAdcReady(pRegADC->ISR);
}

uint16 vAdcConvertADC1IN9(void) {
    /* set sequeence */
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN9);

    /* start conversion and return result */
    return vAdcStartConvertBit12();
}

uint16 vAdcRead(void) {
    /* read result and return */
    return (stpADC1->DR & ADC1_DR_MASK_12bit);
}

static uint16 vAdcStartConvertBit12(void) {
    /* start conversion */
    mStartConversion(stpADC1->CR);

    /* wait for complete conversoin */
    while (mIsConverting(stpADC1->CR) == TRUE) {
        /* busy loop */
    }

    /* read result and return */
    return (stpADC1->DR & ADC1_DR_MASK_12bit);
}

void ADC1_StartConvBemf1(void) {
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN9);
    mStartConversion(stpADC1->CR);
}

void ADC1_StartConvBemf2(void) {
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN11);
    mStartConversion(stpADC1->CR);
}

void ADC1_StartConvBemf3(void) {
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN15);
    mStartConversion(stpADC1->CR);
}

void ADC1_StopConv(void) { mStopConversion(stpADC1->CR); }

void ADC1_SetSequenceBemf1(void) {
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN9);
}

void ADC1_SetSequenceBemf2(void) {
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN11);
}

void ADC1_SetSequenceBemf3(void) {
    mSetSequence(stpADC1->SQR1, ADC_SQR_SQ1_IN15);
}
