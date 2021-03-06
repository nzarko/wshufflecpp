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

size_t WinEditor::start_editor(std::string FullPathToExe, std::string Parameters, size_t SecondsToWait)
{
	size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
	DWORD dwExitCode = 0;
	std::string sTempStr = "";

	/* - NOTE - You should check here to see if the exe even exists */

	/* Add a space to the beginning of the Parameters */
	if (Parameters.size() != 0)
	{
		if (Parameters[0] != ' ')
		{
			Parameters.insert(0, " ");
		}
	}

	/* The first parameter needs to be the exe itself */
	sTempStr = FullPathToExe;
	iPos = sTempStr.find_last_of("\\");
	sTempStr.erase(0, iPos + 1);
	Parameters = sTempStr.append(Parameters);

	/* CreateProcessW can modify Parameters thus we allocate needed memory */
	char * pwszParam = new char[Parameters.size() + 1];
	if (pwszParam == 0)
	{
		return 1;
	}
	const char* pchrTemp = Parameters.c_str();
	strncpy(pwszParam, pchrTemp, Parameters.size() + 1);

	/* CreateProcess API initialization */
	STARTUPINFOA siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);

	if (!CreateProcessA(const_cast<LPCSTR>(FullPathToExe.c_str()),
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
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	const std::string targetfile = settings_["exec"].c_str();
	std::string args ;
	const std::string quote = R"(")";
	for (const auto &item : arguments()) {
		args += quote + item + quote + " ";
	}

	//std::wstring temp = convert(args);
	
	size_t res =
		start_editor(targetfile, args, INFINITE);
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
	std::cout << "Process " << targetfile << " exited with code " << res << std::endl;
	return 0;
}

int WinEditor::launch()
{
	return launch(editor_name_);
}

std::wstring WinEditor::convert(const std::string & input)
{
		try
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return converter.from_bytes(input);
		}
		catch (std::range_error& e)
		{
			size_t length = input.length();
			std::wstring result;
			result.reserve(length);
			for (size_t i = 0; i < length; i++)
			{
				result.push_back(input[i] & 0xFF);
			}
			return result;
		}
	
}

std::shared_ptr<WinEditor> create_editor() {
    return std::make_shared<WinEditor>(std::string("Editor"), std::string("editor.json"));;
}
