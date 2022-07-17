#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void MX_GPIO_Init(void);

void gpio_set_out(uint32_t data);
uint32_t gpio_get_in(void);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

