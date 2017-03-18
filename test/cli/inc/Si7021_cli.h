#ifndef SI7021_CLI_H_
#define SI7021_CLI_H_

#include "stm32f4xx_hal.h"

typedef uint8_t (*print_t)(uint8_t* buf, uint16_t len);

void Si7021_cli_init(print_t func);
void Si7021_cli_engine(uint8_t* char_in);

#endif /* SI7021_CLI_H_ */
