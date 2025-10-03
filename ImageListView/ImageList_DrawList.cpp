#include "ImageListView.h"


inline HBRUSH __get_brush(STATE state, bool isFocus)
{
    COLORREF crBack = g_color.crBack;
    COLORREF crHot = g_color.crHot;
    COLORREF crDown = g_color.crDown;

    static HBRUSH hbrBack, hbrHot, hbrDown;
    if (hbrBack)    DeleteObject(hbrBack);
    if (hbrHot)     DeleteObject(hbrHot);
    if (hbrDown)    DeleteObject(hbrDown);

    hbrBack = CreateSolidBrush(crBack);
    hbrHot = CreateSolidBrush(crHot);         // 热点画刷
    hbrDown = CreateSolidBrush(crDown);       // 按下画刷

    HBRUSH hbr = 0;
    switch (state)
    {
    case DOWN:      hbr = hbrDown;   break;
    case SELECTED:  hbr = hbrDown;   break;
    case HOT:       hbr = hbrHot;    break;
    default:
        break;
    }

    if (hbr)
    {
        if (!isFocus && hbr == hbrDown)
            hbr = hbrDown;
    }
    return hbr;
}

bool CALLBACK CustomListBoxImage_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus)
{
    SetTextColor(hdc, g_color.crText);
    HBRUSH hbr = __get_brush(state, isFocus);
    if (hbr)  FillRect(hdc, rcItem, hbr);
    RECT rc = *rcItem;
    rc.left += 5;
    rc.right = 30;
    const int cyItem = rc.bottom - rc.top;
    SHOWIMAGEDATA& data = s_dataImage[index];

    wstr str;
    str.assign(index);
    DrawTextW(hdc, str.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_LEFT);

    rc.left = rc.right + 5;
    rc.right = rc.left + 16;
    rc.top = rc.top + (cyItem - 16) / 2;

    Gdiplus::Bitmap* pBmp = GetBitmapFromMap(data.file.c_str());
    if (pBmp)
    {
        Gdiplus::Graphics gp(hdc);
        gp.DrawImage(pBmp, (INT)rc.left, (INT)rc.top, 16, 16);
    }

    rc.top = rcItem->top;
    rc.left = rc.right + 10;
    rc.right = rcItem->right;
    DrawTextW(hdc, data.name, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_LEFT);
    return true;
}


inline Gdiplus::Bitmap* _get_bitmap_from_state(PROJECT_DATA* projectData, int index, STATE state, RECT& rcIcon)
{
    // 优先使用大图片里的位置
    LPCWSTR szImageFileName = xml_getimage_filename(projectData, index, state, g_dpiIndex, &rcIcon); // 获取图片的路径
    if (projectData->hBitmap
        && rcIcon.right - rcIcon.left > 0       // 宽度必须大于0
        && rcIcon.bottom - rcIcon.top > 0)      // 高度必须大于0
    {
        // 工程里有这个图片, 直接返回这个图片
        return projectData->hBitmap;
    }
    Gdiplus::Bitmap* hBitmap = QueryBitmapFromMap(szImageFileName);
    if (!hBitmap)return 0;
    //! 工程里没有图片数据, 需要从路径里读取, 能走到这里怀疑是bug
    rcIcon.left = 0;
    rcIcon.top = 0;
    rcIcon.right = hBitmap->GetWidth();
    rcIcon.bottom = hBitmap->GetHeight();
    return hBitmap;
}

inline void _draw_icon(Gdiplus::Graphics& gp, Gdiplus::Bitmap* hBitmap, Gdiplus::Rect& rcDrawIcon, const RECT& rcIcon)
{
    const int offset = 10;
    const int width = rcIcon.right - rcIcon.left;
    const int height = rcIcon.bottom - rcIcon.top;
    if (hBitmap)
    {
        Gdiplus::Rect rcDraw = rcDrawIcon;
        if (width < rcDraw.Width) rcDraw.Width = width;
        if (height < rcDraw.Height) rcDraw.Height = height;
        gp.DrawImage(hBitmap, rcDraw, rcIcon.left, rcIcon.top, width, height, Gdiplus::Unit::UnitPixel);
    }
    rcDrawIcon.Offset(m_data->maxWidth + offset, 0);
}

