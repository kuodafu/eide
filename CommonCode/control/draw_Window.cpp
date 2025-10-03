#include "draw_Window.h"
#include <unordered_map>

#define FREE_CANVAS             0x0001  // ��Ҫ�ͷŻ���
#define FREE_BRUSH              0x0002  // ��Ҫ�ͷű����ı�
#define FREE_BRUSH_NF           0x0004  // ��Ҫ�ͷű����ı�, δ����
#define FREE_FONT               0x0008  // ��Ҫ�ͷ�����
#define FREE_UNSUBCLASS         0x0010  // ��Ҫ��ԭ���໯
static std::unordered_map<HWND, DRAW_WINDOW_DATA> m_map; // ϵͳ��ť����
static std::unordered_map<HICON, LPEX_IMAGE> m_map_icon; // ͼ������, һ��ͼ������Ӧһ��dxͼ��
static std::unordered_map<HWND, CDrawWindow> m_map_obj;  // ���ڶ�Ӧ����ָ��, һ�����ڶ�Ӧһ����ָ��
void draw_ewnd_createed(DRAW_WINDOW_DATA* data);
LPEX_IMAGE _get_def_image();


bool draw_WndProc_OnStyleChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool draw_WndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool draw_WndProc_OnWindowPosChangIng(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool draw_WndProc_OnWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool draw_WndProc_OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool draw_WndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);

bool draw_WndProc_OnNcHittest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
// ���ͻ�����Ϣ, �ƶ�,���Ҽ�����, �ſ�, ˫��, ����뿪����Ҫ��������ִ��, �����Ƿ���, 
bool draw_WndProc_OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
// ����ϵͳ��ť, ������, ͼ��λ��
void draw_Change_SysBtn_Icon_Caption_Pos(DRAW_WINDOW_DATA* data);

bool _draw_Mouse_OnLDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool _draw_Mouse_OnLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool _draw_Mouse_OnRDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool _draw_Mouse_OnRUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool _draw_Mouse_OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool _draw_SysBtnClick(DRAW_WINDOW_DATA* data, bool isRButton);
bool draw_OnPaint_DrawData(DRAW_WINDOW_DATA* data, const PAINTSTRUCT& ps, LPCANVAS hCanvas);
LRESULT draw_WndProc_OnNcHittest_return(HWND hWnd, LPARAM lParam, DRAW_WINDOW_DATA* data);
bool draw_WindowProc_data(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet);
bool WINAPI draw_OnShadowPaint(LPCANVAS hCanvas, LPEX_IMAGE pImg, const RECT* prc, COLORREF crShadow, DWORD argbBorder, int alpha, LPVOID param);
void _draw_window_activate_changed(bool isActivate, LPARAM lParam, LPDRAW_WINDOW_DATA data);
//bool draw_SetColor(LPDRAW_WINDOW_DATA data, const LPDRAW_WINDOW_COLOR pcr, int mask);
//bool draw_GetColor(LPDRAW_WINDOW_DATA data, LPDRAW_WINDOW_COLOR pcr);
// �޸�ϵͳ��ť������λ��
void draw_ModifySysButtonPos(DRAW_WINDOW_DATA* data);

// �滭ϵͳ��ť
bool draw_OnPaint(HWND hWnd, const PAINTSTRUCT& ps, LPCANVAS hCanvas);






inline void _draw_UpdateSysbtn(DRAW_WINDOW_DATA* data)
{
    InvalidateRect(data->hWnd, &data->rcBtn, 0);
}
// ��һ��ֵ��������
inline int _scale(int n)
{
    //if (!s_isDPIAware || s_info->dpiX == 0)
    //    return n;
    //float scale = (float)s_info->dpiX / 96.f;
    //return (int)((float)n * scale);
    return n;
}
inline LRESULT CALLBACK pfn_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DRAW_WINDOW_DATA& data = m_map[hWnd];
    if (!data.fnCallProc)
        return DefWindowProcW(hWnd, message, wParam, lParam);
    LRESULT ret = 0;
    if (draw_WindowProc_data(hWnd, message, wParam, lParam, &data, &ret))
        return ret;
    return data.fnCallProc(data.oldProc, hWnd, message, wParam, lParam);
};
inline DRAW_WINDOW_DATA* draw_GetSysButtonData(HWND hWnd)
{
    auto it = m_map.find(hWnd);
    if (it == m_map.end())
        return 0;
    return &it->second;
}
inline bool Rect_isOverlap(const RECT& rc1, const RECT& rc2)
{
    return (rc1.right > rc2.left && rc2.right > rc1.left
        && rc1.bottom > rc2.top && rc2.bottom > rc1.top);
}
template<typename T, typename R>bool __query(T s, R n)
{
    return (s & n) == n;
}
LPDRAW_WINDOW_DATA CDrawWindow::BindWindow_s(const DRAW_WINDOW_ARGUMENT* arg)
{
    HWND hWnd = arg->hWnd;
    DRAW_WINDOW_DATA& data = m_map[hWnd];
    memcpy(&data, arg, sizeof(DRAW_WINDOW_ARGUMENT));

    if (!data.isUnicode)
        data.isUnicode = IsWindowUnicode(hWnd);
    if (data.isUnicode)
    {
        data.fnCallProc = CallWindowProcW;
        data.fnDefProc  = DefWindowProcW;
        data.SetWndLong = (pfn_SetLong)SetWindowLongPtrW;
        data.GetWndLong = (pfn_GetLong)GetWindowLongPtrW;
        data.SetClsLong = (pfn_SetLong)SetClassLongPtrW;
        data.GetClsLong = (pfn_GetLong)GetClassLongPtrW;
    }
    else
    {
        data.fnCallProc = CallWindowProcA;
        data.fnDefProc  = DefWindowProcA;
        data.SetWndLong = (pfn_SetLong)SetWindowLongPtrA;
        data.GetWndLong = (pfn_GetLong)GetWindowLongPtrA;
        data.SetClsLong = (pfn_SetLong)SetClassLongPtrA;
        data.GetClsLong = (pfn_GetLong)GetClassLongPtrA;
    }

    data.style          = data.GetWndLong(data.hWnd, GWL_STYLE);
    data.styleEx        = data.GetWndLong(data.hWnd, GWL_EXSTYLE);
    data.style          &= ~WS_BORDER;
    if (!data.hFont)
    {
        data.hFont      = _font_create();
        data.freeFlags  |= FREE_FONT;
    }
    if (!data.hbrTextTitle)
    {
        data.hbrTextTitle = _brush_create(data.crTitle);
        data.freeFlags  |= FREE_BRUSH;
    }
    if (!data.hbrTextTitle_nf)
    {
        data.hbrTextTitle_nf = _brush_create(data.crTitle_nf);
        data.freeFlags |= FREE_BRUSH_NF;
    }

    data.isThickFrame = __query(data.style, WS_THICKFRAME);
    data.isActivation = true;
    data.showImage = true;

    GetWindowRect(hWnd, &data.rcWindow);
    data.rcClient = { 0, 0, data.rcWindow.right - data.rcWindow.left, data.rcWindow.bottom - data.rcWindow.top };
    if (data.crShadow || data.crShadow_nf || data.crBorder || data.crBorder_nf)
    {
        // �߿������Ӱ, ֻҪ��һ����Ҫ�滭, �Ǿʹ�����Ӱ����
        if (!data.shadow)
        {
            data.shadow = new CShadow;
            data.shadow->OnPaint(draw_OnShadowPaint, &data);
            data.shadow->create(GetModuleHandleW(0), data.rcWindow, data.hWnd, 0);
        }
        _img_destroy(data.hImageShdow);
        _img_destroy(data.hImageShdow_nf);
        data.hImageShdow = CShadow::GetShadowImage(data.crShadow);
        data.hImageShdow_nf = CShadow::GetShadowImage(data.crShadow_nf);

        data.shadow->SetColor(0, data.crBorder, false);
    }
    WNDPROC oldProc = (WNDPROC)data.GetWndLong(hWnd, GWLP_WNDPROC);
    if (arg->isSubClass && data.oldProc == 0 && oldProc != pfn_proc)
    {
        data.oldProc = (WNDPROC)data.SetWndLong(hWnd, GWLP_WNDPROC, (LONG_PTR)pfn_proc);
        data.freeFlags |= FREE_UNSUBCLASS;
    }
    if (data.count == 0)
        draw_ewnd_createed(&data);
    data.SetWndLong(data.hWnd, GWL_STYLE, data.style);

    //HRGN hRgn = CreateRectRgn(0, 0, data.rcWindow.right - data.rcWindow.left, data.rcWindow.bottom - data.rcWindow.top);
    //SetWindowRgn(hWnd, hRgn, false);
    //DeleteObject(hRgn);
    SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
    return &data;
}

LPDRAW_WINDOW_DATA CDrawWindow::GetObjFromHwnd(HWND hWnd)
{
    auto it = m_map.find(hWnd);
    if (it == m_map.end())
        return 0;
    return &it->second;
}

HWND CDrawWindow::create_s(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, const DRAW_WINDOW_ARGUMENT* arg)
{
    static WNDCLASSEXW wcex;
    if (wcex.lpszClassName == 0)
    {
        wcex.cbSize         = sizeof(WNDCLASSEX);
        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = pfn_proc;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = GetModuleHandleW(0);
        wcex.hIcon          = LoadIconW(0, IDI_APPLICATION);
        wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName   = 0;
        wcex.lpszClassName  = L"draw_window_classname";
        wcex.hIconSm        = wcex.hIcon;
        RegisterClassExW(&wcex);
    }
    HWND hWnd = CreateWindowExW(dwExStyle, wcex.lpszClassName, lpWindowName, dwStyle, x, y, cx, cy, hWndParent, 0, wcex.hInstance, 0);
    if (hWnd)
    {
        DRAW_WINDOW_ARGUMENT def;
        memset(&def, 0, sizeof(def));
        if (arg)
            def = *arg;
        else
            def = GetDefArg(hWnd);
        
        def.hWnd = hWnd;
        def.isSubClass = false;
        LPDRAW_WINDOW_DATA data = (LPDRAW_WINDOW_DATA)CDrawWindow::BindWindow_s(&def);
        data->oldProc = IsWindowUnicode(hWnd) ? DefWindowProcW : DefWindowProcA;
    }
    return hWnd;
}

inline DWORD _get_btn_data(LPDRAW_WINDOW_DATA data, int nPosition, int flags)
{
    DWORD index = nPosition;
    if (!__query(flags, DRAW_WND_BYPOSITION))   // ���ǰ�λ��, �Ǿ���Ҫ��ȡ��λ��
    {
        for (DWORD i = 0; i < data->count; i++)
        {
            if (data->btn[i].ID == nPosition)
                return i;
        }
    }
    if (index < 0 || index >= data->count)
        return -1;
    return index;
}

int CDrawWindow::AddSysButton(int id, LPARAM param)
{
    if (m_data)
        return AddSysButton(m_data->hWnd, id, param);
    return 0;
}

int CDrawWindow::AddSysButton(LPDRAW_WINDOW_DATA data, int id, LPARAM param)
{
    if (!data) return 0;

    for (DWORD i = 0; i < data->count; i++)
    {
        if (id == data->btn[i].ID)
        {
            data->btn[i].param = param;
            return data->count;
        }
    }

    const int btnBufSize = sizeof(data->btn) / sizeof(data->btn[0]);
    if (data->count + 1 > btnBufSize)
        return 0;   // ��ť����̫����, ������

    DRAW_WINDOW_DATA& sysBtn = *data;
    SYSBUTTON_INFO& btn = sysBtn.btn[sysBtn.count];
    const RECT& rcClient = data->rcClient;    // ���ڿͻ�������
    // ��ť����������
    if (sysBtn.count == 0)
    {
        btn.rc.right    = rcClient.right;   // ��һ����ť�Ǵ��ڵ��ұ�
        btn.rc.left     = btn.rc.right - data->sysbtnWidth;
        btn.rc.top      = 0;
        btn.rc.bottom   = data->sysbtnHeight;
    }
    else
    {
        const SYSBUTTON_INFO& prve = sysBtn.btn[sysBtn.count - 1];   // ��һ����ť������

        btn.rc.right    = prve.rc.left;     // ��ť���ұ�����һ����ť�����
        btn.rc.left     = btn.rc.right - data->sysbtnWidth; // ��ť��� = ��ť�ұ� - ��ť���
        btn.rc.top      = prve.rc.top;      // ����/�ױߺ���һ����ť��ͬ
        btn.rc.bottom   = prve.rc.bottom;
    }
    ++sysBtn.count; // ��ť����+1
    btn.ID = id;
    btn.param = param;
    return sysBtn.count;
}

int CDrawWindow::AddSysButton(HWND hWnd, int id, LPARAM param)
{
    LPDRAW_WINDOW_DATA data = draw_GetSysButtonData(hWnd);
    int ret = AddSysButton(data, id, param);
    draw_Change_SysBtn_Icon_Caption_Pos(data);
    return ret;
}

