cmake_minimum_required(VERSION 3.14)

set(
    SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/clock/clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/port/port.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/uart/uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/timer/timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/nvic/nvic.c"
)

# add_executable(Driver ${SRCS})
add_library(driver STATIC ${SRCS})
