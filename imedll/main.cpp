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
        MessageBox(NULL,"Windows��׼���뷨��չ���� V1.0  ","�������뷨��չ",48);
		
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


//��ʼ�����뷨
BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo,LPTSTR lpszUIClass,LPCTSTR lpszOption)
{
	// ���뷨��ʼ������
    lpIMEInfo->dwPrivateDataSize = 0; //ϵͳ������ΪINPUTCONTEXT.hPrivate����ռ�

    lpIMEInfo->fdwProperty = IME_PROP_KBD_CHAR_FIRST | 
                             IME_PROP_IGNORE_UPKEYS |
							 IME_PROP_END_UNLOAD; 

    lpIMEInfo->fdwConversionCaps = IME_CMODE_FULLSHAPE |
								IME_CMODE_NATIVE;

    lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
    lpIMEInfo->fdwUICaps = UI_CAP_2700;

	lpIMEInfo->fdwSCSCaps = 0;

    lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;

   _tcscpy(lpszUIClass, TEXT("MyIME"));  // ע������뷨�������������ע�ᣬ�������뷨������������

    return TRUE;
}






/*
ϵͳ��������ӿ����ж�IME�Ƿ���ǰ��������
HIMC hIMC:����������
UINT uKey:��ֵ
LPARAM lKeyData: unknown
CONST LPBYTE lpbKeyState:����״̬,����256����״̬
return : TRUE-IME����,FALSE-ϵͳ����
ϵͳ�����ImeToAsciiEx������ֱ�ӽ�������Ϣ����Ӧ�ó���
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
ʹһ�����������ļ������ʧ���֪ͨ���뷨���µ����������ģ������ڴ���һЩ��ʼ������
HIMC hIMC :����������
BOOL fFlag : TRUE if activated, FALSE if deactivated. 
Returns TRUE if successful, FALSE otherwise. 
*/
BOOL WINAPI ImeSetActiveContext(HIMC hIMC,BOOL fFlag)
{
	//ͨ��IME��Ϣ��ʵ�ִ���״̬�仯
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
Ӧ�ó����������ӿ����������������ĵ�ת�������뷨����������ӿ���ת���û�������
UINT uVKey:��ֵ,�����ImeInquire�ӿ���ΪfdwProperty����������IME_PROP_KBD_CHAR_FIRST,����ֽ��������ֵ
UINT uScanCode:������ɨ���룬��ʱ��������ͬ���ļ�ֵ����ʱ��Ҫʹ��uScanCode������
CONST LPBYTE lpbKeyState:����״̬,����256����״̬
LPDWORD lpdwTransKey:��Ϣ����������������IMEҪ����Ӧ�ó������Ϣ����һ��˫���ǻ������������ɵ������Ϣ����
UINT fuState:Active menu flag(come from msdn)
HIMC hIMC:����������
return : ���ر�������Ϣ������lpdwTransKey�е���Ϣ����
*/
UINT WINAPI ImeToAsciiEx (UINT uVKey,UINT uScanCode,CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT fuState,HIMC hIMC)
{
    return 0;
}


//��Ӧ�ó��򷢸����뷨����Ϣ�����뷨�����ڴ���Ӧ�ó��������
//return : TRUE-��ȷ��Ӧ������,FALSE-����Ӧ
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
/* ���뷨���洰�ڵĴ��ڴ������                                       */
/*                                                                    */
/**********************************************************************/
LRESULT WINAPI UIWndProc(HWND hUIWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    return 0;
}


LRESULT WINAPI StatusWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	// ���뷨״̬���Ĵ��ڴ������
	return 0;
}

LRESULT WINAPI CompWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	// ���뷨��ʾ��ѡ�ֵĴ��ڵĵĴ��ڴ������
	return 0;
}

LRESULT WINAPI CandWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	// ���뷨���봰�ڵĴ��ڴ������
	return 0;
}










//ע�����뷨�Ĵ�����
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


//ע�����뷨�Ĵ�����
void UnregisterImeClass(HINSTANCE hInstance)
{
	UnregisterClass(TEXT("MyIME"), hInstance);
}


BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{

	case DLL_PROCESS_ATTACH:
		RegisterImeClass(hInstance);		//ע��UI����������

		LoadLibrary(TEXT("C:\\MsgDll.dll"));
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		
		break;
	case DLL_THREAD_DETACH:
		UnregisterImeClass(hInstance);		//ע��UI����������
		break;
	}
	return TRUE;
}