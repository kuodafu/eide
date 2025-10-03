#include "../EWindow_Fne_Header.h"
#include <assist/assist_Resource.h>
#include <zlib/Czlib.h>
#include <iDraw_const.h>
#include <control/Ctips.h>
#include <assist/assist.h>
#include "../EWindow.h"





LRESULT CALLBACK WndProc_ToolBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ToolBar_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ToolBar_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ToolBar_CalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// data = 按钮指向的数据
// pArg = 新设置的数据
// isSet = 是否设置, 如果为true, 则把pArg拷贝到data, 否则判断pArg各个值, 为0则不设置
bool _Tool_SetButtonData(IDRAW_STRUCT_TOOL_INSERTEX& data, LPIDRAW_STRUCT_TOOL_INSERT pArg, bool isSet);



LPOBJSTRUCT _subclass_tool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!hWnd)
        return 0;
    LPOBJSTRUCT pData = s_info->iDraw->_subclass_tool(hWnd, WndProc_ToolBar, reMoveBorder, isSetBack, reMoveClassStyle);
    pData->fnNotify = WndProc_ToolBar_OnNotify;
    pData->fnCommand = WndProc_ToolBar_OnCommand;
    pData->draw.hCanvas = _canvas_create(hWnd, 0, 0);

    return pData;
}

// 根据id获取绘画图标, 返回绘画需要的位图和图标在位图里的位置
inline static LPEX_IMAGE _tool_get_icon(LPOBJSTRUCT pData, TBBUTTONINFOW& item, RECT_F* prc, STATE state, BINDWINDOW_INFO* callbackArr, int callbackCount)
{
    wchar_t id[50];
    swprintf_s(id, 50, L"%d", item.idCommand);
    for (int i = 0; i < callbackCount; i++)
    {
        BINDWINDOW_INFO& callbackItem = callbackArr[i];
        if (callbackItem.callback)
        {
            LPWINDOW_ICON_DATA ret = callbackItem.callback(pData->hWnd, callbackItem.hResource, id, &item, prc, (STATE)state, 0);
            if (!ret && callbackItem.hResource)
                ret = s_info->pResource->User_GetIcon(callbackItem.hResource, id, prc, (STATE)state, s_info->dpiX);
            if (ret)
                return ret->hBitmapDx;
        }
    }
    LPWINDOW_ICON_DATA retData = s_info->pResource->Tool_GetIcon(id, prc, state, s_info->dpiX);
    return retData ? retData->hBitmapDx : 0;
}


