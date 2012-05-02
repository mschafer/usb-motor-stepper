#include "Host.hpp"
#include "Platform.hpp"
#include <string>
#include <boost/foreach.hpp>
#include "SimLink.hpp"
#include "SerialLink.hpp"
#include "ums.h"

namespace ums {

const std::string Host::SIMULATOR_NAME("simulator");
boost::mutex Host::uniqueSim_;

Host::Host(const std::string &linkName) : ownsSim_(false), rxOffset_(0)
{
	if (linkName.compare(SIMULATOR_NAME)==0) {
		// take unique ownership of the simulator because device C code uses globals
		ownsSim_ = uniqueSim_.try_lock();
		if (!ownsSim_) {
			throw std::runtime_error("Simulator already in use");
		}
		sim::Platform::reset();
		link_.reset(new SimLink());
		boost::thread t(boost::ref(simThread_));
		simExec_.swap(t);
	} else {
		link_.reset(new SerialLink(linkName));
	}
}

Host::~Host()
{
	if (ownsSim_) {
		uniqueSim_.unlock();
	}
}

void
Host::enableDevice() const
{
	std::vector<uint8_t> bytes;
	std::string enableStr(UMS_ENABLE);
	BOOST_FOREACH(char c, enableStr) {
		bytes.push_back(c);
	}
	link_->write(bytes);
}

void
Host::Host::SimThread::operator()()
{
	while (run_) {
		{
			sim::Platform::instance().runOnce();
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
}


}
