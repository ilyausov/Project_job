//---------------------------------------------------------------------------
#include "mserial.h"
#include "Com.h"
//---------------------------------------------------------------------------
//-- Очищение буферов приёма/передачи --//
//---------------------------------------------------------------------------
void SComport::PackageClear()
{
    for(int i=0;i<PACKAGE_COUNT;i++)
	{
		PackOut[i] = 0;
		PackIn[i] = 0;
	}
}
//---------------------------------------------------------------------------
//-- Вывод буферов в строку IVE блоков --//
//---------------------------------------------------------------------------
/*void SComport::VisPackIVE(bool type,unsigned char len)
{
	char str[1];

	if(type)    // буфер приема
	{
		Edt_prm->Text = "";
		for(int i=0;i<len;i++)
		{
            Edt_prm->Text = Edt_prm->Text + IntToHex(PackIn[i],2) + "";
		}
	}
	else
	{
		Edt_prd->Text = "";
		for(int i=0;i<len;i++)
		{
            Edt_prd->Text = Edt_prd->Text + IntToHex(PackOut[i],2) + "";
		}
    }
} */
//---------------------------------------------------------------------------
//-- Вывод буфера в строку Modbus HEX --//
//---------------------------------------------------------------------------
void SComport::VisPackRTU(bool type,unsigned char len)
{
	if(type)    // буфер приема
	{
		Edt_prm->Text = "";
		for(int i=0;i<len;i++)
		{
			Edt_prm->Text = Edt_prm->Text + IntToHex(PackIn[i],2);
		}
	}
	else
	{
		Edt_prd->Text = "";
		for(int i=0;i<len;i++)
		{	
			Edt_prd->Text = Edt_prd->Text + IntToHex(PackOut[i],2);
		}
    }
}
//---------------------------------------------------------------------------
//-- Вывод буфера в строку ASCII --//
//---------------------------------------------------------------------------
void SComport::VisPackASCII(bool type)
{
	if(type)    // буфер приема
	{
		Edt_prm->Text = "";
		for(int i=0;PackIn[i]!=0;i++)
		{	
			if(PackIn[i] == 10)
				Edt_prm->Text = Edt_prm->Text + "/lf";
			else if(PackIn[i] == 13)
				Edt_prm->Text = Edt_prm->Text + "/cr";
			else
				Edt_prm->Text = Edt_prm->Text + char(PackIn[i]);
		}
	}
	else
    {
		Edt_prd->Text = "";
		for(int i=0;PackOut[i]!=0;i++)
		{	
			if(PackOut[i] == 10)
				Edt_prd->Text = Edt_prd->Text + "/lf";
			else if(PackOut[i] == 13)
				Edt_prd->Text = Edt_prd->Text + "/cr";
			else
				Edt_prd->Text = Edt_prd->Text + char(PackOut[i]);
		}
    }
}
//---------------------------------------------------------------------------
//-- Перезагрузка порта --//
//---------------------------------------------------------------------------
void __fastcall SComport::Reser_Port(TObject *Sender)
{
	if(StrToInt(((TButton*)Sender)->Hint))  // перезапуск порта
	{
		if(State)
		{
			State = 0;
			Port.Close();
			BTN_reset->Caption = "Пуск порта";
            return;
		}
		else
		{
			if(Port.Open(PortName.c_str(),B_Rate,Data8Bit,P_Rate,OneStopBit))
			{
				State = 1;
                port_err = 0;
				BTN_reset->Caption = "Стоп порта";
                return;
			}
			else
			{
                State = 0;
			    Port.Close();
                BTN_reset->Caption = "Пуск порта";
                if(Port.Open(PortName.c_str(),B_Rate,Data8Bit,P_Rate,OneStopBit))
			    {
                    State = 1;
                    port_err = 0;
				    BTN_reset->Caption = "Стоп порта";
                    return;
                }
                else
                {
                    String war_str = "Внимание! Порт ";
				    war_str += PortName.c_str();
				    war_str += " невозможно открыть!";
				    MessageDlg(war_str,mtWarning,TMsgDlgButtons() << mbOK, 0);
                    return;
                }
			}
		}
	}
	else    // наладка
	{
		if(Pr_nal)
		{
			Pr_nal = 0;
			BTN_nal->Caption = "В наладку";
		}
		else
		{
			Pr_nal = 1;
			BTN_nal->Caption = "В работу";
        }
	}
}
//---------------------------------------------------------------------------
//-- Создание элементов порта --//
//---------------------------------------------------------------------------
void SComport::ComPanGen(unsigned char port_num)
{
	// создаем вкладки портов

	Timer_Com = new TTimer(Form1);
	Timer_Com->Interval = Timer_Int;
	Timer_Com->Enabled = true;
    Timer_Com->Name = "ComTimer" + IntToStr(port_num + 1);
    Timer_Com->OnTimer = Com_Timer;

	LBL_name = new TLabel(PNL_parent);
	LBL_name->Parent = PNL_parent;
	LBL_name->Top = 21;
	LBL_name->Left = 26;
	LBL_name->Caption = "Устройства, подключенные к порту " + PortName;
	LBL_name->Font->Name = "Arial";
	LBL_name->Font->Size = 15;
	LBL_name->Transparent = true;
	LBL_name->Visible = true;
	LBL_name->Layout = tlTop;

	LBL_name = new TLabel(PNL_parent);
	LBL_name->Parent = PNL_parent;
	LBL_name->Top = 60;
	LBL_name->Left = 26;
	LBL_name->Caption = "Передача :";
	LBL_name->Font->Name = "Arial";
	LBL_name->Font->Size = 12;
	LBL_name->Transparent = true;
	LBL_name->Visible = true;
	LBL_name->Layout = tlTop;

	LBL_name = new TLabel(PNL_parent);
	LBL_name->Parent = PNL_parent;
	LBL_name->Top = 92;
	LBL_name->Left = 51;
	LBL_name->Caption = "Прием :";
	LBL_name->Font->Name = "Arial";
	LBL_name->Font->Size = 12;
	LBL_name->Transparent = true;
	LBL_name->Visible = true;
	LBL_name->Layout = tlTop;

	LBL_otl = new TLabel(Form1);
	LBL_otl->Top = 6;
	LBL_otl->Left = 150 + 100*(port_num);
	LBL_otl->Caption = " Отл. " + PortName + " ";
	LBL_otl->Parent = Form1->Pnl_Title;
    //LBL_otl->Font->Name = "Franklin Gothic Demi Cond";
    LBL_otl->Font->Name = "Franklin Gothic Medium";
	LBL_otl->Font->Size = 14;
	LBL_otl->Font->Color = clWhite;
    LBL_otl->Color = clRed;
	LBL_otl->Transparent = false;
	LBL_otl->Visible = false;
    LBL_otl->Layout = tlTop;
	LBL_otl->Font->Style = LBL_otl->Font->Style >> fsBold;

	Edt_prd = new TEdit(PNL_parent);
	Edt_prd->Parent = PNL_parent;
	Edt_prd->Top = 57;
	Edt_prd->Left = 112;
	Edt_prd->Font -> Name = "Arial";
	Edt_prd->Font -> Size = 13;
	Edt_prd->BevelKind = bkFlat;
	Edt_prd->BevelOuter = bvRaised;
	Edt_prd->BorderStyle = bsNone;
	Edt_prd->ReadOnly = true;
	Edt_prd->Height = 26;
	Edt_prd->Width = 850;

	Edt_prm = new TEdit(PNL_parent);
	Edt_prm->Parent = PNL_parent;
	Edt_prm->Top = 89;
	Edt_prm->Left = 112;
	Edt_prm->Font -> Name = "Arial";
	Edt_prm->Font -> Size = 13;
	Edt_prm->BevelKind = bkFlat;
	Edt_prm->BevelOuter = bvRaised;
	Edt_prm->BorderStyle = bsNone;
	Edt_prm->ReadOnly = true;
	Edt_prm->Height = 26;
	Edt_prm->Width = 850;

	RB_prd = new TRadioButton(PNL_parent);
	RB_prd->Parent = PNL_parent;
	RB_prd->Top = 62;
	RB_prd->Left = 970;
	RB_prd->Height = 14;
	RB_prd->Width = 14;
	RB_prd->Caption = "";

	RB_prm = new TRadioButton(PNL_parent);
	RB_prm->Parent = PNL_parent;
	RB_prm->Top = 94;
	RB_prm->Left = 970;
	RB_prm->Height = 14;
	RB_prm->Width = 14;
	RB_prm->Caption = "";

	CB_status = new TCheckBox(PNL_parent);
	CB_status->Parent = PNL_parent;
	CB_status->Top = 61;
	CB_status->Left = 1150;
	CB_status->Height = 17;
	CB_status->Width = 17;
	CB_status->Caption = "";

	CB_nal = new TCheckBox(PNL_parent);
	CB_nal->Parent = PNL_parent;
	CB_nal->Top = 93;
	CB_nal->Left = 1150;
	CB_nal->Height = 17;
	CB_nal->Width = 17;
	CB_nal->Caption = "";

	BTN_reset = new TButton(PNL_parent);
	BTN_reset->Parent = PNL_parent;
	BTN_reset->Top = 56;
	BTN_reset->Left = 1040;
	BTN_reset->Height = 27;
	BTN_reset->Width = 100;
    BTN_reset->Font->Size = 12;
	BTN_reset->Caption = "Пуск порта";
	BTN_reset->Hint = "1";
	BTN_reset->OnClick = Reser_Port;

	BTN_nal = new TButton(PNL_parent);
	BTN_nal->Parent = PNL_parent;
	BTN_nal->Top = 88;
	BTN_nal->Left = 1040;
	BTN_nal->Height = 27;
	BTN_nal->Width = 100;
    BTN_nal->Font->Size = 12;
	BTN_nal->Caption = "В наладку";
	BTN_nal->Hint = "0";
	BTN_nal->OnClick = Reser_Port;

	PC_Com = new TPageControl(PNL_parent);
	PC_Com->Parent = PNL_parent;
    PC_Com->Style = tsButtons;
    //PC_Com->Color = clSilver;
	PC_Com->Top = 136;
	PC_Com->Left = 8;
	PC_Com->Height = 716;
	PC_Com->Width = 1184;
}
//---------------------------------------------------------------------------
//-- Функция инициализации объектов --//
//---------------------------------------------------------------------------
void Init_SComport()
{
	for(int i=0;i<PORT_COUNT;i++ )
    {
        Comport[i] = new SComport();
        Comport[i]->Port = MSCom[i];
	    Comport[i]->State = 0;
	    Comport[i]->B_Rate = CBR_9600;
        Comport[i]->P_Rate = 0; // nonparity
	    Comport[i]->Pr_nal = 0;
	    Comport[i]->PortTask = 0;
	    Comport[i]->DevState = 0;
	    Comport[i]->DevErr = 0;
	    Comport[i]->Timer_Int = 140;
	    Comport[i]->Dev_TK = 150;
	    Comport[i]->Dev_Timer = 0;
    }

	// Comport[0] - Com1
	Comport[0]->PortName = "COM1";
	Comport[0]->PNL_parent = Form1->Pnl_Com1;
	Comport[0]->ComPanGen(0);

    // Comport[1] - Com2
	Comport[1]->PortName = "COM2";
	Comport[1]->PNL_parent = Form1->Pnl_Com2;
	Comport[1]->ComPanGen(1);

    // Comport[2] - Com3
	Comport[2]->PortName = "COM3";
	Comport[2]->PNL_parent = Form1->Pnl_Com3;
	Comport[2]->ComPanGen(2);

	// Comport[3] - Com4
	Comport[3]->PortName = "COM4";
    Comport[3]->P_Rate = 2; // evenparity
	Comport[3]->PNL_parent = Form1->Pnl_Com4;
	Comport[3]->ComPanGen(3);
}


