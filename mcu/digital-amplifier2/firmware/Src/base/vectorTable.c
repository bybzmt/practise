#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "vectorTable.h"

#define VECTORTABLE_SIZE        (256)          /* size Cortex-M3 vector table */
#define VECTORTABLE_ALIGNMENT   (0x100ul)      /* 16 Cortex + 32 ARMCM3 = 48 words */
                                               /* next power of 2 = 256            */
/* externals from startup_stm32f446xx.s */
extern uint32_t g_pfnVectors[];

/* new vector table in RAM */
static uint32_t vectorTable[VECTORTABLE_SIZE] __attribute__(( aligned (VECTORTABLE_ALIGNMENT) ));

void vectorTable_to_ram(void)
{
    for (int i = 0; i < VECTORTABLE_SIZE; i++) {
        vectorTable[i] = g_pfnVectors[i];
    }

    __disable_irq();
    SCB->VTOR = (uint32_t)&vectorTable;
    __DSB();
    __enable_irq();
}
