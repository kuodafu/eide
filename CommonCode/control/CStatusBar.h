#pragma once
#include "WndBase.h"
// ״̬��
class CStatusBar : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return STATUSCLASSNAMEW; };

    // ������Ŀ�Ƿ���Ĭ�ϵ���Ŀ, ���ݴ��ݽ�ȥ����Ŀ�����ж�, �������Ϊ0, ���ڲ���ȡ
    // �жϵ�����, 1: ��Ŀ����=1, 2: ��Ŀ���=-1, 3: ��Ŀû���ı�, û��״̬, ��ȫ���ϲŷ���true
    virtual bool isDefItem(int count = 0)
    {
        if ( count <= 0 )count = GetCount();
        if ( count != 1 ) return false; // ��Ŀ����������1�Ŀ϶�����Ĭ�ϵ�״̬��
        int width = 0;
        if ( GetParts(count, &width) == 0 )
            return false;

        const int state = (int)Send(SB_GETTEXTLENGTHW, 0, 0);
        // ֻ��һ����Ŀ, ������Ŀ���=-1, ����û���ı�, �Ǿ���Ĭ�ϵ���Ŀ��
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

    // ������Ŀ
    // text  = ��Ŀ�ı�ָ����Ŀ�е��ı�����, �����������ʡ��, Ĭ��ֵΪ���ı�
    // width = ��λΪ���أ����Ϊ -1, ��ʾһֱ�����ڵ����ұ�, �����ʡ��, Ĭ��ֵΪ 50
    // tips  = ��̬��ʾ�ı�������Ŀֻ��ͼƬ��û�������ı����������ı����ܱ�ȫ����ʾ������²���Ч, �����������ʡ��, Ĭ��ֵΪ���ı�
    // hIcon = ��Ŀ��ͼ��
    // drawType = �滭����, SBT_ ��ͷ����, Ĭ���ǰ���ʽ
    inline int insert(LPCWSTR text = 0, int width = 50, LPCWSTR tips = 0, HICON hIcon = 0, int drawType = 0)
    {
        // û�м����κ���Ŀ֮ǰ, ״̬��Ĭ�Ͼ���һ�����Ϊ-1����Ŀ
        int count = GetCount();
        if ( count == 0 ) return -1;

        int items[256] = { 0 };
        // ֻ��һ����Ŀ, ������Ŀ���=-1, ����û���ı�, �Ǿ���Ĭ�ϵ���Ŀ��
        if ( isDefItem(count) )
        {
            // ֻ��һ����Ŀ, �����Ǳ���յ���Ŀ, �Ǿ����������Ŀ��Ϣ������, ����������Ŀ
            count = 0;
            items[0] = width;   // ֻ��һ��, �Ǿ�����Ϊ���ݽ����Ŀ��
        }
        else
        {
            if ( GetParts(count, items) == 0 )
                return -1;
            // count = �¼�����Ŀ������, �¼�����Ŀ�ұ� = ���һ����Ŀ���ұ�+���, ����ֱ�ӵ���-1
            items[count] = ( width == -1 ) ? width : items[count - 1] + width;
        }

        // �����µ�
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

    // ȡ��Ŀ����, û�м����κ���Ŀ֮ǰ, ״̬��Ĭ�Ͼ���һ�����Ϊ-1����Ŀ
    inline int GetCount()
    {
        return GetParts();
    }
    // ȡ��Ŀ��Ϣ, ���pArrΪ0, �򷵻���Ŀ����, nArrCount=pArr�ĳ�Ա��
    inline int GetParts(int nArrCount = 0, int* pArr = 0)
    {
        return (int)Send(SB_GETPARTS, nArrCount, (LPARAM)pArr);
    }
    
    // ����Ŀ��Ϣ, ����״̬�����е��������ÿ����Ŀ�ұ�Ե������
    // nArrCount = �����Ա��, ����ܳ���256
    // pArr = ÿ����Ŀ���ұ�����
    inline bool SetParts(int nArrCount, int* pArr)
    {
        return Send(SB_SETPARTS, nArrCount, (LPARAM)pArr) != 0;
    }
    
    // ����Ŀ��Ϣ, ����״̬�����е��������ÿ����Ŀ�ұ�Ե������
    // index = ��Ŀ����, ��0��ʼ
    // text = Ҫ���õ���Ŀ�ı�
    // drawType = �滭����, SBT_ ��ͷ����, Ĭ���ǰ���ʽ
    inline bool SetText(int index, LPCWSTR text, int drawType = 0)
    {
        LONG wParam = MAKELONG(index | drawType, 0);
        return Send(SB_SETTEXTW, wParam, (LPARAM)text) != 0;
    }
    // ��ȡ��Ŀ�ı�����
    // index = ��Ŀ����, ��0��ʼ
    inline int GetTextLength(int index)
    {
        int state = (int)Send(SB_GETTEXTLENGTHW, index, 0);
        return LOWORD(state);
    }
    // ��ȡ��Ŀ����
    // index = ��Ŀ����, ��0��ʼ
    inline int GetType(int index)
    {
        int state = (int)Send(SB_GETTEXTLENGTHW, index, 0);
        return HIWORD(state);
    }
    // ��ȡ��Ŀ�ı�
    // index = ��Ŀ����, ��0��ʼ
    inline wstr GetText(int index)
    {
        int len = GetTextLength(index);
        wstr ret(len);
        int num = (int)Send(SB_GETTEXTW, index, (LPARAM)ret.data());
        ret.resize_strlen();
        return ret;
    }
    // ��ȡ��Ŀ�ı�
    // index = ��Ŀ����, ��0��ʼ
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
    // ����Ŀ��ʾ�ı�, ״̬��������ʹ�� SBT_TOOLTIPS ��ʽ����, �÷���û�з���ֵ
    // �˹�����ʾ�ı��������������ʾ��
    //      ��״̬���е���Ӧ���������һ��ͼ��ʱ
    //      ��״̬���е���Ӧ����������ڴ����С�����ضϵ��ı�ʱ
    // index = ��Ŀ����, ��0��ʼ
    // text = Ҫ���õ���Ŀ��ʾ�ı�
    inline void SetTipsText(int index, LPCWSTR text)
    {
        Send(SB_SETTIPTEXTW, index, (LPARAM)text);
    }
    // ����Ŀͼ��, ״̬����������ͼ�ꡣ����Ӧ�ó�������ٺ������κ�ͼ��
    // index = ��Ŀ����, ��0��ʼ
    // hIcon = Ҫ���õ�ͼ��ľ���������ֵΪ NULL����Ӳ�����ɾ����ͼ��
    inline bool SetIcon(int index, HICON hIcon)
    {
        return Send(SB_SETICON, index, (LPARAM)hIcon) != 0;
    }
    // ȡ��Ŀͼ��, ״̬����������ͼ�ꡣ����Ӧ�ó�������ٺ������κ�ͼ��
    // index = ��Ŀ����, ��0��ʼ
    inline HICON GetIcon(int index)
    {
        return (HICON)Send(SB_GETICON, index, (LPARAM)0);
    }

    // ɾ����Ŀ
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

        // �ӱ�ɾ���ĳ�Ա��ʼѭ��, �������һ����Ա�����ݿ�������ɾ����Ա��λ����
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

    // ��ȡ��Ŀ����
    inline bool GetItemRect(int index, RECT* prc)
    {
        return Send(SB_GETRECT, index, (LPARAM)prc) != 0;
    }

    // ��ȡ��Ŀ�߿���
    // pRet = 3����Ա������, 0=ˮƽ�߿�Ŀ��, 1=��ֱ�߿�Ŀ��, 2=����֮��߿�Ŀ��
    inline bool GetBorders(int* pRet)
    {
        return Send(SB_GETBORDERS, 0, (LPARAM)pRet) != 0;
    }
    

    // ����״̬��������ɫ
    // clr = ������ɫ
    inline bool SetBkColor(COLORREF clr)
    {
        return Send(SB_SETBKCOLOR, 0, (LPARAM)clr) != 0;
    }

};

