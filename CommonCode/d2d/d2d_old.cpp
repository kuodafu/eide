#include "d2d_old.h"

#ifndef __USEXP
#include <d3d11.h>
#pragma comment(lib, "D3D11.lib")
#endif

//TODO D2D对象应该加入引用计数功能, 为0的时候释放资源

// AlphaBlend 函数使用
#pragma comment(lib, "Msimg32.lib")

#define PNG_HEADER 1196314761
#define PNG_IHDR 1380206665
#define PNG_PLTE 1163152464
#define PNG_tRNS 1397641844
#define PNG_acTL 1280598881
#define PNG_fcTL 1280598886
#define PNG_IDAT 1413563465
#define PNG_fdAT 1413571686

#define IMGF_APNG 0x0001

struct __EX_IMAGE;
struct __EX_RGN;
struct __EX_FONT;
struct __EX_PATH;
struct __EX_BRUSH;
struct __EX_PEN;
struct __CANVAS;

bool _dx_init(bool isDebug);
void _dx_uninit();

struct D2D_GDI_DATA_STRUCT
{
#ifndef __USEXP
    ID2D1Factory1* pFactory;                    // D2D工厂接口, 这个接口是所有D2D程序的起始点, 几乎所有的D2D资源都是由这个接口创建的
    IDWriteFactory* pDWriteFactory;             // 文字工厂接口
    IWICImagingFactory2* pWICFactory;           // WIC图像工厂接口
    ID2D1Device* pD2DDevice;                    // d2d设备
    ID2D1DeviceContext* pD2DDeviceContext;      // D2D设备上下文
    ID2D1GdiInteropRenderTarget* pGDIInterop;   // gdi兼容
    D2D1_BITMAP_PROPERTIES1 bp_proper;          // 创建位图属性
#endif
    DWORD flags;                                // 渲染标志
    int lastErrCode;                            // 最后错误
    ULONG_PTR uToken;                           // gdi+Token
    Gdiplus::Matrix* pMatrix;                   // gdi+矩阵
    float DpiX;
    float DpiY;

    ~D2D_GDI_DATA_STRUCT()
    {
        _dx_uninit();
    }
};

static D2D_GDI_DATA_STRUCT d2dInfo;



void __set_isdx(bool isdx)
{
    if (isdx)
        d2dInfo.flags |= EX_RENDER_D2D;
    else
        d2dInfo.flags &= ~EX_RENDER_D2D;
    d2dInfo.flags |= EX_RENDER_GDI;
}

// 判断错误码是否出错, 错误返回true, 并且会设置最后错误
DLLAPIBOOL ISFAILED(HRESULT hr)
{
    if (FAILED(hr))
    {
        ExSetLastError(hr);
        return true;
    }
    return false;
}

DLLAPIBOOL IsDxRender()
{
    return (d2dInfo.flags & EX_RENDER_D2D) != 0;
}
DLLAPIBOOL Flag_Query(DWORD dwFlag)
{
    return (d2dInfo.flags & dwFlag) != 0;
}
DLLAPIBOOL IsGpDraw()
{
    return (d2dInfo.flags & EX_RENDER_GDI) != 0;
}
DLLAPIBOOL IsD2DGdi()
{
    return (d2dInfo.flags & EXGF_RENDER_METHOD_D2D_GDI_COMPATIBLE) != 0;
}

template<typename T>inline void SafeRelease(T*& pObj)
{
    if (pObj)
        pObj->Release();
    pObj = NULL;
}


DLLAPITYPE(int) D2D_APICALL ExSetLastError(int code)
{
    d2dInfo.lastErrCode = code;
    return code;
}
DLLAPITYPE(int) D2D_APICALL ExGetLastError()
{
    return d2dInfo.lastErrCode;
}
DLLAPIBOOL _canvas_isDxRender()
{
    return (d2dInfo.flags & EX_RENDER_D2D) != 0;
}
DLLAPITYPE(int) _canvas_GetDxRender()
{
    return d2dInfo.flags;
}

DLLAPIBOOL recreate_Device()
{
    LRESULT hr = 0;
#ifndef __USEXP
    SafeRelease(d2dInfo.pGDIInterop);
    SafeRelease(d2dInfo.pD2DDeviceContext);
    while (true)
    {
        hr = d2dInfo.pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dInfo.pD2DDeviceContext);
        if (FAILED(hr)) break;

        d2dInfo.pD2DDeviceContext->QueryInterface(IID_PPV_ARGS(&d2dInfo.pGDIInterop));
        d2dInfo.pD2DDeviceContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
        break;
    }
#endif
    return SUCCEEDED(hr);
}

DLLAPIBOOL _canvas_init(bool isDxRender, bool isDebug)
{
    if (d2dInfo.pMatrix)return true;
#ifdef __USEXP
    isDxRender = false;
#endif
    d2dInfo.flags = isDxRender ? EX_RENDER_D2D : EX_RENDER_GDI;
    HRESULT hr = 0;
#ifndef __USEXP
    if (isDxRender)
    {
        hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            hr = CoCreateInstance(CLSID_WICImagingFactory2,
                NULL, CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&d2dInfo.pWICFactory));
            ExSetLastError(hr);
            if (SUCCEEDED(hr))
            {
                if (!_dx_init(isDebug))
                {
                    _dx_uninit();
                    __set_isdx(false);
                }
            }
        }
        if (hr != S_OK)
            d2dInfo.flags = EX_RENDER_GDI;

    }
#endif

    Gdiplus::GdiplusStartupInput input = { 0 };
    input.GdiplusVersion = 1;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&d2dInfo.uToken, &input, 0);
    ExSetLastError(status);
    Assert(status == Gdiplus::Status::Ok);
    if (status != Gdiplus::Status::Ok) return false;

    d2dInfo.pMatrix = new Gdiplus::Matrix;

    return true;
}
bool _dx_init(bool isDebug)
{
#ifndef __USEXP
    comptr<ID3D11Device> pD3DDevice;    // D3D设备
    comptr<IDXGIDevice1> pDxgiDevice;    // dxgi设备
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
    };

    HRESULT hr = 0;
    D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
//#ifdef _DEBUG
//    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
//#else
//    options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
//#endif
    if(isDebug)
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    while (true)
    {
        // 1. 创建D3D11 设备, 也可以创建D3D10, 不过D3D11更好, 创建flag 一定要有 D3D11_CREATE_DEVICE_BGRA_SUPPORT
        hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags,
            featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]),
            D3D11_SDK_VERSION, &pD3DDevice, 0, 0);
        if (FAILED(hr)) break;

        // 2. 获取 IDXGIDevice
        hr = pD3DDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
        if (FAILED(hr)) break;

        // 3. 创建 D2D 工厂
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<void**>(&d2dInfo.pFactory));
        if (FAILED(hr)) break;

        // 4. 创建D2D设备
        hr = d2dInfo.pFactory->CreateDevice(pDxgiDevice, &d2dInfo.pD2DDevice);
        if (FAILED(hr)) break;

        // 5. 创建D2D设备上下文
        hr = d2dInfo.pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dInfo.pD2DDeviceContext);
        if (FAILED(hr)) break;

        d2dInfo.pD2DDeviceContext->QueryInterface(IID_PPV_ARGS(&d2dInfo.pGDIInterop));
        d2dInfo.pD2DDeviceContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
        //d2dInfo.pFactory->GetDesktopDpi(&d2dInfo.DpiX, &d2dInfo.DpiY);
        d2dInfo.pD2DDeviceContext->GetDpi(&d2dInfo.DpiX, &d2dInfo.DpiY);

        d2dInfo.bp_proper.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        d2dInfo.bp_proper.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d2dInfo.bp_proper.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE;
        d2dInfo.bp_proper.dpiX = 96;
        d2dInfo.bp_proper.dpiY = 96;
        d2dInfo.bp_proper.colorContext = 0;

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&d2dInfo.pDWriteFactory));
        if (FAILED(hr)) break;

        return true;
    }
    
    ExSetLastError(hr);
#endif
    return false;
}

void _dx_uninit()
{
#ifndef __USEXP
    SafeRelease(d2dInfo.pGDIInterop);
    SafeRelease(d2dInfo.pD2DDeviceContext);
    SafeRelease(d2dInfo.pD2DDevice);
    SafeRelease(d2dInfo.pDWriteFactory);
    SafeRelease(d2dInfo.pFactory);
    SafeRelease(d2dInfo.pWICFactory);
#endif
}











































struct __HIMAGE_GP_DX
{
    union
    {
        IWICBitmap* dxImg;
        Gdiplus::Bitmap* gpImg;
    };
    __HIMAGE_GP_DX(IWICBitmap* img) { dxImg = img; }
    __HIMAGE_GP_DX(Gdiplus::Bitmap* img) { gpImg = img; }
};


inline static Gdiplus::GraphicsPath* __createRoundedrect(float left, float top, float width, float height, float radiusX, float radiusY, Gdiplus::FillMode fillMode = Gdiplus::FillMode::FillModeAlternate)    // 创建圆角矩形
{
    float offsetX = 0, offsetY = 0;
    Gdiplus::GraphicsPath* path = new Gdiplus::GraphicsPath(fillMode);
    offsetX = 2 * radiusX;
    offsetY = 2 * radiusY;
    width = width + left;
    height = height + top;

    path->AddArc(width - offsetX, top, offsetX, offsetY, 270, 90);
    path->AddArc(width - offsetX, height - offsetY, offsetX, offsetY, 0, 90);
    path->AddArc(left, height - offsetY, offsetX, offsetY, 90, 90);
    path->AddArc(left, top, offsetX, offsetY, 180, 90);
    path->AddLine(left + offsetX, top, width - offsetX / 2, top);

    return path;
}


struct __EX_IMAGE : public EX_IMAGE
{
    UINT curframe;                      // _img_nCurFrame
    UINT maxFrames;                     // _img_nMaxFrames
    UINT width;
    UINT height;
    DWORD flags;
    void* lpHeader;
    void* lpFrames;

#ifndef __USEXP // 不是xp, 使用D2D
    IWICBitmapDecoder* pDecoder;        // _img_pWicDecoder wic位图解码器
    ID2D1Bitmap1* d2dBitmap;            // 创建一次, 销毁时需要判断是否为空然后销毁
    union
    {
        Gdiplus::Bitmap* gpImg;
        IWICBitmap* dxImg;
    };
#else

    Gdiplus::Bitmap* gpImg;

#endif

public:
    __EX_IMAGE() :EX_IMAGE(),
        curframe(0),
        maxFrames(0),
        width(0),
        height(0),
        flags(0),
        lpHeader(0),
        lpFrames(0),
        gpImg(0)
        

    {
#ifndef __USEXP // 不是xp, 使用D2D
        pDecoder = 0;
        d2dBitmap = 0;
#endif
    }

    virtual int Release()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            SafeRelease(dxImg);
            SafeRelease(pDecoder);
            SafeRelease(d2dBitmap);
        }
        else
#endif
        {
            delete gpImg;
        }
        delete this;
        return true;
    }

public:
    virtual UINT GetWidth()
    {
        GetSize(&width, &height);
        return width;
    }
    virtual UINT GetHeight()
    {
        GetSize(&width, &height);
        return height;
    }
    virtual DWORD GetSize(UINT* cx = 0, UINT* cy = 0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            HRESULT hr = dxImg->GetSize(&width, &height);
            ExSetLastError(hr);
            if (FAILED(hr))return 0;
            if (cx)*cx = width;
            if (cy)*cy = height;
            return MAKELONG(width, height);
        }
#endif
        width = gpImg->GetWidth();
        height = gpImg->GetHeight();
        if (cx)*cx = width;
        if (cy)*cy = height;
        return MAKELONG(width, height);
    }

    // 从当前图片拷贝一份新的图片出来, 返回新的图片句柄
    virtual LPEX_IMAGE copy(UINT x = 0, UINT y = 0, UINT width = 0, UINT height = 0)
    {
        GetSize();
        if (width == 0)
            width = this->width;
        if (height == 0)
            height = this->height;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            IWICBitmap* pBitmap;
            HRESULT hr = d2dInfo.pWICFactory->CreateBitmapFromSourceRect(dxImg, x, y, width, height, &pBitmap);
            if (FAILED(hr))return 0;
            return _img_init(pBitmap, 0, 1, 0);
        }
#endif
        Gdiplus::Bitmap* img = gpImg->Clone((INT)x, y, width, height, 0);
        return _img_init(img, 0, 1, 0);
    }

    // 锁定图片数据
    // flags = 1=读, 2=写, 3=读写
    virtual bool lock(UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData)
    {
        if (!lockData) return false;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            UINT imgWidth = 0, imgHeight = 0;
            IWICBitmapLock* pLock = 0;
            while (1)
            {
                if (GetSize() == 0)
                    break;

                if (width == 0)width = this->width;
                if (height == 0)height = this->height;

                WICRect rc = { (INT)x, (INT)y, (INT)width, (INT)height };
                LRESULT hr = dxImg->Lock(&rc, flags, &pLock);
                if (FAILED(hr)) break;

                UINT stride = 0;
                hr = pLock->GetStride(&stride);
                if (FAILED(hr)) break;

                UINT dwLen = 0;
                WICInProcPointer pbData;
                hr = pLock->GetDataPointer(&dwLen, &pbData);

                if (FAILED(hr))break;

                lockData->width = width;
                lockData->height = height;
                lockData->pLock = pLock;
                lockData->pScan0 = pbData;
                lockData->stride = stride;
                return true;
            }
            SafeRelease(pLock);
            return 0;
        }
#endif
        if (GetSize() == 0)
            return 0;

        if (width == 0)width = this->width;
        if (height == 0)height = this->height;

        Gdiplus::Rect rc(x, y, width, height);

        const int gpPixelFromat32bppARGB = (10 | (32 << 8) | 0x00040000 | 0x00020000 | 0x00200000);
        Gdiplus::BitmapData data;
        Gdiplus::Status status = gpImg->LockBits(&rc, flags, gpPixelFromat32bppARGB, &data);
        if (status != Gdiplus::Status::Ok) return false;

        lockData->width = width;
        lockData->height = height;
        lockData->pLock = (IWICBitmapLock*)(LONG_PTR)data.PixelFormat;
        lockData->pScan0 = data.Scan0;
        lockData->stride = data.Stride;
        return true;
    }

    // 取消锁定图片
    virtual bool unlock(EX_IMAGELOCK* lockData)
    {
        if (!lockData) return false;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            SafeRelease(lockData->pLock);
            _wic_drawframe(this, dxImg);
            return true;
        }
#endif
        Gdiplus::BitmapData data;
        data.Width = lockData->width;
        data.Height = lockData->height;
        data.Scan0 = lockData->pScan0;
        data.Stride = lockData->stride;
        data.PixelFormat = (int)(LONG_PTR)lockData->pLock;
        data.Reserved = 0;

        return gpImg->UnlockBits(&data) == Gdiplus::Status::Ok;
    }

    // 取图像帧数
    virtual UINT GetFrameCount()
    {
        return maxFrames;
    }

    // 设置当前活动帧
    virtual bool SelectActiveFrame(UINT index)
    {
        if (maxFrames <= 1)return false;
        // 如果 (__query (pImg, #_img_dwFlags, #IMGF_APNG))
        // _apng_drawframe (pImg, nIndex)
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            comptr<IWICBitmap> pFrame = _wic_selectactiveframe(pDecoder, index);
            if (pFrame)
            {
                _wic_drawframe(this, pFrame);
                curframe = index;
                return true;
            }
            return false;
        }
#endif
        Gdiplus::Status status = gpImg->SelectActiveFrame(&Gdiplus::ImageFormatGIF, index);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }

    // 把当前图片对象保存成图片数据, 返回的图片数据不使用时需要调用本对象的 free 来释放
    // pSize = 接收返回图片数据的大小
    // 左顶右底, 要保存的位置, 默认为图片对象大小
    virtual LPBYTE save(SIZE_T* pSize, UINT left = 0, UINT top = 0, UINT right = 0, UINT bottom = 0)
    {
        return _img_save_rect(this, pSize, left, top, right, bottom);
    }
    virtual LPBYTE save(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left = 0, UINT top = 0, UINT right = 0, UINT bottom = 0)
    {
        return _img_save_formcanvasex(hCanvas, pSize, left, top, right, bottom);
    }

    virtual HBITMAP ToGdiBitmap()
    {
        if (!gpImg)
            return 0;

        if (IsDxRender())
        {
            //SIZE_T size;
            //LPBYTE bmp = _img_save(image, size);

            return 0;
        }
        HBITMAP hBmp = 0;
        gpImg->GetHBITMAP(0, &hBmp);
        return hBmp;
    }


    // 释放 save 返回的地址
    virtual void free(LPBYTE p)
    {
        _img_free(p);
    }


#ifndef __USEXP // 不是xp, 使用D2D
    virtual ID2D1Bitmap1* GetDxBitmap(HRESULT* phr = 0)
    {
        if (!d2dBitmap)
        {
            HRESULT hr = d2dInfo.pD2DDeviceContext->CreateBitmapFromWicBitmap(dxImg, &d2dInfo.bp_proper, &d2dBitmap);
            if (phr)*phr = hr;
            if (FAILED(hr)) return 0;
        }
        return d2dBitmap;
    }
#endif

    virtual Gdiplus::Bitmap* GetGdipBitmap()
    {
        return gpImg;
    }

public:

    static __EX_IMAGE* _img_init(const __HIMAGE_GP_DX& pObj, UINT curframe, UINT frames, IWICBitmapDecoder* pDecoder)
    {
        __EX_IMAGE* pImg = new __EX_IMAGE;
        pImg->gpImg = pObj.gpImg;
        pImg->curframe = curframe;

#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pImg->pDecoder = pDecoder;
            _wic_drawframe(pImg, pObj.dxImg);
        }
        else
#endif
        {
            UINT count = pObj.gpImg->GetFrameCount(&Gdiplus::ImageFormatGIF);
            count = count ? count : 1;
        }
        pImg->maxFrames = frames;
        Assert(ExGetLastError() == 0);
        //{
        //    delete pImg;
        //    return 0;
        //}
        if (pImg && (pImg->width == 0 || pImg->height == 0))
        {
            _img_getsize(pImg, &pImg->width, &pImg->height);
        }
        return pImg;
    }


