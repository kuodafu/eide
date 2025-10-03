#pragma once
#include "WndBase.h"
// 状态条
class CStatusBar : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return STATUSCLASSNAMEW; };

    // 返回栏目是否是默认的栏目, 根据传递进去的栏目数量判断, 如果传递为0, 则内部获取
    // 判断的依据, 1: 栏目数量=1, 2: 栏目宽度=-1, 3: 栏目没有文本, 没有状态, 完全符合才返回true
    virtual bool isDefItem(int count = 0)
    {
        if ( count <= 0 )count = GetCount();
        if ( count != 1 ) return false; // 栏目数量不等于1的肯定不是默认的状态条
        int width = 0;
        if ( GetParts(count, &width) == 0 )
            return false;

        const int state = (int)Send(SB_GETTEXTLENGTHW, 0, 0);
        // 只有一个项目, 并且项目宽度=-1, 并且没有文本, 那就是默认的项目了
        return width == -1 && state == 0;
    }
public:
    CStatusBar() :CControlBase() { ; }
    virtual ~CStatusBar() { ; }
public:
    virtual inline LPCWSTR className() { return CLASS_STATUSBAR; };

public:
    inline bool clear() {
        int tmp[1] = { 0 };
        return Send(SB_SETPARTS, 1, (LPARAM)tmp) != 0;
    }

    // 加入栏目
    // text  = 栏目文本指定栏目中的文本内容, 如果本参数被省略, 默认值为空文本
    // width = 单位为像素，如果为 -1, 表示一直到窗口的最右边, 如果被省略, 默认值为 50
    // tips  = 动态提示文本仅当栏目只有图片但没有内容文本或者内容文本不能被全部显示的情况下才有效, 如果本参数被省略, 默认值为空文本
    // hIcon = 栏目的图标
    // drawType = 绘画类型, SBT_ 开头常量, 默认是凹入式
    inline int insert(LPCWSTR text = 0, int width = 50, LPCWSTR tips = 0, HICON hIcon = 0, int drawType = 0)
    {
        // 没有加入任何栏目之前, 状态栏默认就有一个宽度为-1的栏目
        int count = GetCount();
        if ( count == 0 ) return -1;

        int items[256] = { 0 };
        // 只有一个项目, 并且项目宽度=-1, 并且没有文本, 那就是默认的项目了
        if ( isDefItem(count) )
        {
            // 只有一个项目, 并且是被清空的项目, 那就设置这个项目信息就行了, 不插入新项目
            count = 0;
            items[0] = width;   // 只有一列, 那就设置为传递进来的宽度
        }
        else
        {
            if ( GetParts(count, items) == 0 )
                return -1;
            // count = 新加入项目的索引, 新加入项目右边 = 最后一个项目的右边+宽度, 或者直接等于-1
            items[count] = ( width == -1 ) ? width : items[count - 1] + width;
        }

        // 设置新的
        if ( !SetParts(count + 1, items) )
            return -1;

        if ( text )
            SetText(count, text, drawType);
        if ( tips )
            SetTipsText(count, tips);
        if ( hIcon )
            SetIcon(count, hIcon);

        return count;
    }

    // 取栏目数量, 没有加入任何栏目之前, 状态栏默认就有一个宽度为-1的栏目
    inline int GetCount()
    {
        return GetParts();
    }
    // 取栏目信息, 如果pArr为0, 则返回栏目数量, nArrCount=pArr的成员数
    inline int GetParts(int nArrCount = 0, int* pArr = 0)
    {
        return (int)Send(SB_GETPARTS, nArrCount, (LPARAM)pArr);
    }
    
    // 置栏目信息, 设置状态窗口中的零件数和每个栏目右边缘的坐标
    // nArrCount = 数组成员数, 最大不能超过256
    // pArr = 每个栏目的右边数组
    inline bool SetParts(int nArrCount, int* pArr)
    {
        return Send(SB_SETPARTS, nArrCount, (LPARAM)pArr) != 0;
    }
    
    // 置栏目信息, 设置状态窗口中的零件数和每个栏目右边缘的坐标
    // index = 栏目索引, 从0开始
    // text = 要设置的栏目文本
    // drawType = 绘画类型, SBT_ 开头常量, 默认是凹入式
    inline bool SetText(int index, LPCWSTR text, int drawType = 0)
    {
        LONG wParam = MAKELONG(index | drawType, 0);
        return Send(SB_SETTEXTW, wParam, (LPARAM)text) != 0;
    }
    // 获取栏目文本长度
    // index = 栏目索引, 从0开始
    inline int GetTextLength(int index)
    {
        int state = (int)Send(SB_GETTEXTLENGTHW, index, 0);
        return LOWORD(state);
    }
    // 获取栏目类型
    // index = 栏目索引, 从0开始
    inline int GetType(int index)
    {
        int state = (int)Send(SB_GETTEXTLENGTHW, index, 0);
        return HIWORD(state);
    }
    // 获取栏目文本
    // index = 栏目索引, 从0开始
    inline wstr GetText(int index)
    {
        int len = GetTextLength(index);
        wstr ret(len);
        int num = (int)Send(SB_GETTEXTW, index, (LPARAM)ret.data());
        ret.resize_strlen();
        return ret;
    }
    // 获取栏目文本
    // index = 栏目索引, 从0开始
    inline wstr GetTipsText(int index)
    {
        int len = 260;
        wstr ret;
        int strLen = 0;
        do
        {
            len = len * 2 + 100;
            ret.resize(len);
            strLen = (int)(INT_PTR)Send(SB_GETTIPTEXTW, MAKELONG(index, len), (LPARAM)ret.data());
        } while ( strLen == len - 1 );

        ret.resize_strlen();
        return ret;
    }
    // 置栏目提示文本, 状态条必须是使用 SBT_TOOLTIPS 样式创建, 该方法没有返回值
    // 此工具提示文本在两种情况下显示：
    //      当状态栏中的相应窗格仅包含一个图标时
    //      当状态栏中的相应窗格包含由于窗格大小而被截断的文本时
    // index = 栏目索引, 从0开始
    // text = 要设置的栏目提示文本
    inline void SetTipsText(int index, LPCWSTR text)
    {
        Send(SB_SETTIPTEXTW, index, (LPARAM)text);
    }
    // 置栏目图标, 状态栏不会销毁图标。调用应用程序负责跟踪和销毁任何图标
    // index = 栏目索引, 从0开始
    // hIcon = 要设置的图标的句柄。如果此值为 NULL，则从部件中删除该图标
    inline bool SetIcon(int index, HICON hIcon)
    {
        return Send(SB_SETICON, index, (LPARAM)hIcon) != 0;
    }
    // 取栏目图标, 状态栏不会销毁图标。调用应用程序负责跟踪和销毁任何图标
    // index = 栏目索引, 从0开始
    inline HICON GetIcon(int index)
    {
        return (HICON)Send(SB_GETICON, index, (LPARAM)0);
    }

    // 删除条目
    inline bool DeleteItem(int index)
    {
        int count = GetCount();
        if ( count == 0 || index < 0 || index >= count )
            return false;
        bool ret = false;
        if ( count == 1 )
        {
            count = -1;
            ret = SetParts(1, &count);
            SetText(0, L"", 0);
            SetTipsText(0, L"");
            SetIcon(0, 0);
            SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOCOPYBITS);
            return ret;
        }
        int* rights = new int[count];
        if ( !GetParts(count, rights) )
        {
            delete[] rights;
            return false;
        }

        struct STATUS_ITEM_STRUCT
        {
            HICON hIcon;
            LPWSTR text;
            LPWSTR tips;
            int type;
        };
        int* widths = new int[count]; int right = 0;
        STATUS_ITEM_STRUCT* pItems = new STATUS_ITEM_STRUCT[count];
        memset(pItems, 0, sizeof(STATUS_ITEM_STRUCT) * count);
        for ( int i = 0; i < count; i++ )
        {
            int state = (int)Send(SB_GETTEXTLENGTHW, i, 0);
            int len = LOWORD(state);

            widths[i] = rights[i] - right;
            right = rights[i];
            pItems[i].type = HIWORD(state);
            pItems[i].text = new wchar_t[len + 1];
            pItems[i].hIcon = GetIcon(i);
            pItems[i].text[0] = 0;
            Send(SB_GETTEXTW, i, (LPARAM)pItems[i].text);

            len = 260;
            int strLen = 0;
            do
            {
                len = len * 2 + 100;
                if ( pItems[i].tips )
                    delete[] pItems[i].tips;
                pItems[i].tips = new wchar_t[len];
                pItems[i].tips[0] = 0;
                strLen = (int)(INT_PTR)Send(SB_GETTIPTEXTW, MAKELONG(i, len), (LPARAM)pItems[i].tips);
            } while ( strLen == len - 1 );

        }

        // 从被删除的成员开始循环, 逐个把下一个成员的数据拷贝到被删除成员的位置上
        for ( int i = index; i < count - 1; i++ )
        {
            STATUS_ITEM_STRUCT& prev = pItems[i];
            STATUS_ITEM_STRUCT& next = pItems[i + 1];

            if ( i == index && prev.text ) delete[] prev.text;
            if ( i == index && prev.tips ) delete[] prev.tips;

            prev.hIcon = next.hIcon;
            prev.type = next.type;
            prev.text = next.text;
            prev.tips = next.tips;

            rights[i] = i == 0 ? widths[0] : ( rights[i - 1] + widths[i] );

        }
        ret = SetParts(count - 1, rights);
        delete[] rights;

        for ( int i = 0; i < count - 1; i++ )
        {
            STATUS_ITEM_STRUCT& item = pItems[i];

            SetText(i, item.text, item.type);
            SetTipsText(i, item.tips);
            if ( item.hIcon )
                SetIcon(i, item.hIcon);
            delete[] item.text;
            delete[] item.tips;
        }

        SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOCOPYBITS);
        return ret;


        //int* items = new int[count];
        //if ( !GetParts(count, items) )
        //{
        //    delete[] items;
        //    return false;
        //}

        //for ( int i = index; i < count - 1; i++ )
        //    items[i] = items[i + 1];
        //if ( count == 1 )
        //    items[0] = -1, count = 2;
        //bool ret = SetParts(count - 1, items);
        //delete[] items;
        //return ret;
    }

    // 获取栏目矩形
    inline bool GetItemRect(int index, RECT* prc)
    {
        return Send(SB_GETRECT, index, (LPARAM)prc) != 0;
    }

    // 获取栏目边框宽度
    // pRet = 3个成员的数组, 0=水平边框的宽度, 1=垂直边框的宽度, 2=矩形之间边框的宽度
    inline bool GetBorders(int* pRet)
    {
        return Send(SB_GETBORDERS, 0, (LPARAM)pRet) != 0;
    }
    

    // 设置状态条背景颜色
    // clr = 背景颜色
    inline bool SetBkColor(COLORREF clr)
    {
        return Send(SB_SETBKCOLOR, 0, (LPARAM)clr) != 0;
    }

};

