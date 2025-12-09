#include "watchdog_utils.h"
#include "config.h"
#include <avr/wdt.h>

void initWatchdog() {
  // Setup watchdog timer with configured timeout
  wdt_enable(WATCHDOG_TIMEOUT);
}

void resetWatchdog() {
  // Feed the watchdog to prevent system reset
  wdt_reset();
}

