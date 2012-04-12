#ifndef umc_h
#define umc_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UMC_IMPL
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN const char UMC_ENABLE[];

#define UMC_MAJOR_VERSION 0
#define UMC_MINOR_VERSION 0

/**
 * This function initializes the umc and clears all data including axis configuration.
 * It is left in a disabled state where all commands are ignored until the enable string is received.
 */
void umc_init();

/**
 * This function executes the idle thread once and should be called as often as possible.
 */
void umc_idle();

#ifdef __cplusplus
}
#endif

#endif
