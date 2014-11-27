### CMake toolchain file
## AVR defines
set(AVR_MCU "atmega2560")
set(AVR_F_CPU "16000000L")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/AVRToolchain.cmake")

### Project declaration
project(I2Cdevlib)
cmake_minimum_required(VERSION 2.8)

### Arduino
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(ARDUINO_SDK_PATH "/usr/share/arduino")
find_package(Arduino)
include_arduino_library(Wire)

### I2CLib
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)
add_subdirectory(src)
