#include "hal_init.h"
#include "hal_epwm.h"

static uint32_t task_period_ns = 50000; // 20kHz

void hal_init(void) {
  Device_init();
  Device_initGPIO();
  Interrupt_initModule();
  Interrupt_initVectorTable();
}

void hal_set_task_period(uint32_t period_ns) { task_period_ns = period_ns; }

void hal_start(void) {
  if (task_period_ns > 0)
    hal_epwm_configure_task(task_period_ns);

  EINT;
  ERTM;
}
