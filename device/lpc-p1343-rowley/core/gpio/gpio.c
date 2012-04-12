#include "gpio.h"
#include "lpc134x.h"

static volatile uint32_t *const gpioDIR[]  = {&GPIO_GPIO0DIR,  &GPIO_GPIO1DIR,  &GPIO_GPIO2DIR,  &GPIO_GPIO3DIR };
static volatile uint32_t *const gpioDATA[] = {&GPIO_GPIO0DATA, &GPIO_GPIO1DATA, &GPIO_GPIO2DATA, &GPIO_GPIO3DATA };

//\todo what value to use?
#define RESERVED_PIN (*(pREG32 (0x00000000)))

static volatile uint32_t *const gpioIOCON0[] = {
        &RESERVED_PIN, &IOCON_PIO0_1, &IOCON_PIO0_2, &IOCON_PIO0_3, &RESERVED_PIN, &RESERVED_PIN,
        &IOCON_PIO0_6, &IOCON_PIO0_7, &IOCON_PIO0_8, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN
};

static volatile uint32_t *const gpioIOCON1[] = {
        &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &IOCON_PIO1_4, &IOCON_PIO1_5,
        &IOCON_PIO1_6, &IOCON_PIO1_7, &IOCON_PIO1_8, &IOCON_PIO1_9, &IOCON_PIO1_10
};

static volatile uint32_t *const gpioIOCON2[] = {
        &IOCON_PIO2_0, &IOCON_PIO2_1, &IOCON_PIO2_2, &IOCON_PIO2_3, &IOCON_PIO2_4, &IOCON_PIO2_5,
        &IOCON_PIO2_6, &IOCON_PIO2_7, &IOCON_PIO2_8, &IOCON_PIO2_9, &IOCON_PIO2_10
};

static volatile uint32_t *const gpioIOCON3[] = {
        &IOCON_PIO3_0, &IOCON_PIO3_1, &IOCON_PIO3_2, &IOCON_PIO3_3, &IOCON_PIO3_4, &IOCON_PIO3_5,
        &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN
};

static volatile uint32_t *const *const gpioIOCON[] = { gpioIOCON0, gpioIOCON1, gpioIOCON2, gpioIOCON3 };

void gpioInit(void) {
    /* Enable AHB clock to the GPIO domain. */
    SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_GPIO);
}

void gpioSetPinFunction(uint_fast8_t port, uint_fast8_t pin, gpioPinFunction_t func, uint_fast8_t val)
{
    volatile uint32_t *iocon = gpioIOCON[port][pin];
    if (iocon == &RESERVED_PIN) {
        //\todo error here
        return;
    }

    // set the function to 0 which selects i/o for all the allowed pins
    uint32_t mask = 0x07;
    *iocon = (*iocon) & ~mask;

    // turn off pullup/pulldown, bits 3-4 = 0 means floating
    mask = 0x18;
   *iocon = (*iocon) & ~mask;

    if (func == GPIO_OUTPUT_PIN) {
        *gpioDIR[port] |= (1 << pin);
    } else {
        *gpioDIR[port] &= ~(1 << pin);

        // float, pullup, or pulldown
        switch (func) {
        case GPIO_INPUT_FLOAT_PIN:
            break;

        case GPIO_INPUT_PULLUP_PIN:
            *iocon |= 0x10;
            break;

        case GPIO_INPUT_PULLDOWN_PIN:
            *iocon |= 0x08;
            break;
        }
    }
}

void gpioSetPin(uint_fast8_t port, uint_fast8_t pin, uint_fast8_t val)
{
    uint32_t mask = 1;
    mask <<= pin;
    if (val == 0) {
        *gpioDATA[port] &= (~mask);
    } else {
        *gpioDATA[port] |= mask;
    }
}

uint_fast8_t gpioGetPin(uint_fast8_t port, uint_fast8_t pin)
{
    uint32_t val = *gpioDATA[port];
    uint32_t mask = 1;
    mask <<= pin;

    uint_fast8_t ret = (val & mask) ? 1 : 0;
    return ret;
}

#if 0
/**************************************************************************/
/*! 
 @brief IRQ Handler for GPIO port 0 (currently checks pin 0.1)
 */
