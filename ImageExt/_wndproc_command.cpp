#include "ImageExt.h"

#define ID_RADIO_NORMAL     1100
#define ID_RADIO_HOT        1101
#define ID_RADIO_DOWN       1102
#define ID_RADIO_BAN        1103
static CButton m_btn_normal, m_btn_hot, m_btn_down, m_btn_ban;
// 传递进来的是底下那个分组标签的句柄
void _wnd_create_command(HWND hWnd)
{
    const int height = 24;
    const int offset = 26;

    DWORD dwStyleEx = 0, dwStyle = 0;
    dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_CLIPCHILDREN | BS_AUTORADIOBUTTON;
    m_btn_normal.create(dwStyleEx, L"正常状态", dwStyle, 10, 10 + offset * 0, 80, height, hWnd, ID_RADIO_NORMAL);
    m_btn_hot.create(dwStyleEx, L"点燃状态", dwStyle, 10, 10 + offset * 1, 80, height, hWnd, ID_RADIO_HOT);
    m_btn_down.create(dwStyleEx, L"按下状态", dwStyle, 10, 10 + offset * 2, 80, height, hWnd, ID_RADIO_DOWN);
    m_btn_ban.create(dwStyleEx, L"禁止状态", dwStyle, 10, 10 + offset * 3, 80, height, hWnd, ID_RADIO_BAN);

    m_btn_normal.SetCheck(true);

}


LRESULT _wndproc_command(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int id = LOWORD(wParam);
    switch (id)
    {
    case ID_RADIO_NORMAL:
    {
        break;
    }
    case ID_RADIO_HOT:
    {
        break;
    }
    case ID_RADIO_DOWN:
    {
        break;
    }
    case ID_RADIO_BAN:
    {
        break;
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