int CDrawWindow::InsSysButton(int nPosition, int id, LPARAM param, int flags)
{
    return CDrawWindow::InsSysButton(m_data, nPosition, id, param, flags);
}

int CDrawWindow::InsSysButton(LPDRAW_WINDOW_DATA data, int nPosition, int id, LPARAM param, int flags)
{
    if (!data) return 0;

    for (DWORD i = 0; i < data->count; i++)
    {
        if (id == data->btn[i].ID)
        {
            data->btn[i].param = param;
            return 1;
        }
    }

    DWORD index = _get_btn_data(data, nPosition, flags);
    if (index < 0 || index > data->count)
        return AddSysButton(data, id, param) != 0;    // С��0���ߴ��ڰ�ť����, �����뵽���һ��λ��
    const int btnBufSize = sizeof(data->btn) / sizeof(data->btn[0]);
    if (data->count + 1 > btnBufSize)
        return 0;   // ��ť����̫����, ������
    
    const int size = (data->count - index) * sizeof(SYSBUTTON_INFO);       // (��Ա�ܳ��� - ����λ��) * ��Ա�ߴ�
    SYSBUTTON_INFO* offset = &data->btn[index];     // �������Ҫ�����λ��, �������Ŀ���󿽱�, ����Ŀ������䵽����
    SYSBUTTON_INFO* pNext = &data->btn[index + 1];  // ����λ�õ���һ����Ŀ

    memcpy(pNext, offset, size);
    offset->ID = id;
    offset->param = param;
    data->count++;

    return 1;
}
int CDrawWindow::InsSysButton(HWND hWnd, int nPosition, int id, LPARAM param, int flags)
{
    LPDRAW_WINDOW_DATA data = draw_GetSysButtonData(hWnd);
    int ret = InsSysButton(data, nPosition, id, param, flags);
    draw_ModifySysButtonPos(data);
    return ret;
}


int CDrawWindow::DelSysButton(int nPosition, int flags)
{
    return DelSysButton(m_data, nPosition, flags);
}

int CDrawWindow::DelSysButton(HWND hWnd, int nPosition, int flags)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    int ret = DelSysButton(data, nPosition, flags);
    draw_ModifySysButtonPos(data);
    return ret;
}

int CDrawWindow::DelSysButton(LPDRAW_WINDOW_DATA data, int nPosition, int flags)
{
    if (!data) return 0;

    DWORD index = _get_btn_data(data, nPosition, flags);
    if (index < 0 || index >= data->count)
        return 0;

    if (index != data->count - 1)
    {
        // ��ɾ���Ĳ������һ����Ա, ��Ҫ�ѱ�ɾ���ĳ�Ա��������ݿ�����ǰ��
        int size = (data->count - index - 1);   // ��ɾ��1����Ա����ʣ��ĳ�Ա��
        size *= sizeof(SYSBUTTON_INFO);         // ��Ҫ�������ֽ���, �����Ǳ�ɾ����Ա��������ݳߴ�

        SYSBUTTON_INFO* offset = &data->btn[index];     // ��ɾ���ĳ�Ա��ַ
        SYSBUTTON_INFO* pNext = &data->btn[index + 1];  // ��ɾ����Ա����һ����Ա��ַ, ����������ݿ�����ǰ��
        memcpy(offset, pNext, size);
    }


    // ɾ�����һ����Ա, ���, ����-1
    memset(&data->btn[--data->count], 0, sizeof(SYSBUTTON_INFO));
    return 1;
}

LPARAM CDrawWindow::SetButtonParam(int nPosition, LPARAM param, int flags)
{
    return SetButtonParam(m_data, nPosition, param, flags);
}

LPARAM CDrawWindow::SetButtonParam(HWND hWnd, int nPosition, LPARAM param, int flags)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    return SetButtonParam(data, nPosition, param, flags);
}

LPARAM CDrawWindow::SetButtonParam(LPDRAW_WINDOW_DATA data, int nPosition, LPARAM param, int flags)
{
    if (!data) return 0;
    DWORD index = _get_btn_data(data, nPosition, flags);
    if (index < 0 || index >= data->count)
        return 0;


    LPARAM old = data->btn[index].param;
    data->btn[index].param = param;
    return old;
}

LPARAM CDrawWindow::GetButtonParam(int nPosition, int flags)
{
    return GetButtonParam(m_data, nPosition, flags);
}

LPARAM CDrawWindow::GetButtonParam(HWND hWnd, int nPosition, int flags)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    return GetButtonParam(data, nPosition, flags);
}

LPARAM CDrawWindow::GetButtonParam(LPDRAW_WINDOW_DATA data, int nPosition, int flags)
{
    if (!data) return 0;
    DWORD index = _get_btn_data(data, nPosition, flags);
    if (index < 0 || index >= data->count)
        return 0;
    return data->btn[index].param;
}

int CDrawWindow::GetButtonIdFromPos(int nPosition)
{
    return GetButtonIdFromPos(m_data, nPosition);
}

int CDrawWindow::GetButtonIdFromPos(HWND hWnd, int nPosition)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    return GetButtonIdFromPos(data, nPosition);
}

int CDrawWindow::GetButtonIdFromPos(LPDRAW_WINDOW_DATA data, int nPosition)
{
    if (!data) return -1;
    DWORD index = _get_btn_data(data, nPosition, DRAW_WND_BYPOSITION);
    if (index < 0 || index >= data->count)
        return -1;
    return data->btn[index].ID;
}

int CDrawWindow::GetButtonPosFromId(int id)
{
    return GetButtonPosFromId(m_data, id);
}

int CDrawWindow::GetButtonPosFromId(HWND hWnd, int id)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    return (int)GetButtonPosFromId(data, id);
}

int CDrawWindow::GetButtonPosFromId(LPDRAW_WINDOW_DATA data, int id)
{
    if (!data) return -1;
    return (int)_get_btn_data(data, id, DRAW_WND_BYCOMMAND);
    return LPARAM();
}

// ����ϵͳ��ť, ������, ͼ��λ��
void draw_Change_SysBtn_Icon_Caption_Pos(DRAW_WINDOW_DATA* data)
{
    if (!data)
        return;
    DRAW_WINDOW_DATA* sysBtn = data;
    const RECT& rcClient    = sysBtn->rcClient;  // ���ڿͻ�������
    const int width         = _scale(sysBtn->sysbtnWidth);
    const int height        = _scale(sysBtn->sysbtnHeight);
    const int allWidth      = width * sysBtn->count;    // ����ϵͳ��ť���ܿ��

    sysBtn->rcIcon.left             = 0;
    sysBtn->rcIcon.top              = 0;
    sysBtn->rcIcon.right            = width;
    sysBtn->rcIcon.bottom           = height;
    if (!data->isDrawIcon)
    {
        sysBtn->rcIcon.right        = 0;    // ���滭ͼ��, ͼ��ռ�õ�λ��Ϊ0
        sysBtn->rcIcon.bottom       = 0;
    }

    sysBtn->rcBtn.left              = rcClient.right - allWidth;    // ϵͳ��ťռ��λ�õ���� = �����ұ� - ��ť���� * ��ť���
    sysBtn->rcBtn.top               = 0;                            // ϵͳ��ť���߹̶���0
    sysBtn->rcBtn.right             = rcClient.right;               // �ұ��Ǵ����ұ�
    sysBtn->rcBtn.bottom            = height;                       // �ױ߹̶� �ǰ�ť�߶�

    sysBtn->rcTitle.left            = sysBtn->rcIcon.right;         // ����λ����� = ͼ�갴ť���ұ�
    sysBtn->rcTitle.right           = sysBtn->rcBtn.left;           // ����λ���ұ� = ϵͳ��ť���
    sysBtn->rcTitle.top             = sysBtn->rcBtn.top;            // ���ߺ͵ױ߸�ϵͳ��ťһ��
    sysBtn->rcTitle.bottom          = sysBtn->rcBtn.bottom;         // 

    sysBtn->rcCaption = { sysBtn->rcIcon.left, sysBtn->rcIcon.top, sysBtn->rcBtn.right, sysBtn->rcBtn.bottom };
    InvalidateRect(data->hWnd, &data->rcCaption, 0);

}
// �޸�ϵͳ��ť������λ��
void draw_ModifySysButtonPos(DRAW_WINDOW_DATA* data)
{
    DRAW_WINDOW_DATA* sysBtn = data;
    const RECT& rcClient    = sysBtn->rcClient;  // ���ڿͻ�������
    const int width         = _scale(sysBtn->sysbtnWidth);
    const int height        = _scale(sysBtn->sysbtnHeight);
    for (DWORD i = 0; i < sysBtn->count; i++)
    {
        SYSBUTTON_INFO& btn = sysBtn->btn[i];
        btn.rc.right    = rcClient.right - i * width;   // �ұ�ÿ�ζ��Ǵ����ұ� - ���� * ��ť���
        btn.rc.left     = btn.rc.right - width;         // ��� = �ұ� - ���
        btn.rc.top      = 0;                            // ���߹̶�0
        btn.rc.bottom   = btn.rc.top + height;          // �ױ߹̶�����+�߶�
    }

    draw_Change_SysBtn_Icon_Caption_Pos(data);
}

void draw_ewnd_createed(DRAW_WINDOW_DATA* data)
{
    DRAW_WINDOW_DATA& sysBtn = *data;

    if (data->isDrawDefBtn)
    {
        if (__query(data->style, WS_SYSMENU))
            CDrawWindow::AddSysButton(data, SC_CLOSE, 0);
        if (__query(data->style, WS_MAXIMIZEBOX))       // ����󻯰�ť
            CDrawWindow::AddSysButton(data, SC_MAXIMIZE, 0);
        if (__query(data->style, WS_MINIMIZEBOX))       // ����С����ť
            CDrawWindow::AddSysButton(data, SC_MINIMIZE, 0);
        if (__query(data->styleEx, WS_EX_CONTEXTHELP))  // �а�����ť
            CDrawWindow::AddSysButton(data, SC_HELP, 0);
    }
    draw_Change_SysBtn_Icon_Caption_Pos(data);
}


bool CDrawWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    if (!data)
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    if (data->oldProc != oldProc)
        data->oldProc = oldProc;
    return draw_WindowProc_data(hWnd, message, wParam, lParam, data, pRet);
}

