#include <Windows.h>
#include <fstream>
#include <iostream>
#define BUFF_SIZE 16

int main() {
	std::ofstream fout("D:\\to.txt");
	std::ofstream log("D:\\wlog.txt");
	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	if (hFileMapping == INVALID_HANDLE_VALUE) {
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
		log << "Opening eSem failed";
		return 1;
	}
	HANDLE hCloseEvent = OpenEvent(NULL, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		log << "Opening eSem failed";
		return 1;
	}

	while (WaitForSingleObject(hCloseEvent, 0) != WAIT_OBJECT_0) {
		WaitForSingleObject(hFSem, INFINITE);
		PVOID pBuff = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUFF_SIZE);
		char temp[BUFF_SIZE];
		CopyMemory((TCHAR*)temp, pBuff, BUFF_SIZE * sizeof(TCHAR));
		fout.write((char*)temp, BUFF_SIZE);
		UnmapViewOfFile(pBuff);
		ReleaseSemaphore(hESem, 1, NULL);
	}
	CloseHandle(hFileMapping);
	CloseHandle(hESem);
	CloseHandle(hFSem);
	CloseHandle(hCloseEvent);
	fout.close();

	return 0;
}