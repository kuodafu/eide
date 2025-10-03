#include "ThemeResourceExt.h"
#include <zlib/Czlib.h>
#include <vector>
#include <xml2/tinyxml2.h>
#include <control/ReadColor.h>


THEME_BEGIN_NAMESPACE


// 计算内存是否还足够写入, 如果不够写入, 会申请一块新的内存
inline void _ext_serialize_xml_calcsize(_str& pData, int& bufSize, int offset, int writeSize)
{
    if (offset + writeSize <= bufSize)return;
    int newSize = (writeSize + bufSize) * 2;
    pData.resize(newSize);
    bufSize = newSize;
}
// 把文本写入到地址中, 文本的保存结构是 文本数据 + 结束标识, 最少会写入两个字节
// pData = 要写入的地址
// bufSize = 缓冲区大小, 大小不够时需要重新申请内存
// offset = 地址偏移
// pstr = 要写入的文本
inline void _ext_serialize_xml_write_string(_str& pData, int& bufSize, int& offset, LPCWSTR pstr)
{
    if (pstr && pstr[0])
    {
        int len = (int)wcslen(pstr) + 1;
        len *= sizeof(wchar_t);

        _ext_serialize_xml_calcsize(pData, bufSize, offset, len);
        memcpy(pData.data() + offset, pstr, len);
        offset += len;
        return;
    }
    const int len = 2;
    _ext_serialize_xml_calcsize(pData, bufSize, offset, len);
    offset += len;
}

// 把矩形写入到地址中, 如果写入的矩形(pszRect)不为空, 则返回true
// pData = 要写入的地址
// bufSize = 缓冲区大小, 大小不够时需要重新申请内存
// offset = 地址偏移
// pszRect = 文本形式的rect矩形
inline bool _ext_serialize_xml_write_rect(_str& pData, int& bufSize, int& offset, LPCWSTR pszRect)
{
    bool isOk = false;
    _ext_serialize_xml_calcsize(pData, bufSize, offset, sizeof(RECT));
    LONG* ptr = (LONG*)(pData.data() + offset);
    if (pszRect && pszRect[0])
    {
        swscanf_s(pszRect, L"%d,%d,%d,%d", &ptr[0], &ptr[1], &ptr[2], &ptr[3]);
        if (ptr[2] - ptr[0] > 0 && ptr[3] - ptr[1] > 0) // 右边大于左边, 底边大于顶边
            isOk = true;
    }
    else
    {
        memset(ptr, 0, sizeof(RECT));
    }
    offset += sizeof(RECT);
    return isOk;
}

// 把数值写入到地址中, 返回写入的值
// pData = 要写入的地址
// bufSize = 缓冲区大小, 大小不够时需要重新申请内存
// offset = 地址偏移
// value = 要写入的数值, 占用4个字节
inline int _ext_serialize_xml_write_int(_str& pData, int& bufSize, int& offset, int value)
{
    _ext_serialize_xml_calcsize(pData, bufSize, offset, sizeof(int));
    LONG* ptr = (LONG*)(pData.data() + offset);
    memcpy(ptr, &value, sizeof(int));
    
    offset += sizeof(int);
    return value;
}

// 把指定长度的数据写入到地址中
// pData = 要写入的地址
// bufSize = 缓冲区大小, 大小不够时需要重新申请内存
// offset = 地址偏移
// value = 要写入的数据地址
// size = 要写入的数据大小
inline void _ext_serialize_xml_write(_str& pData, int& bufSize, int& offset, const void* value, int size)
{
    _ext_serialize_xml_calcsize(pData, bufSize, offset, size);
    memcpy(pData.data() + offset, value, size);
    offset += size;
}

