cmake_minimum_required(VERSION 3.14)

project(sample C ASM)

include_directories(./src/include)

add_executable(sample ./src/startup.s ./src/main.c ./src/Driver/Clock/Clock.c ./src/Driver/Port/Port.c)
