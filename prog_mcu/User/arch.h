/*
 * arch.h
 *
 *  Created on: 26 мар. 2022 г.
 *      Author: Lema
 */

#ifndef ARCH_H_
#define ARCH_H_

#include "main.h"

#define ARCH_FLASH_PAGES		3
#define ARCH_FLASH_PAGE_SIZE	1024
#define ARCH_FLASH_START		(FLASH_BASE + 61 * ARCH_FLASH_PAGE_SIZE)
#define ARCH_REC_COUNT			((ARCH_FLASH_PAGE_SIZE * ARCH_FLASH_PAGES) / sizeof(arch_rec))
#define ARCH_REC_COUNT_TRUE		((ARCH_FLASH_PAGE_SIZE * (ARCH_FLASH_PAGES - 1)) / sizeof(arch_rec))

#define ARCH_PAR_DATA_LEN		98
#define ARCH_PAR_FLASH_START	(FLASH_BASE + 60 * ARCH_FLASH_PAGE_SIZE)

#pragma pack(push, 1)
typedef struct
{
	uint8_t data[ARCH_PAR_DATA_LEN];
	uint16_t crc;
} arch_par;
#pragma pack(pop)

void arch_save_par(uint8_t * data, uint8_t len);
uint8_t arch_get_par(uint8_t * data, uint8_t len);

#endif /* ARCH_H_ */
