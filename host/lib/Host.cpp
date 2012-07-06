#include "Host.hpp"
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
		Simulator::reset();
		link_ = &Simulator::instance();
		simRun_ = true;
		boost::thread t(boost::bind(&Host::simThread, this));
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
		ownedLink_.reset();
		if (ownsSim_) {
			uniqueSim_.unlock();
		}
		if (msgExec_.joinable()) {
			msgRun_ = false;
			msgExec_.join();
		}
		if (simExec_.joinable()) {
			simRun_ = false;
			simExec_.join();
		}
	} catch (...) {
		std::cerr << "deleting host threw something" << std::endl;
	}
}

bool
Host::pingDevice()
{
	PingCmd pc;
	pc.cmdId = PingCmd_ID;
	pong_.reset();
	sendCommand(makeCmdBuff(pc));
	int iter = 10;
	while (iter > 0) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(20));
		iter--;
		if (pong_) return true;
	}
	return false;
}

void
Host::enableDevice()
{
	// check for an already enabled device
	if (pingDevice()) return;

	std::vector<uint8_t> bytes;
	std::string enableStr(UMS_ENABLE);
	BOOST_FOREACH(char c, enableStr) {
		bytes.push_back(c);
	}
	accept_.reset();
	link_->write(bytes);

	for (int i=0; i<10; i++) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		if (accept_){
			if (accept_.get().majorVersion != UMS_MAJOR_VERSION ||
				accept_.get().minorVersion != UMS_MINOR_VERSION) {
				throw std::runtime_error("device has incorrect firmware version");
			}
			break;
		}
		if (i == 9)
			throw std::runtime_error("connection to device failed");
	}
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

MessageInfo::buffer_t
Host::receiveMessage()
{
	MessageInfo::buffer_t ret;
	boost::lock_guard<boost::mutex> guard(msgLock_);
	if (!msgQ_.empty()) {
		ret.swap(msgQ_.front());
		msgQ_.pop_front();
	}
	return ret;
}

std::deque<Simulator::position_t>
Host::simulatorPositionLog()
{
	std::deque<Simulator::position_t> ret;
	boost::lock_guard<boost::mutex> guard(simLock_);
	ret.swap(Simulator::instance().positionLog_);
	return ret;
}

void
Host::msgThread()
{
	while (msgRun_) {
		MessageInfo::buffer_t m;
		bool empty = false;
		do {
			try {
				m = MessageInfo::receiveMessage(link_);
			} catch (...) {
				std::cout << "received bogus data\n";
			}
			empty = m.empty();
			if (!m.empty()) {
				boost::lock_guard<boost::mutex> guard(msgLock_);
				switch (m[0]) {
				case AcceptMsg_ID:
					accept_ = *(AcceptMsg *)(&m[0]);
					break;

				case PongMsg_ID:
					pong_ =*(PongMsg *)(&m[0]);
					break;

					///\todo add case for ErrorMsg and track enabled/disabled state in host

				default:
					msgQ_.push_back(MessageInfo::buffer_t());
					msgQ_.back().swap(m);
					break;
				}
			}
		} while (!empty);
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
}

void
Host::simThread()
{
	while (simRun_) {
		{
			boost::lock_guard<boost::mutex> guard(simLock_);
			Simulator::instance().runOnce();
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(2));
	}
}


}
