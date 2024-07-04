cmake_minimum_required(VERSION 3.20.3)
message("Build type: " ${CMAKE_BUILD_TYPE})
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(USER_DIR $ENV{APPDATA})

# Read out project version number from CHANGELOG.md
if(CMAKE_HOST_WIN32)
  set(BASH_PATH "C:/PROGRA~1/Git/bin/bash.exe")
  if(NOT EXISTS ${BASH_PATH})
    set(BASH_PATH "${USER_DIR}/../Local/Programs/Git/bin/bash.exe")
  endif()
endif()
execute_process(COMMAND ${BASH_PATH} ${CMAKE_SOURCE_DIR}/externals/s4-config/peripherie/tools/changelog-get-version.sh 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
    OUTPUT_VARIABLE PROJECT_VERSION_STRING
)
if(NOT PROJECT_VERSION_STRING)
    message(FATAL_ERROR "Failed to get PROJECT_VERSION_STRING from CHANGELOG.md!")
endif()
message(STATUS "Using PROJECT_VERSION_STRING from CHANGELOG.md: ===${PROJECT_VERSION_STRING}===")

project(stm32-cmake-vscode VERSION ${PROJECT_VERSION_STRING} LANGUAGES C CXX ASM)

# Generate version header file
# set(ENV{SOURCE_DATE_EPOCH} 1677906363) # !! use only for Reproducible Build Testing etc. !!
string(TIMESTAMP APP_VERSION_POSTFIX UTC)
string(TIMESTAMP VERSION_DATE_TIME_UINT32 %y%m%d%H%M UTC)
configure_file(${CMAKE_SOURCE_DIR}/externals/s4-config/peripherie/tools/version.h.in ${CMAKE_BINARY_DIR}/version.h @ONLY)

# External projects
set(EXTERNALS "${PROJECT_SOURCE_DIR}/externals" CACHE STRING "")

include_directories(
    ${CMAKE_BINARY_DIR}
    ${EXTERNALS}
    ${EXTERNALS}/s4-config
    ${PROJECT_SOURCE_DIR}/Core
    ${PROJECT_SOURCE_DIR}/Core/Startup
    ${PROJECT_SOURCE_DIR}/Core/TargetSpecific
    ${PROJECT_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
    ${PROJECT_SOURCE_DIR}/Drivers/PlatformPackage/RTE
    ${PROJECT_SOURCE_DIR}/Drivers/PlatformPackage/RGB565
 )

MESSAGE(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}") 

# Project specific settings:
set (TARGET_NAME FW_SystemTorch)
set (TARGET_FILE_ENDING f004)

# make Target Name public:
set(File_SH ${CMAKE_CURRENT_BINARY_DIR}/setEnv.sh)
file(WRITE ${File_SH} "#!/usr/bin/env bash\n")
file(APPEND ${File_SH} "export TARGET_NAME=${TARGET_NAME}")
execute_process(COMMAND chmod a+x ${File_SH} RESULT_VARIABLE res)

set(TARGET_BINARY ${TARGET_NAME}.elf)
add_executable(${TARGET_BINARY})
add_subdirectory(Drivers)
add_subdirectory(GeneratedCode)
add_subdirectory(Core)
target_link_libraries(${TARGET_BINARY}
    GeneratedCodeLib
    DriversLib
    -lm
)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS CHANGELOG.md)

if(CMAKE_CROSSCOMPILING)
    install(DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/
        DESTINATION /lib/firmware
    )
endif()

# Only build:
add_custom_target(build ALL DEPENDS ${TARGET_BINARY}
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
COMMAND ${tools}/arm-none-eabi-objcopy -O binary ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_BINARY} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_BINARY}.bin
)

# Build and encrypt:
add_custom_target(build_and_encrypt ALL DEPENDS build
USES_TERMINAL
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
COMMAND ${BASH_PATH} ${CMAKE_SOURCE_DIR}/externals/sbsfu-se/tools/postbuild-encrypt.sh ${CMAKE_CURRENT_BINARY_DIR} ${TARGET_NAME} "TC22_G473MCY6" 0 ${PROJECT_VERSION_STRING} ${VERSION_DATE_TIME_UINT32}
COMMAND ${BASH_PATH} ${CMAKE_SOURCE_DIR}/externals/sbsfu-se/tools/postbuild-bundle.sh ${CMAKE_CURRENT_BINARY_DIR} ${TARGET_NAME} ${TARGET_FILE_ENDING} ${PROJECT_VERSION_STRING} ${VERSION_DATE_TIME_UINT32}
)

# board IP in User(!)- settings.json eintragen: "cmake.environment": { "MAPRO_BOARD_IP": "192.168.1.13" }
set(BOARD_IP $ENV{MAPRO_BOARD_IP})

# build and copy to MaPro and flash via CAN:
add_custom_target(build_and_run DEPENDS build_and_encrypt
USES_TERMINAL
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
COMMAND ssh-keygen -q -R ${BOARD_IP}
COMMAND scp -ostricthostkeychecking=no -i ~/.ssh/iQsMapro -oHostKeyAlgorithms=+ssh-rsa ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/* root@${BOARD_IP}:/lib/firmware/
COMMAND ssh -ostricthostkeychecking=no -i ~/.ssh/iQsMapro -oHostKeyAlgorithms=+ssh-rsa root@${BOARD_IP} -f /usr/bin/s4-tools can program /lib/firmware/${TARGET_NAME}.${TARGET_FILE_ENDING}
)
