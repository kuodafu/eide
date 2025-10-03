#include "_draw_top_state.h"



const int ImageOffset = 2;
const int buttonwidth = 18;     // 按钮绘画宽度
const int buttonheight = 18;    // 按钮绘画高度
static HWND s_currentStateWindow;           // 记录当前激活的窗口
static HWND hControl;
static HWND hWork;
static HWND hState;

// 按钮被单击
// hWnd = 触发事件的窗口句柄
// info = 绘画数据
// index = 第几个按钮触发的点击事件
LRESULT CALLBACK WndProc_Top_State_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void WndProc_Top_State_InvalidateItem(HWND hWnd, LPTOPSTATE_INFO info, int index);
void WndProc_Top_State_TrackMouse(HWND hWnd);
// 顶边需要绘画状态条的子类化就指向这个回调函数, 内部处理绘画和点击事件
LRESULT CALLBACK WndProc_Top_State(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Top_State_OnMenuSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// 按钮被单击
// hWnd = 触发事件的窗口句柄
// info = 绘画数据
// index = 第几个按钮触发的点击事件
void WndProc_Top_State_OnButtonClick(HWND hWnd, LPTOPSTATE_INFO info, TOP_STATE_BUTTON btnType, LPOBJSTRUCT pData);


LRESULT CALLBACK WndProc_Top_State_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void calc_button_pos(LPTOPSTATE_INFO info, HWND hWnd);


inline TOP_STATE_BUTTON index2enum(int index)
{
    switch (index)
    {
    case 0: return TOP_STATE_BUTTON_CLOSE;
    case 1: return TOP_STATE_BUTTON_PUSHPIN;
    case 2: return TOP_STATE_BUTTON_TRIANGLE;
    case 3: return TOP_STATE_BUTTON_SWITCH;
    case 4: return TOP_STATE_BUTTON_MAXBOX;
    default: return TOP_STATE_BUTTON_UNKNOWN;
    }
}
inline int enum2index(int nEnum)
{
    switch (nEnum)
    {
    case TOP_STATE_BUTTON_CLOSE:    return 0;
    case TOP_STATE_BUTTON_PUSHPIN:  return 1;
    case TOP_STATE_BUTTON_TRIANGLE: return 2;
    case TOP_STATE_BUTTON_SWITCH:   return 3;
    case TOP_STATE_BUTTON_MAXBOX:   return 4;
    default: return -1;
    }
}

// 子类化需要顶边状态条的
LPOBJSTRUCT _subclass_top_state(HWND hWnd, const RECT& rcOffset)
{
    LPOBJSTRUCT pData = s_info->iDraw->_subclass_tool(hWnd, WndProc_Top_State, true, false, 0);
    if (!pData)return 0;

    pData->draw.hCanvas = _canvas_create(pData->hWnd, 0, 0);
    hControl    = s_info->eWnd->GetControl();
    hWork       = s_info->eWnd->GetWork();
    hState      = s_info->eWnd->GetState();

    pData->SetClsLong(pData->hWnd, GCL_STYLE, CS_VREDRAW | CS_HREDRAW);

    //{
    //    DWORD style = pData->GetClsLong(pData->hWnd, GCL_STYLE);
    //    //style &= ~(CS_VREDRAW | CS_HREDRAW);
    //    //style |= (CS_VREDRAW | CS_HREDRAW);
    //    style |= (CS_PARENTDC | CS_SAVEBITS);
    //    pData->SetClsLong(pData->hWnd, GCL_STYLE, style);
    //}



    HWND hWndTab = 0;
    wstr title;
    LPTOPSTATE_INFO info = new TOPSTATE_INFO;
    memset(info, 0, sizeof(TOPSTATE_INFO));
    int flags = 0;
    if (hWnd == hControl)
    {
        //title = L"组件箱";
        hWndTab = s_info->hWndTabControl;
        info->flags = s_info->config->tab_control_flags();
        flags = info->flags;
        __add_flags(info, TOP_STATE_BUTTON_SWITCH); // 加入组件箱选择按钮状态
    }
    else if (hWnd == hState)
    {
        hWndTab = s_info->eWnd->GetStateTab();
        info->flags = s_info->config->tab_state_flags();
        flags = info->flags;
    }
    else if (hWnd == hWork)
    {
        hWndTab = s_info->eWnd->GetWorkTab();
        info->flags = s_info->config->tab_work_flags();
        flags = info->flags;
    }

    __add_flags(info, TOP_STATE_BUTTON_DEF);

    bool isAutoHide = __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE);  // 如果是自动隐藏, 那就需要选中图标
    bool isFloat = __query_flags(info, TOP_STATE_BUTTON_ISFLOAT);
    if (isFloat)    // 浮动窗口, 有最大化或者还原按钮, 需要删除图钉图标
    {
        info->isRestore = __query_flags(info, TOP_STATE_BUTTON_RESTORE);
        // 加入最大化按钮状态
        __add_flags(info, TOP_STATE_BUTTON_MAXBOX);
        // 去除停靠状态, 图钉按钮
        __del_flags(info, TOP_STATE_BUTTON_ISSTOP | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
    }
    else
    {
        // 不是浮动窗口, 加入停靠状态, 图钉按钮
        __add_flags(info, TOP_STATE_BUTTON_ISSTOP | TOP_STATE_BUTTON_PUSHPIN);
        // 去除浮动/最大化/还原 状态
        __del_flags(info, TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX | TOP_STATE_BUTTON_RESTORE);
    }
    if (isAutoHide) // 浮动窗口优先级比自动隐藏高, 有浮动状态时不加入自动隐藏
    {

        ShowWindow(hWnd, SW_HIDE);  // 有自动隐藏的话, 起来就把它隐藏掉
        //info->flags |= (TOP_STATE_BUTTON_ISAOTUHIDE | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
        __add_flags(info, TOP_STATE_BUTTON_PUSHPIN_SEL | TOP_STATE_BUTTON_ISAOTUHIDE);        // 加入图钉状态
        if (!isFloat)
            __add_flags(info, TOP_STATE_BUTTON_PUSHPIN_SEL);    // 加入图钉选中状态
    }

    std::vector<int>* pArr = Tab_GetArr(s_info->pfnGetDataFromHwnd(hWndTab), 0, 0);
    _drawtab_getarr(pData, pArr, true);
    _drawtab_gettopstate_info(pData, info, true);
    info->index = -1;
    info->hWnd = hWnd;

    info->rcOffset = rcOffset;
    info->isSelect = isAutoHide;
    calc_button_pos(info, hWnd);
    if (flags != info->flags)   // 得到的结果不同, 需要写出配置
    {
        if (hWnd == hControl)
        {
            s_info->config->tab_control_flags(&info->flags);
        }
        else if (hWnd == hState)
        {
            s_info->config->tab_state_flags(&info->flags);
        }
        else if (hWnd == hWork)
        {
            s_info->config->tab_work_flags(&info->flags);
        }
    }
    if (hWndTab && pArr)
    {
        const int ItemCount = TabCtrl_GetItemCount(hWndTab);   // 选择夹项目数
        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT;
        const int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
        float width = 0, height = 0;
        int indexTab = TabCtrl_GetCurSel(hWndTab);
        if ((int)pArr->size() != ItemCount)
            pArr->resize(ItemCount);
        for (int i = 0; i < ItemCount; i++)
        {
            item.pszText = text;
            item.cchTextMax = bufSize;
            TabCtrl_GetItem(hWndTab, i, &item);
            _canvas_calctextsize(s_info->window_d2d_info.hCanvas, pData->draw.hFont, text, -1, fmt, 80, 20, &width, &height);
            int newWidth = (int)width + 10;
            int& arrWidth = pArr->at(i);
            if (arrWidth != newWidth)
                arrWidth = newWidth;
            if (i == indexTab)
                title = text;
        }
    }
    if (!title.empty())
        wcscpy_s(info->name, 20, title.c_str());
    return pData;
}

void calc_button_pos(LPTOPSTATE_INFO info, HWND hWnd)
{
    RECT& rc = info->rcWindow;
    GetClientRect(hWnd, &rc);
    rc.left     += info->rcOffset.left;
    rc.top      += info->rcOffset.top;
    rc.right    += info->rcOffset.right;
    rc.bottom   += info->rcOffset.bottom;

    const int cxItem = rc.right - rc.left;
    const int cyItem = s_info->scale(WINDOW_TOP_STATE);
    RECT rcDraw = { 0, 0, cxItem, cyItem };
    const int _width = s_info->scale(buttonwidth);
    const int _height = s_info->scale(buttonheight);

    int left = 0;
    int top = (cyItem - _height) / 2;
    int index = 0;

    if (__query_flags(info, TOP_STATE_BUTTON_CLOSE))     // 有关闭按钮
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[0] = { left, top, left + _width, top + _height };
        index++;
    }
    if (__query_flags(info, TOP_STATE_BUTTON_MAXBOX))    // 有最大化按钮
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[4] = { left, top, left + _width, top + _height };
        index++;
    }
    if (__query_flags(info, TOP_STATE_BUTTON_PUSHPIN))   // 有图钉按钮
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[1] = { left, top, left + _width, top + _height };
        index++;
    }

    if (__query_flags(info, TOP_STATE_BUTTON_TRIANGLE))  // 有三角按钮
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[2] = { left, top, left + _width, top + _height };
        index++;
    }
    if (__query_flags(info, TOP_STATE_BUTTON_SWITCH))    // 有切换按钮
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[3] = { left, top, left + _width, top + _height };
        index++;
    }

    info->count = index;
    return;
}


