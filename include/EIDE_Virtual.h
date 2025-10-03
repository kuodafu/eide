#pragma once
#include <windows.h>


// ��������, �Ӵ��������ķ����л�ȡ�Ĵ�������
// CODELINETYPE = ����������
// CODETYPE = ��������
// CODEOFFSET = ��������ƫ������
enum class ECODETYPE : int
{
    CODELINETYPE_STRUCT                     = 5,    // ��������
    CODELINETYPE_STRUCTMEMBER               = 6,    // �������ͳ�Ա
    CODELINETYPE_GLOBALVAR                  = 7,    // ȫ�ֱ���
    CODELINETYPE_ASSEMBLY                   = 8,    // ����
    CODELINETYPE_ASSEMBLYVAR                = 9,    // ���򼯱���
    CODELINETYPE_FUN                        = 10,   // �ӳ���
    CODELINETYPE_FUNARG                     = 11,   // �ӳ������
    CODELINETYPE_FUNVAR                     = 12,   // �ӳ������
    CODELINETYPE_FUNCODE                    = 13,   // �ӳ������
    CODELINETYPE_CONST                      = 14,   // ����
    CODELINETYPE_IMAGE                      = 15,   // ��ԴͼƬ
    CODELINETYPE_SOUND                      = 16,   // ��Դ����

    CODETYPE_UNDEFINE                       = 0,    // δ����
    CODETYPE_ASSEMBLY                       = 1,    // ����
    CODETYPE_STRUCT                         = 2,    // ��������
    CODETYPE_GLOBALVAR                      = 3,    // ȫ�ֱ���
    CODETYPE_DLL                            = 4,    // DLL������
    CODETYPE_WINDOW                         = 5,    // ���������
    CODETYPE_CONST                          = 6,    // ����
    CODETYPE_IMAGE                          = 7,    // ͼƬ��Դ
    CODETYPE_SOUND                          = 8,    // ������Դ

    CODEOFFSET_STRUCT_NAME                  = 261,  // ������������
    CODEOFFSET_STRUCT_PUBLIC                = 263,  // �������͹���
    CODEOFFSET_STRUCT_REMARKS               = 262,  // �������ͱ�ע
    CODEOFFSET_STRUCT_MEMBER_NAME           = 313,  // �������� ��Ա ����
    CODEOFFSET_STRUCT_MEMBER_TYPE           = 314,  // �������� ��Ա ����
    CODEOFFSET_STRUCT_MEMBER_REFERENCE      = 317,  // �������� ��Ա ��ַ
    CODEOFFSET_STRUCT_MEMBER_ARRAY          = 315,  // �������� ��Ա ����
    CODEOFFSET_STRUCT_MEMBER_REMARKS        = 316,  // �������� ��Ա ��ע

    CODEOFFSET_GLOBALVAR_NAME               = 367,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_TYPE               = 368,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_ARRAY              = 369,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_PUBLIC             = 371,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_REMARKS            = 370,  // ȫ�ֱ��� ��ע
    

    CODEOFFSET_ASSEMBLY_NAME                = 580,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLY_BASECLASS           = 582,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLY_PUBLIC              = 573,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLY_REMARKS             = 581,  // ���򼯱��� ��ע

    CODEOFFSET_ASSEMBLYVAR_NAME             = 632,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLYVAR_TYPE             = 633,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLYVAR_ARRAY            = 634,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLYVAR_REMARKS          = 635,  // ���򼯱��� ��ע

    CODEOFFSET_FUN_NAME                     = 686,  // �ӳ�������
    CODEOFFSET_FUN_RETURNTYPE               = 687,  // �ӳ��� ��������
    CODEOFFSET_FUN_PUBLIC                   = 690,  // �ӳ��� ����
    CODEOFFSET_FUN_EPACKAGE                 = 688,  // �ӳ��� �װ�
    CODEOFFSET_FUN_REMARKS                  = 689,  // �ӳ��� ��ע

