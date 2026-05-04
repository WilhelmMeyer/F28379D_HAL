# F28379D HAL

![Status](https://img.shields.io/badge/status-alpha-orange)
![Version](https://img.shields.io/badge/version-v0.1.0-blue)

Biblioteca de Abstração de Hardware para o TMS320F28379D (LAUNCHXL-F28379D).

> English version available: [README.md](README.md)

---

## Status

Esta biblioteca está atualmente em **alpha**. Ela é funcional e ativamente utilizada em pesquisa, mas a API ainda não é estável. Mudanças que quebram compatibilidade podem ocorrer entre versões.

Versão atual: `v0.1.0`

Para verificar qual versão você está utilizando, compare a definição `HAL_VERSION` no arquivo `hal.h` com a versão mais recente disponível no GitHub.

---

## Visão Geral

F28379D HAL é uma biblioteca de código aberto que simplifica o uso dos periféricos ePWM no microcontrolador TMS320F28379D. Ela fornece uma API para configurar e atualizar saídas PWM em tempo real, desenvolvida para aplicações de pesquisa em eletrônica de potência.

Esta biblioteca está em desenvolvimento ativo no LPEE (Laboratório de Processamento Eletrônico de Energia).

---

## Requisitos

- Hardware: LAUNCHXL-F28379D
- IDE: Code Composer Studio 20.4
- SDK: C2000Ware 6.00.01.00 instalado em `C:/ti/`

---

## Estrutura do Repositório

    F28379D_HAL/
    ├── hal_lib/        — arquivos fonte da biblioteca
    ├── device/         — arquivos de suporte ao dispositivo TI
    ├── examples/       — exemplos de uso
    └── hal.h           — include principal

---

## Uso Básico

Inclua `hal.h` no seu projeto e implemente `hal_task`:

```c
#include "hal.h"

#define TASK_PERIOD_NS  50000   // 20kHz
#define PWM_PERIOD_NS   20000   // 50kHz

uint32_t period_ns = PWM_PERIOD_NS;
float    duty      = 0.5f;

void hal_task(void)
{
    hal_pwm_update_a(HAL_PWM1, period_ns, duty);
}

void main(void)
{
    hal_init();
    hal_pwm_enable_a(HAL_PWM1, PWM_PERIOD_NS, 0.5f);
    hal_set_task_period(TASK_PERIOD_NS);
    hal_start();
    while (1) {}
}
```

`hal_task` é executada periodicamente pela biblioteca na frequência definida por `TASK_PERIOD_NS`. Os parâmetros do PWM devem ser atualizados dentro desta função.

---

## Métodos Disponíveis

### Inicialização

| Método | Descrição |
|--------|-----------|
| `hal_init()` | Inicializa o dispositivo |
| `hal_set_task_period(period_ns)` | Define o período de execução da task em nanosegundos |
| `hal_start()` | Inicia o temporizador da task e habilita as interrupções |

### PWM

| Método | Descrição |
|--------|-----------|
| `hal_pwm_enable_a(module, period_ns, duty)` | Habilita o canal A |
| `hal_pwm_enable_b(module, period_ns, duty)` | Habilita o canal B |
| `hal_pwm_enable_ab(module, period_ns, duty_a, duty_b)` | Habilita os dois canais com duties independentes |
| `hal_pwm_enable_complementary(module, period_ns, duty_a, offset_ns)` | Habilita par complementar — offset positivo = banda morta, negativo = sobreposição |
| `hal_pwm_enable_phase_shift(module, period_ns, phase, duty_a, duty_b)` | Habilita defasagem entre os canais A e B |
| `hal_pwm_enable_phase_shift_trailing(module, period_ns, phase, duty_a, duty_b, offset_ns)` | Defasagem referenciada à borda de descida do canal A |
| `hal_pwm_update_a(module, period_ns, duty)` | Atualiza o canal A |
| `hal_pwm_update_b(module, period_ns, duty)` | Atualiza o canal B |
| `hal_pwm_update_ab(module, period_ns, duty_a, duty_b)` | Atualiza os dois canais |
| `hal_pwm_update_complementary(module, period_ns, duty_a, offset_ns)` | Atualiza o par complementar |
| `hal_pwm_update_phase_shift(module, period_ns, phase, duty_a, duty_b)` | Atualiza a defasagem |
| `hal_pwm_update_phase_shift_trailing(module, period_ns, phase, duty_a, duty_b, offset_ns)` | Atualiza a defasagem trailing |

---

## Exemplos

Os exemplos estão disponíveis na pasta `examples/`.

| Arquivo | Descrição |
|---------|-----------|
| `example_pwm_duty.c` | Canal A com duty cycle variável e frequência fixa |
| `example_pwm_frequency.c` | Canal B com frequência variável e duty cycle fixo |
| `example_pwm_duty_frequency.c` | Canais A e B com duty cycles independentes e frequência variável |
| `example_complementary.c` | Par complementar com duty cycle, frequência e offset variáveis — offset positivo aplica banda morta, negativo aplica sobreposição |
| `example_phase_shift.c` | Defasagem entre canais A e B com fase, duty cycle e frequência variáveis |
| `example_phase_shift_trailing.c` | Defasagem referenciada à borda de descida do canal A com fase, duty cycle, frequência e offset variáveis |

---

## Licença

MIT License