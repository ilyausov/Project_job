//---------------------------------------------------------------------------
#include "IVE.h"
//---------------------------------------------------------------------------
//-- Генерация страницы --//
//---------------------------------------------------------------------------
void S_IVE::BU_IVE_Gen()
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
	// Pnl_Parent->Height = 676;  // для FullHD
	Pnl_Parent->Height = 634;  // для 1280x1024
	Pnl_Parent->Width = 1176;

	Lbl_Uni = new TLabel(Pnl_Parent);
	Lbl_Uni->Parent = Pnl_Parent;
	Lbl_Uni->Top = 15;
	Lbl_Uni->Left = 26;
	Lbl_Uni->Caption = "Адрес устройства: 00" + IntToStr(adr);
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
	GB_Main->Height = 185;
	GB_Main->Width = 705;
	GB_Main->Caption = " Команды ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;

    GB_Err = new TGroupBox(Form1);
	GB_Err->Parent = Pnl_Parent;
	GB_Err->Top = 248;
	GB_Err->Left = 26;
	GB_Err->Width = 321;
	if(type_BU_IVE == 0) GB_Err->Height = 193;
	if(type_BU_IVE == 1) GB_Err->Height = 225;
	GB_Err->Caption = " Состояние ";
	GB_Err->Font->Name = "Arial";
	GB_Err->Font->Size = 12;
	GB_Err->Font->Color = clBlack;
	GB_Err->Font->Style = GB_Main->Font->Style << fsBold;
	
	GB_Par = new TGroupBox(Form1);
	GB_Par->Parent = Pnl_Parent;
	GB_Par->Top = 248;
	GB_Par->Left = 354;
	GB_Par->Width = 377;
	if(type_BU_IVE == 0) GB_Par->Height = 193;
	if(type_BU_IVE == 1) GB_Par->Height = 225;
	GB_Par->Caption = " Характеристики ";
	GB_Par->Font->Name = "Arial";
	GB_Par->Font->Size = 12;
	GB_Par->Font->Color = clBlack;
	GB_Par->Font->Style = GB_Main->Font->Style << fsBold;
	
	if(type_BU_IVE == 0)	// БПИИ
	{
		Lbl_Uni = new TLabel(GB_Main);
		Lbl_Uni->Parent = GB_Main;
		Lbl_Uni->Top = 32;
		Lbl_Uni->Left = 16;
		Lbl_Uni->Caption = "Включение блока (DEL)";
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
		Lbl_Uni->Caption = "Блокировка откл. то КЗ (DEW)";
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
		Lbl_Uni->Caption = "Включение мощности (DEP)";
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
		Lbl_Uni->Caption = "Отображение вых. пар. (OUT)";
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
		Lbl_Uni->Left = 392;
		Lbl_Uni->Caption = "Отображение UI/FP (U/F)";
		Lbl_Uni->Font->Name = "Arial";
		Lbl_Uni->Font->Size = 12;
		Lbl_Uni->Font->Color = clBlack;
		Lbl_Uni->Transparent = true;
		Lbl_Uni->Height = 19;
		Lbl_Uni->Width = 190;
		Lbl_Uni->Layout = tlTop;
		Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
		
		for(int i=0;i<5;i++)
        {
            CB_Zad[i] = new TCheckBox(GB_Main);
			CB_Zad[i]->Parent = GB_Main;
			if(i<3)
			{
				CB_Zad[i]->Left = 264;
				CB_Zad[i]->Top = 32 + i*24;	
			}
			else
			{
				CB_Zad[i]->Left = 648;
				CB_Zad[i]->Top = 32 + (i - 3)*24;
			}
			CB_Zad[i]->Height = 17;
			CB_Zad[i]->Width = 17;
            CB_Zad[i]->Enabled = false;
			
			CB_Ent[i] = new TCheckBox(GB_Main);
			CB_Ent[i]->Parent = GB_Main;
			if(i<3)
			{
				CB_Ent[i]->Left = 288;
				CB_Ent[i]->Top = 32 + i*24;	
			}
			else
			{
				CB_Ent[i]->Left = 672;
				CB_Ent[i]->Top = 32 + (i - 3)*24;
			}
			CB_Ent[i]->Height = 17;
			CB_Ent[i]->Width = 17;
        }
		
		Btn_Zap1 = new TButton(GB_Main);
		Btn_Zap1->Parent = GB_Main;
		Btn_Zap1->Top = 144;
		Btn_Zap1->Left = 528;
		Btn_Zap1->Font->Name = "Arial";
		Btn_Zap1->Font->Size = 12;
		Btn_Zap1->Font->Color = clBlack;
		Btn_Zap1->Font->Style = Btn_Zap1->Font->Style >> fsBold;
		Btn_Zap1->Caption = "Запись";
		Btn_Zap1->Width = 75;
		Btn_Zap1->Height = 26;
		Btn_Zap1->Hint = "1";
		Btn_Zap1->OnClick = BU_IVE_SetZap;
		
		Btn_Zap3 = new TButton(GB_Main);
		Btn_Zap3->Parent = GB_Main;
		Btn_Zap3->Top = 144;
		Btn_Zap3->Left = 608;
		Btn_Zap3->Font->Name = "Arial";
		Btn_Zap3->Font->Size = 12;
		Btn_Zap3->Font->Color = clBlack;
		Btn_Zap3->Font->Style = Btn_Zap3->Font->Style >> fsBold;
		Btn_Zap3->Caption = "Запрос";
		Btn_Zap3->Width = 75;
		Btn_Zap3->Height = 26;
		Btn_Zap3->Hint = "3";
		Btn_Zap3->OnClick = BU_IVE_SetZap;
		
		Lbl_Uni = new TLabel(GB_Err);
		Lbl_Uni->Parent = GB_Err;
		Lbl_Uni->Top = 32;
		Lbl_Uni->Left = 16;
		Lbl_Uni->Caption = "Перегрев МУВБР (DEZ)";
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
		Lbl_Uni->Caption = "Короткое замыкание (DKZ)";
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
		Lbl_Uni->Caption = "Перегрев МК (DK)";
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
		Lbl_Uni->Caption = "Выходная мощность (DE)";
		Lbl_Uni->Font->Name = "Arial";
		Lbl_Uni->Font->Size = 12;
		Lbl_Uni->Font->Color = clBlack;
		Lbl_Uni->Transparent = true;
		Lbl_Uni->Height = 19;
		Lbl_Uni->Width = 190;
		Lbl_Uni->Layout = tlTop;
		Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
		
		for(int i=0;i<4;i++)
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
		Lbl_Uni->Caption = "Опорные";
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
		Lbl_Uni->Caption = "Текущие";
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
		Lbl_Uni->Caption = "U 0-3000 В";
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
		Lbl_Uni->Caption = "I  0-500 мА";
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
		Lbl_Uni->Caption = "P 0-1000 Вт";
		Lbl_Uni->Font->Name = "Arial";
		Lbl_Uni->Font->Size = 12;
		Lbl_Uni->Font->Color = clBlack;
		Lbl_Uni->Transparent = true;
		Lbl_Uni->Height = 19;
		Lbl_Uni->Width = 190;
		Lbl_Uni->Layout = tlTop;
		Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
		
		for (int i=0;i<3;i++)
		{
			Edt_Ent_Char[i] = new TEdit(GB_Par);
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
			Edt_Tek_Char[i]->Left = 296;
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
		
		Btn_Zap2 = new TButton(GB_Par);
		Btn_Zap2->Parent = GB_Par;
		Btn_Zap2->Top = 152;
		Btn_Zap2->Left = 280;
		Btn_Zap2->Font->Name = "Arial";
		Btn_Zap2->Font->Size = 12;
		Btn_Zap2->Font->Color = clBlack;
		Btn_Zap2->Font->Style = Btn_Zap2->Font->Style >> fsBold;
		Btn_Zap2->Caption = "Запись";
		Btn_Zap2->Width = 75;
		Btn_Zap2->Height = 26;
		Btn_Zap2->Hint = "2";
		Btn_Zap2->OnClick = BU_IVE_SetZap;
	}
    else if(type_BU_IVE == 1)	// БПМ
	{
		Lbl_Uni = new TLabel(GB_Main);
		Lbl_Uni->Parent = GB_Main;
		Lbl_Uni->Top = 32;
		Lbl_Uni->Left = 16;
		Lbl_Uni->Caption = "Включение блока (DEL)";
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
		Lbl_Uni->Caption = "Включение ключа (DEW)";
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
		Lbl_Uni->Caption = "Включение вольтдобавки (DEV)";
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
		Lbl_Uni->Caption = "Включение генератора (DEF)";
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
		Lbl_Uni->Caption = "Включение мощности (DEP)";
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
		Lbl_Uni->Caption = "Режим ключа меандр (A2)";
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
		Lbl_Uni->Left = 392;
		Lbl_Uni->Caption = "Режим ключа с паузой (A1)";
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
		Lbl_Uni->Caption = "Отображение вых. пар. (OUT)";
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
		Lbl_Uni->Caption = "Отображение UI/FP (U/F)";
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
        }
		
		Btn_Zap1 = new TButton(GB_Main);
		Btn_Zap1->Parent = GB_Main;
		Btn_Zap1->Top = 144;
		Btn_Zap1->Left = 528;
		Btn_Zap1->Font->Name = "Arial";
		Btn_Zap1->Font->Size = 12;
		Btn_Zap1->Font->Color = clBlack;
		Btn_Zap1->Font->Style = Btn_Zap1->Font->Style >> fsBold;
		Btn_Zap1->Caption = "Запись";
		Btn_Zap1->Width = 75;
		Btn_Zap1->Height = 26;
		Btn_Zap1->Hint = "1";
		Btn_Zap1->OnClick = BU_IVE_SetZap;
		
		Btn_Zap3 = new TButton(GB_Main);
		Btn_Zap3->Parent = GB_Main;
		Btn_Zap3->Top = 144;
		Btn_Zap3->Left = 608;
		Btn_Zap3->Font->Name = "Arial";
		Btn_Zap3->Font->Size = 12;
		Btn_Zap3->Font->Color = clBlack;
		Btn_Zap3->Font->Style = Btn_Zap3->Font->Style >> fsBold;
		Btn_Zap3->Caption = "Запрос";
		Btn_Zap3->Width = 75;
		Btn_Zap3->Height = 26;
		Btn_Zap3->Hint = "3";
		Btn_Zap3->OnClick = BU_IVE_SetZap;
		
		Lbl_Uni = new TLabel(GB_Err);
		Lbl_Uni->Parent = GB_Err;
		Lbl_Uni->Top = 32;
		Lbl_Uni->Left = 16;
		Lbl_Uni->Caption = "Несоответствие импеданса (DEZ)";
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
		Lbl_Uni->Caption = "Перегрев МКК (DKW)";
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
		Lbl_Uni->Caption = "Короткое замыкание (DKZ)";
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
		Lbl_Uni->Caption = "Перегрев МК (DK)";
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
		Lbl_Uni->Caption = "Выходная мощность (DE)";
		Lbl_Uni->Font->Name = "Arial";
		Lbl_Uni->Font->Size = 12;
		Lbl_Uni->Font->Color = clBlack;
		Lbl_Uni->Transparent = true;
		Lbl_Uni->Height = 19;
		Lbl_Uni->Width = 190;
		Lbl_Uni->Layout = tlTop;
		Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;
		
		for(int i=0;i<5;i++)
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
		Lbl_Uni->Caption = "Опорные";
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
		Lbl_Uni->Caption = "Текущие";
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
		Lbl_Uni->Caption = "U 0-650 В";
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
		Lbl_Uni->Caption = "I  0-15.0 А";
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
		Lbl_Uni->Caption = "P 0-3600 Вт";
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
		Lbl_Uni->Caption = "F 0-40.0 кГц";
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
			Edt_Tek_Char[i]->Left = 296;
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
		
		Btn_Zap2 = new TButton(GB_Par);
		Btn_Zap2->Parent = GB_Par;
		Btn_Zap2->Top = 184;
		Btn_Zap2->Left = 280;
		Btn_Zap2->Font->Name = "Arial";
		Btn_Zap2->Font->Size = 12;
		Btn_Zap2->Font->Color = clBlack;
		Btn_Zap2->Font->Style = Btn_Zap2->Font->Style >> fsBold;
		Btn_Zap2->Caption = "Запись";
		Btn_Zap2->Width = 75;
		Btn_Zap2->Height = 26;
		Btn_Zap2->Hint = "2";
		Btn_Zap2->OnClick = BU_IVE_SetZap;
	}
}
//---------------------------------------------------------------------------
//-- Ручной запрос со страницы --//
//---------------------------------------------------------------------------
void __fastcall S_IVE::BU_IVE_SetZap(TObject *Sender)
{
	/*
	switch(StrToInt(((TButton*)Sender)->Hint))
	{
		case 6:
		{
			String war_str = "Внимание! Обучение проводится на рабочем давлении. ";
			war_str += "Установите заслонку в позицию 15-20% и с помощью РРГ создайте рабочую среду. ";
			war_str += "После этого запустите обучение и дождитесь его окончания. ";
			war_str += "Выполнить обучение?";
			if(MessageDlg(war_str,mtWarning,TMsgDlgButtons() << mbYes << mbCancel,0) != mrYes) return;
		}; break;

		default: { }; break;
	}
	*/

	RCom = StrToInt(((TPanel*)Sender)->Hint);
}
//---------------------------------------------------------------------------
//-- Функция формирования запроса --//
//---------------------------------------------------------------------------
void S_IVE::BU_IVE_FrmZap(bool Zap_type)
{
	if(ACom == 1) ACom = 2;
	else if (ACom == 2) ACom = 3;
	else
	{
		if(!(*Pr_BU_IVE)) // требуется полный запрос
		{
			ACom = 1;
			pr_zap[0] = 0;
			pr_zap[1] = 0;
			pr_zap[2] = 0;
		}
		else
			ACom = 3;
	}
	
	unsigned char b;
	float HValue = 0;
	
	if(type_BU_IVE == 0)  // БПИИ
	{
		if(Zap_type) // ручной запрос
		{
			if(RCom == 1)	// запись команд
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x04;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x15;	// первый регистр записи
				SPort->PackOut[5] = 0x15;	// последний регистр записи

				b = 0;
				if (CB_Ent[4]->Checked == false) b = b + 4;  // U/F		
				SPort->PackOut[6] = b;

				b = 0;
				if(CB_Ent[0]->Checked == true) b = b + 8;   // DEL
				if(CB_Ent[2]->Checked == false) b = b + 16;  // DEP
				if(CB_Ent[3]->Checked == false) b = b + 64;  // OUT
				if(CB_Ent[1]->Checked == false) b = b + 128; // DEW
				SPort->PackOut[7] = b;
			}
			else if(RCom == 2) // запись характеристик R0x01 - R0x04
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x08;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x03;	// последний регистр записи

				// U 0-3000 V
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
				HValue = (0x0FFF * HValue / 3000);
				SPort->PackOut[8] = int(HValue) & 0xFF;
				SPort->PackOut[9] = (int(HValue) & 0xFF00) >> 8;
				// I 0-500 mA
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
				HValue = (0x0FFF * HValue / 500);
				SPort->PackOut[6] = int(HValue) & 0xFF;
				SPort->PackOut[7] = (int(HValue) & 0xFF00) >> 8;
				// P 0-1000 W
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
				HValue = (0x0FFF * HValue / 1000);
				SPort->PackOut[10] = int(HValue) & 0xFF;
				SPort->PackOut[11] = (int(HValue) & 0xFF00) >> 8;
			}
			else if(RCom == 3)// чтение регистров R0x01 - R0x16
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x52;	// R - чтение
				SPort->PackOut[2] = 0x02;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x16;	// последний регистр записи
			}
		}
		else // втоматический запрос
		{
            if(ACom == 1) // запись команд R0x15
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x04;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x15;	// первый регистр записи
				SPort->PackOut[5] = 0x15;	// последний регистр записи
				SPort->PackOut[6] = *Kom_BU_IVE[4]&0xFF;
				SPort->PackOut[7] = (*Kom_BU_IVE[4]&0xFF00)>>8;
			}
			else if(ACom == 2) // запись характеристик R0x01 - R0x04
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x0A;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x04;	// последний регистр записи				
				SPort->PackOut[6] = *Kom_BU_IVE[0]&0xFF;
				SPort->PackOut[7] = (*Kom_BU_IVE[0]&0xFF00)>>8;
				SPort->PackOut[8] = *Kom_BU_IVE[1]&0xFF;
				SPort->PackOut[9] = (*Kom_BU_IVE[1]&0xFF00)>>8;
				SPort->PackOut[10] = *Kom_BU_IVE[2]&0xFF;
				SPort->PackOut[11] = (*Kom_BU_IVE[2]&0xFF00)>>8;
				SPort->PackOut[12] = *Kom_BU_IVE[3]&0xFF;
				SPort->PackOut[13] = (*Kom_BU_IVE[3]&0xFF00)>>8;
			}
			else if(ACom == 3) // чтение регистров R0x01 - R0x16
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x52;	// R - чтение
				SPort->PackOut[2] = 0x02;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x16;	// последний регистр записи					
			}
		}
	}
	if(type_BU_IVE == 1)  // БПМ
	{
		if(Zap_type) // ручной запрос
		{
			if(RCom == 1)	// запись команд
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x04;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x15;	// первый регистр записи
				SPort->PackOut[5] = 0x15;	// последний регистр записи
			
				b = 0;
				if (CB_Ent[8]->Checked == false) b = b + 4;  // U/F
				SPort->PackOut[6] = b;

				b = 0;
				if(CB_Ent[3]->Checked == false) b = b + 1;   // DEF
				if(CB_Ent[6]->Checked == true) b = b + 2;   // A1
				if(CB_Ent[5]->Checked == true) b = b + 4;   // A2
				if(CB_Ent[0]->Checked == true) b = b + 8;   // DEL
				if(CB_Ent[4]->Checked == false) b = b + 16;  // DEP
				if(CB_Ent[2]->Checked == false) b = b + 32;  // DEV
				if(CB_Ent[7]->Checked == false) b = b + 64;  // OUT
				if(CB_Ent[1]->Checked == false) b = b + 128; // DEW
				SPort->PackOut[7] = b;
				
				
			}
			else if(RCom == 2) // запись характеристик
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x0A;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x04;	// последний регистр записи	
				
				// U 0-650 V
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
				HValue = (0x0FFF * HValue / 650);
				SPort->PackOut[8] = int(HValue) & 0xFF;
				SPort->PackOut[9] = (int(HValue) & 0xFF00) >> 8;
				// I 0-15 A
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
				HValue = (0x0FFF * HValue / 15.0);
				SPort->PackOut[6] = int(HValue) & 0xFF;
				SPort->PackOut[7] = (int(HValue) & 0xFF00) >> 8;
				// P 0-3600 W
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
				HValue = (0x0FFF * HValue / 3600);
				SPort->PackOut[10] = int(HValue) & 0xFF;
				SPort->PackOut[11] = (int(HValue) & 0xFF00) >> 8;
				// F 0-40 Hz
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
				HValue = (0xFF * HValue / 40.0);
				SPort->PackOut[12] = int(HValue) & 0xFF;			
			}
			else if(RCom == 3) // чтение регистров R0x01 - R0x16
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x52;	// R - чтение
				SPort->PackOut[2] = 0x02;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x16;	// последний регистр записи			
			}
		}
		else // втоматический запрос
		{
            if(ACom == 2) // запись команд
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x04;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x15;	// первый регистр записи
				SPort->PackOut[5] = 0x15;	// последний регистр записи
				SPort->PackOut[6] = *Kom_BU_IVE[4]&0xFF;
				SPort->PackOut[7] = (*Kom_BU_IVE[4]&0xFF00)>>8;
			}
			else if(ACom == 1) // запись характеристик
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x57;	// W - запись
				SPort->PackOut[2] = 0x0A;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x04;	// последний регистр записи
				SPort->PackOut[6] = *Kom_BU_IVE[0]&0xFF;
				SPort->PackOut[7] = (*Kom_BU_IVE[0]&0xFF00)>>8;
				SPort->PackOut[8] = *Kom_BU_IVE[1]&0xFF;
				SPort->PackOut[9] = (*Kom_BU_IVE[1]&0xFF00)>>8;
				SPort->PackOut[10] = *Kom_BU_IVE[2]&0xFF;
				SPort->PackOut[11] = (*Kom_BU_IVE[2]&0xFF00)>>8;
				SPort->PackOut[12] = *Kom_BU_IVE[3]&0xFF;
				SPort->PackOut[13] = (*Kom_BU_IVE[3]&0xFF00)>>8;
			}
			else if(ACom == 3) // чтение регистров R0x01 - R0x16
			{
				SPort->PackOut[0] = adr;	// адрес
				SPort->PackOut[1] = 0x52;	// R - чтение
				SPort->PackOut[2] = 0x02;	// длина посылки мл.б.
				SPort->PackOut[3] = 0x00;	// длина посылки ст.б.
				SPort->PackOut[4] = 0x01;	// первый регистр записи
				SPort->PackOut[5] = 0x16;	// последний регистр записи					
			}
		}		
	}	
	
	// расчет КС
	unsigned int ksIVE = 0;
	
	Buf_len = SPort->PackOut[2] + 5;   // длина посылки
	
	for(int i=0;i<(Buf_len-1);i++)
		ksIVE = ksIVE + SPort->PackOut[i];
	ksIVE = (0xFF00 - ksIVE) & 0xFF;
	SPort->PackOut[Buf_len-1] = ksIVE;	
	
}
//---------------------------------------------------------------------------
//-- Функция обработки ответа --//
//---------------------------------------------------------------------------
bool S_IVE::BU_IVE_ChkRep(bool Zap_type)
{	
	unsigned int b;

	if(type_BU_IVE == 0)	// БПИИ
	{
		// заполнение ручной страницы
		b = SPort->PackIn[6] + SPort->PackIn[7]*0x100; // зад ток
		b = b * 500 / 0xFFF;
		Edt_Zad_Char[1]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[8] + SPort->PackIn[9]*0x100; // зад напр
		b = b * 3000 / 0xFFF;
		Edt_Zad_Char[0]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[10] + SPort->PackIn[11]*0x100; // зад мощн
		b = b * 1000 / 0xFFF;
		Edt_Zad_Char[2]->Text = FloatToStrF(float(b),ffFixed,6,0);	
		
		b = SPort->PackIn[18] + SPort->PackIn[19]*0x100; // вых напр
		b = b * 3072 / 0x3FF;
		Edt_Tek_Char[0]->Text = FloatToStrF(float(b),ffFixed,6,0);

		b = SPort->PackIn[20] + SPort->PackIn[21]*0x100; // вых ток
		b = b * 512 / 0x3FF;
		Edt_Tek_Char[1]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[36] + SPort->PackIn[37]*0x100; // вых мощн
		b = b * 1024 / 0x3FF;
		Edt_Tek_Char[2]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[46]; // регистр команд мл.б
		CB_Zad[4]->Checked = !bool(b&0x04);	// U/F	

		b = SPort->PackIn[47]; // регистр команд ст.б
        CB_Zad[0]->Checked = bool(b&0x08);	//  DEL
        CB_Zad[2]->Checked = !bool(b&0x10);	//  DEP
        CB_Zad[3]->Checked = !bool(b&0x40);	//  OUT
        CB_Zad[1]->Checked = !bool(b&0x80);	//  DEW		
		
		b = SPort->PackIn[48]; // регистр состояний мл.б
		CB_Err[3]->Checked = bool(b&0x01);
        CB_Err[2]->Checked = !bool(b&0x02);
        CB_Err[1]->Checked = !bool(b&0x04);
        CB_Err[0]->Checked = !bool(b&0x08);
		
		if(!Zap_type) // если ручной запрос - не заполняем массив ответов!
		{
			*Otv_BU_IVE[0] = SPort->PackIn[6] + SPort->PackIn[7]*0x100;
			*Otv_BU_IVE[1] = SPort->PackIn[8] + SPort->PackIn[9]*0x100;
			*Otv_BU_IVE[2] = SPort->PackIn[10] + SPort->PackIn[11]*0x100;
			*Otv_BU_IVE[3] = SPort->PackIn[12] + SPort->PackIn[13]*0x100;
			*Otv_BU_IVE[4] = SPort->PackIn[18] + SPort->PackIn[19]*0x100;
			*Otv_BU_IVE[5] = SPort->PackIn[20] + SPort->PackIn[21]*0x100;
			*Otv_BU_IVE[6] = SPort->PackIn[36] + SPort->PackIn[37]*0x100;
			*Otv_BU_IVE[7] = SPort->PackIn[38] + SPort->PackIn[39]*0x100;
			*Otv_BU_IVE[8] = SPort->PackIn[46] + SPort->PackIn[47]*0x100;
			*Otv_BU_IVE[9] = SPort->PackIn[48] + SPort->PackIn[49]*0x100;		
		}
	}
	
	if(type_BU_IVE == 1)	// БПМ
	{
		// заполнение ручной страницы
		b = SPort->PackIn[6] + SPort->PackIn[7]*0x100; // зад ток
		b = b * 150 / 0xFFF;
		Edt_Zad_Char[1]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);
		
		b = SPort->PackIn[8] + SPort->PackIn[9]*0x100; // зад напр
		b = b * 650 / 0xFFF;
		Edt_Zad_Char[0]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[10] + SPort->PackIn[11]*0x100; // зад мощн
		b = b * 3600 / 0xFFF;
		Edt_Zad_Char[2]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[12] + SPort->PackIn[13]*0x100; // зад част
		b = b * 400 / 0xFF;
		Edt_Zad_Char[3]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);
		
		b = SPort->PackIn[18] + SPort->PackIn[19]*0x100; // вых напр
		b = b * 666 / 0x3FF;
		Edt_Tek_Char[0]->Text = FloatToStrF(float(b),ffFixed,6,0);

		b = SPort->PackIn[20] + SPort->PackIn[21]*0x100; // вых ток
		b = b * 102 / 0x3FF;
		Edt_Tek_Char[1]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);
		
		b = SPort->PackIn[36] + SPort->PackIn[37]*0x100; // вых мощн
		b = b * 3072 / 0x3FF;
		Edt_Tek_Char[2]->Text = FloatToStrF(float(b),ffFixed,6,0);
		
		b = SPort->PackIn[38] + SPort->PackIn[39]*0x100; // част дугозащиты
		b = b * 204 / 0x3FF;
		Edt_Tek_Char[3]->Text = FloatToStrF(float(b)/10.0,ffFixed,6,1);
		
		b = SPort->PackIn[46]; // регистр команд мл.б
		CB_Zad[8]->Checked = !bool(b&0x04);	// U/F

		b = SPort->PackIn[47]; // регистр команд ст.б
        CB_Zad[3]->Checked = !bool(b&0x01);	//  DEF
        CB_Zad[6]->Checked = bool(b&0x02);	//  A1
        CB_Zad[5]->Checked = bool(b&0x04);	//  A2
        CB_Zad[0]->Checked = bool(b&0x08);	//  DEL
        CB_Zad[4]->Checked = !bool(b&0x10);	//  DEP
        CB_Zad[2]->Checked = !bool(b&0x20);	//  DEV
        CB_Zad[7]->Checked = !bool(b&0x40);	//  OUT
        CB_Zad[1]->Checked = !bool(b&0x80);	//  DEW

		b = SPort->PackIn[48]; // регистр состояний мл.б
		CB_Err[4]->Checked = bool(b&0x01);
        CB_Err[3]->Checked = !bool(b&0x02);
        CB_Err[2]->Checked = !bool(b&0x04);
        CB_Err[1]->Checked = !bool(b&0x08);
        CB_Err[0]->Checked = !bool(b&0x10);
		
		if(!Zap_type) // если ручной запрос - не заполняем массив ответов!
		{
			*Otv_BU_IVE[0] = SPort->PackIn[6] + SPort->PackIn[7]*0x100;
			*Otv_BU_IVE[1] = SPort->PackIn[8] + SPort->PackIn[9]*0x100;
			*Otv_BU_IVE[2] = SPort->PackIn[10] + SPort->PackIn[11]*0x100;
			*Otv_BU_IVE[3] = SPort->PackIn[12] + SPort->PackIn[13]*0x100;
			*Otv_BU_IVE[4] = SPort->PackIn[18] + SPort->PackIn[19]*0x100;
			*Otv_BU_IVE[5] = SPort->PackIn[20] + SPort->PackIn[21]*0x100;
			*Otv_BU_IVE[6] = SPort->PackIn[36] + SPort->PackIn[37]*0x100;
			*Otv_BU_IVE[7] = SPort->PackIn[38] + SPort->PackIn[39]*0x100;
			*Otv_BU_IVE[8] = SPort->PackIn[46] + SPort->PackIn[47]*0x100;
			*Otv_BU_IVE[9] = SPort->PackIn[48] + SPort->PackIn[49]*0x100;
		}
	}
}
//---------------------------------------------------------------------------
//-- Функция связи с датчиком --//
//---------------------------------------------------------------------------
bool S_IVE::BU_IVE_Manage(unsigned int SH,bool Zap_type)
{
	// переключение радиобаттонов на страницах датчиков при их работе
	RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

    if(SH == 0) // посылка запроса
	{
		SPort->PackageClear();
		// Функция формирования запроса
		BU_IVE_FrmZap(Zap_type);
		SPort->VisPackRTU(0,Buf_len);

		// очистка буфера приёма
		SPort->Port.ResetRB();
		// отправка посылки
		SPort->Port.Write(SPort->PackOut,Buf_len);
		// переход на следующий шаг
		SPort->DevState++;
		// обнуление времени
		SPort->Dev_Timer = 0;
		return 0;
	}
	else if(SH == 1)
	{
		// чтение ответа
        if(SPort->Port.GetRBSize() < PACKAGE_COUNT)
        {
            SPort->Port.Read(SPort->PackIn,SPort->Port.GetRBSize());
        }
        else
        {
            // очистка буфера приёма
            SPort->Port.ResetRB();
            SPort->DevState++;
            return 0;
        }

		// проверка ответа
		if((SPort->PackIn[0] == adr) &&
			((SPort->PackIn[1] == 0x57)||((SPort->PackIn[1] == 0x52)&&(SPort->PackIn[50] != 0))))
		{
			Buf_len = SPort->PackIn[2] + 5; // длина посылки
			SPort->VisPackRTU(1,Buf_len);
			
			if(Zap_type) // ручной ответ
			{			
				if(RCom == 3) // ответ на чтение регистров
					BU_IVE_ChkRep(Zap_type);
			}
			else // автоматический
			{
				if(ACom == 1) //ответ на запись регистров 1
				{
					pr_zap[0] = true;
				}
				else if(ACom == 2) // ответ на запись регистров 2
				{
					pr_zap[1] = true;					
				}
				else // ответ на чтение регистров
				{
					BU_IVE_ChkRep(Zap_type);
					pr_zap[2] = true;
					
                    if(pr_zap[0]&&pr_zap[1]&&pr_zap[2]&&(*Otv_BU_IVE[8]==*Kom_BU_IVE[4]))
						*Pr_BU_IVE = 1;
				}
			}
			
			// переход на следующий шаг
			SPort->DevState++;
			// сброс счетчика ошибок связи
			Err = 0;
			return 0;
		}
		else
		{
            if(SPort->Dev_Timer < SPort->Dev_TK) return 0; // ждем ответа
			// не дождались
			SPort->DevState++; // переходим к следующему устройству
			// увеличение счетчика ошибок связи и проверяем на максимум
			if((Err++) > Max_err)
				return 1;
			return 0;
		}
    }
}
//---------------------------------------------------------------------------
//-- Инициализация структур --//
//---------------------------------------------------------------------------
void Init_BU_IVE()
{
	for(int i=0;i<IVE_COUNT;i++ )
		BU_IVE[i] = new S_IVE();
	
	// БПМ1
	BU_IVE[0]->name = "БПМ1(ИВЭ144)"; // название для отображения
	BU_IVE[0]->adr = 1;
	BU_IVE[0]->type_BU_IVE = 1;
	BU_IVE[0]->Pr_BU_IVE = &PR_SV_BM1;
	BU_IVE[0]->pr_zap[0] = 0;
	BU_IVE[0]->pr_zap[1] = 0;
	BU_IVE[0]->pr_zap[2] = 0;

	BU_IVE[0]->Kom_BU_IVE[0] = &KOM_BM1[0];
	BU_IVE[0]->Kom_BU_IVE[1] = &KOM_BM1[1];
	BU_IVE[0]->Kom_BU_IVE[2] = &KOM_BM1[2];
	BU_IVE[0]->Kom_BU_IVE[3] = &KOM_BM1[3];
	BU_IVE[0]->Kom_BU_IVE[4] = &KOM_BM1[4];

	BU_IVE[0]->Otv_BU_IVE[0] = &OTVET_BM1[0];
	BU_IVE[0]->Otv_BU_IVE[1] = &OTVET_BM1[1];
	BU_IVE[0]->Otv_BU_IVE[2] = &OTVET_BM1[2];
	BU_IVE[0]->Otv_BU_IVE[3] = &OTVET_BM1[3];
	BU_IVE[0]->Otv_BU_IVE[4] = &OTVET_BM1[4];
	BU_IVE[0]->Otv_BU_IVE[5] = &OTVET_BM1[5];
	BU_IVE[0]->Otv_BU_IVE[6] = &OTVET_BM1[6];
	BU_IVE[0]->Otv_BU_IVE[7] = &OTVET_BM1[7];
	BU_IVE[0]->Otv_BU_IVE[8] = &OTVET_BM1[8];
	BU_IVE[0]->Otv_BU_IVE[9] = &OTVET_BM1[9];

	BU_IVE[0]->SPort = Comport[1];     // порт
	BU_IVE[0]->diagnS_byte = 2;        // номер байта связной диагностики
	BU_IVE[0]->diagnS_mask = 0x04;		// маска байта связной диагностики

	BU_IVE[0]->Err = 0;				// кол-во ошибок
	BU_IVE[0]->Max_err = 5;			// максимум ошибок
	BU_IVE[0]->ACom = 0;				// текущий автоматический запрос
	BU_IVE[0]->RCom = 0;				// текущий ручной апрос
	BU_IVE[0]->Buf_len = 0;			// длина запроса
	BU_IVE[0]->BU_IVE_Gen();

    // БПМ2
	BU_IVE[1]->name = "БПМ2(ИВЭ144)"; // название для отображения
	BU_IVE[1]->adr = 2;
	BU_IVE[1]->type_BU_IVE = 1;
	BU_IVE[1]->Pr_BU_IVE = &PR_SV_BM2;
	BU_IVE[1]->pr_zap[0] = 0;
	BU_IVE[1]->pr_zap[1] = 0;
	BU_IVE[1]->pr_zap[2] = 0;

	BU_IVE[1]->Kom_BU_IVE[0] = &KOM_BM2[0];
	BU_IVE[1]->Kom_BU_IVE[1] = &KOM_BM2[1];
	BU_IVE[1]->Kom_BU_IVE[2] = &KOM_BM2[2];
	BU_IVE[1]->Kom_BU_IVE[3] = &KOM_BM2[3];
	BU_IVE[1]->Kom_BU_IVE[4] = &KOM_BM2[4];

	BU_IVE[1]->Otv_BU_IVE[0] = &OTVET_BM2[0];
	BU_IVE[1]->Otv_BU_IVE[1] = &OTVET_BM2[1];
	BU_IVE[1]->Otv_BU_IVE[2] = &OTVET_BM2[2];
	BU_IVE[1]->Otv_BU_IVE[3] = &OTVET_BM2[3];
	BU_IVE[1]->Otv_BU_IVE[4] = &OTVET_BM2[4];
	BU_IVE[1]->Otv_BU_IVE[5] = &OTVET_BM2[5];
	BU_IVE[1]->Otv_BU_IVE[6] = &OTVET_BM2[6];
	BU_IVE[1]->Otv_BU_IVE[7] = &OTVET_BM2[7];
	BU_IVE[1]->Otv_BU_IVE[8] = &OTVET_BM2[8];
	BU_IVE[1]->Otv_BU_IVE[9] = &OTVET_BM2[9];

	BU_IVE[1]->SPort = Comport[1];     // порт
	BU_IVE[1]->diagnS_byte = 2;        // номер байта связной диагностики
	BU_IVE[1]->diagnS_mask = 0x08;		// маска байта связной диагностики

	BU_IVE[1]->Err = 0;				// кол-во ошибок
	BU_IVE[1]->Max_err = 5;			// максимум ошибок
	BU_IVE[1]->ACom = 0;				// текущий автоматический запрос
	BU_IVE[1]->RCom = 0;				// текущий ручной апрос
	BU_IVE[1]->Buf_len = 0;			// длина запроса
	BU_IVE[1]->BU_IVE_Gen();

    // БПИИ
	BU_IVE[2]->name = "БПИИ(ИВЭ341)"; // название для отображения
	BU_IVE[2]->adr = 3;
	BU_IVE[2]->type_BU_IVE = 0;
	BU_IVE[2]->Pr_BU_IVE = &PR_SV_II;
	BU_IVE[2]->pr_zap[0] = 0;
	BU_IVE[2]->pr_zap[1] = 0;
	BU_IVE[2]->pr_zap[2] = 0;

	BU_IVE[2]->Kom_BU_IVE[0] = &KOM_II[0];
	BU_IVE[2]->Kom_BU_IVE[1] = &KOM_II[1];
	BU_IVE[2]->Kom_BU_IVE[2] = &KOM_II[2];
	BU_IVE[2]->Kom_BU_IVE[3] = &KOM_II[3];
	BU_IVE[2]->Kom_BU_IVE[4] = &KOM_II[4];

	BU_IVE[2]->Otv_BU_IVE[0] = &OTVET_II[0];
	BU_IVE[2]->Otv_BU_IVE[1] = &OTVET_II[1];
	BU_IVE[2]->Otv_BU_IVE[2] = &OTVET_II[2];
	BU_IVE[2]->Otv_BU_IVE[3] = &OTVET_II[3];
	BU_IVE[2]->Otv_BU_IVE[4] = &OTVET_II[4];
	BU_IVE[2]->Otv_BU_IVE[5] = &OTVET_II[5];
	BU_IVE[2]->Otv_BU_IVE[6] = &OTVET_II[6];
	BU_IVE[2]->Otv_BU_IVE[7] = &OTVET_II[7];
	BU_IVE[2]->Otv_BU_IVE[8] = &OTVET_II[8];
	BU_IVE[2]->Otv_BU_IVE[9] = &OTVET_II[9];

	BU_IVE[2]->SPort = Comport[1];     // порт
	BU_IVE[2]->diagnS_byte = 2;        // номер байта связной диагностики
	BU_IVE[2]->diagnS_mask = 0x20;		// маска байта связной диагностики

	BU_IVE[2]->Err = 0;				// кол-во ошибок
	BU_IVE[2]->Max_err = 5;			// максимум ошибок
	BU_IVE[2]->ACom = 0;				// текущий автоматический запрос
	BU_IVE[2]->RCom = 0;				// текущий ручной апрос
	BU_IVE[2]->Buf_len = 0;			// длина запроса
	BU_IVE[2]->BU_IVE_Gen();
}