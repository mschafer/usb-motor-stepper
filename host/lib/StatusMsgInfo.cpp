#include "MessageInfo.hpp"
#include <sstream>
#include "messages.h"
#include "ums.h"

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
		size_t cmdCount = UMS_UNPACK_U32(m->commandCounter);
		size_t stepCount = UMS_UNPACK_U32(m->stepCounter);
		std::string limitBits;
		ss << "Status:\tlimit: " << bits(m->limits);
		ss << "\tflags: " << bits(m->flags);
		ss << "\tcmdCount: " << cmdCount << "\tstepCount: " << stepCount;
		ss << "\tx: " << UMS_UNPACK_I32(m->xPos);
		ss << "\ty: " << UMS_UNPACK_I32(m->yPos);
		ss << "\tz: " << UMS_UNPACK_I32(m->zPos);
		ss << "\tu: " << UMS_UNPACK_I32(m->uPos) << std::endl;
		return ss.str();
	}

};

StatusMsgInfo theStatusMsgInfo;

}
