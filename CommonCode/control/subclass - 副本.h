#pragma once

// �����ռ��ڵĲ������ⲿʹ��
// ʹ��ǰ��Ҫ #include<windows.h>
namespace __subclass
{
	typedef struct tagMAKEPROCDATA
	{
		BYTE data[80];		// Ϊ��ͨ��, ָ�д�����������, Ҳ����Ҫ1�ֽڶ���
	}MAKEPROCDATA, * LPMAKEPROCDATA;
}
typedef struct tagSUBCLASSSTRUCT : private __subclass::tagMAKEPROCDATA	
{
	// ˽�м̳�
	HWND hWnd;				// ���໯�Ĵ��ھ��
	WNDPROC oldProc;		// �ɴ��ڹ���
	void* param;			// �û�����
}SUBCLASSSTRUCT, * LPSUBCLASSSTRUCT;

namespace __subclass
{
	// ����VirtualAlloc() ������ֽ�����һҳ, һ��һҳ��4096���ֽ�
	// ÿ��������ô��, ֻ�ü�ʮ���ֽ�, ʣ�µ�4000���ֽڶ��˷���
	// ���������򵥵��ڴ��
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
			// 8k ���ֽ��㹻��, ���֧�� 8192/sizeof(SUBCLASSSTRUCT) �����໯����, �ڲ��ͷŵ�ǰ����
			size_t size = 0x2000;	// 4k����
#ifdef _M_X64
			// x64��Ҫ�Լ�ָ������ĵ�ַ, �����ϵͳ�Զ�����, �п��ܺ�����ַ��ȥ����ĵ�ַ�����4���ֽ�
			INT_PTR base = 0x100000000;
			for (INT_PTR i = 0; i < 50; i++)
			{
				ptr = (_list*)VirtualAlloc((LPVOID)base, size,
					MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (ptr)break;
				base += 0x100000000;
			}
#else
			// x86û��ô�ི��, ��ϵͳ�Լ������ַ
			ptr = (_list*)VirtualAlloc(0, size,
				MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
			if (!ptr)return false;

			root = ptr;	// �׵�ַ, �׽ڵ�
			LPBYTE p = (LPBYTE)ptr;
			size_t len = size / sizeof(SUBCLASSSTRUCT) - 1;
			for (size_t i = 0; i < len; i++)
			{
				// ��һ�����������¼�����ڴ��ַ
				// ÿ����ַ��¼�Ĵ�СΪ SUBCLASSSTRUCT �ṹ��С
				p += sizeof(SUBCLASSSTRUCT);
				ptr->next = (_list*)p;
				ptr = (_list*)p;	// ָ����һ���ڵ�
			}
			ptr->next = 0;
			ptr = (_list*)root;
			return true;
		}
	public:
		simpleMempool() :ptr(0), root(0) { ; }
		~simpleMempool() { VirtualFree(root, 0, MEM_RELEASE); ptr = 0; root = 0; }
		
		// ����ʧ�����׳�int�����쳣
		// �쳣ֵ 1=��ʼ��ʧ��, 2=�ռ䲻��,��Ҫ�ͷ�һЩ�ڴ�
		LPMAKEPROCDATA alloc()
		{
			size_t size = sizeof(SUBCLASSSTRUCT);
			if (!ptr)init();
			if (!ptr)throw int(1);
			void* p = ptr;		// ÿ�����붼�ӵ�ǰ�ڵ㿪ʼȡ, �ͷ���ԭ���׽ڵ�
			if (ptr->next == 0)throw int(2);	// û���ڴ���, �׳��쳣

			ptr = ptr->next;	// ��ǰ�ڵ�ָ����һ���ڴ�
			return (LPMAKEPROCDATA)p;
		}
		bool free(LPMAKEPROCDATA& p)
		{
			if (!ptr || !p)return false;
			// �ͷžͼ򵥵ļ���������
			memset(p, 0, sizeof(SUBCLASSSTRUCT));
			((_list*)p)->next = ptr;	// �ŵ�����ͷ��
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
				MessageBoxW(0, L"����SubClassWindow() ʧ��\nʧ��ԭ��Ϊ:VirtualAlloc() �����ڴ�ʧ��", L"����", MB_OK);
				break;
			case 2:
				MessageBoxW(0, L"����SubClassWindow() ʧ��\nʧ��ԭ��Ϊ:ΪSubClassWindow() �ṩ���ڴ治��", L"����", MB_OK);
				break;
			default:
				MessageBoxW(0, L"����SubClassWindow() ʧ��\nʧ��ԭ��Ϊ:����δ֪����", L"����", MB_OK);
				break;
			}
			return 0;
		}
		return pData;
	}
}

