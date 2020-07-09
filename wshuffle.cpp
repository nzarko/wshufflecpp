// wshuffle.cpp : Defines the entry point for the application.
//


#include <iostream>

#include "shufflerbase.h"
#include "cliparser.h"

using namespace std;
using namespace WShuffleUtils;

const std::string &WSHUFFLE_VERSION = "1.0.0";

void greeter()
{
	std::cout << "WShuffle " << WSHUFFLE_VERSION << std::endl;
	std::cout << "Copyright (C) 2020 Nick Zarkadas (zarkadas.nick@gmail.com)";
	std::cout << std::endl;
}
void test_impl();
int main(int argc, char *argv[])
{
#ifdef _MSC_VER
	UINT oldcp = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);
#endif
	std::cout << "Program sources : " << WSHSRC << std::endl;
    init_app();
	greeter();
	systemInfo(false);
	CLIParser cliParser(argc, argv);
	if (!cliParser.parse()) {
		cerr << "Error parsing command line arguments";
		exit(-1);
	}
	int res =  cliParser.sp_w_shuffle()->exec();
#ifdef _MSC_VER
	SetConsoleOutputCP(oldcp);
#endif
	return res;

}

void test_impl()
{
	StringList slist = { "Giorgos", "Nikos", "Zwh", "Baggelis" };
	cout << slist;

	StringList fls{ "test.txt","C:\\src\\python\\test.py", "C:/Program Files/Boost/asio.cpp",
				   "/home/user/src/wshuffle.cpp", "../python/wshuffle/wshuffle.py" };
	for (auto &x : fls)
	{
		std::string f_in{ x };
		std::string f_out;
		create_out_file(f_in, f_out);
		std::cout << "Output file : " << f_out << std::endl;
	}
}