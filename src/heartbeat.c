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
 * - HEARTBEAT_TIMER_VECTOR address of the timer modules interrupt vector
 * - HEARTBEAT_PORT_BASE_ADDR: base address of the GPIO port to use
 * - HEARTBEAT_PIN: bitmask for the pin to use (e.g., BIT0)
 */

#include <msp430fr2355.h>

#include "heartbeat.h"

static volatile uint16_t *TBxCTL = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CTL);
static volatile uint16_t *TBxCCR0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCR0);
static volatile uint16_t *TBxCCTL0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCTL0);
static volatile uint16_t *PDIR = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1DIR);
static volatile uint16_t *POUT = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1OUT);

/**
 * Initialize a heartbeat LED
 *
 * This function relies upon various #defines being set, as documented in the
 * file-level documentation. A timer module is set to compare mode to create
 * the timer interrupt for the heartbeat LED.
 *
 * @param half_period_ms Half-period of the heartbeat LED in ms.
 */
void heartbeat_init(const uint16_t half_period_ms)
{
    // Disable the timer before configuring to avoid unexpected behavior
    *TBxCTL &= ~MC_3;

    // Calculate the compare register value needed to achieve the desired timing
    // By default, we're going to use a 16-bit counter.
    // ACLK has a frequency of ~2^15, so a continous counter would overflow every 2 seconds.
    const uint32_t COUNTER_MAX = 65535;
    const uint16_t COUNTER_PERIOD_MS = 2000;

    uint16_t heartbeat_half_period_clock_cycles = COUNTER_MAX / (COUNTER_PERIOD_MS / half_period_ms);

    // Set the compare regsiter value so the interrupt fires and the counter restarts
    // every half-period of our heartbeat LED.
    *TBxCCR0 = heartbeat_half_period_clock_cycles;

    // Enable the capture/compare interrupt
    *TBxCCTL0 |= CCIE;

    // Select the 32.768 kHz auxiliary clock as the clock source.
    *TBxCTL |= TBSSEL__ACLK;

    // Use up counter mode
    *TBxCTL |= MC__UP;

    // Configure the heartbeat LED pin as an output
    *PDIR |= HEARTBEAT_PIN;

    // Turn LED off on initialiation
    *POUT &= ~HEARTBEAT_PIN;
}

/**
 * Heartbeat LED interrupt service routine.
 *
 * This toggles the GPIO port defined by HEARTBEAT_PIN and HEARTBEAT_PORT_BASE_ADDR.
 * HEARTBEAT_TIMER_VECTOR specifies the address of the timer interrupt vector
 * we need to use (this needs to match the timer module we're using).
 */
#pragma vector = HEARTBEAT_TIMER_VECTOR
__interrupt void heartbeat_isr(void)
{
    *POUT ^= HEARTBEAT_PIN;

    // Clear the interrupt flag
    *TBxCCTL0 &= ~CCIFG;
}
