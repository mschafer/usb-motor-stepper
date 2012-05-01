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

}
