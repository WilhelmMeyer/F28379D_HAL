# F28379D HAL

Hardware Abstraction Library for the TMS320F28379D (LAUNCHXL-F28379D).

> Portuguese version available: [README.pt.md](README.pt.md)

---

## Overview

F28379D HAL is an open-source library that simplifies the use of ePWM peripherals on the TMS320F28379D microcontroller. It provides a clean API for configuring and updating PWM outputs in real time, designed for power electronics research applications.

This library is under active development at LPEE (Laboratório de Processamento Eletrônico de Energia).

---

## Requirements

- Hardware: LAUNCHXL-F28379D
- IDE: Code Composer Studio 20.4
- SDK: C2000Ware 6.00.01.00 installed at `C:/ti/`

---

## Repository Structure

    F28379D_HAL/
    ├── hal_lib/        — library source files
    ├── device/         — TI device support files
    ├── examples/       — usage examples
    └── hal.h           — main include

---

## Basic Usage

Include `hal.h` in your project and implement `hal_task`:

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

`hal_task` is executed periodically by the library at the frequency defined by `TASK_PERIOD_NS`. PWM parameters should be updated inside this function.

---

## Available Methods

### Initialization

| Method | Description |
|--------|-------------|
| `hal_init()` | Initializes the device |
| `hal_set_task_period(period_ns)` | Sets the task execution period in nanoseconds |
| `hal_start()` | Starts the task timer and enables interrupts |

### PWM

| Method | Description |
|--------|-------------|
| `hal_pwm_enable_a(module, period_ns, duty)` | Enables channel A |
| `hal_pwm_enable_b(module, period_ns, duty)` | Enables channel B |
| `hal_pwm_enable_ab(module, period_ns, duty_a, duty_b)` | Enables both channels independently |
| `hal_pwm_enable_complementary(module, period_ns, duty_a, offset_ns)` | Enables complementary pair — positive offset = deadband, negative = overlap |
| `hal_pwm_enable_phase_shift(module, period_ns, phase, duty_a, duty_b)` | Enables phase shift between channels |
| `hal_pwm_enable_phase_shift_trailing(module, period_ns, phase, duty_a, duty_b, offset_ns)` | Phase shift referenced to the trailing edge of channel A |
| `hal_pwm_update_a(module, period_ns, duty)` | Updates channel A |
| `hal_pwm_update_b(module, period_ns, duty)` | Updates channel B |
| `hal_pwm_update_ab(module, period_ns, duty_a, duty_b)` | Updates both channels |
| `hal_pwm_update_complementary(module, period_ns, duty_a, offset_ns)` | Updates complementary pair |
| `hal_pwm_update_phase_shift(module, period_ns, phase, duty_a, duty_b)` | Updates phase shift |
| `hal_pwm_update_phase_shift_trailing(module, period_ns, phase, duty_a, duty_b, offset_ns)` | Updates trailing phase shift |

---

## Examples

Examples are available in the `examples/` folder.

| File | Description |
|------|-------------|
| `example_pwm_duty.c` | Channel A with variable duty cycle and fixed frequency |
| `example_pwm_frequency.c` | Channel B with variable frequency and fixed duty cycle |
| `example_pwm_duty_frequency.c` | Channels A and B with independent duty cycles and variable frequency |
| `example_complementary.c` | Complementary pair with variable duty cycle, frequency and offset — positive offset applies deadband, negative applies overlap |
| `example_phase_shift.c` | Phase shift between channels A and B with variable phase, duty cycle and frequency |
| `example_phase_shift_trailing.c` | Phase shift referenced to the trailing edge of channel A with variable phase, duty cycle, frequency and offset |

---

## License

MIT License