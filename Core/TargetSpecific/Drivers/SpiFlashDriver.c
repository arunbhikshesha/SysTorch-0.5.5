#include <stdint.h>

#include "SpiFlashDriver.h"
#include "stm32g4xx_hal.h"

extern SPI_HandleTypeDef SpiFlashSpiHandle;
extern DMA_HandleTypeDef SpiFlashDmaRxHandle;
extern DMA_HandleTypeDef SpiFlashDmaTxHandle;

static volatile uint8_t ReceiveActive = 0;

void SpiFlashDriver_PeripherieInit(void)
{
  //-------------------- DMA --------------------------
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  //-------------------- SPI --------------------------
  /* SPI2 parameter configuration*/
  SpiFlashSpiHandle.Instance = SPI2;
  SpiFlashSpiHandle.Init.Mode = SPI_MODE_MASTER;
  SpiFlashSpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
  SpiFlashSpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
  SpiFlashSpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
  SpiFlashSpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
  SpiFlashSpiHandle.Init.NSS = SPI_NSS_SOFT;
  SpiFlashSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  SpiFlashSpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
  SpiFlashSpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
  SpiFlashSpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  SpiFlashSpiHandle.Init.CRCPolynomial = 7;
  SpiFlashSpiHandle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  SpiFlashSpiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&SpiFlashSpiHandle) != HAL_OK)
  {
	// Error_Handler();
  }

  __HAL_SPI_ENABLE(&SpiFlashSpiHandle);

  //-------------------- GPIO --------------------------
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);
}

