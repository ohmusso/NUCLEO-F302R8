ENTRY(Reset_Handler)

_estack = 0x20004000;    /* end of RAM */

MEMORY {
    SRAM (rwx)    : ORIGIN = 0x20000000, LENGTH = 16K
    FLASH (rx)    : ORIGIN = 0x08000000, LENGTH = 64K
}

SECTIONS {
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    }> FLASH

    .text :{
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.glue_7)                  /* glue arm to thumb code */
        *(.glue_7t)                 /* glue thumb to arm code */
        *(.rodata)                  /* constants data */
        *(.rodata.*)                /* constants data */
        . = ALIGN(4);
        _etext = .;
    }> FLASH

    _sidata = LOADADDR(.data);      /* start address of ram which has initial value */

    .data : {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    }> SRAM AT> FLASH

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
        . = ALIGN(4);
        end = .;
        __end__ = .;
    }> SRAM
}
