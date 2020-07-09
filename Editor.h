#pragma once

#include "utils.hpp"

#include <boost/process.hpp> 
#include <boost/assign/list_of.hpp> 

using namespace WShuffleUtils;

class Editor
{
protected:
	std::string editor_name_;
	std::string editor_config_;
	std::map<std::string, std::string> settings_;
	VStrList argumens_;
public:
	virtual VStrList& arguments()
	{
		return argumens_;
	}

	virtual void set_arguments(const VStrList& parameters)
	{
		argumens_ = parameters;
	}

	virtual std::map<std::string, std::string> &settings()
	{
		return settings_;
	}

#ifdef _MSVC_VER
	__declspec(property(get = settings)) std::map<std::string, std::string> Settings;
	__declspec(property(get = arguments, put = set_arguments)) VStrList Arguments;
#endif
	virtual ~Editor() = default;

	virtual std::string editor_name() const
	{
		return editor_name_;
	}

	virtual void set_editor_name(const std::string &name) {
		editor_name_ = name;
	}

	virtual void set_editor_config(const std::string &config_file) {
		editor_config_ = config_file;
	}



	virtual std::string editor_config() const
	{
		return editor_config_;
	}

	explicit Editor(std::string editor_name, std::string settings_file="editor.json");

	virtual int launch(const std::string &editor_name) = 0;
	virtual int launch() = 0;
	
	virtual void read_settings(const std::string &settings_file);
	virtual void read_settings();
};
