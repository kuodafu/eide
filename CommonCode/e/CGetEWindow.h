#pragma once
#include <windows.h>

struct EWINDOW_MDICHILD
{
    HWND hMDIClient1;   // MDI下的第一层窗口
    HWND hMDIClient2;   // MDI下的第二层窗口
    HWND hMDIClient3;   // MDI下的第三层窗口, 可以根据这个窗口判断是否为起始页或者设计器窗口或者代码区
    HWND hMDIClient4;   // MDI下的第四层窗口, 如果是代码区, 这个窗口就是代码窗口
    HWND hVScroll;      // 纵向滚动条
    HWND hHScroll;      // 横向滚动条
};

class IGetEWindow   // 接口类
{
public:
    virtual HWND WINAPI GetEWindow() = 0;

    // 获取菜单条窗口句柄, 包含顶部的运行/对齐/菜单条
    virtual HWND WINAPI GetMenu() = 0;

    // 获取菜单条容器下的菜单条窗口句柄
    virtual HWND WINAPI GetMenuMenu() = 0;
    // 定位工具条
    virtual HWND WINAPI GetMenuLocation() = 0;
    // 对齐工具条
    virtual HWND WINAPI GetMenuAlign() = 0;
    // 标准工具条
    virtual HWND WINAPI GetMenuStandard() = 0;
    // 定位工具条左边的组合框
    virtual HWND WINAPI GetMenuComboBox1() = 0;
    // 定位工具条右边的组合框
    virtual HWND WINAPI GetMenuComboBox2() = 0;
    // 查询指定窗口是否为菜单下的窗口
    virtual bool WINAPI isMenuWindow(HWND hWnd) = 0;


    // 获取代码区下面的选择夹句柄
    virtual HWND WINAPI GetCodeTab() = 0;

    // 获取代码框的窗口句柄, 代码框里包含所有打开的标签
    virtual HWND WINAPI GetMDIClient() = 0;

    // 查询指定窗口是否为MDI下的窗口
    virtual bool WINAPI isMDIClientWindow(HWND hWnd) = 0;


    //! 选择夹相关命令

    // 获取状态夹的窗口句柄, 状态夹包含 输出/提示选择夹/ 输出框/提示框
    virtual HWND WINAPI GetState() = 0;

    // 获取状态夹的选择夹句柄, 选择夹夹包含 输出/提示选择夹/ 输出框/提示框
    virtual HWND WINAPI GetStateTab() = 0;

    // 选择夹的父窗口, 需要填充背景
    virtual HWND WINAPI GetState32770() = 0;

    // 选择夹的父窗口的父窗口, 需要填充背景
    virtual HWND WINAPI GetState32770Parent() = 0;

    // 获取状态夹里的输出编辑框句柄
    virtual HWND WINAPI GetStateTabOutput() = 0;

    // 获取状态夹里的提示编辑框句柄
    virtual HWND WINAPI GetStateTabTips() = 0;

    // 获取状态夹里的变量表输入编辑框句柄
    virtual HWND WINAPI GetStateTabVarTableEdit() = 0;

    // 获取调用表树型框
    virtual HWND WINAPI GetStateTabCallTable() = 0;

    // 变量表堆栈查看组合框
    virtual HWND WINAPI GetStateStackCombobox() = 0;

    // 变量数据类型, 长度, 比如 数据类型: 字节集 / 长度: 0字节
    virtual HWND WINAPI GetStateTypeSize() = 0;

    // 获取变量表树型框
    virtual HWND WINAPI GetStateVarTableTree() = 0;

    // 获取剪辑历史超级列表框
    virtual HWND WINAPI GetStateTabClip() = 0;

    // 获取搜寻1列表框句柄
    virtual HWND WINAPI GetStateTabFind1() = 0;

    // 获取搜寻2列表框句柄
    virtual HWND WINAPI GetStateTabFind2() = 0;

    // 获取监视表超级列表框
    virtual HWND WINAPI GetStateTabMonitorTable() = 0;

    // 获取变量表写入按钮
    virtual HWND WINAPI GetStateTabVarTableWriteBtn() = 0;

    // 获取变量表标签, 仅调试中断状态下有效
    virtual HWND WINAPI GetStateTabVarTableStatic() = 0;

