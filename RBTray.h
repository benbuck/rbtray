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

constexpr auto APP_NAME = L"RBTray";
constexpr auto APP_HOOK_DLL_NAME = L"RBHook.dll";
constexpr auto ERROR_MESSAGE_MAX_SIZE = 512;

constexpr auto WM_ADDTRAY = 0x0401;
constexpr auto WM_REMTRAY = 0x0402;
constexpr auto WM_REFRTRAY = 0x0403;
constexpr auto WM_TRAYCMD = 0x0404;
constexpr auto IDM_RESTORE = 0x1001;
constexpr auto IDM_CLOSE = 0x1002;
constexpr auto IDM_EXIT = 0x1003;
constexpr auto IDM_ABOUT = 0x1004;

#define DLLIMPORT __declspec(dllexport)

BOOL DLLIMPORT RegisterHook(HMODULE);
void DLLIMPORT UnRegisterHook();

static inline bool AreEqual(const wchar_t* left, const wchar_t* right) 
{
    if (left == nullptr || right == nullptr) 
    {
        return false; 
    }

    return (wcscmp(left, right) == 0);
}

static void ShowError(const wchar_t* format, ...)
{
    wchar_t message[ERROR_MESSAGE_MAX_SIZE];

    va_list args;
    va_start(args, format);

    vswprintf(message, sizeof(message) / sizeof(wchar_t), format, args); 

    va_end(args);

    MessageBox(NULL, message, APP_NAME, MB_OK | MB_ICONERROR);
}