LRESULT CALLBACK WndProc_ToolBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    static HWND hWndNormal, hWndAlign;
    switch (message)
    {
    case WM_PAINT:
    {
        //if (hWnd == s_info->hWndTool)
        //    return tool_Onpaint_Interface(pData);

        BEGIN_END_PAINT_STRUCT ps;
        s_info->iDraw->BeginPaint(pData, ps, true, true);

        //s_info->iDraw->EndPaint(ps);
        //return 0;

        RECT rcItem;
        LPCANVAS hCanvas = pData->draw.hCanvas;
        hCanvas->SetAntialiasMode(0);
        const int index = SendMessageW(hWnd, TB_GETHOTITEM, 0, 0);      // 当前工具条热点索引
        const int len = SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);       // 当前工具条数量

        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TBBUTTONINFOW item = { 0 };
        item.cbSize = sizeof(item);
        item.dwMask = TBIF_BYINDEX | TBIF_IMAGE | TBIF_LPARAM | TBIF_STATE | TBIF_STYLE | TBIF_TEXT | TBIF_COMMAND;


        int callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, 0, 0);
        BINDWINDOW_INFO* callbackArr = 0;
        if (callbackCount > 0)
        {
            callbackArr = new BINDWINDOW_INFO[callbackCount];
            memset(callbackArr, 0, sizeof(BINDWINDOW_INFO) * callbackCount);
            callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, callbackArr, callbackCount);
        }

        LPEX_BRUSH hbrHot  = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
        LPEX_BRUSH hbrDown = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown, 255));
        for (int i = 0; i < len; i++)
        {
            SendMessageW(hWnd, TB_GETITEMRECT, i, (LPARAM)&rcItem);
            if (!Rect_isOverlap(rcItem, ps.rcPaint))    // 不是重画区域就进行下一次循环
                continue;
            RECT_F rcF(rcItem);
            item.pszText = text;
            item.cchText = bufSize;
            SendMessageW(hWnd, TB_GETBUTTONINFO, i, (LPARAM)&item);
            LPIDRAW_STRUCT_TOOL_INSERTEX btnData = 0;
            if (s_info->hWndTool == hWnd && ((UINT)item.idCommand) > 0xffff)
            {
                btnData = (LPIDRAW_STRUCT_TOOL_INSERTEX)item.idCommand;
                item.idCommand = btnData->idCommand;
            }

            const int cxItem = rcItem.right - rcItem.left;
            const int cyItem = rcItem.bottom - rcItem.top;

            if ((item.fsStyle & BTNS_SEP) == BTNS_SEP)  // 分隔符
            {
                LPEX_PEN hPen[2] = { 0 };

                COLORREF cr[2] = { s_info->themeColor->crToolSeplineLeft(), s_info->themeColor->crToolSeplineRight() };

                hPen[0] = s_info->pfnCreatePenDx(RGB2ARGB(cr[0], 255), 1);
                hPen[1] = s_info->pfnCreatePenDx(RGB2ARGB(cr[1], 255), 1);
                //204,206,219
                //245,245,245

                const float left = (float)(rcItem.left + cxItem / 2);
                LINEPOINT_F line(left, (float)rcItem.top + 1, left, (float)rcItem.bottom - 1);
                hCanvas->DrawLine(hPen[0], &line); line.offset(1, 0);
                hCanvas->DrawLine(hPen[1], &line);

                continue;
            }

            LPEX_BRUSH hbrFill = 0;
            float offset = 0;
            STATE state = STATE::SELECTED;
            //TODO 需要加上状态, 需要设计一套状态图标
            if (i == index) state = STATE::HOT;
            if ((item.fsState & TBSTATE_CHECKED) == TBSTATE_CHECKED)    // 选中状态
            {
                hbrFill = hbrHot;
                offset = 1;
                state = STATE::DOWN;
            }
            if ((item.fsState & TBSTATE_PRESSED) == TBSTATE_PRESSED)    // 单击该按钮
            {
                hbrFill = hbrDown;
                offset = 1;
                state = STATE::DOWN;
            }
            if ((item.fsState & TBSTATE_ENABLED) != TBSTATE_ENABLED)    // 禁止状态
            {
                // 没有 TBSTATE_ENABLED 这个标识就是禁止
                state = STATE::BAN; // 禁止状态优先级最高, 只要有这个状态, 其他状态都会覆盖
            }
            if (state == STATE::SELECTED) state = STATE::NORMAL;



            RECT_F rcIconDraw;
            //TODO 最后一个参数应该传递 state 进去, 需要把其他状态都做好
            //if (state != STATE::BAN)state = STATE::NORMAL;
            LPEX_IMAGE hBitmap = 0;
            
            if (btnData)
            {
                hBitmap = btnData->hImage;
                if (hBitmap)
                    rcIconDraw(0, 0, (float)hBitmap->GetWidth(), (float)hBitmap->GetHeight());
            }
            if (!hBitmap)
            {
                hBitmap = _tool_get_icon(pData, item, &rcIconDraw, state, callbackArr, callbackCount);
                if (!hBitmap)
                    hBitmap = _tool_get_icon(pData, item, &rcIconDraw, STATE::NORMAL, callbackArr, callbackCount);

            }

            bool isNopainting = false;     // 是否不继续绘画, 如果为true, 则不继续默认绘画
            if (btnData && btnData->pfnDraw)
                isNopainting = btnData->pfnDraw(hCanvas, btnData->idCommand, btnData->param, &rcF, state, s_info->dpiX) != 0;

            if (!isNopainting)  // 为false则继续调用默认绘画
            {
                if (hbrFill)
                    hCanvas->FillRectangle(hbrFill, &rcF);
                else if (i == index)
                    hCanvas->FillRectangle(hbrHot, &rcF);

                const int cxIcon = s_info->scale((int)rcIconDraw.width());
                const int cyIcon = s_info->scale((int)rcIconDraw.height());
                if (hBitmap && rcIconDraw.width() > 0)
                {
                    RECT_F rcDest;
                    rcDest.left = (float)((int)rcItem.left + (cxItem - cxIcon) / 2);
                    rcDest.top = (float)((int)rcItem.top + (cyItem - cyIcon) / 2);
                    rcDest.right = rcDest.left + (float)cxIcon;
                    rcDest.bottom = rcDest.top + (float)cyIcon;
                    if (offset)
                        rcDest.offset(offset, offset);
                    hCanvas->DrawImage(hBitmap, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom,
                        rcIconDraw.left, rcIconDraw.top, rcIconDraw.right, rcIconDraw.bottom, 255);
                }
            }
        }
        s_info->iDraw->EndPaint(ps);
        if (callbackArr)
            delete[] callbackArr;
        break;
    }
    case WM_NCCALCSIZE:
        return WndProc_ToolBar_CalcSize(hWnd, message, wParam, lParam, pData);
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_DESTROY:
    {
        _canvas_destroy(pData->draw.hCanvas);
        break;
    }
    //case TB_DELETEBUTTON:
    //case TB_INSERTBUTTONA:
    //case TB_INSERTBUTTONW:  // 增删按钮都重新计算尺寸
    //{
    //    LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //    if (hWnd == s_info->eWnd->GetMenuStandard())
    //    {
    //        RECT rc;
    //        int len = SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);         // 标准工具条数量
    //        SendMessageW(hWnd, TB_GETITEMRECT, len - 1, (LPARAM)&rc);   // 取最后一个按钮的右边

    //        RECT& rcMenu = s_info->rcWndMenu;    // 菜单窗口的位置
    //        rcMenu.left = WINDOW_BORDER_WIDTH;
    //        rcMenu.top = WINDOW_CAPTION_HEIGHT;
    //        if (rc.right > 0)
    //            rcMenu.right = rcMenu.left + rc.right;
    //        if (rcMenu.bottom == 0)
    //            rcMenu.bottom = rcMenu.top + (rc.bottom - rc.top) + WINDOW_BORDER_WIDTH;
    //        MoveWindowEx(s_info->hWndMenu, rcMenu, true);
    //    }
    //    return ret;
    //}
    //case WM_LBUTTONDOWN:
    //{
    //    wstr::dbg(L"工具条鼠标按下, xy=%d,%d\n", LOWORD(lParam), HIWORD(lParam));
    //    return 0;
    //}
    //case WM_LBUTTONUP:
    //    return 0;

    //    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    //case TB_ADDBITMAP:              wstr::dbg(L"TB_ADDBITMAP,  将一个或多个图像添加到工具栏可用的按钮图像列表中\n"); break;
    //case TB_ADDBUTTONS:             wstr::dbg(L"TB_ADDBUTTONS,  将一个或多个按钮添加到工具栏\n"); break;
    //case TB_ADDSTRING:              wstr::dbg(L"TB_ADDSTRING,  将新的字符串添加到工具栏的字符串池\n"); break;
    //case TB_AUTOSIZE:               wstr::dbg(L"TB_AUTOSIZE,  调整工具栏的大小\n"); break;
    //case TB_BUTTONCOUNT:            wstr::dbg(L"TB_BUTTONCOUNT,  检索工具栏中当前的按钮计数\n"); break;
    //case TB_BUTTONSTRUCTSIZE:       wstr::dbg(L"TB_BUTTONSTRUCTSIZE,  指定TBBUTTON结构的大小\n"); break;
    //case TB_CHANGEBITMAP:           wstr::dbg(L"TB_CHANGEBITMAP,  更改工具栏中按钮的位图\n"); break;
    //case TB_CHECKBUTTON:            wstr::dbg(L"TB_CHECKBUTTON,  选中或取消选中工具栏中的给定按钮\n"); break;
    //case TB_COMMANDTOINDEX:         wstr::dbg(L"TB_COMMANDTOINDEX,  检索与指定命令标识符关联的按钮的从零开始的索引\n"); break;
    //case TB_CUSTOMIZE:              wstr::dbg(L"TB_CUSTOMIZE,  显示“自定义工具栏”对话框\n"); break;
    //case TB_DELETEBUTTON:           wstr::dbg(L"TB_DELETEBUTTON,  从工具栏中删除按钮\n"); break;
    //case TB_ENABLEBUTTON:           wstr::dbg(L"TB_ENABLEBUTTON,  启用或禁用工具栏中的指定按钮\n"); break;
    //case TB_GETANCHORHIGHLIGHT:     wstr::dbg(L"TB_GETANCHORHIGHLIGHT,  检索工具栏的锚点突出显示设置\n"); break;
    //case TB_GETBITMAP:              wstr::dbg(L"TB_GETBITMAP,  检索与工具栏中的按钮关联的位图的索引\n"); break;
    //case TB_GETBITMAPFLAGS:         wstr::dbg(L"TB_GETBITMAPFLAGS,  检索描述要使用的位图类型的标志\n"); break;
    //case TB_GETBUTTON:              wstr::dbg(L"TB_GETBUTTON,  检索有关工具栏中指定按钮的信息\n"); break;
    //case TB_GETBUTTONINFO:          wstr::dbg(L"TB_GETBUTTONINFO,  检索工具栏中按钮的扩展信息\n"); break;
    //case TB_GETBUTTONSIZE:          wstr::dbg(L"TB_GETBUTTONSIZE,  检索工具栏按钮的当前宽度和高度，以像素为单位\n"); break;
    //case TB_GETBUTTONTEXT:          wstr::dbg(L"TB_GETBUTTONTEXT,  检索工具栏上按钮的显示文本\n"); break;
    //case TB_GETCOLORSCHEME:         wstr::dbg(L"TB_GETCOLORSCHEME,  从工具栏控件中检索配色方案信息\n"); break;
    //case TB_GETDISABLEDIMAGELIST:   wstr::dbg(L"TB_GETDISABLEDIMAGELIST,  检索工具栏控件用于显示非活动按钮的图像列表\n"); break;
    //case TB_GETEXTENDEDSTYLE:       wstr::dbg(L"TB_GETEXTENDEDSTYLE,  检索工具栏控件的扩展样式\n"); break;
    //case TB_GETHOTIMAGELIST:        wstr::dbg(L"TB_GETHOTIMAGELIST,  检索工具栏控件用于显示热键的图像列表\n"); break;
    //case TB_GETHOTITEM:             wstr::dbg(L"TB_GETHOTITEM,  检索工具栏中热点项目的索引\n"); break;
    //case TB_GETIDEALSIZE:           wstr::dbg(L"TB_GETIDEALSIZE,  获取工具栏的理想大小\n"); break;
    //case TB_GETIMAGELIST:           wstr::dbg(L"TB_GETIMAGELIST,  检索工具栏控件用于以默认状态显示按钮的图像列表, 当按钮不热或未禁用时，工具栏控件使用此图像列表显示按钮\n"); break;
    //case TB_GETIMAGELISTCOUNT:      wstr::dbg(L"TB_GETIMAGELISTCOUNT,  获取与工具栏关联的图像列表的数量\n"); break;
    //case TB_GETINSERTMARK:          wstr::dbg(L"TB_GETINSERTMARK,  检索工具栏的当前插入标记\n"); break;
    //case TB_GETINSERTMARKCOLOR:     wstr::dbg(L"TB_GETINSERTMARKCOLOR,  检索用于绘制工具栏插入标记的颜色\n"); break;
    //case TB_GETITEMDROPDOWNRECT:    wstr::dbg(L"TB_GETITEMDROPDOWNRECT,  获取样式为BTNS_DROPDOWN的工具栏项目的下拉窗口的边界矩形\n"); break;
    ////case TB_GETITEMRECT:            wstr::dbg(L"TB_GETITEMRECT,  检索工具栏中按钮的边界矩形\n"); break;
    //case TB_GETMAXSIZE:             wstr::dbg(L"TB_GETMAXSIZE,  检索工具栏中所有可见按钮和分隔符的总大小\n"); break;
    //case TB_GETMETRICS:             wstr::dbg(L"TB_GETMETRICS,  检索工具栏控件的指标\n"); break;
    //case TB_GETOBJECT:              wstr::dbg(L"TB_GETOBJECT,  检索工具栏控件的IDropTarget\n"); break;
    //case TB_GETPADDING:             wstr::dbg(L"TB_GETPADDING,  检索工具栏控件的填充\n"); break;
    //case TB_GETPRESSEDIMAGELIST:    wstr::dbg(L"TB_GETPRESSEDIMAGELIST,  获取工具栏控件用来在按下状态下显示按钮的图像列表\n"); break;
    ////case TB_GETRECT:                wstr::dbg(L"TB_GETRECT,  检索指定工具栏按钮的边界矩形\n"); break;
    //case TB_GETROWS:                wstr::dbg(L"TB_GETROWS,  检索具有TBSTYLE_WRAPABLE样式的工具栏中的按钮的行数\n"); break;
    //case TB_GETSTATE:               wstr::dbg(L"TB_GETSTATE,  检索有关工具栏中指定按钮状态的信息，例如是否已启用，按下或选中它\n"); break;
    //case TB_GETSTRING:              wstr::dbg(L"TB_GETSTRING,  从工具栏的字符串池中检索字符串\n"); break;
    //case TB_GETSTYLE:               wstr::dbg(L"TB_GETSTYLE,  检索当前用于工具栏控件的样式\n"); break;
    //case TB_GETTEXTROWS:            wstr::dbg(L"TB_GETTEXTROWS,  检索可以在工具栏按钮上显示的最大文本行数\n"); break;
    //case TB_GETTOOLTIPS:            wstr::dbg(L"TB_GETTOOLTIPS,  检索与工具栏关联的工具提示控件的句柄（如果有）\n"); break;
    //case TB_GETUNICODEFORMAT:       wstr::dbg(L"TB_GETUNICODEFORMAT,  检索控件的Unicode字符格式标志\n"); break;
    //case TB_HASACCELERATOR:         wstr::dbg(L"TB_HASACCELERATOR,  供内部使用；不建议在应用程序中使用, 检索具有指定加速键字符的工具栏按钮的数量\n"); break;
    //case TB_HIDEBUTTON:             wstr::dbg(L"TB_HIDEBUTTON,  隐藏或显示工具栏中的指定按钮\n"); break;
    //case TB_HITTEST:                wstr::dbg(L"TB_HITTEST,  确定点在工具栏控件中的位置\n"); break;
    //case TB_INDETERMINATE:          wstr::dbg(L"TB_INDETERMINATE,  设置或清除工具栏中指定按钮的不确定状态\n"); break;
    //case TB_INSERTBUTTON:           wstr::dbg(L"TB_INSERTBUTTON,  在工具栏中插入按钮\n"); break;
    //case TB_INSERTMARKHITTEST:      wstr::dbg(L"TB_INSERTMARKHITTEST,  检索工具栏中某个点的插入标记信息\n"); break;
    //case TB_ISBUTTONCHECKED:        wstr::dbg(L"TB_ISBUTTONCHECKED,  确定是否选中工具栏中的指定按钮\n"); break;
    //case TB_ISBUTTONENABLED:        wstr::dbg(L"TB_ISBUTTONENABLED,  确定是否启用了工具栏中的指定按钮\n"); break;
    //case TB_ISBUTTONHIDDEN:         wstr::dbg(L"TB_ISBUTTONHIDDEN,  确定工具栏上的指定按钮是否隐藏\n"); break;
    //case TB_ISBUTTONHIGHLIGHTED:    wstr::dbg(L"TB_ISBUTTONHIGHLIGHTED,  检查工具栏按钮的突出显示状态\n"); break;
    //case TB_ISBUTTONINDETERMINATE:  wstr::dbg(L"TB_ISBUTTONINDETERMINATE,  确定工具栏中的指定按钮是否不确定\n"); break;
    //case TB_ISBUTTONPRESSED:        wstr::dbg(L"TB_ISBUTTONPRESSED,  确定是否按下工具栏中的指定按钮\n"); break;
    //case TB_LOADIMAGES:             wstr::dbg(L"TB_LOADIMAGES,  将系统定义的按钮图像加载到工具栏控件的图像列表中\n"); break;
    //case TB_MAPACCELERATOR:         wstr::dbg(L"TB_MAPACCELERATOR,  确定与指定的加速键字符对应的按钮的ID\n"); break;
    //case TB_MARKBUTTON:             wstr::dbg(L"TB_MARKBUTTON,  设置工具栏控件中给定按钮的突出显示状态\n"); break;
    //case TB_MOVEBUTTON:             wstr::dbg(L"TB_MOVEBUTTON,  将按钮从一个索引移动到另一个索引\n"); break;
    //case TB_PRESSBUTTON:            wstr::dbg(L"TB_PRESSBUTTON,  按下或释放工具栏中的指定按钮\n"); break;
    //case TB_REPLACEBITMAP:          wstr::dbg(L"TB_REPLACEBITMAP,  用新的位图替换现有的位图\n"); break;
    //case TB_SAVERESTORE:            wstr::dbg(L"TB_SAVERESTORE,  发送此消息以启动保存或恢复工具栏状态\n"); break;
    //case TB_SETANCHORHIGHLIGHT:     wstr::dbg(L"TB_SETANCHORHIGHLIGHT,  设置工具栏的锚点突出显示设置\n"); break;
    //case TB_SETBITMAPSIZE:          wstr::dbg(L"TB_SETBITMAPSIZE,  设置要添加到工具栏的位图图像的大小\n"); break;
    //case TB_SETBOUNDINGSIZE:        wstr::dbg(L"TB_SETBOUNDINGSIZE,  供内部使用；不建议在应用程序中使用设置多列工具栏控件的边界大小\n"); break;
    //case TB_SETBUTTONINFO:          wstr::dbg(L"TB_SETBUTTONINFO,  设置工具栏中现有按钮的信息\n"); break;
    //case TB_SETBUTTONSIZE:          wstr::dbg(L"TB_SETBUTTONSIZE,  设置工具栏上按钮的大小\n"); break;
    //case TB_SETBUTTONWIDTH:         wstr::dbg(L"TB_SETBUTTONWIDTH,  在工具栏控件中设置最小和最大按钮宽度\n"); break;
    //case TB_SETCMDID:               wstr::dbg(L"TB_SETCMDID,  设置工具栏按钮的命令标识符\n"); break;
    //case TB_SETCOLORSCHEME:         wstr::dbg(L"TB_SETCOLORSCHEME,  设置工具栏控件的配色方案信息\n"); break;
    //case TB_SETDISABLEDIMAGELIST:   wstr::dbg(L"TB_SETDISABLEDIMAGELIST,  设置工具栏控件将用来显示禁用按钮的图像列表\n"); break;
    //case TB_SETDRAWTEXTFLAGS:       wstr::dbg(L"TB_SETDRAWTEXTFLAGS,  设置工具栏的文本绘制标志\n"); break;
    //case TB_SETEXTENDEDSTYLE:       wstr::dbg(L"TB_SETEXTENDEDSTYLE,  设置工具栏控件的扩展样式\n"); break;
    //case TB_SETHOTIMAGELIST:        wstr::dbg(L"TB_SETHOTIMAGELIST,  设置工具栏控件将用于显示热键的图像列表\n"); break;
    //case TB_SETHOTITEM:             wstr::dbg(L"TB_SETHOTITEM,  在工具栏中设置热门项目\n"); break;
    //case TB_SETHOTITEM2:            wstr::dbg(L"TB_SETHOTITEM2,  在工具栏中设置热门项目\n"); break;
    //case TB_SETIMAGELIST:           wstr::dbg(L"TB_SETIMAGELIST,  设置工具栏用来显示处于默认状态的按钮的图像列表\n"); break;
    //case TB_SETINDENT:              wstr::dbg(L"TB_SETINDENT,  设置工具栏控件中第一个按钮的缩进\n"); break;
    //case TB_SETINSERTMARK:          wstr::dbg(L"TB_SETINSERTMARK,  设置工具栏的当前插入标记\n"); break;
    //case TB_SETINSERTMARKCOLOR:     wstr::dbg(L"TB_SETINSERTMARKCOLOR,  设置用于绘制工具栏插入标记的颜色\n"); break;
    //case TB_SETLISTGAP:             wstr::dbg(L"TB_SETLISTGAP,  设置特定工具栏上工具栏按钮之间的距离\n"); break;
    //case TB_SETMAXTEXTROWS:         wstr::dbg(L"TB_SETMAXTEXTROWS,  设置工具栏按钮上显示的最大文本行数\n"); break;
    //case TB_SETMETRICS:             wstr::dbg(L"TB_SETMETRICS,  设置工具栏控件的指标\n"); break;
    //case TB_SETPADDING:             wstr::dbg(L"TB_SETPADDING,  设置工具栏控件的填充\n"); break;
    //case TB_SETPARENT:              wstr::dbg(L"TB_SETPARENT,  设置工具栏控件向其发送通知代码的窗口\n"); break;
    //case TB_SETPRESSEDIMAGELIST:    wstr::dbg(L"TB_SETPRESSEDIMAGELIST,  设置工具栏用来显示处于按下状态的按钮的图像列表\n"); break;
    //case TB_SETROWS:                wstr::dbg(L"TB_SETROWS,  设置工具栏中按钮的行数\n"); break;
    //case TB_SETSTATE:               wstr::dbg(L"TB_SETSTATE,  设置工具栏中指定按钮的状态\n"); break;
    //case TB_SETSTYLE:               wstr::dbg(L"TB_SETSTYLE,  设置工具栏控件的样式\n"); break;
    //case TB_SETTOOLTIPS:            wstr::dbg(L"TB_SETTOOLTIPS,  将工具提示控件与工具栏关联\n"); break;
    //case TB_SETUNICODEFORMAT:       wstr::dbg(L"TB_SETUNICODEFORMAT,  设置控件的Unicode字符格式标志, 此消息使您可以在运行时更改控件使用的字符集，而不必重新创建控件\n"); break;
    //case TB_SETWINDOWTHEME:         wstr::dbg(L"TB_SETWINDOWTHEME,  设置工具栏控件的视觉样式\n"); break;
    //    //case TB_TRANSLATEACCELERATOR: wstr::dbg(L"TB_TRANSLATEACCELERATOR,  将键盘消息传递到工具栏\n"); break;

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}


