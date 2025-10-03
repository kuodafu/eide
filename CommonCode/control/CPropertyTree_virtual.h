#pragma once
#include <tstr.h>
#include <commctrl.h>

// 文件头
#define LIST_PROP_HEAD_FLAG_COLOR       0x00000001      // 有配色信息
#define LIST_PROP_HEAD_FLAG_LIST        0x00000002      // 有列表数据
#define LIST_PROP_HEAD_FLAG_EXPAND      0x00000004      // 有树型框左边的加减框按钮图标数据

#define LIST_PROP_MAJORVERSION          1       // 主版本号
// 2021-01-05 新增1.1, 表项数据新增加一个说明文本
#define LIST_PROP_MINORVERSION          1       // 次版本号     1.0 2020-09-27, 如果改变文件头, 需要更新版本号
#pragma pack(1)

struct LIST_PROP_HEAD
{
    unsigned char header[2];        // 文件头标识 'P', 'D'
    unsigned char MajorVersion;     // 主版本号
    unsigned char MinorVersion;     // 次版本号
    short         bfReserved1;      // 备用1
    short         bfReserved2;      // 备用2
    unsigned int  flags;            // 文件结构标志
};



struct LIST_PROP_DATA
{
    int type;                       // 项目类型
    int index;                      // 组合框当前选中项
    __int64 param;                  // 项目数值
    int subItemCount;               // 子项目树, 组合框数量
    int nameLen;                    // 属性名长度
    int valueLen;                   // 属性值长度
};

struct LIST_PROP_DATA_1_1 : public LIST_PROP_DATA
{
    int textLen;                    // 说明文本长度
};
#pragma pack()

enum PROPERTY_RECT
{
    PROPERTY_RECT_ITEM,     // 项目矩形
    PROPERTY_RECT_WINDOW,   // 属性框矩形
    PROPERTY_RECT_NAME,     // 属性名矩形
    PROPERTY_RECT_VALUE,    // 属性值矩形
    PROPERTY_RECT_BUTTON,   // 右边按钮矩形
    PROPERTY_RECT_EXPAND,   // 扩展按钮矩形
};

enum PROPERTY_ICONTYPE
{
    PROPERTY_ICONTYPE_TREE_ENPAND                   = 0x0000,
    PROPERTY_ICONTYPE_TREE_UNENPAND                 = 0x0001,
    PROPERTY_ICONTYPE_CHECKBOX_SEL                  = 0x0010,
    PROPERTY_ICONTYPE_CHECKBOX_UNSEL                = 0x0011,
    PROPERTY_ICONTYPE_CHECKBOX_INDETERMINATE        = 0x0012,
    PROPERTY_ICONTYPE_RADIO_SEL                     = 0x0020,
    PROPERTY_ICONTYPE_RADIO_UNSEL                   = 0x0021,
    PROPERTY_ICONTYPE_SWITCH_SEL                    = 0x0030,
    PROPERTY_ICONTYPE_SWITCH_UNSEL                  = 0x0031,
};

// 前1个字节记录基本类型, 不可重复, 第二个字节记录数值型的类型, 不是数值型不使用, 高16位记录样式
enum PROPERTY_TYPE
{
    PROPERTY_FLAG_BUTTON                    = 0x00010000,   // 有普通按钮
    PROPERTY_FLAG_BUTTON_COMBOBOX           = 0x00020000,   // 有下拉按钮, 与PROPERTY_FLAG_BUTTON不能同时使用, 如果一起使用则当成是PROPERTY_FLAG_BUTTON
    PROPERTY_FLAG_NOEDIT                    = 0x00040000,   // 不可编辑, 不要显示编辑框
    PROPERTY_FLAG_INDEX                     = 0x00080000,   // 有编号, 只对组合框有效
    PROPERTY_FLAG_CUSTOM                    = 0x00100000,   // 自定义属性值, 属性值显示的文本通过回调来进行设置
    PROPERTY_FLAG_UPDOWN                    = 0x00200000,   // 上下调节器
    PROPERTY_FLAG_LEFTRIGHT                 = 0x00400000,   // 左右调节器
    PROPERTY_FLAG_NOMOUSEWHEEL              = 0x00800000,   // 滚动鼠标滚轮时不处理滚动, 而是让属性框滚动
    PROPERTY_FLAG_DISABLED                  = 0x01000000,   // 禁止状态
    PROPERTY_FLAG_CHECKLINESEL              = 0x02000000,   // 选择类型整行选择, 没有这个选项的话只有在图标的位置点击才会选择


    PROPERTY_TYPE_GROUP     = 0,    // 分组, 父项目
    PROPERTY_TYPE_TEXT      = 1,    // 文本类型, 没有后面的小按钮
    PROPERTY_TYPE_NUMBER    = 2,    // 数值类型, 使用文本记录, 用户取值时转换到对应类型, 配合高位 来确定使用什么类型, 在输入的时候会处理

