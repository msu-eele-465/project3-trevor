/**
 * @file
 * @brief Delay loops
 *
 * This is a wrapper to make using the __delay_cycles() function more convenient.
 */

#ifndef DELAY_H
#define DELAY_H

#include <intrinsics.h>

// MCLK defaults to 1 MHz. If you are using a different clock frequency,
// define MCLK_FREQ when compiling.
#ifndef MCLK_FREQ
#define MCLK_FREQ 1ul
#endif

#define delay_ms(n_ms) (__delay_cycles((unsigned long)(n_ms) * 1000ul * MCLK_FREQ))
#define delay_us(n_us) (__delay_cycles((unsigned long)(n_us) * 1ul * MCLK_FREQ))

#endif // DELAY_H
