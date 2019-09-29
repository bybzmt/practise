/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "debug.h"
#include "adc.h"
#include "spi.h"
#include "MAX31865.h"
#include "24l01.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define MODE_IDLE    0
#define MODE_FERMENT 1
#define MODE_BAKE    2
#define MODE_FAN     4
#define MODE_ROTATE  8

extern uint8_t mode;
extern int16_t setting_top_temperature;
extern int16_t setting_bottom_temperature;
extern float top_temperature;
extern float bottom_temperature;

void display_format(uint16_t number, bool top, bool dp);

void my_tick();


#endif /* __MAIN_H */
