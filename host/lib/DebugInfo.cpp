#include "CommandInfo.hpp"

namespace umc {

class DebugInfo : public CommandInfo
{
public:
	DebugInfo() {
		name_ = "Debug";
		id_ = DebugCmd_ID;

		addToRegistry();
	}

	 MaxCmdBuff parse(const std::string &input) const {
	     throw std::logic_error("No Parser.  Debug command not allowed in input file.");
	}

	std::string generate() const {
		return std::string("generated");
	}

};

DebugInfo theDebug;

}
