//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1 - ������ ������ � 0x15 ������� 
	// 2 - ������ ������������� � 0x01-0x04 �������� 
	// 3 - ������ ��������� 0x01-0x16

	// ������:


struct S_IVE
{
	unsigned char
		adr,				// ����� �����
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ������� �������������� ������
		RCom,				// ������� ������ �����
		Buf_len,			// ����� �������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������
		
	unsigned char
		type_BU_IVE;       // ��� ����� 0-����, 1-��� ...

    bool
		pr_zap[3],			// �������� ���������� ��������
        *Pr_BU_IVE;       	// ������� ������

	unsigned int
		*Kom_BU_IVE[5],   	// ������� �� ������
		*Otv_BU_IVE[10];   	// ��������� ��������

	SComport *SPort;		// ��������� �� ���� ����������
		
	// �������� ������ ��������
	TTabSheet
		*TS_Pan;            // ������� ��� ��������
	String name;            // �������� ��� �����������

	TPanel
		*Pnl_Parent;
	
	TRadioButton			// �������� ������ ������/��������
		*RB_prd,
		*RB_prm;	
	
	TGroupBox
		*GB_Main,
        *GB_Err,
        *GB_Par;
	
	TLabel
		*Lbl_Uni;

	TEdit
		*Edt_Ent_Char[4],	// ������ �����
		*Edt_Zad_Char[4],	// ������ ��� ������ �������
		*Edt_Tek_Char[4];	// ������ ��� ������ �������
		
	TCheckBox
		*CB_Zad[9],
		*CB_Ent[9],
        *CB_Err[5];

	TButton
		*Btn_Zap1,
		*Btn_Zap2,
		*Btn_Zap3;

	void BU_IVE_Gen(); // �������� ��������
	bool BU_IVE_Manage(unsigned int,bool); // ������� �����
	void BU_IVE_FrmZap(bool);		// ������������ �������
	bool BU_IVE_ChkRep(bool);		// ��������� ������
	void __fastcall BU_IVE_SetZap(TObject *Sender); // ������ ������
};

#define IVE_COUNT 3
	
S_IVE *BU_IVE[IVE_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