// 序列化ext需要的xml数据
// xmlText = xml数据, 内部会解析数据保存到serializeData参数中
// serializeData = 接收解析好的结果
// theme = 接收解析结果的基本信息, 这个也是皮肤文件头
inline EXT_ERR _ext_serialize_xml(const wchar_t* xmlText, _str& serializeData, THEME_FILE_HEADER_1_2& theme)
{
    CTinyXml2W xml;
    if (!xml.Parse(xmlText))
    {
        // 保存主题, 解析xml失败
        //MessageBoxW(g_hWnd, L"保存主题, 解析xml失败", L"错误", 0);
        return EXT_ERR::EXT_ERR_PARSEXML;
    }

    SIZE maxSize = { 0 };
    SIZE imgSize = { 0 };
    LPCWSTR lpszMaxSize = xml.GetAttrValue(L"@1", L"maxSize");
    LPCWSTR lpszImgSize = xml.GetAttrValue(L"@1", L"imgSize");
    if (lpszMaxSize && lpszMaxSize[0]) swscanf_s(lpszMaxSize, L"%d,%d", &maxSize.cx, &maxSize.cy);
    if (lpszImgSize && lpszImgSize[0]) swscanf_s(lpszImgSize, L"%d,%d", &imgSize.cx, &imgSize.cy);
    if (maxSize.cx == 0)maxSize.cx = 16;
    if (maxSize.cy == 0)maxSize.cy = 16;

    theme.cbSize        = sizeof(theme);
    theme.dwMaxSize     = MAKELONG(maxSize.cx, maxSize.cy);
    theme.dwImageSize   = MAKELONG(imgSize.cx, imgSize.cy);
    theme.dwReserved    = 0;
    theme.dwCount       = (int)xml.GetChildNodeCount(L"@1");
    theme.dwDataLength  = 0;
    

    // 每个成员记录一个RECT和一个文本的id和一个说明, 文本按20字符算, 说明按40字符算
    int bufSize = theme.dwCount * (sizeof(RECT) + 120);
    serializeData.resize(bufSize);
    int offset = 0;

    tinyxml2::XMLNode* node = 0;
    int count = 0;
    for (DWORD i = 0; i < theme.dwCount; i++)
    {
        wchar_t nodeName[50];
        swprintf_s(nodeName, 50, L"@1/@%d", i + 1);
        xml.GetNode(nodeName, node);
        assert(node);
        LPCWSTR id = xml.GetAttrValue(node, L"id");
        if (!id || !id[0])
        {
            //MessageBoxW(g_hWnd, L"主题中有id为空, 请输入ID再保存", L"错误", 0);
            return EXT_ERR::EXT_ERR_NULLID;
        }
        LPCWSTR text        = xml.GetAttrValue(node, L"text");      // 替换显示的文本
        LPCWSTR remarks     = xml.GetNodeValue(node);               // 备注

        int oldOffset   = offset;
        const int crText[4] =
        {
            (int)str2argb(xml.GetAttrValue(node, L"crText1"), 0),  // 正常文本颜色
            (int)str2argb(xml.GetAttrValue(node, L"crText2"), 0),  // 热点文本颜色
            (int)str2argb(xml.GetAttrValue(node, L"crText3"), 0),  // 按下文本颜色
            (int)str2argb(xml.GetAttrValue(node, L"crText4"), 0),  // 禁止文本颜色
        };
        const int crBack[4] =
        {
            (int)str2argb(xml.GetAttrValue(node, L"crBack1"), 0),  // 正常背景颜色
            (int)str2argb(xml.GetAttrValue(node, L"crBack2"), 0),  // 热点背景颜色
            (int)str2argb(xml.GetAttrValue(node, L"crBack3"), 0),  // 按下背景颜色
            (int)str2argb(xml.GetAttrValue(node, L"crBack4"), 0),  // 禁止背景颜色
        };
        const LPCWSTR szRect[] =
        {
            xml.GetAttrValue(node, L"szRect1"),     // 正常矩形
            xml.GetAttrValue(node, L"szRect2"),     // 热点矩形
            xml.GetAttrValue(node, L"szRect3"),     // 按下矩形
            xml.GetAttrValue(node, L"szRect4"),     // 禁止矩形

            xml.GetAttrValue(node, L"szRect1_125"),     // 正常矩形
            xml.GetAttrValue(node, L"szRect2_125"),     // 热点矩形
            xml.GetAttrValue(node, L"szRect3_125"),     // 按下矩形
            xml.GetAttrValue(node, L"szRect4_125"),     // 禁止矩形

            xml.GetAttrValue(node, L"szRect1_150"),     // 正常矩形
            xml.GetAttrValue(node, L"szRect2_150"),     // 热点矩形
            xml.GetAttrValue(node, L"szRect3_150"),     // 按下矩形
            xml.GetAttrValue(node, L"szRect4_150"),     // 禁止矩形

            xml.GetAttrValue(node, L"szRect1_175"),     // 正常矩形
            xml.GetAttrValue(node, L"szRect2_175"),     // 热点矩形
            xml.GetAttrValue(node, L"szRect3_175"),     // 按下矩形
            xml.GetAttrValue(node, L"szRect4_175"),     // 禁止矩形

            xml.GetAttrValue(node, L"szRect1_200"),     // 正常矩形
            xml.GetAttrValue(node, L"szRect2_200"),     // 热点矩形
            xml.GetAttrValue(node, L"szRect3_200"),     // 按下矩形
            xml.GetAttrValue(node, L"szRect4_200"),     // 禁止矩形
        };
        bool isWrite = crText[0] != 0 || crText[1] != 0 || crText[2] != 0 || crText[3] != 0;        // 文本颜色有一个不为空都写入配置
        isWrite = crBack[0] != 0 || crBack[1] != 0 || crBack[2] != 0 || crBack[3] != 0 || isWrite;  // 背景颜色有一个不为空都写入配置

        _ext_serialize_xml_write_string(serializeData, bufSize, offset, id);        // 写入ID
        _ext_serialize_xml_write_string(serializeData, bufSize, offset, text);      // 写入修改的文本
        _ext_serialize_xml_write_string(serializeData, bufSize, offset, remarks);   // 写入备注

        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[0]);    // 写入正常文本颜色
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[1]);    // 写入热点文本颜色
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[2]);    // 写入按下文本颜色
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crText[3]);    // 写入禁止文本颜色

        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[0]);    // 写入正常背景颜色
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[1]);    // 写入热点背景颜色
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[2]);    // 写入按下背景颜色
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, crBack[3]);    // 写入禁止背景颜色

        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // 备用1
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // 备用2
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // 备用3
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // 备用4
        _ext_serialize_xml_write_int(serializeData, bufSize, offset, 0);    // 备用5

        for (int i = 0; i < sizeof(szRect) / sizeof(szRect[0]); i++)
            isWrite = _ext_serialize_xml_write_rect(serializeData, bufSize, offset, szRect[i]) || isWrite;  // 只要有一个写入, 都需要写出配置

        count++;    // 数量+1
        // 只要没有写入任何矩形, 都回滚到写入ID之前, 下次继续从这个位置开始写入
        if (!isWrite)
        {
            count--;
            const int fillSize = offset - oldOffset;
            memset(serializeData.data() + oldOffset, 0, fillSize);  // 回滚清0
            offset = oldOffset;
        }

    }

    theme.dwCount = count;
    serializeData.resize(offset);
    return EXT_ERR::EXT_ERR_OK;
}



