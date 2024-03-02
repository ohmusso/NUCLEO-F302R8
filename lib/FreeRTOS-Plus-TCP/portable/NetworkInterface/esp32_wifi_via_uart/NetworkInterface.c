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
#include "FreeRTOS_IPv6_Sockets.h"

/* Driver of STM32F302R8 includes. */
#include "src/driver/clock/Clock.h"
#include "src/driver/port/Port.h"
#include "src/driver/uart/uart.h"
#include "src/driver/nvic/nvic.h"

/* UART Task Priority */
#define UART_TASK_PRIORITY (tskIDLE_PRIORITY + 4)

/* buffer size */
#define xRingBufSize ((size_t)(ipconfigNETWORK_MTU * ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS))
#define xBufRxSize ((size_t)(ipconfigNETWORK_MTU))
#define lRingBufInvalidIndex (-1)

TaskHandle_t xTaskAppWifiViaUart = 0;
static BaseType_t xTaskStart = pdFALSE;

static char pcName[ 8 ] = "test";

/* uart rx buffer */
static uint16_t usRxRingBufFront;
static uint16_t usRxRingBufRear;
static uint8_t ucRxRingBuf[xRingBufSize];

/* ethernet buffer */
typedef struct{
    uint8_t ucBytes[ipconfigNETWORK_MTU];
} EthernetBuf_t;
static uint8_t pucNetworkBufRx[xBufRxSize];
static EthernetBuf_t pucNetworkBufTx[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS];

typedef struct {
    uint8_t ucLen;
    const uint8_t* pucWaitData;
} UartWaitData_t;

typedef struct {
    uint8_t ucReq;
    const UartWaitData_t * pxWaitData;
} UartWaitDataUpdateReq_t;

static NetworkInterface_t* pxNetInterface;
static NetworkEndPoint_t xNetEndPoint;
static uint8_t ucMACAddress[ipMAC_ADDRESS_LENGTH_BYTES] = {
    0x9CU, 0x9CU, 0x1FU,
    0xD0U, 0x03U, 0x84U
};
static IPv6_Address_t xPrefix;
static IPv6_Address_t xIPAddressLocal;

static uint8_t ucUartOvrCnt;

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
static BaseType_t xInitializeAddress(void);
static int32_t pushRingBuf(const uint8_t ucData, const uint16_t usFront, const uint16_t usRear);
static int32_t popRingBuf(uint8_t* pData, uint16_t usFront, const uint16_t usRear, const uint16_t usLen);
static uint16_t usGetRingBufDataNum(const uint16_t usRingBufFront, const uint16_t usRingBufRear);

/* function */

/* UART Data Format */
/* tag, data length, data */
/* tag: 2byte, data length: 2byte, data: data length byte */
/* tags */
/*  - 0x01: esp ok: no data length and data */
/*  - 0x02: data : follow data length and data */
#define usLengthTag ((uint16_t)2)
#define usLengthData ((uint16_t)2)
#define usTagData  ((uint16_t)2)
#define usTagAck  ((uint16_t)0xA5A5)
#define usTagError ((uint16_t)0xFFFF)
#define ucRetryNum  ((uint8_t)3)
static void vUartSendData(uint8_t* ucpBuf, size_t xBufLen){
    const uint16_t usOutputTag = usTagData;
    const uint16_t usOutputLength = (uint16_t)xBufLen;

    Usart3_ComEsp32TransmitBytes((uint8_t*)&usOutputTag, usLengthTag);
    Usart3_ComEsp32TransmitBytes((uint8_t*)&usOutputLength, usLengthData);
    Usart3_ComEsp32TransmitBytes(ucpBuf, (uint32_t)xBufLen);
}

static uint16_t usReadTag(void){
    while(usGetRingBufDataNum(usRxRingBufFront, usRxRingBufRear) < 2){
        vTaskDelay(10);
    }

    /* uart read */
    const int32_t lRetFront = popRingBuf(pucNetworkBufRx, usRxRingBufFront, usRxRingBufRear, usLengthTag);
    if( lRetFront == lRingBufInvalidIndex ){
        return usTagError;
    }
    usRxRingBufFront = (uint16_t)lRetFront;

    return *((int16_t*)pucNetworkBufRx);
}

static uint16_t usReadDataLength(void){
    while(usGetRingBufDataNum(usRxRingBufFront, usRxRingBufRear) < 2){
        vTaskDelay(10);
    }

    const int32_t lRetFront = popRingBuf(pucNetworkBufRx, usRxRingBufFront, usRxRingBufRear, usLengthData);
    if( lRetFront == lRingBufInvalidIndex ){
        return 0;
    }
    usRxRingBufFront = (uint16_t)lRetFront;

    return *((int16_t*)pucNetworkBufRx);
}

