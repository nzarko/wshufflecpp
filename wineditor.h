#pragma once
#include "Editor.h"
class WinEditor : 	public Editor
{
private:
	size_t start_editor(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait);
public:
	explicit WinEditor(std::string editor_name, std::string settings_file = "editor.json");
	~WinEditor() override;

	int launch(const std::string &editor_name) override;
	int launch() override;
};

