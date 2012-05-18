#ifndef ums_Platform_hpp
#define ums_Platform_hpp

#include <deque>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <stdint.h>
#include <boost/container/flat_map.hpp>
#include "Axis.hpp"
#include "ILink.hpp"
#include "platform.h"

/**
 * \file Platform specific implementation of ums functions for simulator.
 */

namespace ums { namespace sim {

/**
 * Simulation platform implementation.
 * Singleton.
 */
class Platform : public ILink {
public:
	void runOnce();

	void axis(char name, Axis &a);
	boost::optional<Axis> axis(char name) const;
	void removeAxis(char name);
	AxisCmd axisCommand(char name);

	void portPin(uint8_t addr, uint8_t val);
	uint8_t portPin(uint8_t addr);

	bool timerRunning();
	void timerDelay(uint16_t delay);

	/**
	 * Deletes the singleton and creates a new one.
	 * Used to simulate a hard reset of embedded controller.
	 */
	static void reset();

	static Platform &instance();

	virtual void write(std::vector<uint8_t> &bytes);
	virtual void write(void *bytes, size_t nBytes);
	virtual std::deque<uint8_t> read();
	virtual boost::optional<uint8_t> readByte();


private:
	friend uint8_t ::pf_send_bytes(uint8_t*, uint16_t);
	friend uint8_t ::pf_receive_byte(uint8_t*);

	Platform();

	static Platform *thePlatform_;

	std::deque<uint8_t> toHost_;
	std::deque<uint8_t> fromHost_;
	boost::optional<uint16_t> delay_;
	size_t t_;
	boost::array<bool, 256> pins_;

	typedef boost::container::flat_map<char, Axis> axis_map_t;
	axis_map_t axes_;
	boost::mutex hostCommMutex_;

};

}}
#endif
