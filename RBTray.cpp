// ****************************************************************************
//
// RBTray
// Copyright (C) 1998-2010  Nikolay Redko, J.D. Purcell
// Copyright (C) 2015 Benbuck Nason
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// ****************************************************************************

#include <windows.h>
#include <algorithm>
#include "RBTray.h"
#include "resource.h"

constexpr auto MAX_TRAY_ITEMS = 64;

static UINT WM_TASKBAR_CREATED;

static HINSTANCE _hInstance;
static HMODULE _hLib;
static HWND _hWndHook;
static HWND _hWndItems[MAX_TRAY_ITEMS];
static HWND _hWndForMenu;

static HICON GetWindowIcon(HWND hWnd)
{
    HICON icon = nullptr;

    if ((icon = reinterpret_cast<HICON>(SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0))) != nullptr)
    {
        return icon;
    }
    else if ((icon = reinterpret_cast<HICON>(SendMessage(hWnd, WM_GETICON, ICON_BIG, 0))) != nullptr)
    {
        return icon;
    }
    else if ((icon = reinterpret_cast<HICON>(GetClassLongPtr(hWnd, GCLP_HICONSM))) != nullptr)
    {
        return icon;
    }
    else if ((icon = reinterpret_cast<HICON>(GetClassLongPtr(hWnd, GCLP_HICON))) != nullptr)
    {
        return icon;
    }

    return LoadIcon(NULL, IDI_WINLOGO);
}

static int FindWindowInTray(HWND hWnd)
{
    for (int i = 0; i < MAX_TRAY_ITEMS; i++)
    {
        if (_hWndItems[i] == hWnd)
        {
            return i;
        }
    }

    return -1;
}

static bool AddWindowToTray(int i)
{
    NOTIFYICONDATA nid = { sizeof(nid) };

    nid.cbSize = sizeof(nid);
    nid.hWnd = _hWndHook;
    nid.uID = (UINT)i;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYCMD;

    nid.hIcon = GetWindowIcon(_hWndItems[i]);
    if (!nid.hIcon)
    {
        return false;
    }

    GetWindowText(_hWndItems[i], nid.szTip, sizeof(nid.szTip) / sizeof(nid.szTip[0]));
    
    if (!Shell_NotifyIcon(NIM_ADD, &nid))
    {
        return false;
    }

    if (!Shell_NotifyIcon(NIM_SETVERSION, &nid))
    {
        Shell_NotifyIcon(NIM_DELETE, &nid);
        return false;
    }

    return true;
}

static bool AddWindowToTray(HWND hWnd) 
{
    int i = FindWindowInTray(NULL);
    if (i == -1) 
    {
        return false;
    }

    _hWndItems[i] = hWnd;

    return AddWindowToTray(i);
}

static bool RemoveWindowFromTray(int i) 
{
    NOTIFYICONDATA nid = { };
    
    nid.cbSize = sizeof(nid);
    nid.hWnd   = _hWndHook;
    nid.uID    = (UINT)i;
    
    if (!Shell_NotifyIcon(NIM_DELETE, &nid)) 
    {
        return false;
    }

    return true;
}

static bool RemoveWindowFromTray(HWND hWnd) 
{
    int i = FindWindowInTray(hWnd);
    if (i == -1) 
    {
        return false;
    }

    if (!RemoveWindowFromTray(i)) 
    {
        return false;
    }

    _hWndItems[i] = NULL;

    return true;
}

static bool MinimizeWindowToTray(HWND hWnd)
{
    // Don't minimize MDI child windows
    if ((UINT)GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_MDICHILD)
    {
        return false;
    }

    // If hWnd is a child window, find parent window (e.g. minimize button in
    // Office 2007 (ribbon interface) is in a child window)
    if ((UINT)GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CHILD)
    {
        hWnd = GetAncestor(hWnd, GA_ROOT);
    }

    // Hide window before AddWindowToTray call because sometimes RefreshWindowInTray
    // can be called from inside ShowWindow before program window is actually hidden
    // and as a result RemoveWindowFromTray is called which immediately removes just
    // added tray icon.
    ShowWindow(hWnd, SW_HIDE);

    // Add icon to tray if it's not already there
    if (FindWindowInTray(hWnd) == -1)
    {
        if (!AddWindowToTray(hWnd))
        {
            // If there is something wrong with tray icon restore program window.
            ShowWindow(hWnd, SW_SHOW);
            SetForegroundWindow(hWnd);

            return false;
        }
    }

    // Hide window
    return ShowWindow(hWnd, SW_HIDE);
}

