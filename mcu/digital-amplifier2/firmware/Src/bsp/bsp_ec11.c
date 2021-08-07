#include "base.h"
#include "bsp_ec11.h"

static void ec11_btn_irq(void);
static void ec11_rotate_irq(void);

void bsp_ec11_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStructure.Mode      = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = GPIO_PIN_2;
    GPIO_InitStructure.Mode      = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_SetVector(EXTI0_IRQn, (uint32_t)&ec11_btn_irq);
    NVIC_SetVector(EXTI1_IRQn, (uint32_t)&ec11_rotate_irq);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 7, 0);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 7, 0);

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

static void ec11_btn_irq(void)
{
    static volatile bool a=0;
    a=!a;

    if (a) {
        ec11_sw_down();
    } else {
        ec11_sw_up();
    }

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

static volatile bool ec11_b=0;

static void ec11_rotate_irq(void)
{
    uint8_t ec11_a = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    uint8_t tmp = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);

    if (!ec11_a) {
        ec11_b = tmp;
    } else {
        if (ec11_b != tmp) {
            if (ec11_b) {
                ec11_rotate_left();
            } else {
                ec11_rotate_right();
            }
        }
    }

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
}
