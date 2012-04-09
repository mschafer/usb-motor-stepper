#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Test Engine
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vector>
#include <cfloat>
#include <boost/bind.hpp>
#include <boost/function.hpp>

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

typedef std::vector<double> V;

double f1(int x, const V &u)
{
	return u[0] + double(x);
}

struct Input
{
	typedef double result_type;

	Input() : last_t_(DBL_MAX), u_(2) {
		u_[0] = u_[1] = 0.;
	}

	double operator()(int x, double t) {
		if ( t != last_t_) {
			update(t);
			last_t_ = t;
		}
		return f1(x ,u_);
	}

	const V &update(double t) {
		u_[0] = t;
		u_[2]++;
		return u_;
	}

	double last_t_;
	std::vector<double> u_;
};

const V& stub(V &v) { return v; }

BOOST_AUTO_TEST_CASE( composition_test )
{
	V v(2);
	v[0] = 0; v[1] = 0;
	boost::function<double (int)> f = boost::bind(f1, _1, boost::bind(stub, boost::ref(v)));
	v[0] = 3.;
	BOOST_CHECK_CLOSE_FRACTION(4., f(1), 1.e-8);

	Input in;
	boost::function<double (int, double)> c = boost::bind(boost::ref(in), _1, _2);
	BOOST_CHECK_CLOSE_FRACTION(c(1, 0.), 1., 1.e-8);
	BOOST_CHECK_EQUAL(in.u_[0], 0.);
	BOOST_CHECK_CLOSE_FRACTION(c(2, 0.), 2., 1.e-8);
	BOOST_CHECK_EQUAL(in.u_[0], 0.);

	BOOST_CHECK_CLOSE_FRACTION(c(1, 1.), 2., 1.e-8);
	BOOST_CHECK_EQUAL(in.u_[0], 1.);
	BOOST_CHECK_CLOSE_FRACTION(c(2, 1.), 3., 1.e-8);
	BOOST_CHECK_EQUAL(in.u_[0], 1.);
}
