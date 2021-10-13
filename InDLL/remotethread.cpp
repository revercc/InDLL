#include <Windows.h>
#include <tlhelp32.h>
#include "resource.h"
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")


BOOL ZwCreateThreadExInjectDll(DWORD dwProcessId, char *pszDllFileName);
BOOL CALLBACK _ProcDlgHelp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK _ProcWinMain(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG		stMsg;
	HWND	hWinMain;
	hWinMain = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)_ProcWinMain, NULL);
	while(1)
	{
		if(0 == GetMessage(&stMsg, NULL,0,0))
			break;
		TranslateMessage(&stMsg);
		DispatchMessage(&stMsg);
	}
	return stMsg.wParam;
}


//远程线程注入
BOOL _stdcall _RemoteThread(HWND hDlg)
{

	HANDLE hSnapShot;
	TCHAR szExeName[256] = {0};
	TCHAR szDllName[256] = {0};
	PROCESSENTRY32	stProcess;
	RtlZeroMemory(&stProcess, sizeof(stProcess));
	stProcess.dwSize = sizeof(stProcess);
	WIN32_FIND_DATA stFindFile;
	HANDLE	hFile;
	HANDLE	hProcess;
	LPVOID	lpDllName;
	LPTHREAD_START_ROUTINE lpProcLoadLibrary;

	GetDlgItemText(hDlg, IDC_EDIT1, szExeName, sizeof(szExeName));
	
	hSnapShot		= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);			//建立进程列表快照
	if(Process32First(hSnapShot, &stProcess))
	{
		while(1)
		{
			if(wcscmp(szExeName, stProcess.szExeFile) == 0)							//如过遍历到的进程的可执行文件名称和我们输入的一样
			{
				CloseHandle(hSnapShot);
				GetDlgItemText(hDlg, IDC_EDIT2, szDllName, sizeof(szDllName));
				hFile			= FindFirstFile(szDllName, &stFindFile);			//在当前路径搜索DLL文件
				if(hFile != INVALID_HANDLE_VALUE)						
				{
							FindClose(hFile);
							//打开目标进程并获得权限
							hProcess	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, stProcess.th32ProcessID);
							//在目标进程地址空间中申请内存
							lpDllName	= VirtualAllocEx(hProcess, NULL, wcslen(szDllName) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("申请内存错误！"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							//向目标地址空间申请的内存处写入数据
							char a[256] ={0};			//把宽字符变成窄字符
							for(int i = 0;;i++)
							{
								if(szDllName[i] == 0)
									break;
								a[i] = szDllName[i];
							}
							WriteProcessMemory(hProcess, lpDllName, a, strlen(a) + 1, NULL);
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("写入进程错误！"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							//获得LoadLibraryW函数的地址
							lpProcLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("获得LoadLibraryA地址错误！"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							//创建远程线程
							HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, lpProcLoadLibrary, lpDllName, 0, NULL);
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("创建远程线程错误！"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							WaitForSingleObject(hThread, INFINITE);						//如果远程线程创建成功，则等线程结束后关闭线程
							CloseHandle(hThread);
							CloseHandle(hProcess);
							return TRUE;		
				}
				else
				{
					MessageBox(NULL, TEXT("没用找到DLL文件！"), NULL, MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}
				
			}
			if(Process32Next(hSnapShot, &stProcess) == FALSE)
			{
				MessageBox(NULL, TEXT("没有找到目标进程！"), NULL, MB_OK | MB_ICONEXCLAMATION);
				CloseHandle(hSnapShot);
				return FALSE;
			}
		}
	}
}


//APC注入
BOOL _stdcall _IntoAPC(HWND hDlg)
{
	HANDLE			hSnapProcess;		//进程快照句柄
	HANDLE			hSnapThread;		//线程快照句柄
	PROCESSENTRY32	stProcess;			//这两个结构在使用前需要指明其size大小
	THREADENTRY32	stThread;
	HANDLE			hFile;
	HANDLE			hProcess;			//进程句柄
	HANDLE			hThread;			//线程句柄
	TCHAR	szDllName[256] = {0};
	TCHAR	szExeName[256] = {0};
	WIN32_FIND_DATA stFindFile;
	LPVOID			lpDllName;
	GetDlgItemText(hDlg, IDC_EDIT1, szExeName, sizeof(szExeName));
	RtlZeroMemory(&stProcess, sizeof(stProcess));					
	stProcess.dwSize = sizeof(stProcess);
	RtlZeroMemory(&stThread, sizeof(stThread));
	stThread.dwSize	 = sizeof(stThread);



	hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	Process32First(hSnapProcess, &stProcess);
	
	while(1)
	{
		if(wcscmp(stProcess.szExeFile, szExeName) == 0)
		{
			CloseHandle(hSnapProcess);
			GetDlgItemText(hDlg, IDC_EDIT2, szDllName, sizeof(szDllName));
			hFile = FindFirstFile(szDllName, &stFindFile);
			if(hFile != INVALID_HANDLE_VALUE)
			{

				FindClose(hFile);
				char szDllName1[256] ={0};			//把宽字符变成窄字符
				for(int i = 0;;i++)
				{
					if(szDllName[i] == 0)
						break;
					szDllName1[i] = szDllName[i];
				}
				
				
				hProcess	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, stProcess.th32ProcessID);
				if(hProcess == NULL)
				{
					MessageBox(NULL, TEXT("打开进程失败！"), NULL, MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				lpDllName	= VirtualAllocEx(hProcess, NULL, sizeof(szDllName1) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				WriteProcessMemory(hProcess, lpDllName, szDllName1, sizeof(szDllName1) + 1,NULL);

				if(GetLastError() != 0)
				{
					MessageBox(NULL, TEXT("写入进程错误！"), NULL, MB_OK | MB_ICONEXCLAMATION);
					CloseHandle(hProcess);
					break;
				}


				hSnapThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);				//建立线程快照
				Thread32First(hSnapThread, &stThread);
				while(1)
				{
					if(stThread.th32OwnerProcessID == stProcess.th32ProcessID)
					{
						hThread	= OpenThread(THREAD_ALL_ACCESS, FALSE, stThread.th32ThreadID);
						if(hThread)
						{
							QueueUserAPC((PAPCFUNC)LoadLibraryA, hThread, (ULONG_PTR)lpDllName);
							CloseHandle(hThread);
						}
					}
					if(Thread32Next(hSnapThread, &stThread) == FALSE)
						break;

				}
				return TRUE;



			}
			else
			{
				MessageBox(NULL, TEXT("没有找到DLL文件！"), NULL, MB_OK | MB_ICONEXCLAMATION);
				break;
			}

		}
		if(Process32Next(hSnapProcess, &stProcess) == FALSE)
		{
			
			MessageBox(NULL, TEXT("没有找到目标进程！"), NULL, MB_OK | MB_ICONEXCLAMATION);
			CloseHandle(hSnapProcess);
			break;
		}
	}
	return FALSE;
}


//获得加载基地址
DWORD FindImageBase(HANDLE hProcess)
{
	DWORD	uResult;
	SIZE_T dwSize = 0;


	//获取页的大小
	SYSTEM_INFO sysinfo;
	ZeroMemory(&sysinfo,sizeof(SYSTEM_INFO));
	GetSystemInfo(&sysinfo);

	MEMORY_BASIC_INFORMATION mbi = {0};
	BOOL bFoundMemImage = FALSE ;
	PBYTE pAddress = NULL ;

	pAddress = (PBYTE)sysinfo.lpMinimumApplicationAddress;
	while (pAddress < (PBYTE)sysinfo.lpMaximumApplicationAddress)
	{
		ZeroMemory(&mbi,sizeof(MEMORY_BASIC_INFORMATION));
		dwSize = VirtualQueryEx(hProcess,pAddress,&mbi,sizeof(MEMORY_BASIC_INFORMATION));
		if (dwSize == 0)
		{
			pAddress += sysinfo.dwPageSize ;
			continue;
		}

		switch(mbi.State)
		{
		case MEM_FREE:
		case MEM_RESERVE:
			pAddress = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
			break;
		case MEM_COMMIT:
			if (mbi.Type == MEM_IMAGE)
			{
				uResult	= (ULONG_PTR)pAddress;
				bFoundMemImage = TRUE;
				break;
			}
			pAddress = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
		    break;
		default:
		    break;
		}

		if (bFoundMemImage)
		{
			break;
		}
	}
	return uResult;


}


//更改输入表加载。
BOOL _stdcall _ModifyPE(HWND hDlg)
{

	HANDLE			hFile;
	HANDLE			hSnapProcess;
	PROCESSENTRY32	stProcess;
	WIN32_FIND_DATA stFindFile;
	STARTUPINFO		si;
    PROCESS_INFORMATION pi;
	DWORD			ImageBase;						//目标进程加载基址
	TCHAR			szExeName[256] = {0};
	TCHAR			szDllName[256] = {0};
	ZeroMemory( &si, sizeof(si));
    si.cb	= sizeof(si);
    ZeroMemory( &pi, sizeof(pi));

	GetDlgItemText(hDlg, IDC_EDIT1, szExeName, sizeof(szExeName));
	hFile	= FindFirstFile(szExeName, &stFindFile);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		GetDlgItemText(hDlg, IDC_EDIT2, szDllName, sizeof(szDllName));
		hFile	= FindFirstFile(szDllName, &stFindFile);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			hSnapProcess	= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			Process32First(hSnapProcess, &stProcess);
			while(1)
			{
				if(wcscmp(stProcess.szExeFile, szExeName) == 0)
				{
					MessageBox(NULL, TEXT("此进程已存在，创建进程失败"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}
				if(FALSE == Process32Next(hSnapProcess, &stProcess))
					break;
			}

			//以挂起方式创建目标进程
			CreateProcess(szExeName, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL, NULL, &si, &pi);			
			
			ImageBase	= FindImageBase(pi.hProcess);								//寻找目标进程加载基址


		}
		else
			MessageBox(NULL, TEXT("没有找到DLL文件！"), NULL, MB_OK | MB_ICONEXCLAMATION);
	}
	else
		MessageBox(NULL, TEXT("没有找到可执行文件！"), NULL, MB_OK | MB_ICONEXCLAMATION);
	return FALSE;

}


//增强型远程线程注入dll（可以注入系统进程）
BOOL _stdcall _RemoteThreadPus(HWND hDlg)
{
	HANDLE			hSnapProcess;		//进程快照句柄
	HANDLE			hSnapThread;		//线程快照句柄
	PROCESSENTRY32	stProcess;			//这两个结构在使用前需要指明其size大小
	THREADENTRY32	stThread;
	HANDLE			hFile;
	HANDLE			hProcess;			//进程句柄
	HANDLE			hThread;			//线程句柄
	TCHAR	szDllName[256] = {0};
	TCHAR	szExeName[256] = {0};
	WIN32_FIND_DATA stFindFile;
	LPVOID			lpDllName;
	GetDlgItemText(hDlg, IDC_EDIT1, szExeName, sizeof(szExeName));
	RtlZeroMemory(&stProcess, sizeof(stProcess));					
	stProcess.dwSize = sizeof(stProcess);
	RtlZeroMemory(&stThread, sizeof(stThread));
	stThread.dwSize	 = sizeof(stThread);

	LPTHREAD_START_ROUTINE			lpProcLoadLibrary = NULL;			//loadLibrary()函数的地址
#ifdef _WIN64

	//ZwCreateThreadEx函数指针（64位类型）
	DWORD (* _stdcall ZwCreateThreadEx)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, ULONG CreateSuspended, SIZE_T dwStackSize, SIZE_T dw1, SIZE_T dw2, LPVOID pUnkown);
#else
	DWORD (* _stdcall ZwCreateThreadEx)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, BOOL CreateSuspended, DWORD dwStackSize, DWORD dw1, DWORD dw2, LPVOID pUnkown);

#endif

	HANDLE		hRemoteThread = 0;										//远程线程句柄


	hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	Process32First(hSnapProcess, &stProcess);
	
	while(1)
	{
		if(wcscmp(stProcess.szExeFile, szExeName) == 0)
		{
			CloseHandle(hSnapProcess);
			GetDlgItemText(hDlg, IDC_EDIT2, szDllName, sizeof(szDllName));
			hFile = FindFirstFile(szDllName, &stFindFile);
			if(hFile != INVALID_HANDLE_VALUE)
			{

				FindClose(hFile);
				char szDllName1[256] ={0};			//把宽字符变成窄字符
				for(int i = 0;;i++)
				{
					if(szDllName[i] == 0)
						break;
					szDllName1[i] = szDllName[i];
				}
				

				//获得权限并写入dll文件路径到目标进程
				hProcess	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, stProcess.th32ProcessID);
				if(hProcess == NULL)
				{
					MessageBox(NULL, TEXT("打开进程错误"), NULL, MB_OK);
					return FALSE;
				}
				lpDllName	= VirtualAllocEx(hProcess, NULL, sizeof(szDllName1) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				WriteProcessMemory(hProcess, lpDllName, szDllName1, sizeof(szDllName1) + 1,NULL);

				//获得LoadLibraryA函数的地址
				lpProcLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
				
#ifdef _WIN64
				//创建远程线程
				ZwCreateThreadEx = (DWORD (*)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
								LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, ULONG CreateSuspended, SIZE_T dwStackSize, SIZE_T dw1, SIZE_T dw2, LPVOID pUnkown))GetProcAddress(LoadLibrary(TEXT("ntdll.dll")), "ZwCreateThreadEx");
#else
				ZwCreateThreadEx = (DWORD (*)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
								LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, BOOL CreateSuspended, DWORD dwStackSize, DWORD dw1, DWORD dw2, LPVOID pUnkown))GetProcAddress(LoadLibrary(TEXT("ntdll.dll")), "ZwCreateThreadEx");
#endif


				ZwCreateThreadEx((PHANDLE)&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hProcess, lpProcLoadLibrary, lpDllName, 0,0,0,0,NULL);
				
				if(NULL == hRemoteThread)
				{
					MessageBox(NULL, TEXT("创建远程线程错误"), NULL, MB_OK);
					return FALSE;

				}
				


				WaitForSingleObject(hRemoteThread, INFINITE);						//如果远程线程创建成功，则等线程结束后关闭线程
				CloseHandle(hRemoteThread);
				CloseHandle(hProcess);
				return TRUE;		

			}
			else
			{
				MessageBox(NULL, TEXT("没有找到DLL文件！"), NULL, MB_OK | MB_ICONEXCLAMATION);
				break;
			}

		}
		if(Process32Next(hSnapProcess, &stProcess) == FALSE)
		{
			
			MessageBox(NULL, TEXT("没有找到目标进程！"), NULL, MB_OK | MB_ICONEXCLAMATION);
			CloseHandle(hSnapProcess);
			break;
		}
	}


return  FALSE;
}





