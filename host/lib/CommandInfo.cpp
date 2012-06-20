#include "CommandInfo.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

namespace ums {

CommandInfo::CommandInfo_set &
CommandInfo::registry()
{
	static CommandInfo_set theRegistry;
	return theRegistry;
}

const CommandInfo *
CommandInfo::findByName(const std::string &name)
{
	std::string lower_name = boost::to_lower_copy(name);
	CommandInfo_set::index<Name>::type &nameIdx = registry().get<Name>();
	CommandInfo_set::index<Name>::type::iterator nameit = nameIdx.find(lower_name);
	CommandInfo_set::index<Name>::type::iterator e = nameIdx.end();
	if (nameit == e) return NULL;
	else { return *nameit; }
}

const CommandInfo *
CommandInfo::findById(uint8_t id)
{
	CommandInfo_set::index<Id>::type &idIdx = registry().get<Id>();
	CommandInfo_set::index<Id>::type::iterator idit = idIdx.find(id);
	CommandInfo_set::index<Id>::type::iterator e = idIdx.end();
	if (idit == e) return NULL;
	else { return *idit; }
}

CommandInfo::buffer_t
CommandInfo::parseLine(const std::string &line)
{
    std::string tl = boost::algorithm::trim_left_copy(boost::algorithm::trim_right_copy(line));
    buffer_t ret;

    // ignore comments
    if (boost::starts_with(tl, "#")) {
        return ret;
    } else {
        boost::tokenizer<> tok(tl);
        std::string cmdName = *tok.begin();
        const CommandInfo *ci = findByName(cmdName);
        if (ci != NULL) {
            ci->compile(tl).swap(ret);
        } else {
        	throw std::runtime_error("Unrecognized command " + cmdName);
        }

    }

    return ret;
}

void
CommandInfo::addToRegistry() {
	registry().insert(this);
}

}