// 绘画小按钮
// gp = gdi+绘画图形
// rc = 按钮的位置, 小图标是绘画在这个区域内, 内部计算居中在这个区域里
// index = 绘画第几个按钮, TOP_STATE_BUTTON 枚举常量的值
inline void _draw_icon(LPCANVAS hCanvas, const LPTOPSTATE_INFO info, TOP_STATE_BUTTON index, LPOBJSTRUCT pData)
{
    // iconIndex = 图标在图片中的左边位置, 0=关闭, 1=图钉纵向(固定), 2=图钉横向(浮动), 3=三角, 4=组件箱切换
    LPCWSTR iconIndex;
    int infoIndex;
    if      (index == TOP_STATE_BUTTON_CLOSE)       iconIndex = L"0", infoIndex = 0;
    else if (index == TOP_STATE_BUTTON_PUSHPIN)     iconIndex = L"1", infoIndex = 1;
    else if (index == TOP_STATE_BUTTON_TRIANGLE)    iconIndex = L"3", infoIndex = 2;
    else if (index == TOP_STATE_BUTTON_MAXBOX)      iconIndex = L"5", infoIndex = 4;
    else iconIndex = L"4", infoIndex = 3;
    if (index == TOP_STATE_BUTTON_PUSHPIN && info->isSelect)        iconIndex = L"2";
    else if (index == TOP_STATE_BUTTON_MAXBOX && info->isRestore)   iconIndex = L"6";
    // iconIndex 这个变量已经存放好了该绘画的图标索引


    const RECT& rc = info->rc[infoIndex];
    const int cxIcon = s_info->scale(10);
    const int cyIcon = s_info->scale(10);
    const int cxItem = rc.right - rc.left;
    const int cyItem = rc.bottom - rc.top;

    if (info->index == infoIndex)
    {
        RECT_F rcBack(rc);

        // 需要判断有没有状态, 有状态需要填充一下背景
        LPEX_BRUSH hbrBack = 0;
        if (info->state == STATE::HOT)
        {
            hbrBack = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
        }
        else if (info->state == STATE::DOWN)
        {
            hbrBack = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown, 255));
        }
        if (hbrBack)
            hCanvas->FillRectangle(hbrBack, &rcBack);

    }

    RECT_F rcDest;
    rcDest.left     = (float)(int)(rc.left + (cxItem - cxIcon) / 2);
    rcDest.top      = (float)(int)(rc.top  + (cyItem - cyIcon) / 2);
    rcDest.right    = (float)(int)((rcDest.left + (float)cxIcon));
    rcDest.bottom   = (float)(int)((rcDest.top  + (float)cyIcon));

    RECT_F rcIconDraw;
    LPWINDOW_ICON_DATA iconData = s_info->pResource->TopState_GetIcon(iconIndex, &rcIconDraw, STATE::NORMAL, s_info->dpiX);
    if (iconData)
    {
        hCanvas->DrawImage(iconData->hBitmapDx, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom,
            rcIconDraw.left, rcIconDraw.top, rcIconDraw.right, rcIconDraw.bottom, 255);
    }
}

