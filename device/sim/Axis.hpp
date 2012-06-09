#ifndef ums_Axis_hpp
#define ums_Axis_hpp

#include <boost/array.hpp>
#include "commands.h"

namespace ums {

/** Description of a simulated pin. */
struct Pin {

	/** Default constructor creates an unassigned pin. */
	Pin() : port_(UMS_UNASSIGNED_PORT) {}

	/**
	 * Configure a pin.  Assign to the specified port and pin.
	 * Specify if the pin is inverted or not.
	 * \param port The i/o port that the pin belongs to.  The value \sa UMS_UNASSIGNED_PORT is reserved
	 * to indicate a pin that is not assigned to a physical pin.
	 * \param pin The pin number of the i/o port that this pin is assigned to.
	 * Valid values are 0 - 15.  If the \sa UMS_INVERT_PIN
	 * bit is set, then writing a 1 to the pin sets the physical pin low.
	 */
	void configure(uint8_t port, int8_t pin) {
		port_ = port;
		if ((pin & UMS_INVERT_PIN) != 0) {
			pin_ = pin - UMS_INVERT_PIN;
			inverted_ = true;
		} else {
			pin_ = pin;
			inverted_ = false;
		}
	}

	/** \return Returns true if this pin is not assigned to a physical pin. */
	bool unassigned() const { return (port_ == UMS_UNASSIGNED_PORT); }

	/**
	 * Returns the logical value of the pin.  Inverts the physical value if
	 * necessary.  Unassigned pins always return 0;
	 */
	const bool operator()(boost::array<bool, 256> &pins) const {
		if (unassigned()) return 0;
		else {
			return inverted() ? !pins[index()] : pins[index()];
		}
	}

	/**
	 * Set the value of the physical pin.  This has no effect if the pin is unassigned.
	 * The value set is inverted if necessary.
	 */
	void operator()(boost::array<bool, 256> &pins, bool val) {
		if (!unassigned()) {
			pins[index()] = inverted() ? !val : val;
		}
	}

	/**
	 * Assume pins are represented as boost::array<bool, 256> and map pins by
	 * index = port * 16 + pin;
	 */
	uint8_t index() const { return static_cast<uint8_t>(port_ * 16 + pin_); }

	bool inverted() const { return inverted_; }
	uint8_t port() const { return port_; }
	uint8_t pin() const { return pin_; }

	/** \return The value of the pin property of a command which is pin number with invert bit. */
	uint8_t cmdPin() const { return inverted_ ? pin_ + UMS_INVERT_PIN : pin_; }

private:
	uint8_t port_;
	uint8_t pin_;
	bool inverted_;
};

/** Simulates a single axis as a stepper motor connected to physical pins. */
struct Axis {
	Axis() : maxFwdPos_(INT_MAX), maxRevPos_(INT_MIN), lastStep_(false), position_(0), enabled_(true)
	{}

	Pin step_;
	Pin dir_;
	Pin fwdLimit_;
	Pin revLimit_;
	ptrdiff_t maxFwdPos_;
	ptrdiff_t maxRevPos_;
	bool lastStep_;
	ptrdiff_t position_;
	bool enabled_;

	/**
	 * Called whenever the pins change.  Updates the position if necessary;
	 */
	void update(boost::array<bool, 256> &pins) {
		// nothing to do if step dir not assigned
		if (step_.unassigned() || dir_.unassigned()) return;

		// is there an edge that causes motor to step?
		if (!lastStep_ && step_(pins) == true) {
			lastStep_ = true;
			bool dir = dir_(pins);
			position_ = dir ? position_ + 1 : position_ - 1;

			// reverse limit
			if (position_ < maxRevPos_) {
				position_ = maxRevPos_;
				revLimit_(pins, true);
				std::cout << "limited\n";
			} else {
				revLimit_(pins, false);
			}

			// fwd limit
			if (position_ > maxFwdPos_) {
				position_ = maxFwdPos_;
				fwdLimit_(pins, true);
				std::cout << "limited\n";
			} else {
				fwdLimit_(pins, false);
			}
		}
		lastStep_ = step_(pins);
	}

	void zeroPins(boost::array<bool, 256> &pins)
	{
		step_(pins, false);
		dir_(pins, false);
		fwdLimit_(pins, false);
		revLimit_(pins, false);
	}

	/**
	 * Generate a command to send to the ums that matches the simulator configuration.
	 */
	AxisCmd command() const
	{
		AxisCmd ac;
		ac.name = ' ';
		ac.cmdId = AxisCmd_ID;
		ac.stepPort = step_.port();
		ac.stepPin  = step_.cmdPin();
		ac.dirPort  = dir_.port();
		ac.dirPin   = dir_.cmdPin();
		ac.fwdPort  = fwdLimit_.port();
		ac.fwdPin   = fwdLimit_.cmdPin();
		ac.revPort  = revLimit_.port();
		ac.revPin   = revLimit_.cmdPin();

		return ac;
	}

};

}

#endif
