/*
 * FreeRTOS+TCP <DEVELOPMENT BRANCH>
 * Copyright (C) 2022 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*****************************************************************************
* Note: This file is Not! to be used as is. The purpose of this file is to provide
* a template for writing a network interface. Each network interface will have to provide
* concrete implementations of the functions in this file.
*
* See the following URL for an explanation of this file and its functions:
* https://freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html
*
*****************************************************************************/

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"

/* Driver of STM32F302R8 includes. */
#include "src/driver/clock/Clock.h"
#include "src/driver/port/Port.h"
#include "src/driver/uart/uart.h"
#include "src/driver/nvic/nvic.h"

/* UART Task Priority */
#define UART_TASK_PRIORITY (tskIDLE_PRIORITY + 4)
/* AT command Special Character */
#define ucAtCmdCr ((uint8_t)0x0D)
#define ucAtCmdLf ((uint8_t)0x0A)
#define ucAtCmdO ((uint8_t)'O')
#define ucAtCmdK ((uint8_t)'K')
/* New Line Status */
#define ucUartRcvStatNone ((uint8_t)0x00)
#define ucUartRcvStatWaitLfStart ((uint8_t)0x01)
#define ucUartRcvStatWaitO ((uint8_t)0x02)
#define ucUartRcvStatWaitK ((uint8_t)0x03)
#define ucUartRcvStatWaitCrEnd ((uint8_t)0x04)
#define ucUartRcvStatWaitLfEnd ((uint8_t)0x05)
/* termination character of rxData  */
#define ucTerminationChar ((uint8_t)'\0')
/* uart rx ring buffer size */
#define xRingBufSize ((size_t)1024)
#define xRxDataSize (xRingBufSize + 1U)
#define lRingBufInvalidIndex (-1)

TaskHandle_t xTaskAppWifiViaUart = 0;
static BaseType_t xTaskStart = pdFALSE;

static char pcName[ 8 ] = "test";

/* uart rx buffer */
static int32_t lRxRingBufFront;
static int32_t lRxRingBufRear;
static int32_t lRxRingBufRearTmp;
static uint8_t ucRxRingBuf[xRingBufSize];
uint8_t ucRxData[xRxDataSize];
uint8_t ucTxReq;
static uint8_t ucUartRcvStat;
static uint8_t ucUartOvrCnt;

/* uart tx massage */
/* - ESP32 AT Commands */
/* - see <https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/index.html#> */
static const uint8_t ucTxMsgAT[] = "AT";
static const uint8_t ucTxMsgReset[] = "AT+RST";
static const uint8_t ucTxMsgEchoOff[] = "ATE0";
static const uint8_t ucTxMsgQueryWifiMode[] = "AT+CWMODE?";
static const uint8_t ucTxMsgWifiModeStation[] = "AT+CWMODE=1";
static const uint8_t ucTxMsgQueryWifiConnect[] = "AT+CWJAP?";
static const uint8_t ucTxMsgWifiWEPConnect[] = "AT+CWJAP=\"test\",\"testtest\"";    /* set test access point */
static const uint8_t ucTxMsgGetWifiIp[] = "AT+CIPSTA?";
static const uint8_t ucTxMsgDisconnectWifi[] = "AT+CWQAP";

static const uint8_t* ucTxMsgTable[] = {
    ucTxMsgAT,
    ucTxMsgAT,
    ucTxMsgQueryWifiMode,
    ucTxMsgWifiModeStation,
    ucTxMsgQueryWifiConnect,
    ucTxMsgWifiWEPConnect,
    ucTxMsgGetWifiIp,
    ucTxMsgDisconnectWifi
};

/* prototype declaration  */
static BaseType_t xESP32_Wifi_Via_Uart_NetworkInterfaceInitialise( NetworkInterface_t * pxInterface );

static BaseType_t xESP32_Wifi_Via_Uart_NetworkInterfaceOutput(
    NetworkInterface_t * pxInterface,
    NetworkBufferDescriptor_t * const pxNetworkBuffer,
    BaseType_t xReleaseAfterSend
);

static BaseType_t xESP32_Wifi_Via_Uart_GetPhyLinkStatus( NetworkInterface_t * pxInterface );


