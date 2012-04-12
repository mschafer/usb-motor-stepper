#include "CommandInfo.hpp"

namespace umc {

class ErrorInfo : public CommandInfo
{
public:
    ErrorInfo() {
        name_ = "Error";
        id_ = ErrorCmd_ID;

        addToRegistry();
    }

     MaxCmdBuff parse(const std::string &input) const {
         throw std::logic_error("No Parser.  Error command not allowed in input file.");
    }

    std::string generate() const {
        return std::string("generated");
    }

};

ErrorInfo theError;

}
