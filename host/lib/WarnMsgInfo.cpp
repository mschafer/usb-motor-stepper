#include "MessageInfo.hpp"
#include "messages.h"

namespace ums {

class WarnMsgInfo : public MessageInfo
{
public:
	WarnMsgInfo() {
		name_ = "Warn";
		id_ = WarnMsg_ID;
		size_ = WarnMsg_LENGTH;

		addToRegistry();
	}

	std::string translate(const buffer_t &b) const
	{
		std::stringstream ss;
		const WarnMsg *m = reinterpret_cast<const WarnMsg*>(&b[0]);
		ss << "Warning " << (int)(m->warnId) << ": ";
		switch (m->warnId) {

		case UMS_WARN_CONFIGURE_PIN:
			ss << "attempt to configure reserved pin " << (int)(m->data[0]) << "." << (int)(m->data[1]);
			break;

		case UMS_WARN_BAD_AXIS:
			ss << "attempt to configure unknown axis " << m->data[0];
			break;

		default:
			ss << "unknown id";
			break;
		}

		return ss.str();
	}

};

WarnMsgInfo theWarnMessageInfo;

}
