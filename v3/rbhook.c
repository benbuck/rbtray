#include <windows.h>
#include "rbtray.h"

static HHOOK hMouse = NULL,
//           hCall  = NULL,
             hMsg   = NULL,
			 hKey   = NULL;
static HWND  LastHit;

LRESULT CALLBACK
MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{

    if (nCode<0)return CallNextHookEx(hMouse, nCode, wParam, lParam);
    if (wParam == WM_NCRBUTTONDOWN
        && (((MOUSEHOOKSTRUCT*)lParam)->wHitTestCode == HTREDUCE || ((MOUSEHOOKSTRUCT*)lParam)->wHitTestCode == HTCLOSE))
    {
        LastHit=((MOUSEHOOKSTRUCT*)lParam)->hwnd;
    };
    if (wParam == WM_NCRBUTTONUP && LastHit==((MOUSEHOOKSTRUCT*)lParam)->hwnd
        && (((MOUSEHOOKSTRUCT*)lParam)->wHitTestCode == HTREDUCE|| ((MOUSEHOOKSTRUCT*)lParam)->wHitTestCode == HTCLOSE))
    {
        LastHit=(HWND)NULL;
        PostMessage(FindWindow(NAME, NAME), WM_MYCMD, IDM_TRAY,
            (LPARAM)(((MOUSEHOOKSTRUCT*)lParam)->hwnd));
    }
/*  if (wParam == WM_RBUTTONUP)
    {
        #define WND_CLASS "IRIS.tedit"
        char cls[255];
        int count;
        count = GetClassName((((MOUSEHOOKSTRUCT*)lParam)->hwnd),cls, 250);
        if(0 == strcmp(WND_CLASS, cls))
        {
            PostMessage(FindWindow(NAME, NAME), WM_MYCMD, IDM_LOTUSEDIT,
                (LPARAM)(((MOUSEHOOKSTRUCT*)lParam)->hwnd));
        }
    }
*/
    return CallNextHookEx(hMouse, nCode, wParam, lParam);
};
LRESULT CALLBACK
KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//	char s[255];
    if (nCode<0)return CallNextHookEx(hKey, nCode, wParam, lParam);
	if((((GetKeyState(VK_LWIN) & 0x80) || (GetKeyState(VK_RWIN) & 0x80)) && GetKeyState('T') & 0x80) &&
		(GetWindowLong(GetForegroundWindow(), GWL_STYLE) & WS_CAPTION))
		{
			BYTE kState[256];
			GetKeyboardState(kState);
			kState[VK_LWIN] = 0;
			kState[VK_RWIN] = 0;
			kState['T'] = 0;
			SetKeyboardState(kState);
		    PostMessage(FindWindow(NAME, NAME), WM_MYCMD, IDM_TRAY, (LPARAM)GetForegroundWindow());
	//		wsprintf(s, "%x", GetForegroundWindow());
	//		MessageBox(NULL, s, "",MB_OK);
			return -1;	
	}	
	if((GetKeyState(VK_PAUSE) & 0x80)
		//&&(GetWindowLong(GetForegroundWindow(), GWL_STYLE) & WS_CAPTION)
		)
		{
			BYTE kState[256];
			GetKeyboardState(kState);
			kState[VK_PAUSE] = 0;
			SetKeyboardState(kState);
		    PostMessage(FindWindow(NAME, NAME), WM_MYCMD, IDM_KEY, (LPARAM)GetForegroundWindow());
	//		wsprintf(s, "%x", GetForegroundWindow());
	//		MessageBox(NULL, s, "",MB_OK);
			return -1;	
	}
    PostMessage(FindWindow(NAME, NAME), WM_MYCMD, IDM_KEYSTAT, (LPARAM)GetForegroundWindow());
	return CallNextHookEx(hMouse, nCode, wParam, lParam);
};

LRESULT CALLBACK
GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    int i;
    HMENU hSysMenu;
    if (nCode<0)return CallNextHookEx(hMsg, nCode, wParam, lParam);
    if (((MSG*)lParam)->message==WM_SYSCOMMAND   &&
        (LOWORD(((MSG*)lParam)->wParam) == IDM_TRAY ||
         LOWORD(((MSG*)lParam)->wParam) == IDM_ONTOP||
         LOWORD(((MSG*)lParam)->wParam) == IDM_SIZE))
    {
        PostMessage(FindWindow(NAME, NAME), WM_MYCMD,((MSG*)lParam)->wParam,(LPARAM)(((MSG*)lParam)->hwnd));
    };
    if (((MSG*)lParam)->message==WM_PAINT)
    {
        hSysMenu=GetSystemMenu(((MSG*)lParam)->hwnd, FALSE);
        for(i=0;i<GetMenuItemCount(hSysMenu) && hSysMenu;i++)
            if(GetMenuItemID(hSysMenu,i)==IDM_TRAY) hSysMenu=0;
        if (hSysMenu)
        {
            InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_SEPARATOR,IDM_SEPARATOR, NULL) ;
            if(GetWindowLong(((MSG*)lParam)->hwnd,GWL_EXSTYLE) & WS_EX_TOPMOST)
                InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING|MF_CHECKED,IDM_ONTOP, "Always on top");
            else
                InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING,IDM_ONTOP, "Always on top");
            InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING,IDM_TRAY,  "Minimize in tray");
            InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING,IDM_SIZE,  "My size");
        }
    };
	if (((MSG*)lParam)->message==WM_SHOWWINDOW && ((MSG*)lParam)->wParam == FALSE )
    {
		char title[256];
		GetWindowText(((MSG*)lParam)->hwnd, title, 255);
		if(strstr(title,"Opera") >= 0) 
		{
			PostMessage(FindWindow(NAME, NAME), WM_MYCMD, IDM_TRAY, ((MSG*)lParam)->hwnd);
		}
	}
    return CallNextHookEx(NULL, nCode, wParam, lParam);
};
/*LRESULT CALLBACK
CallWndProc(int hCode,WPARAM wParam,LPARAM lParam)
{
    CWPSTRUCT *pcwps;

    pcwps = (CWPSTRUCT*)lParam;

    if (hCode >= 0 && pcwps && pcwps->hwnd && pcwps->message==WM_SHOWWINDOW)
    {
       PostMessage(FindWindow(NAME, NAME),WM_SHOWWINDOW,0,(WPARAM)(pcwps->hwnd));
       return 0;
    }
    return CallNextHookEx(NULL, hCode, wParam, lParam);
}
*/
BOOL DLLIMPORT
RegisterHook(HMODULE hLib)
{
    hMouse = SetWindowsHookEx(WH_MOUSE,     (HOOKPROC)MouseProc,  hLib, 0);
    hMsg   = SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)GetMsgProc, hLib, 0);
	hKey   = SetWindowsHookEx(WH_KEYBOARD,	(HOOKPROC)KeyboardProc, hLib, 0);
//  hCall  = SetWindowsHookEx(WH_CALLWNDPROC,(HOOKPROC)CallWndProc, hLib, 0);
    if(hMouse==NULL || hMsg==NULL || hKey == NULL)//|| hCall==NULL)
    {
        UnRegisterHook();
        return FALSE;
    }
    return TRUE;
}
void DLLIMPORT
UnRegisterHook(void)
{
    if(hMouse)UnhookWindowsHookEx(hMouse);
    if(hMsg)  UnhookWindowsHookEx(hMsg);
	if(hKey)  UnhookWindowsHookEx(hKey);
//  if(hCall) UnhookWindowsHookEx(hCall);
}