    ___PROPERTY_TYPE_START = 0x7F,  // 组合类型开始

    PROPERTY_FLAG_COMBOBOX      = ___PROPERTY_TYPE_START + 1,   // 组合框类型
    PROPERTY_FLAG_COLOR         = ___PROPERTY_TYPE_START + 2,   // 颜色选择器
    PROPERTY_FLAG_FILE          = ___PROPERTY_TYPE_START + 3,   // 文件选择器
    PROPERTY_FLAG_FONT          = ___PROPERTY_TYPE_START + 4,   // 字体选择器
    PROPERTY_FLAG_BIN           = ___PROPERTY_TYPE_START + 5,   // 字节集
    PROPERTY_FLAG_PIC           = ___PROPERTY_TYPE_START + 6,   // 图片
    PROPERTY_FLAG_MUSIC         = ___PROPERTY_TYPE_START + 7,   // 音频选择器
    PROPERTY_FLAG_IMGLIST       = ___PROPERTY_TYPE_START + 8,   // 图片组选择器
    PROPERTY_FLAG_LINK          = ___PROPERTY_TYPE_START + 9,   // 链接
    PROPERTY_FLAG_CHECKBOX      = ___PROPERTY_TYPE_START + 10,  // 选择框
    PROPERTY_FLAG_CHECKBOX3     = ___PROPERTY_TYPE_START + 11,  // 3态选择框
    PROPERTY_FLAG_RADIO         = ___PROPERTY_TYPE_START + 12,  // 单选框
    PROPERTY_FLAG_AUTORADIO     = ___PROPERTY_TYPE_START + 13,  // 自动单选框
    PROPERTY_FLAG_SWITCH        = ___PROPERTY_TYPE_START + 14,  // 开关

    PROPERTY_FLAG_BYTE          = 0x0100,   // 只对数值型有效, 只允许输入字节型, 0x00 - 0xFF
    PROPERTY_FLAG_SHORT         = 0x0200,   // 只对数值型有效, 只允许输入短整数, 0x0000 - 0xFFFF
    PROPERTY_FLAG_INT           = 0x0300,   // 只对数值型有效, 只允许输入整数,  0x00000000 - 0xFFFFFFFF
    PROPERTY_FLAG_INT64         = 0x0400,   // 只对数值型有效, 只允许输入长整数, 0x0000000000000000 - 0xFFFFFFFFFFFFFFFF
    PROPERTY_FLAG_FLOAT         = 0x0500,   // 只对数值型有效, 只允许输入浮点数
    PROPERTY_FLAG_DOUBLE        = 0x0600,   // 只对数值型有效, 只允许输入双精度浮点数
    PROPERTY_FLAG_UNSIGNED      = 0x1000,   // 只对数值型有效, 无符号数值

    PROPERTY_TYPE_COMBOBOX      = PROPERTY_FLAG_COMBOBOX | PROPERTY_FLAG_BUTTON_COMBOBOX,       // 组合框类型, 用一个数组存放数据
    PROPERTY_TYPE_COLOR         = PROPERTY_FLAG_COLOR | PROPERTY_FLAG_BUTTON,                   // 颜色选择器, 单击按钮后探测颜色选择器, 一个DWORD 保存
    PROPERTY_TYPE_FILE          = PROPERTY_FLAG_FILE | PROPERTY_FLAG_BUTTON,                    // 文件选择器, 单击后探测打开文件对话框
    PROPERTY_TYPE_FONT          = PROPERTY_FLAG_FONT | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,      // 字体选择对话框, 单击后探测打开选择字体对话框, 数据 = LPLOGFONTW + LPCHOOSEFONTW, 两个结构, 固定长度
    PROPERTY_TYPE_BIN           = PROPERTY_FLAG_BIN | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,       // 字节集数据, 数据存放和易语言一样, {1,0,0,0,字节集长度}
    PROPERTY_TYPE_PIC           = PROPERTY_FLAG_PIC | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,       // 图片数据, 弹出个对话框, 对话框显示图片, 外加几个按钮选择图片文件
    PROPERTY_TYPE_MUSIC         = PROPERTY_FLAG_MUSIC | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,     // 音频数据, 弹出个对话框, 对话框显示音频信息, 外加几个按钮试听, 替换删除
    PROPERTY_TYPE_IMGLIST       = PROPERTY_FLAG_IMGLIST | PROPERTY_FLAG_BUTTON | PROPERTY_FLAG_NOEDIT | PROPERTY_FLAG_CUSTOM,   // 图片组, 弹出个对话框, 对话框显示图片组所有图片, 外加添加删除替换等按钮


