#include "exit.h"

/*  Cortex-M4 Register. */
#define EXIT_BASE_ADDRESS 0x40010400

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
} StEXIT;

/* common */
#define EXIT_EXIT3_SHIFT 3
#define EXIT_EXIT3_BIT (1 << EXIT_EXIT3_SHIFT)
#define EXIT_EXIT10_SHIFT 10
#define EXIT_EXIT10_BIT (1 << EXIT_EXIT10_SHIFT)
#define EXIT_EXIT15_SHIFT 15
#define EXIT_EXIT15_BIT (1 << EXIT_EXIT15_SHIFT)

/* IMR */
#define EXIT_IMR_InterruptMasked 0x00
#define EXIT_IMR_InterruptNotMasked 0x01

#define EXIT_IMR1_MR03_NotMasked \
    (EXIT_IMR_InterruptNotMasked << EXIT_EXIT3_SHIFT)
#define EXIT_IMR1_MR10_NotMasked \
    (EXIT_IMR_InterruptNotMasked << EXIT_EXIT10_SHIFT)
#define EXIT_IMR1_MR15_NotMasked \
    (EXIT_IMR_InterruptNotMasked << EXIT_EXIT15_SHIFT)
/* default reset value */
#define EXIT_IMR1_MR23_NotMasked (EXIT_IMR_InterruptNotMasked << 23)
#define EXIT_IMR1_MR24_NotMasked (EXIT_IMR_InterruptNotMasked << 24)
#define EXIT_IMR1_MR25_NotMasked (EXIT_IMR_InterruptNotMasked << 25)
#define EXIT_IMR1_MR26_NotMasked (EXIT_IMR_InterruptNotMasked << 26)
#define EXIT_IMR1_MR27_NotMasked (EXIT_IMR_InterruptNotMasked << 27)
#define EXIT_IMR1_MR28_NotMasked (EXIT_IMR_InterruptNotMasked << 28)

#define Init_EXIT_IMR1                                     \
    (EXIT_IMR1_MR28_NotMasked | EXIT_IMR1_MR27_NotMasked | \
     EXIT_IMR1_MR27_NotMasked | EXIT_IMR1_MR26_NotMasked | \
     EXIT_IMR1_MR25_NotMasked | EXIT_IMR1_MR24_NotMasked | \
     EXIT_IMR1_MR23_NotMasked | EXIT_IMR1_MR15_NotMasked | \
     EXIT_IMR1_MR10_NotMasked | EXIT_IMR1_MR03_NotMasked)

/* RTSR */
#define EXIT_RTSR_RisingTrigerDisable 0x00
#define EXIT_RTSR_RisingTrigerEnable 0x01

#define EXIT_RTSR_TR03_Enable (EXIT_RTSR_RisingTrigerEnable << EXIT_EXIT3_SHIFT)
#define EXIT_RTSR_TR10_Enable \
    (EXIT_RTSR_RisingTrigerEnable << EXIT_EXIT10_SHIFT)
#define EXIT_RTSR_TR15_Enable \
    (EXIT_RTSR_RisingTrigerEnable << EXIT_EXIT15_SHIFT)

#define Init_EXIT_RTSR1 \
    (EXIT_RTSR_TR03_Enable | EXIT_RTSR_TR10_Enable | EXIT_RTSR_TR15_Enable)

/* FTSR */
#define EXIT_FTSR_FallingTrigerDisable 0x00
#define EXIT_FTSR_FallingTrigerEnable 0x01

#define EXIT_FTSR_TR03_Enable \
    (EXIT_FTSR_FallingTrigerEnable << EXIT_EXIT3_SHIFT)
#define EXIT_FTSR_TR10_Enable \
    (EXIT_FTSR_FallingTrigerEnable << EXIT_EXIT10_SHIFT)
#define EXIT_FTSR_TR15_Enable \
    (EXIT_FTSR_FallingTrigerEnable << EXIT_EXIT15_SHIFT)

#define Init_EXIT_FTSR1 \
    (EXIT_FTSR_TR03_Enable | EXIT_FTSR_TR10_Enable | EXIT_FTSR_TR15_Enable)

/* pointer to register */
#define stpEXIT ((StEXIT*)(EXIT_BASE_ADDRESS))

void Exit_Init() {
    stpEXIT->IMR1 = Init_EXIT_IMR1;
    stpEXIT->RTSR1 = Init_EXIT_RTSR1;
    stpEXIT->FTSR1 = Init_EXIT_FTSR1;
}

void Exit_ClearExit3() { stpEXIT->PR1 |= EXIT_EXIT3_BIT; }
uint8 Exit_ClearExit15_10() {
    uint8 exitNum;
    if ((stpEXIT->PR1 & (EXIT_EXIT10_BIT)) > 0) {
        stpEXIT->PR1 |= EXIT_EXIT10_BIT;
        exitNum = 1;
    } else if ((stpEXIT->PR1 & (EXIT_EXIT15_BIT)) > 0) {
        stpEXIT->PR1 |= EXIT_EXIT15_BIT;
        exitNum = 2;
    } else {
        /* error */
        exitNum = 0xFF;
    }
    return exitNum;
}
