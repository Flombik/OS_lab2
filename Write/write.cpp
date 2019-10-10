#include <Windows.h>
#include <fstream>
#define BUFF_SIZE 32

void logToFile(std::string fileName, std::string info) {
	std::ofstream log(fileName, std::ios_base::app);
	SYSTEMTIME time;
	GetSystemTime(&time);
	log << '[' << time.wDay << '.' << time.wMonth << '.' << time.wYear << ' '
		<< time.wHour << ':' << time.wMinute << ':' << time.wSecond << "." << time.wMilliseconds << ']';
	log << ' ' << info;
	log << '\n';
	log.close();
}

int main(int argc, char* argv[]) {
	std::ofstream fout;
	std::string fileToLogName;
	if (argc != 0) {
		fout.open(argv[0]);
		fileToLogName = std::string(argv[1]);
	}

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	//if (hFileMapping == INVALID_HANDLE_VALUE) {
	//	//log << "Opening FileMapping failed";
	//	return 1;
	//}
	HANDLE hESem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "Opening eSem failed");
		return 1;
	}
	else {
		logToFile(fileToLogName, "eSem opened");
	}
	HANDLE hFSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "Opening fSem failed");
		return 1;
	}
	else {
		logToFile(fileToLogName, "fSem opened");
	}
	HANDLE hCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "Opening closeEvent failed");
		return 1;
	}
	else {
		logToFile(fileToLogName, "closeEvent opened");
	}

	logToFile(fileToLogName, "Starting writing...");
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
	logToFile(fileToLogName, "Program closed successfully");

	return 0;
}