    // 有符号数值

    PROPERTY_TYPE_NUMBER_BYTE   = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_BYTE,    // 有符号字节
    PROPERTY_TYPE_NUMBER_SHORT  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_SHORT,   // 有符号短整数
    PROPERTY_TYPE_NUMBER_INT    = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_INT,     // 有符号整数
    PROPERTY_TYPE_NUMBER_INT64  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_INT64,   // 有符号长整数
    PROPERTY_TYPE_NUMBER_FLOAT  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_FLOAT,   // 有符号小数
    PROPERTY_TYPE_NUMBER_DOUBLE = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_DOUBLE,  // 有符号双精度小数

    PROPERTY_TYPE_NUMBER_UBYTE  = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_BYTE,   // 无符号字节
    PROPERTY_TYPE_NUMBER_USHORT = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_SHORT,  // 无符号短整数
    PROPERTY_TYPE_NUMBER_UINT   = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_INT,    // 无符号整数
    PROPERTY_TYPE_NUMBER_UINT64 = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_INT64,  // 无符号长整数
    PROPERTY_TYPE_NUMBER_UFLOAT = PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_FLOAT,  // 无符号小数
    PROPERTY_TYPE_NUMBER_UDOUBLE= PROPERTY_TYPE_NUMBER | PROPERTY_FLAG_UNSIGNED | PROPERTY_FLAG_DOUBLE, // 无符号双精度小数


    PROPERTY_TYPE_TEXT_BUTTON           = PROPERTY_TYPE_TEXT | PROPERTY_FLAG_BUTTON,                                // 文本类型, 带后面的小按钮
    PROPERTY_TYPE_TEXT_LONG             = PROPERTY_TYPE_TEXT_BUTTON | PROPERTY_FLAG_CUSTOM | PROPERTY_FLAG_NOEDIT,  // 长文本类型, 后面有小按钮+自定义显示属性值+不可编辑
    PROPERTY_TYPE_COMBOBOX_NOEDIT       = PROPERTY_TYPE_COMBOBOX | PROPERTY_FLAG_NOEDIT,                            // 不可编辑组合框
    PROPERTY_TYPE_COMBOBOX_NOEDIT_INDEX = PROPERTY_TYPE_COMBOBOX_NOEDIT | PROPERTY_FLAG_INDEX,                      // 不可编辑组合框+编号


    PROPERTY_TYPE_UNKNOWN = -1,         // 未知
};



typedef void(CALLBACK* fnPropertyChanged)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef void(CALLBACK* fnItemChanged)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int downType);
typedef bool(CALLBACK* fnPropertyReturn)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnComboboxMouseWheel)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int count, int pos, int key, LPARAM lParam);
typedef bool(CALLBACK* fnComboboxChanged)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int index);
typedef bool(CALLBACK* fnDropWown)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnItemButtonClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnItemRButtonClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnComboboxLDoubleClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int index, int count);
typedef bool(CALLBACK* fnEditLDoubleClick)(void* param, HTREEITEM hItem, PROPERTY_TYPE type);
typedef bool(CALLBACK* fnItemChar)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int ch, int* pRet);
typedef bool(CALLBACK* fnItemMakeCustomValue)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, LPWSTR buffer);
typedef bool(CALLBACK* fnItemRButtonPaint)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, HDC hdc, int state, int width, int height);
typedef bool(CALLBACK* fnItemDelUp)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, bool isName);
typedef void(CALLBACK* fnCustomValueName)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, LPWSTR buffer);
typedef void(CALLBACK* fnItemReadError)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, int errCode);
typedef bool(CALLBACK* fnItemBeginDraw)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, HDC hdc);  // 开始绘画
typedef bool(CALLBACK* fnItemEndDraw)(void* param, HTREEITEM hItem, PROPERTY_TYPE type, HDC hdc);    // 结束绘画

typedef DWORD ARGB;
// 配色信息
struct PROPERTY_COLOR
{
    LOGFONTW lf;                    // 字体信息结构
    ARGB crBackground;              // 背景颜色, 编辑框/项目背景
    ARGB crBackParent;              // 父项目和左边空白部分的颜色
    ARGB crText;                    // 文本颜色
    ARGB crListBorder;              // 组合框列表边框颜色
    ARGB crSelect;                  // 右边按钮的背景颜色
    ARGB crDisabled;                // 文本禁止颜色
    DWORD ItemHeight;               // 单个项目高度
    ARGB Reserved[12];              // 备用
    int ratioWidth;                 // 宽度千分比, 左边的, 右边的千分比 = 1000 - 左边的千分比
    //PROPERTY_COLOR() { sizeof(*this); }
};