#ifndef __USEXP // 不是xp, 使用D2D
    static ID2D1Bitmap1* _dx_createbitmap(ID2D1DeviceContext* pRt, UINT width, UINT height, bool fGdi)
    {
        if (fGdi)
            d2dInfo.bp_proper.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE;
        else
            d2dInfo.bp_proper.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
        ID2D1Bitmap1* pBitmap;
        D2D1_SIZE_U size = { width, height };
        HRESULT hr = pRt->CreateBitmap(size, 0, 0, &d2dInfo.bp_proper, &pBitmap);
        ExSetLastError(hr);
        return pBitmap;
    }

    static IWICBitmap* _wic_convert(IWICBitmapFrameDecode*& pBitmap, bool bFreeOld)
    {
        IWICBitmap* pBitmapConvert = (IWICBitmap*)pBitmap;
        comptr<IWICFormatConverter> pConverter;    // 位图格式转换
                                                   // 创建位图格式转换
        HRESULT hr = d2dInfo.pWICFactory->CreateFormatConverter(&pConverter);
        Assert(SUCCEEDED(hr));
        if (SUCCEEDED(hr))
        {
            hr = pConverter->Initialize(pBitmap, GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherType::WICBitmapDitherTypeNone, 0, 0.0,
                WICBitmapPaletteType::WICBitmapPaletteTypeCustom);
            if (SUCCEEDED(hr))    // 
            {
                // 创建位图自源
                d2dInfo.pWICFactory->CreateBitmapFromSource(pConverter,
                    WICBitmapCreateCacheOption::WICBitmapCacheOnDemand, &pBitmapConvert);
                if (SUCCEEDED(hr) && bFreeOld)
                {
                    SafeRelease(pBitmap);
                }
            }
        }
        ExSetLastError(hr);
        return pBitmapConvert;
    }
    static IWICBitmap* _wic_selectactiveframe(IWICBitmapDecoder* pDecoder, UINT index)
    {
        IWICBitmapFrameDecode* pFrame = 0;
        HRESULT hr = pDecoder->GetFrame(index, &pFrame);
        IWICBitmap* ret = 0;
        if (SUCCEEDED(hr))
            ret = _wic_convert(pFrame, true);
        ExSetLastError(hr);
        return ret;
    }
    static LPEX_IMAGE _wic_create(DWORD width, DWORD height, REFWICPixelFormatGUID pFormat)
    {

        IWICBitmap* pBitmap;
        HRESULT hr = d2dInfo.pWICFactory->CreateBitmap(
            width, height, pFormat,
            WICBitmapCreateCacheOption::WICBitmapCacheOnDemand,
            &pBitmap
        );
        if (FAILED(hr))return 0;
        return _img_init(pBitmap, 0, 1, 0);
    }
    static __EX_IMAGE* _wic_init_from_decoder(IWICBitmapDecoder* pDecoder)
    {
        __EX_IMAGE* hImg = 0;
        UINT pCount = 0;
        HRESULT hr = pDecoder->GetFrameCount(&pCount);
        if (SUCCEEDED(hr))
        {
            if (SUCCEEDED(hr))
            {
                IWICBitmap* pFrame = _wic_selectactiveframe(pDecoder, 0);
                if (pFrame)
                {
                    hImg = _img_init(pFrame, 0, pCount, pDecoder);
                }
            }
        }
        Assert(SUCCEEDED(hr));
        ExSetLastError(hr);
        return hImg;
    }

    static void _wic_drawframe(__EX_IMAGE* pImage, IWICBitmap* pFrame)
    {
        if (pImage->maxFrames <= 1)return;
        D2D1_RENDER_TARGET_PROPERTIES rtb;
        memset(&rtb, 0, sizeof(rtb));
        comptr<ID2D1RenderTarget> rt;
        comptr<ID2D1Bitmap> pBitmap;

        HRESULT hr = d2dInfo.pFactory->CreateWicBitmapRenderTarget(pImage->dxImg, rtb, &rt);
        Assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
        {
            rt->BeginDraw();
            hr = rt->CreateBitmapFromWicBitmap(pFrame, &pBitmap);
            if (SUCCEEDED(hr) && pBitmap)
            {
                D2D1_RECT_F rc = { 0,1,1,0 };
                rt->DrawBitmap(pBitmap, rc);
            }
            hr = rt->EndDraw();
            Assert(SUCCEEDED(hr));
        }
        ExSetLastError(hr);
    }
    static LPBYTE _wic_savetobin(const __HIMAGE_GP_DX& pObj, SIZE_T& size)
    {
        IStream* stream = 0;
        IWICStream* pIWICStream = 0;
        IWICBitmapEncoder* pEncoder = 0;
        IWICBitmapFrameEncode* pFrame = 0;
        UINT width = 0, height = 0;
        HGLOBAL hMem = 0;
        void* pData = 0;
        LPBYTE ret = 0;
        size = 0;
        int err = 0;
        HRESULT hr = CreateStreamOnHGlobal(0, 0, &stream);
        //WICPixelFormatGUID guid = GUID_WICPixelFormatDontCare;
        WICPixelFormatGUID guid = GUID_WICPixelFormat32bppBGRA;
        //WICPixelFormatGUID guid = GUID_WICPixelFormat32bppRGBA;
        if (FAILED(hr)) goto exit;

        hr = pObj.dxImg->GetSize(&width, &height);
        if (FAILED(hr))goto exit;

        hr = d2dInfo.pWICFactory->CreateStream(&pIWICStream);
        if (FAILED(hr))goto exit;

        hr = pIWICStream->InitializeFromIStream(stream);
        if (FAILED(hr))goto exit;

        hr = d2dInfo.pWICFactory->CreateEncoder(GUID_ContainerFormatPng, 0, &pEncoder);
        if (FAILED(hr))goto exit;

        hr = pEncoder->Initialize(stream, WICBitmapEncoderCacheOption::WICBitmapEncoderNoCache);
        if (FAILED(hr))goto exit;

        hr = pEncoder->CreateNewFrame(&pFrame, 0);
        if (FAILED(hr))goto exit;

        hr = pFrame->Initialize(0);
        if (FAILED(hr))goto exit;

        hr = pFrame->SetSize(width, height);
        if (FAILED(hr))goto exit;

        hr = pFrame->SetPixelFormat(&guid);
        if (FAILED(hr))goto exit;

        hr = pFrame->WriteSource(pObj.dxImg, 0);
        if (FAILED(hr))goto exit;

        hr = pFrame->Commit();
        if (FAILED(hr))goto exit;

        hr = pEncoder->Commit();
        if (FAILED(hr))goto exit;

        hr = GetHGlobalFromStream(stream, &hMem);
        if (FAILED(hr))goto exit;

        pData = GlobalLock(hMem);
        err = GetLastError();

        if (!pData)goto exit;

        size = GlobalSize(hMem);
        ret = (LPBYTE)malloc(size);
        if (ret)memcpy(ret, pData, size);
        GlobalUnlock(hMem);

    exit:
        if (hMem)GlobalFree(hMem);
        if (pFrame)SafeRelease(pFrame);
        if (pEncoder)SafeRelease(pEncoder);
        if (pIWICStream)SafeRelease(pIWICStream);
        if (stream)SafeRelease(stream);
        return ret;
    }



    static void _dx_drawbitmaprectrect(ID2D1DeviceContext* pRt, ID2D1Bitmap1* pBitmap,
        float dstLeft, float dstTop, float dstRight, float dstBottom,
        float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE Alpha)
    {
        D2D_RECT_F rc1{ dstLeft, dstTop, dstRight, dstBottom };
        D2D_RECT_F rc2{ srcLeft, srcTop, srcRight, srcBottom };
        pRt->DrawBitmap(pBitmap,
            &rc1,
            static_cast<float>(Alpha) / 255.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            &rc2);

    }

    static bool _dx_bmp_copyfrom(ID2D1Bitmap1* pDst, ID2D1Bitmap1* pSrc,
        int dx, int dy, int sx, int sy, int sright, int sbottom)
    {
        D2D1_POINT_2U pt = { (UINT32)dx, (UINT32)dy };
        D2D1_RECT_U rc = { (UINT32)sx, (UINT32)sy, (UINT32)sright, (UINT32)sbottom };
        HRESULT hr = pDst->CopyFromBitmap(&pt, pSrc, &rc);
        ExSetLastError(hr);
        return SUCCEEDED(hr);
    }
#endif

    static inline IStream* _img_createfromstream_init(const void* pData, DWORD len)
    {
        if (!len || !pData) return 0;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
        if (hMem)
        {
            IStream* stream;
            HRESULT hr = CreateStreamOnHGlobal(hMem, TRUE, &stream);
            if (SUCCEEDED(hr))
            {
                void* p = GlobalLock(hMem);
                if (p)
                {
                    memcpy(p, pData, len);
                    GlobalUnlock(hMem);
                    return stream;
                }
                ExSetLastError(GetLastError());
            }
            GlobalFree(hMem);
        }
        else
        {
            ExSetLastError(GetLastError());
        }
        // TODO 最后错误, 错误的长度
        return 0;
    }
    static inline bool _HasAlpha(const DIBSECTION& ds)
    {
        if (ds.dsBmih.biBitCount != 32)return false;
        LONG pixels = ds.dsBmih.biWidth * abs(ds.dsBmih.biHeight);
        LPBYTE p = (LPBYTE)ds.dsBm.bmBits;
        LPBYTE pEnd = p + pixels;
        for (LONG i = 0; i < pixels; i += 4)
        {
            if (p[i + 3])
                return true;
        }
        return false;
    }
    static inline Gdiplus::Bitmap* _gp_img_createfromhbitmap(HBITMAP hBitmap, HPALETTE hPalette, int fmt)
    {
        Gdiplus::Bitmap bitmap(hBitmap, hPalette);
        int format = bitmap.GetPixelFormat();
        Gdiplus::Rect rc;
        rc.Width = bitmap.GetWidth();
        rc.Height = bitmap.GetHeight();
        Gdiplus::BitmapData bmData;
        Gdiplus::Status status = bitmap.LockBits(&rc, 1, format, &bmData);
        if (status != Gdiplus::Status::Ok)return 0;

        Gdiplus::Bitmap bitmapFix(rc.Width, rc.Height, bmData.Stride, fmt, (BYTE*)bmData.Scan0);
        bitmap.UnlockBits(&bmData);

        Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(rc.Width, rc.Height);
        Gdiplus::Graphics gp(pBitmap);
        gp.DrawImage(&bitmapFix, rc, 0, 0, rc.Width, rc.Height, Gdiplus::UnitPixel);
        return pBitmap;
    }
    static inline HBITMAP _DDBToDIB(HBITMAP hBmp)
    {
        BITMAP bm = { 0 };
        if (GetObjectW(hBmp, sizeof(bm), &bm) != sizeof(bm))
            return 0;
        if (bm.bmBits)return hBmp;
        HDC hdc = CreateCompatibleDC(0);
        BITMAPINFO bi = { 0 };
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = bm.bmWidth;
        bi.bmiHeader.biHeight = bm.bmHeight;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = bm.bmBitsPixel;
        bi.bmiHeader.biCompression = 0;
        if (bi.bmiHeader.biBitCount < 24)
            bi.bmiHeader.biBitCount = 24;
        void* bits;
        HBITMAP dib = CreateDIBSection(hdc, &bi, 0, &bits, 0, 0);
        if (dib)
        {
            GetDIBits(hdc, hBmp, 0, abs(bi.bmiHeader.biHeight), bits, &bi, 0);
        }
        DeleteDC(hdc);
        return dib;
    }
    static inline Gdiplus::Bitmap* _gp_img_createfromhicon(HICON hIcon)
    {
        return new Gdiplus::Bitmap(hIcon);
        ICONINFO ii = { 0 };
        if (!GetIconInfo(hIcon, &ii))
            return 0;    // 最后错误, 未支持的类型/格式

        HBITMAP dib = _DDBToDIB(ii.hbmColor);
        DIBSECTION ds = { 0 };
        sizeof(ds);
        GetObjectW(dib, sizeof(ds), &ds);
        Gdiplus::Bitmap* sourceBitmap;
        if (_HasAlpha(ds))
        {
            sourceBitmap = new Gdiplus::Bitmap(ds.dsBm.bmWidth, ds.dsBm.bmHeight, ds.dsBm.bmWidthBytes, PixelFormat32bppARGB, (BYTE*)ds.dsBm.bmBits);
        }
        else
        {
            sourceBitmap = new Gdiplus::Bitmap(hIcon);
        }
        DeleteObject(ii.hbmColor);
        DeleteObject(ii.hbmMask);

        Gdiplus::Bitmap* hImg = new Gdiplus::Bitmap(ds.dsBm.bmWidth, ds.dsBm.bmHeight);
        Gdiplus::Graphics gp(hImg);
        Gdiplus::Rect rc;
        rc.X = 0;
        rc.Y = 0;
        rc.Width = ds.dsBm.bmWidth;
        rc.Height = ds.dsBm.bmHeight;
        gp.DrawImage(sourceBitmap, rc, 0, 0, rc.Width, rc.Height, Gdiplus::UnitPixel);
        delete sourceBitmap;
        DeleteObject(dib);
        return hImg;
    }
    static inline int bswap(int i)
    {
        BYTE* p = (BYTE*)&i;
        BYTE t = p[0];
        BYTE t1 = p[1];
        p[0] = p[3];
        p[1] = p[2];
        p[2] = t1;
        p[3] = t;
        return *((int*)p);
    }
    static inline int _apng_thunk_getlength(void* p)
    {
        return bswap(((int*)p)[0]);
    }
    // 完全不知道是在干嘛......
    static inline bool _apng_thunk_getnext(void* lpMem, int& nPos, int dwThunkType, void*& lpThunk, int& dwThunkLen)
    {
        int i = nPos;
        bool ret = false;
        LPBYTE p = (LPBYTE)lpMem;
        int dwThunkDataLen = 0;
        do
        {
            dwThunkDataLen = _apng_thunk_getlength(p + i);
            int tmp = *((int*)(p + i + 4));
            if (tmp == dwThunkType)
                ret = true;

            i = i + dwThunkDataLen + 12;
            if (ret)
            {
                lpThunk = p + i - dwThunkDataLen - 12;
                dwThunkLen = dwThunkDataLen + 12;
                nPos = i;
                break;
            }
        } while (dwThunkDataLen);
        return ret;
    }
    // 完全不知道是在干嘛......
    static inline void _apng_int(LPEX_IMAGE hImage, IStream* lpStream)
    {
        //HGLOBAL hGlobal;
        //HRESULT hr = GetHGlobalFromStream(lpStream, &hGlobal);
        //if (FAILED(hr))return;
        //void* lpMem = GlobalLock(hGlobal);
        //if (lpMem)
        //{
        //    if (*((int*)lpMem) == PNG_HEADER)
        //    {
        //        int nPos = 8;
        //        void* pLHDR = 0, * pPLTE = 0, * pTRNS = 0, * pThunk = 0;
        //        int dwLHDR = 0, dwPLTE = 0, dwTRNS = 0;
        //        int dwLen = 0;
        //        if (_apng_thunk_getnext(lpMem, nPos, PNG_IHDR, pLHDR, dwLHDR))
        //        {
        //            _apng_thunk_getnext(lpMem, nPos, PNG_PLTE, pPLTE, dwPLTE);
        //            _apng_thunk_getnext(lpMem, nPos, PNG_tRNS, pTRNS, dwTRNS);
        //            dwLen = 4 + 8 + dwLHDR + dwPLTE + dwTRNS;
        //            int* pHeader = (int*)new BYTE[dwLen];
        //            memset(pHeader, 0, dwLen);
        //            pHeader[0] = dwLen - 4;
        //            pHeader[1] = PNG_HEADER;
        //            pHeader[2] = 169478669;
        //            memcpy(((LPBYTE)pHeader) + 12, pLHDR, dwLHDR);
        //            dwLen = dwLHDR + 12;
        //            if (pPLTE)
        //            {
        //                memcpy(((LPBYTE)pHeader) + dwLen, pPLTE, dwPLTE);
        //                dwLen += dwPLTE;
        //            }
        //            if (pTRNS)
        //            {
        //                memcpy(((LPBYTE)pHeader) + dwLen, pTRNS, dwTRNS);
        //            }
        //            nPos = 8;
        //            if (_apng_thunk_getnext(lpMem, nPos, PNG_acTL, pThunk, dwLen))
        //            {
        //                UINT nCount = _apng_thunk_getlength(LPBYTE(pThunk) + 8);
        //                LPHANDLE pFrames = new HANDLE[nCount];
        //                memset(pFrames, 0, sizeof(HANDLE) * nCount);
        //
        //                hImage->flags |= IMGF_APNG;
        //                hImage->maxFrames = nCount;
        //                hImage->lpFrames = pFrames;
        //                hImage->lpHeader = pHeader;
        //                int i = 0;
        //                while (_apng_thunk_getnext(lpMem, nPos, PNG_fcTL, pThunk, dwLen))
        //                {
        //                    *((int*)(((LPBYTE)pThunk) + 20)) = _apng_thunk_getlength((LPBYTE)pThunk + 20);
        //                    *((int*)(((LPBYTE)pThunk) + 24)) = _apng_thunk_getlength((LPBYTE)pThunk + 24);
        //                    *((int*)(((LPBYTE)pThunk) + 28)) = _apng_thunk_getlength((LPBYTE)pThunk + 28);
        //
        //                    pFrames[i++] = pThunk;
        //                }
        //            }
        //        }
        //    }
        //}

    }



};



struct __EX_RGN : public EX_RGN
{
#ifndef __USEXP // 不是xp, 使用D2D
    union
    {
        Gdiplus::Region* gpRgn;
        ID2D1Geometry* dxRgn;
    };
#else
    Gdiplus::Region* gpRgn;
#endif
public:
    __EX_RGN() :gpRgn(0) { }
    virtual int Release()
    {
        __EX_RGN* hRgn = this;
        if (!hRgn)return false;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
            SafeRelease(hRgn->dxRgn);
        else
#endif
            delete hRgn->gpRgn;

        hRgn = 0;
        return true;
    }
    virtual bool Hittest(float x, float y)
    {
        __EX_RGN* hRgn = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            D2D1_POINT_2F pt = { x, y };
            BOOL ret = FALSE;
            HRESULT hr = hRgn->dxRgn->FillContainsPoint(pt, 0, 0, &ret);
            if (FAILED(hr))
            {
                ExSetLastError(hr);
                return false;
            }
            return ret;
        }
#endif
        return hRgn->gpRgn->IsVisible(x, y);
    }

};



struct __EX_FONT : public EX_FONT
{
    DWORD flags;
    DWORD count;
    float charWidth;        // 字符'i'的宽度
    float charheight;       // 字符'i'的高度
    HFONT hFont;

#ifndef __USEXP // 不是xp, 使用D2D
    //IDWriteTextLayout* pDWriteTextLayout;       // d2d文本布局对象, 画文本时使用
    IDWriteInlineObject* pWriteInlineObject;    // 文字排版
    union
    {
        Gdiplus::Font* gpFont;
        IDWriteTextFormat* dxFormat;
    };
#else
    Gdiplus::Font* gpFont;
#endif
    LOGFONTW logFont;
public:
    __EX_FONT() :EX_FONT(),
        flags(0),
        count(0),
        charWidth(0),
        charheight(0),
        hFont(0),
        gpFont(0), 
        logFont{ 0 }
    {
#ifndef __USEXP // 不是xp, 使用D2D
        pWriteInlineObject = 0;
#endif
    }

    virtual operator HFONT() { return hFont; }
    virtual int Release()
    {
        __EX_FONT* font = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            SafeRelease(font->dxFormat);
            SafeRelease(font->pWriteInlineObject);
        }
        else
#endif
        {
            delete font->gpFont;
        }
        DeleteObject(font->hFont);
        delete font;
        font = 0;
        return true;
    }
    virtual int height2size(int height)
    {
        // -(参数1 * 参数2 / 参数3)
        // -(-12 * 72 / 96) = 9
        HDC hdc = ::GetDC(0);
        int ret = -MulDiv(height, 72, GetDeviceCaps(hdc, LOGPIXELSY));
        ::ReleaseDC(0, hdc);
        return ret;
    }
    virtual int size2height(int size)
    {
        // -(参数1 * 参数2 / 参数3)
        // -(9 * 96 / 72) = -12
        HDC hdc = ::GetDC(0);
        int ret = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ::ReleaseDC(0, hdc);
        return ret;
    }
    virtual bool GetLogfont(LPLOGFONTW lplf)
    {
        if (!lplf)
            return false;
        memcpy(lplf, &logFont, sizeof(logFont));
        return true;
    }
    virtual HFONT GetFont()
    {
        return hFont;
    }
    virtual int GetCharSize(float* pWidth, float* pHeight)
    {
        if (pWidth)
            *pWidth = charWidth;
        if (pHeight)
            *pHeight = charheight;
        return MAKELONG(charWidth, charheight);
    }
    virtual void SetCharSize(float width, float height)
    {
        charWidth = width;
        charheight = height;
    }


};


struct __EX_PATH : public EX_PATH
{
#ifndef __USEXP // 不是xp, 使用D2D
    ID2D1GeometrySink* pSink;                // dx专用
    union
    {
        Gdiplus::GraphicsPath* gpPath;
        ID2D1PathGeometry* pathGeometry;            // dx专用, 普通路径
        ID2D1Geometry* pGeometry;                   // dx专用, 所有路径的基类
        ID2D1RectangleGeometry* pRectangleGeometry; // dx专用, 圆角路径
        ID2D1RoundedRectangleGeometry* pRoundedRectangleGeometry;   // dx专用, 圆角路径
    };
#else
    Gdiplus::GraphicsPath* gpPath;
#endif
public:
    __EX_PATH() : gpPath(0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        pSink = 0;
#endif
    }

    virtual int Release()
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {

            SafeRelease(path->pSink);
            SafeRelease(path->pathGeometry);
            delete path;
            path = 0;
            return true;
        }
#endif
        if (!path->gpPath)return false;
        delete path->gpPath;
        delete path;
        path = 0;
        return true;
    }
    virtual bool BeginFigure(const LPCPOINT_F pt = 0)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!path->pathGeometry)return false;
            if (!path->pSink)
            {
                HRESULT hr = path->pathGeometry->Open(&path->pSink);
                ExSetLastError(hr);
                if (FAILED(hr) || !path->pSink) return false;
            }
            D2D1_POINT_2F pt1 = { 0 };

            if (pt)
                pt1 = *(D2D1_POINT_2F*)pt;

            path->pSink->BeginFigure(pt1, D2D1_FIGURE_BEGIN_FILLED);
            return true;
        }
#endif
        if (!path->gpPath)return false;
        Gdiplus::Status status = path->gpPath->StartFigure();
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool EndFigure()
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!path->pSink) return false;
            path->pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
            path->pSink->Close();
            SafeRelease(path->pSink);
            return true;
        }
#endif
        if (!path->gpPath)return false;
        Gdiplus::Status status = path->gpPath->CloseFigure();
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool Reset()
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            return _path_reset_dx(path);
        }
#endif
        if (!path->gpPath)return false;
        Gdiplus::Status status = path->gpPath->Reset();
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool AddArc(float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!_path_getpsink(path)) return false;

            D2D1_ARC_SEGMENT arc;
            arc.point.x = x2;
            arc.point.y = y1;
            arc.size.width = radiusX;
            arc.size.height = radiusY;
            arc.rotationAngle = 0.0f;
            arc.sweepDirection = fClockwise
                ? D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE
                : D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
            arc.arcSize = D2D1_ARC_SIZE_SMALL;
            path->pSink->AddArc(arc);
            //path->pSink->AddArc(
            //	D2D1::ArcSegment(
            //		D2D1::Point2F(x2, y2), // end point
            //		D2D1::SizeF(radiusX, radiusY),
            //		0.0f, // rotation angle
            //		D2D1_SWEEP_DIRECTION_CLOCKWISE,
            //		D2D1_ARC_SIZE_SMALL
            //	));
            return true;
        }
#endif
        if (!path->gpPath)return false;
        Gdiplus::Status status = path->gpPath->AddArc(x1, y1, x2, y2, radiusX, radiusY);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool AddBezier(const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!_path_getpsink(path)) return false;
            D2D1_BEZIER_SEGMENT bezier;
            memcpy(&bezier.point1, &pt1, sizeof(bezier.point1));
            memcpy(&bezier.point2, &pt2, sizeof(bezier.point1));
            memcpy(&bezier.point3, &pt3, sizeof(bezier.point2));
            //D2D1::BezierSegment(
            //	D2D1::Point2F(pt1.x, pt1.y),
            //	D2D1::Point2F(pt2.x, pt2.y),
            //	D2D1::Point2F(pt3.x, pt3.y)
            //);
            path->pSink->AddBezier(bezier);
            return true;
        }
#endif
        if (!path->gpPath)return false;
        Gdiplus::Status status = path->gpPath->AddBezier(
            *((Gdiplus::Point*)(&pt1)),
            *((Gdiplus::Point*)(&pt2)),
            *((Gdiplus::Point*)(&pt3)),
            *((Gdiplus::Point*)(&pt4))
        );
        ExSetLastError(status);
        return status == Gdiplus::Ok;
    }
    virtual bool AddLine(const POINT_F& pt1, const POINT_F& pt2)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!_path_getpsink(path)) return false;

            D2D1_POINT_2F p = { pt1.x,pt1.y };
            path->pSink->AddLine(p);

            //path->pSink->AddLine(*((D2D1_POINT_2F*)(&pt1)));
            //path->pSink->AddLine(*((D2D1_POINT_2F*)(&pt2)));
            return true;
        }
#endif
        if (!path->gpPath)return false;
        Gdiplus::Status status = path->gpPath->AddLine(
            *((Gdiplus::PointF*)(&pt1)),
            *((Gdiplus::PointF*)(&pt2)));
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool AddRect(const RECT_F& rc)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!_path_getpsink(path)) return false;
            D2D1_POINT_2F pt[] = {
                D2D1::Point2F(rc.left,rc.top),		// 左上角
                D2D1::Point2F(rc.right,rc.top),		// 右上角
                D2D1::Point2F(rc.right,rc.bottom),	// 右下角
                D2D1::Point2F(rc.left,rc.bottom),	// 左下角
            };
            path->pSink->AddLines(pt, sizeof(pt) / sizeof(pt[0]));
            return true;
        }