// 给定一个绘画图形, 绘画位置, 点的颜色 内部往这个hdc上绘画三条点线+填充背景+绘画文本
// hCanvas = 绘画图形
// rcDraw = 绘画的范围, 内部根据这个矩形绘画, 这个一般是顶边0,0 开始到 右边,WINDOW_TOP_STATE 的位置
// pData = 
// rcClient = 窗口的客户区 GetClientRect 的值
void _draw_top_state(LPCANVAS hCanvas, RECT& rcDraw, LPOBJSTRUCT pData, const RECT& rcClient)
{
    hCanvas->SetAntialiasMode(0);
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    s_info->themeColor->crStatelineNormal();
    ARGB crDot, crBack, crText;
    if (s_currentStateWindow == pData->hWnd)
    {
        crDot = s_info->themeColor->crStatelineHot();   crDot = RGB2ARGB(crDot, 255);
        crBack = s_info->themeColor->crStateBar();      crBack = RGB2ARGB(crBack, 255);
        crText = RGB2ARGB(pData->draw.color.crTextHot, 255);
    }
    else
    {
        crDot = s_info->themeColor->crStatelineNormal();    crDot = RGB2ARGB(crDot, 255);
        crBack = RGB2ARGB(pData->draw.color.crBackGround, 255);
        crText = RGB2ARGB(pData->draw.color.crText, 255);
    }

    const bool isTransverse = true;

    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    calc_button_pos(info, pData->hWnd);

    LPEX_BRUSH hbrBack = s_info->pfnCreateBrushDx(crBack);
    LPEX_BRUSH hbrText = s_info->pfnCreateBrushDx(crText);
    LPEX_PEN hPenDot = s_info->pfnCreatePenDx(crDot, 1);
    LPEX_PEN hPenBorder = s_info->pfnCreatePenDx(RGB2ARGB(pData->draw.color.crBorder, 255), 1);
    //LPEX_PEN hPenBorder = s_info->pfnCreatePenDx(RGB2ARGB(255, 255), 1);

    if (pData->hWnd == s_info->hState)
    {
        rcDraw.top += WINDOW_SPACING;
        rcDraw.bottom += WINDOW_SPACING;

    }
    for (int i = 0; i < sizeof(info->rc) / sizeof(info->rc[0]); i++)
    {
        if (info->rc[i].bottom)
        {
            if (pData->hWnd == s_info->hState)
            {
                info->rc[i].top += WINDOW_SPACING;
                info->rc[i].bottom += WINDOW_SPACING;
            }
            if (info->rcWindow.left)
            {
                info->rc[i].left += info->rcWindow.left;
                info->rc[i].right += info->rcWindow.left;
            }
        }
    }
    //wstr::dbg(L"0x%p(%d) rcDraw = {%d, %d, %d, %d} (%d * %d), info->rcWindow = {%d, %d, %d, %d} (%d * %d)\n",
    //    pData->hWnd, pData->hWnd,
    //    rcDraw, rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
    //    info->rcWindow, info->rcWindow.right - info->rcWindow.left, info->rcWindow.bottom - info->rcWindow.top);

    rcDraw.left = info->rcWindow.left;
    rcDraw.right = info->rcWindow.right;


    RECT_F rcFill(rcDraw);
    
    hCanvas->FillRectangle(hbrBack, &rcFill);

    rcFill.bottom = (float)rcClient.bottom - 1;
    rcFill.left++;
    rcFill.top++;
    hCanvas->DrawRectangle(hPenBorder, &rcFill);

    RECT_F rcText(0, 0, 12, 0);
    RECT_F rcOutput;
    if (info->name && info->name[0])
    {
        rcText.left     = (float)rcDraw.left + 3;
        rcText.top      = (float)rcDraw.top;
        rcText.bottom   = (float)(rcDraw.bottom);
        rcText.right    = (float)rcDraw.right;

        const int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX;
        hCanvas->drawtextex(hFont, hbrText, info->name, -1, fmt, &rcText, 0, 0, 0, 0, &rcOutput);
    }

    if (__query_flags(info, TOP_STATE_BUTTON_CLOSE))     // 有关闭按钮
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_CLOSE, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_MAXBOX))    // 有最大化按钮
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_MAXBOX, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_PUSHPIN))   // 有图钉按钮
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_PUSHPIN, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_TRIANGLE))  // 有三角按钮
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_TRIANGLE, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_SWITCH))    // 有切换按钮
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_SWITCH, pData);


    RECT_F rcDot;
    int width  = rcDraw.right  - rcDraw.left;
    int height = rcDraw.bottom - rcDraw.top;

    rcDot.left      = (float)rcDraw.left + (rcOutput.width() + s_info->scale(6));
    rcDot.top       = (float)rcDraw.top  + (((float)(height - s_info->scale(5))) / 2) + 1;
    rcDot.right     = (float)(info->rcWindow.right - (s_info->scale(buttonwidth) * info->count) - s_info->scale(5));
    rcDot.bottom    = rcDot.top + s_info->scale(5);
    _draw_top_state_dot(hCanvas, rcDot, crDot, isTransverse);

}


