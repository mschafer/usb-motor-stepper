#include "Host.hpp"
#include "Platform.hpp"
#include <string>

namespace ums {

const std::string Host::SIMULATOR_NAME("simulator");

Host::Host(const std::string &linkName)
{
	if (linkName.compare(SIMULATOR_NAME)) {
		sim::Platform::reset();
	}
}

void
Host::enableDevice() const
{

}


}
