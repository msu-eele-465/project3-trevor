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

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdint.h>

/**
 * Initialize a heartbeat LED
 *
 * This function relies upon various #defines being set, as documented in the
 * file-level documentation. A timer module is set to compare mode to create
 * the timer interrupt for the heartbeat LED.
 *
 * @param half_period_ms Half-period of the heartbeat LED in ms.
 */
void heartbeat_init(const uint16_t half_period_ms);

#endif // HEARTBEAT_H
