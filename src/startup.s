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
        movs r1, #4             /* r1 = 4 */
        b LoopCopyDataInit      /* jump to LoopCopyDataInit */

    CopyDataInit:
        ldr r3, =_sidata         /* r3 = *(_sidata) */
        ldr r3, [r3, r1]        /* r3 = *(r3 + r1) */
        str r3, [r0, r1]        /* *(r0 + r1) = r3 */
        adds r1, r1, #4         /* r1 += 4 */

    LoopCopyDataInit:
        ldr r0, =_sdata         /* r0 = *(_sdata) */
        ldr r3, =_edata        /* r3 = *(_edata) */
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
	.word	0 /* SVC_Handler */
	.word	0 /* DebugMon_Handler */
	.word	0
	.word	0 /* PendSV_Handler */
	.word	SysTick_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
