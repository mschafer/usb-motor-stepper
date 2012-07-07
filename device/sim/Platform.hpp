#ifndef ums_Platform_hpp
#define ums_Platform_hpp

#include <deque>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <stdint.h>
#include <iostream>
#include <boost/container/flat_map.hpp>
#include "Axis.hpp"
#include "ILink.hpp"
#include "platform.h"

/**
 * \file Platform specific implementation of ums functions for simulator.
 */

namespace ums {

/**
 * Simulation platform implementation.
 * Singleton.
 */
class Simulator : boost::noncopyable {
public:
	void runOnce();

	const Axis &axis(char name) const;
	Axis &axis(char name);
	void axis(char name, Axis &a);
	void disableAxis(char name);
	AxisCmd axisCommand(char name);

	void portPin(uint8_t addr, uint8_t val);
	uint8_t portPin(uint8_t addr);

	bool timerRunning();
	void timerDelay(uint32_t delay);

	/**
	 * Deletes the singleton and creates a new one.
	 * Used to simulate a hard reset of embedded controller.
	 */
	static void reset(ILink::handle);

	static Simulator &instance();

	/**
	 * Position records have x, y, z, and u position followed by time.
	 * time += delay after each step or delay command.
	 */
	typedef boost::array<ptrdiff_t, 5> position_t;
	std::deque<position_t> positionLog_;

	void writeToHost(const uint8_t *data, uint16_t size);
	boost::optional<uint8_t> readHostByte();

private:
	enum AxisIdx {
		X_IDX=0, Y_IDX=1, Z_IDX=2, U_IDX=3, END_AXIS=4
	};

	Simulator(ILink::handle);

	static Simulator *thePlatform_;
	ILink::handle link_;
	std::deque<uint8_t> toHost_;
	std::deque<uint8_t> fromHost_;
	boost::optional<uint32_t> delay_;
	size_t t_;
	boost::array<bool, 256> pins_;
	Axis axes_[4];
};

}

std::ostream &operator<<(std::ostream &out, const ums::Simulator::position_t &pos);

#endif
