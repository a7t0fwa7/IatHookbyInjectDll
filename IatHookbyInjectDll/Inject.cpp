#include "string.h"
#include "windows.h"
#include "tlhelp32.h"


const char* pkill = "MyDll.dll"; //DLL文件的路径
char* prosess = "TestMain.exe"; //要注入的进程名(目标进程名)

int main()
{
	HANDLE hSnap;
	HANDLE hkernel32 = NULL; //被注入进程的句柄
	PROCESSENTRY32 pe;
	BOOL bNext;
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID Luid;
	LPVOID p;
	FARPROC pfn;

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return 1;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid))
	{
		return 1;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = Luid;

	if(!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		return 1;
	}

	pe.dwSize = sizeof(pe);
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bNext = Process32First(hSnap, &pe);
	while (bNext)
	{
		//遍历进程名，找要被注入的
		if (!stricmp(pe.szExeFile, prosess))
		{

			hkernel32 = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, 1, pe.th32ProcessID);
			break;
		}
		bNext = Process32Next(hSnap, &pe);
	}
	CloseHandle(hSnap);
	//拿到远程进程句柄，使用CreateRemoteThread远程进程注入
	p = VirtualAllocEx(hkernel32, NULL, strlen(pkill), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hkernel32, p, pkill, strlen(pkill), NULL);
	pfn = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	CreateRemoteThread(hkernel32, NULL, 0, (LPTHREAD_START_ROUTINE)pfn, p, NULL, 0);
	return 0;
}

