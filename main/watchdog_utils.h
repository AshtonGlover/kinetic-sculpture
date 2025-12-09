#ifndef WATCHDOG_UTILS_H
#define WATCHDOG_UTILS_H

/**
 * Initialize the watchdog timer
 * Sets up watchdog with the configured timeout
 */
void initWatchdog();

/**
 * Reset the watchdog timer
 * Should be called regularly in the main loop to prevent reset
 */
void resetWatchdog();

#endif // WATCHDOG_UTILS_H