#endif
        if (!path->gpPath)return false;

        Gdiplus::RectF rcF;
        rcF.X = rc.left;
        rcF.Y = rc.top;
        rcF.Width = rc.right - rc.left;
        rcF.Height = rc.bottom - rc.top;
        Gdiplus::Status status = path->gpPath->AddRectangle(rcF);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool AddRoundedRect(const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight)
    {
        __EX_PATH* path = this;
        if (IsDxRender())
        {
            bool b = (radiusTopLeft == 0)	//左上->右上
                ? _path_addline(path, POINT_F(rc.left, rc.top), POINT_F(rc.right - radiusTopRight, rc.top))
                : _path_addarc(path, rc.left, rc.top + radiusTopLeft, rc.left + radiusTopLeft, rc.top, radiusTopLeft, radiusTopLeft, true);
            if (!b)return false;

            b = (radiusTopRight == 0)		// 右上-右下
                ? _path_addline(path, POINT_F(rc.right, rc.top), POINT_F(rc.right, rc.bottom - radiusBottomRight))
                : _path_addarc(path, rc.right - radiusTopRight, rc.top, rc.right, rc.top + radiusTopRight, radiusTopRight, radiusTopRight, true);
            if (!b)return false;

            b = (radiusBottomRight == 0)		// 右下-左下
                ? _path_addline(path, POINT_F(rc.right, rc.bottom), POINT_F(rc.left + radiusBottomLeft, rc.bottom))
                : _path_addarc(path, rc.right, rc.bottom - radiusBottomRight, rc.right - radiusBottomRight, rc.bottom, radiusBottomRight, radiusBottomRight, true);
            if (!b)return false;

            b = (radiusBottomLeft == 0)		// 左下-左上
                ? _path_addline(path, POINT_F(rc.left, rc.bottom), POINT_F(rc.left, rc.top + radiusTopLeft))
                : _path_addarc(path, rc.left + radiusBottomLeft, rc.bottom, rc.left, rc.bottom - radiusBottomLeft, radiusBottomLeft, radiusBottomLeft, true)
                && _path_addline(path, POINT_F(rc.left, rc.bottom - radiusBottomLeft), POINT_F(rc.left, rc.top + radiusTopLeft));
            if (!b)return false;

            return true;
        }
        if (!path->gpPath)return false;

        radiusTopLeft = radiusTopLeft * 2;
        radiusTopRight = radiusTopRight * 2;
        radiusBottomRight = radiusBottomRight * 2;
        radiusBottomLeft = radiusBottomLeft * 2;

        bool b = (radiusTopLeft == 0)	// 左上角圆弧
            ? path->gpPath->AddLine(rc.left, rc.top, rc.right - radiusTopRight, rc.top)
            : path->gpPath->AddArc(rc.left, rc.top, radiusTopLeft, radiusTopLeft, 180, 90);
        if (!b)return false;

        b = (radiusTopRight == 0)		// 右上角圆弧
            ? path->gpPath->AddLine(rc.right, rc.top, rc.right, rc.bottom - radiusBottomRight)
            : path->gpPath->AddArc(rc.right - radiusTopRight, rc.top, radiusTopRight, radiusTopRight, 270, 90);
        if (!b)return false;

        b = (radiusBottomRight == 0)		// 右下角圆弧
            ? path->gpPath->AddLine(rc.right, rc.bottom, rc.left + radiusBottomLeft, rc.bottom)
            : path->gpPath->AddArc(rc.right - radiusBottomRight, rc.bottom - radiusBottomRight, radiusBottomRight, radiusBottomRight, 0, 90);
        if (!b)return false;

        b = (radiusBottomLeft == 0)		// 左下角
            ? path->gpPath->AddLine(rc.left, rc.bottom, rc.left, rc.top + radiusTopLeft)
            : path->gpPath->AddArc(rc.left, rc.bottom - radiusBottomLeft, radiusBottomLeft, radiusBottomLeft, 90, 90)
            && path->gpPath->AddLine(rc.left, rc.top + radiusTopLeft, rc.left, rc.bottom - radiusTopLeft);
        if (!b)return false;
        return true;
    }
    virtual bool Open()
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!path->pathGeometry) return false;
            if (path->pSink) return true;
            HRESULT hr = path->pathGeometry->Open(&path->pSink);
            ExSetLastError(hr);
            return SUCCEEDED(hr);
        }
#endif
        return true;
    }
    virtual bool Close()
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!path->pSink) return false;
            HRESULT hr = path->pSink->Close();
            ExSetLastError(hr);
            return SUCCEEDED(hr);
        }
#endif
        return true;
    }
    virtual bool GetBounds(LPRECT_F rc)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (!path->pathGeometry) return false;
            HRESULT hr = path->pathGeometry->GetBounds(0, (D2D1_RECT_F*)rc);
            ExSetLastError(hr);
            return SUCCEEDED(hr);
        }
#endif
        if (!path->gpPath) return false;
        Gdiplus::Status status = path->gpPath->GetBounds((Gdiplus::RectF*)rc);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    // 测试坐标是否在可见路径内
    virtual bool Hittest(const POINT_F& pt)
    {
        __EX_PATH* path = this;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            BOOL contains = FALSE;
            HRESULT hr = path->pathGeometry->FillContainsPoint(
                *((D2D1_POINT_2F*)(&pt)),
                0, 0, &contains);
            ExSetLastError(hr);
            return contains;
        }
#endif
        if (!path->gpPath)return false;
        return path->gpPath->IsVisible(pt.x, pt.y);
    }



public:
#ifndef __USEXP // 不是xp, 使用D2D
    static inline void _path_destroy_dx(LPEX_PATH path)
    {
        if (!path)return;
        __EX_PATH* pPath = (__EX_PATH*)path;

        SafeRelease(pPath->pSink);
        SafeRelease(pPath->pathGeometry);
    }
    // 销毁路径, 重新创建, 会设置错误
    static inline bool _path_reset_dx(LPEX_PATH path)
    {
        if (!d2dInfo.pFactory || !path)return false;
        _path_destroy_dx(path);
        __EX_PATH* pPath = (__EX_PATH*)path;
        HRESULT hr = d2dInfo.pFactory->CreatePathGeometry(&pPath->pathGeometry);
        ExSetLastError(hr);
        return SUCCEEDED(hr);
    }
    static inline bool _path_getpsink(LPEX_PATH path)
    {
        __EX_PATH* pPath = (__EX_PATH*)path;
        if (!pPath || !pPath->pathGeometry) return false;
        if (!pPath->pSink) return false;
        //{
        //	HRESULT hr = path->pathGeometry->Open(&path->pSink);
        //	ExSetLastError(hr);
        //	return SUCCEEDED(hr);
        //}
        return true;
    }
#endif

};

struct __EX_BRUSH : public EX_BRUSH
{
    union
    {
        Gdiplus::Brush* brush;
        Gdiplus::SolidBrush* Solid;
        Gdiplus::LinearGradientBrush* Linear;

#ifndef __USEXP // 不是xp, 使用D2D
        ID2D1Brush* dxbrush;                        // 基类
        ID2D1SolidColorBrush* SolidBrush;           // 纯色画刷
        ID2D1LinearGradientBrush* LinearBrush;      // 线性渐变画刷
        ID2D1RadialGradientBrush* RadialBrush;      // 放射渐变画刷
        ID2D1BitmapBrush1* BitmapBrush;             // 位图画刷
#endif
    };
    int type;                                       // 当前画笔类型, BRUSHTYPE_ 开头常量

public:
    __EX_BRUSH() :brush(0), type(0) { }

    virtual int Release()
    {
        if (IsDxRender())
        {
#ifndef __USEXP // 不是xp, 使用D2D
            SafeRelease(dxbrush);
#endif
            delete this;
            return 0;
        }
        delete brush;
        delete this;
        return 0;
    }
    virtual bool SetColor(ARGB argb)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (type != BRUSHTYPE_SOLID)
            {
                // 不是纯色画刷, 不支持此功能
                return false;
            }
            SolidBrush->SetColor(ARGB_D2D::toDx(argb));
            return true;
        }
#endif
        Gdiplus::Status status = Solid->SetColor(argb);
        ExSetLastError(status);
        return status == Gdiplus::Ok;
    }
    virtual ARGB GetColor()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (type != BRUSHTYPE_SOLID)
            {
                // 不是纯色画刷, 不支持此功能
                return false;
            }
            ARGB_D2D a = SolidBrush->GetColor();
            return a.toGdiplus();
        }
#endif
        Gdiplus::Color cr;
        Gdiplus::Status status = Solid->GetColor(&cr);
        ExSetLastError(status);
        return cr.GetValue();
    }
#ifndef __USEXP // 不是xp, 使用D2D
    virtual bool SetStartPoint(float x, float y)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_LINEAR)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt;
        pt.x = x;
        pt.y = y;
        LinearBrush->SetStartPoint(pt);
        return true;
    }
    virtual bool GetStartPoint(float* x = 0, float* y = 0)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (!x && !y)return false;
        if (type != BRUSHTYPE_LINEAR)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt = LinearBrush->GetStartPoint();
        if (x)*x = pt.x;
        if (y)*y = pt.y;
        return true;
    }
    virtual bool SetEndPoint(float x, float y)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_LINEAR)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt;
        pt.x = x;
        pt.y = y;
        LinearBrush->SetEndPoint(pt);
        return true;
    }
    virtual bool GetEndPoint(float* x = 0, float* y = 0)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_LINEAR)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt = LinearBrush->GetEndPoint();
        if (x)*x = pt.x;
        if (y)*y = pt.y;
        return true;
    }
    virtual bool SetCenter(float x, float y)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt;
        pt.x = x;
        pt.y = y;
        RadialBrush->SetCenter(pt);
        return true;
    }
    virtual bool GetCenter(float* x = 0, float* y = 0)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt = RadialBrush->GetCenter();
        if (x)*x = pt.x;
        if (y)*y = pt.y;
        return true;
    }
    // 置渐变原点偏移
    virtual bool SetGradientOriginOffset(float x, float y)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt;
        pt.x = x;
        pt.y = y;
        RadialBrush->SetGradientOriginOffset(pt);
        return true;
    }
    virtual bool GetGradientOriginOffset(float* x = 0, float* y = 0)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        D2D1_POINT_2F pt = RadialBrush->GetGradientOriginOffset();
        if (x)*x = pt.x;
        if (y)*y = pt.y;

        return true;
    }
    virtual bool SetRadiusX(float x)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        RadialBrush->SetRadiusX(x);
        return true;
    }
    virtual float GetRadiusX()
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        return RadialBrush->GetRadiusX();
    }
    virtual bool SetRadiusY(float y)
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        RadialBrush->SetRadiusY(y);
        return true;
    }
    virtual float GetRadiusY()
    {
        if (!IsDxRender())return false;	// gdi+不支持此操作
        if (type != BRUSHTYPE_RADIAL)
        {
            // 画刷类型正确, 不支持此功能
            return false;
        }
        return RadialBrush->GetRadiusY();
    }
#endif
public:
    static __EX_BRUSH* create(ARGB argb)
    {
        __EX_BRUSH* brush = new __EX_BRUSH;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            HRESULT hr = d2dInfo.pD2DDeviceContext->CreateSolidColorBrush(ARGB_D2D::toDx(argb),
                D2D1::BrushProperties(),
                &brush->SolidBrush);
            ExSetLastError(hr);
            if (FAILED(hr))
            {
                delete brush;
                return 0;
            }
            brush->type = BRUSHTYPE_SOLID;
            return brush;
        }
#endif
        brush->brush = new Gdiplus::SolidBrush(argb);
        brush->type = BRUSHTYPE_SOLID;
        return brush;
    }
    // 创建渐变画刷
    static __EX_BRUSH* create(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount)
    {
        if (!color || !colorCount)return 0;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES pt;
            memcpy(&pt.startPoint, &pt1, sizeof(pt.startPoint));
            memcpy(&pt.endPoint, &pt2, sizeof(pt.endPoint));

            _ptrs<D2D1_GRADIENT_STOP> gradientStops = new D2D1_GRADIENT_STOP[colorCount];
            const float* ratios = 0;
            if (pRatios && ratiosCount == colorCount)
                ratios = pRatios;

            for (DWORD i = 0; i < colorCount; i++)
            {
                gradientStops[i].color = ARGB_D2D::toDx(color[i]);
                if (ratios)
                    gradientStops[i].position = ratios[i];
                else
                    gradientStops[i].position = ((float)i) / (((float)colorCount) - 1.0f);	// 0-1
            }

            comptr<ID2D1GradientStopCollection> pGradientStops;
            ID2D1LinearGradientBrush* pBrush = 0;

            HRESULT hr = d2dInfo.pD2DDeviceContext->CreateGradientStopCollection(
                gradientStops, colorCount,
                D2D1_GAMMA_2_2,
                (D2D1_EXTEND_MODE)fillMode,
                &pGradientStops);
            ExSetLastError(hr);
            if (FAILED(hr))return 0;

            hr = d2dInfo.pD2DDeviceContext->CreateLinearGradientBrush(pt, pGradientStops, &pBrush);
            ExSetLastError(hr);

            __EX_BRUSH* brush = new __EX_BRUSH;
            brush->dxbrush = pBrush;
            brush->type = BRUSHTYPE_LINEAR;
            return brush;
        }
#endif

        _ptrs<Gdiplus::Color> colors = new Gdiplus::Color[colorCount];
        _ptrs<float> ratios = new float[colorCount];
        memset(ratios, 0, sizeof(float) * colorCount);
        if (pRatios && ratiosCount == colorCount)
            memcpy(ratios, pRatios, sizeof(float) * colorCount);

        for (DWORD i = 0; i < colorCount; i++)
        {
            colors[i] = color[i];
            if (ratios[i] == 0.0f)
                ratios[i] = ((float)i) / (((float)colorCount) - 1.0f);	// 0-1
        }

        //Gdiplus::RectF rc;
        //rc.X = pt1.x;
        //rc.Y = pt1.y;
        //rc.Width = pt2.x - pt1.x;
        //rc.Height = pt2.y - pt1.y;
        //Gdiplus::LinearGradientBrush* pBrush = new Gdiplus::LinearGradientBrush(
        //	rc, colors[0],
        //	colors[colorCount - 1],
        //	(Gdiplus::LinearGradientMode)fillMode
        //);
        Gdiplus::LinearGradientBrush* pBrush = new Gdiplus::LinearGradientBrush(
            *((Gdiplus::PointF*)(&pt1)),
            *((Gdiplus::PointF*)(&pt2)),
            colors[0], colors[colorCount - 1]
        );
        pBrush->SetInterpolationColors(colors, ratios, colorCount);
        pBrush->SetWrapMode((Gdiplus::WrapMode)fillMode);
        __EX_BRUSH* brush = new __EX_BRUSH;
        brush->brush = pBrush;
        brush->type = BRUSHTYPE_LINEAR;
        return brush;
    }
    // pos = 包含中心点以及向外扩展的长度
    static __EX_BRUSH* create(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {	//d2dInfo.pRenderTarget->CreateLinearGradientBrush(,)
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES pt;
            pt.center.x = pos->x;
            pt.center.y = pos->y;
            pt.gradientOriginOffset.x = 0;
            pt.gradientOriginOffset.y = 0;
            pt.radiusX = pos->radiusX;
            pt.radiusY = pos->radiusY;
            _ptrs<D2D1_GRADIENT_STOP> gradientStops = new D2D1_GRADIENT_STOP[colorCount];
            const float* ratios = 0;
            if (pRatios && ratiosCount == colorCount)
                ratios = pRatios;

            for (DWORD i = 0; i < colorCount; i++)
            {
                gradientStops[i].color = ARGB_D2D::toDx(color[i]);
                if (ratios)
                    gradientStops[i].position = ratios[i];
                else
                    gradientStops[i].position = ((float)i) / (((float)colorCount) - 1.0f);	// 0-1
            }

            comptr<ID2D1GradientStopCollection> pGradientStops = 0;
            ID2D1RadialGradientBrush* pBrush = 0;
            HRESULT hr = d2dInfo.pD2DDeviceContext->CreateGradientStopCollection(
                gradientStops, colorCount,
                D2D1_GAMMA_2_2,
                (D2D1_EXTEND_MODE)fillMode,
                &pGradientStops);
            ExSetLastError(hr);
            if (FAILED(hr))return 0;

            hr = d2dInfo.pD2DDeviceContext->CreateRadialGradientBrush(pt, pGradientStops, &pBrush);
            ExSetLastError(hr);

            __EX_BRUSH* brush = new __EX_BRUSH;
            brush->dxbrush = pBrush;
            brush->type = BRUSHTYPE_RADIAL;
            return brush;
        }
#endif
        _ptrs<Gdiplus::Color> colors = new Gdiplus::Color[colorCount];
        _ptrs<float> ratios = new float[colorCount];
        memset(ratios, 0, sizeof(float) * colorCount);
        if (pRatios && ratiosCount == colorCount)
            memcpy(ratios, pRatios, sizeof(float) * colorCount);

        for (DWORD i = 0; i < colorCount; i++)
        {
            colors[i] = color[i];
            if (ratios[i] == 0.0f)
                ratios[i] = ((float)i) / (((float)colorCount) - 1.0f);	// 0-1
        }

        Gdiplus::GraphicsPath path;
        //path.AddRectangle(pos.toGpRectF());
        path.AddEllipse(pos->toGpRectF());
        // 构造路径渐变画刷
        Gdiplus::PathGradientBrush* pBrush = new Gdiplus::PathGradientBrush(&path);

        // 设置中心点色彩
        pBrush->SetCenterColor(colors[0]);

        // 设置每个端点的色彩
        INT count = colorCount - 1;
        pBrush->SetSurroundColors(colors + 1, &count);
        // 设置中心点
        pBrush->SetCenterPoint(Gdiplus::PointF(pos->x, pos->y));
        pBrush->SetInterpolationColors(colors, ratios, colorCount);
        pBrush->SetWrapMode((Gdiplus::WrapMode)fillMode);

        __EX_BRUSH* brush = new __EX_BRUSH;
        brush->brush = pBrush;
        brush->type = BRUSHTYPE_RADIAL;
        return brush;
    }



};


struct __EX_PEN : public EX_PEN
{
    union
    {
        Gdiplus::Pen* pen;
        Gdiplus::SolidBrush* Solid;
        Gdiplus::LinearGradientBrush* Linear;

#ifndef __USEXP // 不是xp, 使用D2D
        ID2D1Brush* dxbrush;                        // 基类
        ID2D1SolidColorBrush* SolidBrush;           // 纯色画刷
        ID2D1LinearGradientBrush* LinearBrush;      // 线性渐变画刷
        ID2D1RadialGradientBrush* RadialBrush;      // 放射渐变画刷
        ID2D1BitmapBrush1* BitmapBrush;             // 位图画刷
#endif
    };
    int type;                           // 当前画笔类型, BRUSHTYPE_ 开头常量
#ifndef __USEXP // 不是xp, 使用D2D
    ID2D1StrokeStyle1* strokeStyle;     // DX专用, 画线的线样式, gdi+的话有单独的线条方法
#endif
    float width;                        // 线条宽度

public:
    __EX_PEN() :pen(0), type(0), width(0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        strokeStyle = 0;
#endif

    }

    virtual int Release()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            SafeRelease(strokeStyle);
            SafeRelease(dxbrush);
            delete this;
            return true;
        }
#endif
        delete pen;
        delete this;
        return true;
    }

    virtual bool SetColor(ARGB argb)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (type != BRUSHTYPE_SOLID)
            {
                // 不是纯色画刷, 不支持此功能
                return false;
            }
            SolidBrush->SetColor(ARGB_D2D::toDx(argb));
            return true;
        }
#endif
        Gdiplus::Status status = pen->SetColor(argb);
        return status == Gdiplus::Ok;
    }
    virtual ARGB GetColor()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (type != BRUSHTYPE_SOLID)
            {
                // 不是纯色画刷, 不支持此功能
                return false;
            }
            ARGB_D2D a = SolidBrush->GetColor();
            return a.toGdiplus();
        }
#endif
        Gdiplus::Color cr;
        bool ret = pen->GetColor(&cr) == Gdiplus::Status::Ok;
        return cr.GetValue();
    }
    virtual bool SetWidth(float width)
    {
        this->width = width;
        if (IsDxRender())
        {
            return true;
        }
        return pen->SetWidth(width) == Gdiplus::Status::Ok;
    }
    virtual float GetWidth()
    {
        if (IsDxRender())
        {
            return width;
        }
        return pen->GetWidth();
    }
    virtual float SetDashStyle(int style = 0, float* dashes = 0, int dashesCount = 0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (strokeStyle)   // 已经存在就销毁后重新创建
                SafeRelease(strokeStyle);
            _pen_get_strokeStyle(this, style, dashes, dashesCount);
            return true;
        }
#endif
        bool ret = pen->SetDashStyle((Gdiplus::DashStyle)style) == Gdiplus::Status::Ok;
        if (dashes && dashesCount)
            pen->SetDashPattern(dashes, dashesCount);
        return ret;
    }
public:
    static LPEX_PEN create(ARGB color, float width, int style, const float* dashes, int dashesCount)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            __EX_PEN* hPen = new __EX_PEN;
            HRESULT hr = d2dInfo.pD2DDeviceContext->CreateSolidColorBrush(ARGB_D2D::toDx(color),
                D2D1::BrushProperties(),
                &hPen->SolidBrush);
            ExSetLastError(hr);
            if (FAILED(hr))
            {
                delete hPen;
                return 0;
            }
            hPen->type = BRUSHTYPE_SOLID;
            hPen->width = width;
            hPen->strokeStyle = 0;
            if ((dashes && dashesCount) || style != D2D1_DASH_STYLE_SOLID)
                _pen_get_strokeStyle(hPen, style, dashes, dashesCount);
            return hPen;
        }
#endif
        __EX_PEN* pen = new __EX_PEN;
        pen->pen = new Gdiplus::Pen(color, width);
        pen->pen->SetAlignment(Gdiplus::PenAlignment::PenAlignmentInset);
        pen->type = BRUSHTYPE_PEN;
        pen->width = width;
        if ((dashes && dashesCount) || style != Gdiplus::DashStyle::DashStyleSolid)
        {
            pen->pen->SetDashStyle(Gdiplus::DashStyle::DashStyleDot);
            pen->pen->SetDashPattern(dashes, dashesCount);
        }
        return pen;
    }

