#ifndef HAL_INIT_H
#define HAL_INIT_H

#include "device.h"
#include "driverlib.h"


void hal_init(void);
void hal_set_task_period(uint32_t period_ns);
void hal_start(void);

#endif
