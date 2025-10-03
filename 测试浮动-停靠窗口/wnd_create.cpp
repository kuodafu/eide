#include "pch.h"
#include <assert.h>

struct CELL_DATA
{
    int size;
    int dockAlign;
    HWND hWnd;
    int level;
    int childLeft;
    int childCount;

    int index;              // �ڸ���Ŀ�е�����
    int flags;

    int left;
    int top;
    int width;
    int height;
    CELL_DATA* prve;        // ��һ����Ŀ������
    CELL_DATA* next;        // ��һ����Ŀ������
    CELL_DATA* parent;      // ����Ŀ������
    CELL_DATA* child;       // ��һ������
    std::vector<HWND>* pChildWnd;   // ��ǰ��Ŀ�µ��Ӵ�������, �����ߴ��ʱ����Ҫ������Щ����
};
//void Parse_cell (HWND hParent, LPCSTR _nodeName, CELL_DATA& data);
//void Parse_pang (HWND hParent, LPCSTR _nodeName, CELL_DATA& data);
//void Parse_view (HWND hParent, LPCSTR _nodeName, CELL_DATA& data);
//void Parse_group(HWND hParent, LPCSTR _nodeName, CELL_DATA& data);

static CELL_DATA* m_cell_wnd;       // �ڴ�����ĸ���, ������ʾ�ڴ�����ĸ��Ӷ������λ������
static CELL_DATA* m_cell_top;       // �ϱ߸��ӵ�λ��
static CELL_DATA* m_cell_bottom;    // �±߸��ӵ�λ��


inline void _xml_getvalue(tinyxml2::XMLNode* node, CELL_DATA* data)
{
    int show        = GetAttrValue(node, "show");
    int type        = GetAttrValue(node, "type");
    int state       = GetAttrValue(node, "state");
    int calcLayout  = GetAttrValue(node, "calcLayout");

    if (show) __add_isshow(data);
    else      __del_isshow(data);

    if (calcLayout) __add_isshow(data);
    else            __del_isshow(data);

    if (type != 2)  __add_iswidth(data);
    else            __del_iswidth(data);

    if (state == 2) __add_isautohide(data);
    else            __del_isautohide(data);

    data->size = GetAttrValue(node, "size");
    data->dockAlign = GetAttrValue(node, "dockAlign");
}

void Parse_group(HWND hParent, LPCSTR _nodeName, CELL_DATA* data)
{
    _str nodeName(100);
    int count = xml.GetChildNodeCount(_nodeName);
    if (!IsWindow(data->hWnd))
    {
        const DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | GS_BOTTOM;
        data->hWnd = CreateGroup(0, 0, style, data->left, data->top, data->width, data->height, hParent, 0, 0);
        SendMessageW(data->hWnd, WM_SETFONT, (WPARAM)CWndBase::DefaultFont(), 0);
        SendMessageW(data->hWnd, TCM_SETPADDING, 0, MAKELONG(5, 4));

        if (m_cell_wnd->left == data->left)
            SendMessageW(data->hWnd, TCM_INSERT_TOPSTATE, 1, TOP_STATE_RESERVE_RIGHT);
        else if (data->parent->childCount > 2)
            SendMessageW(data->hWnd, TCM_INSERT_TOPSTATE, 1, TOP_STATE_RESERVE_LEFT);
        
    }
    TabCtrl_DeleteAllItems(data->hWnd);

    TCITEMW item = { 0 };
    item.mask = TCIF_TEXT | TCIF_PARAM;
    wstr nameW(100);
    for (int i = 0; i < count; i++)
    {
        nodeName.assign(_nodeName).append("/@").append(i + 1);
        int id = GetAttrValue(nodeName.c_str(), "id");
        MAP_INFO& map = m_map[id];
        HWND hWnd = map.hWnd;
        _str::dbg("id = %d, ���ھ�� = %d, type=%d, size=%d, show=%d\n", id, hWnd, __iswidth(data), data->size, __isshow(data));
        item.pszText = (LPWSTR)map.name;
        item.lParam = (LPARAM)id;
        TabCtrl_InsertItem(data->hWnd, i, &item);

        SetParent(hWnd, data->hWnd);
        RECT rc;
        GetClientRect(hWnd, &rc);
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;
        int left = 10, top = 30;
        if (data->width > data->height)         // ������
            left = 10 + (i * 10) + i * cx;
        else
            top = 30 + (i * 10) + i * cy;       // ������

        SetWindowPos(hWnd, 0, left, top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | (i ? SWP_HIDEWINDOW : 0));
    }
}

