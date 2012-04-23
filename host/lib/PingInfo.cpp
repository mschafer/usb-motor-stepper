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

BOOST_FUSION_ADAPT_STRUCT(NoOpCmd, (uint8_t, cmdId));

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


class PingInfo : public CommandInfo
{
public:
	PingInfo() {
		name_ = "Ping";
		id_ = PingCmd_ID;

		addToRegistry();
	}

	 MaxCmdBuff compile(const std::string &input) const {
		 typedef std::string::const_iterator iterator;
		 noop_parser<iterator> grammar;
		 iterator iter = input.begin();
		 iterator end = input.end();
		 NoOpCmd noop;
		 bool r = phrase_parse(iter, end, grammar, ascii::space, noop);
		 if (!r || iter != end) {
			 throw (std::runtime_error("parse NoOp failed"));
		 } else {
			 MaxCmdBuff ret;
			 fillCmdBuff(ret, noop);
			 return ret;
		 }
	}

};

PingInfo thePing;

}
