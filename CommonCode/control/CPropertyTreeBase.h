#pragma once
#include <control/CTreeView.h>
#include <control/CEdit.h>
#include <control/CListBox.h>
#include <control/CStatic.h>
#include <commdlg.h>
#include <vector>
#include <map>
#include <CArray.h>
#include "CPropertyTree_virtual.h"
#include <d2d/d2d.h>



class CPropertyTreeBase
{
public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0) = 0;
public:
    // 增加一个分组, 返回这个项目的句柄
    virtual HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPARAM param = 0) = 0;

    // 增加一个文本类型属性项, 属性值只能是文本
    // hParent = InsertGroup() 返回的值, 需要加入到哪个分组里
    // name = 属性名, 属性名运行后不能修改
    // value = 属性值
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value) = 0;

    // 增加一个文本类型属性项, 属性值只能是文本
    // hParent = InsertGroup() 返回的值, 需要加入到哪个分组里
    // name = 属性名, 属性名运行后不能修改
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT) = 0;

    // 取指定项目的属性值, 失败返回0
    virtual LPCWSTR GetItemValue(HTREEITEM hItem) = 0;

    // 设置指定项目的值
    virtual bool SetItemValue(HTREEITEM hItem, LPCWSTR value) = 0;

    // 取指定项目的项目数值, InsertItem/SetItemValue 时填写的值
    virtual LPARAM GetItemParam(HTREEITEM hItem) = 0;

    // 设置指定项目的值
    virtual bool SetItemParam(HTREEITEM hItem, LPARAM param) = 0;

    // 对组合框项目增加项目数据, 调用后类型自动变成组合框类型
    // 调用成功返回表项的组合框项目树, 失败返回-1
    virtual size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0) = 0;

    // 删除组合框数据, index = 删除的下标, 从0开始, 下标越界/不是组合框类型返回false
    virtual bool DelComboboxData(HTREEITEM hItem, int index) = 0;

    // 取指定项目的组合框项目数, 失败返回-1, 不是组合框类型或者项目句柄无效都返回-1
    virtual size_t GetComboboxDataCount(HTREEITEM hItem) = 0;

    // 取指定项目的组合框项目文本, 失败返回空文本, 不是组合框类型, 下标越界, 句柄无效都返回空文本
    virtual LPCWSTR GetComboboxData(HTREEITEM hItem, int index) = 0;

    // 设置指定项目的组合框项目文本, 失败返回false, 不是组合框类型, 下标越界, 句柄无效都返回false
    virtual bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value) = 0;

    // 取指定项目的组合框项目数值
    virtual LPARAM GetComboboxParam(HTREEITEM hItem, int index) = 0;

    // 设置指定项目的组合框项目数值
    virtual bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param) = 0;

    // 设置指定项目为指定类型, 组合框类型需要增加数据
    virtual void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type) = 0;

    // 取指定项目类型
    virtual PROPERTY_TYPE GetItemType(HTREEITEM hItem) = 0;

    // 设置宽度, 千分比, 最小100, 最大900, 控件未创建前也可以调用
    // isLeft = 是否为设置左边, 设置一边后另一边会一起调整
    // value = 100-900之间的值
    // isUpdate = 是否重画
    virtual void SetWidthRatio(bool isLeft, int value, bool isUpdate = false) = 0;

    // 将当前表项所有数据保存成字节集数据, 返回的地址不使用时需要调用 free() 释放
    // isColor = 是否保存颜色配置信息
    virtual LPBYTE save(DWORD* size, bool isColor, bool isList) = 0;

    // 从内存里加载所有项目数据
    virtual bool load(const void* data, DWORD size, bool isUpdate) = 0;

    // 设置颜色信息
    virtual void SetColor(const PROPERTY_COLOR* color, bool isUpdate) = 0;
    // 默认配色, 0=黑色风格, 1=白色风格
    virtual void DefColor(int type, bool isUpdate) = 0;

    virtual void SetItemFont(const LPLOGFONTW lf, bool isUpdate) = 0;
    // 默认为微软雅黑, 字体大小 -12, 没有样式
    // lfItalic = 斜体,  lfUnderline = 下划线, lfStrikeOut = 删除线
    virtual void SetItemFont(bool isUpdate, const wchar_t* name = L"微软雅黑", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) = 0;

    virtual void SetColorBackground(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorBackParent(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorText(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorListBorder(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetColorSelect(ARGB cr, bool isUpdate = false) = 0;

    virtual bool clear() = 0;

    // 弹出对话框, 根据当前项目类型探测对应对话框, 会触发按钮点击回调
    virtual bool popDialog() = 0;

    // 获取当前属性框的窗口句柄
    virtual HWND GetWindow() = 0;
};

