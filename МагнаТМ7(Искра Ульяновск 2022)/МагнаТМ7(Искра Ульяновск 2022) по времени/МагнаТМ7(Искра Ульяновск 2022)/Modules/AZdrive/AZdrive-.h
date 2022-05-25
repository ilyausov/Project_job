#ifndef AZdriveH
#define AZdriveH
//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
extern TForm1 *Form1;
		
	// Acom : 	0 - ������ ��������� ( ������� ��������� )
	//			1 - ������ ��������� ( ���, ���������, �������� )
	//			2 - ������ ��������� ( ���, ���������, �������� )
	//			3 - ������ ��������� ( ���������, ���������� )
	//			4 - ������ ��������� ( ���������, ���������� )
	//			5 - ������ ��������� ( ZHOME - ��������, ���������, ���������� )
	//			6 - ������ ��������� ( ZHOME - ��������, ���������, ���������� )
	//			7 - ����� �0
	//			8 - ���� �0
	
unsigned char AZ_Req_Buf[9][19] =
{     //0   1    2    3    4    5    6    7    8    9   10    11   12   13   14   15   16   17   18
      0xAA,0x03,0x00,0xC6,0x00,0x02,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 0
      0xAA,0x10,0x18,0x00,0x00,0x06,0x0C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,	// 1
      0xAA,0x03,0x18,0x00,0x00,0x06,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2
      0xAA,0x10,0x18,0x06,0x00,0x04,0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0xCC,0xCC,0x00,0x00,	// 3
      0xAA,0x03,0x18,0x06,0x00,0x04,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 4
      0xAA,0x10,0x02,0xb0,0x00,0x06,0x0C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,	// 5
      0xAA,0x03,0x02,0xb0,0x00,0x06,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 6
      0xAA,0x10,0x00,0x7c,0x00,0x02,0x04,0x00,0x00,0x00,0x08,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,	// 7
      0xAA,0x10,0x00,0x7c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00	// 8		  
};

	// ������
	//0	   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16
	//0xAA 0x03 0x04 0xdd 0xdd 0xdd 0xdd 0xCC 0xCC
	//0xAA 0x10 0x18 0x00 0x00 0x06 0xCC 0xCC
	//0xAA 0x03 0x0� 0xdd 0xdd 0xdd 0xdd 0xDD 0xDD 0xDD 0xDD 0xdd 0xdd 0xdd 0xdd 0xCC 0xCC
	
struct SAZ_drive
{
	unsigned char adr;		// ����� ���������
	String name;            // �������� ��� �����������

	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ������� �������������� ������
		Buf_len,			// ����� �������
	//	diagn_byte,        	// ����� ����� ������� ����������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������
	//unsigned int
	//	*CT_AZ;				// ������� ���������
	//unsigned char
	//	home_norm,
	//	norm,
	//	Stop_byte,			// ��� ���� ��� (�����)
	//	Stop_mask,
	//	Rdy_byte,			// ��� ����������
	//	Rdy_mask,
	//	Run_byte,			// ��� ��������
	//	Run_mask,
	//	HomeOut_byte,		// ��� �������� � Home
	//	HomeOut_mask,
	//	HomeIn_byte,		// ��� ���������� � Home
	//	HomeIn_mask;
			
	unsigned char			// ���������� VIDK
		*Kom_AZ,			// �������
		*Otv_AZ,			// �����       
		*Type_AZ,			// ��� �������� 1-���, 2-���
		*V_AZ;				// ����� �������� (�/�/�)

	int				
        Spd_AZ,				// ��������
		*Put_AZ;			// ���� ��������

	bool
		*Home_AZ,			// ������� �������� � Home
		*Pr_AZ;				// ������� �����
			
	bool
		Data_err1,			// ������� ������������ ������
		Data_err2;			// ������� ������������ ������
		
	int Rem_pos,			// ���������� �������� ���� (��� ������)
		Del_pos,			// ������� ����� (��� ������)
        *Abs_pos,			// ������������� ���� (��� ������)
		*Otn_pos;			// ������������� ���� (��� ������)
		
	unsigned char
		Zad_type;			// ��� �������� 1-���, 2-��� (�� �����������)		
		
