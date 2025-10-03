#include "EIDE_Interface.h"
#include <stdio.h>
#include "EIDE_InternalMethods.h"
#include <atlstr.h>
#include <assist//CFileRW.h>
#include <unordered_map>
#include <Memory_BeaEngine.h>
#include <Memory_MonitorAddress.h>



static int pEmptyCString;
static LPVOID hException;   // 异常管理句柄
static DWORD m_oldProtect;  // 颜色表的内存属性
static PFN_COLORCHANGE m_pCallback; // 颜色被改变的回调
static bool m_isSetCallback;        // 是否设置了回调, 在获取到配色配置的时候判断
struct ERRCALLBACK_DATA
{
    BYTE oldByte;
    LPVOID pErrAddress;
};
static std::unordered_map<LPVOID, ERRCALLBACK_DATA> s_map_opCode;   // 键名是被修改成cc的地址, 触发CC断点就把cc改成记录的值

static LPBYTE m_begin;  // 易语言配色在内存里的起始地址
static LPBYTE m_end;    // 易语言配色在内存里的结束地址
static int m_len;       // 易语言配色在内存里占用的尺寸

static LPBYTE m_pageStart;  // 内存页起始地址
static LPBYTE m_pageEnd;    // 内存页结束地址, 一般是 开始地址 + 0x1000

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
    case  代码类型_程序集:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 84)));
        break;
    case 代码类型_数据类型:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 64)));
        break;
    case 代码类型_全局变量:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 68)));
        break;
    case 代码类型_Dll命令:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 72)));
        break;
    case 代码类型_常量:
    case 代码类型_资源图片:
    case 代码类型_资源音频:
        codeInfo[1] = (*((int*)(pECodeEditWnd + 80)));
        break;
    default:
        break;
    }

}

// IDE代码框管理
class EIDE_CodeManage : public IEIDE_CodeManage
{
private:
    void* pCWnd;
    void* pfnGetLineInfo;
    static constexpr auto FEEDBACK_GROUP{ "群：658718319" };  // 反馈群号

#define _RET_NULL
    // 判断CWnd指针是否为空, 为空的话就返回
#define _ERR_RET(_ret) if (!pCWnd) {\
                            error("error 发生错误! 文件 = " __FILE__ ", 函数 = " __FUNCTION__ ", 行号 = %d  请将此错误信息反馈给开发人员  %s" , __LINE__, FEEDBACK_GROUP);\
                            return _ret;\
                        }
public:
    // 错误上报子程序
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

        //TODO 这里需要上报错误, 没写上报的功能, 现在输出就行了
        OutputDebugStringA(buffer);
        delete[] buffer;
        return;

    }
