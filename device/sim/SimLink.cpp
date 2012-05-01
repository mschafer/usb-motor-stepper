#include "SimLink.hpp"
#include "Platform.hpp"

namespace ums {

SimLink::~SimLink()
{
}

void
SimLink::write(std::vector<uint8_t> &bytes)
{
	sim::Platform &p = sim::Platform::instance();
	p.fromHost_.insert(p.fromHost_.end(), bytes.begin(), bytes.end());
}

void
SimLink::write(void *bytes, size_t nBytes)
{
	sim::Platform &p = sim::Platform::instance();
	uint8_t *pBytes = reinterpret_cast<uint8_t*>(bytes);
	p.fromHost_.insert(p.fromHost_.end(), pBytes, pBytes+nBytes);
}

std::deque<uint8_t>
SimLink::read()
{
	std::deque<uint8_t> ret;
	sim::Platform &p = sim::Platform::instance();
	ret.swap(p.toHost_);
	return ret;
}

boost::optional<uint8_t>
SimLink::readByte()
{
	sim::Platform &p = sim::Platform::instance();
	boost::optional<uint8_t> ret;
	if (!p.toHost_.empty()) {
		ret = p.toHost_.front();
		p.toHost_.pop_front();
	}
	return ret;
}

}
