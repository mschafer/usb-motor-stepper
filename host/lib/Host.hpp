#ifndef ums_Host_hpp
#define ums_Host_hpp

#include <boost/utility.hpp>
#include "ILink.hpp"
#include "MessageInfo.hpp"
#include "CommandInfo.hpp"
#include "Platform.hpp"
#include "commands.h"
#include "messages.h"
#include <boost/thread.hpp>

namespace ums {

class Host : boost::noncopyable {
public:

	/** functionality:
	 * enable()
	 * sendCommands(istream)
	 * registerMessageHandler()
	 *
	 */

	static const std::string SIMULATOR_NAME;

	Host(const std::string &linkName=SIMULATOR_NAME);
	virtual ~Host();

	/**
	 * Send enable code to the device through the link.
	 * The device starts in a disabled state where it will ignore all commands until it is enabled.
	 * It may also transition to a disabled state when a fatal error occurs.
	 * The device should respond to the enable code with an AcceptMsg that contains the firmware version number.
	 */
	void enableDevice();

	/** Send PingCmd to the device. */
	void pingDevice() {
		PingCmd pc;
		pc.cmdId = PingCmd_ID;
		sendCommand(makeCmdBuff(pc));
	}

	/**
	 * Send commands read from an input stream until eof.
	 * Uses getline and assumes there is one command to parse per line of input.
	 */
	void execute(std::istream &in);

	/**
	 * Send a command to the device.
	 * Commands are transmitted asynchronously via the link.  This routine returns immediately,
	 * but the command may not be sent for some time.
	 */
	void sendCommand(const CommandInfo::buffer_t &cmd) {
		link_->write(cmd);
	}

	MessageInfo::buffer_t receiveMessage();
	static std::string translate(const MessageInfo::buffer_t &msgBuff) {
		return MessageInfo::toString(msgBuff);
	}

	std::deque<sim::Platform::position_t> simulatorPositionLog();

	boost::optional<StatusMsg> status_;

private:
	/**
	 * The C code for the simulator uses global variables and so there can be only 1 in use at a time.
	 * The Host ctor acquires this mutex if the link is to the simulator and the dtor releases it.
	 */
	static boost::mutex uniqueSim_;

	bool ownsSim_;
	ILink *link_;
	std::auto_ptr<ILink> ownedLink_;
	boost::optional<AcceptMsg> accept_;

	bool msgRun_;
	void msgThread();
	boost::thread msgExec_;
	boost::mutex msgLock_;
	std::deque<MessageInfo::buffer_t> msgQ_;


	bool simRun_;
	void simThread();
	boost::thread simExec_;
	boost::mutex simLock_;
};

}

#endif

