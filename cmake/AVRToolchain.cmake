##############################################################################
# - AVR_MCU                 [Mandatory]
# - AVR_F_CPU               [Mandatory]
# - AVR_UPLOADTOOL          [Default: avrdude]
# - AVR_UPLOADTOOL_PORT     [Default: /dev/ttyUSB0]
# - AVR_PROGRAMMER          [Default: avrispmkII]
# - AVR_UPLOAD_BAUDRATE     [Default: 115200]
###############################################################################
#=============================================================================#
#                                 Executables                                 #
#=============================================================================#
find_program(AVR_CC avr-gcc)
find_program(AVR_CXX avr-g++)
find_program(AVR_OBJCOPY avr-objcopy)
find_program(AVR_SIZE_TOOL avr-size)
find_program(AVR_OBJDUMP avr-objdump)


#=============================================================================#
#                        Toolchain mandatory variables                        #
#=============================================================================#
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})


#=============================================================================#
#                             Platform Variables                              #
#=============================================================================#
if(AVR_F_CPU)
    add_definitions(-DF_CPU=${AVR_F_CPU})
endif(AVR_F_CPU)


#=============================================================================#
#                                  Flags                                      #
#=============================================================================#
set(AVR_COMMON_FLAGS "-mmcu=${AVR_MCU} -ffunction-sections -fdata-sections")


