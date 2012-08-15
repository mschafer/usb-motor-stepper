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

    buffer_t compile(const std::string &input) const {
        typedef std::string::const_iterator iterator;
        LineParser<iterator> grammar;
        iterator iter = input.begin();
        iterator end = input.end();
        ParsedLineCmd line;
        bool r = phrase_parse(iter, end, grammar, ascii::space, line);
        if (!r || iter != end) {
            throw (std::runtime_error("parse Line failed"));
        } else {
            buffer_t ret;
            fillCmdBuff(ret, line);
            return ret;
        }
    }

    std::string generate() const {
        return std::string("generated");
    }

    static void fillCmdBuff(buffer_t &m, const ParsedLineCmd &p)
    {
        using boost::fusion::at_c;

        ParsedLineDelta ld = p.get<0>();
        int deltaX = at_c<0>(ld).get_value_or(0);
        int deltaY = at_c<1>(ld).get_value_or(0);
        int deltaZ = at_c<2>(ld).get_value_or(0);
        int deltaU = at_c<3>(ld).get_value_or(0);

        int deltaMax = abs(deltaX);
        deltaMax = (deltaMax < abs(deltaY)) ? abs(deltaY) : deltaMax;
        deltaMax = (deltaMax < abs(deltaZ)) ? abs(deltaZ) : deltaMax;
        deltaMax = (deltaMax < abs(deltaU)) ? abs(deltaU) : deltaMax;

        if (deltaMax > INT16_MAX) {
        	throw std::runtime_error("LineCmd parse: overflow, deltas must fit in 16 bit signed integer");
        }

        unsigned int delay = p.get<1>();
        if (delay > UINT32_MAX) {
        	throw std::runtime_error("LineCmd parse: overflow, delay must fit in 32 bit unsigned integer");
        }

        // if the deltas are too big, then we need a long line instead of a line
        if (deltaMax > INT8_MAX || delay > UINT16_MAX) {
            m.resize(LongLineCmd_LENGTH);
            LongLineCmd *l = reinterpret_cast<LongLineCmd *>(&m[0]);
            l->cmdId = LongLineCmd_ID;
            UMS_PACK_16(deltaX, l->deltaX);
            UMS_PACK_16(deltaY, l->deltaY);
            UMS_PACK_16(deltaZ, l->deltaZ);
            UMS_PACK_16(deltaU, l->deltaU);
            UMS_PACK_32(delay, l->delay);
        } else {
            m.resize(LineCmd_LENGTH);
            LineCmd *l = reinterpret_cast<LineCmd *>(&m[0]);
            l->cmdId = LineCmd_ID;
            l->deltaX = (int8_t)deltaX;
            l->deltaY = (int8_t)deltaY;
            l->deltaZ = (int8_t)deltaZ;
            l->deltaU = (int8_t)deltaU;
            UMS_PACK_16(delay, l->delay);
        }
    }

};

// static instance for the registry
LineCmdInfo theLine;

}
