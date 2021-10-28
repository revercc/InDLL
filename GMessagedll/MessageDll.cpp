
#include <Windows.h>
HHOOK hhook;
HMODULE	hInstance;



//键盘钩子回调函数
LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}




//安装键盘钩子
BOOL InstallHook(DWORD hwnd, DWORD dwMsg)
{
	hhook = SetWindowsHookEx(WH_KEYBOARD, keyboardHookProc, hInstance, 0);			//全局消息钩子
	return 0;
}

//卸载键盘钩子
BOOL UnInstallHook()
{
	UnhookWindowsHookEx(hhook);
	return 0;
}




//动态链接库的入口函数
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:					//DLL刚映射到进程序进程空间中
		hInstance = hModule;
		break;

	case DLL_THREAD_ATTACH:						//应用程序创建了一个新线程
		break;

	case DLL_PROCESS_DETACH:					//DLL从进程地址空间中卸载
		break;

	case DLL_THREAD_DETACH:						//应用程序结束一个线程
		break;

	}
	return TRUE;
}


