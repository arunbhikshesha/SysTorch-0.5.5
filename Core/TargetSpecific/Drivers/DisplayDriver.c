#include "stm32g4xx_hal.h"
#include "ewconfig.h"
#include "DisplayDriver.h"


#define DisplaySpiHandle hspi1
extern SPI_HandleTypeDef hspi1; //XXXX Vorhandene SPI-Instanz in main.c
extern DMA_HandleTypeDef DisplayDmaHandle;


volatile uint8_t TransmissionActive;


// SSD1331
static void DisplayDriver_SsdSendCommand(uint8_t command);
static void DisplayDriver_SsdSendCommandWithData(uint8_t command, uint8_t* data, uint8_t size);
static void DisplayDriver_SsdSetArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);



void DisplayDriver_PeripherieInit(void)
{
    /* done in main.c */
}

void DisplayDriver_DisplayOn(void)
{
  DisplayDriver_SsdSendCommand(SSD_SET_DISPLAY_ON);
  HAL_Delay(100);
}

void DisplayDriver_DisplayOff(void)
{
  DisplayDriver_SsdSendCommand(SSD_SET_DISPLAY_OFF);
  HAL_Delay(100);
}

void DisplayDriver_DisplayReset(void)
{
  HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(DISPLAY_RESET_GPIO_Port, DISPLAY_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
}

void DisplayDriver_DisplayInit(void)
{
  // Enable the SPI peripheral
  __HAL_SPI_ENABLE(&DisplaySpiHandle);

  // SSD1331 set remap (mirror vertically)
  uint8_t argument = 0x72;
  DisplayDriver_SsdSendCommandWithData(SSD_SET_REMAP, &argument, 1);
}

void DisplayDriver_TransmitRectangle(const uint16_t *bitmap, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey)
{
  /* Transmission starts */
  TransmissionActive = 1;   
  
  /* Define the display area */
  DisplayDriver_SsdSetArea(posx, posy, sizex, sizey);

  /* Reset the nCS pin */
  DISPLAY_CSX_GPIO_Port->BRR = DISPLAY_CSX_Pin;

  /* Set the DCX pin */
  DISPLAY_DCX_GPIO_Port->BSRR = DISPLAY_DCX_Pin;

  /* Set the SPI in 16-bit mode to match endianess */
  DisplaySpiHandle.Instance->CR2 = SPI_DATASIZE_16BIT;

  /* Disable spi peripherals */
  __HAL_SPI_DISABLE(&DisplaySpiHandle);
  __HAL_DMA_DISABLE(&DisplayDmaHandle);

  CLEAR_BIT(DisplaySpiHandle.Instance->CR2, SPI_CR2_LDMATX);

  /* Clear all flags */
  __HAL_DMA_CLEAR_FLAG(&DisplayDmaHandle, (DMA_FLAG_GL1 << (DisplayDmaHandle.ChannelIndex & 0x1cU)));

  /* Configure DMA Channel data length */
    DisplayDmaHandle.Instance->CNDTR = sizex*sizey;

  /* Configure DMA Channel destination address */
  DisplayDmaHandle.Instance->CPAR = (uint32_t)&DisplaySpiHandle.Instance->DR;

  /* Configure DMA Channel source address */
  DisplayDmaHandle.Instance->CMAR = (uint32_t)bitmap;

  /* Disable the transfer half complete interrupt */
  __HAL_DMA_DISABLE_IT(&DisplayDmaHandle, DMA_IT_HT);

  /* Enable the transfer complete interrupt */
  __HAL_DMA_ENABLE_IT(&DisplayDmaHandle, (DMA_IT_TC | DMA_IT_TE));

  /* Enable the Peripherals */
  __HAL_DMA_ENABLE(&DisplayDmaHandle);
  __HAL_SPI_ENABLE(&DisplaySpiHandle);

  /* Enable Tx DMA Request */
  SET_BIT(DisplaySpiHandle.Instance->CR2, SPI_CR2_TXDMAEN);

  /* nCS pin has to be set after transfer is finished */
}

int DisplayDriver_TransmitActive(void)
{
  return TransmissionActive;
}

void DisplayDriver_DmaCallback(void)
{
  if ((0U != (DMA1->ISR & (DMA_FLAG_TC1))) && (0U != (DisplayDmaHandle.Instance->CCR & DMA_IT_TC)))
  {
    /* Disable the transfer complete and error interrupt */
    __HAL_DMA_DISABLE_IT(&DisplayDmaHandle, DMA_IT_TE | DMA_IT_TC);

    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(&DisplayDmaHandle, DMA_FLAG_TC1);

    /* Transmission ends */
    TransmissionActive = 0;

    /* Wait until the bus is not busy before changing configuration */
    /* SPI is busy in communication or Tx buffer is not empty */
    while(((DisplaySpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET) { }

    /* Set the nCS */
    DISPLAY_CSX_GPIO_Port->BSRR = DISPLAY_CSX_Pin;

    /* Go back to 8-bit mode */
    DisplaySpiHandle.Instance->CR2 = SPI_DATASIZE_8BIT;
  }
  else if ((0U != (DMA1->ISR & (DMA_FLAG_TC1))) && (0U != (DisplayDmaHandle.Instance->CCR & DMA_IT_TE)))
  {
    /* When a DMA transfer error occurs */
    /* A hardware clear of its EN bits is performed */
    /* Disable ALL DMA IT */
    __HAL_DMA_DISABLE_IT(&DisplayDmaHandle, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(&DisplayDmaHandle, DMA_FLAG_GL1 );
  }
}


static void DisplayDriver_SsdSendCommand(uint8_t command)
{
  /* Reset the nCS pin */
  DISPLAY_CSX_GPIO_Port->BRR = DISPLAY_CSX_Pin;

  /* Reset the DCX pin */
  DISPLAY_DCX_GPIO_Port->BRR = DISPLAY_DCX_Pin;

  /* Send the command */
  *((__IO uint8_t*)&DisplaySpiHandle.Instance->DR) = command;

  /* Wait until the bus is not busy before changing configuration */
  while(((DisplaySpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET);

  /* Set the DCX pin */
  DISPLAY_DCX_GPIO_Port->BSRR = DISPLAY_DCX_Pin;

  /* Set the nCS */
  DISPLAY_CSX_GPIO_Port->BSRR = DISPLAY_CSX_Pin;
}

static void DisplayDriver_SsdSendCommandWithData(uint8_t command, uint8_t* data, uint8_t size)
{
  /* Reset the nCS pin */
  DISPLAY_CSX_GPIO_Port->BRR = DISPLAY_CSX_Pin;

  /* Reset the DCX pin */
  DISPLAY_DCX_GPIO_Port->BRR = DISPLAY_DCX_Pin;

  *((__IO uint8_t*)&DisplaySpiHandle.Instance->DR) = command;

  /* Wait until the bus is not busy before changing configuration */
  while(((DisplaySpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET);

  while (size > 0U)
  {
    *((__IO uint8_t*)&DisplaySpiHandle.Instance->DR) = *data; 
    data++;
    size--;

    /* Wait until TXE flag is set to send data */
    while(((DisplaySpiHandle.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
  }

  /* Wait until the bus is not busy before changing configuration */
  while(((DisplaySpiHandle.Instance->SR) & SPI_FLAG_BSY) != RESET);

  /* Set the DCX */
  DISPLAY_DCX_GPIO_Port->BSRR = DISPLAY_DCX_Pin;

  /* Set the nCS */
  DISPLAY_CSX_GPIO_Port->BSRR = DISPLAY_CSX_Pin;
}


static void DisplayDriver_SsdSetArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{    
  static uint16_t old_x = 0xFFFF, old_y = 0xFFFF, old_w = 0xFFFF, old_h = 0xFFFF;
  int b_changed = 0;

  /* Setup the region to fill */
  if (old_x != x || old_w != w)
  {
    /* Set columns */
    unsigned char col[] = { (uint8_t) x, (uint8_t) (x + w - 1) };

    DisplayDriver_SsdSendCommandWithData(SSD_SET_COLUMN, col, 2);
    
    old_x = x;
    old_w = w;
    
    b_changed = 1;
  }

  /* Set rows */
  if (old_y != y || old_h != h)
  {
    unsigned char row[] = { (uint8_t) y, (uint8_t) (y + h - 1) };

    DisplayDriver_SsdSendCommandWithData(SSD_SET_ROW, row, 2);
   
    old_y = y;
    old_h = h;
    
    b_changed = 1;
  }
  
  /* Wait if area changed */
  if (b_changed)
  {
    HAL_Delay(1);
  }
}
