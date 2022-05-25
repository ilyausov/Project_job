//---------------------------------------------------------------------------
//--��������� PCI-1784--//
//---------------------------------------------------------------------------
#include "pci1784.h"
//---------------------------------------------------------------------------
//--������� � ����������������--//
//---------------------------------------------------------------------------
void OpenPCI1784()
{
    for(int i=0;i<COUNT_NUMBER;i++)
    {
        Counter1784[i] = UdCounterCtrl::Create();
        Counter1784[i]->setSelectedDevice(DeviceInformation(deviceDescription));
        Counter1784[i]->setChannelStart(i);
        Counter1784[i]->setChannelCount(1);
        Counter1784[i]->setEnabled(true);
    }
}

void ClosePCI1784() // ������� PCI-1784
{
    for(int i=0;i<COUNT_NUMBER;i++) Counter1784[i]->Dispose();
}

int GetChannel1()
{
	int value = 0;
	Counter1784[0]->Read(value);
    if(value&0x80000000)
    return (0xFFFFFFFF - value);
    return value;
}

int GetChannel2()
{
	int value = 0;
	Counter1784[1]->Read(value);
    if(value&0x80000000)
    return (0xFFFFFFFF - value);
    else return value;
}

int GetChannel3()
{
	int value = 0;
	Counter1784[2]->Read(value);
    if(value&0x80000000)
    return (0xFFFFFFFF - value);
    else return value;
}

int GetChannel4()
{
	int value = 0;
	Counter1784[3]->Read(value);
    if(value&0x80000000)
    return (0xFFFFFFFF - value);
    else return value;
    return 0;
}

void SetChannel1_0() // ��������� ������� ��� ������ 1 � ����
{
    Counter1784[0]->ValueReset();
}

void SetChannel2_0() // ��������� ������� ��� ������ 2 � ����
{
    Counter1784[1]->ValueReset();
}

void SetChannel3_0() // ��������� ������� ��� ������ 3 � ����
{
    Counter1784[2]->ValueReset();
}

void SetChannel4_0() // ��������� ������� ��� ������ 4 � ����
{
    Counter1784[3]->ValueReset();
}
//---------------------------------------------------------------------------
