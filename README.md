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
west build -b native_sim -p always ./samples/hal_drivers/GPIOS/_07_pin_custom_binding
```

A few things to keep in mind

- Each sample is written as simply as possible. Return values are intentionally not checked in these samples, but you should check them in production code.
- Some examples are only supported by a few boards, always check the **boards/** on every sample
- Yes, we accept pull requests, especially from our students. 