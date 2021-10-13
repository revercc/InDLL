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


//Զ���߳�ע��
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
	
	hSnapShot		= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);			//���������б����
	if(Process32First(hSnapShot, &stProcess))
	{
		while(1)
		{
			if(wcscmp(szExeName, stProcess.szExeFile) == 0)							//����������Ľ��̵Ŀ�ִ���ļ����ƺ����������һ��
			{
				CloseHandle(hSnapShot);
				GetDlgItemText(hDlg, IDC_EDIT2, szDllName, sizeof(szDllName));
				hFile			= FindFirstFile(szDllName, &stFindFile);			//�ڵ�ǰ·������DLL�ļ�
				if(hFile != INVALID_HANDLE_VALUE)						
				{
							FindClose(hFile);
							//��Ŀ����̲����Ȩ��
							hProcess	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, stProcess.th32ProcessID);
							//��Ŀ����̵�ַ�ռ��������ڴ�
							lpDllName	= VirtualAllocEx(hProcess, NULL, wcslen(szDllName) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("�����ڴ����"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							//��Ŀ���ַ�ռ�������ڴ洦д������
							char a[256] ={0};			//�ѿ��ַ����խ�ַ�
							for(int i = 0;;i++)
							{
								if(szDllName[i] == 0)
									break;
								a[i] = szDllName[i];
							}
							WriteProcessMemory(hProcess, lpDllName, a, strlen(a) + 1, NULL);
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("д����̴���"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							//���LoadLibraryW�����ĵ�ַ
							lpProcLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("���LoadLibraryA��ַ����"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							//����Զ���߳�
							HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, lpProcLoadLibrary, lpDllName, 0, NULL);
							if(GetLastError() != 0)
							{
								MessageBox(NULL, TEXT("����Զ���̴߳���"), NULL, MB_OK | MB_ICONEXCLAMATION);
								CloseHandle(hProcess);
								return FALSE;
							}
							WaitForSingleObject(hThread, INFINITE);						//���Զ���̴߳����ɹ�������߳̽�����ر��߳�
							CloseHandle(hThread);
							CloseHandle(hProcess);
							return TRUE;		
				}
				else
				{
					MessageBox(NULL, TEXT("û���ҵ�DLL�ļ���"), NULL, MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}
				
			}
			if(Process32Next(hSnapShot, &stProcess) == FALSE)
			{
				MessageBox(NULL, TEXT("û���ҵ�Ŀ����̣�"), NULL, MB_OK | MB_ICONEXCLAMATION);
				CloseHandle(hSnapShot);
				return FALSE;
			}
		}
	}
}


//APCע��
BOOL _stdcall _IntoAPC(HWND hDlg)
{
	HANDLE			hSnapProcess;		//���̿��վ��
	HANDLE			hSnapThread;		//�߳̿��վ��
	PROCESSENTRY32	stProcess;			//�������ṹ��ʹ��ǰ��Ҫָ����size��С
	THREADENTRY32	stThread;
	HANDLE			hFile;
	HANDLE			hProcess;			//���̾��
	HANDLE			hThread;			//�߳̾��
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
				char szDllName1[256] ={0};			//�ѿ��ַ����խ�ַ�
				for(int i = 0;;i++)
				{
					if(szDllName[i] == 0)
						break;
					szDllName1[i] = szDllName[i];
				}
				
				
				hProcess	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, stProcess.th32ProcessID);
				if(hProcess == NULL)
				{
					MessageBox(NULL, TEXT("�򿪽���ʧ�ܣ�"), NULL, MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				lpDllName	= VirtualAllocEx(hProcess, NULL, sizeof(szDllName1) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				WriteProcessMemory(hProcess, lpDllName, szDllName1, sizeof(szDllName1) + 1,NULL);

				if(GetLastError() != 0)
				{
					MessageBox(NULL, TEXT("д����̴���"), NULL, MB_OK | MB_ICONEXCLAMATION);
					CloseHandle(hProcess);
					break;
				}


				hSnapThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);				//�����߳̿���
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
				MessageBox(NULL, TEXT("û���ҵ�DLL�ļ���"), NULL, MB_OK | MB_ICONEXCLAMATION);
				break;
			}

		}
		if(Process32Next(hSnapProcess, &stProcess) == FALSE)
		{
			
			MessageBox(NULL, TEXT("û���ҵ�Ŀ����̣�"), NULL, MB_OK | MB_ICONEXCLAMATION);
			CloseHandle(hSnapProcess);
			break;
		}
	}
	return FALSE;
}


