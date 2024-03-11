# compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mgeneral-regs-only")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Wall -DDEBUG -g3")

# FreeRTOS PORT
set(FREE_RTOS_PORT "MSVC_MINGW")

# test mode
set(TARGET_MODULE "test")