    CODEOFFSET_FUNARG_NAME                  = 740,  // �ӳ������ ����
    CODEOFFSET_FUNARG_TYPE                  = 741,  // �ӳ������ ����
    CODEOFFSET_FUNARG_REFERENCE             = 742,  // �ӳ������ �ο�
    CODEOFFSET_FUNARG_OPTIONAL              = 743,  // �ӳ������ �ɿ�
    CODEOFFSET_FUNARG_ARRAY                 = 744,  // �ӳ������ ����
    CODEOFFSET_FUNARG_REMARKS               = 745,  // �ӳ������ ��ע

    CODEOFFSET_FUNVAR_NAME                  = 796,  // �ӳ������ ����
    CODEOFFSET_FUNVAR_TYPE                  = 797,  // �ӳ������ ����
    CODEOFFSET_FUNVAR_STATIC                = 798,  // �ӳ������ ��̬
    CODEOFFSET_FUNVAR_ARRAY                 = 799,  // �ӳ������ ����
    CODEOFFSET_FUNVAR_REMARKS               = 800,  // �ӳ������ ��ע


    CODEOFFSET_CODE                         = 851,  // �ӳ������
};


// �ӿ�_IDE����༭�����
class IEIDE_CodeManage
{
public:
    // �Ӵ��ھ���г�ʼ������
    virtual bool init(HWND hWnd) = 0;
    // ����
    virtual void destroy() = 0;
    // ��ȡ���λ��, λ��Ϊ����ڲ����
    virtual POINT GetCursorPos() = 0;
    // ���ù��λ��, ��ѱ��к���Ϊ0
    virtual void SetCursorPos(POINT pt) = 0;
    // ��ȡѡ����Ϣ
    // pStartCol = ������ʼ��
    // pEndCol = ���ս�����
    // pNowLine = ���յ�ǰ��
    // pNowLine = ���յ�ǰ��
    // pSel = ����ѡ�з�Χ
    virtual void GetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // ����ѡ����Ϣ
    // pStartCol = ��ʼ��ָ��, Ϊ0��д��
    // pEndCol = ������ָ��, Ϊ0��д��
    // pNowLine = ��ǰ��ָ��, Ϊ0��д��
    // pNowLine = ��ǰ��ָ��, Ϊ0��д��
    // pSel = ѡ�з�Χָ��, Ϊ0��д��
    virtual void SetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // �Ƿ�ѡ�д���
    virtual bool IsSelCode() = 0;

    // ��ȡѡ������
    virtual int GetSelLineCount() = 0;

    // ��ȡѡ������
    virtual int GetSelColumnCount() = 0;

    // ��ȡ����Ϣ
    // lineNumber = �к�
    // flags = ���ʶ, ETAG
    // pData = ������ָ��, EBase*
    virtual int GetLineInfo(int lineNumber, int* flags, int* pData) = 0;

    // ��ȡ��������
    virtual int GetCodeType() = 0;

    // ��ȡ����ָ��
    virtual void* GetObjectThis() = 0;

    // ���ö���ָ��
    virtual void SetObjectThis(void* pThis) = 0;

    // ��궨λ
    virtual void CursorLocation(int lineNumber) = 0;

    // ��֤��ʾ���λ��
    virtual void EnsurVisibleCursor() = 0;

    // ��ȡ�����ı�, ���ص��ı���Ҫ���� �������� free() �������ͷ�
    // nTableIndex = ����
    // nTableindexCol = ����
    virtual LPSTR GetCodeText(int nTableIndex, int nTableindexCol) = 0;

    // �����ڴ�
    virtual void* alloc(int size) = 0;
    // �ͷ��ڴ�, ֻ�ܴ��ݱ������ alloc() �������ص�ָ��, ��������ָ����ܻ������
    virtual void free(void* p) = 0;

    // ��ȡ��������ƫ��
    // nTableIndex = ����
    // nTableindexCol = ����
    // type = ��������ָ��
    // offset = ����ƫ��ָ��
    // pData = ����ָ��, EBase*
    virtual bool GetCodeDataOffset(int nTableIndex, int nTableindexCol, int* pType, int* pOffset, int* pData) = 0;

    // ��ȡ����λ��, λ��Ϊ������
    virtual POINT GetCursorTablePos() = 0;

    // ���ù���λ��, λ��Ϊ������
    // pt = ������
    virtual void SetCursorTablePos(POINT pt) = 0;

    // ��ȡ��ǰ����ָ��Ĵ������
    virtual HWND GetWindow() = 0;
};
