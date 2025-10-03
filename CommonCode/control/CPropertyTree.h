#pragma once
#include "CPropertyTreeBase.h"

// 项目属性值
struct PROPERTY_ITEM_COMBOBOX
{
    wstr name;          // 属性名
    LPARAM param;       // 项目数值
};
struct PROPERTY_ITEM : public IPROPERTY_ITEM
{
    std::vector<PROPERTY_ITEM>* comboboxData;   // 组合框数据
    PROPERTY_ITEM() {
        index = -1;
        param = 0;
        comboboxData = 0;
        type = PROPERTY_TYPE_TEXT;
    }

    PROPERTY_ITEM(const PROPERTY_ITEM& obj) {
        param = obj.param;
        name = obj.name;
        value = obj.value;
        type = obj.type;
        index = obj.index;
        text = obj.text;
        comboboxData = 0;
        if (obj.comboboxData)
        {
            comboboxData = new std::vector<PROPERTY_ITEM>;
            comboboxData->assign(obj.comboboxData->begin(), obj.comboboxData->end());
        }
        return;
    }
    virtual LPCWSTR GetComboboxText(int index)
    {
        if (!comboboxData)return 0;
        int len = (int)comboboxData->size();
        if (index < 0 || index >= len)return 0;
        return comboboxData->at(index).name.c_str();
    }
    virtual LPARAM GetComboboxParam(int index)
    {
        if (!comboboxData)return 0;
        int len = (int)comboboxData->size();
        if (index < 0 || index >= len)return 0;
        return comboboxData->at(index).param;
    }

    // 获取组合框数组数据, 需要外部提供缓冲区
    virtual int GetComboboxArray(IPROPERTY_ITEM** retData)
    {
        if (!comboboxData || !retData)return 0;
        int len = (int)comboboxData->size();
        if (index < 0 || index >= len)return 0;
        std::vector<PROPERTY_ITEM>& arr = *comboboxData;
        for (int i = 0; i < len; i++)
        {
            retData[i] = &(arr[i]);
        }
        return len;
    }
    // 获取当前组合框项目数
    virtual int GetComboboxCount()
    {
        if (!comboboxData)return 0;
        return (int)comboboxData->size();
    }
    ~PROPERTY_ITEM() {
        if (this->comboboxData)
            delete this->comboboxData;
        name.Release();
        value.Release();
    }
    // 返回当前组合框项目数
    inline size_t AddComboboxData(LPCWSTR name, LPARAM param) {
        if (!comboboxData)comboboxData = new std::vector<PROPERTY_ITEM>;
        PROPERTY_ITEM data;
        data.name = name;
        data.param = param;
        comboboxData->push_back(std::move(data));
        PROPERTY_ITEM& item = comboboxData->at(0);
        size_t len = comboboxData->size();
        if ((index == -1)     // 选中项是-1, 设置值
            || (item.name.empty() != value.empty()) // 数组里的值和结构里保存的值不一样
            || (index == 0 && value != item.name))  // 索引=0, 并且结构里的值和数组里的值不一样
        {
            index = 0;
            value = name;
        }
        return len;
    }
    inline bool DelComboboxData(int index) {
        if (!comboboxData)return false;
        int count = (int)comboboxData->size();
        if (index < 0 || index > count)return false;
        auto begin = comboboxData->begin();
        comboboxData->erase(comboboxData->begin() + index);
        //this->index--;
        //if (this->index < 0) this->index = 0;
        if ((size_t)this->index >= comboboxData->size())--this->index;
        if (this->index < 0) this->index = 0;

        return true;

    }
};


class CPropertyTree : public CTreeView
{
private:
    struct D2D_DRAW
    {
        LOGFONTW lf;                    // 字体数据, 父项目字体多个加粗
        LPEX_FONT hFont;                // 绘画字体
        HFONT hFontGdi;                 // gdi字体, 没有加粗
        LPEX_FONT hFontParent;          // 父项目绘画字体

        COLORREF crBackgroundGdi;       // gdi背景颜色, 编辑框/项目背景
        ARGB_D2D crBackground;          // 背景颜色
        LPEX_BRUSH hbrBackground;       // 项目的背景颜色, hbrBackground
        HBRUSH hbrBackgroundGdi;        // 编辑框/列表框 的背景画刷, crBackgroundGdi

        COLORREF crBackParentGdi;       // 父项目和左边空白部分的颜色
        ARGB_D2D crBackParent;          // 父项目和左边空白部分的颜色
        LPEX_BRUSH hbrParent;           // 父项目和左边空白部分的画刷, crBackParent
        LPEX_PEN hpenParent;            // 父项目和左边空白部分的画笔, crBackParent

        COLORREF crTextGdi;             // gdi文本颜色
        ARGB_D2D crText;                // 编辑框的文本颜色
        LPEX_BRUSH hbrText;             // 绘画文本画刷, 编辑框文本颜色, crText

        COLORREF crListBorderGdi;       // 组合框列表边框颜色
        ARGB_D2D crListBorder;          // 组合框列表边框颜色
        LPEX_PEN hpenListBorder;      // 组合框列表边框画笔, crListBorder
        HPEN hpenListBorderGdi;         // 组合框列表边框颜色, crListBorderGdi

        COLORREF crSelectGdi;           // 右边按钮的背景颜色
        ARGB_D2D crSelect;              // 选中颜色, 树型框选中/右边按钮 背景
        LPEX_BRUSH hbrSelect;           // 项目的选中颜色, crSelect
        HBRUSH hbrSelectGdi;            // 项目的选中颜色, gdi画刷

