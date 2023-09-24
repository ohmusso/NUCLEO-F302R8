cmake_minimum_required(VERSION 3.14)

project(sample C ASM)

include_directories(./src/include)

add_subdirectory(src/driver)

add_executable(sample ./src/startup.s ./src/main.c)

target_link_libraries(sample driver)

# GoogleTest requires at least C++14
#set(CMAKE_CXX_STANDARD 14)
