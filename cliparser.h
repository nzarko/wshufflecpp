//
// Created by nick on 26/6/20.
//

#ifndef WSHUFFLE_CLIPARSER_H
#define WSHUFFLE_CLIPARSER_H
#include <memory>
#include "utils.hpp"
#include "shufflerbase.h"

using StringList = WShuffleUtils::StringList ;

class CLIParser {
private:
    int _argc;
    char ** _argv;
    StringList _args_list;
    std::shared_ptr<WShuffle> spWShuffle;
public:
    std::shared_ptr<WShuffle>& sp_w_shuffle() {
	    return spWShuffle;
    }

    void set_sp_w_shuffle(const std::shared_ptr<WShuffle>& sp_w_shuffle) {
	    spWShuffle = sp_w_shuffle;
    }
#ifdef _MSC_VER
    __declspec(property(get = sp_w_shuffle, put = set_sp_w_shuffle)) std::shared_ptr<WShuffle> SpWShuffle;
#endif
    CLIParser(int argc, char *argv[]);
    bool parse() const;
    virtual ~CLIParser();
    const StringList &args();
    const int &argsCount();
};


#endif //WSHUFFLE_CLIPARSER_H
