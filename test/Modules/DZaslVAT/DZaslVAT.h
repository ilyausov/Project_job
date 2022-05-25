//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������:
	// 1  R:xxxxxx\cr\lf		- �������� �������
	// 2  S:0xxxxxxx\cr\lf		- �������� ��������
	// 3  O:\cr\lf				- ������� ��������
	// 4  C:\cr\lf				- ������� ��������
	// 5  H:\cr\lf				- ���������
	// 6  L:00010000\cr\lf		- ��������
	// 7  i:76\cr\lf            - ������ ���������

	// ������:
	// 1  R:\cr\lf				- �������� �������
	// 2  S:\cr\lf				- �������� ��������
	// 3  O:\cr\lf				- ������� ��������
	// 4  C:\cr\lf				- ������� ��������
	// 5  H:\cr\lf				- ���������
	// 6  L:\cr\lf				- ��������
	// 7  i:76xxxxxxsyyyyyyyabc\cr\lf	- ������ ���������

struct SDZaslVAT
{

	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ������� �������������� ������
		RCom,				// ������� ������ �����
		Buf_len,			// ����� �������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������

    bool
        *Pr_DZaslVAT;       // ������� ������

    unsigned char
        *ZadCom_DZaslVAT,   // ������� �������
        *Otvet_DZaslVAT;    // �����

	unsigned int
		*ZadData_DZaslVAT,  // �������
		*TekPos_DZaslVAT,   // ������� �������
		*TekDat_DZaslVAT;   // ��������� �������
	int
		*TekDavl_DZaslVAT;	// ������� ��������

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
		*Lbl_Zap1,
		*Lbl_Zap2,
		*Lbl_Zap3,
		*Lbl_Zap4,
		*Lbl_Zap5,
		*Lbl_Zap6;

	TMaskEdit
		*Edt_Zap1,
		*Edt_Zap2;

	TEdit
		*Edt_Zap7_1,
		*Edt_Zap7_2;
		
	TButton
		*Btn_Zap1,
		*Btn_Zap2,
		*Btn_Zap3,
		*Btn_Zap4,
		*Btn_Zap5,
		*Btn_Zap6,
		*Btn_Zap7;

	void DZaslVAT_Gen(); // �������� ��������
	bool DZaslVAT_Manage(unsigned int,bool); // ������� ����� � ��������
	void DZaslVAT_FrmZap(bool);		// ������������ �������
	bool DZaslVAT_ChkRep(bool);		// ��������� ������
	void __fastcall DZaslVAT_SetZap(TObject *Sender); // ������ ������
};

#define DZaslVAT_COUNT 1
	
SDZaslVAT *DZaslVAT[DZaslVAT_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	