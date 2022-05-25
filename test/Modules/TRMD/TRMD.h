//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  :aa060173ddddss\cr\lf		- ������ ������� 1
	// 2  :aa060573ddddss\cr\lf		- ������ ������� 2
	// 3  :aa060973ddddss\cr\lf		- ������ ������� 3
	// 4  :aa060D73ddddss\cr\lf		- ������ ������� 4
	// 5  :aa0300000004ss\cr\lf		- ������ ������� ����������

	// ������:
	// 1  
	// 2  
	// 3  
	// 4  
	// 5  :aa0308ddd1ddd2ddd3ddd4ss\cr\lf

struct STRMD
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
		*ZadTemp1,			// ������� 1
		*ZadTemp2,			// ������� 2
		*ZadTemp3,			// ������� 3
		*ZadTemp4,			// ������� 4
		*TekTemp1,			// ����������� 1
		*TekTemp2,			// ����������� 2
		*TekTemp3,			// ����������� 3
		*TekTemp4;			// ����������� 4
	bool
		*Pr_Sv;

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
		*Lbl_Uni;			// �������� ���� ��������

	TEdit
		*Edt_Zad1,			// ������ ������ ������� � ����������
		*Edt_Zad2,
		*Edt_Zad3,
		*Edt_Zad4,
		*Edt_Tek1,
		*Edt_Tek2,
		*Edt_Tek3,
		*Edt_Tek4;

	void TRMD_Gen(); // �������� ��������
	bool TRMD_Manage(unsigned int,bool); // ������� ����� � ��������
	void TRMD_FrmZap(bool);		// ������������ �������
	bool TRMD_ChkRep(bool);		// ��������� ������
	void __fastcall TRMD_SetZap(TObject *Sender); // ������ ������
};

#define TRMD_COUNT 1
	
STRMD *TRMD[TRMD_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	