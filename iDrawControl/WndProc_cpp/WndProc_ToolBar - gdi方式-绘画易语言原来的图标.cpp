#include "../EWindow_Fne_Header.h"
#include "../resource.h"
#include "../pch.h"
#include <assist/assist_Resource.h>

struct _TOOL_DEFAULT_ICON_STRUCT
{
    int id;             // ��ֵID
    LPCWSTR szid;     // �ı�ID, ���ID����ֵid���ı���ʽ
    LPCWSTR name;       // �����ť������, ���� ǰ��, ����, ����, ֹͣ....
    LPCWSTR szrc;       // �ı���ʽ�ľ���
    RECT rc;            // �ı���ʽ�ľ��ε�ת��
};
struct TOOL_ICON_DATA
{
    Gdiplus::Bitmap* hBitmap[4];    // �滭��ͼƬ, 4��״̬, 0=֧��,1=�ȵ�,2=����,3=��ֹ
    RECT rc[4];                     // ��ͼƬ�ĸ�λ�û滭, 4��״̬, 0=֧��,1=�ȵ�,2=����,3=��ֹ
    bool isFree[4];
};
static Gdiplus::Bitmap* m_hBitmap;
static Gdiplus::Bitmap* m_hDefBitmap;
static std::map<int, RECT> m_map_tool_icon;

static _TOOL_DEFAULT_ICON_STRUCT m_tool_names[] =
{
    { 57600, L"57600", L"�½�Դ��", L"0,0,16,16", { 0,0,16,16 }, },
    { 57601, L"57601", L"��Դ��", L"16,0,32,16", { 16,0,32,16 }, },
    { 57603, L"57603", L"����Դ��", L"32,0,48,16", { 32,0,48,16 }, },
    { 57635, L"57635", L"����", L"48,0,64,16", { 48,0,64,16 }, },
    { 57634, L"57634", L"����", L"64,0,80,16", { 64,0,80,16 }, },
    { 57637, L"57637", L"ճ��", L"80,0,96,16", { 80,0,96,16 }, },
    { 57644, L"57644", L"�ظ�", L"112,0,128,16", { 112,0,128,16 }, },
    { 57643, L"57643", L"����", L"128,0,144,16", { 128,0,144,16 }, },
    { 32934, L"32934", L"������Ѱ", L"144,0,160,16", { 144,0,160,16 }, },
    { 32854, L"32854", L"����", L"160,0,176,16", { 160,0,176,16 }, },
    { 32860, L"32860", L"ֹͣ", L"176,0,192,16", { 176,0,192,16 }, },
    { 32940, L"32940", L"�鿴", L"192,0,208,16", { 192,0,208,16 }, },
    { 32853, L"32853", L"����", L"208,0,224,16", { 208,0,224,16 }, },
    { 32852, L"32852", L"��������", L"224,0,240,16", { 224,0,240,16 }, },
    { 32867, L"32867", L"���ٷ���", L"240,0,256,16", { 240,0,256,16 }, },
    { 32871, L"32871", L"���е���괦", L"256,0,272,16", { 256,0,272,16 }, },
    { 32855, L"32855", L"���öϵ�", L"272,0,288,16", { 272,0,288,16 }, },
    { 32925, L"32925", L"֪ʶ��", L"288,0,304,16", { 288,0,304,16 }, },
    { 32850, L"32850", L"Ԥ��", L"304,0,320,16", { 304,0,320,16 }, },
    { 111, L"111", L"�����", L"320,0,336,16", { 320,0,336,16 }, },
    { 112, L"112", L"�Ҷ���", L"336,0,352,16", { 336,0,352,16 }, },
    { 190, L"190", L"������", L"352,0,368,16", { 352,0,368,16 }, },
    { 191, L"191", L"�׶���", L"368,0,384,16", { 368,0,384,16 }, },
    { 192, L"192", L"����ˮƽ����", L"384,0,400,16", { 384,0,400,16 }, },
    { 196, L"196", L"���ڴ�ֱ����", L"400,0,416,16", { 400,0,416,16 }, },
    { 150, L"150", L"���ˮƽ����", L"416,0,432,16", { 416,0,432,16 }, },
    { 151, L"151", L"�����ֱ����", L"432,0,448,16", { 432,0,448,16 }, },
    { 193, L"193", L"ˮƽƽ���ֲ�", L"448,0,464,16", { 448,0,464,16 }, },
    { 194, L"194", L"��ֱƽ���ֲ�", L"464,0,480,16", { 464,0,480,16 }, },
    { 195, L"195", L"�ȿ�", L"480,0,496,16", { 480,0,496,16 }, },
    { 120, L"120", L"�ȸ�", L"496,0,512,16", { 496,0,512,16 }, },
    { 121, L"121", L"�ȿ��", L"512,0,528,16", { 512,0,528,16 }, },

    { 32905, L"32905", L"����", L"528,0,544,16", { 528,0,544,16 }, },
    { 32905, L"32906", L"ǰ��", L"560,0,576,16", { 560,0,576,16 }, },

};


