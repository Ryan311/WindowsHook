#include "stdafx.h"
#include "GetCmdLine.h"

/****************************************************************************************/
/*****************************************�ӳ���*****************************************/
/****************************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////////
//���ƣ�UpgradeProcessPrivilege                                                         
//���ܣ���������Ȩ��                                                                                                                                      
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
//���ƣ�GetProcessCommandLineInfo                                                       
//���ܣ���ȡ������������Ϣ                                                                                                                                 
//////////////////////////////////////////////////////////////////////////////////////////
BOOL GetProcessCommandLineInfo(HANDLE hProcess, COMMANDLINEINFO *CommandLineInfo)
{
	if(IsBadReadPtr(CommandLineInfo,sizeof(COMMANDLINEINFO)))	//�ж�ָ���Ƿ���Ч
		return FALSE;

	FARPROC	GetCommandLineA_addr, lstrlenA_addr;
	HANDLE	hThread;

	//��ȡ GetCommandLineA �ĵ�ַ
	GetCommandLineA_addr = GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "GetCommandLineA");
	if(GetCommandLineA_addr == 0)
		return FALSE;
	//����Զ���̣߳�ʹԶ�̽���ִ�� GetCommandLineA ����
	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)GetCommandLineA_addr,NULL, 0, NULL);
	if(hThread == 0)
		return FALSE;
	WaitForSingleObject(hThread, INFINITE);	//�ȴ�Զ���߳̽���
	//���ߣ�������  Email��lzj85@163.com  MyQQ��184186651
	//��ȡԶ�� GetCommandLineA �ķ���ֵ�����������أ����ֵΪԶ�̽��������е��׵�ַ
	GetExitCodeThread(hThread, &(CommandLineInfo->dwDestCommand_addr));
	CloseHandle(hThread);
	if(CommandLineInfo->dwDestCommand_addr == 0)
		return FALSE;

	//��ȡ lstrlenA �ĵ�ַ
	lstrlenA_addr = GetProcAddress( GetModuleHandle(_T("Kernel32.dll")), "lstrlenA");
	if(GetCommandLineA_addr == 0)
		return FALSE;
	//����Զ���̣߳�ʹԶ�̽���ִ�� lstrlenA ����
	hThread = CreateRemoteThread(hProcess, NULL, 0,(LPTHREAD_START_ROUTINE)lstrlenA_addr,
		(void *)CommandLineInfo->dwDestCommand_addr, 0, NULL);
	if(hThread == 0)
		return FALSE;
	WaitForSingleObject (hThread, INFINITE);
	//��ȡԶ�� lstrlenA �ķ���ֵ�����������أ����ֵΪԶ�̽����������ı��ĳ���
	GetExitCodeThread (hThread, &(CommandLineInfo->iDestCommandLength));
	CloseHandle (hThread);

	return TRUE;
}
/*************************************************************************************/
/****************************************�ӳ���***************************************/
/*************************************************************************************/
