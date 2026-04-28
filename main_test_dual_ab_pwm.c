#include "hal.h"

#define TASK_PERIOD_NS 50000 // 20kHz

#define PERIOD_NS 20000 // 50kHz fixo
#define PERIOD_MIN_NS 10000
#define PERIOD_MAX_NS 50000
#define PERIOD_STEP_NS 50

#define DUTY_A_MIN 0.2f
#define DUTY_A_MAX 0.8f
#define DUTY_B_MIN 0.1f
#define DUTY_B_MAX 0.5f
#define DUTY_STEP 0.001f

#define UPDATE_SLOW 200
#define UPDATE_FAST 2000

#define TRIGGER_GPIO 16

void hal_task(void) {
  // PWM1 — só duty varia suavemente, A e B em patamares distintos
  static float duty_a1 = 0.6f;
  static float duty_b1 = 0.2f;
  static int16_t dir_duty_a1 = 1;
  static int16_t dir_duty_b1 = 1;
  static uint16_t counter1 = 0;

  // PWM2 — só período varia suavemente
  static uint32_t period2 = (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2;
  static int16_t dir_period2 = 1;
  static uint16_t counter2 = 0;

  // PWM3 — duty e período variam suavemente
  static float duty_a3 = 0.6f;
  static float duty_b3 = 0.2f;
  static int16_t dir_duty_a3 = 1;
  static int16_t dir_duty_b3 = -1;
  static uint32_t period3 = (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2;
  static int16_t dir_period3 = 1;
  static uint16_t counter3 = 0;

  // PWM4 — brusco
  static float duty_a4 = DUTY_A_MIN;
  static float duty_b4 = DUTY_B_MIN;
  static uint32_t period4 = PERIOD_MIN_NS;
  static uint16_t counter4 = 0;
  static uint16_t trigger_state4 = 0;

  bool updated1 = false;
  bool updated2 = false;
  bool updated3 = false;
  bool updated4 = false;

  if (++counter1 >= UPDATE_SLOW) {
    counter1 = 0;
    duty_a1 += dir_duty_a1 * DUTY_STEP;
    duty_b1 += dir_duty_b1 * DUTY_STEP;
    if (duty_a1 >= DUTY_A_MAX)
      dir_duty_a1 = -1;
    if (duty_a1 <= DUTY_A_MIN)
      dir_duty_a1 = 1;
    if (duty_b1 >= DUTY_B_MAX)
      dir_duty_b1 = -1;
    if (duty_b1 <= DUTY_B_MIN)
      dir_duty_b1 = 1;
    updated1 = true;
  }

  if (++counter2 >= UPDATE_SLOW) {
    counter2 = 0;
    period2 += dir_period2 * PERIOD_STEP_NS;
    if (period2 >= PERIOD_MAX_NS)
      dir_period2 = -1;
    if (period2 <= PERIOD_MIN_NS)
      dir_period2 = 1;
    updated2 = true;
  }

  if (++counter3 >= UPDATE_SLOW) {
    counter3 = 0;
    duty_a3 += dir_duty_a3 * DUTY_STEP;
    duty_b3 += dir_duty_b3 * DUTY_STEP;
    period3 += dir_period3 * PERIOD_STEP_NS;
    if (duty_a3 >= DUTY_A_MAX)
      dir_duty_a3 = -1;
    if (duty_a3 <= DUTY_A_MIN)
      dir_duty_a3 = 1;
    if (duty_b3 >= DUTY_B_MAX)
      dir_duty_b3 = -1;
    if (duty_b3 <= DUTY_B_MIN)
      dir_duty_b3 = 1;
    if (period3 >= PERIOD_MAX_NS)
      dir_period3 = -1;
    if (period3 <= PERIOD_MIN_NS)
      dir_period3 = 1;
    updated3 = true;
  }

  if (++counter4 >= UPDATE_FAST) {
    counter4 = 0;
    duty_a4 = (duty_a4 == DUTY_A_MIN) ? DUTY_A_MAX : DUTY_A_MIN;
    duty_b4 = (duty_b4 == DUTY_B_MIN) ? DUTY_B_MAX : DUTY_B_MIN;
    period4 = (period4 == PERIOD_MIN_NS) ? PERIOD_MAX_NS : PERIOD_MIN_NS;
    trigger_state4 ^= 1;
    GPIO_writePin(TRIGGER_GPIO, trigger_state4);
    updated4 = true;
  }

  if (updated1)
    hal_pwm_update_ab(HAL_PWM1, PERIOD_NS, duty_a1, duty_b1);
  if (updated2)
    hal_pwm_update_ab(HAL_PWM2, period2, 0.6f, 0.2f);
  if (updated3)
    hal_pwm_update_ab(HAL_PWM3, period3, duty_a3, duty_b3);
  if (updated4)
    hal_pwm_update_ab(HAL_PWM4, period4, duty_a4, duty_b4);
}

void main(void) {
  hal_init();

  GPIO_setPinConfig(GPIO_16_GPIO16);
  GPIO_setDirectionMode(TRIGGER_GPIO, GPIO_DIR_MODE_OUT);
  GPIO_setPadConfig(TRIGGER_GPIO, GPIO_PIN_TYPE_STD);
  GPIO_writePin(TRIGGER_GPIO, 0);

  hal_pwm_enable_ab(HAL_PWM1, PERIOD_NS, 0.6f, 0.2f);
  hal_pwm_enable_ab(HAL_PWM2, (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2, 0.6f, 0.2f);
  hal_pwm_enable_ab(HAL_PWM3, (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2, 0.6f, 0.2f);
  hal_pwm_enable_ab(HAL_PWM4, PERIOD_MIN_NS, DUTY_A_MIN, DUTY_B_MIN);

  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();

  while (1) {
  }
}
