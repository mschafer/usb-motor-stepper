
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <stdio.h>
#include "Host.hpp"

using namespace std;
using namespace boost;

template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}

/**
 * Create a host connected to a device via port.
 * An exception is thrown if the connection fails.
 * \return A host with a valid connection.
 */
auto_ptr<ums::Host>
connectToPort(const string &port)
{
	return auto_ptr<ums::Host>(new ums::Host(port));
}

void prompt()
{
	string line;
	while(1) {
		cout << "stepper > ";
		getline(cin, line);
		if (line.compare("exit") == 0) {
			break;
		}
	}
}

/**
 * commands:
 * step
 * line
 * pause
 * file
 * quit
 * port
 */

/**
 * command line options
 * port
 * file name* (process commands in file, then quit)
 * simulator output file name
 */

int main(int argc, char *argv[])
{
	program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("port,P", program_options::value<string>(), "communications port")
		("input-file", program_options::value<vector<string> >(), "input file")
		;

	program_options::positional_options_description pos_desc;
	pos_desc.add("input-file", -1);

	program_options::variables_map vm;
	program_options::store(
			program_options::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(),
			vm);

	if (vm.count("help")) {
		cout << desc << endl;
		return 0;
	}

	// attempt to connect to a port if one was specified, otherwise connect to sim
	// terminate if an error occurs
	auto_ptr<ums::Host> host;
	if (vm.count("port")) {
		cout << "Connecting to port " << vm["port"].as<string>() << endl;
		host =  connectToPort(vm["port"].as<string>());
	} else {
		host.reset(new ums::Host());
	}

	// process input files
    if (vm.count("input-file"))
    {
    	const vector<string> &infiles = vm["input-file"].as<vector<string> >();
    	BOOST_FOREACH(string fname, infiles) {
    		fstream fin(fname.c_str(), ios_base::in);
    		host->execute(fin);
    	}
    } else {
    	prompt();
    }

    return 0;
}
