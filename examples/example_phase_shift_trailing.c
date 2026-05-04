/*
    example_phase_shift_trailing.c

    Demonstra a defasagem referenciada à borda de descida do canal A.

    Hardware: LAUNCHXL-F28379D
    Saídas: PWM1A (P0) e PWM1B (P1)

    A defasagem é medida a partir do desacionamento do canal A.
    O offset define a banda morta mínima entre o desacionamento de A e o acionamento de B.
    A fase, o duty cycle, a frequência e o offset variam simultaneamente.

    hal_task é executada periodicamente pela biblioteca a cada TASK_PERIOD_NS.
*/

#include "hal.h"

#define TASK_PERIOD_NS 50000
#define PERIOD_MIN_NS 10000
#define PERIOD_MAX_NS 50000
#define PERIOD_STEP_NS 50
#define DUTY_MIN 0.1f
#define DUTY_MAX 0.4f
#define DUTY_STEP 0.001f
#define PHASE_MIN 0.05f
#define PHASE_MAX 0.40f
#define PHASE_STEP 0.001f
#define OFFSET_NS 200
#define UPDATE_INTERVAL 100

uint32_t period_ns = 20000;
float duty_a = 0.3f;
float duty_b = 0.3f;
float phase = 0.1f;

void hal_task(void) {
  static int16_t dir_p = 1;
  static int16_t dir_da = 1;
  static int16_t dir_db = -1;
  static int16_t dir_ph = 1;
  static uint16_t counter = 0;

  if (++counter < UPDATE_INTERVAL)
    return;
  counter = 0;

  period_ns += dir_p * PERIOD_STEP_NS;
  duty_a += dir_da * DUTY_STEP;
  duty_b += dir_db * DUTY_STEP;
  phase += dir_ph * PHASE_STEP;

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
  if (phase >= PHASE_MAX)
    dir_ph = -1;
  if (phase <= PHASE_MIN)
    dir_ph = 1;

  hal_pwm_update_phase_shift_trailing(HAL_PWM1, period_ns, phase, duty_a, duty_b, OFFSET_NS);
}

void main(void) {
  hal_init();
  hal_pwm_enable_phase_shift_trailing(HAL_PWM1, period_ns, phase, duty_a, duty_b, OFFSET_NS);
  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();
  while (1) {
  }
}
