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

boost::optional<MessageInfo::buffer_t>
MessageInfo::receiveMessage(ILink *link)
{
	boost::optional<MessageInfo::buffer_t> ret;



	return ret;
}

}
