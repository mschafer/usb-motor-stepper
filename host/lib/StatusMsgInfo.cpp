#include "MessageInfo.hpp"
#include "messages.h"

namespace ums {

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
		ss << "Status:\tlimit: ";
		for (int i=7; i>=0; i--) {
			uint8_t mask = 1 << i;
			if (mask & m->limits) limitBits += "1";
			else limitBits += "0";
		}
		ss << limitBits << "\tcmdCount: " << cmdCount << "\tstepCount: " << stepCount;
		return ss.str();
	}

};

StatusMsgInfo theStatusMsgInfo;

}
