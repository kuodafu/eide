#pragma once
#include "WndMessage/wnd_header.h"



extern wstr m_eCodeFileName;    // �ӱ����л�ȡ����Դ��·��
extern wstr m_eCodeType;        // �ӱ����л�ȡ����Դ������
extern wstr m_eCodeShowName;    // ��ʾ����, ͨ���� Դ���ļ��� - [��������], δ����ľ� * ��������
extern wstr m_eCodeTipsTitle;   // ��ʾ��ı���, һ���� ������-���ܹ���, ���������õĳ�����


extern SYSBTN sysBtn;           // ϵͳ��ť��Ϣ
extern HINSTANCE s_hInst;
extern RECT m_rcTitle;          // ��ʾ����ĵط�, һС���ɫ, �����ȥӦ�õ�����ʾ

extern CLayered m_tips;
extern int m_isHover;           // ����Ƿ��������Ŀ������
extern RECT m_tips_rect;        // ��ʾ��Ϣ��ǰ�ľ���λ��, �ڵ���ʱ��ǰ����

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





