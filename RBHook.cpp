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
#include "rbtray.h"

static HHOOK _hMouse = NULL;
static HHOOK _hWndProc = NULL;
static HWND _hLastHit = NULL;

// Works for 32-bit and 64-bit apps
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		if (wParam == WM_NCRBUTTONDOWN || wParam == WM_NCRBUTTONUP) {
			MOUSEHOOKSTRUCT *info = (MOUSEHOOKSTRUCT*)lParam;
UINT testcode = info->wHitTestCode;			
BOOL isMinHit = testcode == HTMINBUTTON || GetKeyState(VK_SHIFT);
BOOL isCloseHit = testcode == HTCLOSE;
			if (wParam == WM_NCRBUTTONDOWN && (isMinHit || isCloseHit)) {
				_hLastHit = info->hwnd;
				return 1;
			}
			if (wParam == WM_NCRBUTTONUP && (isMinHit || isCloseHit)) {
				if (info->hwnd == _hLastHit) {
					if(isMinHit){
					PostMessage(FindWindow(NAME, NAME), WM_ADDTRAY, 0, (LPARAM)info->hwnd);
				}
					else
					{
						PostMessage(FindWindow(NAME, NAME), WM_FORCECLOSE, 0, (LPARAM)info->hwnd);
					}
				}
				_hLastHit = NULL;
				return 1;
			}
				_hLastHit = NULL;
			}
		else if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) {
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
