#pragma once
#include "WndBase.h"
class CButton :public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return L"button"; }

public:
    CButton() :CControlBase() { ; }
    ~CButton() { ; }

public:
    virtual inline LPCWSTR className() { return CLASS_BUTTON; }

public:
    virtual inline bool SetCheck(bool isCheck)
    {
        return Send(BM_SETCHECK, (isCheck ? BST_CHECKED : BST_UNCHECKED), 0) == 0;
    }
    virtual inline bool GetCheck()
    {
        INT_PTR state = Send(BM_GETCHECK, 0, 0);
        return (state & BST_CHECKED) == BST_CHECKED;
    }
    virtual inline bool IsCheck()
    {
        return GetCheck();
    }

};

