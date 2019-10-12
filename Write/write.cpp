#include <Windows.h>
#include <fstream>
#include "Logger.h"
#define BUFF_SIZE 32

int main(int argc, char* argv[]) {
	std::ofstream fout;
	std::string fileToLogName;
	if (argc > 1) {
		fout.open(argv[1]);
		fileToLogName = std::string(argv[2]);
	}

	Logger logger(fileToLogName, "Write process");

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	//if (hFileMapping == INVALID_HANDLE_VALUE) {
	//	//log << "Opening FileMapping failed";
	//	return 1;
	//}
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
	logger.log("Starting writing...");
	while (WaitForSingleObject(hCloseEvent, 0) != WAIT_OBJECT_0) {
		WaitForSingleObject(hFSem, INFINITE);
		PVOID pBuff = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUFF_SIZE);
		TCHAR temp[BUFF_SIZE];
		CopyMemory(temp, pBuff, BUFF_SIZE);
		fout.write(temp, BUFF_SIZE);
		UnmapViewOfFile(pBuff);
		ReleaseSemaphore(hESem, 1, NULL);
	}
	CloseHandle(hFileMapping);
	CloseHandle(hESem);
	CloseHandle(hFSem);
	CloseHandle(hCloseEvent);
	fout.close();

	logger.log("Program closed successfully");

	return 0;
}