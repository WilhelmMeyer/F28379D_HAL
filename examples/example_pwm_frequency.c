/*
    example_pwm_frequency.c

    Demonstra o controle de frequência em tempo real no canal B.

    Hardware: LAUNCHXL-F28379D
    Saída: PWM1B (P0)

    A frequência varia suavemente entre 20kHz e 100kHz.
    O duty cycle permanece fixo em 50%.
*/

#include "hal.h"

#define TASK_PERIOD_NS 50000
#define PERIOD_MIN_NS 10000
#define PERIOD_MAX_NS 50000
#define PERIOD_STEP_NS 50
#define UPDATE_INTERVAL 100

uint32_t period_ns = 20000;

void hal_task(void) {
  static int16_t dir = 1;
  static uint16_t counter = 0;

  if (++counter < UPDATE_INTERVAL)
    return;
  counter = 0;

  period_ns += dir * PERIOD_STEP_NS;
  if (period_ns >= PERIOD_MAX_NS)
    dir = -1;
  if (period_ns <= PERIOD_MIN_NS)
    dir = 1;

  hal_pwm_update_b(HAL_PWM1, period_ns, 0.5f);
}

void main(void) {
  hal_init();
  hal_pwm_enable_b(HAL_PWM1, period_ns, 0.5f);
  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();
  while (1) {
  }
}