NetworkInterface_t * pxESP32_Wifi_Via_Uart_FillInterfaceDescriptor(
    BaseType_t xEMACIndex,
    NetworkInterface_t * pxInterface
);

static int32_t pushRingBuf(const uint8_t ucData, const int32_t lFront,
                           const int32_t lRear);
static int32_t popRingBuf(uint8_t* pData, int32_t ulRingBufFront,
                          int32_t ulRingBufRear);
static BaseType_t xUartReceive(void);

/* function */

void taskAppWifiViaUart(void* pvParameters) {
    const TickType_t xTaskDuration = 5000; /* 5000ms */
    lRxRingBufFront = 0;
    lRxRingBufRear = 0;
    lRxRingBufRearTmp = 0;
    ucUartRcvStat = ucUartRcvStatNone;
    memset(ucRxRingBuf, 0, xRingBufSize);
    ucTxReq = 0;

    /* send setup command */
    Usart3_ComEsp32TransmitBytes(ucTxMsgReset);
    vTaskDelay(xTaskDuration);
    Usart3_ComEsp32TransmitBytes(ucTxMsgEchoOff);
    vTaskDelay(xTaskDuration);

    Usart3_ComEsp32EnableRx();

    for (;;) {
        if( ucTxReq != 0 ){ /* set by debugger */
            ucUartRcvStat = ucUartRcvStatNone;
            lRxRingBufRearTmp = lRxRingBufRear;
            Usart3_ComEsp32TransmitBytes(ucTxMsgTable[ucTxReq]);
            ucTxReq = 0;
            (void)xUartReceive();
            continue;
        }
        vTaskDelay(xTaskDuration);
    }
}

NetworkInterface_t * pxESP32_Wifi_Via_Uart_FillInterfaceDescriptor(
    BaseType_t xEMACIndex,
    NetworkInterface_t * pxInterface
)
{
    /* initialize pxInterface by 0 */
    memset( pxInterface, '\0', sizeof( *pxInterface ) );
    /* set member */
    pxInterface->pcName = pcName;                    /* Just for logging, debugging. */
    pxInterface->pvArgument = ( void * ) xEMACIndex; /* Has only meaning for the driver functions. */
    pxInterface->pfInitialise = xESP32_Wifi_Via_Uart_NetworkInterfaceInitialise;
    pxInterface->pfOutput = xESP32_Wifi_Via_Uart_NetworkInterfaceOutput;
    pxInterface->pfGetPhyLinkStatus = xESP32_Wifi_Via_Uart_GetPhyLinkStatus;
    FreeRTOS_AddNetworkInterface(pxInterface);

    return pxInterface;
}

BaseType_t xESP32_Wifi_Via_Uart_NetworkInterfaceInitialise( NetworkInterface_t * pxInterface )
{
    /* initialize UART3 to communicate to ESP32 via UART */
    Clock_ComEsp32Config();
    Port_ComEsp32Config();
    Usart3_ComEsp32Config();
    Nvic_ComEsp32Config();
    /* create task */
    xTaskStart = xTaskCreate(taskAppWifiViaUart, "WifiViaUart", configMINIMAL_STACK_SIZE,
                (void *)NULL, UART_TASK_PRIORITY, &xTaskAppWifiViaUart);

    return pdPASS;
}

BaseType_t xESP32_Wifi_Via_Uart_NetworkInterfaceOutput(
    NetworkInterface_t * pxInterface,
    NetworkBufferDescriptor_t * const pxNetworkBuffer,
    BaseType_t xReleaseAfterSend
)
{
    /* FIX ME. */
    return pdFALSE;
}

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
    /* FIX ME. */
}

BaseType_t xESP32_Wifi_Via_Uart_GetPhyLinkStatus( NetworkInterface_t * pxInterface )
{
    /* FIX ME. */
    return pdFALSE;
}

