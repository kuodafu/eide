#pragma once

#include "resource.h"
#include "framework.h"
#include "top_state.h"
#include <xml2/tinyxml2.h>



extern CTinyXml2 xml;
struct MAP_INFO
{
    HWND hWnd;
    LPCWSTR name;
    COLORREF crBack;
    HBRUSH hbrBack;
};
extern std::map<int, MAP_INFO> m_map;


// 窗口插件完毕
void wnd_create(HWND hWnd);

inline int GetAttrValue(LPCSTR nodename, LPCSTR attrName)
{
    LPCSTR vl = xml.GetAttrValue(nodename, attrName);
    return atol(vl);
}
inline int GetAttrValue(tinyxml2::XMLNode* node, LPCSTR attrName)
{
    LPCSTR vl = xml.GetAttrValue(node, attrName);
    return atol(vl);
}
inline int rand(int min, int max)
{
    return (::rand() % (max - min + 1)) + min;
}
