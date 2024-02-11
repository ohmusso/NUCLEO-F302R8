#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#define ipconfigBYTE_ORDER pdLITTLE_ENDIAN
#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES 0
#define ipconfigUSE_IPv4 0
#define ipconfigUSE_IPv6 1
#define ipconfigSUPPORT_OUTGOING_PINGS 1
#define ipconfigUSE_TCP 0
#define ipconfigUSE_TCP_WIN 0
#define ipconfigUSE_DNS 0
/* IPv6 DHCP */
/* - document: https://www.infraexpert.com/study/ipv6z5.html */
#define ipconfigUSE_DHCP 0
/* -- stateless */
#define ipconfigUSE_RA 0     
/* -- statefull */
#define ipconfigUSE_DHCPv6  0

#endif /* FREERTOS_IP_CONFIG_H */
