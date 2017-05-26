#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>


///< ö�ٴ��ڲ���
typedef struct
{
	HWND    hwndWindow;     // ���ھ��
	DWORD   dwProcessID;    // ����ID
}EnumWindowsArg;

///< ö�ٴ��ڻص�����
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	EnumWindowsArg *pArg = (EnumWindowsArg *)lParam;
	DWORD  dwProcessID = 0;
	// ͨ�����ھ��ȡ�ý���ID
	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (dwProcessID == pArg->dwProcessID)
	{
		pArg->hwndWindow = hwnd;
		// �ҵ��˷���FALSE
		return FALSE;
	}
	// û�ҵ��������ң�����TRUE
	return TRUE;
}
///< ͨ������ID��ȡ���ھ��
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
	// ��ȡ���̿���
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot (of processes) ʧ��");
		return 0 ;
	}
	// ��������������ṹ�Ĵ�С
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// ��ʼ�оٽ�����Ϣ
	if (!Process32First(hSnapshot, &pe32))
	{
		printf("Process32First() ʧ��");
		CloseHandle(hSnapshot); // �رվ��
		return 0;
	}

	DWORD zt2pid = 0;
	printf("����ID\t������\t�߳���\t���ȼ�\t������\n"); // �������ȼ�
	do {
		// ��ӡ���������Ϣ
		printf("\t%u", pe32.th32ProcessID);    // ����id
		printf("\t%u", pe32.th32ParentProcessID);  // ������id
		printf("\t%d", pe32.cntThreads);       // �߳���
		printf("\t%d", pe32.pcPriClassBase);   // �������ȼ�
		printf("\t%s\n", pe32.szExeFile);        // ������

		std::string name = pe32.szExeFile;

		std::cout << name << std::endl;

		if (name == "zhengtu2.dat")
		{
			//��ȡ���̵ľ��  
			HANDLE zt2_process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			if (zt2_process != INVALID_HANDLE_VALUE)
			{
				//��ȡ��һ�����Ӵ��ڵľ��  
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

	CloseHandle(hSnapshot);   //�رվ��

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, zt2pid);

	DWORD value = MAX_PATH;
	char buffer[MAX_PATH] = { 0 };
	QueryFullProcessImageName(hProcess, 0, buffer, &value);
	std::cout << "fffff:" << buffer << std::endl;

	MessageBox(NULL, buffer, "", MB_OK);
	

	return 0;
}