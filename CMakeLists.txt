cmake_minimum_required(VERSION 3.14)

project(sample C CXX ASM)
set(CMAKE_CXX_STANDARD 14)

# Common includes
include_directories(./src)
include_directories(./src/include)

# FreeRTOS Kernel
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Absolute path to the directory with FreeRTOSConfig.h")
set(FREERTOS_PORT ${FREE_RTOS_PORT} CACHE STRING "FreeRTOS port name")
set(FREERTOS_HEAP "1" CACHE STRING "FreeRTOS heap model number. 1 .. 5. Or absolute path to custom heap source file")
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}")
add_subdirectory(lib/freertos)

# FreeRTOS Plus TCP/IP
set(FREERTOS_PLUS_TCPIP_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Absolute path to the directory with FreeRTOSIPConfig.h")
include_directories("${FREERTOS_PLUS_TCPIP_CONFIG_FILE_DIRECTORY}")
add_subdirectory(lib/FreeRTOS-Plus-TCP)

# Driver
add_subdirectory(src/driver)

if ("${TARGET_MODULE}" STREQUAL "test")
    enable_testing()
    add_subdirectory(test)
else()
    # Application
    add_subdirectory(src/app)

    add_executable(sample ./src/startup.s ./src/main.c)
    target_link_libraries(sample freertos_kernel freertos_plus_tcpip driver app)
endif()