void SpiFlashDriver_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length)
{
    FLASH_CS_GPIO_Port->BRR = FLASH_CS_Pin;
    *((__IO uint8_t*)&SPI2->DR) = CMD_READ;

    /* clock out address */
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = (address24 >> 16) & 0xFF;

    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = (address24 >> 8) & 0xFF;

    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = address24 & 0xFF;

    switch (length)
    {
        default:
            while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = 0 ;
        case 3:
            while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = 0;
        case 2:
            while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = 0;
        case 1:
            while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
            *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = 0;
        case 0:
            break;
    }
    switch (length)
    {
        case 1:
        *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
        case 2:
        *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
        case 3:
        *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
        default:
            break;
    }

    uint8_t* const buf_end = buffer + length - 4;

    while ((buf_end - buffer) > 3)
    {
        while(((SPI2->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *buffer++ = *((__IO uint8_t*)&SPI2->DR);
	*((__IO uint8_t*)&SPI2->DR) = 0;
	while(((SPI2->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *buffer++ = *((__IO uint8_t*)&SPI2->DR);
	*((__IO uint8_t*)&SPI2->DR) = 0;
	while(((SPI2->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *buffer++ = *((__IO uint8_t*)&SPI2->DR);
	*((__IO uint8_t*)&SPI2->DR) = 0;
	while(((SPI2->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *buffer++ = *((__IO uint8_t*)&SPI2->DR);
	*((__IO uint8_t*)&SPI2->DR) = 0;
    }

    while (buffer < buf_end)
    {
        while(((SPI2->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *buffer++ = *((__IO uint8_t*)&SPI2->DR);
	*((__IO uint8_t*)&SPI2->DR) = 0;
    }

    /* Wait until the bus is ready before releasing Chip select */
    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET) { }

    FLASH_CS_GPIO_Port->BSRR = FLASH_CS_Pin;
    const int rest = length < 4 ? length : 4;
    for (int i = 0; i<rest; i++)
    {
        *buffer++ = *((__IO uint8_t*)&SPI2->DR);
    }
}

void SpiFlashDriver_ReadDataDma(uint32_t address24, uint8_t* buffer, uint32_t length)
{
    /* Pull Flash CS pin low */
    ReceiveActive = 1;
    FLASH_CS_GPIO_Port->BRR = FLASH_CS_Pin;

    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = CMD_READ;

    //clock out address
    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = (address24 >> 16) & 0xFF;

    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = (address24 >> 8) & 0xFF;

    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR) = address24 & 0xFF;

    /* Wait until the bus is ready before reading 4 dummy bytes */
    while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET) { }
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);
    *((__IO uint8_t*)&SpiFlashSpiHandle.Instance->DR);

    /* Reset the threshold bit */
    CLEAR_BIT(SpiFlashSpiHandle.Instance->CR2, SPI_CR2_LDMATX | SPI_CR2_LDMARX);

    /* Set RX Fifo threshold according the reception data length: 8bit */
    SET_BIT(SpiFlashSpiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);

    /******** RX ****************/
    /* Disable the peripheral */
    __HAL_DMA_DISABLE(&SpiFlashDmaRxHandle);

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(&SpiFlashDmaRxHandle, (DMA_FLAG_GI1 << (SpiFlashDmaRxHandle.ChannelIndex & 0x1cU)));

    /* Configure DMA Channel data length */
    SpiFlashDmaRxHandle.Instance->CNDTR = length;

    /* Configure DMA Channel destination address */
    SpiFlashDmaRxHandle.Instance->CPAR = (uint32_t)&SpiFlashSpiHandle.Instance->DR;

    /* Configure DMA Channel source address */
    SpiFlashDmaRxHandle.Instance->CMAR = (uint32_t)buffer;

    __HAL_DMA_DISABLE_IT(&SpiFlashDmaRxHandle, DMA_IT_HT | DMA_IT_TE);
    __HAL_DMA_ENABLE_IT(&SpiFlashDmaRxHandle, (DMA_IT_TC));

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(&SpiFlashDmaRxHandle);

    /* Enable Rx DMA Request */
    SET_BIT(SpiFlashSpiHandle.Instance->CR2, SPI_CR2_RXDMAEN);

    /******** TX ****************/
    /* Disable the peripheral */
    __HAL_DMA_DISABLE(&SpiFlashDmaTxHandle);

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(&SpiFlashDmaTxHandle, (DMA_FLAG_GI1 << (SpiFlashDmaTxHandle.ChannelIndex & 0x1cU)));

    /* Configure DMA Channel data length */
    SpiFlashDmaTxHandle.Instance->CNDTR = length;

    /* Configure DMA Channel destination address */
    SpiFlashDmaTxHandle.Instance->CPAR = (uint32_t)&SpiFlashSpiHandle.Instance->DR;

    /* Configure DMA Channel source address */
    SpiFlashDmaTxHandle.Instance->CMAR = (uint32_t)buffer;

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(&SpiFlashDmaTxHandle);

    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(&SpiFlashSpiHandle);

    /* Enable Tx DMA Request */
    SET_BIT(SpiFlashSpiHandle.Instance->CR2, SPI_CR2_TXDMAEN);
}

int SpiFlashDriver_ReceiveActive(void)
{
    return ReceiveActive;
}

void SpiFlashDriver_DmaCallback()
{
    /* Transfer Complete Interrupt management ***********************************/
    if ((0U != (DMA1->ISR & (DMA_FLAG_TC1 << (SpiFlashDmaRxHandle.ChannelIndex & 0x1cU)))) && (0U != (SpiFlashDmaRxHandle.Instance->CCR & DMA_IT_TC)))
    {
        /* Disable the transfer complete and error interrupt */
        __HAL_DMA_DISABLE_IT(&SpiFlashDmaRxHandle, DMA_IT_TE | DMA_IT_TC);
        /* Clear the transfer complete flag */
        __HAL_DMA_CLEAR_FLAG(&SpiFlashDmaRxHandle, (DMA_FLAG_TC1 << (SpiFlashDmaRxHandle.ChannelIndex & 0x1cU)));


        /* Wait until the bus is not busy before changing configuration */
        /* SPI is busy in communication or Tx buffer is not empty */
        while(((SpiFlashSpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET) { }

        FLASH_CS_GPIO_Port->BSRR = FLASH_CS_Pin;

        ReceiveActive = 0;
    }
}