public:
#ifndef __USEXP // 不是xp, 使用D2D
    // 获取画笔样式, 如果没有则会创建一个样式
    // pen = 画笔对象
    // dashes = 点线的数组, 划线={2,2}, 点线={0,2}, 点划线={2,2,0,2}, 点点划线={2,2,0,2,0,2}, 其他请自定义
    // dashesCount = 数组成员数
    static inline ID2D1StrokeStyle1* _pen_get_strokeStyle(LPEX_PEN pen, int style = D2D1_DASH_STYLE_SOLID, const float* dashes = 0, int dashesCount = 0, HRESULT* err = 0)
    {
        if (!pen)return 0;
        __EX_PEN* _pPen = (__EX_PEN*)pen;
        if (_pPen->strokeStyle)
        {
            return _pPen->strokeStyle;
        }
        //0=实线, 1=划线{2, 2}, 2=点线{0, 2}, 3=点划线{2, 2, 0, 2}, 4=点点划线{2, 2, 0, 2, 0, 2}, 5=自定义
        float arr[] = { 2, 2, 0, 2, 0, 2 };

        D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties;
        strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
        strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
        strokeStyleProperties.dashCap = D2D1_CAP_STYLE_FLAT;
        strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_MITER;
        strokeStyleProperties.miterLimit = 10.f;
        strokeStyleProperties.dashStyle = (D2D1_DASH_STYLE)style;
        strokeStyleProperties.dashOffset = 0.0f;
        strokeStyleProperties.transformType = D2D1_STROKE_TRANSFORM_TYPE_NORMAL;
        if (!dashes || !dashesCount)
        {
            switch (strokeStyleProperties.dashStyle)
            {
            case D2D1_DASH_STYLE_SOLID:
            case D2D1_DASH_STYLE_DASH:
            case D2D1_DASH_STYLE_DOT:
            case D2D1_DASH_STYLE_DASH_DOT:
            case D2D1_DASH_STYLE_DASH_DOT_DOT:
                dashes = 0; dashesCount = 0; break;
            case D2D1_DASH_STYLE_CUSTOM:
                break;
            default:
                break;
            }
        }
        HRESULT hr = d2dInfo.pFactory->CreateStrokeStyle(strokeStyleProperties, dashes, dashesCount, &_pPen->strokeStyle);
        ExSetLastError(hr);
        if (err)*err = hr;
        return _pPen->strokeStyle;
    }
#endif
};


struct __CANVAS : public CANVAS
{
    HWND hWnd;          // 创建绘画环境的窗口句柄, 暂时没用到
    HDC hdc;            // gdi使用的内存DC
    LPBYTE lpBits;      // gdi位图数据地址
    UINT width;         // 位图宽度, width * height = pBits 像素点数量, 像素点数量 * 4 = pBits 的字节数
    UINT height;        // 位图高度, width * height = pBits 像素点数量, 像素点数量 * 4 = pBits 的字节数
    DWORD flags;
    LONG addlock;       // 计数, begindraw +1, enddraw -1, 只有为0的时候才调用 begindraw, 减到0的时候才调用 enddraw

#ifndef __USEXP // 不是xp, 使用D2D
    ID2D1GdiInteropRenderTarget* pGDIInterop;   // d2d兼容gdi对象, 通过这个对象获取HDC
    union
    {
        ID2D1Bitmap1* pBitmap;  // d2d位图
        HBITMAP hBitmap;        // 32位绘制位图
    };
    union
    {
        Gdiplus::Graphics* pGraihics;       // gdi+ 画布
        ID2D1DeviceContext* pRenderTarget;  // d2d设备上下文, d2d使用这个进行绘画, 结果保存在 pBitmap 这个位图里
    };
#else
    HBITMAP hBitmap;        // 32位绘制位图
    Gdiplus::Graphics* pGraihics;       // gdi+ 画布
#endif

public:
    __CANVAS() :
        hWnd(0),
        hdc(0),
        lpBits(0),
        width(0),
        height(0),
        flags(0),
        addlock(0),
        hBitmap(0),
        pGraihics(0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        pGDIInterop = 0;
#endif

    }
    virtual int Release()
    {
        LPCANVAS hCanvas = this;
        if (!hCanvas)return 0;

        __CANVAS* canvas = (__CANVAS*)hCanvas;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            SafeRelease(canvas->pBitmap);
            SafeRelease(canvas->pGDIInterop);
            SafeRelease(canvas->pRenderTarget);
            delete canvas;
            hCanvas = 0;
            return true;
        }
#endif
        _md_destroy(canvas);
        delete canvas->pGraihics;
        delete canvas;
        hCanvas = 0;
        return true;
    }
    virtual HWND GetWindow() { return hWnd; }
    virtual UINT GetWidth() { return width; }
    virtual UINT GetHeight() { return height; }
    virtual bool GetTargetRender(LPVOID* ppObj)
    {
        if (!ppObj)
            return false;
        *ppObj = pGraihics;
        return true;
    }
    virtual HDC GetGdiDC()
    {
        return hdc;
    }
    virtual bool BeginDraw()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (addlock == 0)
                pRenderTarget->BeginDraw();
            __add_lock(1);
            pRenderTarget->SetTarget(pBitmap);
        }
#endif
        return true;
    }
    virtual bool EndDraw()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->SetTarget(0);
            HRESULT hr = S_OK;
            if (__add_lock(-1) == 1)
                hr = pRenderTarget->EndDraw();
            ExSetLastError(hr);
            return SUCCEEDED(hr);
        }
#endif
        return true;
    }

    virtual bool Resize(int cx, int cy)
    {
        return recreate(this, cx, cy, false);
    }

    virtual bool GetSize(UINT* width, UINT* height)
    {
        if (width)*width = this->width;
        if (height)*height = this->height;
        return true;
    }

    virtual HDC GetDC()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            HDC hdcD2D;
            HRESULT hr = pGDIInterop->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hdcD2D);
            if (FAILED(hr))
                ExSetLastError(hr);
            return SUCCEEDED(hr) ? hdcD2D : 0;
        }
#endif
        return hdc;
    }
    virtual void ReleaseDC()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            HRESULT hr = pGDIInterop->ReleaseDC(0);
            if (FAILED(hr))
                ExSetLastError(hr);
        }
#endif
        return;
    }

    virtual bool clear(ARGB argb)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->Clear(ARGB_D2D::toDx(argb));
            return true;
        }
#endif
        int hr = pGraihics->Clear(argb);
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }

    virtual bool flush()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            LRESULT hr = pRenderTarget->Flush();
            ExSetLastError((int)hr);
            return SUCCEEDED(hr);
        }
#endif
        return true;
    }

    virtual bool SetTarget(LPEX_IMAGE bitmap)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            __EX_IMAGE* pBitmap = (__EX_IMAGE*)bitmap;
            pRenderTarget->SetTarget(pBitmap->GetDxBitmap());
            return true;
        }
#endif
        return true;
    }
    virtual bool ReTarget()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->SetTarget(pBitmap);
            return true;
        }
#endif
        return true;
    }
    virtual LPEX_IMAGE GetTarget()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            ID2D1Image* pImg;
            pRenderTarget->GetTarget(&pImg);
            if (pImg)
            {
                __EX_IMAGE* ret = new __EX_IMAGE;
                ret->d2dBitmap = (ID2D1Bitmap1*)pImg;
                ret->height = height;
                ret->width = width;
                return 0;
            }
        }
#endif
        return 0;
    }
    virtual void FreeTarget(LPEX_IMAGE img)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        __EX_IMAGE* _pImg = (__EX_IMAGE*)img;
        _pImg->d2dBitmap = 0;
        _pImg->Release();
#endif
    }


    // 平移
    virtual bool translate(float x, float y)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(x, y)));
            return true;
        }
#endif
        //RotateTransform
        Gdiplus::Status hr = pGraihics->TranslateTransform(x, y);
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }
    // 缩放
    virtual bool scale(float x, float y, float scaleX, float scaleY)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::SizeF(scaleX, scaleY), D2D1::Point2F(x, y)));
            return true;
        }
#endif
        //RotateTransform
        pGraihics->TranslateTransform(x, y);
        Gdiplus::Status hr = pGraihics->ScaleTransform(scaleX, scaleY);
        pGraihics->TranslateTransform(-x, -y);
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }
    // 旋转
    virtual bool rotate(float x, float y, float rotate)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(rotate, D2D1::Point2F(x, y)));
            return true;
        }
#endif
        pGraihics->TranslateTransform(x, y);
        Gdiplus::Status hr = pGraihics->RotateTransform(rotate);
        pGraihics->TranslateTransform(-x, -y);
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }




    // 绘制简单图形
    virtual bool DrawRectangle(const LPEX_PEN hPen, const LPCRECT_F rcF)
    {
        if (!hPen) return false;
        __EX_PEN* _pPen = (__EX_PEN*)hPen;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->DrawRectangle((D2D1_RECT_F*)rcF, _pPen->dxbrush, _pPen->width, _pPen->strokeStyle);
            return true;
        }
#endif
        Gdiplus::Status hr = pGraihics->DrawRectangle(_pPen->pen,
            rcF->left, rcF->top, rcF->right - rcF->left, rcF->bottom - rcF->top);

        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }

    virtual bool FillRectangle(const LPEX_BRUSH hBrush, const LPCRECT_F rcF)
    {
        if (!hBrush) return false;
        __EX_BRUSH* _pBrush = (__EX_BRUSH*)hBrush;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->FillRectangle((D2D1_RECT_F*)rcF, _pBrush->dxbrush);
            return true;
        }
#endif
        //Gdiplus::SmoothingMode oldMode = Gdiplus::SmoothingMode::SmoothingModeAntiAlias;
        //if ((d2dInfo.flags & EX_RENDER_GDI) == EX_RENDER_GDI)
        //{
        //    oldMode = hCanvas->pGraihics->GetSmoothingMode();   // gdi+ 填充矩形需要去除抗锯齿
        //    hCanvas->pGraihics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeNone);
        //}
        Gdiplus::Status hr = pGraihics->FillRectangle(_pBrush->brush,
            rcF->left, rcF->top, rcF->right - rcF->left, rcF->bottom - rcF->top);

        //if ((d2dInfo.flags & EX_RENDER_GDI) == EX_RENDER_GDI)
        //    hCanvas->pGraihics->SetSmoothingMode(oldMode);

        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }
    virtual bool DrawEllipse(const LPEX_PEN hPen, const LPCELLIPSE_F rcF)
    {
        if (!hPen) return false;
        __EX_PEN* _pPen = (__EX_PEN*)hPen;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->DrawEllipse((D2D1_ELLIPSE*)rcF, _pPen->dxbrush, _pPen->width, _pPen->strokeStyle);
            return true;
        }
#endif
        Gdiplus::Status hr = pGraihics->DrawEllipse(_pPen->pen, rcF->toGpRectF());
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }

    virtual bool FillEllipse(const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF)
    {
        if (!hBrush) return false;
        __EX_BRUSH* _pBrush = (__EX_BRUSH*)hBrush;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->FillEllipse((D2D1_ELLIPSE*)rcF, _pBrush->dxbrush);
            return true;
        }
#endif
        Gdiplus::Status hr = pGraihics->FillEllipse(_pBrush->brush, rcF->toGpRectF());
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }
    virtual bool DrawRoundedRectangle(const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded)
    {
        if (!hPen) return false;
        __EX_PEN* _pPen = (__EX_PEN*)hPen;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->DrawRoundedRectangle((D2D1_ROUNDED_RECT*)rcRounded, _pPen->dxbrush, _pPen->width, _pPen->strokeStyle);
            return true;
        }
#endif
        Gdiplus::GraphicsPath* path = __createRoundedrect(
            rcRounded->left, rcRounded->top,
            rcRounded->right - rcRounded->left,
            rcRounded->bottom - rcRounded->top, rcRounded->radiusX, rcRounded->radiusY);
        if (!path)return false;

        Gdiplus::Status hr = pGraihics->DrawPath(_pPen->pen, path);
        ExSetLastError(hr);
        delete path;
        return hr == Gdiplus::Status::Ok;
    }
    virtual bool FillRoundedRectangle(const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded)
    {
        if (!hBrush) return false;
        __EX_BRUSH* _pBrush = (__EX_BRUSH*)hBrush;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->FillRoundedRectangle((D2D1_ROUNDED_RECT*)rcRounded, _pBrush->dxbrush);
            return true;
        }
#endif
        Gdiplus::GraphicsPath* path = __createRoundedrect(
            rcRounded->left, rcRounded->top,
            rcRounded->right - rcRounded->left,
            rcRounded->bottom - rcRounded->top, rcRounded->radiusX, rcRounded->radiusY);
        if (!path)return false;

        Gdiplus::Status hr = pGraihics->FillPath(_pBrush->brush, path);
        ExSetLastError(hr);
        delete path;
        return hr == Gdiplus::Status::Ok;
    }
    virtual bool DrawLine(const LPEX_PEN hPen, const LPCLINEPOINT_F pt)
    {
        if (!hPen)return false;
        __EX_PEN* _pPen = (__EX_PEN*)hPen;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->DrawLine(pt->pt1(), pt->pt2(), _pPen->dxbrush, _pPen->width, _pPen->strokeStyle);
            return true;
        }
#endif
        Gdiplus::Status hr = pGraihics->DrawLine(_pPen->pen, pt->x, pt->y, pt->x1, pt->y1);
        ExSetLastError(hr);
        return hr == Gdiplus::Status::Ok;
    }

    virtual bool DrawPath(const LPEX_PEN hPen, const LPEX_PATH path)
    {
        if (!hPen || !path)
            return false;
        __EX_PATH* pPath = (__EX_PATH*)path;
        __EX_PEN* _pPen = (__EX_PEN*)hPen;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->DrawGeometry(pPath->pathGeometry, _pPen->dxbrush, 1.f, _pPen->strokeStyle);
            return true;
        }
#endif
        Gdiplus::Status status = pGraihics->DrawPath(_pPen->pen, pPath->gpPath);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }

    virtual bool FillPath(const LPEX_BRUSH hBrush, const LPEX_PATH path)
    {
        if (!hBrush || !path)
            return false;
        __EX_PATH* pPath = (__EX_PATH*)path;
        __EX_BRUSH* _pBrush = (__EX_BRUSH*)hBrush;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->FillGeometry(pPath->pathGeometry, _pBrush->dxbrush);
            return true;
        }
#endif
        Gdiplus::Status status = pGraihics->FillPath(_pBrush->brush, pPath->gpPath);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }

    virtual bool DrawImage(LPEX_IMAGE image, float left, float top, BYTE alpha)
    {
        if (!image)return false;
        UINT width, height;
        image->GetSize(&width, &height);
        return DrawImage(image,
            left, top, left + static_cast<float>(width), top + static_cast<float>(height),
            0, 0, static_cast<float>(width), static_cast<float>(height), alpha);
    }
    virtual bool DrawImage(LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha)
    {
        if (!image)return false;
        UINT width, height;
        image->GetSize(&width, &height);
        return DrawImage(image,
            left, top, right, bottom,
            0, 0, static_cast<float>(width), static_cast<float>(height), alpha);
    }
    virtual bool DrawImage(LPEX_IMAGE image,
        float dstLeft, float dstTop, float dstRight, float dstBottom,
        float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha)
    {
        if (!image) return false;
        __EX_IMAGE* _pImg = (__EX_IMAGE*)image;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            HRESULT hr = 0;
            if (!_pImg->GetDxBitmap(&hr))
            {
                D2D1_BITMAP_PROPERTIES1 info;
                info.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
                info.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
                info.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE;
                info.dpiX = 96;
                info.dpiY = 96;
                info.colorContext = 0;

                hr = d2dInfo.pD2DDeviceContext->CreateBitmapFromWicBitmap(_pImg->dxImg, &info, &_pImg->d2dBitmap);
                if (FAILED(hr))
                {
#ifdef _DEBUG
                    char err[100];
#if _MSC_VER < 1201
                    sprintf(err, "dx获取图标失败, 错误码 = 0x%08X\n", hr);
#else
                    sprintf_s(err, 100, "dx获取图标失败, 错误码 = 0x%08X\n", hr);
#endif
                    OutputDebugStringA(err);
#endif
                    return false;
                }
            }
            D2D1_RECT_F rcDst, rcSrc;
            rcDst.left = dstLeft;
            rcDst.top = dstTop;
            rcDst.right = dstRight;
            rcDst.bottom = dstBottom;
            if (srcRight == 0)srcRight = dstRight - dstLeft;
            if (srcBottom == 0)srcBottom = dstBottom - dstTop;
            rcSrc.left = srcLeft;
            rcSrc.top = srcTop;
            rcSrc.right = srcRight;
            rcSrc.bottom = srcBottom;

            pRenderTarget->DrawBitmap(
                _pImg->d2dBitmap,
                &rcDst,
                static_cast<float>(alpha) / 255.0f,
                D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                &rcSrc
            );
            return true;
        }
#endif
        if (alpha == 0)return true;

        Gdiplus::RectF rcDst, rcSrc;
        rcDst.X = dstLeft;
        rcDst.Y = dstTop;
        rcDst.Width = dstRight - dstLeft;
        rcDst.Height = dstBottom - dstTop;
        if (srcRight == 0)srcRight = dstRight - dstLeft;
        if (srcBottom == 0)srcBottom = dstBottom - dstTop;
        rcSrc.X = srcLeft;
        rcSrc.Y = srcTop;
        rcSrc.Width = srcRight - srcLeft;
        rcSrc.Height = srcBottom - srcTop;
        static Gdiplus::ImageAttributes* attr = new Gdiplus::ImageAttributes;

        float cm[] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
        Gdiplus::ColorMatrix m;
        cm[18] = static_cast<float>(alpha) / 255.0f;
        memcpy(&m, cm, sizeof(m));
        Gdiplus::Status status = attr->SetColorMatrix(&m,
            Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

        status = pGraihics->DrawImage(_pImg->gpImg,
            rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height, Gdiplus::Unit::UnitPixel, attr);
        ExSetLastError(status);
        return status == Gdiplus::Status::Ok;
    }
    virtual bool DrawImage(LPEX_IMAGE image,
        float dstLeft, float dstTop, float dstRight, float dstBottom,
        float srcLeft, float srcTop, float srcRight, float srcBottom,
        float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
        DWORD dwFlags, BYTE alpha)
    {
        if (!image || alpha == 0)
            return false;
        float pl = gridPaddingLeft;
        float pt = gridPaddingTop;
        float pr = gridPaddingRight;
        float pb = gridPaddingBottom;
        if ((dwFlags & BIF_DISABLESCALE) == 0)
        {

        }

        // 右-中间
        bool ret = DrawImage(image,
            dstRight - pr, dstTop + pt, dstRight, dstBottom - pb,
            srcRight - gridPaddingRight, srcTop + gridPaddingTop, srcRight, srcBottom - gridPaddingBottom, alpha);

        // 右下
        ret = DrawImage(image,
            dstRight - pr, dstBottom - pb, dstRight, dstBottom, srcRight - gridPaddingRight,
            srcBottom - gridPaddingBottom, srcRight, srcBottom, alpha);

        // 底-中间
        ret = DrawImage(image,
            dstLeft + pl, dstBottom - pb, dstRight - pr, dstBottom,
            srcLeft + gridPaddingLeft, srcBottom - gridPaddingBottom, srcRight - gridPaddingRight, srcBottom, alpha);

        // 左下
        ret = DrawImage(image,
            dstLeft, dstBottom - pb, dstLeft + pl, dstBottom,
            srcLeft, srcBottom - gridPaddingBottom, srcLeft + gridPaddingLeft, srcBottom, alpha);
        // 左-中间
        ret = DrawImage(image,
            dstLeft, dstTop + pt, dstLeft + pl, dstBottom - pb,
            srcLeft, srcTop + gridPaddingTop, srcLeft + gridPaddingLeft, srcBottom - gridPaddingBottom, alpha);
        if ((dwFlags | BIF_GRID_EXCLUSION_CENTER) == 0)  // 中间
        {
            ret = DrawImage(image,
                dstLeft + pl, dstTop + pt, dstRight - pr, dstBottom - pb,
                srcLeft + gridPaddingLeft, srcTop + gridPaddingTop, srcRight - gridPaddingRight, srcBottom - gridPaddingBottom, alpha);
        }

        // 左上
        ret = DrawImage(image,
            dstLeft, dstTop, dstLeft + pl, dstTop + pt,
            srcLeft, srcTop, srcLeft + gridPaddingLeft, srcTop + gridPaddingTop, alpha);

        // 右上
        ret = DrawImage(image,
            dstRight - pr, dstTop, dstRight, dstTop + pt,
            srcRight - gridPaddingRight, srcTop, srcRight, srcTop + gridPaddingTop, alpha);

        // 顶-中间
        ret = DrawImage(image,
            dstLeft + pl, dstTop, dstRight - pr, dstTop + pt,
            srcLeft + gridPaddingLeft, srcTop, srcRight - gridPaddingRight, srcTop + gridPaddingTop, alpha);
        return ret;
    }



    virtual bool calctextsize(LPEX_FONT font, LPCWSTR text, size_t textLen,
        DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight)
    {
        if (!font || !text || !text[0])
            return false;
        if (textLen == ((size_t)-1))
            textLen = wcslen(text);


#ifndef __USEXP // 不是xp, 使用D2D
        IDWriteTextLayout* pDWriteTextLayout = 0;
#else
        IUnknown* pDWriteTextLayout = 0;
#endif
        bool ret = _canvas_calctextsize_ex(font, text, textLen, textFormat, 0,
            layoutWidth, layoutHeight, pWidth, pHeight, (IUnknown**)&pDWriteTextLayout);
        SafeRelease(pDWriteTextLayout);

        return ret;
    }

    virtual bool drawtext(LPEX_FONT font, LPEX_BRUSH hbrText, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc)
    {
        return drawtextex(font, hbrText, text, textLen, textFormat, rc, 0, 0, 0, 0, 0);
    }
    virtual bool drawtextex(LPEX_FONT font, LPEX_BRUSH hbrText,
        LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
        DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText)
    {
        if (!text || !font || !rc || !hbrText || rc->right <= rc->left || rc->bottom <= rc->top)
            return false;

        if (textLen == 0 || textLen == (size_t)-1)
            textLen = wcslen(text);
        if (textLen == 0)return false;
        float iWidth = 0, iHeight = 0;
#ifndef __USEXP // 不是xp, 使用D2D
        IDWriteTextLayout* pDWriteTextLayout = 0;
#else
        IUnknown* pDWriteTextLayout = 0;
#endif
        if (_canvas_calctextsize_ex(font, text, textLen, textFormat, lParam,
            rc->right - rc->left, rc->bottom - rc->top, &iWidth, &iHeight, (IUnknown**)&pDWriteTextLayout))
        {
            if (outrcText)
            {
                outrcText->left = rc->left;
                outrcText->top = rc->top;
                outrcText->right = rc->left + iWidth;
                outrcText->bottom = rc->top + iHeight;
            }

#ifndef __USEXP // 不是xp, 使用D2D
            if (IsDxRender())
            {
                __EX_BRUSH* _pBrush = (__EX_BRUSH*)hbrText;
                if (_pBrush)
                {
                    D2D1_POINT_2F pt = { rc->left, rc->top };
                    pRenderTarget->DrawTextLayout(pt, (IDWriteTextLayout*)pDWriteTextLayout, _pBrush->dxbrush,
                                                  D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }

                SafeRelease(pDWriteTextLayout);
            }
            else
#endif
            {
                RECT rcTmp;
                rcTmp.left = static_cast<LONG>(rc->left);
                rcTmp.top = static_cast<LONG>(rc->top);
                rcTmp.right = static_cast<LONG>(rc->right);
                rcTmp.bottom = static_cast<LONG>(rc->bottom);

                _canvas_gdi_drawtext_buffer(font, hbrText, text, textLen, textFormat,
                    rcTmp, static_cast<DWORD>(iWidth), static_cast<DWORD>(iHeight),
                    lParam, crShadow, iGlowsize, prclayout);
            }
            return true;
        }
        return false;
    }

    virtual bool Alphablend(float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
        float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha)
    {
        if (!sCanvas)return false;
        __CANVAS* _psCanvas = (__CANVAS*)sCanvas;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            RECT_F rc1(xoriginDest, yoriginDest, xoriginDest + wDest, yoriginDest + hDest);
            RECT_F rc2(xoriginSrc, yoriginSrc, xoriginSrc + wSrc, yoriginSrc + hSrc);
            pRenderTarget->DrawBitmap(_psCanvas->pBitmap,
                (D2D_RECT_F*)&rc1,
                static_cast<float>(Alpha) / 255.0f,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                (D2D_RECT_F*)&rc2);

            return true;
        }
#endif
        BLENDFUNCTION blend = { 0 };
        blend.BlendOp = AC_SRC_OVER;
        blend.AlphaFormat = AC_SRC_ALPHA;
        blend.SourceConstantAlpha = Alpha;
        return AlphaBlend(hdc, (int)xoriginDest, (int)yoriginDest, (int)wDest, (int)hDest,
            _psCanvas->hdc, (int)xoriginSrc, (int)yoriginSrc, (int)wSrc, (int)hSrc, blend);
    }
    virtual bool bitblt(int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop)
    {
        if (!sCanvas) return false;
        __CANVAS* _psCanvas = (__CANVAS*)sCanvas;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            flush();
            D2D1_POINT_2L pt = { x, y };
            D2D1_RECT_L rc = { x1, y1, x1 + cx, y1 + cy };
            HRESULT hr = pBitmap->CopyFromBitmap((D2D1_POINT_2U*)&pt, _psCanvas->pBitmap, (D2D1_RECT_U*)&rc);
            ExSetLastError(hr);
            return SUCCEEDED(hr);
        }
