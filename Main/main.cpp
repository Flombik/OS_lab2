#include <Windows.h>
#include <fstream>
#define BUFF_SIZE 16

int main(int argc, char* argv[]) {
	std::ifstream conf;
	if (argc > 1) {
		conf.open(argv[1]);
	}

	conf.close();

	HANDLE hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		BUFF_SIZE,
		TEXT("buff"));
	if (hFileMapping == INVALID_HANDLE_VALUE) {
		printf("CreateFileMapping error: %d\n", GetLastError());
		return 1;
	}
	HANDLE hESem = CreateSemaphore(
		NULL,
		1,
		1,
		TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		printf("CreateSemaphore1 error: %d\n", GetLastError());
		return 1;
	}
	HANDLE hFSem = CreateSemaphore(
		NULL,
		0,
		1,
		TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		printf("CreateSemaphore2 error: %d\n", GetLastError());
		return 1;
	}
	HANDLE hCloseEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		printf("CreateEvent error: %d\n", GetLastError());
		return 1;
	}

	STARTUPINFO si1 = { 0 };
	STARTUPINFO si2 = { 0 };
	PROCESS_INFORMATION pi1 = { 0 };
	PROCESS_INFORMATION pi2 = { 0 };

	ZeroMemory(&si1, sizeof(si1));
	ZeroMemory(&si2, sizeof(si2));
	si1.cb = sizeof(si1);
	si2.cb = sizeof(si2);
	ZeroMemory(&pi1, sizeof(pi1));
	ZeroMemory(&pi2, sizeof(pi2));

	TCHAR fileToReadName[] = TEXT("D:\\from.txt");
	TCHAR fileToWriteName[] = TEXT("D:\\to.txt");

	// Start the child read process. 
	if (!CreateProcess(TEXT("D:\\Visual Studio Prog\\OS2\\Main\\Debug\\Read.exe"),
		fileToReadName,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inherita
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si1,            // Pointer to STARTUPINFO structure
		&pi1)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess for reading failed (%d).\n", GetLastError());
		return 1;
	}

	if (!CreateProcess(TEXT("D:\\Visual Studio Prog\\OS2\\Main\\Debug\\Write.exe"),
		fileToWriteName,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inherita
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si2,            // Pointer to STARTUPINFO structure
		&pi2)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess for writing failed (%d).\n", GetLastError());
		return 1;
	}

	// Wait until child process exits.
 	WaitForSingleObject(pi1.hProcess, INFINITE);
	WaitForSingleObject(pi2.hProcess, INFINITE);

	// Close handles. 
	CloseHandle(pi1.hProcess);
	CloseHandle(pi1.hThread);
	CloseHandle(pi2.hProcess);
	CloseHandle(pi2.hThread);
	CloseHandle(hFileMapping);
	CloseHandle(hESem);
	CloseHandle(hFSem);
	CloseHandle(hCloseEvent);

	return 0;
}