// 打包ext文件数据, 返回错误码, 实际数据由 ExtData 接收, pExtData为0则调用失败
// pngData = 图片数据指针
// pngDataSize = 图片数据大小
// xmlText = xml文件数据
// pExtData = 接收处理好的ext文件数据 结构 = NEW_THEME_FILE_HEADER + 压缩(图片数据尺寸 + 图片数据 + xml序列化数据尺寸 + xml序列化数据)
EXT_ERR PackExtData(const void* pngData, DWORD pngDataSize, const wchar_t* xmlText, _str* pExtData)
{
    if (!pExtData)return EXT_ERR::EXT_ERR_NULLRET;
    _str xmlData;
    THEME_FILE_HEADER_1_2 theme = { 0 };    // 打包数据始终使用最新版的结构, 解包数据根据传递的数据判断版本
    EXT_ERR extErr = _ext_serialize_xml(xmlText, xmlData, theme);
    if (extErr != EXT_ERR::EXT_ERR_OK)      // 把xml转成内存数据
        return extErr;

    theme.majorVersion = 1;
    theme.minorVersion = 2;
    theme.buildVersion = 810;
    DWORD xmlTextSize = (DWORD)xmlData.size();          // xml数据的尺寸
    size_t dataSize = pngDataSize + xmlTextSize + sizeof(DWORD) * 2;

    pExtData->resize(dataSize + sizeof(theme));        // 图片数据长度 + 图片数据 + 文本长度 + 文本数据 + 文件头尺寸
    LPSTR ptr = pExtData->data();
    // 被压缩的数据 = 图片尺寸 + 图片数据 + xml数据尺寸 + xml数据
    memcpy(ptr, &pngDataSize, sizeof(DWORD));   ptr += sizeof(DWORD);   // 写入图片尺寸
    memcpy(ptr, pngData, pngDataSize);          ptr += pngDataSize;     // 写入图片数据
    memcpy(ptr, &xmlTextSize, sizeof(DWORD));   ptr += sizeof(DWORD);   // 写入xml数据大小
    memcpy(ptr, xmlData.data(), xmlTextSize);   ptr += xmlTextSize;     // 写入xml数据

    Czlib zlib;
    uLongf compressSize = 0;
    Byte* pThemeData = zlib.compress(pExtData->c_str(), dataSize, &compressSize);    // 压缩好的数据
    if (compressSize > dataSize)    // 如果压缩后的尺寸比原尺寸还大, 那就得重新调整尺寸
        pExtData->resize(compressSize + sizeof(theme));
    ptr = pExtData->data();
    theme.dwDataLength = compressSize;

    DWORD offset = 0;
    memcpy(ptr + offset, &theme, sizeof(theme));    offset += sizeof(theme);
    memcpy(ptr + offset, pThemeData, compressSize); offset += compressSize;
    pExtData->resize(offset);
    zlib.free(&pThemeData);
    return EXT_ERR::EXT_ERR_OK;
}


