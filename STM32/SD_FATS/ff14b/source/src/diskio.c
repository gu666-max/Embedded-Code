/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "board_sd.h"
/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

extern HAL_SD_CardInfoTypeDef HAL_SD_CardInfoStruct;
#define DEV_SD		0  //将SD卡的设备号设为0
#define BLOCK_SIZE 512  //SD卡的一块（一扇区）为512字节
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/


//设备状态获取函数
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
		stat = sd_staus();
	
		return stat;
}

	return STA_NOINIT;

}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/



//初始化设备
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
		stat = sd_init();

		// translate the reslut code here

		return stat;

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/





//设备读函数
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive number to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    DRESULT res = RES_OK;  // 默认返回成功

		static DWORD scratch_read[BLOCK_SIZE/4];
    switch (pdrv) {
    case DEV_SD:  // SD 卡驱动
      if((DWORD)buff & 3)
			{
				for(uint32_t i=0;i<count;i++)
				{
					if(sd_read((uint8_t*)scratch_read,sector,1)!=0) return RES_ERROR;
					memcpy(buff,scratch_read,BLOCK_SIZE);
					buff+=BLOCK_SIZE;
					sector++;
				}
				return RES_OK;
			}
			if(sd_read(buff,sector,count)!=0)
			{
				return RES_ERROR;
			}
			
			return res;

    default:
        return RES_PARERR;  // 无效驱动号
    }
}




//设备写函数
#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;
	static DWORD scratch_write[BLOCK_SIZE/4];
	res=RES_OK;
	switch (pdrv) {
	case DEV_SD :

		if((DWORD)buff & 3)
		{
			for(uint32_t i=0;i<count;i++)
			{
				memcpy(scratch_write,buff,BLOCK_SIZE);
				if(sd_read((uint8_t*)scratch_write,sector++,1)!=0) return RES_ERROR;
				buff+=BLOCK_SIZE;
			}
			return RES_OK;
			
		}
		
		if(sd_write((uint8_t*)buff,sector,count)!=0)
			return RES_ERROR;
				
		return res;
		
	default:
		 return RES_PARERR;
	}
}

#endif


//获取设备控制参数函数

/**
 * @brief       获取其他控制参数
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive number (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res;

    if (pdrv == DEV_SD)    /* SD卡 */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(DWORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(DWORD *)buff = HAL_SD_CardInfoStruct.LogBlockSize;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = HAL_SD_CardInfoStruct.LogBlockNbr;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    
    return res;
}












