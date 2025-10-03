#pragma once
#include <windows.h>

struct EWINDOW_MDICHILD
{
    HWND hMDIClient1;   // MDI�µĵ�һ�㴰��
    HWND hMDIClient2;   // MDI�µĵڶ��㴰��
    HWND hMDIClient3;   // MDI�µĵ����㴰��, ���Ը�����������ж��Ƿ�Ϊ��ʼҳ������������ڻ��ߴ�����
    HWND hMDIClient4;   // MDI�µĵ��Ĳ㴰��, ����Ǵ�����, ������ھ��Ǵ��봰��
    HWND hVScroll;      // ���������
    HWND hHScroll;      // ���������
};

class IGetEWindow   // �ӿ���
{
public:
    virtual HWND WINAPI GetEWindow() = 0;

    // ��ȡ�˵������ھ��, ��������������/����/�˵���
    virtual HWND WINAPI GetMenu() = 0;

    // ��ȡ�˵��������µĲ˵������ھ��
    virtual HWND WINAPI GetMenuMenu() = 0;
    // ��λ������
    virtual HWND WINAPI GetMenuLocation() = 0;
    // ���빤����
    virtual HWND WINAPI GetMenuAlign() = 0;
    // ��׼������
    virtual HWND WINAPI GetMenuStandard() = 0;
    // ��λ��������ߵ���Ͽ�
    virtual HWND WINAPI GetMenuComboBox1() = 0;
    // ��λ�������ұߵ���Ͽ�
    virtual HWND WINAPI GetMenuComboBox2() = 0;
    // ��ѯָ�������Ƿ�Ϊ�˵��µĴ���
    virtual bool WINAPI isMenuWindow(HWND hWnd) = 0;


    // ��ȡ�����������ѡ��о��
    virtual HWND WINAPI GetCodeTab() = 0;

    // ��ȡ�����Ĵ��ھ��, �������������д򿪵ı�ǩ
    virtual HWND WINAPI GetMDIClient() = 0;

    // ��ѯָ�������Ƿ�ΪMDI�µĴ���
    virtual bool WINAPI isMDIClientWindow(HWND hWnd) = 0;


    //! ѡ����������

    // ��ȡ״̬�еĴ��ھ��, ״̬�а��� ���/��ʾѡ���/ �����/��ʾ��
    virtual HWND WINAPI GetState() = 0;

    // ��ȡ״̬�е�ѡ��о��, ѡ��ма��� ���/��ʾѡ���/ �����/��ʾ��
    virtual HWND WINAPI GetStateTab() = 0;

    // ѡ��еĸ�����, ��Ҫ��䱳��
    virtual HWND WINAPI GetState32770() = 0;

    // ѡ��еĸ����ڵĸ�����, ��Ҫ��䱳��
    virtual HWND WINAPI GetState32770Parent() = 0;

    // ��ȡ״̬���������༭����
    virtual HWND WINAPI GetStateTabOutput() = 0;

    // ��ȡ״̬�������ʾ�༭����
    virtual HWND WINAPI GetStateTabTips() = 0;

    // ��ȡ״̬����ı���������༭����
    virtual HWND WINAPI GetStateTabVarTableEdit() = 0;

    // ��ȡ���ñ����Ϳ�
    virtual HWND WINAPI GetStateTabCallTable() = 0;

    // �������ջ�鿴��Ͽ�
    virtual HWND WINAPI GetStateStackCombobox() = 0;

    // ������������, ����, ���� ��������: �ֽڼ� / ����: 0�ֽ�
    virtual HWND WINAPI GetStateTypeSize() = 0;

    // ��ȡ���������Ϳ�
    virtual HWND WINAPI GetStateVarTableTree() = 0;

    // ��ȡ������ʷ�����б��
    virtual HWND WINAPI GetStateTabClip() = 0;

    // ��ȡ��Ѱ1�б����
    virtual HWND WINAPI GetStateTabFind1() = 0;

    // ��ȡ��Ѱ2�б����
    virtual HWND WINAPI GetStateTabFind2() = 0;

    // ��ȡ���ӱ����б��
    virtual HWND WINAPI GetStateTabMonitorTable() = 0;

    // ��ȡ������д�밴ť
    virtual HWND WINAPI GetStateTabVarTableWriteBtn() = 0;

