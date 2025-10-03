#pragma once
#include <tstr.h>
#include <d2d/d2d_old.h>
#include "WndBaseState.h"

#define EWF_bRendered 16384 


#pragma pack(1)
typedef struct SYSBUTTONIMAGE
{
    WORD flag;                  // 文件标志 19278 NK
    BYTE MajorVersion;          // 生成主题文件主版本号
    BYTE MinorVersion;          // 生成主题文件次版本号
    BYTE btnWidth;              // 系统按钮宽度
    BYTE btnHeight;             // 系统按钮高度
    BYTE count;                 // 系统按钮图片数量
    BYTE Reservations;          // 保留
    DWORD imgSize;              // 图片尺寸
    WORD imgWidth;              // 画布宽度, 主题文件图片宽度
    WORD imgHeight;             // 画布高度, 主题文件图片高度
    DWORD offset;               // 图片偏移量
    void* v1;                   // 保留
#ifndef _M_X64
    void* v2;                   // 保留
#endif
    RECT_F rc[48];              // 按钮在图片中的位置, 12个系统按钮,每个按钮4个状态, 一个48个
}*LPSYSBUTTONIMAGE;
#pragma pack()



struct SYSBUTTON
{
    RECT rc;                    // 系统按钮位置
    DWORD flag;                 // 标志
    DWORD ID;                   // 编号, 根据编号绘画不同图片
};

typedef struct tagSYSBTN
{
    LPSYSBUTTONIMAGE image;         // 所有系统按钮绘画位置和图片指针
    LPEX_IMAGE pImage;              // 绘画图片指针
    LPEX_IMAGE pImage_light;        // 绘画图片指针, 浅色模式使用的图片
    SYSBUTTON btn[10];              // 系统按钮组, 最多允许10个, 静态变量, 会清0
    DWORD count;                    // 当前程序使用的系统按钮数量
    DWORD index;                    // 当前绘画的是哪个按钮
    STATE state;                    // 系统按钮当前状态, 只能有一个按钮有状态
    DWORD downIndex;                // 系统按钮按下索引
    DWORD rdownIndex;               // 系统按钮右键按下索引
    RECT rcBtn;                     // 整个系统按钮位置
    RECT rcCaption;                 // 标题栏位置
    RECT rcIcon;                    // 图标位置
}SYSBTN, * LPSYSBTN;

