#pragma once
#include <control/WndBase.h>
#include <shellapi.h>

class CTray
{
private:
    NOTIFYICONDATAW nc;

public:
	CTray()
	{
        nc = { 0 };
        nc.cbSize = sizeof(nc);
	}
	~CTray()
	{

	}
	operator UINT() { return WM_TASKBARCREATED(); }
	
	inline static UINT WM_TASKBARCREATED()
	{
		static UINT msg;
		if (!msg)
			msg = RegisterWindowMessageW(L"TaskbarCreated");
		return msg;
	}

	inline LPCWSTR className() { return L"TaskbarCreated"; }
	inline LPCWSTR SuperClassName() { return L"TaskbarCreated"; }

public:

    // 从图标数据创建托盘图标
    // hWnd = 需要创建托盘图标的窗口
    // pIcon = 图标数据, 内部会通过这个图片数据创建一个图标, 托盘图标使用这个图标显示
    // iconSize = 图标数据大小, pIcon 或者 iconSize 为0, 则获取窗口的图标
    // tips = 提示信息, 鼠标移动上去提示的信息, 可以调用 SetTips 更改, 最大支持128个字符(包括结束标识符)
	inline BOOL create(HWND hWnd, const LPBYTE pIcon = 0, const size_t iconSize = 0, LPCWSTR tips = 0)
	{
        if (!::IsWindow(hWnd))return FALSE;
        HICON hIcon = 0;
        if (pIcon == 0 || iconSize == 0)
            hIcon = (HICON)SendMessageW(hWnd, WM_GETICON, 0, 0);
        else
            hIcon = GetIcon(pIcon, iconSize);
        return create(hWnd, hIcon, tips);
	}

    // 从图标句柄创建托盘图标
    // hWnd = 需要创建托盘图标的窗口
    // hIcon = 图标句柄, 托盘图标使用这个图标显示
    // tips = 提示信息, 鼠标移动上去提示的信息, 可以调用 SetTips 更改, 最大支持128个字符(包括结束标识符)
    inline BOOL create(HWND hWnd, HICON hIcon, LPCWSTR tips = 0)
	{
        if (!::IsWindow(hWnd))return FALSE;
        nc.hIcon            = hIcon;
        nc.hWnd             = hWnd;
        nc.uID              = 1;
        nc.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
        nc.uCallbackMessage = 11685;
        nc.uTimeout         = 1000;
        //m_hWnd              = hWnd;
        if (tips)
        {
            size_t len = wcslen(tips);
            len = len >= 128 ? 127 : len;
            memcpy_s(nc.szTip, 128, tips, len * sizeof(wchar_t));
        }
        return Shell_NotifyIconW(NIM_ADD, &nc);
	}

    // 销毁托盘图标
    inline BOOL Destroy()
    {
        return Shell_NotifyIconW(NIM_DELETE, &nc);
    }

    // 设置图片图标提示文本, 最大支持128个字符(包括结束标识符)
    // tips = 要设置的提示文本, 为0则清空文本
    inline BOOL SetTips(LPCWSTR tips)
    {
        nc.uFlags = NIF_TIP | NIF_MESSAGE;
        if (!tips)tips = L"";;
        size_t len = wcslen(tips);
        len = len >= 128 ? 127 : len;
        memcpy_s(nc.szTip, 128, tips, len * sizeof(wchar_t));
        return Shell_NotifyIconW(NIM_MODIFY, &nc);
    }

    // 置托盘气泡提示
    // title = 气泡提示的标题, 为0则没有标题
    // tips = 气泡提示的提示文本, 为0则没有提示文本
    // icon = 显示的图标, NIIF_ 开头常量
    // time = 气泡显示时间
    inline BOOL SetTipsBubble(LPCWSTR title, LPCWSTR tips, DWORD icon = NIIF_NONE, DWORD time = 100000)
    {
        nc.dwInfoFlags  = icon;
        nc.uFlags       = NIF_TIP | NIF_ICON | NIF_INFO | NIF_MESSAGE;
        nc.uTimeout     = time;
        if (!title)     title = L"";
        if (!tips)      tips = L"";
        size_t l1       = wcslen(tips);
        size_t l2       = wcslen(title);

        l1 = l1 >= 64  ? 63  : l1;
        l2 = l2 >= 128 ? 127 : l2;
        memcpy_s(nc.szInfoTitle , 64 , title, l1 * sizeof(wchar_t));
        memcpy_s(nc.szTip       , 128, tips , l2 * sizeof(wchar_t));
        return Shell_NotifyIconW(NIM_MODIFY, &nc);
    }

