#include "EIDE_Interface.h"
#include <stdio.h>
#include "EIDE_InternalMethods.h"
#include <atlstr.h>
#include <assist//CFileRW.h>
#include <unordered_map>
#include <Memory_BeaEngine.h>
#include <Memory_MonitorAddress.h>



static int pEmptyCString;
static LPVOID hException;   // �쳣������
static DWORD m_oldProtect;  // ��ɫ����ڴ�����
static PFN_COLORCHANGE m_pCallback; // ��ɫ���ı�Ļص�
static bool m_isSetCallback;        // �Ƿ������˻ص�, �ڻ�ȡ����ɫ���õ�ʱ���ж�
struct ERRCALLBACK_DATA
{
    BYTE oldByte;
    LPVOID pErrAddress;
};
static std::unordered_map<LPVOID, ERRCALLBACK_DATA> s_map_opCode;   // �����Ǳ��޸ĳ�cc�ĵ�ַ, ����CC�ϵ�Ͱ�cc�ĳɼ�¼��ֵ

static LPBYTE m_begin;  // ��������ɫ���ڴ������ʼ��ַ
static LPBYTE m_end;    // ��������ɫ���ڴ���Ľ�����ַ
static int m_len;       // ��������ɫ���ڴ���ռ�õĳߴ�

static LPBYTE m_pageStart;  // �ڴ�ҳ��ʼ��ַ
static LPBYTE m_pageEnd;    // �ڴ�ҳ������ַ, һ���� ��ʼ��ַ + 0x1000

void CALLBACK errorCallback(LPVOID pAddress, MONITORTYPE type, UINT64 param);



void CALLBACK free_lpstr(void* ptr)
{
    LPSTR pstr = (LPSTR)ptr;
    delete[] pstr;
}
inline HANDLE getheap()
{
    static HANDLE hHeap = 0;
    if (!hHeap)
        hHeap = GetProcessHeap();
    return hHeap;
}
inline void* malloc_heap(int size)
{
    return HeapAlloc(getheap(), 0, size);
}
inline bool free_heap(void* ptr)
{
    return HeapFree(getheap(), 0, ptr);
}
inline bool realloc_heap(void* ptr, int size)
{
    return HeapReAlloc(getheap(), 0, ptr, size);
}
inline bool msize_heap(void* ptr)
{
    return HeapSize(getheap(), 0, ptr);
}

bool c_free(void* ptr)
{
    static int pCHeap;
    if (!pCHeap)
    {
        const BYTE find[] = { 131, 198, 15, 131, 230, 240, 86, 106, 0, 255, 53 };
        LPBYTE address = EIDE_FindAddress(__FINDARG(find));
        if (address)
        {
            address += 11;
            pCHeap = (*((int*)address));
        }
    }

    if (pCHeap)
        return HeapFree((*((HANDLE*)pCHeap)), 0, ptr);
    return HeapFree(GetProcessHeap(), 0, (void*)ptr);
}

int CStream_Create()
{
    return (int)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, 20);
}
void CString_Empty(int& CString)
{
    static void* mCString_Empty = 0;
    if (!mCString_Empty)
    {
        const BYTE find[] = { 139, 1, 131, 120, 248, 0, 116 };
        mCString_Empty = EIDE_FindAddress(__FINDARG(find));
    }
    if (mCString_Empty)
    {
        thiscall((void*)CString, mCString_Empty, 0);
    }
}
void CString_Destroy(int CString)
{
    CString_Empty(CString);
    c_free((void*)(CString - 12));
}
void CStream_Destroy(int pCStream)
{
    BOOL isOk = false;
    int address = (*((int*)(pCStream + 8)));
    if (address)
    {
        isOk = HeapFree(GetProcessHeap(), 0, (void*)address);
    }
    LocalFree((HLOCAL)pCStream);
}
void ECodeEdit_GetCodeInfo(int pECodeEditWnd, int codeInfo[4])
{
    if (!pECodeEditWnd) return;
    codeInfo[0] = (*((int*)(pECodeEditWnd + ECodeEdit_nCodeType)));
    codeInfo[2] = (*((int*)(pECodeEditWnd + 88)));
    codeInfo[3] = (*((int*)(pECodeEditWnd + 92)));
    switch (codeInfo[0])
    {
    case  ��������_����:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 84)));
        break;
    case ��������_��������:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 64)));
        break;
    case ��������_ȫ�ֱ���:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 68)));
        break;
    case ��������_Dll����:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 72)));
        break;
    case ��������_����:
    case ��������_��ԴͼƬ:
    case ��������_��Դ��Ƶ:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 80)));
        break;
    default:
        break;
    }

}

