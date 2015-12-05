#include <windows.h>
#include "rbtray.h"

#define MAXCOUNT 64
#define MAXTEXT  64
#define WS_WINDOW_STYLE WS_OVERLAPPED


typedef BOOL (*RegHook)(HMODULE hLib);
typedef void (*UnRegHook)(void);

static HWND hwndHook;
static HINSTANCE thisInstance;
static HWND list[MAXCOUNT];
static HWND CurrentWindow;
static HMODULE hLib;
static DWORD lastLButtonDown;
static UINT WM_TASKBAR_CREATED;
static int keyState;
static BOOL showKeyb;
#define keyname   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\RBTray.exe"
#define param "Keyboard Indicator"
BOOL ReadKbdCfg()
{
    HKEY key;
    DWORD typ;
    DWORD value = 0;
    DWORD size = 4;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE ,keyname,0,KEY_READ,&key)!=ERROR_SUCCESS) return FALSE;
    if(RegQueryValueEx(key,param,NULL,(LPDWORD)&typ,(LPBYTE)&value,(LPDWORD)&size)!=ERROR_SUCCESS)
    {
        RegCloseKey(key);
        return FALSE;
    };
    RegCloseKey(key);
    return (value == 1);
}

void SetKbdCfg()
{
    HKEY key;
    DWORD value = 0;
    DWORD size = 4;
    LONG ret = 0;
    if(showKeyb)
        value = 1L;
    else
        value = 0L;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE ,keyname,0,KEY_SET_VALUE,&key)!=ERROR_SUCCESS) return;
    ret = RegSetValueEx(key, param, 0L, REG_DWORD, (LPBYTE)&value, sizeof(DWORD)); 
    RegCloseKey(key);
    return (value == 1);
}


BOOL CALLBACK
UpdMenu(HWND  hwnd,LPARAM lParam)
{
    int i;
    BOOL flag=lParam;
    //    int Checked;
    HMENU hSysMenu=GetSystemMenu(hwnd, FALSE);
    for(i=0;i<GetMenuItemCount(hSysMenu) && hSysMenu;i++)
        if(GetMenuItemID(hSysMenu,i)==IDM_TRAY) hSysMenu = 0;
    if (hSysMenu && lParam)
    {
        InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_SEPARATOR,IDM_SEPARATOR, NULL) ;
        if(GetWindowLong(hwnd,GWL_EXSTYLE)&WS_EX_TOPMOST)
            InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING|MF_CHECKED,IDM_ONTOP,"Always on top");
        else
            InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING,IDM_ONTOP,"Always on top");
        InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING,IDM_TRAY,"Minimize in tray");
        InsertMenu (hSysMenu, GetMenuItemID(hSysMenu,0),MF_STRING,IDM_SIZE,"My size");
    }
    if (hSysMenu && lParam==FALSE)
    {
        DeleteMenu (hSysMenu,IDM_TRAY,MF_BYCOMMAND);
        DeleteMenu (hSysMenu,IDM_ONTOP,MF_BYCOMMAND);
        DeleteMenu (hSysMenu,IDM_SEPARATOR,MF_BYCOMMAND);
        DeleteMenu (hSysMenu,IDM_SIZE,MF_BYCOMMAND);

    }
    return TRUE;
}

void Resize(HWND hwnd)
{
    RECT rc;
    int dx, dy;
    SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0);

    ShowWindow(hwnd, SW_SHOW);
    dx = (rc.right-rc.left)/35;
    dy = (rc.bottom-rc.top)/28;
    //  dx = dy;
    MoveWindow(hwnd, dx, dy, rc.right-rc.left-2*dx, rc.bottom-rc.top-2*dy, TRUE);
}

HICON
GetWindowIcon(HWND hwnd)
{
    HICON icon;
    if (icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0))
        return icon;
    if (icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0))
        return icon;
    if (icon = (HICON)GetClassLong(hwnd, GCL_HICONSM))
        return icon;
    if (icon = (HICON)GetClassLong(hwnd, GCL_HICON))
        return icon;
    return LoadIcon(NULL, IDI_WINLOGO);
}

int
FindInTray(HWND hwnd)
{
    int i;
    for (i = MAXCOUNT - 1; i >= 0; i--)
        if (list[i] == hwnd) break;
    return i;
}

