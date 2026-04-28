/*
    example_pwm_duty_frequency.c

    Demonstra o controle de duty cycle e frequência nos canais A e B.

    Hardware: LAUNCHXL-F28379D
    Saídas: PWM1A (P0) e PWM1B (P1)

    Os canais A e B compartilham a mesma frequência.
    Cada canal possui seu próprio duty cycle, variando de forma independente.
    A frequência e os dois duty cycles variam simultaneamente.

    hal_task é executada periodicamente pela biblioteca a cada TASK_PERIOD_NS.
*/

#include "hal.h"

#define TASK_PERIOD_NS 50000
#define PERIOD_MIN_NS 10000
#define PERIOD_MAX_NS 50000
#define PERIOD_STEP_NS 50
#define DUTY_MIN 0.1f
#define DUTY_MAX 0.9f
#define DUTY_STEP 0.001f
#define UPDATE_INTERVAL 100

uint32_t period_ns = 20000;
float duty_a = 0.6f;
float duty_b = 0.3f;

void hal_task(void) {
  static int16_t dir_p = 1;
  static int16_t dir_da = 1;
  static int16_t dir_db = -1;
  static uint16_t counter = 0;

  if (++counter < UPDATE_INTERVAL)
    return;
  counter = 0;

  period_ns += dir_p * PERIOD_STEP_NS;
  duty_a += dir_da * DUTY_STEP;
  duty_b += dir_db * DUTY_STEP;

  if (period_ns >= PERIOD_MAX_NS)
    dir_p = -1;
  if (period_ns <= PERIOD_MIN_NS)
    dir_p = 1;
  if (duty_a >= DUTY_MAX)
    dir_da = -1;
  if (duty_a <= DUTY_MIN)
    dir_da = 1;
  if (duty_b >= DUTY_MAX)
    dir_db = -1;
  if (duty_b <= DUTY_MIN)
    dir_db = 1;

  hal_pwm_update_ab(HAL_PWM1, period_ns, duty_a, duty_b);
}

void main(void) {
  hal_init();
  hal_pwm_enable_ab(HAL_PWM1, period_ns, duty_a, duty_b);
  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();
  while (1) {
  }
}
