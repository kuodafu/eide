#pragma once
//#include <control/WndBase.h>
#include "CTreeView.h"
#include <CArray.h>
#include <control/Ctips.h>
#include <d2d/d2d_old.h>

class CTreeList;
class CTreeListBase : public CTreeView
{
public:
    virtual LRESULT OnHeadNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual LRESULT OnTreeNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual LRESULT OnPaintHeader(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual LRESULT WndProc_header(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual void __create_d2d_resouce() = 0;
    virtual HWND GetTreeWindow() = 0;
};

#define TLVIF_TEXT              0x0001  // 存取文本, 获取文本时 cchText须要给值
#define TLVIF_FONT              0x0002  // 设置字体, hFont需要给值
#define TLVIF_BORDER            0x0004  // 边框, border,isManualBorder 需要给值
#define TLVIF_PARAM             0X0008  // 项目数值, param需要给值
#define TLVIF_TYPE              0x0010  // 列类型, type需要给值
#define TLVIF_CRTEXT            0x0020  // 文本颜色, crText需要给值
#define TLVIF_ABSLEFT           0x0040  // 绝对左边
#define TLVIF_ABSCOL            0x0080  // 绝对宽度
#define TLVIF_TEXTFORMAT        0x0100  // 文本对齐格式
#define TLVIF_ALL               0x01FF  // 所有属性


#define BORDER_PEN_NULL         0x0000  // 没有边框
#define BORDER_PEN_SOLID        0x0001  // 实线
#define BORDER_PEN_DASH         0x0002  // 划线
#define BORDER_PEN_DOT          0x0003  // 点线
#define BORDER_PEN_DASHDOT      0x0004  // 点划线
#define BORDER_PEN_DASHDOTDOT   0x0005  // 点点划线
#define BORDER_LEFT             0x0100  // 绘画左边边框
#define BORDER_TOP              0x0200  // 绘画顶边边框
#define BORDER_RIGHT            0x0400  // 绘画右边边框
#define BORDER_BOTTOM           0x0800  // 绘画底边边框
#define BORDER_RECT         (BORDER_LEFT | BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM)  // (BORDER_LEFT | BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM)

#define TLVTYPE_TEXT            0       // 纯文本
#define TLVTYPE_STATIC          1       // 静态文本, 不响应热点按下的绘画
#define TLVTYPE_BUTTON          2       // 按钮
#define TLVTYPE_EDIT            3       // 编辑框
#define TLVTYPE_CHECKBOX        4       // 选择框
#define TLVTYPE_RADIO           5       // 单选框
#define TLVTYPE_COMBOBOX        6       // 组合框
#define TLVTYPE_        0

#define TREEITEM_ICONWDITH      16
#define TREESYSICON_INVALID     -1
#define TREESYSICON_EXPAND      0
#define TREESYSICON_CHECKBOX    1
#define TREESYSICON_ICON        2
typedef struct HMD
{
    HDC hdc;
    HBITMAP hBitmap;
    void* lpBits;
    int width;
    int height;
}*LPHMD;
typedef struct TLVITEM
{
    int cbSize;
    int mask;               // TLVIF_ 标志, 根据标志决定设置哪些成员
    bool isAbsWidth;        // 是否绝对列宽, 为真时调整表头的时候不修改列列宽
    bool isAbsLeft;         // 是否绝对左边, 为真时调整表头的时候不修改列左边
    bool isManualBorder;    // 是否手动设置边框
    int indexCol;           // 列索引
    int width;              // 列宽
    int left;               // 列左边位置
    DWORD border;           // BORDER_ 边框类型
    DWORD format;           // 文本对齐格式
    LPWSTR text;            // 设置或获取文本
    int cchText;            // 缓冲区长度, 获取时需要设置
    int type;               // TLVTYPE_, 列类型, 运行单独设置某行的某列
    LONG_PTR param;         // 项目数值
    ARGB_D2D crText;        // 文本颜色
    LPEX_FONT hFont;        // 字体句柄 _font_create() 创建
}*LPTLVITEM;
typedef const TLVITEM* LPCTLVITEM;
typedef struct TREELIST_DATA
{
    CTreeListBase* pThis;   // 本类指针
    WNDPROC oldProcStatic;  // 旧窗口过程
    WNDPROC oldProcTree;    // 旧窗口过程
    WNDPROC oldProcHeader;  // 旧窗口过程
    HWND hHead;             // 表头句柄
    HWND hStatic;           // 标签句柄, 树型框和表头都放在里面
    HWND hTree;             // 树型框窗口句柄
    int indexHotHead;       // 热点项
    int indexDownHead;      // 按下项
    int nHeadHeight;        // 头高度, 默认30
    HTREEITEM index;        // 当前选中索引
    HTREEITEM hNext;        // 当前选中索引下一个兄弟项, 在单击表项时赋值
    HTREEITEM indexDown;    // 鼠标按下表项
    int indexCol;           // 当前选中列索引
    int indexColIcon;       // 当前鼠标在哪个系统图标中
    HTREEITEM indexHot;     // 热点项, 在鼠标移动赋值
    HTREEITEM hotParent;    // 热点组的父项目
    int ptParam;            // 按下的坐标, 低位X,高位Y
    int width;              // 窗口宽度
    int height;             // 窗口高度
    RECT rcHead;            // 表头位置, 左顶右底
    bool isHover;           // 是否进入控件, 在 NM_SETCURSOR 赋值为true, 在WM_MOUSELEAVE赋值为false
    bool isExpand;          // 是否绘制加减框
    bool isIcon;            // 是否绘制图标
    bool isTips;            // 是否有提示工具
    DWORD CheckType;        // 是否绘制选择框, 0=没有, 1=双态选择框, 2=3态选择框(部分), 3=3态选择框(排除), 4=3态选择框(禁止)
    int state;              // 表项状态
    float Indent;           // 缩进像素, 默认=12.0
    int count;              // 当前树列表项目数
    int rcFirstHotLeft;     // 第一个热点项的左边

    LPEX_FONT hFont;        // 默认绘画字体
    LPEX_BRUSH hBrNormal;   // 正常画刷
    LPEX_BRUSH hBrSF;       // 选中+焦点
    LPEX_BRUSH hBrSelect;   // 选中画刷
    LPEX_BRUSH hBrHot;      // 热点画刷
    LPEX_BRUSH hBrtext;     // 热点画刷
    LPEX_BRUSH hBrLine;     // 线条画刷
    LPEX_PEN hpenLine;      // 线条画刷
    LPEX_BRUSH hBrTopLine;  // 顶部线条画刷
    LPEX_PEN hpenTopLine;   // 顶部线条画刷
    LPEX_BRUSH hBrHove;     // 进入画刷
    LPCANVAS hCanvas;       // 绘画句柄
    LPCANVAS hCanvas_Head;  // 表头绘画句柄

    // 0-128 是树型框图标, 130 - 220 是选择框图标,  220 - 340 是单选框图标
    // 树型框图标 0像素开始, 顶部是0,   16*16, 0=正常收缩,1=正常展开,2=点燃收缩,3=点燃展开, 4-7 加粗
    // 选择框图标 130像素开始, 顶部是0, 15*15, 0=正常未选中, 1=点燃未选中, 2=按下未选中, 3=正常选中, 4=点燃选中, 5=按下选中
    // 单选框图标 220像素开始, 顶部是0, 15*15, 0=正常未选中, 1=点燃未选中, 2=按下未选中, 3=禁止未选中, 4=正常选中, 5=点燃选中, 6=按下选中, 7=禁止选中
    LPEX_IMAGE imgTree;     

}*LPTREELIST_DATA;
typedef struct TREELIST_COL_DATA
{
    DWORD width;
    RECT rc;
    wstr title;
    TREELIST_COL_DATA()
    {
        width = 0;
        rc = { 0 };
    }
}*LPTREELIST_COL_DATA;
typedef const TREELIST_COL_DATA* LPCTREELIST_COL_DATA;
typedef struct HTREELISTITEM_SUBDATA
{
    bool isAbsLeft;
    bool isAbsWidth;
    DWORD width;            // 每列的宽度, 可以根据需要手动修改
    DWORD left;             // 每列的左边位置, 可以根据需要手动修改
    DWORD border;           // BORDER_ 边框类型
    wstr title;             // 当前行每一列的数据
    DWORD type;             // TLVTYPE_, 列类型, 每一列都可以为不同类型, 可以根据需要修改指定行指定列
    LPEX_FONT hFont;        // 绘画字体, 每一列都可以不同, 为0则使用默认
    ARGB_D2D crText;        // 绘画字体颜色, 每一列都可以不同, 为0则使用默认
    LPEX_BRUSH hbrText;     // 绘画字体画刷, 每一列都可以不同, 为0则使用默认
    RECT_F rcText;          // 文本区域
    DWORD format;           // 文本对齐格式
    LPARAM param;           // 列项目数值
    HTREELISTITEM_SUBDATA()
    {
        isAbsLeft = false;
        isAbsWidth = false;
        width = 0;
        left = 0;
        border = 0;
        type = 0;
        hFont = 0;
        hbrText = 0;
        format = 0;
        param = 0;
    }
}*LPHTREELISTITEM_SUBDATA;

// 项目数据, 每个项目的项目数值都是这个结构
// 记录当前项,每一列的数据, 包括列宽,标题,类型,文本颜色,字体
typedef struct HTREELISTITEM
{
    HTREEITEM hItem;
    LONG_PTR param;         // 项目数值
    //bool isAbsWidth;        // 是否绝对列宽, 为真时调整表头的时候不修改列列宽
    //bool isAbsLeft;         // 是否绝对左边, 为真时调整表头的时候不修改列左边
    bool isManualBorder;    // 是否手动设置边框
    int count;              // 列数, 下面所有的数组成员数都是这个值
    int level;              // 缩进层次
    int state;              // 当前表项状态
    int flag;               // 表项标志
    int stateExpand;        // 加减框状态
    int stateCheckBox;      // 加减框状态
    int stateIcon;          // 加减框状态
    int index;              // 在数组里的索引
    LPHTREELISTITEM_SUBDATA draw;    // 绘画信息数组
    HTREELISTITEM(CArray<TREELIST_COL_DATA>& m_header)
    {
        memset(this, 0, sizeof(*this));
        count = m_header.size();
        if (count == 0)
            m_header.resize(++count);
        draw = new HTREELISTITEM_SUBDATA[count];
        memset(draw, 0, sizeof(HTREELISTITEM_SUBDATA) * count);

        //for (int i = 0; i < count; i++)
        //{
        //    draw[i].left = m_header.at(i).rc.left;
        //    draw[i].width = m_header.at(i).width;
        //}
    }
    ~HTREELISTITEM()
    {
        for (int i = 0; i < count; i++)
        {
            _font_destroy(draw[i].hFont);
            _brush_destroy(draw[i].hbrText);
        }
        delete[] draw;
        memset(this, 0, sizeof(*this));
    }

}*LPHTREELISTITEM;

typedef bool (*pfn_OnItemClick)(HTREEITEM hItem, LPHTREELISTITEM data, int& indexCol, int& indexColIcon, bool isRight);
typedef bool (*pfn_OnItemDblClk)(HTREEITEM hItem);
typedef bool (*pfn_OnIndexChenged)(HTREEITEM hItem, LPHTREELISTITEM data, int& indexCol, int& indexColIcon);
typedef void (*pfn_OnItemDelete)(HTREEITEM hItem, LPHTREELISTITEM data);
typedef void (*pfn_OnLButtonUp)(int x, int y);
typedef void (*pfn_OnRButtonUp)(int x, int y);

typedef struct tagTREELIST_EVENT
{
    pfn_OnItemClick pClick;
    pfn_OnItemDblClk pDblclk;
    pfn_OnIndexChenged pChenged;
    pfn_OnItemDelete pDelete;
    pfn_OnLButtonUp pLUp;
    pfn_OnRButtonUp pRUp;
}TREELIST_EVENT, * PTREELIST_EVENT, * LPTREELIST_EVENT;


class CTreeList : public CTreeListBase
{
private:
    typedef struct tagDWAWITEM_STRUCT
    {
        LPCANVAS hCanvas;
        HTREEITEM hItem;
        LPHTREELISTITEM data;
        HTREELISTITEM_SUBDATA subData;
        LPEX_FONT hFont;
        LPEX_BRUSH hbrText;
        tagDWAWITEM_STRUCT()
        {
            hCanvas = 0;
            hItem = 0;
            data = 0;
            hFont = 0;
            hbrText = 0;
        }
    }DWAWITEM_STRUCT, *LPDWAWITEM_STRUCT;

    // 热点组的热点项目
    typedef struct tagHOT_GROUP_ARY
    {
        HTREEITEM hItem;        // 项目句柄
        LPHTREELISTITEM data;   // 项目数据
    }HOT_GROUP_ARY, *LPHOT_GROUP_ARY;

    int m_headerIndexHot;
    bool m_headerIsDown;
    CArray<TREELIST_COL_DATA> m_header;         // 表头数据
    int* m_headerIndex;                         // 列数据索引, 默认是 01234...绘画应该逐个成员从这个数组里获取
    LPTREELIST_DATA m_data;
    CArray<HOT_GROUP_ARY> m_hotGroup;           // 热点项目组          
    Ctips m_tips;
    TREELIST_EVENT m_evt;                       // 本类事件
    bool m_isHotList;                           // 热点时是否显示整个列表
    HTREEITEM* m_hItems;                        // 当前树表项句柄数组
    int m_hItemsCount;                          // 当前树表项句柄数组成员数
protected:
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WndProc_header(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnHeadNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnTreeNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual bool OnStyleChenged(bool isEx, DWORD style);
    LRESULT IndexChanged(HTREEITEM hItem, LPHTREELISTITEM data, int indexCol, int indexColIcon, int x, int y);
    int     enumItem();
    LRESULT __OnSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnMouseLDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnMouseLUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    __OnItemClick(HTREEITEM hItem, LPHTREELISTITEM data, int indexCol, int indexColIcon, bool isRightBtn);
    LRESULT __OnMouseMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnMouseLeave(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT __OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    DrawItem(LPCANVAS hCanvas, HTREEITEM hItem, int index, int scrollPos, const RECT_F& rcParent);
    void    DrawItemLine(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    void    DrawItemBorder(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    //void    GetDrawIInfo(LPHTREELISTITEM data);
    // 设置表项状态, isRemove = 是否删除状态, bDraw = 是否重画
    void    SetUIItem(HTREEITEM hItem, LPHTREELISTITEM data, STATE state, bool isRemove, bool bDraw);
    void    SetUIItem(HTREEITEM hItem, STATE state, bool isRemove, bool bDraw);
    int     GetUIItem(LPHTREELISTITEM data);
    int     GetUIItem(HTREEITEM hItem);
    void    SetStateIcon(LPHTREELISTITEM data, int index, STATE state, bool isRemove);
    int     GetStateIcon(LPHTREELISTITEM data, int index);

    // 设置父项目下索引子项目热点状态
    int     ModifyParentHot(HTREEITEM hParent, HTREEITEM hItem, bool isRemove);


    // 填充表项背景
    void    FillItem(LPCANVAS hCanvas, LPHTREELISTITEM data, int scrollPos, const RECT_F& rcf);
    // 绘画展开收缩图标
    void    DrawItemExpand(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    // 绘画图标
    void    DrawItemIcon(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    // 绘画选择框
    void    DrawItemCheckbox(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item);
    // 绘制类型, 由这个方法把各个类型分类调用其他方法实现
    void    DrawItemType(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // 绘制文本类型, 静态文本
    void    DrawItemType_Text(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // 绘制按钮类型控件
    void    DrawItemType_Button(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // 绘制编辑框类型控件
    void    DrawItemType_Edit(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // 绘制选择框类型控件
    void    DrawItemType_CheckBox(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // 绘制单选框类型控件
    void    DrawItemType_Radio(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);
    // 绘制组合框类型控件
    void    DrawItemTypeCombobox(LPDWAWITEM_STRUCT draw, const RECT_F& rcf);

    void __create_d2d_resouce();
    void __delete_d2d_resouce();
    void __modify_col_width(int indexCol, int cx);
    LRESULT OnHScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaintHeader(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    InvalidateItem(HTREEITEM hItem);

    // 计算加减框的位置, 返回的是加减框在当前项目中的位置, 不是在窗口中的位置
    void CalcExpand(LPHTREELISTITEM data, RECT* rc, int scrollPos);
    // 计算图标位置, 返回的是图标在当前项目中的位置, 不是在窗口中的位置
    void CalcIcon(LPHTREELISTITEM data, RECT* rc, int scrollPos);
    // 计算选择框位置, 返回的是选择框在当前项目中的位置, 不是在窗口中的位置
    void CalcCheckbox(LPHTREELISTITEM data, RECT* rc, int scrollPos);
    // 鼠标是否在加减框中, x,y坐标是项目坐标, 不是窗口客户区坐标
    bool ptinExpand(LPHTREELISTITEM data, int x, int y, int scrollPos);
    // 鼠标是否在图标中, x,y坐标是项目坐标, 不是窗口客户区坐标
    bool ptinIcon(LPHTREELISTITEM data, int x, int y, int scrollPos);
    // 鼠标是否在选择框中, x,y坐标是项目坐标, 不是窗口客户区坐标
    bool ptinCheckbox(LPHTREELISTITEM data, int x, int y, int scrollPos);
    // 鼠标是否在几个系统图标中, 加减框, 选择框, 图标, 不是在这几个图标中返回-1, 否则返回从0开始的索引
    // 0=加减框, 1=选择框, 2=图标
    // x,y坐标是项目坐标, 不是窗口客户区坐标
    int ptinSysIcon(LPHTREELISTITEM data, RECT* prc, int x, int y, int scrollPos);
    // 鼠标是否在表项中, 不在表项中会删除热点状态并重画热点表项
    HTREEITEM MouseIsItem(int x, int y, WPARAM wParam, LPHTREELISTITEM* data, int* indexCol, int* indexColIcon, int scrollPos, bool isDown);
    
public:
    CTreeList();
    virtual ~CTreeList();
    virtual inline LPCWSTR className() { return L"TreeListView"; };

public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0);
    HWND create(HWND hWndParent, DWORD dwExStyleTree);
    HWND create(HWND hWndParent, LPCWSTR title, RECT rc, int style, int styleEx, DWORD dwExStyleTree, LONG_PTR ID, void* lpParam = 0);
    HWND GetTreeWindow() { return m_data ? m_data->hTree : 0; }
    // 设置标题高度, 返回设置前的高度
    int  SetHeaderHeight(int height);
    inline bool Move(int x, int y, int cx, int cy, bool bRepaint = true)
    {
        if (!m_data)return false;
        return MoveWindow(m_data->hStatic, x, y, cx, cy, bRepaint);
    }
    virtual HTREEITEM InsertItem(HTREEITEM hParent = TVGN_ROOT, LPCWSTR itemText = 0, LONG_PTR itemParam = 0, HTREEITEM InsertMode = 0);
    int DoInsertItem(int iInsertAfter, int nWidth, LPCWSTR lpsz);
    bool SetItem(HTREEITEM hItem, const TLVITEM* itemData, int indexCol);
    bool GetItem(HTREEITEM hItem, LPTLVITEM itemData);
    
    bool SetItemText(HTREEITEM hItem, int indexCol, LPCWSTR text);
    wstr GetItemText(HTREEITEM hItem, int indexCol);
    HIMAGELIST SetImageList(HIMAGELIST hImageList, int type = 0);

    // 取列标题
    wstr GetColText(int index);
    // 置列标题
    bool SetColText(int index, LPCWSTR text);
    // 取列信息, 返回的值最好不要修改
    bool GetColInfo(int index, LPTREELIST_COL_DATA& info);
    // 取列项目数值
    LPARAM GetItemColParam(HTREEITEM hItem, int indexCol);
    // 值列项目数值
    bool SetItemColParam(HTREEITEM hItem, int indexCol, LPARAM lParam);
    // 取表项项目数值
    LPARAM GetItemParam(HTREEITEM hItem);
    bool isItem(HTREEITEM hItem);
    // 值表项项目数值
    bool SetItemParam(HTREEITEM hItem, LPARAM lParam);
    inline void IsHotList(bool b) { m_isHotList = b; }
    // 返回当前选中列索引, 失败返回-1, 索引从0开始
    inline int GetIndexCol() { return m_data ? m_data->indexCol : -1; }
    // 取现行选中项
    virtual inline HTREEITEM GetIndex() { return m_data ? m_data->index : 0; }
    virtual bool UpDateItem(HTREEITEM hItem = 0);
    // 修改选中项
    virtual int SetIndex(HTREEITEM hItem = 0);
    virtual void SetColWidth(int index, int width);

public:
    // 绑定点击项目事件, 为0则清楚
    inline void OnItemClick(pfn_OnItemClick pfn = 0) { m_evt.pClick = pfn; }
    inline void OnItemDblclk(pfn_OnItemDblClk pfn = 0) { m_evt.pDblclk = pfn; }
    inline void OnItemDelete(pfn_OnItemDelete pfn = 0) { m_evt.pDelete = pfn; }
    inline void OnIndexChenged(pfn_OnIndexChenged pfn = 0) { m_evt.pChenged = pfn; }
    inline void OnLButtonUp(pfn_OnLButtonUp pfn = 0) { m_evt.pLUp = pfn; }
    inline void OnRButtonUp(pfn_OnRButtonUp pfn = 0) { m_evt.pRUp = pfn; }
    inline int GetHeaderWidths(int** ary) {
        int len = m_header.size();
        if (ary && len)
        {
            *ary = new int[len];
            for (int i = 0; i < len; i++)
                (*ary)[i] = m_header[i].width;
        }
        return len;
    }


public:
    inline HWND GetStatic() { return m_data ? m_data->hStatic : 0; }

    // 取窗口客户取坐标
    inline RECT GetClientRect() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc;
    }

    // 取窗口在屏幕中的位置
    inline RECT GetWindowRect() {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return rc;
    }

    // 取窗口在父窗口中的位置
    inline RECT GetRect() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        ScreenToClient(GetStatic(), (LPPOINT)(&rc));
        ScreenToClient(GetStatic(), ((LPPOINT)(&rc)) + 1);
        return rc;
    }

    // 取用户区宽度
    inline bool GetClientWidth() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.right - rc.left;
    }

    // 取用户区高度
    inline bool GetClientHeight() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.bottom - rc.top;
    }

    // 取窗口在父窗口中的坐标
    inline POINT GetClientPos() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        POINT pt = { rc.left, rc.top };
        HWND hWnd = ::GetParent(GetStatic());
        if (!hWnd)hWnd = GetStatic();
        ScreenToClient(hWnd, &pt);
        return pt;
    }

    //// 设置窗口在父窗口中的坐标
    //inline bool SetClientPos(const LPPOINT ppt) {
    //    if (!ppt) return false;
    //    return SetWindowPos(GetStatic(), 0, ppt->x, ppt->y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    //}

    // 取窗口在屏幕中的坐标
    inline POINT GetPos() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        POINT pt = { rc.left, rc.top };
        return pt;
    }

    // 设置窗口在屏幕中的坐标
    inline bool SetPos(const LPPOINT ppt) {
        if (!ppt) return false;
        return SetWindowPos(GetStatic(), 0, ppt->x, ppt->y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    }

    // 取窗口宽高
    inline SIZE GetSize() {
        RECT rc = { 0 };
        bool ret = ::GetWindowRect(GetStatic(), &rc);
        SIZE si = { 0 };
        si.cx = rc.right - rc.left;
        si.cy = rc.bottom - rc.top;
        return si;
    }

    // 设置窗口宽高
    inline bool SetSize(const LPSIZE psi) {
        if (!psi) return false;
        return SetWindowPos(GetStatic(), 0, 0, 0, psi->cx, psi->cy, SWP_NOOWNERZORDER | SWP_NOMOVE);
    }

    // 设置窗口左边
    inline bool SetLeft(int left) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, left, rc.top, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);

    }

    // 设置窗口顶边
    inline bool SetTop(int top) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, rc.left, top, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
    }

    // 设置窗口宽度
    inline bool SetWidth(int width) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, 0, 0, width, rc.bottom - rc.top, SWP_NOOWNERZORDER | SWP_NOMOVE);

    }

    // 设置窗口高度
    inline bool SetHeight(int height) {
        RECT rc = { 0 };
        ::GetWindowRect(GetStatic(), &rc);
        return SetWindowPos(GetStatic(), 0, 0, 0, rc.right - rc.left, height, SWP_NOOWNERZORDER | SWP_NOMOVE);
    }

    // 取窗口高度
    inline int GetWidth() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.bottom - rc.top;
    }

    // 取窗口高度
    inline int GetHeight() {
        RECT rc = { 0 };
        ::GetClientRect(GetStatic(), &rc);
        return rc.bottom - rc.top;
    }

    // 显示窗口
    inline bool show() {
        return SetVisible(true);
    }

    // 隐藏窗口
    inline bool hide() {
        return SetVisible(false);
    }

    // 置窗口可视状态
    inline bool SetVisible(bool isVisible) {
        return ::ShowWindow(GetStatic(), isVisible ? SW_SHOW : SW_HIDE);
    }

    // 取窗口可视状态
    inline bool GetVisible() {
        return ::IsWindowVisible(GetStatic());
    }

    // 取窗口可视状态
    inline bool IsVisible() {
        return ::IsWindowVisible(GetStatic());
    }

    // 置窗口启用状态, true = 启用, false = 禁止
    inline bool SetEnabled(bool isEnable) {
        return ::EnableWindow(GetStatic(), isEnable);
    }

    // 取窗口启用状态, true = 启用, false = 禁止
    inline bool GetEnabled() {
        return ::IsWindowEnabled(GetStatic());
    }
};
