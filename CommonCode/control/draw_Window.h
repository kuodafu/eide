#pragma once
#include "draw_Window_include.h"

#define DRAW_WND_BYCOMMAND      0x00000000L // nPosition ָ����ID
#define DRAW_WND_BYPOSITION     0x00000400L // nPosition ָ����λ��

// �Ի洰��, �滭ϵͳ��ť, ����, ͼ��, �߿���Ӱ��
// 
class CDrawWindow
{
    LPDRAW_WINDOW_DATA m_data;
public:
    // �󶨴���, �󶨺��ڲ���Ϊ������ڽ����Ի洦��
    // arg = ���ڵ�һЩ������Ϣ
    virtual bool BindWindow(const DRAW_WINDOW_ARGUMENT* arg);
    static LPDRAW_WINDOW_DATA BindWindow_s(const DRAW_WINDOW_ARGUMENT* arg);
    static LPDRAW_WINDOW_DATA GetObjFromHwnd(HWND hWnd);

    // ���ϵͳ��ť, ����ϵͳ��ť����
    // hWnd = ��Ҫ���ϵͳ��ť�Ĵ���
    // id = ϵͳ��ťID
    // param = ��ť��ֵ
    virtual int AddSysButton(int id, LPARAM param);
    static int AddSysButton(HWND hWnd, int id, LPARAM param);
    static int AddSysButton(LPDRAW_WINDOW_DATA data, int id, LPARAM param);

    // ����ϵͳ��ť, ���ز���ɾ���ɹ�
    // hWnd = ��Ҫ���ϵͳ��ť�Ĵ���
    // nPosition = Ҫ�����λ��, ����������ť�������������ֵ
    // id = ϵͳ��ťID
    // param = ��ť��ֵ
    // flags = DRAW_WND_ ��ͷ����, ��ʾ nPosition ��λ�û���ID
    virtual int InsSysButton(int nPosition, int id, LPARAM param, int flags);
    static  int InsSysButton(HWND hWnd, int nPosition, int id, LPARAM param, int flags);
    static  int InsSysButton(LPDRAW_WINDOW_DATA data, int nPosition, int id, LPARAM param, int flags);

    // ɾ��ϵͳ��ť, �����Ƿ�ɾ���ɹ�
    // hWnd = ��Ҫɾ��ϵͳ��ť�Ĵ���
    // id = ϵͳ��ťID
    // flags = DRAW_WND_ ��ͷ����, ��ʾ nPosition ��λ�û���ID
    virtual int DelSysButton(int nPosition, int flags);
    static  int DelSysButton(HWND hWnd, int nPosition, int flags);
    static  int DelSysButton(LPDRAW_WINDOW_DATA data, int nPosition, int flags);

    // ����ϵͳ��ť��Ŀ��ֵ, ��������ǰ����Ŀ��ֵ
    // hWnd = ��Ҫ������ͳ��ť��Ŀ��ֵ�Ĵ���
    // nPosition = Ҫ���õİ�ťID��λ��
    // param = Ҫ���õ���ֵ
    // flags = DRAW_WND_ ��ͷ����, ��ʾ nPosition ��λ�û���ID
    virtual LPARAM SetButtonParam(int nPosition, LPARAM param, int flags);
    static  LPARAM SetButtonParam(HWND hWnd, int nPosition, LPARAM param, int flags);
    static  LPARAM SetButtonParam(LPDRAW_WINDOW_DATA data, int nPosition, LPARAM param, int flags);

    // ��ȡϵͳ��ť��Ŀ��ֵ
    // hWnd = ��Ҫ��ȡϵͳ��ť��Ŀ��ֵ�Ĵ���
    // nPosition = Ҫ��ȡ�İ�ťID��λ��
    // flags = DRAW_WND_ ��ͷ����, ��ʾ nPosition ��λ�û���ID
    virtual LPARAM GetButtonParam(int nPosition, int flags);
    static  LPARAM GetButtonParam(HWND hWnd, int nPosition, int flags);
    static  LPARAM GetButtonParam(LPDRAW_WINDOW_DATA data, int nPosition, int flags);

    // �Ӱ�ť������ȡϵͳ��ťID
    // hWnd = ϵͳ��ť���ڴ���
    // nPosition = Ҫ��ȡ�İ�ťλ��
    virtual int GetButtonIdFromPos(int nPosition);
    static  int GetButtonIdFromPos(HWND hWnd, int nPosition);
    static  int GetButtonIdFromPos(LPDRAW_WINDOW_DATA data, int nPosition);

