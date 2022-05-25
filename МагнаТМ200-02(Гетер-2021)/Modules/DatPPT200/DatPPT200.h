//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  001|00|740|02|=?|106|cr         - ������ ��������

	// ������:
	// 1  001|10|740|06|100023|025|cr	- 1000 mBar

struct SPPT200_dat
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
		*Pres_PPT200;			// ��������

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

	void DatPPT200_Gen(); // �������� ��������
	bool DatPPT200_Manage(unsigned int,bool); // ������� ����� � ��������
	void DatPPT200_FrmZap(bool);		// ������������ �������
	bool DatPPT200_ChkRep(bool);		// ��������� ������
	void __fastcall DatPPT200_SetZap(TObject *Sender); // ������ ������
};

#define DAT_PPT200_COUNT 5
	
SPPT200_dat *Dat_PPT200[DAT_PPT200_COUNT];