bool draw_WindowProc_data(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    DRAW_WINDOW_DATA& sysBtn = *data;
    switch (message)
    {
    case WM_ERASEBKGND:
    case WM_NCACTIVATE:
        *pRet = 1;
        return true;
    case WM_STYLECHANGED:
        return draw_WndProc_OnStyleChanged(hWnd, message, wParam, lParam, &sysBtn, pRet);

    case WM_SYSCOMMAND:
        return draw_WndProc_OnNcCommand(hWnd, message, wParam, lParam, &sysBtn, pRet);
    case WM_WINDOWPOSCHANGING:
        return draw_WndProc_OnWindowPosChangIng(hWnd, message, wParam, lParam, &sysBtn, pRet);
    case WM_WINDOWPOSCHANGED:
        return draw_WndProc_OnWindowPosChanged(hWnd, message, wParam, lParam, &sysBtn, pRet);
    case WM_GETMINMAXINFO:
        return draw_WndProc_OnGetMinMaxInfo(hWnd, message, wParam, lParam, &sysBtn, pRet);
    case WM_NCMOUSEMOVE:
    {
        if (sysBtn.index != -1)
        {
            sysBtn.index = -1;
            sysBtn.state = STATE::NORMAL;
            InvalidateRect(sysBtn.hWnd, &sysBtn.rcBtn, 0);
        }
        break;
    }
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        if (PtInRect(&sysBtn.rcBtn, pt))  // �������ϵͳ��ť��, �Լ�����
        {
            return draw_WndProc_OnMouse(hWnd, message, wParam, lParam, &sysBtn, pRet);
        }
        return false;
    }
    case WM_NCCALCSIZE:
    {
        if (wParam)
        {
            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            *pRet = WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;
            return true;
        }
        return 0;
    }
    case WM_NCPAINT:
        //InvalidateRect(hWnd, 0, 0);
        return 1;
    case WM_PAINT:
        return draw_WndProc_OnPaint(hWnd, message, wParam, lParam, &sysBtn, pRet);
    case WM_NCHITTEST:
        return draw_WndProc_OnNcHittest(hWnd, message, wParam, lParam, &sysBtn, pRet);
    case WM_SIZE:
        data->isZoomed = (wParam == SIZE_MAXIMIZED || wParam == SIZE_MAXSHOW);
        if (data->isZoomed) data->shadow->SetAlpha(0);
        else data->shadow->SetAlpha(255);
        break;
    case WM_DESTROY:
    {
        if (__query(data->freeFlags, FREE_UNSUBCLASS))
            data->SetWndLong(hWnd, GWLP_WNDPROC, (LONG_PTR)data->oldProc);

        *pRet = data->fnCallProc(data->oldProc, hWnd, message, wParam, lParam);
        if (__query(data->freeFlags, FREE_CANVAS) && data->hCanvas)
            data->hCanvas->Release();
        data->hCanvas = 0;

        if (data->shadow)
            delete data->shadow;
        if (__query(data->freeFlags, FREE_BRUSH))
            _brush_destroy(data->hbrTextTitle);
        if (__query(data->freeFlags, FREE_BRUSH_NF))
            _brush_destroy(data->hbrTextTitle_nf);
        if (__query(data->freeFlags, FREE_FONT))
            _font_destroy(data->hFont);
        _img_destroy(data->hImageShdow);
        _img_destroy(data->hImageShdow_nf);
        m_map.erase(hWnd);
        return true;
    }
    case WM_ACTIVATEAPP:
    {
        // lParam = �̱߳�ʶ��, ���wParam����ΪTRUE, ��lParam��ӵ�б�ͣ�ô��ڵ��̵߳ı�ʶ��, ���wParamΪFALSE, ��lParam��ӵ�����ڼ���Ĵ��ڵ��̵߳ı�ʶ��
        //static DWORD tid = 0;
        //if (!tid) tid =  GetCurrentThreadId();
        //if (tid == lParam)
        _draw_window_activate_changed(wParam != 0, lParam, data);
        return 0;
    }
    case WM_SETTEXT:
        InvalidateRect(hWnd, &data->rcTitle, 0);;
        return false;
    default:
        return false;
    }

    return false;
}
bool draw_OnPaint_DrawData(DRAW_WINDOW_DATA* data, const PAINTSTRUCT& ps, LPCANVAS hCanvas)
{
    DRAW_WINDOW_DATA& sysBtn = *data;
    bool ret = false;

    if (Rect_isOverlap(ps.rcPaint, sysBtn.rcIcon))
        ret = CDrawWindow::DrawSysIcon(data, ps, hCanvas, sysBtn.rcIcon) || ret;    // ϵͳͼ����Ҫ�滭

    if (Rect_isOverlap(ps.rcPaint, sysBtn.rcTitle))
    {
        RECT rcTitle = sysBtn.rcTitle;
        if (!data->isDrawIcon)
            rcTitle.left += 6;  // û�л滭ͼ��Ļ�, ������Ҫ����ƫ��һ��
        ret = CDrawWindow::DrawTitle(data, ps, hCanvas, rcTitle) || ret;  // ��������Ҫ�滭
    }

    if (Rect_isOverlap(ps.rcPaint, sysBtn.rcBtn))
        ret = CDrawWindow::DrawSysButton(data, ps, hCanvas, sysBtn.rcBtn) || ret;   // ϵͳ��ť��Ҫ�滭

    return ret;
}
bool CDrawWindow::DrawTitle(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcTitle)
{
    LPEX_BRUSH hbrText = data->showImage ? data->hbrTextTitle : data->hbrTextTitle_nf;

    bool ret = false;
    DRAW_PAINTSTRUCT argPs = { 0 };
    argPs.hCanvas   = hCanvas;
    argPs.rc        = &rcTitle;
    argPs.stage     = DRAW_STAGE_TITLE_BEGIN;
    argPs.hbrText   = hbrText;
    int len = GetWindowTextLengthW(data->hWnd) + 1;
    if (len == 1)
    {
        argPs.lpszTitle = L"";
        if (data->pfnDraw)
        {
            ret = data->pfnDraw.pfn(&argPs);
            argPs.stage = DRAW_STAGE_TITLE_END;
            data->pfnDraw.pfn(&argPs);
        }
        return false;
    }
    const int fmt = DT_VCENTER | DT_SINGLELINE;

    wstr text(len);
    len = GetWindowTextW(data->hWnd, text.data(), len);

    if (data->pfnDraw)
        ret = data->pfnDraw.pfn(&argPs);

    if (!ret)
    {
        hCanvas->drawtext(data->hFont, hbrText, text.c_str(), len, fmt, &rcTitle);

        argPs.stage = DRAW_STAGE_TITLE_END;
        if (data->pfnDraw)
            data->pfnDraw.pfn(&argPs);
    }
    return ret;
}

bool CDrawWindow::DrawSysIcon(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcIcon)
{
    if (!data->isDrawIcon)
        return false;

    //HICON hIcon = 0;
    //const int flags = SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG | SMTO_ERRORONEXIT;
    //LRESULT r = SendMessageTimeoutW(data->hWnd, WM_GETICON, ICON_SMALL2, 0, flags, 1000, (DWORD_PTR*)&hIcon);
    //if (r == 0 || hIcon == 0)
    //    r = SendMessageTimeoutW(data->hWnd, WM_GETICON, ICON_BIG, 0, flags, 1000, (DWORD_PTR*)&hIcon);

    HICON hIcon = (HICON)SendMessageA(data->hWnd, WM_GETICON, ICON_SMALL2, 0);
    //if (!hIcon)
    //    hIcon = (HICON)SendMessageA(data->hWnd, WM_GETICON, ICON_BIG, 0);
    //if (!hIcon)
    //    hIcon = (HICON)SendMessageA(GetAncestor(data->hWnd, GA_ROOT), WM_GETICON, ICON_SMALL2, 0);
    //if (!hIcon)
    //    hIcon = (HICON)SendMessageA(GetAncestor(data->hWnd, GA_ROOTOWNER), WM_GETICON, ICON_SMALL2, 0);
    if (!hIcon)
        hIcon = LoadIconW(0, IDI_APPLICATION);

    LPEX_IMAGE& hImage = m_map_icon[hIcon];
    const int cxIcon = 16;
    const int cyIcon = 16;
    if (!hImage)
    {
        LPMEMDCBITMAP md = md_create(cxIcon, cyIcon);
        Gdiplus::Graphics gp(md->hdc);
        Gdiplus::Bitmap bmp(hIcon);
        gp.DrawImage(&bmp, 0, 0, cxIcon, cyIcon);

    
        int size = 0;
        LPBYTE pngData = GetPngDataFromBitmap(md->hBitmap, &size, md->hdc);
        hImage = _img_create_frommemory(pngData, size);
        delete[] pngData;
        md_destroy(md);
    }

    bool ret = false;
    DRAW_PAINTSTRUCT argPs = { 0 };
    argPs.hCanvas   = hCanvas;
    argPs.hIcon     = hImage;
    argPs.rc        = &rcIcon;
    argPs.stage     = DRAW_STAGE_ICON_BEGIN;
    if (data->pfnDraw)
        ret = data->pfnDraw.pfn(&argPs);
    
    if (!ret)
    {
        if(hImage)
        {
            const float left    = rcIcon.left + (rcIcon.width()  - (float)cxIcon) / 2;
            const float top     = rcIcon.top  + (rcIcon.height() - (float)cyIcon) / 2;
            const float right   = left + (float)cxIcon;
            const float bottom  = top  + (float)cyIcon;
            hCanvas->DrawImage(hImage, left, top, right, bottom, data->isActivation ? 255 : 180);
            ret = true;
        }
        argPs.stage = DRAW_STAGE_ICON_END;  // ���غ�Ͳ������滭����¼���
        if (data->pfnDraw)
            data->pfnDraw.pfn(&argPs);
    }

    return ret;
}

// �滭ָ��������Ĭ��ϵͳͼ��, ��������ťID��Ĭ�ϵİ�ťid, �Ǿͻ滭��Ӧ��Ĭ��ϵͳ��ť, �����Ƿ�滭
// rcF = ���������λ��
// btn = �����ť��Ӧ������, ����ֻʹ�õ��˰�ťID
// i = ����
inline bool _draw_DrawIndexSysButton(DRAW_WINDOW_DATA* data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, RECT_F& rcF, SYSBUTTON_INFO& btn, int i)
{
    LPEX_IMAGE hImage = _get_def_image();

    DRAW_WINDOW_DATA& sysBtn = *data;
    float index = 0;
    switch (btn.ID)
    {
    case SC_CLOSE:
        index = 0; break;
    case SC_RESTORE:
        index = 1; break;
    case SC_MAXIMIZE:
        index = 2; break;
    case SC_MINIMIZE:
        index = 3; break;

    case SC_SETTING:
        index = 4; break;
    case SC_SKIN:
        index = 5; break;
    case SC_HELP:
        index = 6; break;
    case SC_MENU:
        index = 7; break;

    default:
        return false;
    }


    if (sysBtn.isZoomed && btn.ID == SC_MAXIMIZE)
        index = 1;  // �����״̬, ���������ť��ID�����, �Ǿͻ滭��ԭͼ��, ������
    else if (!sysBtn.isZoomed && btn.ID == SC_RESTORE)
        index = 1;  // �������״̬, ���������ť��ID�ǻ�ԭ, �Ǿͻ滭���ͼ��, һ����

    RECT_F rcDraw(rcF);  // �滭�����λ��, ���ϵͳ��ť����30*30, �ǾͰ�ͼƬ�����м��λ��

    if (i == sysBtn.index)
    {
        // �滭�����ȵ���, ��ȡ�滭�ı�����ɫ, ����ͼ��ʹ�õ�����
        DWORD argb = 0;
        if (sysBtn.btn[i].ID == SC_CLOSE)
        {
            if (__query(sysBtn.state, STATE::HOT))
                argb = MAKEARGB(255, 212, 64, 39);
            else if (__query(sysBtn.state, STATE::DOWN))
                argb = MAKEARGB(255, 166, 42, 21);
            else if (__query(sysBtn.state, STATE::BAN))
                index += 3; // û�н�ֹ״̬, ���ﲻ����
        }
        else
        {
            if (__query(sysBtn.state, STATE::HOT))
                argb = RGB2ARGB(sysBtn.crHot, 255);
            else if (__query(sysBtn.state, STATE::DOWN))
                argb = RGB2ARGB(sysBtn.crDown, 255);
            else if (__query(sysBtn.state, STATE::BAN))
                index += 3; // û�н�ֹ״̬, ���ﲻ����
        }
        if (ARGB_GETA(argb))
        {
            LPEX_BRUSH hbr = _brush_create(argb);
            _canvas_fillRectangle(hCanvas, hbr, &rcF);  // ��ɫֵ��Ϊ0, ��Ҫ�滭����
            _brush_destroy(hbr);
        }
        if (__query(sysBtn.state, STATE::DOWN))
        {
            rcDraw.offset(1, 1);
        }

    }

    const float imgWidth    = 30.f;             // Ĭ�ϵ�ͼ����
    const float imgHeight   = 30.f;             // Ĭ�ϵ�ͼ��߶�
    const float imgLeft     = index * imgWidth; // ͼ�����λ�� = ͼ������ * ͼ����
    const float imgTop      = 0;                // ���ɺ�ɫ������Ҫ�����ֵ�ĳ�30
    RECT_F rcImg(imgLeft, imgTop, imgLeft + imgWidth, imgTop + imgHeight);

    rcDraw.left     = rcDraw.left + (rcDraw.width()  - imgWidth) / 2;
    rcDraw.top      = rcDraw.top  + (rcDraw.height() - imgHeight) / 2;
    rcDraw.right    = rcDraw.left + imgWidth;
    rcDraw.bottom   = rcDraw.top  + imgHeight;

    _canvas_drawimagerectrect(hCanvas, hImage,
        rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
        rcImg.left, rcImg.top, rcImg.right, rcImg.bottom, 255);

    return true;
}
bool CDrawWindow::DrawSysButton(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcBtn)
{
    DRAW_WINDOW_DATA& sysBtn = *data;
    RECT_F rcF = rcBtn;
    for (DWORD i = 0; i < sysBtn.count; i++)
    {
        // 0=�ر�,1=��ԭ,2=���,3=��С��,4=����,5=Ƥ��,6=����,7=�˵�
        // ����0, �ױ�30 �ǰ�ɫ�İ�ť, ����30, �ױ�60 �Ǻ�ɫ��ť
        SYSBUTTON_INFO& btn = sysBtn.btn[i];
        const int btnWidth  = btn.rc.right  - btn.rc.left;
        const int btnHeight = btn.rc.bottom - btn.rc.top ;

        rcF.left = rcF.right - (float)btnWidth;

        bool ret = false;
        DRAW_PAINTSTRUCT argPs = { 0 };
        argPs.hCanvas   = hCanvas;
        argPs.rc        = &rcF;
        argPs.stage     = DRAW_STAGE_BUTTON_BEGIN;
        argPs.id        = btn.ID;
        argPs.state     = ((i == sysBtn.index) ? sysBtn.state : STATE::NORMAL);
        argPs.param     = data->pfnDraw.param;
        if (data->pfnDraw)
            ret = data->pfnDraw.pfn(&argPs);

        if (!ret)   // û�����زŻ滭Ĭ�ϵ�ͼ��
        {
            _draw_DrawIndexSysButton(data, ps, hCanvas, rcF, btn, i);

            argPs.stage = DRAW_STAGE_BUTTON_END;
            if (data->pfnDraw)
                data->pfnDraw.pfn(&argPs);  // �滭ϵͳ��ť����

        }
        rcF.right -= (float)btnWidth;   // ��ť�ұ�����ƫ��
    }

    return true;
}

