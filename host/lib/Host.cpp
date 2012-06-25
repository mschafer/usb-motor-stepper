#include "Host.hpp"
#include "Platform.hpp"
#include <string>
#include <boost/foreach.hpp>
#include "SerialLink.hpp"
#include "CommandInfo.hpp"
#include "ums.h"

namespace ums {

const std::string Host::SIMULATOR_NAME("simulator");
boost::mutex Host::uniqueSim_;

Host::Host(const std::string &linkName) : ownsSim_(false)
{
	if (linkName.compare(SIMULATOR_NAME)==0) {
		// take unique ownership of the simulator because device C code uses globals
		ownsSim_ = uniqueSim_.try_lock();
		if (!ownsSim_) {
			throw std::runtime_error("Simulator already in use");
		}
		sim::Platform::reset();
		link_ = &sim::Platform::instance();
		boost::thread t(boost::ref(simThread_));
		simExec_.swap(t);
	} else {
		ownedLink_.reset(new SerialLink(linkName));
		link_ = ownedLink_.get();
	}

	// link is up, start thread to handle received messages
	msgRun_ = true;
	boost::thread t(boost::bind(&Host::msgThread, this));
	msgExec_.swap(t);
}

Host::~Host()
{
	try {
		if (ownsSim_) {
			uniqueSim_.unlock();
		}
		if (msgExec_.joinable()) {
			msgRun_ = false;
			msgExec_.join();
		}
		if (simExec_.joinable()) {
			simThread_.run_ = false;
			simExec_.join();
		}
	} catch (...) {
		std::cerr << "deleting host threw something" << std::endl;
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

	///\todo wait and then check for accept or error
}

void
Host::execute(std::istream &in)
{
	using namespace std;
	string line;
	while (!in.eof()) {
		getline(in, line);
		CommandInfo::buffer_t cb = CommandInfo::parseLine(line);
		if (!cb.empty()) {
			sendCommand(cb);
		}
	}
}

void
Host::msgThread()
{
	while (msgRun_) {
		MessageInfo::buffer_t m;
		do {
			m = MessageInfo::receiveMessage(link_);
			if (!m.empty()) {
				switch (m[0]) {
				case AcceptMsg_ID:
					accept_ = *(AcceptMsg *)(&m[0]);
					break;
				default:
					break;
				}
			}
		} while (!m.empty());
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
}

void
Host::Host::SimThread::operator()()
{
	while (run_) {
		sim::Platform::instance().runOnce();
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
}


}
