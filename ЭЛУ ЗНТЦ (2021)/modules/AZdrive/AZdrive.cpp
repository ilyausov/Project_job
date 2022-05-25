//---------------------------------------------------------------------------
#pragma hdrstop
#include "AZdrive.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//-- Генерация страницы --//
//---------------------------------------------------------------------------
void SAZ_drive::AZdrive_Gen()
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
	//Pnl_Parent->Height = 676 - 308;
    Pnl_Parent->Height = 676;
	Pnl_Parent->Width = 1176 - 192;

	Lbl_Uni = new TLabel(Pnl_Parent);
	Lbl_Uni->Parent = Pnl_Parent;
	Lbl_Uni->Top = 15;
	Lbl_Uni->Left = 26;
	Lbl_Uni->Caption = "Адрес устройства: " + IntToStr(adr);
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
	GB_Main->Top = 56 - 10;
	GB_Main->Left = 26;
    GB_Main->Width = 500;
	GB_Main->Height = 360 - 130;
	GB_Main->Caption = " Настройки ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 152 - 120;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Ускорение";
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
	Lbl_Uni->Top = 184 - 120;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Торможение";
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
	Lbl_Uni->Top = 216 - 120;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Скорость ZHome";
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
	Lbl_Uni->Top = 248 - 120;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Уск./торм. ZHome";
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
	Lbl_Uni->Top = 280 - 120;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Нач. скор. ZHome";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Edt_Tek_acc = new TEdit(GB_Main);
	Edt_Tek_acc->Parent = GB_Main;
	Edt_Tek_acc->Top = 150 - 120;
	Edt_Tek_acc->Left = 192;
	Edt_Tek_acc->Font->Name = "Arial";
	Edt_Tek_acc->Font->Size = 13;
	Edt_Tek_acc->Font->Color = clBlack;
	Edt_Tek_acc->Font->Style = Edt_Tek_acc->Font->Style >> fsBold;
	Edt_Tek_acc->BevelKind = bkFlat;
	Edt_Tek_acc->BevelOuter = bvRaised;
	Edt_Tek_acc->BorderStyle = bsNone;
	Edt_Tek_acc->ReadOnly = true;
	Edt_Tek_acc->Color = 0x008080FF;
	Edt_Tek_acc->Height = 26;
	Edt_Tek_acc->Width = 90;
	
	Edt_Tek_dec = new TEdit(GB_Main);
	Edt_Tek_dec->Parent = GB_Main;
	Edt_Tek_dec->Top = 182 - 120;
	Edt_Tek_dec->Left = 192;
	Edt_Tek_dec->Font->Name = "Arial";
	Edt_Tek_dec->Font->Size = 13;
	Edt_Tek_dec->Font->Color = clBlack;
	Edt_Tek_dec->Font->Style = Edt_Tek_dec->Font->Style >> fsBold;
	Edt_Tek_dec->BevelKind = bkFlat;
	Edt_Tek_dec->BevelOuter = bvRaised;
	Edt_Tek_dec->BorderStyle = bsNone;
	Edt_Tek_dec->ReadOnly = true;
	Edt_Tek_dec->Color = 0x008080FF;
	Edt_Tek_dec->Height = 26;
	Edt_Tek_dec->Width = 90;
	
	Edt_Tek_ZHspd = new TEdit(GB_Main);
	Edt_Tek_ZHspd->Parent = GB_Main;
	Edt_Tek_ZHspd->Top = 214 - 120;
	Edt_Tek_ZHspd->Left = 192;
	Edt_Tek_ZHspd->Font->Name = "Arial";
	Edt_Tek_ZHspd->Font->Size = 13;
	Edt_Tek_ZHspd->Font->Color = clBlack;
	Edt_Tek_ZHspd->Font->Style = Edt_Tek_ZHspd->Font->Style >> fsBold;
	Edt_Tek_ZHspd->BevelKind = bkFlat;
	Edt_Tek_ZHspd->BevelOuter = bvRaised;
	Edt_Tek_ZHspd->BorderStyle = bsNone;
	Edt_Tek_ZHspd->ReadOnly = true;
	Edt_Tek_ZHspd->Color = 0x008080FF;
	Edt_Tek_ZHspd->Height = 26;
	Edt_Tek_ZHspd->Width = 90;
	
	Edt_Tek_ZHacc = new TEdit(GB_Main);
	Edt_Tek_ZHacc->Parent = GB_Main;
	Edt_Tek_ZHacc->Top = 246 - 120;
	Edt_Tek_ZHacc->Left = 192;
	Edt_Tek_ZHacc->Font->Name = "Arial";
	Edt_Tek_ZHacc->Font->Size = 13;
	Edt_Tek_ZHacc->Font->Color = clBlack;
	Edt_Tek_ZHacc->Font->Style = Edt_Tek_ZHacc->Font->Style >> fsBold;
	Edt_Tek_ZHacc->BevelKind = bkFlat;
	Edt_Tek_ZHacc->BevelOuter = bvRaised;
	Edt_Tek_ZHacc->BorderStyle = bsNone;
	Edt_Tek_ZHacc->ReadOnly = true;
	Edt_Tek_ZHacc->Color = 0x008080FF;
	Edt_Tek_ZHacc->Height = 26;
	Edt_Tek_ZHacc->Width = 90;
	
	Edt_Tek_ZHnsp = new TEdit(GB_Main);
	Edt_Tek_ZHnsp->Parent = GB_Main;
	Edt_Tek_ZHnsp->Top = 278 - 120;
	Edt_Tek_ZHnsp->Left = 192;
	Edt_Tek_ZHnsp->Font->Name = "Arial";
	Edt_Tek_ZHnsp->Font->Size = 13;
	Edt_Tek_ZHnsp->Font->Color = clBlack;
	Edt_Tek_ZHnsp->Font->Style = Edt_Tek_ZHnsp->Font->Style >> fsBold;
	Edt_Tek_ZHnsp->BevelKind = bkFlat;
	Edt_Tek_ZHnsp->BevelOuter = bvRaised;
	Edt_Tek_ZHnsp->BorderStyle = bsNone;
	Edt_Tek_ZHnsp->ReadOnly = true;
	Edt_Tek_ZHnsp->Color = 0x008080FF;
	Edt_Tek_ZHnsp->Height = 26;
	Edt_Tek_ZHnsp->Width = 90;
	
	Edt_Rem_acc = new TEdit(GB_Main);
	Edt_Rem_acc->Parent = GB_Main;
	Edt_Rem_acc->Top = 150 - 120;
	Edt_Rem_acc->Left = 288;
	Edt_Rem_acc->Font->Name = "Arial";
	Edt_Rem_acc->Font->Size = 13;
	Edt_Rem_acc->Font->Color = clBlack;
	Edt_Rem_acc->Font->Style = Edt_Rem_acc->Font->Style >> fsBold;
	Edt_Rem_acc->BevelKind = bkFlat;
	Edt_Rem_acc->BevelOuter = bvRaised;
	Edt_Rem_acc->BorderStyle = bsNone;
	Edt_Rem_acc->ReadOnly = true;
	Edt_Rem_acc->Color = clSkyBlue;
	Edt_Rem_acc->Height = 26;
	Edt_Rem_acc->Width = 90;
	
	Edt_Rem_dec = new TEdit(GB_Main);
	Edt_Rem_dec->Parent = GB_Main;
	Edt_Rem_dec->Top = 182 - 120;
	Edt_Rem_dec->Left = 288;
	Edt_Rem_dec->Font->Name = "Arial";
	Edt_Rem_dec->Font->Size = 13;
	Edt_Rem_dec->Font->Color = clBlack;
	Edt_Rem_dec->Font->Style = Edt_Rem_dec->Font->Style >> fsBold;
	Edt_Rem_dec->BevelKind = bkFlat;
	Edt_Rem_dec->BevelOuter = bvRaised;
	Edt_Rem_dec->BorderStyle = bsNone;
	Edt_Rem_dec->ReadOnly = true;
	Edt_Rem_dec->Color = clSkyBlue;
	Edt_Rem_dec->Height = 26;
	Edt_Rem_dec->Width = 90;
	
	Edt_Rem_ZHspd = new TEdit(GB_Main);
	Edt_Rem_ZHspd->Parent = GB_Main;
	Edt_Rem_ZHspd->Top = 214 - 120;
	Edt_Rem_ZHspd->Left = 288;
	Edt_Rem_ZHspd->Font->Name = "Arial";
	Edt_Rem_ZHspd->Font->Size = 13;
	Edt_Rem_ZHspd->Font->Color = clBlack;
	Edt_Rem_ZHspd->Font->Style = Edt_Rem_ZHspd->Font->Style >> fsBold;
	Edt_Rem_ZHspd->BevelKind = bkFlat;
	Edt_Rem_ZHspd->BevelOuter = bvRaised;
	Edt_Rem_ZHspd->BorderStyle = bsNone;
	Edt_Rem_ZHspd->ReadOnly = true;
	Edt_Rem_ZHspd->Color = clSkyBlue;
	Edt_Rem_ZHspd->Height = 26;
	Edt_Rem_ZHspd->Width = 90;
	
	Edt_Rem_ZHacc = new TEdit(GB_Main);
	Edt_Rem_ZHacc->Parent = GB_Main;
	Edt_Rem_ZHacc->Top = 246 - 120;
	Edt_Rem_ZHacc->Left = 288;
	Edt_Rem_ZHacc->Font->Name = "Arial";
	Edt_Rem_ZHacc->Font->Size = 13;
	Edt_Rem_ZHacc->Font->Color = clBlack;
	Edt_Rem_ZHacc->Font->Style = Edt_Rem_ZHacc->Font->Style >> fsBold;
	Edt_Rem_ZHacc->BevelKind = bkFlat;
	Edt_Rem_ZHacc->BevelOuter = bvRaised;
	Edt_Rem_ZHacc->BorderStyle = bsNone;
	Edt_Rem_ZHacc->ReadOnly = true;
	Edt_Rem_ZHacc->Color = clSkyBlue;
	Edt_Rem_ZHacc->Height = 26;
	Edt_Rem_ZHacc->Width = 90;
	
	Edt_Rem_ZHnsp = new TEdit(GB_Main);
	Edt_Rem_ZHnsp->Parent = GB_Main;
	Edt_Rem_ZHnsp->Top = 278 - 120;
	Edt_Rem_ZHnsp->Left = 288;
	Edt_Rem_ZHnsp->Font->Name = "Arial";
	Edt_Rem_ZHnsp->Font->Size = 13;
	Edt_Rem_ZHnsp->Font->Color = clBlack;
	Edt_Rem_ZHnsp->Font->Style = Edt_Rem_ZHnsp->Font->Style >> fsBold;
	Edt_Rem_ZHnsp->BevelKind = bkFlat;
	Edt_Rem_ZHnsp->BevelOuter = bvRaised;
	Edt_Rem_ZHnsp->BorderStyle = bsNone;
	Edt_Rem_ZHnsp->ReadOnly = true;
	Edt_Rem_ZHnsp->Color = clSkyBlue;
	Edt_Rem_ZHnsp->Height = 26;
	Edt_Rem_ZHnsp->Width = 90;
	
	Edt_Vvod_acc = new TEdit(GB_Main);
	Edt_Vvod_acc->Parent = GB_Main;
	Edt_Vvod_acc->Top = 150 - 120;
	Edt_Vvod_acc->Left = 384;
	Edt_Vvod_acc->Font->Name = "Arial";
	Edt_Vvod_acc->Font->Size = 13;
	Edt_Vvod_acc->Font->Color = clBlack;
	Edt_Vvod_acc->Font->Style = Edt_Vvod_acc->Font->Style >> fsBold;
	Edt_Vvod_acc->BevelKind = bkFlat;
	Edt_Vvod_acc->BevelOuter = bvRaised;
	Edt_Vvod_acc->BorderStyle = bsNone;
	Edt_Vvod_acc->ReadOnly = false;
	Edt_Vvod_acc->Color = clWhite;
	Edt_Vvod_acc->Height = 26;
	Edt_Vvod_acc->Width = 90;
	Edt_Vvod_acc->MaxLength = 9;
	Edt_Vvod_acc->Text = "1";
	Edt_Vvod_acc->OnExit = ChkVVdata;
	
	Edt_Vvod_dec = new TEdit(GB_Main);
	Edt_Vvod_dec->Parent = GB_Main;
	Edt_Vvod_dec->Top = 182 - 120;
	Edt_Vvod_dec->Left = 384;
	Edt_Vvod_dec->Font->Name = "Arial";
	Edt_Vvod_dec->Font->Size = 13;
	Edt_Vvod_dec->Font->Color = clBlack;
	Edt_Vvod_dec->Font->Style = Edt_Vvod_dec->Font->Style >> fsBold;
	Edt_Vvod_dec->BevelKind = bkFlat;
	Edt_Vvod_dec->BevelOuter = bvRaised;
	Edt_Vvod_dec->BorderStyle = bsNone;
	Edt_Vvod_dec->ReadOnly = false;
	Edt_Vvod_dec->Color = clWhite;
	Edt_Vvod_dec->Height = 26;
	Edt_Vvod_dec->Width = 90;
	Edt_Vvod_dec->MaxLength = 9;
	Edt_Vvod_dec->Text = "1";
	Edt_Vvod_dec->OnExit = ChkVVdata;
	
	Edt_Vvod_ZHspd = new TEdit(GB_Main);
	Edt_Vvod_ZHspd->Parent = GB_Main;
	Edt_Vvod_ZHspd->Top = 214 - 120;
	Edt_Vvod_ZHspd->Left = 384;
	Edt_Vvod_ZHspd->Font->Name = "Arial";
	Edt_Vvod_ZHspd->Font->Size = 13;
	Edt_Vvod_ZHspd->Font->Color = clBlack;
	Edt_Vvod_ZHspd->Font->Style = Edt_Vvod_ZHspd->Font->Style >> fsBold;
	Edt_Vvod_ZHspd->BevelKind = bkFlat;
	Edt_Vvod_ZHspd->BevelOuter = bvRaised;
	Edt_Vvod_ZHspd->BorderStyle = bsNone;
	Edt_Vvod_ZHspd->ReadOnly = false;
	Edt_Vvod_ZHspd->Color = clWhite;
	Edt_Vvod_ZHspd->Height = 26;
	Edt_Vvod_ZHspd->Width = 90;
	Edt_Vvod_ZHspd->MaxLength = 9;
	Edt_Vvod_ZHspd->Text = "1";
	Edt_Vvod_ZHspd->OnExit = ChkVVdata;
	
	Edt_Vvod_ZHacc = new TEdit(GB_Main);
	Edt_Vvod_ZHacc->Parent = GB_Main;
	Edt_Vvod_ZHacc->Top = 246 - 120;
	Edt_Vvod_ZHacc->Left = 384;
	Edt_Vvod_ZHacc->Font->Name = "Arial";
	Edt_Vvod_ZHacc->Font->Size = 13;
	Edt_Vvod_ZHacc->Font->Color = clBlack;
	Edt_Vvod_ZHacc->Font->Style = Edt_Vvod_ZHacc->Font->Style >> fsBold;
	Edt_Vvod_ZHacc->BevelKind = bkFlat;
	Edt_Vvod_ZHacc->BevelOuter = bvRaised;
	Edt_Vvod_ZHacc->BorderStyle = bsNone;
	Edt_Vvod_ZHacc->ReadOnly = false;
	Edt_Vvod_ZHacc->Color = clWhite;
	Edt_Vvod_ZHacc->Height = 26;
	Edt_Vvod_ZHacc->Width = 90;
	Edt_Vvod_ZHacc->MaxLength = 9;
	Edt_Vvod_ZHacc->Text = "1";
	Edt_Vvod_ZHacc->OnExit = ChkVVdata;
	
	Edt_Vvod_ZHnsp = new TEdit(GB_Main);
	Edt_Vvod_ZHnsp->Parent = GB_Main;
	Edt_Vvod_ZHnsp->Top = 278 - 120;
	Edt_Vvod_ZHnsp->Left = 384;
	Edt_Vvod_ZHnsp->Font->Name = "Arial";
	Edt_Vvod_ZHnsp->Font->Size = 13;
	Edt_Vvod_ZHnsp->Font->Color = clBlack;
	Edt_Vvod_ZHnsp->Font->Style = Edt_Vvod_ZHnsp->Font->Style >> fsBold;
	Edt_Vvod_ZHnsp->BevelKind = bkFlat;
	Edt_Vvod_ZHnsp->BevelOuter = bvRaised;
	Edt_Vvod_ZHnsp->BorderStyle = bsNone;
	Edt_Vvod_ZHnsp->ReadOnly = false;
	Edt_Vvod_ZHnsp->Color = clWhite;
	Edt_Vvod_ZHnsp->Height = 26;
	Edt_Vvod_ZHnsp->Width = 90;
	Edt_Vvod_ZHnsp->MaxLength = 9;
	Edt_Vvod_ZHnsp->Text = "1";
	Edt_Vvod_ZHnsp->OnExit = ChkVVdata;
	
	Btn_Vvod_Par = new TButton(GB_Main);
	Btn_Vvod_Par->Parent = GB_Main;
	Btn_Vvod_Par->Top = 312 - 120;
	Btn_Vvod_Par->Left = 384;
	Btn_Vvod_Par->Font->Name = "Arial";
	Btn_Vvod_Par->Font->Size = 12;
	Btn_Vvod_Par->Font->Color = clBlack;
	Btn_Vvod_Par->Font->Style = Btn_Vvod_Par->Font->Style >> fsBold;
	Btn_Vvod_Par->Caption = "Запись";
	Btn_Vvod_Par->Width = 90;
	Btn_Vvod_Par->Height = 26;
	Btn_Vvod_Par->OnClick = SetPar;

    GB_Spd = new TGroupBox(Form1);
	GB_Spd->Parent = Pnl_Parent;
	GB_Spd->Top = 56 - 10;
	GB_Spd->Left = 552;
    GB_Spd->Width = 354;
	GB_Spd->Height = 186 - 20;
	GB_Spd->Caption = " Скорости ";
	GB_Spd->Font->Name = "Arial";
	GB_Spd->Font->Size = 12;
	GB_Spd->Font->Color = clBlack;
	GB_Spd->Font->Style = GB_Spd->Font->Style << fsBold;

    Lbl_Uni = new TLabel(GB_Spd);
	Lbl_Uni->Parent = GB_Spd;
	Lbl_Uni->Top = 40 - 10;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Большая";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Spd);
	Lbl_Uni->Parent = GB_Spd;
	Lbl_Uni->Top = 72 - 10;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Малая";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Spd);
	Lbl_Uni->Parent = GB_Spd;
	Lbl_Uni->Top = 104 - 10;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Ползущая";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Edt_Rem_spdB = new TEdit(GB_Spd);
	Edt_Rem_spdB->Parent = GB_Spd;
	Edt_Rem_spdB->Top = 40 - 10;
	Edt_Rem_spdB->Left = 144;
	Edt_Rem_spdB->Font->Name = "Arial";
	Edt_Rem_spdB->Font->Size = 13;
	Edt_Rem_spdB->Font->Color = clBlack;
	Edt_Rem_spdB->Font->Style = Edt_Rem_spdB->Font->Style >> fsBold;
	Edt_Rem_spdB->BevelKind = bkFlat;
	Edt_Rem_spdB->BevelOuter = bvRaised;
	Edt_Rem_spdB->BorderStyle = bsNone;
	Edt_Rem_spdB->ReadOnly = true;
	Edt_Rem_spdB->Color = clSkyBlue;
	Edt_Rem_spdB->Height = 26;
	Edt_Rem_spdB->Width = 90;
	
	Edt_Rem_spdM = new TEdit(GB_Spd);
	Edt_Rem_spdM->Parent = GB_Spd;
	Edt_Rem_spdM->Top = 72 - 10;
	Edt_Rem_spdM->Left = 144;
	Edt_Rem_spdM->Font->Name = "Arial";
	Edt_Rem_spdM->Font->Size = 13;
	Edt_Rem_spdM->Font->Color = clBlack;
	Edt_Rem_spdM->Font->Style = Edt_Rem_spdM->Font->Style >> fsBold;
	Edt_Rem_spdM->BevelKind = bkFlat;
	Edt_Rem_spdM->BevelOuter = bvRaised;
	Edt_Rem_spdM->BorderStyle = bsNone;
	Edt_Rem_spdM->ReadOnly = true;
	Edt_Rem_spdM->Color = clSkyBlue;
	Edt_Rem_spdM->Height = 26;
	Edt_Rem_spdM->Width = 90;
	
	Edt_Rem_spdP = new TEdit(GB_Spd);
	Edt_Rem_spdP->Parent = GB_Spd;
	Edt_Rem_spdP->Top = 104 - 10;
	Edt_Rem_spdP->Left = 144;
	Edt_Rem_spdP->Font->Name = "Arial";
	Edt_Rem_spdP->Font->Size = 13;
	Edt_Rem_spdP->Font->Color = clBlack;
	Edt_Rem_spdP->Font->Style = Edt_Rem_spdP->Font->Style >> fsBold;
	Edt_Rem_spdP->BevelKind = bkFlat;
	Edt_Rem_spdP->BevelOuter = bvRaised;
	Edt_Rem_spdP->BorderStyle = bsNone;
	Edt_Rem_spdP->ReadOnly = true;
	Edt_Rem_spdP->Color = clSkyBlue;
	Edt_Rem_spdP->Height = 26;
	Edt_Rem_spdP->Width = 90;
	
	Edt_Vvod_spdB = new TEdit(GB_Spd);
	Edt_Vvod_spdB->Parent = GB_Spd;
	Edt_Vvod_spdB->Top = 40 - 10;
	Edt_Vvod_spdB->Left = 240;
	Edt_Vvod_spdB->Font->Name = "Arial";
	Edt_Vvod_spdB->Font->Size = 13;
	Edt_Vvod_spdB->Font->Color = clBlack;
	Edt_Vvod_spdB->Font->Style = Edt_Vvod_spdB->Font->Style >> fsBold;
	Edt_Vvod_spdB->BevelKind = bkFlat;
	Edt_Vvod_spdB->BevelOuter = bvRaised;
	Edt_Vvod_spdB->BorderStyle = bsNone;
	Edt_Vvod_spdB->ReadOnly = false;
	Edt_Vvod_spdB->Color = clWhite;
	Edt_Vvod_spdB->Height = 26;
	Edt_Vvod_spdB->Width = 90;
	Edt_Vvod_spdB->MaxLength = 9;
	Edt_Vvod_spdB->Text = "1";
	Edt_Vvod_spdB->OnExit = ChkVVdata;
	
	Edt_Vvod_spdM = new TEdit(GB_Spd);
	Edt_Vvod_spdM->Parent = GB_Spd;
	Edt_Vvod_spdM->Top = 72 - 10;
	Edt_Vvod_spdM->Left = 240;
	Edt_Vvod_spdM->Font->Name = "Arial";
	Edt_Vvod_spdM->Font->Size = 13;
	Edt_Vvod_spdM->Font->Color = clBlack;
	Edt_Vvod_spdM->Font->Style = Edt_Vvod_spdM->Font->Style >> fsBold;
	Edt_Vvod_spdM->BevelKind = bkFlat;
	Edt_Vvod_spdM->BevelOuter = bvRaised;
	Edt_Vvod_spdM->BorderStyle = bsNone;
	Edt_Vvod_spdM->ReadOnly = false;
	Edt_Vvod_spdM->Color = clWhite;
	Edt_Vvod_spdM->Height = 26;
	Edt_Vvod_spdM->Width = 90;
	Edt_Vvod_spdM->MaxLength = 9;
	Edt_Vvod_spdM->Text = "1";
	Edt_Vvod_spdM->OnExit = ChkVVdata;
	
	Edt_Vvod_spdP = new TEdit(GB_Spd);
	Edt_Vvod_spdP->Parent = GB_Spd;
	Edt_Vvod_spdP->Top = 104 - 10;
	Edt_Vvod_spdP->Left = 240;
	Edt_Vvod_spdP->Font->Name = "Arial";
	Edt_Vvod_spdP->Font->Size = 13;
	Edt_Vvod_spdP->Font->Color = clBlack;
	Edt_Vvod_spdP->Font->Style = Edt_Vvod_spdP->Font->Style >> fsBold;
	Edt_Vvod_spdP->BevelKind = bkFlat;
	Edt_Vvod_spdP->BevelOuter = bvRaised;
	Edt_Vvod_spdP->BorderStyle = bsNone;
	Edt_Vvod_spdP->ReadOnly = false;
	Edt_Vvod_spdP->Color = clWhite;
	Edt_Vvod_spdP->Height = 26;
	Edt_Vvod_spdP->Width = 90;
	Edt_Vvod_spdP->MaxLength = 9;
	Edt_Vvod_spdP->Text = "1";
	Edt_Vvod_spdP->OnExit = ChkVVdata;
	
	Btn_Vvod_Spd = new TButton(GB_Spd);
	Btn_Vvod_Spd->Parent = GB_Spd;
	Btn_Vvod_Spd->Top = 138 - 10;
	Btn_Vvod_Spd->Left = 240;
	Btn_Vvod_Spd->Font->Name = "Arial";
	Btn_Vvod_Spd->Font->Size = 12;
	Btn_Vvod_Spd->Font->Color = clBlack;
	Btn_Vvod_Spd->Font->Style = Btn_Vvod_Spd->Font->Style >> fsBold;
	Btn_Vvod_Spd->Caption = "Запись";
	Btn_Vvod_Spd->Width = 90;
	Btn_Vvod_Spd->Height = 26;
	Btn_Vvod_Spd->OnClick = SetSpd;

    // 0x0080FF80 - зеленый
    // 0x008080FF - красный

    Data_Table = new TStringGrid(Form1);
    Data_Table->Parent = Pnl_Parent;
	Data_Table->Top = 282;
	Data_Table->Left = 26;
	Data_Table->Font->Name = "Arial";
	Data_Table->Font->Size = 12;
	Data_Table->Font->Color = clBlack;
	Data_Table->Font->Style = Data_Table->Font->Style >> fsBold;
	Data_Table->Width = 829 + 100;
	Data_Table->Height = 380;
    Data_Table->RowCount = 257;
    Data_Table->ColCount = 8;
    Data_Table->DefaultColWidth = 120;
    Data_Table->ColWidths[0] = 60;

    Data_Table->Cells[0][0] = " ";
    Data_Table->Cells[1][0] = " Тип";
    Data_Table->Cells[2][0] = " Позиция";
    Data_Table->Cells[3][0] = " Скорость";
    Data_Table->Cells[4][0] = " Ускорение";
    Data_Table->Cells[5][0] = " Торможение";
    Data_Table->Cells[6][0] = " Link";
    Data_Table->Cells[7][0] = " Next Data";

    for(int i=0;i<256;i++)
    Data_Table->Cells[0][i+1] = " #" + IntToStr(i);

}
//---------------------------------------------------------------------------
void __fastcall SAZ_drive::ChkVVdata(TObject *Sender) 		// контроль ввода параметров
{
	//
    AnsiString
        text = ((TEdit*)Sender)->Text;
    float
        valueText = StrToFloat(text);

	// ускорения
	if( (((TEdit*)Sender) == Edt_Vvod_acc )||
        (((TEdit*)Sender) == Edt_Vvod_dec)||
        (((TEdit*)Sender) == Edt_Vvod_ZHacc))
    {
        if(valueText < 1.0)
        {
            valueText = 1.0;
        }
        else if(valueText > 1000000000.0)
        {
            valueText = 1000000000.0;
        }
    }
	// скорости
	else if((((TEdit*)Sender) == Edt_Vvod_ZHspd )||
        (((TEdit*)Sender) == Edt_Vvod_ZHnsp)||
		(((TEdit*)Sender) == Edt_Vvod_spdB )||
        (((TEdit*)Sender) == Edt_Vvod_spdM)||
        (((TEdit*)Sender) == Edt_Vvod_spdP))
	{
        if(valueText < 1.0)
        {
            valueText = 1.0;
        }
        else if(valueText > 4000000.0)
        {
            valueText = 4000000.0;
        }
    }

    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 10, 0);
}
//---------------------------------------------------------------------------
void __fastcall SAZ_drive::SetPar(TObject *Sender) 			// ввод настроек
{
	//
	data_mech.set_mech[0] = StrToInt(Edt_Vvod_acc -> Text); // текущее ускорение (запомненое)
	data_mech.set_mech[1] = StrToInt(Edt_Vvod_dec -> Text); // текущее торможение (запомненое)
	data_mech.set_mech[2] = StrToInt(Edt_Vvod_ZHspd -> Text); // текущее скорость ZHome (запомненое)
	data_mech.set_mech[3] = StrToInt(Edt_Vvod_ZHacc -> Text); // текущее ускорение ZHome (запомненое)
	data_mech.set_mech[4] = StrToInt(Edt_Vvod_ZHnsp -> Text); // текущее нач.скорость ZHome (запомненое)

	Visual_AZdrive();
	AZdrive_Save();	// сохранение данных	
}
//---------------------------------------------------------------------------
void __fastcall SAZ_drive::SetSpd(TObject *Sender) 			// ввод скоростей
{
	//
	data_mech.v_mech[0] = StrToInt(Edt_Vvod_spdB -> Text); // скорость большая (запомненое)
	data_mech.v_mech[1] = StrToInt(Edt_Vvod_spdM -> Text); // скорость малая (запомненое)
	data_mech.v_mech[2] = StrToInt(Edt_Vvod_spdP -> Text); // скорость ползущая (запомненое)
	
	Visual_AZdrive();
	AZdrive_Save();	// сохранение данных	
}
//------------------------------------//
//--Расчет контрольной суммы--//
//------------------------------------//
unsigned int SAZ_drive::AZ_getKS( unsigned char data[], int n)
{ 
	int s,j,t,i;

	s = 0xFFFF;

	for(i=0;i<n;i=i+1)
	{
		t = data[i];
		s = t ^ s;

		for(j=0;j<8;j=j+1)
		{ 
			if(s & 0x0001 == 1) 
			{ 
				s = s >> 1;
				s = s^0xA001;
			}
			else s = s >> 1;
		}
	}
	return s;
}
//-------------------------------------//
void SAZ_drive::AZ_ZapQuery()                             // определение задачи
{
    // определяем какой запрос необходимо послать
    if(!PR_PERPAR)
    {
        for(int i=1;i<=Max_Op_Data;i++)
        {
            if(Data_err[i])
            {
                if(ACom == 2) ACom = 1;
                else ACom = 2;
				ANum = i;
                AZ_FrmZap(i);
                return;
            }
        }
    // PR_PERPAR = 1;
    }
    else if(*Kom_AZ && !(*Home_AZ) && !Data_errZH && !Data_err[0])	// нужно движение не в HOME и нет ошибок
	{
		if(ACom == 2) ACom = 1; 	// после передачи проверяем запись
		else ACom = 2;				// или посылаем заново
	}
	else if(Data_errZH)				// есть ошибки во втором массиве данных
	{
		if(ACom == 6) ACom = 5; 	// после передачи проверяем запись
		else ACom = 6;				// или посылаем заново
	}
	else if(*Pr_AZ)
	{
		ACom = 0;					// переходим на опрос
	}
	else
	{
		if(ACom == 0) ACom = 6;		// опрашиваем регистры для контроля
		else ACom = 0;				//
	}

    Zad_Op_data[0][0] = *Type_AZ;
    Zad_Op_data[0][1] = *Put_AZ;
    Zad_Op_data[0][2] = *V_AZ;
    Zad_Op_data[0][3] = data_mech.set_mech[0];
    Zad_Op_data[0][4] = data_mech.set_mech[1];
	Zad_Op_data[0][5] = 1000;
	Zad_Op_data[0][6] = 0;
	Zad_Op_data[0][7] = 0;
	Zad_Op_data[0][8] = -256;

	ANum = 0;
    AZ_FrmZap(0);
}
//-------------------------------------//
//----- Формирование запроса к манипулятора -----//
//-------------------------------------//
void SAZ_drive::AZ_FrmZap(unsigned char data_num)
{
	// Формирование запроса ( прямо в массиве )
	if(ACom == 1)
	{
		AZ_Req_Buf_1[0] = adr;

        unsigned int b1,b2,b3,b4;
		int TMP = 0x1800 + data_num*0x40;
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		AZ_Req_Buf_1[3] = b1;
		AZ_Req_Buf_1[2] = b2;
		
		AZ_Req_Buf_1[10] = Zad_Op_data[data_num][0];

		TMP = Zad_Op_data[data_num][1];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[14] = b1;
		AZ_Req_Buf_1[13]  = b2;
		AZ_Req_Buf_1[12]  = b3;
		AZ_Req_Buf_1[11]  = b4;

		TMP = Zad_Op_data[data_num][2];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[18] = b1;
		AZ_Req_Buf_1[17]  = b2;
		AZ_Req_Buf_1[16]  = b3;
		AZ_Req_Buf_1[15]  = b4;
		
		TMP = Zad_Op_data[data_num][3];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[22] = b1;
		AZ_Req_Buf_1[21]  = b2;
		AZ_Req_Buf_1[20]  = b3;
		AZ_Req_Buf_1[19]  = b4;
		
		TMP = Zad_Op_data[data_num][4];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[26] = b1;
		AZ_Req_Buf_1[25]  = b2;
		AZ_Req_Buf_1[24]  = b3;
		AZ_Req_Buf_1[23]  = b4;
		
		TMP = 1000;
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[30] = b1;
		AZ_Req_Buf_1[29]  = b2;
		AZ_Req_Buf_1[28]  = b3;
		AZ_Req_Buf_1[27]  = b4;
		
		TMP = 0;
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[34] = b1;
		AZ_Req_Buf_1[33]  = b2;
		AZ_Req_Buf_1[32]  = b3;
		AZ_Req_Buf_1[31]  = b4;
		
		TMP = Zad_Op_data[data_num][7];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[38] = b1;
		AZ_Req_Buf_1[37]  = b2;
		AZ_Req_Buf_1[36]  = b3;
		AZ_Req_Buf_1[35]  = b4;
		
		TMP = Zad_Op_data[data_num][8];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_1[42] = b1;
		AZ_Req_Buf_1[41]  = b2;
		AZ_Req_Buf_1[40]  = b3;
		AZ_Req_Buf_1[39]  = b4;
		
		Buf_len = 43;
		
		for(int i=0;i<Buf_len;i++)
		SPort->PackOut[i] = AZ_Req_Buf_1[i];
	}
	else if(ACom == 5)
	{
		AZ_Req_Buf_5[0] = adr;
		
		unsigned int b1,b2,b3,b4;
		int TMP = data_mech.set_mech[2];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_5[10] = b1;
		AZ_Req_Buf_5[9]  = b2;
		AZ_Req_Buf_5[8]  = b3;
		AZ_Req_Buf_5[7]  = b4;

		TMP = data_mech.set_mech[3];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;			
		AZ_Req_Buf_5[14] = b1;
		AZ_Req_Buf_5[13]  = b2;
		AZ_Req_Buf_5[12]  = b3;
		AZ_Req_Buf_5[11]  = b4;
			
		TMP = data_mech.set_mech[4];
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		b3 = TMP >> 16; b3 = b3 & 0xFF;
		b4 = TMP >> 24; b4 = b4 & 0xFF;
		AZ_Req_Buf_5[18] = b1;
		AZ_Req_Buf_5[17]  = b2;
		AZ_Req_Buf_5[16]  = b3;
		AZ_Req_Buf_5[15]  = b4;
			
		Buf_len = 19;
		
		for(int i=0;i<Buf_len;i++)
		SPort->PackOut[i] = AZ_Req_Buf_5[i];		
	}
	else if(ACom == 0)
	{
		AZ_Req_Buf_0[0] = adr;

		Buf_len = 6;
		
		for(int i=0;i<Buf_len;i++)
		SPort->PackOut[i] = AZ_Req_Buf_0[i];
	}
	else if(ACom == 2)
	{
		AZ_Req_Buf_2[0] = adr;
		
		unsigned int b1,b2;
        int TMP = 0x1800 + data_num*0x40;
		b1 = TMP & 0xFF;
		b2 = TMP >> 8;  b2 = b2 & 0xFF;
		AZ_Req_Buf_2[3] = b1;
		AZ_Req_Buf_2[2] = b2;

		Buf_len = 6;
		for(int i=0;i<Buf_len;i++)
		SPort->PackOut[i] = AZ_Req_Buf_2[i];
	}
	else if(ACom == 6)
	{
		AZ_Req_Buf_6[0] = adr;

		Buf_len = 6;
		
		for(int i=0;i<Buf_len;i++)
		SPort->PackOut[i] = AZ_Req_Buf_6[i];
	}
	
	int KS = 0;
	KS = AZ_getKS(SPort->PackOut,Buf_len);
	SPort->PackOut[Buf_len + 1] = KS >> 8;
	SPort->PackOut[Buf_len] = KS & 0xFF;
		
	Buf_len = Buf_len + 2;
}
//---------------------------------------------------------------------------
//-- Обработка ответа от манипулятора --//
//---------------------------------------------------------------------------
void SAZ_drive::AZ_ChkRep(unsigned char data_num)
{
	if(ACom == 0) // запрос текущего положения
	{
		// текущая позиция
		*Abs_pos = SPort->PackIn[6]  + (SPort->PackIn[5])*0x100  + (SPort->PackIn[4])*0x10000  + (SPort->PackIn[3])*0x1000000;
		// рассчет относительной (только для относительных)
		if(Zad_Op_data[data_num][0] == 2)
		{
			Del_pos = *Abs_pos - Rem_pos;
			*Otn_pos = *Otn_pos + Del_pos;
			// поправка для перехода через 0 в циклических механизмах
			if((Zad_Op_data[data_num][1] > 0)&&(Del_pos < 0))
				*Otn_pos = *Otn_pos + wrap_len;
			if((Zad_Op_data[data_num][1] < 0)&&(Del_pos > 0))
				*Otn_pos = *Otn_pos - wrap_len;
		}
		else *Otn_pos = 0;
		Rem_pos = *Abs_pos;
		if(*Home_AZ) *Otv_AZ = 1;
	}
	else if(ACom == 2) // запрос параметров движения
	{
		if(!data_num)	// движение ручное
		{
			// расшифровываем ответ
			// тип
			Tek_Op_data[0][0] = SPort->PackIn[6];
			Data_Table->Cells[1][1] = IntToStr(Tek_Op_data[0][0]);
			// задание пути
			Tek_Op_data[0][1] = SPort->PackIn[10]  + (SPort->PackIn[9])*0x100  + (SPort->PackIn[8])*0x10000  + (SPort->PackIn[7])*0x1000000;
			Data_Table->Cells[2][1] = IntToStr(Tek_Op_data[0][1]);
			// скорость
			Tek_Op_data[0][2] = SPort->PackIn[14]  + (SPort->PackIn[13])*0x100  + (SPort->PackIn[12])*0x10000  + (SPort->PackIn[11])*0x1000000;
			Data_Table->Cells[3][1] = IntToStr(Tek_Op_data[0][2]);
			// ускорение
			Tek_Op_data[0][3] = SPort->PackIn[18]  + (SPort->PackIn[17])*0x100  + (SPort->PackIn[16])*0x10000  + (SPort->PackIn[15])*0x1000000;
			Data_Table->Cells[4][1] = IntToStr(Tek_Op_data[0][3]);
			Edt_Tek_acc->Text = IntToStr(Tek_Op_data[0][3]);
			if(Tek_Op_data[0][3] == Zad_Op_data[0][3]) Edt_Tek_acc->Color = 0x0080FF80;
			else Edt_Tek_acc->Color = 0x008080FF;
			// торможение
			Tek_Op_data[0][4] = SPort->PackIn[22]  + (SPort->PackIn[21])*0x100  + (SPort->PackIn[20])*0x10000  + (SPort->PackIn[19])*0x1000000;
			Data_Table->Cells[5][1] = IntToStr(Tek_Op_data[0][4]);
			Edt_Tek_dec->Text = IntToStr(Tek_Op_data[0][4]);
			if(Tek_Op_data[0][4] == Zad_Op_data[0][4]) Edt_Tek_dec->Color = 0x0080FF80;
			else Edt_Tek_dec->Color = 0x008080FF;
			// current
			Tek_Op_data[0][5] = SPort->PackIn[26]  + (SPort->PackIn[25])*0x100  + (SPort->PackIn[24])*0x10000  + (SPort->PackIn[23])*0x1000000;
			// delay
			Tek_Op_data[0][6] = SPort->PackIn[30]  + (SPort->PackIn[29])*0x100  + (SPort->PackIn[28])*0x10000  + (SPort->PackIn[27])*0x1000000;
			// link
			Tek_Op_data[0][7] = SPort->PackIn[34]  + (SPort->PackIn[33])*0x100  + (SPort->PackIn[32])*0x10000  + (SPort->PackIn[31])*0x1000000;
			Data_Table->Cells[6][1] = IntToStr(Tek_Op_data[0][7]);
			// next data
			Tek_Op_data[0][8] = SPort->PackIn[38]  + (SPort->PackIn[37])*0x100  + (SPort->PackIn[36])*0x10000  + (SPort->PackIn[35])*0x1000000;
			Data_Table->Cells[7][1] = IntToStr(Tek_Op_data[0][8]);
			// сравниваем с заданием
			if( (Tek_Op_data[0][0] == Zad_Op_data[0][0]) &&
				(Tek_Op_data[0][1] == Zad_Op_data[0][1]) &&
				(Tek_Op_data[0][2] == Zad_Op_data[0][2]) &&
				(Tek_Op_data[0][3] == Zad_Op_data[0][3]) &&
				(Tek_Op_data[0][4] == Zad_Op_data[0][4]) &&
				(Tek_Op_data[0][5] == Zad_Op_data[0][5]) &&
				(Tek_Op_data[0][6] == Zad_Op_data[0][6]) &&
				(Tek_Op_data[0][7] == Zad_Op_data[0][7]) &&
				(Tek_Op_data[0][8] == Zad_Op_data[0][8]))
					*Otv_AZ = 1;
			Tek_Op_data[0][0] = 0;
			Tek_Op_data[0][1] = 0;
			Tek_Op_data[0][2] = 0;
			Tek_Op_data[0][3] = 0;
			Tek_Op_data[0][4] = 0;
			Tek_Op_data[0][5] = 0;
			Tek_Op_data[0][6] = 0;
			Tek_Op_data[0][7] = 0;
			Tek_Op_data[0][8] = 0;
		}
		else
		{
			Tek_Op_data[data_num][0] = SPort->PackIn[6];
			Tek_Op_data[data_num][1] = SPort->PackIn[10]  + (SPort->PackIn[9])*0x100  + (SPort->PackIn[8])*0x10000  + (SPort->PackIn[7])*0x1000000;
			Tek_Op_data[data_num][2] = SPort->PackIn[14]  + (SPort->PackIn[13])*0x100  + (SPort->PackIn[12])*0x10000  + (SPort->PackIn[11])*0x1000000;
			Tek_Op_data[data_num][3] = SPort->PackIn[18]  + (SPort->PackIn[17])*0x100  + (SPort->PackIn[16])*0x10000  + (SPort->PackIn[15])*0x1000000;
			Tek_Op_data[data_num][4] = SPort->PackIn[22]  + (SPort->PackIn[21])*0x100  + (SPort->PackIn[20])*0x10000  + (SPort->PackIn[19])*0x1000000;
			Tek_Op_data[data_num][5] = SPort->PackIn[26]  + (SPort->PackIn[25])*0x100  + (SPort->PackIn[24])*0x10000  + (SPort->PackIn[23])*0x1000000;
			Tek_Op_data[data_num][6] = SPort->PackIn[30]  + (SPort->PackIn[29])*0x100  + (SPort->PackIn[28])*0x10000  + (SPort->PackIn[27])*0x1000000;
			Tek_Op_data[data_num][7] = SPort->PackIn[34]  + (SPort->PackIn[33])*0x100  + (SPort->PackIn[32])*0x10000  + (SPort->PackIn[31])*0x1000000;
			Tek_Op_data[data_num][8] = SPort->PackIn[38]  + (SPort->PackIn[37])*0x100  + (SPort->PackIn[36])*0x10000  + (SPort->PackIn[35])*0x1000000;
			// сравниваем с заданием
			if( (Tek_Op_data[data_num][0] == Zad_Op_data[data_num][0]) &&
				(Tek_Op_data[data_num][1] == Zad_Op_data[data_num][1]) &&
				(Tek_Op_data[data_num][2] == Zad_Op_data[data_num][2]) &&
				(Tek_Op_data[data_num][3] == Zad_Op_data[data_num][3]) &&
				(Tek_Op_data[data_num][4] == Zad_Op_data[data_num][4]) &&
				(Tek_Op_data[data_num][5] == Zad_Op_data[data_num][5]) &&
				(Tek_Op_data[data_num][6] == Zad_Op_data[data_num][6]) &&
				(Tek_Op_data[data_num][7] == Zad_Op_data[data_num][7]) &&
				(Tek_Op_data[data_num][8] == Zad_Op_data[data_num][8]))
				Data_err[data_num] = 0;
			else
				Data_err[data_num] = 1;
		}
	}
	else if(ACom == 6) // запрос скорости/ускорения/торможения ZHome
	{
			// расшифровываем ответ
			Edt_Tek_ZHspd->Text = IntToStr(SPort->PackIn[6]  + (SPort->PackIn[5])*0x100  + (SPort->PackIn[4])*0x10000  + (SPort->PackIn[3])*0x1000000);
			if(Edt_Tek_ZHspd->Text == Edt_Rem_ZHspd->Text) Edt_Tek_ZHspd->Color = 0x0080FF80;
			else Edt_Tek_ZHspd->Color = 0x008080FF;
			
			// задание пути
			Edt_Tek_ZHacc->Text = IntToStr(SPort->PackIn[10]  + (SPort->PackIn[9])*0x100  + (SPort->PackIn[8])*0x10000  + (SPort->PackIn[7])*0x1000000);
			if(Edt_Tek_ZHacc->Text == Edt_Rem_ZHacc->Text) Edt_Tek_ZHacc->Color = 0x0080FF80;
			else Edt_Tek_ZHacc->Color = 0x008080FF;
			// скорость
			Edt_Tek_ZHnsp->Text = IntToStr(SPort->PackIn[14]  + (SPort->PackIn[13])*0x100  + (SPort->PackIn[12])*0x10000  + (SPort->PackIn[11])*0x1000000);
			if(Edt_Tek_ZHnsp->Text == Edt_Rem_ZHnsp->Text) Edt_Tek_ZHnsp->Color = 0x0080FF80;
			else Edt_Tek_ZHnsp->Color = 0x008080FF;
			// сравниваем с заданием
			if( (Edt_Tek_ZHspd->Text == Edt_Rem_ZHspd->Text) &&
				(Edt_Tek_ZHacc->Text == Edt_Rem_ZHacc->Text) &&
				(Edt_Tek_ZHnsp->Text == Edt_Rem_ZHnsp->Text))
				Data_errZH = 0;
			else
				Data_errZH = 1;
	}	
}
//---------------------------------------------------------------------------
//--Обработка запроса к манипулятору--//
//---------------------------------------------------------------------------
unsigned char SAZ_drive::AZ_manage( unsigned int SH )
{
	// переключение радиобаттонов на страницах манипуляторов при их работе
	RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

    if(SH == 0) // посылка запроса
	{
		SPort->PackageClear();
		// Функция формирования запроса
		AZ_ZapQuery();

		SPort->VisPackRTU(0,Buf_len);

		// очистка буфера приёма
		SPort->Port.ResetRB();
		// отправка посылки
		SPort->Port.Write(SPort->PackOut,int(Buf_len));
		// переход на следующий шаг
		SPort->DevState++;
		// обнуление времени
		SPort->Dev_Timer = 0;
		return 0;
	}
	else if(SH == 1)
	{
		// чтение ответа
		SPort->Port.Read(SPort->PackIn,SPort->Port.GetRBSize());
		// проверка ответа
		if((SPort->PackIn[0] == adr) )
		{
			AZ_ChkRep(ANum);
			
            SPort->VisPackRTU(1,Buf_len);
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
void Visual_AZdata()       // обновить таблицу значений
{
    //
    for(int j=0;j<DRIVE_COUNT;j++)
        for(int i=1;i<=AZ_drive[j]->Max_Op_Data;i++)
	    {
		    AZ_drive[j]->Data_Table->Cells[1][i+1] = AZ_drive[j]->Zad_Op_data[i][0];
            AZ_drive[j]->Data_Table->Cells[2][i+1] = AZ_drive[j]->Zad_Op_data[i][1];
            AZ_drive[j]->Data_Table->Cells[3][i+1] = AZ_drive[j]->Zad_Op_data[i][2];
            AZ_drive[j]->Data_Table->Cells[4][i+1] = AZ_drive[j]->Zad_Op_data[i][3];
            AZ_drive[j]->Data_Table->Cells[5][i+1] = AZ_drive[j]->Zad_Op_data[i][4];
            AZ_drive[j]->Data_Table->Cells[6][i+1] = AZ_drive[j]->Zad_Op_data[i][7];
            AZ_drive[j]->Data_Table->Cells[7][i+1] = AZ_drive[j]->Zad_Op_data[i][8];
	    }
}
//---------------------------------------------------------------------------
void Visual_AZdrive()       // обновить контрольные значения настроечного массива
{
    //
	for(int i=0;i<DRIVE_COUNT;i++)
	{
		// запомненные
		AZ_drive[i]->Edt_Rem_acc->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[0]);
		AZ_drive[i]->Edt_Rem_dec->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[1]);
		AZ_drive[i]->Edt_Rem_ZHspd->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[2]);
		AZ_drive[i]->Edt_Rem_ZHacc->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[3]);
		AZ_drive[i]->Edt_Rem_ZHnsp->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[4]);
		// скорости
		AZ_drive[i]->Edt_Rem_spdB->Text = IntToStr(AZ_drive[i]->data_mech.v_mech[0]);
		AZ_drive[i]->Edt_Rem_spdM->Text = IntToStr(AZ_drive[i]->data_mech.v_mech[1]);
		AZ_drive[i]->Edt_Rem_spdP->Text = IntToStr(AZ_drive[i]->data_mech.v_mech[2]);
	}
}
//---------------------------------------------------------------------------
void AZdrive_Save()			// сохранение данных
{
	// пробуем записать
    if(!DirectoryExists("Data")) CreateDir("Data"); // пересоздаем папку, если нет
	
	for(int i=0;i<DRIVE_COUNT;i++)
	{
		int SizeOfIniFile=(int)sizeof(AZ_drive[i]->data_mech);
		FILE *im0;
		im0=fopen(AZ_drive[i]->loc_data_mech,"wb");
		if(im0)       { fwrite(&AZ_drive[i]->data_mech,SizeOfIniFile,1,im0); fclose(im0); }
		else if(!im0) { MessageBox(NULL, "Невозможно сохранить данные контроллера", "Ошибка!", MB_OK | MB_ICONSTOP);}
	}	
}
//---------------------------------------------------------------------------	
void AZdrive_Load()			// загрузка данных
{
	// пробуем считать
	if(!DirectoryExists("Data")) { MessageBox(NULL, "Невозможно загрузить данные контроллеров ШД!", "Ошибка!", MB_OK | MB_ICONSTOP); return;}

	for(int i=0;i<DRIVE_COUNT;i++)
	{
	    FILE *im0;
	    im0=fopen(AZ_drive[i]->loc_data_mech,"rb");
        int SizeOfIniFile=(int)sizeof(AZ_drive[i]->data_mech);
	    if(im0)
	    {
            fread(&AZ_drive[i]->data_mech,SizeOfIniFile,1,im0); fclose(im0);
        
		    AZ_drive[i]->Edt_Vvod_acc->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[0]); // текущее ускорение (ввод)
		    AZ_drive[i]->Edt_Vvod_dec->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[1]); // текущее торможение (ввод)
		    AZ_drive[i]->Edt_Vvod_ZHspd->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[2]); // текущее скорость ZHome (ввод)
		    AZ_drive[i]->Edt_Vvod_ZHacc->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[3]); // текущее ускорение ZHome (ввод)
		    AZ_drive[i]->Edt_Vvod_ZHnsp->Text = IntToStr(AZ_drive[i]->data_mech.set_mech[4]); // текущее нач.скорость ZHome (ввод)
		    AZ_drive[i]->Edt_Vvod_spdB->Text = IntToStr(AZ_drive[i]->data_mech.v_mech[0]); // скорость большая (ввод)
		    AZ_drive[i]->Edt_Vvod_spdM->Text = IntToStr(AZ_drive[i]->data_mech.v_mech[1]); // скорость малая (ввод)
		    AZ_drive[i]->Edt_Vvod_spdP->Text = IntToStr(AZ_drive[i]->data_mech.v_mech[2]); // скорость ползущая (ввод)
	    }
    	else if(!im0) { MessageBox(NULL,"Невозможно загрузить данные контроллера!", "Ошибка!",MB_OK | MB_ICONSTOP); }
    }
	Visual_AZdrive();
}
//---------------------------------------------------------------------------
/*void AZdrive_Time()	 // инкремент счетчиков
{
	//for(int i=0;i<DRIVE_COUNT;i++)
	//	AZ_drive[i]->CT_AZ++;
}
//---------------------------------------------------------------------------
void SAZ_drive::VIDK(unsigned char a,int b,unsigned char c,bool d,unsigned int e)	// выдача команд контроллерам AZ
{
	// a - скорость (0-бол.,1-мал.,2-ползущ.)
	// b - тип движения ( ABS | OTN )
	// c - путь (  )
	// d - признак движение в HOME
	// e - контр. время перемещения в сек
	
	if (sh_ == 1) goto A1;
	if (sh_ == 2) goto A2;
	if (sh_ == 3) goto A3;
	if (sh_ == 4) goto A4;
	if (sh_ == 5) goto A5;
	if (sh_ == 6) goto A6;
	if (sh_ == 7) goto A7;
	else return;	// выход

A1:	if(diagnS[diagnS_byte] & diagnS_mask) return; // нет связи с контроллером
	SetOut(0,Stop_byte,Stop_mask);			// снять Стоп механизмов
	*Pr_AZ = 1;
	*CT_AZ = 0;
	sh_ = 2;
A2:	if(!(zin[Rdy_byte] & Rdy_mask))			// нет готовности привода
	{
		if(*CT_AZ >= 5)
			diagn[diagn_byte] |= 0x01;		// отказ: "Нет готовности привода"
		return;
	}
	diagn[diagn_byte] &= (~0x01);			// сброс диагностики
	*Otv_AZ = 0;
	*V_AZ = a;
	*Type_AZ = b;
	*Put_AZ = c;
	*Home_AZ = d;
	*Kom_AZ = 1;
	*CT_AZ = 0;
	sh_ = 3;
A3:	if(diagnS[diagnS_byte] & diagnS_mask || !(*Otv_AZ))	// есть диагностика нет связи или нет ответа
	{
		if(*CT_AZ >= 5)
			diagn[diagn_byte] |= 0x04;		// отказ: "Нет ответа на команду ..."
		return;
	}
	diagn[diagn_byte] &= (~0x04);			// сброс диагностики
	*Kom_AZ = 0;
	if(!(*Home_AZ))							// не движение в HOME
	{
		SetOut(1,Run_byte,Run_mask);		// выдать движение
		*CT_AZ = 0;
		sh_ = 6;
		return;
	}
	SetOut(1,HomeOut_byte,HomeOut_mask);	// выдать движение в Home
	*CT_AZ = 0;
	sh_ = 4;
A4:	if(!(zin[HomeIn_byte] & HomeIn_mask))	// механизм не пришел в HOME
	{
		if(*CT_AZ >= e)
			diagn[diagn_byte] |= 0x08;		// отказ: "Мех. ... не пришел в пол. HOME"
		return;		
	}
	diagn[diagn_byte] &= (~0x08);			// сброс диагностики	
	SetOut(0,HomeOut_byte,HomeOut_mask);	// снять движение в Home
	sh_ = 5;
A5:	if(!(zin[Rdy_byte] & Rdy_mask))				// нет готовности привода
	{
		if(*CT_AZ >= 5)
			diagn[diagn_byte] |= 0x01;	// отказ: "Нет готовности привода ..."
		return;
	}
	diagn[diagn_byte] &= (~0x01);			// сброс диагностики
	*Pr_AZ = 0;
	norma = home_norm;
	sh_ = 0;
	return;
A6:	if(zin[Rdy_byte] & Rdy_mask)			// есть готовность привода
	{
		if(*CT_AZ >= 5)
			diagn[diagn_byte] |= 0x02;		// отказ: "Нет ответа на START движ. ..."
		return;
	}
	diagn[diagn_byte] &= (~0x02);			// сброс диагностики
	SetOut(0,Run_byte,Run_mask);			// сбросить начало движение
	*CT_AZ = 0;
	sh_ = 7;
	return;
A7:	if(!(zin[Rdy_byte] & Rdy_mask))			// есть готовность привода
	{
		if(*CT_AZ >= e)
			diagn[diagn_byte] |= 0x10;		// отказ: "Нет завершения движ. ..."
		return;
	}
	diagn[diagn_byte] &= (~0x10);			// сброс диагностики
	*Pr_AZ = 0;
	norma = norm;
	sh_ = 0;		
}*/
//---------------------------------------------------------------------------
//-- Функция инициализации объектов --//
//---------------------------------------------------------------------------
void Init_SAZ_drive()
{
	for(int i=0;i<DRIVE_COUNT;i++ )
    {
        AZ_drive[i] = new SAZ_drive();
        AZ_drive[i]->Err = 0;
	    AZ_drive[i]->Max_err = 5;
	    AZ_drive[i]->ACom = 0;
	    AZ_drive[i]->Buf_len = 0;
	    AZ_drive[i]->wrap_len = 0;
        AZ_drive[i]->Max_Op_Data = 0;
    }

	// AZ_drive[0]
	AZ_drive[0]->name = "Привод кассеты"; 	// название для отображения
	AZ_drive[0]->adr = 0x01;
	AZ_drive[0]->diagn_byte = 18;        	// номер байта рабочих диагностик
	AZ_drive[0]->diagnS_byte = 2;        	// номер байта связной диагностики
	AZ_drive[0]->diagnS_mask = 0x02;		// маска байта связной диагностики
	AZ_drive[0]->loc_data_mech = "Data\\data_mech1.udb"; // путь файла
	
	AZ_drive[0]->Kom_AZ = &KOM_KAS;
	AZ_drive[0]->Otv_AZ = &OTVET_KAS;
	AZ_drive[0]->V_AZ = &V_KAS;
	AZ_drive[0]->Type_AZ = &TYPE_KAS;
	AZ_drive[0]->Put_AZ = &PUT_KAS;
	AZ_drive[0]->Home_AZ = &HOME_KAS;
    AZ_drive[0]->Pr_AZ = &PR_KAS;
	AZ_drive[0]->Abs_pos = &TEK_ABS_KAS;
	AZ_drive[0]->Otn_pos = &TEK_OTN_KAS;
	AZ_drive[0]->SPort = Comport[3];
	AZ_drive[0]->AZdrive_Gen();

	// AZ_drive[1]
	AZ_drive[1]->name = "Привод гориз. перем."; 	// название для отображения
	AZ_drive[1]->adr = 0x02;
	AZ_drive[1]->diagn_byte = 20;        	// номер байта рабочих диагностик
	AZ_drive[1]->diagnS_byte = 2;        	// номер байта связной диагностики
	AZ_drive[1]->diagnS_mask = 0x08;		// маска байта связной диагностики
	AZ_drive[1]->loc_data_mech = "Data\\data_mech2.udb"; // путь файла
	
	AZ_drive[1]->Kom_AZ = &KOM_PER;
	AZ_drive[1]->Otv_AZ = &OTVET_PER;
	AZ_drive[1]->V_AZ = &V_PER;
	AZ_drive[1]->Type_AZ = &TYPE_PER;
	AZ_drive[1]->Put_AZ = &PUT_PER;
	AZ_drive[1]->Home_AZ = &HOME_PER;
    AZ_drive[1]->Pr_AZ = &PR_PER;
	AZ_drive[1]->Abs_pos = &TEK_ABS_PER;
	AZ_drive[1]->Otn_pos = &TEK_OTN_PER;
	AZ_drive[1]->SPort = Comport[3];
	AZ_drive[1]->AZdrive_Gen();

	// AZ_drive[2]
	AZ_drive[2]->name = "Привод верт. перем."; 	// название для отображения
	AZ_drive[2]->adr = 0x03;
	AZ_drive[2]->diagn_byte = 19;        	// номер байта рабочих диагностик
	AZ_drive[2]->diagnS_byte = 2;        	// номер байта связной диагностики
	AZ_drive[2]->diagnS_mask = 0x04;		// маска байта связной диагностики
	AZ_drive[2]->loc_data_mech = "Data\\data_mech3.udb"; // путь файла
	
	AZ_drive[2]->Kom_AZ = &KOM_POD;
	AZ_drive[2]->Otv_AZ = &OTVET_POD;
	AZ_drive[2]->V_AZ = &V_POD;
	AZ_drive[2]->Type_AZ = &TYPE_POD;
    AZ_drive[2]->Put_AZ = &PUT_POD;
	AZ_drive[2]->Home_AZ = &HOME_POD;
    AZ_drive[2]->Pr_AZ = &PR_POD;
	AZ_drive[2]->Abs_pos = &TEK_ABS_POD;
	AZ_drive[2]->Otn_pos = &TEK_OTN_POD;
	AZ_drive[2]->SPort = Comport[3];
	AZ_drive[2]->AZdrive_Gen();

    // AZ_drive[3]
	AZ_drive[3]->name = "Привод вращения"; 	// название для отображения
	AZ_drive[3]->adr = 0x04;
	AZ_drive[3]->diagn_byte = 22;        	// номер байта рабочих диагностик
	AZ_drive[3]->diagnS_byte = 2;        	// номер байта связной диагностики
	AZ_drive[3]->diagnS_mask = 0x01;		// маска байта связной диагностики
	AZ_drive[3]->wrap_len = 30000;
    AZ_drive[3]->Max_Op_Data = 2;
	AZ_drive[3]->loc_data_mech = "Data\\data_mech4.udb"; // путь файла

	AZ_drive[3]->Kom_AZ = &KOM_VR;
	AZ_drive[3]->Otv_AZ = &OTVET_VR;
	AZ_drive[3]->V_AZ = &V_VR;
	AZ_drive[3]->Type_AZ = &TYPE_VR;
    AZ_drive[3]->Put_AZ = &PUT_VR;
	AZ_drive[3]->Home_AZ = &HOME_VR;
    AZ_drive[3]->Pr_AZ = &PR_VR;
	AZ_drive[3]->Abs_pos = &TEK_ABS_VR;
	AZ_drive[3]->Otn_pos = &TEK_OTN_VR;
	AZ_drive[3]->SPort = Comport[3];
	AZ_drive[3]->AZdrive_Gen();

    // AZ_drive[4]
	AZ_drive[4]->name = "Привод пов. датч. толщ."; 	// название для отображения
	AZ_drive[4]->adr = 0x05;
	AZ_drive[4]->diagn_byte = 21;        	// номер байта рабочих диагностик
	AZ_drive[4]->diagnS_byte = 2;        	// номер байта связной диагностики
	AZ_drive[4]->diagnS_mask = 0x10;		// маска байта связной диагностики
	AZ_drive[4]->loc_data_mech = "Data\\data_mech5.udb"; // путь файла

	AZ_drive[4]->Kom_AZ = &KOM_PPD;
	AZ_drive[4]->Otv_AZ = &OTVET_PPD;
	AZ_drive[4]->V_AZ = &V_PPD;
	AZ_drive[4]->Type_AZ = &TYPE_PPD;
    AZ_drive[4]->Put_AZ = &PUT_PPD;
	AZ_drive[4]->Home_AZ = &HOME_PPD;
    AZ_drive[4]->Pr_AZ = &PR_PPD;
	AZ_drive[4]->Abs_pos = &TEK_ABS_PPD;
	AZ_drive[4]->Otn_pos = &TEK_OTN_PPD;
	AZ_drive[4]->SPort = Comport[3];
	AZ_drive[4]->AZdrive_Gen();
	
	AZdrive_Load();							// загрузка данных
}
//---------------------------------------------------------------------------
