add_subdirectory(devices)

add_library(I2Cdev STATIC
            I2Cdev/I2Cdev.cpp
            ${I2CDEV_DEVICES_SRCS})
