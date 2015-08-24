#include "stdafx.h"
#include "GetCmdLine.h"

/****************************************************************************************/
/*****************************************子程序*****************************************/
/****************************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////////
//名称：UpgradeProcessPrivilege                                                         
//功能：提升进程权限                                                                                                                                      
//////////////////////////////////////////////////////////////////////////////////////////
BOOL UpgradeProcessPrivilege(HANDLE hProcess,
							 LPCTSTR lpPrivilegeName = SE_DEBUG_NAME)
{
	HANDLE hToken = NULL;
	if(OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) 
	{
		LUID Luid;
		if(LookupPrivilegeValue(NULL, lpPrivilegeName, &Luid)) 
		{
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			tp.Privileges[0].Luid = Luid;
			return( AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL) );
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//名称：GetProcessCommandLineInfo                                                       
//功能：获取进程命令行信息                                                                                                                                 
//////////////////////////////////////////////////////////////////////////////////////////
BOOL GetProcessCommandLineInfo(HANDLE hProcess, COMMANDLINEINFO *CommandLineInfo)
{
	if(IsBadReadPtr(CommandLineInfo,sizeof(COMMANDLINEINFO)))	//判断指针是否有效
		return FALSE;

	FARPROC	GetCommandLineA_addr, lstrlenA_addr;
	HANDLE	hThread;

	//获取 GetCommandLineA 的地址
	GetCommandLineA_addr = GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "GetCommandLineA");
	if(GetCommandLineA_addr == 0)
		return FALSE;
	//启动远程线程，使远程进程执行 GetCommandLineA 函数
	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)GetCommandLineA_addr,NULL, 0, NULL);
	if(hThread == 0)
		return FALSE;
	WaitForSingleObject(hThread, INFINITE);	//等待远程线程结束
	//作者：梁增健  Email：lzj85@163.com  MyQQ：184186651
	//获取远程 GetCommandLineA 的返回值，若正常返回，则该值为远程进程命令行的首地址
	GetExitCodeThread(hThread, &(CommandLineInfo->dwDestCommand_addr));
	CloseHandle(hThread);
	if(CommandLineInfo->dwDestCommand_addr == 0)
		return FALSE;

	//获取 lstrlenA 的地址
	lstrlenA_addr = GetProcAddress( GetModuleHandle(_T("Kernel32.dll")), "lstrlenA");
	if(GetCommandLineA_addr == 0)
		return FALSE;
	//启动远程线程，使远程进程执行 lstrlenA 函数
	hThread = CreateRemoteThread(hProcess, NULL, 0,(LPTHREAD_START_ROUTINE)lstrlenA_addr,
		(void *)CommandLineInfo->dwDestCommand_addr, 0, NULL);
	if(hThread == 0)
		return FALSE;
	WaitForSingleObject (hThread, INFINITE);
	//获取远程 lstrlenA 的返回值，若正常返回，则该值为远程进程命令行文本的长度
	GetExitCodeThread (hThread, &(CommandLineInfo->iDestCommandLength));
	CloseHandle (hThread);

	return TRUE;
}
/*************************************************************************************/
/****************************************子程序***************************************/
/*************************************************************************************/
