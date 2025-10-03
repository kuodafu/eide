#ifndef INLINE_ASSEMBLER_API_EXPORT_H
#define INLINE_ASSEMBLER_API_EXPORT_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define PLUG_API extern "C" __declspec(dllimport)
#define PLUG_CALL WINAPI

// 易语言版本
enum class ElangVersion {
  kElangVersion_5_95 = 595,
  kElangVersion_5_93 = 593,
  kElangVersion_5_92_2 = 5922, // 易语言592 破解版??
  kElangVersion_5_92 = 592,
  kElangVersion_5_90 = 590,
  kElangVersion_5_81 = 581,
  kElangVersion_5_80 = 580,
  kElangVersion_5_71 = 571,
  kElangVersion_5_70 = 570,
  kElangVersion_5_60 = 560,
  kElangVersion_5_50 = 550,
  kElangVersion_5_41 = 541,
  kElangVersion_5_30 = 530,
  kElangVersion_5_20 = 520,
  kElangVersion_5_11 = 511,
  kElangVersion_UNINIT = 0,
  kElangVersion_UNKNOWN = -1,
};

// 源码类型
enum class SourceKind {
  // 窗体程序
  kSourceKindWindows,
  // 控制台
  kSourceKindConsole,
  // 动态库
  kSourceKindSharedLibrary,
  // 模块
  kSourceKindModule = 1000,
  // Linux 控制台
  kSourceKindLinuxConsole = 10000
};

// 光标位置
enum class CursorPosition {
  kCursorPositionNone = 0x0,                 // 0x0 无
  kCursorPositionDllCommand = 0x1,           // 0x1 DLL命令
  kCursorPositionDllLibrary = 0x2,           // 0x2 DLL库文件
  kCursorPositionDllFunction = 0x3,          // 0x3 DLL函数名
  kCursorPositionDllArgument = 0x4,          // 0x4 DLL参数
  kCursorPositionCustomDataType = 0x5,       // 0x5 自定义数据类型
  kCursorPositionCustomDataTypeMember = 0x6, // 0x6 自定义数据类型成员变量
  kCursorPositionGlobalVariable = 0x7,       // 0x7 全局变量
  kCursorPositionAssembly = 0x8,             // 0x8 程序集
  kCursorPositionAssemblyVariable = 0x9,     // 0x9 程序集变量
  kCursorPositionFuncHeader = 0xA,           // 0xA 函数表头
  kCursorPositionFuncArgument = 0xB,         // 0xB 函数参数
  kCursorPositionFuncVariable = 0xC,         // 0xC 函数变量
  kCursorPositionFuncCode = 0xD,             // 0xD 函数代码
  kCursorPositionResourceSound = 0x10,       // 0x10资源表声音
  kCursorPositionResourceImage = 0x11,       // 0x11资源表图片
};

// 常量资源类型
enum ConstResourcesType {
  kConstResourcesTypeNone = 0,  // 0 无效
  kConstResourcesTypeConst = 1, // 1 常量
  kConstResourcesTypeImage = 2, // 2 图片
  kConstResourcesTypeSound = 3, // 3 声音
};

// 当前活动编辑窗口的类型
enum class EditorType {
  kEditorTypeNone = 0,       // 无
  kEditorTypeAssembly,       // 程序集
  kEditorTypeDataType,       // 用户自定义数据类型
  kEditorTypeGlobalVariable, // 全局变量
  kEditorTypeDLLFunc,        // DLL命令
  kEditorTypeFormDesign,     // 窗体设计
  kEditorTypeConstResource,  // 常量资源
  kEditorTypeImageResource,  // 图片资源
  kEditorTypeSoundResource,  // 声音资源
};

#ifndef PFN_NOTIFY_SYS
typedef int(WINAPI *PFN_NOTIFY_SYS)(int message, DWORD param1, DWORD param2);
#endif

struct Assembly;    // 程序集
struct Function;    // 子程序
struct DLLFunction; // DLL命令
struct DataType;    // 自定义数据类型
struct Window;      // 窗体

class _ia_temp
{
public:

    // 设置dll的路径, 这个是易语言支持库目录下的 iDraw.fne 
    static const wchar_t* _ia_set_dll_name(const wchar_t* dllFileName)
    {
        static wchar_t fileName[260] = L"InlineAssembler.fne";
        if (!dllFileName || !dllFileName[0])
            return fileName;

        size_t len = wcslen(dllFileName);
        if (len >= 259)
            len = 259;
        memcpy(fileName, dllFileName, len * sizeof(wchar_t));
        fileName[len] = 0;
        return fileName;
    }

    // 获取dll路径
    static const wchar_t* _ia_get_dll_name()
    {
        return _ia_set_dll_name(0);
    }

    static void* _ia_get_address(const char* funName)
    {
        static HMODULE hModule = 0;
        if (!hModule)
            hModule = LoadLibraryW(_ia_get_dll_name());

        if (!hModule)throw; // 加载失败直接抛出异常
        return GetProcAddress(hModule, funName);
    }
};




