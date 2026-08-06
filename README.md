Zephyr easy examples
==========================

The main goal of the repo is to serve as a reference material for my students, feel free to copy reproduce and use under your own responsibility. If you are interested on Zephyr training please email to:

- [**hola@modular-mx.com**](hola@modular-mx.com)


This repository contains simple, beginner-friendly sample programs for the Zephyr real-time operating system. The samples demonstrate core Zephyr features and APIs, including hardware abstraction, device drivers, kernel services, and networking capabilities. Each sample is designed to be easy to understand and can serve as a starting point for developing Zephyr applications.

To build each example, follow the steps below for the next example

```bash
cd directory
west init
west update
git clone https://github.com/ModularMX/zephyr-samples.git samples
west build -b native_sim -p always samples/hal_drivers/GPIOS/_07_pin_custom_binding
```

The most common supported boards you are going to find are the following ones, but not all of them are supported on every example

- native_sim
- nucleo_g0b1re
- stm32h573i_dk
- nrf54l15dk/nrf54l15/cpuapp
- mcx_n9xx_evk/mcxn947/cpu0
- frdm_mcxa156

### A few things to keep in mind

- Each sample is written as simply as possible. Return values are intentionally not checked in these samples, but you should do it, in production code.
- Some examples are only supported by a few boards, always check the **boards/** on every sample
- Yes, we accept pull requests, especially from our students. 