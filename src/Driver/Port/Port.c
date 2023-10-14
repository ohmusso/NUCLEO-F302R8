#include "port.h"

#define GPIOA_BASE_ADDRESS 0x48000000 /* GPIOA */
#define GPIOB_BASE_ADDRESS 0x48000400 /* GPIOB */
#define GPIOC_BASE_ADDRESS 0x48000800 /* GPIOC */

typedef struct {
    uint32 MODER;
    uint32 notUsedOSPEEDR;
    uint32 OSPEEDR;
    uint32 notUsedPUPDR;
    uint32 notUsedIDR;
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

#define Init_GPIOA_MODER_02 (GPIOX_MODER_Type_AltFunc << 4)
#define Init_GPIOA_MODER_03 (GPIOX_MODER_Type_AltFunc << 6)
#define Init_GPIOA_MODER_08 (GPIOX_MODER_Type_AltFunc << 16)
#define Init_GPIOA_MODER_09 (GPIOX_MODER_Type_AltFunc << 18)
#define Init_GPIOA_MODER_10 (GPIOX_MODER_Type_AltFunc << 20)
#define Init_GPIOA_MODER                                               \
    (Init_GPIOA_MODER_10 | Init_GPIOA_MODER_09 | Init_GPIOA_MODER_08 | \
     Init_GPIOA_MODER_03 | Init_GPIOA_MODER_02)

#define Init_GPIOB_MODER_13 (GPIOX_MODER_Type_GenOutput << 26)
#define Init_GPIOB_MODER (Init_GPIOB_MODER_13)

/* OSPEEDR */
#define GPIOX_OSPEEDR_Type_Low 0x00
#define GPIOX_OSPEEDR_Type_Mid 0x01
#define GPIOX_OSPEEDR_Type_High 0x03

#define Init_GPIOB_OSPEEDR_13 (GPIOX_OSPEEDR_Type_Mid << 26)
#define Init_GPIOB_OSPEEDR (Init_GPIOB_OSPEEDR_13)

/* ODR */
#define GPIOB_ODR_SHIFT_13 ((uint8)13)
#define GPIOB_ODR_MASK_13 (1 << GPIOB_ODR_SHIFT_13)

/* AFRL */
#define GPIOA_AFRL_USERT2_TX (0x07 << 8)
#define GPIOA_AFRL_USERT2_RX (0x07 << 12)
#define Init_GPIOA_AFRL (GPIOA_AFRL_USERT2_RX | GPIOA_AFRL_USERT2_TX)

/* AFRH */
#define GPIOA_AFRH_TIM1_CH1 (0x06 << 0)
#define GPIOA_AFRH_TIM1_CH2 (0x06 << 4)
#define GPIOA_AFRH_TIM1_CH3 (0x06 << 8)
#define Init_GPIOA_AFRH \
    (GPIOA_AFRH_TIM1_CH3 | GPIOA_AFRH_TIM1_CH2 | GPIOA_AFRH_TIM1_CH1)

/* pointer to GPIOX register */
#define stpGPIOA ((StGPIOX *)(GPIOA_BASE_ADDRESS))
#define stpGPIOB ((StGPIOX *)(GPIOB_BASE_ADDRESS))
#define stpGPIOC ((StGPIOX *)(GPIOC_BASE_ADDRESS))

/* Clock must initialized before Port initialize */
void Port_Init() {
    stpGPIOA->MODER = Init_GPIOA_MODER;
    stpGPIOA->AFRL = Init_GPIOA_AFRL;
    stpGPIOA->AFRH = Init_GPIOA_AFRH;

    stpGPIOB->MODER = Init_GPIOB_MODER;
    stpGPIOB->OSPEEDR = Init_GPIOB_OSPEEDR;
}

void Port_Write(PortOnOff value) {
    stpGPIOB->ODR = (uint32)((value & 0x01) << GPIOB_ODR_SHIFT_13);
}

void Port_Flip() {
    stpGPIOB->ODR = (uint32)((stpGPIOB->ODR) ^ GPIOB_ODR_MASK_13);
}
