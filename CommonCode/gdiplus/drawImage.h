#pragma once
#include <gdiplus.h>

#ifndef assert
#define assert(_) 
#endif
// 从图片的某个位置绘制到指定的位置
// dest = 要绘画到的位置, 位置是左顶右底
// src  = 从图片的哪个位置开始绘画, 位置是左顶右底
inline Gdiplus::Status DrawImageFromRect(Gdiplus::Graphics* gp, Gdiplus::Image* pImg,
    float dstLeft, float dstTop, float dstRight, float dstBottom,
    float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha = 255)
{
    Gdiplus::Status status = Gdiplus::Status::Ok;
    if (alpha == 0)
        return status;

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
    if (alpha != 255)
    {
        float cm[] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
        Gdiplus::ColorMatrix m;
        assert(sizeof(m) == sizeof(cm));
        cm[18] = static_cast<float>(alpha) / 255.0f;
        memcpy(&m, cm, sizeof(m));
        Gdiplus::Status status = attr->SetColorMatrix(&m,
            Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
    }
    status = gp->DrawImage(pImg,
        rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height, Gdiplus::Unit::UnitPixel, attr);
    return status;
}


// 从图片的某个位置拉伸绘画到指定位置上, 九宫格
// dest = 要绘画到的位置, 位置是左顶右底
// src  = 从图片的哪个位置开始绘画, 位置是左顶右底
inline Gdiplus::Status DrawImageFromGrid(Gdiplus::Graphics* gp, Gdiplus::Image* pImg,
    float dstLeft, float dstTop, float dstRight, float dstBottom,
    float srcLeft, float srcTop, float srcRight, float srcBottom,
    float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
    BYTE alpha = 255)
{
    float pl = gridPaddingLeft;
    float pt = gridPaddingTop;
    float pr = gridPaddingRight;
    float pb = gridPaddingBottom;


    // 右-中间
    Gdiplus::Status ret = Gdiplus::Status::Ok;
    
    ret = DrawImageFromRect(gp, pImg,
        dstRight - pr, dstTop + pt, dstRight, dstBottom - pb,
        srcRight - gridPaddingRight, srcTop + gridPaddingTop, srcRight, srcBottom - gridPaddingBottom, alpha);

    // 右下
    ret = DrawImageFromRect(gp, pImg,
        dstRight - pr, dstBottom - pb, dstRight, dstBottom, srcRight - gridPaddingRight,
        srcBottom - gridPaddingBottom, srcRight, srcBottom, alpha);

    // 底-中间
    ret = DrawImageFromRect(gp, pImg,
        dstLeft + pl, dstBottom - pb, dstRight - pr, dstBottom,
        srcLeft + gridPaddingLeft, srcBottom - gridPaddingBottom, srcRight - gridPaddingRight, srcBottom, alpha);

    // 左下
    ret = DrawImageFromRect(gp, pImg,
        dstLeft, dstBottom - pb, dstLeft + pl, dstBottom,
        srcLeft, srcBottom - gridPaddingBottom, srcLeft + gridPaddingLeft, srcBottom, alpha);

    // 左-中间
    ret = DrawImageFromRect(gp, pImg,
        dstLeft, dstTop + pt, dstLeft + pl, dstBottom - pb,
        srcLeft, srcTop + gridPaddingTop, srcLeft + gridPaddingLeft, srcBottom - gridPaddingBottom, alpha);


    // 左上
    ret = DrawImageFromRect(gp, pImg,
        dstLeft, dstTop, dstLeft + pl, dstTop + pt,
        srcLeft, srcTop, srcLeft + gridPaddingLeft, srcTop + gridPaddingTop, alpha);

    // 右上
    ret = DrawImageFromRect(gp, pImg,
        dstRight - pr, dstTop, dstRight, dstTop + pt,
        srcRight - gridPaddingRight, srcTop, srcRight, srcTop + gridPaddingTop, alpha);

    // 顶-中间
    ret = DrawImageFromRect(gp, pImg,
        dstLeft + pl, dstTop, dstRight - pr, dstTop + pt,
        srcLeft + gridPaddingLeft, srcTop, srcRight - gridPaddingRight, srcTop + gridPaddingTop, alpha);
    return ret;
}


// 从图片的某个位置绘制到指定的位置
// dest = 要绘画到的位置, 位置是左顶右底
// src  = 从图片的哪个位置开始绘画, 位置是左顶右底
inline Gdiplus::Status DrawImageFromRect(Gdiplus::Graphics* gp, Gdiplus::Image* pImg, const Gdiplus::RectF& dest, const Gdiplus::RectF& src, BYTE alpha = 255)
{
    return DrawImageFromRect(gp, pImg,
                             dest.X, dest.Y, dest.X + dest.Width, dest.GetBottom(),
                             src.X, src.Y, src.GetRight(), src.GetBottom(), alpha);

    //return DrawImageFromRect(gp, pImg,
    //    dest.left, dest.top, dest.right, dest.bottom,
    //    src.left, src.top, src.right, src.bottom, alpha);
}

// 从图片的某个位置拉伸绘画到指定位置上, 九宫格
// dest = 要绘画到的位置, 位置是左顶右底
// src  = 从图片的哪个位置开始绘画, 位置是左顶右底
inline Gdiplus::Status DrawImageFromGrid(Gdiplus::Graphics* gp, Gdiplus::Image* pImg,
    const Gdiplus::RectF& dest, const Gdiplus::RectF& src, const Gdiplus::RectF& grid, BYTE alpha = 255)
{
    return DrawImageFromGrid(gp, pImg,
                             dest.X, dest.Y, dest.GetRight(), dest.GetBottom(),
                             src.X, src.Y, src.GetRight(), src.GetBottom(),
                             grid.X, grid.Y, grid.GetRight(), grid.GetBottom(), alpha);

    //return DrawImageFromGrid(gp, pImg,
    //                         dest.left, dest.top, dest.right, dest.bottom,
    //                         src.left, src.top, src.right, src.bottom,
    //                         grid.left, grid.top, grid.right, grid.bottom, alpha);
}

inline Gdiplus::Bitmap* _img_create_from_memory(const void* p, DWORD size)
{
    if (!p || !size)return 0;
    IStream* stream = 0;
    HGLOBAL hMem = 0;
    HRESULT hr = 0;
    while (true)
    {
        hMem = GlobalAlloc(GMEM_MOVEABLE, size);
        if (!hMem)break;;

        hr = CreateStreamOnHGlobal(hMem, TRUE, &stream);
        if (FAILED(hr))break;;

        void* ptr = GlobalLock(hMem);
        if (!ptr)break;

        memcpy(ptr, p, size);
        GlobalUnlock(hMem);
        Gdiplus::Bitmap* img = new Gdiplus::Bitmap(stream);
        stream->Release();
        return img;
    }
    if (hMem) GlobalFree(hMem);
    if (stream)stream->Release();
    return 0;
}