#pragma once


#define EWS_BUTTON_CLOSE            (LONG_PTR)0x00000001    // 窗体风格_关闭按钮
#define EWS_BUTTON_MAX              (LONG_PTR)0x00000002    // 窗体风格_最大化按钮
#define EWS_BUTTON_MIN              (LONG_PTR)0x00000004    // 窗体风格_最小化按钮
#define EWS_BUTTON_MENU             (LONG_PTR)0x00000008    // 窗体风格_菜单按钮
#define EWS_BUTTON_SKIN             (LONG_PTR)0x00000010    // 窗体风格_皮肤按钮
#define EWS_BUTTON_SETTING          (LONG_PTR)0x00000020    // 窗体风格_设置按钮
#define EWS_BUTTON_HELP             (LONG_PTR)0x00000040    // 窗体风格_帮助按钮
#define EWS_HASICON                 (LONG_PTR)0x00000080    // 窗体风格_图标
#define EWS_TITLE                   (LONG_PTR)0x00000100    // 窗体风格_标题
#define EWS_FULLSCREEN              (LONG_PTR)0x00000200    // 窗体风格_全屏模式.设置该标记窗口最大化时, 任务栏将被遮挡.
#define EWS_SIZEABLE                (LONG_PTR)0x00000400    // 窗体风格_允许调整尺寸
#define EWS_MOVEABLE                (LONG_PTR)0x00000800    // 窗体风格_允许随意移动
#define EWS_NOSHADOW                (LONG_PTR)0x00001000    // 窗体风格_不显示窗口阴影
#define EWS_NOINHERITBKG            (LONG_PTR)0x00002000    // 窗体风格_不继承父窗口背景数据
#define EWS_NOTABBORDER             (LONG_PTR)0x00004000    // 窗体风格_不显示TAB焦点边框
#define EWS_ESCEXIT                 (LONG_PTR)0x00008000    // 窗体风格_ESC关闭窗口
#define EWS_MAINWINDOW              (LONG_PTR)0x00010000    // 窗体风格_主窗口(拥有该风格时, 当窗口被关闭, 会调用PostQuitMessage()退出消息循环)
#define EWS_CENTERWINDOW            (LONG_PTR)0x00020000    // 窗体风格_窗口居中(如果有父窗口, 则在父窗口中间, 否则为屏幕中间)
#define EWS_NOCAPTIONTOPMOST        (LONG_PTR)0x00040000    // 窗体风格_标题栏取消置顶
#define EWS_BLUR                    (LONG_PTR)0x00080000    // 窗体风格_模糊背景图片

#define EWS_MENU                    (LONG_PTR)0x40000000
#define EWS_MESSAGEBOX              (LONG_PTR)0x80000000



#define EOS_DISABLENOSCROLL         (LONG_PTR)0x02000000    // 组件风格_滚动条不可用时显示禁止状态
#define EOS_SIZEBOX                 (LONG_PTR)0x04000000    // 组件风格_可调整尺寸
#define EOS_DISABLED                (LONG_PTR)0x08000000    // 组件风格_禁止
#define EOS_VISIBLE                 (LONG_PTR)0x10000000    // 组件风格_可视
#define EOS_BORDER                  (LONG_PTR)0x20000000    // 组件风格_边框
#define EOS_VSCROLL                 (LONG_PTR)0x40000000    // 组件风格_水平滚动条
#define EOS_HSCROLL                 (LONG_PTR)0x80000000    // 组件风格_垂直滚动条

#define EOS_EX_DLGMODE              (LONG_PTR)0x00200000    // 组件风格_扩展_对话框模式
#define EOS_EX_AUTOSIZE             (LONG_PTR)0x00400000    // 组件风格_扩展_自适应尺寸
#define EOS_EX_TRANSPARENT          (LONG_PTR)0x00800000    // 组件风格_扩展_鼠标穿透
#define EOS_EX_BLUR                 (LONG_PTR)0x01000000    // 组件风格_扩展_背景模糊
#define EOS_EX_DRAGDROP             (LONG_PTR)0x02000000    // 组件风格_扩展_允许拖拽
#define EOS_EX_ACCEPTFILES          (LONG_PTR)0x04000000    // 组件风格_扩展_接收文件拖放
#define EOS_EX_FOCUSABLE            (LONG_PTR)0x08000000    // 组件风格_扩展_允许焦点
#define EOS_EX_TABSTOP              (LONG_PTR)0x10000000    // 组件风格_扩展_允许TAB焦点
#define EOS_EX_TOPMOST              (LONG_PTR)0x20000000    // 组件风格_扩展_总在最前
#define EOS_EX_COMPOSITED           (LONG_PTR)0x40000000    // 组件风格_扩展_背景混合
#define EOS_EX_CUSTOMDRAW           (LONG_PTR)0x80000000    // 组件风格_扩展_自定义绘制
