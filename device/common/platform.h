#ifndef ums_platform_h
#define ums_platform_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends bytes back to the host from the device.
 * Non-blocking, returns immediately.
 * Queues all the bytes for sending or none if there isn't room.
 * \return non-zero on success, 0 on error
 */
uint8_t pf_send_bytes(uint8_t *data, uint16_t size);

/**
 * Receives a single byte from the host.
 * \param rxByte Pointer to memory to hold the received byte.
 * \return non-zero if a byte was received, 0 if nothing available.
 */
uint8_t pf_receive_byte(uint8_t *rxByte);


enum ums_pin_func {
    UMS_OUTPUT_PIN,          ///\< Push-pull output.
    UMS_INPUT_PULLUP_PIN,    ///\< Input pin with pull up.
    UMS_INPUT_PULLDOWN_PIN   ///\< Input pin with pull down.
};

/**
 * Configure an i/o pin to have the desired functionality.
 * \return 0 on success, otherwise the pin was not configured because it is reserved.
 */
uint8_t pf_configure_port_pin(uint8_t port, uint8_t pin, enum ums_pin_func func);

/**
 * Set the value of an output pin.
 * \param port Port number.
 * \param pin Pin number.
 * \param val If 0, then the pin is set low.  Otherwise the pin is set high.
 */
void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val);

/**
 * Read the value of an input pin.
 * \return 0 if the pin is low, non-zero if the pin is high.
 */
uint8_t pf_read_port_pin(uint8_t port, uint8_t pin);

/**
 * Set the value for the countdown timer before the next call to \sa st_run_once
 * \param val The time delay in microseconds.  If val is 0, then the timer should
 * be disabled if it is running or ignored if it isn't.  Setting val to 0 should ensure
 * that another interrupt does not occur.
 */
void pf_set_step_timer(uint32_t val);

/**
 * Is the timer currently running?
 * \return 0 if the timer is not counting down and non-zero if it is.
 */
uint8_t pf_is_timer_running();

/**
 * This routine sets up the platform specific default pin configurations for each
 * of the four axes. It should be implemented by creating an \sa AxisCmd and
 * calling \sa st_setup_axis.  It is called from st_init.
 */
void pf_init_axes();


#ifdef __cplusplus
}
#endif

#endif
