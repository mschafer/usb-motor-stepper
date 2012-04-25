#include "Platform.hpp"
#include "platform.h"
#include "ums.h"
#include "stepper.h"
#include <boost/foreach.hpp>

namespace ums { namespace sim {

Platform *Platform::thePlatform_ = NULL;

Platform::Platform() : t_(0)
{
	fromHost_.clear();
	toHost_.clear();
	ums_init();
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
	ums_idle();
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

void Platform::write(std::vector<uint8_t> &bytes)
{
	BOOST_FOREACH(uint8_t b, bytes) {
		toHost_.push_back(b);
	}
}

std::deque<uint8_t> Platform::read()
{
	std::deque<uint8_t> ret;
	ret.swap(fromHost_);
	return ret;
}

boost::optional<uint8_t> Platform::readByte()
{
	boost::optional<uint8_t> ret;
	if (fromHost_.size() > 0) {
		ret = fromHost_.front();
		fromHost_.pop_front();
	}
	return ret;
}

}}

///////////////////////////////////////// C API //////////////////////////////////////////

using ums::sim::Platform;

uint8_t pf_send_bytes(uint8_t *data, uint16_t size)
{
	Platform &platform = Platform::instance();

	for (size_t i=0; i<size; i++) {
		platform.toHost_.push_back(data[i]);
	}
	return 1;
}

uint16_t pf_bytes_available()
{
	Platform &platform = Platform::instance();
	return static_cast<uint16_t>(platform.fromHost_.size());
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
	Platform &platform = Platform::instance();
	if (platform.fromHost_.empty()) {
		return 0;
	} else {
		*rxByte = platform.fromHost_.front();
		platform.fromHost_.pop_front();
		return 1;
	}
}

void pf_configure_port_pin(uint8_t port, uint8_t pin, enum ums_pin_func func)
{
    ///\todo mimic configured pins in simulator?
}

void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val)
{
	Platform &platform = Platform::instance();
	uint8_t addr = (port << 4) | pin;
	platform.portPin(addr, val != 0);
}

uint8_t pf_read_port_pin(uint8_t port, uint8_t pin)
{
	Platform &platform = Platform::instance();
	uint8_t addr = (port << 4) | pin;
	return platform.portPin(addr);
}

void pf_set_step_timer(uint16_t delay)
{
	Platform &platform = Platform::instance();
	platform.timerDelay(delay);
}

uint8_t pf_is_timer_running()
{
	Platform &platform = Platform::instance();
	if (platform.timerRunning())
		return 1;
	else
		return 0;
}

void pf_reset()
{
	Platform &platform = Platform::instance();
    platform.reset();
}