        COLORREF crDisabledGdi;         // 禁止文本颜色GDI颜色
        ARGB_D2D crDisabled;            // 禁止文本颜色D2D颜色
        LPEX_BRUSH hbrDisabled;         // 禁止文本颜色D2D画刷

        float chWidth;                  // 字符i的宽度
        float chHeight;                 // 字符i的宽度
        D2D_DRAW() {
            memset(&lf, 0, sizeof(lf));
            hFont = 0; hFontParent = 0; hFontGdi = 0;
            crBackgroundGdi = 0; hbrBackground = 0; hbrBackgroundGdi = 0;
            crBackParentGdi = 0; hbrParent = 0; hpenParent = 0;
            crTextGdi = 0; hbrText = 0;
            crListBorderGdi = 0; hpenListBorder = 0; hpenListBorderGdi = 0;
            crSelectGdi = 0; hbrSelect = 0; hbrSelectGdi = 0;
            crDisabledGdi = 0; hbrDisabled = 0;
        }
    };
    struct PROPERTY_EVENT
    {
        fnPropertyChanged pfnPropertyChanged;
        void* pfnPropertyChangedParam;

        fnItemChanged pfnItemChanged;
        void* pfnItemChangedParam;

        fnPropertyReturn pfnPropertyReturn;
        void* pfnPropertyReturnParam;

        fnComboboxMouseWheel pfnComboboxMouseWheel;
        void* pfnComboboxMouseWheelParam;

        fnComboboxChanged pfnComboboxChanged;
        void* pfnComboboxChangedParam;

        fnDropWown pfnDropWown;
        void* pfnDropWownParam;

        fnItemButtonClick pfnItemButtonClick;
        void* pfnItemButtonClickParam;

        fnItemRButtonClick pfnItemRButtonClick;
        void* pfnItemRButtonClickParam;

        fnComboboxLDoubleClick pfnComboboxLDoubleClick;
        void* pfnComboboxLDoubleClickParam;

        fnEditLDoubleClick pfnEditLDoubleClick;
        void* pfnEditLDoubleClickParam;

        fnItemChar pfnItemChar;
        void* pfnItemCharParam;

        fnItemMakeCustomValue pfnItemMakeCustomValue;
        void* pfnItemMakeCustomValueParam;


        fnItemRButtonPaint pfnItemRButtonPaint;
        void* pfnItemRButtonPaintParam;

        fnItemDelUp pfnItemDelUp;
        void* pfnItemDelUpParam;

        fnCustomValueName pfnCustomValueName;
        void* pfnCustomValueNameParam;

        fnItemReadError pfnItemReadError;
        void* pfnItemReadErrorParam;

        fnItemBeginDraw pfnItemBeginDraw;
        void* pfnItemBeginDrawParam;

        fnItemEndDraw pfnItemEndDraw;
        void* pfnItemEndDrawParam;
    };
    struct RIGHT_BUTTON_INFO
    {
        RECT rc;
        STATE state;
        int type;           // 0=...按钮, 1=下拉按钮
        bool isVisible;
    };

    struct EXPAND_IMAGE_DATA
    {
        LPBYTE pOpen;
        int nOpen;
        LPEX_IMAGE imgOpen;
        LPBYTE pClose;
        int nClose;
        LPEX_IMAGE imgClose;
    };

    // 记录各种资源的信息, 选择框, 单选框, 这些资源都使用一个方法来设置
    struct PROPERTY_IMAGE_STRUCT
    {
        LPBYTE pData;
        int nSize;
        LPEX_IMAGE hImage;
    };

    static constexpr auto leftSize{ static_cast<int>(20) };     // 左边绘画宽度
    int m_itemHeight;               // 项目高度
    int m_offsetLeft;               // 按下分割线时的偏移
    int m_leftWidth;                // 左边列宽度百分比, 100-900, 低于100调整到100, 高于900调整到900
    int m_defColor;                 // 默认配色索引, 0=黑色, 1=白色
    int m_oldMid;                   // 按下中间分割线时分割线的位置
    bool m_isCapture;               // 是否捕获了鼠标
    HCURSOR m_hCursor;              // 当前鼠标指针
    HTREEITEM m_hItemDown;          // 按下的项目句柄, 没有热点项, 只有按下和选中项
    HTREEITEM m_hItemSel;           // 选中的项目句柄, 没有热点项, 只有按下和选中项
    HTREEITEM m_hItemHot;           // 热点的项目句柄, 鼠标移动时记录, 
    D2D_DRAW m_draw;
    bool m_isNoDblclk;              // 是否屏蔽双击, 触发双击后设置为false
    bool m_isFillColorBack;         // 是否填充颜色选择器背景
    int m_editCaret;                // 编辑框光标计数
    int m_midLineHeight;            // 中间这条线的高度
    PROPERTY_EVENT m_evt;           // 挂接的事件

    CEdit m_edit;                   // 输入文本的编辑框, 组合框就在后面画多一个下拉按钮
    CStatic m_static;               // 右边的按钮
    CListBox m_listBox;             // 组合框专用列表, 点击下拉按钮后弹出这个列表

