#include "hal_epwm.h"
#include "driverlib.h"

extern void hal_task(void);

__interrupt void hal_epwm12_isr(void);

typedef struct {
  uint32_t base;
  SysCtl_PeripheralPCLOCKCR clk_peripheral;
  uint16_t gpio_a;
  uint16_t gpio_b;
  uint32_t pin_config_a;
  uint32_t pin_config_b;
} epwm_mapping_t;

static const epwm_mapping_t epwm_map[] = {
    {EPWM1_BASE, SYSCTL_PERIPH_CLK_EPWM1, 0, 1, GPIO_0_EPWM1A, GPIO_1_EPWM1B},
    {EPWM2_BASE, SYSCTL_PERIPH_CLK_EPWM2, 2, 3, GPIO_2_EPWM2A, GPIO_3_EPWM2B},
    {EPWM3_BASE, SYSCTL_PERIPH_CLK_EPWM3, 4, 5, GPIO_4_EPWM3A, GPIO_5_EPWM3B},
    {EPWM4_BASE, SYSCTL_PERIPH_CLK_EPWM4, 6, 7, GPIO_6_EPWM4A, GPIO_7_EPWM4B},
    {EPWM5_BASE, SYSCTL_PERIPH_CLK_EPWM5, 8, 9, GPIO_8_EPWM5A, GPIO_9_EPWM5B},
    {EPWM6_BASE, SYSCTL_PERIPH_CLK_EPWM6, 10, 11, GPIO_10_EPWM6A, GPIO_11_EPWM6B},
};

static void hal_epwm_configure_gpio(uint16_t gpio_num, uint32_t pin_config) {
  GPIO_setPinConfig(pin_config);
  GPIO_setDirectionMode(gpio_num, GPIO_DIR_MODE_OUT);
  GPIO_setPadConfig(gpio_num, GPIO_PIN_TYPE_STD);
}

static bool epwm_pending_latch[6] = {false};

static void hal_epwm_configure_timebase(uint32_t base, uint16_t period) {
  EPWM_setTimeBasePeriod(base, period);
  EPWM_setTimeBaseCounter(base, 0);
  EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP);
  EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
  EPWM_disablePhaseShiftLoad(base);
  EPWM_setPhaseShift(base, 0);
}

void hal_epwm_configure_task(uint32_t period_ns) {
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);

  SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
  SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM12);

  hal_epwm_configure_timebase(EPWM12_BASE, period);

  EPWM_setInterruptSource(EPWM12_BASE, EPWM_INT_TBCTR_ZERO);
  EPWM_setInterruptEventCount(EPWM12_BASE, 1);
  EPWM_enableInterrupt(EPWM12_BASE);

  Interrupt_register(INT_EPWM12, &hal_epwm12_isr);
  Interrupt_enable(INT_EPWM12);

  SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

__interrupt void hal_epwm12_isr(void) {
  hal_task();

  for (int i = 0; i < 6; i++) {
    if (epwm_pending_latch[i]) {
      EPWM_setGlobalLoadOneShotLatch(epwm_map[i].base);
      epwm_pending_latch[i] = false;
    }
  }

  EPWM_clearEventTriggerInterruptFlag(EPWM12_BASE);
  Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

static void hal_epwm_clear_aq_a(uint32_t base) {
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
}

static void hal_epwm_clear_aq_b(uint32_t base) {
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
}

static void hal_epwm_init_module(hal_pwm_module_t module, uint16_t period, hal_pwm_channel_t channels) {
  const epwm_mapping_t *map = &epwm_map[module];

  SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
  SysCtl_enablePeripheral(map->clk_peripheral);

  if (channels & HAL_CHANNEL_A)
    hal_epwm_configure_gpio(map->gpio_a, map->pin_config_a);

  if (channels & HAL_CHANNEL_B)
    hal_epwm_configure_gpio(map->gpio_b, map->pin_config_b);

  hal_epwm_configure_timebase(map->base, period);

  EPWM_setCounterCompareShadowLoadMode(map->base, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_PERIOD);
  EPWM_setCounterCompareShadowLoadMode(map->base, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_PERIOD);

  SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void hal_pwm_enable_a(hal_pwm_module_t module, uint32_t period_ns, float duty) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  uint16_t ticks = (uint16_t)(duty * (float)(period + 1));

  hal_epwm_init_module(module, period, HAL_CHANNEL_A);
  hal_epwm_clear_aq_a(base);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

  EPWM_enableGlobalLoad(base);
  EPWM_setGlobalLoadTrigger(base, EPWM_GL_LOAD_PULSE_CNTR_PERIOD);
  EPWM_enableGlobalLoadOneShotMode(base);
  EPWM_enableGlobalLoadRegisters(base, EPWM_GL_REGISTER_TBPRD_TBPRDHR | EPWM_GL_REGISTER_CMPA_CMPAHR);

  EPWM_setGlobalLoadOneShotLatch(base);
}

void hal_pwm_enable_b(hal_pwm_module_t module, uint32_t period_ns, float duty) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  uint16_t ticks = (uint16_t)(duty * (float)(period + 1));

  hal_epwm_init_module(module, period, HAL_CHANNEL_B);
  hal_epwm_clear_aq_b(base);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

  EPWM_enableGlobalLoad(base);
  EPWM_setGlobalLoadTrigger(base, EPWM_GL_LOAD_PULSE_CNTR_PERIOD);
  EPWM_enableGlobalLoadOneShotMode(base);
  EPWM_enableGlobalLoadRegisters(base, EPWM_GL_REGISTER_TBPRD_TBPRDHR | EPWM_GL_REGISTER_CMPB_CMPBHR);

  EPWM_setGlobalLoadOneShotLatch(base);
}

