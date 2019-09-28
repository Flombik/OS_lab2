#include <Windows.h>
#include <fstream>
#define BUFF_SIZE 16

int main(int argc, char* argv[]) {
	std::ifstream fin;
	if (argc != 0) {
		fin.open(argv[0]);
	}
	//std::ofstream log("D:\\rlog.txt");

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	//if (hFileMapping == INVALID_HANDLE_VALUE)	{
	//	//log << "Opening FileMapping failed";
	//	return 1;
	//}
	HANDLE hESem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		//log << "Opening eSem failed";
		return 1;
	}
	HANDLE hFSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		//log << "Opening fSem failed";
		return 1;
	}
	HANDLE hCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		//log << "Opening closeEvent failed";
		return 1;
	}
	if (argc != 0) {
		HANDLE hFile = CreateFile(argv[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return 1;
		}
	}

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
	//CloseHandle(hFile);
	fin.close();
	//log.close();

	return 0;
}