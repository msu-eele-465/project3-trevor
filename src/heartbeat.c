/**
 * @file
 * @brief Heartbeat LED module
 *
 * This module creates a heartbeat LED using a Timer B module.
 * This module is a singleton because it is tied to a particular timer interrupt;
 * making the isr generic was not trivial... This would be easier in C++...
 *
 * This module relies on preprocessor defines to configure itself:
 * - HEARTBEAT_TIMER_BASE_ADDR: base address of the timer module to use
 * - HEARTBEAT_TIMER_INTERRUPT_ADDR: address of the timer modules interrupt vector
 * - HEARTBEAT_PORT_BASE_ADDR: base address of the GPIO port to use
 * - HEARTBEAT_PIN: bitmask for the pin to use (e.g., BIT0)
 */

#include "heartbeat.h"
#include <msp430fr2355.h>

void heartbeat_init(const uint16_t half_period_ms)
{
    // Calculate addresses of the timer registers. Even if we aren't using TB0, all the register
    // offsets are the same for each Timer B module, so we can always use the TB0 offsets.
    volatile uint16_t *TBCTL = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CTL);
    volatile uint16_t *TBCCR0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCR0);
    volatile uint16_t *TBCCTL0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCTL0);

    // Disable the timer before configuring to avoid unexpected behavior
    *TBCTL &= ~MC_3;

    // Calculate the compare register value needed to achieve the desired timing
    // By default, we're going to use a 16-bit counter.
    // ACLK has a frequency of ~2^15, so a continous counter would overflow every 2 seconds.
    const uint32_t COUNTER_MAX = 65535;
    const uint16_t COUNTER_PERIOD_MS = 2000;

    uint16_t heartbeat_half_period_clock_cycles = COUNTER_MAX / (COUNTER_PERIOD_MS / half_period_ms);

    // Set the compare regsiter value so the interrupt fires and the counter restarts
    // every half-period of our heartbeat LED.
    *TBCCR0 = heartbeat_half_period_clock_cycles;

    // Enable the capture/compare interrupt
    *TBCCTL0 |= CCIE;

    // Select the 32.768 kHz auxiliary clock as the clock source.
    *TBCTL |= TBSSEL__ACLK;

    // Use up counter mode
    *TBCTL |= MC__UP;

    // Configure the heartbeat LED pin as an output
    volatile uint16_t *PDIR = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1DIR);
    *PDIR |= HEARTBEAT_PIN;

    // Turn LED off on initialiation
    volatile uint16_t *POUT = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1OUT);
    *POUT &= ~HEARTBEAT_PIN;
}

#pragma vector = HEARTBEAT_TIMER_INTERRUPT_ADDR
__interrupt void heartbeat_isr(void)
{
    volatile uint16_t *POUT = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1OUT);

    *POUT ^= HEARTBEAT_PIN;

    // Clear the interrupt flag :)
    volatile uint16_t *TBCCTL0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCTL0);
    *TBCCTL0 &= ~CCIFG;
}
