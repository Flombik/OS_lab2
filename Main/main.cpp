#include <Windows.h>
#include <fstream>
#include <string>
#define BUFF_SIZE 32
#define PROC_NAME "Main process"

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
	std::ifstream conf;
	if (argc > 1) {
		conf.open(argv[1]);
	}
	else {
		return 1;
	}

	std::string fileToReadName;
	std::string fileToWriteName;
	std::string fileToLogName;
	conf >> fileToReadName;
	conf >> fileToWriteName;
	conf >> fileToLogName;

	conf.close();

	std::ofstream temp;
	temp.open(fileToLogName);
	temp.close();

	logToFile(fileToLogName, "Starting program");

	std::string attributesForReadProc;
	std::string attributesForWriteProc;
	attributesForReadProc = fileToReadName + ' ' + fileToLogName;
	attributesForWriteProc = fileToWriteName + ' ' + fileToLogName;

	HANDLE hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		BUFF_SIZE,
		TEXT("buff"));
	if (hFileMapping == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "CreateFileMapping error");
		return 1;
	}
	else {
		logToFile(fileToLogName, "FileMapping created");
	}
	HANDLE hESem = CreateSemaphore(
		NULL,
		1,
		1,
		TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "CreateSemaphore1 error");
		return 1;
	}
	else {
		logToFile(fileToLogName, "Semaphore1 created");
	}
	HANDLE hFSem = CreateSemaphore(
		NULL,
		0,
		1,
		TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "CreateSemaphore2 error");
		return 1;
	}
	else {
		logToFile(fileToLogName, "Semaphore2 created");
	}
	HANDLE hCloseEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "CreateEvent error");
		return 1;
	}
	else {
		logToFile(fileToLogName, "Event created");
	}
	HANDLE hLogSemaphore = CreateSemaphore(
		NULL,
		1,
		1,
		TEXT("logSem")
	);
	if (hLogSemaphore == INVALID_HANDLE_VALUE) {
		logToFile(fileToLogName, "LogSemaphore creating error");
		return 1;
	}
	else {
		logToFile(fileToLogName, "LogSemaphore created");
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

	// Start the child read process. 
	if (!CreateProcess(TEXT("D:\\Visual Studio Prog\\OS2\\Main\\Debug\\Read.exe"),
		(TCHAR*)attributesForReadProc.c_str(),        // Command line
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
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "CreateProcess for reading failed");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
		return 1;
	}
	else {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "Process for reading created successfully");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
	}
	if (!CreateProcess(TEXT("D:\\Visual Studio Prog\\OS2\\Main\\Debug\\Write.exe"),
		(TCHAR*)attributesForWriteProc.c_str(),        // Command line
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
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "CreateProcess for writing failed");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
		return 1;
	}
	else {
		WaitForSingleObject(hLogSemaphore, INFINITE);
		logToFile(fileToLogName, "Process for writing created successfully");
		ReleaseSemaphore(hLogSemaphore, 1, NULL);
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

	WaitForSingleObject(hLogSemaphore, INFINITE);
	logToFile(fileToLogName, "Program closed successfully");
	ReleaseSemaphore(hLogSemaphore, 1, NULL);
	CloseHandle(hLogSemaphore);

	return 0;
}