    // 查询指定窗口是否为状态夹下的窗口
    virtual bool WINAPI isStateWindow(HWND hWnd) = 0;


    //! 工作夹相关命令


    // 获取工作夹的窗口句柄, 包含 支持库/程序/属性 选择夹 支持库/程序 树型框
    virtual HWND WINAPI GetWork() = 0;

    // 获取工作夹的选择夹窗口句柄, 选择夹的子窗口是 支持库/程序/属性 选择夹 支持库/程序 树型框
    virtual HWND WINAPI GetWorkTab() = 0;

    // 选择夹的父窗口, 需要填充背景
    virtual HWND WINAPI GetWork32770() = 0;

    // 选择夹的父窗口的父窗口, 需要填充背景
    virtual HWND WINAPI GetWork32770Parent() = 0;

    // 获取程序的树型框句柄, id=1019
    virtual HWND WINAPI GetWorkProgramTree() = 0;

    // 获取支持库的树型框句柄, id=1022
    virtual HWND WINAPI GetWorkFneTree() = 0;

    // 获取事件组合框, 属性框下面
    virtual HWND WINAPI GetWorkEventComboBox() = 0;

    // 获取组件组合框, 属性框上面
    virtual HWND WINAPI GetWorkControlComboBox() = 0;

    // 获取属性框
    virtual HWND WINAPI GetWorkProperty() = 0;

    // 获取属性框下面的编辑框
    virtual HWND WINAPI GetWorkEdit() = 0;

    // 查询指定窗口是否为工作夹下的窗口
    virtual bool WINAPI isWorkWindow(HWND hWnd) = 0;

    //! 组件箱相关命令


    // 获取组件箱的窗口句柄
    virtual HWND WINAPI GetControl() = 0;
    virtual HWND WINAPI GetControl32770() = 0;
    virtual HWND WINAPI GetControl32770Parent() = 0;
    virtual HWND WINAPI GetControlTree() = 0;

    // 查询指定窗口是否为组件箱下的窗口
    virtual bool WINAPI isControlWindow(HWND hWnd) = 0;

    // 获取底部状态条
    virtual HWND WINAPI GetBottomStatusBar() = 0;
    // 获取底部状态条里的向上箭头按钮
    virtual HWND WINAPI GetBottomStatusBarBtn() = 0;
    // 获取底部状态条里的进度条
    virtual HWND WINAPI GetBottomStatusBarProgress() = 0;
    // 获取MDI下第一个子窗口, 会排除其他不是易语言创建的窗口
    virtual HWND GetMDIClient1() = 0;
    // 从MDI下第一层窗口里获取所有易语言自带的子窗口
    // hMDIClient1 = MDI下的第一层窗口, 如果这个句柄为0, 则自动获取第一个MDI下的窗口
    virtual bool GetMDIClientChildWindow(HWND hMDIClient1, EWINDOW_MDICHILD& WndChild) = 0;
};

class CGetEWindow : public IGetEWindow
{
    // 菜单条分类下的句柄
    struct E_MENU_DATA
    {
        HWND hWnd;      // 顶部的菜单栏容器句柄

        HWND menu;      // 菜单条, 这个不是工具条
        HWND location;  // 定位工具条, 里面是两个组合框, 这个工具条只需要绘画背景
        HWND align;     // 对齐工具条
        HWND normal;    // 标准工具条
        HWND hCombox1;  // 定位工具条下的第一个组合框, id = 629
        HWND hCombox2;  // 定位工具条下的第二个组合框, id = 630

        E_MENU_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        E_MENU_DATA& operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            HWND hChild = GetWindow(hWnd, GW_CHILD);
            while (hChild)
            {
                const int id = GetDlgCtrlID(hChild);
                switch (id)
                {
                case 59396:     // 菜单条
                    this->menu = hChild;
                    break;
                case 135:       // 定位工具条
                {
                    this->location = hChild;
                    HWND hCom = GetWindow(this->location, GW_CHILD);
                    while (hCom)
                    {
                        const int idCom = GetDlgCtrlID(hCom);
                        if (idCom == 629)
                            this->hCombox1 = hCom;
                        else if (idCom == 630)
                            this->hCombox2 = hCom;
                        hCom = GetWindow(hCom, GW_HWNDNEXT);
                    }
                    break;
                }
                case 133:       // 对齐工具条
                    this->align = hChild;
                    break;
                case 59392:     // 标准工具条
                    this->normal = hChild;
                    break;
                default:
                    break;
                }
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
            return *this;
        }
    };
    