void hal_pwm_update_a(hal_pwm_module_t module, uint32_t period_ns, float duty) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, (uint16_t)(duty * (float)(period + 1)));

  epwm_pending_latch[module] = true;
}

void hal_pwm_update_b(hal_pwm_module_t module, uint32_t period_ns, float duty) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, (uint16_t)(duty * (float)(period + 1)));

  epwm_pending_latch[module] = true;
}

void hal_pwm_enable_ab(hal_pwm_module_t module, uint32_t period_ns, float duty_a, float duty_b) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1));
  uint16_t ticks_cmpb = (uint16_t)(duty_b * (float)(period + 1));

  hal_epwm_init_module(module, period, HAL_CHANNEL_AB);
  hal_epwm_clear_aq_a(base);
  hal_epwm_clear_aq_b(base);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);

  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

  EPWM_enableGlobalLoad(base);
  EPWM_setGlobalLoadTrigger(base, EPWM_GL_LOAD_PULSE_CNTR_PERIOD);
  EPWM_enableGlobalLoadOneShotMode(base);
  EPWM_enableGlobalLoadRegisters(base, EPWM_GL_REGISTER_TBPRD_TBPRDHR | EPWM_GL_REGISTER_CMPA_CMPAHR |
                                           EPWM_GL_REGISTER_CMPB_CMPBHR);

  EPWM_setGlobalLoadOneShotLatch(base);
}

void hal_pwm_update_ab(hal_pwm_module_t module, uint32_t period_ns, float duty_a, float duty_b) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, (uint16_t)(duty_a * (float)(period + 1)));
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, (uint16_t)(duty_b * (float)(period + 1)));

  epwm_pending_latch[module] = true;
}

void hal_pwm_enable_complementary(hal_pwm_module_t module, uint32_t period_ns, float duty_a, int32_t offset_ns) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  int16_t ticks_off = (int16_t)(offset_ns / HAL_NS_PER_TICK);

  uint16_t ticks_red = (uint16_t)(ticks_off < 0 ? 0 : ticks_off);
  uint16_t ticks_fed = (uint16_t)(ticks_off < 0 ? -ticks_off : 0);
  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1)) + ticks_fed;
  uint16_t ticks_cmpb = (uint16_t)(duty_a * (float)(period + 1)) + ticks_red;

  hal_epwm_init_module(module, period, HAL_CHANNEL_AB);
  hal_epwm_clear_aq_a(base);
  hal_epwm_clear_aq_b(base);

  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);

  EPWM_setRisingEdgeDelayCountShadowLoadMode(base, EPWM_RED_LOAD_ON_CNTR_PERIOD);
  EPWM_setFallingEdgeDelayCountShadowLoadMode(base, EPWM_FED_LOAD_ON_CNTR_PERIOD);

  EPWM_setRisingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);
  EPWM_setFallingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMB);
  EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
  EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_HIGH);
  EPWM_setDeadBandDelayMode(base, EPWM_DB_RED, true);
  EPWM_setDeadBandDelayMode(base, EPWM_DB_FED, true);
  EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_A, false);
  EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_B, false);

  EPWM_setFallingEdgeDelayCount(base, ticks_fed);
  EPWM_setRisingEdgeDelayCount(base, ticks_red);

  EPWM_enableGlobalLoad(base);
  EPWM_setGlobalLoadTrigger(base, EPWM_GL_LOAD_PULSE_CNTR_PERIOD);
  EPWM_enableGlobalLoadOneShotMode(base);
  EPWM_enableGlobalLoadRegisters(base, EPWM_GL_REGISTER_TBPRD_TBPRDHR | EPWM_GL_REGISTER_CMPA_CMPAHR |
                                           EPWM_GL_REGISTER_CMPB_CMPBHR | EPWM_GL_REGISTER_DBRED_DBREDHR |
                                           EPWM_GL_REGISTER_DBFED_DBFEDHR);

  EPWM_setGlobalLoadOneShotLatch(base);
}

