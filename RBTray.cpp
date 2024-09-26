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
#include "RBTray.h"
#include "resource.h"

#define MAXTRAYITEMS 64

static UINT WM_TASKBAR_CREATED;

static HINSTANCE _hInstance;
static HMODULE _hLib;
static HWND _hwndHook;
static HWND _hwndItems[MAXTRAYITEMS];
static HWND _hwndForMenu;

int FindInTray(HWND hwnd) {
    for (int i = 0; i < MAXTRAYITEMS; i++) {
        if (_hwndItems[i] == hwnd) {
            return i;
        }
    }
    return -1;
}

HICON GetWindowIcon(HWND hwnd) {
    HICON icon;
    if (icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0)) {
        return icon;
    }
    if (icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0)) {
        return icon;
    }
    if (icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM)) {
        return icon;
    }
    if (icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON)) {
        return icon;
    }
    return LoadIcon(NULL, IDI_WINLOGO);
}

static bool AddToTray(int i) {
    NOTIFYICONDATA nid;
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize           = NOTIFYICONDATA_V2_SIZE;
    nid.hWnd             = _hwndHook;
    nid.uID              = (UINT)i;
    nid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYCMD;
    nid.hIcon            = GetWindowIcon(_hwndItems[i]);
    GetWindowText(_hwndItems[i], nid.szTip, sizeof(nid.szTip) / sizeof(nid.szTip[0]));
    nid.uVersion         = NOTIFYICON_VERSION;
    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        return false;
    }
    if (!Shell_NotifyIcon(NIM_SETVERSION, &nid)) {
        Shell_NotifyIcon(NIM_DELETE, &nid);
        return false;
    }
    return true;
}

static bool AddWindowToTray(HWND hwnd) {
    int i = FindInTray(NULL);
    if (i == -1) {
        return false;
    }
    _hwndItems[i] = hwnd;
    return AddToTray(i);
}

static bool MinimizeWindowToTray(HWND hwnd)
{
    // Don't minimize MDI child windows
    if ((UINT)GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_MDICHILD)
    {
        return false;
    }

    // If hwnd is a child window, find parent window (e.g. minimize button in
    // Office 2007 (ribbon interface) is in a child window)
    if ((UINT)GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD)
    {
        hwnd = GetAncestor(hwnd, GA_ROOT);
    }

    // Add icon to tray if it's not already there
    if (FindInTray(hwnd) == -1)
    {
        if (!AddWindowToTray(hwnd))
        {
            // If there is something wrong with tray icon restore program window.
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);

            return false;
        }
    }

    // Hide window
    return ShowWindow(hwnd, SW_HIDE);
}

static bool RemoveFromTray(int i) {
    NOTIFYICONDATA nid;
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = NOTIFYICONDATA_V2_SIZE;
    nid.hWnd   = _hwndHook;
    nid.uID    = (UINT)i;
    if (!Shell_NotifyIcon(NIM_DELETE, &nid)) {
        return false;
    }
    return true;
}

static bool RemoveWindowFromTray(HWND hwnd) {
    int i = FindInTray(hwnd);
    if (i == -1) {
        return false;
    }
    if (!RemoveFromTray(i)) {
        return false;
    }
    _hwndItems[i] = NULL;
    return true;
}

static void RestoreWindowFromTray(HWND hwnd) {
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    RemoveWindowFromTray(hwnd);
}

static void CloseWindowFromTray(HWND hwnd) {
    // Use PostMessage to avoid blocking if the program brings up a dialog on exit.
    // Also, Explorer windows ignore WM_CLOSE messages from SendMessage.
    PostMessage(hwnd, WM_CLOSE, 0, 0);

    Sleep(50);
    if (IsWindow(hwnd)) {
        Sleep(50);
    }

    if (!IsWindow(hwnd)) {
        // Closed successfully
        RemoveWindowFromTray(hwnd);
    }
}

void RefreshWindowInTray(HWND hwnd) {
    int i = FindInTray(hwnd);
    if (i == -1) {
        return;
    }
    if (!IsWindow(hwnd) || IsWindowVisible(hwnd)) {
        RemoveWindowFromTray(hwnd);
    } else {
        NOTIFYICONDATA nid;
        ZeroMemory(&nid, sizeof(nid));
        nid.cbSize = NOTIFYICONDATA_V2_SIZE;
        nid.hWnd   = _hwndHook;
        nid.uID    = (UINT)i;
        nid.uFlags = NIF_TIP;
        GetWindowText(hwnd, nid.szTip, sizeof(nid.szTip) / sizeof(nid.szTip[0]));
        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }
}

void ExecuteMenu() {
    HMENU hMenu;
    POINT point;

    hMenu = CreatePopupMenu();
    if (!hMenu) {
        MessageBox(NULL, L"Error creating menu.", APP_NAME, MB_OK | MB_ICONERROR);
        return;
    }
    AppendMenu(hMenu, MF_STRING, IDM_ABOUT,   L"About RBTray");
    AppendMenu(hMenu, MF_STRING, IDM_EXIT,    L"Exit RBTray");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_CLOSE,   L"Close Window");
    AppendMenu(hMenu, MF_STRING, IDM_RESTORE, L"Restore Window");

    GetCursorPos(&point);
    SetForegroundWindow(_hwndHook);

    TrackPopupMenu(hMenu, TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RIGHTALIGN | TPM_BOTTOMALIGN, point.x, point.y, 0, _hwndHook, NULL);

    PostMessage(_hwndHook, WM_USER, 0, 0);
    DestroyMenu(hMenu);
}

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
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

static LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_RESTORE:
                    RestoreWindowFromTray(_hwndForMenu);
                    break;
                case IDM_CLOSE:
                    CloseWindowFromTray(_hwndForMenu);
                    break;
                case IDM_ABOUT:
                    DialogBox(_hInstance, MAKEINTRESOURCE(IDD_ABOUT), _hwndHook, (DLGPROC)AboutDlgProc);
                    break;
                case IDM_EXIT:
                    SendMessage(_hwndHook, WM_DESTROY, 0, 0);
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
                    RestoreWindowFromTray(_hwndItems[wParam]);
                    break;
                case WM_CONTEXTMENU:
                    _hwndForMenu = _hwndItems[wParam];
                    ExecuteMenu();
                    break;
                case WM_MOUSEMOVE:
                    RefreshWindowInTray(_hwndItems[wParam]);
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
            for (int i = 0; i < MAXTRAYITEMS; i++) {
                if (_hwndItems[i]) {
                    RestoreWindowFromTray(_hwndItems[i]);
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
                for (int i = 0; i < MAXTRAYITEMS; i++) {
                    if (_hwndItems[i]) {
                        AddToTray(i);
                    }
                }
            }
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool InstanceExists()
{
    return (_hwndHook != NULL);
}

static bool IsAlreadyMinimized(HWND windowHandle)
{
    return (FindInTray(windowHandle) != -1);
}

static int OnCloseExistingInstance()
{
    if (!InstanceExists())
    {
        return -1;
    }

    // If an application processes this message, it should return zero.
    return SendMessage(_hwndHook, WM_CLOSE, 0, 0);
}

static int OnMinimizeWindowByHandle(HWND windowHandle)
{
    if (windowHandle == NULL)
    {
        MessageBox(NULL, L"Invalid window handle!", APP_NAME, MB_OK | MB_ICONERROR);
        return -1;
    }

    if (IsAlreadyMinimized(windowHandle))
    {
        RestoreWindowFromTray(windowHandle);
    }
    else
    {
        if (!MinimizeWindowToTray(windowHandle))
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

    if (!(_hLib = LoadLibrary(L"RBHook.dll")))
    {
        MessageBox(NULL, L"Error loading RBHook.dll.", APP_NAME, MB_OK | MB_ICONERROR);
        return 0;
    }

    if (!RegisterHook(_hLib))
    {
        MessageBox(NULL, L"Error setting hook procedure.", APP_NAME, MB_OK | MB_ICONERROR);
        return 0;
    }

    wc.style = 0;
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
        MessageBox(NULL, L"Error creating window class", APP_NAME, MB_OK | MB_ICONERROR);
        return 0;
    }

    if (!(_hwndHook = CreateWindow(APP_NAME, APP_NAME, WS_OVERLAPPED, 0, 0, 0, 0, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, (LPVOID)NULL))) {
        MessageBox(NULL, L"Error creating window", APP_NAME, MB_OK | MB_ICONERROR);
        return 0;
    }

    for (int i = 0; i < MAXTRAYITEMS; i++)
    {
        _hwndItems[i] = NULL;
    }

    WM_TASKBAR_CREATED = RegisterWindowMessage(L"TaskbarCreated");

    BOOL registeredHotKey = RegisterHotKey(_hwndHook, 0, MOD_WIN | MOD_ALT, VK_DOWN);
    if (!registeredHotKey)
    {
        MessageBox(NULL, L"Couldn't register hotkey", APP_NAME, MB_OK | MB_ICONERROR);
    }

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (registeredHotKey)
    {
        UnregisterHotKey(_hwndHook, 0);
    }

    return (int)msg.wParam;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /* hPrevInstance */, _In_ LPSTR /* szCmdLine */, _In_ int /* iCmdShow */) 
{
    LPWSTR* argList;
    int argCount;

    argList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (argList == NULL)
    {
        MessageBox(NULL, L"Unable to parse command line!", APP_NAME, MB_OK | MB_ICONERROR);
        return -1;
    }

    if (argCount > 3)
    {
        MessageBox(NULL, L"Too many command line arguments!", APP_NAME, MB_OK | MB_ICONERROR);
        return -1;
    }

    _hwndHook = FindWindow(APP_NAME, APP_NAME);

    if (argCount > 1)
    {
        LPWSTR command = argList[1];

        if (AreEqual(command, L"--exit"))
        {
            return OnCloseExistingInstance();
        }
        else if (AreEqual(command, L"--handle"))
        {
            LPWSTR commandArg = argList[2];

            HWND windowHandle = (HWND)wcstol(commandArg, NULL, 0);
            return OnMinimizeWindowByHandle(windowHandle);
        }
        else if (AreEqual(command, L"--class"))
        {
            LPWSTR className = argList[2];

            HWND windowHandle = FindWindow(className, NULL);
            return OnMinimizeWindowByHandle(windowHandle);
        }
        else if (AreEqual(command, L"--title"))
        {
            LPWSTR titleName = argList[2];

            HWND windowHandle = FindWindow(NULL, titleName);
            return OnMinimizeWindowByHandle(windowHandle);
        }
        else
        {
            MessageBox(NULL, L"Unsupported command line arguments!", APP_NAME, MB_OK | MB_ICONERROR);
            return -1;
        }
    }

    if (_hwndHook)
    {
        MessageBox(NULL, L"RBTray is already running.", APP_NAME, MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    return OnRunInBackground(hInstance);
}

