/*
    example_pwm_single.c

    Exemplo de uso do módulo ePWM em modo canal único.

    Hardware: LAUNCHXL-F28379D
    Biblioteca: F28379D_HAL

    Saída: PWM1A (P0)

    Períodos configurados em nanosegundos.

    hal_task é executada periodicamente pela biblioteca, com a frequência
    definida por TASK_PERIOD_NS. É nessa função que incluímos lógica de controle
    e atualização de parâmetros do PWM.
*/

#include "hal.h"

#define TASK_PERIOD_NS 50000 // 20kHz
#define PWM_PERIOD_NS 20000  // 50kHz
#define PWM_DUTY 0.5f

uint32_t period_ns = PWM_PERIOD_NS;
float duty = PWM_DUTY;

// Atualiza os parâmetros do PWM a cada ciclo da task.
// Modifique period_ns e duty para alterar frequência e duty cycle em tempo real.
void hal_task(void) {
  hal_pwm_update_a(HAL_PWM1, period_ns, duty);
}

void main(void) {
  hal_init();

  hal_pwm_enable_a(HAL_PWM1, PWM_PERIOD_NS, PWM_DUTY);

  hal_set_task_period(TASK_PERIOD_NS);
  hal_start();

  while (1) {
  }
}
