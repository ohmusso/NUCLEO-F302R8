#ifndef DRIVER_UART_H
#define DRIVER_UART_H

#include "Platform_Types.h"
#include "usart_df.h"

extern void Uart_Init();
extern void Usart2_Transmit(uint8 value);
extern void Usart2_TransmitBytes(const uint8* const str);
extern void Usart2_TransmitHexDatas(const char_t* const str, const char_t len);
extern UartRetType Uart2_ReadData(RxDataType* data);

extern void Usart3_ComEsp32Config(void);
extern void Usart3_ComEsp32TransmitBytes(const uint8* const str);
extern uint8 Usart3_ComEsp32Read(void);
/* use in interrupt context */
extern void Usart2_RxIndication();

#endif /* DRIVER_UART_H*/
