#ifndef ums_SimLink_hpp
#define ums_SimLink_hpp

#include "ILink.hpp"

namespace ums {

class SimLink : public ILink
{
	virtual ~SimLink();
	virtual void write(std::vector<uint8_t> &bytes);
	virtual void write(void *bytes, size_t nBytes);
	virtual std::deque<uint8_t> read();
	virtual boost::optional<uint8_t> readByte();

};

}

#endif
