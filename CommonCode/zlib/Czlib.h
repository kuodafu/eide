#pragma once
#include "zlib.h"
class Czlib
{
private:
    int err;
public:
    Czlib()
    {
        err = 0;
    }
    ~Czlib()
    {
        
    }
    // zlibѹ��, �ɹ�����ѹ���������,ʧ�ܷ���0, ���صĵ�ַ�����Զ��ͷ�,��Ҫ���� ������� free(); ���ͷ�
    // data = ��Ҫѹ��������
    // size = ��Ҫѹ�����ݵĴ�С
    // compressSize = ����ѹ����Ĵ�С
    Bytef* compress(const void* data, uLongf size, uLongf* compressSize = 0)
    {
        uLongf bufSize = 0;
        compressEx(data, size, 0, 0, &bufSize);

        if (bufSize == 0) return 0;
        Bytef* ptr = (Bytef*)malloc(bufSize);
        if (!ptr)
        {
            err = 7;// QM_ERROR_LOCALALLOC;
            return 0;
        }

        return compressEx(data, size, ptr, bufSize, compressSize);
    }

    // zlibѹ��, �ɹ�����ѹ���������,ʧ�ܷ���0, ���صĵ�ַ�����Զ��ͷ�,��Ҫ���� ������� free(); ���ͷ�
    // data = ��Ҫѹ��������
    // size = ��Ҫѹ�����ݵĴ�С
    // pBuffer = ѹ��������ݱ���Ļ�����, Ϊ0�򷵻ػ������ߴ�
    // nBufferSize = �������ߴ�
    // compressSize = ����ѹ����Ĵ�С
    Bytef* compressEx(const void* data, uLongf size, LPVOID pBuffer, uLongf nBufferSize, uLongf* compressSize = 0)
    {
        const int offset = 10;
        if (compressSize) *compressSize = 0;
        if (!data)return 0;
        uLongf sourceLen = size;
        uLongf destLen = (uLongf)(((double)(sourceLen + 12)) * 1.1);    // ����������, Ҫ��ԭʼ���ݳ�һ��
        Bytef* ptr = (Bytef*)pBuffer;
        if (!ptr || destLen + offset > nBufferSize) // ���ݵ�ָ��Ϊ0, ���߻�����С����Ҫ�Ļ������ߴ�, ���ػ�������Ҫ�ĳߴ�
        {
            if (compressSize)*compressSize = destLen + offset;
            return 0;
        }
        
        memset(ptr, 0, destLen + offset);

        err = ::compress(ptr + offset, &destLen, (Bytef*)data, sourceLen);
        if (err != Z_OK) return 0;

        *((short*)ptr) = 31082;                 // ǰ�����ֽ��Ǳ�־ Сд��jy
        *((int*)(ptr + 2)) = sourceLen;         // ����4���ֽ���ԭʼ����
        *((int*)(ptr + 6)) = destLen;           // ������4���ֽ���ѹ����ĳ���
        if (compressSize) *compressSize = destLen + offset;
        return ptr;
    }

    // zlib��ѹ, �ɹ����ؽ�ѹ�������,ʧ�ܷ���0, ���صĵ�ַ�����Զ��ͷ�,��Ҫ���� ������� free(); ���ͷ�
    // data = ��Ҫ��ѹ������, ������� compress ���ص�ֵ
    // uncompressSize = ���ؽ�ѹ��Ĵ�С
    Bytef* uncompress(const void* data, uLongf* uncompressSize = 0)
    {
        uLongf bufSize = 0;
        uncompressEx(data, 0, 0, &bufSize);

        if (bufSize == 0) return 0;
        Bytef* ptr = (Bytef*)malloc(bufSize);
        if (!ptr)
        {
            err = 7;// QM_ERROR_LOCALALLOC;
            return 0;
        }

        return uncompressEx(data, ptr, bufSize, uncompressSize);
    }

    // zlib��ѹ, �ɹ����ؽ�ѹ�������,ʧ�ܷ���0, ���صĵ�ַ�����Զ��ͷ�,��Ҫ���� ������� free(); ���ͷ�
    // data = ��Ҫ��ѹ������, ������� compress ���ص�ֵ
    // pBuffer = ��ѹ������ݱ���Ļ�����, Ϊ0�򷵻ػ������ߴ�
    // nBufferSize = �������ߴ�
    // uncompressSize = ���ؽ�ѹ��Ĵ�С
    Bytef* uncompressEx(const void* data, LPVOID pBuffer, uLongf nBufferSize, uLongf* uncompressSize = 0)
    {
        const int offset = 10;
        if (uncompressSize)*uncompressSize = 0;
        if (!data)return 0;
        const short flag        = *((short*)data);                  // ǰ�����ֽ��Ǳ�־ Сд��jy
        uLongf destLen          = *((int*)((char*)data + 2));       // ����4���ֽ���ԭʼ����
        const uLongf dwLen      = *((int*)((char*)data + 6));       // ������4���ֽ���ѹ���󳤶�
        if (destLen <= 0 || flag != 31082 || dwLen <= 0) return 0;  // ֻҪ��һ�������ϾͲ���ѹ

        //destLen
        Bytef* ptr = (Bytef*)pBuffer;
        if (!ptr || nBufferSize < destLen)     // ������������, ���ػ�������Ҫ�ĳߴ�
        {
            if (uncompressSize)*uncompressSize = destLen;
            return 0;
        }

        memset(ptr, 0, destLen);
        err = ::uncompress(ptr, &destLen, (Bytef*)((char*)data + offset), dwLen);    // ǰ10���ֽڼ�¼��־�ͳ���,ʵ��ѹ�����ݴӵ�10���ֽڿ�ʼ
        if (err != Z_OK) return 0;
        if (uncompressSize)*uncompressSize = destLen;
        return ptr;
    }
    int GetError()const { return err; }

    // �ͷ��ɱ��෵�صĵ�ַ, �ͷųɹ� ����p ����Ϊ0
    static void free(Bytef** p)
    {
        if (!p)return;
        ::free(*p);
        *p = 0;
        return;
    }
};

