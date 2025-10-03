#pragma once
#include <IDraw_Header.h>

// 子类化窗口, 把窗口句柄存放到map里, 并设置窗口默认的绘画数据, 如果需要自定义数据, 请在通过返回值进行修改
LPOBJSTRUCT _subclass(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder = true, bool isSetBack = true, DWORD reMoveClassStyle = CS_HREDRAW | CS_VREDRAW);
std::map<HWND, OBJSTRUCT>& _get_subclass_allWindow();
LRESULT CALLBACK SubClassWindow_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


LPOBJSTRUCT _subclass_Button(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);
LPOBJSTRUCT _subclass_Edit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll = true);
LPOBJSTRUCT _subclass_Static(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);
LPOBJSTRUCT _subclass_statusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);

LPOBJSTRUCT _subclass_tree(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false);
LPOBJSTRUCT _subclass_ListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll = true, bool isHot = false);
LPOBJSTRUCT _subclass_listView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false, int fmt = DT_VCENTER | DT_SINGLELINE);
LPOBJSTRUCT _subclass_tool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);
LPOBJSTRUCT _subclass_msctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);
// 子类化易语言代码中间的那个自绘选择夹
LPOBJSTRUCT _subclass_MDIClient(HWND hWnd);
LPOBJSTRUCT _subclass_CustomTab(HWND hWnd);
LPOBJSTRUCT _subclass_Tab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);
LPOBJSTRUCT _subclass_combobox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight);

LPOBJSTRUCT _subclass_MDIClient_CustomTab(HWND hWnd);

// 子类化需要顶边状态条的
// rcOffset = 左顶右底的偏移量
LPOBJSTRUCT _subclass_top_state(HWND hWnd, const RECT& rcOffset);


// 子类化横向滚动条
LPOBJSTRUCT _subclass_MDIClient_HScroll(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data, bool reMoveBorder = true);

// 子类化纵向向滚动条
LPOBJSTRUCT _subclass_MDIClient_VScroll(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data, bool reMoveBorder = true);



// MDIClient下的窗口编辑器, MDIClient下的第二层子窗口, pData是第一层子窗口的数据
LPOBJSTRUCT _subclass_MDIClient_EWindow(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi);
// MDIClient下的起始页
LPOBJSTRUCT _subclass_MDIClient_EHomePage(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi);


// 子类化菜单句柄所在的窗口, 接收WM_DRAWITEM消息, 如果已经子类化, 则不处理
LPOBJSTRUCT _subclass_hMenu_Window(HWND hWnd, pfn_SubClassWndProc proc);
LPOBJSTRUCT _subclass_EWindow(HWND hWnd, HINSTANCE hInst, LPSYSBUTTONIMAGE pNskn);


