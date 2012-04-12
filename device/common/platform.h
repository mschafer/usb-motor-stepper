#ifndef umc_platform_h
#define umc_platform_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends bytes back to the host from the device.
 * \return non-zero on success, 0 on error
 */
uint8_t pf_send_bytes(uint8_t *data, uint16_t size);

/**
 * Receives a single byte from the host.
 * \param rxByte Pointer to memory to hold the received byte.
 * \return non-zero if a byte was received, 0 if nothing available.
 */
uint8_t pf_receive_byte(uint8_t *rxByte);


enum umc_pin_func {
    UMC_OUTPUT_PIN,          ///\< Push-pull output.
    UMC_INPUT_PULLUP_PIN,    ///\< Input pin with pull up.
    UMC_INPUT_PULLDOWN_PIN   ///\< Input pin with pull down.
};

/**
 * Configure an i/o pin to have the desired functionality.
 */
void pf_configure_port_pin(uint8_t port, uint8_t pin, enum umc_pin_func func);

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
 * \param val The time delay in microseconds.
 */
void pf_set_step_timer(uint16_t val);

/**
 * Is the timer currently running?
 * \return 0 if the timer is not counting down and non-zero if it is.
 */
uint8_t pf_is_timer_running();

/**
 * Reset the system to its power on state.
 * May be called by internal logic when something terminal has happened.
 */
void pf_reset();

#ifdef __cplusplus
}
#endif

#endif
