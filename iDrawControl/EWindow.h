#pragma once
#include "WndMessage/wnd_header.h"



extern wstr m_eCodeFileName;    // 从标题中获取到的源码路径
extern wstr m_eCodeType;        // 从标题中获取到的源码类型
extern wstr m_eCodeShowName;    // 显示标题, 通常是 源码文件名 - [程序类型], 未保存的就 * 程序类型
extern wstr m_eCodeTipsTitle;   // 提示里的标题, 一般是 易语言-加密狗版, 或者是设置的程序名


extern SYSBTN sysBtn;           // 系统按钮信息
extern HINSTANCE s_hInst;
extern RECT m_rcTitle;          // 显示标题的地方, 一小块黑色, 鼠标上去应该弹出提示

extern CLayered m_tips;
extern int m_isHover;           // 鼠标是否进入了项目标题中
extern RECT m_tips_rect;        // 提示信息当前的矩形位置, 在调用时钟前设置

void EWindow_OnPaint_DrawTitle(LPCANVAS hCanvas, LPOBJSTRUCT pData, LPCWSTR const titleMid);
void EWindow_OnPaint_DrawEIcon(LPCANVAS hCanvas, LPOBJSTRUCT pData);
void EWindow_DrawTab(LPCANVAS hCanvas, LPOBJSTRUCT pData, PAINTSTRUCT& ps);
void EWindow_TabInit(LPOBJSTRUCT pData);

LRESULT EWindow_TabMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LRESULT EWindow_OnCommand(HWND hWnd, int id, int code, HWND hControl, LPOBJSTRUCT pData);
void EWindow_DrawSystemButton(LPCANVAS hCanvas, LPOBJSTRUCT pData);
LRESULT EWndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT EWndProc_OnDropFile(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LRESULT EWndProc_OnThemeCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
int ThemeMenu_LoadString(int id, LPWSTR lpBuffer, int cchBufferMax);


void ModifySysButtonPos(LPOBJSTRUCT pData);
void AddSysButton(DWORD flag, int ID, LPOBJSTRUCT pData);
void ewnd_createed(HWND hWnd, DWORD style, LPOBJSTRUCT pData);
ICShadow* EWindow_GetShadow();
bool WINAPI OnShadowPaint(LPCANVAS hCanvas, LPEX_IMAGE pImg, const RECT* prc, DWORD crShadow, DWORD argbBorder, int alpha, LPVOID param);
void _draw_ready_title(LPCANVAS hCanvas, ARGB crText, const RECT_F& rc, ARGB crClear = 0);


void _draw_calc_work(RECT& rc);
void _draw_calc_state(RECT& rc);
void _draw_calc_control(RECT& rc);





