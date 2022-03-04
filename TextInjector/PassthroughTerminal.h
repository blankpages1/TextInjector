#pragma once
#include <Windows.h>

class PassthroughTerminal{
public:
	PassthroughTerminal() {
		stdinH = GetStdHandle(STD_INPUT_HANDLE);
	}
	~PassthroughTerminal() {
	}

	void getData(char* buffer, DWORD* readSize, unsigned int maxSize) {
		ReadFile(stdinH, buffer, maxSize, readSize, NULL);
	}

private:
	HANDLE stdinH = 0;

};