bool CALLBACK CustomListBoxItem_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus)
{
    SetTextColor(hdc, g_color.crText);
    HBRUSH hbr = __get_brush(state, isFocus);
    if (hbr)  FillRect(hdc, rcItem, hbr);
    RECT rc = *rcItem;
    rc.left += 5;
    rc.right = 30;
    const int cyItem = rc.bottom - rc.top;
    wstr str;
    str.assign(index + 1);

    DrawTextW(hdc, str.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);
    rc.left = rc.right + 4;
    rc.top = rc.top + ((cyItem) - m_data->maxHeight) / 2;

    std::vector<XMLDATA_STRUCT>& arr = *m_data->arr;
    RECT rcIcon[4];
    Gdiplus::Bitmap* hBitmap[] =
    {
        _get_bitmap_from_state(m_data, index, STATE::NORMAL,    rcIcon[0]),
        _get_bitmap_from_state(m_data, index, STATE::HOT,       rcIcon[1]),
        _get_bitmap_from_state(m_data, index, STATE::DOWN,      rcIcon[2]),
        _get_bitmap_from_state(m_data, index, STATE::BAN,       rcIcon[3]),
    };
    if (hBitmap[0] || hBitmap[1] || hBitmap[2] || hBitmap[3])
    {
        Gdiplus::Graphics gp(hdc);
        Gdiplus::Rect rcDrawIcon(rc.left, rc.top, m_data->maxWidth, m_data->maxHeight);
        _draw_icon(gp, hBitmap[0], rcDrawIcon, rcIcon[0]);
        _draw_icon(gp, hBitmap[1], rcDrawIcon, rcIcon[1]);
        _draw_icon(gp, hBitmap[2], rcDrawIcon, rcIcon[2]);
        _draw_icon(gp, hBitmap[3], rcDrawIcon, rcIcon[3]);
    }

    rc.left += ((m_data->maxWidth + 10) * 4) + 20;  // id画出位置是图片的右边
    rc.top = rcItem->top;                   // 顶边是项目的顶边
    rc.bottom = rcItem->bottom;        // id只绘画上半部分
    rc.right = rcItem->right;
    XMLDATA_STRUCT& data = m_data->arr->at(index);
    data.left = rc.left;
    data.right = rc.left;
    if (data.id)
    {
        const int width = (rc.right - rc.left) / 3;
        rc.right = rc.left + width;
        RECT rcText = rc;
        DrawTextW(hdc, L"id=\"", 4, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        DrawTextW(hdc, L"id=\"", 4, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);

        rc.left = rcText.right;
        rcText.left = rcText.right;
        rcText.right = rc.right - 10;
        COLORREF crText = SetTextColor(hdc, g_color.crTextID);
        DrawTextW(hdc, data.id, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        DrawTextW(hdc, data.id, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        SetTextColor(hdc, crText);

        rc.left = rcText.right;
        DrawTextW(hdc, L"\"", 1, &rc, DT_VCENTER | DT_SINGLELINE);
        rc.left = rcText.right + 20;
        rc.right = rcItem->right;
        data.right = rc.left;
    }
    //rc.left = rc.right + 5;
    //rc.right = rcItem->right;

    if (data.remarks)
    {
        RECT rcText = rc;
        DrawTextW(hdc, L"说明=\"", 4, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        DrawTextW(hdc, L"说明=\"", 4, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);

        rc.left = rcText.right;
        rcText.left = rcText.right;
        rcText.right = rc.right - 10;
        COLORREF crText = SetTextColor(hdc, g_color.crTextRemarks);
        DrawTextW(hdc, data.remarks, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        DrawTextW(hdc, data.remarks, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        SetTextColor(hdc, crText);

        rc.left = rcText.right;
        DrawTextW(hdc, L"\"", 1, &rc, DT_VCENTER | DT_SINGLELINE);
    }
    return true;
}


bool CALLBACK CustomListBox_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus)
{
    SetTextColor(hdc, g_color.crText);
    HBRUSH hbr = __get_brush(state, isFocus);
    if (hbr)  FillRect(hdc, rcItem, hbr);

    RECT rc = *rcItem;
    rc.left += 5;

    SetTextColor(hdc, g_color.crTextProject);
    PROJECT_DATA& data = s_data[index];
    rc.right -= 120;
    DrawTextW(hdc, data.xmlName, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    size_t size = data.arr->size();

    rc.left = rc.right;
    rc.right = rcItem->right - 5;
    static wstr szCount(50);
    szCount.assign(size).append(L"个项目");
    DrawTextW(hdc, szCount.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_RIGHT);
    return true;
}

