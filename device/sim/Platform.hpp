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

namespace ums { namespace sim {

/**
 * Simulation platform implementation.
 * Singleton.
 */
class Platform : public ILink {
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

	typedef boost::array<ptrdiff_t, 4> position_t;
	std::deque<position_t> positionLog_;

private:
	friend uint8_t ::pf_send_bytes(uint8_t*, uint16_t);
	friend uint8_t ::pf_receive_byte(uint8_t*);

	enum AxisIdx {
		X_IDX=0, Y_IDX=1, Z_IDX=2, U_IDX=3, END_AXIS=4
	};

	Platform();

	static Platform *thePlatform_;
	std::deque<uint8_t> toHost_;
	std::deque<uint8_t> fromHost_;
	boost::optional<uint16_t> delay_;
	size_t t_;
	boost::array<bool, 256> pins_;
	boost::mutex hostCommMutex_;
	Axis axes_[4];


};

}}

std::ostream &operator<<(std::ostream &out, const ums::sim::Platform::position_t &pos);

#endif
