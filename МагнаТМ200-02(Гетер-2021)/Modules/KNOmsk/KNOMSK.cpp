//---------------------------------------------------------------------------
#include "KNOmsk.h"
//---------------------------------------------------------------------------
//-- Генерация страницы --//
//---------------------------------------------------------------------------
void S_KNOmsk::KNOmsk_Gen()
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
	Pnl_Parent->Height = 676;  // для FullHD
	//Pnl_Parent->Height = 634;  // для 1280x1024
	Pnl_Parent->Width = 1176;

	Lbl_Uni = new TLabel(Pnl_Parent);
	Lbl_Uni->Parent = Pnl_Parent;
	Lbl_Uni->Top = 15;
	Lbl_Uni->Left = 26;
	Lbl_Uni->Caption = "Адрес устройства: ---";
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
	GB_Main->Height = 312;
	GB_Main->Width = 472;
	GB_Main->Caption = " Управление ";
	GB_Main->Font->Name = "Arial";
	GB_Main->Font->Size = 12;
	GB_Main->Font->Color = clBlack;
	GB_Main->Font->Style = GB_Main->Font->Style << fsBold;

    Btn_Zap1 = new TButton(GB_Main);
	Btn_Zap1->Parent = GB_Main;
	Btn_Zap1->Top = 40;
	Btn_Zap1->Left = 28;
	Btn_Zap1->Font->Name = "Arial";
	Btn_Zap1->Font->Size = 12;
	Btn_Zap1->Font->Color = clBlack;
	Btn_Zap1->Font->Style = Btn_Zap1->Font->Style >> fsBold;
	Btn_Zap1->Caption = "Старт";
	Btn_Zap1->Width = 75;
	Btn_Zap1->Height = 26;
	Btn_Zap1->Hint = "1";
	Btn_Zap1->OnClick = KNOmsk_SetZap;

	Btn_Zap2 = new TButton(GB_Main);
	Btn_Zap2->Parent = GB_Main;
	Btn_Zap2->Top = 40;
	Btn_Zap2->Left = 120;
	Btn_Zap2->Font->Name = "Arial";
	Btn_Zap2->Font->Size = 12;
	Btn_Zap2->Font->Color = clBlack;
	Btn_Zap2->Font->Style = Btn_Zap2->Font->Style >> fsBold;
	Btn_Zap2->Caption = "Стоп";
	Btn_Zap2->Width = 75;
	Btn_Zap2->Height = 26;
	Btn_Zap2->Hint = "2";
	Btn_Zap2->OnClick = KNOmsk_SetZap;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 168;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Температура Т1, °С:";
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
	Lbl_Uni->Top = 200;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Температура Т2, °С:";
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
	Lbl_Uni->Top = 232;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Температура БКО1, °К:";
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
	Lbl_Uni->Top = 264;
	Lbl_Uni->Left = 28;
	Lbl_Uni->Caption = "Температура БКО2, °К:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 190;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Edt_Otv1 = new TEdit(GB_Main);
	Edt_Otv1->Parent = GB_Main;
	Edt_Otv1->Top = 164;
	Edt_Otv1->Left = 208;
	Edt_Otv1->Font->Name = "Arial";
	Edt_Otv1->Font->Size = 13;
	Edt_Otv1->Font->Color = clBlack;
	Edt_Otv1->Font->Style = Edt_Otv1->Font->Style >> fsBold;
	Edt_Otv1->BevelKind = bkFlat;
	Edt_Otv1->BevelOuter = bvRaised;
	Edt_Otv1->BorderStyle = bsNone;
	Edt_Otv1->ReadOnly = true;
	Edt_Otv1->Color = clSkyBlue;
	Edt_Otv1->Height = 26;
	Edt_Otv1->Width = 48;

    Edt_Otv2 = new TEdit(GB_Main);
	Edt_Otv2->Parent = GB_Main;
	Edt_Otv2->Top = 196;
	Edt_Otv2->Left = 208;
	Edt_Otv2->Font->Name = "Arial";
	Edt_Otv2->Font->Size = 13;
	Edt_Otv2->Font->Color = clBlack;
	Edt_Otv2->Font->Style = Edt_Otv2->Font->Style >> fsBold;
	Edt_Otv2->BevelKind = bkFlat;
	Edt_Otv2->BevelOuter = bvRaised;
	Edt_Otv2->BorderStyle = bsNone;
	Edt_Otv2->ReadOnly = true;
	Edt_Otv2->Color = clSkyBlue;
	Edt_Otv2->Height = 26;
	Edt_Otv2->Width = 48;

    Edt_Otv3 = new TEdit(GB_Main);
	Edt_Otv3->Parent = GB_Main;
	Edt_Otv3->Top = 228;
	Edt_Otv3->Left = 208;
	Edt_Otv3->Font->Name = "Arial";
	Edt_Otv3->Font->Size = 13;
	Edt_Otv3->Font->Color = clBlack;
	Edt_Otv3->Font->Style = Edt_Otv3->Font->Style >> fsBold;
	Edt_Otv3->BevelKind = bkFlat;
	Edt_Otv3->BevelOuter = bvRaised;
	Edt_Otv3->BorderStyle = bsNone;
	Edt_Otv3->ReadOnly = true;
	Edt_Otv3->Color = clSkyBlue;
	Edt_Otv3->Height = 26;
	Edt_Otv3->Width = 48;

    Edt_Otv4 = new TEdit(GB_Main);
	Edt_Otv4->Parent = GB_Main;
	Edt_Otv4->Top = 260;
	Edt_Otv4->Left = 208;
	Edt_Otv4->Font->Name = "Arial";
	Edt_Otv4->Font->Size = 13;
	Edt_Otv4->Font->Color = clBlack;
	Edt_Otv4->Font->Style = Edt_Otv4->Font->Style >> fsBold;
	Edt_Otv4->BevelKind = bkFlat;
	Edt_Otv4->BevelOuter = bvRaised;
	Edt_Otv4->BorderStyle = bsNone;
	Edt_Otv4->ReadOnly = true;
	Edt_Otv4->Color = clSkyBlue;
	Edt_Otv4->Height = 26;
	Edt_Otv4->Width = 48;

    // второй столбец

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 40;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Пуск БКО1:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 72;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Пуск КУ:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 104;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "БКО1 отключится:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 136;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Перегрузка:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 168;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Перефазировка:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 200;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Перегрев БКО1:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 232;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Перегрев Т1:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    Lbl_Uni = new TLabel(GB_Main);
	Lbl_Uni->Parent = GB_Main;
	Lbl_Uni->Top = 264;
	Lbl_Uni->Left = 296;
	Lbl_Uni->Caption = "Перегрев Т2:";
	Lbl_Uni->Font->Name = "Arial";
	Lbl_Uni->Font->Size = 12;
	Lbl_Uni->Font->Color = clBlack;
	Lbl_Uni->Transparent = true;
	Lbl_Uni->Height = 19;
	Lbl_Uni->Width = 100;
	Lbl_Uni->Layout = tlTop;
	Lbl_Uni->Font->Style = Lbl_Uni->Font->Style >> fsBold;

    CB_alarm1 = new TCheckBox(GB_Main);
	CB_alarm1->Parent = GB_Main;
	CB_alarm1->Top = 40;
	CB_alarm1->Left = 432;
	CB_alarm1->Height = 17;
	CB_alarm1->Width = 17;
	CB_alarm1->Caption = "";

    CB_alarm2 = new TCheckBox(GB_Main);
	CB_alarm2->Parent = GB_Main;
	CB_alarm2->Top = 72;
	CB_alarm2->Left = 432;
	CB_alarm2->Height = 17;
	CB_alarm2->Width = 17;
	CB_alarm2->Caption = "";

    CB_alarm3 = new TCheckBox(GB_Main);
	CB_alarm3->Parent = GB_Main;
	CB_alarm3->Top = 104;
	CB_alarm3->Left = 432;
	CB_alarm3->Height = 17;
	CB_alarm3->Width = 17;
	CB_alarm3->Caption = "";

    CB_alarm4 = new TCheckBox(GB_Main);
	CB_alarm4->Parent = GB_Main;
	CB_alarm4->Top = 136;
	CB_alarm4->Left = 432;
	CB_alarm4->Height = 17;
	CB_alarm4->Width = 17;
	CB_alarm4->Caption = "";

    CB_alarm5 = new TCheckBox(GB_Main);
	CB_alarm5->Parent = GB_Main;
	CB_alarm5->Top = 168;
	CB_alarm5->Left = 432;
	CB_alarm5->Height = 17;
	CB_alarm5->Width = 17;
	CB_alarm5->Caption = "";

    CB_alarm6 = new TCheckBox(GB_Main);
	CB_alarm6->Parent = GB_Main;
	CB_alarm6->Top = 200;
	CB_alarm6->Left = 432;
	CB_alarm6->Height = 17;
	CB_alarm6->Width = 17;
	CB_alarm6->Caption = "";

    CB_alarm7 = new TCheckBox(GB_Main);
	CB_alarm7->Parent = GB_Main;
	CB_alarm7->Top = 232;
	CB_alarm7->Left = 432;
	CB_alarm7->Height = 17;
	CB_alarm7->Width = 17;
	CB_alarm7->Caption = "";

    CB_alarm8 = new TCheckBox(GB_Main);
	CB_alarm8->Parent = GB_Main;
	CB_alarm8->Top = 264;
	CB_alarm8->Left = 432;
	CB_alarm8->Height = 17;
	CB_alarm8->Width = 17;
	CB_alarm8->Caption = "";

}
//---------------------------------------------------------------------------
unsigned short S_KNOmsk::KNOmsk_GenCC(char* buf,unsigned int len) // рассчет контр. суммы
{
  unsigned short crc = 0xFFFF;

  for(int pos = 0; pos < len; pos++)
  {
    // обрезаем char, инече символы типа '8f' приходят как 'ffffff8f'
    crc ^= (unsigned short)buf[pos] & 0xFF;
    for (int i = 8; i != 0; i--)
    {
      if (crc & 0x0001)
        {
        crc >>= 1;
        crc ^= 0xA001;
      }
      else
        crc >>= 1;
    }
  }
  // мдладший и старший байт поменяны местами !!!
  return crc;
}
//---------------------------------------------------------------------------
//-- Ручной запрос со страницы --//
//---------------------------------------------------------------------------
void __fastcall S_KNOmsk::KNOmsk_SetZap(TObject *Sender)
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
void S_KNOmsk::KNOmsk_FrmZap(bool Zap_type)
{
    if(Zap_type)    // ручной запрос
	{
		switch(RCom)
		{
			case 1:
			{
                Buf_len = 6;
                for(int i=0;i<Buf_len;i++)
                SPort->PackOut[i] = KNOmsk_Req_Buf[1][i];

			};break;
			case 2:
			{
                Buf_len = 6;
                for(int i=0;i<Buf_len;i++)
                SPort->PackOut[i] = KNOmsk_Req_Buf[2][i];
			};break;
        };
	}
	else
	{   // если нет задания - опрос состояния
		if(!*Com_KNOmsk) *Com_KNOmsk = 3;

		if(*Com_KNOmsk == 1) ACom = 1;
                else if(*Com_KNOmsk == 2) ACom = 2;
        else
        {
            // если нужен опрос обнуляем признаки отдельных запросов
            // чтобы гарантированно их выполнить
            if(!*Pr_KNOmsk && pr_sost1 && pr_sost2)
            {
                pr_sost1 = 0;
                pr_sost2 = 0;
            }
            if(!pr_sost1) ACom = 3;
            else if(!pr_sost2) ACom = 4;
            else
            {
                if(ACom == 3) ACom = 4;
                else ACom = 3;
            }
        }

		switch(ACom)
		{
			case 1:
			{
                Buf_len = 6;
                for(int i=0;i<Buf_len;i++)
                SPort->PackOut[i] = KNOmsk_Req_Buf[1][i];
			};break;
			case 2:
			{
                Buf_len = 6;
                for(int i=0;i<Buf_len;i++)
                SPort->PackOut[i] = KNOmsk_Req_Buf[2][i];
			};break;
			case 3:
			{
                Buf_len = 6;
                for(int i=0;i<Buf_len;i++)
                SPort->PackOut[i] = KNOmsk_Req_Buf[3][i];
			};break;
			case 4:
			{
                Buf_len = 6;
                for(int i=0;i<Buf_len;i++)
                SPort->PackOut[i] = KNOmsk_Req_Buf[4][i];
			};break;
		};
    }

    unsigned short crc16 = KNOmsk_GenCC(SPort->PackOut,Buf_len);

    SPort->PackOut[Buf_len] = char(crc16 & 0xFF);
    SPort->PackOut[Buf_len+1] = char((crc16 & 0xFF00) >> 8);

	Buf_len = Buf_len + 2;
}
//---------------------------------------------------------------------------
//-- Функция обработки ответа --//
//---------------------------------------------------------------------------
bool S_KNOmsk::KNOmsk_ChkRep(bool Zap_type)
{
    if(Zap_type)    // ручной запрос
	{
        switch(RCom)
		{
			case 1:
			{
                if((SPort->PackIn[0] == 0x01) &&
                    (SPort->PackIn[1] == 0x05))
                {
                    return 1;
                }
                else return 0;
			};break;
			case 2:
			{
                if((SPort->PackIn[0] == 0x01) &&
                    (SPort->PackIn[1] == 0x05))
                {
                    return 1;
                }
                else return 0;
			};break;
        };
    }
    else
    {
        switch(ACom)
		{
			case 1:
			{
                if((SPort->PackIn[0] == 0x01) &&
                    (SPort->PackIn[1] == 0x05))
                {
                    *Otv_KNOmsk = 1;
                    if(ACom == *Com_KNOmsk) *Pr_KNOmsk = 1;
                    return 1;
                }
                else return 0;
			};break;
			case 2:
			{
                if((SPort->PackIn[0] == 0x01) &&
                    (SPort->PackIn[1] == 0x05))
                {
                    *Otv_KNOmsk = 2;
                    if(ACom == *Com_KNOmsk) *Pr_KNOmsk = 1;
                    return 1;
                }
                else return 0;
			};break;
			case 3:
			{
                if((SPort->PackIn[0] == 0x01) &&
                    (SPort->PackIn[1] == 0x04) &&
                    Buf_len == 19)
                {
                    // проверка контр. суммы
                    unsigned short ras_cc = KNOmsk_GenCC(SPort->PackIn,17);
                    unsigned short got_cc = 0;
                    unsigned short temp_short = 0;
                    got_cc = SPort->PackIn[18] << 8;
                    got_cc = got_cc + SPort->PackIn[17];
                    if(ras_cc != got_cc) return 0;

                    // температура Т1 в 0.1 градусах С
                    temp_short = SPort->PackIn[3] << 8;
                    temp_short = temp_short + SPort->PackIn[4];
                    *OtvM_KNOmsk[4] = temp_short;
                    Edt_Otv1->Text = FloatToStrF((float)*OtvM_KNOmsk[4]/10,ffFixed,5,1);
                    // температура Т2 в 0.1 градусах С
                    temp_short = SPort->PackIn[5] << 8;
                    temp_short = temp_short + SPort->PackIn[6];
                    *OtvM_KNOmsk[5] = temp_short;
                    Edt_Otv2->Text = FloatToStrF((float)*OtvM_KNOmsk[5]/10,ffFixed,5,1);
                    // температура БКО1 в 0.1 градусах К
                    temp_short = SPort->PackIn[7] << 8;
                    temp_short = temp_short + SPort->PackIn[8];
                    *OtvM_KNOmsk[3] = temp_short;
                    Edt_Otv3->Text = FloatToStrF((float)*OtvM_KNOmsk[3]/10,ffFixed,5,1);
                    // температура БКО2 в 0.1 градусах К
                    //*Otv_KNOmsk[3] = SPort->PackIn[7] << 8 + SPort->PackOut[8];
                    Edt_Otv4->Text = "----";
                    // состояние блока 3 и 2 байты
                    temp_short = SPort->PackIn[11] << 8;
                    temp_short = temp_short + SPort->PackIn[12];
                    *OtvM_KNOmsk[0] = temp_short;
                    CB_alarm1->Checked = bool(*OtvM_KNOmsk[0]&0x800);
                    CB_alarm2->Checked = bool(*OtvM_KNOmsk[0]&0x100);
                    CB_alarm3->Checked = bool(*OtvM_KNOmsk[0]&0x20);
                    CB_alarm4->Checked = bool(*OtvM_KNOmsk[0]&0x10);
                    CB_alarm5->Checked = bool(*OtvM_KNOmsk[0]&0x08);
                    CB_alarm6->Checked = bool(*OtvM_KNOmsk[0]&0x01);
                    // состояние блока 1 и 0 байты
                    temp_short = SPort->PackIn[13] << 8;
                    temp_short = temp_short + SPort->PackIn[14];
                    *OtvM_KNOmsk[1] = temp_short;
                    CB_alarm7->Checked = bool(*OtvM_KNOmsk[1]&0x8000);
                    CB_alarm8->Checked = bool(*OtvM_KNOmsk[1]&0x10);
                    // состояние датчиков температуры
                    temp_short = SPort->PackIn[15] << 8;
                    temp_short = temp_short + SPort->PackIn[16];
                    *OtvM_KNOmsk[2] = temp_short;

                    *Otv_KNOmsk = 3;
                    pr_sost1 = 1;
                    if(((ACom == 3)||(ACom == 4)) && pr_sost1 && pr_sost2) *Pr_KNOmsk = 1;

                    return 1;
                }
                else return 0;
			};break;
			case 4:
			{
                if((SPort->PackIn[0] == 0x01) &&
                    (SPort->PackIn[1] == 0x04) &&
                    Buf_len == 7)
                {
                    // проверка контр. суммы
                    unsigned short ras_cc = KNOmsk_GenCC(SPort->PackIn,5);
                    unsigned short got_cc = 0;
                    unsigned short temp_short = 0;
                    got_cc = SPort->PackIn[6] << 8;
                    got_cc = got_cc + SPort->PackIn[5];
                    if(ras_cc != got_cc) return 0;

                    // скорость вращения ШД1 и ШД2
                    temp_short = SPort->PackIn[3] << 8;
                    temp_short = temp_short + SPort->PackIn[4];
                    *OtvM_KNOmsk[6] = temp_short;

                    *Otv_KNOmsk = 3;
                    pr_sost2 = 1;
                    if(((ACom == 3)||(ACom == 4)) && pr_sost1 && pr_sost2) *Pr_KNOmsk = 1;

                    return 1;
                }
                else return 0;
			};break;
		};
    }
    return 0;
}
//---------------------------------------------------------------------------
//-- Функция связи с датчиком --//
//---------------------------------------------------------------------------
bool S_KNOmsk::KNOmsk_Manage(unsigned int SH,bool Zap_type)
{
	// переключение радиобаттонов на страницах датчиков при их работе
	RB_prd->Checked = !SH;
    RB_prm->Checked = SH;

    if(SH == 0) // посылка запроса
	{
		SPort->PackageClear();
		// Функция формирования запроса
		KNOmsk_FrmZap(Zap_type);
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
        Buf_len = SPort->Port.GetRBSize();
        if(Buf_len < PACKAGE_COUNT)
        {
            SPort->Port.Read(SPort->PackIn,Buf_len);
        }
        else
        {
            // очистка буфера приёма
            SPort->Port.ResetRB();
            SPort->DevState++;
            return 0;
        }

		// проверка ответа
		if(KNOmsk_ChkRep(Zap_type))
		{
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
//-- Инициализация структур --//
//---------------------------------------------------------------------------
void Init_KNOmsk()
{
	for(int i=0;i<KNOmsk_COUNT;i++ )
		KNOmsk[i] = new S_KNOmsk();

	// Заслонка
	KNOmsk[0]->name = "Крионасос"; // название для отображения

    KNOmsk[0]->Pr_KNOmsk = &PR_SV_KN;
	KNOmsk[0]->Com_KNOmsk = &KOM_KN;		// команда
    KNOmsk[0]->Otv_KNOmsk = &OTVET_KN;		// ответ
    for(int i=0;i<7;i++)
        KNOmsk[0]->OtvM_KNOmsk[i] = &OTVET_KN_M[i];		// ответ состояний

	KNOmsk[0]->SPort = Comport[5];     // порт
	KNOmsk[0]->diagnS_byte = 2;        // номер байта связной диагностики
	KNOmsk[0]->diagnS_mask = 0x40;		// маска байта связной диагностики

	KNOmsk[0]->Err = 0;				// кол-во ошибок
	KNOmsk[0]->Max_err = 5;			// максимум ошибок
	KNOmsk[0]->ACom = 0;				// текущий автоматический запрос
	KNOmsk[0]->RCom = 0;				// текущий ручной апрос
	KNOmsk[0]->Buf_len = 0;			// длина запроса
	KNOmsk[0]->KNOmsk_Gen();
}