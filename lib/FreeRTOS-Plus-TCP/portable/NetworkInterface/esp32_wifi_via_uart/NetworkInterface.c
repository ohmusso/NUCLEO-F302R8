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
/* AT command new line. CR + LF */
#define ucAtCmdNewLineCr ((uint8_t)0x0D)
#define ucAtCmdNewLineLf ((uint8_t)0x0A)
/* New Line Status */
#define ucNewLineStatusNone ((uint8_t)0x00)
#define ucNewLineStatusWaitLf ((uint8_t)0x01)
/* termination character of rxData  */
#define ucTerminationChar ((uint8_t)'\0')
/* uart rx ring buffer size */
#define xRingBufSize ((size_t)64)
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
static uint8_t ucNewLineStatus;

/* uart tx massage */
static const uint8_t ucTxMsgAt[] = "AT";

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
static BaseType_t xReceiveUart(void);

/* function */

void taskAppWifiViaUart(void* pvParameters) {
    const TickType_t xTaskDuration = 1000; /* 10ms */
    lRxRingBufFront = 0;
    lRxRingBufRear = 0;
    lRxRingBufRearTmp = 0;
    ucNewLineStatus = ucNewLineStatusNone;
    memset(ucRxRingBuf, 0, xRingBufSize);
    /* send initialize command */
    // Usart3_ComEsp32TransmitBytes(ucTxMsgAt);
    // (void)xReceiveUart();
    // Usart2_TransmitBytes(ucRxData);

    for (;;) {
        Usart3_ComEsp32TransmitBytes(ucTxMsgAt);
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

static BaseType_t xReceiveUart(void) {
    int32_t lNotifyRxRingBufRear = lRingBufInvalidIndex;

    xTaskNotifyWait(0U, 0U, (uint32_t*)&lNotifyRxRingBufRear, portMAX_DELAY);

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

    ucRecvData = Usart3_ComEsp32Read(); /* read data and clear interrupt flag */

    if (xTaskStart == pdFAIL){
        return;
    }

    /* push data to ring buffer */
    lRetPushRingBuf =
        pushRingBuf(ucRecvData, lRxRingBufFront, lRxRingBufRearTmp);

    if (lRetPushRingBuf == lRingBufInvalidIndex) {
        /* drop data */
        ucNewLineStatus = ucNewLineStatusNone;
        lRxRingBufRearTmp = lRxRingBufRear;
        return;
    }

    lRxRingBufRearTmp = lRetPushRingBuf;

    /* update status */
    if (ucRecvData == ucAtCmdNewLineCr) {
        /* receive CR */
        ucNewLineStatus = ucNewLineStatusWaitLf;
    } else if (ucNewLineStatus == ucNewLineStatusWaitLf) {
        /* receive CR + LF  */
        ucNewLineStatus = ucNewLineStatusNone;
        /* notify uart task */
        lRxRingBufRear = lRxRingBufRearTmp;
        xTaskNotifyFromISR(xTaskAppWifiViaUart, lRxRingBufRear,
                           eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        /* receive LF without CR */
        ucNewLineStatus = ucNewLineStatusNone;
    }
}
