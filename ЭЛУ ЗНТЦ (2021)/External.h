// ������ ���������� ��������
int
    base_813        = 0x0220,
    base_DA16       = 0x0230,
    base_P32C32_1   = 0x0210,
	base_P32C32_2   = 0x0280,
    base_7225_1     = 0x0260,
    base_7225_2     = 0x0250;
DWord
	base_PISO_P32C32_1 = 0x0000, // PISO/PEX
	base_PISO_P32C32_2 = 0x0000, // PISO/PEX
    base_PISO_P16R16_1 = 0x0000, // PISO/PEX
    base_PISO_P16R16_2 = 0x0000, // PISO/PEX
    base_PISO_813 = 0x0000;      // PISO
// �������� ������
Word
    piso813err = 1,
	pisoP32C32_1err   = 1,
	pisoP32C32_2err   = 1,
    pisoP16R16_1err   = 1,
	pisoP16R16_2err   = 1,
    isoP32C32err1   = 1,
	isoP32C32err2   = 1,
    isoDA16err      = 1,
    iso813err       = 1,
    iso7225err1     = 1,
    iso7225err2     = 1,
	iso7250err      = 1;
//---------------------------------------------------------------------------
#define VALUE_COUNT 6
unsigned int
    // ����� ������� ������ ���������
    timeOprosISO  = 0,
    avrgAikValues[32][VALUE_COUNT] = {0}; // ������ �������� ���������� ���������� ������
//---------------------------------------------------------------------------
// ��� PISO ����
Word wTotalBoards, wInitialCode,wRtn;
DWord wBaseAddr;
     Word  wIrqNo;
     Word  wSubVendor, wSubDevice, wSubAux, wSlotBus, wSlotDevice;
     int   wRetVal;
//---------------------------------------------------------------------------