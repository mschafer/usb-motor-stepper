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

typedef boost::fusion::vector<optional<int>, optional<int>, optional<int>, optional<int> > ParsedLineDelta;
typedef boost::tuple<ParsedLineDelta, unsigned int> ParsedLineCmd;

template <typename Iterator>
struct LineParser : qi::grammar<Iterator, ParsedLineCmd(), ascii::space_type>
{
    LineParser() : LineParser::base_type(start, "Line")
    {
        using qi::_val;
        using qi::_1;
        xaxis = qi::omit[qi::char_("xX")] > qi::int_;
        yaxis = qi::omit[qi::char_("yY")] > qi::int_;
        zaxis = qi::omit[qi::char_("zZ")] > qi::int_;
        uaxis = qi::omit[qi::char_("uU")] > qi::int_;

        start = qi::no_case[qi::lit("line")] >>
                (xaxis ^ yaxis ^ zaxis ^ uaxis) >>
                qi::uint_;
    }

    qi::rule<Iterator, ParsedLineCmd(), ascii::space_type> start;
    qi::rule<Iterator, int(), ascii::space_type> xaxis;
    qi::rule<Iterator, int(), ascii::space_type> yaxis;
    qi::rule<Iterator, int(), ascii::space_type> zaxis;
    qi::rule<Iterator, int(), ascii::space_type> uaxis;
};


class LineCmdInfo : public CommandInfo
{
public:
    LineCmdInfo() {
        name_ = "Line";
        id_ = LineCmd_ID;

        addToRegistry();
    }

    MaxCmdBuff compile(const std::string &input) const {
        typedef std::string::const_iterator iterator;
        LineParser<iterator> grammar;
        iterator iter = input.begin();
        iterator end = input.end();
        ParsedLineCmd line;
        bool r = phrase_parse(iter, end, grammar, ascii::space, line);
        if (!r || iter != end) {
            throw (std::runtime_error("parse Line failed"));
        } else {
            MaxCmdBuff ret;
            fillCmdBuff(&ret, line);
            return ret;
        }
    }

    std::string generate() const {
        return std::string("generated");
    }

    static void fillCmdBuff(MaxCmdBuff *m, const ParsedLineCmd &p)
    {
        using boost::fusion::at_c;

        LineCmd *l = reinterpret_cast<LineCmd *>(m);
        l->cmdId = LineCmd_ID;

        unsigned int delay = p.get<1>();
        l->delay_lo = delay & 0xFF;
        l->delay_hi = (delay >> 8) & 0xFF;

        ParsedLineDelta ld = p.get<0>();
        l->deltaX = at_c<0>(ld).get_value_or(0);
        l->deltaY = at_c<1>(ld).get_value_or(0);
        l->deltaZ = at_c<2>(ld).get_value_or(0);
        l->deltaU = at_c<3>(ld).get_value_or(0);

    }

};

// static instance for the registry
LineCmdInfo theLine;

}
