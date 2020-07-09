#include "Editor.h"

#include <utility>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>

// Short alias for this namespace
namespace pt = boost::property_tree;

Editor::Editor(std::string editor_name, std::string settings_file) :
	editor_name_(std::move(editor_name)),
	editor_config_(std::move(settings_file))
{
    //std::cout << "editor settings file : " << settings_file_path(editor_config_) << '\n';
	read_settings(settings_file_path(editor_config_));
}

void Editor::read_settings(const std::string & settings_file)
{
	///TODO Implement me!!!
	// Create a root
	pt::ptree root;

	// Load the json file in this ptree
	pt::read_json(settings_file, root);
	pt::ptree os_root;
#ifdef _MSC_VER
	os_root = root.get_child("Windows");
#elif defined(__GNUC__)
	os_root = root.get_child("Unix");
#endif
	//
	StringList names;
	BOOST_FOREACH(pt::ptree::value_type &v, os_root.get_child("editors")) {
	    assert(v.first.empty());
	    names.push_back(v.second.data());
	}

    //
    settings_["name"] = names.empty() ? "posix editor" : names.front();
	settings_["path"] = os_root.get<std::string>("editor_path");
	settings_["exec"] = os_root.get<std::string>("editor_exec");
//	std::cout << "Editor Settings : " << '\n';
//	for (const auto &x : settings_) {
//		std::cout << "\t" << x.first << " : " << x.second << '\n';
//	}
	set_editor_name(settings_["name"]);
}

void Editor::read_settings()
{
	read_settings(editor_config_);
}
