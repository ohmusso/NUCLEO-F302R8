#ifndef DRIVER_UART_H
#define DRIVER_UART_H

#include "Platform_Types.h"
#include "usart_df.h"

extern void Uart_Init();
extern void Usart2_Transmit(uint8 value);
extern void Usart2_TransmitBytes(const char_t* const str);
extern UartRetType Uart2_ReadData(RxDataType* data);

/* use in interrupt context */
extern void Usart2_RxIndication();

#endif /* DRIVER_UART_H*/
