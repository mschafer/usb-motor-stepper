#include "lpc134x.h"
#include "core/gpio/gpio.h"

void step_timer_init()
{
    /* enable the clock for CT16B0 */
    SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_CT16B0);

    /* reset the timer */
    TMR_TMR16B0TCR = TMR_TMR16B0TCR_COUNTERRESET_ENABLED;

    /* leave the timer disabled */
    TMR_TMR16B0TCR = 0;

    /* interrupt on match 0, reset timer on match, stop timer on match */
    TMR_TMR16B0MCR = TMR_TMR16B0MCR_MR0_INT_ENABLED |TMR_TMR16B0MCR_MR0_RESET_ENABLED | TMR_TMR16B0MCR_MR0_STOP_ENABLED;

    /* set prescale to divide by 7200, 100ns */
    TMR_TMR16B0PR = 7200;

    /* clear any pending interrupts and then enable the interrupt. */
    TMR_TMR16B0IR = TMR_TMR16B0IR_MR0;
    NVIC_EnableIRQ(TIMER_16_0_IRQn);

    /** \todo debug */
    gpioSetPinFunction(2, 7, GPIO_OUTPUT_PIN, 1);
}

void step_timer_start(uint16_t val)
{
    TMR_TMR16B0MR0 = val;
    TMR_TMR16B0TCR = TMR_TMR16B0TCR_COUNTERENABLE_ENABLED;
}

void step_timer_prescale(uint16_t val)
{
    TMR_TMR16B0PR = val;
}

void TIMER16_0_IRQHandler(void)
{
    /* clear the interrupt flag */
    TMR_TMR16B0IR = TMR_TMR16B0IR_MR0;

    uint8_t v = gpioGetPin(2, 7);
    v = (v == 0) ? 1 : 0;
    gpioSetPin(2, 7, v);
}
