RTC_HandleTypeDef RTC_HandleStruct={0};
RTC_DateTypeDef RTC_DateStruct={0};
RTC_TimeTypeDef RTC_TimeStruct={0};


//只调用这个函数就可以，下面那个HAL_RTC_MspInit是硬件会点函数，HAL_RTC_Init会调用
void hal_rtc_init(void)
{
	RTC_HandleStruct.Instance=RTC;
	//1. 初始化RTC 
	RTC_HandleStruct.Init.AsynchPrediv=127;
	RTC_HandleStruct.Init.HourFormat=RTC_HOURFORMAT_24;
	RTC_HandleStruct.Init.OutPut=RTC_OUTPUT_DISABLE;
	RTC_HandleStruct.Init.OutPutPolarity=RTC_OUTPUT_POLARITY_HIGH;
	RTC_HandleStruct.Init.OutPutType=RTC_OUTPUT_TYPE_PUSHPULL;
	RTC_HandleStruct.Init.SynchPrediv=255;
	
	HAL_RTC_Init(&RTC_HandleStruct);
	
	
	// 5.判断RTC时钟是否是第一次配置，如果是第一次配置，就需要配置时间，否则不需要
	
	printf("CSR = 0x%08lx\r\n", RCC->BDCR);

	if(HAL_RTCEx_BKUPRead(&RTC_HandleStruct,RTC_BKP_DR0)!=0x5050)
	{	
		// 6. 因为我们使用的是LSE,所以就在BKP0R寄存器写0x5050
		HAL_RTCEx_BKUPWrite(&RTC_HandleStruct,RTC_BKP_DR0,0x5050);   //第二个填的参数是RTC_BKP寄存器的编号值，也就是偏移量
		
		//7.配置RTC日期
		RTC_DateStruct.Year=26;
		RTC_DateStruct.WeekDay=7;
		RTC_DateStruct.Month=2;
		RTC_DateStruct.Date=1;
		HAL_RTC_SetDate(&RTC_HandleStruct,&RTC_DateStruct,RTC_FORMAT_BIN);
		
		//8.配置RTC时间
		RTC_TimeStruct.Hours=0;
		RTC_TimeStruct.Minutes=0;
		RTC_TimeStruct.Seconds=0;
		//初始化 RTC 为 24小时制 (RTC_HOURFORMAT_24) 时，不需要设置 TimeFormat，否则可能会出错。
		//RTC_TimeStruct.TimeFormat=RTC_HOURFORMAT12_AM;
		RTC_TimeStruct.SubSeconds=1;
		HAL_RTC_SetTime(&RTC_HandleStruct,&RTC_TimeStruct,RTC_FORMAT_BIN);
	}
	
	
	
}


void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	//2 
	__HAL_RCC_RTC_ENABLE();      //2.1 使能RTC
	__HAL_RCC_PWR_CLK_ENABLE();  //2.2 打开PWR
	HAL_PWR_EnableBkUpAccess();  //2.3 取消备份区域写保护 
	__HAL_RCC_RTC_ENABLE();      //2.4 使能RTC     //这个RTC使能两次
	
	//3. 配置LSE并开启
	RCC_OscInitTypeDef RCC_OscInitStruct={0};
	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState=RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	
	//4.配置RTC的时钟源
	RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct={0};
	RCC_PeriphCLKInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;
	RCC_PeriphCLKInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
	
}


//这个是告诉怎么读取当前的时间以及日期
void HAL_RTC_GET_Time(void)
{
	HAL_RTC_GetTime(&RTC_HandleStruct,&RTC_TimeStruct,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_HandleStruct,&RTC_DateStruct,RTC_FORMAT_BIN);
	printf("%d:%d:%d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
	printf("%d.%d.%d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date);
}


