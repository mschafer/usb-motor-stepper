#include "Platform.hpp"
#include "platform.h"
#include "umc.h"
#include "stepper.h"

namespace ums {

const int Platform::TO_BUFFER_SIZE = 1024;
const int Platform::FROM_BUFFER_SIZE = 2048;

Platform *Platform::thePlatform_ = NULL;

Platform::Platform() : toHost_(TO_BUFFER_SIZE), fromHost_(FROM_BUFFER_SIZE), t_(0)
{
	umc_init();
}

Platform &
Platform::instance()
{
	if (thePlatform_ == NULL) {
	    thePlatform_ = new Platform();
	}
	return *thePlatform_;
}

void
Platform::reset()
{
    if (thePlatform_ != NULL) {
        delete (thePlatform_);
        thePlatform_ = NULL;
    }
}

void
Platform::runOnce()
{
	umc_idle();
	if (timerRunning()) {
		t_ += delay_.get();
		st_run_once();
		delay_.reset();
	}
}

boost::optional<Axis>
Platform::axis(char name) const
{
	boost::optional<Axis> ret;
	axis_map_t::const_iterator it = axes_.find(name);
	if (it != axes_.end()) {
		ret = it->second;
	}
	return ret;
}

void
Platform::axis(char name, Axis &a)
{
	if (name != 'X' && name != 'Y' && name != 'Z' && name != 'U') {
		throw std::range_error("Axis name must be X,Y,Z, or U");
	}
	Axis &v = axes_[name];
	v.zeroPins(pins_);
	v = a;
	v.update(pins_);
}

void
Platform::removeAxis(char name)
{
	axis_map_t::iterator it = axes_.find(name);
	if (it != axes_.end()) {
		it->second.zeroPins(pins_);
		axes_.erase(it);
	}
}

AxisCmd
Platform::axisCommand(char name)
{
	if (name != 'X' && name != 'Y' && name != 'Z' && name != 'U') {
		throw std::range_error("Axis name must be X,Y,Z, or U");
	}
	Axis &a = axes_.at(name);
	AxisCmd ac = a.command();
	ac.name = name;
	return ac;
}

void
Platform::portPin(uint8_t addr, uint8_t val)
{
	pins_[addr] = (val != 0);
	axis_map_t::iterator  it = axes_.begin();
	while (it != axes_.end()) {
		it->second.update(pins_);
		it++;
	}
}

uint8_t
Platform::portPin(uint8_t addr)
{
	return pins_[addr];
}

bool
Platform::timerRunning()
{
	return (bool)delay_;
}

void Platform::timerDelay(uint16_t delay)
{
	delay_ = delay;
}

}

///////////////////////////////////////// C API //////////////////////////////////////////

uint8_t pf_send_bytes(uint8_t *data, uint16_t size)
{
	ums::Platform &platform = ums::Platform::instance();

	// is there enough room?
	if (platform.toHost_.reserve() < size) {
		return 0;
	} else {
		for (int i=0; i<size; i++) {
			platform.toHost_.push_back(data[i]);
		}
		return 1;
	}
}

uint16_t pf_bytes_available()
{
	ums::Platform &platform = ums::Platform::instance();
	return static_cast<uint16_t>(platform.fromHost_.reserve());
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
	ums::Platform &platform = ums::Platform::instance();
	if (platform.fromHost_.empty()) {
		return 0;
	} else {
		*rxByte = platform.fromHost_.front();
		platform.fromHost_.pop_front();
		return 1;
	}
}

void pf_configure_port_pin(uint8_t port, uint8_t pin, enum umc_pin_func func)
{
    ///\todo mimic configured pins in simulator?
}

void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val)
{
	ums::Platform &platform = ums::Platform::instance();
	uint8_t addr = (port << 4) | pin;
	platform.portPin(addr, val != 0);
}

uint8_t pf_read_port_pin(uint8_t port, uint8_t pin)
{
	ums::Platform &platform = ums::Platform::instance();
	uint8_t addr = (port << 4) | pin;
	return platform.portPin(addr);
}

void pf_set_step_timer(uint16_t delay)
{
	ums::Platform &platform = ums::Platform::instance();
	platform.timerDelay(delay);
}

uint8_t pf_is_timer_running()
{
	ums::Platform &platform = ums::Platform::instance();
	if (platform.timerRunning())
		return 1;
	else
		return 0;
}

void pf_reset()
{
    ums::Platform &platform = ums::Platform::instance();
    platform.reset();
}
