set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search gcc path
set(TOOLCHAIN_PREFIX arm-none-eabi-)
find_program(BINUTILS_PATH ${TOOLCHAIN_PREFIX}gcc NO_CACHE)

if (NOT BINUTILS_PATH)
    message(FATAL_ERROR "ARM GCC toolchain not found")
endif ()

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)

# FreeRTOS PORT
set(FREE_RTOS_PORT "GCC_ARM_CM4F")

# target files
file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.s")

# tools
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})         # compiler for Assemble
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)        # compiler for C
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)      # compiler for C++
set(CMAKE_C_LINKER ${TOOLCHAIN_PREFIX}gcc)          # linker for C
set(CMAKE_AR ${TOOLCHAIN_PREFIX}gcc-ar)             # Binary Archive Program
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}gcc-ranlib)     # Binary Archive Program
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}objdump)       # obj dump tool
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)       # binary convert tool

# compiler
## flags
### arm
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mgeneral-regs-only -DSTM32 -DSTM32F3")
### gcc
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -Wall -DDEBUG -g3")

# Linker
## linker script file
set(LINKER_FILE ${CMAKE_CURRENT_LIST_DIR}/stm32f3.ld)
### linker flags
#### if use standard library
##### -nostdlib: FreeRTOS use memset and memcpy. don't use this option 
##### -Wl,--gc-sections
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T\"${LINKER_FILE}\" -Wl,-Map=out.map,--cref -Wl,--gc-sections")
## linker Executable
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINKER} <LINK_FLAGS>  -o <TARGET>.elf <OBJECTS> <LINK_LIBRARIES>")

# path
set(CMAKE_SYSROOT ${ARM_GCC_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
