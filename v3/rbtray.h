//resources
#define CM_CLOSE                        101
#define CM_EXIT                         102
#define CM_HIDE                         103
#define IDD_DIALOG1                     1001
#define ID_OK                           1002
#define IDC_EDIT                        1003

#if !RC_INVOKED

#define NAME "RBTrayHook"

//commands
#define WM_MYCMD     0x0401
#define WM_TRAYCMD   0x0402
#define WM_QUERYTRAY 0x0403
#define WM_DELTRAY   0x0404
#define WM_DBG		 0x0405
#define IDM_CLOSE       0x1001
#define IDM_EXIT        0x1002
#define IDM_RESTORE     0x1003
#define IDM_ALWAYS_TRAY 0x1004
#define IDM_ABOUT       0x1005
#define IDM_DESTROY     0x1006
#define IDM_TRAY        0x1007
#define IDM_HIDE        0x1008
#define IDM_ONTOP       0x1009
#define IDM_SEPARATOR   0x1010
#define IDM_SIZE		0x1011
#define IDM_LOTUSEDIT   0x1012
#define IDM_KEY			0x1013
#define IDM_KEYSTAT     0x1014
#define IDM_SHOWKEY     0x1015
#define DLLIMPORT __declspec(dllexport)

BOOL DLLIMPORT RegisterHook(HMODULE) ;
void DLLIMPORT UnRegisterHook(void);
void DLLIMPORT SetProcessId(DWORD);
//typedef struct tagMOUSEHOOKSTRUCT {
//    POINT   pt;
//    HWND    hwnd;
//    UINT    wHitTestCode;
//    DWORD   dwExtraInfo;
//} MOUSEHOOKSTRUCT, FAR *LPMOUSEHOOKSTRUCT, *PMOUSEHOOKSTRUCT;
#endif
