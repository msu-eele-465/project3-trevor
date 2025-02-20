#include <msp430fr2355.h>

#include "heartbeat.h"

static volatile uint16_t *TBxCTL = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CTL);
static volatile uint16_t *TBxCCR0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCR0);
static volatile uint16_t *TBxCCTL0 = (uint16_t *)(HEARTBEAT_TIMER_BASE_ADDR + OFS_TB0CCTL0);
static volatile uint16_t *PDIR = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1DIR);
static volatile uint16_t *POUT = (uint16_t *)(HEARTBEAT_PORT_BASE_ADDR + OFS_P1OUT);

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