bool draw_OnPaint(HWND hWnd, const PAINTSTRUCT& ps, LPCANVAS hCanvas)
{
    DRAW_WINDOW_DATA* data = draw_GetSysButtonData(hWnd);
    if (!data)
        return false;
    return draw_OnPaint_DrawData(data, ps, hCanvas);
}


bool draw_WndProc_OnStyleChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    LPSTYLESTRUCT pStyle = (LPSTYLESTRUCT)lParam;
    if (wParam == GWL_STYLE)
    {
        data->style = pStyle->styleNew;
        data->isThickFrame = __query(data->style, WS_THICKFRAME);
    }
    else
    {
        data->styleEx = pStyle->styleNew;
    }
    return false;
}
bool draw_WndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    LPCWSTR str = 0;
    bool ret = false;

    switch (wParam)
    {
    case SC_MAXIMIZE:
        str = L"���";
        break;
    case SC_RESTORE:
        str = L"��ԭ";
        break;
    case SC_MINIMIZE:
        str = L"��С��";
        break;
    case SC_CLOSE:
        str = L"�ر�";
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
        ret = true;
        break;
    case SC_HELP:
        str = L"����";
        break;
    case SC_MENU:
        str = L"�˵�";
        ret = true;
        break;
    case SC_SETTING:
        str = L"����";
        ret = true;
        break;
    case SC_SKIN:
        str = L"Ƥ��";
        ret = true;
        break;
    default:
        return false;
    }

    wstr::dbg(L"ϵͳ��ť[%s]������, ID = 0x%X, %d\n", str, wParam, lParam);
    //return 0;
    return ret;
}

inline void CALLBACK _time_proc_show_shadow(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    LPDRAW_WINDOW_DATA data = (LPDRAW_WINDOW_DATA)(id - 1);
    --data->showCount;
    data->showImage = !data->showImage; // ������ʽ�л�
    if (data->showCount == 0)
    {
        wstr::dbg(L"������0, ʱ���˳�\n");
        KillTimer(hWnd, id);
        data->showImage = data->isActivation;
    }
    wstr::dbg(L"ʱ����, ʣ����� = %d\n", data->showCount);
    data->shadow->Update(data->rcWindow, true);
}
bool draw_WndProc_OnWindowPosChangIng(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    const int flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
    if (pos->flags != flags)
        return false;

    if (!data->shadow || pos->hwndInsertAfter != data->shadow->GetWindow())
        return false;

    HWND hParent = (HWND)GetWindowLongPtrW(hWnd, GWLP_HWNDPARENT);
    if (!hParent || IsWindowEnabled(hParent))   // ������û�н�ֹ�Ͳ�����
        return false;

    // ��Ҫ������ʾ��Ӱ����
    wstr::dbg(L"���� = %d\n", data->showCount);

    const bool isSetTimer = data->showCount == 0;
    data->showCount = 10;
    if (isSetTimer)
        SetTimer(hWnd, ((UINT_PTR)(data)) + 1, 100, _time_proc_show_shadow);
    return false;
}
bool draw_WndProc_OnWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    // û����������־, Ҫô�ı��˳ߴ�, Ҫô�ƶ��˴���
    if ((pos->flags & SWP_NOMOVE) == 0 || (pos->flags & SWP_NOSIZE) == 0)
    {
        const int cxScreen = data->rcWindow.right - data->rcWindow.left;
        const int cyScreen = data->rcWindow.bottom - data->rcWindow.top;
        data->rcWindow.left     = pos->x;
        data->rcWindow.top      = pos->y;
        data->rcWindow.right    = pos->x + cxScreen;
        data->rcWindow.bottom   = pos->y + cyScreen;

        if ((pos->flags & SWP_NOSIZE) == 0) // û�в������ߴ��־, �Ǿ��ǵ����˳ߴ�, ��Ҫ���¼���λ��
        {
            data->rcWindow.right = pos->x + pos->cx;
            data->rcWindow.bottom = pos->y + pos->cy;

            data->rcClient.left = 0;
            data->rcClient.top = 0;
            data->rcClient.right = pos->cx;
            data->rcClient.bottom = pos->cy;


            DRAW_WINDOW_DATA* sysBtn = data;

            sysBtn->rcBtn.right = pos->cx;
            sysBtn->rcBtn.left = sysBtn->rcBtn.right - sysBtn->count * sysBtn->sysbtnWidth;
            sysBtn->rcBtn.top = 0;
            sysBtn->rcBtn.bottom = sysBtn->sysbtnHeight;

            draw_ModifySysButtonPos(data);

            //HRGN hRgn = CreateRectRgn(0, 0, pos->cx, pos->cy);
            //SetWindowRgn(hWnd, hRgn, false);
            //DeleteObject(hRgn);
            //InvalidateRect(hWnd, 0, 0);
        }
        data->shadow->Update(data->rcWindow);
    }
    return false;   // ����ֻ��¼һЩ��Ϣ, ��������Ϣ
}
bool draw_WndProc_OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { 0 };
    mi.cbSize = sizeof(MONITORINFO);
    if (hMonitor)
    {
        if (!GetMonitorInfoW(hMonitor, &mi))
            return false;
    }
    OffsetRect(&mi.rcWork, -mi.rcMonitor.left, -mi.rcMonitor.top);

    LPMINMAXINFO pos = (LPMINMAXINFO)lParam;
    // ��߶���
    pos->ptMaxPosition.x = mi.rcWork.left;
    pos->ptMaxPosition.y = mi.rcWork.top;
    //pos->ptMaxPosition.x = 0;
    // 
    //pos->ptMaxPosition.y = 0;

    // ����ȸ߶�
    pos->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
    pos->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
    //pos->ptMaxSize.x = 1366;
    //pos->ptMaxSize.y = 728;
    // �����������С�ߴ�
    pos->ptMinTrackSize.x = data->nMinWidth;    // ��С�϶���С
    pos->ptMinTrackSize.y = data->nMinHeight;
    // ������������ߴ�
    pos->ptMaxTrackSize.x = pos->ptMaxSize.x;   // ����϶���С, ���������ֵ�Ļ�������Ļ��dpi���ŵĻ���󻯻������
    pos->ptMaxTrackSize.y = pos->ptMaxSize.y;
    return true;
}
void CALLBACK _draw_TimerProc_Leave_mouse(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    LPDRAW_WINDOW_DATA data = (LPDRAW_WINDOW_DATA)id;
    LPDRAW_WINDOW_DATA sysBtn = data;
    if (sysBtn->index == -1)
    {
        //s_info->pTips->hide();
        KillTimer(hWnd, id);
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    if (!PtInRect(&sysBtn->rcBtn, pt))
    {
        sysBtn->index = -1;
        sysBtn->state = STATE::NORMAL;
        _draw_UpdateSysbtn(data);
    }

}
bool draw_WndProc_OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    switch (message)
    {
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        return _draw_Mouse_OnLDwon(hWnd, message, wParam, lParam, data, pRet);
    case WM_LBUTTONUP:
        return _draw_Mouse_OnLUp(hWnd, message, wParam, lParam, data, pRet);
    case WM_MOUSEMOVE:
        return _draw_Mouse_OnMove(hWnd, message, wParam, lParam, data, pRet);
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
        return _draw_Mouse_OnRDwon(hWnd, message, wParam, lParam, data, pRet);
    case WM_RBUTTONUP:
        return _draw_Mouse_OnRUp(hWnd, message, wParam, lParam, data, pRet);
    case WM_MOUSEHOVER:

        return false;
    case WM_MOUSELEAVE:
    {
        data->index = -1;
        if (data->state != STATE::NORMAL)
        {
            data->state = STATE::NORMAL;
            _draw_UpdateSysbtn(data);
        }
        return false;
    }
    default:
        return false;
    }
    return false;
}
bool draw_WndProc_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    RECT& rcWindow = data->rcWindow;
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    const int cxClient = rcWindow.right  - rcWindow.left;
    const int cyClient = rcWindow.bottom - rcWindow.top;
    if (!data->hCanvas)
    {
        data->hCanvas = _canvas_create(hWnd, cxClient, cyClient);
        data->freeFlags |= FREE_CANVAS;
    }
    if (data->hCanvas->GetWidth() != (UINT)cxClient || data->hCanvas->GetHeight() != (UINT)cyClient)
        data->hCanvas->Resize(cxClient, cyClient);
    
    data->hCanvas->BeginDraw();
    data->hCanvas->clear(data->crBack);
    draw_OnPaint_DrawData(data, ps, data->hCanvas);
    HDC hdc = data->hCanvas->GetDC();
    BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
    data->hCanvas->ReleaseDC();
    data->hCanvas->EndDraw();

    EndPaint(hWnd, &ps);
    return true;
}

bool draw_WndProc_OnNcHittest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    *pRet = draw_WndProc_OnNcHittest_return(hWnd, lParam, data);
    return true;
}

LRESULT draw_WndProc_OnNcHittest_return(HWND hWnd, LPARAM lParam, DRAW_WINDOW_DATA* data)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ScreenToClient(data->hWnd, &pt);    // �����¼������λ�ö��ǿͻ������� ��Ҫת��һ��

    if (!data->isZoomed && data->isThickFrame)  // �пɵ��߿�, ��û�����, �Ŵ���4���ߵĵ����ߴ�
    {
        const int offset = _scale(5);

        RECT rc = data->rcClient;
        // ���������½�,�Ӵ����ұ߼�ȥ���õ�ֵ��Χ�ڶ��������½�
        rc.left = data->rcClient.right - offset;    // ������굽�Ǹ�������,�����ұߵױ� - ���õ�ֵ,�������½�
        rc.top = data->rcClient.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOMRIGHT;    // ���½�

        rc = data->rcClient;
        rc.top = data->rcClient.bottom - offset;
        rc.right = data->rcClient.left + offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOMLEFT;    // ���½�

        rc = data->rcClient;
        rc.right = data->rcClient.left + offset;
        rc.bottom = data->rcClient.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOPLEFT;        // ���Ͻ�

        rc = data->rcClient;
        rc.left = data->rcClient.right - offset;
        rc.bottom = data->rcClient.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOPRIGHT;        // ���Ͻ�


        rc = data->rcClient;
        rc.right = data->rcClient.left + offset;
        rc.top = data->rcClient.top + offset;
        rc.bottom = data->rcClient.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTLEFT;            // ���

        rc = data->rcClient;
        rc.bottom = data->rcClient.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOP;            // ����

        rc = data->rcClient;
        rc.left = data->rcClient.right - offset;
        if (PtInRect(&rc, pt))
            return HTRIGHT;            // �ұ�

        rc = data->rcClient;
        rc.top = data->rcClient.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOM;            // �ױ�
    }



    //rcTmp = rc;
    //rcTmp.bottom = rc.top + 30;
    //rcTmp.left = rcTmp.right - 30;
    //if (PtInRect(&rcTmp, pt))
    //    return HTCLOSE;        // �رհ�ť



    if (PtInRect(&data->rcBtn, pt))
        return HTCLIENT;

    if (data->isDrawIcon && PtInRect(&data->rcIcon, pt))
        return HTSYSMENU;        // �˵�

    if (PtInRect(&data->rcTitle, pt))
        return HTCAPTION;        // ������

    if (data->isCasualMove && !data->isZoomed)
    {
        // �����ƶ�, ��Ҫ�ж��Ƿ����, ��󻯵Ļ���ֻ�����ڱ�������
        return HTCAPTION;
    }

    if (pt.y > data->rcTitle.bottom)  // ���ߴ��ڱ�������, ���ǿͻ���
        return HTCLIENT;    // �ڴ��ڿͻ�����

    return HTCAPTION;
}

