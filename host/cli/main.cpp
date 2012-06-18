
#include <boost/program_options.hpp>
#include <stdio.h>

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
	using namespace boost;
	program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("port,P", program_options::value<std::string>(), "communications port")
		;

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, desc), vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if (vm.count("port")) {
		std::cout << "port set to " << vm["port"].as<std::string>() << std::endl;
	}

}
