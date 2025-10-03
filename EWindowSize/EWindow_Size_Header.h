#pragma once

#include "pch.h"
#include <map>
#include <vector>
#include <control/WndBaseTypedef.h>
#include <IDraw_Header.h>
#include <hook_detours/apiHook.h>
#include <control/CMenu.h>


// 根据需要开启或者关闭命名空间
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

#define WINDOW_CONTROL_WIDTH    135   // 组件箱宽度
#define Component_Margin 3    // 组件预留边距

#define IDMENU_FLOAT            1000    // 浮动(&F)
#define IDMENU_STOP             1001    // 停靠(&K)
#define IDMENU_AUTOHIDE         1002    // 自动隐藏(&T)
#define IDMENU_CLOSE            1003    // 关闭(&C)

_EWINDOW_BEGIN_NAMESPACE


struct EWINDOW_SIZE_MOVEINFO 
{
    HWND hChild1;       // 在移动组件箱时获取, 这个是MDI下的第一层窗口
    HWND hChild2;       // 在移动组件箱时获取, 这个是MDI下的第二层窗口
    HWND hChild3;       // 在移动组件箱时获取, 这个是MDI下的第三层窗口, 这个窗口决定了MDI显示什么内容
    HWND hChild4;       // 只有起始页或者设计器窗口才会有值
    int windowType;     // 当前MDI下窗口的类型, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页
    int code_type;      // 当前窗口是什么类型, 常量, 全局变量, 类, 数据类型....., 代码切换选择夹被切换的时候赋值

    int heightCombobox; // 组合框高度

    HMENU hMenu;                // 三角按钮的菜单
    LPTOPSTATE_INFO pMenuClick; // 当前是哪个窗口弹出了菜单, 选中菜单有可能需要修改这个结构并重画
};

extern EWINDOW_SIZE_MOVEINFO g_moveInfo;

void EWindow_Size_InitHook();

// 易语言主窗口尺寸被改变
LRESULT EWindow_Size_OnSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// 左边工作夹尺寸被改变, 需要修改工作夹里的组件位置
LRESULT EWindow_Size_OnWorkSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// 顶边菜单栏容器尺寸被改变, 需要修改菜单栏容器里面的组件
LRESULT EWindow_Size_OnMenuSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// 中间代码区MDI窗口被改变, 这里需要修改不少东西
LRESULT EWindow_Size_OnMDIClientSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// 底边的状态夹容器, 主要是变量表需要修改, 其他都是一个窗口
LRESULT EWindow_Size_OnStateSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);
LRESULT EWindow_Size_OnStateSize_lr(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// 组件箱, 需要根据MDIClient窗口显示的内容来调整位置
LRESULT EWindow_Size_OnControlSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

// 工作夹的选择夹被改变
LRESULT CALLBACK WndProc_OnWorkTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage);

// 状态夹的选择夹被改变
LRESULT CALLBACK WndProc_OnStateTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage);

// 自绘选择夹被改变
LRESULT CALLBACK WndProc_OnCustomTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage);



// 状态夹的选择夹被改变
LRESULT EWindow_Size_OnCustomTab(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);

wstr Window_Get_Title(HWND hWnd);


// 移动 MDI, 自绘选择夹, 两个组合框
// 这几个组件一般都是动一个就全部都得动
// 先移动MDI, 在移动自绘选择夹, 最后移动组合框, 其他组件都是根据MDI计算窗口位置
void EWindow_MoveMDI_Tab_Cbx(HDWP& hDwp);


void EWindow_MoveCustomTab(HDWP& hDwp);
void EWindow_MoveMDIClient(HDWP& hDwp);

// 移动组件箱
void EWindow_MoveControl(HDWP& hDwp);


// 移动左上角菜单
void EWindow_MoveMenu(HDWP& hDwp);
// 移动顶边菜单栏容器
void EWindow_MoveMenuWindow(HDWP& hDwp);

// 菜单栏和菜单栏里的组件即将被改变, 这几个组件都是固定死的位置
void EWindow_Menu_SetPos(HWND hWnd, LPWINDOWPOS pos);

// 移动底边的状态条
void EWindow_MoveStatus(HDWP& hDwp);

// 移动底部的状态夹
void EWindow_MoveState(HDWP& hDwp);

// 移动左边工作夹
void EWindow_MoveWork(HDWP& hDwp);

// 移动两个组合框
void EWindow_MoveConmboBox(HDWP& hDwp);

// 判断当前选中的是否是窗口设计器, 返回0 = 代码区, 1=窗口设计器, 2=起始页, -1=MDI没有内容
int EWindow_IsSelWindow();
// 获取指定窗口下的子窗口, 如果传入句柄为0, 则获取MDI下的第一层窗口
// 返回0 = 代码区, 1=窗口设计器, 2=起始页, -1=MDI没有内容
int EWindow_GetMDIClientChild(HWND hChild = 0);
// 移动MDI下的子窗口
void EWindow_MoveMDIClientChild(HDWP& hDwp);


HDWP EWindow_DeferWindowPos(HDWP& hDwp, HWND hWnd, const RECT& rc, bool bRepaint);


void EWindow_StateClick_Control (LPTOPSTATE_INFO info, int id);
void EWindow_StateClick_Work    (LPTOPSTATE_INFO info, int id);
void EWindow_StateClick_State   (LPTOPSTATE_INFO info, int id);
// hWnd = 是哪个窗口触发弹出菜单
void EWindow_PopupMenu(LPTOPSTATE_INFO info);


// 指定窗口是否自动隐藏
bool IsAutoHide(HWND hWnd, LPOBJSTRUCT* ppData = 0, LPTOPSTATE_INFO* pInfo = 0);


_EWINDOW_END_NAMESPACE

