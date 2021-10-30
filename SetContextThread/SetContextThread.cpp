#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>

#pragma pack(1)
struct INSERT_DATA
{
	BYTE ShellCode[0x30];
	ULONG_PTR MyLoadLibraryA;
	ULONG_PTR OldEip;
	char szDllPath[MAX_PATH];
};


__declspec(naked)
void ShellCodeFunc()
{
	_asm
	{
		nop
		pushad
		pushfd
		call L1
		L1 :
		pop	ebx
			lea	eax,ds:[ebx + 0x30]
			push	eax 								//&szDllPath
			call	dword ptr ds : [ebx + 0x28]		//call LoadLibrary
			mov	eax, dword ptr ds : [ebx + 0x2c]	//OldEip
			xchg	eax, [esp + 0x24]
			popfd
			popad
			retn
	}
}

int main(int argc, char* argv[], char* envp)
{
	DWORD  dwPID;
	DWORD  dwNum = 0;
	HANDLE hThread;
	HANDLE hProcess;
	CONTEXT stContext;
	LPVOID lData = NULL;
	INSERT_DATA stInsertData = { 0 };
	char	szDllPath[MAX_PATH] = "C:\\Users\\Administrator\\Desktop\\MsgDll.dll";

	HANDLE hSnap;
	THREADENTRY32 stThreadEntry32 = { 0 };
	stThreadEntry32.dwSize = sizeof(THREADENTRY32);
	stContext.ContextFlags = CONTEXT_ALL;

	scanf_s("%d", &dwPID);

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	Thread32First(hSnap, &stThreadEntry32);
	do {
		if (stThreadEntry32.th32OwnerProcessID == dwPID)
		{
			hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, stThreadEntry32.th32ThreadID);
			SuspendThread(hThread);
			GetThreadContext(hThread, &stContext);


			//Bulid InsertData
			stInsertData.MyLoadLibraryA = (ULONG_PTR)GetProcAddress(LoadLibrary(TEXT("kernel32.dll")), "LoadLibraryA");
			stInsertData.OldEip = stContext.Eip;
			memcpy(stInsertData.ShellCode, ShellCodeFunc, 0x30);
			stInsertData.ShellCode[0] = 0x90;
			memcpy(stInsertData.szDllPath, szDllPath, strlen(szDllPath));

			//将插入数据写入到目标进程
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, stThreadEntry32.th32OwnerProcessID);
			lData = VirtualAllocEx(hProcess, NULL, sizeof(INSERT_DATA), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			WriteProcessMemory(hProcess, lData, &stInsertData, sizeof(INSERT_DATA), &dwNum);


			//设置新的eip
			stContext.Eip = (ULONG_PTR)lData;
			SetThreadContext(hThread, &stContext);


			CloseHandle(hProcess);
			CloseHandle(hThread);
			break;
		}
	} while (Thread32Next(hSnap, &stThreadEntry32));

	return 0;
}




