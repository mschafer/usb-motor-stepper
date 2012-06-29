#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/foreach.hpp>
#include <deque>
#include "SerialLink.hpp"

BOOST_AUTO_TEST_CASE ( serial_test )
{
	try {
		ums::SerialLink master("/dev/master");
		BOOST_CHECK(master.open());
		ums::SerialLink slave("/dev/slave");
		BOOST_CHECK(slave.open());

		std::vector<uint8_t> send(10);
		for (size_t i=0; i<10; i++) send[i] = i;
		master.write(send);

		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

		std::deque<uint8_t> rx = slave.read();
		BOOST_CHECK(rx.size() == 10);
		for (size_t i=0; i<10; i++) {
			BOOST_CHECK(rx[i] == i);
		}

		slave.write(send);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		rx = master.read();
		for (size_t i=0; i<10; i++) {
			BOOST_CHECK(rx[i] == i);
		}

	} catch (...) {
		// failed to open ports
		return;
	}

}