    // �Ӱ�ťID��ȡϵͳ��ť������
    // hWnd = ϵͳ��ť���ڴ���
    // id = ��ťID
    virtual int GetButtonPosFromId(int id);
    static  int GetButtonPosFromId(HWND hWnd, int id);
    static  int GetButtonPosFromId(LPDRAW_WINDOW_DATA data, int id);

    // ���û滭���, ����ԭ���Ļ������
    virtual LPCANVAS SetCanvas(LPCANVAS hCanvas);

    // �ڲ�����һ������, ���ػ��������
    // dwExStyle = ������չ��ʽ
    // lpWindowName = ���ڱ���
    // dwStyle = ������ʽ
    // x,y,cx,cy = �����󶥿��
    // hWndParent = ������, ����Ϊ0
    // arg = ���ڵ����໯����, Ϊ0��ʹ��Ĭ������
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent = 0, const DRAW_WINDOW_ARGUMENT* arg = 0);
    static HWND create_s(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent = 0, const DRAW_WINDOW_ARGUMENT* arg = 0);


    // ��Ϣ������, �����ڲ��Ƿ��Ѵ���������Ϣ, pRet = ���մ�����ķ���ֵ
    // message = ��Ҫ�������Ϣ
    // wParam, lParam = ��Ϣ����������
    // oldProc = ԭ��������Ϣ�Ļص���ַ, �ڲ��п��ܻ���Ҫ����ԭ����
    // pRet = ���մ��������Ϣ����ֵ
    virtual bool WindowProc(UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet);
    static bool WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet);


    // �滭ϵͳ��ť, �ڲ�������� BeginPaint �� EndPaint, �ڲ�ֱ��ʹ�� hCanvas, ��ϵͳ��ť, ͼ��, ���⻭����
    // ��������ǵ��� DrawSysIcon, DrawTitle, DrawSysButton �⼸������ʵ�ֵ�, λ�����ڲ���¼��λ��
    // ��Ҫ�����ķ�ʽ����õ����Ļ滭����
    // hWnd = ���ھ��
    // ps = BeginPaint �õ��Ľṹ����, �ڲ�������ػ������ж��Ƿ���л滭
    // hCanvas = �������, �ڲ�ֱ��ʹ���������������л滭
    virtual bool OnPaint(const PAINTSTRUCT& ps, LPCANVAS hCanvas);

    // �滭ͼ��, ������ָ����λ�û滭ϵͳͼ��
    // ps = BeginPaint �õ��Ľṹ����, �ڲ�������ػ������ж��Ƿ���л滭
    // hCanvas = �������, �ڲ�ֱ��ʹ���������������л滭
    virtual bool DrawSysIcon(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcIcon);
    static  bool DrawSysIcon(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcIcon);

    // �滭����, ������ָ����λ�û滭����
    // ps = BeginPaint �õ��Ľṹ����, �ڲ�������ػ������ж��Ƿ���л滭
    // hCanvas = �������, �ڲ�ֱ��ʹ���������������л滭
    // rcTitle = �滭�����λ��
    virtual bool DrawTitle(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcTitle);
    static  bool DrawTitle(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcTitle);

    // �滭ϵͳ, ������ָ����λ�û滭ϵͳ��ť, ϵͳ��ť���ҵ�������滭
    // ps = BeginPaint �õ��Ľṹ����, �ڲ�������ػ������ж��Ƿ���л滭
    // hCanvas = �������, �ڲ�ֱ��ʹ���������������л滭
    // rcIcon = ����ϵͳͼ��ռ�õ�λ��, �ڲ����ҵ�������滭ͼ��
    virtual bool DrawSysButton(const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcBtn);
    static  bool DrawSysButton(LPDRAW_WINDOW_DATA data, const PAINTSTRUCT& ps, LPCANVAS hCanvas, const RECT_F& rcBtn);

    virtual bool EventDraw(PFN_SysDraw pfn, LPARAM param);
    static  bool EventDraw(HWND hWnd, PFN_SysDraw pfn, LPARAM param);
    static  bool EventDraw(LPDRAW_WINDOW_DATA data, PFN_SysDraw pfn, LPARAM param);
    virtual bool EventBtnClick(PFN_SysBtnClick pfn, LPARAM param);
    static  bool EventBtnClick(HWND hWnd, PFN_SysBtnClick pfn, LPARAM param);
    static  bool EventBtnClick(LPDRAW_WINDOW_DATA data, PFN_SysBtnClick pfn, LPARAM param);


    // ��ȡĬ�ϵĲ����ṹ
    static DRAW_WINDOW_ARGUMENT GetDefArg(HWND hWnd);

};
