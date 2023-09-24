#include "./driver/clock/clock.h"
#include "./driver/port/port.h"

uint32 systickCount;

int main()
{
  Clock_Init();
  Port_Init();
 
  systickCount = 0;

  const uint32 blinkTime = 1000;
  while(1){
    if( systickCount <= blinkTime ){
      Port_Write(Port_Off);
    } 
    else if( systickCount <= (blinkTime * 2) ){
      Port_Write(Port_On);
    }
    else{
      Port_Write(Port_Off);
      systickCount = 0;
    }
  }
}

/* interupt cycle: 1ms */
void SysTick_Handler()
{
  systickCount++;
}
