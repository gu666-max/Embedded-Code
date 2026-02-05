#ifndef __BOARD_SD_H

#define __BOARD_SD_H
#include "board.h"
#include "string.h"
extern SD_HandleTypeDef SD_HandleStruct;
extern HAL_SD_CardInfoTypeDef HAL_SD_CardInfoStruct;
#define SD_INIT_CLK_DIV     118   // ~400kHz
#define SD_TRANS_CLK_DIV   2     // ~12MHz

uint8_t sd_init(void);

uint8_t sd_read(uint8_t *pbuffer,uint32_t BlockAddr, uint32_t NumberOfBlocks);
uint8_t sd_write(const unsigned char *pbuffer,uint32_t BlockAddr, uint32_t NumberOfBlocks);
uint8_t sd_staus(void);

#endif