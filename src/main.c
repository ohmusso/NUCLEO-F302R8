#include "./driver/clock/clock.h"
#include "./driver/port/port.h"
#include "./driver/uart/uart.h"
uint32 systickCount;

int main() {
    Clock_Init();
    Port_Init();
    Uart_Init();

    uint8 uartSendChar = 'a';

    systickCount = 0;

    const uint32 blinkTime = 500;
    while (1) {
        if (systickCount <= blinkTime) {
            Port_Write(Port_Off);
        } else if (systickCount <= (blinkTime * 2)) {
            Port_Write(Port_On);
        } else {
            Port_Write(Port_Off);
            Usart2_Transmit(uartSendChar);
            uartSendChar++;
            if (uartSendChar > 'c') {
                uartSendChar = 'a';
            }
            systickCount = 0;
        }
    }
}

/* interupt cycle: 1ms */
// void SysTick_Handler()
// {
//   systickCount++;
// }
