# LED Driver Demo - Devicetree API

This example demonstrates how to use Zephyr's modern **devicetree-based LED API** to control LEDs in a simpler and more type-safe way.

## Overview

This demo showcases the recommended approach for LED control in Zephyr using the devicetree (DT) API. Instead of manually obtaining device bindings and using numeric indices, this method uses compile-time devicetree macros to reference LEDs directly.

## Differences from Previous Example

### Example 1: Device Binding API (Legacy approach)

```c
const struct device *led_dev = device_get_binding("leds");
led_on(led_dev, LED_INDEX);  // Requires numeric index
led_off(led_dev, LED_INDEX);
```

**Characteristics:**
- Runtime device lookup using string name
- Requires numeric LED index (0, 1, 2, ...)
- More verbose error checking
- Less type-safe

### Example 2: Devicetree API (Modern approach - THIS EXAMPLE)

```c
static const struct led_dt_spec led = LED_DT_SPEC_GET(DT_ALIAS(my_led));
led_on_dt(&led);   // No index needed
led_off_dt(&led);
```

**Characteristics:**
- Compile-time devicetree resolution
- No numeric indices required
- Type-safe LED specification structure
- Simpler and cleaner code
- **Recommended by Zephyr**

## How It Works

### 1. Devicetree Alias Definition

In the board overlay, we create an alias that points to a specific LED:

```dts
/ {
    aliases {
        my_led = &green_led_0;
    };
};
```

**What this does:**
- Creates a symbolic name `my_led` that references the existing `green_led_0` LED
- The alias can be accessed from C code using `DT_ALIAS(my_led)`
- You can point to any LED defined in the base DTS or overlay

### 2. Getting the LED Specification

```c
static const struct led_dt_spec led = LED_DT_SPEC_GET(DT_ALIAS(my_led));
```

**What happens at compile time:**
- `DT_ALIAS(my_led)` resolves to the devicetree node for the LED
- `LED_DT_SPEC_GET()` extracts all necessary information (device, index, etc.) into a `led_dt_spec` structure
- This happens at **compile time**, not runtime!

### 3. Checking if LED is Ready

```c
if (!led_is_ready_dt(&led)) {
    return 0;
}
```

The `led_is_ready_dt()` function checks if the LED device is properly initialized and ready to use.

### 4. Controlling the LED

```c
led_on_dt(&led);   // Turn LED on
led_off_dt(&led);  // Turn LED off
```

The devicetree API functions (`_dt` suffix) automatically handle:
- Device pointer
- LED index
- Error checking

## Advantages of Devicetree API

1. **Compile-time validation**: Errors are caught during build, not at runtime
2. **Type safety**: The `led_dt_spec` structure ensures you pass the correct parameters
3. **Cleaner code**: No manual device lookup or index management
4. **Board portability**: Just change the alias in the overlay; code remains the same
5. **Future-proof**: This is the recommended Zephyr approach going forward

## Board Configuration

### XIAO ESP32C6

**Overlay**: `boards/xiao_esp32c6_hpcore.overlay`

```dts
/ {
    aliases {
        my_led = &yellow_led;  // Points to the onboard yellow LED
    };
};
```

### STM32H573I-DK

**Overlay**: `boards/stm32h573i_dk.overlay`

```dts
/ {
    aliases {
        my_led = &green_led_0;  // Points to the green user LED (LD1)
    };
};
```

**Note**: The base DTS already defines `green_led_0`, `orange_led_0`, `red_led_0`, and `blue_led_0`. We simply create an alias to one of them.
