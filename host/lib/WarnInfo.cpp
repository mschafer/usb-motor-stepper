#include "CommandInfo.hpp"

namespace umc {

class WarnInfo : public CommandInfo
{
public:
    WarnInfo() {
        name_ = "Warn";
        id_ = WarnCmd_ID;

        addToRegistry();
    }

     MaxCmdBuff parse(const std::string &input) const {
         throw std::logic_error("No Parser.  Warn command not allowed in input file.");
    }

    std::string generate() const {
        return std::string("generated");
    }

};

WarnInfo theWarn;

}
