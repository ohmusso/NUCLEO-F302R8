cmake_minimum_required(VERSION 3.14)

set(
    SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/app.c"
)

add_library(app STATIC ${SRCS})
