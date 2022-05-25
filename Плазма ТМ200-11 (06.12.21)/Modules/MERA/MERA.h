//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  :aa0300000002ss\cr\lf		- ������ �������� ��������

	// ������:
	// 1  :aa0304ddd1ddd2ss\cr\lf

struct SMERA_dat
{
	String adr;		// ����� ���������
		
	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ������� �������������� ������
		//RCom,				// ������� ������ �����
		Buf_len,			// ����� �������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������

	unsigned int
		*Pres_MERA;			// ��������

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

	void DatMERA_Gen(); // �������� ��������
	bool DatMERA_Manage(unsigned int,bool); // ������� ����� � ��������
	void DatMERA_FrmZap(bool);		// ������������ �������
	bool DatMERA_ChkRep(bool);		// ��������� ������
	double raspakMERA();				// ���������� ������
};

#define DAT_MERA_COUNT 3
	
SMERA_dat *Dat_MERA[DAT_MERA_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
