#include <iostream>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "lib/pugixml/src/pugixml.hpp"

using namespace std;

namespace {
	const size_t ERROR_IN_COMMAND_LINE = 1;
	const size_t SUCCESS = 0;
	const size_t ERROR_UNHANDLED_EXCEPTION = 2;

}

void evaluateXPath(boost::filesystem::path entry, pugi::xpath_query &xpath) {
	namespace fs = boost::filesystem;

	if (fs::is_regular_file(entry)) {
		string extension = boost::algorithm::to_lower_copy(fs::extension(entry));
		if (extension == ".xml") {
			pugi::xml_document doc;
			if (doc.load_file(entry.string().c_str())) {
				if(xpath.evaluate_boolean(doc)) {
					cout << entry << endl;
				}
			} else {
//				cout << "Failed to load: " << entry << endl;
			}

		}
	}
}

int main(int argc, char **argv) {

	try {
		namespace po = boost::program_options;
		namespace fs = boost::filesystem;

		po::options_description desc("Options");

		vector<string> files;
		string expression = "";

		desc.add_options()
			("help", "Prints help messages")
			("verbose,v", "Verbose output")
			("recursive,r", "Will search for files in directories recursively")
			("expression,e", po::value<string>(&expression)->required(), "XPath expression to test against XML files")
			("file,f", po::value<vector<string> >(&files)->required(), "File or directory where to look for XML files");


		po::variables_map variables_map;
		string usage = "Usage: xpath [options...]";

		try {
			po::store(po::parse_command_line(argc, argv, desc), variables_map); // can throw

			if (variables_map.count("help")) {
				cout << usage << endl
					<< desc << endl;
				return SUCCESS;
			}

			po::notify(variables_map); // throws on error, so do after help in case there are any problems
		}
		catch (po::error &e) {
			cerr << "ERROR: " << e.what() << endl
				<< endl << usage << endl << endl
				<< "Example: xpath -r -e '/BetradarBetData/Sports/Sport[@BetradarSportID=\"1\"]' -f /usr -f /etc" << endl
				<< endl;
			cerr << desc << endl;
			return ERROR_IN_COMMAND_LINE;
		}

		// Check if xpath is valid
		boost::algorithm::trim(expression);
		pugi::xpath_query xpath(expression.c_str());

		vector<string> paths;
		// Collect all xml files
		for (size_t i = 0; i < files.size(); ++i) {
			boost::algorithm::trim(files[i]);
			if (files[i].size() == 0) {
				if (variables_map.count("verbose")) {
					cout << "Empty path passed" << endl;
				}
				continue;
			}

			fs::path path(files[i]);
			if (!fs::exists(path)) {
				if (variables_map.count("verbose")) {
					cout << "Path " << files[i] << " doesn't exist" << endl;
				}
				continue;
			}

			// Recursive
			if (fs::is_directory(path) && variables_map.count("recursive")) {
				for (fs::directory_entry &entry : boost::make_iterator_range(fs::recursive_directory_iterator(path), {}))
					evaluateXPath(entry.path(), xpath);
			}
				// Single
			else if (fs::is_directory(path) && !variables_map.count("recursive")) {
				for (fs::directory_entry &entry : boost::make_iterator_range(fs::directory_iterator(path), {}))
					evaluateXPath(entry.path(), xpath);
			}
				// File
			else {
				fs::path path(files[i]);
				evaluateXPath(path, xpath);
			}

		}
	}
	catch (exception &e) {
		cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << endl;

		return ERROR_UNHANDLED_EXCEPTION;
	}

	return SUCCESS;
}