// IDE��������
class EIDE_CodeManage : public IEIDE_CodeManage
{
private:
    void* pCWnd;
    void* pfnGetLineInfo;
    static constexpr auto FEEDBACK_GROUP{ "Ⱥ��658718319" };  // ����Ⱥ��

#define _RET_NULL
    // �ж�CWndָ���Ƿ�Ϊ��, Ϊ�յĻ��ͷ���
#define _ERR_RET(_ret) if (!pCWnd) {\
                            error("error ��������! �ļ� = " __FILE__ ", ���� = " __FUNCTION__ ", �к� = %d  �뽫�˴�����Ϣ������������Ա  %s" , __LINE__, FEEDBACK_GROUP);\
                            return _ret;\
                        }
public:
    // �����ϱ��ӳ���
    inline static void error(LPCSTR format, ...)
    {
        if (!format)return;
        va_list aptr;
        va_start(aptr, format);

        int bufferSize = (int)(strlen(format) * 2);
        if (bufferSize < 260) bufferSize = 260;
        char* buffer = new char[bufferSize];

        //pointer buffer = realloc(bufferSize, true);
        int r = _vsnprintf_s(buffer, bufferSize, bufferSize - 1, format, aptr);
        while (r == -1)
        {
            bufferSize *= 2;
            delete[] buffer;
            buffer = new char[bufferSize];
            r = _vsnprintf_s(buffer, bufferSize, bufferSize - 1, format, aptr);
        }

        va_end(aptr);

        //TODO ������Ҫ�ϱ�����, ûд�ϱ��Ĺ���, �������������
        OutputDebugStringA(buffer);
        delete[] buffer;
        return;

    }
