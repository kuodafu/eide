// ImageExt.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ImageExt.h"

static std::set<DWORD> s_com1, s_com2;
CButton s_btnSave, s_btnLoad, s_btnUpdate, s_btnModify1, s_btnModify2, s_btnModify3, s_btnReload;
CMyEdit s_editPath, s_editColor;
CCombobox s_editModifyColor1, s_editModifyColor2;
CMyStatic s_static_Color, s_static_path, s_static_ModifyColor1, s_static_ModifyColor2, s_static_back_color;
CButton s_static_draw, s_static_save, s_static_group;
CCustomList s_list;
Gdiplus::Bitmap* s_hImage;
wstr s_loadFile;
wstr s_loadPath;
LPMEMDCBITMAP s_md;
CPrivateProfile s_ini;
HWND g_hWnd;
int s_lastBtnBottom;     // 最后一个按钮的底边
std::vector<LIST_FILEDATA> s_data;
int s_dataCount;     // s_data 实际占用的成员数
int s_dataBufCount;     // s_data 实际占用的成员数

HINSTANCE g_hInst;                              // 当前实例


// 此代码模块中包含的函数的前向声明:
BOOL                InitInstance(int);


BOOL InvalidateRect()
{
    s_static_save.InvalidateRect();
    s_static_draw.InvalidateRect();
    return 0;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    g_hInst = hInstance; // 将实例句柄存储在全局变量中

    _canvas_init(false, false);
    // TODO: 在此处放置代码。

    // 执行应用程序初始化:
    if (!InitInstance(nCmdShow))
    {
        _canvas_uninit();
        return FALSE;
    }

    HACCEL hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCE(IDC_IMAGEEXT));

    MSG msg;

    // 主消息循环:
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    _canvas_uninit();
    return (int)msg.wParam;
}


BOOL InitInstance(int nCmdShow)
{
    WNDCLASSEXW wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = g_hInst;
    wcex.hIcon          = LoadIconW(g_hInst, MAKEINTRESOURCEW(IDI_IMAGEEXT));
    wcex.hCursor        = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = L"主题生成工具";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCEW(IDI_SMALL));

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowExW(0, wcex.lpszClassName, L"主题生成工具", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, g_hInst, nullptr);
    if (!hWnd) return FALSE;
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}


void _draw_test(HDC hdc, int cxClient, int cyClient)
{
    static Gdiplus::Font* hFont = 0;
    if (!hFont)
    {
        HDC hdc = GetDC(0);
        hFont = new Gdiplus::Font(hdc, CWndBase::DefaultFont());
        ReleaseDC(0, hdc);
    }
    Gdiplus::Graphics gp(hdc);
    const int cxImg = s_md->width;
    const int cyImg = s_md->height;
    COLORREF crBack = str2rgb(s_editColor.GetText().c_str(), 0);

    gp.DrawImage(s_hImage, 0, 0, cxImg, cyImg);

    Gdiplus::Pen pen(MAKEARGB(255, 0, 0, 0));               // 右边画个矩形对比一下
    Gdiplus::SolidBrush hbr1(MAKEARGB(100, 255, 255, 255)); // 右边画个矩形对比一下
    Gdiplus::SolidBrush hbr2(MAKEARGB(100, 0, 0, 0));       // 右边画个矩形对比一下

    Gdiplus::SolidBrush hbrText(RGB2ARGB(RGB_REVERSE(crBack), 255));       // 右边画个矩形对比一下
    Gdiplus::Rect rc11(cxImg + 10, 0, cxImg, cyImg);
    gp.DrawImage(s_hImage, cxImg + 10, 0, cxImg, cyImg);
    gp.DrawRectangle(&pen, rc11);

    Gdiplus::Rect rc12(rc11.X + cxImg + 10, 0, cxImg, cyImg);
    gp.FillRectangle(&hbr1, rc12);
    gp.DrawImage(s_hImage, rc12.X, rc12.Y, cxImg, cyImg);

    Gdiplus::Rect rc13(rc12.X + cxImg + 10, 0, cxImg, cyImg);
    gp.FillRectangle(&hbr2, rc13);
    gp.DrawImage(s_hImage, rc13.X, rc13.Y, cxImg, cyImg);


    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = 255;
    AlphaBlend(hdc, 0, cyImg + 10, cxImg, cyImg, s_md->hdc, 0, 0, cxImg, cyImg, blend);

    Gdiplus::Rect rc21(cxImg + 10, cyImg + 10, cxImg, cyImg);
    AlphaBlend(hdc, rc21.X, rc21.Y, cxImg, cyImg, s_md->hdc, 0, 0, cxImg, cyImg, blend);
    gp.DrawRectangle(&pen, rc21);

    Gdiplus::Rect rc22(rc21.X + cxImg + 10, cyImg + 10, cxImg, cyImg);
    gp.FillRectangle(&hbr1, rc22);
    AlphaBlend(hdc, rc22.X, rc22.Y, cxImg, cyImg, s_md->hdc, 0, 0, cxImg, cyImg, blend);

    Gdiplus::Rect rc23(rc22.X + cxImg + 10, cyImg + 10, cxImg, cyImg);
    gp.FillRectangle(&hbr2, rc23);
    AlphaBlend(hdc, rc23.X, rc23.Y, cxImg, cyImg, s_md->hdc, 0, 0, cxImg, cyImg, blend);

    Gdiplus::PointF pt((float)(rc23.X + cxImg + 10), (float)rc23.Y);
    gp.DrawString(L"← 这个是绘画到结果画布的图标", 15, hFont, pt, &hbrText);

}

