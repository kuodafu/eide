#pragma once

namespace mystruct
{
    template<typename T = LPVOID>
    //typedef int T;
    class simpleMempool
    {
    private:
        struct _list
        {
            int flag;
            struct _list* next;
        };
        _list* m_next_ptr;        // ��һ���ɷ����ַ, ����ͷ
        LPBYTE m_begin;            // �ڴ���׵�ַ
        LPBYTE m_end;            // �ڴ�ؽ�����ַ
        LPCRITICAL_SECTION xkq;    // �߳������
        bool m_isVirtualFree;    // �Ƿ���� VirtualFree ���ͷ���

    public:
        // ��ʼ���ڴ��, �ڲ�������ռ�
        bool init(size_t size = 0x8000)
        {
            if (m_next_ptr)return false;
            if (size % 0x1000) size = (size / 0x1000) + 0x1000;        // 4096����
            if (size < 0x8000) size = 0x8000;
            _list* p = (_list*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!p) return false;
            m_isVirtualFree = true;
            return init(p, size);
        }
        // ��ָ���ڵ����ʼ���ڴ��
        bool init(LPVOID pData, size_t size)
        {
            if (m_next_ptr || !pData || !size) return false;
            xkq = new CRITICAL_SECTION;
            InitializeCriticalSection(xkq);

            m_next_ptr = (_list*)pData;

            m_begin = (LPBYTE)m_next_ptr;    // �׵�ַ, �׽ڵ�
            m_end = m_begin + size;            // ��β��ַ
            LPBYTE p = (LPBYTE)m_begin;

            int blockSize = sizeof(T) + sizeof(LPVOID);
            if (blockSize < sizeof(_list))
                blockSize = sizeof(_list);

            const size_t len = size / blockSize - 1;
            for (size_t i = 0; i < len; i++)
            {
                // ��һ�����������¼�����ڴ��ַ
                // ÿ����ַ��¼�Ĵ�СΪ T �ṹ��С
                p += blockSize;
                m_next_ptr->next = (_list*)(p);
                m_next_ptr->flag = 0;
                m_next_ptr = (_list*)p;    // ָ����һ���ڵ�
            }
            m_next_ptr->next = 0;
            m_next_ptr = (_list*)m_begin;
            return true;
        }
        // ����ʼ��, �ͷ��ڴ��
        void uninit()
        {
            if (m_next_ptr)
            {
                if (m_isVirtualFree)
                    VirtualFree(m_begin, 0, MEM_RELEASE);
                m_next_ptr = 0;
                m_begin = 0;
                DeleteCriticalSection(xkq);
                delete xkq;
                xkq = 0;
            }
        }
        simpleMempool() :m_next_ptr(0), m_begin(0), m_end(0), xkq(0), m_isVirtualFree(false) {  }
        ~simpleMempool() { uninit(); }

        // ����ʧ�����׳�int�����쳣
        // �쳣ֵ 1=��ʼ��ʧ��, 2=�ռ䲻��,��Ҫ�ͷ�һЩ�ڴ�, ���׳��쳣��ʾ�����ص��ڴ�й©.....
        T* alloc(bool isClear = false)
        {
            EnterCriticalSection(xkq);
            if (!m_next_ptr)
                init();
            if (!m_next_ptr)
            {
                LeaveCriticalSection(xkq);
                throw int(1);
            }

            m_next_ptr->flag = 1;
            void* p = &m_next_ptr->next;    // ÿ�����붼�ӵ�ǰ�ڵ㿪ʼȡ, �ͷ���ԭ���׽ڵ�

            if (m_next_ptr->next == 0)
            {
                LeaveCriticalSection(xkq);
                throw int(2);                // û���ڴ���, �׳��쳣
            }

            m_next_ptr = m_next_ptr->next;    // ��ǰ�ڵ�ָ����һ���ڴ�
            if (isClear)
                memset(p, 0, sizeof(T));
            LeaveCriticalSection(xkq);
            return static_cast<T*>(p);
        }
        bool free(T*& p)
        {
            _list* pFree = ((_list*)(((LPBYTE)p) - 4));
            if (!m_begin || (LPBYTE)pFree < m_begin || (LPBYTE)pFree > m_end)
            {
                return false;    // ��ַС���׵�ַ, ��������ַ, ����Ĳ����ڴ����ĵ�ַ
            }

            EnterCriticalSection(xkq);
            pFree->flag = 0;

            // �ͷžͼ򵥵ļ���������
            memset(pFree, 0, sizeof(T));

            pFree->next = m_next_ptr;        // �ŵ�����ͷ��
            m_next_ptr = (_list*)pFree;
            p = 0;
            LeaveCriticalSection(xkq);
            return true;
        }
        // ��ѯ��ַ�Ƿ�ռ��, ��ռ�÷���true
        bool query(T* p)
        {
            _list* node = ((_list*)(((LPBYTE)p) - 4));
            if (!m_begin || (LPBYTE)node < m_begin || (LPBYTE)node > m_end)
            {
                return false;    // ��ַС���׵�ַ, ��������ַ, ����Ĳ����ڴ����ĵ�ַ
            }

            return node->flag != 0;

            //do
            //{
            //    if ((T*)&node->next == p)
            //        return false;    // �����ڴ��������, û�б�ռ��
            //    node = node->next;    // ָ����һ��δ��ռ�õĵ�ַ
            //} while (node && (LPBYTE)node < m_end);

            //return true;            // �����ڴ���������ʾ�Ѿ���ռ��
        }
    };
}