typedef LRESULT(__stdcall* SUBWNDPROC)(LPSUBCLASSSTRUCT, UINT, WPARAM, LPARAM);


// ����ֵǿת�� WNDPROC �Ϳ�����ע����ʱ��ֵ, ���ڹ��̵ĵ�һ�����������������ֵ
// proc = ���໯�ص�����
// param = �û�����
inline LPSUBCLASSSTRUCT MakeProc(SUBWNDPROC proc, void* param)
{
	__subclass::LPMAKEPROCDATA pData = __subclass::subclassAlloc();
	if (!pData)return 0;
#ifdef _M_X64
	// ����ִ�е�ָ����45���ֽ�, ������ַ - ָ���ַ - ָ���ֽ���
	const INT_PTR pFun = (INT_PTR)proc - INT_PTR(pData) - 45;

	const unsigned char bin[] = {
		0x50,								// push rax	����ԭʼrax�Ĵ���
		0x48, 0xB8,0,0,0,0,0,0,0,0,			// mov rax, pData(+3) �ѵ�ַ���뵽eax
		0x50,								// push rax	�����ַ
		0x48, 0x8B, 0x40, 0x50,				// mov rax,[rax+50h] �ѵ�ַƫ��0x50��ֵ�浽rax, ָ��hWnd
		0x48, 0xA9, 00, 00, 00, 00,			// test rax, 0	 �ж�rax�Ƿ�Ϊ0
		0x58,								// pop rax	�ѵ�ַȡ�����ŵ�rax
		0x75, 0x04,							// jnz ��ת4���ֽ�
		0x48, 0x89, 0x48, 0x50,				// mov [rax+50h],rcx ��ԭ���ĵ�һ�������浽��ַ+0x50��λ��
		0x58,								// pop rax ��ԭԭʼrax����
		0x48, 0xb9, 0,0,0,0,0,0,0,0,		// mov rcx, pData(+32)
		0xe9, 0,0,0,0						// jmp pFun(+41)
	};
	memcpy(pData->data, bin, sizeof(bin));
	memcpy(pData->data + 3, &pData, 8);
	memcpy(pData->data + 32, &pData, 8);
	memcpy(pData->data + 41, &pFun, 4);

#else
	// ����ִ�е�ָ����39���ֽ�
	const int pFun = (int)proc - int(pData) - 39;
	const unsigned char bin[] = {
		0x50 ,								// push eax 
		0xB8, 0,0,0,0,						// mov eax, pData(+2)
		0x50,								// push eax 
		0x8B, 0x40, 0x50,					// mov eax, [eax+50h] 
		0xA9, 0x00, 0x00, 0x00, 0x00,		// test eax,0 
		0x58,								// pop eax 
		0x75, 0x08,							// jne 4���ֽ� 
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
	// ���ص�����ص����������������໯�Ļص�����
	// ���໯��һ�������� SUBCLASSSTRUCT �ṹָ��
	return ptr;
}

// ����ֵǿת�� WNDPROC �Ϳ�����ע����ʱ��ֵ, ���ڹ��̵ĵ�һ�����������������ֵ
// pThis = �ص��������ڵ���ָ��
// proc = ���໯�ص�����, ������ �෽��, &����::������ ����ȡ
// param = �û�����
template<typename T>
inline LPSUBCLASSSTRUCT MakeProc(T* pThis, LRESULT(__stdcall T::* fnCallback)(LPSUBCLASSSTRUCT, UINT, WPARAM, LPARAM), void* param)
{
	const INT_PTR proc = (INT_PTR) * ((void**)&fnCallback);
	__subclass::LPMAKEPROCDATA pData = __subclass::subclassAlloc();
	if (!pData || !proc)return 0;
#ifdef _M_X64
	// ����ִ�е�ָ����67���ֽ�
	const INT_PTR pFun = proc - (INT_PTR)pData - 67;
	const unsigned char bin[] = {
		0x50,								// push rax	����ԭʼrax�Ĵ���
		0x48, 0xB8,0,0,0,0,0,0,0,0,			// mov rax, pData(+3) �ѵ�ַ���뵽eax
		0x50,								// push rax	�����ַ
		0x48, 0x8B, 0x40, 0x50,				// mov rax,[rax+50h] �ѵ�ַƫ��0x50��ֵ�浽rax, ָ��hWnd
		0x48, 0xA9, 00, 00, 00, 00,			// test rax, 0	 �ж�rax�Ƿ�Ϊ0
		0x58,								// pop rax	�ѵ�ַȡ�����ŵ�rax
		0x75, 0x04,							// jnz ��ת4���ֽ�
		0x48, 0x89, 0x48, 0x50,				// mov [rax+50h],rcx ��ԭ���ĵ�һ�������浽��ַ+0x50��λ��
		0x58,								// pop rax ��ԭԭʼrax����
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
	// ����ִ�е�ָ����50���ֽ�
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
		0x75, 0x08,							// jne 4���ֽ� 
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
	// ���ص�����ص����������������໯�Ļص�����
	// ���໯��һ�������� SUBCLASSSTRUCT �ṹָ��
	return ptr;
}


// ������Ҫʹ��ʱ����Ҫ����FreeSubClass()�����ͷ�, ���������Ϊ���໯�ĵ�һ������
// �˺������� SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (WNDPROC)MakeProc()) ���������໯
// ���ֻ��Ҫ��ȡ���໯�ĺ���, ����� MakeProc()
// hWnd = Ҫ���໯�Ĵ��ھ��
// proc = ���໯�ص�����
// param = �û�����
inline bool __stdcall SubClassWindow(HWND hWnd, SUBWNDPROC proc, void* param)
{
	//if (!IsWindow(hWnd))return false;
	LPSUBCLASSSTRUCT ptr = MakeProc(proc, param);
	if (!ptr)return false;
	ptr->hWnd = hWnd;
	ptr->oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)ptr);
	return true;
}

