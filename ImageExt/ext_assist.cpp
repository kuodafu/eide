#include "ImageExt.h"


// crFixed = 新的颜色
// crBack = 被替换的颜色
// lpBits = 图片在内存里的数据
// nCount = 图片像素, 高度 * 宽度
// sR,sG,sB 新的RGB, crFixed 的RGB
void  PixelFix(DWORD crFixed, DWORD crBack, LPBYTE lpBits, int nCount, BYTE sR, BYTE sG, BYTE sB)
{
    static ARGB s_border = MAKEARGB(255, 246, 246, 246);
    DWORD* cr = (DWORD*)lpBits;        // 转成4字节一个像素点来处理
    for (int i = 0; i < nCount; ++i)
    {
        if (cr[i] == 0 || cr[i] == s_border) continue;    // 等于0 表示是透明, 透明的不处理

        if (cr[i] == crBack)        // FF FF FF 白色, 是白色就是画上的文本颜色, 直接修改成传递进来的颜色
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

// 修改像素点, 替换指定颜色, 不符合的颜色不处理
// crFixed = 新的颜色
// crBack = 被替换的颜色
// lpBits = 图片在内存里的数据
// nCount = 图片像素, 高度 * 宽度
void  PixelFix(DWORD crFixed, DWORD crBack, LPBYTE lpBits, int nCount)
{
    DWORD* cr = (DWORD*)lpBits;        // 转成4字节一个像素点来处理
    for (int i = 0; i < nCount; ++i)
    {
        if (cr[i] == 0) continue;    // 等于0 表示是透明, 透明的不处理

        if (cr[i] == crBack)        // FF FF FF 白色, 是白色就是画上的文本颜色, 直接修改成传递进来的颜色
        {
            cr[i] = crFixed;
            continue;
        }
    }
}

// 修改非透明的像素点
// crFixed = 新的颜色
// lpBits = 图片在内存里的数据
// nCount = 图片像素, 高度 * 宽度
void  PixelFix(DWORD crFixed, LPBYTE lpBits, int nCount)
{
    DWORD* cr = (DWORD*)lpBits;        // 转成4字节一个像素点来处理
    for (int i = 0; i < nCount; ++i)
    {
        if (cr[i] == 0) continue;    // 等于0 表示是透明, 透明的不处理
        //BYTE a = ARGB_GETA(cr[i]);
        //cr[i] = ARGB_SETA(crFixed, a);
        cr[i] = crFixed;
    }
}

inline Gdiplus::Bitmap* _load_image_from_file(LPCWSTR file)
{
    CFileRW f;
    f.open(file, 1, -1);
    LPBYTE pData = 0;
    int size = f.size();
    f.read(&pData);
    return _img_create_from_memory(pData, size);
}

int enum_file(LPCWSTR findPath)
{
    if (s_dataBufCount == 0)
    {
        s_dataBufCount = 1000;
        s_data.resize(s_dataBufCount);
    }
    s_dataCount = 0;
    wstr path(260);
    path = findPath;
    if (path.right(1) != L"\\") path.append(L"\\");
    if (s_loadPath == path)
    {
        int n = 0;
        for (auto i = s_data.begin(); i != s_data.end(); ++i, ++n)
        {
            LIST_FILEDATA& data = *i;
            if (data.file != s_loadFile)continue;

            s_list.SetIndex(n);
            s_list.SetTopIndex(n);
            break;
        }
        return s_dataCount;
    }

    wstr tmpPath(260);
    tmpPath.append(path).append(L"*.*");

    WIN32_FIND_DATA fd = { 0 };
    DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;

    HANDLE hFind = FindFirstFileW(tmpPath.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hFind) return s_dataCount;
    s_loadPath = path;
    int index = -1;
    int n = 0;
    do
    {
        if ((dwFileAttributes & fd.dwFileAttributes) == fd.dwFileAttributes)   // 匹配类型
        {
            if (s_dataCount + 1 >= s_dataBufCount)
            {
                s_dataBufCount *= 2;
                s_data.resize(s_dataBufCount);
            }
            LIST_FILEDATA& data = s_data[s_dataCount++];
            data.file.assign(path).append(fd.cFileName);
            if (index == -1 && data.file == s_loadFile) index = n;
            n++;
            data.name = wcsrchr(data.file.c_str(), '\\') + 1;
            if (data.img)delete data.img;
            data.img = _load_image_from_file(data.file.c_str());
            data.cxImg = data.img->GetWidth();
            data.cyImg = data.img->GetHeight();
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    s_list.SetIndex(index);
    s_list.SetItemCount(s_dataCount);
    s_list.SetTopIndex(index);

    return s_dataCount;
}

void _load_image(const wstr& path)
{
    if (path.empty())return;
    if (s_hImage)delete s_hImage;

    md_destroy(s_md);
    s_editPath.SetText(path.c_str());
    s_loadFile = path;
    s_hImage = _load_image_from_file(s_loadFile.c_str());
    if (!s_hImage)return;
    s_md = md_create(s_hImage->GetWidth(), s_hImage->GetHeight());
    Gdiplus::Graphics gp(s_md->hdc);
    gp.DrawImage(s_hImage, 0, 0, s_hImage->GetWidth(), s_hImage->GetHeight());

}