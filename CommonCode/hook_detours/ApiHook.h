#pragma once
#include <windows.h>
#include "detours/detours.h"



// 每个步骤出错都会抛出异常,类型为 apiHookError
class apiHook
{
private:
	PVOID m_oldFun;	// hook前的地址
	PVOID* m_ppOld;	// 由开始hook时传入进来,取消hook时,把原来的数据m_oldFun写入到这个地址里
	bool m_isBegin;	// 是否已经开始hook
	PVOID m_newFun;	// hook后指向的地址
public:
	// 默认构造函数,不处理
	apiHook() : m_oldFun(0), m_ppOld(0), m_isBegin(0), m_newFun(0)
	{
	}

	// 初始化
	// oldFun	= 被hook的地址,同时hook成功后会把原地址写入这里,失败则不写入 &(LPVOID&)function
	// newFun	= hook指向的地址,必须与被hook的地址定义一致
	// isBegin	= 是否开始hook,默认为真
	apiHook(LPVOID* oldFun, LPVOID newFun, bool isBegin = true) : m_oldFun(0), m_ppOld(0), m_isBegin(0), m_newFun(0)
	{
		hook(oldFun, newFun);
	}

	// 反初始化
	~apiHook()
	{
		//unHook();
	}

	// hook
	// oldFun	= 被hook的地址,同时hook成功后会把原地址写入这里,失败则不写入 &(LPVOID&)function
	// newFun	= hook指向的地址,必须与被hook的地址定义一致
	// isBegin	= 是否开始hook,默认为真
	bool hook(LPVOID* oldFun, LPVOID newFun, bool isBegin = true)
	{
		if (!oldFun || !newFun)return false;
		if ((*oldFun == m_oldFun && m_newFun == newFun) || (oldFun == m_ppOld && m_newFun == newFun))
			return true;


		if (m_oldFun) unHook();
		m_ppOld = oldFun;
		m_oldFun = *oldFun;
		m_newFun = newFun;

		//if (MH_CreateHook(m_oldFun, newFun, reinterpret_cast<LPVOID*>(oldFun)) != MH_OK)
		//	return false;

		DetourRestoreAfterWith();//恢复原来状态
		DetourTransactionBegin();//拦截开始
		DetourUpdateThread(GetCurrentThread());//刷新当前线程
		DetourAttach(m_ppOld, m_newFun);




		if (isBegin) return begin();
		return true;
	}

	// 开始hook, 返回假则表示未hook
	bool begin()
	{
		if (!m_oldFun)return false;
		if (m_isBegin)return true;	// 已经开始就不继续执行,直接返回真

		return 		DetourTransactionCommit();//拦截生效
	}

	// 暂停hook, 返回假则表示未hook
	bool end()
	{
		if (!m_oldFun)return false;
		if (!m_isBegin)return true;	// 没有开始就不执行关闭操作,直接返回真

		//m_isBegin = false;
		//return false;

		return true;
	}

	// 反初始化, 返回假则表示未hook
	bool unHook()
	{
		if (!(m_oldFun)) return false;
		bool b = end();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(m_ppOld, m_newFun);//撤销拦截函数
		DetourTransactionCommit();//


		*m_ppOld = m_oldFun;	// 把原数据写回去
		m_oldFun = 0;
		m_newFun = 0;
		m_isBegin = 0;
		m_ppOld = 0;

		//if (MH_Uninitialize() != MH_OK)
			//return false;

		return b;
	}

};