static void vReadData(const uint16_t uslen){
    const TickType_t xDelay = 10;  /* 10ms */

    /* uart read */
    while(usGetRingBufDataNum(usRxRingBufFront, usRxRingBufRear) < uslen){
        vTaskDelay(xDelay);
    }

    const int32_t lRetFront = popRingBuf(pucNetworkBufRx, usRxRingBufFront, usRxRingBufRear, uslen);
    // if( lRetFront == lRingBufInvalidIndex ){
    //     /* error */
    // }
    usRxRingBufFront = lRetFront;
}

static void vWaitEspInit(void){
    const TickType_t xDelay = 100; /* 100ms */
    const uint16_t usOutputTag = usTagAck;
    uint16_t usRcvTag = 0U;

    while(pdTRUE){
        Usart3_ComEsp32TransmitBytes((uint8_t*)&usOutputTag, usLengthTag);

        vTaskDelay(xDelay);

        int32_t lRetFront = popRingBuf((uint8_t*)&usRcvTag, usRxRingBufFront, usRxRingBufRear, usLengthTag);
        if( lRetFront == lRingBufInvalidIndex ){
            continue;
        }
        usRxRingBufFront = (uint16_t)lRetFront;

        if( usRcvTag == usTagAck ){
            break;
        }
    }

    vTaskDelay(xDelay);
}

void taskAppWifiViaUart(void* pvParameters) {
    const TickType_t xDescriptorWaitTime = pdMS_TO_TICKS(1000);

    usRxRingBufFront = 0;
    usRxRingBufRear = 0;
    memset(ucRxRingBuf, 0, xRingBufSize);

    xInitializeAddress();

    for (;;) {
        uint16_t usTag = usReadTag();
        if( usTag != usTagData ){
            usRxRingBufRear = usRxRingBufFront;
            continue;
        }

        uint16_t usLen = usReadDataLength();
        if( usLen > ipconfigNETWORK_MTU ){
            usRxRingBufRear = usRxRingBufFront;
            continue;
        }

        vReadData(usLen);

        /* send data to tcpip module */
        if( eConsiderFrameForProcessing(pucNetworkBufRx) != eProcessBuffer ){
            /* drop */
            continue;
        }

        NetworkBufferDescriptor_t *pxNetworkBuffer;
        pxNetworkBuffer = pxGetNetworkBufferWithDescriptor((size_t)usLen, xDescriptorWaitTime);
        if( pxNetworkBuffer != (NetworkBufferDescriptor_t*)NULL){
            pxNetworkBuffer->xDataLength = (size_t)usLen;
            pxNetworkBuffer->pxInterface = pxNetInterface;
            pxNetworkBuffer->pxEndPoint = &xNetEndPoint;

            memcpy(pxNetworkBuffer->pucEthernetBuffer, pucNetworkBufRx, (size_t)usLen);
            IPStackEvent_t xRxEvent = {eNetworkRxEvent, (void *)pxNetworkBuffer};

            if( xSendEventStructToIPTask(&xRxEvent, 1000) == pdFAIL ){
                vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
            }
        }

        // const uint8_t testArp[] = {
        //     /* ethernet header */
        //     0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,   /* DestMac */
        //     0x9CU, 0x9CU, 0x1FU, 0xD0U, 0x03U, 0x84U,   /* SrcMac */
        //     0x08U, 0x06U,                               /* Type */
        //     /* ip header */
        //     0x00U, 0x01U,                               /* HwType */
        //     0x08U, 0x00U,                               /* ProtocolType */
        //     0x06U,                                      /* HwSize */
        //     0x04U,                                      /* ProtocolSize */
        //     0x00U, 0x01U,                               /* Opcode */
        //     0x9CU, 0x9CU, 0x1FU, 0xD0U, 0x03U, 0x84U,   /* SenderMac */
        //     192U, 168U, 137U, 5U,                       /* SenderIp */
        //     0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,   /* TargetMac */
        //     192U, 168U, 137U, 1U                        /* TargetIp */
        // };
        // vUartSendData(testArp, sizeof(testArp));
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

    /* copy pointer to this object */
    pxNetInterface = pxInterface;

    return pxInterface;
}

BaseType_t xESP32_Wifi_Via_Uart_NetworkInterfaceInitialise( NetworkInterface_t * pxInterface )
{
    /* initialize UART3 to communicate to ESP32 via UART */
    Clock_ComEsp32Config();
    Port_ComEsp32Config();
    Usart3_ComEsp32Config();
    Nvic_ComEsp32Config();

    /* enable uart rx and uart rx interrupt */
    Usart3_ComEsp32EnableRx();

    /* wait for esp32 initialization */
    vWaitEspInit();

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
    if( ( pxNetworkBuffer == NULL ) || ( pxNetworkBuffer->pucEthernetBuffer == NULL ) || ( pxNetworkBuffer->xDataLength == 0 ) ){
        return pdFALSE;
    }

    BaseType_t xRet;

    if( pxInterface->bits.bInterfaceUp == pdFALSE_UNSIGNED ){
        xRet = pdFALSE;
    }
    else{
        vUartSendData(pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength);

        if( xReleaseAfterSend == pdTRUE ){
            vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
        }

        xRet = pdTRUE;
    }

    return xRet;
}

/* https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Buffer_Management.html */
/* Scheme 1: Implemented by BufferAllocation_1.c */
void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
    for( size_t i = 0; i < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; i++){
        pxNetworkBuffers[i].pucEthernetBuffer = (uint8_t*)&pucNetworkBufTx[i].ucBytes;
    }
}

