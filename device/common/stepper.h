#ifndef ums_stepper_h
#define ums_stepper_h

#include <stdint.h>
#include "commands.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialize stepper.
 */
void st_init();

/**
 * \return Zero if not full, non-zero if full.
 */
uint8_t st_full();

/**
 * Add a step command w/ delay to the FIFO.
 */
void st_add_step(uint8_t stepDir, uint32_t delay);

/**
 * This routine outputs the next step and should be called when the timer goes off.
 * It will reschedule the timer for the next step.
 * Should be safe to call inside an ISR.
 */
void st_run_once();

void st_setup_axis(struct AxisCmd *c);

void st_line_setup(int dx, int dy, int dz, int du, uint16_t delay);

#ifdef __cplusplus
}
#endif

#endif
