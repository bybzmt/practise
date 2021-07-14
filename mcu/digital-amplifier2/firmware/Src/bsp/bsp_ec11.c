#include "base.h"
#include "bsp_ec11.h"

static void ec11_btn_irq(void);
static void ec11_rotate_irq(void);
static void ec11_rotate_irq2(void);

void (*ec11_rotate_left)(void);
void (*ec11_rotate_right)(void);
void (*ec11_sw_down)(void);
void (*ec11_sw_up)(void);


void bsp_ec11_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStructure.Mode      = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIO_InitStructure.Pin       = GPIO_PIN_2; */
    /* GPIO_InitStructure.Mode      = GPIO_MODE_INPUT; */
    /* HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); */

    NVIC_SetVector(EXTI0_IRQn, (uint32_t)&ec11_btn_irq);
    NVIC_SetVector(EXTI1_IRQn, (uint32_t)&ec11_rotate_irq);
    NVIC_SetVector(EXTI2_IRQn, (uint32_t)&ec11_rotate_irq2);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 7, 0);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 7, 0);
    HAL_NVIC_SetPriority(EXTI2_IRQn, 7, 0);

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
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

static volatile bool ec11_a=0;
static volatile bool ec11_b=0;
static volatile uint8_t direction=0;

static void ec11_rotate_irq(void)
{
    ec11_a = !ec11_a;

    if (ec11_a) {
        if (direction == 0) {
            direction = 1;
        }
    } else {
        if (direction == 2) {
            direction = 0;
        } else if (direction == 1) {
            if (ec11_b) {
                ec11_rotate_left();
            }
        }
    }

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
}

static void ec11_rotate_irq2(void)
{
    ec11_b = !ec11_b;

    if (ec11_b) {
        if (direction == 0) {
            direction = 2;
        }
    } else {
        if (direction == 1) {
            direction = 0;
        } else if (direction == 2) {
            if (ec11_a) {
                ec11_rotate_right();
            }
        }
    }

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
}
