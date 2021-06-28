#include "stm32f4xx_hal.h"

static IWDG_HandleTypeDef IwdgHandle;

void msp_iwdg_init(void)
{
    IwdgHandle.Instance = IWDG;
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;
    IwdgHandle.Init.Reload = 40000/128*5;
    HAL_IWDG_Init(&IwdgHandle);
}

void msp_iwdg_feed(void)
{
    HAL_IWDG_Refresh(&IwdgHandle);
}
