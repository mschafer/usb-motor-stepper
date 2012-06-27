#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/foreach.hpp>
#include <deque>
#include "ums.h"
#include "Axis.hpp"
#include "Platform.hpp"
#include "Host.hpp"
#include "CommandInfo.hpp"

using ums::sim::Platform;
using ums::makeCmdBuff;

BOOST_AUTO_TEST_CASE ( ping_test )
{
	ums::Host host;
	host.enableDevice();
	host.pingDevice();

	boost::this_thread::sleep(boost::posix_time::milliseconds(20));

	ums::MessageInfo::buffer_t msgBuff = host.receiveMessage();
	BOOST_CHECK(msgBuff.size() == PongMsg_LENGTH && msgBuff[0] == PongMsg_ID);
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
	host.sendCommand(makeCmdBuff(ac));

	boost::this_thread::sleep(boost::posix_time::milliseconds(20));

	ums::MessageInfo::buffer_t msg = host.receiveMessage();
	BOOST_CHECK(msg.size() == WarnMsg_LENGTH && msg[0] == WarnMsg_ID);

}

BOOST_AUTO_TEST_CASE( simple_step_test )
{
	ums::Host host;
	host.enableDevice();
	Platform &p = Platform::instance();

	StepCmd sc;
	sc.cmdId = StepCmd_ID;
	sc.delay_lo = 100;
	sc.delay_hi = 0;
	sc.stepDir = UMS_X_STEP | UMS_X_DIR;
	host.sendCommand(makeCmdBuff(sc));

	while (!(host.status_ && host.status_.get().commandCounter_lo == 1)) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	BOOST_CHECK(p.positionLog_.size() == 2);
	BOOST_CHECK(p.positionLog_.back()[0] == 1);
	BOOST_CHECK(p.positionLog_[1][4] - p.positionLog_[0][4]== 100);

}

BOOST_AUTO_TEST_CASE( one_step_test )
{
	ums::Host host;
	host.enableDevice();
	Platform &p = Platform::instance();

	StepCmd sc;
	sc.cmdId = StepCmd_ID;
	sc.delay_lo = 100;
	sc.delay_hi = 0;
	sc.stepDir = UMS_X_STEP | UMS_X_DIR | UMS_Y_STEP | UMS_Y_DIR | UMS_U_STEP;
	host.sendCommand(makeCmdBuff(sc));

	boost::this_thread::sleep(boost::posix_time::milliseconds(300));

	std::deque<Platform::position_t> posLog = host.simulatorPositionLog();
	Platform::position_t pos = posLog.back();
	BOOST_CHECK(posLog.size() == 2);
	BOOST_CHECK(pos[0] ==  1);
	BOOST_CHECK(pos[1] ==  1);
	BOOST_CHECK(pos[2] ==  0);
	BOOST_CHECK(pos[3] == -1);


}

BOOST_AUTO_TEST_CASE( short_line_test )
{
	ums::Host host;
	host.enableDevice();
	Platform &p = Platform::instance();

	LineCmd lc;
	lc.cmdId = LineCmd_ID;
	lc.deltaX = 10;
	lc.deltaY = 5;
	lc.deltaZ = 0;
	lc.deltaU = -1;
	lc.delay_lo = 100;
	lc.delay_hi = 0;
	host.sendCommand(makeCmdBuff(lc));

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	std::deque<Platform::position_t> posLog = host.simulatorPositionLog();
	Platform::position_t pos = posLog.back();
	BOOST_CHECK(posLog.size() == 11);
	BOOST_CHECK(pos[0] == 10);
	BOOST_CHECK(pos[1] ==  5);
	BOOST_CHECK(pos[2] ==  0);
	BOOST_CHECK(pos[3] == -1);
}

BOOST_AUTO_TEST_CASE( line_limit_test )
{
	ums::Host host;
	host.enableDevice();
	Platform &p = Platform::instance();
	ums::Axis &xax = p.axis('X');
	xax.maxFwdPos_ = 5;

	LineCmd lc;
	lc.cmdId = LineCmd_ID;
	lc.deltaX = 10;
	lc.deltaY = 10;
	lc.deltaZ = 0;
	lc.deltaU = 0;
	lc.delay_lo = 100;
	lc.delay_hi = 0;
	host.sendCommand(makeCmdBuff(lc));

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	std::deque<Platform::position_t> posLog = host.simulatorPositionLog();
	Platform::position_t pos = posLog.back();
	BOOST_CHECK(posLog.size() == 11);
	BOOST_CHECK(pos[0] ==  5);
	BOOST_CHECK(pos[1] == 10);
	BOOST_CHECK(pos[2] ==  0);
	BOOST_CHECK(pos[3] ==  0);
}
