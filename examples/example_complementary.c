/*
    example_complementary.c

    Demonstra o par complementar com controle de duty cycle, frequência e offset.

    Hardware: LAUNCHXL-F28379D
    Saídas: PWM1A (P0) e PWM1B (P1)

    O duty cycle, a frequência e o offset variam simultaneamente.
    Offset positivo aplica banda morta entre os pulsos.
    Offset negativo aplica sobreposição entre os pulsos.

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
#define OFFSET_MIN_NS -500
#define OFFSET_MAX_NS 500
#define OFFSET_STEP_NS 5
#define UPDATE_INTERVAL 100

uint32_t period_ns = 20000;
float duty = 0.5f;
int32_t offset_ns = 0;

void hal_task(void) {
  static int16_t dir_p = 1;
  static int16_t dir_d = 1;
  static int16_t dir_o = 1;
  static uint16_t counter = 0;

  if (++counter < UPDATE_INTERVAL)
    return;
  counter = 0;

  period_ns += dir_p * PERIOD_STEP_NS;
  duty += dir_d * DUTY_STEP;
  offset_ns += dir_o * OFFSET_STEP_NS;

  if (period_ns >= PERIOD_MAX_NS)
    dir_p = -1;
  if (period_ns <= PERIOD_MIN_NS)
    dir_p = 1;
  if (duty >= DUTY_MAX)
    dir_d = -1;
  if (duty <= DUTY_MIN)
    dir_d = 1;
  if (offset_ns >= OFFSET_MAX_NS)
    dir_o = -1;
  if (offset_ns <= OFFSET_MIN_NS)
    dir_o = 1;

  hal_pwm_update_complementary(HAL_PWM1, period_ns, duty, offset_ns);
}

void main(void) {
  hal_init();
  hal_pwm_enable_complementary(HAL_PWM1, period_ns, duty, offset_ns);
  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();
  while (1) {
  }
}
