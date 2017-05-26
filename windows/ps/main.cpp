#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>


///< 枚举窗口参数
typedef struct
{
	HWND    hwndWindow;     // 窗口句柄
	DWORD   dwProcessID;    // 进程ID
}EnumWindowsArg;

///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	EnumWindowsArg *pArg = (EnumWindowsArg *)lParam;
	DWORD  dwProcessID = 0;
	// 通过窗口句柄取得进程ID
	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (dwProcessID == pArg->dwProcessID)
	{
		pArg->hwndWindow = hwnd;
		// 找到了返回FALSE
		return FALSE;
	}
	// 没找到，继续找，返回TRUE
	return TRUE;
}
///< 通过进程ID获取窗口句柄
HWND GetWindowHwndByPID(DWORD dwProcessID)
{
	HWND hwndRet = NULL;
	EnumWindowsArg ewa;
	ewa.dwProcessID = dwProcessID;
	ewa.hwndWindow = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
	if (ewa.hwndWindow)
	{
		hwndRet = ewa.hwndWindow;
	}
	return hwndRet;
}


//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

int main(int argc, char const *argv[])
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	// 获取进程快照
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot (of processes) 失败");
		return 0 ;
	}
	// 设置输入参数，结构的大小
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// 开始列举进程信息
	if (!Process32First(hSnapshot, &pe32))
	{
		printf("Process32First() 失败");
		CloseHandle(hSnapshot); // 关闭句柄
		return 0;
	}

	DWORD zt2pid = 0;
	printf("进程ID\t父进程\t线程数\t优先级\t进程名\n"); // 基本优先级
	do {
		// 打印进程相关信息
		printf("\t%u", pe32.th32ProcessID);    // 进程id
		printf("\t%u", pe32.th32ParentProcessID);  // 父进程id
		printf("\t%d", pe32.cntThreads);       // 线程数
		printf("\t%d", pe32.pcPriClassBase);   // 基本优先级
		printf("\t%s\n", pe32.szExeFile);        // 进程名

		std::string name = pe32.szExeFile;

		std::cout << name << std::endl;

		if (name == "zhengtu2.dat")
		{
			//获取进程的句柄  
			HANDLE zt2_process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			if (zt2_process != INVALID_HANDLE_VALUE)
			{
				//获取第一个可视窗口的句柄  
				HWND hWnd = GetWindowHwndByPID(pe32.th32ProcessID);
				if (hWnd != INVALID_HANDLE_VALUE)
				{
					SwitchToThisWindow(hWnd, TRUE);
					//ShowWindow(hWnd, SW_SHOW);
					//if (!BringWindowToTop(hWnd))
					if (!SetForegroundWindow(hWnd))
					{
						MessageBox(NULL, GetLastErrorAsString().c_str(), "", MB_OK);
					}


					zt2pid = pe32.th32ProcessID;
					
					//SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
				}
			}
		}

	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);   //关闭句柄

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, zt2pid);

	DWORD value = MAX_PATH;
	char buffer[MAX_PATH] = { 0 };
	QueryFullProcessImageName(hProcess, 0, buffer, &value);
	std::cout << "fffff:" << buffer << std::endl;

	MessageBox(NULL, buffer, "", MB_OK);
	

	return 0;
}