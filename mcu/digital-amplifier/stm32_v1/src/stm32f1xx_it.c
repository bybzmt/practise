#include "base.h"

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
    printf("HardFault\n");
    NVIC_SystemReset();
    while (1);
}

void MemManage_Handler(void) {
    printf("HardFault\n");
    NVIC_SystemReset();
    while (1);
}

void BusFault_Handler(void) {
    printf("BusFault\n");
    NVIC_SystemReset();
    while (1);
}

void UsageFault_Handler(void) {
    printf("UsageFault\n");
    NVIC_SystemReset();
    while (1);
}

/* void DebugMon_Handler(void); */

void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
    xPortSysTickHandler();
    HAL_IncTick();
    /* HAL_SYSTICK_IRQHandler(); */
}


extern I2C_HandleTypeDef hi2c1;

void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void DMA1_Channel6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hi2c1.hdmatx);
}

void DMA1_Channel7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hi2c1.hdmarx);
}
