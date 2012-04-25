#ifndef ums_Host_hpp
#define ums_Host_hpp

#include <boost/utility.hpp>

namespace ums {

class Host : boost::noncopyable {
public:

	/** functionality:
	 * enable()
	 * sendCommands(istream)
	 * registerMessageHandler()
	 *
	 */

	static const std::string SIMULATOR_NAME;

	Host(const std::string &linkName);
	virtual ~Host();

	void enableDevice() const;


private:
	Host();
};

}

#endif