static void RestoreWindowFromTray(HWND hWnd) {
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    RemoveWindowFromTray(hWnd);
}

static void CloseWindowFromTray(HWND hWnd) {
    // Use PostMessage to avoid blocking if the program brings up a dialog on exit.
    // Also, Explorer windows ignore WM_CLOSE messages from SendMessage.
    PostMessage(hWnd, WM_CLOSE, 0, 0);

    Sleep(50);
    if (IsWindow(hWnd)) {
        Sleep(50);
    }

    if (!IsWindow(hWnd)) {
        // Closed successfully
        RemoveWindowFromTray(hWnd);
    }
}

static void RefreshWindowInTray(HWND hWnd) {
    int i = FindWindowInTray(hWnd);
    if (i == -1) {
        return;
    }
    if (!IsWindow(hWnd) || IsWindowVisible(hWnd)) {
        RemoveWindowFromTray(hWnd);
    } else {
        NOTIFYICONDATA nid;
        ZeroMemory(&nid, sizeof(nid));
        nid.cbSize = NOTIFYICONDATA_V2_SIZE;
        nid.hWnd   = _hWndHook;
        nid.uID    = (UINT)i;
        nid.uFlags = NIF_TIP;
        GetWindowText(hWnd, nid.szTip, sizeof(nid.szTip) / sizeof(nid.szTip[0]));
        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }
}

static bool IsWindowAlreadyMinimized(HWND hWnd)
{
    return (FindWindowInTray(hWnd) != -1);
}

static void ExecuteMenu() {
    HMENU hMenu;
    POINT point;

    hMenu = CreatePopupMenu();
    if (!hMenu) {
        MessageBox(NULL, L"Error creating menu.", APP_NAME, MB_OK | MB_ICONERROR);
        return;
    }
    AppendMenu(hMenu, MF_STRING, IDM_ABOUT,   L"About");
    AppendMenu(hMenu, MF_STRING, IDM_EXIT,    L"Exit");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_CLOSE,   L"Close Window");
    AppendMenu(hMenu, MF_STRING, IDM_RESTORE, L"Restore Window");

    GetCursorPos(&point);
    SetForegroundWindow(_hWndHook);

    TrackPopupMenu(hMenu, TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RIGHTALIGN | TPM_BOTTOMALIGN, point.x, point.y, 0, _hWndHook, NULL);

    PostMessage(_hWndHook, WM_USER, 0, 0);
    DestroyMenu(hMenu);
}

static BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
        case WM_CLOSE:
            PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hWnd, TRUE);
                    break;
                case IDCANCEL:
                    EndDialog(hWnd, FALSE);
                    break;
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_RESTORE:
                    RestoreWindowFromTray(_hWndForMenu);
                    break;
                case IDM_CLOSE:
                    CloseWindowFromTray(_hWndForMenu);
                    break;
                case IDM_ABOUT:
                    DialogBox(_hInstance, MAKEINTRESOURCE(IDD_ABOUT), _hWndHook, (DLGPROC)AboutDlgProc);
                    break;
                case IDM_EXIT:
                    SendMessage(_hWndHook, WM_DESTROY, 0, 0);
                    break;
            }
            break;
        case WM_ADDTRAY:
            MinimizeWindowToTray((HWND)lParam);
            break;
        case WM_REMTRAY:
            RestoreWindowFromTray((HWND)lParam);
            break;
        case WM_REFRTRAY:
            RefreshWindowInTray((HWND)lParam);
            break;
        case WM_TRAYCMD:
            switch ((UINT)lParam) {
                case NIN_SELECT:
                    RestoreWindowFromTray(_hWndItems[wParam]);
                    break;
                case WM_CONTEXTMENU:
                    _hWndForMenu = _hWndItems[wParam];
                    ExecuteMenu();
                    break;
                case WM_MOUSEMOVE:
                    RefreshWindowInTray(_hWndItems[wParam]);
                    break;
            }
            break;
        case WM_HOTKEY:
        {
            HWND fgWnd = GetForegroundWindow();
            if (!fgWnd)
                break;

            LONG style = GetWindowLong(fgWnd, GWL_STYLE);
            if (!(style & WS_MINIMIZEBOX)) {
                // skip, no minimize box
                break;
            }

            MinimizeWindowToTray(fgWnd);

            break;
        }
        case WM_DESTROY:
            for (int i = 0; i < MAX_TRAY_ITEMS; i++) {
                if (_hWndItems[i]) {
                    RestoreWindowFromTray(_hWndItems[i]);
                }
            }
            if (_hLib) {
                UnRegisterHook();
                FreeLibrary(_hLib);
            }
            PostQuitMessage(0);
            break;
        default:
            if (msg == WM_TASKBAR_CREATED) {
                for (int i = 0; i < MAX_TRAY_ITEMS; i++) {
                    if (_hWndItems[i]) {
                        AddWindowToTray(i);
                    }
                }
            }
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

