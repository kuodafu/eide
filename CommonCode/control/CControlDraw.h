#pragma once
#include <comdef.h>
// 内存DC, 位图
typedef struct MEMDCBITMAP
{
    HDC hdc;            // 内存DC
    HBITMAP hBitmap;    // 32位绘制位图
    int width;          // 位图宽度, width * height = pBits 像素点数量, 像素点数量 * 4 = pBits 的字节数
    int height;         // 位图高度, width * height = pBits 像素点数量, 像素点数量 * 4 = pBits 的字节数
    LPBYTE lpBits;      // 指向位图数据指针, 可以直接修改这个地址里的数据达到绘画的效果, 4个字节为一个像素点
}*PMEMDCBITMAP, * LPMEMDCBITMAP;


// 创建32位与设备无关位图
// hdc = 设备上下文
// width = 位图宽度
// height = 位图高度
// pBits = 传递一个指针接收位图数据地址
inline HBITMAP Create32Bitmap(HDC hdc, int width, int height, void** ppBits)
{
    BITMAPINFO bm = { 0 };
    bm.bmiHeader.biSize = sizeof(bm.bmiHeader);
    bm.bmiHeader.biWidth = width;
    bm.bmiHeader.biHeight = height;
    bm.bmiHeader.biPlanes = 1;
    bm.bmiHeader.biBitCount = 32;
    bm.bmiHeader.biSizeImage = bm.bmiHeader.biWidth * bm.bmiHeader.biHeight * bm.bmiHeader.biBitCount / 8;
    void* pBits = 0;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bm, DIB_RGB_COLORS, &pBits, 0, 0);
    if (ppBits)*ppBits = pBits;
    return hBitmap;
}

#ifndef ULW_ALPHA
#define ULW_ALPHA   0x00000002
#endif
// 更新分层窗口, 返回错误码, 0=成功
// hWnd = 要重画的窗口
// width = 窗口宽度
// height = 窗口高度
// srcDC = 从哪个dc拷贝到窗口上
// ppt = 窗口新的位置
// alpha = 窗口透明度
inline int UpdateLayered(HWND hWnd, int width, int height, HDC srcDC, PPOINT ppt = 0, int alpha = 255)
{
    int ret = 0;
    HDC hdc = ::GetDC(hWnd);
    SIZE size = { width, height };
    POINT pt = { 0 };
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = (BYTE)alpha;
    BOOL b = UpdateLayeredWindow(hWnd, hdc, ppt, &size, srcDC, &pt, 0, &blend, ULW_ALPHA);
    if (!b)
        ret = GetLastError();
    ReleaseDC(hWnd, hdc);
    return ret;
}

// 释放内存DC
// pmd = md_create() 返回的值
// isFree = 是否释放pmd这个指针, 如果为假, 内部只释放hdc和hBitmap并清零, 为真则把pmd也释放
inline bool md_destroy(LPMEMDCBITMAP pmd, bool isFree = true)
{
    if (!pmd)return false;
    bool ret = false;
    if (pmd->hdc)
    {
        DeleteObject(pmd->hBitmap);
        DeleteDC(pmd->hdc);
        memset(pmd, 0, sizeof(MEMDCBITMAP));
        ret = true;
    }
    if (isFree)
        delete pmd;
    return ret;
}

// 重新调整内存dc尺寸, 如果尺寸一样则不修改, 如果传入的md没有创建dc, 则内部会创建一个
// pmd = 需要重新调整尺寸的md
// width = 位图宽度
// height = 位图高度
inline bool md_resize(LPMEMDCBITMAP pmd, int width, int height)
{
    if (!pmd)return false;
    if (width == pmd->width && height == pmd->height && width > 0 && height > 0) return true;

    if (width <= 0)width = 1;
    if (height <= 0)height = 1;
    pmd->width = width;
    pmd->height = height;

    HDC hdc = pmd->hdc;
    if (!hdc) hdc = CreateCompatibleDC(0);
    if (hdc)
    {
        BITMAPINFO bi = { 0 };
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biSizeImage = width * height * bi.bmiHeader.biBitCount / 8;
        void* lpBits = 0;
        HBITMAP hBmp = CreateDIBSection(hdc, &bi, 0, &lpBits, 0, 0);
        if (hBmp)
        {
            if (pmd->hBitmap)
                DeleteObject(pmd->hBitmap);

            SelectObject(hdc, hBmp);
            pmd->hdc = hdc;
            pmd->hBitmap = hBmp;
            pmd->lpBits = (LPBYTE)lpBits;
            pmd->width = width;
            pmd->height = height;
            return true;
        }

        DeleteDC(hdc);
        return false;
    }
    return false;
}