void hal_pwm_update_complementary(hal_pwm_module_t module, uint32_t period_ns, float duty_a, int32_t offset_ns) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  int16_t ticks_off = (int16_t)(offset_ns / (int32_t)HAL_NS_PER_TICK);

  uint16_t ticks_red = (uint16_t)(ticks_off < 0 ? 0 : ticks_off);
  uint16_t ticks_fed = (uint16_t)(ticks_off < 0 ? -ticks_off : 0);
  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1)) + ticks_fed;
  uint16_t ticks_cmpb = (uint16_t)(duty_a * (float)(period + 1)) + ticks_red;

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);

  EPWM_setFallingEdgeDelayCount(base, ticks_fed);
  EPWM_setRisingEdgeDelayCount(base, ticks_red);

  epwm_pending_latch[module] = true;
}

void hal_pwm_enable_phase_shift(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a, float duty_b) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);

  hal_epwm_init_module(module, period, HAL_CHANNEL_AB);
  hal_epwm_clear_aq_a(base);
  hal_epwm_clear_aq_b(base);

  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1));
  uint16_t ticks_red = (uint16_t)(phase * (float)(period + 1));

  float cmpb_value = phase + duty_b;
  uint16_t ticks_cmpb;
  uint16_t ticks_fed;

  if (cmpb_value <= 1.0f) {
    ticks_cmpb = (uint16_t)(cmpb_value * (float)(period + 1));
    ticks_fed = 0;
  } else {
    ticks_cmpb = period;
    ticks_fed = (uint16_t)((cmpb_value - 1.0f) * (float)(period + 1));
  }

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);

  EPWM_setRisingEdgeDelayCountShadowLoadMode(base, EPWM_RED_LOAD_ON_CNTR_PERIOD);
  EPWM_setFallingEdgeDelayCountShadowLoadMode(base, EPWM_FED_LOAD_ON_CNTR_PERIOD);

  EPWM_setRisingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMB);
  EPWM_setFallingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_DB_RED);
  EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
  EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_HIGH);
  EPWM_setDeadBandDelayMode(base, EPWM_DB_RED, false);
  EPWM_setDeadBandDelayMode(base, EPWM_DB_FED, true);
  EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_A, false);
  EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_B, false);

  EPWM_setRisingEdgeDelayCount(base, ticks_red);
  EPWM_setFallingEdgeDelayCount(base, ticks_fed);

  EPWM_enableGlobalLoad(base);
  EPWM_setGlobalLoadTrigger(base, EPWM_GL_LOAD_PULSE_CNTR_PERIOD);
  EPWM_enableGlobalLoadOneShotMode(base);
  EPWM_enableGlobalLoadRegisters(base, EPWM_GL_REGISTER_TBPRD_TBPRDHR | EPWM_GL_REGISTER_CMPA_CMPAHR |
                                           EPWM_GL_REGISTER_CMPB_CMPBHR | EPWM_GL_REGISTER_DBRED_DBREDHR |
                                           EPWM_GL_REGISTER_DBFED_DBFEDHR);

  EPWM_setGlobalLoadOneShotLatch(base);
}

