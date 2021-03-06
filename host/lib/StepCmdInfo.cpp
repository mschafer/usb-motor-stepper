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
#include "ums.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

using boost::optional;

namespace ums {

typedef boost::fusion::vector<optional<char>, optional<char>, optional<char>, optional<char> > ParsedStepDir;
typedef boost::tuple<ParsedStepDir, unsigned int> ParsedStepCmd;

template <typename Iterator>
struct StepParser : qi::grammar<Iterator, ParsedStepCmd(), ascii::space_type>
{
    StepParser() : StepParser::base_type(start, "Step")
    {
        xdir = qi::lexeme[ qi::omit[qi::char_("xX")] > qi::char_("+-0") ];
        ydir = qi::lexeme[ qi::omit[qi::char_("yY")] > qi::char_("+-0") ];
        zdir = qi::lexeme[ qi::omit[qi::char_("zZ")] > qi::char_("+-0") ];
        udir = qi::lexeme[ qi::omit[qi::char_("uU")] > qi::char_("+-0") ];

        start = qi::no_case[ qi::lit("step") ] >>
                (xdir ^ ydir ^ zdir ^ udir) >>
                qi::uint_;
    }

    qi::rule<Iterator, ParsedStepCmd(), ascii::space_type> start;
    qi::rule<Iterator, char(), ascii::space_type> xdir;
    qi::rule<Iterator, char(), ascii::space_type> ydir;
    qi::rule<Iterator, char(), ascii::space_type> zdir;
    qi::rule<Iterator, char(), ascii::space_type> udir;
};

/**
 * step x+ Y0 u+ z- 2345
 */
class StepCmdInfo : public CommandInfo
{
public:
	StepCmdInfo() {
		name_ = "Step";
		id_ = StepCmd_ID;

		addToRegistry();
	}

	 buffer_t compile(const std::string &input) const {
		 typedef std::string::const_iterator iterator;
		 StepParser<iterator> grammar;
		 iterator iter = input.begin();
		 iterator end = input.end();
		 ParsedStepCmd step;
		 bool r = phrase_parse(iter, end, grammar, ascii::space, step);
		 if (!r || iter != end) {
			 throw (std::runtime_error("parse Step failed"));
		 } else {
			 buffer_t ret;
			 fillCmdBuff(ret, step);
			 return ret;
		 }
	}

	std::string generate() const {
		return std::string("generated");
	}

	static void fillCmdBuff(buffer_t &m, const ParsedStepCmd &p)
	{
	    using boost::fusion::at_c;

	    m.resize(StepCmd_LENGTH);
	    StepCmd *s = reinterpret_cast<StepCmd*>(&m[0]);
	    s->cmdId = StepCmd_ID;

	    unsigned int delay = p.get<1>();
	    UMS_PACK_16(delay, s->delay);

	    ParsedStepDir sd = p.get<0>();
	    s->stepDir = 0;

	    if (at_c<0>(sd)  && at_c<0>(sd) != '0') s->stepDir |= UMS_X_STEP;
	    if (at_c<0>(sd) == '+') s->stepDir                 |= UMS_X_DIR;
	    if (at_c<1>(sd)  && at_c<1>(sd) != '0') s->stepDir |= UMS_Y_STEP;
	    if (at_c<1>(sd) == '+') s->stepDir                 |= UMS_Y_DIR;
	    if (at_c<2>(sd)  && at_c<2>(sd) != '0') s->stepDir |= UMS_Z_STEP;
	    if (at_c<2>(sd) == '+') s->stepDir                 |= UMS_Z_DIR;
	    if (at_c<3>(sd)  && at_c<3>(sd) != '0') s->stepDir |= UMS_U_STEP;
	    if (at_c<3>(sd) == '+') s->stepDir                 |= UMS_U_DIR;
	}
};

// static instance for the registry
StepCmdInfo theStep;

}
