#include <boost/spirit/include/qi.hpp>
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
#include "CommandInfo.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

BOOST_FUSION_ADAPT_STRUCT(PingCmd, (uint8_t, cmdId));

namespace ums {

template <typename Iterator>
struct PingParser : qi::grammar<Iterator, PingCmd(), ascii::space_type>
{
	PingParser () : PingParser::base_type(start)
	{
		start = qi::no_case[qi::lit("ping")] >> qi::attr(PingCmd_ID);
	}
	qi::rule<Iterator, PingCmd(), ascii::space_type> start;
};


class PingCmdInfo : public CommandInfo
{
public:
	PingCmdInfo() {
		name_ = "Ping";
		id_ = PingCmd_ID;

		addToRegistry();
	}

	 MaxCmdBuff compile(const std::string &input) const {
		 typedef std::string::const_iterator iterator;
		 PingParser<iterator> grammar;
		 iterator iter = input.begin();
		 iterator end = input.end();
		 PingCmd ping;
		 bool r = phrase_parse(iter, end, grammar, ascii::space, ping);
		 if (!r || iter != end) {
			 throw (std::runtime_error("parse Ping failed"));
		 } else {
			 MaxCmdBuff ret;
			 fillCmdBuff(ret, ping);
			 return ret;
		 }
	}

};

PingCmdInfo thePing;

}
