#include <Windows.h>
#include <string>
#include <fstream>
#ifndef LOGGER_H
#define LOGGER_H
class Logger
{
public:
	Logger(std::string fileName, std::string procName);
	void log(std::string info);

private:
	std::string fileName;
	std::string procName;
	HANDLE hMut;
};

#endif // !LOGGER_H

