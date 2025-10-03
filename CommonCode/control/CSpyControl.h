#pragma once
#include "WndBase.h"
typedef int(CALLBACK* PFN_SpyControlDown)(HWND hWnd, BOOL isRDown, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlUp)(HWND hWnd, BOOL isRUp, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlMove)(HWND hWnd, WPARAM wParam, LPARAM lParam, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlDraw)(HWND hWnd, HDC hdc, int cxClient, int cyClient, STATE state, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlLeave)(HWND hWnd, LPARAM param);
typedef int(CALLBACK* PFN_SpyControlHover)(HWND hWnd, LPARAM param);

class CSpyControl : public CControlBase
{
private:
// ��ѯ _flags �Ƿ���� _flag
// _flags = ����ѯ��ֵ
// _flag = ��ѯ��ֵ
#define __query(_flags, _flag)    (((_flags) & (_flag)) == (_flag))

// �� _flag ��ӵ� _flags ��
// _flags = ��Ӻ󱣴��ֵ
// _flag = Ҫ��ӵ�ֵ
#define __add_state(_flags, _flag)      _flags = (STATE)((_flags) | (_flag))

// �� _flag �� _flags ��ɾ��
// _flags = ɾ���󱣴��ֵ
// _flag = Ҫɾ����ֵ
#define __del_state(_flags, _flag)      _flags = (STATE)(_flags & ~(_flag))

    typedef struct SPYCONTROL_DATA 
    {
        STATE       state;          // ��ǰ�����״̬
        HCURSOR     hCursor;        // ʹ�õ����ָ��, ����״̬, Ĭ������
        HCURSOR     hCursorDown;    // ʹ�õ����ָ��, ����״̬, ���µ�ʱ���������ͼ��, ���ָ�������ͼ��, Ĭ��������


        PFN_SpyControlDown  pfnDown;        // �����¼�
        LPARAM              nDownParam;     // �����¼�����
        PFN_SpyControlMove  pfnMove;        // �ƶ��¼�
        LPARAM              nMoveParam;     // �ƶ��¼�����
        PFN_SpyControlUp    pfnUp;          // �ſ��¼�
        LPARAM              nUpParam;       // �ſ��¼�����
        PFN_SpyControlDraw  pfnDraw;        // �滭�¼�, �����Ƿ�����
        LPARAM              nDrawParam;     // �滭�¼�����
        PFN_SpyControlLeave pfnLeave;       // ����뿪�¼�, �����Ƿ�����
        LPARAM              nLeaveParam;    // ����뿪�¼�����
        PFN_SpyControlHover pfnHover;       // �������¼�, �����Ƿ�����
        LPARAM              nHoverParam;    // �������¼�����

    }*LPSPYCONTROL_DATA;

    LPSPYCONTROL_DATA m_data;
protected:
    virtual inline LPCWSTR SuperClassName() { return L"static"; }

public:
    CSpyControl() : m_data(0), CControlBase() { ; }
    virtual ~CSpyControl() { ; }
public:
    virtual inline LPCWSTR className() { return L"spyWindowControl"; }
    virtual HWND create(int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0)
    {
        DWORD dwStyle = WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | SS_NOTIFY;
        DWORD dwExStyle = 0;
        return CWndBase::create(dwExStyle, 0, dwStyle, x, y, nWidth, nHeight, hWndParent, id, lpParam);
    }
    virtual LONG_PTR SetCursor(HCURSOR hCursor)
    {
        LPSPYCONTROL_DATA pData = GetData();
        LONG_PTR ret = 0;
        if (pData)
        {
            ret = (LONG_PTR)pData->hCursor;
            pData->hCursor = hCursor;
        }
        return ret;
    }
    virtual LONG_PTR SetCursorDown(HCURSOR hCursor)
    {
        LPSPYCONTROL_DATA pData = GetData();
        LONG_PTR ret = 0;
        if (pData)
        {
            ret = (LONG_PTR)pData->hCursorDown;
            pData->hCursorDown = hCursor;
        }
        return ret;
    }

#define _FUN_DEF(_d)    \
    inline void Event_##_d(PFN_SpyControl##_d pfn, LPARAM param = 0){\
        LPSPYCONTROL_DATA pData = GetData();\
        if (pData){\
            pData->pfn##_d = pfn;\
            pData->n##_d##Param = param;\
        }\
    }
    _FUN_DEF(Down)
    _FUN_DEF(Up)
    _FUN_DEF(Move)
    _FUN_DEF(Draw)
    _FUN_DEF(Leave)
    _FUN_DEF(Hover)