public:
    EIDE_CodeManage() :pCWnd(0), pfnGetLineInfo(0) { }
    // 从窗口句柄中初始化本类
    virtual bool init(HWND hWnd)
    {
        if (!IsWindow(hWnd))
        {
            error("发生错误! 文件 = " __FILE__ ", 函数 = " __FUNCTION__ ", 行号 = %d  请将此错误信息反馈给开发人员  %s", __LINE__, FEEDBACK_GROUP);
            return false;
        }
        pCWnd = CWnd_FromHandle(hWnd);
        _ERR_RET(false);
        return true;
    }
    // 销毁
    virtual void destroy() { pCWnd = 0; pfnGetLineInfo = 0; }
    // 获取光标位置, 位置为表格内部序号
    virtual POINT GetCursorPos()
    {
        POINT pt{ -1, -1 };
        _ERR_RET(pt);

        pt.y = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorColumnEnd)));
        pt.x = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd)));
        return pt;
    }
    // 设置光标位置, 会把表列号置为0
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
    // 获取选中信息
    // pStartCol = 接收起始列
    // pEndCol = 接收结束列
    // pNowLine = 接收当前行
    // pSel = 接收选中范围
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

    // 设置选中信息
    // pStartCol = 起始列指针, 为0则不写入
    // pEndCol = 结束列指针, 为0则不写入
    // pNowLine = 当前行指针, 为0则不写入
    // pNowLine = 当前行指针, 为0则不写入
    // pSel = 选中范围指针, 为0则不写入
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

    // 是否选中代码
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

    // 获取选中行数
    virtual int GetSelLineCount()
    {
        _ERR_RET(0);
        POINT sel = { 0 };
        GetSelInfo(0, 0, 0, &sel);
        if (sel.x == -1 || sel.y == -1)
            return 0;
        return sel.y - sel.x + 1;
    }

    // 获取选中列数
    virtual int GetSelColumnCount()
    {
        _ERR_RET(0);
        int start = 0, end = 0;
        GetSelInfo(&start, &end, 0, 0);

        return abs(end - start);
    }

    // 获取行信息
    // lineNumber = 行号
    // flags = 项标识, ETAG
    // pData = 项数据指针, EBase*
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
                    error("发生错误! 文件 = " __FILE__ ", 函数 = " __FUNCTION__ ", 行号 = %d  请将此错误信息反馈给开发人员  %s", __LINE__, FEEDBACK_GROUP);
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

    // 获取代码类型
    virtual int GetCodeType()
    {
        _ERR_RET(代码类型_无);
        return (*((int*)(((int)pCWnd) + ECodeEdit_nCodeType)));
    }

    // 获取对象指针
    virtual void* GetObjectThis() { return pCWnd; }

    // 设置对象指针
    virtual void SetObjectThis(void* pThis) { pCWnd = (LPBYTE)pThis; pfnGetLineInfo = 0; }

    // 光标定位
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

    // 保证显示光标位置
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

    // 获取代码文本
    // nTableIndex = 表行
    // nTableindexCol = 表列
    virtual LPSTR GetCodeText(int nTableIndex, int nTableindexCol)
    {
        //TODO 获取文本有问题, 不能使用下面的代码, 编译版会有问题, 得修改
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
    // 申请内存
    virtual void* alloc(int size)
    {
        char* p = (char*)malloc(size);
        if (!p) return 0;
        memset(p, 0, size);
        return p;
    }
    // 释放内存, 只能传递本类对象 alloc() 方法返回的指针, 传递其他指针可能会出问题
    virtual void free(void* p)
    {
        ::free(p);
    }

    // 获取代码数据偏移
    // nTableIndex = 表行
    // nTableindexCol = 表列
    // type = 接收类型指针
    // offset = 接收偏移指针
    // pData = 数据指针, EBase*
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

    // 获取光标表位置, 位置为表格序号
    virtual POINT GetCursorTablePos()
    {
        POINT pt = { 0 };
        _ERR_RET(pt);

        pt.y = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorTabColEnd)));
        pt.x = (*((int*)(((int)pCWnd) + ECodeEdit_nCursorRowEnd)));
        return pt;
    }

    // 设置光标表位置, 位置为表格序号
    // pt = 表格序号
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

    // 获取当前对象指向的代码框句柄
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


// 内部方法_获取颜色值内存特征地址
// begin = 颜色开始位置
// end = 颜色结束位置
// len = 该版本颜色长度, 这个是从易语言当中寻找特征的
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
        EIDE_CodeManage::error("%s -> %d -> %s, !1没找到!,开始尝试第二套方案\n", __FILE__, __LINE__, __FUNCTION__);
        const BYTE find56[] = { 199, 5, 152, 66, 92, 0, 5, 0, 6, 0 };  // 5.6
        address = _FindAddress(pBegin, pEnd, __FINDARG(find56));
        if (!address)
        {
            EIDE_CodeManage::error("%s -> %d -> %s, !2找到!,开始尝试第三套方案\n", __FILE__, __LINE__, __FUNCTION__);
            const BYTE find55[] = { 199, 5, 200, 187, 91, 0, 5, 0, 5, 0 };  // 5.5
            address = _FindAddress(pBegin, pEnd, __FINDARG(find55));
            if (!address)
            {
                EIDE_CodeManage::error("%s -> %d -> %s, !3找到!,建议使用易语言5.6及以上版本\n", __FILE__, __LINE__, __FUNCTION__);
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
        EIDE_CodeManage::error("%s -> %d -> %s, 定位特征没有找到1,功能仅支持5.7及以上版本\n", __FILE__, __LINE__, __FUNCTION__);
        // 定位特征没有找到1,功能仅支持5.7及以上版本
        first = true;
        return false;
    }

    const BYTE f4[] = { 129, 249 };
    address = _FindAddress(address, pEnd, __FINDARG(f4));
    if (!address)
    {
        EIDE_CodeManage::error("%s -> %d -> %s, 定位特征没有找到2,功能仅支持5.7及以上版本\n", __FILE__, __LINE__, __FUNCTION__);
        // 定位特征没有找到1,功能仅支持5.7及以上版本
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

// 写入易语言clr颜色的地址, 这里会把内存属性改成可读写再写入, 写入完后改回不可写
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
        // pBuffer = 文件路径
        CFileRW f;
        if (!f.open((LPCWSTR)pBuffer))
            return 0;
        nBufferSize = f.size();
        ptr = new BYTE[nBufferSize];
        f.read(ptr, nBufferSize);
        pBuffer = ptr;
    }
    // pBuffer = 文件数据

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

    int size = m_len;   // m_len是颜色数据大小, 理想情况要写入这么多字节
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