    // 设置托盘图标, 如果从这个图标数据里获取图标句柄失败的话, 就不会设置
    // pIcon = 图标的数据地址
    // iconSize = 图标数据大小
    inline BOOL SetIcon(const LPBYTE pIcon, const size_t iconSize)
    {
        return SetIcon(GetIcon(pIcon, iconSize));
    }

    // 设置托盘图标
    // hIcon = 图标句柄, 为0则调用失败
    inline BOOL SetIcon(HICON hIcon)
    {
        if (!hIcon)return FALSE;
        nc.uFlags = NIF_MESSAGE | NIF_ICON;
        nc.hIcon = hIcon;
        return Shell_NotifyIconW(NIM_MODIFY, &nc);
    }

public:
    // 从图标数据创建图标句柄, 返回的句柄不使用时需要调用 DestroyIcon 销毁
    // pIcon = 图标的数据地址, 如果不是图标数据, 则返回0
    // iconSize = 图标数据大小
    // index = 索引从0开始
    // width = 指定图标的宽度，0表示不改变
    // heigjt = 指定图标的高度，0表示不改变
    inline static HICON GetIcon(const LPBYTE pIcon, const size_t iconSize, const size_t index = 0, DWORD width = 0, DWORD heigjt = 0)
    {
        struct ICONDIRENTRY
        {
            BYTE bWidth;
            BYTE bHeight;
            BYTE bColorCount;
            BYTE bReserved;
            WORD wPlanes;
            WORD wBitCount;
            int dwBytesInRes;
            int dwImageOffset;
        };
        if (pIcon == 0 || iconSize < sizeof(ICONDIRENTRY))
            return FALSE;

        ICONDIRENTRY icon = { 0 };
        short idType = ((short*)pIcon)[1];

        memcpy(&icon, pIcon + (6 + index * 16), sizeof(ICONDIRENTRY));

        LPBYTE ptr = pIcon + icon.dwImageOffset;
        return CreateIconFromResourceEx(ptr, icon.dwBytesInRes, idType, 0x00030000, width, heigjt, LR_DEFAULTCOLOR);
    }

    // 更新托盘图标, 获取托盘图标的窗口, 然后发送鼠标移动消息, 让托盘窗口把失效的图标清除
    // 
    inline static void UpDateTray(BOOL hide = FALSE)
    {
        HWND hWnd = hide ? FindNotifyIconOverflowWindow() : FindTrayWnd();
        RECT rc;
        GetClientRect(hWnd, &rc);
        const int offset = 4;
        for (LONG y = 0; y < rc.bottom; y += offset)
        {
            for (LONG x = 0; x < rc.right; x += offset)
                SendMessageW(hWnd, WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELONG(x, y));
        }
    }

private:

    // 获取托盘窗口
    // win7/win10 都是相同的, 其他系统没有测试
    inline static HWND FindTrayWnd()
    {
        HWND hWnd = 0;

        hWnd = FindWindowW(L"Shell_TrayWnd", 0);
        if (!hWnd) return 0;

        hWnd = FindWindowExW(hWnd, 0, L"TrayNotifyWnd", 0);

        if (!hWnd) return 0;
        hWnd = FindWindowExW(hWnd, 0, L"SysPager", 0);

        if (!hWnd) return 0;
        hWnd = FindWindowExW(hWnd, 0, L"ToolbarWindow32", 0);
        return hWnd;
    }

    inline static HWND FindNotifyIconOverflowWindow()
    {
        HWND hWnd = FindWindowW(L"NotifyIconOverflowWindow", 0);

        if (!hWnd) return 0;
        hWnd = FindWindowExW(hWnd, 0, L"ToolbarWindow32", 0);
        return hWnd;
    }
};