// 创建内存dc和32位位图
// width = 位图宽度
// height = 位图高度
// pmd = 结果保存位置, 如果这个值不为0, 则内部会把申请的hdc和hBitmap写入这个指针, 返回值是返回这个指针
inline LPMEMDCBITMAP md_create(int width, int height, LPMEMDCBITMAP pmd = 0)
{
    if (width <= 0)width = 1;
    if (height <= 0)height = 1;
    if (pmd)
    {
        md_destroy(pmd, false);
    }
    else
    {
        pmd = new MEMDCBITMAP;
        memset(pmd, 0, sizeof(MEMDCBITMAP));
    }

    md_resize(pmd, width, height);
    return pmd;
}


// 从位图句柄里获取位图图片数据, hBitmap 传递0则调用失败, 返回的地址需要调用 delete[] 释放, 如果传入位图句柄宽或高等于0, 则返回0
// 从hBitmap里获取位图数据
// size = 接收位图大小的指针
inline LPBYTE GetBitmapDataFromBitmap(HBITMAP hBitmap, int* size = 0, HDC hSrcDC = 0)
{
    if (size)*size = 0;
    if (!hBitmap)return 0;

    LPBITMAPFILEHEADER pFilehead;
    LPBITMAPINFOHEADER pBitmapinfohead;
    BITMAP bm = { 0 };
    GetObjectW(hBitmap, sizeof(BITMAP), &bm);
    if (bm.bmHeight <= 0 || bm.bmWidth == 0)
        return 0;
    LPMEMDCBITMAP md = 0;
    if (bm.bmBits == 0) // 传入的位图没有数据指针, 创建一个新的dc保存
    {
        md = md_create(bm.bmWidth, bm.bmHeight);
        BitBlt(md->hdc, 0, 0, bm.bmWidth, bm.bmHeight, hSrcDC, 0, 0, SRCCOPY); // 在拷贝到新的dc里
        hBitmap = md->hBitmap;
        bm.bmBits = md->lpBits;
        bm.bmBitsPixel = 32;
        bm.bmPlanes = 1;
    }

    const int bmpSize = bm.bmWidth * bm.bmHeight * bm.bmBitsPixel / 8 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if (size)*size = bmpSize;
    LPBYTE ret = new BYTE[bmpSize];
    memset(ret, 0, bmpSize);
    pFilehead = (LPBITMAPFILEHEADER)ret;
    pBitmapinfohead = (LPBITMAPINFOHEADER)(pFilehead + 1);

    pFilehead->bfType = MAKEWORD('B', 'M');
    pFilehead->bfSize = bmpSize;
    pFilehead->bfReserved1 = 0;
    pFilehead->bfReserved2 = 0;
    pFilehead->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    pBitmapinfohead->biSize = sizeof(BITMAPINFOHEADER);
    pBitmapinfohead->biWidth = bm.bmWidth;
    pBitmapinfohead->biHeight = bm.bmHeight;
    pBitmapinfohead->biPlanes = bm.bmPlanes;
    pBitmapinfohead->biBitCount = bm.bmBitsPixel;
    pBitmapinfohead->biCompression = 0;
    pBitmapinfohead->biSizeImage = 0;
    pBitmapinfohead->biXPelsPerMeter = 0;
    pBitmapinfohead->biYPelsPerMeter = 0;
    pBitmapinfohead->biClrUsed = 0;
    pBitmapinfohead->biClrImportant = 0;

    LPBYTE ptr = (LPBYTE)(pBitmapinfohead + 1);
    //GetDIBits(hdc, hBitmap, 0, bm.bmHeight, ptr, (LPBITMAPINFO)pBitmapinfohead, 0);
    memcpy(ptr, bm.bmBits, bmpSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER));

    md_destroy(md);
    return ret;

}