// ����id��ȡ�滭ͼ��, ���ػ滭��Ҫ��λͼ��ͼ����λͼ���λ��
inline static Gdiplus::Bitmap* _icon_map(int id, RECT* prc)
{
    memset(prc, 0, sizeof(RECT));
    static std::map<int, RECT> map;
    if (!map.empty())
    {
        const int len = sizeof(m_tool_names) / sizeof(m_tool_names[0]);
        for (int i = 0; i < len; i++)
            map[m_tool_names[i].id] = m_tool_names[i].rc;
    }

    bool isOk = false;
    if (m_map_tool_icon.empty())
    {
        // �ȴ���ɫ�������ȡ��������ͼƬ��Ϣ
        tinyxml2::XMLNode* node = 0;
        if (!s_info->crXml->GetNode(L"@1/tool_icon", node))
        {
            s_info->crXml->InsertChildNode(L"@1", L"tool_icon", 0, 0, &node);
        }
        assert(node);

        LPCWSTR xmlPath = s_info->crXml->GetAttrValue(L"@1/tool_icon", L"xmlpath");
        if (!xmlPath || !xmlPath[0])
        {
            xmlPath = L"tool.xml";
            isOk = s_info->crXml->SetAttrValue(node, L"xmlpath", xmlPath);
            assert(isOk && L"���ù�����xml·��ʧ��");
        }

        // �����Ѿ���ȡ���˹�������xml�ļ���, ���û�л�д��, ���԰�ʧ�ܻ����
        // ���濪ʼ�����ȡ���������xml�ļ�, �������ʧ��, ���Ĭ�ϵ�����д��
        CTinyXml2W xml;
        wstr path(260);
        path.assign(s_info->dataPath).append(xmlPath);

        if (!xml.LoadFile(path.c_str()))    // �������ʧ��, ��Ҫ��Ĭ�����ö�д��
        {
            xml.Create(L"ToolIcon");
            xml.SetAttrValue(L"@1", L"png", L"tool.png");

            const int len = sizeof(m_tool_names) / sizeof(m_tool_names[0]);
            for (int i = 0; i < len; i++)
            {
                xml.InsertChildNode(L"@1", m_tool_names[i].szid, m_tool_names[i].name, false, &node);
                isOk = xml.InsertAttr(node, L"rc", m_tool_names[i].szrc);
                assert(isOk && L"д�빤����xmlĬ������ʧ��");

            }
        }

        // ��xml�����png�ļ���
        LPCWSTR pngName = xml.GetAttrValue(L"@1", L"png");
        if (!pngName || !pngName[0])
        {
            pngName = L"tool.png";
            isOk = xml.InsertAttr(L"@1", L"png", pngName);
            assert(isOk && L"д�빤����xmlĬ��ͼƬ��ʧ��");
        }

        DWORD size = 0;
        LPBYTE ptr = GetResource(g_hInst, IDPNG_TOOL, &size, L"PNG");
        m_hDefBitmap = _img_create_from_memory(ptr, size);

        wstr png(260);
        png.assign(s_info->dataPath).append(pngName);
        m_hBitmap = new Gdiplus::Bitmap(png.c_str());
        if (m_hBitmap->GetLastStatus() != Gdiplus::Status::Ok)
        {
            delete m_hBitmap;
            m_hBitmap = m_hDefBitmap;
        }

    }

    auto it = m_map_tool_icon.find(id);
    if (it == m_map_tool_icon.end())
    {
        auto def = map.find(id);
        if (def == map.end())
        {
            __debugbreak();
            return 0;
        }
        *prc = def->second;
        return m_hDefBitmap;
    }
    *prc = it->second;
    return m_hBitmap;
}

LRESULT CALLBACK WndProc_ToolBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ToolBar_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ToolBar_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ToolBar_CalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


LPOBJSTRUCT _subclass_tool(HWND hWnd)
{
    LPOBJSTRUCT pData = _subclass_tool(hWnd, WndProc_ToolBar, true, true);
    //pData->fnNotify = WndProc_ToolBar_OnNotify;
    pData->fnCommand = WndProc_ToolBar_OnCommand;

    return pData;
}



