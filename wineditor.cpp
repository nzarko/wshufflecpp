#include "wineditor.h"

#include <boost/format/free_funcs.hpp>
#include <iostream>

namespace bp = boost::process;

size_t WinEditor::start_editor(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait)
{
	size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
	DWORD dwExitCode = 0;
	std::wstring sTempStr = L"";

	/* - NOTE - You should check here to see if the exe even exists */

	/* Add a space to the beginning of the Parameters */
	if (Parameters.size() != 0)
	{
		if (Parameters[0] != L' ')
		{
			Parameters.insert(0, L" ");
		}
	}

	/* The first parameter needs to be the exe itself */
	sTempStr = FullPathToExe;
	iPos = sTempStr.find_last_of(L"\\");
	sTempStr.erase(0, iPos + 1);
	Parameters = sTempStr.append(Parameters);

	/* CreateProcessW can modify Parameters thus we allocate needed memory */
	wchar_t * pwszParam = new wchar_t[Parameters.size() + 1];
	if (pwszParam == 0)
	{
		return 1;
	}
	const wchar_t* pchrTemp = Parameters.c_str();
	wcsncpy(pwszParam, pchrTemp, Parameters.size() + 1);

	/* CreateProcess API initialization */
	STARTUPINFOW siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);

	if (!CreateProcessW(const_cast<LPCWSTR>(FullPathToExe.c_str()),
		pwszParam, 0, 0, false,
		CREATE_DEFAULT_ERROR_MODE, 0, 0,
		&siStartupInfo, &piProcessInfo) != false)
	{
		/* CreateProcess failed */
		iReturnVal = (size_t)GetLastError();
	}
	else
	{
		/* Watch the process. */
		dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, INFINITE);
	}

	/* Free memory */
	delete[]pwszParam;
	pwszParam = 0;

	/* Release handles */
	CloseHandle(piProcessInfo.hProcess);
	CloseHandle(piProcessInfo.hThread);

	return iReturnVal;
}

WinEditor::WinEditor(std::string editor_name, std::string settings_file) : Editor(editor_name, settings_file)
{
}

WinEditor::~WinEditor()
{
}

int WinEditor::launch(const std::string & editor_name)
{
	const std::wstring targetfile = to_wstring(settings_["exec"]).c_str();
	std::wstring args;
	for (const auto &item : arguments())
		args += to_wstring("\"") + to_wstring(item) + to_wstring("\"");

	std::wcout << "Starting process : " << targetfile << '\n';
	size_t res =
		start_editor(targetfile,
			args.c_str(), INFINITE);
	if (res != 0)
	{
		TCHAR szBuf[80];
		LPVOID lpMsgBuf;
		DWORD dw = DWORD(res);

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		wsprintf(szBuf,
			(LPCSTR)"%s failed with error %d: %s",
			targetfile, dw, lpMsgBuf);

		//MessageBox(NULL, szBuf, L"Error", MB_OK);
		std::cout << szBuf << std::endl;

		LocalFree(lpMsgBuf);
		ExitProcess(dw);

		return -3;
	}
	std::wcout << "Process " << targetfile << " exited with code " << res << std::endl;
	return 0;
}

int WinEditor::launch()
{
	return launch(editor_name_);
}
