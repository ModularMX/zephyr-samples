# INPUT Driver Examples

This directory contains examples for Zephyr's INPUT subsystem, demonstrating various input device drivers and pseudo-devices.

## Current Examples

- ✅ **_01_basic_button** - Basic button input with `gpio-keys` driver
- ✅ **_02_long_press_button** - Short/long press detection with `zephyr,input-longpress`
- ✅ **_03_double_tap_button** - Double tap detection with `zephyr,input-double-tap`

## Potential Future Examples

### ADC-Based Input
- **_04_adc_keys** - Multiple buttons on a single ADC pin using resistor ladder
  - Use case: Save GPIO pins by using voltage dividers
  - Driver: `adc-keys`
  - Detects individual and combination button presses

### Matrix Input
- **_05_keypad_matrix** - Keyboard matrix scanning (rows × columns)
  - Use case: Numeric keypads, small keyboards
  - Driver: `gpio-kbd-matrix`
  - Example: 4×4 keypad (16 keys using 8 GPIO pins)

### Analog Input
- **_06_joystick** - Analog joystick with 2-axis control
  - Use case: Game controllers, navigation controls
  - Driver: `analog-axis`
  - Features: Deadzone configuration, calibration, absolute axis events

### Advanced Combinations
- **_07_multi_function_button** - Combine all pseudo-devices on one button
  - Single tap, double tap, AND long press on the same button
  - Demonstrates how to stack multiple input processors

## Resources

- [Zephyr INPUT Subsystem Documentation](https://docs.zephyrproject.org/latest/hardware/peripherals/input.html)
- [Device Tree Bindings](../../../zephyr/dts/bindings/input/)
