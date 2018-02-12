#include "DominusEngine.h"
#include <stdexcept>
#include <iostream>
#include <fstream>

void logToFile(std::string aLog) 
{
	std::ofstream logFile("log.txt", std::ios::out | std::ios::trunc);

	if (!logFile.is_open()) 
	{
		std::cout << "Couldnt not create or open log file" << std::endl;
		return;
	}
	
	logFile << aLog;
	logFile.close();
}

int main() {
	DominusEngine de;

	try {
		de.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		std::string errorString("Error: ");
		errorString.append(e.what());
		logToFile(errorString);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}