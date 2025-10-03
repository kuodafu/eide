#include "CPropertyTree_virtual.h"
#include "CPropertyTree.h"
#include <unordered_map>

template <typename TKey, typename TValue, typename T>
class CMap : public ICMap
{
    T* m_map;
    int m_addref;
public:
    CMap() : m_map(0), m_addref(1)
    {
        m_map = new T;
    }
    ~CMap()
    {
        delete m_map;
        m_map = 0;
    }
    virtual int Addref()
    {
        return ++m_addref;
    }
    virtual int Release()
    {
        if (--m_addref)
        {
            delete this;
            return 0;
        }
        return m_addref;
    }
    // 从map里获取指定KEY对应的值, 如果没有这个key, 内部会创建一个
    virtual TValue& GetMapValue(TKey key)
    {
        T& m = *m_map;
        return m[key];
    }
    // 给指定key设置一个新的值
    virtual void SetMapValue(TKey key, TValue value)
    {
        T& m = *m_map;
        m[key] = value;
    }
    // 从map里查询指定KEY对应的值, 如果没有这个key不会创建
    virtual TValue QueryMapValue(TKey key)
    {
        T& m = *m_map;
        auto it = m.find(key);
        if (it == m.end())
            return 0;
        return it->second;
    }

    // 从map里删除指定KEY对应的值
    virtual bool erase(LPVOID key)
    {
        T& m = *m_map;
        auto it = m.find(key);
        if (it == m.end())
            return false;
        m.erase(it);
        return true;
    }
    // 获取map成员数
    virtual int size()
    {
        T& m = *m_map;
        return (int)m.size();
    }

    // 清空map
    virtual void clear()
    {
        T& m = *m_map;
        m.clear();
    }

    // 枚举当前所有键名和键值, 返回写入缓冲区的成员数
    // buffer 缓冲区, 需要外部申请缓冲区, 为0则返回需要的缓冲区数量
    // size = 缓冲区数量
    virtual int enumValue(LPMAP_DATA_STRUCT buffer, int size)
    {
        T& m = *m_map;
        if (!buffer || size <= 0)
            return (int)m.size();

        int ret = 0;
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            buffer[ret].name = it->first;
            buffer[ret].value = it->second;
            if (++ret >= size)
                break;
        }
        
        return ret;
    }

    // 枚举当前所有键名和键值, 返回写入缓冲区的成员数
    // pfnCallback = 枚举到成员后调用的回调函数, 为0则返回成员数
    // pArg = 传递到回调函数里的参数
    virtual int enumValueEx(PFN_EnumMap pfnCallback, LPVOID pArg)
    {
        T& m = *m_map;
        int ret = (int)m.size();
        if (pfnCallback)
        {
            ret = 0;
            for (auto it = m.begin(); it != m.end(); ++it)
            {
                ret++;
                if (!pfnCallback(pArg, it->first, it->second))
                    break;
            }
        }
        return ret;

    }
};

class ICPropertyTree_Realization : public ICPropertyTree
{
    CPropertyTree tree;
    int m_addref;
public:
    ICPropertyTree_Realization() { m_addref = 1; }

    virtual LPCWSTR className() { return tree.className(); };
    virtual int Addref() { return ++m_addref; }
    virtual int Release() {
        if (--m_addref)
        {
            delete this;
            return 0;
        }
        return m_addref;
    }
public:
    virtual bool SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size) { return tree.SetIcon(type, pIcon, size); }
