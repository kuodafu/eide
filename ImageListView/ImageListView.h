#pragma once
#include "framework.h"
#include "resource.h"
#include <d2d/d2d_old.h>
#include <CFileDialog.h>
#include <CFileDialogCom.h>
#include <control/CCustomList.h>
#include <vector>
#include <control/CImgList.h>
#include <control/CheckBox.h>
#include <control/CMenu.h>
#include <assist/CFileRW.h>
#include <xml2/tinyxml2.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <psapi.h>
#include <control/CInputBox.h>
#include <ThemeResourceExt.h>
//#include <assist_draw.h>
#include <gdiplus/drawImage.h>

#include <control/ReadColor.h>

struct WINDOW_DRAW_COLOR
{
    COLORREF crBack;            // 程序的背景颜色
    COLORREF crTextProject;     // 工程项目的文本颜色, 左边列表框
    COLORREF crTextItem;        // 项目的文本颜色, 中间列表框
    COLORREF crTextID;          // 项目的ID文本颜色, 中间列表框
    COLORREF crTextText;        // 项目的替换文本颜色, 中间列表框
    COLORREF crTextRemarks;     // 项目的备注文本颜色, 中间列表框
    COLORREF crText;            // 普通文本的文本颜色
    COLORREF crHot;             // 热点颜色
    COLORREF crDown;            // 按下颜色
};
extern WINDOW_DRAW_COLOR g_color;

#define ID_MENU_NEWTHEME        2003    // 新建主题
#define ID_MENU_DELTHEME        2004    // 删除主题
#define ID_MENU_CLEAR_CACHE     2010    // 清除缓存
#define ID_MENU_SETSIZE         2005    // 设置主题尺寸
#define ID_MENU_MAKETHEME       2006    // 生成主题文件
#define ID_MENU_LOCATIONPROJECT 2007    // 定位工程目录
#define ID_MENU_LOCATIONITEM    2008    // 定位图片项目
#define ID_MENU_LOCATIONITEM1   2009    // 定位图片项目

#define ID_MENU_SELIMAGEPATH    2100    // 选择图片素材目录

#define ID_MENU_NEWITEM         2200    // 添加项目
#define ID_MENU_NEWITEMS        2209    // 批量添加项目
#define ID_MENU_DELITEM         2201    // 删除项目
#define ID_MENU_RESET           2250    // 重置
#define ID_MENU_RESET_NORMAL    2251    // 重置正常状态
#define ID_MENU_RESET_HOT       2252    // 重置热点状态
#define ID_MENU_RESET_DOWN      2253    // 重置按下状态
#define ID_MENU_RESET_BAN       2254    // 重置禁止状态
#define ID_MENU_RESETITEM       2255    // 重置项目
#define ID_MENU_RESETITEM_DPI   2256    // 重置当前选中DPI项目

#define ID_MENU_RELOAD          2260    // 重新加载
#define ID_MENU_RELOAD_NORMAL   2261    // 重新加载正常状态
#define ID_MENU_RELOAD_HOT      2262    // 重新加载热点状态
#define ID_MENU_RELOAD_DOWN     2263    // 重新加载按下状态
#define ID_MENU_RELOAD_BAN      2264    // 重新加载禁止状态

#define ID_MENU_SETIMAGE        2280    // 设置正常图片

#define ID_MENU_100_SETNORMAL       0x0120    // 设置正常状态图片
#define ID_MENU_100_SETHOT          0x0121    // 设置点燃状态图片
#define ID_MENU_100_SETDOWN         0x0122    // 设置按下状态图片
#define ID_MENU_100_SETBAN          0x0123    // 设置禁止状态图片
//#define ID_MENU_125_SETNORMAL       0x0220    // 设置正常状态图片
//#define ID_MENU_125_SETHOT          0x0221    // 设置点燃状态图片
//#define ID_MENU_125_SETDOWN         0x0222    // 设置按下状态图片
//#define ID_MENU_125_SETBAN          0x0223    // 设置禁止状态图片
//#define ID_MENU_150_SETNORMAL       0x0320    // 设置正常状态图片
//#define ID_MENU_150_SETHOT          0x0321    // 设置点燃状态图片
//#define ID_MENU_150_SETDOWN         0x0322    // 设置按下状态图片
//#define ID_MENU_150_SETBAN          0x0323    // 设置禁止状态图片
//#define ID_MENU_175_SETNORMAL       0x0420    // 设置正常状态图片
//#define ID_MENU_175_SETHOT          0x0421    // 设置点燃状态图片
//#define ID_MENU_175_SETDOWN         0x0422    // 设置按下状态图片
//#define ID_MENU_175_SETBAN          0x0423    // 设置禁止状态图片
//#define ID_MENU_200_SETNORMAL       0x0520    // 设置正常状态图片
//#define ID_MENU_200_SETHOT          0x0521    // 设置点燃状态图片
//#define ID_MENU_200_SETDOWN         0x0522    // 设置按下状态图片
//#define ID_MENU_200_SETBAN          0x0523    // 设置禁止状态图片

