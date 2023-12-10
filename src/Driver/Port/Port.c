#include "port.h"

#define GPIOA_BASE_ADDRESS 0x48000000 /* GPIOA */
#define GPIOB_BASE_ADDRESS 0x48000400 /* GPIOB */
#define GPIOC_BASE_ADDRESS 0x48000800 /* GPIOC */

typedef struct {
    uint32 MODER;
    uint32 notUsedOSPEEDR;
    uint32 OSPEEDR;
    uint32 notUsedPUPDR;
    uint32 IDR;
    uint32 ODR;
    uint32 notUsedBSRR;
    uint32 notUsedLCKR;
    uint32 AFRL;
    uint32 AFRH;
} StGPIOX;

/* MODER */
#define GPIOX_MODER_Type_Input 0x00
#define GPIOX_MODER_Type_GenOutput 0x01
#define GPIOX_MODER_Type_AltFunc 0x02
#define GPIOX_MODER_Type_Analog 0x03

/* GPIOA */
#define Init_GPIOA_MODER_02 (GPIOX_MODER_Type_AltFunc << 4) /* UART2 Tx */
#define Init_GPIOA_MODER_03 (GPIOX_MODER_Type_AltFunc << 6) /* UART2 Rx */
#define Init_GPIOA_MODER_07 (GPIOX_MODER_Type_Analog << 14) /* BEMF3 */
#define Init_GPIOA_MODER_08 \
    (GPIOX_MODER_Type_AltFunc << 16) /* Motor IN1(TIM1 CC1) */
#define Init_GPIOA_MODER_09 \
    (GPIOX_MODER_Type_AltFunc << 18) /* Motor IN2(TIM1 CC2) */
#define Init_GPIOA_MODER_10 \
    (GPIOX_MODER_Type_AltFunc << 20) /* Motor IN3(TIM1 CC3) */
#define Init_GPIOA_MODER_13 (GPIOX_MODER_Type_AltFunc << 26) /* DBG: SWDIO */
#define Init_GPIOA_MODER_14 (GPIOX_MODER_Type_AltFunc << 28) /* DBG: SWCLK */
#define Init_GPIOA_MODER_15 (GPIOX_MODER_Type_Input << 30)   /* H1 */
#define Init_GPIOA_MODER                                               \
    (Init_GPIOA_MODER_15 | Init_GPIOA_MODER_14 | Init_GPIOA_MODER_13 | \
     Init_GPIOA_MODER_10 | Init_GPIOA_MODER_09 | Init_GPIOA_MODER_08 | \
     Init_GPIOA_MODER_07 | Init_GPIOA_MODER_03 | Init_GPIOA_MODER_02)

/* GPIOB */
#define Init_GPIOB_MODER_00 (GPIOX_MODER_Type_Analog << 0)     /* BEMF2 */
#define Init_GPIOB_MODER_03 (GPIOX_MODER_Type_Input << 6)      /* H2 */
#define Init_GPIOB_MODER_13 (GPIOX_MODER_Type_GenOutput << 26) /* LED */
#define Init_GPIOB_MODER_10 (GPIOX_MODER_Type_Input << 20)     /* H3 */
#define Init_GPIOB_MODER (Init_GPIOB_MODER_13 | Init_GPIOB_MODER_00)

/* GPIOC */
#define Init_GPIOC_MODER_02 (GPIOX_MODER_Type_Analog << 4)     /* Volume */
#define Init_GPIOC_MODER_03 (GPIOX_MODER_Type_Analog << 6)     /* BEMF1 */
#define Init_GPIOC_MODER_10 (GPIOX_MODER_Type_GenOutput << 20) /* Motor EN1 */
#define Init_GPIOC_MODER_11 (GPIOX_MODER_Type_GenOutput << 22) /* Motor EN2 */
#define Init_GPIOC_MODER_12 (GPIOX_MODER_Type_GenOutput << 24) /* Motor EN3 */
#define Init_GPIOC_MODER                                               \
    (Init_GPIOC_MODER_12 | Init_GPIOC_MODER_11 | Init_GPIOC_MODER_10 | \
     Init_GPIOC_MODER_03 | Init_GPIOC_MODER_02)

/* OSPEEDR */
#define GPIOX_OSPEEDR_Type_Low 0x00
#define GPIOX_OSPEEDR_Type_Mid 0x01
#define GPIOX_OSPEEDR_Type_High 0x03

#define Init_GPIOB_OSPEEDR_13 (GPIOX_OSPEEDR_Type_Mid << 26)
#define Init_GPIOB_OSPEEDR (Init_GPIOB_OSPEEDR_13)

/* IDR */
#define GPIOX_IDR_SHIFT_03 (3)
#define GPIOX_IDR_BIT03 (1 << GPIOX_IDR_SHIFT_03)
#define GPIOX_IDR_SHIFT_10 (10)
#define GPIOX_IDR_BIT10 (1 << GPIOX_IDR_SHIFT_10)
#define GPIOX_IDR_SHIFT_15 (15)
#define GPIOX_IDR_BIT15 (1 << GPIOX_IDR_SHIFT_15)

