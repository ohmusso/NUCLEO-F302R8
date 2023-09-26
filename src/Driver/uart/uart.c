#include "uart.h"

#define USART2_BASE_ADDRESS 0x40004400  /* USART2 */

typedef struct
{
    uint32 CR1;
    uint32 CR2;
    uint32 notUsedCR3;
    uint16  notUsedBRRH;
    uint16  BRR;
    uint32  notUseGTPR;
    uint32  notUsedRTOR;
    uint32  notUsedRQR;
    uint32  notUsedISR;
    uint32  notUsedICR;
    uint32  notUsedRDR;
    uint16  noUsedTDRH;
    uint8   noUsedTDRL;
    uint8   TDR;
} StUSART;

/* CR1 */
#define USART_CR1_M_StartBit8     0x00
#define USART_CR1_M_StartBit9     0x01
#define USART_CR1_M_StartBit7     0x02
#define USART_CR1_M     USART_CR1_M_StartBit8
#define USART_CR1_M0    ((USART_CR1_M & 0x01) << 12)
#define USART_CR1_M1    ((USART_CR1_M & 0x02) << 28)

#define USART_CR1_OVER8_16samples 0x00
#define USART_CR1_OVER8_08samples 0x01
#define USART_CR1_OVER8 (USART_CR1_OVER8_16samples << 15)

#define USART_CR1_TE_Disable    0x00
#define USART_CR1_TE_Enable     0x01
#define USART_CR1_TE (USART_CR1_TE_Enable << 3)

#define USART_CR1_RE_Disable    0x00
#define USART_CR1_RE_Enable     0x01
#define USART_CR1_RE (USART_CR1_RE_Enable << 2)

#define USART_CR1_UE_Disable    0x00
#define USART_CR1_UE_Enable     0x01
#define USART_CR1_UE (USART_CR1_UE_Enable << 0) /* this bit will be written when another setting is doen */

#define Init_USART2_CR1  (USART_CR1_M1 | USART_CR1_OVER8_16samples | USART_CR1_M0 | USART_CR1_TE | USART_CR1_RE)

/* CR2 */
#define USART_CR2_STOP_1bit     0x00
#define USART_CR2_STOP_dot5bit  0x01
#define USART_CR2_STOP_2bit     0x02
#define USART_CR2_STOP_1dot5bit 0x03
#define USART_CR1_STOP (USART_CR2_STOP_1bit << 12)

#define Init_USART2_CR2 (USART_CR1_STOP)

/* BRR */
#define USART_BRR_9600      (0x0341)

#define Init_USART2_BRR (USART_BRR_9600) 

/* pointer to GPIOX register */
#define stpUSART2 ((StUSART*)(USART2_BASE_ADDRESS))

/* Clock must initialized before Port initialize */
void Uart_Init()
{
    stpUSART2->CR1 =  Init_USART2_CR1; 
    stpUSART2->CR2 =  Init_USART2_CR2; 
    stpUSART2->BRR =  Init_USART2_BRR; 
    stpUSART2->CR1 |= USART_CR1_UE; 
}

void Usart2_Transmit(
    uint8 value
)
{
    uint8 i;
    for( i = 0; i < 8; i++ ){
        stpUSART2->TDR = value;
    } 
}
