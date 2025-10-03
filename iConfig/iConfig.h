// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <IDraw_Header.h>
#include <xml2/tinyxml2.h>



// 初始化配置类, 会把值都初始化好, 返回类指针
IConfig* _window_get_config();

// 配置被改变, 设置配置项的时候会调用这个函数, 由这个函数转发配置改变事件
void WINAPI iConfig_InfoChanged(int index, LONG_PTR oldValue, LONG_PTR newValue, bool isUpdateWindow);

// 重新读取自绘选择夹颜色配置
// isFirst = 是否首次读取, 如果是 SetInfo设置的话, 需要设置为true
//           如果为true, 重新读取发现配置不同也不会触发配置被改变事件
//           如果为false, 则重新读取后有配置不同的就会触发配置被改变事件
void iConfig_Reload_CustomTabColor(bool isFirst);

#endif //PCH_H
