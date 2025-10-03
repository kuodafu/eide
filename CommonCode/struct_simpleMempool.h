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
        _list* m_next_ptr;        // 下一个可分配地址, 链表头
        LPBYTE m_begin;            // 内存池首地址
        LPBYTE m_end;            // 内存池结束地址
        LPCRITICAL_SECTION xkq;    // 线程许可区
        bool m_isVirtualFree;    // 是否调用 VirtualFree 来释放内

    public:
        // 初始化内存池, 内部会申请空间
        bool init(size_t size = 0x8000)
        {
            if (m_next_ptr)return false;
            if (size % 0x1000) size = (size / 0x1000) + 0x1000;        // 4096对齐
            if (size < 0x8000) size = 0x8000;
            _list* p = (_list*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!p) return false;
            m_isVirtualFree = true;
            return init(p, size);
        }
        // 从指定内地里初始化内存池
        bool init(LPVOID pData, size_t size)
        {
            if (m_next_ptr || !pData || !size) return false;
            xkq = new CRITICAL_SECTION;
            InitializeCriticalSection(xkq);

            m_next_ptr = (_list*)pData;

            m_begin = (LPBYTE)m_next_ptr;    // 首地址, 首节点
            m_end = m_begin + size;            // 结尾地址
            LPBYTE p = (LPBYTE)m_begin;

            int blockSize = sizeof(T) + sizeof(LPVOID);
            if (blockSize < sizeof(_list))
                blockSize = sizeof(_list);

            const size_t len = size / blockSize - 1;
            for (size_t i = 0; i < len; i++)
            {
                // 用一个单向链表记录可用内存地址
                // 每个地址记录的大小为 T 结构大小
                p += blockSize;
                m_next_ptr->next = (_list*)(p);
                m_next_ptr->flag = 0;
                m_next_ptr = (_list*)p;    // 指向下一个节点
            }
            m_next_ptr->next = 0;
            m_next_ptr = (_list*)m_begin;
            return true;
        }
        // 反初始化, 释放内存池
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

        // 申请失败则抛出int类型异常
        // 异常值 1=初始化失败, 2=空间不足,需要释放一些内存, 有抛出异常表示有严重的内存泄漏.....
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
            void* p = &m_next_ptr->next;    // 每次申请都从当前节点开始取, 释放则还原到首节点

            if (m_next_ptr->next == 0)
            {
                LeaveCriticalSection(xkq);
                throw int(2);                // 没有内存了, 抛出异常
            }

            m_next_ptr = m_next_ptr->next;    // 当前节点指向下一块内存
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
                return false;    // 地址小于首地址, 大于最后地址, 传入的不是内存池里的地址
            }

            EnterCriticalSection(xkq);
            pFree->flag = 0;

            // 释放就简单的加入链表中
            memset(pFree, 0, sizeof(T));

            pFree->next = m_next_ptr;        // 放到链表头部
            m_next_ptr = (_list*)pFree;
            p = 0;
            LeaveCriticalSection(xkq);
            return true;
        }
        // 查询地址是否被占用, 被占用返回true
        bool query(T* p)
        {
            _list* node = ((_list*)(((LPBYTE)p) - 4));
            if (!m_begin || (LPBYTE)node < m_begin || (LPBYTE)node > m_end)
            {
                return false;    // 地址小于首地址, 大于最后地址, 传入的不是内存池里的地址
            }

            return node->flag != 0;

            //do
            //{
            //    if ((T*)&node->next == p)
            //        return false;    // 还在内存池链表里, 没有被占用
            //    node = node->next;    // 指向下一个未被占用的地址
            //} while (node && (LPBYTE)node < m_end);

            //return true;            // 不在内存池链表里表示已经被占用
        }
    };
}

