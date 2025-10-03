#pragma once
#include <windows.h>


// 代码类型, 从代码管理类的方法中获取的代码类型
// CODELINETYPE = 代码行类型
// CODETYPE = 代码类型
// CODEOFFSET = 代码数据偏移类型
enum class ECODETYPE : int
{
    CODELINETYPE_STRUCT                     = 5,    // 数据类型
    CODELINETYPE_STRUCTMEMBER               = 6,    // 数据类型成员
    CODELINETYPE_GLOBALVAR                  = 7,    // 全局变量
    CODELINETYPE_ASSEMBLY                   = 8,    // 程序集
    CODELINETYPE_ASSEMBLYVAR                = 9,    // 程序集变量
    CODELINETYPE_FUN                        = 10,   // 子程序
    CODELINETYPE_FUNARG                     = 11,   // 子程序参数
    CODELINETYPE_FUNVAR                     = 12,   // 子程序变量
    CODELINETYPE_FUNCODE                    = 13,   // 子程序代码
    CODELINETYPE_CONST                      = 14,   // 常量
    CODELINETYPE_IMAGE                      = 15,   // 资源图片
    CODELINETYPE_SOUND                      = 16,   // 资源声音

    CODETYPE_UNDEFINE                       = 0,    // 未定义
    CODETYPE_ASSEMBLY                       = 1,    // 程序集
    CODETYPE_STRUCT                         = 2,    // 数据类型
    CODETYPE_GLOBALVAR                      = 3,    // 全局变量
    CODETYPE_DLL                            = 4,    // DLL命令定义表
    CODETYPE_WINDOW                         = 5,    // 窗口设计器
    CODETYPE_CONST                          = 6,    // 常量
    CODETYPE_IMAGE                          = 7,    // 图片资源
    CODETYPE_SOUND                          = 8,    // 声音资源

    CODEOFFSET_STRUCT_NAME                  = 261,  // 数据类型名称
    CODEOFFSET_STRUCT_PUBLIC                = 263,  // 数据类型公开
    CODEOFFSET_STRUCT_REMARKS               = 262,  // 数据类型备注
    CODEOFFSET_STRUCT_MEMBER_NAME           = 313,  // 数据类型 成员 名称
    CODEOFFSET_STRUCT_MEMBER_TYPE           = 314,  // 数据类型 成员 类型
    CODEOFFSET_STRUCT_MEMBER_REFERENCE      = 317,  // 数据类型 成员 传址
    CODEOFFSET_STRUCT_MEMBER_ARRAY          = 315,  // 数据类型 成员 数组
    CODEOFFSET_STRUCT_MEMBER_REMARKS        = 316,  // 数据类型 成员 备注

    CODEOFFSET_GLOBALVAR_NAME               = 367,  // 全局变量 名称
    CODEOFFSET_GLOBALVAR_TYPE               = 368,  // 全局变量 类型
    CODEOFFSET_GLOBALVAR_ARRAY              = 369,  // 全局变量 数组
    CODEOFFSET_GLOBALVAR_PUBLIC             = 371,  // 全局变量 公开
    CODEOFFSET_GLOBALVAR_REMARKS            = 370,  // 全局变量 备注
    

    CODEOFFSET_ASSEMBLY_NAME                = 580,  // 程序集变量 名称
    CODEOFFSET_ASSEMBLY_BASECLASS           = 582,  // 程序集变量 基类
    CODEOFFSET_ASSEMBLY_PUBLIC              = 573,  // 程序集变量 公开
    CODEOFFSET_ASSEMBLY_REMARKS             = 581,  // 程序集变量 备注

    CODEOFFSET_ASSEMBLYVAR_NAME             = 632,  // 程序集变量 名称
    CODEOFFSET_ASSEMBLYVAR_TYPE             = 633,  // 程序集变量 类型
    CODEOFFSET_ASSEMBLYVAR_ARRAY            = 634,  // 程序集变量 数组
    CODEOFFSET_ASSEMBLYVAR_REMARKS          = 635,  // 程序集变量 备注

    CODEOFFSET_FUN_NAME                     = 686,  // 子程序名称
    CODEOFFSET_FUN_RETURNTYPE               = 687,  // 子程序 返回类型
    CODEOFFSET_FUN_PUBLIC                   = 690,  // 子程序 公开
    CODEOFFSET_FUN_EPACKAGE                 = 688,  // 子程序 易包
    CODEOFFSET_FUN_REMARKS                  = 689,  // 子程序 备注

