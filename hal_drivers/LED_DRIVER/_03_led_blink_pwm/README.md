# LED Hardware Blink - PWM LED Driver

This example demonstrates **hardware-based LED blinking** using the PWM LED driver. Unlike software-based blinking (toggling GPIO in a loop), this approach uses hardware PWM to autonomously blink the LED without CPU intervention.

## Overview

This demo shows the key difference between `gpio-leds` and `pwm-leds` drivers, specifically for the `led_blink()` API function which enables hardware-based LED blinking.

## Problem: Why `gpio-leds` Doesn't Support `led_blink()`

In previous examples (`_01_led_driver_demo` and `_02_led_on_off_dt`), we used the `gpio-leds` driver with the compatible string `"gpio-leds"`. This driver has a **limited API**:

```c
// gpio-leds driver API (led_gpio.c)
static DEVICE_API(led, led_gpio_api) = {
    .set_brightness = led_gpio_set_brightness,
    // NO .blink function!
};
```

**Result**: Calling `led_blink()` with a `gpio-leds` device returns `-ENOSYS` (error -88, function not implemented).

### Why?

The `gpio-leds` driver is **simple** and **basic**:
- It only controls GPIO pin state (HIGH/LOW)
- No hardware timer or PWM involvement
- Blinking must be done manually in software using `led_on()` / `led_off()` + `k_sleep()`

## Solution: Using `pwm-leds` Driver

The `pwm-leds` driver uses **hardware PWM (Pulse Width Modulation)** to control LEDs, which enables:
- **Hardware-based blinking**: LED blinks autonomously via PWM hardware
- **CPU-free operation**: No software loops or timers needed
- **Precise timing**: Hardware timers provide accurate blink periods
- **Brightness control**: PWM duty cycle allows dimming/fading

```c
// pwm-leds driver API (led_pwm.c)
static DEVICE_API(led, led_pwm_api) = {
    .blink          = led_pwm_blink,       // ✓ Hardware blink!
    .set_brightness = led_pwm_set_brightness,
};
```

## Key Differences: `gpio-leds` vs `pwm-leds`

| Feature | `gpio-leds` | `pwm-leds` |
|---------|-------------|------------|
| **Compatible string** | `"gpio-leds"` | `"pwm-leds"` |
| **Hardware required** | GPIO only | GPIO + PWM/Timer |
| **`led_on()` / `led_off()`** | ✓ Yes | ✓ Yes |
| **`led_blink()` hardware** | ✗ No (`-ENOSYS`) | ✓ Yes |
| **`led_set_brightness()`** | ✓ On/Off only | ✓ 0-100% via PWM |
| **CPU usage during blink** | High (software loop) | None (hardware) |
| **Power consumption** | Higher | Lower (CPU idle) |
| **Setup complexity** | Simple | Requires PWM config |
| **Pin requirements** | Any GPIO | GPIO with PWM capability |

## How It Works

### 1. PWM Hardware Configuration

**PWM** (Pulse Width Modulation) generates a square wave signal:
- **Period**: Total time for one cycle (e.g., 20 ms)
- **Duty Cycle**: Percentage of time the signal is HIGH (e.g., 50% = LED at half brightness)

For **blinking**, the driver alternates the duty cycle:
- **ON period** (delay_on): Duty cycle = 100%
- **OFF period** (delay_off): Duty cycle = 0%

The PWM hardware controller automatically switches between these states without CPU intervention.

### 2. Devicetree Configuration

#### ESP32C6 (LEDC - LED Controller)

The ESP32 has a dedicated **LEDC** (LED Controller) peripheral designed specifically for LED control with hardware PWM:

```dts
&ledc0 {
    status = "okay";
    #address-cells = <1>;
    #size-cells = <0>;

    channel0@0 {
        reg = <0x0>;    // Channel 0
        timer = <0>;    // Use timer 0
    };
};

/ {
    pwmleds {
        compatible = "pwm-leds";
        
        pwm_led0: pwm_led_0 {
            pwms = <&ledc0 0 1000 PWM_POLARITY_NORMAL>;
            //      ^      ^ ^     ^
            //      |      | |     +-- Polarity (NORMAL = active high)
            //      |      | +-------- Period in nanoseconds (1000 ns = 1 MHz)
            //      |      +---------- Channel number (0)
            //      +----------------- PWM controller reference (&ledc0)
            label = "PWM LED GPIO18";
        };
    };
};
```