BaseType_t xESP32_Wifi_Via_Uart_GetPhyLinkStatus( NetworkInterface_t * pxInterface )
{
    /* FIX ME. */
    return pdFALSE;
}

static BaseType_t xInitializeAddress(void){
    /* End-point-1 : private */
    /* Network: fe80::/10 (link-local) */
    /* IPv6   : fe80::1234/128 */
    /* Gateway: - */
    FreeRTOS_inet_pton6("fe80::", xPrefix.ucBytes);
    FreeRTOS_inet_pton6("fe80::1234", xIPAddressLocal.ucBytes);
    FreeRTOS_FillEndPoint_IPv6(
        pxNetInterface,
        &(xNetEndPoint),
        &(xIPAddressLocal),
        &(xPrefix), 10U,    /* Prefix length. */
        NULL,               /* No gateway */
        NULL,               /* pxDNSServerAddress: Not used yet. */
        ucMACAddress
    );

    /* set option after FreeRTOS_FillEndPoint_IPv6 is called. */
    /* FreeRTOS_FillEndPoint_IPv6 set default value. */
    memcpy(xNetEndPoint.ipv6_settings.xIPAddress.ucBytes, xIPAddressLocal.ucBytes, ipSIZE_OF_IPv6_ADDRESS);
    xNetEndPoint.bits.bWantRA = pdTRUE_UNSIGNED;

    return pdTRUE;
}

static uint16_t usGetRingBufDataNum(const uint16_t usRingBufFront, const uint16_t usRingBufRear){
    uint16_t lDataNum;
    if (usRingBufFront <= usRingBufRear){
        lDataNum = usRingBufRear - usRingBufFront;
    }
    else {
        lDataNum = xRingBufSize + usRingBufRear - usRingBufFront;        
    }

    return lDataNum;
}

static int32_t pushRingBuf(const uint8_t ucData, const uint16_t usFront,
                           const uint16_t usRear) {
    if ((usRear + 1) % xRingBufSize == usFront) {
        return lRingBufInvalidIndex;
    }

    ucRxRingBuf[usRear] = ucData;
    return ((usRear + 1) % xRingBufSize);
}

static int32_t popRingBuf(uint8_t* pData, uint16_t usFront, const uint16_t usRear, const uint16_t usLen) {
    const uint16_t usNum = usGetRingBufDataNum(usFront, usRear);

    if (usNum < usLen) {
        return lRingBufInvalidIndex;
    }

    for (int32_t i = 0; i < usLen; i++){
        pData[i] = ucRxRingBuf[usFront];
        usFront = (usFront + 1) % xRingBufSize;
    }

    return usFront;
}

/* intterupt handler */
void taskAppWifiViaUartIsrHandlerUart3Rx(void) {
    uint8_t ucRecvData;

    const uint8_t ucOreDetect = Usart3_ComEsp32Read(&ucRecvData); /* read data and clear interrupt flag */

    if ( ucOreDetect != 0 ){
        ucUartOvrCnt++;
        return;
    }

    /* push data to ring buffer */
    int32_t lRetPushRingBuf =
        pushRingBuf(ucRecvData, usRxRingBufFront, usRxRingBufRear);

    if (lRetPushRingBuf == lRingBufInvalidIndex) {
        /* drop data */
        return;
    }

    usRxRingBufRear = (uint16_t)lRetPushRingBuf;
}
