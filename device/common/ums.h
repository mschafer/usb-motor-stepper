#ifndef ums_h
#define ums_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UMS_IMPL
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN const char UMS_ENABLE[];

/** Counter of steps taken since last init. */
EXTERN uint32_t umsStepCounter;

/** Bit field of limit switches, 1 indicates stopped motion. */
EXTERN uint8_t umsLimits;

/** Cumulative sum of delays from step timer. */
EXTERN uint32_t umsRunTime;

/** Setting this to 1 will cause main to send a status and then clear it. */
EXTERN uint8_t umsStatusRequest;

#define UMS_MAJOR_VERSION 0
#define UMS_MINOR_VERSION 0

/**
 * This function initializes the ums and clears all data including axis configuration.
 * It is left in a disabled state where all commands are ignored until the enable string is received.
 */
void ums_init();

/**
 * This function executes the idle thread once and should be called as often as possible.
 */
void ums_idle();

#ifdef __cplusplus
}
#endif

#endif
