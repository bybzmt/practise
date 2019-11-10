/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "debug.h"
#include "adc.h"
#include "spi.h"
#include "MAX31865.h"
#include "max7219.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define MODE_IDLE    0
#define MODE_FERMENT 1
#define MODE_BAKE    2
#define MODE_FAN     4
#define MODE_ROTATE  8

#define ADC_COUNT 11

//adc 3次采集的值
extern __IO uint16_t adc3shot[3*(ADC_COUNT+1)];

void display_format();
void my_tick();

#endif /* __MAIN_H */
