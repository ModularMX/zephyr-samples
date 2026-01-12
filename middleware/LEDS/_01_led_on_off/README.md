# LED Driver Demo

This example demonstrates how to use Zephyr's LED driver API to control LEDs on different boards through devicetree configuration.

## Overview

This demo showcases a minimal implementation of LED blinking using the Zephyr LED driver subsystem. The code uses the generic LED API (`device_get_binding()`, `led_on()`, `led_off()`) to control LEDs in a hardware-agnostic way.

## How It Works

### LED Driver Subsystem

Zephyr provides a generic LED driver that abstracts GPIO-based LED control. When you add a node with `compatible = "gpio-leds"` to your devicetree, Zephyr automatically:

1. **Instantiates the LED driver**: The `gpio-leds` compatible binding tells Zephyr to create an LED device that can be accessed by the name `"leds"`.

2. **Maps GPIO pins to LED indices**: Each child node under `leds` becomes an addressable LED (index 0, 1, 2, etc.) that can be controlled through the LED API.

3. **Handles GPIO configuration**: The driver automatically configures the GPIO pins (direction, active state) based on the devicetree properties.

### Code Implementation

```c
const struct device *led_dev = device_get_binding("leds");
led_on(led_dev, LED_INDEX);
led_off(led_dev, LED_INDEX);
```

The application doesn't need to know about specific GPIO ports or pins—it just references LEDs by index through the unified driver interface.

## Supported Boards

### XIAO ESP32C6

For this board, LEDs are connected to the onboard LED and additional GPIO pins.

**Board overlay**: `boards/xiao_esp32c6_hpcore.overlay`

### STM32H573I-DK

For this board, we extend the existing LEDs with two additional LEDs mapped to Arduino header pins D0 and D1.

**Board overlay**: `boards/stm32h573i_dk.overlay`

## Devicetree Overlay Differences

The overlay approach differs between boards due to how their base devicetree files define the `leds` node:

### XIAO ESP32C6 Base DTS

The XIAO board's base DTS defines the leds node **with a label**:

```dts
leds: leds {
    compatible = "gpio-leds";

    yellow_led: led_0 {
        gpios = <&gpio0 15 GPIO_ACTIVE_LOW>;
        label = "User LED1";
    };
};
```

**Overlay approach**: Since the base DTS provides a `leds:` label, the overlay can directly reference it using `&leds`:

```dts
&leds {
    // Add or modify LED definitions here
};
```

### STM32H573I-DK Base DTS

The STM32 board's base DTS defines the leds node **without a label**:

```dts
leds {
    compatible = "gpio-leds";

    green_led_0: led_1 {
        gpios = <&gpioi 9 GPIO_ACTIVE_LOW>;
        label = "User LD1";
    };

    orange_led_0: led_2 {
        gpios = <&gpioi 8 GPIO_ACTIVE_LOW>;
        label = "User LD2";
    };

    red_led_0: led_3 {
        gpios = <&gpiof 1 GPIO_ACTIVE_LOW>;
        label = "User LD3";
    };

    blue_led_0: led_4 {
        gpios = <&gpiof 4 GPIO_ACTIVE_LOW>;
        label = "User LD4";
    };
};
```

**Overlay approach**: Since there's no label to reference, the overlay must reopen the node **by path** and can optionally assign a label for future references:

```dts
/ {
    leds: leds {
        // Reopens the existing /leds node and assigns it the label "leds:"
        // Add new LED definitions here
    };
};
```

This is why the STM32 overlay uses `/ { leds: leds { ... } }` instead of `&leds { ... }`.

### GPIO Active State

LEDs can be configured as active-high or active-low:

- **`GPIO_ACTIVE_LOW`**: LED turns ON when the GPIO pin is driven LOW (0V). Common for LEDs with the cathode connected to the GPIO.
  
- **`GPIO_ACTIVE_HIGH`**: LED turns ON when the GPIO pin is driven HIGH (VCC). Common for LEDs with the anode connected to the GPIO.

The LED driver handles this automatically based on the devicetree configuration.


## Learning Outcomes

1. **Generic LED API usage**: Use hardware-agnostic LED control functions.
2. **Devicetree overlays**: Understand how to extend or modify existing devicetree nodes for custom board configurations.
3. **Label vs. path references**: Learn when to use `&label` references versus path-based node reopening in devicetree.
4. **Driver binding system**: Understand how `compatible` strings map hardware to drivers in Zephyr.
5. **GPIO abstraction**: See how the LED driver abstracts GPIO details (ports, pins, polarity) from application code.
