#ifndef step_timer_h_
#define step_timer_h_

#include <stdint.h>

/**
 * \file
 * This interface controls timer16_0 which is used to drive the stepper.
 */

/**
 * Configure timer16_0 to generate one interrupt based on match register 0 and then stop.
 * Timer is not left running by this call.
 */
void step_timer_init();

/**
 * Start the timer running.  A single interrupt will be generated when the TC reaches val
 * and then the timer will be stopped.
 */
void step_timer_start(uint16_t val);

void step_timer_prescale(uint16_t val);


#endif