#define ID_MENU_SETREMARKS      2280    // 设置说明/备注
#define ID_MENU_SETID           2281    // 设置ID
#define ID_MENU_SETTEXT         2282    // 设置文本, 修改的文本
#define ID_MENU_SETCRTEXT       2283    // 设置文本颜色
#define ID_MENU_SETCRTEXT1      2284    // 设置正常文本颜色
#define ID_MENU_SETCRTEXT2      2285    // 设置热点文本颜色
#define ID_MENU_SETCRTEXT3      2286    // 设置按下文本颜色
#define ID_MENU_SETCRTEXT4      2287    // 设置禁止文本颜色

#define ID_MENU_SETCRBACK       2288    // 设置背景颜色
#define ID_MENU_SETCRBACK1      2289    // 设置正常背景颜色
#define ID_MENU_SETCRBACK2      2290    // 设置热点背景颜色
#define ID_MENU_SETCRBACK3      2291    // 设置按下背景颜色
#define ID_MENU_SETCRBACK4      2292    // 设置禁止背景颜色

#define ID_MENU_100_SETNORMAL1      0x0100    // 设置正常状态图片
#define ID_MENU_100_SETHOT1         0x0101    // 设置点燃状态图片
#define ID_MENU_100_SETDOWN1        0x0102    // 设置按下状态图片
#define ID_MENU_100_SETBAN1         0x0103    // 设置禁止状态图片
#define ID_MENU_125_SETNORMAL1      0x0200    // 设置正常状态图片
#define ID_MENU_125_SETHOT1         0x0201    // 设置点燃状态图片
#define ID_MENU_125_SETDOWN1        0x0202    // 设置按下状态图片
#define ID_MENU_125_SETBAN1         0x0203    // 设置禁止状态图片
#define ID_MENU_150_SETNORMAL1      0x0300    // 设置正常状态图片
#define ID_MENU_150_SETHOT1         0x0301    // 设置点燃状态图片
#define ID_MENU_150_SETDOWN1        0x0302    // 设置按下状态图片
#define ID_MENU_150_SETBAN1         0x0303    // 设置禁止状态图片
#define ID_MENU_175_SETNORMAL1      0x0400    // 设置正常状态图片
#define ID_MENU_175_SETHOT1         0x0401    // 设置点燃状态图片
#define ID_MENU_175_SETDOWN1        0x0402    // 设置按下状态图片
#define ID_MENU_175_SETBAN1         0x0403    // 设置禁止状态图片
#define ID_MENU_200_SETNORMAL1      0x0500    // 设置正常状态图片
#define ID_MENU_200_SETHOT1         0x0501    // 设置点燃状态图片
#define ID_MENU_200_SETDOWN1        0x0502    // 设置按下状态图片
#define ID_MENU_200_SETBAN1         0x0503    // 设置禁止状态图片

#define ID_MENU_SAVE            2220    // 保存


#define ID_MENU_BLACK           2300    // 黑色主题
#define ID_MENU_WHITE           2301    // 白色主题

#define ID_RADIO_100            2400    // 100%缩放
#define ID_RADIO_125            2401    // 125%缩放
#define ID_RADIO_150            2402    // 150%缩放
#define ID_RADIO_175            2403    // 175%缩放
#define ID_RADIO_200            2404    // 200%缩放



#define XML_INI_IMAGEPATH           L"image"

#define XML_NAME_ROOT_PATHNAME      L"path"         // 根节点保存的路径名, 只有一个名字, 需要自己组合成路径, 窗口关闭前需要把图片保存到这个目录下
//#define XML_NAME_ROOT_SIZE          L"size"         // 这个项目的尺寸