void Parse_pang(HWND hParent, LPCSTR _nodeName, CELL_DATA* data)
{
    DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | GS_NOGROP | (__isshow(data) ? WS_VISIBLE : 0);
    int id = GetAttrValue(_nodeName, "id");
    if (!IsWindow(data->hWnd))
    {
        data->hWnd = CreateGroup(0, 0, style, data->left, data->top, data->width, data->height, hParent, id, 0);
        SendMessageW(data->hWnd, TCM_SETPADDING, 0, MAKELONG(10, 0));

        if (m_cell_wnd->left == data->left)
            SendMessageW(data->hWnd, TCM_INSERT_TOPSTATE, 1, TOP_STATE_RESERVE_RIGHT);
        else if (data->parent->childCount > 2)
            SendMessageW(data->hWnd, TCM_INSERT_TOPSTATE, 1, TOP_STATE_RESERVE_LEFT);
    }
    MAP_INFO& map = m_map[id];
    HWND hWnd = map.hWnd;

    SetParent(hWnd, data->hWnd);
    SetWindowPos(hWnd, 0, 10, 50, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

}

void Parse_view(HWND hParent, LPCSTR _nodeName, CELL_DATA* data)
{
    MAP_INFO& map = m_map[10014];
    HWND hWnd = map.hWnd;

    SetWindowPos(hWnd, 0, data->left, data->top, data->width, data->height, SWP_NOZORDER);

}

// ���������ײ���������
// isTop = �Ƿ��Ǵ������ĸ���, ѭ����i==0���Ƕ���
// info = ��ǰ���ֵ�����, �����new����������, �Ѿ���xml���ȡ��������
// parentData = ����Ŀ����, m_cell_wnd ���ָ��
inline void _parse_top_bottom_cell(bool isTop, CELL_DATA* info, CELL_DATA* parentData)
{
    // ��������������, ��Ҫ��������, �����������ڵ㱣�浽ȫ�ֱ�����, ������ʱ����
    // �������ھͷ�Ϊ������������, ��Ҫ������ڵĻ����ڴ������������������ڷ������
    if (isTop)
    {
        // ����ĸ���
        m_cell_top = info;
        m_cell_wnd->height  = m_cell_top->size;

        m_cell_top->left    = parentData->left;
        m_cell_top->top     = parentData->top;
        m_cell_top->width   = parentData->width;
        m_cell_top->height  = m_cell_top->size;
        return;
    }

    // ����ĸ���
    m_cell_bottom = info;
    m_cell_wnd->height      += m_cell_bottom->size;

    m_cell_bottom->left     = parentData->left;
    m_cell_bottom->top      = parentData->top + m_cell_top->height;
    m_cell_bottom->width    = parentData->width;
    m_cell_bottom->height   = m_cell_bottom->size;
}
// ������������λ��
// info = ��ǰcell�ڵ������, �����new����������, �Ѿ���xml���ȡ��������
// parentData = ����Ŀ����
// i = ��ǰ��Ŀ�ڸ���Ŀ�е�����
inline void _parse_cell_calc_child(CELL_DATA* info, CELL_DATA* parentData)
{
    //type=2 �߶�, type=3 ���
    info->left = parentData->left + parentData->childLeft;
    info->top  = parentData->top;
    if (__iswidth(info))
    {
        // ��¼���ǿ��, �������ߵ��ۼ�
        info->width  = info->size + WINDOW_SPACING;
        info->height = parentData->height;
        if (info->size == 630)
        {
            info->width = (parentData->width / parentData->childCount);
        }
        if (info->index == parentData->childCount - 1)
        {
            info->width = parentData->width - info->left + parentData->left;
        }

        if (__isshow(info))
        {
            // ����ʾ״̬, ��Ҫ�����������߸��޸���
            parentData->childLeft += info->width;
        }
        else
        {
            // ������ʾ״̬
        }

        return;
    }

    // ��¼���Ǹ߶�, ��Ҫ�ж��ǲ����Զ�����߶�, 630=�Զ�����߶�
    if (__isautohide(info))
    {
        // state == 2 ���Զ�����
        // ���Զ�����, ��Ҫ�ж���ʾ����߻����ұ�
        //info->width = 20;
    }
    info->width = parentData->width - info->left;
    info->height = info->size;
    parentData->childLeft = info->width;
}

// hParent = ����cell�ĸ�����
// _nodeName = ��ǰcell�Ľڵ�·��
// data = ��ǰcell������
// level = ��ǰcell�Ĳ��
void Parse_cell(HWND hParent, LPCSTR _nodeName, CELL_DATA* data)
{
    _str nodeName(100);
    data->childCount = xml.GetChildNodeCount(_nodeName);
    tinyxml2::XMLNode* node;

    data->hWnd = hParent;
#ifdef _DEBUG
    if (data->level == 0 && data->childCount > 2)
        __debugbreak();
#endif

    for (int i = 0; i < data->childCount; i++)
    {
        nodeName.assign(_nodeName).append("/@").append(i + 1);
        xml.GetNode(nodeName.c_str(), node);
        assert(node);
        CELL_DATA* info = new CELL_DATA;
        memset(info, 0, sizeof(CELL_DATA));
        info->index = i;
        _xml_getvalue(node, info);
        data->child = info;
        if (data->level == 0)
        {
            _parse_top_bottom_cell(i == 0, info, data);
        }
        else
        {
            _parse_cell_calc_child(info, data);
        }
        info->parent = data;
        info->level = data->level + 1;

        LPCSTR name = xml.GetNodeName(node);
        if (strcmp(name, "cell") == 0)
        {
            Parse_cell(data->hWnd, nodeName.c_str(), info);
        }
        else if (strcmp(name, "group") == 0)
        {
            Parse_group(data->hWnd, nodeName.c_str(), info);
        }
        else if (strcmp(name, "pane") == 0)
        {
            Parse_pang(data->hWnd, nodeName.c_str(), info);
        }
        else if (strcmp(name, "view") == 0)
        {
            Parse_view(data->hWnd, nodeName.c_str(), info);
        }
    }
    //SetWindowPos(data.hWnd, 0, 0, 0, data.width, data.height, SWP_NOZORDER | SWP_NOMOVE);
}



void wnd_create(HWND hWnd)
{
    RECT rcWindow;
    GetClientRect(hWnd, &rcWindow);
    const int cxClient = rcWindow.right - rcWindow.left;
    const int cyClient = rcWindow.bottom - rcWindow.top;

    int count = xml.GetChildNodeCount("@1");
    _str nodeName(100);
    tinyxml2::XMLNode* node;
    for (int i = 0; i < count; i++)
    {
        nodeName.assign("@1/@").append(i + 1);
        xml.GetNode(nodeName.c_str(), node);
        assert(node);
        LPCSTR name = xml.GetNodeName(node);

        if (strcmp(name, "cell") == 0)
        {
            m_cell_wnd = new CELL_DATA;
            memset(m_cell_wnd, 0, sizeof(CELL_DATA));

            m_cell_wnd->left = WINDOW_SPACING;
            m_cell_wnd->top = 90;    //TODO ��Ҫ���㶥��Ԥ���߶�
            m_cell_wnd->width = cxClient - WINDOW_SPACING * 2;
            m_cell_wnd->height = 0;
            _xml_getvalue(node, m_cell_wnd);
            Parse_cell(hWnd, nodeName.c_str(), m_cell_wnd);
        }
        else if (strcmp(name, "") == 0)
        {
            CELL_DATA data = { 0 };

        }

    }
}