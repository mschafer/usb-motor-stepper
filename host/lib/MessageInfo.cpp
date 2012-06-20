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

MessageInfo::buffer_t
MessageInfo::receiveMessage(ILink *link)
{
	static size_t rxOffset = 0;
	static buffer_t rxBuff;
	buffer_t ret;

	boost::optional<uint8_t> rx;

	if (rxOffset == 0) {
		rx = link->readByte();
		if (rx) {
			rxOffset = 1;
			rxBuff.push_back(rx.get());
		} else {
			return ret;
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
			if (!rx) return ret;
			rxBuff.push_back(rx.get());
		}
		ret.swap(rxBuff);
		rxOffset = 0;
		rxBuff.clear();
		return ret;
	}
}

std::string
MessageInfo::toString(const buffer_t &b)
{
	if (b.empty()) throw (std::runtime_error("can't translate empty message buffer to string"));
	const MessageInfo *minfo = findById(b[0]);
	if (minfo == NULL) {
		throw (std::runtime_error("can't translate unknown message id"));
	} else {
		return minfo->translate(b);
	}
}

void
MessageInfo::addToRegistry() {
	registry().insert(this);
}

}
