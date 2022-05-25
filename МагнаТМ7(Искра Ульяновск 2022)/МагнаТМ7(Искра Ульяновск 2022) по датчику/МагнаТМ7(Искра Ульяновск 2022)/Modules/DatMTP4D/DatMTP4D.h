#ifndef DatMTP4DH
#define DatMTP4DH
//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  001M^/cr         - ������ ��������

	// ������:
	// 1  001M100023K/cr	- 

struct SMTP4D_dat
{
	String adr;		// ����� ���������
		
	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ������� �������������� ������
		RCom,				// ������� ������ �����
		Buf_len,			// ����� �������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������

	unsigned int
		*Pres_DMTP4D;			// ��������

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
		*GB_Main;
	
	TLabel
		*Lbl_Adr,
		*Lbl_Zap1;

	TEdit
		*Edt_Zap1;		

	void DatMTP4D_Gen(); // �������� ��������
	bool DatMTP4D_Manage(unsigned int,bool); // ������� ����� � ��������
	void DatMTP4D_FrmZap(bool);		// ������������ �������
	bool DatMTP4D_ChkRep(bool);		// ��������� ������
	void __fastcall DatMTP4D_SetZap(TObject *Sender); // ������ ������
};

#define DAT_MTP4D_COUNT 1
	
SMTP4D_dat *Dat_MTP4D[DAT_MTP4D_COUNT];
#endif















