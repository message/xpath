#include <iostream>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "lib/pugixml/src/pugixml.hpp"

namespace {
	const size_t ERROR_IN_COMMAND_LINE = 1;
	const size_t SUCCESS = 0;
	const size_t ERROR_UNHANDLED_EXCEPTION = 2;

}

int main(int argc, char **argv) {

	try {
		namespace po = boost::program_options;

		po::options_description desc("Options");

		std::vector<std::string> files;
		std::string expression = "";

		desc.add_options()
			("help", "Prints help messages")
			("verbose,v", "Verbose output")
			("recursive,r", "Will search for files in directories recursively")
			("expression,e", po::value<std::string>(&expression)->required(), "XPath expression to test against XML files")
			("file,f", po::value<std::vector<std::string> >(&files)->required(), "File or directory where to look for XML files");


		po::variables_map variables_map;
		std::string usage = "Usage: xpath [options...]";

		try {
			po::store(po::parse_command_line(argc, argv, desc), variables_map); // can throw

			if (variables_map.count("help")) {
				std::cout << usage << std::endl
					<< desc << std::endl;
				return SUCCESS;
			}

			po::notify(variables_map); // throws on error, so do after help in case there are any problems
		}
		catch (po::error &e) {
			std::cerr << "ERROR: " << e.what() << std::endl
				<< std::endl << usage << std::endl << std::endl
				<< "Example: xpath -r -e '/BetradarBetData/Sports/Sport[@BetradarSportID=\"1\"]' -f /usr -f /etc" << std::endl
				<< std::endl;
			std::cerr << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}

		// Check if xpath is valid
		boost::algorithm::trim(expression);

		for
		// Collect all xml files
		// Run threads
		std::cout << "Cores: " << std::thread::hardware_concurrency() << std::endl;

	}
	catch (std::exception &e) {
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;

		return ERROR_UNHANDLED_EXCEPTION;
	}

	return SUCCESS;

} // main
