#include "Platform.hpp"
#include "ums.h"
#include "stepper.h"
#include <boost/foreach.hpp>


namespace ums {

Simulator *Simulator::thePlatform_ = NULL;

Simulator::Simulator(ILink::handle link) :
		link_(link), t_(0)
{
	std::fill(pins_.begin(), pins_.end(), false);
}

Simulator &
Simulator::instance()
{
	if (thePlatform_ == NULL) {
		throw std::runtime_error("Simulator has not been reset");
	}
	return *thePlatform_;
}

void
Simulator::reset(ILink::handle link)
{
    if (thePlatform_ != NULL) {
        delete (thePlatform_);
    }
    thePlatform_ = new Simulator(link);
	ums_init();
}

void
Simulator::runOnce()
{
	ums_run_once();
	if (timerRunning()) {
		t_ += delay_.get();
		delay_.reset();
		st_run_once();
		position_t p;
		p[0] = umsXPos;
		p[1] = umsYPos;
		p[2] = umsZPos;
		p[3] = umsUPos;
		p[4] = umsRunTime;

#if 0
		for (size_t i=0; i<END_AXIS; i++) {
			p[i] = axes_[i].position_;
		}
		p[END_AXIS] = t_;
#endif

		positionLog_.push_back(p);
	}
}

void
Simulator::axis(char name, Axis &a)
{
	Axis &old = axis(name);

	old.zeroPins(pins_);
	old = a;
	old.update(pins_);
}

void
Simulator::disableAxis(char name)
{
	axis(name).enabled_ = false;
}

AxisCmd
Simulator::axisCommand(char name)
{
	AxisCmd ac = axis(name).command();
	ac.name = name;
	return ac;
}

void
Simulator::portPin(uint8_t addr, uint8_t val)
{
	pins_[addr] = (val != 0);
	for (size_t i=0; i<END_AXIS; i++) {
		if (axes_[i].enabled_) {
			axes_[i].update(pins_);
		}
	}
}

uint8_t
Simulator::portPin(uint8_t addr)
{
	return pins_[addr];
}

bool
Simulator::timerRunning()
{
	return (bool)delay_;
}

void Simulator::timerDelay(uint32_t delay)
{
	if (delay != 0)
		delay_ = delay;
	else
		delay_.reset();
}

Axis &
Simulator::axis(char name)
{
	switch (name) {
	case 'X':
		return axes_[X_IDX];
		break;
	case 'Y':
		return axes_[Y_IDX];
		break;
	case 'Z':
		return axes_[Z_IDX];
		break;
	case 'U':
		return axes_[U_IDX];
		break;
	default:
		throw std::range_error("Axis name must be X,Y,Z, or U");
	}
}

const Axis &
Simulator::axis(char name) const
{
	switch (name) {
	case 'X':
		return axes_[X_IDX];
		break;
	case 'Y':
		return axes_[Y_IDX];
		break;
	case 'Z':
		return axes_[Z_IDX];
		break;
	case 'U':
		return axes_[U_IDX];
		break;
	default:
		throw std::range_error("Axis name must be X,Y,Z, or U");
	}
}

void
Simulator::writeToHost(const uint8_t *data, uint16_t size)
{
	std::vector<uint8_t> bytes;
	bytes.insert(bytes.end(), data, data+size);
	link_->write(bytes);
}

boost::optional<uint8_t>
Simulator::readHostByte()
{
	return link_->readByte();
}

}

std::ostream &operator<<(std::ostream &out, const ums::Simulator::position_t &pos)
{
	BOOST_FOREACH(ptrdiff_t d, pos) {
		out << d << " ";
	}
	return out;
}

///////////////////////////////////////// C API //////////////////////////////////////////

using ums::Simulator;

uint8_t pf_send_bytes(const uint8_t *data, uint16_t size)
{
	Simulator &sim = Simulator::instance();
	sim.writeToHost(data, size);
	return 1;
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
	Simulator &sim = Simulator::instance();
	boost::optional<uint8_t> o = sim.readHostByte();
	if (o) {
		*rxByte = o.get();
		return 1;
	} else {
		return 0;
	}
}

uint8_t pf_configure_port_pin(uint8_t port, uint8_t pin, enum ums_pin_func func)
{
	// port 6 is reserved to test error messages
	if (port == 6 || pin > 15) return 1;

	Simulator &sim = Simulator::instance();
	uint8_t addr = (port << 4) | pin;
	switch (func) {
	case UMS_INPUT_PULLUP_PIN:
		sim.portPin(addr, true);
		break;
	case UMS_INPUT_PULLDOWN_PIN:
		sim.portPin(addr, false);
		break;
	}
	return 0;
}

void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val)
{
	Simulator &sim = Simulator::instance();
	uint8_t addr = (port << 4) | pin;
	sim.portPin(addr, val != 0);
}

uint8_t pf_read_port_pin(uint8_t port, uint8_t pin)
{
	Simulator &sim = Simulator::instance();
	uint8_t addr = (port << 4) | pin;
	return sim.portPin(addr);
}

void pf_set_step_timer(uint32_t delay)
{
	Simulator &sim = Simulator::instance();
	sim.timerDelay(delay);
}

uint8_t pf_is_timer_running()
{
	Simulator &sim = Simulator::instance();
	if (sim.timerRunning())
		return 1;
	else
		return 0;
}

void pf_init_axes()
{
	Simulator &sim = Simulator::instance();

	uint8_t port = 0;
	ums::Axis axis;
	axis.step_.configure(port, 0);
	axis.dir_.configure(port, 1);
	axis.fwdLimit_.configure(port, 2);
	axis.revLimit_.configure(port, 3 + UMS_INVERT_PIN);
	sim.axis('X', axis);
	struct AxisCmd ac = sim.axisCommand('X');
	st_setup_axis(&ac);

	port = 1;
	axis.step_.configure(port, 0);
	axis.dir_.configure(port, 1);
	axis.fwdLimit_.configure(port, 2 + UMS_INVERT_PIN);
	axis.revLimit_.configure(port, 3);
	sim.axis('Y', axis);
	ac = sim.axisCommand('Y');
	st_setup_axis(&ac);

	port = 2;
	axis.step_.configure(port, 0);
	axis.dir_.configure(port, 1 + UMS_INVERT_PIN);
	axis.fwdLimit_.configure(port, 2);
	axis.revLimit_.configure(port, 3);
	sim.axis('Z', axis);
	ac = sim.axisCommand('Z');
	st_setup_axis(&ac);

	port = 3;
	axis.step_.configure(port, 0 + UMS_INVERT_PIN);
	axis.dir_.configure(port, 1);
	axis.fwdLimit_.configure(port, 2);
	axis.revLimit_.configure(port, 3);
	sim.axis('U', axis);
	ac = sim.axisCommand('U');
	st_setup_axis(&ac);

	// st_setup_axis initializes pins which cause accidental step
	sim.positionLog_.clear();
	sim.axis('X').position_ = 0;
	sim.axis('Y').position_ = 0;
	sim.axis('Z').position_ = 0;
	sim.axis('U').position_ = 0;

}
