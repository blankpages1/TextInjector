#pragma once
#include <string>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class SubProgram{
public:
	SubProgram(std::string executable):executable(executable), arguments() {

	}

	~SubProgram() {
		TerminateProcess(child_process.hProcess, EXIT_FAILURE);
		//CloseHandle(child_process.hProcess);
		//CloseHandle(child_process.hThread);
		//CloseHandle(process_stdin_rd);
		//CloseHandle(process_stdin_wr);
	}

	//error codes
	//1 - failed to create pipe
	int startProcess() {
		
		//define pipe security attributes structure
		SECURITY_ATTRIBUTES pipeSec;
		pipeSec.nLength = sizeof(SECURITY_ATTRIBUTES);
		pipeSec.lpSecurityDescriptor = nullptr;
		pipeSec.bInheritHandle = TRUE;

		//create pipes
		if (!CreatePipe(&process_stdin_rd, &process_stdin_wr, &pipeSec, 0)) {
			std::cout << "Failed to create pipes\n";
			return 1;
		}

		//create std output handle
		HANDLE childOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (!SetHandleInformation(childOut, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
			std::cout << "Failed to make stdout inheritable\n";
			return 2;
		}

		//create child process
		STARTUPINFOA chStartup = {0};
		ZeroMemory(&chStartup, sizeof(STARTUPINFOA));
		chStartup.cb = sizeof(STARTUPINFOA);
		chStartup.hStdInput = process_stdin_rd;
		chStartup.hStdOutput = childOut;
		chStartup.hStdError = childOut;
		chStartup.dwFlags |= STARTF_USESTDHANDLES;

		DWORD success = CreateProcessA(
			NULL,
			(char*) executable.c_str(),
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&chStartup,
			&child_process
			);
		if (!success) {
			std::cout << "Failed to start process\n";
			return 3;
		}
	
		return 0;
	}

	void writeStdin(char* data, DWORD size) {
		DWORD bytesWritten = 0;
		bool success = WriteFile(process_stdin_wr, data, size, &bytesWritten, NULL);
		if (!success) {
			std::cout << "Failed to write data to subprogram stdin\n";
		}
	}	

	void killProcess() {
		TerminateProcess(child_process.hProcess, EXIT_SUCCESS);
	}

	bool isRunning() {
		DWORD exitCode, success;
		success = GetExitCodeProcess(child_process.hProcess, &exitCode);
		return (exitCode == STILL_ACTIVE) && success;
	}

private:
	std::string executable, arguments;

	HANDLE process_stdin_rd = nullptr, process_stdin_wr = nullptr;
	PROCESS_INFORMATION child_process = {0};
	
};

