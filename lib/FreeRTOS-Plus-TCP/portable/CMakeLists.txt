cmake_minimum_required(VERSION 3.14)

add_library(freertos_plus_tcpip_port STATIC
    ./NetworkInterface/esp32_wifi_via_uart/NetworkInterface.c
)

target_include_directories(freertos_plus_tcpip_port PUBLIC
    ./Compiler/GCC
)

target_link_libraries(freertos_plus_tcpip_port PRIVATE freertos_kernel freertos_plus_tcpip)
