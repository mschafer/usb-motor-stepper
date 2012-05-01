#ifndef ums_Platform_hpp
#define ums_Platform_hpp

#include <deque>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <stdint.h>
#include <boost/container/flat_map.hpp>
#include "Axis.hpp"
#include "ILink.hpp"

/**
 * \file Platform specific implementation of ums functions for simulator.
 */

namespace ums { namespace sim {

/**
 * Simulation platform implementation.
 * Singleton.
 */
class Platform : public boost::noncopyable {
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

	std::deque<uint8_t> toHost_;
	std::deque<uint8_t> fromHost_;

private:
	Platform();

	static Platform *thePlatform_;
	boost::optional<uint16_t> delay_;
	size_t t_;
	boost::array<bool, 256> pins_;

	typedef boost::container::flat_map<char, Axis> axis_map_t;
	axis_map_t axes_;

};

}}
#endif
