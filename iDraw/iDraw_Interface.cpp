#if 1
#include "EWindow_Fne_Main.h"
#include <elib/lib2.h>
#include <control/draw_Window_include.h>


// �����ṩ�Ľӿ�

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ֧�ֿ�İ汾��
// 
// ����:
//      ����1: �������汾��ָ��, ����Ϊ0
//      ����2: ���մΰ汾��ָ��, ����Ϊ0
//      ����3: ���ձ���汾��ָ��, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_GetVersion(int* pMajor, int* pMinor, int* pBuild)
{
    if (pMajor) *pMajor = IDRAW_MAJORVERSION;
    if (pMinor) *pMinor = IDRAW_MINORVERSION;
    if (pBuild) *pBuild = IDRAW_BUILDVERSION;
    return IDRAW_VERSION;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ֧�ֿ�İ汾��
// 
// ����:
//      ����1: �������汾��ָ��, ����Ϊ0
//      ����2: ���մΰ汾��ָ��, ����Ϊ0
//      ����3: ���ձ���汾��ָ��, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LPCSTR WINAPI iDraw_GetVersionA(int* pMajor, int* pMinor, int* pBuild)
{
    if (pMajor) *pMajor = IDRAW_MAJORVERSION;
    if (pMinor) *pMinor = IDRAW_MINORVERSION;
    if (pBuild) *pBuild = IDRAW_BUILDVERSION;
    return IDRAW_VERSIONA;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ�����Եİ汾��
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��ȡ����ֵ, 5.93 ����593, 5.11 ����511, 5.92�������汾, �᷵��592��5922
EXTERN_C _declspec(dllexport) int WINAPI iDraw_GetEVersion()
{
    return s_info->EVersion;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��֧�ֿ��������ڴ�, �ڴ��Ǵ� iDraw.fne ���ģ���������, �ڲ����õ��� malloc
// 
// ����:
//      ����1: ����ĳߴ�
// 
// ����ֵ:
//      ������ڴ��ַ, ������0, ����ʧ�ܲ����׳��쳣
EXTERN_C _declspec(dllexport) void* WINAPI iDraw_malloc(int size)
{
    return malloc(size);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ͷ� iDraw_malloc ������ڴ�, �����ͷŵ��� iDraw.fne ���ģ����ڴ�, �ڲ����õ��� free
//      ���������ģ��ȥ�ͷ� iDraw_malloc ������ڴ�, һ�����һ���Ѳ���ͬ���쳣
// 
// ����:
//      ����1: iDraw_malloc ���صĵ�ַ
// 
// ����ֵ:
//      �޷���ֵ
EXTERN_C _declspec(dllexport) void WINAPI iDraw_free(void* ptr)
{
    if (ptr)
        free(ptr);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ���໯���ص�ָ�������, ���������໯������Ϣ�ص�������ĵ��������
// 
// ����:
//      ����1: ���໯���ص�ָ��, ���������໯������Ϣ�ص�������ĵ��������
//      ����2: ��������, ������ο� IGL_ ��ͷ����
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_AppendMenu(LPOBJSTRUCT pData, int index)
{

    return 0;
}

inline ICTinyXml2_XMLNode _get_node(LPCWSTR lpszNodeName, LPCWSTR lpszNodeValue, BOOL isCreate, BOOL* lpIsNewNode)
{

}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡxmlָ���ڵ�, ���ؽڵ�ָ��, ���ж�д��������Ҫ�ڵ�ָ��
// 
// ����:
//      ����1: xml�ڵ���
//      ����2: �ڵ�ֵ�ı�, ����Ϊ0
//      ����3: �ڵ㲻����ʱ�Ƿ񴴽��ýڵ�, ����Ϊ0
//      ����4: ����һ��ָ����շ��صĽڵ��Ƿ����´�����, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) ICTinyXml2_XMLNode WINAPI iDraw_Xml_GetNode(LPCWSTR lpszNodeName, LPCWSTR lpszNodeValue, BOOL isCreate, BOOL* lpIsNewNode)
{
    //if (!lpszNodeName || !lpszNodeName[0]) return 0;
    //if (lpIsNewNode)*lpIsNewNode = FALSE;
    //ICTinyXml2_XMLNode node;
    //wchar_t nodeName[100];
    //swprintf_s(nodeName, 100, L"@1/%s", lpszNodeName);
    //if (!s_info->xml->GetNode(nodeName, node))
    //{
    //    if (!isCreate) return 0;
    //    // �ߵ��������Ҫ�����µĽڵ�
    //    s_info->xml->InsertChildNode(L"@1", lpszNodeName, lpszNodeValue, 0, &node);
    //    if (!node) return 0;
    //    if (lpIsNewNode)*lpIsNewNode = TRUE;
    //}
    //return node;
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡxmlָ���ڵ��µ��ӽڵ�, ���ؽڵ�ָ��, ���ж�д��������Ҫ�ڵ�ָ��
// 
// ����:
//      ����1: xml�ڵ�, ������ֵΪ0, ������ iDraw_Xml_GetNode() һ��
//      ����2: Ҫ��ȡ���ӽڵ�ڵ���, ���Ϊ�������ʧ��
//      ����3: �ڵ�ֵ�ı�, ����Ϊ0
//      ����4: �ڵ㲻����ʱ�Ƿ񴴽��ýڵ�, ����Ϊ0
//      ����5: ����һ��ָ����շ��صĽڵ��Ƿ����´�����, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) ICTinyXml2_XMLNode WINAPI iDraw_Xml_GetChildNode(ICTinyXml2_XMLNode nodeParent, LPCWSTR lpszNodeName, LPCWSTR lpszNodeValue, BOOL isCreate, BOOL* lpIsNewNode)
{
    return 0;
    //if (!lpszNodeName || !lpszNodeName[0]) return 0;
    //if (!nodeParent)
    //    return iDraw_Xml_GetNode(lpszNodeName, lpszNodeValue, isCreate, lpIsNewNode);
    //if (lpIsNewNode)*lpIsNewNode = FALSE;
    //_str nodeNameA = wstr::W2A(lpszNodeName);
    //ICTinyXml2_XMLNode node = s_info->xml->FirstChildElement(nodeParent, nodeNameA.c_str());
    //if (!node)  // û������ڵ�, ��Ҫ�ж��Ƿ񴴽�����ڵ�
    //{
    //    if (!isCreate) return 0;
    //    // �ߵ��������Ҫ�����µĽڵ�
    //    s_info->xml->InsertChildNode(nodeParent, lpszNodeName, lpszNodeValue, 0, &node);
    //    if (!node) return 0;
    //    if (lpIsNewNode)*lpIsNewNode = TRUE;
    //}
    //return node;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ӽڵ㵽ĳ���ڵ���, �������ӵĽڵ��Ѿ�����, �򲻻����, ���ؽڵ�ָ��
// 
// ����:
//      ����1: xml�ڵ�ָ��
//      ����2: Ҫ�����xml�ڵ���
//      ����3: Ҫ�����xml�ڵ�ֵ, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) ICTinyXml2_XMLNode WINAPI iDraw_Xml_InsertChildNode(ICTinyXml2_XMLNode pNode, LPCWSTR lpszNodeName, LPCWSTR pszNodeValue)
{
    return 0;
    //if (!pNode || !lpszNodeName || !lpszNodeName[0])return 0;
    //_str nodeNameA = wstr::W2A(lpszNodeName);

    //ICTinyXml2_XMLNode node = s_info->xml->FirstChildElement(pNode, nodeNameA.c_str());

    //if (!node)  // û������ڵ�, ��Ҫ����һ��
    //{
    //    ICTinyXml2_XMLElement child = s_info->xml->NewElement(nodeNameA.c_str());
    //    if (pszNodeValue)s_info->xml->SetText(child, wstr::W2A(pszNodeValue).c_str());
    //    node = s_info->xml->InsertEndChild(pNode, child);
    //}
    //return node;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ָ���ڵ�����ֵ
// 
// ����:
//      ����1: xml�ڵ���
//      ����2: Ҫ���õ�������
//      ����3: Ҫ���õ�����ֵ
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Xml_SetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, LPCWSTR lpszAttrValue)
{
    return 0;
    //if (!node || !lpszAttrName || !lpszAttrName[0])return false;
    //return s_info->xml->SetAttrValue(node, lpszAttrName, lpszAttrValue);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡָ���ڵ�����ֵ, ��������ֵ�ı�ָ��
// 
// ����:
//      ����1: xml�ڵ���
//      ����2: Ҫ��ȡ��������
//      ����3: Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_Xml_GetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, BOOL* isAttribute)
{
    return 0;
    //if (!node || !lpszAttrName || !lpszAttrName[0])return 0;
    //return s_info->xml->GetAttrValue(node, lpszAttrName, isAttribute);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ָ���ڵ�ֵ
// 
// ����:
//      ����1: xml�ڵ�ָ��
//      ����2: Ҫ���õĽڵ�ֵ
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Xml_SetNodeValue(ICTinyXml2_XMLNode node, LPCWSTR lpszNodeValue)
{
    return 0;
    //if (!node)return false;
    //return s_info->xml->SetNodeValue(node, lpszNodeValue);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����xml������
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Xml_SaveFile()
{
    return 0;
    //return s_info->xml->SaveFile(); // ֧�ֿ�ӿڱ���xml������
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡָ���ڵ�ֵ, ���ؽڵ�ֵ�ı�ָ��
// 
// ����:
//      ����1: xml�ڵ�ָ��
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_Xml_GetNodeValue(ICTinyXml2_XMLNode node)
{
    return 0;
    //if (!node)return 0;
    //return s_info->xml->GetNodeValue(node);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯ָ������, ������һЩĬ�ϵĻ滭��Ϣ, �ڲ������������ػ�
//      ֧�ֿ��������໯����ͨ������ӿڽ��ж��η�װ
// ����:
//      ����1: ��Ҫ���໯�Ĵ��ھ��
//      ����2: �������໯��Ϣ�ص��ĺ���, ��5������, ��������Լ��Ϊ __stdcall, ����ֵΪ LRESULT ����
//              ǰ4����������ͨ���໯��ͬ, ������Ǹ��������ǵ�������ӿڵķ���ֵ
//      ����3: �Ƿ���Ҫɾ���߿�, Ϊtrue��ѱ߿���ص���ʽ��ɾ��
//      ����4: �Ƿ������໭ˢ, ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_SubclassWindow(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder, bool isSetBack)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->_subclass(hWnd, proc, reMoveBorder, isSetBack, 0);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ѵ��ڵ���Ϣת���ػ����ڵĹ���ȥ����
// ����:
//      ����1: ��Ҫ�ػ��Ĵ��ھ��
//      ����2: ��Ҫ�������Ϣ
//      ����3: ����1
//      ����4: ����2
//      ����5: ����2
//      ����6: ����ԭ��������Ϣ�Ļص���ַ
//      ����7: ���մ��������Ϣ����ֵ
// ����ֵ:
//      �����Ƿ�����������Ϣ
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_DrawWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, LRESULT* pRet)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;

    IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT arg;
    arg.hWnd    = hWnd;
    arg.message = message;
    arg.wParam  = wParam;
    arg.lParam  = lParam;
    arg.oldProc = oldProc;
    arg.pRet    = pRet;
    return s_info->pfnControls(IDC_IDRAW_DRAW_WINDOWPROC, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡĬ�ϲ���
// ����:
//      ����1: ��Ҫ�ػ��Ĵ��ھ��, Ϊ0���ڷ��غ��Լ������ھ����ֵ
//      ����2: DRAW_WINDOW_ARGUMENT �ṹָ��, �ڲ���Ĭ�ϲ�����䵽����ṹ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_DrawGetDefArg(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!s_info || !s_info->iDraw || !arg)
        return 0;
    return s_info->pfnControls(IDC_IDRAW_DRAW_DEFARG, (WPARAM)hWnd, (LPARAM)arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��һ�����㴰���ػ������Ӿ�����ͬ���Ӿ�Ч��
// ����:
//      ����1: ��Ҫ�ػ��Ĵ��ھ��
//      ����2: DRAW_WINDOW_ARGUMENT �ṹָ��, Ϊ0��ʹ���뵱ǰ����ƥ�����ɫ��Ϣ
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_DrawWindow(HWND hWnd, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    if (arg && (arg->cbSize == 0 || arg->cbSize > 0x7ffffff))
        return 0;
    s_info->pfnControls(IDC_IDRAW_SUBWINDOWEX, (WPARAM)hWnd, (LPARAM)arg);
    return s_info->pfnGetDataFromHwnd(hWnd);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������ʽ����, ������ں��Ӿ�������ͬ���Ӿ�Ч��
// ����:
//      ����1: ���ڵ���չ��ʽ, ������ο� CreateWindowEx ��˵��
//      ����2: ���ڵı���, Ϊ0��û�б���, unicode�ı�
//      ����3: ���ڵ���ʽ, ������ο� CreateWindowEx ��˵��
//      ����4: ��������Ļ�����λ��
//      ����5: ��������Ļ�Ķ���λ��
//      ����6: ���ڿ��
//      ����7: ���ڸ߶�
//      ����8: ������, ���Ϊ0, ��û�и�����, ������򴴽��Ĵ���ʼ�����������֮��
//      ����9: DRAW_WINDOW_ARGUMENT �ṹָ��, Ϊ0��ʹ���뵱ǰ����ƥ�����ɫ��Ϣ
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_CreateWindow(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, LPDRAW_WINDOW_ARGUMENT arg)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    if (arg && (arg->cbSize == 0 || arg->cbSize > 0x7ffffff))
        return 0;
    CREATESTRUCTW cs = { 0 };
    cs.x = x, cs.y = y, cs.cx = cx, cs.cy = cy;
    cs.lpszName = lpWindowName;
    cs.style = dwStyle;
    cs.dwExStyle = dwExStyle;
    cs.hwndParent = hWndParent;

    HWND hWnd = (HWND)s_info->pfnControls(IDC_IDRAW_CREATEWINDOW, (WPARAM)&cs, (LPARAM)arg);
    if (!hWnd)
        return 0;
    return s_info->pfnGetDataFromHwnd(hWnd);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ҽӴ��ڻ滭�¼�, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
//      �������ڻ滭 ϵͳͼ��, ����, ϵͳ��ʱ, �ᴥ���¼�
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: ���ջ滭�ص��ĺ���, ����ԭ�� = typedef bool(CALLBACK* PFN_SysDraw)(LPIDRAW_STRUCT_DRAWSTRUCT ps);
//      ����3: ���ݵ��ص�������Ĳ���
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Event_Draw(HWND hWnd, PFN_SysDraw pfn, LPARAM param)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.pfn = pfn;
    arg.param = param;
    return s_info->pfnControls(IDC_IDRAW_SYSDRAW, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ҽ�ϵͳ��ť����¼�, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: ���ջ滭�ص��ĺ���, ����ԭ�� = typedef bool(CALLBACK* PFN_SysBtnClick)(int id, BOOL isRButton, LPARAM param);
//      ����3: ���ݵ��ص�������Ĳ���
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Event_SysButtonClick(HWND hWnd, PFN_SysBtnClick pfn, LPARAM param)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.pfn = pfn;
    arg.param = param;
    return s_info->pfnControls(IDC_IDRAW_SYSBTNCLICK, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ϵͳ��ť, ϵͳ��ť�����Ǵ�������, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: ��ť��ID, �������ͬID, ���滻��ֵ
//      ����3: ��ť��ֵ
// ����ֵ:
//      ���������Ӱ�ť������, ϵͳ��ť�����Ǵ�������
EXTERN_C _declspec(dllexport) int WINAPI iDraw_AddSysButton(HWND hWnd, int id, LPARAM param)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.id = id;
    arg.param = param;
    return s_info->pfnControls(IDC_IDRAW_ADDSYSBUTTON, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ɾ��ϵͳ��ť, ϵͳ��ť�����Ǵ�������, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫɾ���İ�ťλ�û�ID
//      ����3: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      �����Ƿ�ɾ���ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_DelSysButton(HWND hWnd, int nPosition, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_DELSYSBUTTON, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ϵͳ��ť, ϵͳ��ť�����Ǵ�������, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ�����λ�û�ID, -1����뵽���(�����), 0����뵽��һ��(���ұ�)
//      ����3: Ҫ���밴ť��ID
//      ����4: ��ť��ֵ
//      ����5: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      ���ز����λ��
EXTERN_C _declspec(dllexport) int WINAPI iDraw_InsSysButton(HWND hWnd, int nPosition, int id, LPARAM param, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.id = id;
    arg.nPosition = nPosition;
    arg.param = param;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_INSSYSBUTTON, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ϵͳ��ť��Ŀ��ֵ, ��������ǰ����Ŀ��ֵ
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ���õİ�ťID��λ��
//      ����3: ��ť��ֵ
//      ����4: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      ��������ǰ����Ŀ��ֵ
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_SetButtonParam(HWND hWnd, int nPosition, LPARAM param, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    arg.param = param;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_SETBTNPARAM, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡϵͳ��ť��Ŀ��ֵ
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ��ȡ�İ�ťID��λ��
//      ����3: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      ���ذ�ť����Ŀ��ֵ
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_GetButtonParam(HWND hWnd, int nPosition, int flags)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    arg.flags = flags;
    return s_info->pfnControls(IDC_IDRAW_GETBTNPARAM, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ӱ�ť������ȡϵͳ��ťID
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ��ȡ�İ�ťλ��, ��0��ʼ
// ����ֵ:
//      ���ذ�ť��ID, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_GetButtonIdFromPos(HWND hWnd, int nPosition)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.nPosition = nPosition;
    return s_info->pfnControls(IDC_IDRAW_GETBTNID, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ӱ�ťID��ȡϵͳ��ť������, ��0��ʼ
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ��ȡ�İ�ťλ��
// ����ֵ:
//      ���ذ�ť������, ʧ�ܷ���-1
EXTERN_C _declspec(dllexport) LPARAM WINAPI iDraw_GetButtonPosFromId(HWND hWnd, int id)
{
    if (!s_info || !s_info->iDraw || !hWnd)
        return 0;
    IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT arg = { 0 };
    arg.hWnd = hWnd;
    arg.id = id;
    return s_info->pfnControls(IDC_IDRAW_GETBTNPOS, (WPARAM)0, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ԭ�ص�����
// ����:
//      ����1: ���ھ��, ��Ϣ�ص���ĵ�һ������
//      ����2: ��Ϣֵ, ��Ϣ�ص���ĵڶ�������
//      ����3: ����1, ��Ϣ�ص���ĵ���������
//      ����4: ����2, ��Ϣ�ص���ĵ��ĸ�����
//      ����4: �������, ��Ϣ�ص���ĵ��������
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LRESULT WINAPI iDraw_CallWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (!s_info)
        return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ���ť, ��ť������Ϊ Button
// ����:
//      ����1: ��ť�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����3: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Button(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubButton(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ��༭��, �༭�������Ϊ Edit, Ŀǰ���и���������޷�ָ���߿�, ���ڻ��������
// ����:
//      ����1: �༭��Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: �Ƿ��ػ�������
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Edit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubEdit(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, isScroll);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ���ǩ, ��ǩ������Ϊ static
// ����:
//      ����1: ��ǩ�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Static(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubStatic(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ�״̬��, ״̬��������Ϊ msctls_statusbar32
// ����:
//      ����1: ״̬���Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_StatusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubStatusBar(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ����Ϳ�, ���Ϳ������Ϊ SysTreeView32
// ����:
//      ����1: ���Ϳ�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: ���Ϳ����Ŀ�߶�
//      ����6: �Ƿ��ػ����Ϳ�Ĺ�����
//      ����7: �Ƿ����ȵ�Ч��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_TreeView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll, bool isHot)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubTreeView(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, ItemHeight, isScroll, isHot);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ��б��, �б�������Ϊ ListBox
// ����:
//      ����1: �б��Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: �б�����Ŀ�߶�
//      ����6: �Ƿ��ػ�������
//      ����7: �Ƿ����ȵ�Ч��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_ListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll, bool isHot)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubListBox(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight, isScroll, isHot);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ������б��, �����б�������Ϊ SysListView32
// ����:
//      ����1: �����б��Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: �����б�����Ŀ�߶�
//      ����6: �Ƿ��ػ�������
//      ����7: �Ƿ����ȵ�Ч��
//      ����8: �б���뷽ʽ
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_ListView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll, bool isHot, int fmt)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubListView(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight, isScroll, isHot, fmt);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ�������, ������������Ϊ ToolbarWindow32
// ����:
//      ����1: �������Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Tool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubTool(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}



// ��������: 
//      ���໯���ػ�ѡ���, ѡ��е�����Ϊ SysTabControl32
// ����:
//      ����1: ѡ��д��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_Tab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubTab(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ�������, ������������Ϊ msctls_updown32
// ����:
//      ����1: �������Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_msctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->Submsctls_updown32(hWnd, reMoveBorder, isSetBack, reMoveClassStyle);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ���Ͽ�, ��Ͽ������Ϊ ComboBox
// ����:
//      ����1: ��Ͽ�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: ��Ͽ���б����߶�
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_Subclass_ComboBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->SubComboBox(hWnd, reMoveBorder, isSetBack, reMoveClassStyle, nItemHeight);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ػ�ָ���ؼ��Ĺ�����, ����滭�е�����, �������пؼ���֧��
// ����:
//      ����1: ��Ҫ�ػ��������Ĵ��ھ��
//      ����2: ����ǳ����б��, ����������ǳ����б��ı���߶�, �������ر���
//      ����3: ��������Ϳ�, ��Ϊ��, ���Ϳ�����ر���
//      ����4: �Ƿ����ԭ����
// ����ֵ:
//      ���ع���������ָ��
EXTERN_C _declspec(dllexport) LPVOID WINAPI iDraw_DrawScroll(HWND hWnd, int nListViewItemHeight, bool isTreeView, bool isCallSetScrollInfo)
{
    if (!s_info || !s_info->iDraw)
        return 0;
    return s_info->iDraw->DrawScroll(hWnd, nListViewItemHeight, isTreeView, isCallSetScrollInfo);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ʼ���滭��Ϣ, �������� WM_PAINT ��Ϣ�µ���, ���׵ĺ����� iDraw_Control_EndPaint()
//      ����� BeginPaint, �ᴴ���ڴ�DC, �����ڴ�DC, DX��Ⱦ����0
//      ��������õ�������ɫ, ������ˢ, �߿򻭱ʶ�ѡ��DC
//      ��� �����ô˽ӿ�, Ҳ���԰�һ�㴦��WM_PAINT��Ϣʱ����
// 
// ����:
//      ����1: ���໯�ĵ��������, �����ǵ������໯�ӿڵķ���ֵ
//      ����2: BEGIN_END_PAINT_STRUCT �ṹָ��, �ڲ���ѳ�ʼ�������ݼ�¼������ṹ��
//      ����3: �Ƿ�ΪDX��Ⱦ, �����DX��Ⱦ, ���ڲ������DX�ĳ�ʼ������
//      ����4: �Ƿ���䱳��, ���Ϊtrue, �򷵻ص�HDC�Ѿ������˱���
// 
// ����ֵ:
//      �����ڴ�DC
EXTERN_C _declspec(dllexport) HDC WINAPI iDraw_BeginPaint(LPOBJSTRUCT pData, BEGIN_END_PAINT_STRUCT* data, bool isDxRender, bool isFillBack)
{
    if (!s_info || !s_info->iDraw || !data)
        return 0;
    return s_info->iDraw->BeginPaint(pData, *data, isDxRender, isFillBack);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ύ�滭��Ϣ, �������� WM_PAINT ��Ϣ�µ���, ���ô˺���ǰ�����Ѿ������� iDraw_Control_BeginPaint()
//      ��iDraw_Control_BeginPaint() ʱ�������ڴ�DC������������
//      ������ EndPaint �ύ�滭��Ϣ
// 
// ����:
//      ����1: BEGIN_END_PAINT_STRUCT �ṹָ��, �˺���������ýṹ������
// 
// ����ֵ:
//      �����Ƿ񿽱��ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_EndPaint(BEGIN_END_PAINT_STRUCT* data)
{
    if (!s_info || !s_info->iDraw || !data)
        return 0;
    return s_info->iDraw->EndPaint(*data);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡDX�滭����ָ��
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ����dx�滭����ָ��
EXTERN_C _declspec(dllexport) ID2DDraw* WINAPI iDraw_GetDxObject()
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ʼ��ʱ�Ĳ���
// 
// ����:
//      ����1: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵ�һ������
//      ����2: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵڶ�������
//      ����3: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵ���������
//      ����4: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵ��ĸ�����
// 
// ����ֵ:
//      ����ֵ��δʹ��
EXTERN_C _declspec(dllexport) int WINAPI iDraw_InterfaceArgument(IDRAW_INFO** pInfo, IGetEWindow** eWnd, int* reserved1, int* reserved2)
{
    if (!s_info)
        return 0;
    if (pInfo)
        *pInfo = s_info;
    if (eWnd)
        *eWnd = s_info->eWnd;
    if (reserved1)
        *reserved1 = 0;
    if (reserved2)
        *reserved2 = 0;
    return 0;
}

inline void CALLBACK _shadow_update(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    if (id)
        KillTimer(hWnd, id);
    RECT rc;
    GetWindowRect(s_info->hEWnd, &rc);
    s_info->shadow->Update(rc, true);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ػ���Ӱ����
// 
// ����:
//      ����1: �Ƿ��첽�ػ�, �������Ҫ���ϻ滭������Ϊtrue
// 
// ����ֵ:
//      ������Ӱ���ھ��
EXTERN_C _declspec(dllexport) HWND WINAPI iDraw_UpdateShadow(bool bAsync)
{
    if (!s_info || !s_info->shadow)
        return 0;
    if (bAsync)
    {
        // 20�����ŵ����ػ�, ��ֹ�������ظ�Ƶ������, ����ֻ֧��20���������һ�ε���
        SetTimer(s_info->shadow->GetWindow(), 10001, 20, _shadow_update);
    }
    else
    {
        _shadow_update(0, 0, 0, 0);
    }

    return s_info->hShadow;

}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ļ�����ext, �ɹ�������Դ���, ʧ�ܷ���0
// 
// ����:
//      ����1: ext���ļ�·��
// 
// ����ֵ:
//      ����ext�ļ�����Դ���, ����ʹ�� User_GetIcon/User_GetIconDx ��������������ȡͼ��
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_LoadExtFromFile(LPCWSTR pszFile)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->LoadFromFile(pszFile);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ڴ����ext, �ɹ�������Դ���, ʧ�ܷ���0
// 
// ����:
//      ����1: ext���ļ�����
//      ����2: �����ݳߴ�
// 
// ����ֵ:
//      ����ext�ļ�����Դ���, ����ʹ�� User_GetIcon/User_GetIconDx ��������������ȡͼ��
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_LoadExtFromMemory(const void* ptr, int size)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->LoadFromMemory(ptr, size);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡĬ���������Դ���, ���ݴ��ھ����ȡ
// 
// ����:
//      ����1: ���ĸ������ϻ�ȡ, ����ֻ�� ��׼������, ����/֧�ֿ�/����� ���Ϳ��⼸����Ĭ��ͼ��
// 
// ����ֵ:
//      ���ش��ڹ�������Դ���
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_GetDefControlIcon(HWND hWnd)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->GetDefControlIcon(hWnd);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ��������а󶨴�����Դ�Ļص�
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ȡ���󶨳ɹ�����true, ʧ�ܷ���false
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_UnBindWindow(LPCSTR guid)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->UnBindWindow(guid);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����Դ�󶨵�������, �´��ػ��������ʱ��������Դ��ȡͼ��
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: Ҫ�󶨵Ĵ��ھ��
//      ����3: ���ص���Դ���, ����������Ϊ0, �����һ������Ϊ0, �򲻻�����趨��ͼ��
//      ����4: �ص�����, �����������Ҫ����ͼ��ʱ����, Ϊ0��ʹ��Ĭ�ϵ���Դ����, ���û��Ĭ�ϵ���Դ�򲻼���
// 
// ����ֵ:
//      �󶨳ɹ�����true, ʧ�ܷ���false
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->BindWindow(guid, hWnd, hResource, callback);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ӵ��ھ����ȡ��Դ, ���û�а󶨹��������, �򷵻�0
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ֮ǰ�󶨵Ĵ��ھ��
//      ����3: ���հ�ʱ���ݵĻص�����, ����Ϊ0
// 
// ����ֵ:
//      �ɹ����ذ�ʱ����Դ���, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) IDRAW_HRESOURCE WINAPI iDraw_GetResourceFromHwnd(LPCSTR guid, HWND hWnd, pfn_GetWindowIcon* callback)
{
    if (!s_info || !s_info->pResource)
        return 0;
    if (callback)*callback = 0;
    LPPLUGIN_INFO plugin = s_info->pfnPlugin_GetInfo(guid);
    if (!plugin) return 0;
    int callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, 0, 0);
    BINDWINDOW_INFO* callbackArr = 0;
    if (callbackCount <= 0) return 0;

    IDRAW_HRESOURCE hResource = 0;
    callbackArr = new BINDWINDOW_INFO[callbackCount];
    memset(callbackArr, 0, sizeof(BINDWINDOW_INFO) * callbackCount);
    callbackCount = s_info->pResource->GetResourceCallbackFromHwnd(hWnd, callbackArr, callbackCount);
    for (int i = 0; i < callbackCount; i++)
    {
        if (callbackArr[i].guid == (LPCSTR)plugin->hash)
        {
            hResource = callbackArr[i].hResource;
            if (callback)*callback = callbackArr[i].callback;
            break;
        }
    }
    delete[] callbackArr;
    return hResource;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����Դ������ȡͼ������
// 
// ����:
//      ����1: ��Դ���, ʹ�� iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon ���ص�ֵ
//      ����2: ͼ��ID
//      ����3: RECT�ṹָ��, �������ID��ext�е�λ��
//      ����4: Ҫ��ȡ��״̬, Ŀǰֻ֧�� STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL ��4��
//      ����5: dpi
// 
// ����ֵ:
//      ����ͼ������, ���û��ͼ���򷵻�0
EXTERN_C _declspec(dllexport) LPWINDOW_ICON_DATA WINAPI iDraw_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, int state, UINT dpi)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->User_GetIcon(hResource, pszID, prc, (STATE)state, dpi);
}
//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����Դ������ȡͼ������
// 
// ����:
//      ����1: ��Դ���, ʹ�� iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon ���ص�ֵ
//      ����2: ͼ��ID
//      ����3: RECT�ṹָ��, �������ID��ext�е�λ��
//      ����4: Ҫ��ȡ��״̬, Ŀǰֻ֧�� STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL ��4��
//      ����5: dpi
// 
// ����ֵ:
//      ����dxͼ������, ���û��ͼ���򷵻�0
EXTERN_C _declspec(dllexport) LPWINDOW_ICON_DATA WINAPI iDraw_GetIconDx(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, int state, UINT dpi)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->User_GetIcon(hResource, pszID, prc, (STATE)state, dpi);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ͼ���������ȡdx����gdi+��ͼƬָ��, gdi+���ص��� Gdiplus::Bitmap* ָ��, dx�����ڲ�ʹ�õ�ͼƬָ��
// 
// ����:
//      ����1: ͼ������, iDraw_GetIcon ���� iDraw_MakeResourceReturn ���ص�ֵ
//      ����2: �Ƿ񷵻�gdiplusλͼ���, Ϊtrue�򷵻�Gdiplusλͼ���, false�򷵻�dxλͼ���
// 
// ����ֵ:
//      ����λͼ���, ���ݵڶ�����������
EXTERN_C _declspec(dllexport) void* WINAPI iDraw_GetImageHandle(LPWINDOW_ICON_DATA pIconData, bool isGdiplus)
{
    if (!pIconData)
        return 0;
    try
    {
        if (isGdiplus)
            pIconData->hBitmapGdip;
        return pIconData->hBitmapDx;
    }
    catch (...)
    {
        return 0;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////
// ���������������ڲ�����
EXTERN_C _declspec(dllexport) void WINAPI iDraw_MenuInsert(int id, LPCWSTR lpszMenuText, int index, HMENU hPopupMenu, bool isPopup)
{
    if (!s_info || !s_info->pfnControls)
        return;
    IDC_STRUCT_MENU_INSERT arg = { id, lpszMenuText, index, hPopupMenu, isPopup };
    s_info->pfnControls(IDC_ICONTROL_INSERTMENUBAR, 0, (LPARAM)&arg);
}
EXTERN_C _declspec(dllexport) void WINAPI iDraw_MenuRemove(int id, bool isIndex)
{
    if (!s_info || !s_info->pfnControls)
        return;
    s_info->pfnControls(IDC_ICONTROL_REMOVEMENUBAR, id, isIndex);
}
//////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ������ˢ, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫ�Ļ�ˢֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ�ˢ, RGB��ɫ
// 
// ����ֵ:
//      ���ش����õ�GDI��ˢ, ���ô˺��������Ļ�ˢ�������ֶ��ͷ�
EXTERN_C _declspec(dllexport) HBRUSH WINAPI iDraw_CreateBrush(COLORREF cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateBrush(cr);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ������ˢ, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫ�Ļ�ˢֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ�ˢ, ARGB��ɫ
// 
// ����ֵ:
//      ���ش����õĻ�ˢ, ���ô˺��������Ļ�ˢ�������ֶ��ͷ�
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_CreateBrushDx(DWORD cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateBrushDx(cr);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ���, ARGB��ɫ
// 
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺��������Ļ��ʲ������ֶ��ͷ�
EXTERN_C _declspec(dllexport) HPEN WINAPI iDraw_CreatePen(int style, int width, COLORREF cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreatePen(style, width, cr);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ���, ARGB��ɫ
// 
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺��������Ļ��ʲ������ֶ��ͷ�
EXTERN_C _declspec(dllexport) LPEX_PEN WINAPI iDraw_CreatePenDx(int style, float width, DWORD cr)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreatePenDx(cr, width);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ������
//      ����2: �����С
//      ����3: ������ʽ, 0=һ��, 1=����, 2=б��, 4=�»���, 8=ɾ����, �����ʽ��Ҫ������
//
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺������������岻�����ֶ��ͷ�
EXTERN_C _declspec(dllexport) HFONT WINAPI iDraw_CreateFont(LPCWSTR name, int lfHeight, int style)
{
    if (!s_info)
        return 0;
    LPEX_FONT hFont = s_info->pfnCreateFontDx(name, lfHeight, (FONTSTYLE)style);
    if (!hFont) return 0;
    return hFont->GetFont();
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: LPLOGFONTW �ṹָ��
//
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺������������岻�����ֶ��ͷ�
EXTERN_C _declspec(dllexport) HFONT WINAPI iDraw_CreateFontLf(LPLOGFONTW lf)
{
    if (!s_info)
        return 0;
    LPEX_FONT hFont = s_info->pfnCreateFontLfDx(lf);
    if (!hFont) return 0;
    return hFont->GetFont();
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ������
//      ����2: �����С
//      ����3: ������ʽ, 0=һ��, 1=����, 2=б��, 4=�»���, 8=ɾ����, �����ʽ��Ҫ������
//
// ����ֵ:
//      ���ش����õ�DX����, ���ô˺������������岻�����ֶ��ͷ�
EXTERN_C _declspec(dllexport) LPEX_FONT WINAPI iDraw_CreateFontDx(LPCWSTR name, int lfHeight, int style)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateFontDx(name, lfHeight, (FONTSTYLE)style);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: LPLOGFONTW �ṹָ��
//
// ����ֵ:
//      ���ش����õ�DX����, ���ô˺������������岻�����ֶ��ͷ�
EXTERN_C _declspec(dllexport) LPEX_FONT WINAPI iDraw_CreateFontLfDx(LPLOGFONTW lf)
{
    if (!s_info)
        return 0;
    return s_info->pfnCreateFontLfDx(lf);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ͨ�����ھ������Ϣֵ��ȡ����Ϣ�󶨵Ļص�
// 
// ����:
//      ����1: ���Ψһ��ʶ
//      ����1: ���ھ��
//      ����2: ע�����Ϣֵ
// 
// ����ֵ:
//      ���������Ϣ�Ļص���Ϣ�ṹָ��, ���û���ҵ��򷵻�0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_GetMessageInfo(HWND hWnd, UINT message, LPREGISTER_MESSAGE_INFO* pArr, int bufSize)
{
    if (!s_info || !s_info->evt.pfnGetMessageInfo)
        return 0;
    return s_info->evt.pfnGetMessageInfo(hWnd, message, pArr, bufSize);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ͨ���¼�ID��ȡ�ص���Ϣ
// 
// ����:
//      ����1: ���Ψһ��ʶ
//      ����2: �¼�ID
// 
// ����ֵ:
//      ��������¼��Ļص���Ϣ�ṹָ��, ���û���ҵ��򷵻�0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_GetEventInfo(UINT nEvent, LPREGISTER_EVENT_INFO* pArr, int bufSize)
{
    if (!s_info || !s_info->evt.pfnGetEventInfo)
        return 0;
    return s_info->evt.pfnGetEventInfo(nEvent, pArr, bufSize);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ע����Ϣ�ص�
// 
// ����:
//      ����1: ���Ψһ��ʶ
//      ����2: ע���ĸ����ڵ���Ϣ, ����ô��ڲ�����, �����ʧ��
//      ����3: ע�����Ϣֵ, �����ڴ��������Ϣʱ����ûص�����
//      ����4: ����ǰ�Ļص�����, ���ڴ��������Ϣǰ��������ص�����, Ϊ0��ȡ���ҽ�����¼�
//      ����5: �����Ļص�����, �����Ѿ����������Ϣ���������ص�����, Ϊ0��ȡ���ҽ�����¼�
//          ����4 �� ����5 �Ļص�ԭ�Ͷ�����ͬ��
//              ��Ҫ5������, ��������ֵ, ������������ƥ��, ����ᵼ�¶�ջ��ƽ��Ӷ����³������
//              ������ �ص�����(���ھ��, ��Ϣֵ, ����1, ����2, ��������ָ��)
//              �����໯������Ϣ����һ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_RegisterMessage(LPCSTR guid, HWND hWnd, UINT message, pfn_Register_Message pfnBefore, pfn_Register_Message pfnAfter)
{
    if (!s_info || !s_info->evt.pfnRegister_Message)
        return 0;
    return s_info->evt.pfnRegister_Message(guid, hWnd, message, pfnBefore, pfnAfter);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ע���¼��ص�
// 
// ����:
//      ����1: Ψһ��ʶ��
//      ����2: ��Ҫע����¼�, �ᴫ�ݵ��ص�������
//      ����3: ����ǰ�Ļص���������, ����������ڵ��� ����4 ʱ���ݹ�ȥ
//      ����4: ����ǰ�Ļص�����, Ϊ0��ȡ���ҽ�����¼�
//      ����5: �����Ļص���������, ����������ڵ��� ����6 ʱ���ݹ�ȥ
//      ����6: �����Ļص�����, Ϊ0��ȡ���ҽ�����¼�
//          ����4 �� ����6 �Ļص�ԭ�Ͷ�����ͬ��
//              ��Ҫ3������, ��������ֵ, ������������ƥ��, ����ᵼ�¶�ջ��ƽ��Ӷ����³������
//              ������ �ص�����(�¼�����, �ص�����, �¼�ָ��)
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Event_RegisterEvent(LPCSTR guid, UINT nEvent, LONG_PTR paramBefore, pfn_Register_Event pfnBefore, LONG_PTR paramAfter, pfn_Register_Event pfnAfter)
{
    if (!s_info || !s_info->evt.pfnRegister_Event)
        return 0;
    return s_info->evt.pfnRegister_Event(guid, nEvent, paramBefore, pfnBefore, paramAfter, pfnAfter);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ��ָ�������������Ϣ�ص�
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
EXTERN_C _declspec(dllexport) int WINAPI iDraw_UnRegister_Message(LPCSTR guid)
{
    if (!s_info || !s_info->evt.pfnUnRegister_Message)
        return 0;
    return s_info->evt.pfnUnRegister_Message(guid);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ��ָ������������¼��ص�
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
EXTERN_C _declspec(dllexport) int WINAPI iDraw_UnRegister_Event(LPCSTR guid)
{
    if (!s_info || !s_info->evt.pfnUnRegister_Event)
        return 0;
    return s_info->evt.pfnUnRegister_Event(guid);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ʼ�����, ֻ�г�ʼ�����������չ�˵�����Ӳ˵���
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: �����ģ����
//      ����3: ����Ĳ����Ϣ�ṹָ��, ��ȡ����ʼ��ǰ���뱣֤���ָ��Ĵ��
//      ����4: �Ƿ������չ�˵�, ���Ϊfalse, ����� iDraw_MenuExt_Insert ��ʧ��
//      ����5: ���Ӿ���ͨѶ�Ļص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_InitPlugin(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback)
{
    if (!s_info || !s_info->pfnPlugin_init)
        return 0;
    return s_info->pfnPlugin_init(guid, hModule, etInfo, pfnMenuExt_Command, callback);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ����ʼ�����, ��֮ǰ��ʼ��ʱ���뵽��չ�˵��Ĳ˵���ɾ��
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_UnInitPlugin(LPCSTR guid)
{
    if (!s_info || !s_info->pfnPlugin_UnInit)
        return 0;
    return s_info->pfnPlugin_UnInit(guid);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ѳ˵�����뵽��չ�˵���, �˵�����¼��� iDraw_InitPlugin ��Ļص���������
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: �����λ��, -1����뵽���
//      ����3: ��ʶ
//      ����4: �˵���ID, ���uFlags����MF_POPUP ����������ǵ���ʽ�Ĳ˵����
//      ����5: �˵���Ŀ��ֵ
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_MenuExt_Insert(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    IDC_STRUCT_MENUEXT_INSERT arg = { guid, uPosition, uFlags, uIDNewItem, lpNewItem, param };
    return s_info->pfnControls(IDC_ICONTROL_INSERT_MENUEXT, 0, (LPARAM)&arg);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ѻ����ŵ������Դ����߳������
// 
// ����:
//      ����1: ������ַ, û�з���ֵ, ����û�з����ź�һ�������Ͳ���, ������ջ��ƽ�⵼�±���
//      ����2: ���ݵ�������Ĳ���
// 
// ����ֵ:
//      ʧ�ܷ���0, �ɹ����ط�0ֵ
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_EWindowThreadCallback(pfn_EWindowThreadCallback pfn, LPARAM param)
{
    if (!s_info)
        return 0;
    return PostMessageA(s_info->hEWnd, s_info->uMsgEWindowThreadCallback, (WPARAM)pfn, param);
}



//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��gdi+λͼ����dxλͼ��������Դ����ֵ, ����ָ����뱣֤���, �ڲ����������ڴ������洫�ݽ������ı���λͼ
// 
// ����:
//      ����1: ͼƬ���ݵ�ַ
//      ����2: ͼƬ���ݴ�С
//      ����3: ���Ƶ�ǰ��Ŀ��ʾ���ı�, �����޸�ԭ����ı�, ֻ�ǻ滭ʱ��ʾ����ı�, ����Ϊ0
//      ����4: ��ע�ı�, ����Ϊ0
//      ����5: �ı�ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
//      ����6: ����ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
// 
// ����ֵ:
//      ������Դ����ֵ, ����ֵ��ʹ��ʱ������� iDraw_FreeResourceReturn ���ͷ�
EXTERN_C _declspec(dllexport) LPWINDOW_ICON_DATA WINAPI iDraw_MakeResourceReturn(const void* pImg, int size, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->MakeResourceReturn(pImg, size, text, remarks, crText, crBack);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ͷ� iDraw_MakeResourceReturn() ���ɵķ���ֵ
// 
// ����:
//      ����1: iDraw_MakeResourceReturn() �ķ���ֵ
// 
// ����ֵ:
//      �����Ƿ��ͷųɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_FreeResourceReturn(LPWINDOW_ICON_DATA pData)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->FreeResourceReturn(pData);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ڴ����gdi+ͼƬ
// 
// ����:
//      ����1: ͼƬ����
//      ����2: ͼƬ���ݴ�С
// 
// ����ֵ:
//      �ɹ�����gdi+ͼƬָ��, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) Gdiplus::Bitmap* WINAPI iDraw_LoadGpImageFromMemory(const void* pData, int size)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->LoadGpImageFromMemory(pData, size);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ͷŴ� iDraw_LoadGpImageFromMemory() ���ص�ֵ
// 
// ����:
//      ����1: iDraw_LoadGpImageFromMemory() ���ص�ֵ
// 
// ����ֵ:
//      �ɹ�����true, ʧ�ܷ���false
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_FreeGpImage(Gdiplus::Bitmap* hImg)
{
    if (!s_info || !s_info->pResource)
        return 0;
    return s_info->pResource->FreeGpImage(hImg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��Ӱ�ť���ӿڹ�������, �ɹ����ع�������ťID, ������������ť�����ʱ������ID���ݹ�ȥ
// 
// ����:
//      ����1: ���Ψһ��ʶ��, �������/�ػ��¼�ʱ������������Ļص�����
//      ����2: ��ť��ID, �����¼�ʱ������ID���ݹ�ȥ
//      ����3: ��ť����, ����Ϊ0, Ϊ0��û�б���, ����ƶ���ȥʱ��������ʾ˵������, �Ӵ���ʾ, ������ⲻ����ʾ�ڹ�������
//      ����4: ��ť˵��, ����Ϊ0, Ϊ0��û��˵��, ����ƶ���ȥʱ��������ʾ˵��
//      ����5: ��Ŀ��ֵ, ��������¼����ݹ�ȥ
//      ����6: ͼƬ����ָ��, �������ʾ���������ϵ�ͼ��
//      ����7: ͼƬ���ݴ�С
//      ����8: �滭�ص�����, pfn_ToolDrawButton ����
//              typedef int(WINAPI* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// ����ֵ:
//      �ɹ����ع�������ťID, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Tool_InsertButton(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"���� iDraw_Tool_InsertButton() ʱ %s ���δ��ʼ��", _str::A2W(guid).c_str());
        return 0;
    }

    IDRAW_STRUCT_TOOL_INSERT arg = { 0 };
    arg.lpszTitle = lpszTitle;
    arg.lpszTips = lpszTips;
    arg.idCommand = idCommand;
    arg.pImageData = pImageData;
    arg.nImageSize = nImageSize;
    arg.param = param;
    arg.pfnDraw = pfnDraw;
    arg.pPlugin = info;
    return s_info->pfnControls(IDC_ICONTROL_TOOL_INSERT, (WPARAM)info, (LPARAM)&arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ɾ���ӿڹ������İ�ť, ֻ��ɾ���Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��, �������/�ػ��¼�ʱ������������Ļص�����
//      ����2: ��ť��ID, ���ʱ��ID
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_DeleteButton(LPCSTR guid, int idCommand)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"���� iDraw_Tool_DeleteButton() ʱ %s ���δ��ʼ��", _str::A2W(guid).c_str());
        return 0;
    }
    return s_info->pfnControls(IDC_ICONTROL_TOOL_DELETE, (WPARAM)idCommand, (LPARAM)info) != 0;
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ�ӿڹ������İ�ť��Ϣ, ֻ�ܻ�ȡ�Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ��ť��ID, ���ʱ��ID
//      ����3: Ϊ0�򲻽���, ���հ�ť����
//      ����4: Ϊ0�򲻽���, ���հ�ť˵��
//      ����5: Ϊ0�򲻽���, ������Ŀ��ֵ
//      ����6: Ϊ0�򲻽���, ����ͼƬ����ָ��
//      ����7: Ϊ0�򲻽���, ����ͼƬ���ݴ�С
//      ����8: Ϊ0�򲻽���, ���ջ滭�ص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_GetButtonData(LPCSTR guid, int idCommand, LPWSTR* lpszTitle, LPWSTR* lpszTips, LPARAM* param, LPCVOID* pImageData, int* nImageSize, pfn_ToolDrawButton* pfnDraw)
{
    if (!s_info || !s_info->pfnControls)
        return false;
    if (!lpszTitle && !lpszTips && param && pImageData && nImageSize && !pfnDraw)
    {
        s_info->SetLastError(L"���� " __FUNCTIONW__ " ʱ δ�����κβ���");
        return false;
    }
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"���� " __FUNCTIONW__ " ʱ % s ���δ��ʼ��", _str::A2W(guid).c_str());
        return false;
    }

    LPIDRAW_STRUCT_TOOL_INSERTEX data = (LPIDRAW_STRUCT_TOOL_INSERTEX)s_info->pfnControls(IDC_ICONTROL_TOOL_GETBBUTTON, (WPARAM)idCommand, (LPARAM)info);
    if (!data)
        return false;
    auto pfn_copystr = [](LPWSTR* pSet, LPCWSTR str)
    {
        if (!pSet) return;

        *pSet = 0;
        if (str)
        {
            int len = wcslen(str) * sizeof(wchar_t) + sizeof(wchar_t);
            LPWSTR ptr = (LPWSTR)iDraw_malloc(len);
            memcpy(ptr, str, len);
            *pSet = ptr;
        }
    };

    pfn_copystr(lpszTitle, data->lpszTitle);
    pfn_copystr(lpszTips , data->lpszTips);
    if (param)      *param      = data->param;
    if (pImageData) *pImageData = data->pImageData;
    if (nImageSize) *nImageSize = data->nImageSize;
    if (pfnDraw)    *pfnDraw    = data->pfnDraw;
    return true;
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ýӿڹ������İ�ť��Ϣ, ֻ�������Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ��ť��ID, ���ʱ��ID
//      ����3: Ϊ0������, ���ð�ť����
//      ����4: Ϊ0������, ���ð�ť˵��
//      ����5: Ϊ0������, ������Ŀ��ֵ
//      ����6: Ϊ0������, ����ͼƬ����ָ��
//      ����7: Ϊ0������, ����ͼƬ���ݴ�С
//      ����8: Ϊ0������, ���û滭�ص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_SetButtonData(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw)
{
    if (!s_info || !s_info->pfnControls)
        return 0;
    if (!lpszTitle && !lpszTips && param && pImageData && nImageSize && !pfnDraw)
    {
        s_info->SetLastError(L"���� " __FUNCTIONW__ " ʱ δ�����κβ���");
        return false;
    }
    LPPLUGIN_INFO info = Plugin_GetInfo(guid);
    if (!info)
    {
        s_info->SetLastError(L"���� " __FUNCTIONW__ " ʱ % s ���δ��ʼ��", _str::A2W(guid).c_str());
        return 0;
    }

    IDRAW_STRUCT_TOOL_INSERT arg = { 0 };
    arg.idCommand = idCommand;
    arg.pPlugin = info;
    return s_info->pfnControls(IDC_ICONTROL_TOOL_SETBBUTTON, (WPARAM)idCommand, (LPARAM)info) != 0;
}



//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���������Խ�������, ��������������൱�ڵ��������� ������ϵͳ����() ����
// 
// ����:
//      ����1: ���ܺ�, ���ܺ���ο��������ṩ�� lib2.h �ļ�, NES_, NAS_, NRS_ ��ͷ����
//      ����2: ����� lib2.h ���ܺ�˵��
//      ����3: ����� lib2.h ���ܺ�˵��
// 
// ����ֵ:
//      ����� lib2.h ���ܺ�˵��
EXTERN_C _declspec(dllexport) int WINAPI iDraw_EIDE_Notify(int nMsg, DWORD dwParam1, DWORD dwParam2)
{
    return s_info->pfnIDE(nMsg, dwParam1, dwParam2);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����������IDE�ӿڹ���, ������������൱�ڵ��� iDraw_EIDE_Notify(NES_RUN_FUNC, ����1, ����[����2, ����3]);
// 
// ����:
//      ����1: ���ܺ�, ���ܺ���ο��������ṩ�� PublicIDEFunctions.h �ļ�, 
//      ����2: ����� PublicIDEFunctions.h ���ܺ�˵��
//      ����3: ����� PublicIDEFunctions.h ���ܺ�˵��
// 
// ����ֵ:
//      ����ù��ܱ���������, δ�������򷵻ؼ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_EIDE_RunFunctions(int nMsg, DWORD dwParam1, DWORD dwParam2)
{
    DWORD arg[] = { dwParam1, dwParam2 };
    return s_info->pfnIDE(NES_RUN_FUNC, nMsg, (DWORD)arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��һ���ı���ɫת����RGB��ɫ
// 
// ����:
//      ����1: Ĭ����ɫ, ����ʧ�ܷ��������ɫ
//      ����2: ָ����ɫ�ı�ָ��
// 
// ����ֵ:
//      ����ת���õ�RGB��ɫֵ
EXTERN_C _declspec(dllexport) COLORREF WINAPI iDraw_STR2RGB(COLORREF crDef, LPCWSTR lpszColorText)
{
    return s_info->pfnControls(IDC_IDRAW_STR2RGB, crDef, (DWORD)lpszColorText);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��һ���ı���ɫת����ARGB��ɫ
// 
// ����:
//      ����1: Ĭ����ɫ, ����ʧ�ܷ��������ɫ
//      ����2: ָ����ɫ�ı�ָ��
// 
// ����ֵ:
//      ����ת���õ�ARGB��ɫֵ
EXTERN_C _declspec(dllexport) ARGB WINAPI iDraw_STR2ARGB(ARGB crDef, LPCWSTR lpszColorText)
{
    return s_info->pfnControls(IDC_IDRAW_STR2ARGB, crDef, (DWORD)lpszColorText);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��RGB��ɫת�����ı� RGB(1,2,3)
// 
// ����:
//      ����1: Ҫת����rgb
//      ����2: ����20��WCHAR������ָ��
// 
// ����ֵ:
//      ����д���ַ���
EXTERN_C _declspec(dllexport) int WINAPI iDraw_RGB2STR(COLORREF crDef, LPWSTR lpszBuffer)
{
    return s_info->pfnControls(IDC_IDRAW_RGB2STR, crDef, (DWORD)lpszBuffer);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��RGB��ɫת�����ı� ARGB(255,1,2,3)
// 
// ����:
//      ����1: Ҫת����argb
//      ����2: ����26��WCHAR������ָ��
// 
// ����ֵ:
//      ����д���ַ���
EXTERN_C _declspec(dllexport) int WINAPI iDraw_ARGB2STR(ARGB crDef, LPWSTR lpszBuffer)
{
    return s_info->pfnControls(IDC_IDRAW_ARGB2STR, crDef, (DWORD)lpszBuffer);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��������
// 
// ����:
//      ����1: Ҫ��ȡ�Ĵ��ھ��
// 
// ����ֵ:
//      �ɹ���������ָ��, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) LPOBJSTRUCT WINAPI iDraw_GetWindowData(HWND hWnd)
{
    return (LPOBJSTRUCT)s_info->pfnControls(IDC_IDRAW_GETWINDOWDATA, 0, (DWORD)hWnd);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ���봰�ڿ��ӵľ���
// 
// ����:
//      ����1: ���վ��ε�ָ��(����������������λ��)
// 
// ����ֵ:
//      �����Ƿ��ȡ�ɹ�
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_GetCodeVisibleRect(RECT* prc)
{
    return (BOOL)s_info->pfnControls(IDC_IDRAW_GETCODEVISIBLERECT, 0, (DWORD)prc);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����InlineHook, �ڲ��ṩ����hook��ʽ
// 
// ����:
//      ����1: ��hook�ĵ�ַ
//      ����2: ����hook�ĺ�����ַ
//      ����3: ʹ�õ�hook��, 0=detoursHook, 1=MinHook, Ŀǰֻ�ṩ������
// 
// ����ֵ:
//      ���ر�hook��ַ��ԭ��ַ, ����ֱ�ӵ��������ַ
EXTERN_C _declspec(dllexport) LPVOID WINAPI iDraw_InlineHook(LPVOID pOldFun, LPVOID pNewFun, int hookMode)
{
    LPVOID arg[] = { pOldFun, pNewFun };
    return (LPVOID)s_info->pfnControls(IDC_INLINEHOOK, hookMode, (DWORD)arg);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����InlineHook, �ڲ��ṩ����hook��ʽ
// 
// ����:
//      ����1: iDraw_InlineHook �ķ���ֵ
// 
// ����ֵ:
//      �����Ƿ�ȡ���ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_UnInlineHook(LPVOID pOldFun)
{
    return s_info->pfnControls(IDC_UNINLINEHOOK, 0, (DWORD)pOldFun);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �������Ի���ʽ�ĵ���ʽ�˵�, ���м��뵽�������ʽ�˵�����Ӳ˵����ᱻ�Ի�
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ���ص���ʽ�˵����
EXTERN_C _declspec(dllexport) HMENU WINAPI iDraw_CreatePopupMenu()
{
    return (HMENU)s_info->pfnControls(IDC_IDRAW_CREATEPOPUPMENU, (WPARAM)0, (LPARAM)0);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡIDE�������(IEIDE_CodeManage)��ָ��
// 
// ����:
//      ����1: ���봰�ڴ��ھ��, Ϊ0���ȡ��ǰ����
// 
// ����ֵ:
//      ���ش��봰�ڶ�Ӧ�� IEIDE_CodeManage ��ָ��, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) IEIDE_CodeManage* WINAPI iDraw_GetCodeManage(HWND hCode)
{
    return (IEIDE_CodeManage*)s_info->pfnControls(IDC_EIDE_GETCODEMANAGE, (WPARAM)0, (LPARAM)hCode);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ�������, �����ǰ����ʼҳ��������������򷵻�0
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ���ص�ǰ�������, �����ǰ����ʼҳ��������������򷵻�0
EXTERN_C _declspec(dllexport) HWND WINAPI iDraw_GetCodeWindow()
{
    return (HWND)s_info->pfnControls(IDC_EIDE_GETCODEWINDOW, (WPARAM)0, (LPARAM)0);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ������λ��, ����ͨ������¼��޸ı߿�ﵽָ���滭������λ��
// 
// ����:
//      ����1: pfn_GetScrollRect �ص�����
//      ����2: ��������ָ��
// 
// ����ֵ:
//      �ɹ����ط�0, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_Scroll_GetRect(pfn_GetScrollRect pfnCallback, LPOBJSTRUCT pData)
{
    return s_info->pfnControls(IDC_SCROLL_GETRECT, (WPARAM)pfnCallback, (LPARAM)pData);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���������λ��, ����ͨ������¼��޸Ĵ��ڷǿͻ���λ��
// 
// ����:
//      ����1: pfn_CalcScroll �ص�����
//      ����2: ��������ָ��
// 
// ����ֵ:
//      �ɹ����ط�0, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_Scroll_CalcRect(pfn_CalcScroll pfnCallback, LPOBJSTRUCT pData)
{
    return s_info->pfnControls(IDC_SCROLL_CALCRECT, (WPARAM)pfnCallback, (LPARAM)pData);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �滭������
// 
// ����:
//      ����1: pfn_DrawScroll �ص�����
//      ����2: ��������ָ��
// 
// ����ֵ:
//      �ɹ����ط�0, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_Scroll_Draw(pfn_DrawScroll pfnCallback, LPOBJSTRUCT pData)
{
    return s_info->pfnControls(IDC_SCROLL_DRAW, (WPARAM)pfnCallback, (LPARAM)pData);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰMDIClient ����������ʾʲô����
//      -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��������ֵ֮һ: -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
EXTERN_C _declspec(dllexport) int WINAPI iDraw_GetMDIClientType()
{
    return (int)s_info->pfnControls(IDC_IMOVE_GETWINDOWTYPE, (WPARAM)0, (LPARAM)0);
}

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰMDIClient ����������ʾʲô��������
//      0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��������ֵ֮һ: 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
EXTERN_C _declspec(dllexport) int WINAPI iDraw_GetMDIClientCodeType()
{
    return (int)s_info->pfnControls(IDC_IMOVE_GETCODETYPE, (WPARAM)0, (LPARAM)0);
}


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��������������
// 
// ����:
//      ����1: ����������ָ��
//      ����2: ���չ���������ָ��, ����ǰ��Ҫ�� IDRAW_STRUCT_SCROLL_DATA.cbSize ��ֵ
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) BOOL WINAPI iDraw_GetScrollData(LPVOID pScroll, IDRAW_STRUCT_SCROLL_DATA* ptr)
{
    return (BOOL)s_info->pfnControls(IDC_GETSCROLLDATA, (WPARAM)pScroll, (LPARAM)ptr);
}
#endif