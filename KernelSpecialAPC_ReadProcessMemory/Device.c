#include "Head.h"

PDEVICE_OBJECT mydevice = NULL;

NTSTATUS Create_Complete_Routine(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	NTSTATUS Status;

	DbgPrint("����\n");
	Status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS My_Dispatch(PDEVICE_OBJECT mydevice, PIRP irp)
{

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	ULONG xh_code = stack->Parameters.DeviceIoControl.IoControlCode;
	ULONG buffer_lenth = 0;
	PUCHAR To_Ring3 = stack->Parameters.DeviceIoControl.Type3InputBuffer;//����д��Ӧ��
	PUCHAR From_Ring3 = irp->UserBuffer;//���Դ�Ӧ�ö���
	stack->Parameters.DeviceIoControl.InputBufferLength;
	stack->Parameters.DeviceIoControl.OutputBufferLength;
	irp->IoStatus.Information = 0;

	PR3_Read_Memory PTask = NULL;

	switch (xh_code)
	{
	case TEST://���ܲ���
	{
		PTask = From_Ring3;
		//DbgBreakPoint();
		if (TRUE == Apc_Read_Process_Memory_By_Pid(PTask->Pid, PTask->Read_Buffer, PTask->Address, PTask->Length)) {
			irp->IoStatus.Information = 1;
		}
		break;
	}
	default:
		break;
	}
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}



NTSTATUS Create_Device(PDRIVER_OBJECT DriverObject) {
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING device_name, device_link_name, event_name1, event_name2;//�豸�����������������¼���
	RtlInitUnicodeString(&device_name, my_device_name);//��ʼ���豸��
	RtlInitUnicodeString(&device_link_name, my_link_name);//��ʼ������������

	status = IoCreateDevice(DriverObject, 200, &device_name, FILE_DEVICE_UNKNOWN, 0, 1, &mydevice);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("�޷������豸!\n");
		return status;
	}
	status = IoCreateSymbolicLink(&device_link_name, &device_name);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("�޷������������ӣ�\n");
		IoDeleteDevice(mydevice);
		return status;
	}
	DbgPrint("�����ɹ���\n");
	//main->DriverUnload = Unload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = Create_Complete_Routine;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = My_Dispatch;
	return STATUS_SUCCESS;
}

