#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std;

int main(int argc, char* argv[]) {
	// Declare the supported options.
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("recursive", po::value<int>(), "set compression level")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << desc << "Help\n";
	    return 1;
	}

	if (vm.count("compression")) {
	    cout << "Compression level was set to "
	 << vm["compression"].as<int>() << ".\n";
	} else {
	    cout << "Compression level was not set.\n";
	}

    return 0;
}
