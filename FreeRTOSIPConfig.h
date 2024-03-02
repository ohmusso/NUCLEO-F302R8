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

#define ipconfigNETWORK_MTU 750
#define ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS 3
#define ipconfigND_CACHE_ENTRIES 12

/* IPv6 DHCP */
/* - document: https://www.infraexpert.com/study/ipv6z5.html */
#define ipconfigUSE_DHCP 1
#define ipconfigUSE_DHCP_HOOK 0
/* -- stateless */
#define ipconfigUSE_RA 1   
/* -- statefull */
#define ipconfigUSE_DHCPv6  0

#endif /* FREERTOS_IP_CONFIG_H */
