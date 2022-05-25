//---------------------------------------------------------------------------
#include "MERA.h"
//---------------------------------------------------------------------------
//-- ��������� �������� --//
//---------------------------------------------------------------------------
void SMERA_dat::DatMERA_Gen()
{
	TS_Pan = new TTabSheet(SPort->PC_Com);
	TS_Pan->PageControl = SPort->PC_Com;
	TS_Pan->Caption = name;
	TS_Pan->TabVisible = true;

	Pnl_Parent = new TPanel(TS_Pan);
	Pnl_Parent->Parent = TS_Pan;
	Pnl_Parent->Caption = "";
	//Pnl_Parent->BevelKind = bkFlat;
    Pnl_Parent->Color = clSilver;
	Pnl_Parent->BevelOuter = bvRaised;
	Pnl_Parent->BorderStyle = bsNone;
	Pnl_Parent->Height = 676;
	Pnl_Parent->Width = 1176;

	Lbl_Adr = new TLabel(Pnl_Parent);
	Lbl_Adr->Parent = Pnl_Parent;
	Lbl_Adr->Top = 15;
	Lbl_Adr->Left = 26;
	Lbl_Adr->Caption = "����� ����������: " + adr;
	Lbl_Adr->Font->Name = "Arial";
	Lbl_Adr->Font->Size = 12;
	Lbl_Adr->Font->Color = clBlack;
	Lbl_Adr->Font->Style = Lbl_Adr->Font->Style << fsBold;
	Lbl_Adr->Transparent = true;
	Lbl_Adr->Height = 19;
	Lbl_Adr->Width = 190;
	Lbl_Adr->Layout = tlTop;
	
	RB_prd = new TRadioButton(Pnl_Parent);
	RB_prd->Parent = Pnl_Parent;
	RB_prd->Top = 18;
	RB_prd->Left = 238;
	RB_prd->Height = 14;
	RB_prd->Width = 14;
	RB_prd->Caption = "";
	
	RB_prm = new TRadioButton(Pnl_Parent);
	RB_prm->Parent = Pnl_Parent;
	RB_prm->Top = 18;
	RB_prm->Left = 278;
	RB_prm->Height = 14;
	RB_prm->Width = 14;
	RB_prm->Caption = "";
		
	GB_Main = new TGroupBox(Form1);
	GB_Main->Parent = Pnl_Parent;
	GB_Main->Top = 56;
	GB_Main->Left = 26;
	GB_Main->Height = 90;
	GB_Main->Width = 385;
	GB_Main->Caption = " ������� ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;
	
	Lbl_Zap1 = new TLabel(GB_Main);
	Lbl_Zap1->Parent = GB_Main;
	Lbl_Zap1->Top = 40;
	Lbl_Zap1->Left = 28;
	Lbl_Zap1->Caption = "��������� �������:";
	Lbl_Zap1->Font->Name = "Arial";
	Lbl_Zap1->Font->Size = 12;
	Lbl_Zap1->Font->Color = clBlack;
	Lbl_Zap1->Transparent = true;
	Lbl_Zap1->Height = 19;
	Lbl_Zap1->Width = 190;
	Lbl_Zap1->Layout = tlTop;
	Lbl_Zap1->Font->Style = Lbl_Zap1->Font->Style >> fsBold;
		
	Edt_Zap1 = new TEdit(GB_Main);
	Edt_Zap1->Parent = GB_Main;
	Edt_Zap1->Top = 35;
	Edt_Zap1->Left = 270;
	Edt_Zap1->Font->Name = "Arial";
	Edt_Zap1->Font->Size = 13;
	Edt_Zap1->Font->Color = clBlack;
	Edt_Zap1->Font->Style = Edt_Zap1->Font->Style >> fsBold;
	Edt_Zap1->BevelKind = bkFlat;
	Edt_Zap1->BevelOuter = bvRaised;
	Edt_Zap1->BorderStyle = bsNone;
	Edt_Zap1->ReadOnly = true;
	Edt_Zap1->Color = clSkyBlue;
	Edt_Zap1->Height = 26;
	Edt_Zap1->Width = 85;
}
//---------------------------------------------------------------------------
double SMERA_dat::raspakMERA()				// ���������� ������
{
	char strB1[] = {0,0,0};
	char strB2[] = {0,0,0};
	char strB3[] = {0,0,0};
	char strB4[] = {0,0,0};

	strB1[0] =  SPort->PackIn[11];
	strB1[1] =  SPort->PackIn[12];
	strB2[0] =  SPort->PackIn[13];
	strB2[1] =  SPort->PackIn[14];
	strB3[0] =  SPort->PackIn[7];
	strB3[1] =  SPort->PackIn[8];
	strB4[0] =  SPort->PackIn[9];
	strB4[1] =  SPort->PackIn[10];

	int b1,b2,b3,b4;
	double f = 0;
	long Pa;
	int st;
	int m1 = 0,m2 = 0,m3 = 0;
	double p = 0,p0 = 0,p1 = 0,p2 = 0,p3 = 0,p4 = 0,p5 = 0,p6 = 0,p7 = 0;
	double       p8 = 0,p9 = 0,p10 = 0,p11 = 0,p12 = 0,p13 = 0,p14 = 0,p15 = 0;
	double       p16 = 0,p17 = 0,p18 = 0,p19 = 0,p20 = 0,p21 = 0,p22 = 0;
	char *string = "0000",*endptr;
	strcpy(string,strB1);
	b1=strtol(string,&endptr,16);

	strcpy(string,strB2);
	b2=strtol(string,&endptr,16);

	strcpy(string,strB3);
	b3=strtol(string,&endptr,16);

	strcpy(string,strB4);
	b4=strtol(string,&endptr,16);

	Pa = (b1*256) + b2;

	if(Pa)
	{
		Pa = Pa << 1;
		st = Pa/256;
		st = st - 127;

		m1 = (b2*256) + b3;
		m1 = m1 << 1;
   
		p = pow(2,st);
   
		if(m1 & 0x8000)
			{p22 = pow(2,st-1);}
		if(m1 & 0x4000)
			{p21 = pow(2,st-2);}
		if(m1 & 0x2000)
			{p20 = pow(2,st-3);}
		if(m1 & 0x1000)
			{p19 = pow(2,st-4);}
		if(m1 & 0x0800)
			{p18 = pow(2,st-5);}
		if(m1 & 0x0400)
			{p17 = pow(2,st-6);}
		if(m1 & 0x0200)
			{p16 = pow(2,st-7);}
		if(m1 & 0x0100)
			{p15 = pow(2,st-8);}
   
    	if(m2 & 0x0080)
			{p14 = pow(2,st-9);}
		if(m2 & 0x0040)
			{p13 = pow(2,st-10);}
		if(m2 & 0x0020)
			{p12 = pow(2,st-11);}
		if(m2 & 0x0010)
			{p11 = pow(2,st-12);}
		if(m2 & 0x0008)
			{p10 = pow(2,st-13);}
		if(m2 & 0x0004)
			{p9 = pow(2,st-14);}
		if(m2 & 0x0002)
			{p8 = pow(2,st-15);}
		if(m2 & 0x0001)
			{p7 = pow(2,st-16);}  

		f = p + p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7+
		p8 + p9 + p10 + p11 + p12 + p13 + p14 + p15 + p16+
		p17 + p18 + p19 + p20 + p21 + p22;
	}
	return(f);	
}
//---------------------------------------------------------------------------
//-- ������� ������������ ������� --//
//---------------------------------------------------------------------------
void SMERA_dat::DatMERA_FrmZap(bool Zap_type)
{
	String temp_str = "";

	if(Zap_type)    // ������ ������
	{
		//
	}
	else
	{
		temp_str = ":";
		temp_str += adr;
		temp_str += "0300000002";
	}

	for(int i=1;i<=temp_str.Length();i++)
		SPort->PackOut[i-1] = temp_str[i];

	unsigned int CRC = 0;
	for(int i=1;i<12;i=i+2)
	{
		AnsiString tmp = "0x";
        tmp = tmp + char(SPort->PackOut[i]);
        tmp = tmp + char(SPort->PackOut[i+1]);
		CRC += StrToInt(tmp);
	}
	CRC = (unsigned int)(0x100-(CRC & 0xff));
	char str[2];
	itoa(CRC,str,16);
	SPort->PackOut[13] = str[0];
	SPort->PackOut[14] = str[1];
	SPort->PackOut[15] = 13;
	SPort->PackOut[16] = 10;
	Buf_len = 17;
}
//---------------------------------------------------------------------------
//-- ������� ��������� ������ --//
//---------------------------------------------------------------------------
bool SMERA_dat::DatMERA_ChkRep(bool Zap_type)
{
	for(int i=0;i<PACKAGE_COUNT;i++)
	{
		if((SPort->PackIn[2]==adr[2])&&
			(SPort->PackIn[i]==10)&&
			(SPort->PackIn[i-1]==13))	// ���� ����� �������
		{
			if(Zap_type)    // ������ ������
			{
				//
			}
			else
			{
				double mera_result = raspakMERA();
				if(mera_result > 0.0)
				{
					*Pres_MERA=int(1000*(log10(mera_result/133.3)+6.0));
					Edt_Zap1->Text = FloatToStrF(133.3*pow(10,(float)*Pres_MERA/1000.0-6.0),ffExponent,3,8);
				}
			}
            //*Pr_Sv = 1;
			return 1;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
//-- ������� ����� � �������� --//
//---------------------------------------------------------------------------
bool SMERA_dat::DatMERA_Manage(unsigned int SH,bool Zap_type)
{
    RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

	if(!SH) // ������� �������
	{
		SPort->PackageClear();

		DatMERA_FrmZap(Zap_type);
		SPort->VisPackASCII(0);

		// ������� ������ �����
		SPort->Port.ResetRB();
		// �������� �������
		SPort->Port.Write(SPort->PackOut,Buf_len);
		// ������� �� ��������� ���
		SPort->DevState = 1;
		// ��������� �������
		SPort->Dev_Timer = 0;
		return 0;
	}
	else if(SH == 1)
	{
		// ?????? ??????
		SPort->Port.Read(SPort->PackIn,SPort->Port.GetRBSize());
		// ???????? ??????
		if(DatMERA_ChkRep(Zap_type))
		{
			SPort->VisPackASCII(1);
			// ??????? ?? ????????? ???
			SPort->DevState++;
			// ????? ???????? ?????? ?????
			Err = 0;
			return 0;
		}
		else
		{
            if(SPort->Dev_Timer < SPort->Dev_TK) return 0; // ???? ??????
			// ?? ?????????
			SPort->DevState++; // ????????? ? ?????????? ??????????
			// ?????????? ???????? ?????? ????? ? ????????? ?? ????????
			if((Err++) > Max_err)
				return 1;
			return 0;
		}
	}
}
//---------------------------------------------------------------------------
//-- ������������� �������� --//
//---------------------------------------------------------------------------
void Init_DatMERA()
{
	for(int i=0;i<DAT_MERA_COUNT;i++ )
		Dat_MERA[i] = new SMERA_dat();

	// ������ 1
	Dat_MERA[0]->name = "�1-����(���)";            // �������� ��� �����������
	Dat_MERA[0]->adr = "01";			// �����
	Dat_MERA[0]->Pres_MERA = &D_D1;      // ���������� �������� ��� ������
	Dat_MERA[0]->SPort = Comport[1];     // ����
	Dat_MERA[0]->diagnS_byte = 0;        // ����� ����� ������� �����������
	Dat_MERA[0]->diagnS_mask = 0x01;		// ����� ����� ������� �����������

	Dat_MERA[0]->Err = 0;				// ���-�� ������
	Dat_MERA[0]->Max_err = 5;			// �������� ������
	Dat_MERA[0]->ACom = 0;				// ������� �������������� ������
	//Dat_MERA[0]->RCom = 0;				// ������� ������ �����
	Dat_MERA[0]->Buf_len = 0;			// ����� �������
	Dat_MERA[0]->DatMERA_Gen();
	
	// ������ 2
	Dat_MERA[1]->name = "�5-���(���)";            // �������� ��� �����������
	Dat_MERA[1]->adr = "05";			// �����
	Dat_MERA[1]->Pres_MERA = &D_D5;      // ���������� �������� ��� ������
	Dat_MERA[1]->SPort = Comport[1];     // ����
	Dat_MERA[1]->diagnS_byte = 0;        // ����� ����� ������� �����������
	Dat_MERA[1]->diagnS_mask = 0x10;		// ����� ����� ������� �����������

	Dat_MERA[1]->Err = 0;				// ���-�� ������
	Dat_MERA[1]->Max_err = 5;			// �������� ������
	Dat_MERA[1]->ACom = 0;				// ������� �������������� ������
	//Dat_MERA[1]->RCom = 0;				// ������� ������ �����
	Dat_MERA[1]->Buf_len = 0;			// ����� �������
	Dat_MERA[1]->DatMERA_Gen();
	
	// ������ 3
	Dat_MERA[2]->name = "�2-�/�(���)";            // �������� ��� �����������
	Dat_MERA[2]->adr = "02";			// �����
	Dat_MERA[2]->Pres_MERA = &D_D2;      // ���������� �������� ��� ������
	Dat_MERA[2]->SPort = Comport[1];     // ����
	Dat_MERA[2]->diagnS_byte = 0;        // ����� ����� ������� �����������
	Dat_MERA[2]->diagnS_mask = 0x02;		// ����� ����� ������� �����������

	Dat_MERA[2]->Err = 0;				// ���-�� ������
	Dat_MERA[2]->Max_err = 5;			// �������� ������
	Dat_MERA[2]->ACom = 0;				// ������� �������������� ������
	//Dat_MERA[2]->RCom = 0;				// ������� ������ �����
	Dat_MERA[2]->Buf_len = 0;			// ����� �������
	Dat_MERA[2]->DatMERA_Gen();
}