#include "MessageInfo.hpp"

namespace ums {

MessageInfo::MessageInfo_set &
MessageInfo::registry()
{
	static MessageInfo_set theRegistry;
	return theRegistry;
}

const MessageInfo *
MessageInfo::findByName(const std::string &name)
{
	std::string lower_name = boost::to_lower_copy(name);
	MessageInfo_set::index<Name>::type &nameIdx = registry().get<Name>();
	MessageInfo_set::index<Name>::type::iterator nameit = nameIdx.find(lower_name);
	MessageInfo_set::index<Name>::type::iterator e = nameIdx.end();
	if (nameit == e) {
		return NULL;
	} else {
		return *nameit;
	}
}

const MessageInfo*
MessageInfo::findById(uint8_t id)
{
	MessageInfo_set::index<Id>::type &idIdx = registry().get<Id>();
	MessageInfo_set::index<Id>::type::iterator idit = idIdx.find(id);
	MessageInfo_set::index<Id>::type::iterator e = idIdx.end();
	if (idit == e) {
		return NULL;
	} else {
		return *idit;
	}
}

void
MessageInfo::receiveMessage(buffer_t &msgBuff, ILink *link)
{
	static size_t rxOffset;
	static buffer_t rxBuff;

	boost::optional<uint8_t> rx;

	if (rxOffset == 0) {
		rx = link->readByte();
		if (rx) {
			rxOffset = 1;
			rxBuff.push_back(rx.get());
		} else {
			return;
		}
	}

	const MessageInfo *minfo = findById(rxBuff[0]);
	if (minfo == NULL) {
		rxOffset = 0;
		rxBuff.clear();
		throw (std::runtime_error("Received bad message id"));
	} else {
		size_t size = minfo->size();
		while (rxBuff.size() < size) {
			rx = link->readByte();
			if (!rx) return;
			rxBuff.push_back(rx.get());
		}
		msgBuff.swap(rxBuff);
		rxBuff.clear();
	}
}

}
