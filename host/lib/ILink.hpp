#ifndef ums_ILink_hpp
#define ums_ILink_hpp

#include <deque>
#include <vector>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

namespace ums {

class ILink : public boost::noncopyable
{
public:
	ILink() {}
	virtual ~ILink() {}

	/**
	 * Write a sequence of bytes to the device.
	 * This method returns immediately and the write is handled asynchronously.
	 * Note that vector is used instead of deque because some implementations require contiguous memory.
	 */
	virtual void write(const std::vector<uint8_t> &bytes) = 0;

	/**
	 * Read a sequence of bytes from the device.
	 * \return A deque containing all available bytes from the device.
	 */
	virtual std::deque<uint8_t> read() = 0;

	/**
	 * Read a single byte from the device.
	 * \return Optional containing the byte read or empty if nothing is available.
	 */
	virtual boost::optional<uint8_t> readByte() = 0;
};

}

#endif