/* ODR */
#define GPIOB_ODR_SHIFT_13 ((uint8)13)
#define GPIOB_ODR_MASK_13 (1 << GPIOB_ODR_SHIFT_13)

#define GPIOX_ODR_BIT10 (1 << 10)
#define GPIOX_ODR_BIT11 (1 << 11)
#define GPIOX_ODR_BIT12 (1 << 12)

/* AFRL */
#define GPIOA_AFRL_USERT2_TX (0x07 << 8)
#define GPIOA_AFRL_USERT2_RX (0x07 << 12)
#define Init_GPIOA_AFRL (GPIOA_AFRL_USERT2_RX | GPIOA_AFRL_USERT2_TX)

/* AFRH */
#define GPIOA_AFRH_TIM1_CH1 (0x06 << 0)
#define GPIOA_AFRH_TIM1_CH2 (0x06 << 4)
#define GPIOA_AFRH_TIM1_CH3 (0x06 << 8)
#define GPIOA_AFRH_SWDIO (0x00 << 20)
#define GPIOA_AFRH_SWCLK (0x00 << 24)
#define Init_GPIOA_AFRH                                          \
    (GPIOA_AFRH_SWCLK | GPIOA_AFRH_SWDIO | GPIOA_AFRH_TIM1_CH3 | \
     GPIOA_AFRH_TIM1_CH2 | GPIOA_AFRH_TIM1_CH1)

/* pointer to GPIOX register */
#define stpGPIOA ((StGPIOX *)(GPIOA_BASE_ADDRESS))
#define stpGPIOB ((StGPIOX *)(GPIOB_BASE_ADDRESS))
#define stpGPIOC ((StGPIOX *)(GPIOC_BASE_ADDRESS))

/* macro */
#define mPortRead(idr, pinBit, pinShift) \
    ((PortOnOff)(((idr) & (pinBit)) >> (pinShift)))

/* Clock must initialized before Port initialize */
void Port_Init() {
    /* GPIOA */
    stpGPIOA->MODER = Init_GPIOA_MODER;
    stpGPIOA->AFRL = Init_GPIOA_AFRL;
    stpGPIOA->AFRH = Init_GPIOA_AFRH;

    /* GPIOB */
    stpGPIOB->MODER = Init_GPIOB_MODER;
    stpGPIOB->OSPEEDR = Init_GPIOB_OSPEEDR;

    /* GPIOC */
    stpGPIOC->MODER = Init_GPIOC_MODER;
}

PortOnOff Port_ReadH1() {
    return mPortRead(stpGPIOA->IDR, GPIOX_IDR_BIT15, GPIOX_IDR_SHIFT_15);
}

PortOnOff Port_ReadH2() {
    return mPortRead(stpGPIOB->IDR, GPIOX_IDR_BIT03, GPIOX_IDR_SHIFT_03);
}

PortOnOff Port_ReadH3() {
    return mPortRead(stpGPIOB->IDR, GPIOX_IDR_BIT10, GPIOX_IDR_SHIFT_10);
}

void Port_Write(PortOnOff value) {
    stpGPIOB->ODR = (uint32)((value & 0x01) << GPIOB_ODR_SHIFT_13);
}

void Port_Flip() {
    stpGPIOB->ODR = (uint32)((stpGPIOB->ODR) ^ GPIOB_ODR_MASK_13);
}

/* Motor Driver Enable */
#define mMotorDriverEnableU() stpGPIOC->ODR |= GPIOX_ODR_BIT10
#define mMotorDriverEnableV() stpGPIOC->ODR |= GPIOX_ODR_BIT11
#define mMotorDriverEnableW() stpGPIOC->ODR |= GPIOX_ODR_BIT12
/* Motor Driver Disable */
#define mMotorDriverDisableU() stpGPIOC->ODR &= (~GPIOX_ODR_BIT10)
#define mMotorDriverDisableV() stpGPIOC->ODR &= (~GPIOX_ODR_BIT11)
#define mMotorDriverDisableW() stpGPIOC->ODR &= (~GPIOX_ODR_BIT12)

void Port_SetMotorDriverEnable(void) {
    mMotorDriverEnableU();
    mMotorDriverEnableV();
    mMotorDriverEnableW();
}

void Port_SetMotorDriverDisable(void) {
    mMotorDriverDisableU();
    mMotorDriverDisableV();
    mMotorDriverDisableW();
}

void Port_SetMotorDriverEnUV(void) {
    mMotorDriverEnableU();
    mMotorDriverEnableV();
    mMotorDriverDisableW();
}

void Port_SetMotorDriverEnVW(void) {
    mMotorDriverDisableU();
    mMotorDriverEnableV();
    mMotorDriverEnableW();
}

void Port_SetMotorDriverEnWU(void) {
    mMotorDriverEnableU();
    mMotorDriverDisableV();
    mMotorDriverEnableW();
}