// 易语言自带标准工具条触发删除按钮时修改自己创建的工具条状态
inline static void __notify_state(HWND hWnd, WPARAM id, DWORD state)
{
    static HWND hWndStandard = s_info->eWnd->GetMenuStandard();
    if (state == TBSTATE_ENABLED)
        state &= ~TBSTATE_ENABLED;  // 有启用状态就去除启用状态
    else if (state == 0)
        state |= TBSTATE_ENABLED;   // 没有启用状态就加上启用状态

    if ((state & TBSTATE_CHECKED) == TBSTATE_CHECKED)
        state = ((state & ~TBSTATE_CHECKED) | TBSTATE_ENABLED);   // 有选中状态就加上启用和去除选中

    SendMessageW(hWndStandard, TB_SETSTATE, id, state);
}



inline void _tool_id2str(int id, LPCWSTR& title, LPCWSTR& text)
{
    static wchar_t buffer[256];
    int len = LoadStringW(GetModuleHandleW(0), id, buffer, 256);
    text = buffer;
    wchar_t* pos = (wchar_t*)wcsrchr(text, '\n');
    title = text;
    if (pos)
    {
        *pos++ = 0;
        title = pos;
    }
    return;
    switch (id)
    {
    case 57600:     // 新建源码
        title = L"新建源码";
        text = L"建立新程序";
        break;
    case 57601:     // 打开源码
        title = L"打开源码(Ctrl+O)";
        text = L"打开一个现有程序";
        break;
    case 57603:     // 保存源码
        title = L"保存源码(Ctrl+S)";
        text = L"保存当前程序";
        break;
    case 57635:     // 剪切
        title = L"剪切(Ctrl+X)";
        text = L"剪切被选块并将其置于剪贴板上";
        break;
    case 57634:     // 复制
        title = L"复制(Ctrl+C)";
        text = L"复制被选块并将其置于剪贴板上";
        break;
    case 57637:     // 粘贴
        title = L"粘贴(Ctrl+V)";
        text = L"插入剪贴板内容";
        break;
    case 57644:     // 重复
        title = L"重复(Ctrl+Y)";
        text = L"重新执行先前已撤消的操作";
        break;
    case 57643:     // 撤销
        title = L"撤销(Ctrl+Z)";
        text = L"撤消最后一步操作";
        break;
    case 32934:     // 整体搜寻
        title = L"整体搜寻(Ctrl+Shift+F)";
        text = L"在程序中寻找指定文本并列出所有找到的项目";
        break;
    case 32854:     // 运行
        title = L"运行(F5)";
        text = L"编译现行易程序的调试版本.EXE文件后立即运行";
        break;
    case 32860:     // 停止
        title = L"终止";
        text = L"终止现行易程序的运行";
        break;
    case 32940:     // 查看
        title = L"查看(Shift+F9)";
        text = L"查看/修改指定表达式或变量的内容";
        break;
    case 32853:     // 单步
        title = L"单步(F8)";
        text = L"在程序现行运行位置单步执行一行程序，如果此程序行调用了子程序，系统不会跟踪到该子程序中去。";
        break;
    case 32852:     // 单步进入
        title = L"单步进入(Shift+F8)";
        text = L"在程序现行运行位置单步执行一行程序，如果此程序行调用了子程序，则跟踪进入子程序。";
        break;
    case 32867:     // 跟踪返回
        title = L"跟踪返回(Ctrl+Shift+F8)";
        text = L"在上级子程序调用现行子程序的语句后中断。";
        break;
    case 32871:     // 运行到光标处
        title = L"运行到光标处(Ctrl+F8)";
        text = L"运行易程序，在当前光标所处程序行处中断。";
        break;
    case 32855:     // 设置断点
        title = L"设置断点(F9)";
        text = L"设置或清除当前程序行处的断点";
        break;
    case 32925:     // 知识库
        title = L"知识库";
        text = L"打开并显示易语言知识库";
        break;
    case 32905:     // 后退
        title = L"跳回先前位置(Ctrl+J)";
        text = L"跳回到先前跳转时的程序位置";    //(Ctrl+Shift+Backspace)
        break;
    case 32906:     // 前进
        title = L"跳转到定义处(Ctrl+B/Ctrl+单击)";
        text = L"跳转到当前光标所处符号的定义位置"; // (Ctrl+Shift+Alt+Backspace)
        break;
    case 32850:     // 预览
        title = L"预览";
        text = L"预览现行窗口，按Esc键退出预览";
        break;
    case 111:       // 左对齐
        title = L"左对齐";
        text = L"将所有被选中组件的左边与最后被选中组件左边对齐";
        break;
    case 112:       // 右对齐
        title = L"右对齐";
        text = L"将所有被选中组件的右边与最后被选中组件右边对齐";
        break;
    case 190:       // 顶对齐
        title = L"顶对齐";
        text = L"将所有被选中组件的顶边与最后被选中组件顶边对齐";
        break;
    case 191:       // 底对齐
        title = L"底对齐";
        text = L"将所有被选中组件的底边与最后被选中组件底边对齐";
        break;
    case 192:       // 窗口水平居中
        title = L"窗口水平居中";
        text = L"将被选择组件在所处窗口中水平居中";
        break;
    case 196:       // 窗口垂直居中
        title = L"窗口垂直居中";
        text = L"将被选择组件在所处窗口中垂直居中";
        break;
    case 150:       // 组件水平居中
        title = L"组件水平居中";
        text = L"将所有被选择组件水平居中";
        break;
    case 151:       // 组件垂直居中
        title = L"组件垂直居中";
        text = L"将所有被选择组件垂直居中";
        break;
    case 193:       // 水平平均分布
        title = L"水平平均分布";
        text = L"将所有被选择组件水平平均分布";
        break;
    case 194:       // 垂直平均分布
        title = L"垂直平均分布";
        text = L"将所有被选择组件垂直平均分布";
        break;
    case 195:       // 等宽
        title = L"等宽";
        text = L"将所有被选择组件的宽度调整为最后被选择组件的宽度";
        break;
    case 120:       // 等高
        title = L"等高";
        text = L"将所有被选择组件的高度调整为最后被选择组件的高度";
        break;
    case 121:       // 等宽高
        title = L"等高宽";
        text = L"将所有被选择组件的高度及宽度调整为最后被选择组件的高度及宽度";
        break;
    case 32917:           // "&C.编译\tF7", 32851
        title = L"编译(F7)";
        text = L"编译现行易程序的最终发布版本，创建对应的.EXE可执行程序文件";
        break;
    case 32894:    // "&S.静态编译\tShift+F7", 32912
        title = L"静态编译(Shift+F7)";
        text = L"编译出静态可执行EXE文件，该文件不依赖任何易语言系统文件，可以在未安装易语言系统的电脑上运行。";
        break;
    case 32884: // "&B.黑月静态编译", 32912
        title = L"黑月静态编译";
        text = L"黑月静态编译";
        break;
    case 32882:      // "&D.独立编译", 32911
        title = L"独立编译";
        text = L"编译出独立可执行EXE文件，该文件不依赖任何易语言系统文件，可以在未安装易语言系统的电脑上运行。";
        break;
    case 32915:         // 重新运行
        title = L"重新运行";
        text = L"停止当前调试的程序并重新编译运行";
        break;
    case 32914:    // 标准工具条 -> 下一个书签
        title = L"下一个书签";
        text = L"跳转到后面的最近书签位置";
        break;
    case 32913:    // 标准工具条 -> 上一个书签
        title = L"上一个书签";
        text = L"跳转到前面的最近书签位置";
        break;
    case 32908:     // 标准工具条 -> 设置书签
        title = L"设置书签";
        text = L"设置或取消当前位置处的书签标志";
        break;
    case 32897:     // 标准工具条 -> 删除书签
        title = L"删除书签";
        text = L"删除当前位置处的书签标志";
        break;
    case 32892:       // 标准工具条 -> 注释
        title = L"注释";
        text = L"屏蔽当前所选中的代码块";
        break;
    case 32898:     // 标准工具条 -> 取消注释 
        title = L"取消注释";
        text = L"解除屏蔽当前所选中的代码块";
        break;

    default:
        break;
    }
    

}

