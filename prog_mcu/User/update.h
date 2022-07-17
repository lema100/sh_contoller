/*
 * update.h
 *
 *  Created on: 27 мар. 2022 г.
 *      Author: Lema
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "crc16.h"
#include <string.h>
#include <stdint.h>

#define FLASH_BASE			0x08000000UL
#define FLASH_PAGE_SIZE		1024
#define UPDATE_START 		(FLASH_BASE + 64 * 1024)
#define UPDATE_END 			(FLASH_BASE + 128 * 1024)
#define UPDATE_APP_START 	(FLASH_BASE + 4 * 1024)
#define UPDATE_APP_END		(FLASH_BASE + 64 * 1024)

#define UPDATE_RESERVED		10

#pragma pack(push, 1)
typedef struct
{
	uint32_t ver;
	uint32_t ver_ext;
	uint32_t start_add;
	uint32_t end_add;
	uint32_t crc;
	uint32_t reserved[UPDATE_RESERVED];
	uint32_t crc_this;
} update_ctx_t;
#pragma pack(pop)

typedef enum
{
	UPDATE_CRC_STRUCT_ERROR = 100,
	UPDATE_LEN_ERROR,
	UPDATE_END_ADDR_ERROR,
	UPDATE_CRC_ERROR,
	UPDATE_CRC_MISMATCH,
	UPDATE_APP_IS_ACTUAL,
	UPDATE_APP_IS_UPDATED,
} update_state_t;

enum
{
	UPDATE_CMD_READ = 10,
	UPDATE_CMD_WRITE,
	UPDATE_CMD_ERASE,
	UPDATE_CMD_CHECK,
};

update_state_t update_check(update_ctx_t *su);

#ifdef __cplusplus
}
#endif

#endif /* UPDATE_H_ */
