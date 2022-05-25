//---------------------------------------------------------------------------
#include "BPM_HP.h"
//---------------------------------------------------------------------------
//-- ��������� �������� --//
//---------------------------------------------------------------------------
void S_BPM_HP::BU_IVE_Gen()
{
	TS_Pan = new TTabSheet(SPort->PC_Com);
	TS_Pan->PageControl = SPort->PC_Com;
    TS_Pan->Font->Size = 12;
	TS_Pan->Caption = name;
	TS_Pan->TabVisible = true;

	Pnl_Parent = new TPanel(TS_Pan);
	Pnl_Parent->Parent = TS_Pan;
	Pnl_Parent->Caption = "";
	//Pnl_Parent->BevelKind = bkFlat;
    Pnl_Parent->Color = clSilver;
	Pnl_Parent->BevelOuter = bvRaised;
	Pnl_Parent->BorderStyle = bsNone;
	Pnl_Parent->Height = 676;  // ��� FullHD
	// Pnl_Parent->Height = 634;  // ��� 1280x1024
	Pnl_Parent->Width = 1176;

	Lbl_Uni = new TLabel(Pnl_Parent);
	Lbl_Uni->Parent = Pnl_Parent;
	Lbl_Uni->Top = 15;
	Lbl_Uni->Left = 26;
	Lbl_Uni->Caption = "����� ����������: 00" + IntToStr(adr);
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style << fsBold;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	
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
	GB_Main->Height = 185;  // d - 241
	GB_Main->Width = 705;
	GB_Main->Caption = " ������� ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;

    GB_Err = new TGroupBox(Form1);
	GB_Err->Parent = Pnl_Parent;
	GB_Err->Top = 248;
	GB_Err->Left = 26;
	GB_Err->Width = 321;
	GB_Err->Height = 200;   // d - 448
	GB_Err->Caption = " ��������� ";
	GB_Err->Font->Name = "Arial";
	GB_Err->Font->Size = 12;
	GB_Err->Font->Color = clBlack;
	GB_Err->Font->Style = GB_Err->Font->Style << fsBold;
	
	GB_Par = new TGroupBox(Form1);
	GB_Par->Parent = Pnl_Parent;
	GB_Par->Top = 248;
	GB_Par->Left = 354;
	GB_Par->Width = 377;
	GB_Par->Height = 355;   // d - 603
	GB_Par->Caption = " �������������� ";
	GB_Par->Font->Name = "Arial";
	GB_Par->Font->Size = 12;
	GB_Par->Font->Color = clBlack;
	GB_Par->Font->Style = GB_Par->Font->Style << fsBold;

    GB_Dugo = new TGroupBox(Form1);
	GB_Dugo->Parent = Pnl_Parent;
	GB_Dugo->Top = 455;
	GB_Dugo->Left = 26;
	GB_Dugo->Width = 321;
	GB_Dugo->Height = 148;
	GB_Dugo->Caption = " ���������� ";
	GB_Dugo->Font->Name = "Arial";
	GB_Dugo->Font->Size = 12;
	GB_Dugo->Font->Color = clBlack;
	GB_Dugo->Font->Style = GB_Dugo->Font->Style << fsBold;
		
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 32;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "��������� ����� (DEL)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 56;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "��������� ����� (DEW)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 80;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "��������� ������������ (DEV)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
		
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 104;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "��������� ���������� (DEF)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 128;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "��������� �������� (DEP)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 32;
	Lbl_Uni->Left = 392;
	Lbl_Uni->Caption = "�������� � DPA ����./����";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 80;
	Lbl_Uni->Left = 392;
	Lbl_Uni->Caption = "����������� ���. ���. (OUT)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 104;
	Lbl_Uni->Left = 392;
	Lbl_Uni->Caption = "����������� UI/IT";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
		
	for(int i=0;i<9;i++)
	{
		CB_Zad[i] = new TCheckBox(GB_Main);
		CB_Zad[i]->Parent = GB_Main;
		if(i<5)
		{
			CB_Zad[i]->Left = 264;
			CB_Zad[i]->Top = 32 + i*24;	
		}
	else
		{
			CB_Zad[i]->Left = 648;
			CB_Zad[i]->Top = 32 + (i - 5)*24;
		}
		CB_Zad[i]->Height = 17;
		CB_Zad[i]->Width = 17;
		CB_Zad[i]->Enabled = false;
		
		CB_Ent[i] = new TCheckBox(GB_Main);
		CB_Ent[i]->Parent = GB_Main;

		if(i<5)
		{
			CB_Ent[i]->Left = 288;
			CB_Ent[i]->Top = 32 + i*24;	
		}
		else
		{
			CB_Ent[i]->Left = 672;
			CB_Ent[i]->Top = 32 + (i - 5)*24;
		}
		CB_Ent[i]->Height = 17;
		CB_Ent[i]->Width = 17;

        if(i==6)
        {
            CB_Ent[i]->Visible = false;
            CB_Zad[i]->Visible = false;
        }
	}

	Btn_Zap1 = new TButton(GB_Main);
	Btn_Zap1->Parent = GB_Main;
	Btn_Zap1->Top = 144;
	Btn_Zap1->Left = 528;
	Btn_Zap1->Font->Name = "Arial";
	Btn_Zap1->Font->Size = 12;
	Btn_Zap1->Font->Color = clBlack;
	Btn_Zap1->Font->Style = Btn_Zap1->Font->Style >> fsBold;
	Btn_Zap1->Caption = "������";
	Btn_Zap1->Width = 75;
	Btn_Zap1->Height = 26;
	Btn_Zap1->Hint = "1";
	Btn_Zap1->OnClick = BU_IVE_SetZap;
	
	Btn_Zap2 = new TButton(GB_Main);
	Btn_Zap2->Parent = GB_Main;
	Btn_Zap2->Top = 144;
	Btn_Zap2->Left = 608;
	Btn_Zap2->Font->Name = "Arial";
	Btn_Zap2->Font->Size = 12;
	Btn_Zap2->Font->Color = clBlack;
	Btn_Zap2->Font->Style = Btn_Zap2->Font->Style >> fsBold;
	Btn_Zap2->Caption = "������";
	Btn_Zap2->Width = 75;
	Btn_Zap2->Height = 26;
	Btn_Zap2->Hint = "3";
	Btn_Zap2->OnClick = BU_IVE_SetZap;
	
	Lbl_Uni = new TLabel(GB_Err);
	Lbl_Uni->Parent = GB_Err;
	Lbl_Uni->Top = 32;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "�������������� ��������� (DEZ)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Lbl_Uni = new TLabel(GB_Err);
	Lbl_Uni->Parent = GB_Err;
	Lbl_Uni->Top = 56;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "�������� ��� (DKW)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Lbl_Uni = new TLabel(GB_Err);
	Lbl_Uni->Parent = GB_Err;
	Lbl_Uni->Top = 80;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "�������� ��������� (DKZ)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Lbl_Uni = new TLabel(GB_Err);
	Lbl_Uni->Parent = GB_Err;
	Lbl_Uni->Top = 104;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "�������� �� (DK)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Lbl_Uni = new TLabel(GB_Err);
	Lbl_Uni->Parent = GB_Err;
	Lbl_Uni->Top = 128;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "���������� �� (DE)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Err);
	Lbl_Uni->Parent = GB_Err;
	Lbl_Uni->Top = 152;
	Lbl_Uni->Left = 16;
	Lbl_Uni->Caption = "��������� ������� (DEL)";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	for(int i=0;i<6;i++)
	{
		CB_Err[i] = new TCheckBox(GB_Err);
		CB_Err[i]->Parent = GB_Err;
		CB_Err[i]->Left = 288;
		CB_Err[i]->Top = 32 + i*24;	
		CB_Err[i]->Height = 17;
		CB_Err[i]->Width = 17;
		CB_Err[i]->Enabled = false;
	}
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 24;
	Lbl_Uni->Left = 50;
	Lbl_Uni->Caption = "�������";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 10;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 24;
	Lbl_Uni->Left = 294;
	Lbl_Uni->Caption = "�������";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 10;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 51;
	Lbl_Uni->Left = 150;
	Lbl_Uni->Caption = "U 0-750 �";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 83;
	Lbl_Uni->Left = 150;
	Lbl_Uni->Caption = "I  0-10.0 �";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 115;
	Lbl_Uni->Left = 150;
	Lbl_Uni->Caption = "P 0-6000 ��";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 147;
	Lbl_Uni->Left = 150;
	Lbl_Uni->Caption = "Ia 0-600 �";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	for (int i=0;i<4;i++)
	{
		Edt_Ent_Char[i] = new TEdit(GB_Par);
        Edt_Ent_Char[i]->Name = "EdtEnt" + IntToStr(i);
		Edt_Ent_Char[i]->Parent = GB_Par;
		Edt_Ent_Char[i]->Top = 48 + i*32;
		Edt_Ent_Char[i]->Left = 24;
		Edt_Ent_Char[i]->Font->Name = "Arial";
		Edt_Ent_Char[i]->Font->Size = 13;
		Edt_Ent_Char[i]->Font->Color = clBlack;
		Edt_Ent_Char[i]->Font->Style = Edt_Ent_Char[i]->Font->Style >> fsBold;
		Edt_Ent_Char[i]->BevelKind = bkFlat;
		Edt_Ent_Char[i]->BevelOuter = bvRaised;
		Edt_Ent_Char[i]->BorderStyle = bsNone;
		Edt_Ent_Char[i]->ReadOnly = false;
		Edt_Ent_Char[i]->Color = clWhite;
		Edt_Ent_Char[i]->Height = 26;
		Edt_Ent_Char[i]->Width = 50;
		Edt_Ent_Char[i]->Text = "0";
        Edt_Ent_Char[i]->MaxLength = 5;
        Edt_Ent_Char[i]->OnExit = Ent_Check;
		
		Edt_Zad_Char[i] = new TEdit(GB_Par);
		Edt_Zad_Char[i]->Parent = GB_Par;
		Edt_Zad_Char[i]->Top = 48 + i*32;
		Edt_Zad_Char[i]->Left = 80;
		Edt_Zad_Char[i]->Font->Name = "Arial";
		Edt_Zad_Char[i]->Font->Size = 13;
		Edt_Zad_Char[i]->Font->Color = clBlack;
		Edt_Zad_Char[i]->Font->Style = Edt_Zad_Char[i]->Font->Style >> fsBold;
		Edt_Zad_Char[i]->BevelKind = bkFlat;
		Edt_Zad_Char[i]->BevelOuter = bvRaised;
		Edt_Zad_Char[i]->BorderStyle = bsNone;
		Edt_Zad_Char[i]->ReadOnly = true;
		Edt_Zad_Char[i]->Color = clSkyBlue;
		Edt_Zad_Char[i]->Height = 26;
		Edt_Zad_Char[i]->Width = 50;
		
		Edt_Tek_Char[i] = new TEdit(GB_Par);
		Edt_Tek_Char[i]->Parent = GB_Par;
		Edt_Tek_Char[i]->Top = 48 + i*32;
		Edt_Tek_Char[i]->Left = 306;
		Edt_Tek_Char[i]->Font->Name = "Arial";
		Edt_Tek_Char[i]->Font->Size = 13;
		Edt_Tek_Char[i]->Font->Color = clBlack;
		Edt_Tek_Char[i]->Font->Style = Edt_Tek_Char[i]->Font->Style >> fsBold;
		Edt_Tek_Char[i]->BevelKind = bkFlat;
		Edt_Tek_Char[i]->BevelOuter = bvRaised;
		Edt_Tek_Char[i]->BorderStyle = bsNone;
		Edt_Tek_Char[i]->ReadOnly = true;
		Edt_Tek_Char[i]->Color = clSkyBlue;
		Edt_Tek_Char[i]->Height = 26;
		Edt_Tek_Char[i]->Width = 50;
    }
		
    Btn_Zap3 = new TButton(GB_Par);
    Btn_Zap3->Parent = GB_Par;
    Btn_Zap3->Top = 190;
    Btn_Zap3->Left = 200;
    Btn_Zap3->Font->Name = "Arial";
    Btn_Zap3->Font->Size = 12;
    Btn_Zap3->Font->Color = clBlack;
    Btn_Zap3->Font->Style = Btn_Zap3->Font->Style >> fsBold;
    Btn_Zap3->Caption = "������";
    Btn_Zap3->Width = 75;
    Btn_Zap3->Height = 26;
    Btn_Zap3->Hint = "2";
    Btn_Zap3->OnClick = BU_IVE_SetZap;

    Btn_Zap4 = new TButton(GB_Par);
	Btn_Zap4->Parent = GB_Par;
	Btn_Zap4->Top = 190;
	Btn_Zap4->Left = 280;
	Btn_Zap4->Font->Name = "Arial";
	Btn_Zap4->Font->Size = 12;
	Btn_Zap4->Font->Color = clBlack;
	Btn_Zap4->Font->Style = Btn_Zap4->Font->Style >> fsBold;
	Btn_Zap4->Caption = "������";
	Btn_Zap4->Width = 75;
	Btn_Zap4->Height = 26;
	Btn_Zap4->Hint = "3";
	Btn_Zap4->OnClick = BU_IVE_SetZap;

    Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 240;
	Lbl_Uni->Left = 90;
	Lbl_Uni->Caption = "����. ���. 4095 ���";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Par);
	Lbl_Uni->Parent = GB_Par;
	Lbl_Uni->Top = 272;
	Lbl_Uni->Left = 90;
	Lbl_Uni->Caption = "����. ����� 1023 ��";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    for (int i=4;i<6;i++)
	{
		Edt_Ent_Char[i] = new TEdit(GB_Par);
        Edt_Ent_Char[i]->Name = "EdtEnt" + IntToStr(i);
		Edt_Ent_Char[i]->Parent = GB_Par;
		Edt_Ent_Char[i]->Top = 108 + i*32;
		Edt_Ent_Char[i]->Left = 24;
		Edt_Ent_Char[i]->Font->Name = "Arial";
		Edt_Ent_Char[i]->Font->Size = 13;
		Edt_Ent_Char[i]->Font->Color = clBlack;
		Edt_Ent_Char[i]->Font->Style = Edt_Ent_Char[i]->Font->Style >> fsBold;
		Edt_Ent_Char[i]->BevelKind = bkFlat;
		Edt_Ent_Char[i]->BevelOuter = bvRaised;
		Edt_Ent_Char[i]->BorderStyle = bsNone;
		Edt_Ent_Char[i]->ReadOnly = false;
		Edt_Ent_Char[i]->Color = clWhite;
		Edt_Ent_Char[i]->Height = 26;
		Edt_Ent_Char[i]->Width = 50;
		Edt_Ent_Char[i]->Text = "0";
        Edt_Ent_Char[i]->MaxLength = 5;
        Edt_Ent_Char[i]->OnExit = Ent_Check;

        Edt_Tek_Char[i] = new TEdit(GB_Par);
		Edt_Tek_Char[i]->Parent = GB_Par;
		Edt_Tek_Char[i]->Top = 108 + i*32;
		Edt_Tek_Char[i]->Left = 306;
		Edt_Tek_Char[i]->Font->Name = "Arial";
		Edt_Tek_Char[i]->Font->Size = 13;
		Edt_Tek_Char[i]->Font->Color = clBlack;
		Edt_Tek_Char[i]->Font->Style = Edt_Tek_Char[i]->Font->Style >> fsBold;
		Edt_Tek_Char[i]->BevelKind = bkFlat;
		Edt_Tek_Char[i]->BevelOuter = bvRaised;
		Edt_Tek_Char[i]->BorderStyle = bsNone;
		Edt_Tek_Char[i]->ReadOnly = true;
		Edt_Tek_Char[i]->Color = clSkyBlue;
		Edt_Tek_Char[i]->Height = 26;
		Edt_Tek_Char[i]->Width = 50;
    }

    Btn_Zap5 = new TButton(GB_Par);
    Btn_Zap5->Parent = GB_Par;
    Btn_Zap5->Top = 310;
    Btn_Zap5->Left = 200;
    Btn_Zap5->Font->Name = "Arial";
    Btn_Zap5->Font->Size = 12;
    Btn_Zap5->Font->Color = clBlack;
    Btn_Zap5->Font->Style = Btn_Zap5->Font->Style >> fsBold;
    Btn_Zap5->Caption = "������";
    Btn_Zap5->Width = 75;
    Btn_Zap5->Height = 26;
    Btn_Zap5->Hint = "4";
    Btn_Zap5->OnClick = BU_IVE_SetZap;

    Btn_Zap6 = new TButton(GB_Par);
	Btn_Zap6->Parent = GB_Par;
	Btn_Zap6->Top = 310;
	Btn_Zap6->Left = 280;
	Btn_Zap6->Font->Name = "Arial";
	Btn_Zap6->Font->Size = 12;
	Btn_Zap6->Font->Color = clBlack;
	Btn_Zap6->Font->Style = Btn_Zap6->Font->Style >> fsBold;
	Btn_Zap6->Caption = "������";
	Btn_Zap6->Width = 75;
	Btn_Zap6->Height = 26;
	Btn_Zap6->Hint = "6";
	Btn_Zap6->OnClick = BU_IVE_SetZap;

    Lbl_Uni = new TLabel(GB_Dugo);
	Lbl_Uni->Parent = GB_Dugo;
	Lbl_Uni->Top = 33;
	Lbl_Uni->Left = 90;
	Lbl_Uni->Caption = "���������� 0-380 �";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
	
	Lbl_Uni = new TLabel(GB_Dugo);
	Lbl_Uni->Parent = GB_Dugo;
	Lbl_Uni->Top = 65;
	Lbl_Uni->Left = 90;
	Lbl_Uni->Caption = "��� 0-640 �";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    for (int i=6;i<8;i++)
	{
		Edt_Ent_Char[i] = new TEdit(GB_Dugo);
        Edt_Ent_Char[i]->Name = "EdtEnt" + IntToStr(i);
		Edt_Ent_Char[i]->Parent = GB_Dugo;
		Edt_Ent_Char[i]->Top = -163 + i*32;
		Edt_Ent_Char[i]->Left = 24;
		Edt_Ent_Char[i]->Font->Name = "Arial";
		Edt_Ent_Char[i]->Font->Size = 13;
		Edt_Ent_Char[i]->Font->Color = clBlack;
		Edt_Ent_Char[i]->Font->Style = Edt_Ent_Char[i]->Font->Style >> fsBold;
		Edt_Ent_Char[i]->BevelKind = bkFlat;
		Edt_Ent_Char[i]->BevelOuter = bvRaised;
		Edt_Ent_Char[i]->BorderStyle = bsNone;
		Edt_Ent_Char[i]->ReadOnly = false;
		Edt_Ent_Char[i]->Color = clWhite;
		Edt_Ent_Char[i]->Height = 26;
		Edt_Ent_Char[i]->Width = 50;
		Edt_Ent_Char[i]->Text = "0";
        Edt_Ent_Char[i]->MaxLength = 5;
        Edt_Ent_Char[i]->OnExit = Ent_Check;

        Edt_Tek_Char[i] = new TEdit(GB_Dugo);
		Edt_Tek_Char[i]->Parent = GB_Dugo;
		Edt_Tek_Char[i]->Top = -163 + i*32;
		Edt_Tek_Char[i]->Left = 250;
		Edt_Tek_Char[i]->Font->Name = "Arial";
		Edt_Tek_Char[i]->Font->Size = 13;
		Edt_Tek_Char[i]->Font->Color = clBlack;
		Edt_Tek_Char[i]->Font->Style = Edt_Tek_Char[i]->Font->Style >> fsBold;
		Edt_Tek_Char[i]->BevelKind = bkFlat;
		Edt_Tek_Char[i]->BevelOuter = bvRaised;
		Edt_Tek_Char[i]->BorderStyle = bsNone;
		Edt_Tek_Char[i]->ReadOnly = true;
		Edt_Tek_Char[i]->Color = clSkyBlue;
		Edt_Tek_Char[i]->Height = 26;
		Edt_Tek_Char[i]->Width = 50;
    }

    Btn_Zap7 = new TButton(GB_Dugo);
    Btn_Zap7->Parent = GB_Dugo;
    Btn_Zap7->Top = 103;
    Btn_Zap7->Left = 200 - 56;
    Btn_Zap7->Font->Name = "Arial";
    Btn_Zap7->Font->Size = 12;
    Btn_Zap7->Font->Color = clBlack;
    Btn_Zap7->Font->Style = Btn_Zap7->Font->Style >> fsBold;
    Btn_Zap7->Caption = "������";
    Btn_Zap7->Width = 75;
    Btn_Zap7->Height = 26;
    Btn_Zap7->Hint = "5";
    Btn_Zap7->OnClick = BU_IVE_SetZap;

    Btn_Zap8 = new TButton(GB_Dugo);
    Btn_Zap8->Parent = GB_Dugo;
    Btn_Zap8->Top = 103;
    Btn_Zap8->Left = 280 - 56;
    Btn_Zap8->Font->Name = "Arial";
    Btn_Zap8->Font->Size = 12;
    Btn_Zap8->Font->Color = clBlack;
    Btn_Zap8->Font->Style = Btn_Zap8->Font->Style >> fsBold;
    Btn_Zap8->Caption = "������";
    Btn_Zap8->Width = 75;
    Btn_Zap8->Height = 26;
    Btn_Zap8->Hint = "7";
    Btn_Zap8->OnClick = BU_IVE_SetZap;
}
//---------------------------------------------------------------------------
//-- �������� ��������� �������� --//
//---------------------------------------------------------------------------
void __fastcall S_BPM_HP::Ent_Check(TObject *Sender)
{
    // �������� ����� �� �������
    AnsiString text = ((TEdit*)Sender)->Text;
    for ( unsigned char i = 1 ; i < text.Length(); i++)
        if (text[i] == '.') text[i] = ',';
    unsigned char format; // ���-�� ������ ����� �������
    float valueText = 0.0;
    try
    {
        valueText = StrToFloat(text);
    }
    catch (Exception &exception)
    {
        ((TEdit*)Sender)->Text = "0";
        return;
    }

    if(((TEdit*)Sender)->Name == "EdtEnt0")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 750.0) valueText = 750.0;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt1")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 10.0) valueText = 10.0;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt2")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 6000) valueText = 6000;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt3")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 600) valueText = 600;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt4")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 4095) valueText = 4095;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt5")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 1023) valueText = 1023;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt6")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 380) valueText = 380;
    }
    else if(((TEdit*)Sender)->Name == "EdtEnt7")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0) valueText = 0;
        // ��������� �� ��������
        else if (valueText > 640) valueText = 640;
    }

    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 5, format);
}
//---------------------------------------------------------------------------
//-- ������ ������ �� �������� --//
//---------------------------------------------------------------------------
void __fastcall S_BPM_HP::BU_IVE_SetZap(TObject *Sender)
{
	/*
	switch(StrToInt(((TButton*)Sender)->Hint))
	{
		case 6:
		{
			String war_str = "��������! �������� ���������� �� ������� ��������. ";
			war_str += "���������� �������� � ������� 15-20% � � ������� ��� �������� ������� �����. ";
			war_str += "����� ����� ��������� �������� � ��������� ��� ���������. ";
			war_str += "��������� ��������?";
			if(MessageDlg(war_str,mtWarning,TMsgDlgButtons() << mbYes << mbCancel,0) != mrYes) return;
		}; break;

		default: { }; break;
	}
	*/

	RCom = StrToInt(((TPanel*)Sender)->Hint);
}
//---------------------------------------------------------------------------
//-- ������� ������������ ������� --//
//---------------------------------------------------------------------------
void S_BPM_HP::BU_IVE_FrmZap(bool Zap_type)
{
    if(!(*Pr_BU_IVE)) // ��������� ������ ������
    {
        if(!pr_zap[0]) ACom = 1;
        else if(!pr_zap[1]) ACom = 2;
        else if(!pr_zap[3]) ACom = 4;
        else if(!pr_zap[4]) ACom = 5;
        else if(!pr_zap[2]) ACom = 3;
        else if(!pr_zap[5]) ACom = 6;
        else
        {
            ACom = 1;
            pr_zap[0] = 0;
            pr_zap[1] = 0;
            pr_zap[2] = 0;
            pr_zap[3] = 0;
            pr_zap[4] = 0;
            pr_zap[5] = 0;
        }
    }
    else
    {
        if(ACom == 3) ACom = 6;
        if(ACom == 6) ACom = 7;
        else ACom = 3;
    }
	
	unsigned char b;
	float HValue = 0;

	if(Zap_type) // ������ ������
	{
		if(RCom == 1)	// ������ ������
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x04;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x15;	// ������ ������� ������
			SPort->PackOut[5] = 0x15;	// ��������� ������� ������
		
			b = 0;
			if(CB_Ent[8]->Checked == false) b = b + 0x04;  // U/F
            if(CB_Ent[5]->Checked == false) b = b + 0x10;   // P/I
			SPort->PackOut[6] = b;

			b = 0;
			if(CB_Ent[3]->Checked == false) b = b + 0x01;   // DEF
			if(CB_Ent[0]->Checked == true) b = b + 0x08;   // DEL
			if(CB_Ent[4]->Checked == false) b = b + 0x10;  // DEP
			if(CB_Ent[2]->Checked == false) b = b + 0x20;  // DEV
			if(CB_Ent[7]->Checked == false) b = b + 0x40;  // OUT
			if(CB_Ent[1]->Checked == false) b = b + 0x80; // DEW
			SPort->PackOut[7] = b;
		}
		else if(RCom == 2) // ������ �������������
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x0A;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x04;	// ��������� ������� ������	
			
			// U 0-750 V
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[0]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[0]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0x00FF * HValue / 750.0);
			SPort->PackOut[12] = int(HValue) & 0xFF;
			SPort->PackOut[13] = (int(HValue) & 0xFF00) >> 8;
			// I 0-10 A
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[1]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[1]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0x0FFF * HValue / 10.0);
			SPort->PackOut[6] = int(HValue) & 0xFF;
			SPort->PackOut[7] = (int(HValue) & 0xFF00) >> 8;
			// P 0-6000 W
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[2]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[2]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0x0FFF * HValue / 6000.0);
			SPort->PackOut[10] = int(HValue) & 0xFF;
			SPort->PackOut[11] = (int(HValue) & 0xFF00) >> 8;
			// Ia 0-600
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[3]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[3]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0xFFF * HValue / 600.0);
			SPort->PackOut[8] = int(HValue) & 0xFF;
            SPort->PackOut[9] = (int(HValue) & 0xFF00) >> 8;
		}
		else if(RCom == 3) // ������ ��������� R0x01 - R0x16
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x52;	// R - ������
			SPort->PackOut[2] = 0x02;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x16;	// ��������� ������� ������
		}
        else if(RCom == 4) // ������ ������������� ����������
		{
			SPort->PackOut[0] = adr_gen;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x06;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x02;	// ��������� ������� ������
		    // 4095 ----------------------------------------------------------
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[4]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[4]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0x0FFF * HValue / 4095);
			SPort->PackOut[6] = int(HValue) & 0xFF;
			SPort->PackOut[7] = (int(HValue) & 0xFF00) >> 8;
			// 1023 ----------------------------------------------------------
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[5]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[5]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0xFFF * HValue / 1023);
			SPort->PackOut[8] = int(HValue) & 0xFF;
            SPort->PackOut[9] = (int(HValue) & 0xFF00) >> 8;
		}
        else if(RCom == 5) // ������ ��������� R0x12 - R0x13
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x06;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x12;	// ������ ������� ������
			SPort->PackOut[5] = 0x13;	// ��������� ������� ������
            // 380 ----------------------------------------------------------
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[6]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[6]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0xFF * HValue / 380.0);
			SPort->PackOut[6] = int(HValue) & 0xFF;
			SPort->PackOut[7] = 0x00;
			// 1350 ----------------------------------------------------------
			try
			{
				HValue = StrToFloat(Edt_Ent_Char[7]->Text);
			}
			catch (Exception &exception)
			{
				Edt_Ent_Char[6]->Text = "0";
				RCom = 0;
				return;
			}
			HValue = (0xFF * HValue / 640.0);
			SPort->PackOut[8] = int(HValue) & 0xFF;
            SPort->PackOut[9] = 0x00;
		}
        else if(RCom == 6) // ������ ��������� R0x01 - R0x02
		{
			SPort->PackOut[0] = adr_gen;	// �����
			SPort->PackOut[1] = 0x52;	// R - ������
			SPort->PackOut[2] = 0x02;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x02;	// ��������� ������� ������
		}
        else if(RCom == 7) // ������ ��������� R0x12 - R0x13
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x52;	// R - ������
			SPort->PackOut[2] = 0x02;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x12;	// ������ ������� ������
			SPort->PackOut[5] = 0x13;	// ��������� ������� ������
		}
	}
	else // ������������� ������
	{
		if(ACom == 2) // ������ ������
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x04;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x15;	// ������ ������� ������
			SPort->PackOut[5] = 0x15;	// ��������� ������� ������
			SPort->PackOut[6] = *Kom_BU_IVE[6]&0xFF;
			SPort->PackOut[7] = (*Kom_BU_IVE[6]&0xFF00)>>8;
		}
		else if(ACom == 1) // ������ �������������
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x0A;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x04;	// ��������� ������� ������
			SPort->PackOut[6] = *Kom_BU_IVE[0]&0xFF;
			SPort->PackOut[7] = (*Kom_BU_IVE[0]&0xFF00)>>8;
			SPort->PackOut[8] = *Kom_BU_IVE[1]&0xFF;
			SPort->PackOut[9] = (*Kom_BU_IVE[1]&0xFF00)>>8;
			SPort->PackOut[10] = *Kom_BU_IVE[2]&0xFF;
			SPort->PackOut[11] = (*Kom_BU_IVE[2]&0xFF00)>>8;
			SPort->PackOut[12] = *Kom_BU_IVE[3]&0xFF;
			SPort->PackOut[13] = (*Kom_BU_IVE[3]&0xFF00)>>8;
		}
		else if(ACom == 3) // ������ ��������� R0x01 - R0x16
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x52;	// R - ������
			SPort->PackOut[2] = 0x02;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x16;	// ��������� ������� ������
		}
        else if(ACom == 4) // ������ �������������
		{
			SPort->PackOut[0] = adr_gen;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x06;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x02;	// ��������� ������� ������
			SPort->PackOut[6] = *Kom_BU_IVE[7]&0xFF;
			SPort->PackOut[7] = (*Kom_BU_IVE[7]&0xFF00)>>8;
			SPort->PackOut[8] = *Kom_BU_IVE[8]&0xFF;
			SPort->PackOut[9] = (*Kom_BU_IVE[8]&0xFF00)>>8;
		}
        else if(ACom == 5) // ������ ������������� ����������
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x57;	// W - ������
			SPort->PackOut[2] = 0x06;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x12;	// ������ ������� ������
			SPort->PackOut[5] = 0x13;	// ��������� ������� ������
			SPort->PackOut[6] = *Kom_BU_IVE[4]&0xFF;
			SPort->PackOut[7] = (*Kom_BU_IVE[4]&0xFF00)>>8;
			SPort->PackOut[8] = *Kom_BU_IVE[5]&0xFF;
			SPort->PackOut[9] = (*Kom_BU_IVE[5]&0xFF00)>>8;
		}
        else if(ACom == 6) // ������ ��������� R0x01 - R0x02
		{
			SPort->PackOut[0] = adr_gen;	// �����
			SPort->PackOut[1] = 0x52;	// R - ������
			SPort->PackOut[2] = 0x02;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x01;	// ������ ������� ������
			SPort->PackOut[5] = 0x02;	// ��������� ������� ������
		}
        else if(ACom == 7) // ������ ��������� R0x12 - R0x13
		{
			SPort->PackOut[0] = adr;	// �����
			SPort->PackOut[1] = 0x52;	// R - ������
			SPort->PackOut[2] = 0x02;	// ����� ������� ��.�.
			SPort->PackOut[3] = 0x00;	// ����� ������� ��.�.
			SPort->PackOut[4] = 0x12;	// ������ ������� ������
			SPort->PackOut[5] = 0x13;	// ��������� ������� ������					
		}
	}		

	// ������ ��
	unsigned int ksIVE = 0;

	Buf_len = SPort->PackOut[2] + 5;   // ����� �������

	for(int i=0;i<(Buf_len-1);i++)
		ksIVE = ksIVE + SPort->PackOut[i];
	ksIVE = (0xFF00 - ksIVE) & 0xFF;
	SPort->PackOut[Buf_len-1] = ksIVE;
}
//---------------------------------------------------------------------------
//-- ������� ��������� ������ --//
//---------------------------------------------------------------------------
void S_BPM_HP::BU_IVE_ChkRep1(bool Zap_type)
{	
	unsigned int b;

	// ���������� ������ ��������
	b = SPort->PackIn[6] + SPort->PackIn[7]*0x100; // ��� ���
	b = b * 100.0 / 0xFFF;
	Edt_Zad_Char[1]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);

	b = SPort->PackIn[8] + SPort->PackIn[9]*0x100; // ��� ��� ���
	b = b * 600 / 0xFFF;
	Edt_Zad_Char[3]->Text = FloatToStrF(float(b),ffFixed,6,0);

	b = SPort->PackIn[10] + SPort->PackIn[11]*0x100; // ��� ����
	b = b * 6000 / 0xFFF;
	Edt_Zad_Char[2]->Text = FloatToStrF(float(b),ffFixed,6,0);

	b = SPort->PackIn[12] + SPort->PackIn[13]*0x100; // ��� ����
	b = b * 750 / 0x0FF;
	Edt_Zad_Char[0]->Text = FloatToStrF(float(b),ffFixed,6,0);
	
    b = SPort->PackIn[18] + SPort->PackIn[19]*0x100; // ��� ����
	b = b * 750 / 0x3FF;
	Edt_Tek_Char[0]->Text = FloatToStrF(float(b),ffFixed,6,0);

	b = SPort->PackIn[20] + SPort->PackIn[21]*0x100; // ��� ���
	b = b * 102.4 / 0x3FF;
	Edt_Tek_Char[1]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);

	b = SPort->PackIn[36] + SPort->PackIn[37]*0x100; // ��� ����
	b = b * 6144 / 0x3FF;
	Edt_Tek_Char[2]->Text = FloatToStrF(float(b),ffFixed,6,0);

	b = SPort->PackIn[36] + SPort->PackIn[37]*0x100; // ��� ��� ���
	b = b * 614 / 0x3FF;
	Edt_Tek_Char[3]->Text = FloatToStrF(float(b),ffFixed,6,0);

	b = SPort->PackIn[46]; // ������� ������ ��.�
	CB_Zad[8]->Checked = !bool(b&0x04);	// U/F
	CB_Zad[5]->Checked = !bool(b&0x10);	// P/I

	b = SPort->PackIn[47]; // ������� ������ ��.�
	CB_Zad[3]->Checked = !bool(b&0x01);	//  DEF
	//CB_Zad[6]->Checked = bool(b&0x02);	//  A1
	//CB_Zad[5]->Checked = bool(b&0x04);	//  A2
	CB_Zad[0]->Checked = bool(b&0x08);	//  DEL
	CB_Zad[4]->Checked = !bool(b&0x10);	//  DEP
	CB_Zad[2]->Checked = !bool(b&0x20);	//  DEV
	CB_Zad[7]->Checked = !bool(b&0x40);	//  OUT
	CB_Zad[1]->Checked = !bool(b&0x80);	//  DEW

	b = SPort->PackIn[48]; // ������� ��������� ��.�
	CB_Err[4]->Checked = bool(b&0x01);
	CB_Err[3]->Checked = !bool(b&0x02);
	CB_Err[2]->Checked = !bool(b&0x04);
	CB_Err[1]->Checked = !bool(b&0x08);
	CB_Err[0]->Checked = !bool(b&0x10);
	CB_Err[5]->Checked = !bool(b&0x20);
	
	if(!Zap_type) // ���� ������ ������ - �� ��������� ������ �������!
	{
		*Otv_BU_IVE[0] = SPort->PackIn[6] + SPort->PackIn[7]*0x100;
		*Otv_BU_IVE[1] = SPort->PackIn[8] + SPort->PackIn[9]*0x100;
		*Otv_BU_IVE[2] = SPort->PackIn[10] + SPort->PackIn[11]*0x100;
		*Otv_BU_IVE[3] = SPort->PackIn[12] + SPort->PackIn[13]*0x100;
        *Otv_BU_IVE[9] = SPort->PackIn[46] + SPort->PackIn[47]*0x100;
        *Otv_BU_IVE[10] = SPort->PackIn[48] + SPort->PackIn[49]*0x100;
        
        if(CB_Zad[0]->Checked) // ������ ��� ���. �����
        {
		    *Otv_BU_IVE[5] = SPort->PackIn[18] + SPort->PackIn[19]*0x100;
            *Otv_BU_IVE[4] = SPort->PackIn[20] + SPort->PackIn[21]*0x100;
            *Otv_BU_IVE[8] = SPort->PackIn[38] + SPort->PackIn[39]*0x100;
            if(CB_Zad[5]->Checked)	// ��������
		    {
			    *Otv_BU_IVE[6] = SPort->PackIn[36] + SPort->PackIn[37]*0x100;
			    *Otv_BU_IVE[7] = 0;
		    }
		    else					// ���. ���
		    {
			    *Otv_BU_IVE[6] = 0;
			    *Otv_BU_IVE[7] = SPort->PackIn[36] + SPort->PackIn[37]*0x100;
		    }
        }
        else
        {
            *Otv_BU_IVE[4] = 0;
            *Otv_BU_IVE[5] = 0;
            *Otv_BU_IVE[6] = 0;
            *Otv_BU_IVE[7] = 0;
            *Otv_BU_IVE[8] = 0;
        }
	}
}
//---------------------------------------------------------------------------
void S_BPM_HP::BU_IVE_ChkRep2(bool Zap_type)
{
    unsigned int b;

    b = SPort->PackIn[6] + SPort->PackIn[7]*0x100; //
	b = b * 4095 / 0xFFF;
	Edt_Tek_Char[4]->Text = FloatToStrF(float(b),ffFixed,6,0);

    b = SPort->PackIn[8] + SPort->PackIn[9]*0x100; //
	b = b * 10230 / 0xFFF;
	Edt_Tek_Char[5]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);

    if(!Zap_type) // ���� ������ ������ - �� ��������� ������ �������!
	{
		*Otv_BU_IVE[11] = SPort->PackIn[6] + SPort->PackIn[7]*0x100;
		*Otv_BU_IVE[12] = SPort->PackIn[8] + SPort->PackIn[9]*0x100;
    }
}
//---------------------------------------------------------------------------
void S_BPM_HP::BU_IVE_ChkRep3(bool Zap_type)
{
    unsigned int b;

    b = SPort->PackIn[6] + SPort->PackIn[7]*0x100; //
	b = b * 380 / 0xFF;
	Edt_Tek_Char[6]->Text = FloatToStrF(float(b),ffFixed,6,0);

    b = SPort->PackIn[8] + SPort->PackIn[9]*0x100; //
	b = b * 640 / 0xFF;
	Edt_Tek_Char[7]->Text = FloatToStrF(float(b),ffFixed,6,0);
}
//---------------------------------------------------------------------------
//-- ������� ����� � �������� --//
//---------------------------------------------------------------------------
bool S_BPM_HP::BU_IVE_Manage(unsigned int SH,bool Zap_type)
{
	// ������������ ������������� �� ��������� �������� ��� �� ������
	RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

    if(SH == 0) // ������� �������
	{
		SPort->PackageClear();
		// ������� ������������ �������
		BU_IVE_FrmZap(Zap_type);
		SPort->VisPackRTU(0,Buf_len);

		// ������� ������ �����
		SPort->Port.ResetRB();
		// �������� �������
		SPort->Port.Write(SPort->PackOut,Buf_len);
		// ������� �� ��������� ���
		SPort->DevState++;
		// ��������� �������
		SPort->Dev_Timer = 0;
		return 0;
	}
	else if(SH == 1)
	{
		// ������ ������
        if(SPort->Port.GetRBSize() < PACKAGE_COUNT)
        {
            SPort->Port.Read(SPort->PackIn,SPort->Port.GetRBSize());
        }
        else
        {
            // ������� ������ �����
            SPort->Port.ResetRB();
            SPort->DevState++;
            return 0;
       }

		// �������� ������
		if(((SPort->PackIn[0] == adr)||(SPort->PackIn[0] == adr_gen)) &&
			((SPort->PackIn[1] == 0x57)||
             (((ACom == 3)||(RCom == 3))&&((SPort->PackIn[1] == 0x52)&&(SPort->PackIn[50] != 0)))||
             (((ACom == 6)||(RCom == 6))&&((SPort->PackIn[1] == 0x52)&&(SPort->PackIn[10] != 0)))||
             (((ACom == 7)||(RCom == 7))&&((SPort->PackIn[1] == 0x52)&&(SPort->PackIn[10] != 0)))))
		{
			Buf_len = SPort->PackIn[2] + 5; // ����� �������
			SPort->VisPackRTU(1,Buf_len);
			
			if(Zap_type) // ������ �����
			{			
				if(RCom == 3) BU_IVE_ChkRep1(Zap_type);
                if(RCom == 6) BU_IVE_ChkRep2(Zap_type);
                if(RCom == 7) BU_IVE_ChkRep3(Zap_type);
			}
			else // ��������������
			{
				if(ACom == 1) pr_zap[0] = true;
				else if(ACom == 2) pr_zap[1] = true;
                else if(ACom == 3)
                {
                    BU_IVE_ChkRep1(Zap_type);
					pr_zap[2] = true;

                    if(pr_zap[0]&&pr_zap[1]&&pr_zap[2]&&pr_zap[3]&&pr_zap[4]&&pr_zap[5]&&(*Otv_BU_IVE[9]==*Kom_BU_IVE[6]))
						*Pr_BU_IVE = 1;
                }
                else if(ACom == 4) pr_zap[3] = true;
                else if(ACom == 5) pr_zap[4] = true;
                else if(ACom == 6)
				{
					BU_IVE_ChkRep2(Zap_type);
					pr_zap[5] = true;

                    if(pr_zap[0]&&pr_zap[1]&&pr_zap[2]&&pr_zap[3]&&pr_zap[4]&&pr_zap[5]&&(*Otv_BU_IVE[9]==*Kom_BU_IVE[6]))
						*Pr_BU_IVE = 1;
				}
                else if(ACom == 7)
				{
					BU_IVE_ChkRep3(Zap_type);
					pr_zap[6] = true;
				}
			}
			
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
			if((Err++) > Max_err)
				return 1;
			return 0;
		}
    }
}
//---------------------------------------------------------------------------
//-- ������������� �������� --//
//---------------------------------------------------------------------------
void Init_BU_IVE()
{
	for(int i=0;i<BPM_HP_COUNT;i++)
    {
		BPM_HP[i] = new S_BPM_HP();
        for(int j=0;j<7;j++) BPM_HP[i]->pr_zap[j] = 0;
	    for(int j=0;j<9;j++) BPM_HP[i]->Kom_BU_IVE[j] = &KOM_BMH1[j];
	    for(int j=0;j<13;j++) BPM_HP[i]->Otv_BU_IVE[j] = &OTVET_BMH1[j];
        BPM_HP[i]->Err = 0;				// ���-�� ������
	    BPM_HP[i]->Max_err = 5;			// �������� ������
	    BPM_HP[i]->ACom = 0;				// ������� �������������� ������
	    BPM_HP[i]->RCom = 0;				// ������� ������ �����
	    BPM_HP[i]->Buf_len = 0;			// ����� �������
    }

	// ���_��
	BPM_HP[0]->name = "���(���-178-10VICS)"; // �������� ��� �����������
	BPM_HP[0]->adr = 1;
	BPM_HP[0]->adr_gen = 2,			// ����� ����������
	BPM_HP[0]->Pr_BU_IVE = &PR_SV_BMH1;
	BPM_HP[0]->SPort = Comport[5];     // ����
	BPM_HP[0]->diagnS_byte = 2;        // ����� ����� ������� �����������
	BPM_HP[0]->diagnS_mask = 0x20;		// ����� ����� ������� �����������
    BPM_HP[0]->BU_IVE_Gen();
}
//---------------------------------------------------------------------------
