# Seeded Random Number Generator (_01_SRNG)

## Overview

This project is a demonstration application designed to test and showcase Kconfig configuration options and board-specific configuration files across different hardware platforms.

## Purpose

The main goal of this project is to:
- Explore how Kconfig works in Zephyr OS
- Test board-specific configuration files (`prj.conf`)
- Demonstrate cross-platform compatibility
- Show how different boards can be configured with different settings

## Important Note

**This is NOT a real random number generator.** This implementation is intended solely for educational and testing purposes. It should not be used in applications requiring cryptographically secure or statistically random numbers.

## Building the Project

To build for a specific board:

```bash
west build -b <board_name> apps/utils/_01_SRNG
```

Replace `<board_name>` with your target board (e.g., `stm32h573i_dk`).

## Flashing

After building:

```bash
west flash
```

## Configuration

Each board can have its own configuration settings defined in the `prj.conf` file. You can also use the Kconfig menu:

```bash
west build -b <board_name> apps/utils/_01_SRNG -t menuconfig
```

## Project Structure

```
_01_SRNG/
├── src/
│   └── main.c          # Application entry point
├── CMakeLists.txt      # Build configuration
├── prj.conf            # Project configuration
├── boards/
│   └── <board_name>.conf   # Board-specific configurations
└── README.md           # This file
```

