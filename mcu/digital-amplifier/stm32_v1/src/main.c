#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

static void SystemClock_Config(void);


extern void UserMain();

static void TaskInit(void *pvParameters)
{
    UNUSED(pvParameters);
    UserMain();
    vTaskDelete(NULL);
}

int main(void)
{
#ifdef SEMIHOSTING
    extern void initialise_monitor_handles(void);
    initialise_monitor_handles();
    printf("boot\n");
#endif

    HAL_Init();

    SystemClock_Config();

    xTaskCreate(TaskInit, "init", 512, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1);
}

static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef clkinitstruct = {0};
    RCC_OscInitTypeDef oscinitstruct = {0};

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
    oscinitstruct.HSIState        = RCC_HSI_ON;
    oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI_DIV2;
    oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL6;
    /* oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL12; */
    if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK) {
        while(1);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV1;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_0)!= HAL_OK) {
        while(1);
    }
}
