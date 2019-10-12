#include <Windows.h>
#include <fstream>
#include "Logger.h"
#define BUFF_SIZE 32

int main(int argc, char* argv[]) {
	std::ifstream conf;
	if (argc > 1) {
		conf.open(argv[1]);
	}
	else {
		return 1;
	}

	if (!conf) {
		printf("Configuration file is not opened! Closing program...");
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
	if (!conf) {
		printf("Log file is not opened! Closing program...");
		return 1;
	}
	temp.close();

	std::string attributesForReadProc;
	std::string attributesForWriteProc;
	attributesForReadProc = "Read " + fileToReadName + ' ' + fileToLogName;
	attributesForWriteProc = "Write " + fileToWriteName + ' ' + fileToLogName;

	Logger logger(fileToLogName, "Main process");
	logger.log("All files are available. Starting program");

	HANDLE hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		BUFF_SIZE,
		TEXT("buff"));
	if (hFileMapping == INVALID_HANDLE_VALUE) {
		logger.log("CreateFileMapping error");
		return 1;
	}
	else {
		logger.log("FileMapping created");
	}
	HANDLE hESem = CreateSemaphore(
		NULL,
		1,
		1,
		TEXT("eSem"));
	if (hESem == INVALID_HANDLE_VALUE) {
		logger.log("CreateSemaphore1 error");
		return 1;
	}
	else {
		logger.log("Semaphore1 created");
	}
	HANDLE hFSem = CreateSemaphore(
		NULL,
		0,
		1,
		TEXT("fSem"));
	if (hFSem == INVALID_HANDLE_VALUE) {
		logger.log("CreateSemaphore2 error");
		return 1;
	}
	else {
		logger.log("Semaphore2 created");
	}
	HANDLE hCloseEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		TEXT("closeEvent"));
	if (hCloseEvent == INVALID_HANDLE_VALUE) {
		logger.log("CreateEvent error");
		return 1;
	}
	else {
		logger.log("Event created");
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
	if (!CreateProcess(NULL,
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
		logger.log("CreateProcess for reading failed");
		return 1;
	}
	else {
		logger.log("Process for reading created successfully");
	}
	if (!CreateProcess(NULL,
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
		logger.log("CreateProcess for writing failed");
		return 1;
	}
	else {
		logger.log("Process for writing created successfully");
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

	logger.log("Program closed successfully");

	return 0;
}