#include "exti.h"

/*  Cortex-M4 Register. */
#define EXTI_BASE_ADDRESS 0x40010400

/* ISER Register Map*/
typedef struct {
    uint32 IMR1;
    uint32 notUsedEMR1;
    uint32 RTSR1;
    uint32 FTSR1;
    uint32 notUsedSWIER1;
    uint32 PR1;
    uint32 reserved[2];
    uint32 notUsedIMR2;
    uint32 notUsedEMR2;
    uint32 notUsedRTSR2;
    uint32 notUsedFTSR2;
    uint32 notUsedSWIER2;
    uint32 notUsedPR2;
} StEXTI;

/* common */
#define EXTI_EXTI3_SHIFT 3
#define EXTI_EXTI3_BIT (1 << EXTI_EXTI3_SHIFT)
#define EXTI_EXTI10_SHIFT 10
#define EXTI_EXTI10_BIT (1 << EXTI_EXTI10_SHIFT)
#define EXTI_EXTI15_SHIFT 15
#define EXTI_EXTI15_BIT (1 << EXTI_EXTI15_SHIFT)

/* IMR */
#define EXTI_IMR_InterruptMasked 0x00
#define EXTI_IMR_InterruptNotMasked 0x01

#define EXTI_IMR1_MR03_NotMasked \
    (EXTI_IMR_InterruptNotMasked << EXTI_EXTI3_SHIFT)
#define EXTI_IMR1_MR10_NotMasked \
    (EXTI_IMR_InterruptNotMasked << EXTI_EXTI10_SHIFT)
#define EXTI_IMR1_MR15_NotMasked \
    (EXTI_IMR_InterruptNotMasked << EXTI_EXTI15_SHIFT)
/* default reset value */
#define EXTI_IMR1_MR23_NotMasked (EXTI_IMR_InterruptNotMasked << 23)
#define EXTI_IMR1_MR24_NotMasked (EXTI_IMR_InterruptNotMasked << 24)
#define EXTI_IMR1_MR25_NotMasked (EXTI_IMR_InterruptNotMasked << 25)
#define EXTI_IMR1_MR26_NotMasked (EXTI_IMR_InterruptNotMasked << 26)
#define EXTI_IMR1_MR27_NotMasked (EXTI_IMR_InterruptNotMasked << 27)
#define EXTI_IMR1_MR28_NotMasked (EXTI_IMR_InterruptNotMasked << 28)

#define Init_EXTI_IMR1                                     \
    (EXTI_IMR1_MR28_NotMasked | EXTI_IMR1_MR27_NotMasked | \
     EXTI_IMR1_MR27_NotMasked | EXTI_IMR1_MR26_NotMasked | \
     EXTI_IMR1_MR25_NotMasked | EXTI_IMR1_MR24_NotMasked | \
     EXTI_IMR1_MR23_NotMasked | EXTI_IMR1_MR15_NotMasked | \
     EXTI_IMR1_MR10_NotMasked | EXTI_IMR1_MR03_NotMasked)

/* RTSR */
#define EXTI_RTSR_RisingTrigerDisable 0x00
#define EXTI_RTSR_RisingTrigerEnable 0x01

#define EXTI_RTSR_TR03_Enable (EXTI_RTSR_RisingTrigerEnable << EXTI_EXTI3_SHIFT)
#define EXTI_RTSR_TR10_Enable \
    (EXTI_RTSR_RisingTrigerEnable << EXTI_EXTI10_SHIFT)
#define EXTI_RTSR_TR15_Enable \
    (EXTI_RTSR_RisingTrigerEnable << EXTI_EXTI15_SHIFT)

#define Init_EXTI_RTSR1 \
    (EXTI_RTSR_TR03_Enable | EXTI_RTSR_TR10_Enable | EXTI_RTSR_TR15_Enable)

/* FTSR */
#define EXTI_FTSR_FallingTrigerDisable 0x00
#define EXTI_FTSR_FallingTrigerEnable 0x01

#define EXTI_FTSR_TR03_Enable \
    (EXTI_FTSR_FallingTrigerEnable << EXTI_EXTI3_SHIFT)
#define EXTI_FTSR_TR10_Enable \
    (EXTI_FTSR_FallingTrigerEnable << EXTI_EXTI10_SHIFT)
#define EXTI_FTSR_TR15_Enable \
    (EXTI_FTSR_FallingTrigerEnable << EXTI_EXTI15_SHIFT)

#define Init_EXTI_FTSR1 \
    (EXTI_FTSR_TR03_Enable | EXTI_FTSR_TR10_Enable | EXTI_FTSR_TR15_Enable)

/* pointer to register */
#define stpEXTI ((StEXTI*)(EXTI_BASE_ADDRESS))

void Exti_Init() {
    stpEXTI->IMR1 = Init_EXTI_IMR1;
    stpEXTI->RTSR1 = Init_EXTI_RTSR1;
    stpEXTI->FTSR1 = Init_EXTI_FTSR1;
}

void Exti_ClearExti3() { stpEXTI->PR1 |= EXTI_EXTI3_BIT; }
uint8 Exti_ClearExti15_10() {
    uint8 EXTINum;
    if ((stpEXTI->PR1 & (EXTI_EXTI10_BIT)) > 0) {
        stpEXTI->PR1 |= EXTI_EXTI10_BIT;
        EXTINum = 1;
    } else if ((stpEXTI->PR1 & (EXTI_EXTI15_BIT)) > 0) {
        stpEXTI->PR1 |= EXTI_EXTI15_BIT;
        EXTINum = 2;
    } else {
        /* error */
        EXTINum = 0xFF;
    }
    return EXTINum;
}
