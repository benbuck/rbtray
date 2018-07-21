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
#include <stdio.h>
#include "RBTray.h"

static HHOOK _hMouse = NULL;
static HHOOK _hWndProc = NULL;
static HWND _hLastHit = NULL;


//#define DEBUG_PRINTF(fmt, ...) do { char buf[1024]; snprintf(buf, sizeof(buf), fmt, ##__VA_ARGS__); OutputDebugStringA(buf); } while (0)

// Works for 32-bit and 64-bit apps
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        //if ((wParam != WM_MOUSEMOVE) && (wParam != WM_NCMOUSEMOVE)) DEBUG_PRINTF("%s(%d): nCode %d, wParam %llx, lParam %llx\n", __FUNCTION__, __LINE__, nCode, (long long int)wParam, (long long int)lParam);
        MOUSEHOOKSTRUCT *info = (MOUSEHOOKSTRUCT*)lParam;
        //if ((wParam != WM_MOUSEMOVE) && (wParam != WM_NCMOUSEMOVE)) DEBUG_PRINTF("%s(%d): pt (%ld, %ld), hwnd %p, wHitTestCode %u, dwExtraInfo %llx\n", __FUNCTION__, __LINE__, info->pt.x, info->pt.y, info->hwnd, info->wHitTestCode, info->dwExtraInfo);
        if ((wParam == WM_NCRBUTTONDOWN) || (wParam == WM_NCRBUTTONUP)) {
            //DEBUG_PRINTF("%s(%d): button\n", __FUNCTION__, __LINE__);
            if (info->wHitTestCode == HTCLIENT) {
                //DEBUG_PRINTF("%s(%d): ignoring client hit test code for non-client message\n", __FUNCTION__, __LINE__);
            } else {
                BOOL shiftKeyDown = (GetKeyState(VK_SHIFT) & 0x8000) ? TRUE : FALSE;
                BOOL isHit = (info->wHitTestCode == HTMINBUTTON) || ((info->wHitTestCode == HTCAPTION) && shiftKeyDown);
                //DEBUG_PRINTF("%s(%d): shift %s, hit %s\n", __FUNCTION__, __LINE__, shiftKeyDown ? "yes" : "no", isHit ? "yes" : "no");
                if ((wParam == WM_NCRBUTTONDOWN) && isHit) {
                    //DEBUG_PRINTF("%s(%d): down hit\n", __FUNCTION__, __LINE__);
                    _hLastHit = info->hwnd;
                    return 1;
                }
                else if ((wParam == WM_NCRBUTTONUP) && isHit) {
                    //DEBUG_PRINTF("%s(%d): up hit\n", __FUNCTION__, __LINE__);
                    if (info->hwnd == _hLastHit) {
                        //DEBUG_PRINTF("%s(%d): up hit match\n", __FUNCTION__, __LINE__);
                        PostMessage(FindWindow(NAME, NAME), WM_ADDTRAY, 0, (LPARAM)info->hwnd);
                    }
                    _hLastHit = NULL;
                    return 1;
                }
                else {
                    //DEBUG_PRINTF("%s(%d): miss\n", __FUNCTION__, __LINE__);
                    _hLastHit = NULL;
                }
            }
        }
        else if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) {
            //DEBUG_PRINTF("%s(%d): not button\n", __FUNCTION__, __LINE__);
            _hLastHit = NULL;
        }
    }
    return CallNextHookEx(_hMouse, nCode, wParam, lParam);
}

// Only works for 32-bit apps or 64-bit apps depending on whether this is complied
// as 32-bit or 64-bit
LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        CWPRETSTRUCT *msg = (CWPRETSTRUCT*)lParam;
        if ((msg->message == WM_WINDOWPOSCHANGED &&
             (((WINDOWPOS*)msg->lParam)->flags & SWP_SHOWWINDOW) != 0) ||
            (msg->message == WM_NCDESTROY))
        {
            PostMessage(FindWindow(NAME, NAME), WM_REFRTRAY, 0, (LPARAM)msg->hwnd);
        }
    }
    return CallNextHookEx(_hWndProc, nCode, wParam, lParam);
}

BOOL DLLIMPORT RegisterHook(HMODULE hLib) {
    _hMouse = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, hLib, 0);
    _hWndProc = SetWindowsHookEx(WH_CALLWNDPROCRET, (HOOKPROC)CallWndRetProc, hLib, 0);
    if (_hMouse == NULL || _hWndProc == NULL) {
        UnRegisterHook();
        return FALSE;
    }
    return TRUE;
}

void DLLIMPORT UnRegisterHook() {
    if (_hMouse) {
        UnhookWindowsHookEx(_hMouse);
        _hMouse = NULL;
    }
    if (_hWndProc) {
        UnhookWindowsHookEx(_hWndProc);
        _hWndProc = NULL;
    }
}