public:
    virtual void SetItemHeight(int height, bool isUpdate = false) { tree.SetItemHeight(height, isUpdate); }
    virtual int GetItemHeight() { return tree.GetItemHeight(); }

    // 是否为分组
    virtual bool isGroup(PROPERTY_TYPE type) { return tree.IsGroup(type); }
    // 是否有按钮
    virtual bool isButton(PROPERTY_TYPE type) { return tree.IsButton(type); }
    // 是否有下拉按钮
    virtual bool isButtonCombobox(PROPERTY_TYPE type) { return tree.IsButtonCombobox(type); }
    // 是否为数值
    virtual bool isNumber(PROPERTY_TYPE type) { return tree.IsNumber(type); }
    // 是否为文本, 长文本, 带按钮文本都返回true
    virtual bool isText(PROPERTY_TYPE type) { return tree.IsText(type); }

    // 以下为扩展类型, 由几个样式组合而成

    // 是否为带按钮文本
    virtual bool isTextButton(PROPERTY_TYPE type) { return tree.IsTextButton(type); }
    // 是否为长文本
    virtual bool isLongText(PROPERTY_TYPE type) { return tree.IsLongText(type); }
    // 是否为不可编辑组合框
    virtual bool isComboboxNoEdit(PROPERTY_TYPE type) { return tree.IsComboboxNoEdit(type); }
    // 是否为不可编辑组合框
    virtual bool isComboboxNoEditIndex(PROPERTY_TYPE type) { return tree.IsComboboxNoEditIndex(type); }
    // 是否为组合框, 不可编辑和带编号的都返回真, 需要判断对应的就使用对应的方法
    virtual bool isCombobox(PROPERTY_TYPE type) { return tree.IsCombobox(type); }
    // 是否为颜色选择器
    virtual bool isColorDialog(PROPERTY_TYPE type) { return tree.IsColorDialog(type); }
    // 是否为文件选择器
    virtual bool isFileDialog(PROPERTY_TYPE type) { return tree.IsFileDialog(type); }
    // 是否为字体选择器
    virtual bool isFontDialog(PROPERTY_TYPE type) { return tree.IsFontDialog(type); }
    // 是否为字节集
    virtual bool isBinDialog(PROPERTY_TYPE type) { return tree.IsBinDialog(type); }
    // 是否为图片选择器
    virtual bool isPicDialog(PROPERTY_TYPE type) { return tree.IsPicDialog(type); }
    // 是否为音频选择器
    virtual bool isMusicDialog(PROPERTY_TYPE type) { return tree.IsMusicDialog(type); }
    // 是否为图片组选择器
    virtual bool isImgListDialog(PROPERTY_TYPE type) { return tree.IsImgListDialog(type); }
    // 是否为链接框
    virtual bool IsLink(PROPERTY_TYPE type) { return tree.IsLink(type); }
    // 是否为选择框
    virtual bool IsCheckBox(PROPERTY_TYPE type) { return tree.IsCheckBox(type); }
    // 是否为3态选择框
    virtual bool IsCheckBox3(PROPERTY_TYPE type) { return tree.IsCheckBox3(type); }
    // 是否为单选框
    virtual bool IsRadio(PROPERTY_TYPE type) { return tree.IsRadio(type); }
    // 是否为自动单选框
    virtual bool IsAutoRadio(PROPERTY_TYPE type) { return tree.IsAutoRadio(type); }
    // 是否为开关
    virtual bool IsSwitch(PROPERTY_TYPE type) { return tree.IsSwitch(type); }
    // 是否为选择类型组件, 选择框/3态选择框, 单选框, 自动单选框, 开关, 都属于选择类型
    virtual bool IsCheckType(PROPERTY_TYPE type) { return tree.IsCheckType(type); }


    // 1=字节,2=短整数,3=整数,4=长整数,5=小数,6=双精度小数, 不会返回其他值
    virtual int  GetNumberType(PROPERTY_TYPE type) { return tree.GetNumberType(type); }
    // 是否有符号
    virtual bool isUnsigned(PROPERTY_TYPE type) { return tree.IsUnsigned(type); }
    // 是否可编辑
    virtual bool isEdit(PROPERTY_TYPE type) { return tree.IsEdit(type); }
    // 是否有编号
    virtual bool isIndex(PROPERTY_TYPE type) { return tree.IsIndex(type); }
    // 是否自定义属性
    virtual bool isCustom(PROPERTY_TYPE type) { return tree.IsCustom(type); }

    // 是否禁止
    virtual bool isDisabled(PROPERTY_TYPE type) { return tree.IsDisabled(type); }
    // 是否可用
    virtual bool isEnabled(PROPERTY_TYPE type) { return tree.IsEnabled(type); }

    // 不响应滚动消息, 并且把消息转发到窗口上
    virtual bool isNoMouseWheel(PROPERTY_TYPE type) { return tree.IsNoMouseWheel(type); }


