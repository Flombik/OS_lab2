#include <Windows.h>
#include <fstream>
#include "Logger.h"
#define BUFF_SIZE 32

int main(int argc, char* argv[]) {
	std::ifstream fin;
	std::string fileToLogName;
	if (argc > 1) {
		fin.open(argv[1]);
		fileToLogName = std::string(argv[2]);
	}

	HANDLE hLogMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("logSem"));
	if (hLogMutex == INVALID_HANDLE_VALUE) {
		return 1;
	}

	Logger logger(fileToLogName, "Read process", hLogMutex);

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	/*if (GetLastError() == ERROR_ACCESS_DENIED)	{
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "Opening FileMapping failed");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
		return 1;
	}
	else {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "FileMapping opened");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
	}*/
	HANDLE hESem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		logger.log("Opening eSem failed");
		return 1;
	}
	else {
		logger.log("eSem opened");
	}
	HANDLE hFSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		logger.log("Opening fSem failed");
		return 1;
	}
	else {
		logger.log("fSem opened");
	}
	HANDLE hCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		logger.log("Opening closeEvent failed");
		return 1;
	}
	else {
		logger.log("closeEvent opened");
	}
	logger.log("Starting reading...");
	while (!fin.eof()) {
		WaitForSingleObject(hESem, INFINITE);
		PVOID pBuff = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUFF_SIZE);
		TCHAR temp[BUFF_SIZE];
		fin.read(temp, BUFF_SIZE);
		CopyMemory(pBuff, temp, BUFF_SIZE);
		UnmapViewOfFile(pBuff);
		ReleaseSemaphore(hFSem, 1, NULL);
	}
	SetEvent(hCloseEvent);
	CloseHandle(hFileMapping);
	CloseHandle(hESem);
	CloseHandle(hFSem);
	CloseHandle(hCloseEvent);
	fin.close();

	logger.log("Program closed successfully");
	CloseHandle(hLogMutex);

	return 0;
}