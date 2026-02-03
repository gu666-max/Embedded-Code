        SD使用方法———基于STM32F407ZET6
/*
    本教程使用STM32 HAL 库开发
*/

一、SD卡的驱动
1. 初始化
首先先初始化SD_HandleTypeDef SD总控结构体中的参数
    Instance 就填 SDIO
    HardwareFlowControl 这个是硬件流控，一定要填 。                ····SDIO_HARDWARE_FLOW_CONTROL_DISABLE！！！
    也就是关闭硬件流控！！！这个必须要关闭。（因为这个STM32的勘误手册里说了）
    ClockPowerSave  这个是关闭电源  我们不关闭
    ClockEdge  这个是时钟边沿  必须要填SDIO_CLOCK_EDGE_RISING
    ClockDiv   时钟分频，因为这是初始化，所以要设为400Khz一下  
    48HZ/(ClockDiv+2)=时钟频率。
    ClockBypass 这个是旁路时钟，就是直接由外边输入的信号，而不是晶振产生的。
    BusWide  这个是使用几个线，通常SD卡支持1线和4线，但是我们初始化阶段只能使用1线。 

    然后调用HAL_SD_Init初始化这些参数

2. 硬件初始化
    HAL_SD_MspInit这个函数是回调函数 完成这个函数，这个函数会被HAL_SD_Init调用
    这个函数的职责是开启时钟，配置GPIO口，配置中断，配置DMA结构体参数

3. 修改数据宽度，SDIO使用4个线收发数据
    因为这个时候已经初始化完了，即将计入数据传输阶段了，所以我们将1线改为4线，
    传输速度更快。
    然后我们还要把SDIO的时钟频率修改，应该变得更快，也就是传输数据更快。
    最大时48/2 也就是24MHZ，但是我刚才试了我的SD，最大就12MHZ，所以我就将
    分频值设为2.也就是12MHZ
    MODIFY_REG(SD_HandleStruct.Instance->CLKCR,SDIO_CLKCR_CLKDIV,2);
    通过这个HAL库中的宏就可以安全的修改那个分频值的寄存器，
    但是在修改的前后要先禁止SD卡控制器，最后在使能SD卡控制器。

这三个步骤我们就将SD卡驱动配置完成。因为我们要使用DMA所以我们也要初始化DMA.
然后我们就可以进行SD卡的写入与读取了。
因为SD卡是块（512字节）来读取与写入。

因为HAL公司已将封装了读取与写入的函数了，所以我们就不字节写了，我们就教这个函数怎么用。

HAL_SD_ReadBlocks_DMA(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
这个函数也就是DMA从SD卡传输数据给到单片机
第一个参数SD的总控结构体，第二个参数SD卡传输回来的数据保存的地址，也就是指针
第三个参数是块地址（0也就是SD的第一个块），第四个是块数量（也就是读取的块的数量）

HAL_SD_WriteBlocks_DMA(SD_HandleTypeDef *hsd, uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
这个函数是DMA写SD卡
第一个参数SD的总控结构体，第二个参数发送数据的指针
第三个参数是块地址（0也就是SD的第一个块），第四个是块数量（也就是写入的块的数量）



















