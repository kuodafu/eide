#pragma once
#include <windows.h>
#include <commctrl.h>
#pragma comment(lib,"Comctl32.lib")
//#include <crc32.h>

class CImgList
{
    HIMAGELIST m_hImageList;
    //CRC32_UINT m_crc32;
private:
    static IStream* _CreateIStream(LPBYTE data, DWORD size)
    {
        IStream* pStream = 0;
        HGLOBAL h = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, size);
        if (h)
        {
            HRESULT hr = CreateStreamOnHGlobal(h, TRUE, &pStream);
            if (SUCCEEDED(hr) && pStream)
            {
                if (data)
                {
                    void* ptr = GlobalLock(h);
                    if (ptr)
                        memcpy(ptr, data, size);
                    ::GlobalUnlock(h);
                }
                return pStream;
            }
            GlobalFree(h);
        }
        return 0;
    }

    // 从流对象里获取字节集数据
    static LPBYTE _GetIStreamBin(IStream* pStream, SIZE_T& size, BOOL isE)
    {
        size = 0;
        HGLOBAL hMem = 0;

        HRESULT hr = GetHGlobalFromStream(pStream, &hMem);
        if (!hMem || FAILED(hr)) return 0;

        void* ptr = GlobalLock(hMem);
        LPBYTE pData = 0;
        if (ptr)
        {
            const BYTE tmp[] = { 77, 73, 84, 76, 0, 0, 0, 255 };
            const int offset = isE ? sizeof(tmp) : 0;
            size = GlobalSize(hMem) + offset;
            pData = new BYTE[size];
            memcpy(pData + offset, ptr, size - offset);
            if ( offset && size > (SIZE_T)offset )
            {
                // 前4个字节 = (MAKELONG('IM', 'LT')); = 77, 73, 84, 76
                memcpy(pData, tmp, offset);
            }
            ::GlobalUnlock(hMem);
        }
        return pData;
    }

public:
    CImgList() :m_hImageList(0)/*, m_crc32(0)*/ { }
    virtual ~CImgList() { Destroy(); }