public:
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0) {
        _canvas_init(true);
        return tree.create(dwExStyle, lpszTitle, dwStyle, dwExStyleTree, x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }

public:
    // 写入字节集, 传递数据指针和长度, 内部会转成易语言的格式
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData, const size_t size) { return tree.SetBinData(data, pData, size); }

    // 写入字节集, 传递数据必须是易语言的格式
    virtual bool SetBinData(IPROPERTY_ITEM& data, const void* pData) { return tree.SetBinData(data, pData); }

    // 写入字体, 字体存放的数据传递是固定的, 两个结构, LOGFONTW + CHOOSEFONTW, 内部会修改字体大小/字符高度
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPLOGFONTW lf) { return tree.SetFontData(data, lf); }

    // 写入字体, 传入CHOOSEFONTW结构指针, 内部会修改字体大小/字符高度
    // CHOOSEFONTW 指向的 LOGFONT结构有值的话也会吧这个结构拷贝保存起来
    virtual bool SetFontData(IPROPERTY_ITEM& data, const LPCHOOSEFONTW pcf) { return tree.SetFontData(data, pcf); }

    // 写入字体, 传递gdi字体指针, 内部会修改字体大小/字符高度
    virtual bool SetFontData(IPROPERTY_ITEM& data, HFONT hFont) { return tree.SetFontData(data, hFont); }


    // 写入字体, 字体存放的数据传递是固定的, 两个结构, LOGFONTW + CHOOSEFONTW
    virtual bool SetFontData(IPROPERTY_ITEM& data, const wchar_t* name = L"微软雅黑", int lfHeight = -12,
        bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) {
        return tree.SetFontData(data, name, lfHeight, lfWeight, lfItalic, lfUnderline, lfStrikeOut);
    }

    // 获取字体数据, 内部有更新 LPCHOOSEFONTW 结构指向的logfont地址
    virtual LPLOGFONTW GetFontData(IPROPERTY_ITEM& data, LPCHOOSEFONTW* pcf) { return tree.GetFontData(data, pcf); }

    // 根据项目句柄获取项目存放项目的数据指针
    virtual IPROPERTY_ITEM* GetItemData(HTREEITEM hItem) { return tree.GetItemData(hItem); }

    // 设置对话框属性, 为0则不设置对应的属性
    // type = 要设置的对话框类型, 目前只支持 PROPERTY_FLAG_FILE文件选择器, PROPERTY_FLAG_BIN字节集选择器
    // filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3\0媒体文件(*.mpg)|*.mpg”
    // defName = 默认文件名
    // title = 窗口标题
    // path = 运行目录
    // notChengedPath = 不改变目录, 需要传递变量指针, 指针为0则不设置
    virtual bool SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, bool* noChangePath = 0)
    {
        return tree.SetOpenDialogProperty(type, filter, title, defName, path, noChangePath);
    }

    // 获取对话框属性, 为0则不获取对应的属性
    // type = 要设置的对话框类型, 目前只支持 PROPERTY_FLAG_FILE文件选择器, PROPERTY_FLAG_BIN字节集选择器
    // filter = 过滤器, 格式：“MP3文件(*.mp3)|*.mp3\0媒体文件(*.mpg)|*.mpg”
    // defName = 默认文件名
    // title = 窗口标题
    // path = 运行目录
    // notChengedPath = 不改变目录
    virtual bool GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter = 0, LPCWSTR* title = 0, LPCWSTR* defName = 0, LPCWSTR* path = 0, bool* noChangePath = 0)
    {
        return tree.GetOpenDialogProperty(type, filter, title, defName, path, noChangePath);
    }



