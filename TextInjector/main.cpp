#include <iostream>
#include <fstream>
#include <string>
#include <vector>


//format
//--------------------
// executable
// argument command line
// delay(millis):string
// delay(millis):string
// delay(millis):string
// ...
//--------------------
struct InputData {
	std::string data = "";
	std::string delay = 0;
};
struct TIOptions {
	std::string executable = "";
	std::string arguments = "";

	std::vector<InputData> inputData;
} options;

struct ParserState {
	uint16_t state = 0, cycleNum = 0;
} optionsState;
enum {
	STATE_READ_EXECUTABLE = 0,
	STATE_READ_ARGUMENTS,
	STATE_READ_DELAY,
	STATE_READ_DATA,
	STATE_END
};
const char LINE_RETURN = '\n';
const char LINE_SEPERATOR = ':';
const char FILE_END = 0x0;

int main(int argc, char** args)
{

	if (argc == 2){ //input argument and configuration file

		std::ifstream optionsFile(args[1]);
		char readBuffer[512] = {};

		do {
			optionsFile.read(readBuffer, 512);
			for (int x = 0; x < 512; x++) {
				char inputChar = readBuffer[x];

				optionsState.cycleNum++;
				if (optionsState.state == STATE_READ_EXECUTABLE) {

					if (inputChar == FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_RETURN) optionsState = { STATE_READ_ARGUMENTS, 0 };
					else {
						options.executable.push_back(inputChar);
					}
				}
				else if (optionsState.state == STATE_READ_ARGUMENTS) {

					if (inputChar == FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_RETURN) optionsState = { STATE_READ_DELAY, 0 };
					else {
						options.arguments.push_back(inputChar);
					}
				}
				else if (optionsState.state == STATE_READ_DELAY) {

					if (inputChar == FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_SEPERATOR) optionsState = { STATE_READ_DATA, 0 };
					else {
						
						if (optionsState.cycleNum == 1) {
							options.inputData.push_back(InputData());
						}
						options.inputData.at(options.inputData.size() - 1).delay.push_back(inputChar);
					}
				}
				else if (optionsState.state == STATE_READ_DATA) {

					if (inputChar == FILE_END) optionsState = { STATE_END, 0 };
					else if (inputChar == LINE_RETURN) optionsState = { STATE_READ_DELAY, 0 };
					else {
						
						options.inputData.at(options.inputData.size() - 1).data.push_back(inputChar);
					}
				}
				if (optionsState.state == STATE_END)
					break;

			}

		} while (!optionsFile.eof());

	}
	else{ //incorrect number of arguments included

		std::cout << "Incorrect number of arguments\n";
		std::cout << "Usage: command optionsfile\n";
		return 1;
	}
}