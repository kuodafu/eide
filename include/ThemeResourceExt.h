#pragma once
#include "ThemeResource.h"

THEME_BEGIN_NAMESPACE

enum class EXT_ERR
{
    EXT_ERR_OK,
    EXT_ERR_PARSEXML,       // 序列号xml时解析xml失败
    EXT_ERR_NULLID,         // 序列号xml时检测到有空ID
    EXT_ERR_NULLRET,        // 没有接收返回值
    EXT_ERR_NULLEXTDATA,    // 解包时传递了不合法的ext数据
    EXT_ERR_UNCOMPRESS,     // 解包时解压数据失败

};

// 打包ext文件数据, 返回错误码, 实际数据由 ExtData 接收, pExtData为0则调用失败
// pngData = 图片数据指针
// pngDataSize = 图片数据大小
// xmlText = xml文件数据
// pExtData = 接收处理好的ext文件数据 结构 = NEW_THEME_FILE_HEADER + 压缩(图片数据尺寸 + 图片数据 + xml序列化数据尺寸 + xml序列化数据)
EXT_ERR PackExtData(const void* pngData, DWORD pngDataSize, const wchar_t* xmlText, _str* pExtData);

// 解包ext文件数据, 返回错误码
// pExtData = ext文件数据
// pngData = 接收png图片数据
// xmlData = 接收xml序列化后的数据
EXT_ERR UnExtData(LPTHEME_FILE_HEADER pExtData, _str* pngData, _str* xmlData);

LPCWSTR GetErrTextW(EXT_ERR err);
LPCSTR GetErrTextA(EXT_ERR err);


THEME_END_NAMESPACE