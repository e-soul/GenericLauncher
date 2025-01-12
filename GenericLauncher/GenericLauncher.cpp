/*
   Copyright 2017-2023 e-soul.org
   All rights reserved.
   Redistribution and use in source and binary forms, with or without modification, are permitted
   provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright notice, this list of conditions
	  and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
	  and the following disclaimer in the documentation and/or other materials provided with the distribution.
   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GenericLauncher.h"

int main()
{
	char ModuleFilename[1024];
	DWORD GetFilenameResult = GetModuleFileNameA(NULL, ModuleFilename, 1024);
	if (0 == GetFilenameResult || 1024 == GetFilenameResult)
	{
		std::cerr << "GetModuleFileNameA failed " << GetLastError() << std::endl;
		return 1;
	}

	std::filesystem::path Executable = std::filesystem::path(ModuleFilename);
	std::filesystem::path LauncherScript = Executable.replace_extension(".bat");

	std::unique_ptr<char[]> CommandLine(_strdup(std::format("cmd.exe /c {}", LauncherScript.string()).c_str()));
	//std::unique_ptr<char[]> CommandLine(_strdup(LauncherScript.string().c_str()));

	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	if (!CreateProcess(NULL, CommandLine.get(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
	{
		std::cerr << "CreateProcess for " << CommandLine << " failed " << GetLastError() << std::endl;
		return 2;
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

	DWORD exitCode = 0;
	if (!GetExitCodeProcess(ProcessInfo.hProcess, &exitCode))
	{
		std::cerr << "GetExitCodeProcess failed " << GetLastError() << std::endl;
	}
	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);

	return exitCode;
}
