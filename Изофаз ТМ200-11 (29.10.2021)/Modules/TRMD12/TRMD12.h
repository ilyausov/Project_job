//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  :aa060173ddddss\cr\lf		- ������ ������� 1
	// 2  :aa060573ddddss\cr\lf		- ������ ������� 2
	// 3  :aa060973ddddss\cr\lf		- ������ ������� 3
	// 4  :aa060D73ddddss\cr\lf		- ������ ������� 4
	// 5  :aa061173ddddss\cr\lf		- ������ ������� 5
	// 6  :aa061573ddddss\cr\lf		- ������ ������� 6
	// 7  :aa061973ddddss\cr\lf		- ������ ������� 7
	// 8  :aa061D73ddddss\cr\lf		- ������ ������� 8
	// 9  :aa062173ddddss\cr\lf		- ������ ������� 9
	// 10 :aa062573ddddss\cr\lf		- ������ ������� 10
	// 11 :aa062973ddddss\cr\lf		- ������ ������� 11
	// 12 :aa062D73ddddss\cr\lf		- ������ ������� 12
	// 13  :aa0300000004ss\cr\lf	- ������ ������� ����������

	// ������:
	// 1
	// ..
	// 12
	// 13  :aa0308ddd1ddd2ddd3ddd4ss\cr\lf
	
#define TRMD_CH_COUNT 12	// ���������� ������� ���������

struct STRMD
{
	String adr;		// ����� ���������
		
	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ����� �������
		Buf_len,			// ����� �������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������

	unsigned int
		*ZadTemp[TRMD_CH_COUNT],			// ������� 1
		//*ZadTemp2,			// ������� 2
		//*ZadTemp3,			// ������� 3
		//*ZadTemp4,			// ������� 4
		*TekTemp[TRMD_CH_COUNT];			// ����������� 1
		//*TekTemp2,			// ����������� 2
		//*TekTemp3,			// ����������� 3
		//*TekTemp4;			// ����������� 4
	bool
		*Pr_Sv[TRMD_CH_COUNT];

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
		*Edt_Zad[TRMD_CH_COUNT],			// ������ ������ ������� � ����������
		//*Edt_Zad2,
		//*Edt_Zad3,
		//*Edt_Zad4,
		*Edt_Tek[TRMD_CH_COUNT];
		//*Edt_Tek2,
		//*Edt_Tek3,
		//*Edt_Tek4;

	void TRMD_Gen(); // �������� ��������
	bool TRMD_Manage(unsigned int); // ������� ����� � ��������
	void TRMD_FrmZap( );		// ������������ �������
	bool TRMD_ChkRep( );		// ��������� ������
	void __fastcall TRMD_SetZap(TObject *Sender); // ������ ������
};

#define TRMD_COUNT 2
	
STRMD *TRMD[TRMD_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
