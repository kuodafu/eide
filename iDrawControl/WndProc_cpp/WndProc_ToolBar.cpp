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

// data = ��ťָ�������
// pArg = �����õ�����
// isSet = �Ƿ�����, ���Ϊtrue, ���pArg������data, �����ж�pArg����ֵ, Ϊ0������
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

// ����id��ȡ�滭ͼ��, ���ػ滭��Ҫ��λͼ��ͼ����λͼ���λ��
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
        const int index = SendMessageW(hWnd, TB_GETHOTITEM, 0, 0);      // ��ǰ�������ȵ�����
        const int len = SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);       // ��ǰ����������

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
            if (!Rect_isOverlap(rcItem, ps.rcPaint))    // �����ػ�����ͽ�����һ��ѭ��
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

            if ((item.fsStyle & BTNS_SEP) == BTNS_SEP)  // �ָ���
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
            //TODO ��Ҫ����״̬, ��Ҫ���һ��״̬ͼ��
            if (i == index) state = STATE::HOT;
            if ((item.fsState & TBSTATE_CHECKED) == TBSTATE_CHECKED)    // ѡ��״̬
            {
                hbrFill = hbrHot;
                offset = 1;
                state = STATE::DOWN;
            }
            if ((item.fsState & TBSTATE_PRESSED) == TBSTATE_PRESSED)    // �����ð�ť
            {
                hbrFill = hbrDown;
                offset = 1;
                state = STATE::DOWN;
            }
            if ((item.fsState & TBSTATE_ENABLED) != TBSTATE_ENABLED)    // ��ֹ״̬
            {
                // û�� TBSTATE_ENABLED �����ʶ���ǽ�ֹ
                state = STATE::BAN; // ��ֹ״̬���ȼ����, ֻҪ�����״̬, ����״̬���Ḳ��
            }
            if (state == STATE::SELECTED) state = STATE::NORMAL;



            RECT_F rcIconDraw;
            //TODO ���һ������Ӧ�ô��� state ��ȥ, ��Ҫ������״̬������
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

            bool isNopainting = false;     // �Ƿ񲻼����滭, ���Ϊtrue, �򲻼���Ĭ�ϻ滭
            if (btnData && btnData->pfnDraw)
                isNopainting = btnData->pfnDraw(hCanvas, btnData->idCommand, btnData->param, &rcF, state, s_info->dpiX) != 0;

            if (!isNopainting)  // Ϊfalse���������Ĭ�ϻ滭
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
    //case TB_INSERTBUTTONW:  // ��ɾ��ť�����¼���ߴ�
    //{
    //    LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //    if (hWnd == s_info->eWnd->GetMenuStandard())
    //    {
    //        RECT rc;
    //        int len = SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);         // ��׼����������
    //        SendMessageW(hWnd, TB_GETITEMRECT, len - 1, (LPARAM)&rc);   // ȡ���һ����ť���ұ�

    //        RECT& rcMenu = s_info->rcWndMenu;    // �˵����ڵ�λ��
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
    //    wstr::dbg(L"��������갴��, xy=%d,%d\n", LOWORD(lParam), HIWORD(lParam));
    //    return 0;
    //}
    //case WM_LBUTTONUP:
    //    return 0;

    //    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    //case TB_ADDBITMAP:              wstr::dbg(L"TB_ADDBITMAP,  ��һ������ͼ����ӵ����������õİ�ťͼ���б���\n"); break;
    //case TB_ADDBUTTONS:             wstr::dbg(L"TB_ADDBUTTONS,  ��һ��������ť��ӵ�������\n"); break;
    //case TB_ADDSTRING:              wstr::dbg(L"TB_ADDSTRING,  ���µ��ַ�����ӵ����������ַ�����\n"); break;
    //case TB_AUTOSIZE:               wstr::dbg(L"TB_AUTOSIZE,  �����������Ĵ�С\n"); break;
    //case TB_BUTTONCOUNT:            wstr::dbg(L"TB_BUTTONCOUNT,  �����������е�ǰ�İ�ť����\n"); break;
    //case TB_BUTTONSTRUCTSIZE:       wstr::dbg(L"TB_BUTTONSTRUCTSIZE,  ָ��TBBUTTON�ṹ�Ĵ�С\n"); break;
    //case TB_CHANGEBITMAP:           wstr::dbg(L"TB_CHANGEBITMAP,  ���Ĺ������а�ť��λͼ\n"); break;
    //case TB_CHECKBUTTON:            wstr::dbg(L"TB_CHECKBUTTON,  ѡ�л�ȡ��ѡ�й������еĸ�����ť\n"); break;
    //case TB_COMMANDTOINDEX:         wstr::dbg(L"TB_COMMANDTOINDEX,  ������ָ�������ʶ�������İ�ť�Ĵ��㿪ʼ������\n"); break;
    //case TB_CUSTOMIZE:              wstr::dbg(L"TB_CUSTOMIZE,  ��ʾ���Զ��幤�������Ի���\n"); break;
    //case TB_DELETEBUTTON:           wstr::dbg(L"TB_DELETEBUTTON,  �ӹ�������ɾ����ť\n"); break;
    //case TB_ENABLEBUTTON:           wstr::dbg(L"TB_ENABLEBUTTON,  ���û���ù������е�ָ����ť\n"); break;
    //case TB_GETANCHORHIGHLIGHT:     wstr::dbg(L"TB_GETANCHORHIGHLIGHT,  ������������ê��ͻ����ʾ����\n"); break;
    //case TB_GETBITMAP:              wstr::dbg(L"TB_GETBITMAP,  �����빤�����еİ�ť������λͼ������\n"); break;
    //case TB_GETBITMAPFLAGS:         wstr::dbg(L"TB_GETBITMAPFLAGS,  ��������Ҫʹ�õ�λͼ���͵ı�־\n"); break;
    //case TB_GETBUTTON:              wstr::dbg(L"TB_GETBUTTON,  �����йع�������ָ����ť����Ϣ\n"); break;
    //case TB_GETBUTTONINFO:          wstr::dbg(L"TB_GETBUTTONINFO,  �����������а�ť����չ��Ϣ\n"); break;
    //case TB_GETBUTTONSIZE:          wstr::dbg(L"TB_GETBUTTONSIZE,  ������������ť�ĵ�ǰ��Ⱥ͸߶ȣ�������Ϊ��λ\n"); break;
    //case TB_GETBUTTONTEXT:          wstr::dbg(L"TB_GETBUTTONTEXT,  �����������ϰ�ť����ʾ�ı�\n"); break;
    //case TB_GETCOLORSCHEME:         wstr::dbg(L"TB_GETCOLORSCHEME,  �ӹ������ؼ��м�����ɫ������Ϣ\n"); break;
    //case TB_GETDISABLEDIMAGELIST:   wstr::dbg(L"TB_GETDISABLEDIMAGELIST,  �����������ؼ�������ʾ�ǻ��ť��ͼ���б�\n"); break;
    //case TB_GETEXTENDEDSTYLE:       wstr::dbg(L"TB_GETEXTENDEDSTYLE,  �����������ؼ�����չ��ʽ\n"); break;
    //case TB_GETHOTIMAGELIST:        wstr::dbg(L"TB_GETHOTIMAGELIST,  �����������ؼ�������ʾ�ȼ���ͼ���б�\n"); break;
    //case TB_GETHOTITEM:             wstr::dbg(L"TB_GETHOTITEM,  �������������ȵ���Ŀ������\n"); break;
    //case TB_GETIDEALSIZE:           wstr::dbg(L"TB_GETIDEALSIZE,  ��ȡ�������������С\n"); break;
    //case TB_GETIMAGELIST:           wstr::dbg(L"TB_GETIMAGELIST,  �����������ؼ�������Ĭ��״̬��ʾ��ť��ͼ���б�, ����ť���Ȼ�δ����ʱ���������ؼ�ʹ�ô�ͼ���б���ʾ��ť\n"); break;
    //case TB_GETIMAGELISTCOUNT:      wstr::dbg(L"TB_GETIMAGELISTCOUNT,  ��ȡ�빤����������ͼ���б������\n"); break;
    //case TB_GETINSERTMARK:          wstr::dbg(L"TB_GETINSERTMARK,  �����������ĵ�ǰ������\n"); break;
    //case TB_GETINSERTMARKCOLOR:     wstr::dbg(L"TB_GETINSERTMARKCOLOR,  �������ڻ��ƹ����������ǵ���ɫ\n"); break;
    //case TB_GETITEMDROPDOWNRECT:    wstr::dbg(L"TB_GETITEMDROPDOWNRECT,  ��ȡ��ʽΪBTNS_DROPDOWN�Ĺ�������Ŀ���������ڵı߽����\n"); break;
    ////case TB_GETITEMRECT:            wstr::dbg(L"TB_GETITEMRECT,  �����������а�ť�ı߽����\n"); break;
    //case TB_GETMAXSIZE:             wstr::dbg(L"TB_GETMAXSIZE,  ���������������пɼ���ť�ͷָ������ܴ�С\n"); break;
    //case TB_GETMETRICS:             wstr::dbg(L"TB_GETMETRICS,  �����������ؼ���ָ��\n"); break;
    //case TB_GETOBJECT:              wstr::dbg(L"TB_GETOBJECT,  �����������ؼ���IDropTarget\n"); break;
    //case TB_GETPADDING:             wstr::dbg(L"TB_GETPADDING,  �����������ؼ������\n"); break;
    //case TB_GETPRESSEDIMAGELIST:    wstr::dbg(L"TB_GETPRESSEDIMAGELIST,  ��ȡ�������ؼ������ڰ���״̬����ʾ��ť��ͼ���б�\n"); break;
    ////case TB_GETRECT:                wstr::dbg(L"TB_GETRECT,  ����ָ����������ť�ı߽����\n"); break;
    //case TB_GETROWS:                wstr::dbg(L"TB_GETROWS,  ��������TBSTYLE_WRAPABLE��ʽ�Ĺ������еİ�ť������\n"); break;
    //case TB_GETSTATE:               wstr::dbg(L"TB_GETSTATE,  �����йع�������ָ����ť״̬����Ϣ�������Ƿ������ã����»�ѡ����\n"); break;
    //case TB_GETSTRING:              wstr::dbg(L"TB_GETSTRING,  �ӹ��������ַ������м����ַ���\n"); break;
    //case TB_GETSTYLE:               wstr::dbg(L"TB_GETSTYLE,  ������ǰ���ڹ������ؼ�����ʽ\n"); break;
    //case TB_GETTEXTROWS:            wstr::dbg(L"TB_GETTEXTROWS,  ���������ڹ�������ť����ʾ������ı�����\n"); break;
    //case TB_GETTOOLTIPS:            wstr::dbg(L"TB_GETTOOLTIPS,  �����빤���������Ĺ�����ʾ�ؼ��ľ��������У�\n"); break;
    //case TB_GETUNICODEFORMAT:       wstr::dbg(L"TB_GETUNICODEFORMAT,  �����ؼ���Unicode�ַ���ʽ��־\n"); break;
    //case TB_HASACCELERATOR:         wstr::dbg(L"TB_HASACCELERATOR,  ���ڲ�ʹ�ã���������Ӧ�ó�����ʹ��, ��������ָ�����ټ��ַ��Ĺ�������ť������\n"); break;
    //case TB_HIDEBUTTON:             wstr::dbg(L"TB_HIDEBUTTON,  ���ػ���ʾ�������е�ָ����ť\n"); break;
    //case TB_HITTEST:                wstr::dbg(L"TB_HITTEST,  ȷ�����ڹ������ؼ��е�λ��\n"); break;
    //case TB_INDETERMINATE:          wstr::dbg(L"TB_INDETERMINATE,  ���û������������ָ����ť�Ĳ�ȷ��״̬\n"); break;
    //case TB_INSERTBUTTON:           wstr::dbg(L"TB_INSERTBUTTON,  �ڹ������в��밴ť\n"); break;
    //case TB_INSERTMARKHITTEST:      wstr::dbg(L"TB_INSERTMARKHITTEST,  ������������ĳ����Ĳ�������Ϣ\n"); break;
    //case TB_ISBUTTONCHECKED:        wstr::dbg(L"TB_ISBUTTONCHECKED,  ȷ���Ƿ�ѡ�й������е�ָ����ť\n"); break;
    //case TB_ISBUTTONENABLED:        wstr::dbg(L"TB_ISBUTTONENABLED,  ȷ���Ƿ������˹������е�ָ����ť\n"); break;
    //case TB_ISBUTTONHIDDEN:         wstr::dbg(L"TB_ISBUTTONHIDDEN,  ȷ���������ϵ�ָ����ť�Ƿ�����\n"); break;
    //case TB_ISBUTTONHIGHLIGHTED:    wstr::dbg(L"TB_ISBUTTONHIGHLIGHTED,  ��鹤������ť��ͻ����ʾ״̬\n"); break;
    //case TB_ISBUTTONINDETERMINATE:  wstr::dbg(L"TB_ISBUTTONINDETERMINATE,  ȷ���������е�ָ����ť�Ƿ�ȷ��\n"); break;
    //case TB_ISBUTTONPRESSED:        wstr::dbg(L"TB_ISBUTTONPRESSED,  ȷ���Ƿ��¹������е�ָ����ť\n"); break;
    //case TB_LOADIMAGES:             wstr::dbg(L"TB_LOADIMAGES,  ��ϵͳ����İ�ťͼ����ص��������ؼ���ͼ���б���\n"); break;
    //case TB_MAPACCELERATOR:         wstr::dbg(L"TB_MAPACCELERATOR,  ȷ����ָ���ļ��ټ��ַ���Ӧ�İ�ť��ID\n"); break;
    //case TB_MARKBUTTON:             wstr::dbg(L"TB_MARKBUTTON,  ���ù������ؼ��и�����ť��ͻ����ʾ״̬\n"); break;
    //case TB_MOVEBUTTON:             wstr::dbg(L"TB_MOVEBUTTON,  ����ť��һ�������ƶ�����һ������\n"); break;
    //case TB_PRESSBUTTON:            wstr::dbg(L"TB_PRESSBUTTON,  ���»��ͷŹ������е�ָ����ť\n"); break;
    //case TB_REPLACEBITMAP:          wstr::dbg(L"TB_REPLACEBITMAP,  ���µ�λͼ�滻���е�λͼ\n"); break;
    //case TB_SAVERESTORE:            wstr::dbg(L"TB_SAVERESTORE,  ���ʹ���Ϣ�����������ָ�������״̬\n"); break;
    //case TB_SETANCHORHIGHLIGHT:     wstr::dbg(L"TB_SETANCHORHIGHLIGHT,  ���ù�������ê��ͻ����ʾ����\n"); break;
    //case TB_SETBITMAPSIZE:          wstr::dbg(L"TB_SETBITMAPSIZE,  ����Ҫ��ӵ���������λͼͼ��Ĵ�С\n"); break;
    //case TB_SETBOUNDINGSIZE:        wstr::dbg(L"TB_SETBOUNDINGSIZE,  ���ڲ�ʹ�ã���������Ӧ�ó�����ʹ�����ö��й������ؼ��ı߽��С\n"); break;
    //case TB_SETBUTTONINFO:          wstr::dbg(L"TB_SETBUTTONINFO,  ���ù����������а�ť����Ϣ\n"); break;
    //case TB_SETBUTTONSIZE:          wstr::dbg(L"TB_SETBUTTONSIZE,  ���ù������ϰ�ť�Ĵ�С\n"); break;
    //case TB_SETBUTTONWIDTH:         wstr::dbg(L"TB_SETBUTTONWIDTH,  �ڹ������ؼ���������С�����ť���\n"); break;
    //case TB_SETCMDID:               wstr::dbg(L"TB_SETCMDID,  ���ù�������ť�������ʶ��\n"); break;
    //case TB_SETCOLORSCHEME:         wstr::dbg(L"TB_SETCOLORSCHEME,  ���ù������ؼ�����ɫ������Ϣ\n"); break;
    //case TB_SETDISABLEDIMAGELIST:   wstr::dbg(L"TB_SETDISABLEDIMAGELIST,  ���ù������ؼ���������ʾ���ð�ť��ͼ���б�\n"); break;
    //case TB_SETDRAWTEXTFLAGS:       wstr::dbg(L"TB_SETDRAWTEXTFLAGS,  ���ù��������ı����Ʊ�־\n"); break;
    //case TB_SETEXTENDEDSTYLE:       wstr::dbg(L"TB_SETEXTENDEDSTYLE,  ���ù������ؼ�����չ��ʽ\n"); break;
    //case TB_SETHOTIMAGELIST:        wstr::dbg(L"TB_SETHOTIMAGELIST,  ���ù������ؼ���������ʾ�ȼ���ͼ���б�\n"); break;
    //case TB_SETHOTITEM:             wstr::dbg(L"TB_SETHOTITEM,  �ڹ�����������������Ŀ\n"); break;
    //case TB_SETHOTITEM2:            wstr::dbg(L"TB_SETHOTITEM2,  �ڹ�����������������Ŀ\n"); break;
    //case TB_SETIMAGELIST:           wstr::dbg(L"TB_SETIMAGELIST,  ���ù�����������ʾ����Ĭ��״̬�İ�ť��ͼ���б�\n"); break;
    //case TB_SETINDENT:              wstr::dbg(L"TB_SETINDENT,  ���ù������ؼ��е�һ����ť������\n"); break;
    //case TB_SETINSERTMARK:          wstr::dbg(L"TB_SETINSERTMARK,  ���ù������ĵ�ǰ������\n"); break;
    //case TB_SETINSERTMARKCOLOR:     wstr::dbg(L"TB_SETINSERTMARKCOLOR,  �������ڻ��ƹ����������ǵ���ɫ\n"); break;
    //case TB_SETLISTGAP:             wstr::dbg(L"TB_SETLISTGAP,  �����ض��������Ϲ�������ť֮��ľ���\n"); break;
    //case TB_SETMAXTEXTROWS:         wstr::dbg(L"TB_SETMAXTEXTROWS,  ���ù�������ť����ʾ������ı�����\n"); break;
    //case TB_SETMETRICS:             wstr::dbg(L"TB_SETMETRICS,  ���ù������ؼ���ָ��\n"); break;
    //case TB_SETPADDING:             wstr::dbg(L"TB_SETPADDING,  ���ù������ؼ������\n"); break;
    //case TB_SETPARENT:              wstr::dbg(L"TB_SETPARENT,  ���ù������ؼ����䷢��֪ͨ����Ĵ���\n"); break;
    //case TB_SETPRESSEDIMAGELIST:    wstr::dbg(L"TB_SETPRESSEDIMAGELIST,  ���ù�����������ʾ���ڰ���״̬�İ�ť��ͼ���б�\n"); break;
    //case TB_SETROWS:                wstr::dbg(L"TB_SETROWS,  ���ù������а�ť������\n"); break;
    //case TB_SETSTATE:               wstr::dbg(L"TB_SETSTATE,  ���ù�������ָ����ť��״̬\n"); break;
    //case TB_SETSTYLE:               wstr::dbg(L"TB_SETSTYLE,  ���ù������ؼ�����ʽ\n"); break;
    //case TB_SETTOOLTIPS:            wstr::dbg(L"TB_SETTOOLTIPS,  ��������ʾ�ؼ��빤��������\n"); break;
    //case TB_SETUNICODEFORMAT:       wstr::dbg(L"TB_SETUNICODEFORMAT,  ���ÿؼ���Unicode�ַ���ʽ��־, ����Ϣʹ������������ʱ���Ŀؼ�ʹ�õ��ַ��������������´����ؼ�\n"); break;
    //case TB_SETWINDOWTHEME:         wstr::dbg(L"TB_SETWINDOWTHEME,  ���ù������ؼ����Ӿ���ʽ\n"); break;
    //    //case TB_TRANSLATEACCELERATOR: wstr::dbg(L"TB_TRANSLATEACCELERATOR,  ��������Ϣ���ݵ�������\n"); break;

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}