    std::map<HTREEITEM, PROPERTY_ITEM> m_items;
    LPCANVAS m_hCanvas;
    RECT m_rcCalcText;              // 编辑框文本偏移
    RIGHT_BUTTON_INFO m_rbutton;    // 右边按钮数据
    EXPAND_IMAGE_DATA m_expand;     // 收缩展开按钮图标数据

    struct OPENDLG_DATA
    {
        wstr filter1;               // 文件过滤器, 记录外部调用传递进来的值
        wstr filter;                // 文件过滤器, 需要在打开对话框前调用, 这个是把 | 换成了 0
        wstr title;                 // 对话框标题
        wstr defName;               // 默认文件名
        wstr path;                  // 默认路径
        bool noChangePath;          // 不改变目录
    };


    std::map<PROPERTY_TYPE, OPENDLG_DATA> m_map_openDlg;  // 保存对话框属性信息, 按类型分类
    std::map<PROPERTY_ICONTYPE, LPEX_IMAGE> m_map_icon;   // 根据PROPERTY_ICONTYPE类型获取到图标

public:
    CPropertyTree();
    ~CPropertyTree();
    virtual inline LPCWSTR className() { return L"JY_PropertyBox"; }
    virtual void SetItemHeight(int height, bool isUpdate = false);
    virtual int GetItemHeight() { return m_itemHeight; }

private:
    inline bool query(int type, int n)
    {
        if (type == n)
            return true;
        if (type == PROPERTY_TYPE_UNKNOWN)
            return false;
        return (type & n) == n;
    }
    inline bool query_lobyte(int type, int n)
    {
        if (type == n)
            return true;
        if (type == PROPERTY_TYPE_UNKNOWN)
            return false;
        return (LOBYTE(type) == n);
    }

public:
    virtual bool SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size);
    virtual LPEX_IMAGE GetIcon(PROPERTY_ICONTYPE type);
    // 是否为分组
    inline bool IsGroup(PROPERTY_TYPE type) { return LOWORD(type) == 0; }
    // 是否有按钮
    inline bool IsButton(PROPERTY_TYPE type) { return query(type, PROPERTY_FLAG_BUTTON); }
    // 是否有下拉按钮
    inline bool IsButtonCombobox(PROPERTY_TYPE type) { return query(type, PROPERTY_FLAG_BUTTON_COMBOBOX); }
    // 是否为数值
    inline bool IsNumber(PROPERTY_TYPE type) { return query_lobyte(type, PROPERTY_TYPE_NUMBER); }
    // 是否为文本, 长文本, 带按钮文本都返回true
    inline bool IsText(PROPERTY_TYPE type) { return query_lobyte(type, PROPERTY_TYPE_TEXT); }

    // 以下为扩展类型, 由几个样式组合而成

    // 是否为带按钮文本
    inline bool IsTextButton(PROPERTY_TYPE type)            { return type == PROPERTY_TYPE_TEXT_BUTTON; }
    // 是否为长文本
    inline bool IsLongText(PROPERTY_TYPE type)              { return type == PROPERTY_TYPE_TEXT_LONG; }
    // 是否为不可编辑组合框
    inline bool IsComboboxNoEdit(PROPERTY_TYPE type)        { return type == PROPERTY_TYPE_COMBOBOX_NOEDIT; }
    // 是否为不可编辑组合框
    inline bool IsComboboxNoEditIndex(PROPERTY_TYPE type)   { return type == PROPERTY_TYPE_COMBOBOX_NOEDIT_INDEX; }
    // 是否为组合框, 不可编辑和带编号的都返回真, 需要判断对应的就使用对应的方法
    inline bool IsCombobox(PROPERTY_TYPE type)              { return query_lobyte(type, PROPERTY_FLAG_COMBOBOX); }
    // 是否为颜色选择器
    inline bool IsColorDialog(PROPERTY_TYPE type)           { return query_lobyte(type, PROPERTY_FLAG_COLOR); }
    // 是否为文件选择器
    inline bool IsFileDialog(PROPERTY_TYPE type)            { return query_lobyte(type, PROPERTY_FLAG_FILE); }
    // 是否为字体选择器
    inline bool IsFontDialog(PROPERTY_TYPE type)            { return query_lobyte(type, PROPERTY_FLAG_FONT); }
    // 是否为字节集
    inline bool IsBinDialog(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_BIN); }
    // 是否为图片选择器
    inline bool IsPicDialog(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_PIC); }
    // 是否为音频选择器
    inline bool IsMusicDialog(PROPERTY_TYPE type)           { return query_lobyte(type, PROPERTY_FLAG_MUSIC); }
    // 是否为图片组选择器
    inline bool IsImgListDialog(PROPERTY_TYPE type)         { return query_lobyte(type, PROPERTY_FLAG_IMGLIST); }
    // 是否为链接框
    inline bool IsLink(PROPERTY_TYPE type)                  { return query_lobyte(type, PROPERTY_FLAG_LINK); }
    // 是否为选择框
    inline bool IsCheckBox(PROPERTY_TYPE type)              { return query_lobyte(type, PROPERTY_FLAG_CHECKBOX); }
    // 是否为3态选择框
    inline bool IsCheckBox3(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_CHECKBOX3); }
    // 是否为单选框
    inline bool IsRadio(PROPERTY_TYPE type)                 { return query_lobyte(type, PROPERTY_FLAG_RADIO); }
    // 是否为自动单选框
    inline bool IsAutoRadio(PROPERTY_TYPE type)             { return query_lobyte(type, PROPERTY_FLAG_AUTORADIO); }
    // 是否为开关
    inline bool IsSwitch(PROPERTY_TYPE type)                { return query_lobyte(type, PROPERTY_FLAG_SWITCH); }

    // 是否为选择类型组件, 选择框/3态选择框, 单选框, 自动单选框, 开关, 都属于选择类型
    inline bool IsCheckType(PROPERTY_TYPE type) { return IsCheckBox(type) || IsCheckBox3(type) || IsRadio(type) || IsAutoRadio(type) || IsSwitch(type); }


    // 1=字节,2=短整数,3=整数,4=长整数,5=小数,6=双精度小数, 不会返回其他值
    inline int  GetNumberType(PROPERTY_TYPE type)           { int ret = HIBYTE(type & ~PROPERTY_FLAG_UNSIGNED); return (ret == 0 || ret > 6) ? 6 : ret; }
    // 是否有符号
    inline bool IsUnsigned(PROPERTY_TYPE type)              { return query(LOWORD(type), PROPERTY_FLAG_UNSIGNED); }
    // 是否可编辑
    inline bool IsEdit(PROPERTY_TYPE type)                  { return !query(type, PROPERTY_FLAG_NOEDIT); }
    // 是否有编号
    inline bool IsIndex(PROPERTY_TYPE type)                 { return query(type, PROPERTY_FLAG_INDEX); }
    // 是否自定义属性
    inline bool IsCustom(PROPERTY_TYPE type)                { return query(type, PROPERTY_FLAG_CUSTOM); }

    // 是否禁止
    inline bool IsDisabled(PROPERTY_TYPE type)              { return query(type, PROPERTY_FLAG_DISABLED); }
    // 是否可用
    inline bool IsEnabled(PROPERTY_TYPE type)               { return !IsDisabled(type); }
    // 选择类型整行选择, 没有这个状态点击图标之外的地方不处理选中
    inline bool IsCheckLineSel(PROPERTY_TYPE type)          { return query(type, PROPERTY_FLAG_CHECKLINESEL); }

    // 不响应滚动消息, 并且把消息转发到窗口上
    inline bool IsNoMouseWheel(PROPERTY_TYPE type)          { return query(type, PROPERTY_FLAG_NOMOUSEWHEEL); }


