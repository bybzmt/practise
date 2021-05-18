#include "base.h"

void button_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_0;
    GPIO_InitStructure.Mode      = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = GPIO_PIN_1;
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    led_on(1);

    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void led_on(bool flag)
{
    if (flag) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
    }
}

void EXTI0_IRQHandler(void)
{
    button_click();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}


void button_server(void* params)
{
}
