#include <intrinsics.h>
#include <msp430fr2355.h>
#include <stdbool.h>

#include "heartbeat.h"

const uint16_t HEARTBEAT_HALF_PERIOD_MS = 500;

void init()
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    heartbeat_init(HEARTBEAT_HALF_PERIOD_MS);

    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Enable global interrupts
    __enable_interrupt();
}

int main(void)
{
    init();

    while (true)
    {
    }
}