void hal_pwm_update_phase_shift(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a, float duty_b) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);

  float cmpb_value = phase + duty_b;
  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1));
  uint16_t ticks_red = (uint16_t)(phase * (float)(period + 1));
  uint16_t ticks_cmpb;
  uint16_t ticks_fed;

  if (cmpb_value <= 1.0f) {
    ticks_cmpb = (uint16_t)(cmpb_value * (float)(period + 1));
    ticks_fed = 0;
  } else {
    ticks_cmpb = period;
    ticks_fed = (uint16_t)((cmpb_value - 1.0f) * (float)(period + 1));
  }

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);
  EPWM_setRisingEdgeDelayCount(base, ticks_red);
  EPWM_setFallingEdgeDelayCount(base, ticks_fed);

  epwm_pending_latch[module] = true;
}

void hal_pwm_enable_phase_shift_trailing(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a,
                                         float duty_b, uint32_t offset_ns) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  uint16_t ticks_off = (uint16_t)(offset_ns / HAL_NS_PER_TICK);
  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1)) + ticks_off;
  uint16_t ticks_red = (uint16_t)(phase * (float)(period + 1)) + ticks_cmpa;
  uint16_t ticks_duty_b = (uint16_t)(duty_b * (float)(period + 1));
  uint16_t ticks_cmpb = ticks_red + ticks_duty_b;

  // Limita CMPB para garantir espaço para o FED
  uint16_t cmpb_max = period - ticks_off;
  if (ticks_cmpb > cmpb_max)
    ticks_cmpb = cmpb_max;

  hal_epwm_init_module(module, period, HAL_CHANNEL_AB);
  hal_epwm_clear_aq_a(base);
  hal_epwm_clear_aq_b(base);

  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
  EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);

  EPWM_setRisingEdgeDelayCountShadowLoadMode(base, EPWM_RED_LOAD_ON_CNTR_PERIOD);
  EPWM_setFallingEdgeDelayCountShadowLoadMode(base, EPWM_FED_LOAD_ON_CNTR_PERIOD);

  EPWM_setRisingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMB);
  EPWM_setFallingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);
  EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
  EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
  EPWM_setDeadBandDelayMode(base, EPWM_DB_RED, true);
  EPWM_setDeadBandDelayMode(base, EPWM_DB_FED, true);
  EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_A, true);
  EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_B, true);

  EPWM_setRisingEdgeDelayCount(base, ticks_off);
  EPWM_setFallingEdgeDelayCount(base, ticks_off);

  EPWM_enableGlobalLoad(base);
  EPWM_setGlobalLoadTrigger(base, EPWM_GL_LOAD_PULSE_CNTR_PERIOD);
  EPWM_enableGlobalLoadOneShotMode(base);
  EPWM_enableGlobalLoadRegisters(base, EPWM_GL_REGISTER_TBPRD_TBPRDHR | EPWM_GL_REGISTER_CMPA_CMPAHR |
                                           EPWM_GL_REGISTER_CMPB_CMPBHR | EPWM_GL_REGISTER_DBRED_DBREDHR |
                                           EPWM_GL_REGISTER_DBFED_DBFEDHR);

  EPWM_setGlobalLoadOneShotLatch(base);
}

void hal_pwm_update_phase_shift_trailing(hal_pwm_module_t module, uint32_t period_ns, float phase, float duty_a,
                                         float duty_b, uint32_t offset_ns) {
  uint32_t base = epwm_map[module].base;
  uint16_t period = (uint16_t)(period_ns / HAL_NS_PER_TICK);
  uint16_t ticks_off = (uint16_t)(offset_ns / HAL_NS_PER_TICK);
  uint16_t ticks_cmpa = (uint16_t)(duty_a * (float)(period + 1)) + ticks_off;
  uint16_t ticks_red = (uint16_t)(phase * (float)(period + 1)) + ticks_cmpa;
  uint16_t ticks_duty_b = (uint16_t)(duty_b * (float)(period + 1));
  uint16_t ticks_cmpb = ticks_red + ticks_duty_b;

  uint16_t cmpb_max = period - ticks_off;
  if (ticks_cmpb > cmpb_max)
    ticks_cmpb = cmpb_max;

  EPWM_setTimeBasePeriod(base, period);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, ticks_cmpa);
  EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, ticks_cmpb);
  EPWM_setRisingEdgeDelayCount(base, ticks_off);
  EPWM_setFallingEdgeDelayCount(base, ticks_off);

  epwm_pending_latch[module] = true;
}
