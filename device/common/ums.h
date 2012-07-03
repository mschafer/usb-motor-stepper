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

/** Counters that indicate the relative position of each axis since reset. */
EXTERN int32_t umsXPos, umsYPos, umsZPos, umsUPos;

#define UMS_SEND_STATUS_NOW 0x01	///\< This bit causes a status message to be sent on the next call to main
#define UMS_STEPPER_RUNNING 0x02    ///\< This bit indicates that the stepper is currently running

/** Status bits. */
EXTERN uint8_t umsStatus;

#define UMS_MAJOR_VERSION 0
#define UMS_MINOR_VERSION 0

// macros for endian independent packing of integers into byte arrays
#define UMS_UNPACK_U16(p) ( ((uint16_t)((p)[0]) << 8) + (p)[1] )
#define UMS_UNPACK_I16(p) ((int16_t)(UMS_UNPACK_U16(p)))
#define UMS_PACK_16(v, p) \
		(p)[0] = (uint16_t(v) >> 8) & 0xFF; \
		(p)[1] =  uint16_t(v) & 0xFF;
#define UMS_UNPACK_U32(p) ( ((uint32_t)((p)[0]) << 24) + ((uint32_t)((p)[1]) << 16) + ((uint32_t)((p)[2]) << 8) + ((p)[3]) )
#define UMS_UNPACK_I32(p) ((int32_t)(UMS_UNPACK_U32(p)))
#define UMS_PACK_32(v, p) \
		(p)[0] = ((v) >> 24) & 0xFF; \
		(p)[1] = ((v) >> 16) & 0xFF; \
		(p)[2] = ((v) >>  8) & 0xFF; \
		(p)[3] =  (v) & 0xFF;


/**
 * This function initializes the ums and clears all data including axis configuration.
 * It is left in a disabled state where all commands are ignored until the enable string is received.
 */
void ums_init();

/**
 * This function executes the main message handling loop once and should be called as often as possible.
 */
void ums_run_once();

#ifdef __cplusplus
}
#endif

#endif
