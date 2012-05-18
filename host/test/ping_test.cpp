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
	ums::Host host;
	host.enableDevice();
	host.pingDevice();

	boost::this_thread::sleep(boost::posix_time::milliseconds(20));

	ums::MessageInfo::buffer_t pongBuff;
	host.receiveMessage(pongBuff);

	BOOST_CHECK(pongBuff.size() == PongMsg_LENGTH && pongBuff[0] == PongMsg_ID);
}

BOOST_AUTO_TEST_CASE( bad_pin_test )
{
	ums::Host host;
	host.enableDevice();

	ums::Axis ax;
	ax.step_.configure(6, 0);
	ax.dir_.configure(0, 1);

	AxisCmd ac = ax.command();
	ac.name = 'X';
	host.sendCommand(&ac, sizeof(ac));

	boost::this_thread::sleep(boost::posix_time::milliseconds(20));

	ums::MessageInfo::buffer_t pongBuff;
	host.receiveMessage(pongBuff);

	BOOST_CHECK(pongBuff.size() == ErrorMsg_LENGTH && pongBuff[0] == ErrorMsg_ID);

}



BOOST_AUTO_TEST_CASE( one_step_test )
{
	ums::Host host;
	host.enableDevice();

	ums::Axis x;
	x.step_.configure(0, 0);
	x.dir_.configure(0, 1);
	Platform::instance().axis('X', x);

	AxisCmd ac = Platform::instance().axisCommand('X');
	host.sendCommand(&ac, sizeof(ac));

	StepCmd sc;
	sc.cmdId = StepCmd_ID;
	sc.delay_lo = 100;
	sc.delay_hi = 0;
	sc.stepDir = UMS_X_STEP | UMS_X_DIR;
	host.sendCommand(&sc, sizeof(sc));

	boost::this_thread::sleep(boost::posix_time::milliseconds(30));

	BOOST_CHECK(Platform::instance().axis('X').get().position_ == 1);
}
