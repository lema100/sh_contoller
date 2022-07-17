/*
 * update.c
 *
 *  Created on: 27 мар. 2022 г.
 *      Author: Lema
 */

#include "update.h"

update_state_t update_check(update_ctx_t *su)
{
	volatile uint32_t crc_calc, update_len;

	memcpy((char *)su, (char *)UPDATE_START, sizeof(update_ctx_t));

	crc_calc = CRC16((char *)su, 0, sizeof(update_ctx_t) - 4);
	if (crc_calc != su->crc_this)
		return UPDATE_CRC_STRUCT_ERROR;

	if (su->end_add >= su->start_add)
		update_len = su->end_add - su->start_add;
	else
		return UPDATE_LEN_ERROR;

	if (su->end_add >= UPDATE_END)
		return UPDATE_END_ADDR_ERROR;

	crc_calc = CRC16((char *)su->start_add + sizeof(update_ctx_t), 0, update_len);

	if (crc_calc != su->crc)
		return UPDATE_CRC_ERROR;

	crc_calc = CRC16((char *)UPDATE_APP_START, 0, update_len);
	if (crc_calc != su->crc)
		return UPDATE_CRC_MISMATCH;

	return UPDATE_APP_IS_ACTUAL;
}