#define XML_NAME_IMAGE              L"image"        // 图片完整路径
#define XML_NAME_RECT               L"rect"
#define XML_NAME_DPIINDEX           L"dpiIndex"     // 选中哪个DPI


extern HWND g_hWnd;
extern HINSTANCE g_hInst;                       // 当前实例
extern WCHAR g_runPath[];                       // 程序运行目录, 后面带 "\"
extern WCHAR g_runFile[];                       // 程序完整路径
struct PROJECT_DATA;
extern CTinyXml2W g_xmlProject;     // 工程xml, 增删工程都从这里修改

// 把dpi索引和状态转成数组下标
inline int _dpiindex2index(int state, int dpiIndex)
{
    int index = 0;
    switch (state)
    {
    case BAN:   index = 3; break;
    case DOWN:  index = 2; break;
    case HOT:   index = 1; break;
    default:    index = 0; break;
        break;
    }
    index = dpiIndex * 4 + index;
    return index;
}

// 提供一个xml项目路径, 内部会加载并读取这个xml的数据, 返回new 出来的xml对象
// xmlFile = xml完整路径, 是从工程xml里读出来的路径, 如果文件不存在则会创建这个文件
// name = 名字, 图片数据是复制到这个目录下
// ppArr = 接收xml里的数据
CTinyXml2W* init_IconItem(PROJECT_DATA* data);


struct SHOWIMAGEDATA
{
    wstr file;      // 记录完整路径
    LPCWSTR name;   // 记录文件名
    SHOWIMAGEDATA() :name(0) { }
    SHOWIMAGEDATA(LPCWSTR fileName) :name(0)
    {
        file = fileName;
        name = wcsrchr(file.c_str(), '\\') + 1;
    }

};
struct XMLDATA_STRUCT
{
    LPWSTR file[20];    // 所有DPI缩放的图片路径
    LPWSTR id;          // id
    LPWSTR text;        // 说明
    LPWSTR remarks;     // 备注
    RECT rc[20];        // 当前图标在大图片里的位置, 0=正常, 1=点燃, 2=按下, 3=禁止, 一共5种DPI缩放位置
    DWORD crText[4];    // 文本ARGB颜色, 值为0则表示没有设置, 使用默认, 0=正常, 1=点燃, 2=按下, 3=禁止
    DWORD crBack[4];    // 背景ARGB颜色, 值为0则表示没有设置, 使用默认, 0=正常, 1=点燃, 2=按下, 3=禁止
    int left;           // id绘画的左边
    int right;          // id绘画的右边, 绘画时记录, 在双击表项时通过这个计算单击的位置
    XMLDATA_STRUCT() :id(0), text(0), left(0), right(0), remarks(0)
    {
        memset(file, 0, sizeof(file));
        memset(rc, 0, sizeof(rc));
        memset(crText, 0, sizeof(crText));
        memset(crBack, 0, sizeof(crBack));
    }
    XMLDATA_STRUCT(LPCWSTR id, LPCWSTR text, LPCWSTR remarks)
        :id(0), text(0), left(0), right(0), remarks(0)
    {
        memset(file, 0, sizeof(file));
        memset(rc, 0, sizeof(rc));
        memset(crText, 0, sizeof(crText));
        memset(crBack, 0, sizeof(crBack));
        if (id      && id[0])     this->id      = wstr::copy(id);
        if (text    && text[0])   this->text    = wstr::copy(text);
        if (remarks && remarks[0])this->remarks = wstr::copy(remarks);
    }
    inline void free()
    {
        for (int i = 0; i < sizeof(file) / sizeof(file[0]); i++)
            if (file[i]) wstr::free(file[i]);

        if (id)     wstr::free(id);
        if (text)   wstr::free(text);
        if (remarks)wstr::free(remarks);
        memset(this, 0, sizeof(*this));
    }
    inline LPCWSTR SetFile    (LPCWSTR pstr, int state, int dpiIndex)
    {
        int index = _dpiindex2index(state, dpiIndex);
        return SetStr(file[index], pstr);
    }
    inline LPCWSTR SetID        (LPCWSTR pstr)  { return SetStr(id,         pstr); }
    inline LPCWSTR SetText      (LPCWSTR pstr)  { return SetStr(text,       pstr); }
    inline LPCWSTR SetRemarks   (LPCWSTR pstr)  { return SetStr(remarks,    pstr); }

private:
    inline LPCWSTR SetStr(LPWSTR& p, LPCWSTR pstr)
    {
        if (p == pstr)return p;
        if (p) wstr::free(p);
        p = 0;
        if (!pstr || !pstr[0])return 0;
        p = wstr::copy(pstr);
        return p;
    }
};

