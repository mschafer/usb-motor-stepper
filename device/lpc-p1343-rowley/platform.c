#include "core/gpio/gpio.h"
#include "core/usbcdc/cdcuser.h"
#include "step_timer.h"
#include "platform.h"

/**
 * \file Implementation of ums platform specific functions for LPC-P1343 with Rowley
 */

uint8_t pf_send_bytes(uint8_t *data, uint16_t size)
{
	CDC_BlockingWriteInEp(data, size);
	return 0;
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
	return CDC_ReadByte(rxByte);
}

void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val)
{
    gpioSetPin(port, pin, val);
}

uint8_t pf_read_port_pin(uint8_t port, uint8_t pin)
{
    return gpioGetPin(port, pin);
}

void pf_set_step_timer(uint32_t val)
{
	step_timer_start(val);
}

uint8_t pf_is_timer_running()
{
    return step_timer_enabled();
}
