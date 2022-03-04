#pragma once
#include <string>
#include <iostream>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct CommandData {
	std::string data = "";
	std::string delay = "";
};

class CommandQueue{
public:
	CommandQueue(std::vector<CommandData> data) :commands(data) {
	}
	~CommandQueue() {
	}

	//gets next command
	//returns false when the queue is empty
	bool getNextCommand(std::string& command) {
		
		//check for end of list
		if (index == commands.size())
			return false;

		CommandData current_command = commands[index++];

		//generate delay integer
		int delay_time = 0;
		try{
			delay_time = std::stoi(current_command.delay);
		}
		catch (std::exception e) {
			std::cout << "Could not convert delay string to integer: " 
				<< current_command.delay << "\n";
		}

		if (delay_time < 0) {
			std::cout << "Delay must be a positive value: "
				<< current_command.delay << "\n";
			delay_time = 0;
		}

		Sleep(delay_time);

		//return command
		command = current_command.data;
		
		return true;
	}

private:
	int index = 0;
	std::vector<CommandData> commands;

};

