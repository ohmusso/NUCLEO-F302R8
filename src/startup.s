.syntax unified
.cpu cortex-m4
.thumb

.global	g_pfnVectors

/* start address for the initialization values of the .data section. defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/* Reset_Handler */
.section .text.Reset_Handler
.weak	Reset_Handler
.type	Reset_Handler, %function
    Reset_Handler:
        movs r1, #0             /* r1 = 0 */
        b LoopCopyDataInit      /* jump to LoopCopyDataInit */

    CopyDataInit:
        ldr r3, =_sidata        /* r3 = *(_sidata) */
        ldr r3, [r3, r1]        /* r3 = *(r3 + r1) */
        str r3, [r0, r1]        /* *(r0 + r1) = r3 */
        adds r1, r1, #4         /* r1 += 4 */

    LoopCopyDataInit:
        ldr r0, =_sdata         /* r0 = *(_sdata) */
        ldr r3, =_edata         /* r3 = *(_edata) */
        adds r2, r0, r1         /* r2 = r0 + r1 */
        cmp r2, r3              /* if( r2 < r3 ) */
        bcc CopyDataInit            /* true then jump to CopyDataInit */
        ldr r2, =_sbss          /* r2 = *(_sbss) */
        b LoopFillZerobss       /* jump to LoopFillZerobss */
    
    FillZerobss:
        movs r3, #0             /* r3 = 0 */
        str r3, [r2]            /* *(r2) = r3 */
        adds r2, r2, #4         /* r2 += 4 */

    LoopFillZerobss:
        ldr r3, =_ebss         /* r3 = _ebss */
        cmp r2, r3              /* if( r2 < r3) */
        bcc FillZerobss             /* true then jump to FillZerobss */

    bl main                     /* call main */

.size Reset_Handler, .-Reset_Handler  /* current location counter - Resdet_Handler */

/* Vector Table */
.section .isr_vector, "a", %progbits
.type	g_pfnVectors, %object
.size	g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
	.word	_estack
	.word	Reset_Handler
	.word	0 // NMI_Handler
	.word	0 // HardFault_Handler
	.word	0 // MemManage_Handler
	.word	0 // BusFault_Handler
	.word	0 // UsageFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler /* Used by FreeRTOS */
	.word	0 /* DebugMon_Handler */
	.word	0
	.word	PendSV_Handler  /* 0038: Used by FreeRTOS */
	.word	SysTick_Handler /* 003C: Used by FreeRTOS */
	.word	0				/* 0040:  */
	.word	0				/* 0044:  */
	.word	0				/* 0048:  */
	.word	0				/* 004C:  */
	.word	0				/* 0050:  */
	.word	0				/* 0054:  */
	.word	0				/* 0058:  */
	.word	0				/* 005C:  */
	.word	0				/* 0060:  */
	.word	IRQ_EXTI3_Handler /* 0064: EXTI Line3 interrupt */
	.word	0				/* 0068:  */
	.word	0				/* 006C:  */
	.word	0				/* 0070:  */
	.word	0				/* 0074:  */
	.word	0				/* 0078:  */
	.word	0				/* 007C:  */
	.word	0				/* 0080:  */
	.word	0				/* 0084:  */
	.word	IRQ_ADC_Handler	/* 0088: ADC1 and ADC2 global interrupt */
	.word	0				/* 008C:  */
	.word	0				/* 0090:  */
	.word	0				/* 0094:  */
	.word	0				/* 0098:  */
	.word	0				/* 009C:  */
	.word	0				/* 00A0:  */
	.word	IRQ_TIM1_UP_Handler	/* 00A4: TIM1 update interrupt */
	.word	0				/* 00A8:  */
	.word	IRQ_TIM1_CC_Handler	/* 00AC:TIM1 capture copare interrupt */
	.word	0				/* 00B0:  */
	.word	0				/* 00B4:  */
	.word	0				/* 00B8:  */
	.word	0				/* 00BC:  */
	.word	0				/* 00C0:  */
	.word	0				/* 00C4:  */
	.word	0				/* 00C8:  */
	.word	0				/* 00CC:  */
	.word	0				/* 00D0:  */
	.word	0				/* 00D4:  */
	.word	IRQ_UART2_Handler /* 00D8: UART2 interrupt */
	.word	IRQ_UART3_Handler				/* 00DC: UART3 interrupt */
	.word	IRQ_EXTI15_10_Handler /* 00E0: EXTI Line[15:10] interrupts */
	.word	0				/* 00E4:  */
	.word	0				/* 00E8:  */
	.word	0				/* 00EC:  */
	.word	0				/* 00F0:  */
	.word	0				/* 00F4:  */
	.word	0				/* 00F8:  */
	.word	0				/* 00FC:  */
	.word	0				/* 0100:  */
	.word	0				/* 0104:  */
	.word	0				/* 0108:  */
	.word	0				/* 010C:  */
	.word	0				/* 0110:  */
	.word	0				/* 0114:  */
	.word	0				/* 0118:  */
	.word	0				/* 011C:  */
	.word	0				/* 0120:  */
	.word	0				/* 0124:  */
	.word	0				/* 0128:  */
	.word	0				/* 012C:  */
	.word	0				/* 0130:  */
	.word	0				/* 0134:  */
	.word	0				/* 0138:  */
	.word	0				/* 013C:  */
	.word	0				/* 0140:  */
	.word	0				/* 0144:  */
	.word	0				/* 0148:  */
	.word	0				/* 014C:  */
	.word	0				/* 0150:  */
	.word	0				/* 0154:  */
	.word	0				/* 0158:  */
	.word	0				/* 015C:  */
	.word	0				/* 0160:  */
	.word	0				/* 0164:  */
	.word	0				/* 0168:  */
	.word	0				/* 016C:  */
	.word	0				/* 0170:  */
	.word	0				/* 0174:  */
	.word	0				/* 0178:  */
	.word	0				/* 017C:  */
	.word	0				/* 0180:  */
	.word	0				/* 0184:  */