public:
    // 外部对组件子类化之后可以调用这个来进行默认处理
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // 处理鼠标在右边的按钮上, 如果是在按钮上则返回true
    bool OnMouseMove(TVHITTESTINFO& info);
    void modify_value_from_edit();

    void InvalidateItem(HTREEITEM hItem);
    void CreateD2DResouce();
    void __create_d2d_resouce();
    void __delete_d2d_resouce(bool isDestroyCanvas);

    virtual LRESULT OnSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    DrawItem(LPCANVAS hCanvas, HTREEITEM hItem, RECT& rc, int index, int scrollPos, int cxClient, int cyClient);
    void    DrawLink(LPCANVAS hCanvas, IPROPERTY_ITEM& data, HTREEITEM hItem, RECT_F& rc, int index, int scrollPos, int cxClient, int cyClient);
    void    DrawCheckBox_Radio(LPCANVAS hCanvas, IPROPERTY_ITEM& data, HTREEITEM hItem, RECT_F& rc, int index, int scrollPos, int cxClient, int cyClient);
    virtual LRESULT OnPaintListBox(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnListCommand(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual void PaintStatic(RECT& rcItem, IPROPERTY_ITEM& data, HDC hdc);
    virtual LRESULT OnEditProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaintStatic(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


    // 鼠标滚动事件, pos = 滚动距离负数向下滚动, 正数向上滚动, key = 滚动时按下了哪个按键, lParam = 鼠标坐标
    virtual bool OnEditMouseWheel(short pos, short key, LPARAM lParam);

    // 编辑框右键按下
    virtual bool OnEditRButtonDown(int mk, LPARAM pt);

    // 表项被单击, hItem=败单击的表项句柄, col=被单击的列, lParam坐标
    virtual void OnItemClick(HTREEITEM hItem, int col, LPARAM lParam);
    // 编辑框按下回车
    void OnEditReturn();
    // 编辑框左键按下
    void OnEditLDown(int key, DWORD pt);
    // 编辑框被双击
    void OnEditDblClk();
    bool OnEditNcCalcSize(WPARAM wParam, LPARAM lParam);

    inline void OnItemPropChanged(HTREEITEM hItem, PROPERTY_ITEM& data) {
        if (m_evt.pfnPropertyChanged)
            m_evt.pfnPropertyChanged(m_evt.pfnPropertyChangedParam, hItem, data.type);
    }

    // 编辑框输入, 限制输入字符, 返回是否允许输入, true = 允许输入, false = 不允许
    bool __Edit_insert_char(int ch, bool bUnsigned, bool isDouble, const DWORD start, const DWORD end, wstr& text, wstr& insertText);

    // 限制编辑框输入最大最小值
    bool __Edit_insert_char_min_max(int ch, bool bUnsigned, double min, double max, double umax, LPCWSTR minstr, LPCWSTR maxstr, LPCWSTR umaxstr);
    // 字符输入, 返回是否拦截, 如果返回true, 则内部会使用 pRet 的值作为返回值
    bool OnEditChar(int ch, int* pRet);
    // 编辑框显示隐藏
    void OnEditShow(bool isShow);
    // 编辑框获取焦点
    void OnEditSetFocus();
    // 编辑框失去焦点
    void OnEditKillFocus();
    // 列表框失去焦点
    void OnListBoxKillFocus();
    // 组合框项目被选中
    void OnComboboxChanged(int index);

    // 删除键按下
    bool OnItemDelDown();
    // 删除键放开, isName = 是否在属性名这一列按下按键
    void OnItemDelUp(bool isName);

    // 移动编辑框,和右边的按钮
    // TODO 待修改, 根据类型显示哪个
    void moveEdit_Static(HTREEITEM hItem);

    // 属性的按钮被单击, 按钮类型, 项目句柄是当前被选中的项目
    virtual void OnValueButtonClick(int type);

    virtual void CalcListRect(PROPERTY_ITEM& data, std::vector<PROPERTY_ITEM>& arr, const int len, RECT& ret);

    virtual int CalcListWidth(PROPERTY_ITEM& data, std::vector<PROPERTY_ITEM>& arr, const int len, const int maxWidth, int width, const int cxClient, const int cyClient);


    // 计算中间这条线的位置
    inline int calcMidLine(int cxClient = 0) {
        if (cxClient <= 0) {
            RECT rc;
            GetClientRect(&rc);
            cxClient = rc.right - rc.left;
        }
        int width = cxClient * m_leftWidth / 1000;
        return width;
    }

    // 生成自定义属性名文本, 根据不同类型返回对应的文本, 所有的自定义都应该进来这里处理
    wstr MakeCustomText(IPROPERTY_ITEM& data, HTREEITEM hItem);

    // 计算列表框位置, 
    // rc = 接收计算的结果
    // isMove = 为true则计算完毕后移动窗口
    bool calcListBoxRect(RECT* rc, bool isMove, int mid, const RECT& selItemRect, const RECT& treeClientRect);
    LRESULT OnVScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0);

public:
    // 写入字节集, 传递数据指针和长度, 内部会转成易语言的格式
    inline bool SetBinData(IPROPERTY_ITEM& data, const void* pData, const size_t size) {
        if (!pData || !size)return false;
        data.value.resize(size / 2 + 8);
        int* ptr = (int*)data.value.data();

        ptr[0] = 1;
        ptr[1] = (int)(LONG_PTR)size;
        memcpy(&ptr[2], pData, size);
        return true;
    }

    // 写入字节集, 传递数据必须是易语言的格式
    inline bool SetBinData(IPROPERTY_ITEM& data, const void* pData) {
        if (!pData || ((int*)pData)[0] != 1)
            return false;
        const int size = ((int*)pData)[1];
        const int reSize = size / 2 * 8;
        data.value.resize(reSize);
        LPWSTR ptr = data.value.data();
        const int cpySize = size + 8;
        memcpy(ptr, pData, cpySize);
        return true;
    }

    // 写入字体, 字体存放的数据传递是固定的, 两个结构, LOGFONTW + CHOOSEFONTW, 内部会修改字体大小/字符高度
    inline bool SetFontData(IPROPERTY_ITEM& data, const LPLOGFONTW lf) {
        if (!lf)return false;
        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW ptr = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(ptr + 1);
        memcpy(ptr, lf, sizeof(LOGFONTW));

        cf->lpLogFont = (LPLOGFONTW)ptr;
        cf->iPointSize = _font_height2size(ptr->lfHeight) * 10;
        return true;
    }

    // 写入字体, 传入CHOOSEFONTW结构指针, 内部会修改字体大小/字符高度
    // CHOOSEFONTW 指向的 LOGFONT结构有值的话也会吧这个结构拷贝保存起来
    inline bool SetFontData(IPROPERTY_ITEM& data, const LPCHOOSEFONTW pcf) {
        if (!pcf)return false;
        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW ptr = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(ptr + 1);
        memcpy(cf, pcf, sizeof(CHOOSEFONTW));
        ptr->lfHeight = _font_size2height(cf->iPointSize / 10);
        if (cf->lpLogFont)
        {
            memcpy(ptr, cf->lpLogFont, sizeof(LOGFONTW));
        }

        cf->lpLogFont = (LPLOGFONTW)ptr;
        return true;
    }

    // 写入字体, 传递gdi字体指针, 内部会修改字体大小/字符高度
    inline bool SetFontData(IPROPERTY_ITEM& data, HFONT hFont) {
        if (!hFont)return false;
        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW ptr = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(ptr + 1);
        GetObjectW(hFont, sizeof(LOGFONTW), ptr);

        cf->lpLogFont = (LPLOGFONTW)ptr;
        cf->iPointSize = _font_height2size(ptr->lfHeight) * 10;
        return true;
    }


    // 写入字体, 字体存放的数据传递是固定的, 两个结构, LOGFONTW + CHOOSEFONTW
    inline bool SetFontData(IPROPERTY_ITEM& data, const wchar_t* name = L"微软雅黑", int lfHeight = -12,
        bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) {

        data.value.resize((sizeof(LOGFONTW) + sizeof(CHOOSEFONTW)) / 2 + 8);

        LPLOGFONTW lf = (LPLOGFONTW)data.value.data();
        LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(lf + 1);

        if (name)
        {
            size_t len = wcslen(name) + 1;
            if (len > 32)len = 32;
            memcpy(lf->lfFaceName, name, len * sizeof(wchar_t));
        }

        lf->lfHeight = lfHeight;
        lf->lfWeight = lfWeight ? FW_BOLD : FW_NORMAL;
        lf->lfItalic = lfItalic;
        lf->lfUnderline = lfUnderline;
        lf->lfStrikeOut = lfStrikeOut;

        cf->lpLogFont = (LPLOGFONTW)lf;
        cf->iPointSize = _font_height2size(lf->lfHeight) * 10;
        return true;
    }

    // 获取字体数据, 内部有更新 LPCHOOSEFONTW 结构指向的logfont地址
    inline LPLOGFONTW GetFontData(IPROPERTY_ITEM& data, LPCHOOSEFONTW* pcf) {
        LPLOGFONTW lf = (LPLOGFONTW)data.value.data();
        if (pcf)*pcf = 0;
        if (lf)
        {
            LPCHOOSEFONTW cf = (LPCHOOSEFONTW)(lf + 1);
            cf->lpLogFont = lf;
            if (pcf) *pcf = cf;
        }
        return lf;
    }

    // 根据项目句柄获取项目存放项目的数据指针
    inline IPROPERTY_ITEM* GetItemData(HTREEITEM hItem) {
        if (!hItem)return 0;
        auto f = m_items.find(hItem);
        if (f == m_items.end()) return 0;
        PROPERTY_ITEM& data = f->second;
        return &data;
    }
    // 取现行选中项
    inline HTREEITEM GetIndex() {
        return m_hItemSel;
    }

    // 设置对话框属性, 为0则不设置对应的属性
    // type = 要设置的对话框类型, 目前只支持 PROPERTY_FLAG_FILE文件选择器, PROPERTY_FLAG_BIN字节集选择器
    // filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3\0媒体文件(*.mpg)|*.mpg”
    // defName = 默认文件名
    // title = 窗口标题
    // path = 运行目录
    // notChengedPath = 不改变目录, 需要传递变量指针, 指针为0则不设置
    bool SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, bool* noChangePath = 0);

    // 获取对话框属性, 为0则不获取对应的属性
    // type = 要设置的对话框类型, 目前只支持 PROPERTY_FLAG_FILE文件选择器, PROPERTY_FLAG_BIN字节集选择器
    // filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3\0媒体文件(*.mpg)|*.mpg”
    // defName = 默认文件名
    // title = 窗口标题
    // path = 运行目录
    // notChengedPath = 不改变目录
    bool GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter = 0, LPCWSTR* title = 0, LPCWSTR* defName = 0, LPCWSTR* path = 0, bool* noChangePath = 0);