    // 代码区分类下的句柄, MDIClient
    struct E_MDICLIENT_DATA
    {
        HWND hWnd;      // 代码区最外层的窗口句柄, 这个句柄的父窗口是易语言主窗口

        E_MDICLIENT_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        E_MDICLIENT_DATA& operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            return *this;
        }
    };
    
    // 状态栏容器, 调试输出, 提示, 变量表 都在这个容器里
    struct E_STATE_DATA
    {
        HWND hWnd;      // 底部状态条容器的窗口句柄, 这个句柄的父窗口是易语言主窗口
        HWND hTab;      // 选择夹句柄, 选择夹包括输出/提示/变量表等组件的句柄
        HWND hTab2;     // 暂时还不知道干嘛用
        HWND h32770;    // 选择夹父窗口
        HWND h32770P;   // 选择夹父窗口的父窗口, 需要填充背景


        HWND hTips;             // 提示编辑框
        HWND hOutput;           // 输出编辑框
        HWND hVarTableEdit;     // 变量表编辑框

        HWND hCallTable;        // 调用表树型框
        HWND hVarTableTree;     // 变量表树型框

        HWND hVarTableStatic;   // 变量表标签, 仅调试中断状态下有效, 
        HWND hVarTabTypeSize;   // 变量数据类型, 长度, 比如 数据类型: 字节集 / 长度: 0字节

        HWND hVarTableWriteBtn; // 变量表写入按钮

        HWND hClip;             // 剪辑历史超级列表框
        HWND hMonitorTable;     // 监视表超级列表框
        HWND hFind1;            // 搜寻1列表框句柄
        HWND hFind2;            // 搜寻2列表框句柄

        HWND hCombobox;         // 变量表堆栈查看组合框

        E_STATE_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        void operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            if (!IsWindow(hWnd))return;
            HWND hChild = GetWindow(hWnd, GW_CHILD);   // 状态夹 -> 状态夹
            if (!hChild)return;

            int id = GetDlgCtrlID(hChild);
            while (id != 130 && id)
            {
                hChild = GetWindow(hChild, GW_HWNDNEXT);
                id = GetDlgCtrlID(hChild);
            }
            if (!hChild)return;
            this->h32770P = hChild;

            hChild = GetWindow(hChild, GW_CHILD);   // #32770
            if (!hChild)return;
            this->h32770 = hChild;

            HWND hTab = GetWindow(hChild, GW_CHILD);
            while (hTab)
            {
                id = GetDlgCtrlID(hTab);
                if (id == 1000)
                    this->hTab = hTab;
                else if (id == 1003)
                    this->hTab2 = hTab;
                hTab = GetWindow(hTab, GW_HWNDNEXT);
            }
            if (!this->hTab)return;

            // 这里开始枚举选择夹下面的组件
            hChild = GetWindow(this->hTab, GW_CHILD);
            while (hChild)
            {
                id = GetDlgCtrlID(hChild);
                switch (id)
                {
                case 1001:  // 提示编辑框
                    this->hTips = hChild;
                    break;
                case 1011:  // 输出编辑框
                    this->hOutput = hChild;
                    break;
                case 2033:  // 调用表树型框, 5.5版
                case 1224:  // 调用表树型框
                    this->hCallTable = hChild;
                    break;
                case 1187:  // 监视表超级列表框
                    this->hMonitorTable = hChild;
                    break;
                case 1188:  // 变量表标签, 仅调试中断状态下有效, 
                    this->hVarTableStatic = hChild;
                    break;
                case 1113:  // 变量表写入按钮
                    this->hVarTableWriteBtn = hChild;
                    break;
                case 1010:  // 变量表编辑框
                    this->hVarTableEdit = hChild;
                    break;
                case 1182:  // 搜寻1列表框句柄
                    this->hFind1 = hChild;
                    break;
                case 1183:  // 搜寻2列表框句柄
                    this->hFind2 = hChild;
                    break;
                case 1199:  // 剪辑历史超级列表框
                    this->hClip = hChild;
                    break;

                case 1167:  // 组合框
                    this->hCombobox = hChild;
                    break;
                case 1109:  // 变量表树型框
                    this->hVarTableTree = hChild;
                    break;
                case 1115:  // 标签
                    this->hVarTabTypeSize = hChild;
                    break;
                default:
                    break;
                }
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
            return;
        }
    };
    
    // 工作夹容器, 支持库树型框/程序树型框, 属性 都是在这个容器里
    struct E_WORK_DATA
    {
        HWND hWnd;          // 底部状态条容器的窗口句柄, 这个句柄的父窗口是易语言主窗口
        HWND hTab;          // 选择夹句柄, 选择夹包括支持库的树型框, 程序的树型框, 还有属性的两个组合框
        HWND hTab2;         // 暂时还不知道干嘛用
        HWND h32770;        // 选择夹父窗口
        HWND h32770P;       // 选择夹父窗口的父窗口, 需要填充背景

        HWND hProperty;     // 易语言自己的属性框
        HWND hEdit;         // 属性框下面的编辑框
        HWND hEvent;        // 事件组合框
        HWND hControl;      // 组件组合框
        HWND hFneTree;      // 支持库的树型框句柄
        HWND hProgram;      // 程序的树型框句柄

        inline HWND WINAPI GetEdit()
        {
            if (!hEdit)
                hEdit = GetWindow(hProperty, GW_CHILD);
            return hEdit;
        }
        E_WORK_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        void operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            if (!IsWindow(hWnd))return;
            HWND hChild = GetWindow(hWnd, GW_CHILD);   // 工作夹 -> 工作夹
            if (!hChild)return;

            int id = GetDlgCtrlID(hChild);
            while (id != 129 && id)
            {
                hChild = GetWindow(hChild, GW_HWNDNEXT);
                id = GetDlgCtrlID(hChild);
            }
            if (!hChild)return;

            this->h32770P = hChild;

            hChild = GetWindow(hChild, GW_CHILD);   // #32770
            if (!hChild)return;

            this->h32770 = hChild;
            HWND hTab = GetWindow(hChild, GW_CHILD);
            while (hTab)
            {
                id = GetDlgCtrlID(hTab);
                if (id == 1000)
                    this->hTab = hTab;
                else if (id == 1002)
                    this->hTab2 = hTab;
                hTab = GetWindow(hTab, GW_HWNDNEXT);
            }
            if (!this->hTab)return;

            // 这里开始枚举选择夹下面的组件
            hChild = GetWindow(this->hTab, GW_CHILD);
            while (hChild)
            {
                id = GetDlgCtrlID(hChild);
                switch (id)
                {
                case 118:   // 易语言自己的属性框
                    this->hProperty = hChild;
                    break;
                case 1059:  // 事件组合框
                    this->hEvent = hChild;
                    break;
                case 1058:  // 组件组合框
                    this->hControl = hChild;
                    break;
                case 1022:  // 支持库的树型框句柄
                    this->hFneTree = hChild;
                    break;
                case 1019:  // 程序的树型框句柄
                    this->hProgram = hChild;
                    break;
                default:
                    break;
                }
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
        }
        
    };

    // 组件箱容器
    struct E_CONTROL_DATA
    {
        HWND hWnd;          // 底部状态条容器的窗口句柄, 这个句柄的父窗口是易语言主窗口
        HWND hTree;         // 组件箱树型框
        HWND h32770;        // 组件箱父窗口
        HWND h32770P;       // 组件箱父窗口的父窗口, 需要填充背景

        E_CONTROL_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        void operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            if (!IsWindow(hWnd))return;
            HWND hChild = GetWindow(hWnd, GW_CHILD);   // 组件箱 -> 组件箱
            if (!hChild)return;

            int id = GetDlgCtrlID(hChild);
            while (id != 110 && id)
            {
                hChild = GetWindow(hChild, GW_HWNDNEXT);
                id = GetDlgCtrlID(hChild);
            }
            if (!hChild)return;

            this->h32770P = hChild;
            hChild = GetWindow(hChild, GW_CHILD);   // #32770
            if (!hChild)return;

            this->h32770 = hChild;
            hChild = GetWindow(hChild, GW_CHILD);
            while (hChild)
            {
                id = GetDlgCtrlID(hChild);
                switch (id)
                {
                case 1225:  // 组件树型框
                    this->hTree = hChild;
                    break;
                }
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
        }

    };

    // 底部状态条容器
    struct E_BOTTOM_STATE_DATA
    {
        HWND hWnd;      // 底部状态条容器的窗口句柄, 这个句柄的父窗口是易语言主窗口
        HWND hWndbtn;   // 上箭头按钮
        HWND hProgress; // 进度条

        E_BOTTOM_STATE_DATA() { memset(this, 0, sizeof(*this)); }
        inline operator HWND() { return hWnd; }
        inline void operator=(HWND hWnd)
        {
            if (!IsWindow(hWnd))return;
            this->hWnd = hWnd;
            HWND hChild1 = GetWindow(hWnd, GW_CHILD);   // 
            HWND hChild2 = GetWindow(hChild1, GW_HWNDNEXT);

            int id = GetDlgCtrlID(hChild1);
            if (id == 113)
            {
                this->hWndbtn = hChild1;
                this->hProgress = hChild2;
            }
            else
            {
                this->hWndbtn = hChild2;
                this->hProgress = hChild1;
            }
        }

    };
    struct WNDDATA
    {
        HWND                hWnd;           // 易语言主窗口句柄
        E_MENU_DATA         menu;           // 易语言菜单栏容器, 菜单, 标准工具条都在这容器里
        E_MDICLIENT_DATA    MDIClient;      // 显示源码的容器, 起始页和写代码的都在这个区域
        E_STATE_DATA        state;          // 状态夹容器, 调试输出, 提示, 变量表 都在这个容器里
        E_WORK_DATA         work;           // 工作夹容器, 支持库树型框在这里
        E_CONTROL_DATA      control;        // 组件箱容器句柄
        E_BOTTOM_STATE_DATA bottomState;    // 底部状态条
        HWND                codeTab;        // 底部状态条

        WNDDATA() : hWnd(0), codeTab(0) { }
        operator HWND() { return hWnd; }
        WNDDATA& operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            return *this;
        }
    };
    WNDDATA m_data;    // 易语言主窗口句柄