public:
    // 增加一个分组, 返回这个项目的句柄
    virtual HTREEITEM InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text = 0, LPARAM param = 0) { return tree.InsertGroup(name, isExpand, text, param); }

    // 设置指定项目的值, 字节集类型和易语言一样
    virtual bool SetItemValue(IPROPERTY_ITEM& data, const void* value) { return tree.SetItemValue(data, value); }

    // 增加一个文本类型属性项, 属性值只能是文本
    // hParent = InsertGroup() 返回的值, 需要加入到哪个分组里
    // name = 属性名, 属性名运行后不能修改
    // value = 属性值
    // text = 备注文本
    // param = 项目数值
    virtual HTREEITEM InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value = 0, PROPERTY_TYPE type = PROPERTY_TYPE_TEXT, LPCWSTR text = 0, LPARAM param = 0) { return tree.InsertItem(hParent, name, value, type, text, param); }

    // 设置说明文本
    virtual bool SetItemRemarks(HTREEITEM hItem, LPCWSTR value) { return tree.SetItemRemarks(hItem, value); }
    // 获取说明文本
    virtual LPCWSTR GetItemRemarks(HTREEITEM hItem) { return tree.GetItemRemarks(hItem); }

    // 删除指定项目
    virtual BOOL DeleteItem(HTREEITEM hItem) { return tree.DeleteItem(hItem); }


    // 取指定项目的属性名
    virtual LPCWSTR GetItemName(HTREEITEM hItem) { return tree.GetItemName(hItem); }
    virtual bool SetItemName(HTREEITEM hItem, LPCWSTR name) { return tree.SetItemName(hItem, name); }

    // 取指定项目的属性值, 失败返回0, 字节集类型需要在前面加上 {1,0,0,0,长度} 8个字节, 和易语言一样
    virtual LPCWSTR GetItemValue(HTREEITEM hItem) { return tree.GetItemValue(hItem); }

    // 设置指定项目的值, 字节集类型和易语言一样
    virtual bool SetItemValue(HTREEITEM hItem, LPCWSTR value) { return tree.SetItemValue(hItem, value); }

    // 取指定项目的项目数值, InsertItem/SetItemValue 时填写的值
    virtual LPARAM GetItemParam(HTREEITEM hItem) { return tree.GetItemParam(hItem); }

    // 设置指定项目的值
    virtual bool SetItemParam(HTREEITEM hItem, LPARAM param) { return tree.SetItemParam(hItem, param); }

    // 对组合框项目增加项目数据, 调用后类型自动变成组合框类型
    // 调用成功返回表项的组合框项目树, 失败返回-1
    virtual size_t AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param = 0) { return tree.AddComboboxData(hItem, name, param); }

    // 删除组合框数据, index = 删除的下标, 从0开始, 下标越界/不是组合框类型返回false
    virtual bool DelComboboxData(HTREEITEM hItem, int index) { return tree.DelComboboxData(hItem, index); }

    // 取指定项目的组合框项目数, 失败返回-1, 不是组合框类型或者项目句柄无效都返回-1
    virtual size_t GetComboboxDataCount(HTREEITEM hItem) { return tree.GetComboboxDataCount(hItem); }

    // 取指定项目的组合框项目文本, 失败返回空文本, 不是组合框类型, 下标越界, 句柄无效都返回空文本
    virtual LPCWSTR GetComboboxData(HTREEITEM hItem, int index) { return tree.GetComboboxData(hItem, index); }

    // 设置指定项目的组合框项目文本, 失败返回false, 不是组合框类型, 下标越界, 句柄无效都返回false
    virtual bool SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value) { return tree.SetComboboxData(hItem, index, value); }

    // 获取组合框现行选中项, 失败返回-1
    virtual int GetComboboxIndex(HTREEITEM hItem) { return tree.GetComboboxIndex(hItem); }

    // 设置组合框现行选中项, 索引从0开始
    virtual bool SetComboboxIndex(HTREEITEM hItem, int index) { return tree.SetComboboxIndex(hItem, index); }

    // 取指定项目的组合框项目数值
    virtual LPARAM GetComboboxParam(HTREEITEM hItem, int index) { return tree.GetComboboxParam(hItem, index); }

    // 设置指定项目的组合框项目数值
    virtual bool SetComboboxParam(HTREEITEM hItem, int index, LPARAM param) { return tree.SetComboboxParam(hItem, index, param); }

    // 设置指定项目为指定类型, 组合框类型需要增加数据
    virtual void SetItemType(HTREEITEM hItem, PROPERTY_TYPE type) { tree.SetItemType(hItem, type); }

    // 取指定项目类型
    virtual PROPERTY_TYPE GetItemType(HTREEITEM hItem) { return tree.GetItemType(hItem); }

    // 设置宽度, 千分比, 最小100, 最大900, 控件未创建前也可以调用
    // isLeft = 是否为设置左边, 设置一边后另一边会一起调整
    // value = 100-900之间的值
    // isUpdate = 是否重画
    virtual void SetWidthRatio(bool isLeft, int value, bool isUpdate = false) { tree.SetWidthRatio(isLeft, value, isUpdate); }

    // 将当前表项所有数据保存成字节集数据, 返回的地址不使用时需要调用 free() 释放
    // isColor = 是否保存颜色配置信息
    virtual LPBYTE save(DWORD* size, bool isColor, bool isList) { return tree.save(size, isColor, isList); }

    // 从内存里加载所有项目数据
    virtual bool load(const void* data, DWORD size, bool isUpdate) { return tree.load(data, size, isUpdate); }

    // 设置颜色信息
    virtual void SetColor(const PROPERTY_COLOR* color, bool isUpdate) { return tree.SetColor(color, isUpdate); }
    virtual int GetColor(PROPERTY_COLOR* color) { return tree.GetColor(color); }
    // 默认配色, 0=黑色风格, 1=白色风格
    virtual int DefColor(int type, bool isUpdate) { return tree.DefColor(type, isUpdate); }

    virtual int SetItemFont(const LPLOGFONTW lf, bool isUpdate) { return tree.SetItemFont(lf, isUpdate); }
    // 默认为微软雅黑, 字体大小 -12, 没有样式
    // lfItalic = 斜体,  lfUnderline = 下划线, lfStrikeOut = 删除线
    virtual int SetItemFont(bool isUpdate, const wchar_t* name = L"微软雅黑", int size = -12, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false) {
        return SetItemFont(isUpdate, name, size, lfItalic, lfUnderline, lfStrikeOut);
    }

    virtual int SetColorBackground(ARGB cr, bool isUpdate = false) { return tree.SetColorBackground(cr, isUpdate); }
    virtual int SetColorBackParent(ARGB cr, bool isUpdate = false) { return tree.SetColorBackParent(cr, isUpdate); }
    virtual int SetColorText(ARGB cr, bool isUpdate = false) { return tree.SetColorText(cr, isUpdate); }
    virtual int SetColorListBorder(ARGB cr, bool isUpdate = false) { return tree.SetColorListBorder(cr, isUpdate); }
    virtual int SetColorSelect(ARGB cr, bool isUpdate = false) { return tree.SetColorSelect(cr, isUpdate); }
    virtual int SetColorDisabled(ARGB cr, bool isUpdate = false) { return tree.SetColorDisabled(cr, isUpdate); }
    virtual void SetIsFillColorBack(bool isFill) { return tree.SetIsFillColorBack(isFill); }

    virtual bool SetItemDisabledState(HTREEITEM hItem, bool isDisabled) { return tree.SetItemDisabledState(hItem, isDisabled); }
    // 禁止项目
    virtual bool DisabledItem(HTREEITEM hItem) { return tree.DisabledItem(hItem); }
    // 启用项目
    virtual bool EnabledItem(HTREEITEM hItem) { return tree.EnabledItem(hItem); }
    // 项目是否禁止
    virtual bool IsItemDisabled(HTREEITEM hItem) { return tree.IsItemDisabled(hItem); }
    // 项目是否启用
    virtual bool IsItemEnabled(HTREEITEM hItem) { return tree.IsItemEnabled(hItem); }

    virtual bool clear() { return tree.clear(); }

    // 弹出对话框, 根据当前项目类型探测对应对话框, 会触发按钮点击回调
    virtual void popDialog() { return tree.popDialog(); }

    // 获取属性框矩形
    virtual void GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type) { tree.GetPropertyItemRect(hItem, prc, type); }

    virtual LPBYTE GetExpand_Open(int* size) { return tree.GetExpand_Open(size); }
    virtual LPBYTE GetExpand_Close(int* size) { return tree.GetExpand_Close(size); }
    virtual bool SetExpand_Open(const void* pData, int size) { return tree.SetExpand_Open(pData, size); }
    virtual bool SetExpand_Close(const void* pData, int size) { return tree.SetExpand_Close(pData, size); }

