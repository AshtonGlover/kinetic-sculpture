#include "watchdog_utils.h"
#include "config.h"

// Renesas RA4M1 (UNO R4) uses the Renesas core; some environments expose mbed APIs.
// Use the mbed Watchdog API when available; otherwise compile with no-op stubs.
#if __has_include(<mbed.h>)
#include <mbed.h>
static mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();
#elif __has_include("mbed.h")
#include "mbed.h"
static mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();
#else
// No watchdog header available in this build environment.
#warning "Watchdog API header not found; watchdog functions will be no-ops."
#endif

void initWatchdog() {
  // Setup watchdog timer with configured timeout (milliseconds).
#if __has_include(<mbed.h>) || __has_include("mbed.h")
  watchdog.start(WATCHDOG_TIMEOUT);
#endif
}

void resetWatchdog() {
  // Feed the watchdog to prevent system reset
#if __has_include(<mbed.h>) || __has_include("mbed.h")
  watchdog.kick();
#endif
}

