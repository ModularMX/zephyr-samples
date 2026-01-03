# FPU Basic Demonstration (_02_FPU)

## Overview

This project demonstrates hardware Floating Point Unit (FPU) functionality in Zephyr with proper printk formatting. It serves as an educational example showing how to configure and use FPU in embedded systems.

## Purpose

The main goals of this project are to:
- Enable and configure hardware FPU support via Kconfig
- Demonstrate FPU arithmetic operations (add, subtract, multiply, divide)
- Show how to print floating-point values with `printk()` using the `%f` format specifier
- Illustrate Kconfig and menuconfig usage for hardware feature configuration
- Provide a foundation for floating-point applications

## Configuration Methods

### Method 1: Direct <board>.conf Editing (Fastest)
1. Open `apps/utils/_02_FPU/<board>.conf`
2. Add or modify config lines like: `CONFIG_FPU=y`
3. Rebuild: `west build -b <board> .`

**Note:** After <board>.conf changes, you may need a clean rebuild:
```bash
west build -p always -b <board> .
```
The `-p always` flag forces CMake to re-scan Kconfig options.

### Method 2: Interactive menuconfig (Recommended for Exploration)
1. Run the menuconfig task:
   ```bash
   west build -b <board> . -t menuconfig
   ```
2. Navigate using arrow keys (or search for FPU)
3. Toggle options with `<space>` or `<y>/<n>`
4. Save and exit: `<Esc>` twice
5. Rebuild: `west build -b <board> .`

**Pro tip:** In menuconfig, press `/` to search for options like "FPU"

## FPU Configuration Options Used

This project enables the following Kconfig options (see `<board>.conf`):

| Option | Purpose |
|--------|---------|
| `CONFIG_FPU=y` | Enable ARM hardware floating-point unit |
| `CONFIG_FP_HARDABI=y` | Use hardware FP calling convention (efficient) |
| `CONFIG_CBPRINTF_FP_SUPPORT=y` | Enable `%f` format in printk() |
| `CONFIG_CBPRINTF_NANO=n` | Use full printf (not nano) |
| `CONFIG_PICOLIBC_IO_FLOAT=y` | FP support in C standard library |

## Building the Project

Basic build:
```bash
west build -b stm32h573i_dk apps/utils/_02_FPU
```

Clean build (forces Kconfig re-evaluation):
```bash
west build -p always -b stm32h573i_dk apps/utils/_02_FPU
```

## Flashing to Board

```bash
west flash
```

## Monitoring Serial Output

Run the Serial Monitor task, or:
```bash
python -m serial.tools.miniterm COM3 115200 --eol LF
```

Expected output (floating-point values printed with `%f`):
```
[iter 0]
a: 3.141590
b: 2.500000
sum: 5.641590
...
```