static void
DelFromTray(int i)
{
    NOTIFYICONDATA nid;

    if (i < 0)
        return;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd   = hwndHook;
    nid.uID    = (UINT)list[i];
    list[i] = 0;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

static void
AddToTray(int i)
{
    NOTIFYICONDATA nid;

    nid.cbSize           = sizeof(NOTIFYICONDATA);
    nid.hWnd             = hwndHook;
    nid.uID              = (UINT)list[i];
    nid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYCMD;
    nid.hIcon            = GetWindowIcon(list[i]);
    GetWindowText(list[i], nid.szTip, MAXTEXT);

    Shell_NotifyIcon(NIM_ADD, &nid);
}

static void
ShowIt(HWND hwnd)
{
    ShowWindow(hwnd, SW_SHOW);
    ShowWindow(hwnd, SW_RESTORE);
    SetForegroundWindow(hwnd);
}

static void
ShowTheWindow(HWND hwnd)
{
    if ((GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD))
    {
        HWND parent = hwnd;
        do
        parent = GetParent(parent);
        while (parent && FindInTray(parent) < 0);
        if (parent)
            ShowTheWindow(parent);
    }
    ShowIt(hwnd);
    DelFromTray(FindInTray(hwnd));

}

static void
CloseTheWindow(HWND hwnd)
{
    SetForegroundWindow(hwnd);
    SendMessage(hwnd, WM_CLOSE, 0, 0);
    if (!IsWindow(hwnd)) DelFromTray(FindInTray(hwnd));

}

void ExecuteMenu()
{
    HMENU hMenu;
    POINT point;
    int k;
    hMenu=CreatePopupMenu();
    if(!hMenu)
    {
        MessageBox(NULL, "Error crerating menu", "RBTray",MB_OK);
        return;
    };
    AppendMenu(hMenu,MF_STRING,IDM_ABOUT,   "About RBTray\0");
    AppendMenu(hMenu,MF_SEPARATOR,0,NULL);//------------------
    k = FindInTray(CurrentWindow);
    if(k != -1)
    {
        AppendMenu(hMenu,MF_STRING,IDM_RESTORE, "Restore window\0");
        AppendMenu(hMenu,MF_STRING,IDM_CLOSE,   "Close window\0");
        AppendMenu(hMenu,MF_STRING,IDM_DESTROY, "Kill window\0");
        AppendMenu(hMenu,MF_SEPARATOR,0,NULL);//------------------
    }
    AppendMenu(hMenu,MF_STRING|(showKeyb ? MF_CHECKED : 0),IDM_SHOWKEY, "Keyboard Indicator\0");
    AppendMenu(hMenu,MF_SEPARATOR,0,NULL);//------------------
    AppendMenu(hMenu,MF_STRING,IDM_EXIT,    "Exit RBTray\0");

    GetCursorPos (&point);
    SetForegroundWindow (hwndHook);

    TrackPopupMenu (hMenu, TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RIGHTALIGN | TPM_BOTTOMALIGN,
        point.x, point.y, 0, hwndHook, NULL);
    //					point.x, GetSystemMetrics(SM_CYSCREEN), 0, hwndHook, NULL);

    PostMessage (hwndHook, WM_USER, 0, 0);
    DestroyMenu(hMenu);
}

BOOL CALLBACK
AboutDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
        SetDlgItemText(hWnd,IDC_EDIT,"nredko@gmail.com");
        break;

    case WM_CLOSE:
        PostMessage( hWnd, WM_COMMAND, IDCANCEL, 0l );
        break;

    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case IDOK:
            EndDialog( hWnd, TRUE );
            break;

        case IDCANCEL:
            EndDialog( hWnd, FALSE );
            break;

        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void
DestroyTheWindow(HWND hwnd)
{
    DWORD pID;
    HANDLE hProc;

    GetWindowThreadProcessId(hwnd,&pID);
    hProc=OpenProcess(PROCESS_ALL_ACCESS,TRUE,pID);
    if(TerminateProcess(hProc,1)) DelFromTray(FindInTray(hwnd));
}
/*
void
FixChars()
{
if (CountClipboardFormats() == 0) 
return; 
if (!OpenClipboard(hwnd)) 
return; 

// Add a separator and then a menu item for each format. 

AppendMenu(hmenu, MF_SEPARATOR, 0, NULL); 
uFormat = EnumClipboardFormats(0); 

while (uFormat) 
{ 
// Call an application-defined function to get the name 
// of the clipboard format. 

lpFormatName = GetPredefinedClipboardFormatName(uFormat); 

// For registered formats, get the registered name. 

if (lpFormatName == NULL) 
{

// Note that, if the format name is larger than the
// buffer, it is truncated. 
if (GetClipboardFormatName(uFormat, szFormatName, 
sizeof(szFormatName))) 
lpFormatName = szFormatName; 
else 
lpFormatName = "(unknown)"; 
} 

// Add a menu item for the format. For displayable 
// formats, use the format ID for the menu ID. 

if (IsDisplayableFormat(uFormat)) 
{ 
fuFlags = MF_STRING; 
idMenuItem = uFormat; 
} 
else 
{ 
fuFlags = MF_STRING | MF_GRAYED; 
idMenuItem = 0; 
} 
AppendMenu(hmenu, fuFlags, idMenuItem, lpFormatName); 

uFormat = EnumClipboardFormats(uFormat); 
} 
CloseClipboard(); 

}
*/

void ShowKeyState()
{
    short caps, num, scroll;
    int newKeyState;
    NOTIFYICONDATA nid;
    if(!showKeyb) {
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd   = hwndHook;
        nid.uID    = 7777;
        Shell_NotifyIcon(NIM_DELETE, &nid);
        return;
    }
    caps = GetKeyState(VK_CAPITAL) & 1;
    scroll = GetKeyState(VK_SCROLL) & 1;
    num = GetKeyState(VK_NUMLOCK) & 1;
    newKeyState = scroll | (caps << 1) | (num << 2);
    if(newKeyState >= 0 && keyState != newKeyState)
    {
        nid.cbSize           = sizeof(NOTIFYICONDATA);
        nid.hWnd             = hwndHook;
        nid.uID              = 7777;
        nid.uCallbackMessage = WM_TRAYCMD;
        nid.uFlags           = NIF_ICON|NIF_MESSAGE;
        nid.hIcon            = LoadIcon(thisInstance, MAKEINTRESOURCE(200 + newKeyState));
        if(keyState == -1)
            Shell_NotifyIcon(NIM_ADD, &nid);    
        else
            Shell_NotifyIcon(NIM_MODIFY, &nid);    
        keyState = newKeyState;
    }
}
LRESULT CALLBACK
HookWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NOTIFYICONDATA nid;
    int i;
    char buff[255];
    //  DWORD ProcessId;
    HMENU hSysMenu;
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_RESTORE:
            ShowTheWindow(CurrentWindow);
            break;
        case IDM_EXIT:
            SendMessage(hwndHook,WM_DESTROY,0,0);
            break;
        case IDM_CLOSE:
            CloseTheWindow(CurrentWindow);
            break;
        case IDM_ABOUT:
            DialogBox(thisInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwndHook, (DLGPROC)AboutDlgProc );
            break;
        case IDM_DESTROY:
            if(MessageBox(NULL,"Kill the window?","RBTray",MB_YESNO | MB_ICONWARNING )==IDYES)DestroyTheWindow(CurrentWindow);
            break;
        case IDM_SHOWKEY:
            showKeyb = !showKeyb;
            keyState = -1;
            SetKbdCfg(showKeyb);
            ShowKeyState();
            break;
        };
        break;
    case WM_MYCMD:
        switch (wParam)
        {
            /*
        case IDM_LOTUSEDIT:
            buff[0] = '\0';
            GetWindowText((HWND)lParam, buff, 10);
            //buff[250] = '\0';
            MessageBox(NULL,buff,"code",MB_OK);
            break;
            */
        case IDM_SIZE:
            Resize((HWND)lParam);
            break;
        case IDM_TRAY:
            // prohibit hiding of explorer's taskbar			
            GetClassName((HWND)lParam, buff, 255);
            if(stricmp("Shell_TrayWnd", buff) == 0) break;
            if(stricmp("DV2ControlHost", buff) == 0) break;
            //MessageBox(NULL, buff, "RBTray", MB_OK);
            for(i = 0 ; i < MAXCOUNT ;i++) { 
                if(!list[i]) break;
            }
            if (i == MAXCOUNT) break;

            list[i]=(HWND)lParam;
            ShowWindow((HWND)lParam, SW_HIDE);
            ShowWindow((HWND)lParam, SW_MINIMIZE);
            AddToTray(i);
            ShowWindow((HWND)lParam, SW_HIDE);
            ShowWindow(GetParent((HWND)lParam), SW_HIDE);
            UpdMenu((HWND)lParam,(LPARAM)TRUE);
            break;
            /*
            case IDM_KEY:
            // Fix chars typed in incorrect keyboard layout
            FixChars();
            break;
            */
        case IDM_KEYSTAT:
            ShowKeyState();
            break;
        case IDM_ONTOP:
            hSysMenu=GetSystemMenu((HWND)lParam, FALSE);
            if(hSysMenu)
            {
                if(GetMenuState(hSysMenu, IDM_ONTOP,MF_BYCOMMAND) & MF_CHECKED)
                    //GetWindowLong((HWND)lParam,GWL_EXSTYLE)&WS_EX_TOPMOST)
                {
                    CheckMenuItem(hSysMenu,IDM_ONTOP,MF_BYCOMMAND |MF_UNCHECKED);
                    SetWindowPos((HWND)lParam,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW);
                }
                else
                {
                    CheckMenuItem(hSysMenu,IDM_ONTOP,MF_BYCOMMAND |MF_CHECKED);
                    SetWindowPos((HWND)lParam,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW);
                }
            }
            break;
        }
    case WM_DELTRAY:
        CloseTheWindow((HWND)wParam);
        break;
    case WM_TRAYCMD:
        switch ((UINT)lParam) {
    case WM_LBUTTONDOWN:
        // Hmm, there's probably a better way to do this... SetCapture maybe?
        // But I don't feel like messing with it :)
        lastLButtonDown = GetTickCount();
        break;
    case WM_LBUTTONUP:
        if ((GetTickCount() - lastLButtonDown) <= 1000) {
            ShowTheWindow((HWND)wParam);
        }
        break;
    case WM_RBUTTONUP:
        CurrentWindow=(HWND)wParam;
        ExecuteMenu();
        break;
    case WM_MOUSEMOVE:
        if (!IsWindow((HWND)wParam))
            DelFromTray(FindInTray((HWND)wParam));
        else {
            nid.uID              = (UINT)wParam;
            nid.uFlags           = NIF_ICON | NIF_TIP;
            nid.hIcon            = GetWindowIcon((HWND)wParam);
            GetWindowText((HWND)wParam, nid.szTip, MAXTEXT);
            Shell_NotifyIcon(NIM_MODIFY, &nid);
        }
        }
        break;
    case WM_DESTROY:
        for (i = 0; i < MAXCOUNT; i++)
            if (list[i])
            {
                ShowIt(list[i]);
                DelFromTray(i);
            }
            showKeyb = FALSE;
            ShowKeyState();
            UnRegisterHook();
            FreeLibrary(hLib);
            EnumWindows((WNDENUMPROC)UpdMenu,FALSE);
            PostQuitMessage(0);
            ExitProcess(0);
            break;
    }


    // I can't put this in the switch since WM_TASKBAR_CREATED isn't constant
    if (msg == WM_TASKBAR_CREATED) {
        for (i = 0; i < MAXCOUNT; i++) {
            if (list[i]) {
                AddToTray(i);
            }
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR szCmdLine, int iCmdShow)
{
    WNDCLASS wc;
    MSG msg;
    int i;

    lastLButtonDown = 0;
    thisInstance = hInstance;
    hwndHook = FindWindow(NAME, NAME);
    if (strstr(szCmdLine,"--exit"))
    {
        if (hwndHook)SendMessage(hwndHook, WM_CLOSE, 0, 0);
        return 0;
    };
    showKeyb = ReadKbdCfg();

    if (!(hLib = LoadLibrary("RBHook.dll")))
    {
        MessageBox(NULL, "Error loading RBHook.dll", "RBTray", MB_OK
            | MB_ICONHAND);
        return FALSE;
    }
    if (!RegisterHook(hLib))
    {
        MessageBox(NULL, "Error setting hook procedure", "RBTray", MB_OK | MB_ICONHAND);
        return FALSE;
    }
    wc.hCursor        = NULL;
    wc.hIcon          = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = NAME;
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hInstance      = hInstance;
    wc.style          = 0;
    wc.lpfnWndProc    = HookWndProc;
    wc.cbWndExtra     = sizeof(HWND) + sizeof(HWND);
    wc.cbClsExtra     = 0;
    if (!RegisterClass(&wc))
    {
        MessageBox(NULL,"Error creating window class", "RBTray", MB_OK | MB_ICONHAND);
        return 2;
    }
    if (!(hwndHook = CreateWindow(NAME, NAME,
        WS_WINDOW_STYLE,
        0, 0, 0, 0,
        (HWND) NULL,
        (HMENU) NULL,
        (HINSTANCE)hInstance,
        (LPVOID) NULL)))
    {
        MessageBox(NULL, "Error creating window", "RBTray", MB_OK);
        return 3;
    }
    WM_TASKBAR_CREATED = RegisterWindowMessage("TaskbarCreated");
    for(i = 0 ; i < MAXCOUNT ;list[i++] = 0);
    keyState = -1;
    ShowKeyState();
    EnumWindows((WNDENUMPROC)UpdMenu,TRUE);
    while (IsWindow(hwndHook) && GetMessage(&msg, hwndHook, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
