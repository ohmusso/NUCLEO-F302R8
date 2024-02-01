#ifndef DRIVER_PORT_H
#define DRIVER_PORT_H

#include "Platform_Types.h"

typedef uint8 PortOnOff;
#define Port_Off ((PortOnOff)0)
#define Port_On ((PortOnOff)1)

extern void Port_Init();
extern PortOnOff Port_ReadH1();
extern PortOnOff Port_ReadH2();
extern PortOnOff Port_ReadH3();
extern void Port_Write(PortOnOff value);
extern void Port_Flip();

extern void Port_SetMotorDriverEnable(void);
extern void Port_SetMotorDriverDisable(void);
extern void Port_SetMotorDriverEnUV(void);
extern void Port_SetMotorDriverEnVW(void);
extern void Port_SetMotorDriverEnWU(void);

extern void Port_ComEsp32Config(void);

#endif /* DRIVER_PORT_H*/
