#include "../EWindow_Fne_Header.h"
#include "../resource.h"
#include "../pch.h"
#include <assist/assist_Resource.h>

struct _TOOL_DEFAULT_ICON_STRUCT
{
    int id;             // 数值ID
    LPCWSTR szid;     // 文本ID, 这个ID是数值id的文本形式
    LPCWSTR name;       // 这个按钮的名字, 比如 前进, 后退, 运行, 停止....
    LPCWSTR szrc;       // 文本形式的矩形
    RECT rc;            // 文本形式的矩形的转换
};
struct TOOL_ICON_DATA
{
    Gdiplus::Bitmap* hBitmap[4];    // 绘画的图片, 4个状态, 0=支持,1=热点,2=按下,3=禁止
    RECT rc[4];                     // 从图片哪个位置绘画, 4个状态, 0=支持,1=热点,2=按下,3=禁止
    bool isFree[4];
};
static Gdiplus::Bitmap* m_hBitmap;
static Gdiplus::Bitmap* m_hDefBitmap;
static std::map<int, RECT> m_map_tool_icon;

static _TOOL_DEFAULT_ICON_STRUCT m_tool_names[] =
{
    { 57600, L"57600", L"新建源码", L"0,0,16,16", { 0,0,16,16 }, },
    { 57601, L"57601", L"打开源码", L"16,0,32,16", { 16,0,32,16 }, },
    { 57603, L"57603", L"保存源码", L"32,0,48,16", { 32,0,48,16 }, },
    { 57635, L"57635", L"剪切", L"48,0,64,16", { 48,0,64,16 }, },
    { 57634, L"57634", L"复制", L"64,0,80,16", { 64,0,80,16 }, },
    { 57637, L"57637", L"粘贴", L"80,0,96,16", { 80,0,96,16 }, },
    { 57644, L"57644", L"重复", L"112,0,128,16", { 112,0,128,16 }, },
    { 57643, L"57643", L"撤销", L"128,0,144,16", { 128,0,144,16 }, },
    { 32934, L"32934", L"整体搜寻", L"144,0,160,16", { 144,0,160,16 }, },
    { 32854, L"32854", L"运行", L"160,0,176,16", { 160,0,176,16 }, },
    { 32860, L"32860", L"停止", L"176,0,192,16", { 176,0,192,16 }, },
    { 32940, L"32940", L"查看", L"192,0,208,16", { 192,0,208,16 }, },
    { 32853, L"32853", L"单步", L"208,0,224,16", { 208,0,224,16 }, },
    { 32852, L"32852", L"单步进入", L"224,0,240,16", { 224,0,240,16 }, },
    { 32867, L"32867", L"跟踪返回", L"240,0,256,16", { 240,0,256,16 }, },
    { 32871, L"32871", L"运行到光标处", L"256,0,272,16", { 256,0,272,16 }, },
    { 32855, L"32855", L"设置断点", L"272,0,288,16", { 272,0,288,16 }, },
    { 32925, L"32925", L"知识库", L"288,0,304,16", { 288,0,304,16 }, },
    { 32850, L"32850", L"预览", L"304,0,320,16", { 304,0,320,16 }, },
    { 111, L"111", L"左对齐", L"320,0,336,16", { 320,0,336,16 }, },
    { 112, L"112", L"右对齐", L"336,0,352,16", { 336,0,352,16 }, },
    { 190, L"190", L"顶对齐", L"352,0,368,16", { 352,0,368,16 }, },
    { 191, L"191", L"底对齐", L"368,0,384,16", { 368,0,384,16 }, },
    { 192, L"192", L"窗口水平居中", L"384,0,400,16", { 384,0,400,16 }, },
    { 196, L"196", L"窗口垂直居中", L"400,0,416,16", { 400,0,416,16 }, },
    { 150, L"150", L"组件水平居中", L"416,0,432,16", { 416,0,432,16 }, },
    { 151, L"151", L"组件垂直居中", L"432,0,448,16", { 432,0,448,16 }, },
    { 193, L"193", L"水平平均分布", L"448,0,464,16", { 448,0,464,16 }, },
    { 194, L"194", L"垂直平均分布", L"464,0,480,16", { 464,0,480,16 }, },
    { 195, L"195", L"等宽", L"480,0,496,16", { 480,0,496,16 }, },
    { 120, L"120", L"等高", L"496,0,512,16", { 496,0,512,16 }, },
    { 121, L"121", L"等宽高", L"512,0,528,16", { 512,0,528,16 }, },

    { 32905, L"32905", L"后退", L"528,0,544,16", { 528,0,544,16 }, },
    { 32905, L"32906", L"前进", L"560,0,576,16", { 560,0,576,16 }, },

};


