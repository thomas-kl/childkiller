// License: GPL Version 3.0. See License.txt
// Author: Thomas Klambauer
// Email: Thomas AT Klambauer.info

#define WIN32_LEAN_AND_MEAN
// As per docs of CreateJobObject function (available starting with Windows XP)
#define _WIN32_WINNT 0x0500 

#include <stdio.h>

#include "WinSDKVer.h"

#include "Windows.h"
#include <string>
#include <iostream>

using namespace std;

void printLastError() {
	wcerr << L"Error: GetLastError= " << GetLastError() << endl;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		wcout << "chik Version 1.0" << endl;
		wcout << " Author: Thomas AT Klambauer.info" << endl;
		wcout << " License: GPL Version 3" << endl;
		wcout << endl;
		wcout << L"chik [Command] [Arg1] [Arg2] ..." << endl;
		wcout << L"  Windows tool, that executes the given command and arguments via the system" << endl;
		wcout << L"  function, while ensuring that killing the original chik process will" << endl;
		wcout << L"  also kill all child processes spawned by command." << endl;
		return 0;
	}

	HANDLE jobHandle = CreateJobObject(NULL, NULL);
	if (jobHandle == NULL) {
		wcerr << L"CreateJobObject failed" << endl;
		printLastError();
		return 1;
	}

	HANDLE thisProcess = GetCurrentProcess();

	if (thisProcess == NULL) {
		wcerr << L"GetCurrentProcess failed" << endl;
		printLastError();
		return 1;
	}

	JOBOBJECT_BASIC_LIMIT_INFORMATION info = { 0 };
	info.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedInfo;
	extendedInfo.BasicLimitInformation = info;

	int length = sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION);

	if (!SetInformationJobObject(jobHandle, JobObjectExtendedLimitInformation, &extendedInfo, length)) {
		wcerr << L"SetInformationJobObject failed" << endl;
		printLastError();
		return 1;
	}

	if (AssignProcessToJobObject(jobHandle, thisProcess) == 0) {
		wcerr << L"AssignProcessToJobObject failed" << endl;
		printLastError();
		return 1;
	}

	if (argc > 1) {
		string command;

		// Ignore argv[0] (contains this executable's path).
		for (int i = 1; i < argc; i++) {
			string arg(argv[i]);

			// If parameter contains a space, quote the params
			//  to forward them exactly as we received them.
			if (arg.find(" ") != string::npos) {
				arg = "\"" + arg + "\"";
			}
			command.append(arg);
			// Add a space to separate the final args, but no trailing space.
			if (i != argc - 1) {
				command.append(" ");
			}
		}

		// This will cause windows to spawn a "cmd.exe /c" process
		return system(command.c_str());
	}

	return 0;
}

