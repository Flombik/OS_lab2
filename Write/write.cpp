#include <Windows.h>
#include <fstream>
#define BUFF_SIZE 16

int main(int argc, char* argv[]) {
	std::ofstream fout;
	if (argc != 0) {
		fout.open(argv[0]);
	}
	//std::ofstream log("D:\\wlog.txt");
	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("buff"));
	//if (hFileMapping == INVALID_HANDLE_VALUE) {
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
		//log << "Opening eSem failed";
		return 1;
	}
	HANDLE hCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		//log << "Opening eSem failed";
		return 1;
	}
	//HANDLE hFile = CreateFile("D:\\to.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

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

	return 0;
}