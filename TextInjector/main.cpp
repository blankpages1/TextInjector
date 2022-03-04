#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "SubProgram.h"
#include "CommandQueue.h"
#include "PassthroughTerminal.h"

//format
//--------------------
// executable command line
// delay(millis):string
// delay(millis):string
// delay(millis):string
// ...
//--------------------

struct TIOptions {
	std::string executable = "";
	std::vector<CommandData> inputData;
};

struct ParserState {
	uint16_t state = 0, cycleNum = 0;
} optionsState;
enum {
	STATE_READ_EXECUTABLE = 0,
	STATE_READ_DELAY,
	STATE_READ_DATA,
	STATE_END
};
const char LINE_RETURN = '\n';
const char LINE_SEPERATOR = ':';
const char LINE_FILE_END = 0x0;

//basic validation
bool validateOptions(TIOptions& options) {
	if (options.executable.size() == 0) {
		std::cout << "Options file must contain executable\n";
		return false;
	}
	if (options.inputData.size() == 0) {
		std::cout << "Options file must contain at least one command\n";
		return false;
	}
	for (auto n : options.inputData) {
		if (n.delay.size() == 0) {
			std::cout << "Every command must be given a delay\n";
			return false;
		}
	}
	return true;
}

void run(TIOptions& options) {
	SubProgram program(options.executable);
	CommandQueue commands(options.inputData);

	int errorCode = 0;
	if (errorCode = program.startProcess()) {
		std::cout << "Could not start process: " << options.executable
			<< ", "  << errorCode << "\n";
	}
	else {
		//write command from file
		std::string nextCommand = "";
		while (commands.getNextCommand(nextCommand)) {
			nextCommand.append("\n");
			program.writeStdin((char*)nextCommand.c_str(), nextCommand.size());
		}

		//revert to passthrough terminal
		PassthroughTerminal passthrough;
		char buffer[512];
		DWORD readSize = 0;
		while (program.isRunning()) {
			passthrough.getData(buffer, &readSize, 512);
			program.writeStdin(buffer, readSize);
		}

		program.killProcess();
	}

}

int main(int argc, char** args)
{

	if (argc == 2){ //input argument and configuration file

		//read options file
		TIOptions options = {};
		std::ifstream optionsFile(args[1]);
		char readBuffer[512] = {0};

		do {
			optionsFile.read(readBuffer, 512);
			for (int x = 0; x < 512; x++) {
				char inputChar = readBuffer[x];
				optionsState.cycleNum++;
				if (optionsState.state == STATE_READ_EXECUTABLE) {
					if (inputChar == LINE_FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_RETURN) optionsState = { STATE_READ_DELAY, 0 };
					else {
						options.executable.push_back(inputChar);
					}
				}
				else if (optionsState.state == STATE_READ_DELAY) {
					if (inputChar == LINE_FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_SEPERATOR) optionsState = { STATE_READ_DATA, 0 };
					else {
						if (optionsState.cycleNum == 1) {
							options.inputData.push_back({});
						}
						options.inputData.at(options.inputData.size() - 1).delay.push_back(inputChar);
					}
				}
				else if (optionsState.state == STATE_READ_DATA) {
					if (inputChar == LINE_FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_RETURN) optionsState = { STATE_READ_DELAY, 0 };
					else {
						options.inputData.at(options.inputData.size() - 1).data.push_back(inputChar);
					}
				}
				if (optionsState.state == STATE_END)
					break;

			}

		} while (!optionsFile.eof());
		//done reading options file

		//validate options and run program
		if (validateOptions(options)) run(options);

	}
	else{ //incorrect number of arguments included

		std::cout << "Incorrect number of arguments\n";
		std::cout << "Usage: command optionsfile\n";
		return 1;
	}

	return 0;
}