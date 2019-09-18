#ifndef MAX31865_H
#define MAX31865_H

#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "spi.h"

typedef struct {
    SPIx spi;

    bool Is3Wire;
} MAX31865;


float MAX31865_Read_1shot(MAX31865 *max_gpio);
float MAX31865_Read(MAX31865 *max_gpio);
void MAX31856_Off(MAX31865 *max_gpio);
void MAX31856_AutoConvert(MAX31865 *max_gpio);

#endif
