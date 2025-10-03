#pragma once
#include <comdef.h>
// �ڴ�DC, λͼ
typedef struct MEMDCBITMAP
{
    HDC hdc;            // �ڴ�DC
    HBITMAP hBitmap;    // 32λ����λͼ
    int width;          // λͼ���, width * height = pBits ���ص�����, ���ص����� * 4 = pBits ���ֽ���
    int height;         // λͼ�߶�, width * height = pBits ���ص�����, ���ص����� * 4 = pBits ���ֽ���
    LPBYTE lpBits;      // ָ��λͼ����ָ��, ����ֱ���޸������ַ������ݴﵽ�滭��Ч��, 4���ֽ�Ϊһ�����ص�
}*PMEMDCBITMAP, * LPMEMDCBITMAP;


// ����32λ���豸�޹�λͼ
// hdc = �豸������
// width = λͼ���
// height = λͼ�߶�
// pBits = ����һ��ָ�����λͼ���ݵ�ַ
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
// ���·ֲ㴰��, ���ش�����, 0=�ɹ�
// hWnd = Ҫ�ػ��Ĵ���
// width = ���ڿ��
// height = ���ڸ߶�
// srcDC = ���ĸ�dc������������
// ppt = �����µ�λ��
// alpha = ����͸����
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

// �ͷ��ڴ�DC
// pmd = md_create() ���ص�ֵ
// isFree = �Ƿ��ͷ�pmd���ָ��, ���Ϊ��, �ڲ�ֻ�ͷ�hdc��hBitmap������, Ϊ�����pmdҲ�ͷ�
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

// ���µ����ڴ�dc�ߴ�, ����ߴ�һ�����޸�, ��������mdû�д���dc, ���ڲ��ᴴ��һ��
// pmd = ��Ҫ���µ����ߴ��md
// width = λͼ���
// height = λͼ�߶�
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

// �����ڴ�dc��32λλͼ
// width = λͼ���
// height = λͼ�߶�
// pmd = �������λ��, ������ֵ��Ϊ0, ���ڲ���������hdc��hBitmapд�����ָ��, ����ֵ�Ƿ������ָ��
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


