//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
	extern TForm1 *Form1;

	// �������: KOM_TMN
	// 1 	0x01 0x05 0x02 0x01	0x00 0x01 0xCC 0xCC - �����
	// 2 	0x01 0x05 0x02 0x01	0x00 0x10 0xCC 0xCC	- ����
	// 3 	0x01 0x04 0x01 0x00	0x00 0x07 0xCC 0xCC	- ����� ��������� 1
    // 4 	0x01 0x04 0x01 0x0D	0x00 0x01 0xCC 0xCC	- ����� ��������� 2
	
	// ������: OTVET_TMN (��� VIDK)
	// 1  - �����
	// 2  - ����
	// 3  - ����� ��������� 1
    // 4  - ����� ��������� 2

unsigned char KNOmsk_Req_Buf[5][6] =
{   0x01,0x02,0x03,0x04,0x05,0x06,
    0x01,0x05,0x02,0x01,0x00,0x01,
    0x01,0x05,0x02,0x01,0x00,0x10,
    0x01,0x04,0x01,0x00,0x00,0x07,
    0x01,0x04,0x01,0x0D,0x00,0x01};

struct S_KNOmsk
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
        pr_sost1,           // �������� ������� �� ��������� �������
        pr_sost2;           // ������� ����� ������������ ������ ����� ������ �� ���

    bool
        *Pr_KNOmsk;         // ������� �����. ������
    unsigned char
        *Com_KNOmsk,        // ������� �������
        *Otv_KNOmsk;     // �����
    unsigned int
        *OtvM_KNOmsk[7];     // ����� ���������

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
		*Lbl_Uni;
		
	TEdit
		*Edt_Otv1,
        *Edt_Otv2,
		*Edt_Otv3,
        *Edt_Otv4;

    TCheckBox
		*CB_alarm1,
        *CB_alarm2,
        *CB_alarm3,
        *CB_alarm4,
        *CB_alarm5,
        *CB_alarm6,
        *CB_alarm7,
		*CB_alarm8;
		
	TButton
		*Btn_Zap1,
		*Btn_Zap2;

	void KNOmsk_Gen(); // �������� ��������
	bool KNOmsk_Manage(unsigned int,bool); // ������� ����� � ��������
	void KNOmsk_FrmZap(bool);		// ������������ �������
	bool KNOmsk_ChkRep(bool);		// ��������� ������
    unsigned short KNOmsk_GenCC(char*,unsigned int); // ������� �����. �����
    // bool KNOmsk_ChkCC(*char,*char); // �������� ����������� �����
	void __fastcall KNOmsk_SetZap(TObject *Sender); // ������ ������
};

#define KNOmsk_COUNT 1
	
S_KNOmsk *KNOmsk[KNOmsk_COUNT];

	
	
	
	
	
	
	
	
	
	

	
	
	
