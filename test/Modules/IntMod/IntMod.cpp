//---------------------------------------------------------------------------
#pragma hdrstop
#include "IntMod.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
//-- ��������� �������� --//
//---------------------------------------------------------------------------
void IM_Gen(unsigned char Mod_Count)
{
	IntMod[0]->TS_Pan = new TTabSheet(IntMod[0]->SPort->PC_Com);
	IntMod[0]->TS_Pan->PageControl = IntMod[0]->SPort->PC_Com;
	IntMod[0]->TS_Pan->Caption = "����� �������/����������";
	IntMod[0]->TS_Pan->TabVisible = true;

	IntMod[0]->Pnl_Parent = new TPanel(IntMod[0]->TS_Pan);
	IntMod[0]->Pnl_Parent->Parent = IntMod[0]->TS_Pan;
	IntMod[0]->Pnl_Parent->Caption = "";
	//Pnl_Parent->BevelKind = bkFlat;
    IntMod[0]->Pnl_Parent->Color = clSilver;
	IntMod[0]->Pnl_Parent->BevelOuter = bvRaised;
	IntMod[0]->Pnl_Parent->BorderStyle = bsNone;
	IntMod[0]->Pnl_Parent->Height = 676;
	IntMod[0]->Pnl_Parent->Width = 1176;
    IntMod[0]->Pnl_Parent->DoubleBuffered = true;
	
	IntMod[0]->RB_prd = new TRadioButton(IntMod[0]->Pnl_Parent);
	IntMod[0]->RB_prd->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->RB_prd->Top = 18;
	IntMod[0]->RB_prd->Left = 18;
	IntMod[0]->RB_prd->Height = 14;
	IntMod[0]->RB_prd->Width = 14;
	IntMod[0]->RB_prd->Caption = "";
	
	IntMod[0]->RB_prm = new TRadioButton(IntMod[0]->Pnl_Parent);
	IntMod[0]->RB_prm->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->RB_prm->Top = 18;
	IntMod[0]->RB_prm->Left = 58;
	IntMod[0]->RB_prm->Height = 14;
	IntMod[0]->RB_prm->Width = 14;
	IntMod[0]->RB_prm->Caption = "";

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 80;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "������ �������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 112;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "������ �������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 144;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "������ �������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 176;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "��������� �����";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 208;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "��������� ����";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 240;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "������ � Home";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 272;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "������ �����������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 304;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "���������� ������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 336;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "�� ������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    IntMod[0]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	IntMod[0]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	IntMod[0]->Lbl_Uni->Top = 368;
	IntMod[0]->Lbl_Uni->Left = 32;
	IntMod[0]->Lbl_Uni->Caption = "�� ������";
	IntMod[0]->Lbl_Uni->Font->Name = "Arial";
	IntMod[0]->Lbl_Uni->Font->Size = 12;
	IntMod[0]->Lbl_Uni->Font->Color = clBlack;
	IntMod[0]->Lbl_Uni->Transparent = true;
	IntMod[0]->Lbl_Uni->Height = 19;
	IntMod[0]->Lbl_Uni->Width = 180;
	IntMod[0]->Lbl_Uni->Layout = tlTop;
	IntMod[0]->Lbl_Uni->Font->Style = IntMod[0]->Lbl_Uni->Font->Style >> fsBold;

    for(int i=0;i<Mod_Count;i++)
	{
		for(int j=0;j<IMBits_COUNT;j++)
		{
            IntMod[i]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	        IntMod[i]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	        IntMod[i]->Lbl_Uni->Top = 55;
	        IntMod[i]->Lbl_Uni->Left = 230 + i * 100;
	        IntMod[i]->Lbl_Uni->Caption = "KOM";
	        IntMod[i]->Lbl_Uni->Font->Name = "Arial";
	        IntMod[i]->Lbl_Uni->Font->Size = 9;
	        IntMod[i]->Lbl_Uni->Font->Color = clBlack;
	        IntMod[i]->Lbl_Uni->Transparent = true;
	        IntMod[i]->Lbl_Uni->Height = 19;
	        IntMod[i]->Lbl_Uni->Width = 26;
	        IntMod[i]->Lbl_Uni->Layout = tlTop;
	        IntMod[i]->Lbl_Uni->Font->Style = IntMod[i]->Lbl_Uni->Font->Style >> fsBold;

            IntMod[i]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	        IntMod[i]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	        IntMod[i]->Lbl_Uni->Top = 55;
	        IntMod[i]->Lbl_Uni->Left = 230 + 32 + i * 100;
	        IntMod[i]->Lbl_Uni->Caption = "OTV";
	        IntMod[i]->Lbl_Uni->Font->Name = "Arial";
	        IntMod[i]->Lbl_Uni->Font->Size = 9;
	        IntMod[i]->Lbl_Uni->Font->Color = clBlack;
	        IntMod[i]->Lbl_Uni->Transparent = true;
	        IntMod[i]->Lbl_Uni->Height = 19;
	        IntMod[i]->Lbl_Uni->Width = 26;
	        IntMod[i]->Lbl_Uni->Layout = tlTop;
	        IntMod[i]->Lbl_Uni->Font->Style = IntMod[i]->Lbl_Uni->Font->Style >> fsBold;

			IntMod[i]->Img_KomIM[j] = new TImage(IntMod[0]->Pnl_Parent);
			IntMod[i]->Img_KomIM[j] -> Parent = IntMod[0]->Pnl_Parent;
			IntMod[i]->Img_KomIM[j] -> Top = 77 + 32 * j;
			IntMod[i]->Img_KomIM[j] -> Left = 230 + i * 100;
			IntMod[i]->Img_KomIM[j] -> Height = 25;
			IntMod[i]->Img_KomIM[j] -> Width = 25;
			IntMod[i]->Img_KomIM[j] -> Picture= Form1->check0->Picture;
			IntMod[i]->Img_KomIM[j] -> Transparent = true;
			IntMod[i]->Img_KomIM[j] -> Hint = IntToStr(j);
			IntMod[i]->Img_KomIM[j] -> OnClick = IntMod[i]->IMSetKom;
			IntMod[i]->Img_KomIM[j] -> OnDblClick = IntMod[i]->IMSetKom;
			
			IntMod[i]->Img_OtvIM[j] = new TImage(IntMod[0]->Pnl_Parent);
			IntMod[i]->Img_OtvIM[j] -> Parent = IntMod[0]->Pnl_Parent;
			IntMod[i]->Img_OtvIM[j] -> Top = 77 + 32 * j;
			IntMod[i]->Img_OtvIM[j] -> Left = 230 + 32 + i * 100;
			IntMod[i]->Img_OtvIM[j] -> Height = 25;
			IntMod[i]->Img_OtvIM[j] -> Width = 25;
			IntMod[i]->Img_OtvIM[j] -> Picture= Form1->check0->Picture;
			IntMod[i]->Img_OtvIM[j] -> Transparent = true;
			IntMod[i]->Img_OtvIM[j] -> Hint = IntToStr(j);
			IntMod[i]->Img_OtvIM[j] -> OnClick = IntMod[i]->IMSetOtv;
			IntMod[i]->Img_OtvIM[j] -> OnDblClick = IntMod[i]->IMSetOtv;

            IntMod[i]->Lbl_Uni = new TLabel(IntMod[0]->Pnl_Parent);
	        IntMod[i]->Lbl_Uni->Parent = IntMod[0]->Pnl_Parent;
	        IntMod[i]->Lbl_Uni->Top = 335 + 64;
	        IntMod[i]->Lbl_Uni->Left = 225 + i * 100;
	        IntMod[i]->Lbl_Uni->Caption = "������ " + IntToStr(IntMod[i]->adr);
	        IntMod[i]->Lbl_Uni->Font->Name = "Arial";
	        IntMod[i]->Lbl_Uni->Font->Size = 12;
	        IntMod[i]->Lbl_Uni->Font->Color = clBlack;
	        IntMod[i]->Lbl_Uni->Transparent = true;
	        IntMod[i]->Lbl_Uni->Height = 19;
	        IntMod[i]->Lbl_Uni->Width = 80;
	        IntMod[i]->Lbl_Uni->Layout = tlTop;
	        IntMod[i]->Lbl_Uni->Font->Style = IntMod[i]->Lbl_Uni->Font->Style >> fsBold;
		}
	}
}
//-------------------------------------//
//----- ��������� ������ -----//
//-------------------------------------//
void __fastcall SIntMod::IMSetKom(TObject *Sender)
{
	unsigned int j = StrToInt(((TImage*)Sender)->Hint);
    if(*Kom_IM & int(pow(2,j)))
         *Kom_IM &= (~int(pow(2,j)));
    else
         *Kom_IM |= int(pow(2,j));
}
//-------------------------------------//
//----- ��������� ������ -----//
//-------------------------------------//
void __fastcall SIntMod::IMSetOtv(TObject *Sender)
{
	if(!pr_otl) return;

    unsigned int j = StrToInt(((TImage*)Sender)->Hint);
    if(*Otv_IM & int(pow(2,j)))
         *Otv_IM &= (~int(pow(2,j)));
    else
         *Otv_IM |= int(pow(2,j));
}
//-------------------------------------//
//----- ������������ ������� -----//
//-------------------------------------//
void SIntMod::IM_FrmZap( )
{
    if(Type_Im) // ������ ����������
    {
        IM_temp_str = "";

		IM_temp_str = '>';
		IM_temp_str += IntToHex(adr,1);
        IM_temp_str += IntToHex(char((*Kom_IM & 0xF000) >> 12),1);
        IM_temp_str += IntToHex(char((*Kom_IM & 0xF00) >> 8),1);
		IM_temp_str += IntToHex(char((*Kom_IM & 0xF0) >> 4),1);
        IM_temp_str += IntToHex(char(*Kom_IM & 0x0F),1);

        for(int i=1;i<=IM_temp_str.Length();i++)
		SPort->PackOut[i-1] = IM_temp_str[i];

		get_summ_IM(SPort->PackOut);
	
        SPort->PackOut[8] = 13;
	    SPort->PackOut[9] = 10;
	    Buf_len = 10;
	}
    else    // ����� ������
    {
        IM_temp_str = "";

		IM_temp_str = '<';
		IM_temp_str += IntToHex(adr,1);
        IM_temp_str += IntToHex(char((*Otv_IM & 0xF000) >> 12),1);
        IM_temp_str += IntToHex(char((*Otv_IM & 0xF00) >> 8),1);
		IM_temp_str += IntToHex(char((*Otv_IM & 0xF0) >> 4),1);
        IM_temp_str += IntToHex(char(*Otv_IM & 0x0F),1);

        for(int i=1;i<=IM_temp_str.Length();i++)
		SPort->PackOut[i-1] = IM_temp_str[i];

		get_summ_IM(SPort->PackOut);
	
        SPort->PackOut[8] = 13;
	    SPort->PackOut[9] = 10;
	    Buf_len = 10;
    }
}
//---------------------------------------------------------------------------
//-- ��������� ������ --//
//---------------------------------------------------------------------------
bool SIntMod::IM_ChkRep()
{
for(int i=0;i<PACKAGE_COUNT-10;i++)
{
        if(Type_Im) // ���������
        {
                if( (SPort->PackIn[i-9]== '<') &&
                (SPort->PackIn[i-8] == (48 + adr))&&
			    (SPort->PackIn[i]==10)&&
			    (SPort->PackIn[i-1]==13))
		    {
                                if(isxdigit(SPort->PackIn[i-8])&&isxdigit(SPort->PackIn[i-7])&&isxdigit(SPort->PackIn[i-6])&&isxdigit(SPort->PackIn[i-5])&&isxdigit(SPort->PackIn[i-4]))
                                {
				        if(chk_summ_IM(SPort->PackIn,i-8))
                                        {
                                                        IM_char_tmp[0] = '0';
					                IM_char_tmp[1] = 'x';
					                IM_char_tmp[2] = SPort->PackIn[i-7];
					                IM_char_tmp[3] = SPort->PackIn[i-6];
					                IM_char_tmp[4] = SPort->PackIn[i-5];
					                IM_char_tmp[5] = SPort->PackIn[i-4];
					                *Otv_IM = strtol(IM_char_tmp,NULL,16);
                                                        return 1;
				        }
                               }
                }
        }
        else    // ������
        {
            if( (SPort->PackIn[i-9]== '>') &&
                (SPort->PackIn[i-8] == (48 + adr))&&
			    (SPort->PackIn[i]==10)&&
			    (SPort->PackIn[i-1]==13))
		    {
				SPort->Port.ResetRB();

                                if(isxdigit(SPort->PackIn[i-8])&&isxdigit(SPort->PackIn[i-7])&&isxdigit(SPort->PackIn[i-6])&&isxdigit(SPort->PackIn[i-5])&&isxdigit(SPort->PackIn[i-4]))
                                {
				        if(chk_summ_IM(SPort->PackIn,i-8))
				        {
                                        
                                                IM_char_tmp[0] = '0';
					        IM_char_tmp[1] = 'x';
					        IM_char_tmp[2] = SPort->PackIn[i-7];
					        IM_char_tmp[3] = SPort->PackIn[i-6];
					        IM_char_tmp[4] = SPort->PackIn[i-5];
					        IM_char_tmp[5] = SPort->PackIn[i-4];
					        *Kom_IM = strtol(IM_char_tmp,NULL,16);
					        return 1;
                                        }
				}
                }
            }
}
return 0;
}
//---------------------------------------------------------------------------
void get_summ_IM(char* buf)
{
        IM_KS = 0;
	for(int j=1;j<=5;j++)
	{
                IM_tmp = "0x";
		IM_tmp = IM_tmp + char(buf[j]);
		IM_KS += StrToInt(IM_tmp);
	}
	IM_KS = (unsigned int)(0x100-(IM_KS & 0xff));
	itoa(IM_KS,IM_str,16);
	buf[6] = IM_str[0];
	buf[7] = IM_str[1];
}
//---------------------------------------------------------------------------
bool chk_summ_IM(char* buf,unsigned char n_pos)
{
	IM_KS = 0;
	for(int k=n_pos;(k<=n_pos+4)&&(isxdigit(buf[k]));k++)
	{
                IM_tmp = "0x";
		IM_tmp = IM_tmp + char(buf[k]);
		IM_KS += StrToInt(IM_tmp);
	}
	IM_KS = (unsigned int)(0x100-(IM_KS & 0xff));
	itoa(IM_KS,IM_str,16);
	if((buf[n_pos+5] == IM_str[0])&&(buf[n_pos+6] == IM_str[1])) return 1;
	else return 0;
}
//---------------------------------------------------------------------------
//--��������� ������� � ������������--//
//---------------------------------------------------------------------------
unsigned char SIntMod::IM_manage( unsigned int SH )
{
	// ������������ ������������� �� ��������� ������������� ��� �� ������
	IntMod[0]->RB_prd->Checked = !SH;
    IntMod[0]->RB_prm->Checked = SH;

    if(Type_Im) // ������ ����������
    {
        if(SH == 0) // ������� �������
	    {
		    SPort->PackageClear();
		    // ������� ������������ �������
            IM_FrmZap();

		    SPort->VisPackASCII(0);

		    // ������� ������ �����
		    SPort->Port.ResetRB();
		    // �������� �������
		    SPort->Port.Write(SPort->PackOut,int(Buf_len));
		    // ������� �� ��������� ���
		    SPort->DevState++;
		    // ��������� �������
		    SPort->Dev_Timer = 0;
		    return 0;
	    }
	    else if(SH == 1)
	    {

		    // ������ ������
		    SPort->Port.Read(SPort->PackIn,PACKAGE_COUNT-28);
		    // �������� ������

		    if(IM_ChkRep())
		    {
                SPort->VisPackASCII(1);
			    // ������� �� ��������� ���
			    SPort->DevState++;
			    // ����� �������� ������ �����
			    Err = 0;
			    return 0;
		    }
		    else
		    {
                if(SPort->Dev_Timer < SPort->Dev_TK) return 0; // ���� ������
			    // �� ���������
			    SPort->DevState++; // ��������� � ���������� ����������
			    // ���������� �������� ������ ����� � ��������� �� ��������
                            if(Err <= Max_err) Err++;
			    if((Err) > Max_err)
				    return 1;
			    return 0;
		    }
        }
    }
    else
    {
        if(SH == 0)
	    {
		    // ������ �������
		    SPort->Port.Read(SPort->PackIn,PACKAGE_COUNT-28);
		    // �������� ������

		    if(IM_ChkRep())
		    {
                                SPort->VisPackASCII(1);
				SPort->PackageClear();
			    SPort->DevState++;
			    Err = 0;
			    return 0;
		    }
			// ���������� �������� ������ ����� � ��������� �� ��������
			if(Err <= Max_err) Err++;
                        if((Err) > Max_err)
				return 1;
			return 0;
        }
        else if(SH == 1) // ������� ������
	    {
		    SPort->PackageClear();
            IM_FrmZap();
		    SPort->VisPackASCII(0);
			SPort->Port.Write(SPort->PackOut,int(Buf_len));
		    SPort->DevState++;
		    return 0;
	    }
    }
}
//---------------------------------------------------------------------------
void Visual_IM()       // �������� ��������
{
    for(int i=0;i<IntMod_COUNT;i++)
        for(int j=0;j<IMBits_COUNT;j++)
        {
            if(*IntMod[i]->Kom_IM & int(pow(2,j)))
                IntMod[i]->Img_KomIM[j] -> Picture= Form1->check1->Picture;
            else
                IntMod[i]->Img_KomIM[j] -> Picture= Form1->check0->Picture;

            if(*IntMod[i]->Otv_IM & int(pow(2,j)))
                IntMod[i]->Img_OtvIM[j] -> Picture= Form1->check1->Picture;
            else
                IntMod[i]->Img_OtvIM[j] -> Picture= Form1->check0->Picture;
        }
}
//---------------------------------------------------------------------------
//-- ������� ������������� �������� --//
//---------------------------------------------------------------------------
void Init_SIntMod()
{
	for(int i=0;i<IntMod_COUNT;i++ )
        IntMod[i] = new SIntMod();

	// ������ 1
	IntMod[0]->adr = 0x01;
	IntMod[0]->Type_Im = 0;				// ���: 1-������./0 - ������
	IntMod[0]->Buf_len = 0;
	IntMod[0]->diagnS_byte = 2;        	// ����� ����� ������� �����������
	IntMod[0]->diagnS_mask = 0x10;		// ����� ����� ������� �����������
    IntMod[0]->Err = 0;
	if(IntMod[0]->Type_Im)
		IntMod[0]->Max_err = 5;
	else
		IntMod[0]->Max_err = 30;
	IntMod[0]->Kom_IM = &KOM_MOD;
	IntMod[0]->Otv_IM = &OTVET_MOD;
	IntMod[0]->SPort = Comport[2];
	
	IM_Gen(IntMod_COUNT);
}
//---------------------------------------------------------------------------