private:
    inline void FreeData()
    {
        if (m_data)
            delete m_data;
        m_data = 0;
    }
    inline LPSPYCONTROL_DATA GetData()
    {
        if (!m_data)
        {
            m_data = new SPYCONTROL_DATA;
            memset(m_data, 0, sizeof(SPYCONTROL_DATA));
            m_data->hCursor = LoadCursorW(0, IDC_HAND);
            m_data->hCursorDown = LoadCursorW(0, IDC_HAND);
        }
        return m_data;
    }
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LPSPYCONTROL_DATA pData = GetData();
        switch (message)
        {
        case WM_ENABLE:
            InvalidateRect();
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        case WM_MOUSEMOVE:
            return _move_(oldProc, hWnd, message, wParam, lParam, pData);
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
            return _leave_(oldProc, hWnd, message, wParam, lParam, pData);
        case WM_MOUSEHOVER:
        case WM_NCMOUSEHOVER:
        {
            break;
        }
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            return _down_(oldProc, hWnd, message, wParam, lParam, pData);

            // ����ſ����Ҽ��ſ���ֻ���ͷ����, ���ͷ������¼�ȥ����ſ�
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        {
            ::ReleaseCapture();
            __del_state(pData->state, STATE::DOWN); // ɾ������״̬
            ::InvalidateRect(hWnd, 0, 0);
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_CAPTURECHANGED:
            return _capture_release_(oldProc, hWnd, message, wParam, lParam, pData);
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            FreeData();
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        case WM_PAINT:
        {
            RECT rc;
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rc);
            const int cxClient = rc.right - rc.left;
            const int cyClient = rc.bottom - rc.top;
            LPMEMDCBITMAP md = md_create(cxClient, cyClient);
            HDC hdc = md->hdc;

            if (!pData->pfnDraw || pData->pfnDraw(hWnd, hdc, cxClient, cyClient, pData->state, pData->nDrawParam) == 0)
            {
                HBRUSH hbrBack = GetStockBrush(WHITE_BRUSH);
                HGDIOBJ hbrOld = SelectObject(hdc, hbrBack);
                HCURSOR hCursor = __query(pData->state, STATE::DOWN) ? 0 : pData->hCursor;

                Rectangle(hdc, 0, 0, cxClient, cyClient);



                MoveToEx(hdc, 0, cyClient / 2, 0);
                LineTo(hdc, cxClient, cyClient / 2);
                MoveToEx(hdc, cxClient / 2, 0, 0);
                LineTo(hdc, cxClient / 2, cyClient);

                if (hCursor)    // �����ָ��ͻ滭, û�оͲ�����
                    DrawIconEx(hdc, 2, 2, hCursor, 0, 0, 0, 0, DI_NORMAL | DI_DEFAULTSIZE);
                SelectObject(hdc, hbrOld);
            }

            BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
            return 0;
        }
        default:
            break;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    inline static LRESULT _move_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        if (!pData->pfnMove || pData->pfnMove(hWnd, wParam, lParam, pData->nMoveParam) == 0)
        {
            // �ڲ�����Ĵ���
        }

        if (!__query(pData->state, STATE::HOVER))  // û�н���״̬�ͼ���һ������״̬
        {
            __add_state(pData->state, STATE::HOVER | STATE::HOT);    // ��ӽ�����ȵ�״̬

            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize      = sizeof(tme);
            tme.dwFlags     = TME_LEAVE | TME_HOVER;
            tme.dwHoverTime = 500;    // ������೤ʱ��󴥷������¼�,��λΪ����,HOVER_DEFAULTΪϵͳĬ��
            tme.hwndTrack   = hWnd;
            TrackMouseEvent(&tme);
            ::InvalidateRect(hWnd, 0, 0);

            // �������ʱ����������ָ��Ϊ����, ��갴�µ�ʱ������Ϊ����
            SetClassLongPtrW(hWnd, GCLP_HCURSOR, (LONG_PTR)pData->hCursor);

            if (!pData->pfnHover || pData->pfnHover(hWnd, pData->nHoverParam) == 0)
            {

            }
            
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    inline static LRESULT _leave_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        __del_state(pData->state, STATE::HOVER | STATE::HOT);   // ����뿪, ɾ��������ȵ�״̬
        ::InvalidateRect(hWnd, 0, 0);
        SetClassLongPtrW(hWnd, GCLP_HCURSOR, (LONG_PTR)pData->hCursor); // �������ָ��Ϊ����

        if (!pData->pfnLeave || pData->pfnLeave(hWnd, pData->nLeaveParam) == 0)
        {

        }

        return 0;
    }

    // ���, �Ҽ�ִ�е����̶�һ��
    inline static LRESULT _down_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        __add_state(pData->state, STATE::DOWN); // ��Ӱ���״̬
        const int isRDown = (message == WM_RBUTTONDOWN ? 1 : 0);
        ::SetCursor(pData->hCursorDown);    // �������ָ��Ϊ����
        ::SetCapture(hWnd);                 // �������


        if (isRDown)
            __add_state(pData->state, BUSYING);
        else
            __del_state(pData->state, BUSYING);

        if (!pData->pfnDown || pData->pfnDown(hWnd, isRDown, pData->nDownParam) == 0)
        {

        }
        ::InvalidateRect(hWnd, 0, 0);       // �ػ����
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    // ������ͷ��¼�����, ���ſ���Ϣֻ�����ͷ����, ���ſ��¼��п��ܻ���ղ���, �ͷ���Ϣһ�㲻��©
    inline static LRESULT _capture_release_(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPSPYCONTROL_DATA pData)
    {
        if (!pData->pfnUp || pData->pfnUp(hWnd, __query(pData->state, BUSYING), pData->nUpParam) == 0)
        {

        }
        if (__query(pData->state, STATE::DOWN))
            ::InvalidateRect(hWnd, 0, 0);       // �а���״̬���ػ���ť, û�оͲ��ػ�
        __del_state(pData->state, STATE::DOWN); // ɾ������״̬
        __del_state(pData->state, BUSYING);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

};