// 从HDC里获取位图图片数据, hdc传递0则调用失败, 返回的地址需要调用 delete[] 释放
// 如果传入HDC的位图句柄宽或高等于0, 则返回0
inline LPBYTE GetBitmapDataFromHDC(HDC hdc, int* size = 0)
{
    if (size)*size = 0;
    if (!hdc)return 0;
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
    if (!hBitmap)return 0;
    return GetBitmapDataFromBitmap(hBitmap, size, hdc);
}

// 从窗口句柄里获取位图图片数据, 窗口句柄传入0则返回整个桌面, 返回的地址需要调用 delete[] 释放
inline LPBYTE GetBitmapDataFromHwnd(HWND hWnd, int* size = 0)
{
    if (size)*size = 0;
    HDC hdc = GetDC(hWnd);
    if (!hdc)return 0;
    LPBYTE ret = GetBitmapDataFromHDC(hdc, size);
    ReleaseDC(hWnd, hdc);
    return ret;
}


// 从gdi+位图里保存成png图片, gdi+未初始化则返回0
// gpBitmap = gdi+位图
// pSize = 提供一个地址接收返回数据的大小
// pClsid = 转换的格式, 为0则使用png的clsid
inline LPBYTE SavePngFromGdiplusBitmap(void* gpBitmap, int* pSize, CLSID* pClsid = 0)
{
    if (pSize)*pSize = 0;
    static HMODULE hModule = 0;
    CLSID clsid;
    typedef int     (WINAPI* fun_GdipSaveImageToStream)(HANDLE img, IStream* stream, LPCLSID clsid, int encoderParams);
    static fun_GdipSaveImageToStream pGdipSaveImageToStream;;
    if (!hModule)
    {
        hModule = GetModuleHandleW(L"gdiplus.dll");
        if (!hModule)return 0;
        pGdipSaveImageToStream = (fun_GdipSaveImageToStream)GetProcAddress(hModule, "GdipSaveImageToStream");
    }
    if (!pGdipSaveImageToStream)return 0;

    IStream* stream;
    LPBYTE ret = 0;
    HRESULT hr = CreateStreamOnHGlobal(0, 1, &stream);
    HGLOBAL hMem = 0;
    while (SUCCEEDED(hr))
    {
        if (pClsid) clsid = *pClsid;
        else hr = CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &clsid);

        hr = pGdipSaveImageToStream(gpBitmap, stream, &clsid, 0);
        if (hr != 0)break;

        hr = GetHGlobalFromStream(stream, &hMem);
        if (FAILED(hr)) break;

        void* pData = GlobalLock(hMem);
        if (pData)
        {
            SIZE_T size = GlobalSize(hMem);
            ret = new BYTE[size];
            memcpy(ret, pData, size);
            GlobalUnlock(hMem);
            if (pSize)*pSize = (int)size;
        }

        break;
    }
    if (stream)stream->Release();
    return ret;
}

