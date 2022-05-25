//---------------------------------------------------------------------------
#include "TRMD.h"
//---------------------------------------------------------------------------
//-- √енераци€ страницы --//
//---------------------------------------------------------------------------
void STRMD::TRMD_Gen()
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
	Lbl_Adr->Caption = "јдрес устройства: " + adr;
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
	GB_Main->Height = 135;
	GB_Main->Width = 385;
	GB_Main->Caption = " ƒанные ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;
	
	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 52;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "«адани€:";
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
	Lbl_Uni->Top = 84;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "“екущие:";
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
	Lbl_Uni->Top = 28;
	Lbl_Uni->Left = 125;
	Lbl_Uni->Caption = " анал 1";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 10;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 28;
	Lbl_Uni->Left = 185;
	Lbl_Uni->Caption = " анал 2";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 10;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 28;
	Lbl_Uni->Left = 245;
	Lbl_Uni->Caption = " анал 3";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 10;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 28;
	Lbl_Uni->Left = 305;
	Lbl_Uni->Caption = " анал 4";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 10;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

	Edt_Zad1 = new TEdit(GB_Main);
	Edt_Zad1->Parent = GB_Main;
	Edt_Zad1->Top = 47;
	Edt_Zad1->Left = 120;
	Edt_Zad1->Font->Name = "Arial";
	Edt_Zad1->Font->Size = 13;
	Edt_Zad1->Font->Color = clBlack;
	Edt_Zad1->Font->Style = Edt_Zad1->Font->Style >> fsBold;
	Edt_Zad1->BevelKind = bkFlat;
	Edt_Zad1->BevelOuter = bvRaised;
	Edt_Zad1->BorderStyle = bsNone;
	Edt_Zad1->ReadOnly = true;
	Edt_Zad1->Color = clSkyBlue;
	Edt_Zad1->Height = 26;
	Edt_Zad1->Width = 55;

	Edt_Zad2 = new TEdit(GB_Main);
	Edt_Zad2->Parent = GB_Main;
	Edt_Zad2->Top = 47;
	Edt_Zad2->Left = 180;
	Edt_Zad2->Font->Name = "Arial";
	Edt_Zad2->Font->Size = 13;
	Edt_Zad2->Font->Color = clBlack;
	Edt_Zad2->Font->Style = Edt_Zad2->Font->Style >> fsBold;
	Edt_Zad2->BevelKind = bkFlat;
	Edt_Zad2->BevelOuter = bvRaised;
	Edt_Zad2->BorderStyle = bsNone;
	Edt_Zad2->ReadOnly = true;
	Edt_Zad2->Color = clSkyBlue;
	Edt_Zad2->Height = 26;
	Edt_Zad2->Width = 55;

	Edt_Zad3 = new TEdit(GB_Main);
	Edt_Zad3->Parent = GB_Main;
	Edt_Zad3->Top = 47;
	Edt_Zad3->Left = 240;
	Edt_Zad3->Font->Name = "Arial";
	Edt_Zad3->Font->Size = 13;
	Edt_Zad3->Font->Color = clBlack;
	Edt_Zad3->Font->Style = Edt_Zad3->Font->Style >> fsBold;
	Edt_Zad3->BevelKind = bkFlat;
	Edt_Zad3->BevelOuter = bvRaised;
	Edt_Zad3->BorderStyle = bsNone;
	Edt_Zad3->ReadOnly = true;
	Edt_Zad3->Color = clSkyBlue;
	Edt_Zad3->Height = 26;
	Edt_Zad3->Width = 55;

	Edt_Zad4 = new TEdit(GB_Main);
	Edt_Zad4->Parent = GB_Main;
	Edt_Zad4->Top = 47;
	Edt_Zad4->Left = 300;
	Edt_Zad4->Font->Name = "Arial";
	Edt_Zad4->Font->Size = 13;
	Edt_Zad4->Font->Color = clBlack;
	Edt_Zad4->Font->Style = Edt_Zad4->Font->Style >> fsBold;
	Edt_Zad4->BevelKind = bkFlat;
	Edt_Zad4->BevelOuter = bvRaised;
	Edt_Zad4->BorderStyle = bsNone;
	Edt_Zad4->ReadOnly = true;
	Edt_Zad4->Color = clSkyBlue;
	Edt_Zad4->Height = 26;
	Edt_Zad4->Width = 55;

	Edt_Tek1 = new TEdit(GB_Main);
	Edt_Tek1->Parent = GB_Main;
	Edt_Tek1->Top = 79;
	Edt_Tek1->Left = 120;
	Edt_Tek1->Font->Name = "Arial";
	Edt_Tek1->Font->Size = 13;
	Edt_Tek1->Font->Color = clBlack;
	Edt_Tek1->Font->Style = Edt_Tek1->Font->Style >> fsBold;
	Edt_Tek1->BevelKind = bkFlat;
	Edt_Tek1->BevelOuter = bvRaised;
	Edt_Tek1->BorderStyle = bsNone;
	Edt_Tek1->ReadOnly = true;
	Edt_Tek1->Color = clWhite;
	Edt_Tek1->Height = 26;
	Edt_Tek1->Width = 55;

	Edt_Tek2 = new TEdit(GB_Main);
	Edt_Tek2->Parent = GB_Main;
	Edt_Tek2->Top = 79;
	Edt_Tek2->Left = 180;
	Edt_Tek2->Font->Name = "Arial";
	Edt_Tek2->Font->Size = 13;
	Edt_Tek2->Font->Color = clBlack;
	Edt_Tek2->Font->Style = Edt_Tek2->Font->Style >> fsBold;
	Edt_Tek2->BevelKind = bkFlat;
	Edt_Tek2->BevelOuter = bvRaised;
	Edt_Tek2->BorderStyle = bsNone;
	Edt_Tek2->ReadOnly = true;
	Edt_Tek2->Color = clWhite;
	Edt_Tek2->Height = 26;
	Edt_Tek2->Width = 55;

	Edt_Tek3 = new TEdit(GB_Main);
	Edt_Tek3->Parent = GB_Main;
	Edt_Tek3->Top = 79;
	Edt_Tek3->Left = 240;
	Edt_Tek3->Font->Name = "Arial";
	Edt_Tek3->Font->Size = 13;
	Edt_Tek3->Font->Color = clBlack;
	Edt_Tek3->Font->Style = Edt_Tek3->Font->Style >> fsBold;
	Edt_Tek3->BevelKind = bkFlat;
	Edt_Tek3->BevelOuter = bvRaised;
	Edt_Tek3->BorderStyle = bsNone;
	Edt_Tek3->ReadOnly = true;
	Edt_Tek3->Color = clWhite;
	Edt_Tek3->Height = 26;
	Edt_Tek3->Width = 55;

	Edt_Tek4 = new TEdit(GB_Main);
	Edt_Tek4->Parent = GB_Main;
	Edt_Tek4->Top = 79;
	Edt_Tek4->Left = 300;
	Edt_Tek4->Font->Name = "Arial";
	Edt_Tek4->Font->Size = 13;
	Edt_Tek4->Font->Color = clBlack;
	Edt_Tek4->Font->Style = Edt_Tek4->Font->Style >> fsBold;
	Edt_Tek4->BevelKind = bkFlat;
	Edt_Tek4->BevelOuter = bvRaised;
	Edt_Tek4->BorderStyle = bsNone;
	Edt_Tek4->ReadOnly = true;
	Edt_Tek4->Color = clWhite;
	Edt_Tek4->Height = 26;
	Edt_Tek4->Width = 55;
}
//---------------------------------------------------------------------------
//-- –учной запрос со страницы --//
//---------------------------------------------------------------------------
void __fastcall STRMD::TRMD_SetZap(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
//-- ‘ункци€ формировани€ запроса --//
//---------------------------------------------------------------------------
void STRMD::TRMD_FrmZap(bool Zap_type)
{
	String temp_str = "";

	if(Zap_type)    // ручной запрос
	{
		//
	}
	else
	{
		// зависит от кол-ва каналов !!!
		if(ACom == 5) ACom = 1;         // если нужна запись 1 канала управлени€
		//else if(ACom == 1) ACom = 2;  // если нужна запись 2 канала управлени€
        //else if(ACom == 2) ACom = 3;  // если нужна запись 3 канала управлени€
        //else if(ACom == 3) ACom = 4;  // если нужна запись 3 канала управлени€
		else ACom = 5;

		switch(ACom)
		{
			case 1:
			{
				Edt_Zad1->Text = FloatToStrF(float(*ZadTemp1)/10.0,ffFixed,5,1);
				temp_str = ":";
				temp_str += adr;
				temp_str += "0601730000";
				char temp_char[4] = {0};
				itoa(*ZadTemp1,temp_char,16);
				String temp_date = "";
				temp_date = String(temp_char);
				for(int i=0;i<temp_date.Length();i++)
			 		temp_str[13-i] = temp_date[temp_date.Length() - i];
			};break;
			case 2:
			{
				Edt_Zad2->Text = FloatToStrF(float(*ZadTemp2)/10.0,ffFixed,5,1);
				temp_str = ":";
				temp_str += adr;
				temp_str += "0605730000";
				char temp_char[4] = {0};
				itoa(*ZadTemp2,temp_char,16);
				String temp_date = "";
				temp_date = String(temp_char);
				for(int i=0;i<temp_date.Length();i++)
			 		temp_str[13-i] = temp_date[temp_date.Length() - i];
			};break;
			case 3:
			{
				Edt_Zad3->Text = FloatToStrF(float(*ZadTemp3)/10.0,ffFixed,5,1);
				temp_str = ":";
				temp_str += adr;
				temp_str += "0609730000";
				char temp_char[4] = {0};
				itoa(*ZadTemp3,temp_char,16);
				String temp_date = "";
				temp_date = String(temp_char);
				for(int i=0;i<temp_date.Length();i++)
			 		temp_str[13-i] = temp_date[temp_date.Length() - i];
			};break;
			case 4:
			{
				Edt_Zad4->Text = FloatToStrF(float(*ZadTemp4)/10.0,ffFixed,5,1);
				temp_str = ":";
				temp_str += adr;
				temp_str += "060D730000";
				char temp_char[4] = {0};
				itoa(*ZadTemp4,temp_char,16);
				String temp_date = "";
				temp_date = String(temp_char);
				for(int i=0;i<temp_date.Length();i++)
			 		temp_str[13-i] = temp_date[temp_date.Length() - i];
			};break;
			case 5:
			{
				temp_str = ":";
				temp_str += adr;
				temp_str += "0300000004";
			};break;
		}
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
	String str = "";
	str = str + IntToHex((int)(CRC),2);
	SPort->PackOut[13] = str[1];
	SPort->PackOut[14] = str[2];
	SPort->PackOut[15] = 13;
	SPort->PackOut[16] = 10;
	Buf_len = 17;
}
//---------------------------------------------------------------------------
//-- ‘ункци€ обработки ответа --//
//---------------------------------------------------------------------------
bool STRMD::TRMD_ChkRep(bool Zap_type)
{
    for(int i=0;i<PACKAGE_COUNT;i++)
	{
		if( (SPort->PackIn[2]==adr[2])&&
			(SPort->PackIn[i]==10)&&
			(SPort->PackIn[i-1]==13))	// есть конец посылки
		{
			if(Zap_type)    // ручной запрос
			{
				//
			}
			else
			{
				if(ACom == 5)
				{
                    char tmp[4] = {0};
                                        if(isxdigit(SPort->PackIn[7])&&isxdigit(SPort->PackIn[8])&&isxdigit(SPort->PackIn[9])&&isxdigit(SPort->PackIn[10]))
                                        {
					        tmp[0] = SPort->PackIn[7];
					        tmp[1] = SPort->PackIn[8];
					        tmp[2] = SPort->PackIn[9];
					        tmp[3] = SPort->PackIn[10];

					        *TekTemp1 = strtol(tmp,NULL,16);
                                                Edt_Tek1->Text = FloatToStrF(float(*TekTemp1)/10.0,ffFixed,5,1);
                                        }

                                        if(isxdigit(SPort->PackIn[11])&&isxdigit(SPort->PackIn[12])&&isxdigit(SPort->PackIn[13])&&isxdigit(SPort->PackIn[14]))
                                        {
					        tmp[0] = SPort->PackIn[11];
					        tmp[1] = SPort->PackIn[12];
					        tmp[2] = SPort->PackIn[13];
					        tmp[3] = SPort->PackIn[14];

					        *TekTemp2 = strtol(tmp,NULL,16);
					        Edt_Tek2->Text = FloatToStrF(float(*TekTemp2)/10.0,ffFixed,5,1);
                                        }

                                        if(isxdigit(SPort->PackIn[15])&&isxdigit(SPort->PackIn[16])&&isxdigit(SPort->PackIn[17])&&isxdigit(SPort->PackIn[18]))
                                        {
					        tmp[0] = SPort->PackIn[15];
					        tmp[1] = SPort->PackIn[16];
					        tmp[2] = SPort->PackIn[17];
					        tmp[3] = SPort->PackIn[18];

					        *TekTemp3 = strtol(tmp,NULL,16);
					        Edt_Tek3->Text = FloatToStrF(float(*TekTemp3)/10.0,ffFixed,5,1);
                                        }

                                        if(isxdigit(SPort->PackIn[19])&&isxdigit(SPort->PackIn[20])&&isxdigit(SPort->PackIn[21])&&isxdigit(SPort->PackIn[22]))
                                        {
					        tmp[0] = SPort->PackIn[19];
					        tmp[1] = SPort->PackIn[20];
					        tmp[2] = SPort->PackIn[21];
					        tmp[3] = SPort->PackIn[22];

					        *TekTemp4 = strtol(tmp,NULL,16);
					        Edt_Tek4->Text = FloatToStrF(float(*TekTemp4)/10.0,ffFixed,5,1);
                                        }
				}
			}
            *Pr_Sv = 1;
			return 1;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
//-- ‘ункци€ св€зи с датчиком --//
//---------------------------------------------------------------------------
bool STRMD::TRMD_Manage(unsigned int SH,bool Zap_type)
{
	// переключение радиобаттонов на страницах датчиков при их работе
	RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

    if(SH == 0) // посылка запроса
	{
		SPort->PackageClear();
		// ‘ункци€ формировани€ запроса
		TRMD_FrmZap(Zap_type);
		SPort->VisPackASCII(0);

		// очистка буфера приЄма
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
		SPort->Port.Read(SPort->PackIn,SPort->Port.GetRBSize());
		// проверка ответа
		if(TRMD_ChkRep(Zap_type))
		{
			TRMD_ChkRep(Zap_type);
			SPort->VisPackASCII(1);
			// переход на следующий шаг
			SPort->DevState++;
			// сброс счетчика ошибок св€зи
			Err = 0;
			return 0;
		}
		else
		{
            if(SPort->Dev_Timer < SPort->Dev_TK) return 0; // ждем ответа
			// не дождались
			SPort->DevState++; // переходим к следующему устройству
			// увеличение счетчика ошибок св€зи и провер€ем на максимум
			if((Err++) > Max_err)
				return 1;
			return 0;
		}
    }
}
//---------------------------------------------------------------------------
//-- »нициализаци€ структур --//
//---------------------------------------------------------------------------
void Init_TRMD()
{
	for(int i=0;i<TRMD_COUNT;i++ )
		TRMD[i] = new STRMD();

	// ƒатчик 1
	TRMD[0]->name = "“ермодат";            // название дл€ отображени€
	TRMD[0]->adr = "03";			// адрес
	TRMD[0]->ZadTemp1 = &ZAD_TEMP;      // переменные режима
	TRMD[0]->ZadTemp2 = &ZAD_TEMP2;      // переменные режима
	TRMD[0]->ZadTemp3 = &ZAD_TEMP3;      // переменные режима
	TRMD[0]->ZadTemp4 = &ZAD_TEMP4;      // переменные режима
	TRMD[0]->TekTemp1 = &TEK_TEMP;      // переменные режима
	TRMD[0]->TekTemp2 = &TEK_TEMP2;      // переменные режима
	TRMD[0]->TekTemp3 = &TEK_TEMP3;      // переменные режима
	TRMD[0]->TekTemp4 = &TEK_TEMP4;      // переменные режима
	TRMD[0]->Pr_Sv = &PR_TEMP;			// переменные режима
	TRMD[0]->SPort = Comport[1];     // порт
	TRMD[0]->diagnS_byte = 0;        // номер байта св€зной диагностики
	TRMD[0]->diagnS_mask = 0x04;		// маска байта св€зной диагностики

	TRMD[0]->Err = 0;				// кол-во ошибок
	TRMD[0]->Max_err = 5;			// максимум ошибок
	TRMD[0]->ACom = 1;				// текущий автоматический запрос
	TRMD[0]->RCom = 0;				// текущий ручной апрос
	TRMD[0]->Buf_len = 0;			// длина запроса
	TRMD[0]->TRMD_Gen();
}