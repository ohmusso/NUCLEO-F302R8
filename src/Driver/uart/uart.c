#include "uart.h"

#define USART1_BASE_ADDRESS 0x40013800 /* USART1 */
#define USART2_BASE_ADDRESS 0x40004400 /* USART2 */
#define USART3_BASE_ADDRESS 0x40004800 /* USART3 */

typedef struct {
    uint32 CR1;
    uint32 CR2;
    uint32 notUsedCR3;
    uint16 notUsedBRRH;
    uint16 BRR;
    uint32 notUseGTPR;
    uint32 notUsedRTOR;
    uint32 notUsedRQR;
    uint32 ISR;
    uint32 notUsedICR;
    uint8 RDR;
    uint8 notUsedRDR[3];
    uint16 noUsedTDRH;
    uint8 noUsedTDRL;
    uint8 TDR;
} StUSART;

/* CR1 */
#define USART_CR1_M_StartBit8 0x00
#define USART_CR1_M_StartBit9 0x01
#define USART_CR1_M_StartBit7 0x02
#define USART_CR1_M USART_CR1_M_StartBit8
#define USART_CR1_M0 ((USART_CR1_M & 0x01) << 12)
#define USART_CR1_M1 ((USART_CR1_M & 0x02) << 28)

#define USART_CR1_OVER8_16samples 0x00
#define USART_CR1_OVER8_08samples 0x01
#define USART_CR1_OVER8 (USART_CR1_OVER8_16samples << 15)

#define USART_CR1_RXNEIE_Enable (1 << 5)

#define USART_CR1_TE_Disable 0x00
#define USART_CR1_TE_Enable 0x01
#define USART_CR1_TE (USART_CR1_TE_Enable << 3)

#define USART_CR1_RE_Disable 0x00
#define USART_CR1_RE_Enable 0x01
#define USART_CR1_RE (USART_CR1_RE_Enable << 2)

#define USART_CR1_UE_Disable 0x00
#define USART_CR1_UE_Enable 0x01
/* this bit will be written when another setting is doen */
#define USART_CR1_UE (USART_CR1_UE_Enable << 0)

#define Init_USART2_CR1                                        \
    (USART_CR1_M1 | USART_CR1_OVER8_16samples | USART_CR1_M0 | \
     USART_CR1_RXNEIE_Enable | USART_CR1_TE | USART_CR1_RE)

/* CR2 */
#define USART_CR2_STOP_1bit 0x00
#define USART_CR2_STOP_dot5bit 0x01
#define USART_CR2_STOP_2bit 0x02
#define USART_CR2_STOP_1dot5bit 0x03
#define USART_CR2_STOP (USART_CR2_STOP_1bit << 12)

#define Init_USART2_CR2 (USART_CR2_STOP)

/* BRR fck = 8MHz */
#define USART_BRR_9600 (0x0341)
#define USART_BRR_115200 (0x0045)

#define Init_USART2_BRR (USART_BRR_9600)

/* pointer to UART register */
#define stpUSART1 ((volatile StUSART*)(USART1_BASE_ADDRESS))
#define stpUSART2 ((volatile StUSART*)(USART2_BASE_ADDRESS))
#define stpUSART3 ((volatile StUSART*)(USART3_BASE_ADDRESS))

static void enqueueUsart2RxQueue(uint8 data);
static int dequeueUsart2RxQueue();

/* Clock must initialized before Port initialize */
void Uart_Init() {
    stpUSART2->CR1 = Init_USART2_CR1;
    stpUSART2->CR2 = Init_USART2_CR2;
    stpUSART2->BRR = Init_USART2_BRR;
    stpUSART2->CR1 |= USART_CR1_UE;
}

#define Uart_WaitUntilTxComp(stpReg) \
    do {                             \
        ; /* busy wait */            \
    } while ((((stpReg)->ISR >> 7) & 0x00000001) == 0)

void Usart2_Transmit(uint8 value) {
    Uart_WaitUntilTxComp(stpUSART2);
    stpUSART2->TDR = value;
}

void Usart2_TransmitBytes(const uint8* const str) {
    int i = 0;

    while (str[i] != '\0') {
        Uart_WaitUntilTxComp(stpUSART2);
        stpUSART2->TDR = str[i];
        i++;
    }

    Uart_WaitUntilTxComp(stpUSART2);
    stpUSART2->TDR = '\r';
    Uart_WaitUntilTxComp(stpUSART2);
    stpUSART2->TDR = '\n';
}

void Usart2_TransmitHexDatas(const char_t* const str, const char_t len) {
    int i = 0;
    char_t c = '0';

    while (i < len) {
        Uart_WaitUntilTxComp(stpUSART2);
        if ((str[i] >= 0) && (str[i] <= 9)) {
            c = str[i] + '0';
        } else if ((str[i] >= 10) && (str[i] <= 15)) {
            c = str[i] - 10 + 'A';
        } else {
            c = str[i];
        }

        stpUSART2->TDR = c;
        i++;
    }

    Uart_WaitUntilTxComp(stpUSART2);
    stpUSART2->TDR = '\r';
    Uart_WaitUntilTxComp(stpUSART2);
    stpUSART2->TDR = '\n';
}

/* Return */
/*  - UartRetFetchData */
/*  - UartRetNoData */
UartRetType Uart2_ReadData(RxDataType* data) {
    UartRetType ret;
    int rxData = dequeueUsart2RxQueue();
    if (rxData >= 0) {
        *(data) = (RxDataType)rxData;
        ret = UartRetFetchData;
    } else {
        ret = UartRetNoData;
    }
    return ret;
}

#define UART2_RX_QUEUE_SIZE 4
static int usart2RxQueue[UART2_RX_QUEUE_SIZE];
static int usart2RxQueueFront = 0;
static int usart2RxQueueRear = 0;
static void enqueueUsart2RxQueue(uint8 data) {
    if ((usart2RxQueueRear + 1) % UART2_RX_QUEUE_SIZE == usart2RxQueueFront) {
        return;
    }

    usart2RxQueue[usart2RxQueueRear] = (int)data;
    usart2RxQueueRear = (usart2RxQueueRear + 1) % UART2_RX_QUEUE_SIZE;
}

static int dequeueUsart2RxQueue() {
    if (usart2RxQueueFront == usart2RxQueueRear) {
        return -1;
    }

    int data = usart2RxQueue[usart2RxQueueFront];
    usart2RxQueueFront = (usart2RxQueueFront + 1) % UART2_RX_QUEUE_SIZE;
    return data;
}

void Usart3_ComEsp32Config(void) {
    stpUSART3->CR1 = (USART_CR1_M1 | USART_CR1_OVER8_16samples | USART_CR1_M0 |
                      USART_CR1_RXNEIE_Enable | USART_CR1_TE | USART_CR1_RE);
    stpUSART3->CR2 = USART_CR2_STOP;
    stpUSART3->BRR = USART_BRR_9600;
    stpUSART3->CR1 |= USART_CR1_UE;
}

void Usart3_ComEsp32TransmitBytes(const uint8* const str) {
    int i = 0;

    while (str[i] != '\0') {
        Uart_WaitUntilTxComp(stpUSART3);
        stpUSART3->TDR = str[i];
        i++;
    }

    Uart_WaitUntilTxComp(stpUSART3);
    stpUSART3->TDR = '\r';
    Uart_WaitUntilTxComp(stpUSART3);
    stpUSART3->TDR = '\n';
}

uint8 Usart3_ComEsp32Read(void) { return stpUSART3->RDR; }

/* use in interrupt context */
void Usart2_RxIndication() { enqueueUsart2RxQueue(stpUSART2->RDR); }
