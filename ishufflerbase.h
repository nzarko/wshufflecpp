//
// Created by nick on 26/6/20.
//

#ifndef WSHUFFLE_ISHUFFLERBASE_H
#define WSHUFFLE_ISHUFFLERBASE_H


#include "utils.hpp"
using namespace WShuffleUtils;

class IShufflerBase {

public:
    IShufflerBase();
    virtual ~IShufflerBase();

    virtual const std::string version() const ;
    virtual bool write_to(const std::string &filename, const StringList& w_list = {}) = 0;
    virtual bool write_to() = 0;
    virtual bool write_to(const VStrList& files) = 0;
    virtual void setFilenames(const VStrList &file_list) = 0;
	virtual  bool read_from(const std::string &filename, StringList &w_list) = 0;
	virtual  bool read_from(const VStrList &file_list) = 0;
	virtual int exec() = 0;

};


#endif //WSHUFFLE_ISHUFFLERBASE_H
