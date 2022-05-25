//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  001M^/cr         - ������ ��������

	// ������:
	// 1  001M260014K/cr	- 1000 mBar

struct SMTM9D_dat
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
		*Pres_DMTM9D;			// ��������

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

	void DatMTM9D_Gen(); // �������� ��������
	bool DatMTM9D_Manage(unsigned int,bool); // ������� ����� � ��������
	void DatMTM9D_FrmZap(bool);		// ������������ �������
	bool DatMTM9D_ChkRep(bool);		// ��������� ������
	void __fastcall DatMTM9D_SetZap(TObject *Sender); // ������ ������
};

#define DAT_MTM9D_COUNT 1
	
SMTM9D_dat *Dat_MTM9D[DAT_MTM9D_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	