public:
    EIDE_CodeManage() :pCWnd(0), pfnGetLineInfo(0) { }
    // �Ӵ��ھ���г�ʼ������
    virtual bool init(HWND hWnd)
    {
        if (!IsWindow(hWnd))
        {
            error("��������! �ļ� = " __FILE__ ", ���� = " __FUNCTION__ ", �к� = %d  �뽫�˴�����Ϣ������������Ա  %s", __LINE__, FEEDBACK_GROUP);
            return false;
        }
        pCWnd = CWnd_FromHandle(hWnd);
        _ERR_RET(false);
        return true;
    }
    // ����
    virtual void destroy() { pCWnd = 0; pfnGetLineInfo = 0; }
    // ��ȡ���λ��, λ��Ϊ����ڲ����
    virtual POINT GetCursorPos()
    {
        POINT pt{ -1, -1 };
        _ERR_RET(pt);

        pt.y = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnEnd)));
        pt.x = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd)));
        return pt;
    }
    // ���ù��λ��, ��ѱ��к���Ϊ0
    virtual void SetCursorPos(POINT pt)
    {
        _ERR_RET(_RET_NULL);

        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorTabColStart))) = 0;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorTabColEnd))) = 0;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnStart))) = pt.y;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnEnd))) = pt.y;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowStart))) = pt.x;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd))) = pt.x;

        int pSelectRow = (*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo)));
        if (pSelectRow)
        {
            free_heap((void*)pSelectRow);
            (*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo))) = 0;
            (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoMaxLen))) = 0;
            (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoLen))) = 0;
        }
    }
    // ��ȡѡ����Ϣ
    // pStartCol = ������ʼ��
    // pEndCol = ���ս�����
    // pNowLine = ���յ�ǰ��
    // pSel = ����ѡ�з�Χ
    virtual void GetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0)
    {
        _ERR_RET(_RET_NULL);

        if (pStartCol)  *pStartCol = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnStart)));
        if (pEndCol)    *pEndCol = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnEnd)));
        if (pNowLine)   *pNowLine = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd)));
        if (pSel)
        {
            int size = (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoLen)));

            if (size == 0)
            {
                pSel->x = -1;
                pSel->y = -1;
                return;
            }
            *pSel = (**((POINT**)(((int)pCWnd) + ECodeEdit_pSelectRowInfo)));
        }

    }

    // ����ѡ����Ϣ
    // pStartCol = ��ʼ��ָ��, Ϊ0��д��
    // pEndCol = ������ָ��, Ϊ0��д��
    // pNowLine = ��ǰ��ָ��, Ϊ0��д��
    // pNowLine = ��ǰ��ָ��, Ϊ0��д��
    // pSel = ѡ�з�Χָ��, Ϊ0��д��
    virtual void SetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0)
    {
        _ERR_RET(_RET_NULL);

        if (pStartCol)  (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnStart))) = (*pStartCol);
        if (pEndCol)    (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnEnd))) = (*pEndCol);
        if (pNowLine)
        {
            (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowStart))) = (*pNowLine);
            (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd))) = (*pNowLine);
        }
        if (pSel)
        {
            LPBYTE pSelectRow = (LPBYTE)(*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo)));
            if (pSel->x == -1 || pSel->y == -1)
            {
                if (pSelectRow)
                {
                    free_heap(pSelectRow);
                    (*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo))) = 0;
                    (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoMaxLen))) = 0;
                    (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoLen))) = 0;
                }
            }
            else
            {
                if (!pSelectRow)
                {
                    pSelectRow = (LPBYTE)malloc_heap(132);
                    if (!pSelectRow)
                        return;
                    (*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo))) = (int)pSelectRow;
                    (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoMaxLen))) = 132;// msize_heap(pSelectRow);
                    (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoLen))) = 8;
                }
                (*((int*)(pSelectRow + 0))) = pSel->x;
                (*((int*)(pSelectRow + 4))) = pSel->y;
            }
        }
    }

    // �Ƿ�ѡ�д���
    virtual bool IsSelCode()
    {
        _ERR_RET(false);
        int start = 0, end = 0;
        POINT sel = { 0 };
        GetSelInfo(&start, &end, 0, &sel);
        if (start == end && (sel.x == -1 || sel.y == -1))
            return false;
        return true;
    }

    // ��ȡѡ������
    virtual int GetSelLineCount()
    {
        _ERR_RET(0);
        POINT sel = { 0 };
        GetSelInfo(0, 0, 0, &sel);
        if (sel.x == -1 || sel.y == -1)
            return 0;
        return sel.y - sel.x + 1;
    }

    // ��ȡѡ������
    virtual int GetSelColumnCount()
    {
        _ERR_RET(0);
        int start = 0, end = 0;
        GetSelInfo(&start, &end, 0, 0);

        return abs(end - start);
    }

    // ��ȡ����Ϣ
    // lineNumber = �к�
    // flags = ���ʶ, ETAG
    // pData = ������ָ��, EBase*
    virtual int GetLineInfo(int lineNumber, int* pFlags, int* pData)
    {
        _ERR_RET(0);
        if (!pfnGetLineInfo)
        {
            const BYTE find[] = { 131, 236, 16, 139, 68, 36, 24, 83, 85, 139, 233 };
            LPBYTE address = EIDE_FindAddress(__FINDARG(find));

            if (!address)
            {
                const BYTE find_593[] = { 0x83, 0xEC, 0x10, 0x53, 0x55, 0x8B, 0x6C, 0x24, 0x20, 0x8B, 0xD9 };
                address = EIDE_FindAddress(__FINDARG(find_593));
                if (!address)
                {
                    error("��������! �ļ� = " __FILE__ ", ���� = " __FUNCTION__ ", �к� = %d  �뽫�˴�����Ϣ������������Ա  %s", __LINE__, FEEDBACK_GROUP);
                    return -1;
                }
            }
            pfnGetLineInfo = address;
        }
        if (pfnGetLineInfo)
        {
            void* address = pfnGetLineInfo;
            int nFlags = 0;
            int data = 0;
            int nRet = thiscall(pCWnd, pfnGetLineInfo, 9, lineNumber, 0, &address, &data, 0, 0, 0, &nFlags, 0);

            if (pFlags)*pFlags = nFlags;
            if (pData)*pData = data;
            return nRet;
        }
        return 0;
    }

    // ��ȡ��������
    virtual int GetCodeType()
    {
        _ERR_RET(��������_��);
        return (*((int*)(((int)pCWnd) + ECodeEdit_nCodeType)));
    }

    // ��ȡ����ָ��
    virtual void* GetObjectThis() { return pCWnd; }

    // ���ö���ָ��
    virtual void SetObjectThis(void* pThis) { pCWnd = (LPBYTE)pThis; pfnGetLineInfo = 0; }

    // ��궨λ
    virtual void CursorLocation(int lineNumber)
    {
        _ERR_RET(_RET_NULL);
        static LPBYTE ptr = 0;
        if (!ptr)
        {
            const BYTE find[] = { 106, 0, 106, 1, 106, 0, 106, 0, 80, 139, 206, 232 };
            LPBYTE address = EIDE_FindAddress(__FINDARG(find));
            if (address)
            {
                address += 12;
                address = (*((int*)(address))) + address + 4;
                ptr = address;
            }
        }

        if (ptr)
        {
            int nRet = thiscall(pCWnd, ptr, 5, lineNumber, 0, 0, 1, 0);
            nRet = 0;
        }
    }

    // ��֤��ʾ���λ��
    virtual void EnsurVisibleCursor()
    {
        _ERR_RET(_RET_NULL);
        static LPBYTE ptr = 0;
        if (!ptr)
        {
            const BYTE find[] = { 106, 1, 106, 0, 106, 0, 139, 206, 232 };
            LPBYTE address = EIDE_FindAddress(__FINDARG(find));
            if (address)
            {
                address += 9;
                address = (*((int*)(address))) + address + 4;
                ptr = address;
            }
        }

        if (ptr)
        {
            int nRet = thiscall(pCWnd, ptr, 3, 0, 0, 1);
            nRet = 0;
        }

    }

    // ��ȡ�����ı�
    // nTableIndex = ����
    // nTableindexCol = ����
    virtual LPSTR GetCodeText(int nTableIndex, int nTableindexCol)
    {
        //TODO ��ȡ�ı�������, ����ʹ������Ĵ���, ������������, ���޸�
        return 0;
        _ERR_RET(0);
        static LPBYTE pfnTableInfo = 0;
        static LPBYTE pfnGetCodeText = 0;
        if (!pfnTableInfo)
        {
            const BYTE find[] = { 106, 0, 106, 0, 106, 0, 86, 139, 207, 232 };
            LPBYTE address = EIDE_FindAddress(__FINDARG(find));
            if (address)
            {
                address += 10;
                address = (*((int*)(address))) + address + 4;
                pfnTableInfo = address;
            }
        }

        if (!pfnGetCodeText)
        {
            const BYTE find[] = { 81, 139, 68, 36, 28, 83, 139, 92, 36, 16 };
            pfnGetCodeText = EIDE_FindAddress(__FINDARG(find));
        }

        LPSTR ret = 0;
        if (pfnTableInfo && pfnGetCodeText)
        {
            int pCStream = CStream_Create();
            int nRet = thiscall(pCWnd, pfnTableInfo, 4, pCStream, 0, 0, 0);

            int* pCString = &pEmptyCString;
            CStringA* cs = (CStringA*)(&pCString);

            nRet = thiscall(pCWnd, pfnGetCodeText, 6, nTableIndex, nTableindexCol, cs, 0, 0, pCStream);
            if (nRet)
            {
                LPCSTR pstr = (LPCSTR)pCString;
                if (pstr)
                {
                    size_t len = strlen(pstr) + 1;
                    ret = (char*)alloc(len);
                    memcpy(ret, pstr, len);
                    //EIDE_PushMemory(ret, free_lpstr, 30000);
                }
                CString_Destroy((int)cs);
            }
            CStream_Destroy(pCStream);
        }
        return ret;
    }
    // �����ڴ�
    virtual void* alloc(int size)
    {
        char* p = (char*)malloc(size);
        if (!p) return 0;
        memset(p, 0, size);
        return p;
    }
    // �ͷ��ڴ�, ֻ�ܴ��ݱ������ alloc() �������ص�ָ��, ��������ָ����ܻ������
    virtual void free(void* p)
    {
        ::free(p);
    }

    // ��ȡ��������ƫ��
    // nTableIndex = ����
    // nTableindexCol = ����
    // type = ��������ָ��
    // offset = ����ƫ��ָ��
    // pData = ����ָ��, EBase*
    virtual bool GetCodeDataOffset(int nTableIndex, int nTableindexCol, int* pType, int* pOffset, int* pData)
    {
        _ERR_RET(false);
        static LPBYTE pfn = 0;
        if (!pfn)
        {
            const BYTE find[] = { 139, 68, 36, 28, 131, 236, 8, 133, 192 };
            pfn = EIDE_FindAddress(__FINDARG(find));
        }

        if (pfn)
        {
            int codeInfo[4] = { 0 };
            ECodeEdit_GetCodeInfo((int)pCWnd, codeInfo);

            int type = 0, offset = 0, data = 0;
            int nRet = thiscall(&codeInfo, pfn, 7, nTableIndex, nTableindexCol, &type, &offset, &data, 1, 0);
            if (pType)      *pType      = type;
            if (pOffset)    *pOffset    = offset;
            if (pData)      *pData      = data;
            return nRet;
        }
        return 0;
    }

    // ��ȡ����λ��, λ��Ϊ������
    virtual POINT GetCursorTablePos()
    {
        POINT pt = { 0 };
        _ERR_RET(pt);

        pt.y = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorTabColEnd)));
        pt.x = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd)));
        return pt;
    }

    // ���ù���λ��, λ��Ϊ������
    // pt = ������
    virtual void SetCursorTablePos(POINT pt)
    {
        _ERR_RET(_RET_NULL);
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorTabColStart))) = pt.y;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorTabColEnd))) = pt.y;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowStart))) = pt.x;
        (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd))) = pt.x;

        int pSelectRow = (*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo)));
        if (pSelectRow)
        {
            free_heap((void*)pSelectRow);
            (*((int*)(((int)pCWnd) + ECodeEdit_pSelectRowInfo))) = 0;
            (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoMaxLen))) = 0;
            (*((int*)(((int)pCWnd) + ECodeEdit_nSelectRowInfoLen))) = 0;
        }
    }

    // ��ȡ��ǰ����ָ��Ĵ������
    virtual HWND GetWindow()
    {
        _ERR_RET(0);
        HWND hWnd = (HWND)(*((int*)(((int)pCWnd) + CWnd_m_hWnd)));
        return hWnd;
    }
};

