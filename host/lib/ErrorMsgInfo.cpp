#include "MessageInfo.hpp"
#include "messages.h"

namespace ums {

class ErrorMsgInfo : public MessageInfo
{
public:
	ErrorMsgInfo() {
		name_ = "Error";
		id_ = ErrorMsg_ID;
		size_ = ErrorMsg_LENGTH;

		addToRegistry();
	}

	std::string translate(const buffer_t &b) const
	{
		std::stringstream ss;
		const ErrorMsg *em = reinterpret_cast<const ErrorMsg*>(&b[0]);
		ss << "Error id: " << (int)(em->errorId);
		switch (em->errorId) {

		case UMS_ERROR_UNKNOWN_CMD_ID:
			ss << "received unknown command id " << (int)(em->data[0]);
			break;

		case UMS_ERROR_CONFIGURE_PIN:
			ss << "attempt to configure reserved pin " << (int)(em->data[0]) << "." << (int)(em->data[1]);
			break;

		case UMS_ERROR_BAD_AXIS:
			ss << "attempt to configure unknown axis " << em->data[0];
			break;

		default:
			ss << "unknown id";
			break;
		}

		return ss.str();
	}

};

ErrorMsgInfo theErrorMessageInfo;

}
