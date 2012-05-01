#include "Host.hpp"
#include "Platform.hpp"
#include <string>
#include <boost/foreach.hpp>
#include "SimLink.hpp"
#include "SerialLink.hpp"
#include "ums.h"

namespace ums {

const std::string Host::SIMULATOR_NAME("simulator");

Host::Host(const std::string &linkName)
{
	if (linkName.compare(SIMULATOR_NAME)==0) {
		sim::Platform::reset();
		link_.reset(new SimLink());
	} else {
		link_.reset(new SerialLink(linkName));
	}
}

Host::~Host()
{

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

boost::optional<std::vector<uint8_t> >
Host::receiveMessage()
{
	boost::optional<std::vector<uint8_t> > ret;
#if 0
		if (rxOffset_ == 0) {
			boost::optional<uint8_t> rx = link_->readByte();
			if (rx) {
				rxOffset = 1;
			} else {
				return NULL;
			}
		}


	    if (cmdBuff[0] > MAX_CMD_ID) {
	        handle_bad_cmd(cmdBuff);
	    }

		uint8_t len = cmd_get_length(cmdBuff[0]);
		while (rxOffset < len) {
			if (!pf_receive_byte(cmdBuff+rxOffset)) {
				return NULL;
			}
			rxOffset++;
		}
		rxOffset = 0;

#endif

	return ret;
}


}