#=============================================================================#
#                                 C Flags                                     #
#=============================================================================#
set(AVR_C_FLAGS "${AVR_COMMON_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS                "-g -Os       ${AVR_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG          "-g           ${AVR_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG ${AVR_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE        "-Os -DNDEBUG -w ${AVR_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -g       -w ${AVR_C_FLAGS}" CACHE STRING "")


#=============================================================================#
#                                 C++ Flags                                   #
#=============================================================================#
set(AVR_CXX_FLAGS "${AVR_COMMON_FLAGS} -fno-exceptions ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS                "-g -Os       ${AVR_CXX_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG          "-g           ${AVR_CXX_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG ${AVR_CXX_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE        "-Os -DNDEBUG ${AVR_CXX_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g       ${AVR_CXX_FLAGS}" CACHE STRING "")


#=============================================================================#
#                       Executable Linker Flags                               #
#=============================================================================#
set(AVR_LINKER_FLAGS "-Wl,--gc-sections,--relax")
set(CMAKE_EXE_LINKER_FLAGS                "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG          "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL     "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE        "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${AVR_LINKER_FLAGS}" CACHE STRING "")


#=============================================================================#
#                       Shared Lbrary Linker Flags                            #
#=============================================================================#
set(CMAKE_SHARED_LINKER_FLAGS                "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG          "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL     "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE        "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${AVR_LINKER_FLAGS}" CACHE STRING "")

set(CMAKE_MODULE_LINKER_FLAGS                "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG          "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL     "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE        "${AVR_LINKER_FLAGS}" CACHE STRING "")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${AVR_LINKER_FLAGS}" CACHE STRING "")

# Needed to delete de "-rdynamic" flag in the compilation
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" CACHE STRING "")


#=============================================================================#
#                                  Includes                                   #
#=============================================================================#
include(CMakeParseArguments)


#=============================================================================#
#                               Upload Variables                              #
#=============================================================================#
# default upload tool
if(NOT AVR_UPLOADTOOL)
   set(
      AVR_UPLOADTOOL avrdude
      CACHE STRING "Set default upload tool: avrdude"
   )
   find_program(AVR_UPLOADTOOL avrdude)
endif(NOT AVR_UPLOADTOOL)

# default upload tool port
if(NOT AVR_UPLOADTOOL_PORT)
   set(
      AVR_UPLOADTOOL_PORT /dev/ttyUSB0
      CACHE STRING "Set default upload tool port: /dev/ttyUSB0"
   )
endif(NOT AVR_UPLOADTOOL_PORT)

# default programmer (hardware)
if(NOT AVR_PROGRAMMER)
   set(
      AVR_PROGRAMMER avrispmkII
      CACHE STRING "Set default programmer hardware model: avrispmkII"
   )
endif(NOT AVR_PROGRAMMER)

# default upload baudrate
if(NOT AVR_UPLOAD_BAUDRATE)
   set(
      AVR_UPLOAD_BAUDRATE 115200
      CACHE STRING "Set default upload baudrate: 115200"
   )
endif(NOT AVR_UPLOAD_BAUDRATE)


#=============================================================================#
#                            Add AVR executable                               #
#=============================================================================#
function(add_avr_executable EXECUTABLE_NAME)
    cmake_parse_arguments(INPUT "" "" "SRCS;LIBS" ${ARGN})

    if(NOT INPUT_SRCS)
        message(FATAL_ERROR "No source files given for ${EXECUTABLE_NAME}.")
    endif(NOT INPUT_SRCS)

    # set file names
    set(ELF_FILE ${EXECUTABLE_NAME}.elf)
    set(HEX_FILE ${EXECUTABLE_NAME}.hex)
    set(MAP_FILE ${EXECUTABLE_NAME}.map)
    set(EEPROM_IMAGE ${EXECUTABLE_NAME}-eeprom.hex)

    # elf file
    add_executable(${ELF_FILE} EXCLUDE_FROM_ALL ${INPUT_SRCS})

    # libraries
    target_link_libraries(${ELF_FILE} ${INPUT_LIBS})

    add_custom_command(
        OUTPUT ${HEX_FILE}
        COMMAND
            ${AVR_OBJCOPY} -j .text -j .data -O ihex ${ELF_FILE} ${HEX_FILE}
        COMMAND
            ${AVR_SIZE_TOOL} -C --mcu=${AVR_MCU} ${ELF_FILE}
        DEPENDS ${ELF_FILE}
    )

    # eeprom
    add_custom_command(
        OUTPUT ${EEPROM_IMAGE}
        COMMAND
            ${AVR_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
                --change-section-lma .eeprom=0 --no-change-warnings
                -O ihex ${ELF_FILE} ${EEPROM_IMAGE}
        DEPENDS ${ELF_FILE}
    )

    add_custom_target(
        ${EXECUTABLE_NAME}
        ALL
        DEPENDS ${HEX_FILE} ${EEPROM_IMAGE}
    )

    set_target_properties(
        ${EXECUTABLE_NAME}
        PROPERTIES
            OUTPUT_NAME "${ELF_FILE}"
    )

    # clean
    get_directory_property(clean_files ADDITIONAL_MAKE_CLEAN_FILES)
    set_directory_properties(
        PROPERTIES
            ADDITIONAL_MAKE_CLEAN_FILES "${MAP_FILE}"
    )

    # upload - with avrdude
    add_custom_target(
        upload_${EXECUTABLE_NAME}
        ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            ${AVR_UPLOADTOOL_OPTIONS}
            -b ${AVR_UPLOAD_BAUDRATE}
            -U flash:w:${HEX_FILE}
            -P ${AVR_UPLOADTOOL_PORT}
        DEPENDS ${HEX_FILE}
        COMMENT "Uploading ${HEX_FILE} to ${AVR_MCU} using ${AVR_PROGRAMMER}"
    )

    # upload eeprom only - with avrdude
    # see also bug http://savannah.nongnu.org/bugs/?40142
    add_custom_target(
        upload_eeprom
        ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            ${AVR_UPLOADTOOL_OPTIONS}
            -b ${AVR_UPLOAD_BAUDRATE}
            -U eeprom:w:${EEPROM_IMAGE}
            -P ${AVR_UPLOADTOOL_PORT}
        DEPENDS ${EEPROM_IMAGE}
        COMMENT "Uploading ${EEPROM_IMAGE} to ${AVR_MCU} using ${AVR_PROGRAMMER}"
    )

    # get status
    add_custom_target(
        get_status
        ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            -b ${AVR_UPLOAD_BAUDRATE}
            -P ${AVR_UPLOADTOOL_PORT} -n -v
        COMMENT "Get status from ${AVR_MCU}"
    )

    # get fuses
    add_custom_target(
        get_fuses
        ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            -b ${AVR_UPLOAD_BAUDRATE}
            -P ${AVR_UPLOADTOOL_PORT} -n
            -U lfuse:r:-:b
            -U hfuse:r:-:b
        COMMENT "Get fuses from ${AVR_MCU}"
    )

    # set fuses
    add_custom_target(
        set_fuses
        ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            -b ${AVR_UPLOAD_BAUDRATE}
            -P ${AVR_UPLOADTOOL_PORT}
            -U lfuse:w:${AVR_L_FUSE}:m
            -U hfuse:w:${AVR_H_FUSE}:m
        COMMENT "Setup: High Fuse: ${AVR_H_FUSE} Low Fuse: ${AVR_L_FUSE}"
    )

    # get oscillator calibration
    add_custom_target(
        get_calibration
            ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            -b ${AVR_UPLOAD_BAUDRATE}
            -P ${AVR_UPLOADTOOL_PORT}
            -U calibration:r:${AVR_MCU}_calib.tmp:r
        COMMENT "Write calibration status of internal oscillator to ${AVR_MCU}_calib.tmp."
   )

    # set oscillator calibration
    add_custom_target(
        set_calibration
        ${AVR_UPLOADTOOL} -p ${AVR_MCU} -c ${AVR_PROGRAMMER}
            -b ${AVR_UPLOAD_BAUDRATE}
            -P ${AVR_UPLOADTOOL_PORT}
            -U calibration:w:${AVR_MCU}_calib.hex
        COMMENT "Program calibration status of internal oscillator from ${AVR_MCU}_calib.hex."
    )

    # disassemble
    add_custom_target(
        disassemble_${EXECUTABLE_NAME}
        ${AVR_OBJDUMP} -h -S ${ELF_FILE} > ${EXECUTABLE_NAME}.lst
        DEPENDS ${ELF_FILE}
    )
endfunction(add_avr_executable)
