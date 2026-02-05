extern DMA_HandleTypeDef hdma_sdio_rx;
extern DMA_HandleTypeDef hdma_sdio_tx;
extern SD_HandleTypeDef SD_HandleStruct;
extern TIM_HandleTypeDef htim2;


void SDIO_IRQHandler(void)
{

  HAL_SD_IRQHandler(&SD_HandleStruct);

}



void DMA2_Stream3_IRQHandler(void)
{

  HAL_DMA_IRQHandler(&hdma_sdio_rx);

}


void DMA2_Stream6_IRQHandler(void)
{

  HAL_DMA_IRQHandler(&hdma_sdio_tx);

}


