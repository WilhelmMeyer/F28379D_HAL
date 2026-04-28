#include "hal.h"

#define TRIGGER_GPIO 2
#define TASK_PERIOD_NS 50000

#define DUTY_A 0.5f
#define DUTY_B 0.4f
#define PHASE_MIN 0.1f
#define PHASE_MAX 0.9f
#define PHASE_STEP 0.001f

#define PERIOD_LOW_NS 20000
#define PERIOD_HIGH_NS 50000
#define PERIOD_HOLD 2000

#define UPDATE_INTERVAL 100

void hal_task(void)
{
    static float     phase          = PHASE_MIN;
    static int16_t   dir_phase      = 1;
    static uint16_t  counter_phase  = 0;

    static uint32_t  period_ns      = PERIOD_LOW_NS;
    static uint16_t  counter_period = 0;
    static uint16_t  trigger_state  = 0;

    if (++counter_phase >= UPDATE_INTERVAL) {
        counter_phase = 0;

        phase += dir_phase * PHASE_STEP;
        if (phase >= PHASE_MAX) dir_phase = -1;
        if (phase <= PHASE_MIN) dir_phase =  1;

        hal_pwm_update_phase_shift(HAL_PWM1, period_ns, phase, DUTY_A, DUTY_B);
    }

    if (++counter_period >= PERIOD_HOLD) {
        counter_period = 0;

        period_ns = (period_ns == PERIOD_LOW_NS) ? PERIOD_HIGH_NS : PERIOD_LOW_NS;

        trigger_state ^= 1;
        GPIO_writePin(TRIGGER_GPIO, trigger_state);

        hal_pwm_update_phase_shift(HAL_PWM1, period_ns, phase, DUTY_A, DUTY_B);
    }
}

void main(void) {
  hal_init();

  GPIO_setPinConfig(GPIO_2_GPIO2);
  GPIO_setDirectionMode(TRIGGER_GPIO, GPIO_DIR_MODE_OUT);
  GPIO_setPadConfig(TRIGGER_GPIO, GPIO_PIN_TYPE_STD);
  GPIO_writePin(TRIGGER_GPIO, 0);

  hal_pwm_enable_phase_shift(HAL_PWM1, PERIOD_LOW_NS, PHASE_MIN, DUTY_A, DUTY_B);

  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();

  while (1) {
  }
}