bool _draw_Mouse_OnLDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    LPDRAW_WINDOW_DATA sysBtn = data;
    if (sysBtn->index == -1)
        return false;

    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (message == WM_LBUTTONDBLCLK)
        return false;   // ˫��������
    switch (sysBtn->btn[sysBtn->index].ID)
    {
    case SC_CLOSE:
        if (!sysBtn->isZoomed && (pt.x > sysBtn->rcBtn.right - 5 || pt.y < 5))    // ���ұ߻��߶���5������
            return false;   // �������, ����������Ͻ� 5��������, �����ǵ����ߴ�, ���ﲻ���� 
        break;
    default:
        if (!sysBtn->isZoomed && pt.y < 5)
            return false;   // �������, ������ڶ��� 5��������, �����ǵ����ߴ�, ���ﲻ���� 
        break;
    }
    sysBtn->downIndex = sysBtn->index;
    sysBtn->state = STATE::DOWN;
    _draw_UpdateSysbtn(data);   // �ı�״̬Ϊ����, Ȼ���ػ�
    return true;
}
bool _draw_Mouse_OnLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    LPDRAW_WINDOW_DATA sysBtn = data;
    // ��ǰ��갴�µİ�ť��Ϊ-1, �ҵ�ǰ������İ�ť���ڰ��µİ�ť, ��ʾ�ſ�ʱ�İ�ť�ǰ��µİ�ť
    if (sysBtn->downIndex != -1 && sysBtn->downIndex == sysBtn->index)
    {
        sysBtn->downIndex = -1;
        _draw_SysBtnClick(data, false);
        *pRet = data->fnCallProc(data->oldProc, hWnd, message, wParam, lParam);
        return true;
    }

    // �ߵ�����, ���ºͷſ��Ĳ���ͬһ����ť

    if (sysBtn->state)    // �ſ�ʱ�а�ť��״̬, ��Ҫȥ��״̬
    {
        for (DWORD i = 0; i < sysBtn->count; i++)
        {
            if (PtInRect(&sysBtn->btn[i].rc, pt))    // ���ĸ���ť��
            {
                sysBtn->state = STATE::HOT;
                _draw_UpdateSysbtn(data);
                *pRet = data->fnCallProc(data->oldProc, hWnd, message, wParam, lParam);
                return true;
            }
        }
        // �������κΰ�ť��, ����ť����Ϊ��ͨ״̬, ��������һ�㴦��
        sysBtn->state = STATE::NORMAL;
        _draw_UpdateSysbtn(data);

    }
    return false;
}
bool _draw_Mouse_OnRDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    DRAW_WINDOW_DATA* sysBtn = data;
    if (sysBtn->index != -1)    // ��갴��ǰ�ȵ���϶��Ѿ�ȷ����, ������µ�ʱ���ȵ�����ֵ, �Ǿ��ǰ������ȵ�����
    {
        sysBtn->rdownIndex = sysBtn->index;    //�Ҽ����µİ�ť�����ǵ�ǰ������ڵ�����
        *pRet = data->fnCallProc(data->oldProc, hWnd, message, wParam, lParam);
        return true;
    }
    return false;
}
bool _draw_Mouse_OnRUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    DRAW_WINDOW_DATA* sysBtn = data;
    // ��ǰ��갴�µİ�ť��Ϊ-1, �ҵ�ǰ������İ�ť���ڰ��µİ�ť, ��ʾ�ſ�ʱ�İ�ť�ǰ��µİ�ť
    if (sysBtn->rdownIndex != -1 && sysBtn->rdownIndex == sysBtn->index)
    {
        _draw_SysBtnClick(data, true);
        *pRet = data->fnCallProc(data->oldProc, hWnd, message, wParam, lParam);
        return true;
    }
    return false;
}
bool _draw_Mouse_OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DRAW_WINDOW_DATA* data, LRESULT* pRet)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    DRAW_WINDOW_DATA* sysBtn = data;
    bool ret = false;
    if (PtInRect(&sysBtn->rcBtn, pt))
    {
        ret = true; // ֻҪ����ϵͳ��ť��Χ��, ���Ǵ������
        for (DWORD i = 0; i < sysBtn->count; i++)
        {
            if (PtInRect(&sysBtn->btn[i].rc, pt))    // ���ĸ���ť��
            {
                if (i == sysBtn->index) // ����ڵ�λ�ú��ϴ�һ��
                    return true;
                //TRACKMOUSEEVENT tm = { 0 };
                //tm.cbSize = sizeof(tm);
                //tm.dwHoverTime = 10;
                //tm.hwndTrack = pData->hObj;
                //tm.dwFlags = TME_NONCLIENT;
                //BOOL b = TrackMouseEvent(&tm);

                sysBtn->index = i;
                sysBtn->state = (wParam & VK_LBUTTON) ? STATE::DOWN : STATE::HOT;
                _draw_UpdateSysbtn(data);
                SetTimer(hWnd, (UINT_PTR)data, 50, _draw_TimerProc_Leave_mouse);
                if (sysBtn->state == STATE::HOT)
                {
                    //TODO ������Ҫһ����ʾ, ���ָ��ȥ��ʾ��ʾ��Ϣ
                    //LPCWSTR title = L"";
                    //LPCWSTR tips = _sysbtnid2tips(sysBtn->btn[i].ID, &title);
                    //if (tips && tips[0] || title && title[0])
                    //{
                    //    s_info->pTips->SetTipsTitle(title);

                    //    LPEX_FONT hFont = g_scale.GetFont(s_info->defData->draw.hFont);
                    //    LPEX_FONT hFontBold = g_scale.GetFont(s_info->defData->draw.hFontBold);
                    //    POINT pt = { sysBtn->btn[i].rc.left, sysBtn->btn[i].rc.bottom };
                    //    pt.x += s_info->rcEWindow.left;
                    //    pt.y += s_info->rcEWindow.top;
                    //    s_info->pTips->show(tips, hFontBold, hFont, pt.x, pt.y);
                    //}
                }

                return ret;
            }
        }

    }
    //else if (PtInRect(&sysBtn->rcCaption, pt))
    //{
    //    // ����ڱ�����
    //}
    //else if (PtInRect(&sysBtn->rcIcon, pt))
    //{
    //    // �����ϵͳͼ����
    //}

    // �ߵ���������û�����κ�ϵͳ��ť��
    if (sysBtn->index != -1)
    {
        sysBtn->index = -1;
        _draw_UpdateSysbtn(data);
    }
    return ret;
}

bool _draw_SysBtnClick(DRAW_WINDOW_DATA* data, bool isRButton)
{
    DRAW_WINDOW_DATA* sysBtn = data;
    bool isRet = false;
    int id = data->btn[sysBtn->index].ID;
    LPARAM param = data->btn[sysBtn->index].param;
    if (data->isZoomed && id == SC_MAXIMIZE)        // ����Ѿ����, ���Ұ�ťid����󻯰�ť, �ǾͰ�id��Ϊ��ԭ
        id = SC_RESTORE;
    else if (!data->isZoomed && id == SC_RESTORE)   // ���û�����, ���Ұ�ťid�ǻ�ԭ��ť, �ǾͰ�id��Ϊ���
        id = SC_MAXIMIZE;

    wstr::dbg(L"ϵͳ��ť������, ID = %d, ���� = %d\n", id, sysBtn->index);
    if (sysBtn->pfnSysBtnClick)
    {
        isRet = sysBtn->pfnSysBtnClick.pfn(id, param, isRButton, sysBtn->pfnSysBtnClick.param);
    }

    if (isRet)
    {
        sysBtn->state = STATE::HOT; // ���ſ�, �����ȵ�״̬, ����ſ���λ�ÿ϶��ǰ���ʱ�İ�ť����
        _draw_UpdateSysbtn(data);
        return true;    // �ص����������˵���¼�, �ڲ����ٴ���
    }

    if (isRButton)
    {
        // �Ҽ�����ϵͳ��ť, �������⴦��
        sysBtn->rdownIndex = -1;
        return true;
    }
    // �������ϵͳ��ť, �������⴦��
    sysBtn->downIndex = -1;     // ���ſ������ð��µ�����Ϊ-1

    SendMessageW(data->hWnd, WM_SYSCOMMAND, id, 0);

    sysBtn->state = STATE::HOT; // ���ſ�, �����ȵ�״̬, ����ſ���λ�ÿ϶��ǰ���ʱ�İ�ť����
    _draw_UpdateSysbtn(data);



    return true;

}


bool WINAPI draw_OnShadowPaint(LPCANVAS hCanvas, LPEX_IMAGE _pImg, const RECT* prc, COLORREF crShadow, DWORD argbBorder, int alpha, LPVOID param)
{
    int cxWin = prc->right - prc->left;
    int cyWin = prc->bottom - prc->top;
    RECT_F rc;
    rc.left = 10;
    rc.top = 10;
    rc.right = (float)cxWin - 10;
    rc.bottom = (float)cyWin - 10;
    const int cxClient = (int)(rc.right - rc.left);   // ��������Ӱ�ڿͻ������, �߿�ͱ�����������������Χ�ڻ滭
    const int cyClient = (int)(rc.bottom - rc.top);   // ��������Ӱ�ڿͻ������, �߿�ͱ�����������������Χ�ڻ滭
    if (alpha > 0)
    {
        LPDRAW_WINDOW_DATA data = (LPDRAW_WINDOW_DATA)param;
        LPEX_IMAGE hImage = data->showImage ? data->hImageShdow : data->hImageShdow_nf;
        DWORD crBorder = argbBorder; // data->showImage ? data->crBorder : data->crBorder_nf;
        _canvas_drawimagegridPadding(hCanvas, hImage,
            0, 0, (float)cxWin, (float)cyWin,   // �����ĸ�λ����
            0, 0, 40, 40,                       // ��ͼƬ���ĸ�λ��
            16, 16, 20, 20,                     // �Ź�����
            0, 255);


        if (data->showCount % 2)
        {
            DWORD crBack = data->crBack;
            BYTE _a = ARGB_GETA(crBack);
            if (_a > 128)
                crBack = ARGB_SETA(crBack, _a - 50);
            else
                crBack = ARGB_SETA(crBack, _a + 50);

            LPEX_BRUSH hbrBack = _brush_create(crBack);
            RECT_F rcf(rc);
            rcf.bottom = rcf.top + data->sysbtnHeight;
            _canvas_fillRectangle(hCanvas, hbrBack, &rcf);
            _brush_destroy(hbrBack);
        }

        if (ARGB_GETA(crBorder))
        {
            LPEX_PEN hPen = _pen_create(crBorder);

            int oldMode = 0;
            _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);     // ���߲���Ҫ�����
            _canvas_drawRectangle(hCanvas, hPen, &rc);
            _canvas_SetAntialiasMode(hCanvas, oldMode, 0);
            _pen_destroy(hPen);
        }

    }

    return true;
}

void _draw_window_activate_changed(bool isActivate, LPARAM lParam, LPDRAW_WINDOW_DATA data)
{
    data->isActivation = isActivate;
    data->showImage = isActivate;
    RECT rc = data->rcBtn;
    rc.left = 0;    // �ػ�����������, ͼ��, ϵͳ��ť

    InvalidateRect(data->hWnd, &rc, 0);    // �ػ�����, ������һ����ɫ, δ������һ����ɫ
    ARGB crShadow = data->isActivation ? data->crShadow : data->crShadow_nf;
    ARGB crBorder = data->isActivation ? data->crBorder : data->crBorder_nf;

    data->shadow->SetColor(crShadow, crBorder, true);
}