    CODEOFFSET_FUNARG_NAME                  = 740,  // 子程序参数 名称
    CODEOFFSET_FUNARG_TYPE                  = 741,  // 子程序参数 类型
    CODEOFFSET_FUNARG_REFERENCE             = 742,  // 子程序参数 参考
    CODEOFFSET_FUNARG_OPTIONAL              = 743,  // 子程序参数 可空
    CODEOFFSET_FUNARG_ARRAY                 = 744,  // 子程序参数 数组
    CODEOFFSET_FUNARG_REMARKS               = 745,  // 子程序参数 备注

    CODEOFFSET_FUNVAR_NAME                  = 796,  // 子程序变量 名称
    CODEOFFSET_FUNVAR_TYPE                  = 797,  // 子程序变量 类型
    CODEOFFSET_FUNVAR_STATIC                = 798,  // 子程序变量 静态
    CODEOFFSET_FUNVAR_ARRAY                 = 799,  // 子程序变量 数组
    CODEOFFSET_FUNVAR_REMARKS               = 800,  // 子程序变量 备注


    CODEOFFSET_CODE                         = 851,  // 子程序代码
};


// 接口_IDE代码编辑框管理
class IEIDE_CodeManage
{
public:
    // 从窗口句柄中初始化本类
    virtual bool init(HWND hWnd) = 0;
    // 销毁
    virtual void destroy() = 0;
    // 获取光标位置, 位置为表格内部序号
    virtual POINT GetCursorPos() = 0;
    // 设置光标位置, 会把表列号置为0
    virtual void SetCursorPos(POINT pt) = 0;
    // 获取选中信息
    // pStartCol = 接收起始列
    // pEndCol = 接收结束列
    // pNowLine = 接收当前行
    // pNowLine = 接收当前行
    // pSel = 接收选中范围
    virtual void GetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // 设置选中信息
    // pStartCol = 起始列指针, 为0则不写入
    // pEndCol = 结束列指针, 为0则不写入
    // pNowLine = 当前行指针, 为0则不写入
    // pNowLine = 当前行指针, 为0则不写入
    // pSel = 选中范围指针, 为0则不写入
    virtual void SetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // 是否选中代码
    virtual bool IsSelCode() = 0;

    // 获取选中行数
    virtual int GetSelLineCount() = 0;

    // 获取选中列数
    virtual int GetSelColumnCount() = 0;

    // 获取行信息
    // lineNumber = 行号
    // flags = 项标识, ETAG
    // pData = 项数据指针, EBase*
    virtual int GetLineInfo(int lineNumber, int* flags, int* pData) = 0;

    // 获取代码类型
    virtual int GetCodeType() = 0;

    // 获取对象指针
    virtual void* GetObjectThis() = 0;

    // 设置对象指针
    virtual void SetObjectThis(void* pThis) = 0;

    // 光标定位
    virtual void CursorLocation(int lineNumber) = 0;

    // 保证显示光标位置
    virtual void EnsurVisibleCursor() = 0;

    // 获取代码文本, 返回的文本需要调用 本类对象的 free() 方法来释放
    // nTableIndex = 表行
    // nTableindexCol = 表列
    virtual LPSTR GetCodeText(int nTableIndex, int nTableindexCol) = 0;

    // 申请内存
    virtual void* alloc(int size) = 0;
    // 释放内存, 只能传递本类对象 alloc() 方法返回的指针, 传递其他指针可能会出问题
    virtual void free(void* p) = 0;

    // 获取代码数据偏移
    // nTableIndex = 表行
    // nTableindexCol = 表列
    // type = 接收类型指针
    // offset = 接收偏移指针
    // pData = 数据指针, EBase*
    virtual bool GetCodeDataOffset(int nTableIndex, int nTableindexCol, int* pType, int* pOffset, int* pData) = 0;

    // 获取光标表位置, 位置为表格序号
    virtual POINT GetCursorTablePos() = 0;

    // 设置光标表位置, 位置为表格序号
    // pt = 表格序号
    virtual void SetCursorTablePos(POINT pt) = 0;

    // 获取当前对象指向的代码框句柄
    virtual HWND GetWindow() = 0;
};