	int Zad_pos,			// ������� ��������� (�� �����������)
		Zad_spd,			// ������� �������� (�� �����������)
		Tek_pos,			// ������� ��������� (�� �����������)
		Tek_acc,			// ������� ��������� (�� �����������)
		Tek_dec,			// ������� ���������� (�� �����������)
		Tek_ZHspd,			// ������� �������� ZHome (�� �����������)
		Tek_ZHacc,			// ������� ��������� ZHome (�� �����������)
		Tek_ZHnsp,			// ������� ���.�������� ZHome (�� �����������)
		Rem_acc,			// ������� ��������� (����������)
		Rem_dec,			// ������� ���������� (����������)
		Rem_ZHspd,			// ������� �������� ZHome (����������)
		Rem_ZHacc,			// ������� ��������� ZHome (����������)
		Rem_ZHnsp,			// ������� ���.�������� ZHome (����������)
		Rem_spdB,			// �������� ������� (����������)
		Rem_spdM,			// �������� ����� (����������)
		Rem_spdP;			// �������� �������� (����������)

	SComport *SPort;		// ��������� �� ���� ����������
		
	// �������� ������ ��������
	TTabSheet
		*TS_Pan;            // ������� ��� ��������
		
	TPanel
		*Pnl_Parent;
		
	TLabel
		*Lbl_Uni;
		
	TRadioButton			// �������� ������ ������/��������
		*RB_prd,
		*RB_prm;
		
	TGroupBox
		*GB_Main,
		*GB_Spd;
			
	TEdit					//�������� ��
		*Edt_Zad_type,		// ������� ���� �������� (�� �����������)
		*Edt_Zad_pos,		// ������� ��������� (�� �����������)
		*Edt_Zad_spd,		// ������� �������� (�� �����������)
		*Edt_Tek_pos,		// ������� ��������� (�� �����������)
		
		*Edt_Tek_acc,		// ������� ��������� (�� �����������)
		*Edt_Tek_dec,		// ������� ���������� (�� �����������)
		*Edt_Tek_ZHspd,		// ������� �������� ZHome (�� �����������)
		*Edt_Tek_ZHacc,		// ������� ��������� ZHome (�� �����������)
		*Edt_Tek_ZHnsp,		// ������� ���.�������� ZHome (�� �����������)
		
		*Edt_Rem_acc,		// ������� ��������� (����������)
		*Edt_Rem_dec,		// ������� ���������� (����������)
		*Edt_Rem_ZHspd,		// ������� �������� ZHome (����������)
		*Edt_Rem_ZHacc,		// ������� ��������� ZHome (����������)	
		*Edt_Rem_ZHnsp,		// ������� ���.�������� ZHome (����������)
		
		*Edt_Vvod_acc,		// ������� ��������� (����)
		*Edt_Vvod_dec,		// ������� ���������� (����)
		*Edt_Vvod_ZHspd,	// ������� �������� ZHome (����)
		*Edt_Vvod_ZHacc,	// ������� ��������� ZHome (����)
		*Edt_Vvod_ZHnsp,	// ������� ���.�������� ZHome (����)
		
		*Edt_Rem_spdB,		// �������� ������� (����������)
		*Edt_Rem_spdM,		// �������� ����� (����������)
		*Edt_Rem_spdP,		// �������� �������� (����������)
		
		*Edt_Vvod_spdB,		// �������� ������� (����)
		*Edt_Vvod_spdM,		// �������� ����� (����)
		*Edt_Vvod_spdP;		// �������� �������� (����)
		
	TButton
		*Btn_Vvod_Par,		// ������ ������ ��������
		*Btn_Vvod_Spd;		// ������ ������ ���������

	//void VIDK(unsigned char,int,unsigned char,bool,unsigned int); // ������ ������
	void AZdrive_Gen(); 								// �������� ��������
	unsigned char AZ_manage( unsigned int );		// ������� ����� � ���������
	void AZ_FrmZap( );								// ������������ �������
	void AZ_ChkRep( );								// ��������� ������
	unsigned int AZ_getKS( unsigned char*,int );		// ������ ����������� �����
	void __fastcall ChkVVdata(TObject *Sender); 		// �������� �������� ����������
	void __fastcall SetPar(TObject *Sender); 			// ���� ��������
	void __fastcall SetSpd(TObject *Sender); 			// ���� ���������
};

	void Visual_AZdrive(); // �������� ����������� �������� ������������ �������
	void AZdrive_Save(); // ���������� ������	
	void AZdrive_Load(); // �������� ������
    // void AZdrive_Time(); // ��������� ���������

#define DRIVE_COUNT 1
	
SAZ_drive *AZ_drive[DRIVE_COUNT];

//------------------------------------------------------------------------------
// ���������� �������� ���������
char *loc_data1_udb = "Data\\data1.udb"; // ���� 1 ����� 

struct DATA_FILE1	// ���������
{
    int data_par[DRIVE_COUNT][5];
    int data_spd[DRIVE_COUNT][3];
};

DATA_FILE1 data_1;
//------------------------------------------------------------------------------
#endif

	
	
	
	
