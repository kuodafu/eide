#pragma once
#include <IDraw_Header.h>

// ���໯����, �Ѵ��ھ����ŵ�map��, �����ô���Ĭ�ϵĻ滭����, �����Ҫ�Զ�������, ����ͨ������ֵ�����޸�
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
// ���໯�����Դ����м���Ǹ��Ի�ѡ���
LPOBJSTRUCT _subclass_MDIClient(HWND hWnd);
LPOBJSTRUCT _subclass_CustomTab(HWND hWnd);
LPOBJSTRUCT _subclass_Tab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle);
LPOBJSTRUCT _subclass_combobox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight);

LPOBJSTRUCT _subclass_MDIClient_CustomTab(HWND hWnd);

// ���໯��Ҫ����״̬����
// rcOffset = ���ҵ׵�ƫ����
LPOBJSTRUCT _subclass_top_state(HWND hWnd, const RECT& rcOffset);


// ���໯���������
LPOBJSTRUCT _subclass_MDIClient_HScroll(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data, bool reMoveBorder = true);

// ���໯�����������
LPOBJSTRUCT _subclass_MDIClient_VScroll(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data, bool reMoveBorder = true);



// MDIClient�µĴ��ڱ༭��, MDIClient�µĵڶ����Ӵ���, pData�ǵ�һ���Ӵ��ڵ�����
LPOBJSTRUCT _subclass_MDIClient_EWindow(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi);
// MDIClient�µ���ʼҳ
LPOBJSTRUCT _subclass_MDIClient_EHomePage(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi);


// ���໯�˵�������ڵĴ���, ����WM_DRAWITEM��Ϣ, ����Ѿ����໯, �򲻴���
LPOBJSTRUCT _subclass_hMenu_Window(HWND hWnd, pfn_SubClassWndProc proc);
LPOBJSTRUCT _subclass_EWindow(HWND hWnd, HINSTANCE hInst, LPSYSBUTTONIMAGE pNskn);