#define IA_GET_PTR_ITERATOR(_name) \
    static __FN_##_name__ _PFN_CALLBACK_ = 0;\
    if(!_PFN_CALLBACK_) _PFN_CALLBACK_ = (__FN_##_name__)_ia_temp::_ia_get_address(#_name);\
    if (!_PFN_CALLBACK_) {\
        MessageBoxA(((HWND)0), "未能找到\""## #_name ##"\"接口, 程序无法正常启动", #_name ## "未找到", 0);\
        exit(0);\
    }

#define IA_FUNC0(_ret, _name) \
        inline _ret PLUG_CALL _name(){\
        typedef _ret(PLUG_CALL* __FN_##_name__)();\
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_();}

#define IA_FUNC1(_ret, _name, _1, _2) \
        inline _ret PLUG_CALL _name(_1 _2){\
        typedef _ret(PLUG_CALL* __FN_##_name__)(_1);\
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2);}


#define IA_FUNC2(_ret, _name, _1, _2, _3, _4) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4);}

#define IA_FUNC3(_ret, _name, _1, _2, _3, _4, _5, _6) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6);}

#define IA_FUNC4(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5, _7); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8);}

#define IA_FUNC5(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5, _7, _9); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10);}

#define IA_FUNC6(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12);}

#define IA_FUNC7(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14);}

#define IA_FUNC8(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16);}

#define IA_FUNC9(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) \
        inline _ret PLUG_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16, _17 _18){\
        typedef _ret(PLUG_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15, _17); \
        IA_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16, _18);}

// 易语言 notify_sys_pfn 函数指针
IA_FUNC0(PFN_NOTIFY_SYS, ia_notify_sys_pfn)
// 插件版本
IA_FUNC0(int, ia_version)
// 插件版本字符串
IA_FUNC0(const char *, ia_version_string)
// 插件是否支持
IA_FUNC0(BOOL, ia_is_support)
// 易语言版本
IA_FUNC0(ElangVersion, ia_epl_version)
// 易语言IDE路径
IA_FUNC0(const char *, ia_ide_path)
// 易语言IDE窗口句柄
IA_FUNC0(HWND, ia_ide_hwnd)
// 易语言IDE输出
IA_FUNC1(void, ia_ide_output, const char *, text)
// 输出一行
IA_FUNC1(void, ia_ide_output_line, const char *, text)
// 源码路径
IA_FUNC0(const char *, ia_source_path)
// 源码目录
IA_FUNC0(const char *, ia_source_dir)
// 输出路径
IA_FUNC0(const char *, ia_output_path)
// 输出目录
IA_FUNC0(const char *, ia_output_dir)
// 源码类型
IA_FUNC0(SourceKind, ia_source_kind)
// 申请内存
IA_FUNC1(void *, ia_malloc, size_t, size)
// 释放内存
IA_FUNC1(void, ia_free, void *, ptr)
// 易语言运行状态 0 未运行 1 运行中 2 暂停
IA_FUNC0(int, ia_run_status)
// 是否正在调试
IA_FUNC0(BOOL, ia_is_debugging)
// 调试的进程id
IA_FUNC0(DWORD, ia_debugging_process_id)
// 调试的进程句柄
IA_FUNC0(HANDLE, ia_debugging_process_handle)
// 调试的线程id
IA_FUNC0(DWORD, ia_debugging_thread_id)
// 调试的线程句柄
IA_FUNC0(HANDLE, ia_debugging_thread_handle)
// 通过ID获取数据类型名称
IA_FUNC1(const char *, ia_datatype_id_to_name, int, datatype_id)
// 通过名称获取数据类型ID
IA_FUNC1(size_t, ia_datatype_name_to_id, const char *, datatype_name)
// 通过函数ID获取变量注释
IA_FUNC2(const char *, ia_function_get_variable_comment_by_id, size_t, func_id, const char *, variable_name)
// 通过函数名称获取变量注释
IA_FUNC2(const char *, ia_function_get_variable_comment, const char *, func_name, const char *, variable_name)
// 通过函数ID获取函数备注
IA_FUNC1(const char *, ia_function_get_comment_by_id, size_t, func_id)
// 通过函数名称获取函数备注
IA_FUNC1(const char *, ia_function_get_comment, const char *, func_name)

///////////////////////////////////////////////////////////////////////////////////
// 程序集

// 程序集数量
IA_FUNC0(size_t, ia_assembly_count)
// 通过名称获取程序集 id 保存ID
IA_FUNC2(Assembly*, ia_get_assembly, const char *, name, size_t *, id)
// 通过ID获取程序集
IA_FUNC1(Assembly*, ia_get_assembly_by_id, size_t, id)
// 通过索引获取程序集 id 保存ID
IA_FUNC2(Assembly*, ia_get_assembly_by_index, size_t, index, size_t *, id)
// 程序集ID
IA_FUNC1(size_t, ia_assembly_get_id, Assembly *, ass)
// 程序集名称
IA_FUNC1(const char *, ia_assembly_get_name, Assembly *, ass)
// 设置 程序集名称
IA_FUNC2(void, ia_assembly_set_name, Assembly *, ass, const char *, name)
// 程序集注释
IA_FUNC1(const char *, ia_assembly_get_comment, Assembly *, ass)
// 设置 程序集注释
IA_FUNC2(void, ia_assembly_set_comment, Assembly *, ass, const char *, comment)
// 程序集是否公开
IA_FUNC1(BOOL, ia_assembly_is_public, Assembly *, ass)
// 设置程序集是否公开
IA_FUNC2(void, ia_assembly_set_public, Assembly *, ass, BOOL, is_public)
// 程序集是否是类
IA_FUNC1(BOOL, ia_assembly_is_class, Assembly *, ass)
// 程序集基类ID
IA_FUNC1(size_t, ia_assembly_get_base_id, Assembly *, ass)
// 设置程序集基类ID
IA_FUNC2(void, ia_assembly_set_base_id, Assembly *, ass, size_t, base_id)
// 程序集子程序数量
IA_FUNC1(size_t, ia_assembly_get_func_count, Assembly *, ass)
// 程序集子程序ID
IA_FUNC2(size_t, ia_assembly_get_func_id, Assembly *, ass, size_t, index)
// 程序集变量数量
IA_FUNC1(size_t, ia_assembly_get_variable_count, Assembly *, ass)
// 程序集变量ID
IA_FUNC2(size_t, ia_assembly_get_variable_id, Assembly *, ass, size_t, index)
// 程序集变量名称
IA_FUNC2(const char *, ia_assembly_get_variable_name, Assembly *, ass, size_t, index)
// 设置 程序集变量名称
IA_FUNC3(void, ia_assembly_set_variable_name, Assembly *, ass, size_t, index, const char *, name)
// 程序集变量类型ID
IA_FUNC2(int, ia_assembly_get_variable_datatype_id, Assembly *, ass, size_t, index)
// 设置 程序集变量类型ID
IA_FUNC3(void, ia_assembly_set_variable_datatype_id, Assembly *, ass, size_t, index, int, datatype_id)
// 程序集变量是否是数组
IA_FUNC2(BOOL, ia_assembly_get_variable_is_array, Assembly *, ass, size_t, index)

// 新增程序集(名称,注释,基类ID -1 为类 0 普通程序集 大于0 为基类ID)
IA_FUNC3(Assembly*, ia_assembly_add, const char *, name, const char *, comment, int, base_class_id)






///////////////////////////////////////////////////////////////////////////////////
// 子程序

// 子程序数量
IA_FUNC0(size_t, ia_func_count)
// 通过名称获取子程序 func_id 保存ID
IA_FUNC2(Function*, ia_get_func, const char *, name, size_t *, func_id)
// 通过ID获取子程序
IA_FUNC1(Function*, ia_get_func_by_id, size_t, func_id)
// 通过索引获取子程序 func_id 保存ID
IA_FUNC2(Function*, ia_get_func_by_index, size_t, index, size_t *, func_id)
// 子程序ID
IA_FUNC1(size_t, ia_func_get_id, Function *, func)
// 子程序名称
IA_FUNC1(const char *, ia_func_get_name, Function *, func)
// 设置 子程序名称
IA_FUNC2(void, ia_func_set_name, Function *, func, const char *, name)
// 子程序易包
IA_FUNC1(const char *, ia_func_get_epk, Function *, func)
// 设置 子程序易包
IA_FUNC2(void, ia_func_set_epk, Function *, func, const char *, epk)
// 子程序注释
IA_FUNC1(const char *, ia_func_get_comment, Function *, func)
// 设置 子程序注释
IA_FUNC2(void, ia_func_set_comment, Function *, func, const char *, comment)
// 子程序程序集ID
IA_FUNC1(size_t, ia_func_get_assembly_id, Function *, func)
// 设置 子程序程序集ID
IA_FUNC2(void, ia_func_set_assembly_id, Function *, func, size_t, assembly_id)
// 子程序返回值类型ID
IA_FUNC1(int, ia_func_get_ret_datatype_id, Function *, func)
// 设置 子程序返回值类型ID
IA_FUNC2(void, ia_func_set_ret_datatype_id, Function *, func, int, datatype_id)
// 子程序变量数量
IA_FUNC1(size_t, ia_func_get_variable_count, Function *, func)
// 子程序变量ID
IA_FUNC2(size_t, ia_func_get_variable_id, Function *, func, size_t, index)
// 子程序变量名称
IA_FUNC2(const char *, ia_func_get_variable_name, Function *, func, size_t, index)
// 设置 子程序变量名称
IA_FUNC3(void, ia_func_set_variable_name, Function *, func, size_t, index, const char *, name)
// 子程序变量类型ID
IA_FUNC2(int, ia_func_get_variable_datatype_id, Function *, func, size_t, index)
// 设置 子程序变量类型ID
IA_FUNC3(void, ia_func_set_variable_datatype_id, Function *, func, size_t, index, int, datatype_id)
// 子程序变量是否静态
IA_FUNC2(BOOL, ia_func_get_variable_is_static, Function *, func, size_t, index)
// 设置 子程序变量是否静态
IA_FUNC3(void, ia_func_set_variable_static, Function *, func, size_t, index, BOOL, is_static)
// 子程序变量是否数组
IA_FUNC2(BOOL, ia_func_get_variable_is_array, Function *, func, size_t, index)
// 子程序参数数量
IA_FUNC1(size_t, ia_func_get_param_count, Function *, func)
// 子程序参数ID
IA_FUNC2(size_t, ia_func_get_param_id, Function *, func, size_t, index)
// 子程序参数名称
IA_FUNC2(const char *, ia_func_get_param_name, Function *, func, size_t, index)
// 设置 子程序参数名称
IA_FUNC3(void, ia_func_set_param_name, Function *, func, size_t, index, const char *, name)
// 子程序参数类型ID
IA_FUNC2(int, ia_func_get_param_datatype_id, Function *, func, size_t, index)
// 设置 子程序参数类型ID
IA_FUNC3(void, ia_func_set_param_datatype_id, Function *, func, size_t, index, int, datatype_id)
// 子程序参数是否参考(引用)
IA_FUNC2(BOOL, ia_func_get_param_is_reference, Function *, func, size_t, index)
// 设置 子程序参数是否参考(引用)
IA_FUNC3(void, ia_func_set_param_reference, Function *, func, size_t, index, BOOL, is_ref)
// 子程序参数是否数组
IA_FUNC2(BOOL, ia_func_get_param_is_array, Function *, func, size_t, index)
// 设置 子程序参数是否数组
IA_FUNC3(void, ia_func_set_param_array, Function *, func, size_t, index, BOOL, is_array)
// 子程序是否公开
IA_FUNC1(BOOL, ia_func_is_public, Function *, func)
// 设置 子程序是否公开
IA_FUNC2(void, ia_func_set_public, Function *, func, BOOL, is_public)
// 子程序是否隐藏
IA_FUNC1(BOOL, ia_func_is_hidden, Function *, func)
// 设置 子程序是否隐藏
IA_FUNC2(void, ia_func_set_hidden, Function *, func, BOOL, is_hidden)





// 20240129 新增

// 函数变量数组维数
IA_FUNC2(size_t, ia_func_get_variable_array_dimension_count, Function *, func, size_t, index)




// 函数变量数组维数数据 dimension_data 维数数据 缓冲区大小为 dimension_count * sizeof(size_t)
PLUG_API void PLUG_CALL ia_func_get_variable_array_data(Function *func, size_t index, size_t *dimension_data);
// 设置 函数变量数组维数数据 dimension_data 维数数据 1,2,3  dimension_count 维数数量 3
PLUG_API void PLUG_CALL ia_func_set_variable_array_data(Function *func, size_t index, size_t *dimension_data,
                                                        size_t dimension_count);
// 设置1维数组数组大小
PLUG_API void PLUG_CALL ia_func_set_variable_array_size(Function *func, size_t index, size_t size);

// 新增函数局部变量 返回值为变量ID 名称 注释 类型ID 是否静态 变量ID保存 index 保存变量索引(可空)
PLUG_API size_t PLUG_CALL ia_func_add_variable(Function *func, const char *name, const char *comment, int datatype_id,
                                               BOOL is_static, size_t *index);
// 新增函数参数 返回值为参数ID 名称 注释 类型ID 是否引用 是否可空 是否数组 参数ID保存 index 保存参数索引(可空)
PLUG_API size_t PLUG_CALL ia_func_add_param(Function *func, const char *name, const char *comment, int datatype_id,
                                            BOOL is_reference, BOOL is_nullable, BOOL is_array, size_t *index);
// 删除函数局部变量
PLUG_API void PLUG_CALL ia_func_remove_variable(Function *func, size_t index);
// 删除函数参数
PLUG_API void PLUG_CALL ia_func_remove_param(Function *func, size_t index);
// 清空函数局部变量
PLUG_API void PLUG_CALL ia_func_clear_variables(Function *func);
// 清空函数参数
PLUG_API void PLUG_CALL ia_func_clear_params(Function *func);
// 子程序参数是否可空
PLUG_API BOOL PLUG_CALL ia_func_get_param_is_nullable(Function *func, size_t index);
// 设置 子程序参数是否可空
PLUG_API void PLUG_CALL ia_func_set_param_nullable(Function *func, size_t index, BOOL is_nullable);

// 新增子程序 名称 注释 易包 返回值类型ID 程序集ID 是否公开 函数ID保存
PLUG_API Function *PLUG_CALL ia_func_add(const char *name, const char *comment, const char *epk, int datatype_id, size_t ass_id,
                                         BOOL is_public, size_t *func_id);
// 删除子程序
PLUG_API void PLUG_CALL ia_func_remove(Function *func);
// 删除子程序
PLUG_API void PLUG_CALL ia_func_remove_by_id(size_t func_id);

///////////////////////////////////////////////////////////////////////////////////
// DLL 命令

// DLL 命令数量
PLUG_API size_t PLUG_CALL ia_dll_func_count();
// 获取DLL命令 id 保存ID
PLUG_API DLLFunction *PLUG_CALL ia_dll_func_get(const char *name, size_t *id);
// 通过ID获取DLL命令
PLUG_API DLLFunction *PLUG_CALL ia_dll_func_get_by_id(size_t id);
// 通过索引获取DLL命令 id 保存ID
PLUG_API DLLFunction *PLUG_CALL ia_dll_func_get_by_index(size_t index, size_t *id);
// DLL命令名称
PLUG_API const char *PLUG_CALL ia_dll_func_get_name(DLLFunction *dll_func);
// 设置 DLL命令名称
PLUG_API void PLUG_CALL ia_dll_func_set_name(DLLFunction *dll_func, const char *name);
// DLL命令模块名称
PLUG_API const char *PLUG_CALL ia_dll_func_get_module_name(DLLFunction *dll_func);
// 设置 DLL命令模块名称
PLUG_API void PLUG_CALL ia_dll_func_set_module_name(DLLFunction *dll_func, const char *module_name);
// DLL命令函数名称
PLUG_API const char *PLUG_CALL ia_dll_func_get_func_name(DLLFunction *dll_func);
// 设置 DLL命令函数名称
PLUG_API void PLUG_CALL ia_dll_func_set_func_name(DLLFunction *dll_func, const char *func_name);
// DLL命令注释
PLUG_API const char *PLUG_CALL ia_dll_func_get_comment(DLLFunction *dll_func);
// 设置 DLL命令注释
PLUG_API void PLUG_CALL ia_dll_func_set_comment(DLLFunction *dll_func, const char *comment);
// DLL命令返回值类型ID
PLUG_API int PLUG_CALL ia_dll_func_get_ret_datatype_id(DLLFunction *dll_func);
// 设置 DLL命令返回值类型ID
PLUG_API void PLUG_CALL ia_dll_func_set_ret_datatype_id(DLLFunction *dll_func, int datatype_id);
// DLL命令参数数量
PLUG_API size_t PLUG_CALL ia_dll_func_get_param_count(DLLFunction *dll_func);
// DLL命令参数ID
PLUG_API size_t PLUG_CALL ia_dll_func_get_param_id(DLLFunction *dll_func, size_t index);
// DLL命令参数类型ID
PLUG_API int PLUG_CALL ia_dll_func_get_param_datatype_id(DLLFunction *dll_func, size_t index);
// 设置 DLL命令参数类型ID
PLUG_API void PLUG_CALL ia_dll_func_set_param_datatype_id(DLLFunction *dll_func, size_t index, size_t datatype_id);
// DLL命令参数名称
PLUG_API const char *PLUG_CALL ia_dll_func_get_param_name(DLLFunction *dll_func, size_t index);
// 设置 DLL命令参数名称
PLUG_API void PLUG_CALL ia_dll_func_set_param_name(DLLFunction *dll_func, size_t index, const char *name);
// DLL命令参数注释
PLUG_API const char *PLUG_CALL ia_dll_func_get_param_comment(DLLFunction *dll_func, size_t index);
// 设置 DLL命令参数注释
PLUG_API void PLUG_CALL ia_dll_func_set_param_comment(DLLFunction *dll_func, size_t index, const char *comment);
// DLL命令参数是否为引用
PLUG_API BOOL PLUG_CALL ia_dll_func_get_param_is_reference(DLLFunction *dll_func, size_t index);
// 设置 DLL命令参数是否为引用
PLUG_API void PLUG_CALL ia_dll_func_set_param_reference(DLLFunction *dll_func, size_t index, BOOL is_ref);
// DLL命令参数是否为数组
PLUG_API BOOL PLUG_CALL ia_dll_func_get_param_is_array(DLLFunction *dll_func, size_t index);
// 设置 DLL命令参数是否为数组
PLUG_API void PLUG_CALL ia_dll_func_set_param_array(DLLFunction *dll_func, size_t index, BOOL is_array);
// DLL命令是否公开
PLUG_API BOOL PLUG_CALL ia_dll_func_is_public(DLLFunction *dll_func);
// 设置 DLL命令是否公开
PLUG_API void PLUG_CALL ia_dll_func_set_public(DLLFunction *dll_func, BOOL is_public);

// 20240129 新增

// 新增DLL命令 name 名称  module 模块  func_name 命令名 ret_datatype_id 返回值类型ID is_public 是否公开 id 保存ID
PLUG_API DLLFunction *PLUG_CALL ia_dll_func_add(const char *name, const char *module, const char *func_name,
                                                size_t ret_datatype_id, BOOL is_public, size_t *id);
// 移除DLL命令
PLUG_API void PLUG_CALL ia_dll_func_remove(DLLFunction *dll_func);
PLUG_API void PLUG_CALL ia_dll_func_remove_by_id(size_t dll_func_id);
// DLL命令新增参数
PLUG_API size_t PLUG_CALL ia_dll_func_param_add(DLLFunction *dll_func, const char *name, const char *comment, int datatype_id,
                                                BOOL is_reference, BOOL is_array, size_t *index);
// DLL命令移除参数
PLUG_API void PLUG_CALL ia_dll_func_param_remove(DLLFunction *dll_func, size_t index);

///////////////////////////////////////////////////////////////////////////////////
// 自定义数据类型

// 自定义数据类型数量
PLUG_API size_t PLUG_CALL ia_datatype_count();
// 通过名称获取自定义数据类型 index 保存索引 返回值 为自定义数据类型ID
PLUG_API DataType *PLUG_CALL ia_datatype_get(const char *name, size_t *id);
// 通过ID获取自定义数据类型
PLUG_API DataType *PLUG_CALL ia_datatype_get_by_id(size_t id);
// 通过索引获取自定义数据类型 id 保存ID
PLUG_API DataType *PLUG_CALL ia_datatype_get_by_index(size_t index, size_t *id);
// 自定义数据类型名称
PLUG_API const char *PLUG_CALL ia_datatype_get_name(DataType *datatype);
// 设置 自定义数据类型名称
PLUG_API void PLUG_CALL ia_datatype_set_name(DataType *datatype, const char *name);
// 自定义数据类型注释
PLUG_API const char *PLUG_CALL ia_datatype_get_comment(DataType *datatype);
// 设置 自定义数据类型注释
PLUG_API void PLUG_CALL ia_datatype_set_comment(DataType *datatype, const char *comment);
// 自定义数据类型成员变量数量
PLUG_API size_t PLUG_CALL ia_datatype_get_variable_count(DataType *datatype);
// 自定义数据类型成员变量ID
PLUG_API size_t PLUG_CALL ia_datatype_get_variable_id(DataType *datatype, size_t index);
// 自定义数据类型成员变量名称
PLUG_API const char *PLUG_CALL ia_datatype_get_variable_name(DataType *datatype, size_t index);
// 设置 自定义数据类型成员变量名称
PLUG_API void PLUG_CALL ia_datatype_set_variable_name(DataType *datatype, size_t index, const char *name);
// 自定义数据类型成员变量注释
PLUG_API const char *PLUG_CALL ia_datatype_get_variable_comment(DataType *datatype, size_t index);
// 设置 自定义数据类型成员变量注释
PLUG_API void PLUG_CALL ia_datatype_set_variable_comment(DataType *datatype, size_t index, const char *comment);
// 自定义数据类型成员变量类型ID
PLUG_API int PLUG_CALL ia_datatype_get_variable_datatype_id(DataType *datatype, size_t index);
// 设置 自定义数据类型成员变量类型ID
PLUG_API void PLUG_CALL ia_datatype_set_variable_datatype_id(DataType *datatype, size_t index, int datatype_id);
// 自定义数据类型成员变量是否为引用
PLUG_API BOOL PLUG_CALL ia_datatype_get_variable_is_reference(DataType *datatype, size_t index);
// 设置 自定义数据类型成员变量是否为引用
PLUG_API void PLUG_CALL ia_datatype_set_variable_reference(DataType *datatype, size_t index, BOOL is_ref);
// 自定义数据类型成员变量是否为数组
PLUG_API BOOL PLUG_CALL ia_datatype_get_variable_is_array(DataType *datatype, size_t index);
// 自定义数据类型是否公开
PLUG_API BOOL PLUG_CALL ia_datatype_is_public(DataType *datatype);
// 设置 自定义数据类型是否公开
PLUG_API void PLUG_CALL ia_datatype_set_public(DataType *datatype, BOOL is_public);

// 20240129 新增
// 自定义数据类型成员数组维数
PLUG_API size_t PLUG_CALL ia_datatype_get_variable_array_dimension_count(DataType *datatype, size_t index);
// 自定义数据类型成员数组维数数据 dimension_data 维数数据
PLUG_API void PLUG_CALL ia_datatype_get_variable_array_data(DataType *datatype, size_t index, size_t *dimension_data);
// 设置 自定义数据类型成员数组维数数据 dimension_data 维数数据 dimension_count 维数数量
PLUG_API void PLUG_CALL ia_datatype_set_variable_array_data(DataType *datatype, size_t index, size_t *dimension_data,
                                                            size_t dimension_count);

// 新增自定义数据类型 name 名称 comment 注释 is_public 是否公开 id 保存ID
PLUG_API DataType *PLUG_CALL ia_datatype_add(const char *name, const char *comment, BOOL is_public, size_t *id);
// 移除自定义数据类型
PLUG_API void PLUG_CALL ia_datatype_remove(DataType *datatype);
PLUG_API void PLUG_CALL ia_datatype_remove_by_id(int id);
// 新增自定义数据类型成员变量 name 名称 comment 注释 datatype_id 类型ID is_reference 是否引用 array 数组大小(-1不是数组) id 保存ID
// index 保存索引
PLUG_API size_t PLUG_CALL ia_datatype_variable_add(DataType *datatype, const char *name, const char *comment, int datatype_id,
                                                   BOOL is_reference, size_t array, size_t *id, size_t *index);
// 新增自定义数据类型成员变量 name 名称 comment 注释 datatype_id 类型ID is_reference 是否引用 dimension_data 维数数据
// dimension_count 维数数量 id 保存ID index 保存索引
PLUG_API size_t PLUG_CALL ia_datatype_variable_addex(DataType *datatype, const char *name, const char *comment, int datatype_id,
                                                     BOOL is_reference, size_t *dimension_data, size_t dimension_count,
                                                     size_t *id, size_t *index);

///////////////////////////////////////////////////////////////////////////////////
// 全局变量

// 全局变量数量
PLUG_API size_t PLUG_CALL ia_global_variable_count();
// 通过名称获取全局变量 index 保存索引 返回值 为全局变量ID
PLUG_API size_t PLUG_CALL ia_get_global_variable(const char *name, size_t *index);
// 全局变量名称
PLUG_API const char *PLUG_CALL ia_global_variable_get_name(size_t index);
// 设置 全局变量名称
PLUG_API void PLUG_CALL ia_global_variable_set_name(size_t index, const char *name);
// 全局变量类型ID
PLUG_API int PLUG_CALL ia_global_variable_get_datatype_id(size_t index);
// 设置 全局变量类型ID
PLUG_API void PLUG_CALL ia_global_variable_set_datatype_id(size_t index, int datatype_id);
// 全局变量注释
PLUG_API const char *PLUG_CALL ia_global_variable_get_comment(size_t index);
// 设置 全局变量注释
PLUG_API void PLUG_CALL ia_global_variable_set_comment(size_t index, const char *comment);
// 全局变量ID
PLUG_API size_t PLUG_CALL ia_global_variable_get_id(size_t index);
// 全局变量是否为数组
PLUG_API BOOL PLUG_CALL ia_global_variable_is_array(size_t index);
// 全局变量是否公开
PLUG_API BOOL PLUG_CALL ia_global_variable_is_public(size_t index);
// 设置 全局变量是否公开
PLUG_API void PLUG_CALL ia_global_variable_set_public(size_t index, BOOL is_public);

///////////////////////////////////////////////////////////////////////////////////
// 常量资源表

// 常量资源表数量
PLUG_API size_t PLUG_CALL ia_const_resources_count();
// 通过名称获取常量资源表 index 保存索引 返回值 为常量资源ID
PLUG_API size_t PLUG_CALL ia_get_const_resources(const char *name, size_t *index);
// 常量资源ID
PLUG_API size_t PLUG_CALL ia_const_resources_get_id(size_t index);
// 常量资源名称
PLUG_API const char *PLUG_CALL ia_const_resources_get_name(size_t index);
// 设置常量资源名称
PLUG_API void PLUG_CALL ia_const_resources_set_name(size_t index, const char *name);
// 常量资源注释
PLUG_API const char *PLUG_CALL ia_const_resources_get_comment(size_t index);
// 设置常量资源注释
PLUG_API void PLUG_CALL ia_const_resources_set_comment(size_t index, const char *comment);
// 常量资源是否为公开
PLUG_API BOOL PLUG_CALL ia_const_resources_is_public(size_t index);
// 设置常量资源是否为公开
PLUG_API void PLUG_CALL ia_const_resources_set_public(size_t index, BOOL is_public);
// 常量资源类型
PLUG_API ConstResourcesType PLUG_CALL ia_const_resources_get_type(size_t index);
// 常量资源类型ID
PLUG_API size_t PLUG_CALL ia_const_resources_get_datatype_id(size_t index);
// 常量资源 大小 (图片/声音/文本类型需要获取大小 申请内存 调用 ia_const_resources_read_data 读取数据)
PLUG_API size_t PLUG_CALL ia_const_resources_get_data_size(size_t index);
// 读取常量double数据
PLUG_API double PLUG_CALL ia_const_resources_read_double(size_t index);
// 读取常量布尔值
PLUG_API BOOL PLUG_CALL ia_const_resources_read_bool(size_t index);
// 读取常量数据
PLUG_API BOOL PLUG_CALL ia_const_resources_read_data(size_t index, void *buffer, size_t *size);

///////////////////////////////////////////////////////////////////////////////////
// 模块

// 模块数量
PLUG_API size_t PLUG_CALL ia_ecom_count();
// 模块名称
PLUG_API const char *PLUG_CALL ia_ecom_get_name(size_t index);
// 模块路径 如果名称$开头 表面是相对路径 易语言/ecom/目录下的模块
PLUG_API const char *PLUG_CALL ia_ecom_get_path(size_t index);
// 移除模块
PLUG_API void PLUG_CALL ia_ecom_remove(size_t index);
// 导入模块 path 路径
PLUG_API BOOL PLUG_CALL ia_ecom_import(const char *path);

///////////////////////////////////////////////////////////////////////////////////
// 条件宏
// 条件宏数量
PLUG_API size_t PLUG_CALL ia_cond_macro_count();
// 通过名称获取条件宏 返回值 index
PLUG_API size_t PLUG_CALL ia_cond_macro_get(const char *name);
// 条件宏名称
PLUG_API const char *PLUG_CALL ia_cond_macro_get_name(size_t index);
// 设置 条件宏名称
PLUG_API void PLUG_CALL ia_cond_macro_set_name(size_t index, const char *name);
// 条件宏值
PLUG_API const char *PLUG_CALL ia_cond_macro_get_value(size_t index);
// 设置 条件宏值
PLUG_API void PLUG_CALL ia_cond_macro_set_value(size_t index, const char *value);
// 条件宏激活的索引
PLUG_API size_t PLUG_CALL ia_cond_macro_get_active();
// 设置 条件宏激活的索引
PLUG_API void PLUG_CALL ia_cond_macro_set_active(size_t index);
// 添加 条件宏(如果宏已存在 则修改宏值)
PLUG_API void PLUG_CALL ia_cond_macro_add(const char *name, const char *value);
// 移除 条件宏 参数 索引
PLUG_API void PLUG_CALL ia_cond_macro_remove_at(size_t index);
// 移除 条件宏 参数 名称
PLUG_API void PLUG_CALL ia_cond_macro_remove(const char *name);

///////////////////////////////////////////////////////////////////////////////////
// 光标位置
PLUG_API CursorPosition PLUG_CALL ia_cursor_position();
// 光标所在函数 func_id 保存函数ID 返回值 为函数指针
PLUG_API Function *PLUG_CALL ia_cursor_function(size_t *func_id);
// 当前活动的编辑器窗口句柄
PLUG_API HWND PLUG_CALL ia_active_editor_hwnd();
// 当前活动的编辑器类型
PLUG_API EditorType PLUG_CALL ia_active_editor_type();

// 内联汇编关键字
PLUG_API const char *PLUG_CALL ia_keyword();
// 20240127 新增

// IDE 菜单条Menu
PLUG_API HMENU PLUG_CALL ia_ide_menu_bar();

// 程序集变量数组维数
PLUG_API size_t PLUG_CALL ia_assembly_get_variable_array_dimension_count(Assembly *ass, size_t index);
// 程序集变量数组维数数据 dimension_data 维数数据
PLUG_API void PLUG_CALL ia_assembly_get_variable_array_data(Assembly *ass, size_t index, size_t *dimension_data);
// 设置 程序集变量数组维数数据 dimension_data 维数数据 dimension_count 维数数量
PLUG_API void PLUG_CALL ia_assembly_set_variable_array_data(Assembly *ass, size_t index, size_t *dimension_data,
                                                            size_t dimension_count);

// 全局变量数组维数
PLUG_API size_t PLUG_CALL ia_global_variable_get_array_dimension_count(size_t index);
// 全局变量数组维数数据 dimension_data 维数数据
PLUG_API void PLUG_CALL ia_global_variable_get_array_data(size_t index, size_t *dimension_data);
// 设置全局变量数组维数数据 dimension_data 维数数据 dimension_count 维数数量
PLUG_API void PLUG_CALL ia_global_variable_set_array_data(size_t index, size_t *dimension_data, size_t dimension_count);

//struct LIB_INFO;
// 引用支持库列表
PLUG_API size_t PLUG_CALL ia_fne_size();
// 引用支持库guid
PLUG_API const char *PLUG_CALL ia_fne_get_guid(size_t index);
// 引用支持库LIB_INFO
PLUG_API LIB_INFO *PLUG_CALL ia_fne_get_lib_info(size_t index);
// 引用支持库模块句柄
PLUG_API HMODULE PLUG_CALL ia_fne_get_module(size_t index);

// 程序配置
// 名称
PLUG_API const char *PLUG_CALL ia_program_config_get_name();
PLUG_API void PLUG_CALL ia_program_config_set_name(const char *name);
// 描述
PLUG_API const char *PLUG_CALL ia_program_config_get_desc();
PLUG_API void PLUG_CALL ia_program_config_set_desc(const char *desc);
// 作者
PLUG_API const char *PLUG_CALL ia_program_config_get_author();
PLUG_API void PLUG_CALL ia_program_config_set_author(const char *author);
// 邮政编码
PLUG_API const char *PLUG_CALL ia_program_config_get_zip_code();
PLUG_API void PLUG_CALL ia_program_config_set_zip_code(const char *zip_code);
// 联系地址
PLUG_API const char *PLUG_CALL ia_program_config_get_address();
PLUG_API void PLUG_CALL ia_program_config_set_address(const char *address);
// 电话号码
PLUG_API const char *PLUG_CALL ia_program_config_get_phone();
PLUG_API void PLUG_CALL ia_program_config_set_phone(const char *phone);
// 传真
PLUG_API const char *PLUG_CALL ia_program_config_get_fax();
PLUG_API void PLUG_CALL ia_program_config_set_fax(const char *fax);
// 电子邮件
PLUG_API const char *PLUG_CALL ia_program_config_get_email();
PLUG_API void PLUG_CALL ia_program_config_set_email(const char *email);
// 网址
PLUG_API const char *PLUG_CALL ia_program_config_get_homepage();
PLUG_API void PLUG_CALL ia_program_config_set_homepage(const char *homepage);
// 版权信息
PLUG_API const char *PLUG_CALL ia_program_config_get_copyright();
PLUG_API void PLUG_CALL ia_program_config_set_copyright(const char *copyright);
// 版本号major.minor.build 主.次.创建.修订
PLUG_API int PLUG_CALL ia_program_config_get_major_version();
PLUG_API void PLUG_CALL ia_program_config_set_major_version(int major_version);
PLUG_API int PLUG_CALL ia_program_config_get_minor_version();
PLUG_API void PLUG_CALL ia_program_config_set_minor_version(int minor_version);
PLUG_API int PLUG_CALL ia_program_config_get_build_version();
PLUG_API void PLUG_CALL ia_program_config_set_build_version(int build_version);
PLUG_API int PLUG_CALL ia_program_config_get_revise_version();
PLUG_API void PLUG_CALL ia_program_config_set_revise_version(int revise_version);
// 版本号major.minor.build 主.次.创建.修订
PLUG_API void PLUG_CALL ia_program_config_set_version(int major_version, int minor_version, int build_version,
                                                      int revise_version);

// 20240617 新增
// 程序集所属窗口ID
PLUG_API size_t PLUG_CALL ia_assembly_get_window_id(Assembly *ass);
// 设置程序集所属窗口ID
PLUG_API void PLUG_CALL ia_assembly_set_window_id(Assembly *ass, size_t window_id);
// 窗口部分
// 窗口数量
PLUG_API size_t PLUG_CALL ia_window_count();
// 获取窗口对象
PLUG_API Window *PLUG_CALL ia_get_window(size_t index, size_t *id);
// 窗口名称
PLUG_API const char *PLUG_CALL ia_window_get_name(Window *window);
// 设置窗口名称
PLUG_API void PLUG_CALL ia_window_set_name(Window *window, const char *name);
// 窗口备注
PLUG_API const char *PLUG_CALL ia_window_get_comment(Window *window);
// 设置窗口备注
PLUG_API void PLUG_CALL ia_window_set_comment(Window *window, const char *comment);
// 窗口程序集ID
PLUG_API size_t PLUG_CALL ia_window_get_assembly_id(Window *window);
// 设置窗口程序集ID
PLUG_API void PLUG_CALL ia_window_set_assembly_id(Window *window, size_t assembly_id);
// 子控件数量
PLUG_API size_t PLUG_CALL ia_window_child_count(Window *window);
// 获取子控件ID
PLUG_API size_t PLUG_CALL ia_window_child_get_id(Window *window, size_t index);
// 获取子控件名称
PLUG_API const char *PLUG_CALL ia_window_child_get_name(Window *window, size_t index);
// 设置子控件名称
PLUG_API void PLUG_CALL ia_window_child_set_name(Window *window, size_t index, const char *name);
// 获取子控件备注
PLUG_API const char *PLUG_CALL ia_window_child_get_comment(Window *window, size_t index);
// 设置子控件备注
PLUG_API void PLUG_CALL ia_window_child_set_comment(Window *window, size_t index, const char *comment);
// 获取子控件类型
PLUG_API size_t PLUG_CALL ia_window_child_get_datatype_id(Window *window, size_t index);
// 设置子控件类型
PLUG_API void PLUG_CALL ia_window_child_set_datatype_id(Window *window, size_t index, size_t datatype_id);

// 20240627 新增
// 常量部分

// 添加文本常量 位置(-1或者大于当前最大索引则为添加到末尾) 名称 常量值 备注 是否公开 是否长字符串 id 保存id(可以为0) 返回值 为常量资源索引
PLUG_API size_t PLUG_CALL ia_const_resources_add_string(size_t index, const char *name, const char *value, const char *comment,
                                                        BOOL is_public, BOOL is_long, size_t *id);
// 添加数值常量 位置(-1或者大于当前最大索引则为添加到末尾) 名称 常量值 备注 是否公开 id 保存id(可以为0) 返回值 为常量资源索引
PLUG_API size_t PLUG_CALL ia_const_resources_add_number(size_t index, const char *name, double value, const char *comment,
                                                        BOOL is_public, size_t *id);
// 添加时间常量 位置(-1或者大于当前最大索引则为添加到末尾) 名称 常量值 备注 是否公开 id 保存id(可以为0) 返回值 为常量资源索引
// 时间格式 [2024年1月29日16时1分27秒]/ 2024年1月29日16时1分27秒
PLUG_API size_t PLUG_CALL ia_const_resources_add_time(size_t index, const char *name, const char *value, const char *comment,
                                                      BOOL is_public, size_t *id);
PLUG_API size_t PLUG_CALL ia_const_resources_add_time_double(size_t index, const char *name, double value, const char *comment,
                                                             BOOL is_public, size_t *id);
// 添加布尔常量 位置(-1或者大于当前最大索引则为添加到末尾) 名称 常量值 备注 是否公开 id 保存id(可以为0) 返回值 为常量资源索引
PLUG_API size_t PLUG_CALL ia_const_resources_add_bool(size_t index, const char *name, BOOL value, const char *comment,
                                                      BOOL is_public, size_t *id);
// 添加图片常量 名称 图片数据 图片大小 备注 是否公开 id 保存id(可以为0) 返回值 为常量资源索引
PLUG_API size_t PLUG_CALL ia_const_resources_add_image(const char *name, void *data, size_t size, const char *comment,
                                                       BOOL is_public, size_t *id);
// 添加声音常量 名称 声音数据 声音大小 备注 是否公开 id 保存id(可以为0) 返回值 为常量资源索引
PLUG_API size_t PLUG_CALL ia_const_resources_add_sound(const char *name, void *data, size_t size, const char *comment,
                                                       BOOL is_public, size_t *id);

// 删除常量资源(从index_start到index_end 删除单个 index_start=index_end)
PLUG_API void PLUG_CALL ia_const_resources_remove(size_t index_start, size_t index_end);

// 修改常量资源文本 index 常量资源索引 value 新值
PLUG_API void PLUG_CALL ia_const_resources_set_string(size_t index, const char *value);
// 修改常量资源数值 index 常量资源索引 value 新值
PLUG_API void PLUG_CALL ia_const_resources_set_number(size_t index, double value);
// 修改常量资源时间 index 常量资源索引 value 新值
PLUG_API void PLUG_CALL ia_const_resources_set_time(size_t index, const char *value);
PLUG_API void PLUG_CALL ia_const_resources_set_time_double(size_t index, double value);
// 修改常量资源布尔 index 常量资源索引 value 新值
PLUG_API void PLUG_CALL ia_const_resources_set_bool(size_t index, BOOL value);




//#define IA_GET_PTR_ITERATOR(_name) \
//    static __FN_##_name__ _PFN_CALLBACK_ = 0;\
//    if(!_PFN_CALLBACK_) _PFN_CALLBACK_ = (__FN_##_name__)_ia_temp::_ia_get_address(#_name);\
//    if (!_PFN_CALLBACK_) {\
//        MessageBoxA(((HWND)0), "未能找到\""## #_name ##"\"接口, 程序无法正常启动", #_name ## "未找到", 0);\
//        exit(0);\
//    }

//// 通过名称获取程序集 id 保存ID
//inline Assembly* __stdcall ia_get_assembly(const char* name, size_t* id){
//    typedef decltype(ia_get_assembly)* PFN_ia_get_assembly;
//
//    static PFN_ia_get_assembly _pfn_call = 0;
//    if (!_pfn_call)
//        _pfn_call = (PFN_ia_get_assembly)_ia_temp::_ia_get_address("ia_get_assembly");
//    if (!_pfn_call) {
//        MessageBoxA(((HWND)0), "未能找到\"""ia_get_assembly""\"接口, 程序无法正常启动", "ia_get_assembly""未找到", 0); exit(0); 
//    } return _pfn_call(name, id);
//}







#endif