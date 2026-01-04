# Double Tap Button Detection

This example demonstrates how to detect double tap (double click) button presses using Zephyr's `zephyr,input-double-tap` driver.

## What Does This Example Do?

The program detects when a button is pressed twice in rapid succession, similar to double-clicking a mouse. It differentiates between:
- **Single tap**: A regular button press
- **Double tap**: Two button presses within 300ms

## What is the `zephyr,input-double-tap` Driver?

`zephyr,input-double-tap` is a **pseudo-device** in Zephyr. Like `input-longpress`, it doesn't control hardware directly but processes input events to detect patterns.

### How Does It Work?

1. **Monitors input events**: The driver listens to events from another input device (like `gpio-keys`)
2. **Tracks timing**: When a button is pressed and released, it starts a timer
3. **Detects the pattern**:
   - If the button is pressed again **within the delay window** → generates a "double tap" event
   - If the delay expires without a second press → only the first press is registered

### Advantages

- ✅ **Pattern detection built-in**: No manual timing or state machine code needed
- ✅ **Devicetree configuration**: Declarative setup, minimal C code
- ✅ **Configurable timing**: Adjust the delay window to your needs
- ✅ **Multi-button support**: Can monitor multiple buttons simultaneously

## Devicetree Configuration

### Basic Example (One Button)

```dts
/ {
    buttons {
        compatible = "gpio-keys";
        button0: button_0 {
            gpios = <&gpio0 18 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>;
            zephyr,code = <INPUT_KEY_0>;
        };
    };

    double_tap {
        compatible = "zephyr,input-double-tap";
        input = <&buttons>;
        input-codes = <INPUT_KEY_0>;        /* Original button code */
        double-tap-codes = <INPUT_KEY_A>;   /* Code for double tap */
        double-tap-delay-ms = <300>;        /* 300ms window */
    };
};
```

### Property Explanation

| Property | Type | Description |
|----------|------|-------------|
| `input` | phandle | Reference to the input device to monitor (optional - if omitted, listens to all) |
| `input-codes` | array | Original button codes to detect |
| `double-tap-codes` | array | Codes to emit when double tap is detected |
| `double-tap-delay-ms` | int | Maximum time between taps in milliseconds |

### Multi-Button Example

```dts
double_tap {
    compatible = "zephyr,input-double-tap";
    input = <&buttons>;
    input-codes = <INPUT_KEY_0>, <INPUT_KEY_1>;     /* Two buttons */
    double-tap-codes = <INPUT_KEY_A>, <INPUT_KEY_B>; /* 1:1 mapping */
    double-tap-delay-ms = <300>;
};
```

Arrays map **positionally**:
- `INPUT_KEY_0` → `INPUT_KEY_A` (double tap)
- `INPUT_KEY_1` → `INPUT_KEY_B` (double tap)


## Kconfig Configuration

Enable the required drivers in `prj.conf`:

```ini
CONFIG_GPIO=y
CONFIG_INPUT=y
CONFIG_INPUT_DOUBLE_TAP=y  # Important!
```

## Hardware Setup

### XIAO ESP32C6
- **Button**: GPIO18 with internal pull-up
- **Configuration**: GPIO_ACTIVE_LOW (pressing connects to GND)
- **Status**: ✅ Works perfectly

### STM32H573I-DK

⚠️ **IMPORTANT NOTE**: The onboard button on the STM32H573I-DK board had issues during testing (erratic behavior with spurious events).

**Solution**: Use an external button connected to Arduino pins:
- **Pin**: Arduino D0 (PB10)
- **Configuration**: GPIO_PULL_UP + GPIO_ACTIVE_LOW
- **Connection**: 
  - One button terminal → PB10 (Arduino D0)
  - Other terminal → GND
- **Status**: ✅ Works perfectly with external button


### Event Behavior

**Important**: When you successfully double tap, you'll receive:
1. First press: `INPUT_KEY_0` event (single tap)
2. Second press (within delay): `INPUT_KEY_A` event (double tap)

The driver doesn't suppress the first single tap event - it only adds the double tap event when the pattern is detected.

## Use Cases

- **User Interfaces**: Different actions for single vs. double tap
  - Single tap: Select item
  - Double tap: Activate/open item
  
- **Power Management**: 
  - Single tap: Wake from sleep
  - Double tap: Trigger action
  
- **Security**: Prevent accidental activation
  - Require double tap for critical operations
  
- **Navigation**: Quick shortcuts
  - Double tap to go back/forward
  - Double tap to toggle modes

- **Accessibility**: Alternative to long press for users who prefer quicker interactions

## Comparison with Other Input Drivers

| Driver | Detects | Use Case |
|--------|---------|----------|
| `gpio-keys` | Press/release | Basic button input |
| `input-longpress` | Short/long press | Hold for different action |
| `input-double-tap` | Double click | Quick repeated presses |

You can combine these drivers! For example:
- Single tap, double tap, AND long press on the same button
- Each generates different event codes for different actions

## Additional Resources

- [Zephyr INPUT Subsystem Documentation](https://docs.zephyrproject.org/latest/hardware/peripherals/input.html)
- [Input Double Tap Driver Binding](zephyr/dts/bindings/input/zephyr,input-double-tap.yaml)
- [GPIO Keys Driver](zephyr/dts/bindings/input/gpio-keys.yaml)
- [Input Longpress Driver](zephyr/dts/bindings/input/zephyr,input-longpress.yaml)