//主窗口窗口过程
BOOL CALLBACK _ProcWinMain(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON	hIcon;
	static	HMENU	hMenu;
	
	switch(message)
	{
	
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_BUTTON2:

			SendMessage(hDlg, WM_CLOSE, NULL, NULL);
			break;
		case IDC_BUTTON1:
			
			if( (MF_USECHECKBITMAPS | MF_CHECKED) ==  GetMenuState(hMenu, ID_40001, MF_BYCOMMAND))
			{  
				if(TRUE == _RemoteThread(hDlg))
					MessageBox(NULL, TEXT("成功注入DLL！"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if((MF_USECHECKBITMAPS | MF_CHECKED) == GetMenuState(hMenu, ID_40002, MF_BYCOMMAND))
			{
				if(TRUE == _IntoAPC(hDlg))
					MessageBox(NULL, TEXT("成功注入DLL！"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if((MF_USECHECKBITMAPS | MF_CHECKED) == GetMenuState(hMenu, ID_40003, MF_BYCOMMAND))
			{
				if(TRUE == _ModifyPE(hDlg))
					MessageBox(NULL, TEXT("成功注入DLL！"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if((MF_USECHECKBITMAPS | MF_CHECKED) == GetMenuState(hMenu, ID_40004, MF_BYCOMMAND))
			{
				if(TRUE == _RemoteThreadPus(hDlg))
					MessageBox(NULL, TEXT("成功注入DLL！"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else
				MessageBox(NULL, TEXT("请先选择你注入的方式！"), TEXT("警告"),MB_OK | MB_ICONEXCLAMATION);
			break;
		case ID_40001:
			CheckMenuRadioItem(hMenu, ID_40001, ID_40004, ID_40001, MF_BYCOMMAND);
			break;
		case ID_40002:
			CheckMenuRadioItem(hMenu, ID_40001, ID_40004, ID_40002, MF_BYCOMMAND);
			break;
		case ID_40003:
			CheckMenuRadioItem(hMenu, ID_40001, ID_40004, ID_40003, MF_BYCOMMAND);
			break;
		case ID_40004:
			CheckMenuRadioItem(hMenu, ID_40001, ID_40004, ID_40004, MF_BYCOMMAND);
			break;
		case ID_40006:
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hDlg, (DLGPROC)_ProcDlgHelp, NULL);
			break;

		}
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;
	case WM_DESTROY:
		PostQuitMessage(NULL);
		break;
	case WM_INITDIALOG:
		hIcon	= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, LPARAM(hIcon));
		hMenu = GetMenu(hDlg);

		
		
		break;
	default:
		return FALSE;

	}
	return TRUE;

}

BOOL CALLBACK _ProcDlgHelp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_BUTTON1:
			EndDialog(hDlg, NULL);
			break;
		}
		
		break;
	
	default:
		return FALSE;
	}
	return TRUE;
}