LRESULT CALLBACK WndProc_static_draw(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rc;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;
        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        HDC hdc = md->hdc;
        COLORREF crBack = str2rgb(s_editColor.GetText().c_str(), 0);

        HBRUSH hbr = CreateSolidBrush(crBack);
        FillRect(hdc, &rc, hbr);
        if (s_md)
        {
            const int offsetDraw = 10;
            LPMEMDCBITMAP tmpmd = md_create(cxClient - offsetDraw, cyClient - offsetDraw);
            RECT tcTmp = { 0, 0, cxClient - offsetDraw, cyClient - offsetDraw };
            FillRect(tmpmd->hdc, &tcTmp, hbr);

            _draw_test(tmpmd->hdc, tcTmp.right, tcTmp.bottom);
            BitBlt(hdc, offsetDraw, offsetDraw, tcTmp.right, tcTmp.bottom, tmpmd->hdc, 0, 0, SRCCOPY);
            md_destroy(tmpmd);
        }
        DeleteObject(hbr);
        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, md->hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK WndProc_static_back_color(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static LPMEMDCBITMAP md;
    static Gdiplus::Bitmap* hImage;
    static DWORD oldCr = -1;

    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    {
        if (!md || (wParam & MK_LBUTTON) == 0) break;
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        COLORREF cr = GetPixel(md->hdc, x, y);
        if (cr == oldCr)break;
        oldCr = cr;
        s_editColor.SetText(wstr::to_string(cr).c_str());
        InvalidateRect();
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rc;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;
        if (!md)
        {
            md = md_create(cxClient, cyClient);
            DWORD size = 0;
            LPBYTE ptr = GetResource(g_hInst, IDB_PNG1, &size, L"PNG");
            hImage = _img_create_from_memory(ptr, size);
            Gdiplus::Graphics gp(md->hdc);
            gp.DrawImage(hImage, 0, 0);
        }

        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, md->hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        _wnd_create(hWnd);
        wstr value;
        for (int i = 0; i < s_editModifyColor1.GetCount(); i++)
        {
            value = s_editModifyColor1.GetItemText(i);
            ARGB cr = str2argb(value.c_str(), MAKEARGB(255, 255, 255, 255));
            s_com1.insert(cr);
        }
        for (int i = 0; i < s_editModifyColor2.GetCount(); i++)
        {
            value = s_editModifyColor2.GetItemText(i);
            ARGB cr = str2argb(value.c_str(), MAKEARGB(255, 255, 255, 255));
            s_com2.insert(cr);
        }
        break;
    }
    case WM_DESTROY:
    {
        s_data.clear();
        s_dataBufCount = 0;
        s_dataCount = 0;
        ini_write(L"s_editPath", s_editPath.GetWindowTextW());
        wstr name(50);
        for (int i = 0; i < s_editModifyColor1.GetCount(); i++)
        {
            name.assign(L"s_editModifyColor1_").append(i);
            ini_write(name.c_str(), s_editModifyColor1.GetItemText(i));
        }
        for (int i = 0; i < s_editModifyColor2.GetCount(); i++)
        {
            name.assign(L"s_editModifyColor2_").append(i);
            ini_write(name.c_str(), s_editModifyColor2.GetItemText(i));
        }
        ini_write(L"s_editModifyColor1_index", s_editModifyColor1.GetSel());
        ini_write(L"s_editModifyColor2_index", s_editModifyColor2.GetSel());

        ini_write(L"s_editColor", s_editColor.GetWindowTextW());
        wstr rect(100);
        RECT rc;
        GetWindowRect(hWnd, &rc);
        rect.Format(L"%d,%d,%d,%d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
        ini_write(L"windowPos", rect);

        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        const int cxClient = LOWORD(lParam);
        const int cyClient = HIWORD(lParam);
        s_list.Move(10, s_lastBtnBottom, 322, cyClient - s_lastBtnBottom - 210);
        s_static_back_color.Move(10, cyClient - 204, 322, 194);

        const int height = 200;
        const int width = cxClient - 360;
        s_static_draw.Move(350, 26, width, height);
        s_static_save.Move(350, 30 + height, width, cyClient - (40 + height + 210));
        s_static_group.Move(350, cyClient - 210, width, 200);
        break;
    }
    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case ID_BTN_LOAD:
        {
            wstr file = Dialog_Open(L"*.*\0*.*", L"选择图片文件");
            if (file.empty())break;
            _load_image(file);
            if (!file.empty())
            {
                wstr path = file;
                wchar_t* pos = wcsrchr(path.data(), '\\');
                if (pos)
                {
                    pos[1] = 0;
                    enum_file(path.c_str());
                }
                
            }
            s_list.InvalidateRect();
            InvalidateRect();
            break;
        }
        case ID_BTN_UPDATE:
            InvalidateRect();
            break;
        case ID_BTN_MODIFY1:
        case ID_BTN_MODIFY2:
        case ID_BTN_MODIFY3:
        {
            const int cxImg = s_md->width;
            const int cyImg = s_md->height;
            
            wstr text1 = s_editModifyColor1.GetWindowTextW();
            wstr text2 = s_editModifyColor2.GetWindowTextW();
            COLORREF crModify1 = str2argb(text1.c_str(), MAKEARGB(255, 255, 255, 255));
            COLORREF crModify2 = str2argb(text2.c_str(), MAKEARGB(255, 255, 255, 255));
            Gdiplus::Graphics gp(s_md->hdc);
            
            if (id == ID_BTN_MODIFY1)
            {
                gp.Clear(0);
                gp.DrawImage(s_hImage, 0, 0, s_hImage->GetWidth(), s_hImage->GetHeight());
                PixelFix(crModify2, crModify1, s_md->lpBits, cxImg * cyImg, RGB_GETR(crModify2), RGB_GETG(crModify2), RGB_GETB(crModify2));
            }
            else if (id == ID_BTN_MODIFY2)
            {
                PixelFix(crModify2, crModify1, s_md->lpBits, cxImg * cyImg);
            }
            else
            {
                PixelFix(crModify2, s_md->lpBits, cxImg * cyImg);
            }
            if (s_com1.find(crModify1) == s_com1.end())
            {
                s_com1.insert(crModify1);
                wstr argbStr = argb2str(crModify1);
                s_editModifyColor1.AddString(argbStr.c_str());
                s_editModifyColor1.SetWindowTextW(argbStr.c_str());
            }
            if (s_com2.find(crModify2) == s_com2.end())
            {
                s_com2.insert(crModify2);
                wstr argbStr = argb2str(crModify2);
                s_editModifyColor2.AddString(argbStr.c_str());
                s_editModifyColor2.SetWindowTextW(argbStr.c_str());
            }

            InvalidateRect();
            break;
        }
        case ID_BTN_RELOAD:
        {
            if (!s_md)break;

            Gdiplus::Graphics gp(s_md->hdc);
            gp.Clear(0);
            gp.DrawImage(s_hImage, 0, 0, s_hImage->GetWidth(), s_hImage->GetHeight());
            InvalidateRect();
            break;
        }
        case ID_BTN_SAVE:
        {
            if (!s_md)
            {
                MessageBoxW(hWnd, L"没有选择图片", L"错误", 0);
                break;
            }
            const wchar_t* pos = 0;
            if (!s_loadFile.empty())
                pos = wcsrchr(s_loadFile.c_str(), '\\');
            if (pos)pos++;
            wstr path = Dialog_Save(L"*.png\0*.png", L"图片文件", pos);
            if (path.empty())break;

            IStream* pStream;
            HRESULT hr = CreateStreamOnHGlobal(0, 0, &pStream);
            if (FAILED(hr))break;
            
            int size = 0;
            LPBYTE ptr = GetPngDataFromBitmap(s_md->hBitmap, &size);
            CFileRW f;
            f.open(path.c_str(), 4, -1);
            f.write(ptr, size);
            delete[] ptr;

            break;
        }
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rc;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;
        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        HDC hdc = md->hdc;

        FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);
        break;
    }
    case WM_CTLCOLORSTATIC:
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)GetStockBrush(WHITE_BRUSH);
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}


