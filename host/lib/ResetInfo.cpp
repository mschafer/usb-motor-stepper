#include <boost/algorithm/string.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/foreach.hpp>
#include "CommandInfo.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

namespace umc {

class ResetInfo : public CommandInfo
{
public:
    ResetInfo() {
        name_ = "Reset";
        id_ = ResetCmd_ID;

        addToRegistry();
    }

    MaxCmdBuff parse(const std::string &input) const {

        std::string lowerInput = boost::algorithm::to_lower_copy(input);
        boost::algorithm::trim_left(lowerInput);
        boost::algorithm::trim_right(lowerInput);
        if (lowerInput.compare("reset") != 0) {
            throw (std::runtime_error("parse Reset failed"));
        }

        MaxCmdBuff ret;
        ret.cmdId = ResetCmd_ID;
        return ret;
    }

    std::string generate() const {
        return std::string("generated");
    }

};

// static instance for the registry
ResetInfo theReset;

}
