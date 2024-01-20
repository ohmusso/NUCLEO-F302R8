cmake_minimum_required(VERSION 3.14)

add_subdirectory(portable)

add_library(freertos_plus_tcpip STATIC
  FreeRTOS_ARP.c
  FreeRTOS_BitConfig.c
  FreeRTOS_DHCP.c
  FreeRTOS_DHCPv6.c
  FreeRTOS_DNS.c
  FreeRTOS_DNS_Cache.c
  FreeRTOS_DNS_Callback.c
  FreeRTOS_DNS_Networking.c
  FreeRTOS_DNS_Parser.c
  FreeRTOS_ICMP.c
  FreeRTOS_IP.c
  FreeRTOS_IP_Timers.c
  FreeRTOS_IP_Utils.c
  FreeRTOS_IPv4.c
  FreeRTOS_IPv4_Sockets.c
  FreeRTOS_IPv4_Utils.c
  FreeRTOS_IPv6.c
  FreeRTOS_IPv6_Sockets.c
  FreeRTOS_IPv6_Utils.c
  FreeRTOS_ND.c
  FreeRTOS_RA.c
  FreeRTOS_Routing.c
  FreeRTOS_Sockets.c
  FreeRTOS_Stream_Buffer.c
  FreeRTOS_TCP_IP.c
  FreeRTOS_TCP_IP_IPv4.c
  FreeRTOS_TCP_IP_IPv6.c
  FreeRTOS_TCP_Reception.c
  FreeRTOS_TCP_State_Handling.c
  FreeRTOS_TCP_State_Handling_IPv4.c
  FreeRTOS_TCP_State_Handling_IPv6.c
  FreeRTOS_TCP_Transmission.c
  FreeRTOS_TCP_Transmission_IPv4.c
  FreeRTOS_TCP_Transmission_IPv6.c
  FreeRTOS_TCP_Utils.c
  FreeRTOS_TCP_Utils_IPv4.c
  FreeRTOS_TCP_Utils_IPv6.c
  FreeRTOS_TCP_WIN.c
  FreeRTOS_Tiny_TCP.c
  FreeRTOS_UDP_IP.c
  FreeRTOS_UDP_IPv4.c
  FreeRTOS_UDP_IPv6.c
)

target_include_directories(freertos_plus_tcpip
    PUBLIC
        include
)

target_link_libraries(freertos_plus_tcpip PRIVATE freertos_plus_tcpip_port freertos_kernel)
