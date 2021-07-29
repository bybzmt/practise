#include "base.h"

void (*ec11_rotate_left)(void);
void (*ec11_rotate_right)(void);
void (*ec11_sw_down)(void);
void (*ec11_sw_up)(void);

void bsp_ec11_init(void);
