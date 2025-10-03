#pragma once
#include "WndBase.h"
class CMyStatic : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return L"static"; };

public:
    CMyStatic() : CControlBase() { ; }
    virtual ~CMyStatic() { ; }
public:
    virtual inline LPCWSTR className() { return CLASS_STATIC; };
};

