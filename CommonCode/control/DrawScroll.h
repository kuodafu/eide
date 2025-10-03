#pragma once
#include <windows.h>
#include "CControlDraw.h"
#include "src/scroll.h"
#include <commctrl.h>


// ��ʼ���Ի������
bool IDrawScroll_Init();
// ȡ����ʼ���Ի������
bool IDrawScroll_UnInit();
class IDrawScroll;

enum ISCROLL_TYPE
{
    ISCROLL_DRAWTYPE_BACK       = 0x00, // �滭����, ������յ���dc���ڴ�dc
    ISCROLL_DRAWTYPE_BTN1       = 0x01, // �滭�������İ�ť1, ������յ���dc���ڴ�dc
    ISCROLL_DRAWTYPE_BTN2       = 0x02, // �滭�������İ�2, ������յ���dc���ڴ�dc
    ISCROLL_DRAWTYPE_THUMB      = 0x03, // �滭�������Ļ���, ������յ���dc���ڴ�dc
    ISCROLL_DRAWTYPE_BEFOR      = 0x04, // �滭��ʼ, ��ʱû�д�������¼�
    ISCROLL_DRAWTYPE_ALERT      = 0x05, // �滭����, ������յ���HDC�Ǵ��ڵ�hdc, ���յ���λ���Ǽ�������п�����Ҫ�滭��λ��, ��������¼���ʱ��������Ѿ��滭�����dc����
    ISCROLL_DRAWTYPE_BORDER     = 0x06, // �滭�߿�, ������յ���HDC�Ǵ��ڵ�hdc, ���յ���λ���Ǵ��ڱ߿����, ��������¼���ʱ��������Ѿ��滭�����dc����

    ISCROLL_GETRECT_SCREEN      = 0x10, // �������������Ļ�е�λ��
    ISCROLL_GETRECT_CLIENT      = 0x11, // ����������ڴ����е�λ��
    ISCROLL_GETRECT_BORDER      = 0x12, // ���㴰�ڷǿͻ���λ��, �ڲ��Ǹ������λ�ü���������滭λ�õ�, ������������ṹ�û滭λ�ý��иı�
};
typedef bool (CALLBACK* pfn_IScroll_CalcScroll)(IDrawScroll* pObj, RECT* prc);
typedef bool (CALLBACK* pfn_IScroll_DrawScroll)(IDrawScroll* pObj, bool isVScroll, HDC hdc, Gdiplus::Graphics* gp, ISCROLL_TYPE type, RECT_F* rcDraw);
typedef bool (CALLBACK* pfn_IScroll_GetScrollRect)(IDrawScroll* pObj, bool isVScroll, ISCROLL_TYPE type, RECT* prc);

// �Ѵ��ڷǿͻ����ճ�һ��ط����ƹ�����
class IDrawScroll
{
    // ��ťλ��, ���ػ�ʱ��¼
    struct DRAWSCROLL_RECT
    {
        RECT thumb; // �����λ��
        RECT btn1;  // ��ť1��λ��
        RECT btn2;  // ��ť2��λ��
        RECT rc;    // ��������λ��
        float maxBlockHeight;   // ������������, �������ߴ� - ������ť�ĳߴ�
        float blockHeight;      // ����ĸ߶�
        float maxPos;           // ���λ�� - ҳ���� + 1
    };


public:
    void clear();
    MEMDCBITMAP m_md;   // ��������dc
    MEMDCBITMAP m_vmd;  // ���������dc
    MEMDCBITMAP m_hmd;  // ���������dc

    UINT m_dpi;         // DPI����, �������ֵ��������

    pfn_IScroll_CalcScroll m_pfn_CalcScroll;
    pfn_IScroll_DrawScroll m_pfn_DrawScroll;
    pfn_IScroll_GetScrollRect m_pfn_GetScrollRect;

    DRAWSCROLL_RECT m_vrc;      // ���������λ��, ���ػ���ʱ���¼
    DRAWSCROLL_RECT m_hrc;      // ���������λ��, ���ػ���ʱ���¼
    bool m_lDownIsVscroll;      // ������µ��Ƿ�Ϊ���������
    int  m_isCapture;           // �Ƿ񲶻������, 1=���������, 2=���������, 0=δ����

    int m_VState;               // �����������ǰ״̬, ��λ��״̬, ��λ���ĸ���ť, 1=��ť1, 2=��ť2, 3=����, ����=û��
    int m_HState;               // �����������ǰ״̬, ��λ��״̬, ��λ���ĸ���ť, 1=��ť1, 2=��ť2, 3=����, ����=û��
    POINT m_downOffset;         // �������ʱ����ڻ������λ��, ����ʹ��x, ����ʹ��y
    int m_downPos;              // ����ʱ��������λ��
    POINT m_downPoint;          // ��갴��ʱ��λ��

    HPEN m_hPen;
    int m_nBorderWidth;         // �߿���, �еľ�Ϊ1, û�о�Ϊ0


public:

    HWND m_hWnd;                // �󶨵Ĵ��ھ��
    HWND m_hWndVBind;           // �󶨵�������������
    HWND m_hWndHBind;           // �󶨵ĺ�����������

    bool m_isVScroll;           // �Ƿ������������
    bool m_isHScroll;           // �Ƿ��к��������
    bool m_isCallSetScroll;     // �Ƿ����ԭ����
    bool m_isAbsScroll;         // ���Ի���������
    bool m_isControl;           // �Ƿ�Ϊ�������ؼ�
    bool m_isBind;              // �Ƿ�Ϊ�󶨵����
    bool m_isSleep;             // �Ƿ���ʱ����

    SCROLLINFO m_vScroll;       // �����������Ϣ
    SCROLLINFO m_hScroll;       // �����������Ϣ

