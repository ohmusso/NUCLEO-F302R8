cmake_minimum_required(VERSION 3.14)

add_subdirectory(portable)

add_library(freertos_plus_tcpip STATIC
    FreeRTOS_ARP.c
    FreeRTOS_DHCP.c
    FreeRTOS_DNS.c
    FreeRTOS_DNS_Cache.c
    FreeRTOS_DNS_Callback.c
    FreeRTOS_DNS_Networking.c
    FreeRTOS_DNS_Parser.c
    FreeRTOS_ICMP.c
    FreeRTOS_IP.c
    FreeRTOS_IP_Timers.c
    FreeRTOS_IP_Utils.c
    FreeRTOS_Sockets.c
    FreeRTOS_Stream_Buffer.c
    FreeRTOS_TCP_IP.c
    FreeRTOS_TCP_Reception.c
    FreeRTOS_TCP_State_Handling.c
    FreeRTOS_TCP_Transmission.c
    FreeRTOS_TCP_Utils.c
    FreeRTOS_TCP_WIN.c
    FreeRTOS_Tiny_TCP.c
    FreeRTOS_UDP_IP.c
)

target_include_directories(freertos_plus_tcpip
    PUBLIC
        include
)

target_link_libraries(freertos_plus_tcpip PRIVATE freertos_plus_tcpip_port freertos_kernel)