public:


    // 项目属性被改变, 为空则取消挂接这个事件, 事件两个参数, 参数1是触发事件的项目句柄, 参数2是项目的类型
    virtual void OnPropertyChanged(fnPropertyChanged pfn = 0, void* param = 0) { return tree.OnPropertyChanged(pfn, param); }

    // 项目选择改变, 为空则取消挂接这个事件, 事件两个参数, 参数1是触发事件的项目句柄, 参数2是在哪个位置点击, -1=展开收缩按钮, 1=属性名, 2=属性值
    virtual void OnItemChanged(fnItemChanged pfn = 0, void* param = 0) { return tree.OnItemChanged(pfn, param); }

    // 项目属性值按下了回车键, 回调函数返回是否拦截, 为空则取消挂接这个事件, 如果回调函数返回true则不对按下回车键做进一步处理
    // 事件两个参数, 参数1是触发事件的项目句柄, 参数2是项目的类型
    virtual void OnPropertyReturn(fnPropertyReturn pfn = 0, void* param = 0) { return tree.OnPropertyReturn(pfn, param); }

    // 组合框项目鼠标滚动, 回调函数返回是否拦截, 如果回调函数返回true则不对鼠标滚动做进一步处理
    // count = 组合框项目总数, pos = 滚动距离负数向下滚动, 正数向上滚动, key = 滚动时按下了哪个按键, lParam = 鼠标坐标
    virtual void OnComboboxMouseWheel(fnComboboxMouseWheel pfn = 0, void* param = 0) { return tree.OnComboboxMouseWheel(pfn, param); }

    // 组合框项目改变, 回调函数返回是否拦截, 如果回调函数返回true则不会改变项目
    // index = 要改变的项目索引, 返回false后组合框现行选中项就是这个索引
    virtual void OnComboboxChanged(fnComboboxChanged pfn = 0, void* param = 0) { return tree.OnComboboxChanged(pfn, param); }

    // 组合框下拉箭头被按下, 即将探测列表, 回调函数返回是否拦截, 如果回调函数返回true则不会弹出列表
    virtual void OnDropDown(fnDropWown pfn = 0, void* param = 0) { return tree.OnDropDown(pfn, param); }

    // 属性按钮被单击, 回调函数返回是否拦截, 如果回调函数返回true则不会弹出内置的对话框
    virtual void OnItemButtonClick(fnItemButtonClick pfn = 0, void* param = 0) { return tree.OnItemButtonClick(pfn, param); }

    // 项目被右键单击, 返回是否拦截, 拦截后右键不会改变表项, 不会触发单击
    virtual void OnItemRButtonClick(fnItemRButtonClick pfn = 0, void* param = 0) { return tree.OnItemRButtonClick(pfn, param); }

    // 组合框双击编辑框, 回调函数返回是否拦截, 如果回调函数返回true则不对双击做进一步处理
    // index = 双击时组合框的索引, count = 组合框项目数
    virtual void OnComboboxLDoubleClick(fnComboboxLDoubleClick pfn = 0, void* param = 0) { return tree.OnComboboxLDoubleClick(pfn, param); }

    // 编辑框被双击, 回调函数返回是否拦截, 如果回调函数返回true则不全选编辑框
    virtual void OnEditLDoubleClick(fnEditLDoubleClick pfn = 0, void* param = 0) { return tree.OnEditLDoubleClick(pfn, param); }

    // 项目输入字符, 回调函数返回是否拦截, 如果回调函数返回true则使用 pRet 返回的值作为返回值
    // ch = 输入的字符, pRet = 返回值指针, 需要把返回值写入这里
    virtual void OnItemChar(fnItemChar pfn = 0, void* param = 0) { return tree.OnItemChar(pfn, param); }

    // 生成自定义属性值的显示名字, 三个参数, 项目句柄, 项目类型, 显示文本缓冲区, 最大100个字符
    virtual void OnItemMakeCustomValue(fnItemMakeCustomValue pfn = 0, void* param = 0) { return tree.OnItemMakeCustomValue(pfn, param); }

    // 右边小按钮被重画, 返回true则内部不进行绘画, 需要用户在回调里进行绘画操作
    // hdc = 按钮绘画dc, 不是内存DC, state = 当前状态, STATE::HOT, STATE::DOWN, width = 按钮宽度, height = 按钮高度
    virtual void OnItemRButtonPaint(fnItemRButtonPaint pfn = 0, void* param = 0) { return tree.OnItemRButtonPaint(pfn, param); }

    // del键放开, 删除当前项数据, 在属性名这一列按下的默认处理是清除值, 不是属性名按下的只清除双击会打开对话框的项目
    // isName = 是否在属性名这一列按下按键
    virtual void OnItemKeyDelUp(fnItemDelUp pfn = 0, void* param = 0) { return tree.OnItemKeyDelUp(pfn, param); }

    // 自定义属性名字
    // isName = 是否在属性名这一列按下按键
    virtual void OnCustomValueName(fnCustomValueName pfn = 0, void* param = 0) { return tree.OnCustomValueName(pfn, param); }

    // 项目读取数据失败, 字节集, 图片, 音频, 图片组这些....
    virtual void OnItemReadError(fnItemReadError pfn = 0, void* param = 0) { return tree.OnItemReadError(pfn, param); }

    // 即将绘画, 内部绘画前先调用用户的绘画
    virtual void OnItemBeginDraw(fnItemBeginDraw pfn = 0, void* param = 0) { tree.OnItemBeginDraw(pfn, param); }

    // 绘画结束, 内部绘画已经结束, 用户可以在这基础上进行二次绘画
    virtual void OnItemEndDraw(fnItemEndDraw pfn = 0, void* param = 0) { tree.OnItemEndDraw(pfn, param); }

    virtual bool Expand(HTREEITEM hItem, BOOL b) { return tree.Expand(hItem, b); }
    virtual bool Expand(HTREEITEM hItem) { return tree.Expand(hItem); }
    virtual bool IsExpand(HTREEITEM hItem) { return tree.IsExpand(hItem); }
    // 保证显示
    virtual bool EnsureVisible(HTREEITEM hItem) { return tree.EnsureVisible(hItem); }

    virtual bool Move(int x, int y, int cx, int cy, bool bRepaint = true) { return tree.Move(x, y, cx, cy, bRepaint); }




    virtual DWORD GetItemCount() { return tree.GetItemCount(); }
    virtual void InvalidateItem(HTREEITEM hItem) { tree.InvalidateItem(hItem); }

    // 置现行选中项
    virtual bool SetIndex(HTREEITEM hItem) { return tree.SetIndex(hItem); }

    // 取可视项目数
    virtual unsigned int GetVisibleCount() { return tree.GetVisibleCount(); }

    // 取第一个项目
    virtual HTREEITEM GetFirstItem() { return tree.GetFirstItem(); }

    // 取根项目
    virtual HTREEITEM GetRootItem() { return tree.GetRootItem(); }

    // 取子项目, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetChild(HTREEITEM hItem = 0) { return tree.GetChild(hItem); }

    // 取下一个兄弟项, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextSibling(HTREEITEM hItem = 0) { return tree.GetNextSibling(hItem); }

    // 取上一个兄弟项, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetPrevSibling(HTREEITEM hItem = 0) { return tree.GetPrevSibling(hItem); }

    // 取父项目, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetParent(HTREEITEM hItem = 0) { return tree.GetParent(hItem); }

    // 取第一个可视项目
    virtual HTREEITEM GetFirstVisible() { return tree.GetFirstVisible(); }

    // 取下一个可视, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextVisible(HTREEITEM hItem = 0) { return tree.GetNextVisible(hItem); }

    // 取上一个可视, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetPrevVisible(HTREEITEM hItem = 0) { return tree.GetPrevVisible(hItem); }

    // 取现行选中项
    virtual HTREEITEM GetSelection() { return tree.GetSelection(); }

    // 取现行选中项
    virtual HTREEITEM GetIndex() { return tree.GetIndex(); }
    // 取根项目
    virtual HTREEITEM GetRoot() { return tree.GetRoot(); }

    // 取最后一个可视项目
    virtual HTREEITEM GetLastVisible() { return tree.GetLastVisible(); }

    // 取下一个项目, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextItem(HTREEITEM hItem = 0, int code = TVGN_NEXT) { return tree.GetNextItem(); }



    // 取下一个选中项, 项目句柄为0则取当前选中项目句柄
    virtual HTREEITEM GetNextSelectedItem(HTREEITEM hItem = 0) { return tree.GetNextSelectedItem(hItem); }

    virtual HWND GetWindow() { return tree.GetWindow(); }
    // 取子项目数
    virtual int GetSubCount(HTREEITEM hItem) { return tree.GetSubCount(hItem); }
    // 取项目矩形, isTitle = 是否值包含标题
    virtual RECT GetItemRect(HTREEITEM hItem, bool isTitle = FALSE) { return tree.GetItemRect(hItem, isTitle); }
    // 计算中间这条线的位置
    virtual int calcMidLine(int cxClient = 0) { return tree.calcMidLine(cxClient); }


public:
    virtual void free(void* ptr) { if (!ptr)return; ::free(ptr); }




};


extern "C" API_DECLSPEC ICPropertyTree * CProperty_Create()
{
    return new ICPropertyTree_Realization;
}

extern "C" API_DECLSPEC ICMap* CMap_Create(bool isHash)
{
    typedef LPVOID Tname;
    typedef LPVOID Tvalue;
    if (isHash)
        return new CMap<Tname, Tvalue, std::unordered_map<Tname, Tvalue>>;
    return new CMap<Tname, Tvalue, std::map<Tname, Tvalue>>;

}