public:
    CGetEWindow(HWND hWnd) { SetEWindow(hWnd); }
    CGetEWindow()  { }
    ~CGetEWindow() { }

    // 返回类内部记录的窗口
    inline HWND WINAPI GetEWindow() { return m_data; }
    inline operator HWND() { return m_data; }
    inline void operator()(HWND hWnd) { SetEWindow(hWnd); }
    inline void operator=(HWND hWnd) { SetEWindow(hWnd); }

public:
    // 获取菜单条窗口句柄, 包含顶部的运行/对齐/菜单条
    inline HWND WINAPI GetMenu() { return m_data.menu; }

    // 获取菜单条容器下的菜单条窗口句柄
    inline HWND WINAPI GetMenuMenu() { return m_data.menu.menu; }
    // 定位工具条
    inline HWND WINAPI GetMenuLocation() { return m_data.menu.location; }
    // 对齐工具条
    inline HWND WINAPI GetMenuAlign() { return m_data.menu.align; }
    // 标准工具条
    inline HWND WINAPI GetMenuStandard() { return m_data.menu.normal; }
    // 定位工具条左边的组合框
    inline HWND WINAPI GetMenuComboBox1() { return m_data.menu.hCombox1; }
    // 定位工具条右边的组合框
    inline HWND WINAPI GetMenuComboBox2() { return m_data.menu.hCombox2; }
    // 查询指定窗口是否为菜单下的窗口
    inline bool WINAPI isMenuWindow(HWND hWnd)
    {
        if (!hWnd)return false;
        HWND hWndOld = hWnd;
        do
        {
            if (m_data.menu == hWnd)return true;
            hWnd = GetParent(hWnd);
        } while (hWnd);

        hWnd = hWndOld;
        return hWnd == GetMenu() || hWnd == GetMenuMenu() || hWnd == GetMenuLocation() ||
            hWnd == GetMenuAlign() || hWnd == GetMenuStandard() || hWnd == GetMenuComboBox1() ||
            hWnd == GetMenuComboBox2();
        return false;
    }