// ������Ҫʹ��ʱ����Ҫ����FreeSubClass()�����ͷ�, ���������Ϊ���໯�ĵ�һ������
// �˺������� SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (WNDPROC)MakeProc()) ���������໯
// ���ֻ��Ҫ��ȡ���໯�ĺ���, ����� MakeProc()
// hWnd = Ҫ���໯�Ĵ��ھ��
// pThis = �ص��������ڵ���ָ��
// proc = ���໯�ص�����, ������ �෽��, &����::������ ����ȡ
// param = �û�����
template<typename T> 
inline bool __stdcall SubClassWindow( HWND hWnd, T* pThis, LRESULT(__stdcall T::* proc)(LPSUBCLASSSTRUCT, UINT, WPARAM, LPARAM), void* param)
{
	if (!IsWindow(hWnd)) return false;
	LPSUBCLASSSTRUCT ptr = MakeProc(pThis, proc, param);
	if (!ptr)return false;
	ptr->hWnd = hWnd;
	ptr->oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)ptr);
	// ���ص�����ص����������������໯�Ļص�����
	// ���໯��һ�������� SUBCLASSSTRUCT �ṹָ��
	return true;
}


// �ͷ�������ڴ�ռ�, ���ú��Ѵ��������0, ���������໯���һ��������ֵ
// ���ô˺���ǰ�����ȵ��� SetWindowLongW(hWnd, GWLP_WNDPROC, oldProc) ��ԭ���໯
inline bool __stdcall FreeSubClass(__subclass::LPMAKEPROCDATA& proc)
{
	return __subclass::mempool.free(proc);
}

