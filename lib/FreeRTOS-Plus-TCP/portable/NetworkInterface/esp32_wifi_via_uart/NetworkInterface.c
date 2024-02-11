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
/* AT command Special Character */
#define ucAtCmdCr ((uint8_t)0x0D)
#define ucAtCmdLf ((uint8_t)0x0A)
#define ucAtCmdO ((uint8_t)'O')
#define ucAtCmdK ((uint8_t)'K')
/* termination character of rxData  */
#define cTerminationChar ((char)'\0')
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
static uint8_t pucUartRxData[xRxDataSize];

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
static uint8_t ucMACAddress[ipMAC_ADDRESS_LENGTH_BYTES];
static IPv6_Address_t xPrefix;
static IPv6_Address_t xIPAddressLocal;
static IPv6_Address_t xIPAddressGlobal;

static uint8_t ucUartOvrCnt;
static uint8_t ucTxReq;

/* To use AT command paser */
#define xParseWorkBufLen ((size_t)128)
static char cParseWorkBuf[xParseWorkBufLen];

/* uart tx massage */
/* - ESP32 AT Commands */
/* - see <https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/index.html#> */
static const uint8_t ucTxMsgReset[] = "AT+RST";
static const uint8_t ucTxMsgEchoOff[] = "ATE0";
static const uint8_t ucTxMsgEnableIpv6[] = "AT+CIPV6=1";
// static const uint8_t ucTxMsgQueryWifiMode[] = "AT+CWMODE?";
static const uint8_t ucTxMsgWifiModeStation[] = "AT+CWMODE=1";
// static const uint8_t ucTxMsgQueryWifiConnect[] = "AT+CWJAP?";
static const uint8_t ucTxMsgWifiWEPConnect[] = "AT+CWJAP=\"test\",\"testtest\"";    /* set test access point */
static const uint8_t ucTxMsgGetWifiIp[] = "AT+CIPSTA?";
static const uint8_t ucTxMsgGetWifiMac[] = "AT+CIPSTAMAC?";
// static const uint8_t uucTxMsgDisconnectWifi[] = "AT+CWQAP";
static const uint8_t ucTxMsgPassThroughRcvMode[] = "AT+CIPMODE=1";
static const uint8_t ucTxMsgPassThroughSndMode[] = "AT+CIPSEND=4";
static const uint8_t ucTxMsgTest[] = "ABC";