public:

    // 获取代码区下面的选择夹句柄
    inline HWND WINAPI GetCodeTab() { return m_data.codeTab; }

    // 获取代码框的窗口句柄, 代码框里包含所有打开的标签
    inline HWND WINAPI GetMDIClient() { return m_data.MDIClient; }

    // 查询指定窗口是否为MDI下的窗口
    inline bool WINAPI isMDIClientWindow(HWND hWnd)
    {
        HWND hParent = hWnd;    // MDIClient 窗口都是往上找4层
        for (int i = 0; i < 4 && hParent; i++)
        {
            if (hParent == m_data.MDIClient)
                return true;
            hParent = GetParent(hParent);
        }
        return false;
        
        if (hParent == m_data.MDIClient)
            return true;

        do
        {
            if (m_data.MDIClient == hWnd)return true;
            hWnd = GetParent(hWnd);
        } while (hWnd);

        return false;
    }

public:
    //! 选择夹相关命令

    // 获取状态夹的窗口句柄, 状态夹包含 输出/提示选择夹/ 输出框/提示框
    inline HWND WINAPI GetState() { return m_data.state; }

    // 获取状态夹的选择夹句柄, 选择夹夹包含 输出/提示选择夹/ 输出框/提示框
    inline HWND WINAPI GetStateTab() { return m_data.state.hTab; }

    // 选择夹的父窗口, 需要填充背景
    inline HWND WINAPI GetState32770() { return m_data.state.h32770; }

    // 选择夹的父窗口的父窗口, 需要填充背景
    inline HWND WINAPI GetState32770Parent() { return m_data.state.h32770P; }

    // 获取状态夹里的输出编辑框句柄
    inline HWND WINAPI GetStateTabOutput() { return m_data.state.hOutput; }
    
    // 获取状态夹里的提示编辑框句柄
    inline HWND WINAPI GetStateTabTips() { return m_data.state.hTips; }

    // 获取状态夹里的变量表输入编辑框句柄
    inline HWND WINAPI GetStateTabVarTableEdit() { return m_data.state.hVarTableEdit; }

    // 获取调用表树型框
    inline HWND WINAPI GetStateTabCallTable() { return m_data.state.hCallTable; }

    // 变量表堆栈查看组合框
    inline HWND WINAPI GetStateStackCombobox() { return m_data.state.hCombobox; }

    // 变量数据类型, 长度, 比如 数据类型: 字节集 / 长度: 0字节
    inline HWND WINAPI GetStateTypeSize() { return m_data.state.hVarTabTypeSize; }

    // 获取变量表树型框
    inline HWND WINAPI GetStateVarTableTree() { return m_data.state.hVarTableTree; }

    // 获取剪辑历史超级列表框
    inline HWND WINAPI GetStateTabClip() { return m_data.state.hClip; }

    // 获取搜寻1列表框句柄
    inline HWND WINAPI GetStateTabFind1() { return m_data.state.hFind1; }

    // 获取搜寻2列表框句柄
    inline HWND WINAPI GetStateTabFind2() { return m_data.state.hFind2; }

    // 获取监视表超级列表框
    inline HWND WINAPI GetStateTabMonitorTable() { return m_data.state.hMonitorTable; }

    // 获取变量表写入按钮
    inline HWND WINAPI GetStateTabVarTableWriteBtn() { return m_data.state.hVarTableWriteBtn; }

    // 获取变量表标签, 仅调试中断状态下有效
    inline HWND WINAPI GetStateTabVarTableStatic() { return m_data.state.hVarTableStatic; }

    // 查询指定窗口是否为状态夹下的窗口
    inline bool WINAPI isStateWindow(HWND hWnd)
    {
        //return hWnd == GetState() || hWnd == GetStateTab() || hWnd == GetState32770() ||
        //    hWnd == GetState32770Parent() || hWnd == GetStateTabOutput() || hWnd == GetStateTabTips() ||
        //    hWnd == GetStateTabVarTableEdit() || hWnd == GetStateTabCallTable() || hWnd == GetStateStackCombobox() ||
        //    hWnd == GetStateTypeSize() || hWnd == GetStateVarTableTree() || hWnd == GetStateTabClip() ||
        //    hWnd == GetStateTabFind1() || hWnd == GetStateTabFind2() || hWnd == GetStateTabMonitorTable() ||
        //    hWnd == GetStateTabVarTableWriteBtn() || hWnd == GetStateTabVarTableStatic();
        do
        {
            if (m_data.state == hWnd)return true;
            hWnd = GetParent(hWnd);
        } while (hWnd);
        return false;
    }