// 给定一个绘画图形, 绘画位置, 点的颜色 内部往这个hdc上绘画三条点线
// hCanvas = 绘画图形
// rcDraw = 绘画的范围, 内部根据这个矩形绘画
// crDot = 点的颜色
// isTransverse = 是否横向, 为 true 则从左往右绘画, false 则从上往下绘画
void _draw_top_state_dot(LPCANVAS hCanvas, const RECT_F& rcDraw, ARGB crDot, bool isTransverse)
{
    //const float dashArray[] = { (float)s_info->scale(1), (float)s_info->scale(3) };
    const float dashArray[] = { 1.f, 3.f };
    LPEX_PEN hPen = _pen_create(crDot, (float)s_info->scale(1), 5, dashArray, sizeof(dashArray) / sizeof(dashArray[0]));
    LINEPOINT_F pt[3];
    const float x1 = rcDraw.left;
    const float y1 = rcDraw.top;
    const float x2 = rcDraw.right;
    const float y2 = rcDraw.bottom;

    const int n0 = 0;
    const int n2 = s_info->scale(2);
    const int n4 = s_info->scale(4);

    if (isTransverse)
    {
        pt[0](x1 + n0, y1 + n0, x2 - n0, y1 + n0);  // 上面
        pt[1](x1 + n2, y1 + n2, x2 - n2, y1 + n2);  // 中间
        pt[2](x1 + n0, y1 + n4, x2 - n0, y1 + n4);  // 下面

        //gp->DrawLine(&pen, x1 + 0, y1 + 0, x2 - 0, y1 + 0);  // 上面
        //gp->DrawLine(&pen, x1 + 2, y1 + 2, x2 - 2, y1 + 2);  // 中间
        //gp->DrawLine(&pen, x1 + 0, y1 + 4, x2 - 0, y1 + 4);  // 下面
    }
    else
    {
        // 走到这就是纵向绘画, 从上往下绘画
        pt[0](x1 + n0, y1 + n0, x1 - n0, y2 + n0);  // 左边
        pt[1](x1 + n2, y1 + n2, x1 + n2, y2 - n2);  // 中间
        pt[2](x1 + n4, y1 + n0, x1 + n4, y2 - n0);  // 右边

        //gp->DrawLine(&pen, x1 + 0, y1 + 0, x1 - 0, y2 + 0);  // 左边
        //gp->DrawLine(&pen, x1 + 2, y1 + 2, x1 + 2, y2 - 2);  // 中间
        //gp->DrawLine(&pen, x1 + 4, y1 + 0, x1 + 4, y2 - 0);  // 右边
    }
    hCanvas->DrawLine(hPen, &pt[0]);
    hCanvas->DrawLine(hPen, &pt[1]);
    hCanvas->DrawLine(hPen, &pt[2]);
    _pen_destroy(hPen);
}


