##########################################################################
# - ARDUINO_SDK_PATH        [Mandatory]
# - ARDUINO_PLATFORM        [Default: avr]
# - ARDUINO_VARIANT         [Default: standard]
##########################################################################

#=============================================================================#
# detect_arduino_version(VAR_NAME)
#
#       VERSION_NAME - Name of Arduino detected version
#       VERSION_DEFINE - ARDUINO macro name
#=============================================================================#
function(detect_arduino_version VERSION_NAME VERSION_DEFINE)
    file(READ ${ARDUINO_VERSION_PATH} RAW_VERSION)

    if("${RAW_VERSION}" MATCHES "([0-9]+[.][0-9]+[.][0-9]+)")
        set(PARSED_VERSION ${CMAKE_MATCH_1})
    elseif("${RAW_VERSION}" MATCHES "([0-9]+[.][0-9]+)")
        set(PARSED_VERSION ${CMAKE_MATCH_1}.0)
    else()
        message(FATAL_ERROR "Unable to autodetect Arduino version")
    endif()

    string(REPLACE "." "" VERSION_DEF ${PARSED_VERSION})
    set(${VERSION_DEFINE} "${VERSION_DEF}"    PARENT_SCOPE)
    set(${VERSION_NAME}   "${PARSED_VERSION}" PARENT_SCOPE)
endfunction()

#=============================================================================#
# add_arduinocore_lib()
#
# Generates de arduinocore library
#=============================================================================#
function(add_arduinocore_lib)
    file(GLOB ARDUINO_CORE_SRC_FILES
        "${ARDUINO_SDK_PATH}/hardware/arduino/${ARDUINO_PLATFORM}/cores/arduino/*.cpp"
        "${ARDUINO_SDK_PATH}/hardware/arduino/${ARDUINO_PLATFORM}/cores/arduino/*.c"
    )
    add_library(arduinocore STATIC ${ARDUINO_CORE_SRC_FILES})
endfunction()

#=============================================================================#
# include_arduino_library(LIB_NAME)
#
# Generates the requested Arduino library name LIB_NAME
#=============================================================================#
function(include_arduino_library LIB_NAME)
    set(LIB_PATH "${ARDUINO_SDK_PATH}/hardware/arduino/${ARDUINO_PLATFORM}/libraries/${LIB_NAME}")

    set(HDR_SEARCH_LIST
        ${LIB_PATH}/*.h
        ${LIB_PATH}/*.hh
        ${LIB_PATH}/*.hxx)

    file(GLOB_RECURSE HDR_FILES ${HDR_SEARCH_LIST})

    if(HDR_FILES)
        set(DIR_LIST "")
        foreach(FILE_PATH ${HDR_FILES})
            get_filename_component(DIR_PATH ${FILE_PATH} PATH)
            set(DIR_LIST ${DIR_LIST} ${DIR_PATH})
        endforeach()
        list(REMOVE_DUPLICATES DIR_LIST)

        include_directories(${DIR_LIST})
    endif()
endfunction()

#=============================================================================#
# add_arduino_library(LIB_NAME)
#
# Generates the requested Arduino library name LIB_NAME
#=============================================================================#
function(add_arduino_library LIB_NAME)
    set(LIB_PATH "${ARDUINO_SDK_PATH}/hardware/arduino/${ARDUINO_PLATFORM}/libraries/${LIB_NAME}")

    include_arduino_library(LIB_NAME)

    set(SRC_SEARCH_LIST
        ${LIB_PATH}/*.cpp
        ${LIB_PATH}/*.c
        ${LIB_PATH}/*.cc
        ${LIB_PATH}/*.cxx)

    file(GLOB_RECURSE SRC_FILES ${SRC_SEARCH_LIST})

    if(SRC_FILES)
        add_library(${LIB_NAME} STATIC
                    ${SRC_FILES})
    else()
        message("Library ${LIB_NAME} in ${LIB_PATH} not found.")
    endif()
endfunction()

#=============================================================================#
#                                Initialization                               #
#=============================================================================#
if(NOT ARDUINO_FOUND AND ARDUINO_SDK_PATH)
    find_file(ARDUINO_VERSION_PATH
        NAMES lib/version.txt
        PATHS ${ARDUINO_SDK_PATH}
        DOC "Path to Arduino version file.")

    if(NOT ARDUINO_VERSION_PATH)
        message(FATAL_ERROR "Invalid Arduino SDK path (${ARDUINO_SDK_PATH}).\n")
    endif()

    detect_arduino_version(ARDUINO_SDK_VERSION ARDUINO_VERSION_DEFINE)
    set(ARDUINO_VERSION_DEFINE  ${ARDUINO_VERSION_DEFINE} CACHE STRING "Arduino Version Define")

    message(STATUS "Detected arduino SDK version ${ARDUINO_SDK_VERSION} in ${ARDUINO_SDK_PATH}")
    set(ARDUINO_FOUND True CACHE INTERNAL "Arduino Found")
endif()

#=============================================================================#
#                            Default variables                                #
#=============================================================================#
if(NOT ARDUINO_VARIANT)
    set(ARDUINO_VARIANT standard)
endif()

if(NOT ARDUINO_PLATFORM)
    set(ARDUINO_PLATFORM avr)
endif()

if(ARDUINO_VERSION_DEFINE)
    add_definitions(-DARDUINO=${ARDUINO_VERSION_DEFINE})
endif()

#=============================================================================#
#                           Final configuration                               #
#=============================================================================#
if(ARDUINO_SDK_PATH)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${ARDUINO_SDK_PATH}/hardware/tools/${ARDUINO_PLATFORM})

    if(ARDUINO_PLATFORM STREQUAL "avr")
        include_directories("${ARDUINO_SDK_PATH}/hardware/tools/avr/avr/include")
    endif()

    include_directories("${ARDUINO_SDK_PATH}/hardware/arduino/${ARDUINO_PLATFORM}/cores/arduino")
    include_directories("${ARDUINO_SDK_PATH}/hardware/arduino/${ARDUINO_PLATFORM}/variants/${ARDUINO_VARIANT}")
else()
    message(FATAL_ERROR "Could not find Arduino SDK (set ARDUINO_SDK_PATH)!")
endif()
