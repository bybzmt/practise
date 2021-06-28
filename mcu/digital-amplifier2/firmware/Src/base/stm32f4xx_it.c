#include "stm32f4xx_it.h"
#include "base.h"

void NMI_Handler(void)
{
    NVIC_SystemReset();
}

void HardFault_Handler(void)
{
    NVIC_SystemReset();
    while (1) {}
}

void MemManage_Handler(void)
{
    NVIC_SystemReset();
    while (1) {}
}

void BusFault_Handler(void)
{
    NVIC_SystemReset();
    while (1) {}
}

void UsageFault_Handler(void)
{
    NVIC_SystemReset();
    while (1) {}
}

void DebugMon_Handler(void)
{
}

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    HAL_IncTick();
    xPortSysTickHandler();
}