struct PROJECT_DATA
{
    LPWSTR xmlName;         // xml文件名字, 没有后缀, 需要根据目录拼接成完整路径, 一般是 g_runPath + "data\project\" + xmlName +".xml"
    std::vector<XMLDATA_STRUCT>* arr;   // xml里的数据
    Gdiplus::Bitmap* hBitmap;           // 大图片的图片指针
    int maxWidth;           // 所有图片里最大的宽度
    int maxHeight;          // 所有图片里最大的高度
    int imgWidth;           // 最终生成图片文件的宽度
    int imgHeight;          // 最终生成图片文件的高度
    int themeIndex;         // 0=黑色, 1=白色
    PROJECT_DATA() :xml(0), xmlName(0), arr(0), hBitmap(0), maxWidth(0), maxHeight(0), imgWidth(0), imgHeight(0), themeIndex(0) { }

    ~PROJECT_DATA()
    {

    }
    inline CTinyXml2W* GetXml() { return xml; }
    inline void free()
    {
        if (xml) delete xml;
        if (xmlName) wstr::free(xmlName);
        if (arr)
        {
            for (auto it = arr->begin(); it != arr->end(); ++it)
            {
                XMLDATA_STRUCT& data = *it;
                data.free();
            }
            delete arr;
        }
        if (hBitmap)delete hBitmap;

        xmlName = 0;
        xml = 0;
        arr = 0;
        hBitmap = 0;
    }
    PROJECT_DATA(LPCWSTR name) :xml(0), xmlName(0), arr(0), hBitmap(0), maxWidth(0), maxHeight(0), imgWidth(0), imgHeight(0), themeIndex(0)
    {
        assert(name && name[0]);
        this->xmlName = wstr::copy(name);
        xml = init_IconItem(this);
    }
private:
    CTinyXml2W* xml;        // xml文件对应的xml对象, 析构需要释放, 构造函数里会创建该对象

};

// 根据文件名来判断工程目录下有没有创建这个目录
// path = 接收根据这个名字获取的工程目录, 目录后面不带 '\'
// xmlFile = 接收根据这个名字获取的工程项目xml文件完整路径
// pngFile = 所有图片打包到一起的图片路径, 主题数据从这个图片路径里读取
bool isCreatePath(LPCWSTR name, wstr* path = 0, wstr* xmlFile = 0, wstr* pngFile = 0);

extern std::vector<PROJECT_DATA>  s_data;       // 工程列表, 记录当前新建的所有工程
extern std::vector<SHOWIMAGEDATA> s_dataImage;      // 显示素材的图片
extern CButton g_radioDPI[5];
extern int g_dpiIndex;
extern CCustomList s_listProject;   // 左边工程列表框
extern CCustomList s_listItem;      // 中间项目列表框
extern CCustomList s_listImage;     // 显示图片资源的列表
extern HWND g_hWnd;
extern CMenu s_menu;
extern CMenu s_menuImage;
extern CMenu s_menuFile;
extern wstr s_path;                 // 文件列表的路径


extern HBRUSH s_hbrBack;
extern CFileDialog_Com s_fileDlg;



extern CTinyXml2W s_xml_info;       // 程序配置信息
extern wstr m_pathname;
extern PROJECT_DATA* m_data;

// GetBitmapFromMap 是存在就返回, appendBitmapToMap 是存在就释放, 然后重新创建
// 从路径加载图片, 返回加载后的图片指针
// 内部会把图片保存到 s_map_file 这个map里, 如果之前存在会释放之前保存的图片
Gdiplus::Bitmap* appendBitmapToMap(const wchar_t* path);

// GetBitmapFromMap 是存在就返回, appendBitmapToMap 是存在就释放, 然后重新创建
// 从指定路径获取图片, 如果未加载内部会加载
Gdiplus::Bitmap* GetBitmapFromMap(const wchar_t* path);

// 从指定路径查询图片, 如果有则返回图片句柄, 没有就返回0
Gdiplus::Bitmap* QueryBitmapFromMap(const wchar_t* path);

