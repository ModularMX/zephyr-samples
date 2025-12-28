/**
 * @file main.c
 * @brief Basic FPU (Floating Point Unit) demonstration with printk
 *
 * This application demonstrates the usage of Zephyr's FPU capabilities with
 * printk floating-point formatting. It showcases:
 * - Hardware FPU arithmetic operations (addition, subtraction, multiplication, division)
 * - Printing floating-point values using printk() with %f format specifier
 * - FPU configuration via Kconfig/prj.conf
 * - Practical FPU usage patterns in embedded systems
 *
 * The FPU configuration is defined in <board>.conf:
 * - CONFIG_FPU=y                   Enable hardware floating-point unit
 * - CONFIG_FP_HARDABI=y            Use hardware FP calling convention (more efficient)
 * - CONFIG_CBPRINTF_FP_SUPPORT=y   Enable %f formatting in printk
 * - CONFIG_PICOLIBC_IO_FLOAT=y     Enable FP support in standard C library
 *
 *
 * @note Floating-point arithmetic may have performance and power implications.
 *       Only enable FPU if your application genuinely requires it.
 *
 * @author Jordan Garcia
 * @date 2025
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define LOOP_MS 1500

static float a = 3.14159f;
static float b = 2.5f;
static float accumulator = 0.0f;

static void dump_value(const char *label, float value);


int main(void)
{
    uint32_t iter = 0U;

    printk("\nFPU basic demo using printk with %f\n", 0.0);

    while (1)
    {
        float sum = a + b;
        float diff = a - b;
        float prod = a * b;
        float quot = (b != 0.0f) ? (a / b) : 0.0f;

        accumulator = (accumulator + prod * 0.01f) * 0.99f;

        printk("\n[iter %u]\n", iter++);
        dump_value("a", a);
        dump_value("b", b);
        dump_value("sum", sum);
        dump_value("diff", diff);
        dump_value("prod", prod);
        dump_value("quot", quot);
        dump_value("accum", accumulator);

        a += 0.25f;
        b += 0.10f;
        if (a > 6.0f)
        {
            a = 3.0f;
        }
        if (b > 3.5f)
        {
            b = 1.5f;
        }

        k_msleep(LOOP_MS);
    }

    return 0;
}

static void dump_value(const char *label, float value)
{
    printk("%s: %f\n", label, (double)value);
}