// 解包ext文件数据, 返回错误码
// pExtData = ext文件数据
// pngData = 接收png图片数据
// xmlData = 接收xml序列化后的数据
EXT_ERR UnExtData(LPTHEME_FILE_HEADER pExtData, _str* pngData, _str* xmlData)
{
    if (!pngData && !xmlData)return EXT_ERR::EXT_ERR_NULLRET;
    // 结构 = THEME_FILE_HEADER + 压缩(图片数据尺寸 + 图片数据 + xml序列化数据尺寸 + xml序列化数据)
    if (!pExtData) return EXT_ERR::EXT_ERR_NULLEXTDATA;
    void* pCompressData = 0;
    if (pExtData->cbSize == sizeof(THEME_FILE_HEADER))  // 1.0 版本
    {
        if (pExtData->dwReserved != 0 || pExtData->dwCount == 0
            || pExtData->dwDataLength == 0 || pExtData->dwMaxSize == 0)
            return EXT_ERR::EXT_ERR_NULLEXTDATA;
        pCompressData = (void*)(pExtData + 1);
    }
    else if (pExtData->cbSize == sizeof(THEME_FILE_HEADER_1_1))  // 1.1版本
    {
        THEME_FILE_HEADER_1_1* data = (THEME_FILE_HEADER_1_1*)pExtData;

        if (data->dwReserved != 0 || data->dwCount == 0
            || data->dwDataLength == 0 || data->dwMaxSize == 0 || data->dwImageSize == 0)
            return EXT_ERR::EXT_ERR_NULLEXTDATA;
        pCompressData = (void*)(data + 1);  // 1.1的偏移量比1.0的要大, 偏移量后面的处理方式都一样
    }
    else if (pExtData->cbSize == sizeof(THEME_FILE_HEADER_1_2))  // 1.2版本
    {
        THEME_FILE_HEADER_1_2* data = (THEME_FILE_HEADER_1_2*)pExtData;

        if (data->dwReserved != 0 || data->dwCount == 0
            || data->dwDataLength == 0 || data->dwMaxSize == 0 || data->dwImageSize == 0)
            return EXT_ERR::EXT_ERR_NULLEXTDATA;
        pCompressData = (void*)(data + 1);  // 1.2的偏移量比1.1的要大, 偏移量后面的处理方式都一样
    }
    else
    {
        return EXT_ERR::EXT_ERR_NULLEXTDATA;    // 其他版本暂时不处理
    }
    Czlib zlib;
    Bytef* p = zlib.uncompress(pCompressData, 0);
    if (!p) return EXT_ERR::EXT_ERR_UNCOMPRESS;   // 解压失败就返回

    LPBYTE ptr = p;
    DWORD pngSize = (*((DWORD*)ptr));   // 前4个字节的图片尺寸
    ptr += sizeof(DWORD);   // 指针往后偏移, 指向图片数据
    if (pngData)pngData->assign((LPCSTR)ptr, pngSize);  // 把图片数据拷贝进变量里

    ptr += pngSize;     // 偏移加上图片尺寸, 走到这ptr指向xml数据长度
    DWORD textSize = (*((DWORD*)ptr));  // xml数据长度
    ptr += sizeof(DWORD);
    if (xmlData)xmlData->assign((LPCSTR)ptr, textSize); // 把xml数据拷贝进变量里
    zlib.free(&p);
    return EXT_ERR::EXT_ERR_OK;

}

LPCWSTR GetErrTextW(EXT_ERR err)
{
    switch (err)
    {
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_OK:           return L"";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_PARSEXML:     return L"序列号xml时解析xml失败";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLID:       return L"序列号xml时检测到有空ID";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLRET:      return L"没有接收返回值";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLEXTDATA:  return L"解包时传递了不合法的ext数据";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_UNCOMPRESS:   return L"解包时解压数据失败";
    default: return L"";
    }
}
LPCSTR GetErrTextA(EXT_ERR err)
{
    switch (err)
    {
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_OK:           return "";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_PARSEXML:     return "序列号xml时解析xml失败";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLID:       return "序列号xml时检测到有空ID";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLRET:      return "没有接收返回值";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_NULLEXTDATA:  return "解包时传递了不合法的ext数据";
    case THEME_NAMESPACE EXT_ERR::EXT_ERR_UNCOMPRESS:   return "解包时解压数据失败";
    default: return "";
    }
}

THEME_END_NAMESPACE