#endif
        return BitBlt(hdc, x, y, cx, cy, _psCanvas->hdc, x1, y1, rop);
    }

    virtual bool SetRgn(float left, float top, float right, float bottom, int mode = 0)
    {
        return SetClip(left, top, right, bottom);
    }
    virtual bool SetClip(float left, float top, float right, float bottom, int mode = 0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            D2D1_RECT_F rcRgn = { left, top, right, bottom };
            pRenderTarget->PushAxisAlignedClip(rcRgn, D2D1_ANTIALIAS_MODE(mode));// D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
            return true;
        }
#endif
        Gdiplus::RectF rcf(left, top, right - left, bottom - top);
        Gdiplus::Status status = pGraihics->SetClip(rcf, Gdiplus::CombineMode(mode)); // Gdiplus::CombineMode::CombineModeReplace
        return status == Gdiplus::Status::Ok;
    }
    virtual bool ResetClip()
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            pRenderTarget->PopAxisAlignedClip();
            return true;
        }
#endif
        Gdiplus::Status status = pGraihics->ResetClip();
        return status == Gdiplus::Status::Ok;
    }

    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调与 SetSmoothingMode, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool SetAntialiasMode(int mode, int* oldMode = 0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            D2D1_ANTIALIAS_MODE v = (mode == 0) ? D2D1_ANTIALIAS_MODE_ALIASED : D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
            if (oldMode)
            {
                *oldMode = pRenderTarget->GetAntialiasMode();
                *oldMode = (*oldMode == D2D1_ANTIALIAS_MODE_PER_PRIMITIVE) ? 1 : 0;
            }
            pRenderTarget->SetAntialiasMode(v);
            return true;
        }
#endif
        if (mode == 0) mode = Gdiplus::SmoothingMode::SmoothingModeNone;
        else if (mode == 1) mode = Gdiplus::SmoothingMode::SmoothingModeAntiAlias;

        if (oldMode)
        {
            *oldMode = pGraihics->GetSmoothingMode();
            *oldMode = (*oldMode == Gdiplus::SmoothingMode::SmoothingModeAntiAlias) ? 1 : 0;
        }
        Gdiplus::Status status = pGraihics->SetSmoothingMode((Gdiplus::SmoothingMode)mode);
        return status == Gdiplus::Status::Ok;
    }

    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调用 SetAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool SetSmoothingMode(int mode, int* oldMode = 0)
    {
        return SetAntialiasMode(mode, oldMode);
    }

    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调与 SetTextRenderingHint, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool SetTextAntialiasMode(int mode, int* oldMode = 0)
    {
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            D2D1_TEXT_ANTIALIAS_MODE v;
            switch (mode)
            {
            case 1: v = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE; break;
            case 2: v = D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE; break;
            case 3: v = D2D1_TEXT_ANTIALIAS_MODE_ALIASED; break;
            default:v = D2D1_TEXT_ANTIALIAS_MODE_DEFAULT; break;
            }
            if (oldMode)
            {
                *oldMode = pRenderTarget->GetTextAntialiasMode();
                switch (*oldMode)
                {
                case D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE:    *oldMode = 1; break;
                case D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE:    *oldMode = 2; break;
                case D2D1_TEXT_ANTIALIAS_MODE_ALIASED:      *oldMode = 3; break;
                default:                                    *oldMode = 0; break;
                }
            }
            pRenderTarget->SetTextAntialiasMode(v);
            return true;
        }
#endif
        Gdiplus::TextRenderingHint v;
        switch (mode)
        {
        case 1: v = Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit; break;
        case 2: v = Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias; break;
        case 3: v = Gdiplus::TextRenderingHint::TextRenderingHintSystemDefault; break;
        default:v = Gdiplus::TextRenderingHint::TextRenderingHintSystemDefault; break;
        }

        if (oldMode)
        {
            *oldMode = pGraihics->GetTextRenderingHint();
            switch (*oldMode)
            {
            case Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit:
                *oldMode = 1; break;
            case Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias:
                *oldMode = 2; break;
            default:
                *oldMode = 0; break;
            }
        }
        Gdiplus::Status status = pGraihics->SetTextRenderingHint(v);
        return status == Gdiplus::Status::Ok;
    }
    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调用 SetTextAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool SetTextRenderingHint(int mode, int* oldMode = 0)
    {
        return SetTextAntialiasMode(mode, oldMode);
    }






    inline operator HDC() const { return hdc; }
    inline operator HBITMAP() const { return hBitmap; }
#ifndef __USEXP // 不是xp, 使用D2D
    inline operator ID2D1Bitmap1* () const { return pBitmap; }
    inline operator ID2D1DeviceContext* () const { return pRenderTarget; }
#endif
    inline operator Gdiplus::Graphics* () const { return pGraihics; }








private:

    inline LPWSTR prefixstring(LPCWSTR text, DWORD format, size_t*& nPreFix, int& count)
    {
        wchar_t* ret = 0;
        if ((format & DT_NOPREFIX) == 0 && text)
        {
            LPCWSTR lpOffset = wcschr(text, '&');
            if (lpOffset)
            {
                size_t len = wcslen(text) + 1;
                size_t nOffset = lpOffset - text;

                ret = new wchar_t[len];
                memset(ret, 0, sizeof(wchar_t) * len);
                count = (int)len;
                nPreFix = new size_t[len];
                memset(nPreFix, 0, sizeof(size_t) * len);
                memcpy(ret, text, nOffset * sizeof(wchar_t));
                memcpy(ret + nOffset, lpOffset + 1, (len - nOffset - 2) * sizeof(wchar_t));
                nPreFix[0] = nOffset;
                return ret;
                count = 0;
                int i = 0, n = 0;
                while (text[n])
                {
                    if (text[n] == '&' && count == 0)
                        nPreFix[count++] = n;
                    else
                        ret[i++] = text[n];
                    n++;
                }
            }

        }
        return (LPWSTR)ret;
    }

    inline void _gdi_calctext(HDC hdc, HFONT hFont, LPCWSTR text, size_t textLen,
        DWORD textFormat, LPDRAWTEXTPARAMS lParam, float layoutWidth, float layoutHeight,
        float& lpWidth, float& lpHeight)
    {
        textFormat = textFormat & ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_BOTTOM);
        RECT rc;
        rc.right = static_cast<LONG>(layoutWidth);
        rc.bottom = static_cast<LONG>(layoutHeight);
        rc.left = 0;
        rc.top = 0;
        HGDIOBJ hOldFont = SelectObject(hdc, hFont);
        if (textLen <= 0)textLen = wcslen(text);
        size_t len = textLen + 10;
        _ptrs<wchar_t> pText = new wchar_t[len];
        memset(pText, 0, len * sizeof(wchar_t));
        memcpy(pText, text, textLen * sizeof(wchar_t));
        DrawTextExW(hdc, pText, (int)textLen, &rc, textFormat | DT_CALCRECT, lParam);
        SelectObject(hdc, hOldFont);
        lpWidth = static_cast<float>(rc.right);
        lpHeight = static_cast<float>(rc.bottom);
    }


    inline bool _canvas_calctextsize_ex(LPEX_FONT font, LPCWSTR text, size_t textLen,
        DWORD textFormat, LPDRAWTEXTPARAMS lParam, float layoutWidth, float layoutHeight,
        float* pWidth, float* pHeight, IUnknown** ppDWriteTextLayout)
    {
        if (layoutWidth < 0)layoutWidth = 0;
        if (layoutHeight < 0)layoutHeight = 0;
        __EX_FONT* _pFont = (__EX_FONT*)font;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            size_t* nPreFix = 0;
            int nPreFixCount = 0;
            LPWSTR lpwzTextFix = prefixstring(text, textFormat, nPreFix, nPreFixCount);
            HRESULT hr = 0;

            IDWriteTextLayout* pDWriteTextLayout = 0;
            hr = d2dInfo.pDWriteFactory->CreateTextLayout(lpwzTextFix ? lpwzTextFix : text,
                (UINT32)textLen, _pFont->dxFormat, layoutWidth, layoutHeight, &pDWriteTextLayout);
            if (ppDWriteTextLayout)
                *ppDWriteTextLayout = pDWriteTextLayout;

            ExSetLastError(hr);
            if (FAILED(hr))return false;

            if (_pFont->logFont.lfUnderline)
            {
                DWRITE_TEXT_RANGE tmp;
                tmp.length = (UINT32)textLen;
                tmp.startPosition = 0;
                pDWriteTextLayout->SetUnderline(_pFont->logFont.lfUnderline, tmp);
            }
            if (_pFont->logFont.lfStrikeOut)
            {
                DWRITE_TEXT_RANGE tmp;
                tmp.length = (UINT32)textLen;
                tmp.startPosition = 0;
                pDWriteTextLayout->SetStrikethrough(_pFont->logFont.lfStrikeOut, tmp);
            }
            pDWriteTextLayout->SetWordWrapping(
                (textFormat & DT_SINGLELINE)
                ? DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
                : DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP);
            if ((textFormat & DT_PATH_ELLIPSIS) || (textFormat & DT_WORD_ELLIPSIS) || (textFormat & DT_END_ELLIPSIS))
            {
                if (!_pFont->pWriteInlineObject)
                {
                    hr = d2dInfo.pDWriteFactory->CreateEllipsisTrimmingSign(_pFont->dxFormat, &_pFont->pWriteInlineObject);
                    ExSetLastError(hr);
                }

                DWRITE_TRIMMING tmp;
                memset(&tmp, 0, sizeof(DWRITE_TRIMMING));
                if ((textFormat & DT_END_ELLIPSIS))
                {
                    tmp.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
                    tmp.delimiter = 0;
                    tmp.delimiterCount = 0;
                }
                else if ((textFormat & DT_WORD_ELLIPSIS))
                {
                    tmp.granularity = DWRITE_TRIMMING_GRANULARITY_WORD;
                }
                if (SUCCEEDED(hr))
                {
                    pDWriteTextLayout->SetTrimming(&tmp, _pFont->pWriteInlineObject);
                }
            }
            DWRITE_TEXT_METRICS t = { 0 };
            pDWriteTextLayout->GetMetrics(&t);
            if (pWidth)*pWidth = t.widthIncludingTrailingWhitespace;
            if (pHeight)*pHeight = t.height;
            DWRITE_TEXT_ALIGNMENT textAlignment;
            if (textFormat & DT_CENTER)
                textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER;
            else if (textFormat & DT_RIGHT)
                textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING;
            else
                textAlignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING;
            pDWriteTextLayout->SetTextAlignment(textAlignment);
            DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment;
            if (textFormat & DT_VCENTER)
                paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
            else if (textFormat & DT_BOTTOM)
                paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR;
            else
                paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
            pDWriteTextLayout->SetParagraphAlignment(paragraphAlignment);
            if (nPreFix)
            {
                DWRITE_TEXT_RANGE tmp;
                tmp.length = 1;
                tmp.startPosition = (UINT32)nPreFix[0];
                pDWriteTextLayout->SetUnderline(TRUE, tmp);
            }
            if (lpwzTextFix)
            {
                delete[] nPreFix;
                delete[] lpwzTextFix;
            }

            return SUCCEEDED(hr);
        }
#endif
        HDC hdc = GetDC();
        int err = 0;
        if (hdc == 0)
        {
            err = 1;
            hdc = ::GetDC(0);
        }
        float iWidth = 0, iHeight = 0;
        if (hdc)
        {
            _gdi_calctext(hdc, _pFont->hFont, text, textLen, textFormat, lParam, layoutWidth, layoutHeight, iWidth, iHeight);
            if (err == 0)
                ReleaseDC();
            else
                ::ReleaseDC(0, hdc);
        }
        if (pWidth)*pWidth = iWidth;
        if (pHeight)*pHeight = iHeight;
        return true;
    }

    //_declspec(naked)
    inline void  PixelFix(DWORD crFixed, LPBYTE lpBits, int nCount, BYTE sR, BYTE sG, BYTE sB)
    {
        DWORD* cr = (DWORD*)lpBits;        // 转成4字节一个像素点来处理
        for (int i = 0; i < nCount; ++i)
        {
            if (cr[i] == 0) continue;    // 等于0 表示是透明, 透明的不处理

            if (cr[i] == 0xFFFFFF)        // FF FF FF 白色, 是白色就是画上的文本颜色, 直接修改成传递进来的颜色
            {
                cr[i] = crFixed;
                continue;
            }
            LPBYTE p = (LPBYTE)(&cr[i]);
            p[3] = (BYTE)(cr[i] & 0xff);

            p[2] = ((p[3] * sR) >> 8);
            p[1] = ((p[3] * sG) >> 8);
            p[0] = ((p[3] * sB) >> 8);

        }
    }

    inline void _canvas_gdi_drawtext_buffer(LPEX_FONT font,
        const LPEX_BRUSH hbrText, LPCWSTR lpwzText, size_t textLen, DWORD textFormat,
        const RECT& rc, DWORD iWidth, DWORD iHeight,
        LPDRAWTEXTPARAMS lParam, DWORD crShadow, int iGlowsize, const RECT_F* prcLayout)
    {
        if (!hbrText || !font)return;
        __EX_FONT* _pFont = (__EX_FONT*)font;
        DWORD crText = _brush_getcolor(hbrText);
        if (ARGB_GETA(crText) == 0)return;

        // TODO 这里还有一段操作
        HRGN hRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
        int top = rc.top, left = rc.left;

        if (textFormat & DT_VCENTER)
            top = top + ((int)(rc.bottom - rc.top - (int)iHeight)) / 2;
        else if (textFormat & DT_BOTTOM)
            top = rc.bottom - iHeight;

        if (textFormat & DT_CENTER)
            left = left + (rc.right - rc.left - iWidth) / 2;
        else if (textFormat & DT_RIGHT)
            left = rc.right - iWidth;

        DWORD crMaskBkg = 0, crMaskText = RGB(255, 255, 255), crSha = ARGB_GETA(crShadow);
        DWORD crTextFixed = 0;
        float nOffset = 0;
        BYTE sR = 0, sG = 0, sB = 0, sA = 0;
        if (ARGB_GETA(crShadow) == 0 || iGlowsize == 0)
        {
            crSha = 0;
            iGlowsize = 0;
        }
        else
        {
            nOffset = static_cast<float>(iGlowsize) / 3.0f;
        }

        if (crSha == 0)
        {
            crTextFixed = crText;
            ((BYTE*)(&crTextFixed))[3] = 255;
            sA = ARGB_GETA(crText);
            sR = ARGB_GETR(crText);
            sG = ARGB_GETG(crText);
            sB = ARGB_GETB(crText);
        }
        else
        {
            crTextFixed = crSha;
            ((BYTE*)(&crTextFixed))[3] = 255;
            sA = ((BYTE*)(&crSha))[3];
            sR = ((BYTE*)(&crSha))[2];
            sG = ((BYTE*)(&crSha))[1];
            sB = ((BYTE*)(&crSha))[0];
            iWidth = iWidth + static_cast<DWORD>(nOffset) * 2;
            iHeight = iHeight + static_cast<DWORD>(nOffset) * 2;
        }
        LPMEMDCBITMAP md = md_create(iWidth, iHeight);

        bool bClearType = Flag_Query(EXGF_TEXT_CLEARTYPE);
        RECT trc;
        trc.left = static_cast<LONG>(nOffset);
        trc.top = static_cast<LONG>(nOffset);
        trc.right = iWidth - static_cast<LONG>(nOffset);
        trc.bottom = iHeight - static_cast<LONG>(nOffset);
        SetBkMode(md->hdc, TRANSPARENT);
        SetBkColor(md->hdc, 0);                     // 背景黑色
        SetTextColor(md->hdc, RGB(255, 255, 255));  // 前景白色, 通过 PixelFix() 这个函数直接修改像素点颜色

        //SetTextColor(hMD->hdc, RGB(crText.getR(), crText.getG(), crText.getB()));
        //FillRect(hMD->hdc, &trc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        //FillRect(hMD->hdc, &trc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        HGDIOBJ hOldFont = SelectObject(md->hdc, _pFont->hFont);
        if (textLen <= 0)textLen = wcslen(lpwzText);
        size_t len = textLen + 10;
        wchar_t* pText = new wchar_t[len];
        memset(pText, 0, len * sizeof(wchar_t));
        memcpy(pText, lpwzText, textLen * sizeof(wchar_t));
        DrawTextExW(md->hdc, pText, (int)textLen, &trc, textFormat, lParam);
        SelectObject(md->hdc, hOldFont);
        delete[] pText;

        if (crSha)
        {
            len = static_cast<size_t>(iWidth) * static_cast<size_t>(iHeight) * 4;
            LPBYTE buffer = new BYTE[len];
            memset(buffer, 0, len);
            memcpy(buffer, md->lpBits, len);
            PixelFix(crTextFixed, (LPBYTE)md->lpBits, iWidth * iHeight, sR, sG, sB);
            crTextFixed = crText;
            ((BYTE*)(&crTextFixed))[3] = 255;
            sA = ARGB_GETA(crText);
            sR = ARGB_GETR(crText);
            sG = ARGB_GETG(crText);
            sB = ARGB_GETB(crText);
            memcpy(md->lpBits, buffer, len);
            if (!bClearType)
                PixelFix(crTextFixed, (LPBYTE)md->lpBits, iWidth * iHeight, sR, sG, sB);
            delete[] buffer;
        }
        else
        {
            if (!bClearType)
                PixelFix(crTextFixed, (LPBYTE)md->lpBits, iWidth * iHeight, sR, sG, sB);
        }


        HDC hdc = GetDC();
        if (hdc)
        {
            BLENDFUNCTION blend;
            blend.BlendOp = AC_SRC_OVER;
            blend.BlendFlags = 0;
            blend.AlphaFormat = AC_SRC_ALPHA;
            blend.SourceConstantAlpha = sA;

            SelectClipRgn(hdc, hRgn);
            if (prcLayout)
            {
                RECT rcLay;
                //rcLay.left = static_cast<LONG>(prcLayout->left);
                //rcLay.top = static_cast<LONG>(prcLayout->top);
                //rcLay.right = static_cast<LONG>(prcLayout->right);
                //rcLay.bottom = static_cast<LONG>(prcLayout->bottom);

                rcLay.left = left - static_cast<LONG>(nOffset);
                rcLay.top = top - static_cast<LONG>(nOffset);
                rcLay.right = iWidth + left - static_cast<LONG>(nOffset);
                rcLay.bottom = iHeight + top - static_cast<LONG>(nOffset);
            }
            //BitBlt(hdc, left - static_cast<LONG>(nOffset), top - static_cast<LONG>(nOffset), iWidth, iHeight, hMD->hdc, iWidth, iHeight, SRCCOPY);
            AlphaBlend(hdc, left - static_cast<LONG>(nOffset), top - static_cast<LONG>(nOffset), iWidth, iHeight, md->hdc, 0, 0, iWidth, iHeight, blend);
            SelectClipRgn(hdc, 0);
        }
        ReleaseDC();
        DeleteObject(hRgn);
        md_destroy(md);

    }

    inline long __add_lock(int value)
    {
        return InterlockedExchangeAdd(&addlock, value);
    }