void DeleteBitmapFromMap(const wchar_t* path);

LONG_PTR _hash_fun(const wchar_t* pstr);

void assist_destroy(bool isFree);

// 把素材路径转换成工程路径
// lpszProjectPath = 工程目录, 不带'\'
// lpszImageFileName = 图片素材完整路径
LPCWSTR _srcfile_to_projectfile(LPCWSTR lpszProjectPath, LPCWSTR lpszImageFileName);

void __createPath(LPWSTR path);
// 删除某个项目, 会从数组里删除成员, 从xml里删除对应节点, 从位图数组里释放目录下的文件
void eraseItem(int index);

// 从当前选中的工程获取指定状态的图片位置
// projectData = 哪个项目数据
// index = 第几个成员, 索引从0开始
// state = STATE 枚举常量里的值, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
RECT& xml_getimage_rect(PROJECT_DATA* projectData, int index, STATE state);

// 从当前选中的工程获取指定状态的图片地址, 如果没有则返回0
// projectData = 哪个项目数据
// index = 第几个成员, 索引从0开始
// state = STATE 枚举常量里的值, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, 内部根据传入的dpi索引设置对应位置的矩形, 图标索引 = dpiIndex * 4 + 状态
// rcIcon = 接收图片在大图片里的位置
const wchar_t* xml_getimage_filename(PROJECT_DATA* projectData, int index, STATE state, int dpiIndex, RECT* rcIcon = 0);

// 设置xml指定索引指定状态的文件路径
// projectData = 哪个项目数据
// index = 第几个成员, 索引从0开始
// filename = 要设置的文件路径
// state = STATE 枚举常量里的值, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, 内部根据传入的dpi索引设置对应位置的矩形, 图标索引 = dpiIndex * 4 + 状态
bool xml_setimage_filename(PROJECT_DATA* projectData, int index, const wchar_t* filename, STATE state, int dpiIndex);

// 清空指定索引的图片数据, 会从图片map中释放, 会删除
// state = 要删除的状态, -1=删除全部, 现在只有 NORMAL, HOT, DOWN, BAN 这4种状态
// dpiIndex = 0=100, 1=125, 2=150, 3=175, 4=200, 内部根据传入的dpi索引设置对应位置的矩形, 图标索引 = dpiIndex * 4 + 状态
void xml_delete_image(PROJECT_DATA* projectData, int index, int state, int dpiIndex);

/// 判断一个路径是否是已存在的目录
bool IsDirectory(const wchar_t* pstrPath);

// 删除目录及目录中的所有内容
// pstrFolder = 文件夹路径, 后面可加可不加反斜杠
// recursive = 是否删除子目录, 为true则递归删除子目录
bool DeleteFolder(const wchar_t* pstrFolder, bool recursive = false);

/*复制目录及目录中的所有内容*/
// pstrFolder = 要拷贝的目录
// pstrDest = 拷贝到的目录
bool CopyFolder(const wchar_t* pstrFolder, const wchar_t* pstrDest);


// 枚举目录下的图片文件 *.jpg;*.png;*.jpeg;*.bmp;
void init_Image(const wchar_t* pszPath);

// 初始化项目, 从主题目录里读取已有的项目
void init_project();


bool CALLBACK WndProc_command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lpResult);
bool CALLBACK WndProc_create(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lpResult);


bool CALLBACK CustomListBoxImage_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus);
bool CALLBACK CustomListBoxItem_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus);
bool CALLBACK CustomListBox_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus);

void CALLBACK CustomList_ItemRClick(HWND hWnd, int index);
void CALLBACK CustomList_ItemClick(HWND hWnd, int index, bool isDblClk);

void CALLBACK CustomList_ItemSelect(HWND hWnd, int index);


Gdiplus::Bitmap* _img_create_from_file(const wchar_t* file);

// 设置是否修改了数据, 有修改数据则设置窗口标题前面加个星号
void ModyfyTitle(bool isModify);

// 取文件名, 返回的名字包含'\'
inline LPCWSTR _file_get_name(LPCWSTR name)
{
    if (!name || !name[0])return 0;
    LPCWSTR pos = wcsrchr(name, '\\');
    if (!pos) pos = wcsrchr(name, '/');
    if (!pos)return 0;
    return pos;
}