LRESULT CALLBACK WndProc_ToolBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    static HWND hWndNormal, hWndAlign;
    switch (message)
    {
    case WM_PAINT:
    {
        RECT rc, rcItem;
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;
        const LPEX_FONT pFont = pData->draw.hFont ? pData->draw.hFont : g_defData.draw.hFont;    // �滭����
        const HFONT hFont = pFont->hFont;    // �滭����

        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        assert(md);
        HDC hdc = md->hdc;  // ���md=��, ����������
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, pData->draw.color.crText);
        SelectObject(hdc, hFont);
        Gdiplus::Graphics gp(hdc);


        FillRect(hdc, &rc, pData->draw.color.hbrBackGround);

        const int index = SendMessageW(hWnd, TB_GETHOTITEM, 0, 0);      // ��ǰ�������ȵ�����
        const int len = SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);       // ��ǰ����������

        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TBBUTTONINFOW item = { 0 };
        item.cbSize = sizeof(item);
        item.dwMask = TBIF_BYINDEX | TBIF_IMAGE | TBIF_LPARAM | TBIF_STATE | TBIF_STYLE | TBIF_TEXT | TBIF_COMMAND;
        item.pszText = text;
        item.cchText = bufSize;

        for (int i = 0; i < len; i++)
        {
            HIMAGELIST hList = 0;
            SendMessageW(hWnd, TB_GETITEMRECT, i, (LPARAM)&rcItem);
            // ��Ŀ���ұ�С���ػ���������, ��ʾ��Ŀ���ػ���������, û���ڻ��Ʒ�Χ��
            // ��Ŀ����ߴ����ػ�������ұ�, ��ʾ��Ŀ���ػ�������ұ�, û���ڻ��Ʒ�Χ��
            if (rcItem.right < ps.rcPaint.left || rcItem.left > ps.rcPaint.right)
                continue;

            SendMessageW(hWnd, TB_GETBUTTONINFO, i, (LPARAM)&item);
            const int cxItem = rcItem.right - rcItem.left;
            const int cyItem = rcItem.bottom - rcItem.top;
            COLORREF crBack;
            if (i == index)
            {
                FillRect(hdc, &rcItem, pData->draw.color.hbrHot);
                SetTextColor(hdc, pData->draw.color.crTextHot);
                crBack = pData->draw.color.crHot;
            }
            else
            {
                SetTextColor(hdc, pData->draw.color.crText);
                crBack = pData->draw.color.crBackGround;
            }

            if ((item.fsStyle & BTNS_SEP) == BTNS_SEP)  // �ָ���
            {
                const int left = rcItem.left + cxItem / 2;
                MoveToEx(hdc, left, rcItem.top + 1, 0);
                LineTo(hdc, left, rcItem.bottom - 1);
                continue;
            }

            auto it = m_map_tool_icon.find(item.idCommand);
            RECT* prcIconDraw = 0;
            TOOL_ICON_DATA* iconData = 0;   // �Ƿ��map���ȡͼƬ�滭, ��Ϊ0��������ȡͼƬ�滭
            if (it != m_map_tool_icon.end())
                iconData = &(it->second);
            
            Gdiplus::Bitmap* hBitmapGdip = 0;
            if (i == index)   // �ȵ���Ŀ
            {
                hList = (HIMAGELIST)SendMessageW(hWnd, TB_GETHOTIMAGELIST, 0, 0);
                if (iconData)
                {
                    hBitmapGdip = iconData->hBitmap[1];
                    prcIconDraw = &iconData->rc[1];
                }
            }
            else
            {
                hList = (HIMAGELIST)SendMessageW(hWnd, TB_GETIMAGELIST, 0, 0);
                if (iconData)
                {
                    hBitmapGdip = iconData->hBitmap[0];
                    prcIconDraw = &iconData->rc[0];
                }
            }

            if ((item.fsState & TBSTATE_CHECKED) == TBSTATE_CHECKED)    // ѡ��״̬
            {
                FillRect(hdc, &rcItem, pData->draw.color.hbrHot);
                OffsetRect(&rcItem, 1, 1);
            }
            if ((item.fsState & TBSTATE_ENABLED) == TBSTATE_ENABLED)    // ����״̬
            {

            }
            if ((item.fsState & TBSTATE_PRESSED) == TBSTATE_PRESSED)    // �����ð�ť
            {
                OffsetRect(&rcItem, 1, 1);
            }
            if (iconData && hBitmapGdip)
            {
                const int cxIcon = 16;
                const int cyIcon = 16;

                Gdiplus::Rect rcDest;
                rcDest.X = rcItem.left + (cxItem - cxIcon) / 2;
                rcDest.Y = rcItem.top + (cyItem - cyIcon) / 2;
                rcDest.Width = cxIcon;
                rcDest.Height = cyIcon;

                if ((item.fsState & TBSTATE_ENABLED) == TBSTATE_ENABLED)    // û�����״̬���ǽ�ֹ
                {
                    // ������״̬��ֱ�ӻ�

                    gp.DrawImage(hBitmapGdip, rcDest, prcIconDraw->left, prcIconDraw->top,
                        prcIconDraw->right - prcIconDraw->left, prcIconDraw->bottom - prcIconDraw->top, Gdiplus::Unit::UnitPixel);
                    hList = 0;  // ���ú���滭��
                }
                else
                {
                    // �ǽ�ֹ״̬, ��ͼƬ�ĳɽ�ֹ��Ч���ڻ滭��dc��
                    hBitmapGdip = iconData->hBitmap[3];
                    prcIconDraw = &iconData->rc[3];
                    if (hBitmapGdip)
                    {
                        gp.DrawImage(hBitmapGdip, rcDest, prcIconDraw->left, prcIconDraw->top,
                            prcIconDraw->right - prcIconDraw->left, prcIconDraw->bottom - prcIconDraw->top, Gdiplus::Unit::UnitPixel);
                        hList = 0;
                    }
                }
            }

            if (hList)
            {
                HICON hIcon = ImageList_GetIcon(hList, item.iImage, ILD_TRANSPARENT);
                if (hIcon)
                {
                    int cxIcon, cyIcon;
                    ImageList_GetIconSize(hList, &cxIcon, &cyIcon);
                    if ((item.fsState & TBSTATE_ENABLED) == TBSTATE_ENABLED)    // û�����״̬���ǽ�ֹ
                    {
                        // ������״̬��ֱ�ӻ�
                        DrawIconEx(hdc,
                            rcItem.left + (cxItem - cxIcon) / 2,
                            rcItem.top + (cyItem - cyIcon) / 2,
                            hIcon, cxIcon, cyIcon, 0, 0, DI_NORMAL);
                    }
                    else
                    {
                        // �ǽ�ֹ״̬, ��ͼƬ�ĳɽ�ֹ��Ч���ڻ滭��dc��
                        LPMEMDCBITMAP md_ban = _image_to_disabled(hIcon, cxIcon, cyIcon, crBack);
                        BitBlt(hdc,
                            rcItem.left + (cxItem - cxIcon) / 2,
                            rcItem.top + (cyItem - cyIcon) / 2,
                            cxIcon, cyIcon, md_ban->hdc, 0, 0, SRCCOPY);
                        md_destroy(md_ban);
                    }
                    DestroyIcon(hIcon);
                }
            }
        }
        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);

        break;
    }
    case WM_NCCALCSIZE:
        return WndProc_ToolBar_CalcSize(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CtlColor(hWnd, message, wParam, lParam, pData);
    }
    return 0;
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
}


