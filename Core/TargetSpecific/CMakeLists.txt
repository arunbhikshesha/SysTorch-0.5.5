target_sources(${TARGET_BINARY} PRIVATE
    Drivers/DisplayDriver.c
    ew_bsp_display.c
    ew_bsp_event.c
    ew_bsp_flash_reader.c
    ew_bsp_inout.c
    ew_bsp_system.c
    ew_bsp_touch.c
)

target_include_directories(${TARGET_BINARY} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers
)
