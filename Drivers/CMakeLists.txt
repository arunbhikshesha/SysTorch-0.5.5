# STM32Cube_FW_H7_V1.11.0

set(DriversLib_SOURCES
    BSP/STM32G4xx_Nucleo/stm32g4xx_nucleo.c

    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_fdcan.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_usart.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_usart_ex.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_exti.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_gpio.c
    STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_utils.c
)

add_library(DriversLib STATIC
    ${DriversLib_SOURCES}
)

add_subdirectory(PlatformPackage/RGB565)
add_subdirectory(PlatformPackage/RTE)

target_include_directories(DriversLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32G4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/STM32G4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/PlatformPackage/RGB565
    ${CMAKE_CURRENT_SOURCE_DIR}/PlatformPackage/RTE
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/STM32G4xx_Nucleo
)




