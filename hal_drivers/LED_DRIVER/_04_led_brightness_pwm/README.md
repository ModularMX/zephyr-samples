# LED Brightness Control - PWM Fade Effect

This example demonstrates **PWM brightness control** to create a smooth LED fading effect from 0% to 100% and back.

## Overview

This demo uses the **same PWM LED hardware configuration** as `_03_led_blink_pwm`, but instead of using `led_blink()` for automatic blinking, it uses `led_set_brightness()` to manually control the LED brightness level.

**Use case**: Smooth fading, dimming, or custom lighting patterns

## How Brightness Control Works

### PWM Duty Cycle

Brightness is controlled by varying the **PWM duty cycle**:

| Brightness | Duty Cycle | LED State |
|------------|------------|-----------|
| 0% | 0% | OFF (always LOW) |
| 25% | 25% | 25% ON, 75% OFF |
| 50% | 50% | 50% ON, 50% OFF |
| 75% | 75% | 75% ON, 25% OFF |
| 100% | 100% | Fully ON (always HIGH) |

The PWM frequency is high enough (>100 Hz) that the human eye perceives the average brightness, not individual pulses.

## Overlays

#### ESP32C6
- Uses LEDC (LED Controller) channel 0
- GPIO20 configured as PWM output
- See: [`boards/xiao_esp32c6_hpcore.overlay`](boards/xiao_esp32c6_hpcore.overlay)

#### STM32H573I-DK
- Uses TIM2 (Timer 2) channels 3 and 4
- Arduino D1 (PB10) and D0 (PB11) configured as PWM outputs
- See: [`boards/stm32h573i_dk.overlay`](boards/stm32h573i_dk.overlay)