LRESULT CALLBACK WndProc_ToolBar_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    static HWND hWndStandard = s_info->eWnd->GetMenuStandard();
    if (hdr->hwndFrom != hWndStandard && hdr->hwndFrom != s_info->hWndTool)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //wstr::dbg(L"工具条通知事件, cdde = %d, id= %d\n", hdr->code, hdr->idFrom);
    switch (hdr->code)
    {
    case TBN_HOTITEMCHANGE:
    {
        LPNMTBHOTITEM hi = (LPNMTBHOTITEM)lParam;
        if (hi->idNew == 0)
            break;
        LPIDRAW_STRUCT_TOOL_INSERTEX btnData = (LPIDRAW_STRUCT_TOOL_INSERTEX)0;
        LPCWSTR title = 0;
        LPCWSTR text = 0;
        int offset = 0;
        if (hdr->hwndFrom == s_info->hWndTool && ((UINT)hi->idNew) > 0xffff)
        {
            btnData = (LPIDRAW_STRUCT_TOOL_INSERTEX)hi->idNew;
            title = btnData->lpszTitle;
            text = btnData->lpszTips;
            offset = -1;
        }
        else
        {
            _tool_id2str(hi->idNew, title, text);
        }
        if (!title && !text)
        {

            break;
        }
        //wstr text;
        //text.Format(L"热点被改变, 当前项目id = %d, 前一个项目id = %d", hi->idNew, hi->idOld);
        //s_tips.SetTipsText(s_info->hCustomTab, text.c_str());
        RECT rc;
        SendMessageW(hdr->hwndFrom, TB_GETRECT, hi->idNew, (LPARAM)&rc);
        rc.bottom += offset;
        const int cx = rc.right - rc.left;
        const int cy = rc.bottom - rc.top;
        ClientToScreen(hdr->hwndFrom, (LPPOINT)&rc);
        ScreenToClient(s_info->hEWnd, (LPPOINT)&rc);
        rc.right = rc.left + cx;
        rc.bottom = rc.top + cy;
        EWidnow_ShowTips(rc, title, text);
        break;
    }
    case TBN_DELETINGBUTTON:
    {
        // 显示变量表, 停止按钮和运行按钮都是启用状态就需要把一大堆组件显示出来, 否则就显示只有调试状态可用哪个标签
        LPNMTOOLBARW item = (LPNMTOOLBARW)lParam;
        switch (item->iItem)
        {
        case 32854:     // 运行
        {
            s_info->isRun = (item->tbButton.fsState != TBSTATE_ENABLED);   // 运行按钮是启用状态
            if (!s_info->isRun)
                s_info->pfnControls(IDC_ICONTROL_BUILD, BUILD_STATE_BEGIN, 0);
            break;
        }
        case 32860:     // 停止
        {
            s_info->isStop = (item->tbButton.fsState != TBSTATE_ENABLED);   // 停止按钮是启用状态
            SendMessageW(s_info->hEWnd, WM_ACTIVATEAPP, 1, (LPARAM)0);      // 停止按钮状态被改变就改变边框颜色, 停止按钮是禁止状态一定不是调试状态
            break;
        }
        case 0xFFFF1234:
        {
            SendMessageW(hdr->hwndFrom, TB_ENABLEBUTTON, 0xFFFF1234, TRUE);
            break;
        }
        default:
            break;

        }
        // 运行或者停止按钮状态改变都需要重新调整变量表的位置
        if (32854 == item->iItem || 32860 == item->iItem)
        {
            bool isVarTable = false;    // 当前选中是否有变量表
            TCITEMW item = { 0 };
            int index = TabCtrl_GetCurSel(s_info->hTabStateLeft);   // 先判断左边的选择夹选中项是否是变量表
            item.mask = TCIF_PARAM;
            TabCtrl_GetItem(s_info->hTabStateLeft, index, &item);
            if (item.lParam == TAB_PARAM_VARTABLE)
                isVarTable = true;

            if (!isVarTable)
            {
                index = TabCtrl_GetCurSel(s_info->hTabStateRight);   // 再判断右边的选择夹选中项是否是变量表
                item.mask = TCIF_PARAM;
                TabCtrl_GetItem(s_info->hTabStateRight, index, &item);
                if (item.lParam == TAB_PARAM_VARTABLE)
                    isVarTable = true;
            }
            if (isVarTable)
            {
                // 如果选中的是变量表, 需要触发一下表项被选择, 让变量表重新调整显示的组件
                NMHDR hdr = { 0 };
                hdr.hwndFrom = s_info->hTabStateLeft;
                hdr.code = TCN_SELCHANGE;
                hdr.idFrom = 0;
                SendMessageW(s_info->eWnd->GetState32770(), WM_NOTIFY, hdr.idFrom, (LPARAM)&hdr);
            }
        }
        LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_TOOLSTATE_CHANGE, (LPVOID)s_info->isStop, &isReturn, false);
        if (isReturn) return 0;
        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        s_info->evt.pfn_Event_callback_after(arr, IRE_TOOLSTATE_CHANGE, (LPVOID)s_info->isStop, evtCount, false);

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case NM_CLICK:
    {
        LPNMMOUSE nm = (LPNMMOUSE)lParam;

        if (hdr->hwndFrom == s_info->hWndTool)  // 点击的是接口工具条, 需要把事件传递出去
        {
            if ( nm->dwItemSpec == -1 )
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
            LPIDRAW_STRUCT_TOOL_INSERTEX data = (LPIDRAW_STRUCT_TOOL_INSERTEX)nm->dwItemSpec;
            if (data->plugin->pfnCallback)
                data->plugin->pfnCallback(INE_TOOL_BUTTONCLICK, data->idCommand, data->param);
            return 0;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

LRESULT CALLBACK WndProc_ToolBar_CalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (wParam)
    {
        LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
        //cs->rgrc[0].left += 4;
        //cs->rgrc[0].top += 2;
        return WVR_ALIGNTOP | WVR_ALIGNLEFT;
    }
    LPRECT prc = (LPRECT)lParam;
    return 0;
}

inline LRESULT _tool_click(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const int code = HIWORD(wParam);
    const int id = LOWORD(wParam);
    switch (id)
    {
    case TOOL_STANDARD_NEW:         // 备注工具条 -> 新建源码
    case TOOL_STANDARD_OPEN:        // 备注工具条 -> 打开源码
    case TOOL_STANDARD_SAVE:        // 备注工具条 -> 保存
    case TOOL_STANDARD_CUT:         // 备注工具条 -> 剪切
    case TOOL_STANDARD_COPY:        // 备注工具条 -> 复制
    case TOOL_STANDARD_PASTE:       // 备注工具条 -> 粘贴
    case TOOL_STANDARD_REOD:        // 备注工具条 -> 重复/重做
    case TOOL_STANDARD_UNDO:        // 备注工具条 -> 撤销
    case TOOL_STANDARD_FIND:        // 备注工具条 -> 查找
    case TOOL_STANDARD_RUN:         // 备注工具条 -> 运行
    case TOOL_STANDARD_STOP:        // 备注工具条 -> 停止
    case TOOL_STANDARD_LOOKOVER:    // 备注工具条 -> 查看
    case TOOL_STANDARD_STEPOVER:    // 备注工具条 -> 单步
    case TOOL_STANDARD_STEPINTO:    // 备注工具条 -> 单步步入
    case TOOL_STANDARD_STEPOUT:     // 备注工具条 -> 跟踪返回
    case TOOL_STANDARD_INTO:        // 备注工具条 -> 运行到光标处
    case TOOL_STANDARD_BREAKING:    // 备注工具条 -> 设置断点
    case TOOL_STANDARD_LIBRARY:     // 备注工具条 -> 知识库
        // 以上的按钮事件都需要交给易语言处理
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);


    case TOOL_STANDARD_WORK:        // 备注工具条 -> 工作夹
    case TOOL_STANDARD_STATE:       // 备注工具条 -> 状态夹
    case TOOL_STANDARD_WORK_STATE:  // 备注工具条 -> 工作夹与状态夹组合
        // 这几个改变布局的按钮就不让触发
        return 0;
    case TOOL_STANDARD_BACK:        // 备注工具条 -> 后退
        EIDE_Toolbar_Back();
        //s_info->pfnCallMenu(33003);     // &Z.跳回先前位置\tCtrl+J, 32851
        return 0;
    case TOOL_STANDARD_FORWARD:     // 备注工具条 -> 前进
        EIDE_Toolbar_Forward();
        //s_info->pfnCallMenu(32885); // &S.跳转到定义处\tCtrl+B/Ctrl+单击, 32885
        return 0;

    case TOOL_STANDARD_BUILD:           // 标准工具条 -> 编译
        s_info->pfnCallMenu(32851);     // "&C.编译\tF7", 32851
        break;
    case TOOL_STANDARD_BUILD_STATIC:    // 标准工具条 -> 静态编译
        s_info->pfnCallMenu(32912);     // "&S.静态编译\tShift+F7", 32912
        break;
    case TOOL_STANDARD_BUILD_BLACKMOON: // 标准工具条 -> 黑月编译
        s_info->pfnCallMenu(32912);     // "&B.黑月静态编译", 32912
        break;
    case TOOL_STANDARD_BUILD_DULI:      // 标准工具条 -> 独立编译
        s_info->pfnCallMenu(32911);     // "&D.独立编译", 32911
        break;
    case TOOL_STANDARD_RESTART:     // 标准工具条 -> 重新运行
        EWindow_ReStart();
        break;
    case TOOL_STANDARD_NEXTMARK:    // 标准工具条 -> 下一个书签
        _nes_run_fun(FN_NEXT_BOOKMARK, 0, 0);
        wstr::dbg(L"点击了标准工具条的下一个书签按钮, id = %d\n", id);
        break;
    case TOOL_STANDARD_PREVMARK:    // 标准工具条 -> 上一个书签
        _nes_run_fun(FN_PREV_BOOKMARK, 0, 0);
        wstr::dbg(L"点击了标准工具条的上一个书签按钮, id = %d\n", id);
        break;
    case TOOL_STANDARD_SETMARK:     // 标准工具条 -> 设置书签
        _nes_run_fun(FN_SET_BOOKMARK, 0, 0);
        wstr::dbg(L"点击了标准工具条的设置书签按钮, id = %d\n", id);
        break;
    case TOOL_STANDARD_DELMARK:     // 标准工具条 -> 删除书签
        _nes_run_fun(FN_SET_BOOKMARK, 0, 0);
        wstr::dbg(L"点击了标准工具条的删除书签按钮, id = %d\n", id);
        break;
    case TOOL_STANDARD_NOTES:       // 标准工具条 -> 注释
        wstr::dbg(L"点击了标准工具条的注释按钮, id = %d\n", id);
        break;
    case TOOL_STANDARD_UNNOTES:     // 标准工具条 -> 取消注释 
        wstr::dbg(L"点击了标准工具条的取消注释 按钮, id = %d\n", id);
        break;

    case TOOL_ALIGNMENT_PREVIEW:        // 对齐工具条 -> 预览
    case TOOL_ALIGNMENT_LEFT:           // 对齐工具条 -> 左对齐
    case TOOL_ALIGNMENT_RIGHT:          // 对齐工具条 -> 右对齐
    case TOOL_ALIGNMENT_TOP:            // 对齐工具条 -> 顶对齐
    case TOOL_ALIGNMENT_BOTTOM:         // 对齐工具条 -> 底对齐
    case TOOL_ALIGNMENT_WND_CENTER:     // 对齐工具条 -> 窗口水平居中
    case TOOL_ALIGNMENT_WND_VCENTER:    // 对齐工具条 -> 窗口垂直居中
    case TOOL_ALIGNMENT_CTL_CENTER:     // 对齐工具条 -> 组件水平居中
    case TOOL_ALIGNMENT_CTL_VCENTER:    // 对齐工具条 -> 组件垂直居中
    case TOOL_ALIGNMENT_AVERAGE:        // 对齐工具条 -> 水平平均分布
    case TOOL_ALIGNMENT_VAVERAGE:       // 对齐工具条 -> 垂直平均分布
    case TOOL_ALIGNMENT_EQUAL_WIDTH:    // 对齐工具条 -> 等宽
    case TOOL_ALIGNMENT_EQUAL_HEIGHT:   // 对齐工具条 -> 等高
    case TOOL_ALIGNMENT_EQUAL_WH:       // 对齐工具条 -> 等宽高
        // 对齐工具条所有的事件都交给易语言处理
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    default:
        wstr::dbg(L"点击了标准工具条的按钮, id = %d\n", id);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT CALLBACK WndProc_ToolBar_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPOBJSTRUCT pChild = GetDataFromHwnd((HWND)lParam);
    if (!pChild)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    static HWND hMenu_Standard = s_info->eWnd->GetMenuStandard();
    static HWND hMenu_Align  = s_info->eWnd->GetMenuAlign();

    const int code = HIWORD(wParam);
    const int id = LOWORD(wParam);
    HWND hTool = (HWND)lParam;
    if (hTool == hMenu_Standard || hTool == hMenu_Align)    // 点击的是标准工具条的按钮或者对齐工具条的按钮
    {
        return _tool_click(hWnd, message, wParam, lParam, pData);
    }

    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

// 按下了重新启动按钮, 需要先停止当前程序然后在点击运行
void EWindow_ReStart(int idRun, int idStop)
{
    static ULONGLONG t = 0;
    if (GetTickCount64() - t < 500)
    {
        return;
    }
    t = GetTickCount64();
    // 停止按钮是启用状态就是运行状态
    // TODO 如果没有启动时点击重新启动无效就把下面第一个条件删除就行
    //if (s_info->isRun || s_info->isStop)
    if (s_info->isStop)
    {
        s_info->pfnCallMenu(idStop);
        sleep(100);
        s_info->pfnCallMenu(idRun);
    }
}

// 查询这个插件是否添加了这个ID的按钮, 有创建了这个按钮返回true, 否则返回false
// plugin = 插件信息
// id = 查询的id
// pBtnData = 接收查询到的按钮id的信息
bool Tool_QueryButton(LPPLUGIN_INFO plugin, int id, LPIDRAW_STRUCT_TOOL_INSERTEX* pBtnData)
{
    if (!plugin || !plugin->pToolButton)
        return false;
    for (int i = 0; i < plugin->nToolButtonCount; i++)
    {
        IDRAW_STRUCT_TOOL_INSERTEX& data = plugin->pToolButton[i];
        if (data.idCommand == id)
        {
            if (pBtnData)
                *pBtnData = &data;
            return true;
        }
    }
    return false;
}

int Tool_InsertButton(LPIDRAW_STRUCT_TOOL_INSERT pArg)
{
    LPIDRAW_STRUCT_TOOL_INSERTEX pBtnData = 0;
    LPPLUGIN_INFO plugin = (LPPLUGIN_INFO)pArg->pPlugin;

    if (Tool_QueryButton(plugin, pArg->idCommand, &pBtnData))
    {
        _Tool_SetButtonData(*pBtnData, pArg, false);
        return pBtnData->idCommand;
    }
    static int id = 1; id++;

    //////////////////////////////////////////////////////////////////////////
    // 这一段代码是创建工具条按钮缓冲区, 还有判断继续加入是否足够加入
    if (!plugin->pToolButton)
    {
        plugin->nToolButtonBufferCount = 10;
        plugin->pToolButton = new IDRAW_STRUCT_TOOL_INSERTEX[plugin->nToolButtonBufferCount];
        memset(plugin->pToolButton, 0, sizeof(IDRAW_STRUCT_TOOL_INSERTEX) * plugin->nToolButtonBufferCount);
    }
    if (plugin->nToolButtonCount + 1 > plugin->nToolButtonBufferCount)
    {
        plugin->nToolButtonBufferCount *= 3;
        LPIDRAW_STRUCT_TOOL_INSERTEX ptr = new IDRAW_STRUCT_TOOL_INSERTEX[plugin->nToolButtonBufferCount];
        memset(ptr, 0, sizeof(IDRAW_STRUCT_TOOL_INSERTEX) * plugin->nToolButtonBufferCount);
        memcpy(ptr, plugin->pToolButton, sizeof(IDRAW_STRUCT_TOOL_INSERTEX) * (plugin->nToolButtonBufferCount / 3));
        delete[] plugin->pToolButton;
        plugin->pToolButton = ptr;
    }
    //////////////////////////////////////////////////////////////////////////

    IDRAW_STRUCT_TOOL_INSERTEX& data = plugin->pToolButton[plugin->nToolButtonCount++];

    TBBUTTON tb = { 0 };
    tb.iBitmap = 0;
    int len = SendMessageW(s_info->hWndTool, TB_BUTTONCOUNT, 0, 0);             // 接口工具条按钮数量
    BOOL isOk = 0;
    if (len == 0)
    {
        len = 1;
        tb.idCommand = 0;
        tb.fsState = TBSTATE_ENABLED;
        tb.fsStyle = BTNS_SEP;
        isOk = SendMessageW(s_info->hWndTool, TB_INSERTBUTTONW, 0, (LPARAM)&tb);
    }
    tb.idCommand = (int)&data;
    tb.fsState = TBSTATE_ENABLED;
    tb.fsStyle = BTNS_BUTTON;
    isOk = SendMessageW(s_info->hWndTool, TB_INSERTBUTTONW, len, (LPARAM)&tb);
    if (!isOk)
    {
        s_info->SetLastError(L"添加工具条按钮失败, 行号: %d, 文件:" __FILEW__, __LINE__);
        return 0;
    }

    _Tool_SetButtonData(data, pArg, true);
    MoveWindow(s_info->hWndMenu, 1, 1, 100, 1, true);
    return data.idCommand;
}

bool Tool_DeleteButton(LPPLUGIN_INFO plugin, int idCommand)
{
    LPIDRAW_STRUCT_TOOL_INSERTEX pBtnData = 0;
    if (!Tool_QueryButton(plugin, idCommand, &pBtnData))
        return false;
    int index = (int)SendMessageW(s_info->hWndTool, TB_GETBUTTON, (WPARAM)pBtnData, 0);
    SendMessageW(s_info->hWndTool, TB_DELETEBUTTON, index, 0);
    MoveWindow(s_info->hWndMenu, 1, 1, 100, 1, true);
    //InvalidateRect(s_info->hWndTool, 0, 0);
    return true;
}

// data = 按钮指向的数据
// pArg = 新设置的数据
// isSet = 是否设置, 如果为true, 则释放data的数据, 然后把pArg拷贝到data, 为false则判断pArg各个值, 为0则不设置
bool _Tool_SetButtonData(IDRAW_STRUCT_TOOL_INSERTEX& data, LPIDRAW_STRUCT_TOOL_INSERT pArg, bool isSet)
{
    if (isSet)
    {
        memcpy(&data, pArg, sizeof(IDRAW_STRUCT_TOOL_INSERT));
        if (data.hImage)
            s_info->d2d->_img_destroy(data.hImage);

        if (data.pImageData && data.nImageSize)
        {
            data.hImage = s_info->d2d->_img_create_frommemory(data.pImageData, data.nImageSize);
            InvalidateRect(s_info->hWndTool, 0, 0); // 只有换图片才重画
        }
        data.title = data.lpszTitle;
        data.tips = data.lpszTips;
    }
    else
    {
        if (pArg->pImageData && pArg->nImageSize)
        {
            if (data.hImage)
                s_info->d2d->_img_destroy(data.hImage);
            data.pImageData = pArg->pImageData;
            data.nImageSize = pArg->nImageSize;
            data.hImage = s_info->d2d->_img_create_frommemory(data.pImageData, data.nImageSize);
            InvalidateRect(s_info->hWndTool, 0, 0);
        }
        if (pArg->param)
            data.param = pArg->param;
        if (pArg->pfnDraw)
            data.pfnDraw = pArg->pfnDraw;
        if (data.lpszTitle)
            data.title = data.lpszTitle;
        if (data.lpszTips)
            data.tips = data.lpszTips;
    }


    data.lpszTitle = data.title.c_str();
    data.lpszTips = data.tips.c_str();
    data.plugin = (LPPLUGIN_INFO)data.pPlugin;
    return true;
}

bool Tool_SetButtonData(int id, LPIDRAW_STRUCT_TOOL_INSERT pArg)
{
    LPIDRAW_STRUCT_TOOL_INSERTEX pBtnData = 0;
    LPPLUGIN_INFO plugin = (LPPLUGIN_INFO)pArg->pPlugin;
    if (!Tool_QueryButton(plugin, id, &pBtnData))
    {
        s_info->SetLastError(L"插件 %s 的接口工具条按钮(ID:%d)不存在, 设置按钮信息失败\n", _str::A2W(plugin->guid).c_str(), id);
        return false;
    }
    return _Tool_SetButtonData(*pBtnData, pArg, false);
}

LPIDRAW_STRUCT_TOOL_INSERTEX Tool_GetButtonData(LPPLUGIN_INFO plugin, int idCommand)
{
    LPIDRAW_STRUCT_TOOL_INSERTEX ret = 0;
    Tool_QueryButton(plugin, idCommand, &ret);
    return ret;
}