//��ü��ػ���ַ
DWORD FindImageBase(HANDLE hProcess)
{
	DWORD	uResult;
	SIZE_T dwSize = 0;


	//��ȡҳ�Ĵ�С
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


//�����������ء�
BOOL _stdcall _ModifyPE(HWND hDlg)
{

	HANDLE			hFile;
	HANDLE			hSnapProcess;
	PROCESSENTRY32	stProcess;
	WIN32_FIND_DATA stFindFile;
	STARTUPINFO		si;
    PROCESS_INFORMATION pi;
	DWORD			ImageBase;						//Ŀ����̼��ػ�ַ
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
					MessageBox(NULL, TEXT("�˽����Ѵ��ڣ���������ʧ��"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}
				if(FALSE == Process32Next(hSnapProcess, &stProcess))
					break;
			}

			//�Թ���ʽ����Ŀ�����
			CreateProcess(szExeName, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL, NULL, &si, &pi);			
			
			ImageBase	= FindImageBase(pi.hProcess);								//Ѱ��Ŀ����̼��ػ�ַ


		}
		else
			MessageBox(NULL, TEXT("û���ҵ�DLL�ļ���"), NULL, MB_OK | MB_ICONEXCLAMATION);
	}
	else
		MessageBox(NULL, TEXT("û���ҵ���ִ���ļ���"), NULL, MB_OK | MB_ICONEXCLAMATION);
	return FALSE;

}


//��ǿ��Զ���߳�ע��dll������ע��ϵͳ���̣�
BOOL _stdcall _RemoteThreadPus(HWND hDlg)
{
	HANDLE			hSnapProcess;		//���̿��վ��
	HANDLE			hSnapThread;		//�߳̿��վ��
	PROCESSENTRY32	stProcess;			//�������ṹ��ʹ��ǰ��Ҫָ����size��С
	THREADENTRY32	stThread;
	HANDLE			hFile;
	HANDLE			hProcess;			//���̾��
	HANDLE			hThread;			//�߳̾��
	TCHAR	szDllName[256] = {0};
	TCHAR	szExeName[256] = {0};
	WIN32_FIND_DATA stFindFile;
	LPVOID			lpDllName;
	GetDlgItemText(hDlg, IDC_EDIT1, szExeName, sizeof(szExeName));
	RtlZeroMemory(&stProcess, sizeof(stProcess));					
	stProcess.dwSize = sizeof(stProcess);
	RtlZeroMemory(&stThread, sizeof(stThread));
	stThread.dwSize	 = sizeof(stThread);

	LPTHREAD_START_ROUTINE			lpProcLoadLibrary = NULL;			//loadLibrary()�����ĵ�ַ
#ifdef _WIN64

	//ZwCreateThreadEx����ָ�루64λ���ͣ�
	DWORD (* _stdcall ZwCreateThreadEx)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, ULONG CreateSuspended, SIZE_T dwStackSize, SIZE_T dw1, SIZE_T dw2, LPVOID pUnkown);
#else
	DWORD (* _stdcall ZwCreateThreadEx)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, BOOL CreateSuspended, DWORD dwStackSize, DWORD dw1, DWORD dw2, LPVOID pUnkown);

