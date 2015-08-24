//定义命令行信息结构体
typedef struct tagCOMMANDLINEINFO {
	DWORD	dwDestCommand_addr;		//目标进程命令行的起始地址
	DWORD	iDestCommandLength;		//目标进程命令行的长度
} COMMANDLINEINFO;

//函数声明
BOOL UpgradeProcessPrivilege(HANDLE, LPCTSTR);
BOOL GetProcessCommandLineInfo(HANDLE, COMMANDLINEINFO *);