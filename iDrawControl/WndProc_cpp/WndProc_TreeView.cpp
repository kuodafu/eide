#include "../EWindow_Fne_Header.h"

// 支持库/程序树型框的通知事件
LRESULT CALLBACK WndProc_TreeView_OnNotify_fneTree(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_TreeView_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_TreeView(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// 显示组件箱原图标配置被改变时触发
LRESULT TreeView_ReloadImageList(LPOBJSTRUCT pData);
// 重新加载树型框使用的资源
LRESULT TreeView_ReloadSource(LPOBJSTRUCT pData);


// 树型框绘画数据, 这些画刷每次绘画都从画刷列表里创建, 保证是最新的
struct TREE_DRAW_INFO
{
    LPEX_BRUSH hbrText;
    LPEX_BRUSH hbrHot;
    LPEX_BRUSH hbrSel;
    LPEX_BRUSH hbrDown;
    LOGFONTW hFont;         // 一般字体
    LOGFONTW hFontBold;     // 加粗字体
};

inline TREE_DRAW_INFO* _tree_get_draw(LPOBJSTRUCT pData, TREE_DRAW_INFO* vl, bool isSet)
{
    if (isSet)
        pData->param = vl;
    return (TREE_DRAW_INFO*)pData->param;
}


struct TREE_ICON_INFO
{
    int count;
    LPEX_IMAGE hImage;
};
static std::map<HIMAGELIST, TREE_ICON_INFO> m_map_icon;

// 绘画原来的图片组
void TreeView_DrawItem_ImageList(TREE_DRAW_INFO* draw, LPOBJSTRUCT pData, int iImage, const RECT& rcIcon, HIMAGELIST hList);
void TreeView_DrawItem(BINDWINDOW_INFO* callbackArr, int callbackCount, HTREEITEM hItem, LPOBJSTRUCT pData, int scrollPos, const RECT& rc, HIMAGELIST hImgList, TREE_DRAW_INFO* draw);
inline void IDraw_tree_view_Destroy()
{
    for (auto it = m_map_icon.begin(); it != m_map_icon.end(); ++it)
        _img_destroy(it->second.hImage);
    m_map_icon.clear();
}

LPOBJSTRUCT _subclass_tree(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int Itemheight, bool isScroll, bool isHot)
{
    static bool ffff = false;
    if (!ffff)
    {
        ffff = true;
        IDraw_PushPluginFreeCallback(IDraw_tree_view_Destroy);
    }
    if (Itemheight < 10) Itemheight = 24;
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_TreeView, reMoveBorder, isSetBack, reMoveClassStyle);
    if (!pData)
        return 0;
    pData->draw.nItemHeight = Itemheight;
    SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, s_info->scale(pData->draw.nItemHeight), 0);
    SetWindowLongPtrW(pData->hWnd, GWL_STYLE, GetWindowLongPtrW(pData->hWnd, GWL_STYLE) | TVS_FULLROWSELECT);
    //pData->fnNotify = WndProc_TreeView_OnNotify;
    pData->fnNotify = WndProc_TreeView_OnNotify_fneTree;
    pData->draw.hCanvas = _canvas_create(pData->hWnd, 0, 0);
    pData->draw.isHot = isHot;

    TREE_DRAW_INFO* draw = new TREE_DRAW_INFO;
    _tree_get_draw(pData, draw, true);

    pData->draw.hFont->GetLogfont(&draw->hFont);
    pData->draw.hFont->GetLogfont(&draw->hFontBold);
    draw->hFontBold.lfHeight--;
    draw->hFontBold.lfWeight = FW_BOLD;


    if (isScroll && s_info->MY_MF_OWNERDRAW)
    {
        pData->drawScroll = ScrollCreate(pData->hWnd, pData->draw.color.crBorder,
            0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
            true,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
            true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
            false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
            true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
        );
    }
    return pData;
}
#include <assist/CFileRW.h>
#include <control/CImgList.h>