// ��λͼ������ȡλͼͼƬ����, hBitmap ����0�����ʧ��, ���صĵ�ַ��Ҫ���� delete[] �ͷ�, �������λͼ������ߵ���0, �򷵻�0
// ��hBitmap���ȡλͼ����
// size = ����λͼ��С��ָ��
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
    if (bm.bmBits == 0) // �����λͼû������ָ��, ����һ���µ�dc����
    {
        md = md_create(bm.bmWidth, bm.bmHeight);
        BitBlt(md->hdc, 0, 0, bm.bmWidth, bm.bmHeight, hSrcDC, 0, 0, SRCCOPY); // �ڿ������µ�dc��
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

// ��HDC���ȡλͼͼƬ����, hdc����0�����ʧ��, ���صĵ�ַ��Ҫ���� delete[] �ͷ�
// �������HDC��λͼ������ߵ���0, �򷵻�0
inline LPBYTE GetBitmapDataFromHDC(HDC hdc, int* size = 0)
{
    if (size)*size = 0;
    if (!hdc)return 0;
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
    if (!hBitmap)return 0;
    return GetBitmapDataFromBitmap(hBitmap, size, hdc);
}

// �Ӵ��ھ�����ȡλͼͼƬ����, ���ھ������0�򷵻���������, ���صĵ�ַ��Ҫ���� delete[] �ͷ�
inline LPBYTE GetBitmapDataFromHwnd(HWND hWnd, int* size = 0)
{
    if (size)*size = 0;
    HDC hdc = GetDC(hWnd);
    if (!hdc)return 0;
    LPBYTE ret = GetBitmapDataFromHDC(hdc, size);
    ReleaseDC(hWnd, hdc);
    return ret;
}


// ��gdi+λͼ�ﱣ���pngͼƬ, gdi+δ��ʼ���򷵻�0
// gpBitmap = gdi+λͼ
// pSize = �ṩһ����ַ���շ������ݵĴ�С
// pClsid = ת���ĸ�ʽ, Ϊ0��ʹ��png��clsid
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

// ��λͼ������ȡpngͼƬ����, �������λͼ������ߵ���0, �򷵻�0
// hBitmap = Ҫ��ȡ��λͼ
// size = ����pngͼƬ��С��ָ��
// hSrcDC = Դdc, �Ǳ��� hBitmap��DC, ��ȡʧ��ʱ������dc��������
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

    // ȡ����������ַ,���������һ��ȡʧ��,�򷵻�
    if (!pGdipCreateBitmapFromStream || !pGdiplusStartup || !pGdiplusShutdown || !pGdipSaveImageToStream || !pGdipDisposeImage
        || !pGdipCreateBitmapFromScan0 || !pGdipBitmapLockBits || !pGdipBitmapUnlockBits)
    {
        //if (szError)*szError = L"GdipCreateBitmapFromStream,GdiplusStartup,GdiplusShutdown,GdipSaveImageToStream,GdipDisposeImage\r\n����һ����ȡʧ��";
        return 0;
    }

    ULONG_PTR uToken = 0;
    int input[4] = { 0 };
    input[0] = 1;
    int hr = pGdiplusStartup(&uToken, &input, 0);        // ��ʼ��gdi+
    if (hr != 0)
    {
        //if (szError)*szError = L"gdi+��ʼ��ʧ��";
        return 0;
    }


    LPMEMDCBITMAP md = 0;
    void* pBits = bm.bmBits;
    if (pBits == 0)     // �����λͼû������ָ��, ����һ���µ�dc����
    {
        md = md_create(bm.bmWidth, bm.bmHeight);
        BitBlt(md->hdc, 0, 0, bm.bmWidth, bm.bmHeight, hSrcDC, 0, 0, SRCCOPY); // �ڿ������µ�dc��
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


// ��HDC���ȡpngͼƬ����, ���ھ������0�򷵻���������, ���صĵ�ַ��Ҫ���� delete[] �ͷ�
inline LPBYTE GetPngDataFromHDC(HDC hdc, int* size = 0)
{
    if (size)*size = 0;
    if (!hdc)return 0;
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
    if (!hBitmap)return 0;
    return GetPngDataFromBitmap(hBitmap, size, hdc);
}

// �Ӵ��ھ�����ȡpngͼƬ����, hdc����0�����ʧ��, ���صĵ�ַ��Ҫ���� delete[] �ͷ�
inline LPBYTE GetPngDataFromHwnd(HWND hWnd, int* size = 0)
{
    if (size)*size = 0;
    HDC hdc = GetDC(hWnd);
    if (!hdc)return 0;
    LPBYTE ret = GetPngDataFromHDC(hdc, size);
    ReleaseDC(hWnd, hdc);
    return ret;
}


// ImgConvert ��������,��ͼƬ����ת����ָ����ʽ��ͼƬ,����ת����ͼƬ���ݵ�ָ��,��Ҫʹ�� delete[] p; ���ͷ�
// data		= ��Ҫת����ͼƬ����
// size		= ͼƬ���ݳߴ�ָ��,�봫��ָ��,�ɹ��������������ת����ͼƬ���ݵĳߴ�
// iFromat	= ת������ʽ, 1=bmp, 2=jpg, 3=gif, 4=tiff, ����=png
// szError	= �ο���ȥ������Ϣ,��ΪNULL
inline LPBYTE ImgConvert(const LPBYTE data, INT_PTR* size, int iFromat, LPCWSTR* szError)
{
    if (!size || !data)return 0;    // û�ṩ�ߴ����ͼƬ���ݶ�����

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

    // ȡ����������ַ,���������һ��ȡʧ��,�򷵻�
    if (!pGdipCreateBitmapFromStream || !pGdiplusStartup || !pGdiplusShutdown || !pGdipSaveImageToStream || !pGdipDisposeImage)
    {
        if (szError)*szError = L"GdipCreateBitmapFromStream,GdiplusStartup,GdiplusShutdown,GdipSaveImageToStream,GdipDisposeImage\r\n����һ����ȡʧ��";
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
    hr = pGdiplusStartup(&uToken, &input, 0);        // ��ʼ��gdi+

    if (hr != 0)
    {
        if (szError)*szError = L"gdi+��ʼ��ʧ��";
        return 0;
    }

    CLSID clsid = { 0 };
    switch (iFromat)    // ȡͼƬת����GUID
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
        if (szError)*szError = L"��ȡͼƬGUIDʧ��";
        return 0;    // ��ȡGUIDʧ�ܾͷ���
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nSize);    //�����ڴ�
    if (!hMem)
    {
        if (szError)*szError = L"�ڴ�����ʧ��";
        return 0;
    }

    void* pMem = GlobalLock(hMem);    //�����ڴ�,��������ڴ������ڴ��׵�ַ
    while (pMem)
    {
        memcpy(pMem, data, nSize);          //��ͼƬ����д������ڴ�
        GlobalUnlock(hMem);                 //�����ڴ�

        hr = CreateStreamOnHGlobal(hMem, FALSE, &stream);               //����������
        if (FAILED(hr) || !stream)break;

        hr = pGdipCreateBitmapFromStream(stream, &hBitmap);             //�����ﴴ��λͼ
        if (FAILED(hr) || !hBitmap)break;


        hr = CreateStreamOnHGlobal(NULL, FALSE, &DataStream);           //������,���������ȡת���������
        if (FAILED(hr) || !DataStream)break;

        hr = pGdipSaveImageToStream(hBitmap, DataStream, &clsid, 0);    //ת��ͼƬ
        if (FAILED(hr))break;

        hr = GetHGlobalFromStream(DataStream, &hMemDataStream);         //�������ȡ�ڴ���
        if (FAILED(hr))break;

        pDataStream = GlobalLock(hMemDataStream);                       //�����ڴ���
        nSize = GlobalSize(hMemDataStream);                             //ȡת�����ڴ��С
        if (pDataStream)
        {
            newData = new BYTE[nSize];              //������
            *size = nSize;                          // �ο���ȥ�µĳߴ�
            memset(newData, 0, nSize);
            memcpy(newData, pDataStream, nSize);    //��ת�����ͼƬ����д�뵽������

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
