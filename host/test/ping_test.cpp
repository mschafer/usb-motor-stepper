#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/foreach.hpp>
#include <deque>
#include "ums.h"
#include "Axis.hpp"
#include "Platform.hpp"

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
	Platform::reset();
	Platform &p = Platform::instance();
	std::string enableStr(UMS_ENABLE);
	BOOST_FOREACH(char c, enableStr) {
		p.fromHost_.push_back(c);
	}

	ums::Axis x;
	x.step_.configure(0, 0);
	x.dir_.configure(0, 1);
	p.axis('X', x);

	AxisCmd ac = p.axisCommand('X');
	uint8_t *d = reinterpret_cast<uint8_t*>(&ac);
	for (size_t i=0; i<sizeof(ac); i++) {
		p.fromHost_.push_back(d[i]);
	}

	p.runOnce();

	StepCmd sc;
	sc.cmdId = StepCmd_ID;
	sc.delay_lo = 100;
	sc.delay_hi = 0;
	sc.stepDir = UMS_X_STEP | UMS_X_DIR;
	d = reinterpret_cast<uint8_t*>(&sc);
	for (size_t i=0; i<sizeof(sc); i++) {
		p.fromHost_.push_back(d[i]);
	}
	p.runOnce();

	BOOST_CHECK(p.axis('X').get().position_ == 1);
}
