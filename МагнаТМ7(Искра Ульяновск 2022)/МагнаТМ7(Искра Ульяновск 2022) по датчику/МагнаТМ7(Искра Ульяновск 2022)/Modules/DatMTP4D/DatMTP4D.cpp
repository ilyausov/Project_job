//---------------------------------------------------------------------------
#include "DatMTP4D.h"
//---------------------------------------------------------------------------
//-- Генерация страницы --//
//---------------------------------------------------------------------------
void SMTP4D_dat::DatMTP4D_Gen()
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
void __fastcall SMTP4D_dat::DatMTP4D_SetZap(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
//-- Функция формирования запроса --//
//---------------------------------------------------------------------------
void SMTP4D_dat::DatMTP4D_FrmZap(bool Zap_type)
{
	String temp_str = "";

	if(Zap_type)    // ручной запрос
	{
		//
	}
	else
	{
		temp_str += adr;
		temp_str += "M";
	}

	Buf_len = temp_str.Length();
	
	for(int i=1;i<=Buf_len;i++)
		SPort->PackOut[i-1] = temp_str[i];

	unsigned int Sum = 0;	
    for(int i=0;i<Buf_len;i++)
        Sum += SPort->PackOut[i];
    Sum = 64 + Sum%64;
	SPort->PackOut[Buf_len] = Sum;
    SPort->PackOut[Buf_len+1] = 13;
	Buf_len = Buf_len + 2;
}
//---------------------------------------------------------------------------
//-- Функция обработки ответа --//
//---------------------------------------------------------------------------
bool SMTP4D_dat::DatMTP4D_ChkRep(bool Zap_type)
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
			if((SPort->PackIn[3]=='M')&&
				isdigit(SPort->PackIn[4])&&
				isdigit(SPort->PackIn[5])&&
				isdigit(SPort->PackIn[6])&&
				isdigit(SPort->PackIn[7])&&
				isdigit(SPort->PackIn[8])&&
				isdigit(SPort->PackIn[9]))
			{
				float result = 0;
				result = (float)(SPort->PackIn[4]-'0')+
					(float)(SPort->PackIn[5]-'0')/10.0+
					(float)(SPort->PackIn[6]-'0')/100.0+
					(float)(SPort->PackIn[7]-'0')/1000.0;
				result = result * pow(10,int(10*(SPort->PackIn[8] - '0') + (SPort->PackIn[9] - '0'))-20);

				if((result>1.0E-10)&&(result<1.0E+10))
				{
					*Pres_DMTP4D = int((log10(result)+5.5)*1000.0);
					Edt_Zap1->Text = FloatToStrF(100.0*pow(10.0,float(*Pres_DMTP4D)/1000.0-5.5),ffExponent,3,8);
				}
				else
				{
                    *Pres_DMTP4D = 1;
					Edt_Zap1->Text = "Вне диапазона";
					return 1;
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
bool SMTP4D_dat::DatMTP4D_Manage(unsigned int SH,bool Zap_type)
{
    RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

	if(!SH) // посылка запроса
	{
		SPort->PackageClear();

		DatMTP4D_FrmZap(Zap_type);
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
		if(DatMTP4D_ChkRep(Zap_type))
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
void Init_DatMTP4D()
{
	for(int i=0;i<DAT_MTP4D_COUNT;i++ )
	{
		Dat_MTP4D[i] = new SMTP4D_dat();
		Dat_MTP4D[i]->Err = 0;				// кол-во ошибок
		Dat_MTP4D[i]->Max_err = 5;			// максимум ошибок
		Dat_MTP4D[i]->ACom = 0;				// текущий автоматический запрос
		Dat_MTP4D[i]->RCom = 0;				// текущий ручной апрос
		Dat_MTP4D[i]->Buf_len = 0;			// длина запроса
	}

	// Датчик 1
	Dat_MTP4D[0]->name = "Д2(MTP4D)";            // название для отображения
	Dat_MTP4D[0]->adr = "002";			// адрес
	Dat_MTP4D[0]->Pres_DMTP4D = &D_D2;      // переменная давления для записи
	Dat_MTP4D[0]->SPort = Comport[1];     // порт
	Dat_MTP4D[0]->diagnS_byte = 0;        // номер байта связной диагностики
	Dat_MTP4D[0]->diagnS_mask = 0x02;		// маска байта связной диагностики
	Dat_MTP4D[0]->DatMTP4D_Gen();
}