#include <Windows.h>
#include <fstream>
#include <iostream>
#define BUFF_SIZE 16

int main() {
	std::ifstream fin("D:\\from.txt");
	std::ofstream log("D:\\rlog.txt");

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	if (hFileMapping == INVALID_HANDLE_VALUE)	{
		log << "Opening FileMapping failed";
		return 1;
	}
	HANDLE hESem = OpenSemaphore(NULL, FALSE, TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		log << "Opening eSem failed";
		return 1;
	}
	HANDLE hFSem = OpenSemaphore(NULL, FALSE, TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		log << "Opening fSem failed";
		return 1;
	}
	HANDLE hCloseEvent = OpenEvent(NULL, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		log << "Opening closeEvent failed";
		return 1;
	}

	while (!fin.eof()) {
		WaitForSingleObject(hESem, INFINITE);
		PVOID pBuff = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUFF_SIZE);
		char temp[BUFF_SIZE];
		fin.read(temp, BUFF_SIZE);
		CopyMemory(pBuff, (TCHAR*)temp, BUFF_SIZE * sizeof(TCHAR));
		UnmapViewOfFile(pBuff);
		ReleaseSemaphore(hFSem, 1, NULL);
	}
	SetEvent(hCloseEvent);
	CloseHandle(hFileMapping);
	CloseHandle(hESem);
	CloseHandle(hFSem);
	CloseHandle(hCloseEvent);
	fin.close();
	log.close();

	return 0;
}