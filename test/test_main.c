#include <intrinsics.h>
#include <msp430fr2355.h>
#include <stdbool.h>

#include "delay.h"

void init()
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    // Enable global interrupts
    // __enable_interrupt();
}

int main(void)
{
    init();

    while (true)
    {
        // (__delay_cycles(500 * 1000 * 1));
        // __delay_cycles(500ul * 1000ul * 1ul);
        delay_ms(1000);
        P1OUT ^= BIT0;
    }
}
