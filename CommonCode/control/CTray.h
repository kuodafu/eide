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

    // ��ͼ�����ݴ�������ͼ��
    // hWnd = ��Ҫ��������ͼ��Ĵ���
    // pIcon = ͼ������, �ڲ���ͨ�����ͼƬ���ݴ���һ��ͼ��, ����ͼ��ʹ�����ͼ����ʾ
    // iconSize = ͼ�����ݴ�С, pIcon ���� iconSize Ϊ0, ���ȡ���ڵ�ͼ��
    // tips = ��ʾ��Ϣ, ����ƶ���ȥ��ʾ����Ϣ, ���Ե��� SetTips ����, ���֧��128���ַ�(����������ʶ��)
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

    // ��ͼ������������ͼ��
    // hWnd = ��Ҫ��������ͼ��Ĵ���
    // hIcon = ͼ����, ����ͼ��ʹ�����ͼ����ʾ
    // tips = ��ʾ��Ϣ, ����ƶ���ȥ��ʾ����Ϣ, ���Ե��� SetTips ����, ���֧��128���ַ�(����������ʶ��)
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

    // ��������ͼ��
    inline BOOL Destroy()
    {
        return Shell_NotifyIconW(NIM_DELETE, &nc);
    }

    // ����ͼƬͼ����ʾ�ı�, ���֧��128���ַ�(����������ʶ��)
    // tips = Ҫ���õ���ʾ�ı�, Ϊ0������ı�
    inline BOOL SetTips(LPCWSTR tips)
    {
        nc.uFlags = NIF_TIP | NIF_MESSAGE;
        if (!tips)tips = L"";;
        size_t len = wcslen(tips);
        len = len >= 128 ? 127 : len;
        memcpy_s(nc.szTip, 128, tips, len * sizeof(wchar_t));
        return Shell_NotifyIconW(NIM_MODIFY, &nc);
    }

    // ������������ʾ
    // title = ������ʾ�ı���, Ϊ0��û�б���
    // tips = ������ʾ����ʾ�ı�, Ϊ0��û����ʾ�ı�
    // icon = ��ʾ��ͼ��, NIIF_ ��ͷ����
    // time = ������ʾʱ��
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

    // ��������ͼ��, ��������ͼ���������ȡͼ����ʧ�ܵĻ�, �Ͳ�������
    // pIcon = ͼ������ݵ�ַ
    // iconSize = ͼ�����ݴ�С
    inline BOOL SetIcon(const LPBYTE pIcon, const size_t iconSize)
    {
        return SetIcon(GetIcon(pIcon, iconSize));
    }

    // ��������ͼ��
    // hIcon = ͼ����, Ϊ0�����ʧ��
    inline BOOL SetIcon(HICON hIcon)
    {
        if (!hIcon)return FALSE;
        nc.uFlags = NIF_MESSAGE | NIF_ICON;
        nc.hIcon = hIcon;
        return Shell_NotifyIconW(NIM_MODIFY, &nc);
    }

public:
    // ��ͼ�����ݴ���ͼ����, ���صľ����ʹ��ʱ��Ҫ���� DestroyIcon ����
    // pIcon = ͼ������ݵ�ַ, �������ͼ������, �򷵻�0
    // iconSize = ͼ�����ݴ�С
    // index = ������0��ʼ
    // width = ָ��ͼ��Ŀ�ȣ�0��ʾ���ı�
    // heigjt = ָ��ͼ��ĸ߶ȣ�0��ʾ���ı�
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

    // ��������ͼ��, ��ȡ����ͼ��Ĵ���, Ȼ��������ƶ���Ϣ, �����̴��ڰ�ʧЧ��ͼ�����
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

    // ��ȡ���̴���
    // win7/win10 ������ͬ��, ����ϵͳû�в���
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