LPEX_IMAGE _get_def_image()
{
    static LPEX_IMAGE hImg;
    if (!hImg)
    {
        // "D:\cpp\SVN����Ŀ¼\�����Խ���\src\ϵͳ��ť.png"
        const BYTE data[] =
        {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0xF0,
            0x00, 0x00, 0x00, 0x3C, 0x08, 0x06, 0x00, 0x00, 0x00, 0x58, 0x7F, 0x37, 0x5D, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
            0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
            0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7,
            0x6F, 0xA8, 0x64, 0x00, 0x00, 0x0A, 0x7C, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5E, 0xED, 0x9D, 0x5D, 0x68, 0x54, 0xD7, 0x16,
            0xC7, 0x33, 0xF1, 0x2B, 0x99, 0x41, 0x92, 0xA3, 0x26, 0x9A, 0x89, 0xC6, 0x74, 0x7A, 0xAD, 0x0F, 0x72, 0x7D, 0x30, 0x88,
            0xB5, 0xB9, 0x60, 0x1F, 0x0A, 0x51, 0x94, 0xCB, 0x35, 0xC9, 0x20, 0x88, 0xF8, 0x24, 0x23, 0x05, 0x6F, 0xA9, 0x41, 0x88,
            0xCF, 0x7D, 0x89, 0x0F, 0x92, 0xDA, 0xF6, 0x52, 0x18, 0x7C, 0x11, 0x41, 0xD0, 0x24, 0x7A, 0xB9, 0xE2, 0x07, 0x92, 0x3E,
            0x14, 0xEF, 0x83, 0x88, 0xE3, 0xCB, 0x15, 0x4B, 0x2C, 0x26, 0x69, 0x6C, 0xE2, 0xB7, 0xA9, 0xC1, 0x26, 0xA9, 0xF1, 0x9A,
            0xE9, 0xFA, 0xEF, 0x59, 0x7B, 0x7A, 0x9C, 0x9C, 0xF9, 0x48, 0xCF, 0x3E, 0xCE, 0x4C, 0x5C, 0x3F, 0xF8, 0x73, 0xCE, 0xDE,
            0x7B, 0x9D, 0x73, 0xB2, 0x9D, 0xBD, 0xCE, 0xDA, 0x1F, 0xC7, 0x73, 0x4A, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41,
            0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x61, 0x26, 0x3E, 0xDE, 0x16, 0x03, 0x65, 0xA4, 0x0A,
            0x52, 0xB9, 0x4A, 0x39, 0x33, 0x49, 0x1A, 0x23, 0xFD, 0xA6, 0x52, 0x82, 0x20, 0x14, 0x0C, 0xCB, 0xE3, 0x39, 0x00, 0xBB,
            0x84, 0xB9, 0xE0, 0x12, 0x8B, 0x25, 0x14, 0x30, 0xA5, 0xBC, 0x2D, 0x06, 0x32, 0x45, 0x5E, 0x3B, 0xB9, 0xDA, 0x09, 0xE9,
            0xB1, 0x9E, 0x3D, 0x7B, 0x16, 0x85, 0xB0, 0x9F, 0xC8, 0x9A, 0xD3, 0xE8, 0x9B, 0x95, 0x5D, 0x45, 0xC1, 0x6C, 0x1C, 0xD8,
            0x4F, 0xAA, 0x25, 0x05, 0x79, 0x5F, 0x77, 0xBF, 0xB1, 0x85, 0xD3, 0x20, 0x1F, 0xE5, 0x28, 0x33, 0x01, 0xBA, 0xCC, 0x88,
            0xA6, 0xF5, 0x2C, 0x9C, 0x5F, 0xE1, 0xF3, 0xF9, 0x1A, 0x6D, 0x7A, 0x0F, 0x5B, 0x2E, 0x32, 0x89, 0xBE, 0x6E, 0x36, 0xCD,
            0x09, 0x2E, 0x5C, 0xB8, 0x10, 0xEA, 0xEB, 0xEB, 0x0B, 0xD3, 0xAE, 0x6A, 0xBC, 0x7E, 0xBF, 0x3F, 0x04, 0x61, 0xBF, 0xAE,
            0xAE, 0xCE, 0x1A, 0x18, 0x18, 0x88, 0xF4, 0xF4, 0xF4, 0xA8, 0xB4, 0x17, 0x4C, 0xFC, 0x7F, 0x22, 0x0E, 0x71, 0x52, 0xE1,
            0x94, 0x67, 0x18, 0xEB, 0xF8, 0xF1, 0xE3, 0x0D, 0xC3, 0xC3, 0xC3, 0x47, 0x26, 0x27, 0x27, 0x7B, 0x5F, 0xBD, 0x7A, 0x15,
            0x1B, 0x1F, 0x1F, 0xEF, 0x45, 0x1A, 0xF9, 0x28, 0x4F, 0x98, 0xCD, 0x0D, 0x6A, 0xD1, 0x45, 0x1D, 0x1D, 0x1D, 0xFD, 0xB9,
            0xB5, 0xB5, 0x75, 0x0B, 0xA5, 0x03, 0x24, 0x38, 0xAF, 0x7F, 0xC7, 0x8E, 0x1D, 0x9B, 0x9F, 0x3C, 0x79, 0xD2, 0xCF, 0x5D,
            0x58, 0x38, 0xB1, 0x09, 0x66, 0xD3, 0x65, 0xAE, 0xE7, 0x24, 0xD2, 0x26, 0x9C, 0xAA, 0x7E, 0x6A, 0x6A, 0xEA, 0x57, 0x3E,
            0x65, 0x5A, 0x60, 0x03, 0xDB, 0xC4, 0x21, 0xC5, 0xCD, 0x9D, 0x3B, 0x77, 0x22, 0xD4, 0x88, 0x47, 0xA9, 0xF1, 0x46, 0xAF,
            0x5E, 0xBD, 0xDA, 0x4E, 0x8D, 0xB9, 0x1F, 0xBA, 0x76, 0xED, 0x5A, 0xFB, 0xE3, 0xC7, 0x8F, 0x7B, 0x51, 0x36, 0x70, 0xF7,
            0x6E, 0x84, 0xCD, 0x8D, 0x32, 0xF1, 0x9A, 0x1C, 0x35, 0x8B, 0xD8, 0xD4, 0x24, 0xD6, 0x7F, 0xBF, 0xFF, 0x3E, 0x8C, 0x3A,
            0xA2, 0x6E, 0xA9, 0x42, 0x3E, 0xCA, 0x61, 0x97, 0x30, 0x37, 0x43, 0x45, 0x45, 0x85, 0x95, 0x4D, 0x6C, 0x6A, 0x9C, 0x20,
            0x9C, 0x17, 0x0D, 0x97, 0xBA, 0x56, 0xC3, 0x2D, 0x2D, 0x2D, 0x1F, 0x53, 0x5E, 0xF5, 0xB6, 0x6D, 0xDB, 0x1A, 0x1F, 0x3D,
            0x7A, 0x34, 0x88, 0x7C, 0x72, 0xE2, 0xBB, 0x94, 0x57, 0xA3, 0xAC, 0xDD, 0x93, 0x74, 0xCA, 0x4C, 0xC0, 0xCE, 0x6E, 0xCB,
            0x69, 0xB7, 0xE4, 0x74, 0x6D, 0x00, 0xDB, 0xC4, 0x21, 0x45, 0x8F, 0x35, 0x32, 0x32, 0xD2, 0xE5, 0xD4, 0x98, 0x21, 0x38,
            0xF1, 0x5F, 0x29, 0x12, 0xB3, 0xAD, 0x31, 0x9C, 0x9C, 0x35, 0x9D, 0xF8, 0x10, 0x23, 0x7C, 0xFB, 0xD5, 0x57, 0x0D, 0xBA,
            0x6E, 0x63, 0x63, 0x63, 0xB1, 0x9E, 0xAE, 0xAE, 0xC8, 0xBF, 0x7B, 0x7A, 0x22, 0x3F, 0xF6, 0xF5, 0x45, 0xED, 0xF5, 0x86,
            0x1D, 0x1F, 0xE2, 0x1A, 0xF2, 0x93, 0x28, 0x37, 0x9B, 0x8C, 0xC0, 0x8E, 0x0F, 0x31, 0x8A, 0x1F, 0x91, 0x17, 0xCE, 0x8B,
            0x8B, 0x3C, 0x7D, 0xFA, 0x74, 0x64, 0xEF, 0xDE, 0xBD, 0xFB, 0x1F, 0x3E, 0x7C, 0xF8, 0x13, 0x5F, 0x74, 0x60, 0xEB, 0xD6,
            0xAD, 0x1F, 0x92, 0x9D, 0xA9, 0x31, 0xA8, 0xDD, 0x29, 0x3F, 0x42, 0xDA, 0xA6, 0x8F, 0xB8, 0x48, 0x97, 0xD9, 0xD3, 0x46,
            0x1D, 0x18, 0xFB, 0x4E, 0xE2, 0x62, 0x53, 0xD7, 0xCB, 0x37, 0x70, 0x4C, 0x0B, 0xD1, 0x56, 0x37, 0xE8, 0xD3, 0xA7, 0x4F,
            0x87, 0xFF, 0x73, 0xEE, 0x5C, 0x18, 0xFB, 0xC8, 0x43, 0x54, 0xD6, 0x76, 0x38, 0xC0, 0x14, 0x4E, 0x8E, 0x9A, 0x4E, 0x7C,
            0x88, 0x09, 0xAC, 0x07, 0x0F, 0x1E, 0x28, 0x47, 0x7D, 0xF9, 0xF2, 0x65, 0xFF, 0xE7, 0x07, 0x3E, 0xFF, 0xE4, 0xDE, 0xBD,
            0x7B, 0x51, 0xEA, 0x3E, 0xC7, 0xBE, 0xFD, 0xE6, 0x9B, 0xB0, 0xFD, 0x46, 0x06, 0x3B, 0xD8, 0x27, 0x0E, 0x73, 0x07, 0xA2,
            0xEB, 0xFD, 0xFB, 0xF7, 0x7B, 0xB9, 0xE9, 0x38, 0x82, 0x72, 0xAF, 0xA2, 0x30, 0xBA, 0xCB, 0x01, 0x44, 0x5E, 0x38, 0x2F,
            0x2E, 0xF6, 0xFA, 0xF5, 0xEB, 0x69, 0x6C, 0xC9, 0x89, 0x07, 0x9B, 0x9A, 0x9A, 0x36, 0x53, 0xB9, 0x7D, 0x6C, 0xEC, 0x96,
            0x4C, 0x4E, 0x92, 0x36, 0x42, 0xA2, 0x2C, 0x61, 0xE2, 0x8A, 0x6C, 0x0E, 0x3A, 0x97, 0x1C, 0xD8, 0xA2, 0xDF, 0x33, 0x4A,
            0xBD, 0x2B, 0x8C, 0x01, 0x55, 0x77, 0xF2, 0x5C, 0x77, 0xB7, 0xEE, 0x3A, 0x5A, 0x70, 0x64, 0xE4, 0x71, 0x59, 0xEC, 0xF9,
            0xE8, 0x68, 0x17, 0x97, 0x19, 0xC5, 0xC9, 0x61, 0xB5, 0xD8, 0xC4, 0x24, 0x16, 0x1C, 0x17, 0xF5, 0xC2, 0x0D, 0x8A, 0xC0,
            0x18, 0x58, 0x39, 0xEC, 0x8D, 0x1B, 0x37, 0xDA, 0xF5, 0x8D, 0x0C, 0x82, 0x1D, 0xEC, 0x13, 0x87, 0xB9, 0x07, 0xCE, 0x49,
            0xC1, 0xCE, 0xD1, 0x89, 0x91, 0x3F, 0x5B, 0xE7, 0x9D, 0xCD, 0x24, 0x16, 0xFE, 0x21, 0x27, 0xCE, 0x9E, 0x3D, 0xFB, 0x43,
            0x5B, 0x5B, 0xDB, 0x17, 0xD3, 0xD3, 0xD3, 0xF1, 0xD2, 0xD2, 0x52, 0x1F, 0xB6, 0x87, 0x0E, 0x1D, 0xEA, 0xB8, 0x72, 0xE5,
            0x0A, 0xBA, 0xCF, 0x58, 0x87, 0xF5, 0xE2, 0x1F, 0x7C, 0x36, 0xE0, 0x6F, 0x10, 0x84, 0x8C, 0x50, 0xDB, 0x55, 0xDB, 0x85,
            0x0B, 0x17, 0x86, 0xD6, 0xAD, 0x5B, 0xA7, 0xC6, 0xF6, 0x13, 0x13, 0x13, 0x37, 0xFF, 0xF5, 0xE5, 0x97, 0xDF, 0xAD, 0x5F,
            0xBF, 0x1E, 0x37, 0x30, 0x85, 0xB6, 0x33, 0x05, 0xDD, 0x30, 0x7E, 0x39, 0x78, 0xF0, 0xE0, 0x7E, 0xBA, 0x61, 0xDE, 0xE4,
            0x2C, 0x05, 0xD2, 0xC8, 0x47, 0x39, 0x67, 0x19, 0x47, 0x4D, 0x58, 0x61, 0xCC, 0xAB, 0xBB, 0xCD, 0x3A, 0x02, 0x23, 0x22,
            0xF3, 0x98, 0x58, 0x4F, 0x6C, 0x99, 0xC0, 0x1E, 0xE5, 0xB2, 0x75, 0xA1, 0x75, 0x3E, 0x26, 0xB4, 0x30, 0x7B, 0xED, 0x96,
            0xD4, 0x08, 0x3B, 0x43, 0x5C, 0xAC, 0xCB, 0x4D, 0xB2, 0x2A, 0x47, 0x99, 0x44, 0x45, 0xDB, 0xD4, 0x2E, 0x34, 0x22, 0x31,
            0xED, 0xAB, 0x09, 0x2C, 0xAF, 0xBA, 0xD0, 0x1A, 0xA7, 0xC8, 0xAB, 0xC5, 0x26, 0x26, 0x49, 0x46, 0x60, 0x2D, 0xD4, 0x79,
            0x51, 0x49, 0x49, 0x88, 0xDA, 0xF6, 0x1B, 0xF3, 0x00, 0xA6, 0x23, 0xB0, 0x66, 0xF7, 0xEE, 0xDD, 0xA1, 0x17, 0x2F, 0x5E,
            0xA8, 0x89, 0x5F, 0x6C, 0x91, 0xE6, 0x22, 0xCF, 0x28, 0xC7, 0x6C, 0x33, 0x85, 0x79, 0x35, 0x61, 0x85, 0x6E, 0xF3, 0x9E,
            0x3D, 0x7B, 0x22, 0xBA, 0x3B, 0x8D, 0xB1, 0x31, 0xCF, 0x4E, 0x9B, 0x5A, 0x46, 0x4A, 0x3A, 0x49, 0x26, 0x60, 0x97, 0x30,
            0x37, 0x4A, 0x4E, 0xD7, 0x06, 0xB0, 0x4D, 0x1C, 0x62, 0x84, 0x55, 0xB3, 0x98, 0xFD, 0x36, 0xEE, 0xC4, 0x98, 0x81, 0xB6,
            0x37, 0x5E, 0xBB, 0xBC, 0x9A, 0xC4, 0xD2, 0x38, 0x39, 0xAE, 0x16, 0x9B, 0x98, 0x24, 0x39, 0x06, 0xD6, 0x42, 0xF7, 0xF5,
            0xDE, 0xD0, 0xD0, 0x8C, 0x49, 0x3C, 0x93, 0x63, 0xE0, 0x54, 0x3A, 0x3A, 0x3A, 0x1A, 0xC8, 0x6F, 0x62, 0xD8, 0x72, 0x96,
            0xA7, 0x04, 0xF5, 0x52, 0x11, 0x26, 0xAC, 0x78, 0xCC, 0x5B, 0x85, 0xC8, 0xAB, 0x27, 0xB6, 0x30, 0x4B, 0x0D, 0x3B, 0x65,
            0xED, 0x9E, 0x7C, 0x3E, 0x79, 0x95, 0xAF, 0x65, 0xA4, 0xBC, 0x39, 0x30, 0xD6, 0x80, 0xD1, 0x60, 0xE1, 0xC4, 0x88, 0xC4,
            0x18, 0xF3, 0x42, 0xD7, 0xAF, 0x5F, 0x6F, 0xA7, 0xDF, 0xD7, 0xD3, 0x65, 0x24, 0xE0, 0xE4, 0xB8, 0x5A, 0x6C, 0x62, 0x14,
            0xFB, 0x2C, 0x34, 0x84, 0xC9, 0x23, 0x27, 0x07, 0x36, 0x39, 0x0B, 0x9D, 0x6F, 0xD4, 0x3A, 0x30, 0x96, 0x8A, 0x78, 0xB6,
            0x59, 0x4F, 0x58, 0x05, 0x10, 0x79, 0xF5, 0x12, 0x13, 0xEC, 0x48, 0x26, 0x48, 0x7D, 0x90, 0xC3, 0x49, 0xA6, 0xBA, 0xCC,
            0x4E, 0x38, 0x5D, 0xCF, 0x49, 0xA6, 0xB1, 0x77, 0x93, 0x33, 0xC9, 0x28, 0x29, 0x0F, 0x72, 0x58, 0x98, 0xD4, 0x82, 0xB0,
            0xFF, 0x56, 0x1E, 0xE4, 0x70, 0x70, 0x5C, 0x2D, 0x36, 0x31, 0xCD, 0x1B, 0xEB, 0xC0, 0x88, 0x84, 0x83, 0x83, 0x83, 0x49,
            0x07, 0x46, 0xBE, 0x17, 0xEB, 0xC0, 0xF9, 0x04, 0x0E, 0x0B, 0xE7, 0xC4, 0x3A, 0x2F, 0x96, 0x8A, 0xF4, 0x58, 0x57, 0x8D,
            0x8D, 0x49, 0xA6, 0x9F, 0xC4, 0x12, 0xF2, 0x87, 0x9A, 0x99, 0x86, 0xB0, 0x9F, 0xC8, 0x9A, 0x93, 0xE8, 0x27, 0xB1, 0xA2,
            0xDA, 0x91, 0xB1, 0x94, 0x84, 0x74, 0xB1, 0x3C, 0x89, 0x65, 0x6A, 0xC2, 0x49, 0x98, 0x7B, 0xE8, 0xC6, 0xEB, 0xD9, 0xAC,
            0x68, 0x01, 0xE1, 0xE4, 0xA8, 0xEF, 0x42, 0xBD, 0x05, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10,
            0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41,
            0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x28, 0x7C, 0x8A, 0xE6, 0xFF, 0x03, 0xC7, 0xE3, 0xF1, 0x32, 0x52,
            0x85, 0xCF, 0xE7, 0x4B, 0xFB, 0xDE, 0x69, 0x2A, 0x9F, 0xA4, 0xF2, 0x31, 0x92, 0x7C, 0x9D, 0x50, 0x10, 0x0A, 0x89, 0xE9,
            0xE9, 0x69, 0xBC, 0x3E, 0x07, 0xAF, 0x57, 0xC9, 0x28, 0xB6, 0x13, 0x5C, 0x42, 0x37, 0x43, 0x0B, 0xE2, 0xA4, 0x50, 0xA0,
            0x14, 0xCD, 0xD7, 0x09, 0x33, 0x45, 0x5E, 0x3B, 0xB9, 0xDA, 0x09, 0xE9, 0x81, 0xE3, 0x2E, 0x5D, 0xBA, 0x34, 0x0A, 0xBD,
            0x0B, 0x4E, 0xAC, 0x6F, 0x56, 0x76, 0x71, 0x51, 0xC1, 0x93, 0xB3, 0x03, 0x53, 0xA5, 0xFC, 0x14, 0xDD, 0x6A, 0x49, 0x41,
            0xEC, 0x93, 0x54, 0xF7, 0x1B, 0x5B, 0xCA, 0x2B, 0x47, 0x3E, 0xCA, 0x51, 0xA6, 0x0E, 0x70, 0x09, 0x9D, 0xA7, 0x8C, 0xCE,
            0x87, 0x37, 0x53, 0xE2, 0x15, 0xAF, 0x50, 0xF2, 0x6D, 0x97, 0x94, 0xDF, 0xA8, 0x45, 0xF9, 0xEF, 0x61, 0xCB, 0x45, 0xC6,
            0xB0, 0x5D, 0x37, 0xA3, 0xD8, 0xBC, 0xE8, 0xD9, 0xBE, 0x7D, 0x7B, 0x68, 0xED, 0xDA, 0xB5, 0x61, 0xAA, 0x93, 0x6A, 0xBC,
            0xE3, 0xE3, 0xE3, 0x21, 0x08, 0xFB, 0x43, 0x43, 0x43, 0x56, 0x7D, 0x7D, 0x7D, 0xA4, 0xB9, 0xB9, 0xD9, 0xB3, 0x97, 0xDA,
            0x95, 0xCF, 0x2B, 0x8F, 0x43, 0x9C, 0x54, 0x38, 0xE5, 0x99, 0x04, 0x75, 0xDD, 0xB7, 0x6F, 0x5F, 0x43, 0x6D, 0x6D, 0xED,
            0x91, 0xB2, 0xB2, 0xB2, 0xDE, 0xF9, 0xF3, 0xE7, 0xC7, 0x02, 0x81, 0x40, 0x2F, 0xD2, 0xC8, 0xD7, 0xFF, 0x16, 0x73, 0x02,
            0x72, 0x12, 0xBC, 0xB0, 0x2E, 0x5E, 0x59, 0x59, 0xF9, 0xF3, 0x99, 0x33, 0x67, 0xB6, 0x50, 0x3A, 0x40, 0x15, 0xF4, 0x91,
            0xFC, 0xE7, 0xCF, 0x9F, 0xDF, 0x4C, 0x77, 0x6B, 0xBC, 0x00, 0x1B, 0x5D, 0x58, 0x23, 0x6F, 0xA5, 0xA4, 0xF3, 0xE4, 0xDC,
            0x65, 0xA6, 0xBF, 0x01, 0x8E, 0xA4, 0xD2, 0xBC, 0xEF, 0x0A, 0x9C, 0x83, 0x7E, 0x4C, 0xBC, 0xBA, 0x75, 0xC6, 0xF5, 0xEC,
            0x82, 0x8D, 0x89, 0xEB, 0x15, 0x02, 0x6B, 0xD6, 0xAC, 0x89, 0x2C, 0x5A, 0xB4, 0x68, 0x34, 0x18, 0x0C, 0x46, 0x1B, 0x1B,
            0x1B, 0xDB, 0xE7, 0xCD, 0x9B, 0xD7, 0x0F, 0x6D, 0xDA, 0xB4, 0xA9, 0x7D, 0xD9, 0xB2, 0x65, 0xBD, 0x28, 0xAB, 0x7F, 0xFF,
            0x7D, 0x4F, 0x5E, 0x2B, 0x5B, 0x5E, 0x4A, 0x8E, 0x9A, 0x45, 0x6C, 0x6A, 0x0C, 0xFA, 0xDD, 0xAC, 0xBF, 0x6D, 0xD9, 0x12,
            0x46, 0x1D, 0x51, 0xB7, 0x54, 0x21, 0x1F, 0xE5, 0xB0, 0xE3, 0x43, 0x8C, 0xF0, 0xFC, 0xF9, 0x73, 0x2B, 0x9B, 0xD8, 0xD4,
            0x2C, 0xE4, 0x28, 0x41, 0x38, 0x2F, 0xED, 0xC6, 0x2D, 0xCB, 0x1A, 0xEE, 0xEE, 0xEE, 0xFE, 0x98, 0xF2, 0xAA, 0x2F, 0x5E,
            0xBC, 0xD8, 0x58, 0x55, 0x55, 0x35, 0x88, 0x7C, 0x72, 0xE2, 0xBB, 0x94, 0x67, 0xE4, 0xEB, 0x84, 0xEC, 0x18, 0x33, 0x9C,
            0x26, 0x55, 0xB0, 0xB3, 0xDB, 0xF2, 0xBE, 0x2B, 0xEC, 0xE7, 0xCB, 0x26, 0x13, 0xD7, 0x2B, 0x04, 0xA8, 0x1E, 0x56, 0x4D,
            0x4D, 0x4D, 0x97, 0x53, 0x63, 0x86, 0xE0, 0xC4, 0xFF, 0xA3, 0x48, 0xCC, 0xE6, 0xC6, 0x70, 0x72, 0xD6, 0x74, 0xE2, 0x43,
            0x8C, 0xF0, 0xE9, 0x67, 0x9F, 0x35, 0xE8, 0xBA, 0x2D, 0x5E, 0xBC, 0x38, 0xD6, 0x1C, 0x0E, 0x47, 0xFE, 0xD1, 0xDC, 0x1C,
            0xF9, 0xCB, 0x07, 0x1F, 0x44, 0xED, 0xF5, 0x86, 0x1D, 0x1F, 0xE2, 0x1A, 0xF2, 0x13, 0xBC, 0xE5, 0x73, 0x46, 0x1B, 0x4A,
            0x15, 0xDB, 0x99, 0x85, 0x7E, 0x60, 0x3F, 0x22, 0x2F, 0x9C, 0x17, 0xC9, 0x25, 0x4B, 0x96, 0x8C, 0x9C, 0x38, 0x71, 0x62,
            0x7F, 0x75, 0x75, 0xF5, 0x4F, 0x48, 0xD3, 0x45, 0x07, 0x2E, 0x5D, 0xBA, 0xF4, 0x21, 0x39, 0xB0, 0x91, 0x31, 0xA8, 0xDD,
            0x89, 0xE8, 0x9C, 0xF8, 0x94, 0x4A, 0xB2, 0xDB, 0x8A, 0xB4, 0xBD, 0xCC, 0x9E, 0xE6, 0xE3, 0x5C, 0xC1, 0xE7, 0x48, 0x9E,
            0xCF, 0x49, 0xF6, 0x72, 0xDA, 0x16, 0x35, 0x54, 0x07, 0x35, 0xEE, 0x43, 0xB4, 0xD5, 0x0D, 0x7A, 0xD7, 0xAE, 0x5D, 0xE1,
            0xBF, 0xEF, 0xDC, 0x19, 0xC6, 0x3E, 0xF2, 0x10, 0x95, 0xB5, 0x1D, 0x1F, 0x66, 0x04, 0x27, 0x47, 0x4D, 0x27, 0x3E, 0xC4,
            0x35, 0xA8, 0xC3, 0xF2, 0xE5, 0xCB, 0x95, 0xA3, 0x2E, 0x58, 0xB0, 0xA0, 0xBF, 0xF3, 0xEB, 0xCE, 0x4F, 0x56, 0xAE, 0x5C,
            0x19, 0xF5, 0xFB, 0xFD, 0xB1, 0x4F, 0x0F, 0x1C, 0x08, 0xDB, 0x6F, 0x64, 0xB0, 0x33, 0x55, 0x67, 0x44, 0xD7, 0x15, 0x2B,
            0x56, 0xE0, 0x5D, 0xDB, 0xAA, 0xED, 0x38, 0x09, 0xE5, 0x9E, 0x44, 0x61, 0xAA, 0x04, 0xC6, 0xBA, 0x01, 0x44, 0x5E, 0x38,
            0x2F, 0xB2, 0x7C, 0x3E, 0xDF, 0x34, 0xB6, 0xE4, 0xC4, 0x83, 0x97, 0x2F, 0x5F, 0xDE, 0x4C, 0x36, 0xC9, 0xB1, 0xB1, 0x5B,
            0xE8, 0x3C, 0x69, 0x9D, 0xC4, 0x5E, 0x96, 0xAA, 0x54, 0xDB, 0x3F, 0x83, 0xFD, 0xFC, 0x4E, 0xE7, 0xCB, 0x56, 0x5E, 0x4C,
            0xD0, 0xDF, 0x6F, 0xD1, 0xEF, 0x19, 0xA5, 0xDE, 0x55, 0xAF, 0xEE, 0x4E, 0xEE, 0x6C, 0x6D, 0x55, 0x5D, 0x47, 0x08, 0x8E,
            0x8C, 0x3C, 0x2E, 0x8B, 0x55, 0x54, 0x56, 0x76, 0x21, 0x9F, 0x0F, 0x37, 0x86, 0x93, 0xC3, 0x6A, 0xB1, 0x89, 0x31, 0xF0,
            0xF7, 0xC3, 0x71, 0x51, 0x2F, 0xDC, 0xA0, 0x36, 0x6C, 0xD8, 0x70, 0x04, 0xFB, 0x50, 0x43, 0x43, 0x43, 0xBB, 0xBE, 0x91,
            0x41, 0xB0, 0x33, 0x59, 0x5F, 0x38, 0x27, 0x05, 0x3B, 0x47, 0x27, 0x46, 0xFE, 0x6C, 0x9D, 0x37, 0xE7, 0x49, 0x2C, 0x72,
            0x56, 0x38, 0xEC, 0x44, 0x4B, 0x4B, 0xCB, 0x0F, 0x9D, 0x9D, 0x9D, 0x5F, 0x20, 0x4D, 0xD0, 0xC6, 0x17, 0x3F, 0x7A, 0xF4,
            0x68, 0x47, 0x53, 0x53, 0x93, 0xFA, 0x3A, 0x21, 0xD2, 0x89, 0x23, 0xF2, 0x03, 0xFD, 0x4D, 0xF2, 0x75, 0x42, 0x21, 0x2B,
            0x14, 0x8C, 0xD4, 0x76, 0x6A, 0x6A, 0x2A, 0x74, 0xFB, 0xF6, 0x6D, 0x35, 0xB6, 0xA7, 0x08, 0x7C, 0xF3, 0x9F, 0x6D, 0x6D,
            0xDF, 0xDD, 0xBA, 0x75, 0x2B, 0xF9, 0x75, 0x42, 0x6D, 0x67, 0x0A, 0xBA, 0x51, 0xFE, 0x72, 0xEC, 0xD8, 0xB1, 0xFD, 0xD4,
            0x93, 0x7D, 0xE3, 0xEB, 0x84, 0x48, 0x23, 0x1F, 0xE5, 0x9C, 0x65, 0x16, 0x38, 0x2B, 0xC9, 0x8F, 0x31, 0xAF, 0xEE, 0x36,
            0x93, 0xB3, 0xAA, 0x08, 0x8C, 0x88, 0xCC, 0x63, 0x62, 0x35, 0xB1, 0xA5, 0x0E, 0x70, 0x09, 0x9D, 0x27, 0x19, 0xE5, 0xD0,
            0x45, 0x46, 0x5A, 0x0B, 0x69, 0xA7, 0x32, 0xDA, 0xC7, 0x84, 0x96, 0xEB, 0x4F, 0xAD, 0xE0, 0x5C, 0xD8, 0x24, 0x76, 0xFF,
            0xB8, 0xAE, 0x5D, 0xF6, 0x72, 0xDA, 0x1A, 0x83, 0xEA, 0xB0, 0x2A, 0x17, 0xB1, 0xB9, 0x11, 0xA8, 0x0E, 0x8E, 0x5D, 0x68,
            0x44, 0x62, 0xDA, 0x57, 0x13, 0x58, 0x5E, 0x75, 0xA1, 0x35, 0x4E, 0x91, 0x57, 0x8B, 0x4D, 0x8C, 0x81, 0x3A, 0xE8, 0x08,
            0xAC, 0x85, 0x3A, 0x4F, 0xC6, 0xE3, 0x21, 0x6A, 0xDB, 0x6F, 0xCC, 0x03, 0x98, 0x8E, 0xC0, 0x9A, 0x53, 0xA7, 0x4E, 0x85,
            0x02, 0x81, 0x80, 0x9A, 0xF8, 0xC5, 0x16, 0xE9, 0x44, 0x89, 0x47, 0x50, 0xA3, 0x29, 0xC7, 0x6C, 0x33, 0x85, 0x79, 0x35,
            0x61, 0x85, 0x6E, 0xF3, 0xC9, 0x93, 0x27, 0x23, 0xBA, 0x3B, 0x8D, 0xB1, 0x31, 0xC6, 0xC8, 0x54, 0x59, 0x53, 0xCB, 0x48,
            0x49, 0x27, 0xC9, 0x24, 0xB6, 0x33, 0x4A, 0xAE, 0xD7, 0x86, 0x4C, 0x5E, 0x1F, 0x8E, 0x99, 0xEB, 0xEC, 0xB7, 0x17, 0x4E,
            0x8C, 0x19, 0x68, 0x7B, 0xE3, 0xB5, 0xCB, 0xAB, 0x49, 0x2C, 0x8D, 0x93, 0xE3, 0x6A, 0xB1, 0x89, 0x31, 0x50, 0x57, 0x3D,
            0x06, 0xD6, 0x42, 0xF7, 0x75, 0x65, 0x5D, 0xDD, 0x8C, 0x49, 0x3C, 0x93, 0x63, 0xE0, 0x54, 0x0E, 0x1F, 0x3E, 0xDC, 0x40,
            0x7E, 0x13, 0xC3, 0x96, 0xB3, 0xBC, 0x83, 0x1A, 0x4C, 0x50, 0x2F, 0x15, 0x61, 0xC2, 0x0A, 0x63, 0x5E, 0xCA, 0xAB, 0x42,
            0xE4, 0xD5, 0x13, 0x5B, 0x98, 0xA5, 0x86, 0x9D, 0x3A, 0xC0, 0x25, 0x74, 0x9E, 0xBC, 0x3D, 0x79, 0x05, 0xA7, 0xCC, 0xC7,
            0x32, 0x12, 0xD5, 0x25, 0x6F, 0x0E, 0x8C, 0x35, 0x60, 0x34, 0x58, 0x38, 0x31, 0x22, 0x31, 0xC6, 0xBC, 0xD0, 0xC6, 0x8D,
            0x1B, 0xDB, 0xE9, 0xF7, 0xF5, 0x74, 0x19, 0x09, 0x38, 0x39, 0xAE, 0x16, 0x9B, 0x18, 0xC5, 0x3E, 0x0B, 0x0D, 0x61, 0xF2,
            0xC8, 0xC9, 0x81, 0x4D, 0xCE, 0x42, 0xE7, 0x15, 0x6A, 0x30, 0x6A, 0x1D, 0x18, 0x4B, 0x45, 0x98, 0x6D, 0xA6, 0x86, 0xAB,
            0x26, 0xAC, 0x28, 0x3F, 0x80, 0xC8, 0xAB, 0x97, 0x98, 0xD8, 0xCE, 0x35, 0x74, 0xEE, 0xD4, 0x07, 0x39, 0x66, 0x88, 0xCB,
            0x3D, 0xF9, 0x3A, 0x61, 0xEA, 0xB5, 0xD2, 0x89, 0xCD, 0x8D, 0x41, 0x75, 0x72, 0xEC, 0x32, 0xA7, 0x8A, 0xCD, 0x8D, 0x61,
            0x7F, 0x90, 0x03, 0xC2, 0xA4, 0x16, 0x84, 0xFD, 0xB7, 0xF2, 0x20, 0x87, 0x83, 0xE3, 0x6A, 0xB1, 0x89, 0x51, 0x50, 0x2F,
            0xFB, 0x3A, 0x30, 0x22, 0xE1, 0xEA, 0xD5, 0xAB, 0x93, 0x0E, 0x8C, 0x7C, 0x2F, 0xD6, 0x81, 0xF3, 0x06, 0x55, 0x44, 0x3F,
            0x89, 0x55, 0x43, 0x2A, 0xA7, 0x74, 0xF2, 0x49, 0x2C, 0x2E, 0x33, 0xFA, 0x24, 0x96, 0x90, 0x3F, 0xE8, 0x37, 0x54, 0x33,
            0xD3, 0x10, 0xF6, 0x39, 0x7B, 0xCE, 0x81, 0xBA, 0xE1, 0x89, 0x2B, 0xF4, 0x3A, 0xB4, 0x23, 0x63, 0x29, 0x09, 0xE9, 0x62,
            0x79, 0x12, 0xAB, 0x68, 0xFE, 0x37, 0x92, 0xF0, 0x76, 0xD1, 0x8D, 0xD7, 0xE7, 0xF3, 0xCD, 0xF9, 0xAF, 0xF4, 0x39, 0x39,
            0xEA, 0xBB, 0x50, 0x6F, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x41,
            0x10, 0x04, 0x41, 0x10, 0x04, 0xA1, 0xD0, 0x28, 0x29, 0xF9, 0x1D, 0xE9, 0xF2, 0x33, 0xCD, 0xE6, 0xBA, 0x4C, 0xAC, 0x00,
            0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
        };
        const int size = sizeof(data) / sizeof(data[0]);
        hImg = _img_create_frommemory(data, size);
    }
    return hImg;
}