static BaseType_t xUartReceive(void) {
    const TickType_t xTaskDuration = 10000; /* 10000ms */
    int32_t lNotifyRxRingBufRear = lRingBufInvalidIndex;

    xTaskNotifyWait(0U, 0U, (uint32_t*)&lNotifyRxRingBufRear, xTaskDuration);

    if (lNotifyRxRingBufRear == lRingBufInvalidIndex) {
        /* no event */
        ucRxData[0] = ucTerminationChar;
        return pdFAIL;
    }

    /* read buffer */
    int32_t lPopRingBuf =
        popRingBuf(ucRxData, lRxRingBufFront, lNotifyRxRingBufRear);

    if (lPopRingBuf == lRingBufInvalidIndex) {
        /* error popRingBuf */
        ucRxData[0] = ucTerminationChar;
        return pdFAIL;
    }

    lRxRingBufFront = lPopRingBuf;
    return pdPASS;
}

static int32_t pushRingBuf(const uint8_t ucData, const int32_t lFront,
                           const int32_t lRear) {
    if ((lRear + 1) % xRingBufSize == lFront) {
        return lRingBufInvalidIndex;
    }

    ucRxRingBuf[lRear] = ucData;
    return ((lRear + 1) % xRingBufSize);
}

static int32_t popRingBuf(uint8_t* pData, int32_t lFront, int32_t lRear) {
    if (lFront == lRear) {
        return lRingBufInvalidIndex;
    }

    while (lFront != lRear) {
        *pData = ucRxRingBuf[lFront];
        pData++;
        lFront = (lFront + 1) % xRingBufSize;
    }

    *pData = ucTerminationChar;

    return lFront;
}

/* intterupt handler */
void taskAppWifiViaUartIsrHandlerUart3Rx(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int32_t lRetPushRingBuf;
    uint8_t ucRecvData;
    uint8_t ucOreDetect;

    ucOreDetect = Usart3_ComEsp32Read(&ucRecvData); /* read data and clear interrupt flag */

    if ( ucOreDetect != 0 ){
        ucUartOvrCnt++;
        return;
    }

    /* push data to ring buffer */
    lRetPushRingBuf =
        pushRingBuf(ucRecvData, lRxRingBufFront, lRxRingBufRearTmp);

    if (lRetPushRingBuf == lRingBufInvalidIndex) {
        /* drop data */
        ucUartRcvStat = ucUartRcvStatNone;
        return;
    }

    lRxRingBufRearTmp = lRetPushRingBuf;

    /* detect OK */
    switch (ucUartRcvStat) {
        case ucUartRcvStatNone:
            if (ucRecvData == ucAtCmdCr) {
                ucUartRcvStat = ucUartRcvStatWaitLfStart;
            }
            break;
        case ucUartRcvStatWaitLfStart:
            if (ucRecvData == ucAtCmdLf) {
                ucUartRcvStat = ucUartRcvStatWaitO;
            }
            else{
                ucUartRcvStat = ucUartRcvStatNone;
            }
            break;
        case ucUartRcvStatWaitO:
            if (ucRecvData == ucAtCmdO) {
                ucUartRcvStat = ucUartRcvStatWaitK;
            }
            else if (ucRecvData == ucAtCmdCr) {
                /* LF ⇒ CR */
                ucUartRcvStat = ucUartRcvStatWaitLfStart;
            }
            else{
                ucUartRcvStat = ucUartRcvStatNone;
            }
            break;
        case ucUartRcvStatWaitK:
            if (ucRecvData == ucAtCmdK) {
                ucUartRcvStat = ucUartRcvStatWaitCrEnd;
            }
            else{
                ucUartRcvStat = ucUartRcvStatNone;
            }
            break;
        case ucUartRcvStatWaitCrEnd:
            if (ucRecvData == ucAtCmdCr) {
                ucUartRcvStat = ucUartRcvStatWaitLfEnd;
            }
            else{
                ucUartRcvStat = ucUartRcvStatNone;
            }
            break;
        case ucUartRcvStatWaitLfEnd:
            if (ucRecvData == ucAtCmdLf) {
                /* complete receiving data */
                lRxRingBufRear = lRxRingBufRearTmp;

                /* notify uart task */
                xTaskNotifyFromISR(xTaskAppWifiViaUart, lRxRingBufRear,
                                eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

                ucUartRcvStat = ucUartRcvStatNone;
            }
            else{
                ucUartRcvStat = ucUartRcvStatNone;
            }
            break;
        default:
            ucUartRcvStat = ucUartRcvStatNone;            
            break;
    }
}
