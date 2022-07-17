/*
 * arch.c
 *
 *  Created on: 26 мар. 2022 г.
 *      Author: Lema
 */

#include "arch.h"

void arch_save_par(uint8_t * data, uint8_t len)
{
	arch_par _ctx;
	uint32_t address = ARCH_PAR_FLASH_START;
	uint32_t address2 = ARCH_PAR_FLASH_START + ARCH_FLASH_PAGE_SIZE / 2;

	memset(_ctx.data, 0, ARCH_PAR_DATA_LEN);
	memcpy(_ctx.data, data, len);
	_ctx.crc = CRC16((char *)&_ctx, 0, sizeof(arch_par) - 2);

	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.NbPages = 1;
	pEraseInit.PageAddress = address;

	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&pEraseInit, &PageError);
	for (uint32_t i = 0; i < sizeof(arch_par); i += 4)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, *(uint32_t *)(((uint8_t *)&_ctx) + i));
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address2 + i, *(uint32_t *)(((uint8_t *)&_ctx) + i));
	}
	HAL_FLASH_Lock();
}

uint8_t arch_get_par(uint8_t * data, uint8_t len)
{
	arch_par * _ctx;
	arch_par * _ctx2;
	uint32_t address = ARCH_PAR_FLASH_START;
	uint32_t address2 = ARCH_PAR_FLASH_START + ARCH_FLASH_PAGE_SIZE / 2;
	_ctx = (arch_par *)address;
	_ctx2 = (arch_par *)address2;

	if (_ctx->crc == CRC16((char *)_ctx, 0, sizeof(arch_par) - 2))
	{
		memcpy(data, _ctx->data, len);
		return 1;
	}

	if (_ctx2->crc == CRC16((char *)_ctx2, 0, sizeof(arch_par) - 2))
	{
		memcpy(data, _ctx2->data, len);
		return 1;
	}

	return 0;
}