public:
    //! 工作夹相关命令


    // 获取工作夹的窗口句柄, 包含 支持库/程序/属性 选择夹 支持库/程序 树型框
    inline HWND WINAPI GetWork() { return m_data.work; }

    // 获取工作夹的选择夹窗口句柄, 选择夹的子窗口是 支持库/程序/属性 选择夹 支持库/程序 树型框
    inline HWND WINAPI GetWorkTab() { return m_data.work.hTab; }

    // 选择夹的父窗口, 需要填充背景
    inline HWND WINAPI GetWork32770() { return m_data.work.h32770; }

    // 选择夹的父窗口的父窗口, 需要填充背景
    inline HWND WINAPI GetWork32770Parent() { return m_data.work.h32770P; }

    // 获取程序的树型框句柄, id=1019
    inline HWND WINAPI GetWorkProgramTree() { return m_data.work.hProgram; }

    // 获取支持库的树型框句柄, id=1022
    inline HWND WINAPI GetWorkFneTree() { return m_data.work.hFneTree; }

    // 获取事件组合框, 属性框下面
    inline HWND WINAPI GetWorkEventComboBox() { return m_data.work.hEvent; }

    // 获取组件组合框, 属性框上面
    inline HWND WINAPI GetWorkControlComboBox() { return m_data.work.hControl; }

    // 获取属性框
    inline HWND WINAPI GetWorkProperty() { return m_data.work.hProperty; }

    // 获取属性框下面的编辑框
    
    inline HWND WINAPI GetWorkEdit() { return m_data.work.GetEdit(); }

    // 查询指定窗口是否为工作夹下的窗口
    inline bool WINAPI isWorkWindow(HWND hWnd)
    {
        //return hWnd == GetWork() || hWnd == GetWork32770() || hWnd == GetWork32770Parent() ||
        //    hWnd == GetWorkControlComboBox() || hWnd == GetWorkEventComboBox() || hWnd == GetWorkFneTree() ||
        //    hWnd == GetWorkProgramTree() || hWnd == GetWorkProperty() || hWnd == GetWorkProperty();
        do
        {
            if (m_data.work == hWnd)return true;
            hWnd = GetParent(hWnd);
        } while (hWnd);
        return false;

    }



