#define NAME L"RBTrayHook"

#define WM_ADDTRAY  0x0401
#define WM_REMTRAY  0x0402
#define WM_REFRTRAY 0x0403
#define WM_TRAYCMD  0x0404
#define IDM_RESTORE 0x1001
#define IDM_CLOSE   0x1002
#define IDM_EXIT    0x1003
#define IDM_ABOUT   0x1004

#define DLLIMPORT __declspec(dllexport)

BOOL DLLIMPORT RegisterHook(HMODULE);
void DLLIMPORT UnRegisterHook();
