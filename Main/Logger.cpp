#include "Logger.h"

Logger::Logger(std::string fileName, std::string procName, HANDLE hMut) {
	this->fileName = fileName;
	this->procName = procName;
	this->hMut = hMut;
}

void Logger::log(std::string info) {
	WaitForSingleObject(hMut, INFINITE);
	std::ofstream log(this->fileName, std::ios_base::app);
	SYSTEMTIME time;
	GetSystemTime(&time);
	log << '[' << time.wDay << '.' << time.wMonth << '.' << time.wYear << ' '
		<< time.wHour << ':' << time.wMinute << ':' << time.wSecond << "." << time.wMilliseconds << ']';
	log << ' ' << "<<From " << this->procName << ">> " << info;
	log << '\n';
	log.close();
	ReleaseMutex(this->hMut);
}