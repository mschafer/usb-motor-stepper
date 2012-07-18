
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <stdio.h>
#include "Host.hpp"
#include "ums.h"

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

struct Executor
{
	Executor(ums::Host *host, istream &in) :
		host_(host), in_(in) {}

	void operator()() {
		try {
			host_->execute(in_);
		} catch (std::exception &ex) {
			cerr << ex.what() << endl;
			throw;
		}
	}

	ums::Host *host_;
	istream &in_;
};


int main(int argc, char *argv[])
{
	program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("port,P", program_options::value<string>(), "communications port")
		("output,O", program_options::value<string>(), "output file for simulation results")
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
		exit(0);
	}


#if 1 // use this to pause long enough to attach xcode
	string line;
	getline(cin, line);
#endif

	// attempt to connect to a port if one was specified, otherwise connect to sim
	// terminate if an error occurs
	auto_ptr<ums::Host> host;
	auto_ptr<ofstream> out;
	string outName("sim.out");
	if (vm.count("port")) {
		cout << "Connecting to port " << vm["port"].as<string>() << endl;
		string portName = vm["port"].as<string>();
		cout << "attempting connection to " << portName << endl;
		host =  connectToPort(portName);
	} else {
		host.reset(new ums::Host());
		if (vm.count("output")) {
			outName = vm["output"].as<string>();
		}
		out.reset(new ofstream(outName.c_str(), ios_base::out));
		*out << "X\tY\tZ\tU\ttime" << endl;
	}

	// error if no input files were provided
	if (vm.count("input-file") == 0) {
		cerr << "error: no input files" << endl;
		exit(-1);
	}

	// process input files
	host->enableDevice();
	cout << "connection succeeded" << endl;
	const vector<string> &infiles = vm["input-file"].as<vector<string> >();
	BOOST_FOREACH(string fname, infiles) {
		cout << "executing " << fname << endl;
		fstream fin(fname.c_str(), ios_base::in);

		boost::thread exe(Executor(host.get(), fin));
		bool exeDone = false;
		bool running = true;
		while (!exeDone || running) {
			if (!exeDone) {
				exeDone = exe.timed_join(boost::posix_time::milliseconds(1));
				cout << "exeDone " << exeDone << endl;
			}

			ums::MessageInfo::buffer_t msg = host->receiveMessage();
			if (msg.size() != 0) {
				cout << "message received" << endl;
				cout << ums::MessageInfo::toString(msg);
				if (msg[0] == StatusMsg_ID) {
					StatusMsg *sm = (StatusMsg*)(&msg[0]);
					running = (sm->flags & UMS_STEPPER_RUNNING) != 0;
				}
			}
		}

		// record the simulator log
		if (out.get() != NULL) {
			std::deque<ums::Simulator::position_t> simLog = host->simulatorPositionLog();
			BOOST_FOREACH (ums::Simulator::position_t pos, simLog) {
				BOOST_FOREACH (ptrdiff_t p, pos) {
					*out << p << "\t";
				}
				*out << endl;
			}
		}
	}

	exit(0);
}
