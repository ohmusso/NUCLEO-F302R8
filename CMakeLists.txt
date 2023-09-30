cmake_minimum_required(VERSION 3.14)

project(sample C ASM)

# FreeRTOS
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Absolute path to the directory with FreeRTOSConfig.h")
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "FreeRTOS port name")
set(FREERTOS_HEAP "1" CACHE STRING "FreeRTOS heap model number. 1 .. 5. Or absolute path to custom heap source file")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ICCARM__")
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}")
add_subdirectory(lib/freertos)

# user code

## common user includes
include_directories(./src)
include_directories(./src/include)

## driver
add_subdirectory(src/driver)

## application
add_subdirectory(src/app)

# bios and exe file
add_executable(sample ./src/startup.s ./src/main.c)
target_link_libraries(sample freertos_kernel driver app)

# GoogleTest requires at least C++14
#set(CMAKE_CXX_STANDARD 14)