// 更新当前激活的窗口, 会重画激活的这个窗口和之前激活的窗口
void TopState_UpdateStateWindow(HWND hWnd)
{
    HWND hWndClick = hWnd;

    while (hWnd && hWnd != hControl && hWnd != hWork && hWnd != hState)
    {
        hWnd = (HWND)GetWindowLongPtrW(hWnd, GWLP_HWNDPARENT);
    }
    if (!hWnd)
    {
        hWnd = hWndClick;
        while (hWnd && hWnd != hControl && hWnd != hWork && hWnd != hState)
        {
            hWnd = GetParent(hWnd);
        }
    }
    if (s_currentStateWindow != hWnd)
    {
        if (!hWnd && hWndClick)
        {
            wchar_t cls[260];
            GetClassNameW(hWndClick, cls, 260);
            if (wcscmp(cls, L"ComboLBox") == 0) // 点击了组合框的列表则不处理
                return;
        }

        if (hWnd && (hWnd == hControl || hWnd == hWork || hWnd == hState))
        {
            const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
            if (isLButton)
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);
                LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
                LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
                if (info)
                {
                    const int count = sizeof(info->rc) / sizeof(info->rc[0]);
                    int min = 0x7fffffff;
                    for (int i = 0; i < count; i++)
                    {
                        //if (info->rc[i].left < min)
                        //    min = info->rc[i].left;
                        if (PtInRect(&info->rc[i], pt)) // 在按钮中, 不触发
                            return;
                    }
                }
            }
        }

        HWND hOld = s_currentStateWindow;
        if (s_currentStateWindow)
            InvalidateRect(s_currentStateWindow, 0, 0);
        s_currentStateWindow = hWnd;
        if (s_currentStateWindow)
            InvalidateRect(s_currentStateWindow, 0, 0);
        s_info->pfnMove(INE_STATE_UPDATEHWND, (WPARAM)hOld, (LPARAM)s_currentStateWindow);
    }
}
HWND TopState_GetActivateWindow()
{
    return s_currentStateWindow;
}
void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate)
{
    LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if (!info)return;
    if (!lpszText || !lpszText[0])
    {
        info->name[0] = 0;
    }
    else
    {
        size_t len = wcslen(lpszText) + 1;
        if (len >= 20)len = 20;
        memcpy(info->name, lpszText, len * sizeof(wchar_t));
    }
    if (isUpdate)
        InvalidateRect(hWnd, 0, 0);
}


