#pragma once
#include "Editor.h"
class WinEditor : 	public Editor
{
private:
	size_t start_editor(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait);
	size_t start_editor(std::string FullPathToExe, std::string Parameters, size_t SecondsToWait);
public:
	explicit WinEditor(std::string editor_name, std::string settings_file = "editor.json");
	~WinEditor() override;

	int launch(const std::string &editor_name) override;
	int launch() override;
	std::wstring convert(const std::string& input);
	
};
std::shared_ptr<WinEditor> create_editor();