static UartWaitDataUpdateReq_t xUartWaitDataUpdateReq;
static const uint8_t ucResMsgOk[] = "\r\nOK\r\n";
static const UartWaitData_t xUartResOk = {
    sizeof(ucResMsgOk) - 1U,    /* decrement string termination '\0' */
    ucResMsgOk
};
static const uint8_t ucResMsgBeginSend[] = "\r\n>";
static const UartWaitData_t xUartResBeginSend = {
    sizeof(ucResMsgBeginSend) - 1U, /* decrement string termination '\0' */
    ucResMsgBeginSend
};
static const uint8_t ucResMsgSendOk[] = "SEND OK";
static const UartWaitData_t xUartResSendOk = {
    sizeof(ucResMsgSendOk) - 1U, /* decrement string termination '\0' */
    ucResMsgSendOk
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
static BaseType_t xDetectUartRxWaitData(const uint8_t ucRxData);
static int32_t pushRingBuf(const uint8_t ucData, const int32_t lFront,
                           const int32_t lRear);
static int32_t popRingBuf(uint8_t* pData, int32_t ulRingBufFront,
                          int32_t ulRingBufRear);
static BaseType_t xUartReceive(void);
static void xUartAtSend(const uint8_t* pucTxData, const UartWaitData_t* const pxWaitData);
static void xUartDataSend(const uint8_t* pucTxData, const UartWaitData_t* const pxWaitData);
static BaseType_t xInitializeWifiViaUart(void);
static BaseType_t xUpdateWifiMac(void);
static BaseType_t xUpdateWifiIp(void);
static BaseType_t xSetWifieReciveMode(void);
static BaseType_t xSendWifi(void);
static void vfillHexMACAddress(uint8_t* const pucMACAddress, const char* const pcAsciiMACAddress);
static const char* pcSkipUntilNextStr(const char* pcIterator, const char* pcStr);
static void vCopyDoubleQuoteStr(char* pcStrTo, const char* pcStrFrom);

/* function */

void taskAppWifiViaUart(void* pvParameters) {
    lRxRingBufFront = 0;
    lRxRingBufRear = 0;
    lRxRingBufRearTmp = 0;
    memset(ucRxRingBuf, 0, xRingBufSize);

    (void)xInitializeWifiViaUart();

    for (;;) {
        (void)xSendWifi();
        vTaskDelay(3000);
        // if( xUartReceive() == pdFAIL ){
        // }
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
        pucUartRxData[0] = (uint8_t)cTerminationChar;
        return pdFAIL;
    }

    /* read buffer */
    int32_t lPopRingBuf =
        popRingBuf(pucUartRxData, lRxRingBufFront, lNotifyRxRingBufRear);

    if (lPopRingBuf == lRingBufInvalidIndex) {
        /* error popRingBuf */
        pucUartRxData[0] = (uint8_t)cTerminationChar;
        return pdFAIL;
    }

    lRxRingBufFront = lPopRingBuf;
    return pdPASS;
}

static void xUartAtSend(const uint8_t* pucTxData, const UartWaitData_t* const pxWaitData){
    lRxRingBufRearTmp = lRxRingBufRear;
    xUartWaitDataUpdateReq.pxWaitData = pxWaitData;
    xUartWaitDataUpdateReq.ucReq++;
    Usart3_ComEsp32TransmitStr(pucTxData);
}

static void xUartDataSend(const uint8_t* pucTxData, const UartWaitData_t* const pxWaitData){
    lRxRingBufRearTmp = lRxRingBufRear;
    xUartWaitDataUpdateReq.pxWaitData = pxWaitData;
    xUartWaitDataUpdateReq.ucReq++;
    Usart3_ComEsp32TransmitBytes(pucTxData);
}

static BaseType_t xInitializeWifiViaUart(void){
    const TickType_t xTaskDuration = 5000; /* 5000ms */

    /* send setup command */
    /* reset esp32 */
    Usart3_ComEsp32TransmitStr(ucTxMsgReset);
    vTaskDelay(xTaskDuration);

    /* echo off */
    Usart3_ComEsp32TransmitStr(ucTxMsgEchoOff);
    vTaskDelay(xTaskDuration);
    
    /* enable ipv6 */
    Usart3_ComEsp32TransmitStr(ucTxMsgEnableIpv6);
    vTaskDelay(xTaskDuration);

    /* set wifi station mode */
    Usart3_ComEsp32TransmitStr(ucTxMsgWifiModeStation);
    vTaskDelay(xTaskDuration);

    /* connect AP using WEP  */
    Usart3_ComEsp32TransmitStr(ucTxMsgWifiWEPConnect);
    vTaskDelay(xTaskDuration);

    /* enable uart rx and uart rx interrupt */
    Usart3_ComEsp32EnableRx();

    /* get mac and ipv6 address */
    (void)xUpdateWifiMac();
    (void)xUpdateWifiIp();
    (void)xSetWifieReciveMode();

    /* End-point-1 : private */
    /* Network: fe80::/10 (link-local) */
    /* IPv6   : fe80::7009/128 */
    /* Gateway: - */
    FreeRTOS_inet_pton6("fe80::", xPrefix.ucBytes);
    FreeRTOS_FillEndPoint_IPv6( pxNetInterface, &(xNetEndPoint),
                                &(xIPAddressLocal),
                                &(xPrefix), 10U,    /* Prefix length. */
                                NULL,               /* No gateway */
                                NULL,               /* pxDNSServerAddress: Not used yet. */
                                ucMACAddress);

    return pdTRUE;
}

/* AT command parser */
/* https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/Wi-Fi_AT_Commands.html#at-cipstamac-query-set-the-mac-address-of-an-esp32-station */
static BaseType_t xUpdateWifiMac(void){
    const char cResMacPrefix[] = "+CIPSTAMAC:";
    const char* pcAsciiIpIterator;

    xUartAtSend(ucTxMsgGetWifiMac, &xUartResOk);
    if( xUartReceive() == pdFAIL ){
        return pdFAIL;
    }

    /* parse */
    pcAsciiIpIterator = (char*)pucUartRxData;

    /* skip until MACAddress */
    pcAsciiIpIterator = pcSkipUntilNextStr(pcAsciiIpIterator, cResMacPrefix);

    if( *pcAsciiIpIterator == cTerminationChar ){
        return pdFAIL;
    }

    vCopyDoubleQuoteStr(cParseWorkBuf, pcAsciiIpIterator);
    vfillHexMACAddress(ucMACAddress, cParseWorkBuf);

    return pdPASS;
}

/* https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/Wi-Fi_AT_Commands.html#at-cipsta-query-set-the-ip-address-of-an-esp32-station */
static BaseType_t xUpdateWifiIp(void){
    const char cResKeyIpv6LL[] = ":ip6ll:";
    const char cResKeyIpv6GL[] = ":ip6gl:";
    const char* pcAsciiIpIterator;

    xUartAtSend(ucTxMsgGetWifiIp, &xUartResOk);
    if( xUartReceive() == pdFAIL ){
        return pdFAIL;
    }

    /* parse */
    pcAsciiIpIterator = (char*)pucUartRxData;

    /* skip until ipv6 link local */
    pcAsciiIpIterator = pcSkipUntilNextStr(pcAsciiIpIterator, cResKeyIpv6LL);

    /* ipv6 link local */
    vCopyDoubleQuoteStr(cParseWorkBuf, pcAsciiIpIterator);
    if( FreeRTOS_inet_pton6(cParseWorkBuf, xIPAddressLocal.ucBytes) == pdFALSE){
        return pdFALSE;
    }

    /* skip until ipv6 global link */
    pcAsciiIpIterator = pcSkipUntilNextStr(pcAsciiIpIterator, cResKeyIpv6GL);

    /* no ipv6 global link if AP does not support ipv6 */
    if( *pcAsciiIpIterator != cTerminationChar ){
        /* ipv6 global link */
        vCopyDoubleQuoteStr(cParseWorkBuf, pcAsciiIpIterator);
        if( FreeRTOS_inet_pton6(cParseWorkBuf, xIPAddressGlobal.ucBytes) == pdFALSE){
            return pdFALSE;
        }
    }

    return pdPASS;
}

static BaseType_t xSetWifieReciveMode(void){
    /* set recive mode */
    xUartAtSend(ucTxMsgPassThroughRcvMode, &xUartResOk);
    if( xUartReceive() == pdFAIL ){
        return pdFAIL;
    }

    return pdTRUE;
}

static BaseType_t xSendWifi(void){
    xUartAtSend(ucTxMsgPassThroughSndMode, &xUartResBeginSend);
    if( xUartReceive() == pdFAIL ){
        return pdFAIL;
    }

    xUartDataSend(ucTxMsgTest, &xUartResSendOk);
    if( xUartReceive() == pdFAIL ){
        return pdFAIL;
    }

    return pdTRUE;
}

/* Ascii MACAddress string = "xx:xx:xx:xx:xx:xx" */
static void vfillHexMACAddress(uint8_t* const pucMACAddress, const char* const pcAsciiMACAddress){
    uint8_t ucAsciiMACIterator = 0;

    for( uint8_t ucMACIterator = 0; ucMACIterator < ipMAC_ADDRESS_LENGTH_BYTES; ucMACIterator++ ){
        uint8_t ucHex;
        
        ucHex = ucASCIIToHex(pcAsciiMACAddress[ucAsciiMACIterator]);
        ucHex = ucHex << 4;

        ucAsciiMACIterator++;

        ucHex |= ucASCIIToHex(pcAsciiMACAddress[ucAsciiMACIterator]);

        pucMACAddress[ucMACIterator] = ucHex;
        ucAsciiMACIterator++;

        ucAsciiMACIterator++;    /* skip ':' */
    }
}

static const char* pcSkipUntilNextStr(const char* pcIterator, const char* pcStr){
    const char* pcRetIterator = pcIterator;

    /* check empty string */
    if( (*pcIterator == '\0') || (*pcStr == '\0')){
        return pcIterator;
    }

    while(True){
        const char* pcCurIterator = pcRetIterator;
        const char* pcStrIterator = pcStr;
        uint8_t ucStringMatch = 1; /* if string does not match, set by 0  */

        /* compare string */
        while(*pcStrIterator != cTerminationChar){
            /* check string end */
            if(*pcCurIterator == cTerminationChar){
                /* string end */
                return pcCurIterator;
            }

            /* compare character */
            if( *pcCurIterator != *pcStrIterator ){
                ucStringMatch = 0;
                break;
            }

            /* next character */
            pcCurIterator++;
            pcStrIterator++;
        }

        if( ucStringMatch == 1 ){
            /* string Match */
            pcRetIterator += strlen(pcStr);
            break;
        }
        pcRetIterator++;
    }

    /* string match  */
    return pcRetIterator;
}

/* pcStrFrom is double quote string */
static void vCopyDoubleQuoteStr(char* pcStrTo, const char* pcStrFrom){

    /* is first character double quote */
    if( *pcStrFrom != '"'){
        return;
    }

    pcStrFrom++;

    while(True){
        if( *pcStrFrom == '"' ){
            /* termination */
            *pcStrTo = cTerminationChar;
            break;
        }

        if( *pcStrFrom == cTerminationChar ){
            /* invalid but termination */
            *pcStrTo = cTerminationChar;
            break;
        }

        *pcStrTo = *pcStrFrom;

        pcStrTo++;
        pcStrFrom++;
    }

    return;
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

    *pData = (uint8_t)cTerminationChar;

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
        return;
    }

    lRxRingBufRearTmp = lRetPushRingBuf;

    /* detect OK */
    const BaseType_t xDetect = xDetectUartRxWaitData(ucRecvData);
    if(xDetect != pdFALSE ){
        /* complete receiving data */
        lRxRingBufRear = lRxRingBufRearTmp;

        /* notify uart task */
        xTaskNotifyFromISR(xTaskAppWifiViaUart, lRxRingBufRear,
                        eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static UartWaitData_t xUartWaitData;
static size_t xUartWaitIterator;
static BaseType_t xDetectUartRxWaitData(const uint8_t ucRxData){

    if( xUartWaitDataUpdateReq.ucReq > 0 ){
        (void)memcpy((void*)&xUartWaitData, (void*)xUartWaitDataUpdateReq.pxWaitData, sizeof(UartWaitData_t));
        xUartWaitIterator = 0U;
        xUartWaitDataUpdateReq.ucReq = 0U;
    }

    if( xUartWaitData.pucWaitData == (uint8_t*)0 ){
        /* reset */
        xUartWaitIterator = 0U;
        return pdFAIL;
    }

    /* if character match */
    if( ucRxData != xUartWaitData.pucWaitData[xUartWaitIterator] ){
        /* character does not match */

        /* reset */
        xUartWaitIterator = 0U;

        /* re-evaluate 1st character */
        if( ucRxData != xUartWaitData.pucWaitData[xUartWaitIterator] ){
            /* 1st character does not match */
            /* nop */
        }
        else{
            /* 1st character match */
            xUartWaitIterator++;
        }
    }
    else{
        /* character match */
        xUartWaitIterator++;
    }

    BaseType_t xRet;
    if( xUartWaitIterator >= xUartWaitData.ucLen ){
        /* detect */
        xUartWaitIterator = 0U;
        xRet = pdTRUE;
    }
    else{
        /* detecting */
        xRet = pdFALSE;
    }

    return xRet;
}
