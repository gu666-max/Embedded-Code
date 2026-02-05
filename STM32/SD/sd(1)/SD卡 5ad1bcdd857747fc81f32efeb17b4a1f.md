# SD卡

CMD0   → 卡复位，进入 Idle 状态
CMD8   → 判断是否为 SD v2.0 + 支持的电压范围
CMD55  → 告诉卡：下一个是应用命令（ACMD）
ACMD41 → 申请工作电压 + 等待卡上电完成
CMD2   → 读取 CID（卡唯一身份）
CMD3   → 获取/设置 RCA（卡地址，非 0）
CMD9   → 读取 CSD（容量、块长度、速度等）
CMD7   → 选中卡，进入 TRANSFER 状态

1. SD卡内部

![image.png](image.png)

1. SD卡寄存器

![image.png](image%201.png)

SD卡有三种状态，但是对于我们来说就有两种状态，分别是卡识别模式和数据传输模式

当SD卡在卡识别状态接收到CMD3 (SEND_RCA)命令后，SD卡就进入
数据传输模式

下面就是从卡识别到数据传输模式的流程，

> 在系统复位后，主机处于
卡识别模式，寻找总线上可用的SDIO设备，对SD卡进行数据读写之前需要识别卡的种类：
V1.0标准卡、V2.0标准卡、V2.0高容量卡或者不被识别卡；同时，SD卡也处于卡识别模式，
直到被主机识别到，即当SD卡在卡识别状态接收到CMD3 (SEND_RCA)命令后，SD卡就进入
数据传输模式，而主机在总线上所有卡被识别后也进入数据传输模式。
> 

> 在卡识别模式下，主机会复位所有处于“卡识别模式”的SD卡，确认其工作电压范围，
识别SD卡类型，并且获取SD卡的相对地址(卡相对地址较短，便于寻址)。在卡识别过程中，
要求SD卡工作在识别时钟频率FOD的状态下。
> 

![image.png](image%202.png)

数据传输过程

![image.png](image%203.png)

SD卡初始化的指令

```c
CMD0   → 卡复位，进入 Idle 状态
CMD8   → 判断是否为 SD v2.0 + 支持的电压范围
CMD55  → 告诉卡：下一个是应用命令（ACMD）
ACMD41 → 申请工作电压 + 等待卡上电完成
CMD2   → 读取 CID（卡唯一身份）
CMD3   → 获取/设置 RCA（卡地址，非 0）
CMD9   → 读取 CSD（容量、块长度、速度等）
CMD7   → 选中卡，进入 TRANSFER 状态
```

![image.png](image%204.png)

SD_HandleTypeDef  SD总控结构体

HAL_SD_CardInfoTypeDef SD卡信息

HAL_SD_ConfigWideBusOperation  修改SD卡的数据位宽

HAL_SD_ReadBlocks 读SD数据块

HAL_SD_WriteBlocks 写SD卡数据块

HAL_SD_GetCardInfo 读取SD卡的相关状态信息

IO口的模式要设置为复用推挽。 

1. 读数据
2. 配置SD卡的IO口
3. 配置SD_HandleTypeDef总控结构体的参数，并调用HAL_SD_Init初始化
4. 重新配置SD卡的数据位宽（设为4位）
5. 写数据

| 阶段 | 总线 | ClockDiv | SD_CLK | 说明 |
| --- | --- | --- | --- | --- |
| 初始化 | 1bit | 118 | ~400k | 必须 |
| 切线宽 | 1bit→4bit | 118 | ~400k | 可低速 |
| 传输（稳） | 4bit | 2 | ~12M | 推荐 |
| 传输（快） | 4bit | 0 | 24M | DMA |
|  |  |  |  |  |
|  |  |  |  |  |
|  |  |  |  |  |

> 分频计算公式：48M/(分频值+2）=最终SDIO的时钟频率
> 

> 最开始初始化阶段只能将始终设为400k以下，SDIO不分频是48M，所以要进行118分频
> 

> 并且最开始只能1线传输，可以等到初始化完毕改成4线传输，然后就可以提升频率了。
> 

> 然后我们设置分频值为2，那么时钟频率就是12M了。因为SDIO时钟频率最大传输为25MHZ，但是我们现在没有使用DMA所以12Mhz就可以了，等到用上DMA设为24Mhz。
> 

SDIO的时钟：SDIO是挂载在APB2下的外设，但是他的时钟是（SDIO_SCK)PLL48提供的

SDIO_CK = SDIOCLK/(2 +CLKDIV)  最大值为24MHZ

![image.png](image%205.png)

![image.png](image%206.png)

总控结构体的成员

![image.png](image%207.png)

HAL_SD_CardInfoTypeDef结构体的成员，里面是SD卡的信息，可以通过

HAL_SD_GetCardInfo查询SD卡的信息，然后保存在结构体里。