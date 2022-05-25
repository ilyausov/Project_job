//---------------------------------------------------------------------------
#include "mserial.h"
#include "Com.h"
//---------------------------------------------------------------------------
//-- �������� ������� �����/�������� --//
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
//-- ����� ������ � ������ Modbus HEX --//
//---------------------------------------------------------------------------
void SComport::VisPackRTU(bool type,unsigned char len)
{
	if(type)    // ����� ������
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
//-- ����� ������ � ������ ASCII --//
//---------------------------------------------------------------------------
void SComport::VisPackASCII(bool type)
{
	if(type)    // ����� ������
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
//-- ������������ ����� --//
//---------------------------------------------------------------------------
void __fastcall SComport::Reser_Port(TObject *Sender)
{
	if(StrToInt(((TButton*)Sender)->Hint))  // ���������� �����
	{
		if(State)
		{
			State = 0;
			Port.Close();
			BTN_reset->Caption = "���� �����";
		}
		else
		{
			if(Port.Open(PortName.c_str(),B_Rate,Data8Bit,P_Rate,OneStopBit))
			{
				State = 1;
				BTN_reset->Caption = "���� �����";
                com1_err_alarm = 0;
                com2_err_alarm = 0;
                com3_err_alarm = 0;
                com4_err_alarm = 0;
			}
			else
			{
				String war_str = "��������! ���� ";
				war_str += PortName.c_str();
				war_str += " ���������� �������!";
				MessageDlg(war_str,mtWarning,TMsgDlgButtons() << mbOK, 0);
			}
		}
	}
	else    // �������
	{
		if(Pr_nal)
		{
			Pr_nal = 0;
			BTN_nal->Caption = "� �������";
		}
		else
		{
			Pr_nal = 1;
			BTN_nal->Caption = "� ������";
        }
	}
}
//---------------------------------------------------------------------------
//-- �������� ��������� ����� --//
//---------------------------------------------------------------------------
void SComport::ComPanGen(unsigned char port_num)
{
	// ������� ������� ������

	Timer_Com = new TTimer(Form1);
	Timer_Com->Interval = Timer_Int;
	Timer_Com->Enabled = true;

	switch(port_num)
	{
		case 0:
		{
			Timer_Com->OnTimer = Comport[0]->Timer_Com1_Timer;
		};  break;
		case 1:
		{
			Timer_Com->OnTimer = Comport[1]->Timer_Com2_Timer;
		};  break;
		case 2:
		{
			Timer_Com->OnTimer = Comport[2]->Timer_Com3_Timer;
		};  break;
		case 3:
		{
			Timer_Com->OnTimer = Comport[3]->Timer_Com4_Timer;
		};  break;
	}

	LBL_name = new TLabel(PNL_parent);
	LBL_name->Parent = PNL_parent;
	LBL_name->Top = 21;
	LBL_name->Left = 26;
	LBL_name->Caption = "����������, ������������ � ����� " + PortName;
	LBL_name->Font->Name = "Arial";
	LBL_name->Font->Size = 15;
	LBL_name->Transparent = true;
	LBL_name->Visible = true;
	LBL_name->Layout = tlTop;

	LBL_name = new TLabel(PNL_parent);
	LBL_name->Parent = PNL_parent;
	LBL_name->Top = 60;
	LBL_name->Left = 26;
	LBL_name->Caption = "�������� :";
	LBL_name->Font->Name = "Arial";
	LBL_name->Font->Size = 12;
	LBL_name->Transparent = true;
	LBL_name->Visible = true;
	LBL_name->Layout = tlTop;

	LBL_name = new TLabel(PNL_parent);
	LBL_name->Parent = PNL_parent;
	LBL_name->Top = 92;
	LBL_name->Left = 51;
	LBL_name->Caption = "����� :";
	LBL_name->Font->Name = "Arial";
	LBL_name->Font->Size = 12;
	LBL_name->Transparent = true;
	LBL_name->Visible = true;
	LBL_name->Layout = tlTop;

	LBL_otl = new TLabel(Form1);
	LBL_otl->Top = 10;
	LBL_otl->Left = 250 + 140*(port_num);
	LBL_otl->Caption = "������� " + PortName;
	LBL_otl->Parent = Form1->Pnl_Title;
	LBL_otl->Font->Name = "Arial";
	LBL_otl->Font->Size = 12;
	LBL_otl->Font->Color = clRed;
	LBL_otl->Transparent = true;
	LBL_otl->Visible = false;
    LBL_otl->Layout = tlTop;
	LBL_otl->Font->Style = LBL_otl->Font->Style << fsBold;

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
	BTN_reset->Caption = "���� �����";
	BTN_reset->Hint = "1";
	BTN_reset->OnClick = Reser_Port;

	BTN_nal = new TButton(PNL_parent);
	BTN_nal->Parent = PNL_parent;
	BTN_nal->Top = 88;
	BTN_nal->Left = 1040;
	BTN_nal->Height = 27;
	BTN_nal->Width = 100;
    BTN_nal->Font->Size = 12;
	BTN_nal->Caption = "� �������";
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
//-- ������� ������������� �������� --//
//---------------------------------------------------------------------------
void Init_SComport()
{
	for(int i=0;i<PORT_COUNT;i++ )
		Comport[i] = new SComport();

	// Comport[0] - Com1
    Comport[0]->Port = Comport1;
	Comport[0]->State = 0;
        Comport[0]->B_Rate = CBR_9600;
	Comport[0]->P_Rate = 0;
	Comport[0]->Pr_nal = 0;
	Comport[0]->PortTask = 0;
	Comport[0]->DevState = 0;
	Comport[0]->DevErr = 0;
	Comport[0]->Timer_Int = 150;
	Comport[0]->Dev_TK = 200;
	Comport[0]->Dev_Timer = 0;
	Comport[0]->PortName = "COM1";
	Comport[0]->PNL_parent = Form1->Pnl_Com1;
	Comport[0]->ComPanGen(0);

    // Comport[1] - Com2
    Comport[1]->Port = Comport2;
	Comport[1]->State = 0;
	Comport[1]->B_Rate = CBR_9600;
        Comport[1]->P_Rate = 0;
	Comport[1]->Pr_nal = 0;
	Comport[1]->PortTask = 0;
	Comport[1]->DevState = 0;
	Comport[1]->DevErr = 0;
	Comport[1]->Timer_Int = 200;
	Comport[1]->Dev_TK = 250;
	Comport[1]->Dev_Timer = 0;
	Comport[1]->PortName = "COM2";
	Comport[1]->PNL_parent = Form1->Pnl_Com2;
	Comport[1]->ComPanGen(1);

    // Comport[2] - Com3
    Comport[2]->Port = Comport3;
	Comport[2]->State = 0;
        if(PR_KLASTER)
        {
                Comport[2]->B_Rate = CBR_115200;	// nonparity
                Comport[2]->P_Rate = 0;
        }
        else
        {
                Comport[2]->B_Rate = CBR_9600;
                Comport[2]->P_Rate = 2;	// evenparity
        }
	Comport[2]->Pr_nal = 0;
	Comport[2]->PortTask = 0;
	Comport[2]->DevState = 0;
	Comport[2]->DevErr = 0;
        if(PR_KLASTER)
        {
                Comport[2]->Timer_Int = 50;
                Comport[2]->Dev_TK = 70;
        }
        else
        {
                Comport[2]->Timer_Int = 100;
                Comport[2]->Dev_TK = 150;
        }
	Comport[2]->Dev_Timer = 0;
	Comport[2]->PortName = "COM3";
	Comport[2]->PNL_parent = Form1->Pnl_Com3;
	Comport[2]->ComPanGen(2);
	
	// Comport[3] - Com4
    Comport[3]->Port = Comport4;
	Comport[3]->State = 0;
	Comport[3]->B_Rate = CBR_9600;
	Comport[3]->P_Rate = 0;	// nonparity
	Comport[3]->Pr_nal = 0;
	Comport[3]->PortTask = 0;
	Comport[3]->DevState = 0;
	Comport[3]->DevErr = 0;
	Comport[3]->Timer_Int = 100;
	Comport[3]->Dev_TK = 150;
	Comport[3]->Dev_Timer = 0;
	Comport[3]->PortName = "COM4";
	Comport[3]->PNL_parent = Form1->Pnl_Com4;
	Comport[3]->ComPanGen(3);
}


