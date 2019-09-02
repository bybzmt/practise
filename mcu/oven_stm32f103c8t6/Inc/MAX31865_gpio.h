#ifndef MAX31865_GPIO_H
#define MAX31865_GPIO_H

#include <stdbool.h>
#include "stm32f1xx_hal.h"


typedef struct {
    GPIO_TypeDef *CE_PORT;
    uint16_t CE_PIN;

    GPIO_TypeDef *CLK_PORT;
    uint16_t CLK_PIN;

    GPIO_TypeDef *MOSI_PORT;
    uint16_t MOSI_PIN;

    GPIO_TypeDef *MISO_PORT;
    uint16_t MISO_PIN;

    bool Is3Wire;
} MAX31865_GPIO;


float MAX31865_readTemp(MAX31865_GPIO *max_gpio);
void MAX31865_init(MAX31865_GPIO *max_gpio);

#endif
