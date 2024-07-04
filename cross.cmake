
IF(WIN32)
    SET(TOOLCHAIN_EXT ".exe")
    SET(ARMGCC_DIR $ENV{ARMGCC_DIR_WINDOWS})
ELSE()
    SET(TOOLCHAIN_EXT "")
    set(ARMGCC_DIR $ENV{ARMGCC_DIR})
ENDIF()

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

set(tools ${ARMGCC_DIR}/bin)
SET(TOOLCHAIN_BIN_DIR ${ARMGCC_DIR}/bin CACHE INTERNAL "bin dir")
SET(TOOLCHAIN_INC_DIR ${ARMGCC_DIR}/arm-none-eabi/include)
SET(TOOLCHAIN_LIB_DIR ${ARMGCC_DIR}/arm-none-eabi/lib)
SET(CMAKE_FIND_ROOT_PATH ${ARMGCC_DIR} CACHE INTERNAL "root path")
SET(CMAKE_C_COMPILER ${tools}/arm-none-eabi-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "c compiler")
SET(CMAKE_CXX_COMPILER ${tools}/arm-none-eabi-g++${TOOLCHAIN_EXT})
SET(CMAKE_ASM_COMPILER ${tools}/arm-none-eabi-gcc${TOOLCHAIN_EXT})
SET(CMAKE_SYSROOT ${tools}/arm-none-eabi CACHE INTERNAL "sys root")


# for _DEBUG and _RELEASE:
add_compile_definitions(G4xx_BL=1 ARM_MATH_CM4 flash_layout)
add_compile_definitions(EW_FRAME_BUFFER_COLOR_FORMAT=EW_FRAME_BUFFER_COLOR_FORMAT_RGB565)
add_compile_definitions(STM32G473xx USE_FULL_LL_DRIVER USE_HAL_DRIVER USE_NUCLEO_64)

SET(CMAKE_C_FLAGS "-mcpu=cortex-m4 -std=gnu11 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb" CACHE INTERNAL "c flags")
SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE INTERNAL "cxx flags")
SET(CMAKE_EXE_LINKER_FLAGS "-T${PROJECT_SOURCE_DIR}/Core/Startup/TC22_G473MCY6.lds --specs=nosys.specs -Wl,--print-memory-usage -Wl,--gc-sections -static -Wl,--start-group -Wl,--end-group" CACHE INTERNAL "linker flags")

# Debug
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG" CACHE INTERNAL "c flags debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "cxx flags debug")

# Release:
SET(CMAKE_C_FLAGS_RELEASE "-O1" CACHE INTERNAL "c flags release")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "cxx flags release")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)