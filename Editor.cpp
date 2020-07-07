#include "Editor.h"

#include <utility>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

// Short alias for this namespace
namespace pt = boost::property_tree;

Editor::Editor(std::string editor_name, std::string settings_file) :
	editor_name_(std::move(editor_name)),
	editor_config_(std::move(settings_file))
{
	read_settings(settings_file);
}

void Editor::read_settings(const std::string & settings_file)
{
	///TODO Implement me!!!
	// Create a root
	pt::ptree root;

	// Load the json file in this ptree
	pt::read_json("editor.json", root);
	settings_["name"] = root.get<std::string>("editor_name");
	settings_["path"] = root.get<std::string>("editor_path");
	settings_["exec"] = root.get<std::string>("editor_exec");
	std::cout << "Editor Settings : " << '\n';
	for (const auto &x : settings_) {
		std::cout << "\t" << x.first << " : " << x.second << '\n';
	}
	set_editor_name(settings_["name"]);
}

void Editor::read_settings()
{
	read_settings(editor_config_);
}
