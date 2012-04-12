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

namespace umc {

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
class StepInfo : public CommandInfo
{
public:
	StepInfo() {
		name_ = "Step";
		id_ = StepCmd_ID;

		addToRegistry();
	}

	 MaxCmdBuff parse(const std::string &input) const {
		 typedef std::string::const_iterator iterator;
		 StepParser<iterator> grammar;
		 iterator iter = input.begin();
		 iterator end = input.end();
		 ParsedStepCmd step;
		 bool r = phrase_parse(iter, end, grammar, ascii::space, step);
		 if (!r || iter != end) {
			 throw (std::runtime_error("parse Step failed"));
		 } else {
			 MaxCmdBuff ret;
			 fillCmdBuff(&ret, step);
			 return ret;
		 }
	}

	std::string generate() const {
		return std::string("generated");
	}

	static void fillCmdBuff(MaxCmdBuff *m, const ParsedStepCmd &p)
	{
	    using boost::fusion::at_c;

	    StepCmd *s = reinterpret_cast<StepCmd*>(m);
	    s->cmdId = StepCmd_ID;

	    unsigned int delay = p.get<1>();
	    s->delay_lo = delay & 0xFF;
	    s->delay_hi = (delay >> 8) & 0xFF;

	    ParsedStepDir sd = p.get<0>();
	    s->stepDir = 0;

	    if (at_c<0>(sd)) s->stepDir        |= UMC_X_STEP;
	    if (at_c<0>(sd) == '+') s->stepDir |= UMC_X_DIR;
	    if (at_c<1>(sd)) s->stepDir        |= UMC_Y_STEP;
	    if (at_c<1>(sd) == '+') s->stepDir |= UMC_Y_DIR;
	    if (at_c<2>(sd)) s->stepDir        |= UMC_Z_STEP;
	    if (at_c<2>(sd) == '+') s->stepDir |= UMC_Z_DIR;
	    if (at_c<3>(sd)) s->stepDir        |= UMC_U_STEP;
	    if (at_c<3>(sd) == '+') s->stepDir |= UMC_U_DIR;
	}
};

// static instance for the registry
StepInfo theStep;

}