    // ��ȡ�������ǩ, �������ж�״̬����Ч
    virtual HWND WINAPI GetStateTabVarTableStatic() = 0;

    // ��ѯָ�������Ƿ�Ϊ״̬���µĴ���
    virtual bool WINAPI isStateWindow(HWND hWnd) = 0;


    //! �������������


    // ��ȡ�����еĴ��ھ��, ���� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    virtual HWND WINAPI GetWork() = 0;

    // ��ȡ�����е�ѡ��д��ھ��, ѡ��е��Ӵ����� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    virtual HWND WINAPI GetWorkTab() = 0;

    // ѡ��еĸ�����, ��Ҫ��䱳��
    virtual HWND WINAPI GetWork32770() = 0;

    // ѡ��еĸ����ڵĸ�����, ��Ҫ��䱳��
    virtual HWND WINAPI GetWork32770Parent() = 0;

    // ��ȡ��������Ϳ���, id=1019
    virtual HWND WINAPI GetWorkProgramTree() = 0;

    // ��ȡ֧�ֿ�����Ϳ���, id=1022
    virtual HWND WINAPI GetWorkFneTree() = 0;

    // ��ȡ�¼���Ͽ�, ���Կ�����
    virtual HWND WINAPI GetWorkEventComboBox() = 0;

    // ��ȡ�����Ͽ�, ���Կ�����
    virtual HWND WINAPI GetWorkControlComboBox() = 0;

    // ��ȡ���Կ�
    virtual HWND WINAPI GetWorkProperty() = 0;

    // ��ȡ���Կ�����ı༭��
    virtual HWND WINAPI GetWorkEdit() = 0;

    // ��ѯָ�������Ƿ�Ϊ�������µĴ���
    virtual bool WINAPI isWorkWindow(HWND hWnd) = 0;

    //! ������������


    // ��ȡ�����Ĵ��ھ��
    virtual HWND WINAPI GetControl() = 0;
    virtual HWND WINAPI GetControl32770() = 0;
    virtual HWND WINAPI GetControl32770Parent() = 0;
    virtual HWND WINAPI GetControlTree() = 0;

    // ��ѯָ�������Ƿ�Ϊ������µĴ���
    virtual bool WINAPI isControlWindow(HWND hWnd) = 0;

    // ��ȡ�ײ�״̬��
    virtual HWND WINAPI GetBottomStatusBar() = 0;
    // ��ȡ�ײ�״̬��������ϼ�ͷ��ť
    virtual HWND WINAPI GetBottomStatusBarBtn() = 0;
    // ��ȡ�ײ�״̬����Ľ�����
    virtual HWND WINAPI GetBottomStatusBarProgress() = 0;
    // ��ȡMDI�µ�һ���Ӵ���, ���ų��������������Դ����Ĵ���
    virtual HWND GetMDIClient1() = 0;
    // ��MDI�µ�һ�㴰�����ȡ�����������Դ����Ӵ���
    // hMDIClient1 = MDI�µĵ�һ�㴰��, ���������Ϊ0, ���Զ���ȡ��һ��MDI�µĴ���
    virtual bool GetMDIClientChildWindow(HWND hMDIClient1, EWINDOW_MDICHILD& WndChild) = 0;
};

class CGetEWindow : public IGetEWindow
{
    // �˵��������µľ��
    struct E_MENU_DATA
    {
        HWND hWnd;      // �����Ĳ˵����������

        HWND menu;      // �˵���, ������ǹ�����
        HWND location;  // ��λ������, ������������Ͽ�, ���������ֻ��Ҫ�滭����
        HWND align;     // ���빤����
        HWND normal;    // ��׼������
        HWND hCombox1;  // ��λ�������µĵ�һ����Ͽ�, id = 629
        HWND hCombox2;  // ��λ�������µĵڶ�����Ͽ�, id = 630

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
                case 59396:     // �˵���
                    this->menu = hChild;
                    break;
                case 135:       // ��λ������
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
                case 133:       // ���빤����
                    this->align = hChild;
                    break;
                case 59392:     // ��׼������
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
    
    // �����������µľ��, MDIClient
    struct E_MDICLIENT_DATA
    {
        HWND hWnd;      // �����������Ĵ��ھ��, �������ĸ�������������������

        E_MDICLIENT_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        E_MDICLIENT_DATA& operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            return *this;
        }
    };
    
    // ״̬������, �������, ��ʾ, ������ �������������
    struct E_STATE_DATA
    {
        HWND hWnd;      // �ײ�״̬�������Ĵ��ھ��, �������ĸ�������������������
        HWND hTab;      // ѡ��о��, ѡ��а������/��ʾ/�����������ľ��
        HWND hTab2;     // ��ʱ����֪��������
        HWND h32770;    // ѡ��и�����
        HWND h32770P;   // ѡ��и����ڵĸ�����, ��Ҫ��䱳��


        HWND hTips;             // ��ʾ�༭��
        HWND hOutput;           // ����༭��
        HWND hVarTableEdit;     // ������༭��

        HWND hCallTable;        // ���ñ����Ϳ�
        HWND hVarTableTree;     // ���������Ϳ�

        HWND hVarTableStatic;   // �������ǩ, �������ж�״̬����Ч, 
        HWND hVarTabTypeSize;   // ������������, ����, ���� ��������: �ֽڼ� / ����: 0�ֽ�

        HWND hVarTableWriteBtn; // ������д�밴ť

        HWND hClip;             // ������ʷ�����б��
        HWND hMonitorTable;     // ���ӱ����б��
        HWND hFind1;            // ��Ѱ1�б����
        HWND hFind2;            // ��Ѱ2�б����

        HWND hCombobox;         // �������ջ�鿴��Ͽ�

        E_STATE_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        void operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            if (!IsWindow(hWnd))return;
            HWND hChild = GetWindow(hWnd, GW_CHILD);   // ״̬�� -> ״̬��
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

            // ���￪ʼö��ѡ�����������
            hChild = GetWindow(this->hTab, GW_CHILD);
            while (hChild)
            {
                id = GetDlgCtrlID(hChild);
                switch (id)
                {
                case 1001:  // ��ʾ�༭��
                    this->hTips = hChild;
                    break;
                case 1011:  // ����༭��
                    this->hOutput = hChild;
                    break;
                case 2033:  // ���ñ����Ϳ�, 5.5��
                case 1224:  // ���ñ����Ϳ�
                    this->hCallTable = hChild;
                    break;
                case 1187:  // ���ӱ����б��
                    this->hMonitorTable = hChild;
                    break;
                case 1188:  // �������ǩ, �������ж�״̬����Ч, 
                    this->hVarTableStatic = hChild;
                    break;
                case 1113:  // ������д�밴ť
                    this->hVarTableWriteBtn = hChild;
                    break;
                case 1010:  // ������༭��
                    this->hVarTableEdit = hChild;
                    break;
                case 1182:  // ��Ѱ1�б����
                    this->hFind1 = hChild;
                    break;
                case 1183:  // ��Ѱ2�б����
                    this->hFind2 = hChild;
                    break;
                case 1199:  // ������ʷ�����б��
                    this->hClip = hChild;
                    break;

                case 1167:  // ��Ͽ�
                    this->hCombobox = hChild;
                    break;
                case 1109:  // ���������Ϳ�
                    this->hVarTableTree = hChild;
                    break;
                case 1115:  // ��ǩ
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
    
    // ����������, ֧�ֿ����Ϳ�/�������Ϳ�, ���� ���������������
    struct E_WORK_DATA
    {
        HWND hWnd;          // �ײ�״̬�������Ĵ��ھ��, �������ĸ�������������������
        HWND hTab;          // ѡ��о��, ѡ��а���֧�ֿ�����Ϳ�, ��������Ϳ�, �������Ե�������Ͽ�
        HWND hTab2;         // ��ʱ����֪��������
        HWND h32770;        // ѡ��и�����
        HWND h32770P;       // ѡ��и����ڵĸ�����, ��Ҫ��䱳��

        HWND hProperty;     // �������Լ������Կ�
        HWND hEdit;         // ���Կ�����ı༭��
        HWND hEvent;        // �¼���Ͽ�
        HWND hControl;      // �����Ͽ�
        HWND hFneTree;      // ֧�ֿ�����Ϳ���
        HWND hProgram;      // ��������Ϳ���

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
            HWND hChild = GetWindow(hWnd, GW_CHILD);   // ������ -> ������
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

            // ���￪ʼö��ѡ�����������
            hChild = GetWindow(this->hTab, GW_CHILD);
            while (hChild)
            {
                id = GetDlgCtrlID(hChild);
                switch (id)
                {
                case 118:   // �������Լ������Կ�
                    this->hProperty = hChild;
                    break;
                case 1059:  // �¼���Ͽ�
                    this->hEvent = hChild;
                    break;
                case 1058:  // �����Ͽ�
                    this->hControl = hChild;
                    break;
                case 1022:  // ֧�ֿ�����Ϳ���
                    this->hFneTree = hChild;
                    break;
                case 1019:  // ��������Ϳ���
                    this->hProgram = hChild;
                    break;
                default:
                    break;
                }
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
        }
        
    };

    // ���������
    struct E_CONTROL_DATA
    {
        HWND hWnd;          // �ײ�״̬�������Ĵ��ھ��, �������ĸ�������������������
        HWND hTree;         // ��������Ϳ�
        HWND h32770;        // ����丸����
        HWND h32770P;       // ����丸���ڵĸ�����, ��Ҫ��䱳��

        E_CONTROL_DATA() { memset(this, 0, sizeof(*this)); }
        operator HWND() { return hWnd; }
        void operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            if (!IsWindow(hWnd))return;
            HWND hChild = GetWindow(hWnd, GW_CHILD);   // ����� -> �����
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
                case 1225:  // ������Ϳ�
                    this->hTree = hChild;
                    break;
                }
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
        }

    };

