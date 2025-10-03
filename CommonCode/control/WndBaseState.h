#pragma once

//    normal                = 0x00000000,    // 正常
//    ban                   = 0x00000001,    // 禁止
//    selected              = 0x00000002,    // 选择
//    focus                 = 0x00000004,    // 焦点
//    down                  = 0x00000008,    // 按下
//    checked               = 0x00000010,    // 选中
//    indeterMinate         = 0x00000020,    // 半选中
//    readonly              = 0x00000040,    // 只读
//    hot                   = 0x00000080,    // 点燃
//    def                   = 0x00000100,    // 默认
//    subShow               = 0x00000200,    // 子项目可视
//    subHide               = 0x00000400,    // 子项目隐藏
//    busying               = 0x00000800,    // 繁忙中
//    zhanwei               = 0x00001000,    // 占位
//    rolling               = 0x00002000,    // 滚动中
//    effecting             = 0x00004000,    // 动画中
//    hide                  = 0x00008000,    // 隐藏
//    zhanwei1              = 0x00010000,    // 占位
//
//    allowModifySize       = 0x00020000,    // 允许修改尺寸
//    allowDrag             = 0x00040000,    // 允许拖动
//    zhanwei1              = 0x00080000,    // 占位
//    allowFocus            = 0x00100000,    // 允许焦点
//    allowSelect           = 0x00200000,    // 允许选择
//    allowHrefHot          = 0x00400000,    // 允许超链接点燃
//    allowHrefEnd          = 0x00800000,    // 允许超链接已访问
//    allowMulti            = 0x01000000,    // 允许多选
//    zhanwei3              = 0x02000000,    // 占位
//    zhanwei4              = 0x04000000,    // 占位
//    zhanwei5              = 0x08000000,    // 占位
//    zhanwei6              = 0x10000000,    // 占位
//    password              = 0x20000000,    // 密码模式


enum STATE
{
    NORMAL                  = 0X00000000,    // 正常
    BAN                     = 0X00000001,    // 禁止
    SELECTED                = 0X00000002,    // 选择
    FOCUS                   = 0X00000004,    // 焦点
    DOWN                    = 0X00000008,    // 按下
    CHECKED                 = 0X00000010,    // 选中
    INDETERMINATE           = 0X00000020,    // 半选中
    READONLY                = 0X00000040,    // 只读
    HOT                     = 0X00000080,    // 点燃
    DEF                     = 0X00000100,    // 默认
    SUBSHOW                 = 0X00000200,    // 子项目可视
    SUBHIDE                 = 0X00000400,    // 子项目隐藏
    BUSYING                 = 0X00000800,    // 繁忙中
    HOVER                   = 0X00001000,    // 进入
    ROLLING                 = 0X00002000,    // 滚动中
    EFFECTING               = 0X00004000,    // 动画中
    HIDE                    = 0X00008000,    // 隐藏
    DOWNDROP                = 0X00010000,    // 下拉按钮被按下

    ALLOWMODIFYSIZE         = 0X00020000,    // 允许修改尺寸
    ALLOWDRAG               = 0X00040000,    // 允许拖动
    ZHANWEI2                = 0X00080000,    // 占位
    ALLOWFOCUS              = 0X00100000,    // 允许焦点
    ALLOWSELECT             = 0X00200000,    // 允许选择
    ALLOWHREFHOT            = 0X00400000,    // 允许超链接点燃
    ALLOWHREFEND            = 0X00800000,    // 允许超链接已访问
    ALLOWMULTI              = 0X01000000,    // 允许多选
    ZHANWEI3                = 0X02000000,    // 占位
    ZHANWEI4                = 0X04000000,    // 占位
    ZHANWEI5                = 0X08000000,    // 占位
    ZHANWEI6                = 0X10000000,    // 占位
    PASSWORD                = 0X20000000,    // 密码模式
};
