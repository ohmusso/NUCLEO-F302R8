cmake_minimum_required(VERSION 3.14)

set(
    SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/app.c"
)

add_library(app STATIC ${SRCS})

target_link_libraries(app PRIVATE freertos_kernel freertos_plus_tcpip)
