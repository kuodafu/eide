// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <IDraw_Header.h>
#include <control/ReadColor.h>


// 释放所有画刷
void DeleteDefaultObject();
// 创建画刷, 创建的画刷不能释放, 创建的画刷同一个颜色只会创建一次, 程序结束时需要调用 DeleteDefaultObject() 清理
HBRUSH Brush_Create(COLORREF cr);
HPEN Pen_Create(int iStyle, int cWidth, COLORREF color);

// 创建dx画刷, 创建的画刷不能释放, 创建的画刷同一个颜色只会创建一次, 程序结束时需要调用 DeleteDefaultObject() 清理
LPEX_BRUSH Brush_CreateDx(DWORD cr);
LPEX_PEN   Pen_CreateDx(DWORD cr, float width);
LPEX_FONT Font_CreateDx(LPCWSTR name = 0, LONG lfHeight = 0, FONTSTYLE fontStyle = FONTSTYLE::FontStyleRegular);
LPEX_FONT Font_CreateLfDx(const LPLOGFONTW logFont);
LPEX_FONT Font_CreateDpi(LPEX_FONT hFont, UINT dpi);
// 这个函数会释放所有画刷, 画笔, 字体, 然后重新创建
// 当程序在运行过程中，图形设备可能因为某些原因而失效
// 例如，设备显示分辨率被改变，或者用户移除显示适配器（显示器）等等
// 如果设备丢失，那么渲染目标也会变得无效，顺带着所有的设备相关的资源都会失效
// Direct2D 当接收到设备丢失的信号后，会在调用 EndDraw 函数后返回 D2DERR_RECREATE_TARGET 代码
// 如果你收到这个错误码，你必须重新创建渲染目标以及设备相关资源
// 重新加载dx资源, 会删除所有dx对象, 然后重新创建
// 一般是在 EndDraw返回false, 且最后错误=D2DERR_RECREATE_TARGET 时调用
void ReloadDxResource();

LPOBJSTRUCT GetDefData(LPOBJSTRUCT pData = 0);
// 重新从配置文件里加载默认配色
void ReloadDefColor();

// 初始化颜色配置里 Window 这个节点的数据
void iTheme_Init_Window();

// 初始化颜色配置里 CustomCodeTab 这个节点的数据, 代码切换选择夹
void iTheme_Init_CustomCodeTab();



#endif //PCH_H
