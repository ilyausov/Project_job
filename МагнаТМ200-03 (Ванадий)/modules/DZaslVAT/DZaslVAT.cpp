//---------------------------------------------------------------------------
#include "DZaslVAT.h"
//---------------------------------------------------------------------------
//-- Генерация страницы --//
//---------------------------------------------------------------------------
void SDZaslVAT::DZaslVAT_Gen()
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
	Pnl_Parent->Height = 676;
	Pnl_Parent->Width = 1176;

	Lbl_Adr = new TLabel(Pnl_Parent);
	Lbl_Adr->Parent = Pnl_Parent;
	Lbl_Adr->Top = 15;
	Lbl_Adr->Left = 26;
	Lbl_Adr->Caption = "Адрес устройства: ---";
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
	GB_Main->Height = 244;
	GB_Main->Width = 579;
	GB_Main->Caption = " Команды ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;
	
	Lbl_Zap1 = new TLabel(GB_Main);
	Lbl_Zap1->Parent = GB_Main;
	Lbl_Zap1->Top = 40;
	Lbl_Zap1->Left = 28;
	Lbl_Zap1->Caption = "Контроль позиции";
	Lbl_Zap1->Font->Name = "Arial";
	Lbl_Zap1->Font->Size = 12;
	Lbl_Zap1->Font->Color = clBlack;
	Lbl_Zap1->Transparent = true;
	Lbl_Zap1->Height = 19;
	Lbl_Zap1->Width = 190;
	Lbl_Zap1->Layout = tlTop;
	Lbl_Zap1->Font->Style = Lbl_Zap1->Font->Style >> fsBold;
		
	Lbl_Zap2 = new TLabel(GB_Main);
	Lbl_Zap2->Parent = GB_Main;
	Lbl_Zap2->Top = 72;
	Lbl_Zap2->Left = 28;
	Lbl_Zap2->Caption = "Контроль давления";
	Lbl_Zap2->Font->Name = "Arial";
	Lbl_Zap2->Font->Size = 12;
	Lbl_Zap2->Font->Color = clBlack;
	Lbl_Zap2->Transparent = true;
	Lbl_Zap2->Height = 19;
	Lbl_Zap2->Width = 190;
	Lbl_Zap2->Layout = tlTop;
	Lbl_Zap2->Font->Style = Lbl_Zap2->Font->Style >> fsBold;
	
	Lbl_Zap3 = new TLabel(GB_Main);
	Lbl_Zap3->Parent = GB_Main;
	Lbl_Zap3->Top = 104;
	Lbl_Zap3->Left = 28;
	Lbl_Zap3->Caption = "Открыть заслонку";
	Lbl_Zap3->Font->Name = "Arial";
	Lbl_Zap3->Font->Size = 12;
	Lbl_Zap3->Font->Color = clBlack;
	Lbl_Zap3->Transparent = true;
	Lbl_Zap3->Height = 19;
	Lbl_Zap3->Width = 190;
	Lbl_Zap3->Layout = tlTop;
	Lbl_Zap3->Font->Style = Lbl_Zap3->Font->Style >> fsBold;
	
	Lbl_Zap4 = new TLabel(GB_Main);
	Lbl_Zap4->Parent = GB_Main;
	Lbl_Zap4->Top = 136;
	Lbl_Zap4->Left = 28;
	Lbl_Zap4->Caption = "Закрыть заслонку";
	Lbl_Zap4->Font->Name = "Arial";
	Lbl_Zap4->Font->Size = 12;
	Lbl_Zap4->Font->Color = clBlack;
	Lbl_Zap4->Transparent = true;
	Lbl_Zap4->Height = 19;
	Lbl_Zap4->Width = 190;
	Lbl_Zap4->Layout = tlTop;
	Lbl_Zap4->Font->Style = Lbl_Zap4->Font->Style >> fsBold;

	Lbl_Zap5 = new TLabel(GB_Main);
	Lbl_Zap5->Parent = GB_Main;
	Lbl_Zap5->Top = 168;
	Lbl_Zap5->Left = 28;
	Lbl_Zap5->Caption = "Удержание";
	Lbl_Zap5->Font->Name = "Arial";
	Lbl_Zap5->Font->Size = 12;
	Lbl_Zap5->Font->Color = clBlack;
	Lbl_Zap5->Transparent = true;
	Lbl_Zap5->Height = 19;
	Lbl_Zap5->Width = 190;
	Lbl_Zap5->Layout = tlTop;
	Lbl_Zap5->Font->Style = Lbl_Zap5->Font->Style >> fsBold;

	Lbl_Zap6 = new TLabel(GB_Main);
	Lbl_Zap6->Parent = GB_Main;
	Lbl_Zap6->Top = 200;
	Lbl_Zap6->Left = 28;
	Lbl_Zap6->Caption = "Обучение заслонки";
	Lbl_Zap6->Font->Name = "Arial";
	Lbl_Zap6->Font->Size = 12;
	Lbl_Zap6->Font->Color = clBlack;
	Lbl_Zap6->Transparent = true;
	Lbl_Zap6->Height = 19;
	Lbl_Zap6->Width = 190;
	Lbl_Zap6->Layout = tlTop;
	Lbl_Zap6->Font->Style = Lbl_Zap6->Font->Style >> fsBold;

	Edt_Zap1 = new TMaskEdit(GB_Main);
	Edt_Zap1->Parent = GB_Main;
	Edt_Zap1->Top = 35;
	Edt_Zap1->Left = 200;
	Edt_Zap1->Font->Name = "Arial";
	Edt_Zap1->Font->Size = 13;
	Edt_Zap1->Font->Color = clBlack;
	Edt_Zap1->Font->Style = Edt_Zap1->Font->Style >> fsBold;
	Edt_Zap1->BevelKind = bkFlat;
	Edt_Zap1->BevelOuter = bvRaised;
	Edt_Zap1->BorderStyle = bsNone;
	Edt_Zap1->MaxLength = 6;
	Edt_Zap1->Height = 26;
	Edt_Zap1->Width = 70;
	Edt_Zap1->EditMask = "999999;1;0";

	Edt_Zap2 = new TMaskEdit(GB_Main);
	Edt_Zap2->Parent = GB_Main;
	Edt_Zap2->Top = 67;
	Edt_Zap2->Left = 200;
	Edt_Zap2->Font->Name = "Arial";
	Edt_Zap2->Font->Size = 13;
	Edt_Zap2->Font->Color = clBlack;
	Edt_Zap2->Font->Style = Edt_Zap2->Font->Style >> fsBold;
	Edt_Zap2->BevelKind = bkFlat;
	Edt_Zap2->BevelOuter = bvRaised;
	Edt_Zap2->BorderStyle = bsNone;
	Edt_Zap2->MaxLength = 7;
	Edt_Zap2->Height = 26;
	Edt_Zap2->Width = 70;
	Edt_Zap2->EditMask = "9999999;1;0";

	Edt_Zap7_1 = new TEdit(GB_Main);
	Edt_Zap7_1->Parent = GB_Main;
	Edt_Zap7_1->Top = 35;
	Edt_Zap7_1->Left = 485;
	Edt_Zap7_1->Font->Name = "Arial";
	Edt_Zap7_1->Font->Size = 13;
	Edt_Zap7_1->Font->Color = clBlack;
	Edt_Zap7_1->Font->Style = Edt_Zap7_1->Font->Style >> fsBold;
	Edt_Zap7_1->BevelKind = bkFlat;
	Edt_Zap7_1->BevelOuter = bvRaised;
	Edt_Zap7_1->BorderStyle = bsNone;
	Edt_Zap7_1->ReadOnly = true;
	Edt_Zap7_1->Color = clSkyBlue;
	Edt_Zap7_1->Height = 26;
	Edt_Zap7_1->Width = 70;

	Edt_Zap7_2 = new TEdit(GB_Main);
	Edt_Zap7_2->Parent = GB_Main;
	Edt_Zap7_2->Top = 67;
	Edt_Zap7_2->Left = 485;
	Edt_Zap7_2->Font->Name = "Arial";
	Edt_Zap7_2->Font->Size = 13;
	Edt_Zap7_2->Font->Color = clBlack;
	Edt_Zap7_2->Font->Style = Edt_Zap7_2->Font->Style >> fsBold;
	Edt_Zap7_2->BevelKind = bkFlat;
	Edt_Zap7_2->BevelOuter = bvRaised;
	Edt_Zap7_2->BorderStyle = bsNone;
	Edt_Zap7_2->ReadOnly = true;
	Edt_Zap7_2->Color = clSkyBlue;
	Edt_Zap7_2->Height = 26;
	Edt_Zap7_2->Width = 70;

	Btn_Zap1 = new TButton(GB_Main);
	Btn_Zap1->Parent = GB_Main;
	Btn_Zap1->Top = 35;
	Btn_Zap1->Left = 285;
	Btn_Zap1->Font->Name = "Arial";
	Btn_Zap1->Font->Size = 12;
	Btn_Zap1->Font->Color = clBlack;
	Btn_Zap1->Font->Style = Btn_Zap1->Font->Style >> fsBold;
	Btn_Zap1->Caption = "Запись";
	Btn_Zap1->Width = 75;
	Btn_Zap1->Height = 26;
	Btn_Zap1->Hint = "1";
	Btn_Zap1->OnClick = DZaslVAT_SetZap;

	Btn_Zap2 = new TButton(GB_Main);
	Btn_Zap2->Parent = GB_Main;
	Btn_Zap2->Top = 67;
	Btn_Zap2->Left = 285;
	Btn_Zap2->Font->Name = "Arial";
	Btn_Zap2->Font->Size = 12;
	Btn_Zap2->Font->Color = clBlack;
	Btn_Zap2->Font->Style = Btn_Zap2->Font->Style >> fsBold;
	Btn_Zap2->Caption = "Запись";
	Btn_Zap2->Width = 75;
	Btn_Zap2->Height = 26;
	Btn_Zap2->Hint = "2";
	Btn_Zap2->OnClick = DZaslVAT_SetZap;

	Btn_Zap3 = new TButton(GB_Main);
	Btn_Zap3->Parent = GB_Main;
	Btn_Zap3->Top = 99;
	Btn_Zap3->Left = 285;
	Btn_Zap3->Font->Name = "Arial";
	Btn_Zap3->Font->Size = 12;
	Btn_Zap3->Font->Color = clBlack;
	Btn_Zap3->Font->Style = Btn_Zap3->Font->Style >> fsBold;
	Btn_Zap3->Caption = "Запись";
	Btn_Zap3->Width = 75;
	Btn_Zap3->Height = 26;
	Btn_Zap3->Hint = "3";
	Btn_Zap3->OnClick = DZaslVAT_SetZap;

	Btn_Zap4 = new TButton(GB_Main);
	Btn_Zap4->Parent = GB_Main;
	Btn_Zap4->Top = 131;
	Btn_Zap4->Left = 285;
	Btn_Zap4->Font->Name = "Arial";
	Btn_Zap4->Font->Size = 12;
	Btn_Zap4->Font->Color = clBlack;
	Btn_Zap4->Font->Style = Btn_Zap4->Font->Style >> fsBold;
	Btn_Zap4->Caption = "Запрос";
	Btn_Zap4->Width = 75;
	Btn_Zap4->Height = 26;
	Btn_Zap4->Hint = "4";
	Btn_Zap4->OnClick = DZaslVAT_SetZap;

	Btn_Zap5 = new TButton(GB_Main);
	Btn_Zap5->Parent = GB_Main;
	Btn_Zap5->Top = 163;
	Btn_Zap5->Left = 285;
	Btn_Zap5->Font->Name = "Arial";
	Btn_Zap5->Font->Size = 12;
	Btn_Zap5->Font->Color = clBlack;
	Btn_Zap5->Font->Style = Btn_Zap5->Font->Style >> fsBold;
	Btn_Zap5->Caption = "Запрос";
	Btn_Zap5->Width = 75;
	Btn_Zap5->Height = 26;
	Btn_Zap5->Hint = "5";
	Btn_Zap5->OnClick = DZaslVAT_SetZap;

	Btn_Zap6 = new TButton(GB_Main);
	Btn_Zap6->Parent = GB_Main;
	Btn_Zap6->Top = 195;
	Btn_Zap6->Left = 285;
	Btn_Zap6->Font->Name = "Arial";
	Btn_Zap6->Font->Size = 12;
	Btn_Zap6->Font->Color = clBlack;
	Btn_Zap6->Font->Style = Btn_Zap6->Font->Style >> fsBold;
	Btn_Zap6->Caption = "Запрос";
	Btn_Zap6->Width = 75;
	Btn_Zap6->Height = 26;
	Btn_Zap6->Hint = "6";
	Btn_Zap6->OnClick = DZaslVAT_SetZap;

	Btn_Zap7 = new TButton(GB_Main);
	Btn_Zap7->Parent = GB_Main;
	Btn_Zap7->Top = 51;
	Btn_Zap7->Left = 395;
	Btn_Zap7->Font->Name = "Arial";
	Btn_Zap7->Font->Size = 12;
	Btn_Zap7->Font->Color = clBlack;
	Btn_Zap7->Font->Style = Btn_Zap7->Font->Style >> fsBold;
	Btn_Zap7->Caption = "Запрос";
	Btn_Zap7->Width = 75;
	Btn_Zap7->Height = 26;
	Btn_Zap7->Hint = "7";
	Btn_Zap7->OnClick = DZaslVAT_SetZap;
}
//---------------------------------------------------------------------------
//-- Ручной запрос со страницы --//
//---------------------------------------------------------------------------
void __fastcall SDZaslVAT::DZaslVAT_SetZap(TObject *Sender)
{
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

	RCom = StrToInt(((TPanel*)Sender)->Hint);
}
//---------------------------------------------------------------------------
//-- Функция формирования запроса --//
//---------------------------------------------------------------------------
void SDZaslVAT::DZaslVAT_FrmZap(bool Zap_type)
{
	String temp_str = "";

	if(Zap_type)    // ручной запрос
	{
		switch(RCom)
		{
			case 1:
			{
				temp_str = "R:";
				temp_str += Edt_Zap1->EditText;
			};break;
			case 2:
			{
				temp_str = "S:0";
				temp_str += Edt_Zap2->EditText;

			};break;
			case 3:
			{
				temp_str = "O:";
			};break;
			case 4:
			{
				temp_str = "C:";
			};break;
			case 5:
			{
				temp_str = "H:";
			};break;
			case 6:
			{
				temp_str = "L:00010000";
			};break;
			case 7:
			{
				temp_str = "i:76";
			};break;
		};
	}
	else
	{   // если нет задания - опрос состояния
		if(!*ZadCom_DZaslVAT) *ZadCom_DZaslVAT = 7;
		ACom = *ZadCom_DZaslVAT;

		switch(ACom)
		{
			case 1:
			{
				temp_str = "R:000000";
				String temp_date = String(*ZadData_DZaslVAT);
				for(int i=0;i<temp_date.Length();i++)
					temp_str[8-i] = temp_date[temp_date.Length() - i];
			};break;
			case 2:
			{
				temp_str = "S:00000000";
				String temp_date = String(*ZadData_DZaslVAT);
				for(int i=0;i<temp_date.Length();i++)
					temp_str[10-i] = temp_date[temp_date.Length() - i];
			};break;
			case 3:
			{
				temp_str = "O:";
			};break;
			case 4:
			{
				temp_str = "C:";
			};break;
			case 5:
			{
				temp_str = "H:";
			};break;
			case 7:
			{
				temp_str = "i:76";
			};break;
		}
    }

	for(int i=1;i<=temp_str.Length();i++)
		SPort->PackOut[i-1] = temp_str[i];
	Buf_len = temp_str.Length();
	SPort->PackOut[Buf_len] = 13;
	SPort->PackOut[Buf_len+1] = 10;
	Buf_len = Buf_len + 2;
}
//---------------------------------------------------------------------------
//-- Функция обработки ответа --//
//---------------------------------------------------------------------------
bool SDZaslVAT::DZaslVAT_ChkRep(bool Zap_type)
{
	for(int i=0;i<PACKAGE_COUNT;i++)
	{
		if((SPort->PackIn[i]==10)&&(SPort->PackIn[i-1]==13))	// есть конец посылки
		{
			if(Zap_type)    // ручной запрос
			{
				if(RCom == 7)
				{	// расшифровывается общий опрос
					char temp_str[6] = {0};
					for(int i=0; i<=5; i++) temp_str[i]=SPort->PackIn[4+i];
						*TekPos_DZaslVAT = atoi(temp_str);
					for(int i=0; i<=6; i++) temp_str[i]=SPort->PackIn[11+i];
						*TekDavl_DZaslVAT = atoi(temp_str);
					if(SPort->PackIn[10] == '-')
						*TekDavl_DZaslVAT = *TekDavl_DZaslVAT * (-1);

					Edt_Zap7_1->Text = String(*TekPos_DZaslVAT);
					Edt_Zap7_2->Text = String(*TekDavl_DZaslVAT);
				}
				return 1;
			}
			else
			{
		if(ACom == 1)
                {
                    if((SPort->PackIn[0]!='R')||
                        (SPort->PackIn[1]!=':')) return 0;
                }
                else if(ACom == 2)
                {
                    if((SPort->PackIn[0]!='S')||
                        (SPort->PackIn[1]!=':')) return 0;
                }
                if(ACom == 7)
                {
		if((SPort->PackIn[0]!='i')||
                        (SPort->PackIn[1]!=':')||
                        (SPort->PackIn[2]!='7')||
                        (SPort->PackIn[3]!='6')) return 0;
				    // расшифровывается общий опрос
				    char temp_str[6] = {0};
				    for(int i=0; i<=5; i++) temp_str[i]=SPort->PackIn[4+i];
					    *TekPos_DZaslVAT = atoi(temp_str);
				    for(int i=0; i<=6; i++) temp_str[i]=SPort->PackIn[11+i];
					    *TekDavl_DZaslVAT = atoi(temp_str);
				    if(SPort->PackIn[10] == '-')
					    *TekDavl_DZaslVAT = *TekDavl_DZaslVAT * (-1);

				    if(*TekDavl_DZaslVAT < 0) *TekDat_DZaslVAT = 0;
				    else if(*TekDavl_DZaslVAT > 10000) *TekDat_DZaslVAT = 10000;
				    else *TekDat_DZaslVAT = *TekDavl_DZaslVAT;

				    Edt_Zap7_1->Text = String(*TekPos_DZaslVAT);
				    Edt_Zap7_2->Text = String(*TekDavl_DZaslVAT);
                }
                if(ACom == *ZadCom_DZaslVAT)
                {
                    *Pr_DZaslVAT = true;
                    *Otvet_DZaslVAT = ACom;
                }
                return 1;
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
//-- Функция связи с датчиком --//
//---------------------------------------------------------------------------
bool SDZaslVAT::DZaslVAT_Manage(unsigned int SH,bool Zap_type)
{
	// переключение радиобаттонов на страницах датчиков при их работе
	RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

    if(SH == 0) // посылка запроса
	{
		SPort->PackageClear();
		// Функция формирования запроса
		DZaslVAT_FrmZap(Zap_type);
		SPort->VisPackASCII(0);

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
		if(DZaslVAT_ChkRep(Zap_type))
		{
			SPort->VisPackASCII(1);
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
void Init_DZaslVAT()
{
	for(int i=0;i<DZaslVAT_COUNT;i++ )
    {
		DZaslVAT[i] = new SDZaslVAT();
        DZaslVAT[i]->Err = 0;				// кол-во ошибок
	    DZaslVAT[i]->Max_err = 5;			// максимум ошибок
	    DZaslVAT[i]->ACom = 0;				// текущий автоматический запрос
	    DZaslVAT[i]->RCom = 0;				// текущий ручной апрос
	    DZaslVAT[i]->Buf_len = 0;			// длина запроса
    }

	// Заслонка
	DZaslVAT[0]->name = "Дросс. засл. камеры(VAT)"; // название для отображения
	DZaslVAT[0]->ZadCom_DZaslVAT = &KOM_DZASL1;   		// команда задания
	DZaslVAT[0]->ZadData_DZaslVAT = &DATA_DZASL1;  		// задание
    DZaslVAT[0]->Otvet_DZaslVAT = &OTVET_DZASL1;
    DZaslVAT[0]->Pr_DZaslVAT = &PR_DZASL1;
	DZaslVAT[0]->TekDat_DZaslVAT = &D_D2;   			// показания датчика
	DZaslVAT[0]->TekPos_DZaslVAT = &TEK_POZ_DZASL1;		// переменная давления для записи
	DZaslVAT[0]->TekDavl_DZaslVAT = &TEK_DAVL_DZASL1;	// переменная давления для записи
	DZaslVAT[0]->SPort = Comport[0];     // порт
	DZaslVAT[0]->diagnS_byte = 0;        // номер байта связной диагностики
	DZaslVAT[0]->diagnS_mask = 0x40;		// маска байта связной диагностики
	DZaslVAT[0]->DZaslVAT_Gen();

    DZaslVAT[1]->name = "Дросс. засл. масс-спектр.(VAT)"; // название для отображения
	DZaslVAT[1]->ZadCom_DZaslVAT = &KOM_DZASL2;   		// команда задания
	DZaslVAT[1]->ZadData_DZaslVAT = &DATA_DZASL2;  		// задание
    DZaslVAT[1]->Otvet_DZaslVAT = &OTVET_DZASL2;
    DZaslVAT[1]->Pr_DZaslVAT = &PR_DZASL2;
	DZaslVAT[1]->TekDat_DZaslVAT = &D_D0;   			// показания датчика
	DZaslVAT[1]->TekPos_DZaslVAT = &TEK_POZ_DZASL2;		// переменная давления для записи
	DZaslVAT[1]->TekDavl_DZaslVAT = &TEK_DAVL_DZASL2;	// переменная давления для записи
	DZaslVAT[1]->SPort = Comport[1];     // порт
	DZaslVAT[1]->diagnS_byte = 2;        // номер байта связной диагностики
	DZaslVAT[1]->diagnS_mask = 0x80;		// маска байта связной диагностики
	DZaslVAT[1]->DZaslVAT_Gen();
}