**LEDC advantages:**
- Up to 8 channels (can control 8 LEDs independently)
- Flexible timer configuration
- Any GPIO can be routed to LEDC via pinmux

#### STM32H573I-DK (TIM2 - Hardware Timer)

STM32 uses **hardware timers** (TIM1-TIM17) with PWM channels:

```dts
&pwm2 {
    pinctrl-0 = <&tim2_ch3_pb10 &tim2_ch4_pb11>;
    //            ^              ^
    //            |              +-- Arduino D0: Timer 2, Channel 4, Pin PB11
    //            +----------------- Arduino D1: Timer 2, Channel 3, Pin PB10
    pinctrl-names = "default";
};

/ {
    pwmleds {
        compatible = "pwm-leds";
        
        pwm_led_d1: pwm_led_0 {
            pwms = <&pwm2 3 PWM_MSEC(20) PWM_POLARITY_NORMAL>;
            //      ^     ^ ^            ^
            //      |     | |            +-- Polarity
            //      |     | +--------------- Period (20 ms = 50 Hz)
            //      |     +----------------- Channel 3 (PB10)
            //      +----------------------- PWM2 (Timer 2)
            label = "PWM LED Arduino D1";
        };
    };
};
```

**STM32 timer notes:**
- Not all pins support PWM (must be connected to a timer channel)
- Check pinctrl file (`stm32h573iikxq-pinctrl.dtsi`) for available `tim<N>_ch<X>_p<Y><Z>` mappings

### 3. Pinctrl Configuration

#### ESP32C6

Uses pinmux to route LEDC output to any GPIO:

```dts
&pinctrl {
    ledc0_ch0_default: ledc0_ch0_default {
        group1 {
            pinmux = <LEDC_CH0_GPIO18>;  // Route LEDC channel 0 to GPIO18
            output-enable;
        };
    };
};
```

#### STM32

Timer channels have **fixed alternate functions** per pin:

```dts
// Defined in stm32h573iikxq-pinctrl.dtsi:
tim2_ch3_pb10: tim2_ch3_pb10 {
    pinmux = <STM32_PINMUX('B', 10, AF1)>;  // PB10, Alternate Function 1
};

tim2_ch4_pb11: tim2_ch4_pb11 {
    pinmux = <STM32_PINMUX('B', 11, AF1)>;  // PB11, Alternate Function 1
};
```


## Project Configuration

**Important**: 
- `CONFIG_PWM=y` is **required** to enable the PWM subsystem
- For ESP32, `CONFIG_PWM_LED_ESP32=y` is enabled automatically when LEDC is in devicetree
- For STM32, `CONFIG_PWM_STM32=y` is enabled automatically when timers are configured

## How to Verify PWM Support for a Pin

### STM32 Method

1. **Find the pinctrl file** for your MCU:
   ```
   modules/hal/stm32/dts/st/h5/stm32h573iikxq-pinctrl.dtsi
   ```

2. **Search for timer channels** on your pin:
   ```bash
   # Search for: tim<N>_ch<X>_p<port><pin>
   # Example: tim2_ch3_pb10
   ```

3. **Check the result**:
   ```dts
   tim2_ch3_pb10: tim2_ch3_pb10 {
       pinmux = <STM32_PINMUX('B', 10, AF1)>;
   };
   ```
   - **Yes**: Pin PB10 supports TIM2 Channel 3 via Alternate Function 1
   - **No matches**: Pin doesn't support PWM

### ESP32 Method

**All GPIOs** on ESP32 support PWM via LEDC/MCPWM through the flexible pinmux system. Just route any LEDC channel to your desired GPIO:

```dts
pinmux = <LEDC_CH0_GPIO18>;  // LEDC Channel 0 → GPIO18
```