// �������Դ���׼����������ɾ����ťʱ�޸��Լ������Ĺ�����״̬
inline static void __notify_state(HWND hWnd, WPARAM id, DWORD state)
{
    static HWND hWndStandard = s_info->eWnd->GetMenuStandard();
    if (state == TBSTATE_ENABLED)
        state &= ~TBSTATE_ENABLED;  // ������״̬��ȥ������״̬
    else if (state == 0)
        state |= TBSTATE_ENABLED;   // û������״̬�ͼ�������״̬

    if ((state & TBSTATE_CHECKED) == TBSTATE_CHECKED)
        state = ((state & ~TBSTATE_CHECKED) | TBSTATE_ENABLED);   // ��ѡ��״̬�ͼ������ú�ȥ��ѡ��

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
    case 57600:     // �½�Դ��
        title = L"�½�Դ��";
        text = L"�����³���";
        break;
    case 57601:     // ��Դ��
        title = L"��Դ��(Ctrl+O)";
        text = L"��һ�����г���";
        break;
    case 57603:     // ����Դ��
        title = L"����Դ��(Ctrl+S)";
        text = L"���浱ǰ����";
        break;
    case 57635:     // ����
        title = L"����(Ctrl+X)";
        text = L"���б�ѡ�鲢�������ڼ�������";
        break;
    case 57634:     // ����
        title = L"����(Ctrl+C)";
        text = L"���Ʊ�ѡ�鲢�������ڼ�������";
        break;
    case 57637:     // ճ��
        title = L"ճ��(Ctrl+V)";
        text = L"�������������";
        break;
    case 57644:     // �ظ�
        title = L"�ظ�(Ctrl+Y)";
        text = L"����ִ����ǰ�ѳ����Ĳ���";
        break;
    case 57643:     // ����
        title = L"����(Ctrl+Z)";
        text = L"�������һ������";
        break;
    case 32934:     // ������Ѱ
        title = L"������Ѱ(Ctrl+Shift+F)";
        text = L"�ڳ�����Ѱ��ָ���ı����г������ҵ�����Ŀ";
        break;
    case 32854:     // ����
        title = L"����(F5)";
        text = L"���������׳���ĵ��԰汾.EXE�ļ�����������";
        break;
    case 32860:     // ֹͣ
        title = L"��ֹ";
        text = L"��ֹ�����׳��������";
        break;
    case 32940:     // �鿴
        title = L"�鿴(Shift+F9)";
        text = L"�鿴/�޸�ָ�����ʽ�����������";
        break;
    case 32853:     // ����
        title = L"����(F8)";
        text = L"�ڳ�����������λ�õ���ִ��һ�г�������˳����е������ӳ���ϵͳ������ٵ����ӳ�����ȥ��";
        break;
    case 32852:     // ��������
        title = L"��������(Shift+F8)";
        text = L"�ڳ�����������λ�õ���ִ��һ�г�������˳����е������ӳ�������ٽ����ӳ���";
        break;
    case 32867:     // ���ٷ���
        title = L"���ٷ���(Ctrl+Shift+F8)";
        text = L"���ϼ��ӳ�����������ӳ���������жϡ�";
        break;
    case 32871:     // ���е���괦
        title = L"���е���괦(Ctrl+F8)";
        text = L"�����׳����ڵ�ǰ������������д��жϡ�";
        break;
    case 32855:     // ���öϵ�
        title = L"���öϵ�(F9)";
        text = L"���û������ǰ�����д��Ķϵ�";
        break;
    case 32925:     // ֪ʶ��
        title = L"֪ʶ��";
        text = L"�򿪲���ʾ������֪ʶ��";
        break;
    case 32905:     // ����
        title = L"������ǰλ��(Ctrl+J)";
        text = L"���ص���ǰ��תʱ�ĳ���λ��";    //(Ctrl+Shift+Backspace)
        break;
    case 32906:     // ǰ��
        title = L"��ת�����崦(Ctrl+B/Ctrl+����)";
        text = L"��ת����ǰ����������ŵĶ���λ��"; // (Ctrl+Shift+Alt+Backspace)
        break;
    case 32850:     // Ԥ��
        title = L"Ԥ��";
        text = L"Ԥ�����д��ڣ���Esc���˳�Ԥ��";
        break;
    case 111:       // �����
        title = L"�����";
        text = L"�����б�ѡ���������������ѡ�������߶���";
        break;
    case 112:       // �Ҷ���
        title = L"�Ҷ���";
        text = L"�����б�ѡ��������ұ������ѡ������ұ߶���";
        break;
    case 190:       // ������
        title = L"������";
        text = L"�����б�ѡ������Ķ��������ѡ��������߶���";
        break;
    case 191:       // �׶���
        title = L"�׶���";
        text = L"�����б�ѡ������ĵױ������ѡ������ױ߶���";
        break;
    case 192:       // ����ˮƽ����
        title = L"����ˮƽ����";
        text = L"����ѡ�����������������ˮƽ����";
        break;
    case 196:       // ���ڴ�ֱ����
        title = L"���ڴ�ֱ����";
        text = L"����ѡ����������������д�ֱ����";
        break;
    case 150:       // ���ˮƽ����
        title = L"���ˮƽ����";
        text = L"�����б�ѡ�����ˮƽ����";
        break;
    case 151:       // �����ֱ����
        title = L"�����ֱ����";
        text = L"�����б�ѡ�������ֱ����";
        break;
    case 193:       // ˮƽƽ���ֲ�
        title = L"ˮƽƽ���ֲ�";
        text = L"�����б�ѡ�����ˮƽƽ���ֲ�";
        break;
    case 194:       // ��ֱƽ���ֲ�
        title = L"��ֱƽ���ֲ�";
        text = L"�����б�ѡ�������ֱƽ���ֲ�";
        break;
    case 195:       // �ȿ�
        title = L"�ȿ�";
        text = L"�����б�ѡ������Ŀ�ȵ���Ϊ���ѡ������Ŀ��";
        break;
    case 120:       // �ȸ�
        title = L"�ȸ�";
        text = L"�����б�ѡ������ĸ߶ȵ���Ϊ���ѡ������ĸ߶�";
        break;
    case 121:       // �ȿ��
        title = L"�ȸ߿�";
        text = L"�����б�ѡ������ĸ߶ȼ���ȵ���Ϊ���ѡ������ĸ߶ȼ����";
        break;
    case 32917:           // "&C.����\tF7", 32851
        title = L"����(F7)";
        text = L"���������׳�������շ����汾��������Ӧ��.EXE��ִ�г����ļ�";
        break;
    case 32894:    // "&S.��̬����\tShift+F7", 32912
        title = L"��̬����(Shift+F7)";
        text = L"�������̬��ִ��EXE�ļ������ļ��������κ�������ϵͳ�ļ���������δ��װ������ϵͳ�ĵ��������С�";
        break;
    case 32884: // "&B.���¾�̬����", 32912
        title = L"���¾�̬����";
        text = L"���¾�̬����";
        break;
    case 32882:      // "&D.��������", 32911
        title = L"��������";
        text = L"�����������ִ��EXE�ļ������ļ��������κ�������ϵͳ�ļ���������δ��װ������ϵͳ�ĵ��������С�";
        break;
    case 32915:         // ��������
        title = L"��������";
        text = L"ֹͣ��ǰ���Եĳ������±�������";
        break;
    case 32914:    // ��׼������ -> ��һ����ǩ
        title = L"��һ����ǩ";
        text = L"��ת������������ǩλ��";
        break;
    case 32913:    // ��׼������ -> ��һ����ǩ
        title = L"��һ����ǩ";
        text = L"��ת��ǰ��������ǩλ��";
        break;
    case 32908:     // ��׼������ -> ������ǩ
        title = L"������ǩ";
        text = L"���û�ȡ����ǰλ�ô�����ǩ��־";
        break;
    case 32897:     // ��׼������ -> ɾ����ǩ
        title = L"ɾ����ǩ";
        text = L"ɾ����ǰλ�ô�����ǩ��־";
        break;
    case 32892:       // ��׼������ -> ע��
        title = L"ע��";
        text = L"���ε�ǰ��ѡ�еĴ����";
        break;
    case 32898:     // ��׼������ -> ȡ��ע�� 
        title = L"ȡ��ע��";
        text = L"������ε�ǰ��ѡ�еĴ����";
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
    //wstr::dbg(L"������֪ͨ�¼�, cdde = %d, id= %d\n", hdr->code, hdr->idFrom);
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
        //text.Format(L"�ȵ㱻�ı�, ��ǰ��Ŀid = %d, ǰһ����Ŀid = %d", hi->idNew, hi->idOld);
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
        // ��ʾ������, ֹͣ��ť�����а�ť��������״̬����Ҫ��һ��������ʾ����, �������ʾֻ�е���״̬�����ĸ���ǩ
        LPNMTOOLBARW item = (LPNMTOOLBARW)lParam;
        switch (item->iItem)
        {
        case 32854:     // ����
        {
            s_info->isRun = (item->tbButton.fsState != TBSTATE_ENABLED);   // ���а�ť������״̬
            if (!s_info->isRun)
                s_info->pfnControls(IDC_ICONTROL_BUILD, BUILD_STATE_BEGIN, 0);
            break;
        }
        case 32860:     // ֹͣ
        {
            s_info->isStop = (item->tbButton.fsState != TBSTATE_ENABLED);   // ֹͣ��ť������״̬
            SendMessageW(s_info->hEWnd, WM_ACTIVATEAPP, 1, (LPARAM)0);      // ֹͣ��ť״̬���ı�͸ı�߿���ɫ, ֹͣ��ť�ǽ�ֹ״̬һ�����ǵ���״̬
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
        // ���л���ֹͣ��ť״̬�ı䶼��Ҫ���µ����������λ��
        if (32854 == item->iItem || 32860 == item->iItem)
        {
            bool isVarTable = false;    // ��ǰѡ���Ƿ��б�����
            TCITEMW item = { 0 };
            int index = TabCtrl_GetCurSel(s_info->hTabStateLeft);   // ���ж���ߵ�ѡ���ѡ�����Ƿ��Ǳ�����
            item.mask = TCIF_PARAM;
            TabCtrl_GetItem(s_info->hTabStateLeft, index, &item);
            if (item.lParam == TAB_PARAM_VARTABLE)
                isVarTable = true;

            if (!isVarTable)
            {
                index = TabCtrl_GetCurSel(s_info->hTabStateRight);   // ���ж��ұߵ�ѡ���ѡ�����Ƿ��Ǳ�����
                item.mask = TCIF_PARAM;
                TabCtrl_GetItem(s_info->hTabStateRight, index, &item);
                if (item.lParam == TAB_PARAM_VARTABLE)
                    isVarTable = true;
            }
            if (isVarTable)
            {
                // ���ѡ�е��Ǳ�����, ��Ҫ����һ�±��ѡ��, �ñ��������µ�����ʾ�����
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

        if (hdr->hwndFrom == s_info->hWndTool)  // ������ǽӿڹ�����, ��Ҫ���¼����ݳ�ȥ
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
    case TOOL_STANDARD_NEW:         // ��ע������ -> �½�Դ��
    case TOOL_STANDARD_OPEN:        // ��ע������ -> ��Դ��
    case TOOL_STANDARD_SAVE:        // ��ע������ -> ����
    case TOOL_STANDARD_CUT:         // ��ע������ -> ����
    case TOOL_STANDARD_COPY:        // ��ע������ -> ����
    case TOOL_STANDARD_PASTE:       // ��ע������ -> ճ��
    case TOOL_STANDARD_REOD:        // ��ע������ -> �ظ�/����
    case TOOL_STANDARD_UNDO:        // ��ע������ -> ����
    case TOOL_STANDARD_FIND:        // ��ע������ -> ����
    case TOOL_STANDARD_RUN:         // ��ע������ -> ����
    case TOOL_STANDARD_STOP:        // ��ע������ -> ֹͣ
    case TOOL_STANDARD_LOOKOVER:    // ��ע������ -> �鿴
    case TOOL_STANDARD_STEPOVER:    // ��ע������ -> ����
    case TOOL_STANDARD_STEPINTO:    // ��ע������ -> ��������
    case TOOL_STANDARD_STEPOUT:     // ��ע������ -> ���ٷ���
    case TOOL_STANDARD_INTO:        // ��ע������ -> ���е���괦
    case TOOL_STANDARD_BREAKING:    // ��ע������ -> ���öϵ�
    case TOOL_STANDARD_LIBRARY:     // ��ע������ -> ֪ʶ��
        // ���ϵİ�ť�¼�����Ҫ���������Դ���
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);


    case TOOL_STANDARD_WORK:        // ��ע������ -> ������
    case TOOL_STANDARD_STATE:       // ��ע������ -> ״̬��
    case TOOL_STANDARD_WORK_STATE:  // ��ע������ -> ��������״̬�����
        // �⼸���ı䲼�ֵİ�ť�Ͳ��ô���
        return 0;
    case TOOL_STANDARD_BACK:        // ��ע������ -> ����
        EIDE_Toolbar_Back();
        //s_info->pfnCallMenu(33003);     // &Z.������ǰλ��\tCtrl+J, 32851
        return 0;
    case TOOL_STANDARD_FORWARD:     // ��ע������ -> ǰ��
        EIDE_Toolbar_Forward();
        //s_info->pfnCallMenu(32885); // &S.��ת�����崦\tCtrl+B/Ctrl+����, 32885
        return 0;

    case TOOL_STANDARD_BUILD:           // ��׼������ -> ����
        s_info->pfnCallMenu(32851);     // "&C.����\tF7", 32851
        break;
    case TOOL_STANDARD_BUILD_STATIC:    // ��׼������ -> ��̬����
        s_info->pfnCallMenu(32912);     // "&S.��̬����\tShift+F7", 32912
        break;
    case TOOL_STANDARD_BUILD_BLACKMOON: // ��׼������ -> ���±���
        s_info->pfnCallMenu(32912);     // "&B.���¾�̬����", 32912
        break;
    case TOOL_STANDARD_BUILD_DULI:      // ��׼������ -> ��������
        s_info->pfnCallMenu(32911);     // "&D.��������", 32911
        break;
    case TOOL_STANDARD_RESTART:     // ��׼������ -> ��������
        EWindow_ReStart();
        break;
    case TOOL_STANDARD_NEXTMARK:    // ��׼������ -> ��һ����ǩ
        _nes_run_fun(FN_NEXT_BOOKMARK, 0, 0);
        wstr::dbg(L"����˱�׼����������һ����ǩ��ť, id = %d\n", id);
        break;
    case TOOL_STANDARD_PREVMARK:    // ��׼������ -> ��һ����ǩ
        _nes_run_fun(FN_PREV_BOOKMARK, 0, 0);
        wstr::dbg(L"����˱�׼����������һ����ǩ��ť, id = %d\n", id);
        break;
    case TOOL_STANDARD_SETMARK:     // ��׼������ -> ������ǩ
        _nes_run_fun(FN_SET_BOOKMARK, 0, 0);
        wstr::dbg(L"����˱�׼��������������ǩ��ť, id = %d\n", id);
        break;
    case TOOL_STANDARD_DELMARK:     // ��׼������ -> ɾ����ǩ
        _nes_run_fun(FN_SET_BOOKMARK, 0, 0);
        wstr::dbg(L"����˱�׼��������ɾ����ǩ��ť, id = %d\n", id);
        break;
    case TOOL_STANDARD_NOTES:       // ��׼������ -> ע��
        wstr::dbg(L"����˱�׼��������ע�Ͱ�ť, id = %d\n", id);
        break;
    case TOOL_STANDARD_UNNOTES:     // ��׼������ -> ȡ��ע�� 
        wstr::dbg(L"����˱�׼��������ȡ��ע�� ��ť, id = %d\n", id);
        break;

    case TOOL_ALIGNMENT_PREVIEW:        // ���빤���� -> Ԥ��
    case TOOL_ALIGNMENT_LEFT:           // ���빤���� -> �����
    case TOOL_ALIGNMENT_RIGHT:          // ���빤���� -> �Ҷ���
    case TOOL_ALIGNMENT_TOP:            // ���빤���� -> ������
    case TOOL_ALIGNMENT_BOTTOM:         // ���빤���� -> �׶���
    case TOOL_ALIGNMENT_WND_CENTER:     // ���빤���� -> ����ˮƽ����
    case TOOL_ALIGNMENT_WND_VCENTER:    // ���빤���� -> ���ڴ�ֱ����
    case TOOL_ALIGNMENT_CTL_CENTER:     // ���빤���� -> ���ˮƽ����
    case TOOL_ALIGNMENT_CTL_VCENTER:    // ���빤���� -> �����ֱ����
    case TOOL_ALIGNMENT_AVERAGE:        // ���빤���� -> ˮƽƽ���ֲ�
    case TOOL_ALIGNMENT_VAVERAGE:       // ���빤���� -> ��ֱƽ���ֲ�
    case TOOL_ALIGNMENT_EQUAL_WIDTH:    // ���빤���� -> �ȿ�
    case TOOL_ALIGNMENT_EQUAL_HEIGHT:   // ���빤���� -> �ȸ�
    case TOOL_ALIGNMENT_EQUAL_WH:       // ���빤���� -> �ȿ��
        // ���빤�������е��¼������������Դ���
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    default:
        wstr::dbg(L"����˱�׼�������İ�ť, id = %d\n", id);
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
    if (hTool == hMenu_Standard || hTool == hMenu_Align)    // ������Ǳ�׼�������İ�ť���߶��빤�����İ�ť
    {
        return _tool_click(hWnd, message, wParam, lParam, pData);
    }

    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

// ����������������ť, ��Ҫ��ֹͣ��ǰ����Ȼ���ڵ������
void EWindow_ReStart(int idRun, int idStop)
{
    static ULONGLONG t = 0;
    if (GetTickCount64() - t < 500)
    {
        return;
    }
    t = GetTickCount64();
    // ֹͣ��ť������״̬��������״̬
    // TODO ���û������ʱ�������������Ч�Ͱ������һ������ɾ������
    //if (s_info->isRun || s_info->isStop)
    if (s_info->isStop)
    {
        s_info->pfnCallMenu(idStop);
        sleep(100);
        s_info->pfnCallMenu(idRun);
    }
}

// ��ѯ�������Ƿ���������ID�İ�ť, �д����������ť����true, ���򷵻�false
// plugin = �����Ϣ
// id = ��ѯ��id
// pBtnData = ���ղ�ѯ���İ�ťid����Ϣ
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
    // ��һ�δ����Ǵ�����������ť������, �����жϼ��������Ƿ��㹻����
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
    int len = SendMessageW(s_info->hWndTool, TB_BUTTONCOUNT, 0, 0);             // �ӿڹ�������ť����
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
        s_info->SetLastError(L"��ӹ�������ťʧ��, �к�: %d, �ļ�:" __FILEW__, __LINE__);
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

// data = ��ťָ�������
// pArg = �����õ�����
// isSet = �Ƿ�����, ���Ϊtrue, ���ͷ�data������, Ȼ���pArg������data, Ϊfalse���ж�pArg����ֵ, Ϊ0������
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
            InvalidateRect(s_info->hWndTool, 0, 0); // ֻ�л�ͼƬ���ػ�
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
        s_info->SetLastError(L"��� %s �Ľӿڹ�������ť(ID:%d)������, ���ð�ť��Ϣʧ��\n", _str::A2W(plugin->guid).c_str(), id);
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