/**************************************************************************/
void PIOINT0_IRQHandler(void) {
    uint32_t regVal;

    regVal = gpioIntStatus(0, 1);
    if (regVal) {
        gpioIntClear(0, 1);
    }
    return;
}

/**************************************************************************/
/*! 
 @brief IRQ Handler for GPIO port 1 (currently checks pin 1.1)
 */
/**************************************************************************/
void PIOINT1_IRQHandler(void) {
    uint32_t regVal;

#if defined CFG_ALTRESET && CFG_ALTRESET_PORT == 1
    regVal = gpioIntStatus(CFG_ALTRESET_PORT, CFG_ALTRESET_PIN);
    if (regVal)
    {
        // Cause a reset and wait
        cpuReset();
    }
#endif

#ifdef CFG_CHIBI
    // Check for interrupt on 1.8
    regVal = gpioIntStatus(1, 8);
    if (regVal)
    {
        chibi_counter++;
        chb_ISR_Handler();
        gpioIntClear(1, 8);
    }
#else
    regVal = gpioIntStatus(1, 1);
    if (regVal) {
        gpioIntClear(1, 1);
    }
#endif

    return;
}

/**************************************************************************/
/*! 
 @brief IRQ Handler for GPIO port 2 (currently checks pin 2.1)
 */
/**************************************************************************/
void PIOINT2_IRQHandler(void) {
    uint32_t regVal;

    regVal = gpioIntStatus(2, 1);
    if (regVal) {
        gpioIntClear(2, 1);
    }
    return;
}

/**************************************************************************/
/*! 
 @brief IRQ Handler for GPIO port 3 (currently checks pin 3.1)
 */
/**************************************************************************/
void PIOINT3_IRQHandler(void) {
    uint32_t regVal;

    regVal = gpioIntStatus(3, 1);
    if (regVal) {
        gpioIntClear(3, 1);
    }
    return;
}

/**************************************************************************/
/*! 
 @brief Initialises GPIO and enables the GPIO interrupt
 handler for all GPIO ports.
 */
/**************************************************************************/

/**************************************************************************/
/*! 
 @brief Sets the direction (input/output) for a specific port pin

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..11)
 @param[in]  dir
 The pin direction (gpioDirection_Input or
 gpioDirection_Output)
 */
/**************************************************************************/
void gpioSetDir(uint32_t portNum, uint32_t bitPos, gpioDirection_t dir) {
    if (!_gpioInitialised)
        gpioInit();

    // Get the appropriate register (handled this way to optimise code size)
    REG32 *gpiodir = &GPIO_GPIO0DIR;
    switch (portNum) {
    case 0:
        gpiodir = &GPIO_GPIO0DIR;
        break;
    case 1:
        gpiodir = &GPIO_GPIO1DIR;
        break;
    case 2:
        gpiodir = &GPIO_GPIO2DIR;
        break;
    case 3:
        gpiodir = &GPIO_GPIO3DIR;
        break;
    }

    // Toggle dir
    dir == gpioDirection_Output ?
            (*gpiodir |= (1 << bitPos)) : (*gpiodir &= ~(1 << bitPos));
}

/**************************************************************************/
/*! 
 @brief Gets the value for a specific port pin

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)

 @return     The current value for the specified port pin (0..1)
 */
/**************************************************************************/
uint32_t gpioGetValue(uint32_t portNum, uint32_t bitPos) {
    if (!_gpioInitialised)
        gpioInit();

    uint32_t value = 0;

    switch (portNum) {
    case 0:
        value = (GPIO_GPIO0DATA & (1 << bitPos)) ? 1 : 0;
        break;
    case 1:
        value = (GPIO_GPIO1DATA & (1 << bitPos)) ? 1 : 0;
        break;
    case 2:
        value = (GPIO_GPIO2DATA & (1 << bitPos)) ? 1 : 0;
        break;
    case 3:
        value = (GPIO_GPIO3DATA & (1 << bitPos)) ? 1 : 0;
        break;
    default:
        break;
    }

    return value;
}

/**************************************************************************/
/*! 
 @brief Sets the value for a specific port pin (only relevant when a
 pin is configured as output).

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)
 @param[in]  bitValue
 The value to set for the specified bit (0..1).  0 will set
 the pin low and 1 will set the pin high.
 */