// �������Դ���׼����������ɾ����ťʱ�޸��Լ������Ĺ�����״̬
inline static void __notify_state(HWND hWnd, WPARAM id, DWORD state)
{
    if (state == TBSTATE_ENABLED)
        state &= ~TBSTATE_ENABLED;  // ������״̬��ȥ������״̬
    else if (state == 0)
        state |= TBSTATE_ENABLED;   // û������״̬�ͼ�������״̬

    if ((state & TBSTATE_CHECKED) == TBSTATE_CHECKED)
        state = ((state & ~TBSTATE_CHECKED) | TBSTATE_ENABLED);   // ��ѡ��״̬�ͼ������ú�ȥ��ѡ��

    SendMessageW(s_info->hMenu_Normal, TB_SETSTATE, id, state);
}
LRESULT CALLBACK WndProc_ToolBar_OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
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

LRESULT CALLBACK WndProc_ToolBar_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPOBJSTRUCT pChild = GetDataFormHwnd((HWND)lParam);
    if (!pChild)
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

    const int code = HIWORD(wParam);
    const int id = LOWORD(wParam);
    if ((HWND)lParam == s_info->hMenu_Normal)       // ������Ǳ�׼�������İ�ť
    {
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
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

        case TOOL_STANDARD_WORK:        // ��ע������ -> ������
        case TOOL_STANDARD_STATE:       // ��ע������ -> ״̬��
        case TOOL_STANDARD_WORK_STATE:  // ��ע������ -> ��������״̬�����
            // �⼸���ı䲼�ֵİ�ť�Ͳ��ô���
            return 0;
        case TOOL_STANDARD_BACK:        // ��ע������ -> ����
            SendMessageW(s_info->hEWnd, WM_COMMAND, 33003, 0);  // &Z.������ǰλ��\tCtrl+J
            return 0;
        case TOOL_STANDARD_FORWARD:     // ��ע������ -> ǰ��
            SendMessageW(s_info->hEWnd, WM_COMMAND, 32885, 0);  // &S.��ת�����崦\tCtrl+B/Ctrl+����
            return 0;
        default:
            wstr::dbg(L"����˱�׼�������İ�ť, id = %d\n", id);
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        }
        return 0;
    }
    else if ((HWND)lParam == s_info->hMenu_Align)   // ������Ƕ��빤�����İ�ť
    {
        switch (id)
        {
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
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        default:
            break;
        }
        return 0;
    }


    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

}
