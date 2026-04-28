#include "hal.h"

#define TASK_PERIOD_NS 50000 // 20kHz

#define PERIOD_NS 20000 // 50kHz fixo
#define PERIOD_MIN_NS 10000
#define PERIOD_MAX_NS 50000
#define PERIOD_STEP_NS 50

#define DUTY_MIN 0.1f
#define DUTY_MAX 0.9f
#define DUTY_STEP 0.001f

#define OFFSET_MIN_NS -500
#define OFFSET_MAX_NS 500
#define OFFSET_STEP_NS 5

#define UPDATE_SLOW 200
#define UPDATE_FAST 2000

#define TRIGGER_GPIO 16
#define TRIGGER_GPIO2 24

// Etapas do teste brusco de offset
static const int32_t offset_steps[] = {1500, -1500};
// static const int32_t offset_steps[] = {500, 200, 0, -200, -500};
#define OFFSET_STEPS_COUNT 2

void hal_task(void) {
  // PWM1 — duty varia suavemente, offset fixo
  static float duty1 = 0.5f;
  static int16_t dir_duty1 = 1;
  static uint16_t counter1 = 0;

  // PWM2 — período varia suavemente, offset fixo
  static uint32_t period2 = (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2;
  static int16_t dir_period2 = 1;
  static uint16_t counter2 = 0;

  // PWM3 — duty, período e offset variam suavemente
  static float duty3 = 0.5f;
  static int16_t dir_duty3 = 1;
  static uint32_t period3 = (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2;
  static int16_t dir_period3 = 1;
  static int32_t offset3 = 0;
  static int16_t dir_offset3 = 1;
  static uint16_t counter3 = 0;

  // PWM4 — offset varia suavemente
  static int32_t offset4 = OFFSET_MIN_NS;
  static int16_t dir_offset4 = 1;
  static uint16_t counter4 = 0;

  // PWM5 — offset varia bruscamente por etapas
  static uint8_t step5 = 0;
  static uint16_t counter5 = 0;
  static uint16_t trigger_state5 = 0;

  // PWM6 — variação brusca período e duty
  static float duty6 = DUTY_MIN;
  static uint32_t period6 = PERIOD_MIN_NS;
  static int32_t offset6 = OFFSET_MIN_NS;
  static uint16_t counter6 = 0;
  static uint16_t trigger_state6 = 0;

  bool updated1 = false;
  bool updated2 = false;
  bool updated3 = false;
  bool updated4 = false;
  bool updated5 = false;
  bool updated6 = false;

  if (++counter1 >= UPDATE_SLOW) {
    counter1 = 0;
    duty1 += dir_duty1 * DUTY_STEP;
    if (duty1 >= DUTY_MAX)
      dir_duty1 = -1;
    if (duty1 <= DUTY_MIN)
      dir_duty1 = 1;
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
    duty3 += dir_duty3 * DUTY_STEP;
    period3 += dir_period3 * PERIOD_STEP_NS;
    offset3 += dir_offset3 * OFFSET_STEP_NS;
    if (duty3 >= DUTY_MAX)
      dir_duty3 = -1;
    if (duty3 <= DUTY_MIN)
      dir_duty3 = 1;
    if (period3 >= PERIOD_MAX_NS)
      dir_period3 = -1;
    if (period3 <= PERIOD_MIN_NS)
      dir_period3 = 1;
    if (offset3 >= OFFSET_MAX_NS)
      dir_offset3 = -1;
    if (offset3 <= OFFSET_MIN_NS)
      dir_offset3 = 1;
    updated3 = true;
  }

  if (++counter4 >= UPDATE_SLOW) {
    counter4 = 0;
    offset4 += dir_offset4 * OFFSET_STEP_NS;
    if (offset4 >= OFFSET_MAX_NS)
      dir_offset4 = -1;
    if (offset4 <= OFFSET_MIN_NS)
      dir_offset4 = 1;
    updated4 = true;
  }

  if (++counter5 >= UPDATE_FAST) {
    counter5 = 0;
    step5 = (step5 + 1) % OFFSET_STEPS_COUNT;
    trigger_state5 ^= 1;
    GPIO_writePin(TRIGGER_GPIO, trigger_state5);
    updated5 = true;
  }

  if (++counter6 >= UPDATE_FAST) {
    counter6 = 0;
    duty6 = (duty6 == DUTY_MIN) ? DUTY_MAX : DUTY_MIN;
    period6 = (period6 == PERIOD_MIN_NS) ? PERIOD_MAX_NS : PERIOD_MIN_NS;
    offset6 = (offset6 == OFFSET_MIN_NS) ? OFFSET_MAX_NS : OFFSET_MIN_NS;
    trigger_state6 ^= 1;
    GPIO_writePin(TRIGGER_GPIO2, trigger_state6);
    updated6 = true;
  }

  // if (updated1)
  //   hal_pwm_update_complementary(HAL_PWM1, PERIOD_NS, duty1, -200);
  // if (updated2)
  //   hal_pwm_update_complementary(HAL_PWM2, period2, 0.5f, -200);
  // if (updated3)
  //   hal_pwm_update_complementary(HAL_PWM3, period3, duty3, offset3);
  // if (updated4)
  //   hal_pwm_update_complementary(HAL_PWM4, PERIOD_NS, 0.5f, offset4);
  if (updated5)
    {}
    // hal_pwm_update_complementary(HAL_PWM5, PERIOD_NS, 0.5f, offset_steps[step5]);
  // if (updated6)
  //   hal_pwm_update_complementary(HAL_PWM6, period6, duty6, offset6);
}

void main(void) {
  hal_init();

  GPIO_setPinConfig(GPIO_16_GPIO16);
  GPIO_setDirectionMode(TRIGGER_GPIO, GPIO_DIR_MODE_OUT);
  GPIO_setPadConfig(TRIGGER_GPIO, GPIO_PIN_TYPE_STD);
  GPIO_writePin(TRIGGER_GPIO, 0);

  GPIO_setPinConfig(GPIO_24_GPIO24);
  GPIO_setDirectionMode(TRIGGER_GPIO2, GPIO_DIR_MODE_OUT);
  GPIO_setPadConfig(TRIGGER_GPIO2, GPIO_PIN_TYPE_STD);
  GPIO_writePin(TRIGGER_GPIO2, 0);

  // hal_pwm_enable_complementary(HAL_PWM1, PERIOD_NS, 0.5f, 200);
  // hal_pwm_enable_complementary(HAL_PWM2, (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2, 0.5f, 200);
  // hal_pwm_enable_complementary(HAL_PWM3, (PERIOD_MIN_NS + PERIOD_MAX_NS) / 2, 0.5f, 0);
  // hal_pwm_enable_complementary(HAL_PWM4, PERIOD_NS, 0.5f, OFFSET_MIN_NS);
  hal_pwm_enable_complementary(HAL_PWM5, PERIOD_NS, 0.5f, offset_steps[0]);

  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();

  while (1) {
  }
}
