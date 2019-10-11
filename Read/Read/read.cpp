#include <Windows.h>
#include <fstream>
#define BUFF_SIZE 32
#define PROC_NAME "Read process"

void logToFile(std::string fileName, std::string info) {
	std::ofstream log(fileName, std::ios_base::app);
	SYSTEMTIME time;
	GetSystemTime(&time);
	log << '[' << time.wDay << '.' << time.wMonth << '.' << time.wYear << ' '
		<< time.wHour << ':' << time.wMinute << ':' << time.wSecond << "." << time.wMilliseconds << ']';
	log << ' ' << "<<From " << PROC_NAME << ">> " << info;
	log << '\n';
	log.close();
}

int main(int argc, char* argv[]) {
	std::ifstream fin;
	std::string fileToLogName;
	if (argc != 0) {
		fin.open(argv[0]);
		fileToLogName = std::string(argv[1]);
	}

	HANDLE hLogSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("logSem"));
	if (hLogSemaphore == INVALID_HANDLE_VALUE) {
		return 1;
	}

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	//if (hFileMapping == INVALID_HANDLE_VALUE)	{
	//	//log << "Opening FileMapping failed";
	//	return 1;
	//}
	HANDLE hESem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "Opening eSem failed");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
		return 1;
	}
	else {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "eSem opened");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
	}
	HANDLE hFSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "Opening fSem failed");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
		return 1;
	}
	else {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "fSem opened");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
	}
	HANDLE hCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "Opening closeEvent failed");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
		return 1;
	}
	else {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "closeEvent opened");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
	}
	WaitForSingleObject(hLogSemaphore, INFINITE);
	logToFile(fileToLogName, "Starting reading...");
	ReleaseSemaphore(hLogSemaphore, 1, NULL);
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

	WaitForSingleObject(hLogSemaphore, INFINITE);
	logToFile(fileToLogName, "Program closed successfully");
	ReleaseSemaphore(hLogSemaphore, 1, NULL);
	CloseHandle(hLogSemaphore);

	return 0;
}