#include "core/gpio/gpio.h"
#include "platform.h"

/**
 * \file Implementation of umc platform specific functions for LPC-P1343 with Rowley
 */

uint8_t pf_send_bytes(uint8_t *data, uint16_t size)
{
    return 0;
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
    return 0;
}

void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val)
{
    gpioSetPin(port, pin, val);
}

uint8_t pf_read_port_pin(uint8_t port, uint8_t pin)
{
    return gpioGetPin(port, pin);
}

void pf_set_step_timer(uint16_t val)
{

}

uint8_t pf_is_timer_running()
{
    return 0;
}
