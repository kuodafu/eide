#pragma once

// 命名空间内的不建议外部使用
// 使用前需要 #include<windows.h>
namespace __subclass
{
	typedef struct tagMAKEPROCDATA
	{
		BYTE data[80];		// 为了通用, 指令都写到这个数组里, 也不需要1字节对齐
	}MAKEPROCDATA, * LPMAKEPROCDATA;
}
typedef struct tagSUBCLASSSTRUCT : private __subclass::tagMAKEPROCDATA	
{
	// 私有继承
	HWND hWnd;				// 子类化的窗口句柄
	WNDPROC oldProc;		// 旧窗口过程
	void* param;			// 用户数据
}SUBCLASSSTRUCT, * LPSUBCLASSSTRUCT;

namespace __subclass
{
	// 由于VirtualAlloc() 申请的字节数是一页, 一般一页是4096个字节
	// 每次申请那么多, 只用几十个字节, 剩下的4000个字节都浪费了
	// 所以做个简单的内存池
	class simpleMempool
	{
	private:
		struct _list
		{
			struct _list* next;
		};
		_list* ptr;
		void* root;
		bool init()
		{
			if (ptr)return false;
			// 8k 个字节足够了, 最多支持 8192/sizeof(SUBCLASSSTRUCT) 次子类化操作, 在不释放的前提下
			size_t size = 0x2000;	// 4k对齐
#ifdef _M_X64
			// x64需要自己指定申请的地址, 如果让系统自动分配, 有可能函数地址减去申请的地址会大于4个字节
			INT_PTR base = 0x100000000;
			for (INT_PTR i = 0; i < 50; i++)
			{
				ptr = (_list*)VirtualAlloc((LPVOID)base, size,
					MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (ptr)break;
				base += 0x100000000;
			}
#else
			// x86没那么多讲究, 让系统自己分配地址
			ptr = (_list*)VirtualAlloc(0, size,
				MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
			if (!ptr)return false;

			root = ptr;	// 首地址, 首节点
			LPBYTE p = (LPBYTE)ptr;
			size_t len = size / sizeof(SUBCLASSSTRUCT) - 1;
			for (size_t i = 0; i < len; i++)
			{
				// 用一个单向链表记录可用内存地址
				// 每个地址记录的大小为 SUBCLASSSTRUCT 结构大小
				p += sizeof(SUBCLASSSTRUCT);
				ptr->next = (_list*)p;
				ptr = (_list*)p;	// 指向下一个节点
			}
			ptr->next = 0;
			ptr = (_list*)root;
			return true;
		}
	public:
		simpleMempool() :ptr(0), root(0) { ; }
		~simpleMempool() { VirtualFree(root, 0, MEM_RELEASE); ptr = 0; root = 0; }
		
		// 申请失败则抛出int类型异常
		// 异常值 1=初始化失败, 2=空间不足,需要释放一些内存
		LPMAKEPROCDATA alloc()
		{
			size_t size = sizeof(SUBCLASSSTRUCT);
			if (!ptr)init();
			if (!ptr)throw int(1);
			void* p = ptr;		// 每次申请都从当前节点开始取, 释放则还原到首节点
			if (ptr->next == 0)throw int(2);	// 没有内存了, 抛出异常

			ptr = ptr->next;	// 当前节点指向下一块内存
			return (LPMAKEPROCDATA)p;
		}
		bool free(LPMAKEPROCDATA& p)
		{
			if (!ptr || !p)return false;
			// 释放就简单的加入链表中
			memset(p, 0, sizeof(SUBCLASSSTRUCT));
			((_list*)p)->next = ptr;	// 放到链表头部
			ptr = (_list*)p;
			p = 0;
			return true;
		}
	};
	static simpleMempool mempool;
	inline LPMAKEPROCDATA subclassAlloc()
	{
		LPMAKEPROCDATA pData = 0;
		try
		{
			pData = mempool.alloc();
		}
		catch (int e)
		{
			switch (e)
			{
			case 1:
				MessageBoxW(0, L"调用SubClassWindow() 失败\n失败原因为:VirtualAlloc() 申请内存失败", L"错误", MB_OK);
				break;
			case 2:
				MessageBoxW(0, L"调用SubClassWindow() 失败\n失败原因为:为SubClassWindow() 提供的内存不足", L"错误", MB_OK);
				break;
			default:
				MessageBoxW(0, L"调用SubClassWindow() 失败\n失败原因为:其他未知错误", L"错误", MB_OK);
				break;
			}
			return 0;
		}
		return pData;
	}
}

typedef LRESULT(__stdcall* SUBWNDPROC)(LPSUBCLASSSTRUCT, UINT, WPARAM, LPARAM);


// 返回值强转成 WNDPROC 就可以在注册类时赋值, 窗口过程的第一个参数就是这个返回值
// proc = 子类化回调函数
// param = 用户数据
inline LPSUBCLASSSTRUCT MakeProc(SUBWNDPROC proc, void* param)
{
	__subclass::LPMAKEPROCDATA pData = __subclass::subclassAlloc();
	if (!pData)return 0;
#ifdef _M_X64
	// 这里执行的指令有45个字节, 函数地址 - 指令地址 - 指令字节数
	const INT_PTR pFun = (INT_PTR)proc - INT_PTR(pData) - 45;

	const unsigned char bin[] = {
		0x50,								// push rax	保存原始rax寄存器
		0x48, 0xB8,0,0,0,0,0,0,0,0,			// mov rax, pData(+3) 把地址放入到eax
		0x50,								// push rax	保存地址
		0x48, 0x8B, 0x40, 0x50,				// mov rax,[rax+50h] 把地址偏移0x50的值存到rax, 指向hWnd
		0x48, 0xA9, 00, 00, 00, 00,			// test rax, 0	 判断rax是否为0
		0x58,								// pop rax	把地址取出来放到rax
		0x75, 0x04,							// jnz 跳转4个字节
		0x48, 0x89, 0x48, 0x50,				// mov [rax+50h],rcx 把原来的第一个参数存到地址+0x50的位置
		0x58,								// pop rax 还原原始rax数据
		0x48, 0xb9, 0,0,0,0,0,0,0,0,		// mov rcx, pData(+32)
		0xe9, 0,0,0,0						// jmp pFun(+41)
	};
	memcpy(pData->data, bin, sizeof(bin));
	memcpy(pData->data + 3, &pData, 8);
	memcpy(pData->data + 32, &pData, 8);
	memcpy(pData->data + 41, &pFun, 4);

#else
	// 这里执行的指令有39个字节
	const int pFun = (int)proc - int(pData) - 39;
	const unsigned char bin[] = {
		0x50 ,								// push eax 
		0xB8, 0,0,0,0,						// mov eax, pData(+2)
		0x50,								// push eax 
		0x8B, 0x40, 0x50,					// mov eax, [eax+50h] 
		0xA9, 0x00, 0x00, 0x00, 0x00,		// test eax,0 
		0x58,								// pop eax 
		0x75, 0x08,							// jne 4个字节 
		0xFF, 0x74, 0x24, 0x08,				// push [esp+8h] 
		0x8F, 0x40, 0x50,					// pop [eax+50h] 
		0x58,								// pop eax 
		0xc7, 0x44, 0x24, 0x04,	0,0,0,0,	// mov [esp+4], pData(+30)
		0xe9, 0,0,0,0						// jmp pFun(+35)
	};
	memcpy(pData->data, bin, sizeof(bin));
	memcpy(pData->data + 2, &pData, 4);
	memcpy(pData->data + 30, &pData, 4);
	memcpy(pData->data + 35, &pFun, 4);

#endif

	FlushInstructionCache(GetCurrentProcess(), pData, sizeof(SUBCLASSSTRUCT));
	LPSUBCLASSSTRUCT ptr = (LPSUBCLASSSTRUCT)pData;
	ptr->param = param;
	// 返回的这个回调函数可以用作子类化的回调函数
	// 子类化第一个参数是 SUBCLASSSTRUCT 结构指针
	return ptr;
}

// 返回值强转成 WNDPROC 就可以在注册类时赋值, 窗口过程的第一个参数就是这个返回值
// pThis = 回调函数所在的类指针
// proc = 子类化回调函数, 可以用 类方法, &类名::方法名 来获取
// param = 用户数据
template<typename T>
inline LPSUBCLASSSTRUCT MakeProc(T* pThis, LRESULT(__stdcall T::* fnCallback)(LPSUBCLASSSTRUCT, UINT, WPARAM, LPARAM), void* param)
{
	const INT_PTR proc = (INT_PTR) * ((void**)&fnCallback);
	__subclass::LPMAKEPROCDATA pData = __subclass::subclassAlloc();
	if (!pData || !proc)return 0;
#ifdef _M_X64
	// 这里执行的指令有67个字节
	const INT_PTR pFun = proc - (INT_PTR)pData - 67;
	const unsigned char bin[] = {
		0x50,								// push rax	保存原始rax寄存器
		0x48, 0xB8,0,0,0,0,0,0,0,0,			// mov rax, pData(+3) 把地址放入到eax
		0x50,								// push rax	保存地址
		0x48, 0x8B, 0x40, 0x50,				// mov rax,[rax+50h] 把地址偏移0x50的值存到rax, 指向hWnd
		0x48, 0xA9, 00, 00, 00, 00,			// test rax, 0	 判断rax是否为0
		0x58,								// pop rax	把地址取出来放到rax
		0x75, 0x04,							// jnz 跳转4个字节
		0x48, 0x89, 0x48, 0x50,				// mov [rax+50h],rcx 把原来的第一个参数存到地址+0x50的位置
		0x58,								// pop rax 还原原始rax数据
		0x41, 0x51,							// push r9
		0x8f, 0x44, 0x24, 0x28,				// pop [esp-28h]
		0x4d, 0x8b, 0xc8,					// mov r9, r8
		0x4c, 0x8b, 0xc2,					// mov r8, rdx
		0x48, 0xba,	0,0,0,0,0,0,0,0,		// mov rdx, pData(+44) 
		0x48, 0xb9, 0,0,0,0,0,0,0,0,		// mov rcx, pThis(+54)
		0xe9, 0,0,0,0						// jmp pFun(+63)
	};
	memcpy(pData->data, bin, sizeof(bin));
	memcpy(pData->data + 3, &pData, 8);
	memcpy(pData->data + 44, &pData, 8);
	memcpy(pData->data + 54, &pThis, 8);
	memcpy(pData->data + 63, &pFun, 4);

#else
	// 这里执行的指令有50个字节
	const INT_PTR pFun = proc - (INT_PTR)pData - 50;
	char as[] = { 
	};
	const unsigned char bin[] = {
		0x50 ,								// push eax 
		0xB8, 0,0,0,0,						// mov eax, pData(+2)
		0x50,								// push eax 
		0x8B, 0x40, 0x50,					// mov eax, [eax+50h] 
		0xA9, 0x00, 0x00, 0x00, 0x00,		// test eax,0 
		0x58,								// pop eax 
		0x75, 0x08,							// jne 4个字节 
		0xFF, 0x74, 0x24, 0x08,				// push [esp+8h] 
		0x8F, 0x40, 0x50,					// pop [eax+50h] 
		0x58,								// pop eax 
		0xff, 0x34, 0x24,					// push esp
		0xc7, 0x44, 0x24, 0x04,	0,0,0,0,	// mov [esp+4], pThis(+33)
		0xc7, 0x44, 0x24, 0x08,	0,0,0,0,	// mov [esp+8], pData(+41)
		0xe9, 0,0,0,0						// jmp pFun(+46)
	};
	memcpy(pData->data, bin, sizeof(bin));
	memcpy(pData->data + 2, &pData, 4);
	memcpy(pData->data + 32, &pThis, 4);
	memcpy(pData->data + 41, &pData, 4);
	memcpy(pData->data + 46, &pFun, 4);

#endif
	FlushInstructionCache(GetCurrentProcess(), pData, sizeof(SUBCLASSSTRUCT));
	LPSUBCLASSSTRUCT ptr = (LPSUBCLASSSTRUCT)pData;
	ptr->param = param;
	// 返回的这个回调函数可以用作子类化的回调函数
	// 子类化第一个参数是 SUBCLASSSTRUCT 结构指针
	return ptr;
}


// 当不需要使用时必须要调用FreeSubClass()进行释放, 传入的数据为子类化的第一个参数
// 此函数调用 SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (WNDPROC)MakeProc()) 来进行子类化
// 如果只需要获取子类化的函数, 请调用 MakeProc()
// hWnd = 要子类化的窗口句柄
// proc = 子类化回调函数
// param = 用户数据
inline bool __stdcall SubClassWindow(HWND hWnd, SUBWNDPROC proc, void* param)
{
	//if (!IsWindow(hWnd))return false;
	LPSUBCLASSSTRUCT ptr = MakeProc(proc, param);
	if (!ptr)return false;
	ptr->hWnd = hWnd;
	ptr->oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)ptr);
	return true;
}

