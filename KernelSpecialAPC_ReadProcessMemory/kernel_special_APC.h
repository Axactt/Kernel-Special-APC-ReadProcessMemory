#pragma once
#include<ntifs.h>


VOID Apc_Init();

//����ṹָ����Ϊ kernelroutine �Ĳ���
typedef struct _ReadMemory_Context {
	BOOLEAN Success;//�Ƿ�ɹ���ȡ
	UCHAR ErrorCode;//��ʱ���ش���
	PUCHAR R0Buffer;//�ȶ��������������
	ULONG64 Address;//��Ҫ��ȡ�ĵ�ַ
	ULONG64 Length;//��Ҫ��ȡ�ĳ���
}ReadMemory_Context,*PReadMemory_Context;


//ͨ��PID�����ȡ�ڴ�
BOOLEAN Apc_Read_Process_Memory_By_Pid(ULONG64 Pid, PUCHAR R3_Buffer, ULONG64 Address, ULONG64 Length);

//ͨ��EPROCESS�����ȡ�ڴ�
BOOLEAN Apc_Read_Process_Memory_By_Eprocess(PEPROCESS PEProcess, PUCHAR R3_Buffer, ULONG64 Address, ULONG64 Length);