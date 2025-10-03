#pragma once
#include "DrawScroll.h"
#include <commctrl.h>
#include "WndBaseState.h"

//typedef struct OBJDATA
//{
//#ifdef __cplusplus
//    OBJDATA() { memset(this, 0, sizeof(*this)); }
//#endif
//    void* pWnd;
//    RECT rc;                        // 组件相对位置
//    RECT rcClient;                  // 组件客户区位置
//    RECT rcWnd;                     // 组件窗口位置
//    RECT rcDirty;                   // 组件脏区域
//    RECT rcText;                    // 组件文本偏移位置
//
//    COLORINFO cr;                   // 组件颜色
//    
//    void* hObj;                     // 组件句柄
//    void* hChildFirst;              // 第一个子组件
//    void* hChildLast;               // 最后一个子组件
//    void* hLayout;                  // 布局句柄
//    DWORD flags;                    // 组件标志
//    void* pBackgroundImage;         // 组件背景图片
//    void* hTheme;                   // 组件主题句柄
//
//    void* minmax_reserved_1;        // 备用1
//    void* minmax_reserved_2;        // 备用2
//    POINT minmax_maxsize;           // 组件最大尺寸
//    POINT minmax_maxpostion;        // 组件
//    POINT minmax_mintracksize;
//    POINT minmax_maxtracksize;
//
//    void* hPathWindow;            
//    void* hPathClient;    
//    float radius_uint;
//    RECT_F radius;
//    DWORD propCount;
//    void* objNext;
//    void* objPrev;
//    void* objnextTabstop;
//    void* objVScroll;
//    void* objHScroll;
//    LPWSTR pTitle;
//    LPWSTR pTips;
//    void* pObjJS;
//    void* hTableJsEvents;
//    int state;
//    void* OwnerData;
//    LPCANVAS hCanvas;
//    void* userData;
//    LONG_PTR styleEx;
//    LONG_PTR style;
//    LPEX_FONT hFont;
//    LPWSTR atomName;
//    HCURSOR hCursor;
//    void* pPropListEntry;
//    LPWSTR pCls;
//    void* fnClsProc;
//    LONG_PTR id;
//    DWORD textFormat;
//    LPEX_RGN hRgnSizebox;
//    void* objParent;
//    void* lParam;
//    DWORD AlphaDisable;
//    DWORD Alpha;
//    void* fnSubClass;
//    float fHUE;
//    float fBlur;
//    LONG_PTR nodeid;
//}*LPOBJDATA;