bool CDrawWindow::BindWindow(const DRAW_WINDOW_ARGUMENT* arg)
{
    if (!m_data)
    {
        m_data = CDrawWindow::BindWindow_s(arg);
        return m_data != 0;
    }
    return false;
}




LPCANVAS CDrawWindow::SetCanvas(LPCANVAS hCanvas)
{
    if (m_data)
    {
        LPCANVAS ret = m_data->hCanvas;
        m_data->hCanvas = hCanvas;
        return ret;
    }
    return 0;
}

HWND CDrawWindow::create(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, const DRAW_WINDOW_ARGUMENT* arg)
{
    if (!m_data)
        return CDrawWindow::create_s(dwExStyle, lpWindowName, dwStyle, x, y, cx, cy, hWndParent, arg);
    return 0;
}


bool CDrawWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet)
{
    if (m_data)
        return WindowProc(m_data->hWnd, message, wParam, lParam, oldProc, pRet);
    return false;
}


bool CDrawWindow::OnPaint(const PAINTSTRUCT& ps, LPCANVAS hCanvas)
{
    if (m_data)
        return draw_OnPaint(m_data->hWnd, ps, hCanvas);
    return false;
}

bool CDrawWindow::DrawSysIcon(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcIcon)
{
    if (m_data)
        return CDrawWindow::DrawSysIcon(m_data, ps, hCanvas, (RECT_F&)rcIcon);
    return false;
}