public:
    static __CANVAS* create(UINT width, UINT height, HWND hWnd = 0)
    {
        __CANVAS* hCanvas = new __CANVAS;
        hCanvas->hWnd = hWnd;

        if (!recreate(hCanvas, width, height, false))
        {
            delete hCanvas;
            hCanvas = 0;
        }
        return hCanvas;
    }

    static bool recreate(LPCANVAS hCanvas, UINT width, UINT height, bool fCopy)
    {
        if (!hCanvas)return false;
        __CANVAS* _pCanvas = (__CANVAS*)hCanvas;

        if (width == _pCanvas->width && height == _pCanvas->height && width > 0 && height > 0) return true;
        if (width <= 0)width = 1;
        if (height <= 0)height = 1;
        _pCanvas->width = width;
        _pCanvas->height = height;
#ifndef __USEXP // 不是xp, 使用D2D
        if (IsDxRender())
        {
            if (_pCanvas->pRenderTarget == 0)
            {
                HRESULT hr = d2dInfo.pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                    &_pCanvas->pRenderTarget);
                if (FAILED(hr))return false;
                _pCanvas->pRenderTarget->QueryInterface(IID_PPV_ARGS(&_pCanvas->pGDIInterop));
            }

            bool fGdi = ((_pCanvas->flags & CVF_GDI_COMPATIBLE) || IsD2DGdi());
            ID2D1Bitmap1* pBitmap = __EX_IMAGE::_dx_createbitmap(_pCanvas->pRenderTarget, width, height, fGdi);
            if (pBitmap)
            {
                SafeRelease(_pCanvas->pBitmap);
                _pCanvas->pBitmap = pBitmap;
                return true;
            }
            return false;
        }
#endif
        if (_md_create(hCanvas, width, height))
        {
            if (_pCanvas->pGraihics) delete _pCanvas->pGraihics;
            _pCanvas->pGraihics = new Gdiplus::Graphics(_pCanvas->hdc);
            if (!(d2dInfo.flags & EXGF_RENDER_CANVAS_ALIAS))    // 没有不抗锯齿这个标志
                _pCanvas->pGraihics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
        }
        return true;
    }
    
public:

    static bool _md_destroy(LPCANVAS pData)
    {
        if (!pData)return false;
        __CANVAS* _pCanvas = (__CANVAS*)pData;
        if (_pCanvas->hdc)
        {
            DeleteObject(_pCanvas->hBitmap);
            DeleteDC(_pCanvas->hdc);
            _pCanvas->hBitmap = 0;
            _pCanvas->hdc = 0;
            _pCanvas->lpBits = 0;
            return true;
        }
        return false;

    }
    static bool _md_create(LPCANVAS pData, int width, int height)
    {
        __CANVAS* _pCanvas = (__CANVAS*)pData;
        if (width <= 0)width = 1;
        if (height <= 0)height = 1;
        HDC hdc = CreateCompatibleDC(0);
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
                DeleteObject(SelectObject(hdc, hBmp));
                _md_destroy(pData);
                _pCanvas->hdc = hdc;
                _pCanvas->hBitmap = hBmp;
                _pCanvas->lpBits = (LPBYTE)lpBits;
                return true;
            }
            else
            {
                ExSetLastError(GetLastError());
                DeleteDC(hdc);
            }
        }
        return false;
    }

};












































DLLAPIBOOL _canvas_destroy(LPCANVAS& hCanvas)
{
    if (!hCanvas)return false;
    bool ret = hCanvas->Release() == 0;
    hCanvas = 0;
    return ret;
}

DLLAPICANVAS _canvas_create(HWND hWnd, UINT width, UINT height)
{
    LPCANVAS hCanvas = __CANVAS::create(width, height, hWnd);
    return hCanvas;
}


DLLAPIBOOL _canvas_uninit()
{
    if (IsDxRender())
    {
        _dx_uninit();
        delete d2dInfo.pMatrix;
    }
    Gdiplus::GdiplusShutdown(d2dInfo.uToken);
    CoUninitialize();
    return true;
}


//DLLAPIBOOL _brush_GetradientStopCollection(LPEX_BRUSH brush)
//{
//	if (!brush)return false;
//	if (!IsDxRender())return false;	// gdi+不支持此操作
//	if (brush->type != BRUSHTYPE_LINEAR)
//	{
//		// 画刷类型正确, 不支持此功能
//		return false;
//	}
//	ID2D1GradientStopCollection* a;
//	brush->LinearBrush->GetGradientStopCollection(&a);
//}




// 绘制相关
DLLAPIBOOL _canvas_begindraw(const LPCANVAS hCanvas)
{
    if (!hCanvas)return false;
    return hCanvas->BeginDraw();
}

DLLAPIBOOL _canvas_enddraw(const LPCANVAS hCanvas)
{
    if (!hCanvas)return false;
    return hCanvas->EndDraw();
}

DLLAPIBOOL _canvas_resize(const LPCANVAS hCanvas, int cx, int cy)
{
    if (!hCanvas)return false;
    return hCanvas->Resize(cx, cy);
}

DLLAPIBOOL _canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height)
{
    if (!hCanvas)return false;
    return hCanvas->GetSize(width, height);
}

DLLAPITYPE(HDC) _canvas_getdc(const LPCANVAS hCanvas)
{
    if (!hCanvas)return 0;
    return hCanvas->GetDC();
}
DLLAPITYPE(void) _canvas_releasedc(const LPCANVAS hCanvas)
{
    if (!hCanvas)return;
    return hCanvas->ReleaseDC();
}

DLLAPIBOOL _canvas_clear(const LPCANVAS hCanvas, ARGB argb)
{
    if (!hCanvas)return false;
    return hCanvas->clear(argb);
}

DLLAPIBOOL _canvas_flush(const LPCANVAS hCanvas)
{
    if (!hCanvas)return false;
    return hCanvas->flush();
}

DLLAPIBOOL _canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap)
{
    if (!hCanvas)return false;
    return hCanvas->SetTarget(bitmap);
}

DLLAPIBOOL _canvas_ReTarget(const LPCANVAS hCanvas)
{
    if (!hCanvas)return false;
    return hCanvas->ReTarget();
}

DLLAPITYPE(void) _canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image)
{
    if (!hCanvas || !image)return;
    hCanvas->FreeTarget(image);
}

DLLAPITYPE(LPEX_IMAGE) _canvas_GetTarget(const LPCANVAS hCanvas)
{
    if (!hCanvas)return 0;
    return hCanvas->GetTarget();
}


// 平移
// x, y = 平移的横向纵向坐标
DLLAPIBOOL _canvas_translate(const LPCANVAS hCanvas, float x, float y)
{
    if (!hCanvas)return false;
    return hCanvas->translate(x, y);
}

// 缩放
// x, y = 缩放的坐标点
// scaleX, scaleY = 缩放的横向纵向比例
DLLAPIBOOL _canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY)
{

    if (!hCanvas)return false;
    return hCanvas->scale(x, y, scaleX, scaleY);
}

// 旋转
// x, y = 旋转的坐标点
// rotate = 旋转的角度, 负数则反向旋转
DLLAPIBOOL _canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate)
{
    if (!hCanvas)return false;
    return hCanvas->rotate(x, y, rotate);
}



// 绘制简单图形

DLLAPIBOOL _canvas_drawRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECT_F rcF)
{
    if (!hPen || !hCanvas)return false;
    return hCanvas->DrawRectangle(hPen, rcF);
}

DLLAPIBOOL _canvas_fillRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECT_F rcF)
{
    if (!hBrush || !hCanvas)return false;
    return hCanvas->FillRectangle(hBrush, rcF);
}

DLLAPIBOOL _canvas_drawEllipse(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCELLIPSE_F rcF)
{
    if (!hPen || !hCanvas)return false;
    return hCanvas->DrawEllipse(hPen, rcF);
}

DLLAPIBOOL _canvas_fillEllipse(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF)
{
    if (!hBrush || !hCanvas)return false;
    return hCanvas->FillEllipse(hBrush, rcF);

}

DLLAPIBOOL _canvas_drawRoundedRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded)
{
    if (!hPen || !hCanvas)return false;
    return hCanvas->DrawRoundedRectangle(hPen, rcRounded);
}

DLLAPIBOOL _canvas_fillRoundedRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded)
{
    if (!hBrush || !hCanvas)return false;
    return hCanvas->FillRoundedRectangle(hBrush, rcRounded);
}

DLLAPIBOOL _canvas_drawLine(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCLINEPOINT_F pt)
{
    if (!hPen || !hCanvas)return false;
    return hCanvas->DrawLine(hPen, pt);
}



DLLAPIBOOL _canvas_drawpath(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPEX_PATH path)
{
    if (!hPen || !hCanvas || !path)return false;
    return hCanvas->DrawPath(hPen, path);
}

DLLAPIBOOL _canvas_fillpath(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPEX_PATH path)
{
    if (!hBrush || !hCanvas || !path)return false;
    return hCanvas->FillPath(hBrush, path);
}


DLLAPIBOOL _canvas_drawimagerectrect(const LPCANVAS hCanvas, LPEX_IMAGE image,
    float dstLeft, float dstTop, float dstRight, float dstBottom,
    float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha)
{
    if (!hCanvas || !image) return false;
    return hCanvas->DrawImage(image, dstLeft, dstTop, dstRight, dstBottom, srcLeft, srcTop, srcRight, srcBottom, alpha);
}


DLLAPIBOOL _canvas_drawimage(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, BYTE alpha)
{
    if (!hCanvas || !image)return false;
    return hCanvas->DrawImage(image, left, top, alpha);
}
DLLAPIBOOL _canvas_drawimagerect(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha)
{
    if (!hCanvas || !image)return false;
    __EX_IMAGE* _pImg = (__EX_IMAGE*)image;
    return hCanvas->DrawImage(image,
        left, top, right, bottom,
        0, 0, static_cast<float>(_pImg->width), static_cast<float>(_pImg->height), alpha);
}

DLLAPIBOOL _canvas_drawimagegridPadding(const LPCANVAS hCanvas, LPEX_IMAGE image,
    float dstLeft, float dstTop, float dstRight, float dstBottom,
    float srcLeft, float srcTop, float srcRight, float srcBottom,
    float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
    DWORD dwFlags, BYTE alpha)
{
    if (!hCanvas || !image || alpha == 0)
        return false;
    return hCanvas->DrawImage(image, dstLeft, dstTop, dstRight, dstBottom,
        srcLeft, srcTop, srcRight, srcBottom,
        gridPaddingLeft, gridPaddingTop, gridPaddingRight, gridPaddingBottom,
        dwFlags, alpha);
}



DLLAPITYPE(bool) _canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen,
    DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight)
{
    if (!hCanvas || !font || !text || !text[0])
        return false;
    return hCanvas->calctextsize(font, text, textLen, textFormat, layoutWidth, layoutHeight, pWidth, pHeight);
}


DLLAPIBOOL _canvas_drawtextex(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hbrText,
    LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
    DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText)
{
    if (!text || !hCanvas || !font || !rc || !hbrText || rc->right <= rc->left || rc->bottom <= rc->top)return false;
    return hCanvas->drawtextex(font, hbrText, text, textLen, textFormat, rc, iGlowsize, crShadow, lParam, prclayout, outrcText);
}

DLLAPIBOOL _canvas_drawtext(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hbrText, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc)
{
    if (!text || !hCanvas || !font || !rc || !hbrText || rc->right <= rc->left || rc->bottom <= rc->top)return false;
    return hCanvas->drawtext(font, hbrText, text, textLen, textFormat, rc);
}


DLLAPIBOOL _canvas_bitblt(const LPCANVAS hCanvas, int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop)
{
    if (!hCanvas || !sCanvas)return false;
    return hCanvas->bitblt(x, y, cx, cy, sCanvas, x1, y1, rop);
}

DLLAPIBOOL _canvas_alphablend(const LPCANVAS hCanvas,
    float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
    float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha)
{
    if (!hCanvas || !sCanvas)return false;
    return hCanvas->Alphablend(xoriginDest, yoriginDest, wDest, hDest, sCanvas, xoriginSrc, yoriginSrc, wSrc, hSrc, Alpha);
}




DLLAPIBOOL _canvas_setrgn(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode)
{
    return hCanvas->SetRgn(left, top, right, bottom, mode);
}
DLLAPIBOOL _canvas_setClip(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode)
{
    if (!hCanvas)return false;
    return hCanvas->SetClip(left, top, right, bottom, mode);

}

DLLAPIBOOL _canvas_resetClip(const LPCANVAS hCanvas)
{
    if (!hCanvas)return false;
    return hCanvas->ResetClip();
}

// 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
// 此函数调与 _canvas_SetSmoothingMode, 功能一模一样, 多加一个是为了和d2d名字一样
DLLAPIBOOL _canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!hCanvas)return false;
    return hCanvas->SetAntialiasMode(mode, oldMode);
}


// 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
// 此函数调用 _canvas_SetAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
DLLAPIBOOL _canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!hCanvas)return false;
    return hCanvas->SetSmoothingMode(mode, oldMode);
}

// 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
// 此函数调与 _canvas_SetTextRenderingHint, 功能一模一样, 多加一个是为了和d2d名字一样
DLLAPIBOOL _canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!hCanvas)return false;
    return hCanvas->SetTextAntialiasMode(mode, oldMode);
}

// 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
// 此函数调用 _canvas_SetTextAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
DLLAPIBOOL _canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!hCanvas)return false;
    return hCanvas->SetTextRenderingHint(mode, oldMode);
}

DLLAPIBRUSH _brush_create(ARGB argb)
{
    return __EX_BRUSH::create(argb);
}

// 创建渐变画刷
DLLAPIBRUSH _brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount)
{
    return __EX_BRUSH::create(pt1, pt2, color, colorCount, fillMode, pRatios, ratiosCount);
}

// pos = 包含中心点以及向外扩展的长度
DLLAPIBRUSH _brush_create_RadialGradient(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount)
{
    return __EX_BRUSH::create(pos, color, colorCount, fillMode, pRatios, ratiosCount);
}

DLLAPIBOOL _brush_destroy(LPEX_BRUSH& pBrush)
{
    if (!pBrush)return false;
    bool ret = pBrush->Release() == 0;
    pBrush = 0;
    return ret;
}

DLLAPIBOOL _brush_setcolor(LPEX_BRUSH brush, ARGB color)
{
    if (!brush)return false;
    return brush->SetColor(color);
}

DLLAPITYPE(DWORD) _brush_getcolor(LPEX_BRUSH brush)
{
    if (!brush)return false;
    return brush->GetColor();
}

#ifndef __USEXP // 不是xp, 使用D2D
DLLAPIBOOL _brush_setstartpoint(LPEX_BRUSH brush, float x, float y)
{
    if (!brush)return false;
    return brush->SetStartPoint(x, y);

}
DLLAPIBOOL _brush_setendpoint(LPEX_BRUSH brush, float x, float y)
{
    if (!brush)return false;
    return brush->SetEndPoint(x, y);
}
DLLAPIBOOL _brush_getstartpoint(LPEX_BRUSH brush, float* x, float* y)
{
    if (!brush)return false;
    return brush->GetStartPoint(x, y);
}
DLLAPIBOOL _brush_getendpoint(LPEX_BRUSH brush, float* x, float* y)
{
    if (!brush)return false;
    return brush->GetEndPoint(x, y);
}
// 设置渐变画刷中心坐标
DLLAPIBOOL _brush_setcenter(LPEX_BRUSH brush, float x, float y)
{
    if (!brush)return false;
    return brush->SetCenter(x, y);
}
DLLAPIBOOL _brush_GetCenter(LPEX_BRUSH brush, float* x, float* y)
{
    if (!brush)return false;
    return brush->GetCenter(x, y);
}
// 置渐变原点偏移
DLLAPIBOOL _brush_SetGradientOriginOffset(LPEX_BRUSH brush, float x, float y)
{
    if (!brush)return false;
    return brush->SetGradientOriginOffset(x, y);
}
DLLAPIBOOL _brush_GetGradientOriginOffset(LPEX_BRUSH brush, float* x, float* y)
{
    if (!brush)return false;
    return brush->GetGradientOriginOffset(x, y);
}
DLLAPIBOOL _brush_SetRadiusX(LPEX_BRUSH brush, float x)
{
    if (!brush)return false;
    return brush->SetRadiusX(x);
}
DLLAPIBOOL _brush_SetRadiusY(LPEX_BRUSH brush, float y)
{
    if (!brush)return false;
    return brush->SetRadiusY(y);
}

DLLAPITYPE(float) _brush_GetRadiusX(LPEX_BRUSH brush)
{
    if (!brush)return false;
    return brush->GetRadiusX();
}
DLLAPITYPE(float) _brush_GetRadiusY(LPEX_BRUSH brush)
{
    if (!brush)return false;
    return brush->GetRadiusY();
}
#endif




DLLAPIPEN _pen_create(ARGB color, float width, int style, const float* dashes, int dashesCount)
{
    return __EX_PEN::create(color, width, style, dashes, dashesCount);
}

DLLAPIBOOL _pen_destroy(LPEX_PEN& pen)
{
    if (!pen)return false;
    bool ret = pen->Release() == 0;
    pen = 0;
    return ret;
}

DLLAPIBOOL _pen_setcolor(LPEX_PEN pen, ARGB color)
{
    if (!pen)return false;
    return pen->SetColor(color);
}
DLLAPITYPE(DWORD) _pen_getcolor(LPEX_PEN pen)
{
    if (!pen)return false;
    return pen->GetColor();
}

DLLAPIBOOL _pen_SetWidth(LPEX_PEN pen, float width)
{
    if (!pen)return false;
    return pen->SetWidth(width);
}

DLLAPITYPE(float) _pen_GetWidth(LPEX_PEN pen)
{
    if (!pen)return false;
    return pen->GetWidth();
}

// 设置画笔线条样式
// style = 0=实线, 1=划线{2, 2}, 2=点线{0, 2}, 3=点划线{2, 2, 0, 2}, 4=点点划线{2, 2, 0, 2, 0, 2}, 5=自定义
//      Gdiplus::DashStyle 或者 D2D1_DASH_STYLE 这两个枚举常量的成员
// dashes = 点线的数组, 划线={2,2}, 点线={0,2}, 点划线={2,2,0,2}, 点点划线={2,2,0,2,0,2}, 其他请自定义
// dashesCount = 数组成员数
DLLAPIBOOL _pen_SetDashStyle(LPEX_PEN pen, int style, float* dashes, int dashesCount)
{
    if (!pen)return false;
    return pen->SetDashStyle(style, dashes, dashesCount);
}

//// 设置线条和空格的阵列
//// dashes = 点线的数组, 划线={2,2}, 点线={0,2}, 点划线={2,2,0,2}, 点点划线={2,2,0,2,0,2}, 其他请自定义
//// dashesCount = 数组成员数
//DLLAPITYPE(float) _pen_SetDashPattern(LPEX_PEN pen, float* dashes, int dashesCount)
//{
//    if (!pen)return false;
//
//    if (IsDxRender())
//    {
//        if (pen->strokeStyle)   // 已经存在就销毁后重新创建
//            SafeRelease(pen->strokeStyle);
//
//        _pen_get_strokeStyle(pen, style, dashes, dashesCount);
//        return pen->width;
//        return true;
//    }
//    return pen->pen->SetDashStyle();
//}








DLLAPIPATH _path_create(int fillMode)
{
    __EX_PATH* path = new __EX_PATH;
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        if (__EX_PATH::_path_reset_dx(path))
            return path;
        return 0;
    }
#endif
    path->gpPath = new Gdiplus::GraphicsPath((Gdiplus::FillMode)fillMode);
    return path;
}

DLLAPIPATH _path_createroundedrectangle(const LPCRECTROUNDED_F rcRounded, int fillMode)
{
    __EX_PATH* path = new __EX_PATH;
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        d2dInfo.pFactory->CreateRoundedRectangleGeometry((D2D1_ROUNDED_RECT*)rcRounded, &path->pRoundedRectangleGeometry);
        return path;
    }
#endif
    path->gpPath = __createRoundedrect(
        rcRounded->left, rcRounded->top,
        rcRounded->right - rcRounded->left,
        rcRounded->bottom - rcRounded->top, rcRounded->radiusX, rcRounded->radiusY,
        (Gdiplus::FillMode)fillMode);
    return path;
}

DLLAPIPATH _path_createrectangle(const LPCRECT_F rc, int fillMode)
{
    __EX_PATH* path = new __EX_PATH;
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        d2dInfo.pFactory->CreateRectangleGeometry((D2D1_RECT_F*)rc, &path->pRectangleGeometry);
        return path;
    }
#endif
    path->gpPath = new Gdiplus::GraphicsPath((Gdiplus::FillMode)fillMode);
    path->gpPath->AddRectangle(*((Gdiplus::RectF*)rc));
    return path;
}

DLLAPIBOOL _path_destroy(LPEX_PATH& path)
{
    if (!path)return false;
    bool ret = path->Release() == 0;
    path = 0;
    return ret;
}

DLLAPIBOOL _path_beginfigure(LPEX_PATH path, const LPCPOINT_F pt)
{
    if (!path)return false;
    return path->BeginFigure(pt);
}

DLLAPIBOOL _path_endfigure(LPEX_PATH path)
{
    if (!path)return false;
    return path->EndFigure();
}

DLLAPIBOOL _path_reset(LPEX_PATH path)
{
    if (!path)return false;
    return path->Reset();
}

DLLAPIBOOL _path_addarc(LPEX_PATH path, float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise)
{
    if (!path)return false;
    return path->AddArc(x1, y1, x2, y2, radiusX, radiusY, fClockwise);
}

