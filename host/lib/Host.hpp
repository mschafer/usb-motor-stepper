#ifndef ums_Host_hpp
#define ums_Host_hpp

#include <boost/utility.hpp>
#include "ILink.hpp"
#include "MessageInfo.hpp"
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
	 */
	void enableDevice() const;

	/** Send PingCmd to the device. */
	void pingDevice() const {
		PingCmd pc;
		pc.cmdId = PingCmd_ID;
		sendCommand(&pc, PingCmd_LENGTH);
	}

	/**
	 * Send a command to the device.
	 * Commands are transmitted asynchronously via the link.  This routine returns immediately,
	 * but the command may not be sent for some time.
	 */
	void sendCommand(void *cmdBuff, size_t cmdSize) const {
		link_->write(cmdBuff, cmdSize);
	}

	void receiveMessage(MessageInfo::buffer_t &msgBuff) const {
		msgBuff.clear();
		MessageInfo::receiveMessage(msgBuff, link_.get());
	}

private:
	/**
	 * The C code for the simulator uses global variables and so there can be only 1 in use at a time.
	 * The Host ctor acquires this mutex if the link is to the simulator and the dtor releases it.
	 */
	static boost::mutex uniqueSim_;

	bool ownsSim_;
	std::auto_ptr<ILink> link_;
	std::vector<uint8_t> rxMessage_;
	size_t rxOffset_;
	boost::thread simExec_;

	struct SimThread
	{
		bool run_;

		SimThread() : run_(true) {}
		void operator()();
	} simThread_;
};

}

#endif

