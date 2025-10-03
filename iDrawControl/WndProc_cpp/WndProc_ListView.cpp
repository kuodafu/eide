#include "../EWindow_Fne_Header.h"

LRESULT CALLBACK WndProc_ListView(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ListView_Header(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ListView_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LPEX_IMAGE _check_uncheck();
LPEX_IMAGE _check_check();
static std::map<int, LPOBJSTRUCT> m_map_listView;

// 超级列表框数据
struct LISTVIEW_CUSTOM_DATA
{
    LPEX_BRUSH hbrText;
    LPEX_BRUSH hbrHot;
    LPEX_BRUSH hbrSel;
    bool isCheckBox;
};
struct LISTVIEW_ICON_INFO
{
    int count;
    LPEX_IMAGE hImage;
};
static std::map<HIMAGELIST, LISTVIEW_ICON_INFO> m_map_icon;

inline void IDraw_listview_Destroy()
{
    for (auto it = m_map_icon.begin(); it != m_map_icon.end(); ++it)
        _img_destroy(it->second.hImage);
    m_map_icon.clear();
    m_map_listView.clear();
}

LPEX_IMAGE _check_uncheck()
{
    static LPEX_IMAGE img;
    if (!img)
    {
        BYTE data[] =
        {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x0F,
0x00, 0x00, 0x00, 0x0F, 0x04, 0x03, 0x00, 0x00, 0x00, 0xC9, 0xF8, 0x88, 0x79, 0x00, 0x00, 0x00, 0x0F, 0x50, 0x4C, 0x54,
0x45, 0xFF, 0xFF, 0xFF, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xFF, 0xFF, 0xFF, 0xB2, 0xB2, 0xB2, 0xC4, 0x42, 0x08, 0xD0,
0x00, 0x00, 0x00, 0x03, 0x74, 0x52, 0x4E, 0x53, 0x00, 0xC3, 0xC4, 0x86, 0xA8, 0x46, 0xFA, 0x00, 0x00, 0x00, 0x27, 0x49,
0x44, 0x41, 0x54, 0x78, 0x5E, 0xB5, 0xCD, 0xC1, 0x0D, 0x00, 0x00, 0x04, 0xC0, 0xC0, 0x4A, 0x6C, 0xC4, 0x02, 0xC4, 0xFE,
0x33, 0x89, 0x07, 0x1B, 0xB8, 0x57, 0x7F, 0xE5, 0x48, 0x8D, 0x00, 0xB5, 0xE1, 0x7F, 0xB1, 0xAF, 0xE4, 0x34, 0x5D, 0x82,
0x10, 0x8B, 0x6E, 0x78, 0x03, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 0x65, 0x78,
0x75, 0x69, 0xCB, 0xD8, 0xB2, 0xC4, 0x2D, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        img = _img_create_frommemory(data, sizeof(data));
    }
    return img;
}
LPEX_IMAGE _check_check()
{
    static LPEX_IMAGE img;
    if (!img)
    {
        BYTE data[] =
        {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x0F,
0x00, 0x00, 0x00, 0x0F, 0x04, 0x03, 0x00, 0x00, 0x00, 0xC9, 0xF8, 0x88, 0x79, 0x00, 0x00, 0x00, 0x0F, 0x50, 0x4C, 0x54,
0x45, 0xFF, 0xFF, 0xFF, 0x6F, 0xB6, 0x4C, 0x6F, 0xB6, 0x4C, 0x6F, 0xB6, 0x4C, 0xFF, 0xFF, 0xFF, 0x9F, 0xF4, 0xC5, 0x93,
0x00, 0x00, 0x00, 0x03, 0x74, 0x52, 0x4E, 0x53, 0x00, 0xC3, 0xC4, 0x86, 0xA8, 0x46, 0xFA, 0x00, 0x00, 0x00, 0x3C, 0x49,
0x44, 0x41, 0x54, 0x78, 0x5E, 0x3D, 0x8D, 0xC1, 0x0D, 0x00, 0x20, 0x08, 0x03, 0x31, 0x4E, 0xA4, 0x74, 0x03, 0xBA, 0xFF,
0x4C, 0xA6, 0x55, 0xBC, 0xD7, 0xE5, 0x92, 0x42, 0x7C, 0x46, 0x8A, 0x15, 0x31, 0xD3, 0x3C, 0x41, 0x0B, 0xEB, 0x0A, 0xA8,
0xE2, 0x20, 0x81, 0x82, 0x84, 0xC5, 0xB2, 0x80, 0x7C, 0x73, 0x05, 0x0B, 0xFA, 0xA0, 0xE9, 0x5F, 0x3B, 0x3E, 0x07, 0x5F,
0x13, 0x10, 0x9A, 0xCF, 0xB2, 0x3F, 0xC9, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 0x65,
0x78, 0x75, 0x69, 0xCB, 0xD8, 0xB2, 0xC4, 0x2D, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        img = _img_create_frommemory(data, sizeof(data));
    }
    return img;
}

LPOBJSTRUCT _subclass_listView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll, bool isHot, int fmt)
{
    if (nItemHeight < 10) nItemHeight = 24;
    static bool first = true;
    if (first)
    {
        IDraw_PushPluginFreeCallback(IDraw_listview_Destroy);
        first = false;
    }

    static int s_id = 0xBA5C;
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_ListView, reMoveBorder, isSetBack, 0);
    if (!fmt) fmt = DT_VCENTER | DT_SINGLELINE;
    pData->draw.fmt = fmt;
    HWND hHead = ListView_GetHeader(hWnd);
    if (hHead)
    {
        LPOBJSTRUCT pHead = _subclass(hHead, WndProc_ListView_Header, false, false);
        //pHead->draw.color.crBackGround  = s_info->themeColor->crBackExtern();
        //pHead->draw.color.hbrBackGround = s_info->pfnCreateBrush(pHead->draw.color.crBackGround);
        pHead->draw.isHot       = true;
        pHead->draw.indexDown   = -1;
        pHead->draw.indexHot    = -1;
    }

    const DWORD styleList = (DWORD)SendMessageW(hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
    pData->fnNotify = WndProc_ListView_OnNotify;
    pData->draw.nItemHeight  = nItemHeight;
    pData->draw.isHot        = isHot;
    pData->draw.indexDown    = -1;
    pData->draw.indexHot     = -1;
    pData->draw.hCanvas      = _canvas_create(pData->hWnd, 0, 0); // dx绘画
    pData->draw.isCheckBox   = __query(styleList, LVS_EX_CHECKBOXES);
    
    int id = GetDlgCtrlID(hWnd);
    if (id == 0)
    {
        pData->SetWndLong(hWnd, GWLP_ID, s_id);
        id = s_id++;
    }
    m_map_listView[id] = pData;
    if (isScroll && s_info->MY_MF_OWNERDRAW)
    {
        pData->drawScroll = ScrollCreate(pData->hWnd, pData->draw.color.crBorder,
            nItemHeight,// m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
            false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
            true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
            false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
            true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
        );

    }

    SetTimer(hWnd, (UINT_PTR)pData, 50, 0);
    return pData;
}

// 重画选中或者热点项目, 会把前一个热点/按下项目取消, 然后设置新的热点/按下项目
inline static void InvalidateItem(LPOBJSTRUCT pData, int index, bool isHot)
{
    RECT rc;
    int* pIndex = isHot ? &pData->draw.indexHot : &pData->draw.indexSel;
    if (*pIndex != -1)
    {
        ListView_GetItemRect(pData->hWnd, *pIndex, &rc, LVIR_BOUNDS);
        ::InvalidateRect(pData->hWnd, &rc, 0);
    }
    *pIndex = index;
    if (index != -1)
    {
        ListView_GetItemRect(pData->hWnd, index, &rc, LVIR_BOUNDS);
        ::InvalidateRect(pData->hWnd, &rc, 0);
    }
    //::InvalidateRect(pData->hWnd, 0, 0);
}

inline static void _listView_DrawItem(LPCANVAS hCanvas, LVITEMW& item, const RECT& rcItem, const RECT& rcText, LPOBJSTRUCT pData, int i, int index, int nColumnCount, LISTVIEW_CUSTOM_DATA& draw)
{
    LPEX_BRUSH hbrBack = 0;

    if (i == index)
    {
        hbrBack = draw.hbrSel;
    }
    else if (i == pData->draw.indexHot)
    {
        hbrBack = draw.hbrHot;
    }

    if (hbrBack)
    {
        RECT_F rcItemF(rcItem);
        int oldMode = 0;
        if (s_info->isGpRender)
            _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);
        _canvas_fillRectangle(hCanvas, hbrBack, &rcItemF);
        if (s_info->isGpRender)
            _canvas_SetAntialiasMode(hCanvas, oldMode, 0);
    }

    RECT_F rcf(rcText);
    //rcf.left += 5.f;
    if (draw.isCheckBox)
    {
        int nHeight = rcItem.bottom - rcItem.top;
        RECT_F rcIcon;
        rcIcon.left = (float)((nHeight - 16)) / 2;
        rcIcon.top = (float)(rcItem.top + (nHeight - 16) / 2);
        rcIcon.right = rcIcon.left + 16;
        rcIcon.bottom = rcIcon.top + 16;

        rcf.left = rcIcon.right + rcIcon.left;

        UINT isCheck = ListView_GetCheckState(pData->hWnd, i);

        LPEX_IMAGE hImg = isCheck ? _check_check() : _check_uncheck();
        _canvas_drawimagerect(hCanvas, hImg, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
    }

    int fmt = pData->draw.fmt;
    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    _canvas_drawtext(hCanvas, hFont, draw.hbrText, item.pszText, -1, fmt, &rcf);


    int cchTextMax = item.cchTextMax;
    LPWSTR pszText = item.pszText;
    for (int nColIndex = 1; nColIndex < nColumnCount; nColIndex++)
    {
        item.cchTextMax = cchTextMax;
        item.pszText = pszText;
        const int width = ListView_GetColumnWidth(pData->hWnd, nColIndex);
        ListView_GetItemText(pData->hWnd, i, nColIndex, item.pszText, item.cchTextMax);

        rcf.left = rcf.right;
        rcf.right = rcf.left + width;
        rcf.left += 5;
        _canvas_drawtext(hCanvas, hFont, draw.hbrText, item.pszText, -1, fmt, &rcf);
    }
}
inline static void _listView_DrawItem_Icon(LPCANVAS hCanvas, LVITEMW& item, LPEX_IMAGE pIcon, RECT_F& rcDraw, HIMAGELIST hImageList, LPOBJSTRUCT pData, int index, LISTVIEW_CUSTOM_DATA& draw)
{
    RECT rcIcon = { 0 };
    ListView_GetItemRect(pData->hWnd, index, &rcIcon, LVIR_ICON);
    const int cx = rcIcon.right - rcIcon.left;
    const int cy = rcIcon.bottom - rcIcon.top;

    if (pIcon)
    {
        const int cxIcon    = (int)(rcDraw.right - rcDraw.left);
        const int cyIcon    = (int)(rcDraw.bottom - rcDraw.top);
        const int offsetX   = (cx - cxIcon) / 2;
        const int offsetY   = (cy - cyIcon) / 2;

        RECT_F rc;  // 绘画到组件上的位置
        rc.left     = (float)(rcIcon.left + offsetX);
        rc.top      = (float)(rcIcon.top  + offsetY);
        rc.right    = rc.left + (float)cxIcon;
        rc.bottom   = rc.top  + (float)cyIcon;
        _canvas_drawimagerectrect(hCanvas, pIcon, rc.left, rc.top, rc.right, rc.bottom,
            rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 255);
        return;
    }

    int cxIcon = 0;
    int cyIcon = 0;

    LISTVIEW_ICON_INFO& iconInfo = m_map_icon[hImageList];
    const int count = ImageList_GetImageCount(hImageList);
    ImageList_GetIconSize(hImageList, &cxIcon, &cyIcon);

    if (iconInfo.count != count)    // 需要重新创建图片组
    {
        _img_destroy(iconInfo.hImage);
        iconInfo.count = count;

        LPMEMDCBITMAP md = md_create(cxIcon * count, cyIcon);
        Gdiplus::Graphics gp(md->hdc);
        for (int i = 0; i < count; i++)
        {
            HICON hIcon = ImageList_GetIcon(hImageList, i, ILD_NORMAL);
            Gdiplus::Bitmap bmp(hIcon);
            gp.DrawImage(&bmp, i * cxIcon, 0, cxIcon, cyIcon);
            DestroyIcon(hIcon);
        }
        int size = 0;
        LPBYTE pngData = GetPngDataFromBitmap(md->hBitmap, &size, md->hdc);
        iconInfo.hImage = _img_create_frommemory(pngData, size);
        delete[] pngData;
        md_destroy(md);

    }
    LPEX_IMAGE hImg = iconInfo.hImage;
    if (!hImg) return;
    // 图标在图片里的位置
    RECT_F rcDrawIcon((float)(cxIcon * item.iImage), 0, (float)hImg->GetWidth(), (float)cyIcon);
    rcDrawIcon.right = rcDrawIcon.left + cxIcon;


    const int offsetX = (cx - cxIcon) / 2;
    const int offsetY = (cy - cyIcon) / 2;

    RECT_F rc;  // 绘画到组件上的位置
    rc.left     = (float)(rcIcon.left + offsetX);
    rc.top      = (float)(rcIcon.top  + offsetY);
    rc.right    = rc.left + (float)cxIcon;
    rc.bottom   = rc.top  + (float)cyIcon;


    _canvas_drawimagerectrect(hCanvas, hImg, rc.left, rc.top, rc.right, rc.bottom,
        rcDrawIcon.left, rcDrawIcon.top, rcDrawIcon.right, rcDrawIcon.bottom, 255);
}

inline LPWINDOW_ICON_DATA _lv_get_icon(BINDWINDOW_INFO* callbackArr, int callbackCount, LVITEMW& item, RECT_F& rcDraw, int state, LPOBJSTRUCT pData)
{
    LPEX_IMAGE pIcon = 0;
    for (int i = 0; i < callbackCount; i++)
    {
        BINDWINDOW_INFO& callbackItem = callbackArr[i];
        if (callbackItem.callback)
        {
            LPWINDOW_ICON_DATA ret = callbackItem.callback(pData->hWnd, callbackItem.hResource, item.pszText, &item, &rcDraw, (STATE)state, 0);
            if (!ret)
            {
                ret = s_info->pResource->User_GetIcon(callbackItem.hResource, item.pszText, &rcDraw, (STATE)state, s_info->dpiX);
            }
            if (ret) return ret;
        }
    }
    return 0;
}

LRESULT CALLBACK WndProc_ListView(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (pObj && s_info->MY_MF_OWNERDRAW)
    {
        LRESULT ret = 0;
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            return ret;
        }
    }
    switch (message)
    {
    case WM_DPICHANGED:
    {
        EWindow_DPIChange(pData);

        //ListView_GetOrigin();
        SetTimer(hWnd, (UINT_PTR)pData, 50, 0);
        break;
    }
    case WM_ERASEBKGND:
        return WndProc_EraseBkgnd(hWnd, message, wParam, lParam, pData);
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        const bool isDx = true;
        LPCANVAS hCanvas = pData->draw.hCanvas;
        s_info->iDraw->BeginPaint(pData, ps, isDx, true);

        int oldMode = 0;
        _canvas_SetTextRenderingHint(hCanvas, 1, &oldMode);

        LISTVIEW_CUSTOM_DATA draw;
        draw.hbrHot     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
        draw.hbrText    = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crText, 255));
        draw.hbrSel     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crSelected, 255));

        const int nTopIndex     = ListView_GetTopIndex(hWnd);
        const int nItemCount    = ListView_GetItemCount(hWnd);
        const int index         = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);

        RECT rcItem, rcText;
        const int bufSize = 0x1000;
        wchar_t text[bufSize];
        LVITEMW item;
        item.mask = LVIF_INDENT | LVIF_IMAGE | LVIF_STATE | LVIF_TEXT | LVIF_PARAM;
        item.iSubItem = 0;
        item.pszText = text;
        item.cchTextMax = bufSize;

        DWORD style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        bool isIcon = (style & LVS_LIST) == 0;  // 只要为0, 就表示是大图标

        HIMAGELIST hImageList = ListView_GetImageList(hWnd, isIcon ? LVSIL_NORMAL : LVSIL_SMALL);

        int callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, 0, 0);
        BINDWINDOW_INFO* callbackArr = 0;
        if (callbackCount > 0)
        {
            callbackArr = new BINDWINDOW_INFO[callbackCount];
            memset(callbackArr, 0, sizeof(BINDWINDOW_INFO) * callbackCount);
            callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, callbackArr, callbackCount);
        }

        HWND hHeader = ListView_GetHeader(hWnd);
        const int nColumnCount = (int)SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0);

        const DWORD styleList = (DWORD)SendMessageW(hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
        draw.isCheckBox = __query(styleList, LVS_EX_CHECKBOXES);
        
        for (int i = nTopIndex; i < nItemCount; i++)
        {
            item.iItem = i;
            ListView_GetItemRect(hWnd, i, &rcItem, LVIR_BOUNDS);
            if (!Rect_isOverlap(rcItem, ps.rcPaint))    // 不是重画区域就进行下一次循环
                continue;
            ListView_GetItemRect(hWnd, i, &rcText, LVIR_LABEL);
            ListView_GetItem(hWnd, &item);

            item.pszText = text;
            item.cchTextMax = bufSize;
            _listView_DrawItem(hCanvas, item, rcItem, rcText, pData, i, index, nColumnCount, draw);

            if (hImageList || callbackCount > 0)
            {
                RECT_F rcDraw;
                LPEX_IMAGE pIcon = 0;
                LPWINDOW_ICON_DATA ret = _lv_get_icon(callbackArr, callbackCount, item, rcDraw, STATE::NORMAL, pData);
                if (ret)
                    pIcon = ret->hBitmapDx;
                _listView_DrawItem_Icon(hCanvas, item, pIcon, rcDraw, hImageList, pData, i, draw);
            }

        }
        _canvas_SetTextRenderingHint(hCanvas, oldMode, 0);
        s_info->iDraw->EndPaint(ps);
        if (callbackArr)
            delete[] callbackArr;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        LVHITTESTINFO info;
        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);
        int index = ListView_SubItemHitTest(hWnd, &info);

        InvalidateItem(pData, info.iItem, false);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        delete pData->drawScroll;
        pData->drawScroll = 0;
        _canvas_destroy(pData->draw.hCanvas);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSEMOVE:
    {
        if (!pData->draw.isHot)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        // 需要处理热点项
        LVHITTESTINFO info;
        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);
        int index = ListView_SubItemHitTest(hWnd, &info);
        index = info.iItem;
        if (index != -1)
        {
            if (index == pData->draw.indexHot)
            {
                // 本次移动的还是上次的项目, 不需要重画
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
            }
            // 移动的不是上一次的项目, 需要重画上一次的项目, 在设置当前项目
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.dwHoverTime = INFINITE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
            InvalidateItem(pData, index, true);
        }
        else
        {
            // 移动的不在任何项目内, 需要重画之前的热点项
            if (pData->draw.indexHot != -1)
            {
                InvalidateItem(pData, -1, true);
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSELEAVE:
    {
        if (pData->draw.indexHot != -1)
        {
            InvalidateItem(pData, -1, true);
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_GETFONT:
        return (LRESULT)pData->draw.hFont->GetFont();
    case WM_TIMER:
    {
        if (wParam == (WPARAM)pData)
        {
            DWORD style = pData->GetWndLong(hWnd, GWL_STYLE);
            pData->SetWndLong(hWnd, GWL_STYLE, style | LVS_OWNERDRAWFIXED);
            WINDOWPOS pos = { 0 };
            pos.hwnd = hWnd;
            SendMessageA(hWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&pos);

            //SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
            pData->SetWndLong(hWnd, GWL_STYLE, style);
            SendMessageA(hWnd, LVM_UPDATE, 0, 0);
            break;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}




void WndProc_ListView_Header_DrawItem(HDC hdc, HDITEMW& item, RECT& rcItem, LPOBJSTRUCT pData, int index, int nItemCount)
{
    if (index == pData->draw.indexHot)
    {
        FillRect(hdc, &rcItem, pData->draw.color.hbrHot);
    }


    HPEN hPen[] =
    {
        CreatePen(PS_INSIDEFRAME, 1, s_info->themeColor->crToolSeplineLeft()),
        CreatePen(PS_INSIDEFRAME, 1, s_info->themeColor->crToolSeplineRight()),
    };
    HGDIOBJ old = SelectObject(hdc, hPen[1]);

    const int offset = nItemCount != index + 1;
    MoveToEx(hdc, rcItem.right - offset, rcItem.top, 0);
    LineTo(hdc, rcItem.right - offset, rcItem.bottom);

    SelectObject(hdc, hPen[0]);
    MoveToEx(hdc, rcItem.right - offset - 1, rcItem.top, 0);
    LineTo(hdc, rcItem.right - offset - 1, rcItem.bottom);


    SelectObject(hdc, old); // 还原DC

    DeleteObject(hPen[0]);  // 删除两个画笔
    DeleteObject(hPen[1]);
    rcItem.left += 5;
    if (index == pData->draw.indexHot && pData->draw.indexDown != -1)
        OffsetRect(&rcItem, 1, 1);
    DrawTextW(hdc, item.pszText, -1, &rcItem, DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WndProc_ListView_Header(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_ERASEBKGND:
        return WndProc_EraseBkgnd(hWnd, message, wParam, lParam, pData);
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps);
        RECT rcItem;
        
        const int nItemCount = Header_GetItemCount(hWnd);
        int fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

        const int bufSize = 0x1000;
        wchar_t text[bufSize];
        HDITEMW item = { 0 };
        item.mask = HDI_FORMAT | HDI_STATE | HDI_TEXT | HDI_WIDTH;
        item.pszText = text;
        item.cchTextMax = bufSize;
        for (int i = 0; i < nItemCount; i++)
        {
            Header_GetItemRect(hWnd, i, &rcItem);
            Header_GetItem(hWnd, i, &item);

            WndProc_ListView_Header_DrawItem(hdc, item, rcItem, pData, i, nItemCount);
        }

        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_NCPAINT:
        return WndProc_DefNcPaint(hWnd, message, wParam, lParam, pData);
    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if ((wParam & MK_LBUTTON) == 0)
        {
            const int len = Header_GetItemCount(hWnd);
            RECT rcItem;
            for (int i = 0; i < len; i++)
            {
                Header_GetItemRect(hWnd, i, &rcItem);
                if (x >= rcItem.left && x < rcItem.right)
                {
                    // 鼠标移动到某个项目中
                    TRACKMOUSEEVENT tme = { 0 };
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.dwHoverTime = INFINITE;
                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);
                    if (pData->draw.indexHot != i)
                    {
                        pData->draw.indexHot = i;
                        if (pData->draw.isHot)  // 允许热点才重画
                            ::InvalidateRect(hWnd, 0, 0);
                    }
                    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
                }
            }

            if (pData->draw.indexHot != -1)
                ::InvalidateRect(hWnd, 0, 0);
            pData->draw.indexHot = -1;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSELEAVE:
    {
        if (pData->draw.indexHot != -1)
            InvalidateRect(hWnd, 0, 0);
        pData->draw.indexHot = -1;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (pData->draw.indexHot != -1)
        {
            pData->draw.indexDown = 1;
            ::InvalidateRect(hWnd, 0, 0);
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_CAPTURECHANGED:
    case WM_LBUTTONUP:
    {
        if (pData->draw.indexHot != -1)
            ::InvalidateRect(hWnd, 0, 0);
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        const int len = Header_GetItemCount(hWnd);
        if (len > 0)
        {
            RECT rcFirst, rcLast;
            Header_GetItemRect(hWnd, 0, &rcFirst);
            Header_GetItemRect(hWnd, len - 1, &rcLast);
            if (y < 0 || y > rcFirst.bottom - rcFirst.top || x < rcFirst.left || x > rcLast.right)
            {
                pData->draw.indexHot = -1;
                ::InvalidateRect(hWnd, 0, 0);
            }
        }
        pData->draw.indexDown = -1;
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

LRESULT CALLBACK WndProc_ListView_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    switch (hdr->code)
    {
    case NM_CUSTOMDRAW:
    {
        LPOBJSTRUCT pList = GetDataFromHwnd(hdr->hwndFrom);
        if (!pList)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW) lParam;
        switch (lpNMCustomDraw->nmcd.dwDrawStage)
        {
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        //case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
        //{
        //    SelectObject(lpNMCustomDraw->nmcd.hdc, pList->draw.hFont->hFont);
        //    return CDRF_NEWFONT;
        //}
        default:
            return CDRF_DODEFAULT;
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        }
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}


LRESULT CALLBACK ListView_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPMEASUREITEMSTRUCT mi = (LPMEASUREITEMSTRUCT)lParam;
    auto it = m_map_listView.find(mi->CtlID);
    if (it == m_map_listView.end())
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    LPOBJSTRUCT pList = it->second;
    mi->itemHeight = s_info->scale(pList->draw.nItemHeight);
    KillTimer(pList->hWnd, (UINT_PTR)pList);
    return 1;
}
