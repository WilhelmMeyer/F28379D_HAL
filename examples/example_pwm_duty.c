/*
    example_pwm_duty.c

    Demonstrates real-time duty cycle control on channel A.

    Hardware: LAUNCHXL-F28379D
    Output: PWM1A (P0)

    The duty cycle varies smoothly between 10% and 90%.
    Frequency remains fixed at 50kHz.

    hal_task is executed periodically by the library at TASK_PERIOD_NS.
    PWM parameters are updated inside this function.
*/

#include "hal.h"

#define TASK_PERIOD_NS 50000
#define PWM_PERIOD_NS 20000

#define DUTY_MIN 0.1f
#define DUTY_MAX 0.9f
#define DUTY_STEP 0.001f
#define UPDATE_INTERVAL 100

float duty = 0.5f;

void hal_task(void) {
  static int16_t dir = 1;
  static uint16_t counter = 0;

  if (++counter < UPDATE_INTERVAL)
    return;
  counter = 0;

  duty += dir * DUTY_STEP;
  if (duty >= DUTY_MAX)
    dir = -1;
  if (duty <= DUTY_MIN)
    dir = 1;

  hal_pwm_update_a(HAL_PWM1, PWM_PERIOD_NS, duty);
}

void main(void) {
  hal_init();
  hal_pwm_enable_a(HAL_PWM1, PWM_PERIOD_NS, duty);
  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();
  while (1) {
  }
}