// 重画选中或者热点项目, 会把前一个热点/按下项目取消, 然后设置新的热点/按下项目
inline static void InvalidateItem(LPOBJSTRUCT pData, HTREEITEM hItem, bool isHot)
{
    RECT rc;
    HTREEITEM* pItem = isHot ? &pData->draw.hItemHot : &pData->draw.hItemSel;
    if (*pItem)
    {
        TreeView_GetItemRect(pData->hWnd, *pItem, &rc, 0);
        ::InvalidateRect(pData->hWnd, &rc, 0);
    }
    *pItem = hItem;
    if (hItem)
    {
        TreeView_GetItemRect(pData->hWnd, hItem, &rc, 0);
        ::InvalidateRect(pData->hWnd, &rc, 0);
    }
}


LRESULT CALLBACK WndProc_TreeView(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (pObj && s_info->MY_MF_OWNERDRAW)
    {
        LRESULT ret = 0;
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            if (message == WM_HSCROLL)
                InvalidateRect(hWnd, 0, 0);
            return ret;
        }
    }
    switch (message)
    {
    case WM_DPICHANGED:
    {
        EWindow_DPIChange(pData);

        SendMessageW(pData->hWnd, TVM_SETITEMHEIGHT, s_info->scale(pData->draw.nItemHeight), 0);
        break;
    }
    case WM_HSCROLL:
        InvalidateRect(hWnd, 0, 0);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    case WM_KEYDOWN:
        return WndProc_TreeView_KeyDown(hWnd, message, wParam, lParam, pData);
    //case WM_SETFONT:
    //{
    //    if (wParam)
    //    {
    //        LOGFONTW lf = { 0 };
    //        if (GetObjectW((HFONT)wParam, sizeof(lf), &lf) == sizeof(lf))
    //        {
    //            // 传入的字体有效
    //            TREE_DRAW_INFO* draw = _tree_get_draw(pData, 0, false);
    //            draw->hFont = s_info->pfnCreateFontLfDx(&lf);

    //            lf.lfWeight = FW_BOLD;
    //            lf.lfHeight--;
    //            draw->hFontBold = s_info->pfnCreateFontLfDx(&lf);
    //        }
    //    }

    //    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //}
    case WM_DESTROY:
    {
        TREE_DRAW_INFO* draw = _tree_get_draw(pData, 0, false);
        _tree_get_draw(pData, 0, true);
        delete draw;

        delete pData->drawScroll;
        pData->drawScroll = 0;
        _canvas_destroy(pData->draw.hCanvas);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_ERASEBKGND:
        return WndProc_EraseBkgnd(hWnd, message, wParam, lParam, pData);
    case WM_LBUTTONDBLCLK:
    {
        //if (hWnd != s_info->eWnd->GetWorkProgramTree())
        //    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        if (pData->draw.hItemSel && TreeView_GetChild(hWnd, pData->draw.hItemSel))
        {
            LRESULT ret = 0;
            if (hWnd == s_info->eWnd->GetWorkProgramTree())
            {
                TVITEMEXW item = { 0 };
                item.hItem = pData->draw.hItemSel;
                item.mask = TVIF_PARAM;
                TreeView_GetItem(pData->hWnd, &item);
                int type = HIWORD(item.lParam);
                int index = LOWORD(item.lParam);
                const int dbltype = s_info->config->programTreeDblclk();  //  0=只切换, 1=只展开, 2=展开+切换
                if (dbltype == 0)           // 只切换
                {
                    ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData); // 易语言本身的处理就是只切换不展开
                }
                else if (dbltype == 1)      // 只展开
                {
                    TreeView_Expand(hWnd, pData->draw.hItemSel, TVE_TOGGLE);
                }
                else
                {
                    // 切换+展开
                    // 以下这些分类都是需要调用原来的双击事件
                    if (type == 0x2000 ||       // 全局变量
                        type == 0x3000 ||       // 自定义数据类型
                        type == 0x4000 ||       // dll命令
                        type == 0x8000 ||       // 模块引用, 这个也需要调用原来的处理
                        type >= 0x1000 && type < 0x2000)    // 程序集/类
                        ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
                    TreeView_Expand(hWnd, pData->draw.hItemSel, TVE_TOGGLE);
                }

                return ret;
            }

        }
        TVHITTESTINFO ht;
        ht.pt.x = GET_X_LPARAM(lParam);
        ht.pt.y = GET_Y_LPARAM(lParam);
        HTREEITEM hItem = TreeView_HitTest(hWnd, &ht);
        if (hItem)
        {
            RECT rc;
            TreeView_GetItemRect(pData->hWnd, hItem, &rc, true);
            if (ht.pt.x > rc.left)
            {
                ht.pt.x = rc.left + 5;
                ht.pt.y = rc.top + 5;
                lParam = MAKELONG(ht.pt.x, ht.pt.y);
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDOWN:
    {
        if (message == WM_LBUTTONDOWN)
            SetFocus(hWnd);
        TVHITTESTINFO ht;
        ht.pt.x = GET_X_LPARAM(lParam);
        ht.pt.y = GET_Y_LPARAM(lParam);
        HTREEITEM hItem = TreeView_HitTest(hWnd, &ht);
        if (hItem)
        {
            RECT rc;
            TreeView_GetItemRect(pData->hWnd, hItem, &rc, true);
            HTREEITEM hChild = TreeView_GetChild(pData->hWnd, hItem);
            if (hChild) // 有子项, 且点击的位置大于文本左边
            {
                if (ht.pt.x > rc.left)
                {
                    ht.pt.x = rc.left + 1;
                    ht.pt.y = rc.top + 1;
                }
            }
            else if (!PtInRect(&rc, ht.pt))     // 没有子项的话都视为在文本中点击
            {
                ht.pt.x = rc.left + 1;
                ht.pt.y = rc.top + 1;
            }
            lParam = MAKELONG(ht.pt.x, ht.pt.y);
            TreeView_Select(hWnd, hItem, TVGN_CARET);
            if (pData->draw.hItemSel != hItem)
            {
                InvalidateItem(pData, hItem, false);
            }
            //InvalidateItem(pData, hItem, false);
        }

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    //case WM_LBUTTONUP:
    //{

    //}
    case WM_SIZE:
    {
        _canvas_resize(pData->draw.hCanvas, LOWORD(lParam), HIWORD(lParam));
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_NCHITTEST:
    {
        if (s_info->MY_MF_OWNERDRAW)
        {
            const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            const int isBorder = IsBorder(pData->style, pData->styleEx);
            RECT rc;
            GetWindowRect(hWnd, &rc);
            if (isBorder)
                rc.left++, rc.top++, rc.right--, rc.bottom--;

            if (pObj)
            {
                LONG vThumbSize = (LONG)(pObj->m_vInfo.btnThumbSize);
                LONG hThumbSize = (LONG)(pObj->m_hInfo.btnThumbSize);
                if (pObj->m_isVScroll)
                    rc.right -= vThumbSize;
                if (pObj->m_isHScroll)
                    rc.bottom -= hThumbSize;
            }

            if (PtInRect(&rc, pt))
                return HTCLIENT;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSEMOVE:
    {
        if (!pData->draw.isHot)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        // 需要处理热点项
        TVHITTESTINFO ht = { 0 };
        ht.pt.x = GET_X_LPARAM(lParam);
        ht.pt.y = GET_Y_LPARAM(lParam);
        HTREEITEM hItem = TreeView_HitTest(hWnd, &ht);
        if (hItem)
        {
            if (hItem == pData->draw.hItemHot)
            {
                // 本次移动的还是上次的句柄, 不需要重画
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
            }
            // 移动的不是上一次的项目, 需要重画上一次的项目, 在设置当前项目

            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.dwHoverTime = INFINITE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
            InvalidateItem(pData, hItem, true);
        }
        else
        {
            // 移动的不在任何项目内, 需要重画之前的热点项
            if (pData->draw.hItemHot)
            {
                InvalidateItem(pData, 0, true);
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSELEAVE:
    {
        if (pData->draw.hItemHot)
        {
            InvalidateItem(pData, 0, true);
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps, true, true);
        LPCANVAS hCanvas = pData->draw.hCanvas;
        RECT rcItem = { 0 };
        int oldMode = 0;
        _canvas_SetTextRenderingHint(hCanvas, 1, &oldMode);
        HTREEITEM hItem = TreeView_GetNextItem(hWnd, 0, TVGN_FIRSTVISIBLE);
        int visibleCount = TreeView_GetVisibleCount(hWnd) + 1;
        const int scrollPos = GetScrollPos(hWnd, SB_HORZ);  // 横向滚动条位置
        HIMAGELIST hImgList = TreeView_GetImageList(pData->hWnd, TVSIL_NORMAL);

        TREE_DRAW_INFO* draw = _tree_get_draw(pData, 0, false);
        draw->hbrText    = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crText, 255));
        draw->hbrHot     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
        draw->hbrSel     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crSelected, 255));
        draw->hbrDown    = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown, 255));


        int callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, 0, 0);
        BINDWINDOW_INFO* callbackArr = 0;
        if (callbackCount > 0)
        {
            callbackArr = new BINDWINDOW_INFO[callbackCount];
            memset(callbackArr, 0, sizeof(BINDWINDOW_INFO) * callbackCount);
            callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, callbackArr, callbackCount);
        }

        for (int i = 0; i < visibleCount; i++)
        {
            if (hItem == 0) break;
            TreeView_GetItemRect(pData->hWnd, hItem, &rcItem, false);
            // 项目的顶边大于重画的区域的底边, 项目在重画区域底下, 不需要重画
            // 项目的底边小于重画的区域顶边, 项目在重画区域的上面, 不需要重画
            if (Rect_isOverlap(rcItem, ps.rcPaint))
            //if (!(rcItem.bottom < ps.rcPaint.top || rcItem.top > ps.rcPaint.bottom))
            {
                TreeView_DrawItem(callbackArr, callbackCount, hItem, pData, scrollPos, rcItem, hImgList, draw);
            }

            hItem = TreeView_GetNextItem(hWnd, hItem, TVGN_NEXTVISIBLE);
        }

        _canvas_SetTextRenderingHint(hCanvas, oldMode, 0);

        s_info->iDraw->EndPaint(ps);
        if (callbackArr)
            delete[] callbackArr;
        break;
    }
    case TVM_SETITEMW:
    {
        if (wParam == 121007124 && lParam == 20752843)
            return TreeView_ReloadImageList(pData);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_NCPAINT:
        if (wParam == 121007124 && lParam == 20752843)
            return TreeView_ReloadImageList(pData);
        return WndProc_DefNcPaint(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

// 获取程序树型框的ID, 会从配置获取对应的文本颜色, crText == -1表示没有找到这个颜色
inline LPCWSTR _tree_program_get_id(TVITEMEXW& item, COLORREF& crText)
{
#define _PROTREE_TOSTR(_type) L#_type
#define _PROTREE_RET(_type, _cr) if(type == _type) { if (index == 0) { crText = _cr; return _PROTREE_TOSTR(_type); } return (_PROTREE_TOSTR(_type) ## "FFFF"); }

    // 程序树型框需要从项目数值里获取图标
    crText = -1;
    int type = HIWORD(item.lParam);
    int index = LOWORD(item.lParam);
    
    // 以下宏都是判断type 是否和传入的参数相等, 相等就判断 index 是否为0, 为0就是顶层项目, 不为0就是子项目
    _PROTREE_RET(0x0000, -1);   // 程序数据
    _PROTREE_RET(0x2000, s_info->themeColor->crTab_text_globalvar() );   // 全局变量
    _PROTREE_RET(0x3000, s_info->themeColor->crTab_text_struct()    );   // 自定义数据类型
    _PROTREE_RET(0x4000, s_info->themeColor->crTab_text_dll()       );   // dll命令
    _PROTREE_RET(0x5000, s_info->themeColor->crTab_text_window()    );   // 窗口
    _PROTREE_RET(0x6000, s_info->themeColor->crTab_text_const()     );   // 常量
    //_PROTREE_RET(0x8000);       // 模块引用表
    _PROTREE_RET(0x9000, -1);   // 外部文件记录表
    if (type == 0x8000)
    {
        if (index == 0) return L"0x8000";
        return ((item.iImage == 15) ? (L"0x8000FFFE") : (L"0x8000FFFF"));   // 模块引用表, E=继续公开, F=加载的模块
    }
    if (type == 0x7000)
    {
        if (index == 0) return L"0x7000";
        crText = (index == 1) ? (s_info->themeColor->crTab_text_image()) : (s_info->themeColor->crTab_text_sound());
        return ((index == 1) ? (L"0x70000001") : (L"0x70000002") );         // 1=图片, 2=声音
    }
    if (type == 0xF000)     // 文件夹需要处理是否已经打开
    {
        if (item.cChildren && (item.state & TVIS_EXPANDED)) return L"0xF001";
        return L"0xF000";
    }
    if (type >= 0x1000 && type < 0x2000)    // 类和子程序有点问题, 需要特别处理, 需要根据图标索引来处理
    {
        // 0=类, 3=程序集
        if (item.iImage == 0)
        {
            if (index == 0)
            {
                crText = s_info->themeColor->crTab_text_class();
                return L"0x1001";
            }
            return L"0x1001FFFF";     // 图片索引0=类
        }
        if(index == 0)
            crText = s_info->themeColor->crTab_text_assembly();
        return ((index == 0) ? (L"0x1000") : (L"0x1000FFFF") );         // 否则就是程序集
    }
#ifdef _DEBUG
    __debugbreak();
#endif
    return item.pszText;
}

inline LPWINDOW_ICON_DATA _tv_get_icon(BINDWINDOW_INFO* callbackArr, int callbackCount, LPCWSTR pszID, TVITEMEXW& item, RECT_F& rcDraw, int state, LPOBJSTRUCT pData)
{
    LPEX_IMAGE pIcon = 0;
    for (int i = 0; i < callbackCount; i++)
    {
        BINDWINDOW_INFO& callbackItem = callbackArr[i];
        if (callbackItem.callback)
        {
            LPWINDOW_ICON_DATA ret = callbackItem.callback(pData->hWnd, callbackItem.hResource, pszID, &item, &rcDraw, (STATE)state, s_info->dpiX);
            //if (!ret)
            //{
            //    RECT rcTmp = { 0 };
            //    ret = s_info->pResource->User_GetIcon(callbackItem.hResource, pszID, &rcTmp, (STATE)state, 0);
            //    rcDraw(rcTmp);
            //}
            if (ret) return ret;
        }
    }
    return 0;
}

// 获取支持库树型框图标, 支持库树型框需要处理的东西比较多
// 1. 先从用户绑定的回调里获取
// 2. 在从默认资源插件里获取, 会判断是不是用户图标, 如果不是那就从图片组索引查找
inline LPWINDOW_ICON_DATA __tree_fne_geticon(BINDWINDOW_INFO* callbackArr, int callbackCount, TVITEMEXW& item, RECT_F& rcDraw, STATE state, int* cxIcon, int* cyIcon, LPOBJSTRUCT pData)
{
    int iconType = 0;
    LPWINDOW_ICON_DATA ret = 0;
    if (callbackArr && callbackCount > 0)
    {
        ret = _tv_get_icon(callbackArr, callbackCount, item.pszText, item, rcDraw, state, pData);
        if (ret)
        {
            *cxIcon = (int)(rcDraw.width());
            *cyIcon = (int)(rcDraw.height());
            return ret;
        }
    }
    ret = s_info->pResource->TreeFne_GetIcon(item.pszText, &rcDraw, state, s_info->dpiX);
    if (!ret)return 0;
    iconType = ret->nIconType;
    *cxIcon = (int)(rcDraw.width());
    *cyIcon = (int)(rcDraw.height());
    LPEX_IMAGE pIcon = ret->hBitmapDx;
    if (iconType != 0)  // 返回的不是用户的图标, 需要根据图标索引找用户的图标, 如果都没有找到就返回 pIcon
    {
        wchar_t id[50];
        swprintf_s(id, 50, L"0x%04X", item.iImage);
        RECT_F rc;
        LPWINDOW_ICON_DATA ret1 = s_info->pResource->TreeFne_GetIcon(id, &rc, state, s_info->dpiX);
        iconType = ret1 ? ret1->nIconType : 0;
        LPEX_IMAGE pIconIndex = ret1 ? ret1->hBitmapDx : 0;
        if (iconType == 0)  // 从图标索引能找到用户图标, 那就返回
        {
            *cxIcon = (int)(rcDraw.width());
            *cyIcon = (int)(rcDraw.height());
            rcDraw = rc;
            return ret1;
        }
        // 走到这就是从图标索引找不到图标, 按第一次查找到的结果返回
    }
    return ret;
}

// 获取组件箱树型框图标
// 1. 从用户绑定的回调里获取
// 2. 根据标题从资源插件里获取
inline LPWINDOW_ICON_DATA __tree_control_geticon(BINDWINDOW_INFO* callbackArr, int callbackCount, TVITEMEXW& item, RECT_F& rcDraw, STATE state, int* cxIcon, int* cyIcon, LPOBJSTRUCT pData)
{
    if (s_info->config->tree_control_orgIcon())
        return 0;   // 显示原始图标, 
    int iconType = 0;
    LPWINDOW_ICON_DATA ret = 0;
    if (callbackArr && callbackCount > 0)
    {
        ret = _tv_get_icon(callbackArr, callbackCount, item.pszText, item, rcDraw, state, pData);
    }
    else
    {
        ret = s_info->pResource->TreeControl_GetIcon(item.pszText, &rcDraw, state, s_info->dpiX);
    }
    if (ret)
    {
        *cxIcon = (int)(rcDraw.width());
        *cyIcon = (int)(rcDraw.height());
    }
    return ret;
}

// 获取程序树型框图标
// 1. 从用户绑定的回调里获取
// 2. 根据标题从资源插件里获取
inline LPWINDOW_ICON_DATA __tree_program_geticon(BINDWINDOW_INFO* callbackArr, int callbackCount, TVITEMEXW& item, RECT_F& rcDraw, STATE state, int* cxIcon, int* cyIcon, LPOBJSTRUCT pData, ARGB& crText)
{
    LPCWSTR id = _tree_program_get_id(item, crText);
    crText = (crText == -1) ? 0 : RGB2ARGB(crText, 255);

    int iconType = 0;
    LPWINDOW_ICON_DATA ret = 0;
    if (callbackArr && callbackCount > 0)
    {
        ret = _tv_get_icon(callbackArr, callbackCount, id, item, rcDraw, state, pData);
    }
    else
    {
        ret = s_info->pResource->TreeProgram_GetIcon(id, &rcDraw, state, s_info->dpiX);
    }
    if (ret)
    {
        *cxIcon = (int)(rcDraw.width());
        *cyIcon = (int)(rcDraw.height());
    }
    return ret;
}

void TreeView_DrawItem(BINDWINDOW_INFO* callbackArr, int callbackCount, HTREEITEM hItem, LPOBJSTRUCT pData, int scrollPos, const RECT& rcItem, HIMAGELIST hList, TREE_DRAW_INFO* draw)
{
    RECT rcText;
    RECT_F rcIcon, rcExpand;
    int cxIcon = 0;
    int cyIcon = 0;
    const int bufSize = 0x1000;
    wchar_t szText[bufSize];
    TVITEMEXW item = { 0 };
    item.hItem = hItem;
    item.pszText = szText;
    item.cchTextMax = bufSize;
    item.mask = TVIF_STATE | TVIF_CHILDREN | TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
    TreeView_GetItem(pData->hWnd, &item);
    TreeView_GetItemRect(pData->hWnd, hItem, &rcText, true);
    //rcText.left -= scrollPos;
    rcText.right = rcItem.right;

    ARGB crText = 0;
    RECT_F rcDraw;
    LPEX_IMAGE pIcon = 0;
    
    static HWND hTreeFne        = s_info->eWnd->GetWorkFneTree();
    static HWND hTreeProgram    = s_info->eWnd->GetWorkProgramTree();
    static HWND hTreeControl    = s_info->eWnd->GetControlTree();
    STATE state = STATE::NORMAL;
    LPWINDOW_ICON_DATA retData = 0;
    if (pData->hWnd == hTreeFne)
        retData = __tree_fne_geticon(callbackArr, callbackCount, item, rcDraw, state, &cxIcon, &cyIcon, pData);
    else if (pData->hWnd == hTreeControl)
        retData = __tree_control_geticon(callbackArr, callbackCount, item, rcDraw, state, &cxIcon, &cyIcon, pData);
    else if (pData->hWnd == hTreeProgram)
        retData = __tree_program_geticon(callbackArr, callbackCount, item, rcDraw, state, &cxIcon, &cyIcon, pData, crText);
    else
        retData = _tv_get_icon(callbackArr, callbackCount, szText, item, rcDraw, state, pData);
    
    if (retData)
    {
        cxIcon = (int)rcDraw.width();
        cyIcon = (int)rcDraw.height();
        pIcon = retData->hBitmapDx;
    }
    if (rcDraw.right <= 0 && hList)
    {
        ImageList_GetIconSize(hList, &cxIcon, &cyIcon);
        // 是组件箱, 并且是显示原图标, 那就使用图片组的尺寸
        const bool isSize = (pData->hWnd == hTreeControl && s_info->config->tree_control_orgIcon());
        if (!isSize && (cxIcon || cyIcon))
            cxIcon = 16, cyIcon = 16;

        //if ((cxIcon || cyIcon))
        //    cxIcon = 16, cyIcon = 16;
    }

    rcIcon.right = static_cast<float>(rcText.left - (cxIcon ? 3 : 0));
    //if (rcIcon.right < (float)cxIcon)rcIcon.right = (float)cxIcon;
    rcIcon.left     = static_cast<float>(rcIcon.right - cxIcon);
    rcIcon.top      = static_cast<float>(rcItem.top + ((rcItem.bottom - rcItem.top - cyIcon) / 2));
    rcIcon.bottom   = static_cast<float>(rcIcon.top + cyIcon);
    

    rcExpand.right  = static_cast<float>(rcIcon.left - 2);
    rcExpand.left   = static_cast<float>(rcExpand.right - 16);
    rcExpand.top    = static_cast<float>(rcItem.top + ((rcItem.bottom - rcItem.top - 16) / 2));
    rcExpand.bottom = static_cast<float>(rcExpand.top + 16);

    


    LPCANVAS hCanvas = pData->draw.hCanvas;
    state = STATE::NORMAL;
    LPEX_BRUSH hbrBack = 0;
    if (hItem == pData->draw.hItemSel)
    {
        hbrBack = draw->hbrSel;
        state = STATE::DOWN;
    }
    else if (hItem == pData->draw.hItemHot)
    {
        hbrBack = draw->hbrHot;
        state = STATE::HOT;
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
    state = STATE::NORMAL;

    if (pIcon && rcDraw.right > 0)
    {
        bool isOk = _canvas_drawimagerectrect(hCanvas, pIcon,
            rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom,
            rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            255);
        //wstr::dbg(L"自定义图标 %d, 错误码 = %d, 图片信息 = {%d, 0x%08X, %d, %d, %d, %d, 0x%08X, 0x%08X, 0x%08X, 0x%08X}\n", isOk, ExGetLastError(), (*pIcon));

    }
    else
    {
        TreeView_DrawItem_ImageList(draw, pData, item.iImage, rcIcon, hList);
    }

    if (item.cChildren && rcExpand.right >= 0.f)
    {
        const LPCWSTR pszIDExpand = (item.state & TVIS_EXPANDED) ? L"1" : L"0";
        LPWINDOW_ICON_DATA iconData = s_info->pResource->TreeExpand_GetIcon(pszIDExpand, &rcDraw, STATE::NORMAL, s_info->dpiX);
        if (iconData)
            _canvas_drawimagerectrect(hCanvas, iconData->hBitmapDx, rcExpand.left, rcExpand.top, rcExpand.right, rcExpand.bottom,
                rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 255);
    }

    RECT_F rcTextF(rcText);
    LPCWSTR pDrawStr = szText;
    while (pDrawStr[0] == '\r' || pDrawStr[0] == '\n')
        pDrawStr++;

    
    LPEX_FONT hFont = s_info->pfnCreateFontLfDx(&draw->hFont);
    if (item.state & TVIS_BOLD)
    {
        hFont = s_info->pfnCreateFontLfDx(&draw->hFontBold);
    }
    hFont = g_scale.GetFont(hFont);
    const int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX;
    LPEX_BRUSH hbrText = draw->hbrText;
    if (crText != 0)
        hbrText = s_info->pfnCreateBrushDx(crText);
    _canvas_drawtext(hCanvas, hFont, hbrText, pDrawStr, -1, fmt, &rcTextF);
}

LRESULT TreeView_ReloadImageList(LPOBJSTRUCT pData)
{
    HIMAGELIST hList = TreeView_GetImageList(pData->hWnd, TVSIL_NORMAL);
    if (!hList) return 0;
    TREE_ICON_INFO& iconInfo = m_map_icon[hList];
    _img_destroy(iconInfo.hImage);
    iconInfo.count = 0;
    return 0;
}

// 绘画原来的图片组
void TreeView_DrawItem_ImageList(TREE_DRAW_INFO* draw, LPOBJSTRUCT pData, int iImage, const RECT& rcIcon, HIMAGELIST hList)
{
    if (!hList)return;

    const int count = ImageList_GetImageCount(hList);
    if (iImage < 0 || iImage >= count) return;
    const int cxIcon = rcIcon.right - rcIcon.left;
    const int cyIcon = rcIcon.bottom - rcIcon.top;

    TREE_ICON_INFO& iconInfo = m_map_icon[hList];
    if (iconInfo.count != count)
    {
        _img_destroy(iconInfo.hImage);
        iconInfo.count = count;

        LPMEMDCBITMAP md = md_create(cxIcon * count, cyIcon);
        Gdiplus::Graphics gp(md->hdc);
        for (int i = 0; i < count; i++)
        {
            HICON hIcon = ImageList_GetIcon(hList, i, ILD_NORMAL);
            Gdiplus::Bitmap bmp(hIcon);
            gp.DrawImage(&bmp, i * cxIcon, 0,cxIcon, cyIcon);
            DestroyIcon(hIcon);
        }
        int size = 0;
        LPBYTE pngData = GetPngDataFromBitmap(md->hBitmap, &size, md->hdc);
        iconInfo.hImage = _img_create_frommemory(pngData, size);
        delete[] pngData;
        md_destroy(md);
    }

    LPEX_IMAGE hImg = iconInfo.hImage;
    RECT_F rcDraw((float)(cxIcon * iImage), 0, (float)hImg->GetWidth(), (float)cyIcon);
    rcDraw.right = rcDraw.left + cxIcon;
    bool isOk = _canvas_drawimagerectrect(pData->draw.hCanvas, hImg,
        (float)rcIcon.left, (float)rcIcon.top, (float)rcIcon.right, (float)rcIcon.bottom,
        rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
        255);

    //wstr::dbg(L"是否成功 = %d, 错误码 = %d, 图片信息 = {%d, 0x%08X, %d, %d, %d, %d, 0x%08X, 0x%08X, 0x%08X, 0x%08X}\n", isOk, ExGetLastError(), (*hImg));
}

LRESULT CALLBACK WndProc_TreeView_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    return WndProc_TreeView_OnNotify_fneTree(hWnd, message, wParam, lParam, pData);
    LPNMHDR hdr = (LPNMHDR)lParam;
    static HWND hFnd = s_info->eWnd->GetWorkFneTree();
    if (hdr->hwndFrom == hFnd)
        return WndProc_TreeView_OnNotify_fneTree(hWnd, message, wParam, lParam, pData);
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}
LRESULT CALLBACK WndProc_TreeView_OnNotify_fneTree(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    //_str::dbg("hde->code = %d, hWnd = %d, hWnd = 0x%08X\n", hdr->code, hdr->hwndFrom, hdr->hwndFrom);
    switch (hdr->code)
    {
    //case NM_CLICK:
    //case NM_RCLICK:
    //case TVN_SELCHANGED:
    //case TVN_SELCHANGINGA:  // 正在改变表项
    //case TVN_SELCHANGINGW:  //
    case TVN_SELCHANGEDA:   // 已经改变表项
    case TVN_SELCHANGEDW:
    {
        LPNMTREEVIEWW item = (LPNMTREEVIEWW)lParam;
        HTREEITEM hItem = item->itemNew.hItem;
        LPOBJSTRUCT pChild = GetDataFromHwnd(hdr->hwndFrom);
        if(!pChild)
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

        while (true)
        {
            if (hdr->code == TVN_SELCHANGING)
            {
                InvalidateItem(pChild, hItem, false);
                break;
            }
            if (hdr->code == TVN_SELCHANGED)
            {
                InvalidateItem(pChild, hItem, false);
                break;
            }
            if (hdr->code == NM_RCLICK || hdr->code == NM_CLICK)
            {
                TVHITTESTINFO ht;
                GetCursorPos(&ht.pt);
                ScreenToClient(hdr->hwndFrom, &ht.pt);
                hItem = TreeView_HitTest(hdr->hwndFrom, &ht);
                if (!hItem)
                    return 0;
            }

            InvalidateItem(pChild, hItem, false);
            if (hdr->code == NM_RCLICK)
            {
                TreeView_SelectItem(hdr->hwndFrom, hItem);
            }
            break;
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        break;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

