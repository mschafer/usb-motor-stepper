#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
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
using boost::optional;

namespace ums {

template <typename Iterator>
struct DelayParser : qi::grammar<Iterator, unsigned int(), ascii::space_type>
{
    DelayParser() : DelayParser::base_type(start, "Delay")
    {
        start = qi::no_case[qi::lit("delay")] >> qi::uint_;
    }

    qi::rule<Iterator, unsigned int(), ascii::space_type> start;
};


class DelayCmdInfo : public CommandInfo
{
public:
    DelayCmdInfo() {
        name_ = "Delay";
        id_ = DelayCmd_ID;

        addToRegistry();
    }

    buffer_t compile(const std::string &input) const {
        typedef std::string::const_iterator iterator;
        DelayParser<iterator> grammar;
        iterator iter = input.begin();
        iterator end = input.end();
        unsigned int delayVal;
        bool r = phrase_parse(iter, end, grammar, ascii::space, delayVal);
        if (!r || iter != end) {
            throw (std::runtime_error("parse Delay failed"));
        } else {
            buffer_t ret;
            fillCmdBuff(ret, delayVal);
            return ret;
        }
    }

    std::string generate() const {
        return std::string("generated");
    }

    static void fillCmdBuff(buffer_t &m, unsigned int delayVal)
    {
    	m.resize(DelayCmd_LENGTH);
    	DelayCmd *d = reinterpret_cast<DelayCmd *>(&m[0]);
    	d->cmdId = DelayCmd_ID;
    	d->delay_hi = (uint8_t)((delayVal >> 24) & 0xFF);
    	d->delay_hm = (uint8_t)((delayVal >> 16) & 0xFF);
    	d->delay_lm = (uint8_t)((delayVal >>  8) & 0xFF);
    	d->delay_lo = (uint8_t)( delayVal        & 0xFF);
    }

};

// static instance for the registry
DelayCmdInfo theDelayCmd;

}
