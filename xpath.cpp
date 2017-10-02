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

namespace fs = boost::filesystem;
namespace po = boost::program_options;

pugi::xpath_node_set evaluateXPath(fs::path entry, pugi::xpath_query &xpath) {
    pugi::xpath_node_set set;
    if (fs::is_regular_file(entry)) {
        string extension = boost::algorithm::to_lower_copy(fs::extension(entry));
        if (extension == ".xml") {
            pugi::xml_document doc;
            if (doc.load_file(entry.string().c_str())) {
                if (xpath.evaluate_boolean(doc)) {
                    set = xpath.evaluate_node_set(doc);
                    return set;
                }
            }
        }
    }

    return set;
}

void printPathInfo(fs::path path, bool printSize) {
    vector<string> parts;
    parts.push_back(path.string().c_str());
    if (printSize) {
        parts.push_back("(" + (boost::lexical_cast<std::string>(fs::file_size(path) / 1024)) + " kb)");
    }

    cout << boost::algorithm::join(parts, " ") << endl;
}

void printNode(pugi::xpath_node_set nodeSet) {
    if (nodeSet.size() == 0) {
        return;
    }

    for (pugi::xpath_node_set::const_iterator it = nodeSet.begin(); it != nodeSet.end(); ++it) {
        pugi::xml_document doc;
        pugi::xpath_node node = *it;
        doc.append_copy(node.node());
        doc.print(cout);
        doc.reset();
    }

    cout << "-----------------------------------------" << endl;
}

int main(int argc, char **argv) {

    try {
        po::options_description desc("Options");

        vector<string> files;
        string expression = "";

        desc.add_options()
                ("help", "Prints help messages")
                ("verbose,v", "Verbose output")
                ("size,s", "Outputs file size")
                ("print,p", "Prints node")
                ("recursive,r", "Will search for files in directories recursively")
                ("expression,e", po::value<string>(&expression)->required(),
                 "XPath expression to test against XML files")
                ("file,f", po::value<vector<string> >(&files)->required(),
                 "File or directory where to look for XML files");


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
            <<
            "Example: xpath -r -e '/BetradarBetData/Sports/Sport[@BetradarSportID=\"19\"]/Category/Tournament/Match/MatchOdds/Bet[@OddsType=\"204\"]' -f /usr -f /etc" <<
            endl
            
            << "Example: xpath -e '//BetResult[count(W[@OddsType=\"20\"]) > 1]' -f ~/FILE.XML" << endl
            << endl

            << "Example: xpath -r -p -e '/TainOdds/Match/Odds[@subtype=\"10299\"]' -f ~/FILE.XML" << endl
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
                for (fs::directory_entry &entry : boost::make_iterator_range(fs::recursive_directory_iterator(path),
                                                                             {})) {
                    pugi::xpath_node_set nodeSet = evaluateXPath(entry.path(), xpath);
                    if (nodeSet.size()) {
                        printPathInfo(entry.path(), variables_map.count("size"));
                        if (variables_map.count("print")) {
                            printNode(nodeSet);
                        }
                    }
                }
            }
                // Single
            else if (fs::is_directory(path) && !variables_map.count("recursive")) {
                for (fs::directory_entry &entry : boost::make_iterator_range(fs::directory_iterator(path), {})) {
                    pugi::xpath_node_set nodeSet = evaluateXPath(entry.path(), xpath);
                    if (nodeSet.size()) {
                        printPathInfo(entry.path(), variables_map.count("size"));
                        if (variables_map.count("print")) {
                            printNode(nodeSet);
                        }
                    }

                }
            }
                // File
            else {
                fs::path path(files[i]);
                pugi::xpath_node_set nodeSet = evaluateXPath(path, xpath);
                if (nodeSet.size()) {
                    printPathInfo(path, variables_map.count("size"));
                    if (variables_map.count("print")) {
                        printNode(nodeSet);
                    }
                }
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