DLLAPIBOOL _path_addbezier(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4)
{
    if (!path)return false;
    return path->AddBezier(pt1, pt2, pt3, pt4);
}

DLLAPIBOOL _path_addline(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2)
{
    if (!path)return false;
    return path->AddLine(pt1, pt2);
}

DLLAPIBOOL _path_addrect(LPEX_PATH path, const RECT_F& rc)
{
    if (!path)return false;
    return path->AddRect(rc);
}

DLLAPIBOOL _path_addroundedrect(LPEX_PATH path, const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight)
{
    if (!path)return false;
    return path->AddRoundedRect(rc, radiusTopLeft, radiusTopRight, radiusBottomLeft, radiusBottomRight);
}

DLLAPIBOOL _path_open(LPEX_PATH path)
{
    if (!path)return false;
    return path->Open();
}
DLLAPIBOOL _path_close(LPEX_PATH path)
{
    if (!path)return false;
    return path->Close();
}

// 取路径边界矩形
DLLAPIBOOL _path_getbounds(LPEX_PATH path, LPRECT_F rc)
{
    if (!path)return false;
    return path->GetBounds(rc);
}

// 测试坐标是否在可见路径内
DLLAPIBOOL _path_hittest(LPEX_PATH path, const POINT_F& pt)
{
    if (!path)return false;
    return path->Hittest(pt);
}














DLLAPIBOOL _img_destroy(LPEX_IMAGE& img)
{
    if (!img)return false;
    bool ret = img->Release() == 0;
    img = 0;
    return ret;
}
DLLAPIBOOL _img_getsize(LPEX_IMAGE image, UINT* width, UINT* height)
{
    if (!image)return false;
    return image->GetSize(width, height) != 0;
}
DLLAPITYPE(UINT) _img_width(LPEX_IMAGE image)
{
    if (!image) return 0;
    return image->GetWidth();
}
DLLAPITYPE(UINT) _img_height(LPEX_IMAGE image)
{
    if (!image) return 0;
    return image->GetHeight();
}



DLLAPIIMAGE _img_create_fromstream(IStream* stream)
{
    __EX_IMAGE* hImg = 0;
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        IWICBitmapDecoder* pDecoder;    // WIC位图解码器
        // 从流里创建WIC位图解码器
        HRESULT hr = d2dInfo.pWICFactory->CreateDecoderFromStream(stream, 0,
                                                                  WICDecodeOptions::WICDecodeMetadataCacheOnLoad,
                                                                  &pDecoder);
        ExSetLastError(hr);
        if (FAILED(hr))return 0;
        hImg = __EX_IMAGE::_wic_init_from_decoder(pDecoder);
        if (hImg && hImg->width == 0)
            hImg->dxImg->GetSize(&hImg->width, &hImg->height);
    }
    else
#endif
    {
        Gdiplus::Bitmap* pBmp = new Gdiplus::Bitmap(stream);
        if (!pBmp) return 0;
        hImg = __EX_IMAGE::_img_init(pBmp, 0, 1, 0);
        if (hImg && hImg->width == 0)
        {
            hImg->width = hImg->gpImg->GetWidth();
            hImg->height = hImg->gpImg->GetHeight();
        }
    }
    if (hImg)
    {
        __EX_IMAGE::_apng_int(hImg, stream);
    }
    return hImg;
}

DLLAPIIMAGE _img_create(UINT width, UINT height)
{
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        return __EX_IMAGE::_wic_create(width, height, GUID_WICPixelFormat32bppPBGRA);
    }
#endif
    Gdiplus::Bitmap* hImg = new Gdiplus::Bitmap(width, height);
    return __EX_IMAGE::_img_init(hImg, 0, 1, 0);
}
DLLAPIIMAGE _img_create_frommemory(const void* pData, DWORD len)
{
    comptr<IStream> stream = __EX_IMAGE::_img_createfromstream_init(pData, len);
    if (stream)
        return _img_create_fromstream(stream);
    return 0;
}
DLLAPIIMAGE _img_create_fromfile(LPCWSTR file)
{
    if (!file || !file[0]) return 0;
    FILE* f;
    void* p = 0;
    errno_t err = _wfopen_s(&f, file, L"rb+");
    if (!f)
        return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if ((p = malloc(size)))
        size = (long)fread_s(p, size, 1, size, f);

    fclose(f);
    LPEX_IMAGE hImg = _img_create_frommemory(p, size);
    ::free(p);
    return hImg;
}
DLLAPIIMAGE _img_create_frombitmap(HBITMAP hBitmap, HPALETTE hPalette, bool fPreAlpha)
{
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        IWICBitmap* pBmp;
        HRESULT hr = d2dInfo.pWICFactory->CreateBitmapFromHBITMAP(hBitmap,
                                                                  hPalette,
                                                                  fPreAlpha ? WICBitmapUseAlpha : WICBitmapUsePremultipliedAlpha,
                                                                  &pBmp);
        ExSetLastError(hr);
        if (FAILED(hr))return 0;
        return __EX_IMAGE::_img_init(pBmp, 0, 1, 0);
    }
#endif
    Gdiplus::Bitmap* pBmp = __EX_IMAGE::_gp_img_createfromhbitmap(hBitmap, hPalette, fPreAlpha ? PixelFormat32bppARGB : PixelFormat32bppPARGB);
    if (!pBmp)
        return 0;
    return __EX_IMAGE::_img_init(pBmp, 0, 1, 0);
}
DLLAPIIMAGE _img_create_formicon(HICON hIcon)
{
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        IWICBitmap* pBmp;
        HRESULT hr = d2dInfo.pWICFactory->CreateBitmapFromHICON(hIcon, &pBmp);
        ExSetLastError(hr);
        if (FAILED(hr))return 0;

        return __EX_IMAGE::_img_init(pBmp, 0, 1, 0);
    }
#endif
    Gdiplus::Bitmap* pBmp = __EX_IMAGE::_gp_img_createfromhicon(hIcon);
    return __EX_IMAGE::_img_init(pBmp, 0, 1, 0);
}

DLLAPIIMAGE _bitmap_create(UINT width, UINT height, bool fGdi)
{
    __EX_IMAGE* p = new __EX_IMAGE;

#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        ID2D1Bitmap1* pBitmap;
        pBitmap = __EX_IMAGE::_dx_createbitmap(d2dInfo.pD2DDeviceContext, width, height, fGdi);
        if (pBitmap)
        {
            p->height = height;
            p->width = width;
            p->d2dBitmap = pBitmap;
            return p;
        }
    }
#endif
    Gdiplus::Bitmap* pBmp = new Gdiplus::Bitmap(width, height);
    p->height = height;
    p->width = width;
    p->gpImg = pBmp;
    return p;
}

DLLAPIIMAGE _img_copyrect(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height)
{
    if (!image)return 0;
    return image->copy(x, y, width, height);
}

DLLAPIIMAGE _img_copy(LPEX_IMAGE image)
{
    if (!image)return 0;
    return image->copy();
}

// flags = 1=读, 2=写, 3=读写
DLLAPIBOOL _img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData)
{
    if (!image || !lockData)return false;
    return image->lock(x, y, width, height, flags, lockData);
}
DLLAPIBOOL _img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData)
{
    if (!image || !lockData)return false;
    return image->unlock(lockData);
}

// 取图像帧数
DLLAPITYPE(UINT) _img_getframecount(LPEX_IMAGE image)
{
    if (image)
        return image->GetFrameCount();
    return 0;
}
// 设置当前活动帧
DLLAPIBOOL _img_selectactiveframe(LPEX_IMAGE image, UINT index)
{
    if (!image)return false;
    return image->SelectActiveFrame(index);
}


#ifndef __USEXP // 不是xp, 使用D2D
DLLAPITYPE(LPBYTE) _img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom)
{
    if (pSize) *pSize = 0;
    if (!pBitmap) return 0;
    if (right == 0 || bottom == 0)
    {
        D2D1_SIZE_F si = ((ID2D1Bitmap*)pBitmap)->GetSize();
        right = (UINT)si.width;
        bottom = (UINT)si.height;
    }
    SIZE_T size = 0;
    if (!IsDxRender())
    {
        // GDI+不支持此操作
        return 0;
    }
    if (!pBitmap)return 0;
    comptr<IWICImageEncoder> pImgEncoder;
    comptr<IWICStream> pWICStream;
    comptr<IStream> stream;
    comptr<IWICBitmapEncoder> pEncoder;
    comptr<IWICBitmapFrameEncode> pFrame;

    // 1. 创建图像编码器
    HRESULT hr = d2dInfo.pWICFactory->CreateImageEncoder(d2dInfo.pD2DDevice, &pImgEncoder);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 2. 创建流
    hr = d2dInfo.pWICFactory->CreateStream(&pWICStream);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 3. 创建编码器
    hr = d2dInfo.pWICFactory->CreateEncoder(GUID_ContainerFormatPng, 0, &pEncoder);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 4. 初始化流
    hr = CreateStreamOnHGlobal(0, TRUE, &stream);
    if (FAILED(ExSetLastError(hr)))return 0;
    hr = pWICStream->InitializeFromIStream(stream);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 5. 初始化编码器
    hr = pEncoder->Initialize(stream, WICBitmapEncoderCacheOption::WICBitmapEncoderNoCache);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 6. 创建新帧
    hr = pEncoder->CreateNewFrame(&pFrame, 0);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 7. 初始化新帧
    hr = pFrame->Initialize(0);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 8. 写入帧
    WICImageParameters imgParams;
    imgParams.PixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    imgParams.PixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    imgParams.Left = (FLOAT)left;
    imgParams.Top = (FLOAT)top;
    imgParams.PixelWidth = right - left;
    imgParams.PixelHeight = bottom - top;
    imgParams.DpiX = d2dInfo.DpiX;
    imgParams.DpiY = d2dInfo.DpiY;
    hr = pImgEncoder->WriteFrame(pBitmap, pFrame, &imgParams);
    if (FAILED(ExSetLastError(hr)))return 0;
    // 9. 提交
    hr = pFrame->Commit();
    if (FAILED(ExSetLastError(hr)))return 0;
    hr = pEncoder->Commit();
    if (FAILED(ExSetLastError(hr)))return 0;

    // 10. 从流里取出数据
    HGLOBAL hMem = 0;
    hr = GetHGlobalFromStream(stream, &hMem);
    if (FAILED(ExSetLastError(hr)))return 0;

    void* pData = GlobalLock(hMem);
    int err = GetLastError();
    if (!pData)return 0;

    size = GlobalSize(hMem);
    LPBYTE ret = (LPBYTE)malloc(size);
    if (ret)memcpy(ret, pData, size);
    GlobalUnlock(hMem);
    if (pSize) *pSize = size;

    return ret;
}
#endif


DLLAPITYPE(LPBYTE) _img_save(LPEX_IMAGE image, SIZE_T* pSize)
{
    return _img_save_rect(image, pSize, 0, 0, 0, 0);
}


static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            ::free(pImageCodecInfo);
            return j;  // Success
        }
    }

    ::free(pImageCodecInfo);
    return -1;  // Failure
}

static LPBYTE _gp_savetobin(const __HIMAGE_GP_DX& pObj, SIZE_T& size)
{
    IStream* stream;
    LPBYTE ret = 0;
    HRESULT hr = CreateStreamOnHGlobal(0, 0, &stream);
    if (SUCCEEDED(hr))
    {
        CLSID pngClsid;
        GetEncoderClsid(L"image/png", &pngClsid);
        Gdiplus::Status status = pObj.gpImg->Save(stream, &pngClsid);
        if (status == Gdiplus::Status::Ok)
        {
            HGLOBAL hMem;
            hr = GetHGlobalFromStream(stream, &hMem);
            if (SUCCEEDED(hr))
            {
                void* pData = GlobalLock(hMem);
                if (pData)
                {
                    size = GlobalSize(hMem);
                    ret = (LPBYTE)malloc(size);
                    if (!ret) throw;
                    memcpy(ret, pData, size);
                    GlobalUnlock(hMem);
                }
            }
        }
        stream->Release();
    }
    ExSetLastError(hr);
    return ret;
}
DLLAPITYPE(LPBYTE) _img_save_rect(LPEX_IMAGE image, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom)
{
    if (!image)return 0;
    SIZE_T size = 0;
    __EX_IMAGE* pImage = (__EX_IMAGE*)image;
    if (!right || !bottom)
    {
        UINT cx = 0, cy = 0;
        pImage->GetSize(&cx, &cy);
        if (!right)
            right = cx;
        if (!bottom)
            bottom = cy;
    }

    const int width = (int)(right - left);
    const int height = (int)(bottom - top);
    if(width <= 0 || height <= 0)
        return 0;   // 宽度或者高度为0, 不保存

#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        if (!pImage->d2dBitmap && d2dInfo.pD2DDeviceContext)
        {
            HRESULT hr = d2dInfo.pD2DDeviceContext->CreateBitmapFromWicBitmap(pImage->dxImg, &pImage->d2dBitmap);
            ExSetLastError(hr);
            if (FAILED(hr))return 0;
        }

        LPBYTE ret = _img_save_frombitmap(pImage->d2dBitmap, &size, left, top, right, bottom);
        if (pSize)*pSize = size;
        return ret;
    }
#endif

    Gdiplus::Bitmap hImg(width, height);
    Gdiplus::Graphics gp(&hImg);
    Gdiplus::Rect rc;
    rc.X = (int)left;
    rc.Y = (int)top;
    rc.Width = width;
    rc.Height = height;
    gp.DrawImage(pImage->gpImg, rc, rc.X, rc.Y, rc.Width, rc.Height, Gdiplus::UnitPixel);

    LPBYTE ret = _gp_savetobin(&hImg, size);
    
    if (pSize)*pSize = size;
    return ret;
}

DLLAPITYPE(LPBYTE) _img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T* pSize)
{
    return _img_save_formcanvasex(hCanvas, pSize, 0, 0, 0, 0);
}

DLLAPITYPE(LPBYTE) _img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom)
{
    if (!hCanvas) return 0;
    __CANVAS* _pCanvas = (__CANVAS*)hCanvas;

    if (!right || !bottom)
    {
        UINT cx = 0, cy = 0;
        hCanvas->GetSize(&cx, &cy);
        if (!right)
            right = cx;
        if (!bottom)
            bottom = cy;
    }

    const int width = (int)(right - left);
    const int height = (int)(bottom - top);
    if (width <= 0 || height <= 0)
        return 0;   // 宽度或者高度为0, 不保存

#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
        return _img_save_frombitmap(_pCanvas->pBitmap, pSize, left, top, right, bottom);
#endif

    Gdiplus::Bitmap* pBmp = __EX_IMAGE::_gp_img_createfromhbitmap(_pCanvas->hBitmap, 0, PixelFormat32bppARGB);
    if (!pBmp)
        return 0;
    Gdiplus::Bitmap hImg(width, height);
    Gdiplus::Graphics gp(&hImg);
    Gdiplus::Rect rc;
    rc.X = (int)left;
    rc.Y = (int)top;
    rc.Width = width;
    rc.Height = height;
    gp.DrawImage(pBmp, rc, rc.X, rc.Y, rc.Width, rc.Height, Gdiplus::UnitPixel);
    SIZE_T size = 0;
    LPBYTE ret = _gp_savetobin(&hImg, size);

    if (pSize)*pSize = size;

    delete pBmp;
    return ret;
}

DLLAPITYPE(HBITMAP) _img_ToGdiBitmap(LPEX_IMAGE image)
{
    if (!image) return 0;
    return image->ToGdiBitmap();
}

DLLAPITYPE(void) _img_free(LPBYTE ptr)
{
    if (ptr)
        ::free(ptr);
}















DLLAPIBOOL _font_destroy(LPEX_FONT& font)
{
    if (!font)return false;
    bool ret = font->Release() == 0;
    font = 0;
    return ret;
}

DLLAPIFONT _font_create_logfont(const LPLOGFONTW logFont, DWORD flags)
{
    if (!logFont)return 0;
    __EX_FONT* font = new __EX_FONT;

    //if ((flags & EFF_DISABLEDPISCALE) == 0)
    //{
    //	logFont.lfHeight *= DpiY;
    //}
    font->count = 1;
    font->flags = flags;
    memcpy(&font->logFont, logFont, sizeof(LOGFONTW));
    if (logFont->lfWeight == 0)
        logFont->lfWeight = FW_NORMAL;

#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        static wchar_t pLocalName[260];
        if (pLocalName[0] == 0)
            GetUserDefaultLocaleName(pLocalName, 260);
        int lfHeight = (logFont->lfHeight > 0) ? logFont->lfHeight : (logFont->lfHeight * -1);
        HRESULT hr = d2dInfo.pDWriteFactory->CreateTextFormat(
            logFont->lfFaceName,
            0,
            (DWRITE_FONT_WEIGHT)logFont->lfWeight,
            (DWRITE_FONT_STYLE)logFont->lfItalic,
            DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
            static_cast<FLOAT>(lfHeight),
            pLocalName,
            &font->dxFormat);
        ExSetLastError(hr);
        if (FAILED(hr))
        {
            delete font;
            return 0;
        }
        font->hFont = CreateFontIndirectW(logFont);
        return font;
    }
#endif
    HDC hdc = ::GetDC(0);
    font->gpFont = new Gdiplus::Font(hdc, logFont);
    font->hFont = CreateFontIndirectW(logFont);
    ::ReleaseDC(0, hdc);
    return font;
}
DLLAPIFONT _font_create(LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle)
{
    LOGFONTW logFont = { 0 };
    SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), &logFont, 0);
    // 判断 是否启用dpi缩放,logFont.lfHeight /= dpiY;
    if (name)
        memcpy(logFont.lfFaceName, name, (wcslen(name) + 1) * sizeof(wchar_t));
    if (lfHeight != 0)
        logFont.lfHeight = lfHeight;
    int style = (int)fontStyle;
    if (style)
    {
        logFont.lfWeight = (style & (int)FONTSTYLE::FontStyleBold) ? FW_BOLD : FW_NORMAL;
        logFont.lfItalic = (style & (int)FONTSTYLE::FontStyleItalic) ? 1 : 0;
        logFont.lfUnderline = (style & (int)FONTSTYLE::FontStyleUnderline) ? 1 : 0;
        logFont.lfStrikeOut = (style & (int)FONTSTYLE::FontStyleStrikeout) ? 1 : 0;
    }
    return _font_create_logfont(&logFont, 0);
}


DLLAPITYPE(int) _font_height2size(int height)
{
    // -(参数1 * 参数2 / 参数3)
    // -(-12 * 72 / 96) = 9
    HDC hdc = GetDC(0);
    int ret = -MulDiv(height, 72, GetDeviceCaps(hdc, LOGPIXELSY));
    ReleaseDC(0, hdc);
    return ret;
}

DLLAPITYPE(int) _font_size2height(int size)
{
    // -(参数1 * 参数2 / 参数3)
    // -(9 * 96 / 72) = -12
    HDC hdc = GetDC(0);
    int ret = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(0, hdc);
    return ret;
}


DLLAPITYPE(HFONT) _font_GetFont(LPEX_FONT font)
{
    if (!font)return 0;
    return font->GetFont();
}







typedef struct EX_MATRIX
{

}*PEX_MATRIX, * LPEX_MATRIX;

DLLAPIBOOL _matrix_reset(LPEX_MATRIX matrix)
{
    if (!matrix)return false;
    return false;
}
int _matrix_create()
{
    return 0;
}
















DLLAPIBOOL _rgn_destroy(LPEX_RGN& hRgn)
{
    if (!hRgn)return false;
    bool ret = hRgn->Release() == 0;
    hRgn = 0;
    return ret;
}

DLLAPIRGN _rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY)
{
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        ID2D1Geometry* hRgn;
        HRESULT hr = S_OK;
        if (radiusX == 0 && radiusY == 0)
        {
            ID2D1RectangleGeometry* tmp;
            D2D1_RECT_F rc = { left, top, right, bottom };
            hr = d2dInfo.pFactory->CreateRectangleGeometry(&rc, &tmp);
            hRgn = tmp;
        }
        else
        {
            ID2D1RoundedRectangleGeometry* tmp;
            D2D1_ROUNDED_RECT rc;
            rc.rect = { left, top, right, bottom };
            rc.radiusX = radiusX;
            rc.radiusY = radiusY;
            hr = d2dInfo.pFactory->CreateRoundedRectangleGeometry(&rc, &tmp);
            hRgn = tmp;
        }
        if (ExSetLastError(hr) == 0)
        {
            __EX_RGN* rgn = new __EX_RGN;
            rgn->dxRgn = hRgn;;
            return rgn;
        }
        return 0;
    }
#endif

    __EX_RGN* rgn = new __EX_RGN;
    if (radiusX == 0 && radiusY == 0)
    {
        Gdiplus::RectF rc = { left, top, right, bottom };
        rgn->gpRgn = new Gdiplus::Region(rc);
    }
    else
    {
        Gdiplus::GraphicsPath* path = __createRoundedrect(left, top, right, bottom, radiusX, radiusY);
        rgn->gpRgn = new Gdiplus::Region(path);
        delete path;
    }
    return rgn;
}
DLLAPIRGN _rgn_create(float left, float top, float right, float bottom)
{
    return _rgn_create_round(left, top, right, bottom, 0.0f, 0.0f);
}