static bool InstanceExists()
{
    return (_hWndHook != NULL);
}

static int OnCloseExistingInstance()
{
    if (!InstanceExists())
    {
        return -1;
    }

    // If an application processes this message, it should return zero.
    return SendMessage(_hWndHook, WM_CLOSE, 0, 0);
}

static int OnMinimizeWindowByHandle(HWND hWnd)
{
    if (hWnd == NULL)
    {
        MessageBox(NULL, L"Invalid window handle!", APP_NAME, MB_OK | MB_ICONERROR);
        return -1;
    }

    if (IsWindowAlreadyMinimized(hWnd))
    {
        RestoreWindowFromTray(hWnd);
    }
    else
    {
        if (!MinimizeWindowToTray(hWnd))
        {
            MessageBox(NULL, L"Can't minimize window!", APP_NAME, MB_OK | MB_ICONERROR);
            return -2;
        }
    }

    return 0;
}

static int OnRunInBackground(HINSTANCE hInstance)
{
    WNDCLASS wc {};
    MSG msg;

    _hInstance = hInstance;

    if (!(_hLib = LoadLibrary(APP_HOOK_DLL_NAME)))
    {
        ShowError(L"Error loading %s.", APP_HOOK_DLL_NAME);
        return 0;
    }

    if (!RegisterHook(_hLib))
    {
        ShowError(L"Error setting hook procedure!");
        return 0;
    }

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = HookWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = APP_NAME;

    if (!RegisterClass(&wc))
    {
        ShowError(L"Error creating window class!");
        return 0;
    }

    if (!(_hWndHook = CreateWindow(APP_NAME, APP_NAME, WS_OVERLAPPED, 0, 0, 0, 0, nullptr, nullptr, (HINSTANCE)hInstance, nullptr)))
    {
        ShowError(L"Error creating window!");
        return 0;
    }

    std::fill(std::begin(_hWndItems), std::end(_hWndItems), nullptr);

    for (int i = 0; i < MAX_TRAY_ITEMS; i++)
    {
        _hWndItems[i] = NULL;
    }

    WM_TASKBAR_CREATED = RegisterWindowMessage(L"TaskbarCreated");

    BOOL registeredHotKey = RegisterHotKey(_hWndHook, 0, MOD_WIN | MOD_ALT, VK_DOWN);
    if (!registeredHotKey)
    {
        ShowError(L"Couldn't register hotkey!");
    }

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (registeredHotKey)
    {
        UnregisterHotKey(_hWndHook, 0);
    }

    return static_cast<int>(msg.wParam);
}

static bool HandleCommand(LPWSTR command, LPWSTR commandArg) 
{
    if (AreEqual(command, L"--exit")) 
    {
        return OnCloseExistingInstance() == 0;
    }
    else if (AreEqual(command, L"--handle")) 
    {
        HWND hWnd = (HWND)wcstol(commandArg, NULL, 0);
        return OnMinimizeWindowByHandle(hWnd) == 0;
    }
    else if (AreEqual(command, L"--class")) 
    {
        HWND hWnd = FindWindow(commandArg, NULL);
        return OnMinimizeWindowByHandle(hWnd) == 0;
    }
    else if (AreEqual(command, L"--title")) 
    {
        HWND hWnd = FindWindow(NULL, commandArg);
        return OnMinimizeWindowByHandle(hWnd) == 0;
    }
    else
    {
        ShowError(L"Unsupported command line arguments!");
    }

    return false;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /* hPrevInstance */, _In_ LPSTR /* szCmdLine */, _In_ int /* iCmdShow */) 
{
    LPWSTR* argList;
    int argCount;

    argList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (argList == NULL)
    {
        ShowError(L"Unable to parse command line!");
        return -1;
    }

    if (argCount > 3)
    {
        ShowError(L"Too many command line arguments!");
        return -1;
    }

    _hWndHook = FindWindow(APP_NAME, APP_NAME);

    if (argCount > 1)
    {
        LPWSTR command = argList[1];
        LPWSTR commandArg = argCount > 2 ? argList[2] : nullptr;

        if (!HandleCommand(command, commandArg)) {
            return -1;
        }
    }

    if (_hWndHook)
    {
        ShowError(L"%s is already running.", APP_NAME);
        return 0;
    }

    return OnRunInBackground(hInstance);
}