// 从位图句柄里获取png图片数据, 如果传入位图句柄宽或高等于0, 则返回0
// hBitmap = 要获取的位图
// size = 接收png图片大小的指针
// hSrcDC = 源dc, 是保存 hBitmap的DC, 获取失败时会从这个dc拷贝数据
inline LPBYTE GetPngDataFromBitmap(HBITMAP hBitmap, int* size = 0, HDC hSrcDC = 0)
{
    if (size)*size = 0;
    if (!hBitmap)return 0;


    BITMAP bm = { 0 };
    GetObjectW(hBitmap, sizeof(BITMAP), &bm);
    if (bm.bmHeight <= 0 || bm.bmWidth == 0) return 0;
    if (bm.bmBits == 0 && hSrcDC == 0) return 0;

    struct gpBitmapData
    {
    public:
        UINT Width;
        UINT Height;
        INT Stride;
        int PixelFromat;
        void* Scan0;
        UINT_PTR Reserved;
    };
    struct GPRECT
    {
        int    left;
        int    top;
        int    width;
        int    height;
    };

    typedef void* GPBMP;
    typedef int     (WINAPI* pfn_GdipCreateBitmapFromScan0)(INT width, INT height, INT stride, INT format, BYTE* scan0, GPBMP* bitmap);
    typedef int     (WINAPI* pfn_GdipBitmapLockBits)(GPBMP bitmap, GPRECT* rect, UINT flags, int format, gpBitmapData* lockedBitmapData);
    typedef int     (WINAPI* pfn_GdipBitmapUnlockBits)(GPBMP bitmap, gpBitmapData* lockedBitmapData);

    typedef HRESULT(WINAPI* fun_GdipCreateBitmapFromStream)(IStream* stream, HANDLE* p);
    typedef int     (WINAPI* fun_GdiplusStartup)(ULONG_PTR* uToken, void* input, void* output);
    typedef void    (WINAPI* fun_GdiplusShutdown)(ULONG_PTR uToken);
    typedef int     (WINAPI* fun_GdipSaveImageToStream)(HANDLE img, IStream* stream, LPCLSID clsid, int encoderParams);
    typedef int     (WINAPI* fun_GdipDisposeImage)(HANDLE img);


    static HMODULE hModule;
    static fun_GdipCreateBitmapFromStream pGdipCreateBitmapFromStream;
    static fun_GdiplusStartup pGdiplusStartup;
    static fun_GdiplusShutdown pGdiplusShutdown;
    static fun_GdipSaveImageToStream pGdipSaveImageToStream;;
    static fun_GdipDisposeImage pGdipDisposeImage;

    static pfn_GdipCreateBitmapFromScan0 pGdipCreateBitmapFromScan0;
    static pfn_GdipBitmapLockBits pGdipBitmapLockBits;
    static pfn_GdipBitmapUnlockBits pGdipBitmapUnlockBits;

    if (!hModule || !pGdipCreateBitmapFromStream || !pGdiplusStartup || !pGdiplusShutdown || !pGdipSaveImageToStream || !pGdipDisposeImage
        || !pGdipCreateBitmapFromScan0 || !pGdipBitmapLockBits || !pGdipBitmapUnlockBits)
    {
        hModule = LoadLibraryW(L"gdiplus.dll");
        if (!hModule)return 0;
        pGdipCreateBitmapFromStream = (fun_GdipCreateBitmapFromStream)GetProcAddress(hModule, "GdipCreateBitmapFromStream");
        pGdiplusStartup = (fun_GdiplusStartup)GetProcAddress(hModule, "GdiplusStartup");
        pGdiplusShutdown = (fun_GdiplusShutdown)GetProcAddress(hModule, "GdiplusShutdown");
        pGdipSaveImageToStream = (fun_GdipSaveImageToStream)GetProcAddress(hModule, "GdipSaveImageToStream");
        pGdipDisposeImage = (fun_GdipDisposeImage)GetProcAddress(hModule, "GdipDisposeImage");

        pGdipCreateBitmapFromScan0 = (pfn_GdipCreateBitmapFromScan0)GetProcAddress(hModule, "GdipCreateBitmapFromScan0");
        pGdipBitmapLockBits = (pfn_GdipBitmapLockBits)GetProcAddress(hModule, "GdipBitmapLockBits");
        pGdipBitmapUnlockBits = (pfn_GdipBitmapUnlockBits)GetProcAddress(hModule, "GdipBitmapUnlockBits");
    }

    // 取几个函数地址,如果有其中一个取失败,则返回
    if (!pGdipCreateBitmapFromStream || !pGdiplusStartup || !pGdiplusShutdown || !pGdipSaveImageToStream || !pGdipDisposeImage
        || !pGdipCreateBitmapFromScan0 || !pGdipBitmapLockBits || !pGdipBitmapUnlockBits)
    {
        //if (szError)*szError = L"GdipCreateBitmapFromStream,GdiplusStartup,GdiplusShutdown,GdipSaveImageToStream,GdipDisposeImage\r\n其中一个获取失败";
        return 0;
    }

    ULONG_PTR uToken = 0;
    int input[4] = { 0 };
    input[0] = 1;
    int hr = pGdiplusStartup(&uToken, &input, 0);        // 初始化gdi+
    if (hr != 0)
    {
        //if (szError)*szError = L"gdi+初始化失败";
        return 0;
    }


    LPMEMDCBITMAP md = 0;
    void* pBits = bm.bmBits;
    if (pBits == 0)     // 传入的位图没有数据指针, 创建一个新的dc保存
    {
        md = md_create(bm.bmWidth, bm.bmHeight);
        BitBlt(md->hdc, 0, 0, bm.bmWidth, bm.bmHeight, hSrcDC, 0, 0, SRCCOPY); // 在拷贝到新的dc里
        hBitmap = md->hBitmap;
        pBits = md->lpBits;
        bm.bmBitsPixel = 32;
    }


    const int gpPixelFormat32bppARGB = (10 | (32 << 8) | 0x00040000 | 0x00020000 | 0x00200000);
    const int gpImageLockModeRead = 1;
    const int gpImageLockModeWrite = 2;
    gpBitmapData data;
    GPBMP gpbmp = 0;
    GPRECT gprc = { 0, 0, bm.bmWidth, bm.bmHeight };
    hr = pGdipCreateBitmapFromScan0(bm.bmWidth, bm.bmHeight, 0, gpPixelFormat32bppARGB, 0, &gpbmp);
    hr = pGdipBitmapLockBits(gpbmp, &gprc, gpImageLockModeRead | gpImageLockModeWrite, gpPixelFormat32bppARGB, &data);
    const int sizeImage = bm.bmWidth * bm.bmHeight * bm.bmBitsPixel / 8;
    memcpy(data.Scan0, pBits, sizeImage);
    pGdipBitmapUnlockBits(gpbmp, &data);


    LPBYTE retPng = SavePngFromGdiplusBitmap(gpbmp, size);


    md_destroy(md);
    pGdiplusShutdown(uToken);
    return retPng;
}


