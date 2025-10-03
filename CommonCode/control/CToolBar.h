#pragma once
#include "WndBase.h"
#include <d2d/d2d_old.h>
#include <control/WndBaseObjStruct.h>
#include <vector>

constexpr auto TOOL_WIDTH = 80;
constexpr auto TOOL_HEIGHT = 80;

class CToolBar : public CControlBase
{
private:
    struct TOOLCHILD
    {
        RECT rc;
        STATE state;
        LPWSTR text;
        LPWSTR tips;
        DWORD type;
        ARGB crBack[4];
    };
    OBJSTRUCT s_toolData;
    std::vector<TOOLCHILD> s_sub;
    HWND s_hWnd;                // ���������
    size_t s_indexHot = -1;     // ��¼��������ǰ�ȵ���
    size_t s_indexDown = -1;    // ��¼�������ĸ���ť����
    LPARAM s_ptDown;            // ��¼���µ�����
    INT_PTR s_pos;              // ����λ��
private:
    virtual inline LPCWSTR SuperClassName() { return CLASS_TOOL; }

    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

    LRESULT CALLBACK WndProc(LPOBJSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaint(LPOBJSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseMove(LPOBJSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam);
    BOOL drawitem(LPCANVAS hCanvas, HDC hdc, Gdiplus::Graphics* gp, size_t i);
    size_t pt2index(const POINT& pt, bool b);
    bool __query(size_t index, STATE state);
    bool __set(size_t index, STATE state, BOOL isRemove, BOOL isUpdate);
    bool SetUiState(STATE state, BOOL isRemove, BOOL isUpdate);
    bool QueryUiState(STATE state);
    virtual LPCWSTR className() { return CLASS_TOOL; }
public:
    size_t GetFirstVisible();
    LPOBJSTRUCT create(HWND hParent);
    bool AddButton(LPCWSTR text, LPCWSTR tips);
};

