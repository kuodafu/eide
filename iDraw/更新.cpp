#include <thread>

在文件头部增加声明：

#define EXTERADLL_UPCHEECK  L"up.exe" //检查更新的exe程序
#define UPDATE_SOFTKEY		L"9db2bcdf1341f0795476f7d69f18f630" //支持库正式版
#define UPDATE_SOFTKEY_BETA L"9320f55215bed793f6e15053c74868f2" //支持库测试版
#define UPDATE_VERSION1			L"0"
#define UPDATE_VERSION2			L"0"

typedef void (CALLBACK* callback_onUpdate)(wstr, wstr, bool);
bool checkUpdate(callback_onUpdate func);
void doUpdate();
//如果有更新，将自动触发这个事件。
void  CALLBACK  onUpdate(wstr message, wstr version, bool isForce);

//上方为声明部分。
////////////////////////////////////////////////////////////////////////////////




在 void _ide_ready() 中增加代码：
//--------------------------------------------------------------------------------------------------------------
//下方为检查更新
	std::thread t_checkup(
		[] {checkUpdate(onUpdate); }
	);
	t_checkup.detach();
//--------------------------------------------------------------------------------------------------------------





在文件最后部分增加：

typedef void  (CALLBACK* callback_onUpdate)(wstr, wstr, bool);
//如果有更新，将自动触发这个事件。
void  CALLBACK  onUpdate(wstr message, wstr version, bool isForce)
{
	if (isForce == true)
	{
		//如果需要强制更新就每次都弹出提示,但并不要求用户强制更新。
		wchar_t text[260];
		swprintf_s(text, 260, L"该版本已经不再推荐使用，请更新到最新版：%s \n %s", version.c_str(), message.c_str());
		MessageBoxW(s_hWndEWindow, text, L"易IDE视觉库缺少更新程序", MB_ICONINFORMATION);
	}

	//这里在界面上给出一个更新提示。
	if (MessageBoxW(0, L"已经有新版了！是否进行更新？", L"更新提示", MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		doUpdate();
	}
}

bool checkUpdate(callback_onUpdate callback)
{
	//每次启动易语言只检查一次更新
	static bool isChecked;
	if (isChecked == true)
		return false;
	isChecked = true;

	/*
	更新的逻辑：
	默认：每日只更新一次。
	如果用户手动设置了不更新，则改为每7天检查一次。
	*/
	tinyxml2::XMLNode* node;
	wchar_t nodeName[100];
	swprintf_s(nodeName, 100, L"@1/%s", L"UpdateCheck");
	if (!s_info->xml->GetNode(nodeName, node))
	{
		//没有就要添加一个新的。
		s_info->xml->InsertChildNode(L"@1", L"UpdateCheck", L"", 0, &node);
	}

	_str nodeNameA = wstr::W2A(L"LastCheckTime");
	tinyxml2::XMLNode* childnode = node->FirstChildElement(nodeNameA.c_str());
	if (!childnode)  // 没有这个节点, 需要创建一个
	{
		tinyxml2::XMLDocument* doc = s_info->xml->GetDocument();
		tinyxml2::XMLElement* child = doc->NewElement(nodeNameA.c_str());
		childnode = node->InsertEndChild(child);
	}

	wstr nodeValue = s_info->xml->GetNodeValue(childnode);
	wstr timestr(13);
	timestr.Format(L"%d", time(NULL));

	int lastTime = atoi(nodeValue.w2a().c_str());
	int nowTime = atoi(timestr.w2a().c_str());
	if (nowTime - lastTime < 86400)//大于1天再检查更新。
	{
		//没有超过1天，不检查更新
		return false;
	}

	s_info->xml->SetNodeValue(childnode, timestr.c_str());
	s_info->xml->SaveFile();
	wstr Path_up_exe(260);
	Path_up_exe.assign(m_runPath).append(IDRAW_FNE_IDRAW).append(L"\\").append(EXTERADLL_UPCHEECK);
	if (PathFileExists(Path_up_exe.c_str()) == FALSE)
	{
		//文件不存在,暂时不作处理
		wchar_t text[260];
		swprintf_s(text, 260, L"iDraw 目录下 缺少更新程序 %s, 请下载新的%s覆盖 或者重新安装支持库", EXTERADLL_UPCHEECK, EXTERADLL_UPCHEECK);
		//MessageBoxW(s_hWndEWindow, text, L"易IDE视觉库缺少更新程序", 0);
		return false;
	}

	wstr params(260), iniPath(MAX_PATH);
	//需要传递的命令行信息
	//"softkey:9db2bcdf1341f0795476f7d69f18f630
	//dir:F:\\Documents\\Desktop\\硬核更新\\使用新版助手更新测试\\tools\\lib\\
		//type:0
		//version1:0
		//version2:0
		//ini:D:\updateout.ini"

	wchar_t tmpPath[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, tmpPath);

	iniPath.Format(L"%s_eideup.ini", tmpPath);
	params.Format(L"softkey:%s dir:%s type:0 version1:%s version2:%s ini:%s", UPDATE_SOFTKEY_BETA, m_runPath, UPDATE_VERSION1, UPDATE_VERSION2, iniPath.c_str());

	//先将配置文件的项清除
	WritePrivateProfileString(L"updatePz", L"update", L"0", iniPath.c_str());
	STARTUPINFOW startInfo = { 0 };
	startInfo.cb = sizeof(STARTUPINFOW);
	PROCESS_INFORMATION pi;

	wstr cmdstr(MAX_PATH * 2);
	cmdstr.Format(L"%s %s", Path_up_exe.c_str(), params.c_str());
	BOOL bRet = CreateProcessW(NULL, cmdstr.data(), NULL, NULL, FALSE, NULL, NULL, NULL, &startInfo, &pi);

	if (bRet == TRUE)
	{
		WaitForSingleObject(pi.hProcess, 10000);//等待10s后 如果还没有响应则关闭掉。
		TerminateProcess(pi.hProcess, 2);//强制退出进程。
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		int isUpdate = GetPrivateProfileInt(L"updatePz", L"update", 0, iniPath.c_str());
		if (isUpdate == 1)
		{
			wstr message(MAX_PATH), version(MAX_PATH);
			bool isForce;
			//需要更新。
			GetPrivateProfileString(L"updatePz", L"data", L"", message.data(), MAX_PATH, iniPath.c_str());
			GetPrivateProfileString(L"updatePz", L"version", L"", version.data(), MAX_PATH, iniPath.c_str());
			isForce = GetPrivateProfileInt(L"updatePz", L"ForceUpdate", 0, iniPath.c_str());

			if (callback != nullptr)
				callback(message, version, isForce);
			return true;
		}

		else
		{
			//无需更新
		}
	}

	return false;
}
void doUpdate()
{
	//! 注意，这里的m_runPath 必须是定位到 易语言lib 目录的
	wstr dllPath(MAX_PATH), params(MAX_PATH);
	dllPath.assign(m_runPath).append(IDRAW_FNE_IDRAW).append(L"\\").append(EXTERADLL_UPCHEECK);
	params.Format(L"softkey:%s dir:%s type:1 version1:%s version2:%s", UPDATE_SOFTKEY_BETA, m_runPath, UPDATE_VERSION1, UPDATE_VERSION2);
	ShellExecute(0, L"open", dllPath.c_str(), params.c_str(), 0, SW_SHOW);
}