    // �ײ�״̬������
    struct E_BOTTOM_STATE_DATA
    {
        HWND hWnd;      // �ײ�״̬�������Ĵ��ھ��, �������ĸ�������������������
        HWND hWndbtn;   // �ϼ�ͷ��ť
        HWND hProgress; // ������

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
        HWND                hWnd;           // �����������ھ��
        E_MENU_DATA         menu;           // �����Բ˵�������, �˵�, ��׼������������������
        E_MDICLIENT_DATA    MDIClient;      // ��ʾԴ�������, ��ʼҳ��д����Ķ����������
        E_STATE_DATA        state;          // ״̬������, �������, ��ʾ, ������ �������������
        E_WORK_DATA         work;           // ����������, ֧�ֿ����Ϳ�������
        E_CONTROL_DATA      control;        // ������������
        E_BOTTOM_STATE_DATA bottomState;    // �ײ�״̬��
        HWND                codeTab;        // �ײ�״̬��

        WNDDATA() : hWnd(0), codeTab(0) { }
        operator HWND() { return hWnd; }
        WNDDATA& operator=(HWND hWnd)
        {
            this->hWnd = hWnd;
            return *this;
        }
    };
    WNDDATA m_data;    // �����������ھ��
public:
    CGetEWindow(HWND hWnd) { SetEWindow(hWnd); }
    CGetEWindow()  { }
    ~CGetEWindow() { }

    // �������ڲ���¼�Ĵ���
    inline HWND WINAPI GetEWindow() { return m_data; }
    inline operator HWND() { return m_data; }
    inline void operator()(HWND hWnd) { SetEWindow(hWnd); }
    inline void operator=(HWND hWnd) { SetEWindow(hWnd); }

public:
    // ��ȡ�˵������ھ��, ��������������/����/�˵���
    inline HWND WINAPI GetMenu() { return m_data.menu; }

    // ��ȡ�˵��������µĲ˵������ھ��
    inline HWND WINAPI GetMenuMenu() { return m_data.menu.menu; }
    // ��λ������
    inline HWND WINAPI GetMenuLocation() { return m_data.menu.location; }
    // ���빤����
    inline HWND WINAPI GetMenuAlign() { return m_data.menu.align; }
    // ��׼������
    inline HWND WINAPI GetMenuStandard() { return m_data.menu.normal; }
    // ��λ��������ߵ���Ͽ�
    inline HWND WINAPI GetMenuComboBox1() { return m_data.menu.hCombox1; }
    // ��λ�������ұߵ���Ͽ�
    inline HWND WINAPI GetMenuComboBox2() { return m_data.menu.hCombox2; }
    // ��ѯָ�������Ƿ�Ϊ�˵��µĴ���
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