struct IPROPERTY_ITEM
{
    wstr name;          // 属性名
    wstr value;         // 文本型属性值
    wstr text;          // 说明文本, 1.1新增
    LPARAM param;       // 项目数值
    int index;          // 组合框选中项
    PROPERTY_TYPE type;
    IPROPERTY_ITEM() { param = 0; index = 0; type = PROPERTY_TYPE_GROUP; }
    // 返回当前组合框项目数
    virtual size_t AddComboboxData(LPCWSTR name, LPARAM param) = 0;
    virtual bool DelComboboxData(int index) = 0;
    virtual LPCWSTR GetComboboxText(int index) = 0;
    virtual LPARAM GetComboboxParam(int index) = 0;

    // 获取组合框数组数据, 需要外部提供缓冲区
    virtual int GetComboboxArray(IPROPERTY_ITEM**) = 0;
    // 获取当前组合框项目数
    virtual int GetComboboxCount() = 0;


};

typedef BOOL(CALLBACK* PFN_EnumMap)(LPVOID ptr, LPVOID name, LPVOID value);
typedef struct MAP_DATA_STRUCT
{
    LPVOID name;
    LPVOID value;
}*LPMAP_DATA_STRUCT;

class ICMap
{
public:
    virtual int Addref() = 0;
    virtual int Release() = 0;

    // 从map里获取指定KEY对应的值, 如果没有这个key, 内部会创建一个
    virtual LPVOID& GetMapValue(LPVOID key) = 0;
    // 给指定key设置一个新的值
    virtual void SetMapValue(LPVOID key, LPVOID value) = 0;
    // 从map里查询指定KEY对应的值, 如果没有这个key不会创建
    virtual LPVOID QueryMapValue(LPVOID key) = 0;

    // 从map里删除指定KEY对应的值
    virtual bool erase(LPVOID key) = 0;
    // 获取map成员数
    virtual int size() = 0;

    // 清空map
    virtual void clear() = 0;


    // 枚举当前所有键名和键值, 返回写入缓冲区的成员数
    // buffer 缓冲区, 需要外部申请缓冲区, 为0则返回需要的缓冲区数量
    // size = 缓冲区数量
    virtual int enumValue(LPMAP_DATA_STRUCT buffer, int size) = 0;

    // 枚举当前所有键名和键值, 返回写入缓冲区的成员数
    // pfnCallback = 枚举到成员后调用的回调函数, 为0则返回成员数
    // pArg = 传递到回调函数里的参数
    virtual int enumValueEx(PFN_EnumMap pfnCallback, LPVOID pArg) = 0;

};


class ICPropertyTree
{
public:
    virtual LPCWSTR className() = 0;
    virtual void SetItemHeight(int height, bool isUpdate = false) = 0;
    virtual int GetItemHeight() = 0;

    virtual int Addref() = 0;
    virtual int Release() = 0;

public:
    virtual bool SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size) = 0;