    SCROLL_RECT_INFO m_vInfo;   // ���������λ��
    SCROLL_RECT_INFO m_hInfo;   // ���������λ��
    Gdiplus::Bitmap* m_VBmp;    // ���������λͼ, λ����Ϣ���� m_vInfo ��¼
    Gdiplus::Bitmap* m_HBmp;    // ���������λͼ, λ����Ϣ���� m_hInfo ��¼
    int m_PageDown;             // ʱ����������ҳ���ʶ, SendMessage��wParam����
    int m_isSysListView;        // �Ƿ�Ϊ�����б��, ������Ҫ���⴦��, Ϊ0��ʾ���ǳ���, ����Ϊ����߶�
    int m_isSysTreeView;        // �Ƿ�Ϊ���Ϳ�, ���Ϳ���Ҫ������λ���ڷ�����Ϣ

    POINT m_thumbPoint;         // �����ƶ�ʱ�������

    //int m_vThumbSize;           // �������������������
    //int m_hThumbSize;           // �����������������߶�

public:
    // �������ʱ�����ʼ��������
    // �߿���ɫ, �����б߿�ʱ�滭, �ڲ��ᴴ��һ������
    IDrawScroll(COLORREF crBorder, SCROLL_RECT_INFO& info);
    ~IDrawScroll();

    inline void OnCalcScroll(pfn_IScroll_CalcScroll callback = 0) { m_pfn_CalcScroll = callback; }
    inline void OnDrawScroll(pfn_IScroll_DrawScroll callback = 0) { m_pfn_DrawScroll = callback; }
    inline void OnGetScrollRect(pfn_IScroll_GetScrollRect callback = 0) { m_pfn_GetScrollRect = callback; }
    void SetBorderColor(COLORREF clrBorder);
public:

    static bool IsBindControl(HWND hWnd, bool* isVScroll);
    static void SetBindControl(HWND hWnd, bool isBind, bool isVScroll);
    static IDrawScroll* GetobjFromHwnd(HWND hWnd);
    static IDrawScroll* SetobjFromHwnd(HWND hWnd, IDrawScroll* pScroll);
    // �󶨴���, �󶨺��ڲ��ᴦ���������Ϣ
    // hWnd = Ҫ�󶨵Ĵ���
    // isSubClass = �Ƿ����໯, ���Ϊtrue, �ڲ������໯�������, ���Ϊ��, ������ⲿ����Ϣ���ݵ� OnWndProc ������
    // isReCalcSize = �Ƿ����¼���ߴ�, Ϊtrue���ڲ������ SetWindowPos �������¼���ǿͻ���
    bool bind(HWND hWnd, bool isSubClass = false, bool isReCalcSize = false);

    
    // ���Խ�����������Ϣ��Ͷ�ݽ���, �ڲ�������Ҫ�������Ϣ, �����Ƿ��Ѵ���, ����ֵ��pRet
    bool OnWndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    void InvalidateRect() {
        if (m_isControl)
            ::InvalidateRect(m_hWnd, 0, 0);
        else
            OnNcPaint(0, m_hWnd, WM_NCPAINT, 0, 0, 0);
    }

    // ȡ���������λ��, ������Ļ����
    bool GetVScrollRect(RECT* rcScreen = 0, RECT* rcClient = 0, RECT* rcBtn1 = 0, RECT* rcBtn2 = 0, RECT* rcThumb = 0, RECT* rcWindow = 0, RECT* rcWindowClient = 0, RECT* rcBorder = 0);

    // ȡ���������λ��, ������Ļ����
    bool GetHScrollRect(RECT* rcScreen = 0, RECT* rcClient = 0, RECT* rcBtn1 = 0, RECT* rcBtn2 = 0, RECT* rcThumb = 0, RECT* rcWindow = 0, RECT* rcWindowClient = 0, RECT* rcBorder = 0);


    inline bool IsState(int state, bool isVScroll) { return ((GetState(isVScroll)) & state) == state; }
    inline void SetState(int state, bool isVScroll) {
        int i = MAKELONG(state, HIWORD(isVScroll ? m_VState : m_HState));
        if (isVScroll) m_VState = i;
        else m_HState = i;
    }
    inline int GetState(bool isVScroll) { return LOWORD(isVScroll ? m_VState : m_HState); }
    inline void SetIndex(int index, bool isVScroll) {
        int i = MAKELONG(LOWORD(isVScroll ? m_VState : m_HState), index);
        if (isVScroll) m_VState = i;
        else m_HState = i;
    }
    inline int GetIndex(bool isVScroll) { return HIWORD(isVScroll ? m_VState : m_HState); }

    bool OnDrawScroll(HDC hdcWindow);
private:

    // ��������ĸ���ť��λ��
    int GetVHittestPos(const POINT& pt);
    int GetHHittestPos(const POINT& pt);
    
    bool OnNcHittest(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcMouseMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcCalcSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnPrint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcLbuttonDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnCaptureChanged(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcLbuttonUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    // ��갴�»�����ƶ����, û�а��»���Ļ����ᴥ��
    bool OnThumbMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcLbuttonDblclk(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcRbuttonDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcRbuttonUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnNcRbuttonDblclk(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);

    bool OnHScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
    bool OnVScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet);



    // �������������, ����һ��hdc�ͻ����������, �ڲ��ѽ���滭�����dc��
    // cxClient = ������������
    // cyClient = ����������߶�
    void DrawVScroll(HDC hdc, int cxClient, int cyClient, const RECT& rcScroll);

    // ���ƺ��������, ����һ��hdc�ͻ����������, �ڲ��ѽ���滭�����dc��
    // cxClient = ������������
    // cyClient = ����������߶�
    void DrawHScroll(HDC hdc, int cxClient, int cyClient, const RECT& rcScroll);
};