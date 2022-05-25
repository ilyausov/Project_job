//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  001|00|740|02|=?|106|cr         - ������ ��������

	// ������:
	// 1  001|10|740|06|100023|025|cr	- 1000 mBar

struct SMPT200_dat
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
		*Pres_MPT200;			// ��������

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

	void DatMPT200_Gen(); // �������� ��������
	bool DatMPT200_Manage(unsigned int,bool); // ������� ����� � ��������
	void DatMPT200_FrmZap(bool);		// ������������ �������
	bool DatMPT200_ChkRep(bool);		// ��������� ������
	void __fastcall DatMPT200_SetZap(TObject *Sender); // ������ ������
};

#define DAT_MPT200_COUNT 3
	
SMPT200_dat *Dat_MPT200[DAT_MPT200_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