public:
    // 是否为分组
    virtual bool isGroup(PROPERTY_TYPE type) = 0;
    // 是否有按钮
    virtual bool isButton(PROPERTY_TYPE type) = 0;
    // 是否有下拉按钮
    virtual bool isButtonCombobox(PROPERTY_TYPE type) = 0;
    // 是否为数值
    virtual bool isNumber(PROPERTY_TYPE type) = 0;
    // 是否为文本, 长文本, 带按钮文本都返回true
    virtual bool isText(PROPERTY_TYPE type) = 0;

    // 以下为扩展类型, 由几个样式组合而成

    // 是否为带按钮文本
    virtual bool isTextButton(PROPERTY_TYPE type) = 0;
    // 是否为长文本
    virtual bool isLongText(PROPERTY_TYPE type) = 0;
    // 是否为不可编辑组合框
    virtual bool isComboboxNoEdit(PROPERTY_TYPE type) = 0;
    // 是否为不可编辑组合框
    virtual bool isComboboxNoEditIndex(PROPERTY_TYPE type) = 0;
    // 是否为组合框, 不可编辑和带编号的都返回真, 需要判断对应的就使用对应的方法
    virtual bool isCombobox(PROPERTY_TYPE type) = 0;
    // 是否为颜色选择器
    virtual bool isColorDialog(PROPERTY_TYPE type) = 0;
    // 是否为文件选择器
    virtual bool isFileDialog(PROPERTY_TYPE type) = 0;
    // 是否为字体选择器
    virtual bool isFontDialog(PROPERTY_TYPE type) = 0;
    // 是否为字节集
    virtual bool isBinDialog(PROPERTY_TYPE type) = 0;
    // 是否为图片选择器
    virtual bool isPicDialog(PROPERTY_TYPE type) = 0;
    // 是否为音频选择器
    virtual bool isMusicDialog(PROPERTY_TYPE type) = 0;
    // 是否为图片组选择器
    virtual bool isImgListDialog(PROPERTY_TYPE type) = 0;
    // 是否为链接框
    virtual bool IsLink(PROPERTY_TYPE type) = 0;
    // 是否为选择框
    virtual bool IsCheckBox(PROPERTY_TYPE type) = 0;
    // 是否为3态选择框
    virtual bool IsCheckBox3(PROPERTY_TYPE type) = 0;
    // 是否为单选框
    virtual bool IsRadio(PROPERTY_TYPE type) = 0;
    // 是否为自动单选框
    virtual bool IsAutoRadio(PROPERTY_TYPE type) = 0;
    // 是否为开关
    virtual bool IsSwitch(PROPERTY_TYPE type) = 0;
    // 是否为选择类型组件, 选择框/3态选择框, 单选框, 自动单选框, 开关, 都属于选择类型
    virtual bool IsCheckType(PROPERTY_TYPE type) = 0;

    // 1=字节,2=短整数,3=整数,4=长整数,5=小数,6=双精度小数, 不会返回其他值
    virtual int  GetNumberType(PROPERTY_TYPE type) = 0;
    // 是否有符号
    virtual bool isUnsigned(PROPERTY_TYPE type) = 0;
    // 是否可编辑
    virtual bool isEdit(PROPERTY_TYPE type) = 0;
    // 是否有编号
    virtual bool isIndex(PROPERTY_TYPE type) = 0;
    // 是否自定义属性
    virtual bool isCustom(PROPERTY_TYPE type) = 0;

    // 是否禁止
    virtual bool isDisabled(PROPERTY_TYPE type) = 0;
    // 是否可用
    virtual bool isEnabled(PROPERTY_TYPE type) = 0;

    // 不响应滚动消息, 并且把消息转发到窗口上
    virtual bool isNoMouseWheel(PROPERTY_TYPE type) = 0;


public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0) = 0;

public:
    // 写入字节集, 传递数据指针和长度, 内部会转成易语言的格式
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData, const size_t size) = 0;

    // 写入字节集, 传递数据必须是易语言的格式
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData) = 0;

    // 写入字体, 字体存放的数据传递是固定的, 两个结构, LOGFONTW + CHOOSEFONTW, 内部会修改字体大小/字符高度
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPLOGFONTW lf) = 0;

    // 写入字体, 传入CHOOSEFONTW结构指针, 内部会修改字体大小/字符高度
    // CHOOSEFONTW 指向的 LOGFONT结构有值的话也会吧这个结构拷贝保存起来
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPCHOOSEFONTW pcf) = 0;

    // 写入字体, 传递gdi字体指针, 内部会修改字体大小/字符高度
    virtual bool SetFontData(IPROPERTY_ITEM& data, HFONT hFont) = 0;


    // 写入字体, 字体存放的数据传递是固定的, 两个结构, LOGFONTW + CHOOSEFONTW
    virtual bool SetFontData(IPROPERTY_ITEM& data, const wchar_t* name = L"微软雅黑", int lfHeight = -12,
        bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) = 0;

    // 获取字体数据, 内部有更新 LPCHOOSEFONTW 结构指向的logfont地址
    virtual LPLOGFONTW GetFontData(IPROPERTY_ITEM& data, LPCHOOSEFONTW* pcf) = 0;

    // 根据项目句柄获取项目存放项目的数据指针
    virtual IPROPERTY_ITEM* GetItemData(HTREEITEM hItem) = 0;


    // 设置对话框属性, 为0则不设置对应的属性
    // type = 要设置的对话框类型, 目前只支持 PROPERTY_FLAG_FILE文件选择器, PROPERTY_FLAG_BIN字节集选择器
    // filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3\0媒体文件(*.mpg)|*.mpg”
    // defName = 默认文件名
    // title = 窗口标题
    // path = 运行目录
    // notChengedPath = 不改变目录, 需要传递变量指针, 指针为0则不设置
    virtual bool SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, bool* noChangePath = 0) = 0;

    // 获取对话框属性, 为0则不获取对应的属性
    // type = 要设置的对话框类型, 目前只支持 PROPERTY_FLAG_FILE文件选择器, PROPERTY_FLAG_BIN字节集选择器
    // filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3\0媒体文件(*.mpg)|*.mpg”
    // defName = 默认文件名
    // title = 窗口标题
    // path = 运行目录
    // notChengedPath = 不改变目录
    virtual bool GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter = 0, LPCWSTR* title = 0, LPCWSTR* defName = 0, LPCWSTR* path = 0, bool* noChangePath = 0) = 0;