public:
    //! 组件箱相关命令


    // 获取组件箱的窗口句柄, 包含 支持库/程序/属性 选择夹 支持库/程序 树型框
    inline HWND WINAPI GetControl() { return m_data.control; }
    inline HWND WINAPI GetControl32770() { return m_data.control.h32770; }
    inline HWND WINAPI GetControl32770Parent() { return m_data.control.h32770P; }
    inline HWND WINAPI GetControlTree() { return m_data.control.hTree; }

    // 查询指定窗口是否为组件箱下的窗口
    inline bool WINAPI isControlWindow(HWND hWnd)
    {
        do
        {
            if (m_data.control == hWnd)return true;
            hWnd = GetParent(hWnd);
        } while (hWnd);
        return false;

    }
public:
    // 获取底部状态条
    inline HWND WINAPI GetBottomStatusBar() { return m_data.bottomState; }
    // 获取底部状态条里的向上箭头按钮
    inline HWND WINAPI GetBottomStatusBarBtn() { return m_data.bottomState.hWndbtn; }
    // 获取底部状态条里的进度条
    inline HWND WINAPI GetBottomStatusBarProgress() { return m_data.bottomState.hProgress; }

    // 获取MDI下第一个子窗口, 会排除其他不是易语言创建的窗口
    inline HWND GetMDIClient1()
    {
        HWND hWnd = GetWindow(m_data.MDIClient, GW_CHILD);
        while (hWnd)
        {
            int id = GetDlgCtrlID(hWnd);
            if (id >= 65280 && id < 66000)  // 第一层窗口是 65280 开始
                break;
            hWnd = GetWindow(hWnd, GW_HWNDNEXT);
        }
        return hWnd;
    }
    // 从MDI下第一层窗口里获取所有易语言自带的子窗口
    // hMDIClient1 = MDI下的第一层窗口, 如果这个句柄为0, 则自动获取第一个MDI下的窗口
    inline bool GetMDIClientChildWindow(HWND hMDIClient1, EWINDOW_MDICHILD& WndChild)
    {
        if (!hMDIClient1)
            hMDIClient1 = GetMDIClient1();
        if (!hMDIClient1)
            return false;

        WndChild = { 0 };
        WndChild.hMDIClient1 = hMDIClient1;
        HWND hWnd = GetWindow(hMDIClient1, GW_CHILD);
        while (hWnd)
        {
            int id = GetDlgCtrlID(hWnd);
            if (id == 59648)
                break;
            hWnd = GetWindow(hWnd, GW_HWNDNEXT);
        }
        if (!hWnd) return false;
        WndChild.hMDIClient2 = hWnd;
        hWnd = GetWindow(hWnd, GW_CHILD);

        int id3 = 0;    // 第三层窗口的ID
        while (hWnd)
        {
            int id = GetDlgCtrlID(hWnd);
            switch (id)
            {
            case 59904:     // 横向滚动条
                WndChild.hHScroll = hWnd;
                break;
            case 59920:     // 纵向滚动条
                WndChild.hVScroll = hWnd;
                break;
            case 59936:     // 不知道干嘛的滚动条
                break;

            case 1:         // 要么是设计器, 要么是起始页
            case 59648:     // 代码窗口的父窗口, 里面就的代码窗口了
                WndChild.hMDIClient3 = hWnd;
                id3 = id;
                break;
            default:
                break;
            }
            if (WndChild.hHScroll && WndChild.hVScroll && WndChild.hMDIClient3)
                break;
            hWnd = GetWindow(hWnd, GW_HWNDNEXT);
        }
        hWnd = WndChild.hMDIClient3;
        if (!hWnd) return false;

        if (id3 == 59648)   // 是代码窗口
        {
            hWnd = GetWindow(hWnd, GW_CHILD);
            while (hWnd)
            {
                int id = GetDlgCtrlID(hWnd);
                if (id == 1)
                    break;
                hWnd = GetWindow(hWnd, GW_HWNDNEXT);
            }
            WndChild.hMDIClient4 = hWnd;
        }
        else
        {
            WndChild.hMDIClient4 = GetWindow(hWnd, GW_CHILD);
        }
        return true;
    }


