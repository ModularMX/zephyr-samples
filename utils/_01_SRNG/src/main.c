/**
 * @file main.c
 * @brief Seeded Random Number Generator demonstration application
 *
 * This application demonstrates the usage of Zephyr's random number generation
 * APIs using seeded (non-cryptographic) random number generation. It showcases
 * various ways to use the random number generation APIs including:
 * - Getting random numbers of different bit widths (8, 16, 32, 64-bit)
 * - Filling buffers with random data
 * - Generating random numbers within a specific range
 * - Simulating random events (dice rolls)
 * - Analyzing random bit patterns
 *
 * @note This is a seeded random number generator, NOT suitable for cryptographic
 *       purposes. For cryptographic needs, use sys_csrand_get().
 *
 * @author Jordan Garcia
 * @date 2025
 */
/* ============================================================================
 * INCLUDES
 * ============================================================================ */
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/util.h>

/* ============================================================================
 * GLOBALS
 * ============================================================================ */

// Settings
static const int32_t sleep_time_ms = 2000; /**< Sleep time between demonstrations in milliseconds */
static const int32_t buffer_size = 16;     /**< Size of random data buffer */

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================ */

/**
 * @brief Demonstrate basic random number generation with different bit widths
 *
 * This function generates and displays random numbers of 8, 16, 32, and 64-bit
 * widths using the sys_rand8_get(), sys_rand16_get(), sys_rand32_get(), and
 * sys_rand64_get() APIs respectively.
 *
 * @return void
 *
 */
void demo_basic_random(void);

/**
 * @brief Demonstrate filling a buffer with random data
 *
 * This function demonstrates the sys_rand_get() API by filling a buffer
 * with random data and displaying it in hexadecimal format. This is useful
 * for applications that need bulk random data, such as initialization vectors.
 *
 * @return void
 *
 */
void demo_random_buffer(void);

/**
 * @brief Demonstrate generating random numbers within a specific range
 *
 * This function generates random numbers constrained to a specific range (1-100)
 * using the modulo operator. This is useful for simulations and probabilistic
 * algorithms.
 *
 * @note The modulo method does not guarantee uniform distribution and should
 *       only be used for non-critical applications.
 *
 * @return void
 */
void demo_random_range(void);

/**
 * @brief Demonstrate random simulation (dice rolls)
 *
 * This function simulates rolling a 6-sided die multiple times, demonstrating
 * how to generate random numbers within a practical range for game simulation.
 *
 * @return void
 */
void demo_random_dice_simulation(void);

/**
 * @brief Demonstrate analysis of random bit patterns
 *
 * This function generates a random 32-bit value and displays:
 * - The hexadecimal representation
 * - The binary representation
 * - The count of set bits (population count)
 *
 * This demonstrates bit manipulation with random data.
 *
 * @return void
 */
void demo_random_patterns(void);

/* ============================================================================
 * MAIN APPLICATION
 * ============================================================================ */

int main(void)
{
    printk("========================================\n");
    printk("Seeded Random Number Generator Demo\n");
    printk("========================================\n");

    while (1)
    {
        demo_basic_random();
        k_msleep(sleep_time_ms);

        demo_random_buffer();
        k_msleep(sleep_time_ms);

        demo_random_range();
        k_msleep(sleep_time_ms);

        demo_random_dice_simulation();
        k_msleep(sleep_time_ms);

        demo_random_patterns();
        k_msleep(sleep_time_ms);

        printk("\n----------------------------------------\n");
    }

    return 0;
}

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ============================================================================ */

void demo_basic_random(void)
{
    printk("\n=== Basic Random Number Generation ===\n");

    uint8_t rand8 = sys_rand8_get();
    uint16_t rand16 = sys_rand16_get();
    uint32_t rand32 = sys_rand32_get();
    uint64_t rand64 = sys_rand64_get();

    printk("Random 8-bit  value:  %u (0x%02x)\n", rand8, rand8);
    printk("Random 16-bit value:  %u (0x%04x)\n", rand16, rand16);
    printk("Random 32-bit value:  %u (0x%08x)\n", rand32, rand32);
    printk("Random 64-bit value:  %llu (0x%016llx)\n", rand64, rand64);
}

void demo_random_buffer(void)
{
    printk("\n=== Buffer Random Generation ===\n");

    uint8_t buffer[buffer_size];

    // Fill buffer with general random data
    sys_rand_get(buffer, sizeof(buffer));

    printk("Random buffer (hex): ");
    for (int i = 0; i < sizeof(buffer); i++)
    {
        printk("%02x ", buffer[i]);
    }
    printk("\n");
}

void demo_random_range(void)
{
    printk("\n=== Random Number in Range (1-100) ===\n");

    for (uint8_t i = 0; i < 5; i++)
    {
        uint32_t random_val = sys_rand32_get();
        uint32_t range_val = (random_val % 100) + 1; // Range: 1-100
        printk("Random value %d: %u\n", i + 1, range_val);
    }
}

void demo_random_dice_simulation(void)
{
    printk("\n=== Dice Roll Simulation (6-sided die) ===\n");

    for (uint8_t i = 0; i < 10; i++)
    {
        uint8_t dice = (sys_rand8_get() % 6) + 1; // Range: 1-6
        printk("Roll %d: %u\n", i + 1, dice);
    }
}

void demo_random_patterns(void)
{
    printk("\n=== Random Bit Patterns ===\n");

    uint32_t pattern = sys_rand32_get();
    uint32_t set_bits = __builtin_popcountl(pattern);

    printk("Pattern:        0x%08x\n", pattern);
    printk("Binary:         ");
    for (int i = 31; i >= 0; i--)
    {
        printk("%d", (pattern >> i) & 1);
        if (i % 8 == 0)
            printk(" ");
    }
    printk("\n");
    printk("Number of set bits: %u out of 32\n", set_bits);
}