public:
    // 增加一个分组, 返回这个项目的句柄
    virtual HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text = 0, LPARAM param = 0) = 0;

    // 设置指定项目的值, 字节集类型和易语言一样
    virtual bool SetItemValue(IPROPERTY_ITEM& data, const void* value) = 0;

    // 增加一个文本类型属性项, 属性值只能是文本
    // hParent = InsertGroup() 返回的值, 需要加入到哪个分组里
    // name = 属性名, 属性名运行后不能修改
    // value = 属性值
    // text = 备注文本
    // param = 项目数值
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value = 0, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT, LPCWSTR text = 0, LPARAM param = 0) = 0;

    // 设置说明文本
    virtual bool SetItemRemarks(HTREEITEM hItem, LPCWSTR value) = 0;
    // 获取说明文本
    virtual LPCWSTR GetItemRemarks(HTREEITEM hItem) = 0;
    // 删除指定项目
    virtual BOOL DeleteItem(HTREEITEM hItem) = 0;

    // 取指定项目的属性名
    virtual LPCWSTR GetItemName(HTREEITEM hItem) = 0;
    virtual bool SetItemName(HTREEITEM hItem, LPCWSTR name) = 0;

    // 取指定项目的属性值, 失败返回0, 字节集类型需要在前面加上 {1,0,0,0,长度} 8个字节, 和易语言一样
    virtual LPCWSTR GetItemValue(HTREEITEM hItem) = 0;

    // 设置指定项目的值, 字节集类型和易语言一样
    virtual bool SetItemValue(HTREEITEM hItem, LPCWSTR value) = 0;

    // 取指定项目的项目数值, InsertItem/SetItemValue 时填写的值
    virtual LPARAM GetItemParam(HTREEITEM hItem) = 0;

    // 设置指定项目的值
    virtual bool SetItemParam(HTREEITEM hItem, LPARAM param) = 0;

    // 对组合框项目增加项目数据, 调用后类型自动变成组合框类型
    // 调用成功返回表项的组合框项目树, 失败返回-1
    virtual size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0) = 0;

    // 删除组合框数据, index = 删除的下标, 从0开始, 下标越界/不是组合框类型返回false
    virtual bool DelComboboxData(HTREEITEM hItem, int index) = 0;

    // 取指定项目的组合框项目数, 失败返回-1, 不是组合框类型或者项目句柄无效都返回-1
    virtual size_t GetComboboxDataCount(HTREEITEM hItem) = 0;

    // 取指定项目的组合框项目文本, 失败返回空文本, 不是组合框类型, 下标越界, 句柄无效都返回空文本
    virtual LPCWSTR GetComboboxData(HTREEITEM hItem, int index) = 0;

    // 设置指定项目的组合框项目文本, 失败返回false, 不是组合框类型, 下标越界, 句柄无效都返回false
    virtual bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value) = 0;

    // 获取组合框现行选中项, 失败返回-1
    virtual int GetComboboxIndex(HTREEITEM hItem) = 0;

    // 设置组合框现行选中项, 索引从0开始
    virtual bool SetComboboxIndex(HTREEITEM hItem, int index) = 0;

    // 取指定项目的组合框项目数值
    virtual LPARAM GetComboboxParam(HTREEITEM hItem, int index) = 0;

    // 设置指定项目的组合框项目数值
    virtual bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param) = 0;

    // 设置指定项目为指定类型, 组合框类型需要增加数据
    virtual void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type) = 0;

    // 取指定项目类型
    virtual PROPERTY_TYPE GetItemType(HTREEITEM hItem) = 0;

    // 设置宽度, 千分比, 最小100, 最大900, 控件未创建前也可以调用
    // isLeft = 是否为设置左边, 设置一边后另一边会一起调整
    // value = 100-900之间的值
    // isUpdate = 是否重画
    virtual void SetWidthRatio(bool isLeft, int value, bool isUpdate = false) = 0;

    // 将当前表项所有数据保存成字节集数据, 返回的地址不使用时需要调用 free() 释放
    // isColor = 是否保存颜色配置信息
    virtual LPBYTE save(DWORD* size, bool isColor, bool isList) = 0;

    // 从内存里加载所有项目数据
    virtual bool load(const void* data, DWORD size, bool isUpdate) = 0;

    // 设置颜色信息
    virtual void SetColor(const PROPERTY_COLOR* color, bool isUpdate) = 0;
    virtual int GetColor(PROPERTY_COLOR* color) = 0;
    // 默认配色, 0=黑色风格, 1=白色风格
    virtual int DefColor(int type, bool isUpdate) = 0;

    virtual int SetItemFont(const LPLOGFONTW lf, bool isUpdate) = 0;
    // 默认为微软雅黑, 字体大小 -12, 没有样式
    // lfItalic = 斜体,  lfUnderline = 下划线, lfStrikeOut = 删除线
    virtual int SetItemFont(bool isUpdate, const wchar_t* name = L"微软雅黑", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) = 0;

    virtual int SetColorBackground(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorBackParent(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorText(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorListBorder(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorSelect(ARGB cr, bool isUpdate = false) = 0;
    virtual int SetColorDisabled(ARGB cr, bool isUpdate = false) = 0;
    virtual void SetIsFillColorBack(bool isFill) = 0;

    virtual bool SetItemDisabledState(HTREEITEM hItem, bool isDisabled) = 0;
    // 禁止项目
    virtual bool DisabledItem(HTREEITEM hItem) = 0;
    // 启用项目
    virtual bool EnabledItem(HTREEITEM hItem) = 0;
    // 项目是否禁止
    virtual bool IsItemDisabled(HTREEITEM hItem) = 0;
    // 项目是否启用
    virtual bool IsItemEnabled(HTREEITEM hItem) = 0;

    virtual bool clear() = 0;

    // 弹出对话框, 根据当前项目类型探测对应对话框, 会触发按钮点击回调
    virtual void popDialog() = 0;

    // 获取属性框矩形
    virtual void GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type) = 0;

    virtual LPBYTE GetExpand_Open(int* size) = 0;
    virtual LPBYTE GetExpand_Close(int* size) = 0;
    virtual bool SetExpand_Open(const void* pData, int size) = 0;
    virtual bool SetExpand_Close(const void* pData, int size) = 0;

public:
    
    
    // 项目属性被改变, 为空则取消挂接这个事件, 事件两个参数, 参数1是触发事件的项目句柄, 参数2是项目的类型
    virtual void OnPropertyChanged(fnPropertyChanged pfn = 0, void* param = 0) = 0;

    // 项目选择改变, 为空则取消挂接这个事件, 事件两个参数, 参数1是触发事件的项目句柄, 参数2是在哪个位置点击, -1=展开收缩按钮, 1=属性名, 2=属性值
    virtual void OnItemChanged(fnItemChanged pfn = 0, void* param = 0) = 0;
    
    // 项目属性值按下了回车键, 回调函数返回是否拦截, 为空则取消挂接这个事件, 如果回调函数返回true则不对按下回车键做进一步处理
    // 事件两个参数, 参数1是触发事件的项目句柄, 参数2是项目的类型
    virtual void OnPropertyReturn(fnPropertyReturn pfn = 0, void* param = 0) = 0;

    // 组合框项目鼠标滚动, 回调函数返回是否拦截, 如果回调函数返回true则不对鼠标滚动做进一步处理
    // count = 组合框项目总数, pos = 滚动距离负数向下滚动, 正数向上滚动, key = 滚动时按下了哪个按键, lParam = 鼠标坐标
    virtual void OnComboboxMouseWheel(fnComboboxMouseWheel pfn = 0, void* param = 0) = 0;

    // 组合框项目改变, 回调函数返回是否拦截, 如果回调函数返回true则不会改变项目
    // index = 要改变的项目索引, 返回false后组合框现行选中项就是这个索引
    virtual void OnComboboxChanged(fnComboboxChanged pfn = 0, void* param = 0) = 0;

    // 组合框下拉箭头被按下, 即将探测列表, 回调函数返回是否拦截, 如果回调函数返回true则不会弹出列表
    virtual void OnDropDown(fnDropWown pfn = 0, void* param = 0) = 0;

    // 属性按钮被单击, 回调函数返回是否拦截, 如果回调函数返回true则不会弹出内置的对话框
    virtual void OnItemButtonClick(fnItemButtonClick pfn = 0, void* param = 0) = 0;

    // 项目被右键单击, 返回是否拦截, 拦截后右键不会改变表项, 不会触发单击
    virtual void OnItemRButtonClick(fnItemRButtonClick pfn = 0, void* param = 0) = 0;

    // 组合框双击编辑框, 回调函数返回是否拦截, 如果回调函数返回true则不对双击做进一步处理
    // index = 双击时组合框的索引, count = 组合框项目数
    virtual void OnComboboxLDoubleClick(fnComboboxLDoubleClick pfn = 0, void* param = 0) = 0;
    
    // 编辑框被双击, 回调函数返回是否拦截, 如果回调函数返回true则不全选编辑框
    virtual void OnEditLDoubleClick(fnEditLDoubleClick pfn = 0, void* param = 0) = 0;

    // 项目输入字符, 回调函数返回是否拦截, 如果回调函数返回true则使用 pRet 返回的值作为返回值
    // ch = 输入的字符, pRet = 返回值指针, 需要把返回值写入这里
    virtual void OnItemChar(fnItemChar pfn = 0, void* param = 0) = 0;

    // 生成自定义属性值的显示名字, 三个参数, 项目句柄, 项目类型, 显示文本缓冲区, 最大100个字符
    virtual void OnItemMakeCustomValue(fnItemMakeCustomValue pfn = 0, void* param = 0) = 0;

    // 右边小按钮被重画, 返回true则内部不进行绘画, 需要用户在回调里进行绘画操作
    // hdc = 按钮绘画dc, 不是内存DC, state = 当前状态, STATE::HOT, STATE::DOWN, width = 按钮宽度, height = 按钮高度
    virtual void OnItemRButtonPaint(fnItemRButtonPaint pfn = 0, void* param = 0) = 0;

    // del键放开, 删除当前项数据, 在属性名这一列按下的默认处理是清除值, 不是属性名按下的只清除双击会打开对话框的项目
    // isName = 是否在属性名这一列按下按键
    virtual void OnItemKeyDelUp(fnItemDelUp pfn = 0, void* param = 0) = 0;

    // 自定义属性名字
    // isName = 是否在属性名这一列按下按键
    virtual void OnCustomValueName(fnCustomValueName pfn = 0, void* param = 0) = 0;

    // 项目读取数据失败, 字节集, 图片, 音频, 图片组这些....
    virtual void OnItemReadError(fnItemReadError pfn = 0, void* param = 0) = 0;
    // 即将绘画, 内部绘画前先调用用户的绘画
    virtual void OnItemBeginDraw(fnItemBeginDraw pfn = 0, void* param = 0) = 0;

    // 绘画结束, 内部绘画已经结束, 用户可以在这基础上进行二次绘画
    virtual void OnItemEndDraw(fnItemEndDraw pfn = 0, void* param = 0) = 0;

    virtual bool Expand(HTREEITEM hItem, BOOL b) = 0;
    virtual bool Expand(HTREEITEM hItem) = 0;
    virtual bool IsExpand(HTREEITEM hItem) = 0;


    virtual DWORD GetItemCount() = 0;
    virtual void InvalidateItem(HTREEITEM hItem) = 0;

    // 置现行选中项
    virtual bool SetIndex(HTREEITEM hItem) = 0;

    // 取可视项目数
    virtual unsigned int GetVisibleCount() = 0;

    // 取第一个项目
    virtual HTREEITEM GetFirstItem() = 0;

    // 取根项目
    virtual HTREEITEM GetRootItem() = 0;

    // 取子项目, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetChild(HTREEITEM hItem = 0) = 0;

    // 取下一个兄弟项, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextSibling(HTREEITEM hItem = 0) = 0;

    // 取上一个兄弟项, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetPrevSibling(HTREEITEM hItem = 0) = 0;

    // 取父项目, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetParent(HTREEITEM hItem = 0) = 0;

    // 取第一个可视项目
    virtual HTREEITEM GetFirstVisible() = 0;

    // 取下一个可视, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextVisible(HTREEITEM hItem = 0) = 0;

    // 取上一个可视, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetPrevVisible(HTREEITEM hItem = 0) = 0;

    // 取现行选中项
    virtual HTREEITEM GetSelection() = 0;

    // 取现行选中项
    virtual HTREEITEM GetIndex() = 0;
    // 取根项目
    virtual HTREEITEM GetRoot() = 0;

    // 取最后一个可视项目
    virtual HTREEITEM GetLastVisible() = 0;

    // 取下一个项目, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextItem(HTREEITEM hItem = 0, int code = TVGN_NEXT) = 0;



    // 取下一个选中项, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextSelectedItem(HTREEITEM hItem = 0) = 0;

    virtual HWND GetWindow() = 0;
    // 取子项目数
    virtual int GetSubCount(HTREEITEM hItem) = 0;
    // 取项目矩形, isTitle = 是否值包含标题
    virtual RECT GetItemRect(HTREEITEM hItem, bool isTitle = FALSE) = 0;
    // 计算中间这条线的位置
    virtual int calcMidLine(int cxClient = 0) = 0;



    // 保证显示
    virtual bool EnsureVisible(HTREEITEM hItem) = 0;

    virtual bool Move(int x, int y, int cx, int cy, bool bRepaint = true) = 0;

public:
    virtual void free(void* ptr) = 0;

};

#ifdef _EXPORTING
#define API_DECLSPEC __declspec(dllexport)
#else
#define API_DECLSPEC __declspec(dllimport)
#endif

typedef ICPropertyTree*(__cdecl* pfn_CProperty_Create)();
typedef ICMap*(__cdecl* pfn_map_Create)(bool isHash);
extern "C" API_DECLSPEC ICPropertyTree* CProperty_Create();
extern "C" API_DECLSPEC ICMap* CMap_Create(bool isHash);
