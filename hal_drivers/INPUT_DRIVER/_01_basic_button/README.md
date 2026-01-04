# Basic Button Input with gpio-keys

This example demonstrates how to use the `gpio-keys` driver from Zephyr's INPUT subsystem to detect button presses and releases with hardware interrupt support.

## Overview

The `gpio-keys` driver is part of Zephyr's INPUT subsystem and provides:
- **Interrupt-driven operation**: No polling required, events are generated automatically on GPIO edge changes
- **Debouncing support**: Optional software debouncing to filter mechanical switch noise
- **Event-based API**: Callbacks are invoked when button state changes
- **Minimal CPU usage**: The system remains in low-power state until an interrupt occurs

## Hardware Configuration

### Supported Boards

- **XIAO ESP32-C6**: External button on GPIO18 with internal pull-up
- **STM32H573I-DK**: Onboard user button (PC13) already defined in the board DTS

### Pull-Up vs Pull-Down Resistors

The button configuration requires a pull resistor to ensure a defined logic level when the button is not pressed.

#### Internal Pull-Up (Active-Low)

This is the most common configuration. The button connects the GPIO to ground when pressed.

```dts
buttons {
    compatible = "gpio-keys";
    
    user_button: button_0 {
        gpios = <&gpio0 18 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
        label = "User Button";
        zephyr,code = <INPUT_KEY_0>;
    };
};
```

**Logic:**
- Button released: GPIO is HIGH (pulled up internally to VCC)
- Button pressed: GPIO is LOW (connected to GND)
- `GPIO_ACTIVE_LOW`: Inverts the logic so `evt->value = 1` means pressed

#### Internal Pull-Down (Active-High)

Use this when the button connects the GPIO to VCC instead of GND.

```dts
buttons {
    compatible = "gpio-keys";
    
    user_button: button_0 {
        gpios = <&gpio0 18 (GPIO_ACTIVE_HIGH | GPIO_PULL_DOWN)>;
        label = "User Button";
        zephyr,code = <INPUT_KEY_0>;
    };
};
```

**Logic:**
- Button released: GPIO is LOW (pulled down internally to GND)
- Button pressed: GPIO is HIGH (connected to VCC)
- `GPIO_ACTIVE_HIGH`: Direct logic, `evt->value = 1` means pressed

#### External Resistor (No Internal Pull)

When using an external pull-up or pull-down resistor, **do not specify** `GPIO_PULL_UP` or `GPIO_PULL_DOWN` flags. This leaves the GPIO in high-impedance mode, allowing only the external resistor to control the logic level.

```dts
buttons {
    compatible = "gpio-keys";
    
    /* External pull-up resistor (e.g., 10kΩ to VCC) */
    user_button: button_0 {
        gpios = <&gpio0 18 GPIO_ACTIVE_LOW>;  /* No GPIO_PULL_* flags */
        label = "User Button";
        zephyr,code = <INPUT_KEY_0>;
    };
};
```

**When to use external resistors:**
- Custom resistance values (typically 4.7kΩ - 47kΩ)
- GPIO doesn't support internal pull resistors
- Shared signal line with specific electrical requirements
- Better noise immunity with lower resistance values

### Debouncing

Mechanical buttons generate multiple edges when pressed/released due to contact bounce. The `gpio-keys` driver supports software debouncing:

```dts
user_button: button_0 {
    gpios = <&gpio0 18 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
    label = "User Button";
    zephyr,code = <INPUT_KEY_0>;
    debounce-interval-ms = <30>;  /* 30ms debounce time */
};
```

**Recommended values:**
- Tactile switches: 10-30ms
- Mechanical switches: 30-50ms
- If not specified: No debouncing (default)

## Software Configuration

### Event Codes

The `zephyr,code` property defines the event code sent by the driver. This uses standard Linux input event codes from `<zephyr/dt-bindings/input/input-event-codes.h>`:

```dts
zephyr,code = <INPUT_KEY_0>;  /* First button */
zephyr,code = <INPUT_KEY_1>;  /* Second button */
zephyr,code = <INPUT_KEY_ENTER>;
zephyr,code = <INPUT_KEY_ESC>;
```

This allows applications to distinguish between multiple buttons and implement different behaviors for each.

### Callback Registration

The INPUT subsystem uses a static callback registration macro:

```c
INPUT_CALLBACK_DEFINE(device, callback_function, user_data);
```

**Parameters:**
- `device`: Device pointer or `NULL` to receive events from all input devices
- `callback_function`: Function to call when an event occurs
- `user_data`: Optional pointer passed to the callback

**Callback signature:**
```c
void callback(struct input_event *evt, void *user_data);
```

**Event structure:**
```c
struct input_event {
    uint8_t type;   /* INPUT_EV_KEY for buttons */
    uint16_t code;  /* Value from zephyr,code property */
    int32_t value;  /* 1 = pressed, 0 = released */
    uint32_t sync;  /* Synchronization flag */
};
```

### Interrupt-Driven Operation

The `gpio-keys` driver operates entirely on interrupts:

1. **Initialization**: The driver configures GPIO interrupts on both rising and falling edges
2. **Button Press**: GPIO edge triggers an interrupt → ISR queues an input event → Callback invoked
3. **Button Release**: Same process, but `evt->value = 0`
4. **No Polling**: The main thread can sleep indefinitely (`k_sleep(K_FOREVER)`)

**Benefits:**
- Zero CPU usage while waiting for button events
- Immediate response (sub-millisecond latency)
- Ideal for battery-powered applications

## Configuration Options

### prj.conf

```ini
CONFIG_GPIO=y      # Enable GPIO driver
CONFIG_INPUT=y     # Enable INPUT subsystem
```

## References

- [Zephyr INPUT Subsystem Documentation](https://docs.zephyrproject.org/latest/services/input/index.html)
- [gpio-keys Driver Bindings](https://docs.zephyrproject.org/latest/build/dts/api/bindings/input/gpio-keys.html)
- [ Keys events](https://docs.zephyrproject.org/latest/doxygen/html/group__input__events.html)