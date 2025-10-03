#pragma once
#include "CButton.h"
class CCheckBox : public CButton
{
protected:

public:
    CCheckBox() :CButton() { ; }
    virtual ~CCheckBox() { ; }

public:
    virtual inline LPCWSTR className() { return CLASS_CHECKBOX; };
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0)
    {
        dwStyle |= BS_CHECKBOX;
        dwStyle |= WS_CHILD;
        return CWndBase::create(dwExStyle, lpszTitle, dwStyle, x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }


};

