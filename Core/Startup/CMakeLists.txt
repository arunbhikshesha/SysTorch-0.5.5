target_sources(${TARGET_BINARY} PRIVATE
    system_stm32g4xx_BL.c
    bootloader_util.c
    startup_stm32g474retx.s
    syscalls.c
)