DLLAPIRGN _rgn_combine(LPEX_RGN _hRgnSrc, LPEX_RGN _hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY)
{
    if (!_hRgnSrc || !_hRgnDst) return 0;

    __EX_RGN* hRgnDst = (__EX_RGN*)_hRgnDst;
    __EX_RGN* hRgnSrc = (__EX_RGN*)_hRgnSrc;
#ifndef __USEXP // 不是xp, 使用D2D
    if (IsDxRender())
    {
        ID2D1PathGeometry1* hRgn;
        ID2D1GeometrySink* pSink;
        HRESULT hr = d2dInfo.pFactory->CreatePathGeometry(&hRgn);
        if (FAILED(hr))return 0;
        hr = hRgn->Open(&pSink);
        if (SUCCEEDED(hr))
        {
            D2D_MATRIX_3X2_F tranform;
            tranform.m11 = 1.0f;
            tranform.m12 = 0.f;
            tranform.m11 = 0.f;
            tranform.m22 = 1.0f;
            tranform.dx = dstOffsetX;
            tranform.dy = dstOffsetY;
            hr = hRgnSrc->dxRgn->CombineWithGeometry(hRgnDst->dxRgn,
                                                     (D2D1_COMBINE_MODE)nCombineMode,
                                                     &tranform, 0.0f, pSink);
            pSink->Close();
            SafeRelease(pSink);
            __EX_RGN* rgn = new __EX_RGN;
            rgn->dxRgn = hRgn;
            return rgn;
        }
        SafeRelease(hRgn);
        return 0;
    }
#endif
    __EX_RGN* rgn = new __EX_RGN;
    rgn->gpRgn = hRgnDst->gpRgn->Clone();
    rgn->gpRgn->Translate(dstOffsetX, dstOffsetY);
    if (nCombineMode == RGN_COMBINE_UNION)			// 并集
        rgn->gpRgn->Union(hRgnSrc->gpRgn);
    else if (nCombineMode == RGN_COMBINE_INTERSECT)	// 交集
        rgn->gpRgn->Intersect(hRgnSrc->gpRgn);
    else if (nCombineMode == RGN_COMBINE_XOR)		// 异或
        rgn->gpRgn->Xor(hRgnSrc->gpRgn);
    else
        rgn->gpRgn->Exclude(hRgnSrc->gpRgn);		// 排除
    return rgn;
}

DLLAPIBOOL _rgn_hittest(LPEX_RGN hRgn, float x, float y)
{
    if (!hRgn)return false;
    return hRgn->Hittest(x, y);
}
















class D2DDraw : public ID2DDraw
{
public:
    // 初始化相关

    virtual bool _canvas_init(bool isDxRender)
    {
        return ::_canvas_init(isDxRender, false);
    }
    virtual bool _canvas_isDxRender()
    {
        return ::_canvas_isDxRender();
    }
    virtual bool _canvas_destroy(LPCANVAS& hCanvas)
    {
        return ::_canvas_destroy(hCanvas);
    }

    virtual LPCANVAS _canvas_create(HWND hWnd, UINT width, UINT height)
    {
        return ::_canvas_create(hWnd, width, height);
    }

    virtual bool _canvas_uninit()
    {
        return ::_canvas_uninit();
    }



    virtual int ExSetLastError(int code)
    {
        return ::ExSetLastError(code);
    }

    virtual int ExGetLastError()
    {
        return ::ExGetLastError();
    }



    // 绘制相关
    virtual bool _canvas_clear(const LPCANVAS hCanvas, ARGB argb)
    {
        return ::_canvas_clear(hCanvas, argb);
    }

    virtual bool _canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap)
    {
        return ::_canvas_SetTarget(hCanvas, bitmap);
    }

    virtual bool _canvas_ReTarget(const LPCANVAS hCanvas)
    {
        return ::_canvas_ReTarget(hCanvas);
    }

    virtual LPEX_IMAGE _canvas_GetTarget(const LPCANVAS hCanvas)
    {
        return ::_canvas_GetTarget(hCanvas);
    }

    virtual void _canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image)
    {
        ::_canvas_FreeTarget(hCanvas, image);
    }

    virtual bool _canvas_begindraw(const LPCANVAS hCanvas)
    {
        return ::_canvas_begindraw(hCanvas);
    }

    // 平移
    virtual bool _canvas_translate(const LPCANVAS hCanvas, float x, float y)
    {
        return ::_canvas_translate(hCanvas, x, y);
    }

    // 缩放
    virtual bool _canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY)
    {
        return ::_canvas_scale(hCanvas, x, y, scaleX, scaleY);
    }

    // 旋转
    virtual bool _canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate)
    {
        return ::_canvas_rotate(hCanvas, x, y, rotate);
    }

    virtual bool _canvas_enddraw(const LPCANVAS hCanvas)
    {
        return ::_canvas_enddraw(hCanvas);
    }

    virtual bool _canvas_resize(const LPCANVAS hCanvas, int cx, int cy)
    {
        return ::_canvas_resize(hCanvas, cx, cy);
    }

    virtual HDC _canvas_getdc(const LPCANVAS hCanvas)
    {
        return ::_canvas_getdc(hCanvas);
    }

    virtual void _canvas_releasedc(const LPCANVAS hCanvas)
    {
        return ::_canvas_releasedc(hCanvas);
    }

    virtual bool _canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen,
        DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight)
    {
        return ::_canvas_calctextsize(hCanvas, font, text, textLen, textFormat, layoutWidth, layoutHeight, pWidth, pHeight);
    }

    virtual bool _canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height)
    {
        return ::_canvas_getsize(hCanvas, width, height);
    }

    // 绘制简单图形
    virtual bool _canvas_drawRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECT_F rcF)
    {
        return ::_canvas_drawRectangle(hCanvas, hPen, rcF);
    }

    virtual bool _canvas_fillRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECT_F rcF)
    {
        return ::_canvas_fillRectangle(hCanvas, hBrush, rcF);
    }

    virtual bool _canvas_drawEllipse(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCELLIPSE_F rcF)
    {
        return ::_canvas_drawEllipse(hCanvas, hPen, rcF);
    }

    virtual bool _canvas_fillEllipse(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF)
    {
        return ::_canvas_fillEllipse(hCanvas, hBrush, rcF);
    }

    virtual bool _canvas_drawRoundedRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded)
    {
        return ::_canvas_drawRoundedRectangle(hCanvas, hPen, rcRounded);
    }

    virtual bool _canvas_fillRoundedRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded)
    {
        return ::_canvas_fillRoundedRectangle(hCanvas, hBrush, rcRounded);
    }

    virtual bool _canvas_drawLine(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCLINEPOINT_F pt)
    {
        return ::_canvas_drawLine(hCanvas, hPen, pt);
    }



    virtual bool _canvas_fillpath(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPEX_PATH path)
    {
        return ::_canvas_fillpath(hCanvas, hBrush, path);
    }

    virtual bool _canvas_drawpath(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPEX_PATH path)
    {
        return ::_canvas_drawpath(hCanvas, hPen, path);
    }




    virtual bool _canvas_drawimage(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, BYTE alpha)
    {
        return ::_canvas_drawimage(hCanvas, image, left, top, alpha);
    }

    virtual bool _canvas_drawimagerect(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha)
    {
        return ::_canvas_drawimagerect(hCanvas, image, left, top, right, bottom, alpha);
    }

    virtual bool _canvas_drawimagerectrect(const LPCANVAS hCanvas, LPEX_IMAGE image,
        float dstLeft, float dstTop, float dstRight, float dstBottom,
        float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha)
    {
        return ::_canvas_drawimagerectrect(hCanvas, image, dstLeft, dstTop, dstRight, dstBottom, srcLeft, srcTop, srcRight, srcBottom, alpha);
    }

    virtual bool _canvas_drawimagegridPadding(const LPCANVAS hCanvas, LPEX_IMAGE image,
        float dstLeft, float dstTop, float dstRight, float dstBottom,
        float srcLeft, float srcTop, float srcRight, float srcBottom,
        float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
        DWORD dwFlags, BYTE alpha)
    {
        return ::_canvas_drawimagegridPadding(hCanvas, image, dstLeft, dstTop, dstRight, dstBottom,
            srcLeft, srcTop, srcRight, srcBottom,
            gridPaddingLeft, gridPaddingTop, gridPaddingRight, gridPaddingBottom, dwFlags, alpha);
    }


    virtual bool _canvas_drawtext(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc)
    {
        return ::_canvas_drawtext(hCanvas, font, hBrush, text, textLen, textFormat, rc);
    }

    virtual bool _canvas_drawtextex(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush,
        LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
        DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText)
    {
        return ::_canvas_drawtextex(hCanvas, font, hBrush, text, textLen, textFormat, rc, iGlowsize, crShadow, lParam, prclayout, outrcText);
    }


    virtual bool _canvas_alphablend(const LPCANVAS hCanvas,
        float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
        float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha)
    {
        return ::_canvas_alphablend(hCanvas, xoriginDest, yoriginDest, wDest, hDest, sCanvas, xoriginSrc, yoriginSrc, wSrc, hSrc, Alpha);
    }

    virtual bool _canvas_bitblt(LPCANVAS hCanvas, int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop)
    {
        return ::_canvas_bitblt(hCanvas, x, y, cx, cy, sCanvas, x1, y1, rop);
    }

    virtual bool _canvas_flush(const LPCANVAS hCanvas)
    {
        return ::_canvas_flush(hCanvas);
    }

    virtual bool _canvas_setrgn(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode = 0)
    {
        return ::_canvas_setrgn(hCanvas, left, top, right, bottom, mode);
    }

    virtual bool _canvas_setClip(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode = 0)
    {
        return ::_canvas_setClip(hCanvas, left, top, right, bottom, mode);
    }

    virtual bool _canvas_resetClip(const LPCANVAS hCanvas)
    {
        return ::_canvas_resetClip(hCanvas);
    }


    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调与 _canvas_SetSmoothingMode, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool _canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0)
    {
        return ::_canvas_SetAntialiasMode(hCanvas, mode, oldMode);
    }


    // 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
    // 此函数调用 _canvas_SetAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool _canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0)
    {
        return ::_canvas_SetSmoothingMode(hCanvas, mode, oldMode);
    }


    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调与 _canvas_SetTextRenderingHint, 功能一模一样, 多加一个是为了和d2d名字一样
    virtual bool _canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0)
    {
        return ::_canvas_SetTextAntialiasMode(hCanvas, mode, oldMode);
    }

    // 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
    // 此函数调用 _canvas_SetTextAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
    virtual bool _canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode = 0)
    {
        return ::_canvas_SetTextRenderingHint(hCanvas, mode, oldMode);
    }




    // 画刷
    virtual LPEX_BRUSH _brush_create(ARGB color)
    {
        return ::_brush_create(color);
    }

    // 创建线性渐变画刷
    // pt1,pt2 = 起点, 终点坐标
    // color,colorCount = 颜色数组, 颜色数组数量
    // fillMode = 填充模式
    // pRatios, ratiosCount = 渐变比例数组, 取值范围0-1, 数组数量
    virtual LPEX_BRUSH _brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode = 0, const float* pRatios = 0, DWORD ratiosCount = 0)
    {
        return ::_brush_create_LinearGradient(pt1, pt2, color, colorCount, fillMode, pRatios, ratiosCount);
    }

    // 创建放射渐变画刷, 从中心点往外扩散渐变
    // pos = 包含中心点以及向外扩展的长度
    // color,colorCount = 颜色数组, 颜色数组数量
    // fillMode = 填充模式
    // pRatios, ratiosCount = 渐变比例数组, 取值范围0-1, 数组数量
    virtual LPEX_BRUSH _brush_create_RadialGradient(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode = 0, const float* pRatios = 0, DWORD ratiosCount = 0)
    {
        return ::_brush_create_RadialGradient(pos, color, colorCount, fillMode, pRatios, ratiosCount);
    }

    virtual bool _brush_destroy(LPEX_BRUSH& pBrush)
    {
        return ::_brush_destroy(pBrush);
    }

    virtual LPEX_PEN _pen_create(ARGB color, float width = 1.0f, int style = 0, const float* dashes = 0, int dashesCount = 0)
    {
        return ::_pen_create(color, width, style, dashes, dashesCount);
    }

    virtual bool _pen_destroy(LPEX_PEN& pBrush)
    {
        return ::_pen_destroy(pBrush);
    }
    virtual bool _pen_setcolor(LPEX_PEN pen, ARGB color)
    {
        return ::_pen_setcolor(pen, color);
    }
    virtual DWORD _pen_getcolor(LPEX_PEN pen)
    {
        return ::_pen_getcolor(pen);
    }
    virtual bool _brush_setcolor(LPEX_BRUSH brush, ARGB color)
    {
        return ::_brush_setcolor(brush, color);
    }

    virtual DWORD _brush_getcolor(LPEX_BRUSH brush)
    {
        return ::_brush_getcolor(brush);
    }

#ifndef __USEXP // 不是xp, 使用D2D
    virtual bool _brush_setstartpoint(LPEX_BRUSH brush, float x, float y)
    {
        return ::_brush_setstartpoint(brush, x, y);
    }

    virtual bool _brush_setendpoint(LPEX_BRUSH brush, float x, float y)
    {
        return ::_brush_setendpoint(brush, x, y);
    }

    virtual bool _brush_getstartpoint(LPEX_BRUSH brush, float* x, float* y)
    {
        return ::_brush_getstartpoint(brush, x, y);
    }

    virtual bool _brush_getendpoint(LPEX_BRUSH brush, float* x, float* y)
    {
        return ::_brush_getendpoint(brush, x, y);
    }

    virtual bool _brush_setcenter(LPEX_BRUSH brush, float x, float y)
    {
        return ::_brush_setcenter(brush, x, y);
    }

    virtual bool _brush_SetGradientOriginOffset(LPEX_BRUSH brush, float x, float y)
    {
        return ::_brush_SetGradientOriginOffset(brush, x, y);
    }

    virtual bool _brush_SetRadiusX(LPEX_BRUSH brush, float x)
    {
        return ::_brush_SetRadiusX(brush, x);
    }

    virtual bool _brush_SetRadiusY(LPEX_BRUSH brush, float x)
    {
        return ::_brush_SetRadiusY(brush, x);
    }

    virtual bool _brush_GetCenter(LPEX_BRUSH brush, float* x, float* y)
    {
        return ::_brush_GetCenter(brush, x, y);
    }

    virtual bool _brush_GetGradientOriginOffset(LPEX_BRUSH brush, float* x, float* y)
    {
        return ::_brush_GetGradientOriginOffset(brush, x, y);
    }

    virtual float _brush_GetRadiusY(LPEX_BRUSH brush)
    {
        return ::_brush_GetRadiusY(brush);
    }
#endif

    // 路径
    virtual LPEX_PATH _path_create(int fillMode)
    {
        return ::_path_create(fillMode);
    }

    virtual LPEX_PATH _path_createroundedrectangle(const LPCRECTROUNDED_F rcRounded, int fillMode)
    {
        return ::_path_createroundedrectangle(rcRounded, fillMode);
    }

    virtual LPEX_PATH _path_createrectangle(const LPCRECT_F rcRounded, int fillMode)
    {
        return ::_path_createrectangle(rcRounded, fillMode);
    }

    virtual bool _path_destroy(LPEX_PATH& path)
    {
        return ::_path_destroy(path);
    }

    virtual bool _path_beginfigure(LPEX_PATH path, const LPCPOINT_F pt)
    {
        return ::_path_beginfigure(path, pt);
    }

    virtual bool _path_endfigure(LPEX_PATH path)
    {
        return ::_path_endfigure(path);
    }

    virtual bool _path_reset(LPEX_PATH path)
    {
        return ::_path_reset(path);
    }

    virtual bool _path_addarc(LPEX_PATH path, float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise)
    {
        return ::_path_addarc(path, x1, y1, x2, y2, radiusX, radiusY, fClockwise);
    }

    virtual bool _path_addbezier(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4)
    {
        return ::_path_addbezier(path, pt1, pt2, pt3, pt4);
    }

    virtual bool _path_addline(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2)
    {
        return ::_path_addline(path, pt1, pt2);
    }

    virtual bool _path_addrect(LPEX_PATH path, const RECT_F& rc)
    {
        return ::_path_addrect(path, rc);
    }

    virtual bool _path_addroundedrect(LPEX_PATH path, const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight)
    {
        return ::_path_addroundedrect(path, rc, radiusTopLeft, radiusTopRight, radiusBottomLeft, radiusBottomRight);
    }

    virtual bool _path_open(LPEX_PATH path)
    {
        return ::_path_open(path);
    }

    virtual bool _path_close(LPEX_PATH path)
    {
        return ::_path_close(path);
    }

    // 取路径边界矩形
    virtual bool _path_getbounds(LPEX_PATH path, LPRECT_F rc)
    {
        return ::_path_getbounds(path, rc);
    }

    // 测试坐标是否在可见路径内
    virtual bool _path_hittest(LPEX_PATH path, const POINT_F& pt)
    {
        return ::_path_hittest(path, pt);
    }



    // 图片
    virtual bool _img_destroy(LPEX_IMAGE& img)
    {
        return ::_img_destroy(img);
    }

    virtual LPEX_IMAGE _img_create(UINT width, UINT height)
    {
        return ::_img_create(width, height);
    }

    virtual LPEX_IMAGE _img_create_frommemory(const void* pData, DWORD len)
    {
        return ::_img_create_frommemory(pData, len);
    }

    virtual LPEX_IMAGE _img_create_fromfile(LPCWSTR file)
    {
        return ::_img_create_fromfile(file);
    }

    virtual LPEX_IMAGE _img_create_frombitmap(HBITMAP hBitmap, HPALETTE hPalette, bool fPreAlpha)
    {
        return ::_img_create_frombitmap(hBitmap, hPalette, fPreAlpha);
    }

    virtual LPEX_IMAGE _img_create_formicon(HICON hIcon)
    {
        return ::_img_create_formicon(hIcon);
    }

    virtual LPEX_IMAGE _img_create_fromstream(IStream* stream)
    {
        return ::_img_create_fromstream(stream);
    }


    virtual bool _img_getsize(LPEX_IMAGE image, UINT* width, UINT* height)
    {
        return ::_img_getsize(image, width, height);
    }

    virtual UINT _img_width(LPEX_IMAGE image)
    {
        return ::_img_width(image);
    }

    virtual UINT _img_height(LPEX_IMAGE image)
    {
        return ::_img_height(image);
    }

    virtual LPEX_IMAGE _img_copyrect(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height)
    {
        return ::_img_copyrect(image, x, y, width, height);
    }

    virtual LPEX_IMAGE _img_copy(LPEX_IMAGE image)
    {
        return ::_img_copy(image);
    }


    // 锁定位图, 成功返回true, 失败返回false, 不使用时必须调用 _img_unlock 释放数据
    // x,y,width,height = 左顶宽高, 宽高为0时会自动获取图片大小
    // flags = 1=读, 2=写, 3=读写
    virtual bool _img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData)
    {
        return ::_img_lock(image, x, y, width, height, flags, lockData);
    }

    // 
    virtual bool _img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData)
    {
        return ::_img_unlock(image, lockData);
    }


    // 取图像帧数
    virtual UINT _img_getframecount(LPEX_IMAGE image)
    {
        return ::_img_getframecount(image);
    }

    // 设置当前活动帧
    virtual bool _img_selectactiveframe(LPEX_IMAGE image, UINT index)
    {
        return ::_img_selectactiveframe(image, index);
    }

#ifndef __USEXP
    // 从D2D位图指定左顶宽高获取图片或效果数据, 返回图片数据指针和尺寸, 需要调用free 释放
    virtual LPBYTE _img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom)
    {
        return ::_img_save_frombitmap(pBitmap, pSize, left, top, right, bottom);
    }
#endif

    // 从绘画句柄里获取图片数据, 返回图片数据指针和尺寸, 需要调用free 释放
    virtual LPBYTE _img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T* pSize)
    {
        return ::_img_save_formcanvas(hCanvas, pSize);
    }

    // 从绘画句柄指定左顶宽高获取图片数据, 返回图片数据指针和尺寸, 需要调用free 释放
    virtual LPBYTE _img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom)
    {
        return ::_img_save_formcanvasex(hCanvas, pSize, left, top, right, bottom);
    }

    // 从图片句柄获取图片数据, 返回图片数据指针和尺寸, 需要调用free 释放
    virtual LPBYTE _img_save(LPEX_IMAGE image, SIZE_T* pSize)
    {
        return ::_img_save(image, pSize);
    }

    // 从图片句柄指定左顶宽高获取图片数据, 返回图片数据指针和尺寸, 需要调用free 释放
    virtual LPBYTE _img_save_rect(LPEX_IMAGE image, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom)
    {
        return ::_img_save_rect(image, pSize, left, top, right, bottom);
    }

    // 释放 _img_save_xxx 返回的数据
    virtual void _img_free(LPBYTE ptr)
    {
        ::free(ptr);
    }

    // 把图片转成gdi的位图
    virtual HBITMAP _img_ToGdiBitmap(LPEX_IMAGE image)
    {
        return ::_img_ToGdiBitmap(image);
    }

    // 位图
    virtual LPEX_IMAGE _bitmap_create(UINT width, UINT height, bool fGdi)
    {
        return ::_bitmap_create(width, height, fGdi);
    }




    // 字体
    // 从LOGFONTW 里创建字体
    virtual LPEX_FONT _font_create_logfont(const LPLOGFONTW logFont, DWORD flags)
    {
        return ::_font_create_logfont(logFont, flags);
    }

    // 从字体名中创建字体, 如果参数都为0则创建默认字体
    virtual LPEX_FONT _font_create(LPCWSTR name = 0, LONG lfHeight = 0, FONTSTYLE fontStyle = FONTSTYLE::FontStyleRegular)
    {
        return ::_font_create(name, lfHeight, fontStyle);
    }

    virtual bool _font_destroy(LPEX_FONT& font)
    {
        return ::_font_destroy(font);
    }

    // 字符高度转字体大小, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
    virtual int _font_height2size(int height)
    {
        return ::_font_height2size(height);
    }

    // 字体大小转字符高度, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
    virtual int _font_size2height(int size)
    {
        return ::_font_size2height(size);
    }



    // 区域

    virtual bool _rgn_destroy(LPEX_RGN& hRgn)
    {
        return ::_rgn_destroy(hRgn);
    }

    virtual LPEX_RGN _rgn_create(float left, float top, float right, float bottom)
    {
        return ::_rgn_create(left, top, right, bottom);
    }

    virtual LPEX_RGN _rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY)
    {
        return ::_rgn_create_round(left, top, right, bottom, radiusX, radiusY);
    }

    virtual LPEX_RGN _rgn_combine(LPEX_RGN hRgnSrc, LPEX_RGN hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY)
    {
        return ::_rgn_combine(hRgnSrc, hRgnDst, nCombineMode, dstOffsetX, dstOffsetY);
    }

    virtual bool _rgn_hittest(LPEX_RGN hRgn, float x, float y)
    {
        return ::_rgn_hittest(hRgn, x, y);
    }

    virtual bool recreate_Device()
    {
        return ::recreate_Device();
    }

};

DLLAPITYPE(ID2DDraw*) _canvas_new()
{
    return new D2DDraw;
}


