// 从HDC里获取png图片数据, 窗口句柄传入0则返回整个桌面, 返回的地址需要调用 delete[] 释放
inline LPBYTE GetPngDataFromHDC(HDC hdc, int* size = 0)
{
    if (size)*size = 0;
    if (!hdc)return 0;
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
    if (!hBitmap)return 0;
    return GetPngDataFromBitmap(hBitmap, size, hdc);
}

// 从窗口句柄里获取png图片数据, hdc传递0则调用失败, 返回的地址需要调用 delete[] 释放
inline LPBYTE GetPngDataFromHwnd(HWND hWnd, int* size = 0)
{
    if (size)*size = 0;
    HDC hdc = GetDC(hWnd);
    if (!hdc)return 0;
    LPBYTE ret = GetPngDataFromHDC(hdc, size);
    ReleaseDC(hWnd, hdc);
    return ret;
}


// ImgConvert 函数功能,把图片数据转换到指定格式的图片,返回转换后图片数据的指针,需要使用 delete[] p; 来释放
// data		= 需要转换的图片数据
// size		= 图片数据尺寸指针,请传递指针,成功后这个参数接收转换后图片数据的尺寸
// iFromat	= 转换到格式, 1=bmp, 2=jpg, 3=gif, 4=tiff, 其他=png
// szError	= 参考回去错误信息,可为NULL
inline LPBYTE ImgConvert(const LPBYTE data, INT_PTR* size, int iFromat, LPCWSTR* szError)
{
    if (!size || !data)return 0;    // 没提供尺寸或者图片数据都返回

    typedef HRESULT(WINAPI* fun_GdipCreateBitmapFromStream)(IStream* stream, HANDLE* p);
    typedef int     (WINAPI* fun_GdiplusStartup)(ULONG_PTR* uToken, void* input, void* output);
    typedef void    (WINAPI* fun_GdiplusShutdown)(ULONG_PTR uToken);
    typedef int     (WINAPI* fun_GdipSaveImageToStream)(HANDLE img, IStream* stream, LPCLSID clsid, int encoderParams);
    typedef int     (WINAPI* fun_GdipDisposeImage)(HANDLE img);


    static HMODULE hModule;
    static fun_GdipCreateBitmapFromStream pGdipCreateBitmapFromStream;
    static fun_GdiplusStartup pGdiplusStartup;
    static fun_GdiplusShutdown pGdiplusShutdown;
    static fun_GdipSaveImageToStream pGdipSaveImageToStream;;
    static fun_GdipDisposeImage pGdipDisposeImage;

    if (!hModule || !pGdipCreateBitmapFromStream || !pGdiplusStartup || !pGdiplusShutdown || !pGdipSaveImageToStream || !pGdipDisposeImage)
    {
        hModule = LoadLibraryW(L"gdiplus.dll");
        if (!hModule)return 0;
        pGdipCreateBitmapFromStream = (fun_GdipCreateBitmapFromStream)GetProcAddress(hModule, "GdipCreateBitmapFromStream");
        pGdiplusStartup = (fun_GdiplusStartup)GetProcAddress(hModule, "GdiplusStartup");
        pGdiplusShutdown = (fun_GdiplusShutdown)GetProcAddress(hModule, "GdiplusShutdown");
        pGdipSaveImageToStream = (fun_GdipSaveImageToStream)GetProcAddress(hModule, "GdipSaveImageToStream");
        pGdipDisposeImage = (fun_GdipDisposeImage)GetProcAddress(hModule, "GdipDisposeImage");
    }

    // 取几个函数地址,如果有其中一个取失败,则返回
    if (!pGdipCreateBitmapFromStream || !pGdiplusStartup || !pGdiplusShutdown || !pGdipSaveImageToStream || !pGdipDisposeImage)
    {
        if (szError)*szError = L"GdipCreateBitmapFromStream,GdiplusStartup,GdiplusShutdown,GdipSaveImageToStream,GdipDisposeImage\r\n其中一个获取失败";
        return 0;
    }


    LPBYTE newData = 0;
    HRESULT hr = 0;
    IStream* stream = 0, * DataStream = 0;
    HGLOBAL hMemDataStream = 0;
    HANDLE hBitmap = 0;
    void* pDataStream = 0;
    INT_PTR nSize = *size;
    ULONG_PTR uToken = 0;
    int input[4] = { 0 };
    input[0] = 1;
    hr = pGdiplusStartup(&uToken, &input, 0);        // 初始化gdi+

    if (hr != 0)
    {
        if (szError)*szError = L"gdi+初始化失败";
        return 0;
    }

    CLSID clsid = { 0 };
    switch (iFromat)    // 取图片转换的GUID
    {
    case 1:         // bmp
        hr = CLSIDFromString(L"{557CF400-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        break;
    case 2:         // jpg
        hr = CLSIDFromString(L"{557CF401-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        break;
    case 3:         // gif
        hr = CLSIDFromString(L"{557CF402-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        break;
    case 4:         //tiff
        hr = CLSIDFromString(L"{557CF405-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        break;
    default:        // png
        hr = CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        break;
    }
    if (hr != 0)
    {
        if (szError)*szError = L"获取图片GUID失败";
        return 0;    // 获取GUID失败就返回
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nSize);    //申请内存
    if (!hMem)
    {
        if (szError)*szError = L"内存申请失败";
        return 0;
    }

    void* pMem = GlobalLock(hMem);    //锁定内存,返回这个内存句柄的内存首地址
    while (pMem)
    {
        memcpy(pMem, data, nSize);          //把图片数据写入这个内存
        GlobalUnlock(hMem);                 //解锁内存

        hr = CreateStreamOnHGlobal(hMem, FALSE, &stream);               //创建流对象
        if (FAILED(hr) || !stream)break;

        hr = pGdipCreateBitmapFromStream(stream, &hBitmap);             //从流里创建位图
        if (FAILED(hr) || !hBitmap)break;


        hr = CreateStreamOnHGlobal(NULL, FALSE, &DataStream);           //创建流,从这个流里取转换后的数据
        if (FAILED(hr) || !DataStream)break;

        hr = pGdipSaveImageToStream(hBitmap, DataStream, &clsid, 0);    //转换图片
        if (FAILED(hr))break;

        hr = GetHGlobalFromStream(DataStream, &hMemDataStream);         //从流里获取内存句柄
        if (FAILED(hr))break;

        pDataStream = GlobalLock(hMemDataStream);                       //锁定内存句柄
        nSize = GlobalSize(hMemDataStream);                             //取转换后内存大小
        if (pDataStream)
        {
            newData = new BYTE[nSize];              //缓冲区
            *size = nSize;                          // 参考回去新的尺寸
            memset(newData, 0, nSize);
            memcpy(newData, pDataStream, nSize);    //把转换后的图片数据写入到缓冲区

        }
        GlobalUnlock(hMemDataStream);
        hr = pGdipDisposeImage(hBitmap);
    }


    GlobalFree(hMem);
    if (stream)stream->Release();
    if (DataStream)DataStream->Release();
    if (uToken)pGdiplusShutdown(uToken);

    return newData;

}