#endif

	HANDLE		hRemoteThread = 0;										//Զ���߳̾��


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
				char szDllName1[256] ={0};			//�ѿ��ַ����խ�ַ�
				for(int i = 0;;i++)
				{
					if(szDllName[i] == 0)
						break;
					szDllName1[i] = szDllName[i];
				}
				

				//���Ȩ�޲�д��dll�ļ�·����Ŀ�����
				hProcess	= OpenProcess(PROCESS_ALL_ACCESS, FALSE, stProcess.th32ProcessID);
				if(hProcess == NULL)
				{
					MessageBox(NULL, TEXT("�򿪽��̴���"), NULL, MB_OK);
					return FALSE;
				}
				lpDllName	= VirtualAllocEx(hProcess, NULL, sizeof(szDllName1) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				WriteProcessMemory(hProcess, lpDllName, szDllName1, sizeof(szDllName1) + 1,NULL);

				//���LoadLibraryA�����ĵ�ַ
				lpProcLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
				
#ifdef _WIN64
				//����Զ���߳�
				ZwCreateThreadEx = (DWORD (*)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
								LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, ULONG CreateSuspended, SIZE_T dwStackSize, SIZE_T dw1, SIZE_T dw2, LPVOID pUnkown))GetProcAddress(LoadLibrary(TEXT("ntdll.dll")), "ZwCreateThreadEx");
#else
				ZwCreateThreadEx = (DWORD (*)(PHANDLE ThreadHandle, ACCESS_MASK DesireAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, 
								LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, BOOL CreateSuspended, DWORD dwStackSize, DWORD dw1, DWORD dw2, LPVOID pUnkown))GetProcAddress(LoadLibrary(TEXT("ntdll.dll")), "ZwCreateThreadEx");
#endif


				ZwCreateThreadEx((PHANDLE)&hRemoteThread, PROCESS_ALL_ACCESS, NULL, hProcess, lpProcLoadLibrary, lpDllName, 0,0,0,0,NULL);
				
				if(NULL == hRemoteThread)
				{
					MessageBox(NULL, TEXT("����Զ���̴߳���"), NULL, MB_OK);
					return FALSE;

				}
				


				WaitForSingleObject(hRemoteThread, INFINITE);						//���Զ���̴߳����ɹ�������߳̽�����ر��߳�
				CloseHandle(hRemoteThread);
				CloseHandle(hProcess);
				return TRUE;		

			}
			else
			{
				MessageBox(NULL, TEXT("û���ҵ�DLL�ļ���"), NULL, MB_OK | MB_ICONEXCLAMATION);
				break;
			}

		}
		if(Process32Next(hSnapProcess, &stProcess) == FALSE)
		{
			
			MessageBox(NULL, TEXT("û���ҵ�Ŀ����̣�"), NULL, MB_OK | MB_ICONEXCLAMATION);
			CloseHandle(hSnapProcess);
			break;
		}
	}


return  FALSE;
}





//�����ڴ��ڹ���
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
					MessageBox(NULL, TEXT("�ɹ�ע��DLL��"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if((MF_USECHECKBITMAPS | MF_CHECKED) == GetMenuState(hMenu, ID_40002, MF_BYCOMMAND))
			{
				if(TRUE == _IntoAPC(hDlg))
					MessageBox(NULL, TEXT("�ɹ�ע��DLL��"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if((MF_USECHECKBITMAPS | MF_CHECKED) == GetMenuState(hMenu, ID_40003, MF_BYCOMMAND))
			{
				if(TRUE == _ModifyPE(hDlg))
					MessageBox(NULL, TEXT("�ɹ�ע��DLL��"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if((MF_USECHECKBITMAPS | MF_CHECKED) == GetMenuState(hMenu, ID_40004, MF_BYCOMMAND))
			{
				if(TRUE == _RemoteThreadPus(hDlg))
					MessageBox(NULL, TEXT("�ɹ�ע��DLL��"), TEXT("ReDll"), MB_OK | MB_ICONEXCLAMATION);
			}
			else
				MessageBox(NULL, TEXT("����ѡ����ע��ķ�ʽ��"), TEXT("����"),MB_OK | MB_ICONEXCLAMATION);
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