inline static void CALLBACK TimerProc_LeaveState(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if (info->isDown)   // 按下状态不处理
        return;
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    for (int i = 0; i < sizeof(info->rc) / sizeof(info->rc[0]); i++)
    {
        if (PtInRect(&info->rc[i], pt))
        {
            return;
        }
    }

    if (info->index != -1)
    {
        info->index = -1;
        info->state = STATE::HOT;
        InvalidateRect(hWnd, 0, 0);
    }
    KillTimer(hWnd, id);
}

LRESULT CALLBACK WndProc_Top_State(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //wstr::dbg(L"输出消息, 消息值 = %d\n", message);
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if (!info)
        return DefWindowProcW(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == 0 && lParam == 0)
        {
            return WndProc_Top_State_OnMenuSelect(hWnd, message, wParam, lParam, pData);
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        s_info->iDraw->BeginPaint(pData, ps, true, true);
        RECT rcDraw = { 0, 0, ps.cxClient, s_info->scale(WINDOW_TOP_STATE) };
        _draw_top_state(pData->draw.hCanvas, rcDraw, pData, ps.rc);
        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_NCHITTEST:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        RECT rc;
        GetWindowRect(hWnd, &rc);
        if (hWnd == s_info->hState)         // 状态条是移动顶边
        {
            if (y < rc.top + WINDOW_SPACING)
                return HTTOP;
        }
        else if (hWnd == s_info->hWork)     // 工作夹移动右边
        {
            if (x > rc.right - WINDOW_SPACING)
                return HTRIGHT;
        }
        else if (hWnd == s_info->hControl)  // 组件箱移动左边
        {
            if (x < rc.left + WINDOW_SPACING)
                return HTLEFT;
        }
        return HTCLIENT;
    }
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    {
        const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        const int left = info->rc->right - (s_info->scale(buttonwidth) * info->count + 5);
        SetCapture(hWnd);
        if (pt.x < left)
        {
            info->isDownTitle = true;
            // 鼠标在几个按钮的左边, 这里就是需要移动窗口了
            return _draw_top_state_lbuttondown(pData, info, wParam, pt.x, pt.y);
        }
        info->isDownTitle = false;
        info->isDown = true;
        info->state = STATE::DOWN;
        WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
    {
        if (info->isDownTitle)
            return _draw_top_state_lbuttonup(pData, info, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        ReleaseCapture();
        break;
    }
    case WM_CAPTURECHANGED:
    {
        if (info->isDownTitle)
        {
            info->isDownTitle = false;
            // 不是按下状态那就是移动窗口结束
            return _draw_top_state_capture_changed(pData, info, wParam, lParam);
        }
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);

        if (PtInRect(&info->rc[info->index], pt))  // 放开时鼠标还在原来的项目中
        {
            info->state = STATE::HOT;
            WndProc_Top_State_OnButtonClick(hWnd, info, index2enum(info->index), pData);
        }
        else
        {
            // 鼠标放开不在原来的项目中
            //info->indexHot = -1;
            info->state = STATE::NORMAL;
        }

        WndProc_Top_State_InvalidateItem(hWnd, info, info->index);   // 重画旧项目
        info->index = -1;
        info->isDown = false;
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (info->isDownTitle)
        {
            // && (wParam & MK_LBUTTON) == MK_LBUTTON
            return _draw_top_state_move(pData, info, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return WndProc_Top_State_OnMouseMove(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        if (info)delete info;
        _drawtab_getarr(pData, 0, true);
        _drawtab_gettopstate_info(pData, 0, true);
        _canvas_destroy(pData->draw.hCanvas);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_SIZE:
    {
        calc_button_pos(info, hWnd);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCCALCSIZE:
    {
        if (wParam)
        {
            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;

            return WVR_REDRAW;
        }
        LPRECT prc = (LPRECT)lParam;
        return 0;
    }
    case WM_GETMINMAXINFO:
    {
        // TODO 需要根据当前所在位置决定最大最小位置
        LPMINMAXINFO pos = (LPMINMAXINFO)lParam;
        if (hWnd == s_info->hControl)   // 组件箱最小宽度min, 最大宽度不能大于MDI窗口的左边+mdi
        {
            // 允许调整的最小尺寸
            pos->ptMinTrackSize.x = WINDOW_MINTRACKSIZE;    // 最小拖动宽度
            const int x = s_info->rcMDIClient.left + WINDOW_MIN_MDIWIDTH;
            const int width = s_info->rcControl.right - x;
            pos->ptMaxTrackSize.x = width;  // 最大拖动宽度
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
                pos->ptMinTrackSize.x = WINDOW_AUTO_HEIGHT + WINDOW_SPACING;
        }
        else if (hWnd == s_info->hState)    // 状态夹, 最小高度min, 最大高度不能大于MDI窗口的顶边+mdi
        {
            pos->ptMinTrackSize.y = WINDOW_MINTRACKSIZE;    // 最小拖动高度
            const int y = s_info->rcMDIClient.top + WINDOW_MIN_MDIHEIGHT;    // 顶边是MDI窗口的顶边+mdi
            const int height = s_info->rcState.bottom - y;  // 最大高度 = 状态夹的底边 - MDI顶边+mdi
            pos->ptMaxTrackSize.y = height; // 最大拖动高度
            // 是自动隐藏最小高度就设置成状态条高度+分隔条高度
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            {
                pos->ptMinTrackSize.y = 75;     // WINDOW_TOP_STATE + WINDOW_SPACING;
                pos->ptMaxTrackSize.y = s_info->rcState1.bottom - y; // 最大拖动高度
            }
        }
        else if (hWnd == s_info->hWork)     // 工作夹, 最小宽度 min, 最大宽度不能大于MDI窗口的右边-mdi
        {
            pos->ptMinTrackSize.x = WINDOW_MINTRACKSIZE;    // 最小拖动宽度
            pos->ptMaxTrackSize.x = s_info->rcMDIClient.right - WINDOW_MIN_MDIWIDTH; // 最大拖动宽度

            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
                pos->ptMinTrackSize.x = 120;
        }

        return 0;
    }
    case WM_NCLBUTTONDOWN:
    {
        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return WndProc_CtlColor(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}


void WndProc_Top_State_InvalidateItem(HWND hWnd, LPTOPSTATE_INFO info, int index)
{
    if (index == -1) return;
    InvalidateRect(hWnd, &info->rc[index], 0);
}
void WndProc_Top_State_TrackMouse(HWND hWnd)
{
    TRACKMOUSEEVENT tme = { 0 };
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hWnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&tme);
}

LRESULT CALLBACK WndProc_Top_State_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if ((wParam & MK_LBUTTON) == MK_LBUTTON || (wParam & MK_RBUTTON) == MK_RBUTTON || !info)
    {
        return 0;  // 左键右键按下时不处理
    }

    const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    // 走到这里就是在某个项目内
    for (int i = 0; i < sizeof(info->rc) / sizeof(info->rc[0]); i++)
    {
        if (PtInRect(&info->rc[i], pt))  // 在某个项目中
        {
            WndProc_Top_State_TrackMouse(hWnd);
            if (info->index == i)
                return 0;   // 鼠标还是在上一次的按钮中
            // 走到这就不是在上一次的按钮中了, 这里需要设置新的热点按钮索引, 需要重画按钮

            WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
            info->index = i;
            info->state = STATE::HOT;
            WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
            SetTimer(hWnd, (UINT_PTR)pData, 50, TimerProc_LeaveState);
            return 0;
        }
    }

    // 走到这里就是不是在任何项目内, 需要重画先前热点项
    if (info->index != -1)
    {
        WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
        info->index = -1;
        info->state = STATE::NORMAL;
    }
    return 0;
}

// 按钮被单击
// hWnd = 触发事件的窗口句柄
// info = 绘画数据
// btnType = 哪个按钮被点击, 在触发这个事件的时候这个值是单个标志位
void WndProc_Top_State_OnButtonClick(HWND hWnd, LPTOPSTATE_INFO info, TOP_STATE_BUTTON btnType, LPOBJSTRUCT pData)
{
    if (btnType == TOP_STATE_BUTTON_PUSHPIN)
    {
        info->isSelect = !info->isSelect;
        // 选中就加上自动隐藏和图钉选中状态
        if (info->isSelect)
        {
            info->flags |= (TOP_STATE_BUTTON_ISAOTUHIDE |  TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
        }
        else
        {
            info->flags &= ~(TOP_STATE_BUTTON_ISAOTUHIDE | TOP_STATE_BUTTON_PUSHPIN_SEL);
        }

        
    }
    else if (btnType == TOP_STATE_BUTTON_MAXBOX)
    {
        info->isRestore = !info->isRestore;
        info->flags |= (TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX);
        if (info->isRestore)
        {
            info->flags |= (TOP_STATE_BUTTON_RESTORE);
        }
        else
        {
            info->flags &= ~(TOP_STATE_BUTTON_RESTORE);
        }
    }


    WPARAM wParam = 0;
    switch (btnType)
    {
    case TOP_STATE_BUTTON_TRIANGLE: // 三角按钮被点击
    case TOP_STATE_BUTTON_CLOSE:    // 关闭按钮被点击
        wParam = btnType;
        break;
    case TOP_STATE_BUTTON_PUSHPIN:  // 图钉按钮被点击
    case TOP_STATE_BUTTON_MAXBOX:   // 最大化按钮被点击
    {
        // 图钉按钮/最大化按钮 被点击后都需要改变配置
        if (info->hWnd == hControl)
        {
            s_info->config->tab_control_flags(&info->flags);
        }
        else if (info->hWnd == hState)
        {
            s_info->config->tab_state_flags(&info->flags);
        }
        else if (info->hWnd == hWork)
        {
            s_info->config->tab_work_flags(&info->flags);
        }
        wParam = btnType;
        break;
    }
    case TOP_STATE_BUTTON_SWITCH:   // 组件箱切换显示效果
    {
        // 这个一般只有组件箱有
        LPOBJSTRUCT data = GetDataFromHwnd(s_info->eWnd->GetControl32770Parent());
        if (!data)return;
        RECT rc;
        GetWindowRect(data->hWnd, &rc);
        DWORD pt = MAKELONG(rc.right - 40, rc.top + 12);
        data->oldProc(data->hWnd, WM_NCLBUTTONDOWN, 1000, pt);
        data->oldProc(data->hWnd, WM_NCLBUTTONUP, 1000, pt);
        if (!IsWindowVisible(data->hWnd))
            ShowWindow(data->hWnd, SW_SHOW);
        return;
    }
    default:
        return;
    }
    info->hWnd = hWnd;
    s_info->pfnMove(INE_STATE_CLICK, wParam, (LPARAM)info);

}

LRESULT CALLBACK WndProc_Top_State_OnMenuSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
#define IDMENU_FLOAT            1000    // 浮动(&F)
#define IDMENU_STOP             1001    // 停靠(&K)
#define IDMENU_AUTOHIDE         1002    // 自动隐藏(&T)
#define IDMENU_CLOSE            1003    // 关闭(&C)
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    const int id = LOWORD(wParam);
    switch (id)
    {
    case IDMENU_FLOAT:       // 浮动(&F)
    {
        // 直接加上浮动/最大化图标, 去除图钉图标, 去除停靠状态
        __add_flags(info, TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX);
        __del_flags(info, TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_ISSTOP);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case IDMENU_STOP:        // 停靠(&K)
    {
        // 直接去除浮动/最大化图标/还原图标, 加入图钉图标, 加入停靠状态
        info->flags &= ~(TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX);
        info->flags |= (TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_ISSTOP);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case IDMENU_AUTOHIDE:    // 自动隐藏(&T)
    {
        // 直接设置为true, 能选中这个菜单的原来不是自动隐藏
        // 加上自动隐藏和图钉选中状态
        info->isSelect = true;
        info->flags |= (TOP_STATE_BUTTON_ISAOTUHIDE | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case IDMENU_CLOSE:       // 关闭(&C)
    {

        break;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }


    // 所有菜单被点击都需要保存配置
    if (info->hWnd == hControl)
    {
        s_info->config->tab_control_flags(&info->flags);
    }
    else if (info->hWnd == hState)
    {
        s_info->config->tab_state_flags(&info->flags);
    }
    else if (info->hWnd == hWork)
    {
        s_info->config->tab_work_flags(&info->flags);
    }


    LRESULT ret = s_info->pfnAllWindowProc(hWnd, message, &wParam, &lParam, pData);
    if (pData->isReturn)
        return ret;
    return 0;
}