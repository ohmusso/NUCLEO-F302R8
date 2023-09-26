#ifndef DRIVER_UART_H
#define DRIVER_UART_H

#include "Platform_Types.h"

extern void Uart_Init();
extern void Usart2_Transmit(
    uint8 value
);
#endif /* DRIVER_UART_H*/