/**************************************************************************/
void gpioSetValue(uint32_t portNum, uint32_t bitPos, uint32_t bitVal) {
    if (!_gpioInitialised)
        gpioInit();

    // Get the appropriate register (handled this way to optimise code size)
    REG32 *gpiodata = &GPIO_GPIO0DATA;
    switch (portNum) {
    case 0:
        gpiodata = &GPIO_GPIO0DATA;
        break;
    case 1:
        gpiodata = &GPIO_GPIO1DATA;
        break;
    case 2:
        gpiodata = &GPIO_GPIO2DATA;
        break;
    case 3:
        gpiodata = &GPIO_GPIO3DATA;
        break;
    }

    // Toggle value
    bitVal == 1 ? (*gpiodata |= (1 << bitPos)) : (*gpiodata &= ~(1 << bitPos));
}

/**************************************************************************/
/*! 
 @brief Sets the interrupt sense, event, etc.

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)
 @param[in]  sense
 Whether the interrupt should be configured as edge or level
 sensitive.
 @param[in]  edge
 Whether one edge or both trigger an interrupt.
 @param[in]  event
 Whether the rising or the falling edge (high or low)
 should be used to trigger the interrupt.

 @section Example

 @code
 // Initialise gpio
 gpioInit();
 // Set GPIO1.8 to input
 gpioSetDir(1, 8, gpioDirection_Input);
 // Disable the internal pullup/down resistor on P1.8
 gpioSetPullup (&IOCON_PIO1_8, gpioPullupMode_Inactive);
 // Setup an interrupt on GPIO1.8
 gpioSetInterrupt(1,                               // Port
 8,                               // Pin
 gpioInterruptSense_Edge,         // Edge/Level Sensitive
 gpioInterruptEdge_Single,        // Single/Double Edge
 gpioInterruptEvent_ActiveHigh);  // Rising/Falling
 // Enable the interrupt
 gpioIntEnable(1, 8);
 @endcode
 */
/**************************************************************************/
void gpioSetInterrupt(uint32_t portNum, uint32_t bitPos,
        gpioInterruptSense_t sense, gpioInterruptEdge_t edge,
        gpioInterruptEvent_t event) {
    if (!_gpioInitialised)
        gpioInit();

    // Get the appropriate register (handled this way to optimise code size)
    REG32 *gpiois = &GPIO_GPIO0IS; // Interrupt sense (edge or level sensitive)
    REG32 *gpioibe = &GPIO_GPIO0IBE; // Interrupt both edges (0 = int controlled by GPIOIEV, 1 = both edges trigger interrupt)
    REG32 *gpioiev = &GPIO_GPIO0IEV; // 0 = falling edge or low, 1 = rising edge or high (depending on GPIOIS)
    switch (portNum) {
    case 0:
        gpiois = &GPIO_GPIO0IS;
        gpioibe = &GPIO_GPIO0IBE;
        gpioiev = &GPIO_GPIO0IEV;
        break;
    case 1:
        gpiois = &GPIO_GPIO1IS;
        gpioibe = &GPIO_GPIO1IBE;
        gpioiev = &GPIO_GPIO1IEV;
        break;
    case 2:
        gpiois = &GPIO_GPIO2IS;
        gpioibe = &GPIO_GPIO2IBE;
        gpioiev = &GPIO_GPIO2IEV;
        break;
    case 3:
        gpiois = &GPIO_GPIO3IS;
        gpioibe = &GPIO_GPIO3IBE;
        gpioiev = &GPIO_GPIO3IEV;
        break;
    }

    if (sense == gpioInterruptSense_Edge) {
        *gpiois &= ~(0x1 << bitPos);
        edge == gpioInterruptEdge_Single ?
                (*gpioibe &= ~(0x1 << bitPos)) : (*gpioibe |= (0x1 << bitPos));
    } else {
        *gpiois |= (0x1 << bitPos);
    }

    event == gpioInterruptEvent_ActiveHigh ?
            (*gpioiev &= ~(0x1 << bitPos)) : (*gpioiev |= (0x1 << bitPos));

    return;
}

/**************************************************************************/
/*! 
 @brief Enables the interrupt mask for a specific port pin

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)
 */
