//---------------------------------------------------------------------------
#include "DatPPT200.h"
//---------------------------------------------------------------------------
//-- Генерация страницы --//
//---------------------------------------------------------------------------
void SPPT200_dat::DatPPT200_Gen()
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
	Lbl_Adr->Caption = "Адрес устройства: " + adr;
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
	GB_Main->Caption = " Команды ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;
	
	Lbl_Zap1 = new TLabel(GB_Main);
	Lbl_Zap1->Parent = GB_Main;
	Lbl_Zap1->Top = 40;
	Lbl_Zap1->Left = 28;
	Lbl_Zap1->Caption = "Показания датчика:";
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
//-- Ручной запрос со страницы --//
//---------------------------------------------------------------------------
void __fastcall SPPT200_dat::DatPPT200_SetZap(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
//-- Функция формирования запроса --//
//---------------------------------------------------------------------------
void SPPT200_dat::DatPPT200_FrmZap(bool Zap_type)
{
	String temp_str = "";

	if(Zap_type)    // ручной запрос
	{
		//
	}
	else
	{
		temp_str += adr;
		temp_str += "0074002=?";
	}

	Buf_len = temp_str.Length();
	
	for(int i=1;i<=Buf_len;i++)
		SPort->PackOut[i-1] = temp_str[i];

	// добавление КС
	unsigned int Sum = 0;	
    for(int i=0;i<Buf_len;i++)
        Sum += SPort->PackOut[i];
    Sum = Sum & 0xFF;
    String str = IntToStr(Sum);
    for(int i=1;i<=3;i++)
    {
        if(i<=str.Length()) SPort->PackOut[Buf_len+3-i] = str[str.Length()-i+1];
        else SPort->PackOut[Buf_len+3-i] = '0';
    }
	SPort->PackOut[Buf_len+3] = 13;
	Buf_len = Buf_len + 4;
}
//---------------------------------------------------------------------------
//-- Функция обработки ответа --//
//---------------------------------------------------------------------------
bool SPPT200_dat::DatPPT200_ChkRep(bool Zap_type)
{
	if(Zap_type)    // ручной запрос
	{
		
	}
	else
	{
		if(SPort->PackIn[0] != adr[1]) return 0;
		if(SPort->PackIn[1] != adr[2]) return 0;
		if(SPort->PackIn[2] != adr[3]) return 0;
		
        SPort->VisPackASCII(1);

		try
        {
			if(	isdigit(SPort->PackIn[10])&&
				isdigit(SPort->PackIn[11])&&
				isdigit(SPort->PackIn[12])&&
				isdigit(SPort->PackIn[13])&&
				isdigit(SPort->PackIn[14])&&
				isdigit(SPort->PackIn[15]))
			{
				float result = 0;
				result = (float)(SPort->PackIn[10]-'0')+
					(float)(SPort->PackIn[11]-'0')/10.0+
					(float)(SPort->PackIn[12]-'0')/100.0+
					(float)(SPort->PackIn[13]-'0')/1000.0;
				result = result * pow(10,int(10*(SPort->PackIn[14] - '0') + (SPort->PackIn[15] - '0'))-20+2);

				if((result>1.0E-10)&&(result<1.0E+10))
				{
					*Pres_PPT200 = int((log10(result)+3.5)*1000.0);
					Edt_Zap1->Text = FloatToStrF(pow(10.0,(float(*Pres_PPT200)/1000.0-3.5)),ffExponent,3,8);
				}
				else
				{
					Edt_Zap1->Text = "Вне диапазона";
					return 0;
				}
			}
			else
			{
				Edt_Zap1->Text = "Ошибка";
				return 0;
			}
		}
		catch (Exception &exception)
		{
			Edt_Zap1->Text = "Исключение";
			return 0;
		}		
		return 1;
    }
}
//---------------------------------------------------------------------------
//-- Функция связи с датчиком --//
//---------------------------------------------------------------------------
bool SPPT200_dat::DatPPT200_Manage(unsigned int SH,bool Zap_type)
{
    RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

	if(!SH) // посылка запроса
	{
		SPort->PackageClear();

		DatPPT200_FrmZap(Zap_type);
		SPort->VisPackASCII(0);

		// очистка буфера приёма
		SPort->Port.ResetRB();
		// отправка посылки
		SPort->Port.Write(SPort->PackOut,Buf_len);
		// переход на следующий шаг
		SPort->DevState = 1;
		// обнуление времени
		SPort->Dev_Timer = 0;
		return 0;
	}
	else if(SH == 1)
	{
		SPort->Port.Read(SPort->PackIn,SPort->Port.GetRBSize());
		if(DatPPT200_ChkRep(Zap_type))
		{
			SPort->DevState++;
			Err = 0;
			return 0;
		}
		else
		{
            if(SPort->Dev_Timer < SPort->Dev_TK) return 0;
			SPort->DevState++;
			if((Err++) > Max_err)
				return 1;
			return 0;
		}
	}
}
//---------------------------------------------------------------------------
//-- Инициализация структур --//
//---------------------------------------------------------------------------
void Init_DatPPT200()
{
	for(int i=0;i<DAT_PPT200_COUNT;i++ )
	{
		Dat_PPT200[i] = new SPPT200_dat();
		Dat_PPT200[i]->Err = 0;				// кол-во ошибок
		Dat_PPT200[i]->Max_err = 5;			// максимум ошибок
		Dat_PPT200[i]->ACom = 0;				// текущий автоматический запрос
		Dat_PPT200[i]->RCom = 0;				// текущий ручной апрос
		Dat_PPT200[i]->Buf_len = 0;			// длина запроса	
	}

	// Датчик 1
	Dat_PPT200[0]->name = "Д2(PPT200)";		// название для отображения
	Dat_PPT200[0]->adr = "002";				// адрес
	Dat_PPT200[0]->Pres_PPT200 = &D_D2;      // переменная давления для записи
	Dat_PPT200[0]->SPort = Comport[2];     // порт
	Dat_PPT200[0]->diagnS_byte = 0;        // номер байта связной диагностики
	Dat_PPT200[0]->diagnS_mask = 0x02;		// маска байта связной диагностики
	Dat_PPT200[0]->DatPPT200_Gen();

    // Датчик 2
	Dat_PPT200[1]->name = "Д6(PPT200)";		// название для отображения
	Dat_PPT200[1]->adr = "006";				// адрес
	Dat_PPT200[1]->Pres_PPT200 = &D_D6;      // переменная давления для записи
	Dat_PPT200[1]->SPort = Comport[2];     // порт
	Dat_PPT200[1]->diagnS_byte = 0;        // номер байта связной диагностики
	Dat_PPT200[1]->diagnS_mask = 0x20;		// маска байта связной диагностики
	Dat_PPT200[1]->DatPPT200_Gen();

    // Датчик 3
	Dat_PPT200[2]->name = "Д3(PPT200)";		// название для отображения
	Dat_PPT200[2]->adr = "003";				// адрес
	Dat_PPT200[2]->Pres_PPT200 = &D_D3;      // переменная давления для записи
	Dat_PPT200[2]->SPort = Comport[2];     // порт
	Dat_PPT200[2]->diagnS_byte = 0;        // номер байта связной диагностики
	Dat_PPT200[2]->diagnS_mask = 0x04;		// маска байта связной диагностики
	Dat_PPT200[2]->DatPPT200_Gen();
}