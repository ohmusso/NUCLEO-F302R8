cmake_minimum_required(VERSION 3.14)

set(
    SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/clock/clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/port/port.c"
)

# add_executable(Driver ${SRCS})
add_library(driver STATIC ${SRCS})
