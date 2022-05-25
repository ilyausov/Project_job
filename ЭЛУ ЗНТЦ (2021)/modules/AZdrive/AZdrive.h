#ifndef AZdriveH
#define AZdriveH
//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
// ���� �������� (������������� ����� � ����������� ���.61)
#define ABS 1	// �������� � ���������� �������
#define OTN 2	// �������� �� ������������� ����
#define CON 7	// "�����������" �������� �� ���. ��������
#define ABS_F 10	// �������� � ������ ����. (��� ������.)
#define ABS_R 11	// �������� � �������� ����. (��� ������.)

extern TForm1 *Form1;
		
	// Acom : 	0 - ������ ��������� ( ������� ��������� )
	//			1 - ������ ��������� ( ���, ���������, ��������, ���������, ����������, current, delay, link, next data )
	//			2 - ������ ��������� ( ���, ���������, ��������.. )
	//			3 - ������ ��������� ( ���������, ���������� ) - �/�
	//			4 - ������ ��������� ( ���������, ���������� ) - �/�
	//			5 - ������ ��������� ( ZHOME - ��������, ���������, ���������� )
	//			6 - ������ ��������� ( ZHOME - ��������, ���������, ���������� )
	//			7 - ����� �0 - �/�
	//			8 - ���� �0 - �/�
	//			9 - ������ ��������� ( link, next data ) - �/�
	//			10 - ������ ��������� ( link, next data ) - �/�
/*	
unsigned char AZ_Req_Buf_1[8] =
{     //0   1    2    3    4    5    6    7    8    9   10    11   12   13   14   15   16   17   18
      0xAA,0x03,0x00,0xC6,0x00,0x02,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 0
      0xAA,0x10,0x18,0x00,0x00,0x12,0x24,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,	// 1
      0xAA,0x03,0x18,0x00,0x00,0x12,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2
      0xAA,0x10,0x18,0x06,0x00,0x04,0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0xCC,0xCC,0x00,0x00,	// 3
      0xAA,0x03,0x18,0x06,0x00,0x04,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 4
      0xAA,0x10,0x02,0xb0,0x00,0x06,0x0C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,	// 5
      0xAA,0x03,0x02,0xb0,0x00,0x06,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 6
      0xAA,0x10,0x00,0x7c,0x00,0x02,0x04,0x00,0x00,0x00,0x08,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,	// 7
      0xAA,0x10,0x00,0x7c,0x00,0x02,0x04,0x00,0x00,0x00,0x00,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,	// 8
      0xAA,0x10,0x18,0x0D,0x00,0x04,0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0xCC,0xCC,0x00,0x00,	// 9
      0xAA,0x03,0x18,0x0D,0x00,0x04,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00	// 10
};
*/
unsigned char AZ_Req_Buf_0[6] =
{0xAA,0x03,0x00,0xC6,0x00,0x02};
	
unsigned char AZ_Req_Buf_1[43] =
{0xAA,0x10,0x18,0x00,0x00,0x12,0x24,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01};
//0    1	2	 3	  4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34   35   36   37   38   39   40   41   42      
unsigned char AZ_Req_Buf_2[6] =
{0xAA,0x03,0x18,0x00,0x00,0x12};

unsigned char AZ_Req_Buf_5[19] =
{0xAA,0x10,0x02,0xb0,0x00,0x06,0x0C,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01};
	
unsigned char AZ_Req_Buf_6[6] =
{0xAA,0x03,0x02,0xb0,0x00,0x06};

	// ������
	//0	   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16
	//0xAA 0x03 0x04 0xdd 0xdd 0xdd 0xdd 0xCC 0xCC
	//0xAA 0x10 0x18 0x00 0x00 0x06 0xCC 0xCC
	//0xAA 0x03 0x0� 0xdd 0xdd 0xdd 0xdd 0xDD 0xDD 0xDD 0xDD 0xdd 0xdd 0xdd 0xdd 0xCC 0xCC
	
// ������ data:
//	 0 - 0x1800
//	 1 - 0x1840
//	 2 - 0x1880
//	   ..
//	255 - 0x57C0
	
struct DATA_MECH	// ���������
{
	int set_mech[5]; // ��������� ����������
	// 0 - ���������
	// 1 - ����������
	// 2 - �������� ZHome
	// 3 - �����./����. ZHome
	// 4 - ���.�������� ZHome
	int v_mech[3];	// ��������
	// 0 - ���, 1 - �����, 2 - ����.	
};
	
struct SAZ_drive
{
	unsigned char adr;		// ����� ���������
	String name;            // �������� ��� �����������

	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		ACom,				// ������� �������������� ������
		ANum,				// ����� ��������������� �������
		Buf_len,			// ����� �������
		diagn_byte,        	// ����� ����� ������� ����������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������
		
	unsigned int
		wrap_len;			// ���-�� ���. �� ������ ��� ������.
			
	unsigned char			// ���������� VIDK
		*Kom_AZ,			// �������
		*Otv_AZ,			// �����
		*Type_AZ;			// ��� �������� 1-���, 2-���

	int
        *V_AZ,				// ��������
		*Put_AZ;			// ���� ��������

	bool
		*Home_AZ,			// ������� �������� � Home
		*Pr_AZ;				// ������� �����
			
	bool
		Data_errZH,			// ������� ������������ ������ ZHome
		Data_err[256];	// ������� ������������ ������
		// ���, ����, ��������
		// ���������, ����������
		// link, next data
	
	int Rem_pos,			// ���������� �������� ���� (��� ������)
		Del_pos,			// ������� ����� (��� ������)
        *Abs_pos,			// ������������� ���� (��� ������)
		*Otn_pos;			// ������������� ���� (��� ������)
	
	/*
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
		Rem_spdP;			// �������� �������� (����������) */
		
	int Zad_Op_data[256][9],
        Tek_Op_data[256][9];

    unsigned char
        Max_Op_Data;        // ���������� ����� ��� ������

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
		
	TStringGrid
		*Data_Table;

	DATA_MECH data_mech;

	// ���������� �������� ���������
	char *loc_data_mech; // ���� ����� 

	//void VIDK(unsigned char,int,unsigned char,bool,unsigned int); // ������ ������
	void AZdrive_Gen(); 								// �������� ��������
	unsigned char AZ_manage( unsigned int );		// ������� ����� � ���������
    void AZ_ZapQuery();                             // ����������� ������
	void AZ_FrmZap(unsigned char);					// ������������ �������
	void AZ_ChkRep(unsigned char);								// ��������� ������
	unsigned int AZ_getKS( unsigned char*,int );		// ������ ����������� �����
	void __fastcall ChkVVdata(TObject *Sender); 		// �������� �������� ����������
	void __fastcall SetPar(TObject *Sender); 			// ���� ��������
	void __fastcall SetSpd(TObject *Sender); 			// ���� ���������
};

    void Visual_AZdata(); // �������� ������� ��������
	void Visual_AZdrive(); // �������� ����������� �������� ������������ �������
	void AZdrive_Save(); // ���������� ������	
	void AZdrive_Load(); // �������� ������
    //void AZdrive_Time(); // ��������� ���������

#define DRIVE_COUNT 5
SAZ_drive *AZ_drive[DRIVE_COUNT];
//------------------------------------------------------------------------------
#endif



	
	
	
	
