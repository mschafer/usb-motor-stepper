#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/foreach.hpp>
#include <deque>
#include "ums.h"
#include "Axis.hpp"
#include "Platform.hpp"
#include "Host.hpp"

using ums::sim::Platform;

BOOST_AUTO_TEST_CASE ( ping_test )
{
	Platform::reset();
	Platform &p = Platform::instance();
	std::string enableStr(UMS_ENABLE);
	BOOST_FOREACH(char c, enableStr) {
		p.fromHost_.push_back(c);
	}

	std::deque<uint8_t> buff;
}

BOOST_AUTO_TEST_CASE( one_step_test )
{
	ums::Host host(ums::Host::SIMULATOR_NAME);
	host.enableDevice();

	ums::Axis x;
	x.step_.configure(0, 0);
	x.dir_.configure(0, 1);
	Platform::instance().axis('X', x);

	AxisCmd ac = Platform::instance().axisCommand('X');
	host.sendCommand(&ac, sizeof(ac));

	Platform::instance().runOnce();

	StepCmd sc;
	sc.cmdId = StepCmd_ID;
	sc.delay_lo = 100;
	sc.delay_hi = 0;
	sc.stepDir = UMS_X_STEP | UMS_X_DIR;
	host.sendCommand(&sc, sizeof(sc));
	Platform::instance().runOnce();

	BOOST_CHECK(Platform::instance().axis('X').get().position_ == 1);
}
