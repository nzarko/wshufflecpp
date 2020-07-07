//
// Created by nick on 26/6/20.
//

#include "cliparser.h"
#include <iostream>
#include <boost/program_options.hpp>

CLIParser::CLIParser(int argc, char **argv):
        _argc(argc), _argv(argv)
{
    std::cout << "Number of arguments : " << _argc << '\n';
    std::cout << "Arguments : " << args() << std::endl;
    WShuffleUtils::VStrList input_file_list;
    spWShuffle = std::make_shared<WShuffle>(input_file_list, 3, 5);
}

bool CLIParser::parse() const
{
    using namespace std;
    namespace po = boost::program_options;
    try {
        int opt;
        int between;
        int repetitions;
        bool shuffle;
        string config_file;

        // Declare a group of options that will be
        // allowed only on command line
        po::options_description generic("Generic options");
        generic.add_options()
                ("version,v", "print version string")
                ("help", "produce help message")
                ("config,c", po::value<string>(&config_file)->default_value("wshuffle.cfg"),
                 "name of a file of a configuration.")
                ;

        // Declare a group of options that will be
        // allowed both on command line and in
        // config file
        po::options_description config("Configuration");
        config.add_options()
                ("optimization", po::value<int>(&opt)->default_value(10),
                 "optimization level")
                ("between,b", po::value<int>(&between)->default_value(3),
                 "Number of lines between repetitions.")
                ("repetitions,r",po::value<int>(&repetitions)->default_value(5),
                 "Number of translations block repetitions.")
                ("ns", po::bool_switch(&shuffle), "Shuffle or not.")
                ("include-path,I",
                 po::value< vector<string> >()->composing(),
                 "include path")
                ;

        // Hidden options, will be allowed both on command line and
        // in config file, but will not be shown to the user.
        po::options_description hidden("Hidden options");
        hidden.add_options()
                ("input-file", po::value< vector<string> >()->required(), "input file")
                ;


        po::options_description cmdline_options;
        cmdline_options.add(generic).add(config).add(hidden);

        po::options_description config_file_options;
        config_file_options.add(config).add(hidden);

        po::options_description visible("Allowed options");
        visible.add(generic).add(config);

        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;
        store(po::command_line_parser(_argc, _argv).
                options(cmdline_options).positional(p).run(), vm);
        notify(vm);

        ifstream ifs(config_file.c_str());
        if (!ifs)
        {
            cout << "can not open config file: " << config_file << "\n";
            return false;
        }
        else
        {
            store(parse_config_file(ifs, config_file_options), vm);
            notify(vm);
        }

        if (vm.count("help")) {
            cout << visible << "\n";
            return true;
        }

        if (vm.count("version")) {
            cout << spWShuffle->version() << endl;
            return true;
        }

        if (vm.count("include-path"))
        {
            cout << "Include paths are: "
                 << vm["include-path"].as< vector<string> >() << "\n";
        }

        if(vm.count("repetitions")) {
            cout << "Repetitions are : " << vm["repetitions"].as<int>() << '\n';
            spWShuffle->setRepetitions(vm["repetitions"].as<int>());
        }

        if(vm.count("between")) {
            cout << "Lines between blocks are : " << vm["between"].as<int>() << '\n';
            spWShuffle->setLinesBetween(vm["between"].as<int>());
        }


        if (vm.count("input-file"))
        {
            cout << "Input files are: " << '\n'
                 << vm["input-file"].as< vector<string> >() << "\n";
            spWShuffle->setFilenames(vm["input-file"].as< vector<string> >());
            cout << "Output files are : \n" << spWShuffle->getFileOut() << std::endl;
			cout << "Editor outfiles are : \n" << spWShuffle->SpEditor->Arguments;
        }

        cout << "Optimization level is " << opt << "\n";
        cout << "Between is :" << between << "\n";
		spWShuffle->setLinesBetween(between);
        cout << "Repetitions are : " << repetitions << "\n";
		spWShuffle->setRepetitions(repetitions);
        if(vm.count("ns")) {
			shuffle = vm["ns"].as<bool>();
            if (shuffle)
                cout << "\"No Shuffle (--ns )\" has been set" << "\n";
            else {
                cout << "\"No Shuffle (--ns )\" has not been set" << "\n";
            }
			spWShuffle->set_shuffle(shuffle);
        }
        cout << spWShuffle->__str();
		//spWShuffle->SpEditor->launch();
    	
    	return true;
    	
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return false;
    }
    return true;
}

CLIParser::~CLIParser() {

}

const StringList &CLIParser::args() {
    for(int i = 0; i < _argc; ++i)
        _args_list.push_back(_argv[i]);
    return _args_list;
}

const int &CLIParser::argsCount() {
    return _argc;
}