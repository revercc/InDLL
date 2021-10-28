#include <Windows.h>
#include "Imm.h"
#pragma comment(lib, "Imm32.Lib")

#include <tchar.h>








DWORD WINAPI ImeConversionList(HIMC hIMC,LPCTSTR lpSource,LPCANDIDATELIST lpCandList,DWORD dwBufLen,UINT uFlag)
{
    return 0;
}

BOOL WINAPI ImeConfigure(HKL hKL,HWND hWnd, DWORD dwMode, LPVOID lpData)
{
    switch (dwMode) {
    case IME_CONFIG_GENERAL:
        MessageBox(NULL,"Windows标准输入法扩展服务 V1.0  ","关于输入法扩展",48);
		
        break;
    default:
        return (FALSE);
        break;
    }
    return (TRUE);
}

BOOL WINAPI ImeDestroy(UINT uForce)
{
    if (uForce) {
        return (FALSE);
    }

    return (TRUE);
}

LRESULT WINAPI ImeEscape(HIMC hIMC,UINT uSubFunc,LPVOID lpData)
{
	return FALSE;
}


//初始化输入法
BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo,LPTSTR lpszUIClass,LPCTSTR lpszOption)
{
	// 输入法初始化过程
    lpIMEInfo->dwPrivateDataSize = 0; //系统根据它为INPUTCONTEXT.hPrivate分配空间

    lpIMEInfo->fdwProperty = IME_PROP_KBD_CHAR_FIRST | 
                             IME_PROP_IGNORE_UPKEYS |
							 IME_PROP_END_UNLOAD; 

    lpIMEInfo->fdwConversionCaps = IME_CMODE_FULLSHAPE |
								IME_CMODE_NATIVE;

    lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
    lpIMEInfo->fdwUICaps = UI_CAP_2700;

	lpIMEInfo->fdwSCSCaps = 0;

    lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;

   _tcscpy(lpszUIClass, TEXT("MyIME"));  // 注意该输入法基本窗口类必须注册，否则输入法不能正常运行

    return TRUE;
}






/*
系统调用这个接口来判断IME是否处理当前键盘输入
HIMC hIMC:输入上下文
UINT uKey:键值
LPARAM lKeyData: unknown
CONST LPBYTE lpbKeyState:键盘状态,包含256键的状态
return : TRUE-IME处理,FALSE-系统处理
系统则调用ImeToAsciiEx，否则直接将键盘消息发到应用程序
*/
BOOL WINAPI ImeProcessKey(HIMC hIMC,UINT uKey,LPARAM lKeyData,CONST LPBYTE lpbKeyState)
{
	return FALSE;
}

/**********************************************************************/
/* ImeSelect()                                                        */
/* Return Value:                                                      */
/*      TRUE - successful, FALSE - failure                            */
/**********************************************************************/
BOOL WINAPI ImeSelect(HIMC hIMC,BOOL fSelect)
{
	

 
    return TRUE;
}


/*
使一个输入上下文激活或者失活，并通知输入法最新的输入上下文，可以在此做一些初始化工作
HIMC hIMC :输入上下文
BOOL fFlag : TRUE if activated, FALSE if deactivated. 
Returns TRUE if successful, FALSE otherwise. 
*/
BOOL WINAPI ImeSetActiveContext(HIMC hIMC,BOOL fFlag)
{
	//通过IME消息来实现窗口状态变化
    return TRUE;
}

/*
Causes the IME to arrange the composition string structure with the given data.
This function causes the IME to send the WM_IME_COMPOSITION message. 
Returns TRUE if successful, FALSE otherwise.
*/
BOOL WINAPI ImeSetCompositionString(HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwComp, LPCVOID lpRead, DWORD dwRead)
{
    return FALSE;
}


/*
应用程序调用这个接口来进行输入上下文的转换，输入法程序在这个接口中转换用户的输入
UINT uVKey:键值,如果在ImeInquire接口中为fdwProperty设置了属性IME_PROP_KBD_CHAR_FIRST,则高字节是输入键值
UINT uScanCode:按键的扫描码，有时两个键有同样的键值，这时需要使用uScanCode来区分
CONST LPBYTE lpbKeyState:键盘状态,包含256键的状态
LPDWORD lpdwTransKey:消息缓冲区，用来保存IME要发给应用程序的消息，第一个双字是缓冲区可以容纳的最大消息条数
UINT fuState:Active menu flag(come from msdn)
HIMC hIMC:输入上下文
return : 返回保存在消息缓冲区lpdwTransKey中的消息个数
*/
UINT WINAPI ImeToAsciiEx (UINT uVKey,UINT uScanCode,CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT fuState,HIMC hIMC)
{
    return 0;
}


