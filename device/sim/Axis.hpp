#ifndef ums_Axis_hpp
#define ums_Axis_hpp

#include <boost/array.hpp>
#include "commands.h"

namespace ums {

struct Pin {
	Pin() : port_(UMS_UNASSIGNED_PORT) {}

	void configure(uint8_t port, int8_t pin) {
		port_ = port;
		if (pin > UMS_INVERT_PIN) {
			pin_ = pin - UMS_INVERT_PIN;
			inverted_ = true;
		} else {
			pin_ = pin;
			inverted_ = false;
		}
	}

	bool unassigned() const { return (port_ == UMS_UNASSIGNED_PORT); }
	bool &operator()(boost::array<bool, 256> &pins) const { return pins[index()]; }
	void operator()(boost::array<bool, 256> &pins, bool val) const { pins[index()] = val; }
	uint8_t index() const { return static_cast<uint8_t>(port_ * 16 + pin_); }
	bool inverted() const { return inverted_; }
	uint8_t port() const { return port_; }
	uint8_t pin() const { return pin_; }
	uint8_t cmdPin() const { return inverted_ ? pin_ + UMS_INVERT_PIN : pin_; }

private:
	uint8_t port_;
	uint8_t pin_;
	bool inverted_;
};

struct Axis {
	Axis() : maxFwdPos_(INT_MAX), maxRevPos_(INT_MIN), lastStep_(false), position_(0)
	{}

	Pin step_;
	Pin dir_;
	Pin fwdLimit_;
	Pin revLimit_;
	ptrdiff_t maxFwdPos_;
	ptrdiff_t maxRevPos_;
	bool lastStep_;
	ptrdiff_t position_;

	/**
	 * Called whenever the pins change.  Updates the position if necessary;
	 */
	void update(boost::array<bool, 256> &pins) {
		// nothing to do if step dir not assigned
		if (step_.unassigned() || dir_.unassigned()) return;

		// is there an edge that causes motor to step?
		if (step_(pins) != lastStep_ && step_(pins) == !step_.inverted()) {
			lastStep_ = step_(pins);
			bool dir = dir_(pins);
			dir = dir_.inverted() ? !dir : dir;
			position_ = dir ? position_ + 1 : position_ - 1;

			if (position_ < maxRevPos_) {
				position_ = maxRevPos_;
				if (!revLimit_.unassigned()) {
					revLimit_(pins) = !revLimit_.inverted();
				}
			} else if (!revLimit_.unassigned()) {
				revLimit_(pins) = revLimit_.inverted();
			}

			if (position_ > maxFwdPos_) {
				position_ = maxFwdPos_;
				if (!fwdLimit_.unassigned()) {
					fwdLimit_(pins) = !fwdLimit_.inverted();
				}
			} else if (!fwdLimit_.unassigned()){
				fwdLimit_(pins) = fwdLimit_.inverted();
			}
		}
	}

	void zeroPins(boost::array<bool, 256> &pins)
	{
		if (!step_.unassigned()) step_(pins) = 0;
		if (!dir_.unassigned()) dir_(pins) = 0;
		if (!fwdLimit_.unassigned()) fwdLimit_(pins) = 0;
		if (!revLimit_.unassigned()) revLimit_(pins) = 0;
	}

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
