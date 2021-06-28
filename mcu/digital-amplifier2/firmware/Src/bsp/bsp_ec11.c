#include "base.h"
#include "bsp_ec11.h"

static void ec11_btn_irq(void);
static void ec11_rotate_irq(void);

void (*ec11_rotate_left)(void);
void (*ec11_rotate_right)(void);
void (*ec11_sw_down)(void);
void (*ec11_sw_up)(void);

void bsp_ec11_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_0;
    GPIO_InitStructure.Mode      = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = GPIO_PIN_1;
    GPIO_InitStructure.Mode      = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = GPIO_PIN_2;
    GPIO_InitStructure.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_SetVector(EXTI0_IRQn, (uint32_t)&ec11_btn_irq);
    NVIC_SetVector(EXTI1_IRQn, (uint32_t)&ec11_rotate_irq);

    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

static void ec11_btn_irq(void)
{
    static bool a=0;
    a=!a;

    if (a) {
        ec11_sw_down();
    } else {
        ec11_sw_up();
    }

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

static void ec11_rotate_irq(void)
{
    static uint8_t ec11_a=0;
    static uint8_t direction=0;

    uint8_t ec11_b = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);

    ec11_a = !ec11_a;
    if (ec11_a) {
        /* 上升沿 */
        if (ec11_b) {
            direction=2;
        } else {
            direction=1;
        }
    } else {
        /* 下降沿 */
        if (direction==1 && ec11_b==1) {
            /* 左转 */
            ec11_rotate_left();
        } else if (direction==2 && ec11_b==0) {
            /* 右转 */
            ec11_rotate_right();
        }

        direction=0;
    }

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
}
