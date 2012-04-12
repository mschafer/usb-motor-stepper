#ifndef umc_Platform_hpp
#define umc_Platform_hpp

#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <stdint.h>
#include <boost/container/flat_map.hpp>
#include "Axis.hpp"

/**
 * \file Platform specific implementation of umc functions.
 */

namespace umc {

/**
 * Simulation platform implementation.
 * Singleton.
 */
class Platform {
public:

	static const int TO_BUFFER_SIZE;
	static const int FROM_BUFFER_SIZE;

	boost::circular_buffer<uint8_t> toHost_;
	boost::circular_buffer<uint8_t> fromHost_;

	void runOnce();

	void axis(char name, Axis &a);
	boost::optional<Axis> axis(char name) const;
	void removeAxis(char name);
	AxisCmd axisCommand(char name);

	void portPin(uint8_t addr, uint8_t val);
	uint8_t portPin(uint8_t addr);

	bool timerRunning();
	void timerDelay(uint16_t delay);

	void reset();

	static Platform &instance();
private:
	Platform();

	static Platform *thePlatform_;

	boost::optional<uint16_t> delay_;
	size_t t_;
	boost::array<bool, 256> pins_;

	typedef boost::container::flat_map<char, Axis> axis_map_t;
	axis_map_t axes_;

};

}
#endif