public:
    inline operator HIMAGELIST() { return m_hImageList; }
    inline BOOL Destroy()
    {
        //m_crc32 = 0;
        if ( m_hImageList )
        {
            if (ImageList_Destroy(m_hImageList))
            {
                m_hImageList = 0;
                return true;
            }
        }
        return false;
    }

    // 创建图片组, 如果当前类已经有图片组, 那会销毁当前图片组然后创建
    // cx = 每个图像的宽度(以像素为单位)
    // cy = 每个图像的高度(以像素为单位)
    // flags = 一组位标志，用于指定要创建的图像列表的类型, ILC_ 开头常量, 默认为 ILC_COLOR32 | ILC_MASK
    // count = 图片组数量
    inline HIMAGELIST create(int cx, int cy, UINT flags = ILC_COLOR32 | ILC_MASK, int count = 0, int maxCount = 0)
    {
        Destroy();
        maxCount = max(count, maxCount);
        m_hImageList = ImageList_Create(cx, cy, flags, count, maxCount);
        return m_hImageList;
    }

    // 从字节集创建图片组, 内部智能判断是否为易语言格式, 如果当前类已经有图片组, 那会销毁当前图片组然后创建
    // data = 图片组数据指针
    // size = data的数据长度
    inline HIMAGELIST create(LPBYTE data, DWORD size)
    {
        if (!data || size == 0) return 0;

        LPBYTE pData = data;
        int dwLen = size;
        bool ise = false;
        COLORREF clrBk = 0;
        if (size > 8)
        {
            const DWORD IMAGE_LIST_MARK = (MAKELONG('IM', 'LT'));
            LPDWORD pHead = (LPDWORD)data;
            if (pHead[0] == IMAGE_LIST_MARK)
            {
                ise = true;
                clrBk = pHead[1];
                pData += 8;
                dwLen -= 8;
            }
        }
        
        //CRC32_UINT crc32 = _crc32::crc32(0, pData, dwLen);
        //if ( crc32 == m_crc32 )
        //    return m_hImageList;    // 传递了相同的数据, 不需要重新创建
        Destroy();
        IStream* pStream = _CreateIStream(pData, dwLen);
        if (!pStream) return 0;

        //m_crc32 = crc32;    // 记录当前数据的crc32值, 下次创建之前先比对是不是一样
        m_hImageList = ImageList_Read(pStream);
        pStream->Release();
        return m_hImageList;
    }

    // 从位图文件创建图片组, 如果当前类已经有图片组, 那会销毁当前图片组然后创建
    // lpbmp = 位图文件路径
    // width = 宽度
    // IconCount = 图标数
    // crMask = 掩码色
    inline HIMAGELIST create(LPCWSTR lpbmp, int width, int IconCount, COLORREF crMask = -1)
    {
        Destroy();
        m_hImageList = ImageList_LoadImageW(0, lpbmp, width, IconCount, crMask, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        return m_hImageList;
    }
    inline void swap(CImgList& imgList)
    {
        HIMAGELIST hList = imgList.m_hImageList;
        //CRC32_UINT crc32 = imgList.m_crc32;
        imgList.m_hImageList = m_hImageList;
        //imgList.m_crc32 = m_crc32;
        m_hImageList = hList;
        //m_crc32 = crc32;
    }
    inline HIMAGELIST GetHIml() { return m_hImageList; }

    // 附加图片组, 返回旧的图片组句柄
    inline HIMAGELIST Attach(HIMAGELIST hImageList)
    {
        HIMAGELIST hOld = m_hImageList;
        m_hImageList = hImageList;
        //m_crc32 = 0;
        return hOld;
    }
    inline int add(HICON hIcon)
    {
        return ImageList_AddIcon(m_hImageList, hIcon);
    }
    inline HIMAGELIST clone()
    {
        return ImageList_Duplicate(m_hImageList);
    }
    inline int ReplaceIcon(int index, HICON hIcon)
    {
        return ImageList_ReplaceIcon(m_hImageList, index, hIcon);
    }
    inline BOOL Delete(int index)
    {
        return ImageList_Remove(m_hImageList, index);
    }
    inline COLORREF GetBkColor()
    {
        return ImageList_GetBkColor(m_hImageList);
    }
    inline COLORREF SetBkColor(COLORREF clr)
    {
        return ImageList_SetBkColor(m_hImageList, clr);
    }
    // 返回的图标句柄需要调用 DestroyIcon 销毁
    inline HICON GetIcon(int index, int flag = ILD_NORMAL)
    {
        return ImageList_GetIcon(m_hImageList, index, flag);
    }
    // 检索有关图像的信息, 返回的图标句柄不能调用 DeleteObject 销毁
    inline BOOL GetImageInfo(int index, IMAGEINFO* pImageInfo)
    {
        return ImageList_GetImageInfo(m_hImageList, index, pImageInfo);
    }
    inline BOOL GetIconSize(int* width, int* height)
    {
        return ImageList_GetIconSize(m_hImageList, width, height);
    }
    inline BOOL SetIconSize(int cx, int cy)
    {
        return ImageList_SetIconSize(m_hImageList, cx, cy);
    }
    inline int GetIconCount() { return ImageList_GetImageCount(m_hImageList); }
    inline BOOL SetIconCount(int count) { return ImageList_SetImageCount(m_hImageList, count); }

    // 保存当前图片组到字节集
    // size = 返回图片组文件的长度
    // isE  = 是否为易语言格式
    inline LPBYTE save(int* size, BOOL isE = false)
    {
        if (size)*size = 0;
        LPBYTE ret = 0;
        IStream* pStream = _CreateIStream(0, 0);
        if (!pStream) return 0;


        if (ImageList_Write(m_hImageList, pStream))
        {
            SIZE_T binSize;
            ret = _GetIStreamBin(pStream, binSize, isE);
            if (size)*size = (int)binSize;
        }
        pStream->Release();
        return ret;
    }
    inline BOOL DrawIcon(int index, HDC hdc, int x, int y, int flag = ILD_NORMAL)
    {
        return ImageList_Draw(m_hImageList, index, hdc, x, y, flag);
    }
    inline BOOL DrawIcon(int index, HDC hdc, int x, int y, int cx, int cy, COLORREF clrBk, COLORREF clrFg, int flag = 0)
    {
        return ImageList_DrawEx(m_hImageList, index, hdc, x, y, cx, cy, clrBk, clrFg, flag);
    }
    inline BOOL BeginDrag(int index, int x, int y)
    {
        return ImageList_BeginDrag(m_hImageList, index, x, y);
    }
    inline BOOL DragEnter(HWND hWnd, int x, int y)
    {
        return ImageList_DragEnter(hWnd, x, y);
    }
    inline BOOL DragShowNolock(BOOL b)
    {
        return ImageList_DragShowNolock(b);
    }
    inline BOOL DragMove(int x, int y)
    {
        return ImageList_DragMove(x, y);
    }
    inline BOOL DragLeave(HWND hWnd)
    {
        return ImageList_DragLeave(hWnd);
    }
    inline void EndDrag()
    {
        ImageList_EndDrag();
    }
};

