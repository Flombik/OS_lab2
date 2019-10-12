#include "Logger.h"

Logger::Logger(std::string fileName, std::string procName) {
	this->fileName = fileName;
	this->procName = procName;
	this->hMut = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("logMut"));
	if (GetLastError() == ERROR_FILE_NOT_FOUND) {
		this->hMut = CreateMutex(NULL, FALSE, TEXT("logMut"));
	}
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