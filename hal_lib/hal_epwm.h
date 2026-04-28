#ifndef HAL_EPWM_H
#define HAL_EPWM_H

#include "device.h"
#include <stdbool.h>
#include <stdint.h>

#define HAL_NS_PER_TICK (1000000000UL / (DEVICE_SYSCLK_FREQ / 2UL))

typedef enum { HAL_PWM1 = 0, HAL_PWM2, HAL_PWM3, HAL_PWM4, HAL_PWM5, HAL_PWM6 } hal_pwm_module_t;

typedef enum { HAL_CHANNEL_A = 0x01, HAL_CHANNEL_B = 0x02, HAL_CHANNEL_AB = 0x03 } hal_pwm_channel_t;

void hal_epwm_configure_task(uint32_t period_ns);

void hal_pwm_enable_a(hal_pwm_module_t module, uint32_t period_ns, float duty);
void hal_pwm_enable_b(hal_pwm_module_t module, uint32_t period_ns, float duty);
void hal_pwm_enable_ab(hal_pwm_module_t module, uint32_t period_ns, float duty_a, float duty_b);
void hal_pwm_enable_complementary(hal_pwm_module_t module, uint32_t period_ns, float duty_a, int32_t offset_ns);
void hal_pwm_enable_phase_shift(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a, float duty_b);
void hal_pwm_enable_phase_shift_trailing(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a,
                                         float duty_b, uint32_t offset_ns);

void hal_pwm_update_a(hal_pwm_module_t module, uint32_t period_ns, float duty);
void hal_pwm_update_b(hal_pwm_module_t module, uint32_t period_ns, float duty);
void hal_pwm_update_ab(hal_pwm_module_t module, uint32_t period_ns, float duty_a, float duty_b);
void hal_pwm_update_complementary(hal_pwm_module_t module, uint32_t period_ns, float duty_a, int32_t offset_ns);
void hal_pwm_update_phase_shift(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a, float duty_b);
void hal_pwm_update_phase_shift_trailing(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a,
                                         float duty_b, uint32_t offset_ns);

#endif
