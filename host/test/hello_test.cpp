#define BOOST_TEST_MAIN
//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Test Engine
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vector>
#include <cfloat>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "MemoryLink.hpp"

int add( int i, int j ) { return i+j; }

BOOST_AUTO_TEST_CASE( hello_test )
{
    // seven ways to detect and report the same error:
    BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error
    
    BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error
    
    if( add( 2,2 ) != 4 )
        BOOST_ERROR( "Ouch..." );            // #3 continues on error
    
    if( add( 2,2 ) != 4 )
        BOOST_FAIL( "Ouch..." );             // #4 throws on error
    
    if( add( 2,2 ) != 4 ) throw "Ouch..."; // #5 throws on error
    
    BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                        "add(..) result: " << add( 2,2 ) );
    
    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );	  // #7 continues on error
}

BOOST_AUTO_TEST_CASE( memory_link_test)
{
	std::pair<ums::MemoryLink::handle, ums::MemoryLink::handle> links = ums::MemoryLink::createPair();

	std::vector<uint8_t> v;
	v.push_back(1);
	links.first->write(v);

	boost::optional<uint8_t> b;
	b = links.second->readByte();

	BOOST_CHECK(b && b.get() == 1);

	v[0] = 2;
	links.second->write(v);
	b = links.first->readByte();
	BOOST_CHECK(b && b.get() == 2);
}