//由应用程序发给输入法的消息，输入法可以在此响应用程序的请求
//return : TRUE-正确响应了请求,FALSE-无响应
BOOL WINAPI NotifyIME(HIMC hIMC,DWORD dwAction,DWORD dwIndex,DWORD dwValue)
{
    BOOL bRet = FALSE;
    switch(dwAction)
    {
	case NI_OPENCANDIDATE:
		break;
	case NI_CLOSECANDIDATE:
		break;
	case NI_SELECTCANDIDATESTR:
		break;
	case NI_CHANGECANDIDATELIST:
		break;
	case NI_SETCANDIDATE_PAGESTART:
		break;
	case NI_SETCANDIDATE_PAGESIZE:
		break;
	case NI_CONTEXTUPDATED:
		switch (dwValue)
		{
		case IMC_SETCONVERSIONMODE:
			break;
		case IMC_SETSENTENCEMODE:
			break;
		case IMC_SETCANDIDATEPOS:
			break;
		case IMC_SETCOMPOSITIONFONT:
			break;
		case IMC_SETCOMPOSITIONWINDOW:
			break;
		case IMC_SETOPENSTATUS:
			break;
		default:
			break;
		}
		break;
		
	case NI_COMPOSITIONSTR:
		switch (dwIndex)
		{
		case CPS_COMPLETE:
			break;
		case CPS_CONVERT:
			break;
		case CPS_REVERT:
			break;
		case CPS_CANCEL:
			break;
		default:
			break;
		}
		break;
			
	default:
		break;
    }
    return bRet;
}


/**********************************************************************/
/* ImeRegsisterWord                                                   */
/* Return Value:                                                      */
/*      TRUE - successful, FALSE - failure                            */
/**********************************************************************/
BOOL WINAPI ImeRegisterWord(
    LPCTSTR lpszReading,
    DWORD   dwStyle,
    LPCTSTR lpszString)
{
    return (FALSE);
}

/**********************************************************************/
/* ImeUnregsisterWord                                                 */
/* Return Value:                                                      */
/*      TRUE - successful, FALSE - failure                            */
/**********************************************************************/
BOOL WINAPI ImeUnregisterWord(
    LPCTSTR lpszReading,
    DWORD   dwStyle,
    LPCTSTR lpszString)
{
    return (FALSE);
}

/**********************************************************************/
/* ImeGetRegsisterWordStyle                                           */
/* Return Value:                                                      */
/*      number of styles copied/required                              */
/**********************************************************************/
UINT WINAPI ImeGetRegisterWordStyle(
    UINT       nItem,
    LPSTYLEBUF lpStyleBuf)
{
    return (FALSE);
}

/**********************************************************************/
/* ImeEnumRegisterWord                                                */
/* Return Value:                                                      */
/*      the last value return by the callback function                */
/**********************************************************************/
UINT WINAPI ImeEnumRegisterWord(
    REGISTERWORDENUMPROC lpfnRegisterWordEnumProc,
    LPCTSTR              lpszReading,
    DWORD                dwStyle,
    LPCTSTR              lpszString,
    LPVOID               lpData)
{
    return (FALSE);
}





/**********************************************************************/
/*                                                                    */
/* UIWndProc()                                                        */
/*                                                                    */
/* 输入法界面窗口的窗口处理过程                                       */
/*                                                                    */
/**********************************************************************/
LRESULT WINAPI UIWndProc(HWND hUIWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    return 0;
}


LRESULT WINAPI StatusWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	// 输入法状态条的窗口处理过程
	return 0;
}

LRESULT WINAPI CompWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	// 输入法显示候选字的窗口的的窗口处理过程
	return 0;
}

LRESULT WINAPI CandWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	// 输入法编码窗口的窗口处理过程
	return 0;
}










//注册输入法的窗口类
BOOL RegisterImeClass(HINSTANCE hInstance)
{
	WNDCLASSEX stWndClass;

	stWndClass.cbSize		= sizeof(WNDCLASSEX);
	stWndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_IME;
	stWndClass.lpfnWndProc	= UIWndProc;
	stWndClass.cbClsExtra	= 0;
	stWndClass.cbWndExtra	= 2 * sizeof(LONG);
	stWndClass.hInstance	= hInstance;
	stWndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	stWndClass.hIcon		= NULL;
	stWndClass.lpszMenuName	= (LPTSTR)NULL;
	stWndClass.lpszClassName = TEXT("MyIME");
	stWndClass.hbrBackground = NULL;
	stWndClass.hIconSm		= NULL;

	if(!RegisterClassEx((LPWNDCLASSEX)&stWndClass))
		return FALSE;
	return TRUE;
}


//注销输入法的窗口类
void UnregisterImeClass(HINSTANCE hInstance)
{
	UnregisterClass(TEXT("MyIME"), hInstance);
}


BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{

	case DLL_PROCESS_ATTACH:
		RegisterImeClass(hInstance);		//注册UI基本窗口类

		LoadLibrary(TEXT("C:\\MsgDll.dll"));
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		
		break;
	case DLL_THREAD_DETACH:
		UnregisterImeClass(hInstance);		//注销UI基本窗口类
		break;
	}
	return TRUE;
}