    // ��ȡ�����������ѡ��о��
    inline HWND WINAPI GetCodeTab() { return m_data.codeTab; }

    // ��ȡ�����Ĵ��ھ��, �������������д򿪵ı�ǩ
    inline HWND WINAPI GetMDIClient() { return m_data.MDIClient; }

    // ��ѯָ�������Ƿ�ΪMDI�µĴ���
    inline bool WINAPI isMDIClientWindow(HWND hWnd)
    {
        HWND hParent = hWnd;    // MDIClient ���ڶ���������4��
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
    //! ѡ����������

    // ��ȡ״̬�еĴ��ھ��, ״̬�а��� ���/��ʾѡ���/ �����/��ʾ��
    inline HWND WINAPI GetState() { return m_data.state; }

    // ��ȡ״̬�е�ѡ��о��, ѡ��ма��� ���/��ʾѡ���/ �����/��ʾ��
    inline HWND WINAPI GetStateTab() { return m_data.state.hTab; }

    // ѡ��еĸ�����, ��Ҫ��䱳��
    inline HWND WINAPI GetState32770() { return m_data.state.h32770; }

    // ѡ��еĸ����ڵĸ�����, ��Ҫ��䱳��
    inline HWND WINAPI GetState32770Parent() { return m_data.state.h32770P; }

    // ��ȡ״̬���������༭����
    inline HWND WINAPI GetStateTabOutput() { return m_data.state.hOutput; }
    
    // ��ȡ״̬�������ʾ�༭����
    inline HWND WINAPI GetStateTabTips() { return m_data.state.hTips; }

    // ��ȡ״̬����ı���������༭����
    inline HWND WINAPI GetStateTabVarTableEdit() { return m_data.state.hVarTableEdit; }

    // ��ȡ���ñ����Ϳ�
    inline HWND WINAPI GetStateTabCallTable() { return m_data.state.hCallTable; }

    // �������ջ�鿴��Ͽ�
    inline HWND WINAPI GetStateStackCombobox() { return m_data.state.hCombobox; }

    // ������������, ����, ���� ��������: �ֽڼ� / ����: 0�ֽ�
    inline HWND WINAPI GetStateTypeSize() { return m_data.state.hVarTabTypeSize; }

    // ��ȡ���������Ϳ�
    inline HWND WINAPI GetStateVarTableTree() { return m_data.state.hVarTableTree; }

    // ��ȡ������ʷ�����б��
    inline HWND WINAPI GetStateTabClip() { return m_data.state.hClip; }

    // ��ȡ��Ѱ1�б����
    inline HWND WINAPI GetStateTabFind1() { return m_data.state.hFind1; }

    // ��ȡ��Ѱ2�б����
    inline HWND WINAPI GetStateTabFind2() { return m_data.state.hFind2; }

    // ��ȡ���ӱ����б��
    inline HWND WINAPI GetStateTabMonitorTable() { return m_data.state.hMonitorTable; }

    // ��ȡ������д�밴ť
    inline HWND WINAPI GetStateTabVarTableWriteBtn() { return m_data.state.hVarTableWriteBtn; }

    // ��ȡ�������ǩ, �������ж�״̬����Ч
    inline HWND WINAPI GetStateTabVarTableStatic() { return m_data.state.hVarTableStatic; }

    // ��ѯָ�������Ƿ�Ϊ״̬���µĴ���
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
    //! �������������


    // ��ȡ�����еĴ��ھ��, ���� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    inline HWND WINAPI GetWork() { return m_data.work; }

    // ��ȡ�����е�ѡ��д��ھ��, ѡ��е��Ӵ����� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    inline HWND WINAPI GetWorkTab() { return m_data.work.hTab; }

    // ѡ��еĸ�����, ��Ҫ��䱳��
    inline HWND WINAPI GetWork32770() { return m_data.work.h32770; }

    // ѡ��еĸ����ڵĸ�����, ��Ҫ��䱳��
    inline HWND WINAPI GetWork32770Parent() { return m_data.work.h32770P; }

    // ��ȡ��������Ϳ���, id=1019
    inline HWND WINAPI GetWorkProgramTree() { return m_data.work.hProgram; }

    // ��ȡ֧�ֿ�����Ϳ���, id=1022
    inline HWND WINAPI GetWorkFneTree() { return m_data.work.hFneTree; }

    // ��ȡ�¼���Ͽ�, ���Կ�����
    inline HWND WINAPI GetWorkEventComboBox() { return m_data.work.hEvent; }

    // ��ȡ�����Ͽ�, ���Կ�����
    inline HWND WINAPI GetWorkControlComboBox() { return m_data.work.hControl; }

    // ��ȡ���Կ�
    inline HWND WINAPI GetWorkProperty() { return m_data.work.hProperty; }

    // ��ȡ���Կ�����ı༭��
    
    inline HWND WINAPI GetWorkEdit() { return m_data.work.GetEdit(); }

    // ��ѯָ�������Ƿ�Ϊ�������µĴ���
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
    //! ������������


    // ��ȡ�����Ĵ��ھ��, ���� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    inline HWND WINAPI GetControl() { return m_data.control; }
    inline HWND WINAPI GetControl32770() { return m_data.control.h32770; }
    inline HWND WINAPI GetControl32770Parent() { return m_data.control.h32770P; }
    inline HWND WINAPI GetControlTree() { return m_data.control.hTree; }

    // ��ѯָ�������Ƿ�Ϊ������µĴ���
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
    // ��ȡ�ײ�״̬��
    inline HWND WINAPI GetBottomStatusBar() { return m_data.bottomState; }
    // ��ȡ�ײ�״̬��������ϼ�ͷ��ť
    inline HWND WINAPI GetBottomStatusBarBtn() { return m_data.bottomState.hWndbtn; }
    // ��ȡ�ײ�״̬����Ľ�����
    inline HWND WINAPI GetBottomStatusBarProgress() { return m_data.bottomState.hProgress; }

    // ��ȡMDI�µ�һ���Ӵ���, ���ų��������������Դ����Ĵ���
    inline HWND GetMDIClient1()
    {
        HWND hWnd = GetWindow(m_data.MDIClient, GW_CHILD);
        while (hWnd)
        {
            int id = GetDlgCtrlID(hWnd);
            if (id >= 65280 && id < 66000)  // ��һ�㴰���� 65280 ��ʼ
                break;
            hWnd = GetWindow(hWnd, GW_HWNDNEXT);
        }
        return hWnd;
    }
    // ��MDI�µ�һ�㴰�����ȡ�����������Դ����Ӵ���
    // hMDIClient1 = MDI�µĵ�һ�㴰��, ���������Ϊ0, ���Զ���ȡ��һ��MDI�µĴ���
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

        int id3 = 0;    // �����㴰�ڵ�ID
        while (hWnd)
        {
            int id = GetDlgCtrlID(hWnd);
            switch (id)
            {
            case 59904:     // ���������
                WndChild.hHScroll = hWnd;
                break;
            case 59920:     // ���������
                WndChild.hVScroll = hWnd;
                break;
            case 59936:     // ��֪������Ĺ�����
                break;

            case 1:         // Ҫô�������, Ҫô����ʼҳ
            case 59648:     // ���봰�ڵĸ�����, ����͵Ĵ��봰����
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

        if (id3 == 59648)   // �Ǵ��봰��
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
    // �����µ�������������, ���е�ȡ����������Ǵ�������ڽ��в���
    inline void SetEWindow(HWND hEWnd) {
        m_data = hEWnd;
        HWND hChild = GetWindow(m_data, GW_CHILD);
        while (hChild)
        {
            int id = GetDlgCtrlID(hChild);
            switch (id)
            {
            case 59648: // ������
                m_data.MDIClient = hChild;
                break;
            case 59393: // �ײ�״̬��
                m_data.bottomState = hChild;
                break;
            case 59419: // �˵���
            {
                //if (!GetDlgItem(hChild, 59392))
                //    SendMessageA(hEWnd, WM_COMMAND, 59392, 0);

                //if(!GetDlgItem(hChild, 133))
                //    SendMessageA(hEWnd, WM_COMMAND, 133, 0);

                m_data.menu = hChild;
                break;
            }
            case 59422: // ״̬��
                m_data.state = hChild;
                break;
            case 59420: // ������
                m_data.work = hChild;
                break;
            case 59421: // �����
                m_data.control = hChild;
                break;
            case 59392: // ��������ѡ���
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

