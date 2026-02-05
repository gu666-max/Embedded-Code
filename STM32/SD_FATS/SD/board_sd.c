#include "board_sd.h"

SD_HandleTypeDef SD_HandleStruct;
HAL_SD_CardInfoTypeDef HAL_SD_CardInfoStruct;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

uint8_t sd_init(void)
{
	memset(&SD_HandleStruct,0,sizeof(SD_HandleStruct));
	HAL_SD_CardInfoTypeDef HAL_SD_CardInfoStruct={0};
	SD_HandleStruct.Instance=SDIO;
	SD_HandleStruct.Init.HardwareFlowControl=SDIO_HARDWARE_FLOW_CONTROL_DISABLE;  //硬件流控
	SD_HandleStruct.Init.ClockPowerSave=SDIO_CLOCK_POWER_SAVE_ENABLE;   //是否关闭时钟电源
	SD_HandleStruct.Init.ClockEdge=SDIO_CLOCK_EDGE_RISING;  //时钟边沿
	SD_HandleStruct.Init.ClockDiv=SD_INIT_CLK_DIV;  //初始化分频
	SD_HandleStruct.Init.ClockBypass=SDIO_CLOCK_BYPASS_DISABLE;  //旁路时钟
	SD_HandleStruct.Init.BusWide=SDIO_BUS_WIDE_1B;   //数据宽度
	if(HAL_SD_Init(&SD_HandleStruct)!=HAL_OK)
		return 1;  //初始化失败返回1
	
	
	if(HAL_SD_ConfigWideBusOperation(&SD_HandleStruct,SDIO_BUS_WIDE_4B)!=HAL_OK)
		return 2;   //设置数据宽度失败，返回2
	
	__HAL_SD_DISABLE(&SD_HandleStruct);
	MODIFY_REG(SD_HandleStruct.Instance->CLKCR,SDIO_CLKCR_CLKDIV,0);
	__HAL_SD_ENABLE(&SD_HandleStruct);
	
	HAL_SD_GetCardInfo(&SD_HandleStruct,&HAL_SD_CardInfoStruct);
	return 0;  //成功返回0
}

void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{

	__HAL_RCC_SDIO_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.Pin=GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	//GPIO8 DAT0 ,GPIO9 DAT1, GPIO10,DAT2,GPIO11 DAT2,GPIO12 SCK
	GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate=GPIO_AF12_SDIO;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=GPIO_PIN_2; //CMD
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);

	   /* SDIO DMA Init */
    /* SDIO_RX Init */
   hdma_sdio_rx.Instance = DMA2_Stream3;
   hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
   hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
   hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
   hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
   hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
   hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
   hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
   hdma_sdio_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
   hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
   hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
   hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
   hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&SD_HandleStruct,hdmarx,hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&SD_HandleStruct,hdmatx,hdma_sdio_tx);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
	
}


uint8_t sd_read(uint8_t *pbuffer,uint32_t BlockAddr, uint32_t NumberOfBlocks)
{

	if(HAL_SD_ReadBlocks_DMA(&SD_HandleStruct,pbuffer,BlockAddr,NumberOfBlocks)!=HAL_OK)
		return 1;

	
	while (HAL_SD_GetCardState(&SD_HandleStruct) != HAL_SD_CARD_TRANSFER);
   // 建议增加一个超时退出逻辑，防止死机

	
	return 0;
}


uint8_t sd_write(const unsigned char *pbuffer,uint32_t BlockAddr, uint32_t NumberOfBlocks)
{


	while(HAL_SD_GetCardState(&SD_HandleStruct)!=HAL_SD_CARD_TRANSFER);
	
	
	int a=HAL_SD_WriteBlocks_DMA(&SD_HandleStruct,(uint8_t*)pbuffer,BlockAddr,NumberOfBlocks);

	while(HAL_SD_GetCardState(&SD_HandleStruct)!=HAL_SD_CARD_TRANSFER);
	
	return a;
		
}


uint8_t sd_staus(void)
{
	return 0;  //(HAL_SD_GetCardState(&SD_HandleStruct)==HAL_SD_CARD_READY?0:1);
}







void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
	
}


void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
	
}