// 根据id获取绘画图标, 返回绘画需要的位图和图标在位图里的位置
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
        // 先从颜色配置里读取工具条的图片信息
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
            assert(isOk && L"设置工具条xml路径失败");
        }

        // 上面已经读取好了工具条的xml文件名, 如果没有会写入, 调试版失败会断下
        // 下面开始载入读取出来的这个xml文件, 如果加载失败, 则把默认的配置写出
        CTinyXml2W xml;
        wstr path(260);
        path.assign(s_info->dataPath).append(xmlPath);

        if (!xml.LoadFile(path.c_str()))    // 如果加载失败, 需要把默认配置都写出
        {
            xml.Create(L"ToolIcon");
            xml.SetAttrValue(L"@1", L"png", L"tool.png");

            const int len = sizeof(m_tool_names) / sizeof(m_tool_names[0]);
            for (int i = 0; i < len; i++)
            {
                xml.InsertChildNode(L"@1", m_tool_names[i].szid, m_tool_names[i].name, false, &node);
                isOk = xml.InsertAttr(node, L"rc", m_tool_names[i].szrc);
                assert(isOk && L"写入工具条xml默认配置失败");

            }
        }

        // 从xml里读出png文件名
        LPCWSTR pngName = xml.GetAttrValue(L"@1", L"png");
        if (!pngName || !pngName[0])
        {
            pngName = L"tool.png";
            isOk = xml.InsertAttr(L"@1", L"png", pngName);
            assert(isOk && L"写入工具条xml默认图片名失败");
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
        const LPEX_FONT pFont = pData->draw.hFont ? pData->draw.hFont : g_defData.draw.hFont;    // 绘画字体
        const HFONT hFont = pFont->hFont;    // 绘画字体

        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        assert(md);
        HDC hdc = md->hdc;  // 如果md=空, 就让他崩溃
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, pData->draw.color.crText);
        SelectObject(hdc, hFont);
        Gdiplus::Graphics gp(hdc);


        FillRect(hdc, &rc, pData->draw.color.hbrBackGround);

        const int index = SendMessageW(hWnd, TB_GETHOTITEM, 0, 0);      // 当前工具条热点索引
        const int len = SendMessageW(hWnd, TB_BUTTONCOUNT, 0, 0);       // 当前工具条数量

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
            // 项目的右边小于重画区域的左边, 表示项目在重画区域的左边, 没有在绘制范围内
            // 项目的左边大于重画区域的右边, 表示项目在重画区域的右边, 没有在绘制范围内
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

            if ((item.fsStyle & BTNS_SEP) == BTNS_SEP)  // 分隔符
            {
                const int left = rcItem.left + cxItem / 2;
                MoveToEx(hdc, left, rcItem.top + 1, 0);
                LineTo(hdc, left, rcItem.bottom - 1);
                continue;
            }

            auto it = m_map_tool_icon.find(item.idCommand);
            RECT* prcIconDraw = 0;
            TOOL_ICON_DATA* iconData = 0;   // 是否从map里获取图片绘画, 不为0则从这里获取图片绘画
            if (it != m_map_tool_icon.end())
                iconData = &(it->second);
            
            Gdiplus::Bitmap* hBitmapGdip = 0;
            if (i == index)   // 热点项目
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

            if ((item.fsState & TBSTATE_CHECKED) == TBSTATE_CHECKED)    // 选中状态
            {
                FillRect(hdc, &rcItem, pData->draw.color.hbrHot);
                OffsetRect(&rcItem, 1, 1);
            }
            if ((item.fsState & TBSTATE_ENABLED) == TBSTATE_ENABLED)    // 启用状态
            {

            }
            if ((item.fsState & TBSTATE_PRESSED) == TBSTATE_PRESSED)    // 单击该按钮
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

                if ((item.fsState & TBSTATE_ENABLED) == TBSTATE_ENABLED)    // 没有这个状态就是禁止
                {
                    // 是启用状态就直接画

                    gp.DrawImage(hBitmapGdip, rcDest, prcIconDraw->left, prcIconDraw->top,
                        prcIconDraw->right - prcIconDraw->left, prcIconDraw->bottom - prcIconDraw->top, Gdiplus::Unit::UnitPixel);
                    hList = 0;  // 不让后面绘画了
                }
                else
                {
                    // 是禁止状态, 把图片改成禁止的效果在绘画到dc上
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
                    if ((item.fsState & TBSTATE_ENABLED) == TBSTATE_ENABLED)    // 没有这个状态就是禁止
                    {
                        // 是启用状态就直接画
                        DrawIconEx(hdc,
                            rcItem.left + (cxItem - cxIcon) / 2,
                            rcItem.top + (cyItem - cyIcon) / 2,
                            hIcon, cxIcon, cyIcon, 0, 0, DI_NORMAL);
                    }
                    else
                    {
                        // 是禁止状态, 把图片改成禁止的效果在绘画到dc上
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


// 易语言自带标准工具条触发删除按钮时修改自己创建的工具条状态
inline static void __notify_state(HWND hWnd, WPARAM id, DWORD state)
{
    if (state == TBSTATE_ENABLED)
        state &= ~TBSTATE_ENABLED;  // 有启用状态就去除启用状态
    else if (state == 0)
        state |= TBSTATE_ENABLED;   // 没有启用状态就加上启用状态

    if ((state & TBSTATE_CHECKED) == TBSTATE_CHECKED)
        state = ((state & ~TBSTATE_CHECKED) | TBSTATE_ENABLED);   // 有选中状态就加上启用和去除选中

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
    if ((HWND)lParam == s_info->hMenu_Normal)       // 点击的是标准工具条的按钮
    {
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
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

        case TOOL_STANDARD_WORK:        // 备注工具条 -> 工作夹
        case TOOL_STANDARD_STATE:       // 备注工具条 -> 状态夹
        case TOOL_STANDARD_WORK_STATE:  // 备注工具条 -> 工作夹与状态夹组合
            // 这几个改变布局的按钮就不让触发
            return 0;
        case TOOL_STANDARD_BACK:        // 备注工具条 -> 后退
            SendMessageW(s_info->hEWnd, WM_COMMAND, 33003, 0);  // &Z.跳回先前位置\tCtrl+J
            return 0;
        case TOOL_STANDARD_FORWARD:     // 备注工具条 -> 前进
            SendMessageW(s_info->hEWnd, WM_COMMAND, 32885, 0);  // &S.跳转到定义处\tCtrl+B/Ctrl+单击
            return 0;
        default:
            wstr::dbg(L"点击了标准工具条的按钮, id = %d\n", id);
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        }
        return 0;
    }
    else if ((HWND)lParam == s_info->hMenu_Align)   // 点击的是对齐工具条的按钮
    {
        switch (id)
        {
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
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        default:
            break;
        }
        return 0;
    }


    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

}
