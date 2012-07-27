#include "MessageInfo.hpp"
#include <sstream>
#include "messages.h"

namespace ums {

class PongMsgInfo : public MessageInfo
{
public:
	PongMsgInfo() {
		name_ = "Pong";
		id_ = PongMsg_ID;
		size_ = PongMsg_LENGTH;

		addToRegistry();
	}

	std::string translate(const buffer_t &b) const
	{	std::stringstream ss;
		const PongMsg *pm = reinterpret_cast<const PongMsg*>(&b[0]);
		ss << "Pong";
		return ss.str();
	}

};

PongMsgInfo thePongMsgInfo;

}
