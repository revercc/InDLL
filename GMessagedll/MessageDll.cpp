
#include <Windows.h>
HHOOK hhook;
HMODULE	hInstance;



//���̹��ӻص�����
LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}




//��װ���̹���
BOOL InstallHook(DWORD hwnd, DWORD dwMsg)
{
	hhook = SetWindowsHookEx(WH_KEYBOARD, keyboardHookProc, hInstance, 0);			//ȫ����Ϣ����
	return 0;
}

//ж�ؼ��̹���
BOOL UnInstallHook()
{
	UnhookWindowsHookEx(hhook);
	return 0;
}




//��̬���ӿ����ں���
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:					//DLL��ӳ�䵽��������̿ռ���
		hInstance = hModule;
		break;

	case DLL_THREAD_ATTACH:						//Ӧ�ó��򴴽���һ�����߳�
		break;

	case DLL_PROCESS_DETACH:					//DLL�ӽ��̵�ַ�ռ���ж��
		break;

	case DLL_THREAD_DETACH:						//Ӧ�ó������һ���߳�
		break;

	}
	return TRUE;
}