/**************************************************************************/
void gpioIntEnable(uint32_t portNum, uint32_t bitPos) {
    if (!_gpioInitialised)
        gpioInit();

    switch (portNum) {
    case 0:
        GPIO_GPIO0IE |= (0x1 << bitPos);
        break;
    case 1:
        GPIO_GPIO1IE |= (0x1 << bitPos);
        break;
    case 2:
        GPIO_GPIO2IE |= (0x1 << bitPos);
        break;
    case 3:
        GPIO_GPIO3IE |= (0x1 << bitPos);
        break;
    default:
        break;
    }
    return;
}

/**************************************************************************/
/*! 
 @brief Disables the interrupt mask for a specific port pin

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)
 */
/**************************************************************************/
void gpioIntDisable(uint32_t portNum, uint32_t bitPos) {
    if (!_gpioInitialised)
        gpioInit();

    switch (portNum) {
    case 0:
        GPIO_GPIO0IE &= ~(0x1 << bitPos);
        break;
    case 1:
        GPIO_GPIO1IE &= ~(0x1 << bitPos);
        break;
    case 2:
        GPIO_GPIO2IE &= ~(0x1 << bitPos);
        break;
    case 3:
        GPIO_GPIO3IE &= ~(0x1 << bitPos);
        break;
    default:
        break;
    }
    return;
}

/**************************************************************************/
/*! 
 @brief Gets the interrupt status for a specific port pin

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)

 @return     The interrupt status for the specified port pin (0..1)
 */
/**************************************************************************/
uint32_t gpioIntStatus(uint32_t portNum, uint32_t bitPos) {
    if (!_gpioInitialised)
        gpioInit();

    uint32_t regVal = 0;

    switch (portNum) {
    case 0:
        if (GPIO_GPIO0MIS & (0x1 << bitPos)) {
            regVal = 1;
        }
        break;
    case 1:
        if (GPIO_GPIO1MIS & (0x1 << bitPos)) {
            regVal = 1;
        }
        break;
    case 2:
        if (GPIO_GPIO2MIS & (0x1 << bitPos)) {
            regVal = 1;
        }
        break;
    case 3:
        if (GPIO_GPIO3MIS & (0x1 << bitPos)) {
            regVal = 1;
        }
        break;
    default:
        break;
    }
    return (regVal);
}

/**************************************************************************/
/*! 
 @brief Clears the interrupt for a port pin

 @param[in]  portNum
 The port number (0..3)
 @param[in]  bitPos
 The bit position (0..31)
 */
/**************************************************************************/
void gpioIntClear(uint32_t portNum, uint32_t bitPos) {
    if (!_gpioInitialised)
        gpioInit();

    switch (portNum) {
    case 0:
        GPIO_GPIO0IC |= (0x1 << bitPos);
        break;
    case 1:
        GPIO_GPIO1IC |= (0x1 << bitPos);
        break;
    case 2:
        GPIO_GPIO2IC |= (0x1 << bitPos);
        break;
    case 3:
        GPIO_GPIO3IC |= (0x1 << bitPos);
        break;
    default:
        break;
    }
    return;
}

/**************************************************************************/
/*! 
 @brief Configures the internal pullup/down resistor for GPIO pins
 (only relevant for pins configured as inputs)

 @param[in]  ioconReg
 A pointer to the IOCON registry value corresponding to
 the pin you wish to change (for example: &IOCON_PIO2_0
 for GPIO pin 2.0).
 @param[in]  mode
 The 'mode' that the pin should be set to, which must be
 correspond to a value defined in gpioPullupMode_t
 
 @warning    By default, all GPIO pins have the internal pull-up
 resistor enabled.  This may cause unusual behaviour if
 care isn't taken to set the internal resistor to an
 appropriate state.

 @section Example

 @code
 // Initialise gpio
 gpioInit();
 // Set GPIO1.8 to input
 gpioSetDir(1, 8, gpioDirection_Input);
 // Disable the internal pullup/down resistor on P1.8
 gpioSetPullup(&IOCON_PIO1_8, gpioPullupMode_Inactive);
 @endcode
 */
/**************************************************************************/
void gpioSetPullup(volatile uint32_t *ioconReg, gpioPullupMode_t mode) {
    if (!_gpioInitialised)
        gpioInit();

    // ToDo: Disable interrupts while we are doing this?

    *ioconReg &= ~(IOCON_COMMON_MODE_MASK);
    *ioconReg |= mode;

    // ToDo: Re-enable interrupts?
};
#endif