inline HBRUSH __get_brush(STATE state, bool isFocus)
{
    static HBRUSH hbrBack = CreateSolidBrush(RGB(255, 255, 255));
    static HBRUSH hbrHot = CreateSolidBrush(RGB(229, 243, 255));     // 热点画刷
    static HBRUSH hbrSel = CreateSolidBrush(RGB(205, 232, 255));     // 选中画刷
    static HBRUSH hbrDown = CreateSolidBrush(RGB(220, 233, 255));     // 按下画刷
    static HBRUSH hbrKillFocus = CreateSolidBrush(RGB(217, 217, 217)); // 失去焦点

    HBRUSH hbr = 0;
    switch (state)
    {
    case DOWN:      hbr = hbrDown;   break;
    case SELECTED:  hbr = hbrSel;    break;
    case HOT:       hbr = hbrHot;    break;
    default:
        break;
    }

    if (hbr)
    {
        if (!isFocus && hbr == hbrSel)
            hbr = hbrKillFocus;
    }
    return hbr;
}

bool CALLBACK CustomListBox_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus)
{

    HBRUSH hbr = __get_brush(state, isFocus);
    if (hbr)  FillRect(hdc, rcItem, hbr);

    RECT rc = *rcItem;

    LIST_FILEDATA& data = s_data[index];
    rc.left = 5;
    rc.right = rc.left + 20;
    wstr str(260);
    str.assign(index + 1);
    DrawTextW(hdc, str.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE);

    const int ItemHeight = rc.bottom - rc.top;
    rc.left = rc.right;
    rc.right = rc.left + ItemHeight;
    int cyImg = data.cyImg;
    int cxImg = data.cxImg;
    if (cyImg > ItemHeight) cyImg = ItemHeight;
    if (cxImg > ItemHeight) cxImg = ItemHeight;

    int left = rc.left + (ItemHeight - cxImg) / 2;
    int top = rc.top + (ItemHeight - cyImg) / 2;

    Gdiplus::Graphics gp(hdc);
    gp.DrawImage(data.img, left, top, cxImg, cyImg);

    rc.left = rc.right + 5;
    rc.right = rcItem->right;
    DrawTextW(hdc, data.name, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    return true;
}

void CALLBACK CustomList_ItemSelect(HWND hWnd, int index)
{
    LIST_FILEDATA& data = s_data[index];
    _load_image(data.file.c_str());
    s_static_draw.InvalidateRect();
}

void CALLBACK CustomList_ItemRClick(HWND hWnd, int index)
{

}