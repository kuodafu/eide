#pragma once

#include "pch.h"
#include <map>
#include <vector>
#include <control/WndBaseTypedef.h>
#include <IDraw_Header.h>
#include <hook_detours/apiHook.h>
#include <control/CMenu.h>


// ������Ҫ�������߹ر������ռ�
#if 1
#define __EWINDOW_TMPNAME EWindow_Size
#define _EWINDOW_NAMESPACE __EWINDOW_TMPNAME :: 
#define _EWINDOW_BEGIN_NAMESPACE    namespace __EWINDOW_TMPNAME{
#define _EWINDOW_END_NAMESPACE      }
#else
#define _EWINDOW_NAMESPACE
#define _EWINDOW_BEGIN_NAMESPACE 
#define _EWINDOW_END_NAMESPACE
#endif

#define WINDOW_CONTROL_WIDTH    135   // �������
#define Component_Margin 3    // ���Ԥ���߾�

#define IDMENU_FLOAT            1000    // ����(&F)
#define IDMENU_STOP             1001    // ͣ��(&K)
#define IDMENU_AUTOHIDE         1002    // �Զ�����(&T)
#define IDMENU_CLOSE            1003    // �ر�(&C)

_EWINDOW_BEGIN_NAMESPACE


struct EWINDOW_SIZE_MOVEINFO 
{
    HWND hChild1;       // ���ƶ������ʱ��ȡ, �����MDI�µĵ�һ�㴰��
    HWND hChild2;       // ���ƶ������ʱ��ȡ, �����MDI�µĵڶ��㴰��
    HWND hChild3;       // ���ƶ������ʱ��ȡ, �����MDI�µĵ����㴰��, ������ھ�����MDI��ʾʲô����
    HWND hChild4;       // ֻ����ʼҳ������������ڲŻ���ֵ
    int windowType;     // ��ǰMDI�´��ڵ�����, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ
    int code_type;      // ��ǰ������ʲô����, ����, ȫ�ֱ���, ��, ��������....., �����л�ѡ��б��л���ʱ��ֵ

    int heightCombobox; // ��Ͽ�߶�

    HMENU hMenu;                // ���ǰ�ť�Ĳ˵�
    LPTOPSTATE_INFO pMenuClick; // ��ǰ���ĸ����ڵ����˲˵�, ѡ�в˵��п�����Ҫ�޸�����ṹ���ػ�
};

extern EWINDOW_SIZE_MOVEINFO g_moveInfo;

void EWindow_Size_InitHook();

// �����������ڳߴ类�ı�
LRESULT EWindow_Size_OnSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// ��߹����гߴ类�ı�, ��Ҫ�޸Ĺ�����������λ��
LRESULT EWindow_Size_OnWorkSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// ���߲˵��������ߴ类�ı�, ��Ҫ�޸Ĳ˵���������������
LRESULT EWindow_Size_OnMenuSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// �м������MDI���ڱ��ı�, ������Ҫ�޸Ĳ��ٶ���
LRESULT EWindow_Size_OnMDIClientSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// �ױߵ�״̬������, ��Ҫ�Ǳ�������Ҫ�޸�, ��������һ������
LRESULT EWindow_Size_OnStateSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);
LRESULT EWindow_Size_OnStateSize_lr(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// �����, ��Ҫ����MDIClient������ʾ������������λ��
LRESULT EWindow_Size_OnControlSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// �����е�ѡ��б��ı�
LRESULT CALLBACK WndProc_OnWorkTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage);

// ״̬�е�ѡ��б��ı�
LRESULT CALLBACK WndProc_OnStateTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage);

// �Ի�ѡ��б��ı�
LRESULT CALLBACK WndProc_OnCustomTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage);



// ״̬�е�ѡ��б��ı�
LRESULT EWindow_Size_OnCustomTab(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

wstr Window_Get_Title(HWND hWnd);


// �ƶ� MDI, �Ի�ѡ���, ������Ͽ�
// �⼸�����һ�㶼�Ƕ�һ����ȫ�����ö�
// ���ƶ�MDI, ���ƶ��Ի�ѡ���, ����ƶ���Ͽ�, ����������Ǹ���MDI���㴰��λ��
void EWindow_MoveMDI_Tab_Cbx(HDWP& hDwp);


void EWindow_MoveCustomTab(HDWP& hDwp);
void EWindow_MoveMDIClient(HDWP& hDwp);

// �ƶ������
void EWindow_MoveControl(HDWP& hDwp);


// �ƶ����Ͻǲ˵�
void EWindow_MoveMenu(HDWP& hDwp);
// �ƶ����߲˵�������
void EWindow_MoveMenuWindow(HDWP& hDwp);

// �˵����Ͳ˵����������������ı�, �⼸��������ǹ̶�����λ��
void EWindow_Menu_SetPos(HWND hWnd, LPWINDOWPOS pos);

// �ƶ��ױߵ�״̬��
void EWindow_MoveStatus(HDWP& hDwp);

// �ƶ��ײ���״̬��
void EWindow_MoveState(HDWP& hDwp);

// �ƶ���߹�����
void EWindow_MoveWork(HDWP& hDwp);

// �ƶ�������Ͽ�
void EWindow_MoveConmboBox(HDWP& hDwp);

// �жϵ�ǰѡ�е��Ƿ��Ǵ��������, ����0 = ������, 1=���������, 2=��ʼҳ, -1=MDIû������
int EWindow_IsSelWindow();
// ��ȡָ�������µ��Ӵ���, ���������Ϊ0, ���ȡMDI�µĵ�һ�㴰��
// ����0 = ������, 1=���������, 2=��ʼҳ, -1=MDIû������
int EWindow_GetMDIClientChild(HWND hChild = 0);
// �ƶ�MDI�µ��Ӵ���
void EWindow_MoveMDIClientChild(HDWP& hDwp);


HDWP EWindow_DeferWindowPos(HDWP& hDwp, HWND hWnd, const RECT& rc, bool bRepaint);


void EWindow_StateClick_Control (LPTOPSTATE_INFO info, int id);
void EWindow_StateClick_Work    (LPTOPSTATE_INFO info, int id);
void EWindow_StateClick_State   (LPTOPSTATE_INFO info, int id);
// hWnd = ���ĸ����ڴ��������˵�
void EWindow_PopupMenu(LPTOPSTATE_INFO info);


// ָ�������Ƿ��Զ�����
bool IsAutoHide(HWND hWnd, LPOBJSTRUCT* ppData = 0, LPTOPSTATE_INFO* pInfo = 0);


_EWINDOW_END_NAMESPACE