// 当不需要使用时必须要调用FreeSubClass()进行释放, 传入的数据为子类化的第一个参数
// 此函数调用 SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (WNDPROC)MakeProc()) 来进行子类化
// 如果只需要获取子类化的函数, 请调用 MakeProc()
// hWnd = 要子类化的窗口句柄
// pThis = 回调函数所在的类指针
// proc = 子类化回调函数, 可以用 类方法, &类名::方法名 来获取
// param = 用户数据
template<typename T> 
inline bool __stdcall SubClassWindow( HWND hWnd, T* pThis, LRESULT(__stdcall T::* proc)(LPSUBCLASSSTRUCT, UINT, WPARAM, LPARAM), void* param)
{
	if (!IsWindow(hWnd)) return false;
	LPSUBCLASSSTRUCT ptr = MakeProc(pThis, proc, param);
	if (!ptr)return false;
	ptr->hWnd = hWnd;
	ptr->oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)ptr);
	// 返回的这个回调函数可以用作子类化的回调函数
	// 子类化第一个参数是 SUBCLASSSTRUCT 结构指针
	return true;
}


// 释放申请的内存空间, 调用后会把传入参数置0, 参数是子类化里第一个参数的值
// 调用此函数前必须先调用 SetWindowLongW(hWnd, GWLP_WNDPROC, oldProc) 还原子类化
inline bool __stdcall FreeSubClass(__subclass::LPMAKEPROCDATA& proc)
{
	return __subclass::mempool.free(proc);
}