public:
    // 增加一个分组, 返回这个项目的句柄
    HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text = 0, LPARAM param = 0);

    // 设置指定项目的值, 字节集类型和易语言一样
    bool SetItemValue(IPROPERTY_ITEM& data, const void* value);

    // 设置说明文本
    bool SetItemRemarks(HTREEITEM hItem, LPCWSTR value);
    // 获取说明文本
    LPCWSTR GetItemRemarks(HTREEITEM hItem);

    // 增加一个文本类型属性项, 属性值只能是文本
    // hParent = InsertGroup() 返回的值, 需要加入到哪个分组里
    // name = 属性名, 属性名运行后不能修改
    // value = 属性值
    // text = 备注文本
    // param = 项目数值
    HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value = 0, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT, LPCWSTR text = 0, LPARAM param = 0);

    // 删除指定项目
    BOOL DeleteItem(HTREEITEM hItem);

    // 取指定项目的属性名
    LPCWSTR GetItemName(HTREEITEM hItem);
    bool SetItemName(HTREEITEM hItem, LPCWSTR name);

    // 取指定项目的属性值, 失败返回0, 字节集类型需要在前面加上 {1,0,0,0,长度} 8个字节, 和易语言一样
    LPCWSTR GetItemValue(HTREEITEM hItem);

    // 设置指定项目的值, 字节集类型和易语言一样
    bool SetItemValue(HTREEITEM hItem, LPCWSTR value);

    // 取指定项目的项目数值, InsertItem/SetItemValue 时填写的值
    LPARAM GetItemParam(HTREEITEM hItem);

    // 设置指定项目的值
    bool SetItemParam(HTREEITEM hItem, LPARAM param);

    // 对组合框项目增加项目数据, 调用后类型自动变成组合框类型
    // 调用成功返回表项的组合框项目树, 失败返回-1
    size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0);

    // 删除组合框数据, index = 删除的下标, 从0开始, 下标越界/不是组合框类型返回false
    bool DelComboboxData(HTREEITEM hItem, int index);

    // 取指定项目的组合框项目数, 失败返回-1, 不是组合框类型或者项目句柄无效都返回-1
    size_t GetComboboxDataCount(HTREEITEM hItem);

    // 取指定项目的组合框项目文本, 失败返回空文本, 不是组合框类型, 下标越界, 句柄无效都返回空文本
    LPCWSTR GetComboboxData(HTREEITEM hItem, int index);

    // 设置指定项目的组合框项目文本, 失败返回false, 不是组合框类型, 下标越界, 句柄无效都返回false
    bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value);

    // 获取组合框现行选中项, 失败返回-1
    int GetComboboxIndex(HTREEITEM hItem);

    // 设置组合框现行选中项, 索引从0开始
    bool SetComboboxIndex(HTREEITEM hItem, int index);

    // 取指定项目的组合框项目数值
    LPARAM GetComboboxParam(HTREEITEM hItem, int index);

    // 设置指定项目的组合框项目数值
    bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param);

    // 设置指定项目为指定类型, 组合框类型需要增加数据
    void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type);

    // 取指定项目类型
    PROPERTY_TYPE GetItemType(HTREEITEM hItem);

    // 设置宽度, 千分比, 最小100, 最大900, 控件未创建前也可以调用
    // isLeft = 是否为设置左边, 设置一边后另一边会一起调整
    // value = 100-900之间的值
    // isUpdate = 是否重画
    void SetWidthRatio(bool isLeft, int value, bool isUpdate = false);

    // 将当前表项所有数据保存成字节集数据, 返回的地址不使用时需要调用 free() 释放
    // isColor = 是否保存颜色配置信息
    LPBYTE save(DWORD* size, bool isColor, bool isList);

    void Load_Color(LIST_PROP_HEAD* head, LPBYTE& pData, bool isUpdate);
    // pData 必须传递读取列表之后的地址
    void Load_Expand(LIST_PROP_HEAD* head, LPBYTE& pData);
    // 从内存里加载所有项目数据
    bool load(const void* data, DWORD size, bool isUpdate);

    // 从加载的数据里添加到树型框里
    bool Load_Insert_Tree(LIST_PROP_HEAD* head, LPBYTE& pData);

    // 设置颜色信息
    void SetColor(const PROPERTY_COLOR* color, bool isUpdate);
    int GetColor(PROPERTY_COLOR* color);
    // 默认配色, 0=黑色风格, 1=白色风格
    int DefColor(int type, bool isUpdate);

    int SetItemFont(const LPLOGFONTW lf, bool isUpdate);
    // 默认为微软雅黑, 字体大小 -12, 没有样式
    // lfItalic = 斜体,  lfUnderline = 下划线, lfStrikeOut = 删除线
    int SetItemFont(bool isUpdate, const wchar_t* name = L"微软雅黑", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false);

    int SetColorBackground(ARGB cr, bool isUpdate = false);
    int SetColorBackParent(ARGB cr, bool isUpdate = false);
    int SetColorText(ARGB cr, bool isUpdate = false);
    int SetColorListBorder(ARGB cr, bool isUpdate = false);
    int SetColorSelect(ARGB cr, bool isUpdate = false);
    int SetColorDisabled(ARGB cr, bool isUpdate = false);
    inline void SetIsFillColorBack(bool isFill) { m_isFillColorBack = isFill; InvalidateRect(); }

    inline bool SetItemDisabledState(HTREEITEM hItem, bool isDisabled) { return isDisabled ? DisabledItem(hItem) : EnabledItem(hItem); }
    // 禁止项目
    bool DisabledItem(HTREEITEM hItem);
    // 启用项目
    bool EnabledItem(HTREEITEM hItem);
    // 项目是否禁止
    bool IsItemDisabled(HTREEITEM hItem);
    // 项目是否启用
    bool IsItemEnabled(HTREEITEM hItem);

    inline bool clear() {
        m_items.clear();
        m_listBox.hide();
        m_edit.hide();
        m_rbutton.isVisible = false;
        InvalidateRect(&m_rbutton.rc);
        return TreeView_DeleteItem(m_hWnd, TVI_ROOT);
    }

    // 弹出对话框, 根据当前项目类型探测对应对话框, 会触发按钮点击回调
    inline void popDialog() {
        IPROPERTY_ITEM* data = GetItemData(m_hItemSel);
        if (!data)return;
        const bool isBtn = IsButton(data->type);
        const bool isCbb = IsCombobox(data->type);
        if (!isBtn && !isCbb)return;
        const int type = isCbb ? 1 : (isBtn ? 0 : 0);   // 现在暂时只有组合框和按钮
        OnValueButtonClick(type);
    }

    // 获取属性框矩形
    void GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type);

    LPBYTE GetExpand_Open(int* size);
    LPBYTE GetExpand_Close(int* size);
    bool SetExpand_Open(const void* pData, int size);
    bool SetExpand_Close(const void* pData, int size);
