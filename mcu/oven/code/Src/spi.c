#include "spi.h"
#include "debug.h"

#ifdef SPI_GPIO
    #include "spi_gpio.c"
#elif defined(SPI_DMA)
    #include "spi_dma.c"
#else
    #include "spi_hspi.c"
#endif
