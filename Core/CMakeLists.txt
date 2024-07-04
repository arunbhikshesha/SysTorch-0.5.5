target_sources(${TARGET_BINARY} PRIVATE
    data.c
    data_handler_parameter.c
    DeviceDriver.c
    dma.c
    DS2431.c
    DS2484.c
    ewmain.c
    fdcan2.c
    gpio.c
    gui.c
    i2c.c
    inout.c
    iwdg.c
    msg.c
    serial.c
    spi.c
    stm32g4xx_hal_msp.c
    stm32g4xx_it.c
    timers.c
    errors.c
    init.c
    main.c
    tms.c
    TestBoard.c
)

add_subdirectory(Startup)
add_subdirectory(TargetSpecific)

target_include_directories(${TARGET_BINARY} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Startup
    ${CMAKE_CURRENT_SOURCE_DIR}/TargetSpecific
)
