#include "port.h"

#define GPIOB_BASE_ADDRESS     0x48000400                  /* GPIOB */

typedef struct
{
    uint32 MODER;
    uint32 notUsedOSPEEDR;
    uint32 OSPEEDR;
    uint32 notUsedPUPDR;
    uint32 notUsedIDR;
    uint32 ODR;
    uint32 notUsedBSRR;
    uint32 notUsedLCKR;
    uint32 notUsedAFRL;
    uint32 notUsedAFRH;
} StGPIOB;

/* MODER */
#define GPIOB_MODER_Type_Input      0x00
#define GPIOB_MODER_Type_GenOutput  0x01
#define GPIOB_MODER_Type_AltFunc    0x02
#define GPIOB_MODER_Type_Analog     0x03

#define Init_GPIOB_MODER_13        (GPIOB_MODER_Type_GenOutput << 26)
#define Init_GPIOB_MODER           (Init_GPIOB_MODER_13)

/* OSPEEDR */
#define GPIOB_OSPEEDR_Type_Low      0x00
#define GPIOB_OSPEEDR_Type_Mid      0x01
#define GPIOB_OSPEEDR_Type_High     0x03

#define Init_GPIOB_OSPEEDR_13      (GPIOB_OSPEEDR_Type_Mid << 26)
#define Init_GPIOB_OSPEEDR         (Init_GPIOB_OSPEEDR_13)

/* ODR */
#define GPIOB_ODR_SHIFT_13      ((uint8)13)

// StGPIOB* stpGPIOB = (StGPIOB*)(GPIOB_BASE_ADDRESS);
#define stpGPIOB ((StGPIOB*)(GPIOB_BASE_ADDRESS))

/* Clock must initialized before Port initialize */
void Port_Init()
{
    stpGPIOB->MODER = Init_GPIOB_MODER;
    stpGPIOB->OSPEEDR = Init_GPIOB_OSPEEDR;
}

void Port_Write(
    PortOnOff value
)
{
    stpGPIOB->ODR = (uint32)((value & 0x01) << GPIOB_ODR_SHIFT_13);
}

void Port_WriteOn()
{
    stpGPIOB->ODR = (0x01 << GPIOB_ODR_SHIFT_13);
}
