#include"Head.h"


VOID(*KeInitializeApc)(PKAPC Apc,PETHREAD pKthread,int a3,PVOID KernelRoutine,PVOID RundownRoutine,PVOID NormalRoutine,UCHAR ApcMode,ULONG64 NormalContext) = NULL;
VOID(*KeInsertQueueApc)(PKAPC Apc, ULONG64 SystemArgument1, ULONG64 SystemArgument2, int a4) = NULL;

ULONG64 Old_Pid = NULL;
PEPROCESS Old_Process = NULL;

VOID Apc_Init() {
	PULONG64 P_KeInitializeApc = (PULONG64)&KeInitializeApc;
	PULONG64 P_KeInsertQueueApc = (PULONG64)&KeInsertQueueApc;

	//����KeInitializeApc������ַ
	*P_KeInitializeApc = 0xfffff8042b62a530;

	//����KeInsertQueueApc������ַ
	*P_KeInsertQueueApc = 0xfffff8042b63dc10;
}

//�����ڴ�Ľ��̵��߳��������ô˺���
VOID Read_Memory(PKAPC APC, PULONG64 MormalRoutine, PReadMemory_Context* PPcontext, PULONG64 Pin_time, PULONG64 s2) {
	//*s2 ��NULL
	PReadMemory_Context Pcontext = *PPcontext;
	if ((__rdtsc() - *Pin_time) > 490000000) {
		return;//���APC������ʱ��̫��������Ͳ���������
	}
	__try {
		memcpy(Pcontext->R0Buffer, Pcontext->Address, Pcontext->Length);
		Pcontext->Success = 1;

	}except(1) {
	
		Pcontext->Success = 2;
		Pcontext->ErrorCode = 0xff;
	}
	return;
}

BOOLEAN My_RundownRoutine() {
	return TRUE;//����APCʧ��ʱ�����������
}

BOOLEAN New_SpecialApc_Task_Init(PKAPC APC,PETHREAD Thread, PReadMemory_Context Pcontext) {
	//DbgBreakPoint();
	KeInitializeApc(APC, Thread, 0, (PVOID)Read_Memory, NULL, (PVOID)My_RundownRoutine, 0, (ULONG64)Pcontext);
}

//ͨ��PID�����ȡ�ڴ�
BOOLEAN Apc_Read_Process_Memory_By_Pid(ULONG64 Pid, PUCHAR R3_Buffer, ULONG64 Address, ULONG64 Length) {
	PEPROCESS PEProcess = NULL;
	if (Old_Pid == Pid) {
		return Apc_Read_Process_Memory_By_Eprocess(Old_Process, R3_Buffer, Address, Length);
	}
	else {
		if (!Get_pEprocess_By_Pid(Pid, &PEProcess))
			return FALSE;
		return Apc_Read_Process_Memory_By_Eprocess(PEProcess, R3_Buffer, Address, Length);
	}
}

//ͨ��EPROCESS�����ȡ�ڴ�
BOOLEAN Apc_Read_Process_Memory_By_Eprocess(PEPROCESS PEProcess,  PUCHAR R3_Buffer, ULONG64 Address, ULONG64 Length) {
	
	PETHREAD Thread = NULL;
	BOOLEAN Return_Code = FALSE;
	PUCHAR R0_Buffer = ExAllocatePool(NonPagedPool, Length);
	ULONG64 in_time = __rdtsc();//��ȡ��ǰcpuʱ��
	ULONG64 into = 0;//����APC.Inserted���ж�����Ϊ���仯��̫�죬�׳�bug
	KAPC APC = { 0 };
	ReadMemory_Context context = { 0 };//���ں� kernelroutine ������������
	context.R0Buffer = R0_Buffer;
	context.Address = Address;
	context.Length = Length;

	if (R0_Buffer == NULL)
		return Return_Code;
	memset(R0_Buffer, 0, Length);

	while ((__rdtsc() - in_time) < 100000000) {//100���붼�Ҳ����ܲ���̣߳�˵����������ǹ¶�����������
		if (!Get_APC_pThrean_By_Process(PEProcess, &Thread)) {
			continue;
		}
		New_SpecialApc_Task_Init(&APC, Thread, &context);
		KeInsertQueueApc(&APC, in_time, NULL, 0);
		into = 1;
		break;
	}
	if (into == 0) {//����APC.Inserted���ж�����Ϊ���仯��̫�죬�׳�bug
		goto EXIT;
	}
	while (context.Success != TRUE && (__rdtsc() - in_time) < 500000000 );//�����������ȴ�500����
	if (context.Success == TRUE) {
		memcpy(R3_Buffer, R0_Buffer, Length);
		Return_Code = context.Success;
	}
EXIT:
	ExFreePool(R0_Buffer);
	return Return_Code;
}