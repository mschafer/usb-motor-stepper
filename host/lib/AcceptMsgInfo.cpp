#include "MessageInfo.hpp"
#include "messages.h"
#include <sstream>

namespace ums {

class AcceptMsgInfo : public MessageInfo
{
public:
	AcceptMsgInfo() {
		name_ = "Accept";
		id_ = AcceptMsg_ID;
		size_ = AcceptMsg_LENGTH;

		addToRegistry();
	}

	std::string translate(const buffer_t &b) const
	{	std::stringstream ss;
		const AcceptMsg *am = reinterpret_cast<const AcceptMsg*>(&b[0]);
		ss << "Accept connection from device version "<< am->majorVersion << "." << am->minorVersion;
		return ss.str();
	}

};

AcceptMsgInfo theAcceptMsgInfo;

}