IEIDE_CodeManage* EIDE_CreateIEIDE_CodeManage(HWND hWnd)
{
    pEmptyCString = CStream_Create();
    *((int*)pEmptyCString) = -1;
    pEmptyCString += 12;
    EIDE_CodeManage* pObj = new EIDE_CodeManage;
    pObj->init(hWnd);
    return pObj;
}
void EIDE_DeleteIEIDE_CodeManage(IEIDE_CodeManage* pObj)
{
    EIDE_CodeManage* p = (EIDE_CodeManage*)pObj;
    if (!p) return;
    delete p;
}

inline LPBYTE _FindAddress(LPBYTE pBegin, LPBYTE pEnd, const void* findData, int findDataSize)
{
    for (LPBYTE p = (LPBYTE)pBegin; p != pEnd; p++)
    {
        if (memcmp(p, findData, findDataSize) == 0)
            return p;
    }
    return 0;
}


// �ڲ�����_��ȡ��ɫֵ�ڴ�������ַ
// begin = ��ɫ��ʼλ��
// end = ��ɫ����λ��
// len = �ð汾��ɫ����, ����Ǵ������Ե���Ѱ��������
inline bool _EColor_FindAddress(LPBYTE& begin, LPBYTE& end, int& len)
{
    static bool first = false;
    if (first) return len != 0;
    static LPBYTE pBegin = (LPBYTE)GetModuleHandleW(0);
    static LPBYTE pEnd = pBegin + 1376256;

    const BYTE find_595[] = { 80, 255, 21, 244, 119, 95, 0, 185, 96, 83, 103, 0 };    // 5.95
    LPBYTE address = _FindAddress(pBegin, pEnd, __FINDARG(find_595));
    if ( !address )
    {
        const BYTE find_593[] = { 80, 255, 21, 244, 119, 95, 0, 185, 64, 83, 103, 0 };    // 5.93
        address = _FindAddress(pBegin, pEnd, __FINDARG(find_593));
        if ( !address )
        {
            const BYTE find[] = { 11, 0, 185, 221, 0, 0, 0, 190 };    // 5.9 5.8 5.81 5.7
            address = _FindAddress(pBegin, pEnd, __FINDARG(find));
        }
    }


    if (!address)
    {
        EIDE_CodeManage::error("%s -> %d -> %s, !1û�ҵ�!,��ʼ���Եڶ��׷���\n", __FILE__, __LINE__, __FUNCTION__);
        const BYTE find56[] = { 199, 5, 152, 66, 92, 0, 5, 0, 6, 0 };  // 5.6
        address = _FindAddress(pBegin, pEnd, __FINDARG(find56));
        if (!address)
        {
            EIDE_CodeManage::error("%s -> %d -> %s, !2�ҵ�!,��ʼ���Ե����׷���\n", __FILE__, __LINE__, __FUNCTION__);
            const BYTE find55[] = { 199, 5, 200, 187, 91, 0, 5, 0, 5, 0 };  // 5.5
            address = _FindAddress(pBegin, pEnd, __FINDARG(find55));
            if (!address)
            {
                EIDE_CodeManage::error("%s -> %d -> %s, !3�ҵ�!,����ʹ��������5.6�����ϰ汾\n", __FILE__, __LINE__, __FUNCTION__);
                first = true;
                return false;
            }
            else
            {
                address = (LPBYTE)6011840;  //0x005BBBC0
            }
        }
        else
        {
            address = (LPBYTE)6046352;  //0x005C4290
        }
    }
    else
    {
        address += 8;
        address = (*((LPBYTE*)address));
    }

    begin = address + 92;
    const BYTE f1[] = { 129, 57, 107, 202, 156, 0, 116, 32 };
    address = _FindAddress(pBegin, pEnd, __FINDARG(f1));
    if (!address)
    {
        const BYTE f2[] = { 129, 57, 108, 202, 156, 0, 116, 16 };
        address = _FindAddress(pBegin, pEnd, __FINDARG(f2));
        if (!address)
        {
            const BYTE f3[] = { 129, 57, 109, 202, 156, 0, 117, 67 };
            address = _FindAddress(pBegin, pEnd, __FINDARG(f3));
        }
    }
    if (!address)
    {
        EIDE_CodeManage::error("%s -> %d -> %s, ��λ����û���ҵ�1,���ܽ�֧��5.7�����ϰ汾\n", __FILE__, __LINE__, __FUNCTION__);
        // ��λ����û���ҵ�1,���ܽ�֧��5.7�����ϰ汾
        first = true;
        return false;
    }

    const BYTE f4[] = { 129, 249 };
    address = _FindAddress(address, pEnd, __FINDARG(f4));
    if (!address)
    {
        EIDE_CodeManage::error("%s -> %d -> %s, ��λ����û���ҵ�2,���ܽ�֧��5.7�����ϰ汾\n", __FILE__, __LINE__, __FUNCTION__);
        // ��λ����û���ҵ�1,���ܽ�֧��5.7�����ϰ汾
        first = true;
        return false;
    }

    address += 2;
    len = (*((int*)address));
    end = begin + len;
    if (m_pCallback && !m_isSetCallback)
    {
        MonitorAddress(begin, len, MONITORTYPE_WRITE, errorCallback, 0);
    }
    first = true;
    return true;
}

