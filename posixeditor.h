//
// Created by nick on 7/7/20.
//

#ifndef WSHUFFLE_POSIXEDITOR_H
#define WSHUFFLE_POSIXEDITOR_H
#include "Editor.h"

class ProcessManager;

class PosixEditor :public Editor {
private:
    size_t start_editor(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait);
public:
    explicit PosixEditor(std::string editor_name, std::string settings_file = "editor.json");
    ~PosixEditor() override;

    int launch(const std::string &editor_name) override;
    int launch() override;

};

std::shared_ptr<PosixEditor> create_editor();
#endif //WSHUFFLE_POSIXEDITOR_H
