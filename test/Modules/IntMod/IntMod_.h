//---------------------------------------------------------------------------
//--���������� ����������� ��� ������ �� RS-485--//
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
extern TForm1 *Form1;
	
	// �������
	// ">" "adr" "D" "D" "D" "D" "/cr" "/lf"

	// ������
	// "<" "adr" "D" "D" "D" "D" "/cr" "/lf"
	
#define IMBits_COUNT 8
	
struct SIntMod
{
	unsigned char adr;		// ����� ���������

	unsigned char
		Err,				// ���-�� ������
		Max_err,			// �������� ������
		Buf_len,			// ����� �������
		diagnS_byte,        // ����� ����� ������� �����������
		diagnS_mask;        // ����� ����� ������� �����������
		
	bool
		Type_Im;			// ���: 1-������./0 - ������
			
	unsigned int			// ����������
		*Kom_IM,			// �������
		*Otv_IM;			// �����       

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
			
	TImage
		*Img_KomIM[IMBits_COUNT],		//
		*Img_OtvIM[IMBits_COUNT];		//

	unsigned char IM_manage( unsigned int );		// ������� �����
	void IM_FrmZap( );								// ������������ �������
	bool IM_ChkRep( );								// ��������� ������
	void __fastcall IMSetKom(TObject *Sender); 		// �������� �������� ����������
	void __fastcall IMSetOtv(TObject *Sender); 		// �������� �������� ����������
};

    String IM_temp_str;
	char IM_char_tmp[6];
	unsigned int IM_KS = 0;
	String IM_tmp;
	char IM_str[2];

	void IM_Gen(unsigned char);						// �������� ��������	
	void Visual_IM();
	void get_summ_IM(char*);
	bool chk_summ_IM(char*,unsigned char);

#define IntMod_COUNT 1
	
SIntMod *IntMod[IntMod_COUNT];
//------------------------------------------------------------------------------



	
	
	
	
