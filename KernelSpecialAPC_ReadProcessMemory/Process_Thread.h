#pragma once
#include<ntifs.h>


enum  Eprocess_Members_Offset //�Լ���취��ȡ���̽ṹƫ��
{
	ThreadListHead = 0x30,//�߳�����
	ProcessListEntry = 0x240,//��������
	UniqueProcessId = 0x2e0,//����PID

};

enum Ethread_Members_Offset //�Լ���취��ȡ�߳̽ṹƫ��
{
	Running = 0x71,
	SpecialApcDisable = 0x1e6,
	ThreadListEntry = 0x2f8,
};

BOOLEAN Get_pEprocess_By_Pid(ULONG64 Pid, PEPROCESS* pProcess);

BOOLEAN Get_APC_pThrean_By_Process(PEPROCESS Process, PETHREAD* pThread);