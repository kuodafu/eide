#pragma once

#include "resource.h"
#include <d2d/d2d_old.h>
#include <gdiplus/drawImage.h>


#include <control/CButton.h>
#include <control/CEdit.h>
#include <control/CCombobox.h>
#include <control/CStatic.h>
#include <CFileDialogCom.h>
#include <CFileDialog.h>
#include <control/WndControl6_0.h>
#include <assist/CFileRW.h>
#include <control/ReadColor.h>
#include <assist/CPrivateProfile.h>

#include <control/CCustomList.h>
#include <vector>
#include <set>
#include <map>

#include <assist/assist.h>
#include <assist/assist_Resource.h>

#define ID_BTN_LOAD     1001
#define ID_BTN_UPDATE   1002
#define ID_BTN_MODIFY1  1003
#define ID_BTN_MODIFY2  1004
#define ID_BTN_MODIFY3  1005
#define ID_BTN_SAVE     1006
#define ID_BTN_RELOAD   1007
#define ID_EDIT_PATH    2001



struct LIST_FILEDATA
{
    wstr file;      // 文件完整路径
    LPCWSTR name;   // 显示用的文件名, 是 file的一部分
    Gdiplus::Bitmap* img;   // 显示的图标
    int cxImg;
    int cyImg;
    LIST_FILEDATA()
    {
        name = 0;
        img = 0;
        cxImg = 0;
        cyImg = 0;
    }
    ~LIST_FILEDATA()
    {
        if (img)delete img;
        file.Release();
    }
};


extern CButton s_btnSave, s_btnLoad, s_btnUpdate, s_btnModify1, s_btnModify2, s_btnModify3, s_btnReload;
extern CMyEdit s_editPath, s_editColor;
extern CCombobox s_editModifyColor1, s_editModifyColor2;
extern CMyStatic s_static_Color, s_static_path, s_static_ModifyColor1, s_static_ModifyColor2, s_static_back_color;
extern CButton s_static_draw, s_static_save, s_static_group;
extern CCustomList s_list;
extern Gdiplus::Bitmap* s_hImage;
extern wstr s_loadFile;
extern wstr s_loadPath;
extern LPMEMDCBITMAP s_md;
extern CPrivateProfile s_ini;
extern HWND g_hWnd;
extern int s_lastBtnBottom;     // 最后一个按钮的底边

extern std::vector<LIST_FILEDATA> s_data;
extern int s_dataCount;     // s_data 实际占用的成员数
extern int s_dataBufCount;     // s_data 实际占用的成员数
extern HINSTANCE g_hInst;                                // 当前实例

void _wnd_create(HWND hWnd);
// 传递进来的是底下那个分组标签的句柄
void _wnd_create_command(HWND hWnd);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc_static_back_color(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_static_draw(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_static_save(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CALLBACK CustomList_ItemRClick(HWND hWnd, int index);
void CALLBACK CustomList_ItemSelect(HWND hWnd, int index);
bool CALLBACK CustomListBox_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus);
LRESULT _wndproc_command(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



inline wstr ini_read(LPCWSTR name, LPCWSTR defValue)
{
    return s_ini.read(L"config", name, defValue);
}

inline bool ini_write(LPCWSTR name, const wstr& value)
{
    return s_ini.write(L"config", name, value.c_str());
}
inline int ini_read_int(LPCWSTR name, int defValue)
{
    return s_ini.read_int(L"config", name, defValue);
}

inline bool ini_write(LPCWSTR name, int value)
{
    return s_ini.write(L"config", name, value);
}

// crFixed = 新的颜色
// crBack = 被替换的颜色
// lpBits = 图片在内存里的数据
// nCount = 图片像素, 高度 * 宽度
// sR,sG,sB 新的RGB, crFixed 的RGB
void  PixelFix(DWORD crFixed, DWORD crBack, LPBYTE lpBits, int nCount, BYTE sR, BYTE sG, BYTE sB);

// 修改像素点, 替换指定颜色, 不符合的颜色不处理
// crFixed = 新的颜色
// crBack = 被替换的颜色
// lpBits = 图片在内存里的数据
// nCount = 图片像素, 高度 * 宽度
void  PixelFix(DWORD crFixed, DWORD crBack, LPBYTE lpBits, int nCount);
// 修改非透明的像素点
// crFixed = 新的颜色
// lpBits = 图片在内存里的数据
// nCount = 图片像素, 高度 * 宽度
void  PixelFix(DWORD crFixed, LPBYTE lpBits, int nCount);

int enum_file(LPCWSTR findPath);

void _load_image(const wstr& path);

BOOL InvalidateRect();