private:
    // 生成列表数据
    _str make_listdata();

public:


    // 项目属性被改变, 为空则取消挂接这个事件, 事件两个参数, 参数1是触发事件的项目句柄, 参数2是项目的类型
    inline void OnPropertyChanged(fnPropertyChanged pfn = 0, void* param = 0) {
        m_evt.pfnPropertyChanged = pfn;
        m_evt.pfnPropertyChangedParam = param;
    }

    // 项目选择改变, 为空则取消挂接这个事件, 事件两个参数, 参数1是触发事件的项目句柄, 参数2是在哪个位置点击, -1=展开收缩按钮, 1=属性名, 2=属性值
    inline void OnItemChanged(fnItemChanged pfn = 0, void* param = 0) {
        m_evt.pfnItemChanged = pfn;
        m_evt.pfnItemChangedParam = param;
    }

    // 项目属性值按下了回车键, 回调函数返回是否拦截, 为空则取消挂接这个事件, 如果回调函数返回true则不对按下回车键做进一步处理
    // 事件两个参数, 参数1是触发事件的项目句柄, 参数2是项目的类型
    inline void OnPropertyReturn(fnPropertyReturn pfn = 0, void* param = 0) {
        m_evt.pfnPropertyReturn = pfn;
    }

    // 组合框项目鼠标滚动, 回调函数返回是否拦截, 如果回调函数返回true则不对鼠标滚动做进一步处理
    // count = 组合框项目总数, pos = 滚动距离负数向下滚动, 正数向上滚动, key = 滚动时按下了哪个按键, lParam = 鼠标坐标
    inline void OnComboboxMouseWheel(fnComboboxMouseWheel pfn = 0, void* param = 0) {
        m_evt.pfnComboboxMouseWheel = pfn;
        m_evt.pfnComboboxMouseWheelParam = param;
    }

    // 组合框项目改变, 回调函数返回是否拦截, 如果回调函数返回true则不会改变项目
    // index = 要改变的项目索引, 返回false后组合框现行选中项就是这个索引
    inline void OnComboboxChanged(fnComboboxChanged pfn = 0, void* param = 0) {
        m_evt.pfnComboboxChanged = pfn;
        m_evt.pfnComboboxChangedParam = param;
    }

    // 组合框下拉箭头被按下, 即将探测列表, 回调函数返回是否拦截, 如果回调函数返回true则不会弹出列表
    inline void OnDropDown(fnDropWown pfn = 0, void* param = 0) {
        m_evt.pfnDropWown = pfn;
        m_evt.pfnDropWownParam = param;
    }

    // 属性按钮被单击, 回调函数返回是否拦截, 如果回调函数返回true则不会弹出内置的对话框
    inline void OnItemButtonClick(fnItemButtonClick pfn = 0, void* param = 0) {
        m_evt.pfnItemButtonClick = pfn;
        m_evt.pfnItemButtonClickParam = param;
    }

    // 项目被右键单击, 返回是否拦截, 拦截后右键不会改变表项, 不会触发单击
    inline void OnItemRButtonClick(fnItemRButtonClick pfn = 0, void* param = 0) {
        m_evt.pfnItemRButtonClick = pfn;
        m_evt.pfnItemRButtonClickParam = param;
    }

    // 组合框双击编辑框, 回调函数返回是否拦截, 如果回调函数返回true则不对双击做进一步处理
    // index = 双击时组合框的索引, count = 组合框项目数
    inline void OnComboboxLDoubleClick(fnComboboxLDoubleClick pfn = 0, void* param = 0) {
        m_evt.pfnComboboxLDoubleClick = pfn;
        m_evt.pfnComboboxLDoubleClickParam = param;
    }

    // 编辑框被双击, 回调函数返回是否拦截, 如果回调函数返回true则不全选编辑框
    inline void OnEditLDoubleClick(fnEditLDoubleClick pfn = 0, void* param = 0) {
        m_evt.pfnEditLDoubleClick = pfn;
        m_evt.pfnEditLDoubleClickParam = param;
    }

    // 项目输入字符, 回调函数返回是否拦截, 如果回调函数返回true则使用 pRet 返回的值作为返回值
    // ch = 输入的字符, pRet = 返回值指针, 需要把返回值写入这里
    inline void OnItemChar(fnItemChar pfn = 0, void* param = 0) {
        m_evt.pfnItemChar = pfn;
        m_evt.pfnItemCharParam = param;
    }

    // 生成自定义属性值的显示名字, 三个参数, 项目句柄, 项目类型, 显示文本缓冲区, 最大100个字符
    inline void OnItemMakeCustomValue(fnItemMakeCustomValue pfn = 0, void* param = 0) {
        m_evt.pfnItemMakeCustomValue = pfn;
        m_evt.pfnItemMakeCustomValueParam = param;
    }

    // 右边小按钮被重画, 返回true则内部不进行绘画, 需要用户在回调里进行绘画操作
    // hdc = 按钮绘画dc, 不是内存DC, state = 当前状态, STATE::HOT, STATE::DOWN, width = 按钮宽度, height = 按钮高度
    inline void OnItemRButtonPaint(fnItemRButtonPaint pfn = 0, void* param = 0) {
        m_evt.pfnItemRButtonPaint = pfn;
        m_evt.pfnItemRButtonPaintParam = param;
    }

    // del键放开, 删除当前项数据, 在属性名这一列按下的默认处理是清除值, 不是属性名按下的只清除双击会打开对话框的项目
    // isName = 是否在属性名这一列按下按键
    inline void OnItemKeyDelUp(fnItemDelUp pfn = 0, void* param = 0) {
        m_evt.pfnItemDelUp = pfn;
        m_evt.pfnItemDelUpParam = param;
    }

    // 自定义属性名字
    // isName = 是否在属性名这一列按下按键
    inline void OnCustomValueName(fnCustomValueName pfn = 0, void* param = 0) {
        m_evt.pfnCustomValueName = pfn;
        m_evt.pfnCustomValueNameParam = param;
    }

    // 项目读取数据失败, 字节集, 图片, 音频, 图片组这些....
    inline void OnItemReadError(fnItemReadError pfn = 0, void* param = 0) {
        m_evt.pfnItemReadError = pfn;
        m_evt.pfnItemReadErrorParam = param;
    }

    // 即将绘画, 内部绘画前先调用用户的绘画
    inline void OnItemBeginDraw(fnItemBeginDraw pfn = 0, void* param = 0) {
        m_evt.pfnItemBeginDraw = pfn;
        m_evt.pfnItemBeginDrawParam = param;
    }

    // 绘画结束, 内部绘画已经结束, 用户可以在这基础上进行二次绘画
    inline void OnItemEndDraw(fnItemEndDraw pfn = 0, void* param = 0) {
        m_evt.pfnItemEndDraw = pfn;
        m_evt.pfnItemEndDrawParam = param;
    }

};

