//������������Ϣ�ṹ��
typedef struct tagCOMMANDLINEINFO {
	DWORD	dwDestCommand_addr;		//Ŀ����������е���ʼ��ַ
	DWORD	iDestCommandLength;		//Ŀ����������еĳ���
} COMMANDLINEINFO;

//��������
BOOL UpgradeProcessPrivilege(HANDLE, LPCTSTR);
BOOL GetProcessCommandLineInfo(HANDLE, COMMANDLINEINFO *);