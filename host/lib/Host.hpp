#ifndef ums_Host_hpp
#define ums_Host_hpp

#include <boost/utility.hpp>
#include "ILink.hpp"
#include "MessageInfo.hpp"

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

	Host(const std::string &linkName);
	virtual ~Host();

	/**
	 * Send enable code to the device through the link.
	 * The device starts in a disabled state where it will ignore all commands until it is enabled.
	 * It may also transition to a disabled state when a fatal error occurs.
	 */
	void enableDevice() const;

	/**
	 * Send a command to the device.
	 * Commands are transmitted asynchronously via the link.  This routine returns immediately,
	 * but the command may not be sent for some time.
	 */
	void sendCommand(void *cmdBuff, size_t cmdSize) {
		link_->write(cmdBuff, cmdSize);
	}

	void receiveMessage(MessageInfo::buffer_t msgBuff) {
		msgBuff.clear();
		MessageInfo::receiveMessage(msgBuff, link_.get());
	}

private:
	std::auto_ptr<ILink> link_;
	std::vector<uint8_t> rxMessage_;
	size_t rxOffset_;

	Host();
};

}

#endif