private:
    // 设置新的易语言主窗口, 所有的取句柄操作都是从这个窗口进行查找
    inline void SetEWindow(HWND hEWnd) {
        m_data = hEWnd;
        HWND hChild = GetWindow(m_data, GW_CHILD);
        while (hChild)
        {
            int id = GetDlgCtrlID(hChild);
            switch (id)
            {
            case 59648: // 代码区
                m_data.MDIClient = hChild;
                break;
            case 59393: // 底部状态栏
                m_data.bottomState = hChild;
                break;
            case 59419: // 菜单条
            {
                //if (!GetDlgItem(hChild, 59392))
                //    SendMessageA(hEWnd, WM_COMMAND, 59392, 0);

                //if(!GetDlgItem(hChild, 133))
                //    SendMessageA(hEWnd, WM_COMMAND, 133, 0);

                m_data.menu = hChild;
                break;
            }
            case 59422: // 状态夹
                m_data.state = hChild;
                break;
            case 59420: // 工作夹
                m_data.work = hChild;
                break;
            case 59421: // 组件箱
                m_data.control = hChild;
                break;
            case 59392: // 代码区的选择夹
                m_data.codeTab = hChild;
                break;
            default:
                break;
            }
            hChild = GetWindow(hChild, GW_HWNDNEXT);
        }
        return;
    }

};

