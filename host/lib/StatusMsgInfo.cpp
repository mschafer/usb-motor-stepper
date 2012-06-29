#include "MessageInfo.hpp"
#include "messages.h"

namespace ums {

std::string bits(uint8_t v) {
	std::string b;
	for (int i=7; i>=0; i--) {
		uint8_t mask = 1 << i;
		if ((mask & v) != 0) b += "1";
		else b += "0";
	}
	return b;
}

class StatusMsgInfo : public MessageInfo
{
public:
	StatusMsgInfo() {
		name_ = "Status";
		id_ = StatusMsg_ID;
		size_ = StatusMsg_LENGTH;

		addToRegistry();
	}

	std::string translate(const buffer_t &b) const
	{	std::stringstream ss;
		const StatusMsg *m = reinterpret_cast<const StatusMsg*>(&b[0]);
		size_t cmdCount = m->commandCounter_lo +        (m->commandCounter_lm << 8) +
				         (m->commandCounter_hm << 16) + (m->commandCounter_hi << 24);
		size_t stepCount = m->stepCounter_lo +        (m->stepCounter_lm << 8) +
				          (m->stepCounter_hm << 16) + (m->stepCounter_hi << 24);
		std::string limitBits;
		ss << "Status:\tlimit: " << bits(m->limits);
		ss << "\tflags: " << bits(m->flags);
		ss << "\tcmdCount: " << cmdCount << "\tstepCount: " << stepCount;
		return ss.str();
	}

};

StatusMsgInfo theStatusMsgInfo;

}