bool CDrawWindow::DrawTitle(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcTitle)
{
    if (m_data)
        return CDrawWindow::DrawTitle(m_data, ps, hCanvas, (RECT_F&)rcTitle);
    return false;
}



bool CDrawWindow::DrawSysButton(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcBtn)
{
    if (m_data)
        return DrawSysButton(m_data, ps, hCanvas, (RECT_F&)rcBtn);
    return false;
}

bool CDrawWindow::EventDraw(PFN_SysDraw pfn, LPARAM param)
{
    return EventDraw(m_data, pfn, param);
}

bool CDrawWindow::EventDraw(HWND hWnd, PFN_SysDraw pfn, LPARAM param)
{
    return EventDraw(draw_GetSysButtonData(hWnd), pfn, param);
}

bool CDrawWindow::EventDraw(LPDRAW_WINDOW_DATA data, PFN_SysDraw pfn, LPARAM param)
{
    if (!data)
        return false;
    data->pfnDraw.pfn = pfn;
    data->pfnDraw.param = param;
    return true;
}

bool CDrawWindow::EventBtnClick(PFN_SysBtnClick pfn, LPARAM param)
{
    return EventBtnClick(m_data, pfn, param);
}

bool CDrawWindow::EventBtnClick(HWND hWnd, PFN_SysBtnClick pfn, LPARAM param)
{
    return EventBtnClick(draw_GetSysButtonData(hWnd), pfn, param);
}

bool CDrawWindow::EventBtnClick(LPDRAW_WINDOW_DATA data, PFN_SysBtnClick pfn, LPARAM param)
{
    if (!data)
        return false;
    data->pfnSysBtnClick.pfn = pfn;
    data->pfnSysBtnClick.param = param;
    return true;
}

DRAW_WINDOW_ARGUMENT CDrawWindow::GetDefArg(HWND hWnd)
{    
    DRAW_WINDOW_ARGUMENT arg;
    memset(&arg, 0, sizeof(arg));
        
    arg.hWnd            = hWnd;
    arg.sysbtnHeight    = 30;
    arg.sysbtnWidth     = 30;
    arg.isSubClass      = true;
    arg.isDrawIcon      = true;
    arg.isUnicode       = IsWindowUnicode(hWnd);

    arg.crNormal        = RGB2ARGB(RGB(45, 45, 48), 255);
    arg.crHot           = RGB2ARGB(RGB(80, 80, 83), 255);
    arg.crDown          = RGB2ARGB(RGB(65, 65, 68), 255);
    arg.crBack          = RGB2ARGB(RGB(37, 37, 38), 255);
    arg.crShadow        = RGB2ARGB(RGB(0, 122, 204), 255);
    arg.crShadow_nf     = RGB2ARGB(RGB(66, 66, 69), 255);
    //arg.crBorder        = RGB2ARGB(RGB(128, 128, 128), 255);
    arg.crBorder        = RGB2ARGB(RGB(0, 122, 204), 255);
    arg.crBorder_nf     = RGB2ARGB(RGB(66, 66, 69), 255);
    arg.crTitle         = RGB2ARGB(RGB(241, 241, 241), 255);
    arg.crTitle_nf      = RGB2ARGB(RGB(137, 137, 138), 255);

    return arg;
    
}


