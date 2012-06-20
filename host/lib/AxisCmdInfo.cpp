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
#include <boost/foreach.hpp>
#include "CommandInfo.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

namespace ums {

typedef boost::fusion::vector<boost::optional<char>, unsigned int, unsigned int> ParsedPortPin;

template <typename Iterator>
struct PortPinParser : qi::grammar<Iterator, ParsedPortPin(), ascii::space_type>
{
    typedef qi::uint_parser<uint8_t, 10, 1, 2> uint2_p;
    PortPinParser(const char *pinName) : PortPinParser::base_type(start, "PortPin")
    {

        port = qi::omit[qi::char_("pP")] > qi::uint_;
        pin = qi::omit[qi::char_('_')] > qi::uint_;

        start = -qi::char_("+-") >> qi::no_case[qi::lit(pinName)] > ':' > port > pin;

        qi::on_error<qi::fail>(start,
                std::cerr
                << phx::val("Error! Expecting ")
                << qi::_4                               // what failed?
                << phx::val(" here: \"")
                << phx::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
                << phx::val("\"")
                << std::endl
        );
    }

    qi::rule<Iterator, unsigned int(), ascii::space_type> port;
    qi::rule<Iterator, unsigned int(), ascii::space_type> pin;
    qi::rule<Iterator, ParsedPortPin(), ascii::space_type> start;
};

typedef boost::fusion::vector<boost::optional<ParsedPortPin>, boost::optional<ParsedPortPin>,
        boost::optional<ParsedPortPin>, boost::optional<ParsedPortPin> > PinVec_t;
typedef boost::fusion::vector<char, PinVec_t> ParsedAxis;

template <typename Iterator>
struct AxisParser : qi::grammar<Iterator, ParsedAxis(), ascii::space_type>
{
    AxisParser() : AxisParser::base_type(start, "Axis"), stepPin("step"),
            dirPin("dir"), fwdPin("fwd"), revPin("rev")
    {
        start = qi::no_case[qi::lit("axis")] >
        qi::char_("xXyYzZuU") >>
        (stepPin ^ dirPin ^ fwdPin ^ revPin);

        qi::on_error<qi::fail>(start,
                std::cerr
                << phx::val("Error! Expecting ")
                << qi::_4                               // what failed?
                << phx::val(" here: \"")
                << phx::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
                << phx::val("\"")
                << std::endl
        );
    }

    qi::rule<Iterator, ParsedAxis(), ascii::space_type> start;
    PortPinParser<Iterator> stepPin;
    PortPinParser<Iterator> dirPin;
    PortPinParser<Iterator> fwdPin;
    PortPinParser<Iterator> revPin;
};

/**
 * axis +step:p2_7 dir:p0_2 -fwd:p2_3 +rev:p2_4
 */
class AxisCmdInfo : public CommandInfo
{
public:
	AxisCmdInfo() {
		name_ = "Axis";
		id_ = AxisCmd_ID;

		addToRegistry();
	}

	 buffer_t compile(const std::string &input) const {
		 typedef std::string::const_iterator iterator;
		 AxisParser<iterator> grammar;
		 iterator iter = input.begin();
		 iterator end = input.end();
         ParsedAxis pa;
		 bool r = phrase_parse(iter, end, grammar, ascii::space, pa);
		 if (!r || iter != end) {
			 throw (std::runtime_error("parse Axis failed"));
		 } else {
			 buffer_t ret;
			 createCmd(ret, pa);
			 return ret;
		 }
	}

	static void createCmd(buffer_t &cmd, const ParsedAxis &pa)
	{
	    using boost::fusion::at_c;

	    cmd.resize(AxisCmd_LENGTH);
	    AxisCmd &ac = *reinterpret_cast<AxisCmd *>(&cmd[0]);
	    ac.cmdId = AxisCmd_ID;
	    ac.name = at_c<0>(pa);
	    PinVec_t pinvec = at_c<1>(pa);

        ac.stepPort = UMS_UNASSIGNED_PORT; ac.stepPin = 0;
        ac.dirPort = UMS_UNASSIGNED_PORT; ac.dirPin = 0;
        ac.fwdPort = UMS_UNASSIGNED_PORT; ac.fwdPin = 0;
        ac.revPort = UMS_UNASSIGNED_PORT; ac.revPin = 0;

        //step
	    if (at_c<0>(pinvec)) {
	        ParsedPortPin pin = at_c<0>(pinvec).get();
	        fillPortPin(pin, ac.stepPort, ac.stepPin);
	    }

        //dir
        if (at_c<1>(pinvec)) {
            ParsedPortPin pin = at_c<1>(pinvec).get();
            fillPortPin(pin, ac.dirPort, ac.dirPin);
        }

        //fwd
        if (at_c<2>(pinvec)) {
            ParsedPortPin pin = at_c<2>(pinvec).get();
            fillPortPin(pin, ac.fwdPort, ac.fwdPin);
        }

        //rev
        if (at_c<3>(pinvec)) {
            ParsedPortPin pin = at_c<3>(pinvec).get();
            fillPortPin(pin, ac.revPort, ac.revPin);
        }
	}

	static void fillPortPin(const ParsedPortPin &pp, uint8_t &port, uint8_t &pin)
	{
        using boost::fusion::at_c;

	    char s = at_c<0>(pp).get_value_or('+');
	    bool inverted = (s == '-');
        unsigned int uport = at_c<1>(pp);
        unsigned int upin = at_c<2>(pp);
        if (uport > 255 || upin > 15) {
            throw std::runtime_error("error: port # > 255 or pin # > 15");
        }
        port = uport & 0xFF;
        pin = upin & 0xFF;
        if (inverted) {
            upin |= UMS_INVERT_PIN;
        }
	}
};

// static instance for the registry
AxisCmdInfo theAxis;

}
