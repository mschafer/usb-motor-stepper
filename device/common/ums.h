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
EXTERN uint32_t umsStepCounter;

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
