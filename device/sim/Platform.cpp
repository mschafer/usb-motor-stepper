#include "Platform.hpp"
#include "ums.h"
#include "stepper.h"
#include <boost/foreach.hpp>


namespace ums { namespace sim {

Platform *Platform::thePlatform_ = NULL;

Platform::Platform() : t_(0)
{
}

Platform &
Platform::instance()
{
	if (thePlatform_ == NULL) {
	    thePlatform_ = new Platform();
		ums_init();
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

void
Platform::write(std::vector<uint8_t> &bytes)
{
	boost::lock_guard<boost::mutex> guard(hostCommMutex_);
	fromHost_.insert(fromHost_.end(), bytes.begin(), bytes.end());
}

void
Platform::write(void *bytes, size_t nBytes)
{
	boost::lock_guard<boost::mutex> guard(hostCommMutex_);
	uint8_t *pBytes = reinterpret_cast<uint8_t*>(bytes);
	fromHost_.insert(fromHost_.end(), pBytes, pBytes+nBytes);
}

std::deque<uint8_t>
Platform::read()
{
	boost::lock_guard<boost::mutex> guard(hostCommMutex_);
	std::deque<uint8_t> ret;
	ret.swap(toHost_);
	return ret;
}

boost::optional<uint8_t>
Platform::readByte()
{
	boost::lock_guard<boost::mutex> guard(hostCommMutex_);
	boost::optional<uint8_t> ret;
	if (!toHost_.empty()) {
		ret = toHost_.front();
		toHost_.pop_front();
	}
	return ret;
}

}}

///////////////////////////////////////// C API //////////////////////////////////////////

using ums::sim::Platform;

uint8_t pf_send_bytes(uint8_t *data, uint16_t size)
{
	Platform &platform = Platform::instance();
	boost::lock_guard<boost::mutex> guard(platform.hostCommMutex_);
	for (size_t i=0; i<size; i++) {
		platform.toHost_.push_back(data[i]);
	}
	return 1;
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
	Platform &platform = Platform::instance();
	boost::lock_guard<boost::mutex> guard(platform.hostCommMutex_);
	if (platform.fromHost_.empty()) {
		return 0;
	} else {
		*rxByte = platform.fromHost_.front();
		platform.fromHost_.pop_front();
		return 1;
	}
}

uint8_t pf_configure_port_pin(uint8_t port, uint8_t pin, enum ums_pin_func func)
{
	// port 6 is reserved to test error messages
	if (port == 6 || pin > 15) return 1;

	Platform &platform = Platform::instance();
	uint8_t addr = (port << 4) | pin;
	switch (func) {
	case UMS_INPUT_PULLUP_PIN:
		platform.portPin(addr, true);
		break;
	case UMS_INPUT_PULLDOWN_PIN:
		platform.portPin(addr, false);
		break;
	}
	return 0;
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

void pf_set_step_timer(uint32_t delay)
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

void pf_init_axes()
{
	Platform &platform = Platform::instance();

	uint8_t port = 0;
	ums::Axis axis;
	axis.step_.configure(port, 0);
	axis.dir_.configure(port, 1);
	axis.fwdLimit_.configure(port, 2);
	axis.revLimit_.configure(port, 3 + UMS_INVERT_PIN);
	platform.axis('X', axis);
	struct AxisCmd ac = platform.axisCommand('X');
	st_setup_axis(&ac);

	port = 1;
	axis.step_.configure(port, 0);
	axis.dir_.configure(port, 1);
	axis.fwdLimit_.configure(port, 2 + UMS_INVERT_PIN);
	axis.revLimit_.configure(port, 3);
	platform.axis('Y', axis);
	ac = platform.axisCommand('Y');
	st_setup_axis(&ac);

	port = 2;
	axis.step_.configure(port, 0);
	axis.dir_.configure(port, 1 + UMS_INVERT_PIN);
	axis.fwdLimit_.configure(port, 2);
	axis.revLimit_.configure(port, 3);
	platform.axis('Z', axis);
	ac = platform.axisCommand('Z');
	st_setup_axis(&ac);

	port = 3;
	axis.step_.configure(port, 0 + UMS_INVERT_PIN);
	axis.dir_.configure(port, 1);
	axis.fwdLimit_.configure(port, 2);
	axis.revLimit_.configure(port, 3);
	platform.axis('U', axis);
	ac = platform.axisCommand('U');
	st_setup_axis(&ac);
}