// д��������clr��ɫ�ĵ�ַ, �������ڴ����Ըĳɿɶ�д��д��, д�����Ļز���д
inline void memcpy_color(void* p, const void* p2, size_t size)
{
    DWORD old;
    VirtualProtect(m_begin, m_len, PAGE_READWRITE, &old);
    memcpy(p, p2, size);
    VirtualProtect(m_begin, m_len, old, &old);
}

int EColor_SetData(void* pBuffer, int nBufferSize)
{
    if (!pBuffer) return 0;
    if (!_EColor_FindAddress(m_begin, m_end, m_len))
        return 0;

    LPBYTE ptr = 0;
    if (nBufferSize == 0)
    {
        // pBuffer = �ļ�·��
        CFileRW f;
        if (!f.open((LPCWSTR)pBuffer))
            return 0;
        nBufferSize = f.size();
        ptr = new BYTE[nBufferSize];
        f.read(ptr, nBufferSize);
        pBuffer = ptr;
    }
    // pBuffer = �ļ�����

    LPBYTE p = (LPBYTE)pBuffer;
    p += 4;

    int size = nBufferSize - 4;
    if (size > m_len)size = m_len;
    memcpy_color(m_begin, p, size);

    if (ptr) delete[] ptr;
    return size;
}
void EColor_SetBeaEnginePath(LPCWSTR file)
{
    SetBeaEngineDllPath(file);
}
void EColor_ColorChangeCallback(PFN_COLORCHANGE pCallback)
{
    if (!m_isSetCallback && m_begin)
    {
        MonitorAddress(m_begin, m_len, MONITORTYPE_WRITE, errorCallback, 0);
        m_isSetCallback = true;
    }

    m_pCallback = pCallback;
}
int EColor_GetData(void* pBuffer, int nBufferSize)
{
    if (!_EColor_FindAddress(m_begin, m_end, m_len))
        return 0;
    if (m_len > 0x7FFF) m_len = 0;
    if (!pBuffer || !nBufferSize) return m_len + 4;

    if (!m_len) return 0;

    const BYTE c[] = { 109, 202, 156, 0 };
    memcpy(pBuffer, c, 4);
    pBuffer = (LPBYTE)pBuffer + 4;

    int size = m_len;   // m_len����ɫ���ݴ�С, �������Ҫд����ô���ֽ�
    if (size + 4 > nBufferSize)
        size = nBufferSize - 4;
    memcpy(pBuffer, m_begin, size);
    return size;
}

void CALLBACK errorCallback(LPVOID pAddress, MONITORTYPE type, UINT64 param)
{
    if (m_pCallback)
        m_pCallback();
}

