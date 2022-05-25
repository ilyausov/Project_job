//---------------------------------------------------------------------------
#include <vcl.h>
#include <vfw.h>
#include <Clipbrd.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "Unit1.h"
#include "Logic.cpp"
#include "Header.h"
#include "Modules\Com.cpp"
#include "Modules\DZaslVAT\DZaslVAT.cpp"
#include "Modules\MERA\MERA.cpp"
#include "Modules\TRMD\TRMD.cpp"
#include "Modules\DatMTM9D\DatMTM9D.cpp"
#include "Modules\AZdrive\AZdrive.cpp"
#include "Modules\IntMod\IntMod.cpp"

#include "Names.h"
#include "External.cpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "CSPIN"
#pragma resource "*.dfm"

TForm1 *Form1;

//---------------------------------------------------------------------------
//--�����: ������������ ����� � �������--//
//---------------------------------------------------------------------------
class TLogicThread : public TThread
{
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType;     // must be 0x1000
    LPCSTR szName;    // pointer to name (in user addr space)
    DWORD dwThreadID; // thread ID (-1=caller thread)
    DWORD dwFlags;    // reserved for future use, must be zero
  } THREADNAME_INFO;
private:
  void SetName();
protected:
    void __fastcall Execute();
public:
    LARGE_INTEGER
        TimeNew, TimeOld, TimeFreq;
    void __fastcall LM();
    __fastcall TLogicThread(bool CreateSuspended);
} *LogicThread;
//---------------------------------------------------------------------------
//--����������� ������� ����� ������--//
//---------------------------------------------------------------------------
__fastcall TLogicThread::TLogicThread(bool CreateSuspended)
    : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void TLogicThread::SetName()
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = "LogicThread";
    info.dwThreadID = -1;
    info.dwFlags = 0;

    __try
    {
         RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD),(DWORD*)&info );
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
}
//---------------------------------------------------------------------------
void __fastcall TLogicThread::Execute()
{
    SetName();
    //---- Place thread code here ----
    FreeOnTerminate = true; // ���������� ������ ����� ����������
    // ����������� ������� ����������
    QueryPerformanceFrequency(&TimeFreq);
    TimeFreq.QuadPart /= 1000;
    // ����������� �������� �������
    QueryPerformanceCounter(&TimeNew);
    while ( !Terminated )
    {
        TimeOld = TimeNew;
        // ����������� �������� �������
        QueryPerformanceCounter(&TimeNew);
        // ������� ���� ������
        Synchronize(LM);
    }
};
//---------------------------------------------------------------------------
//--�����: ������������ ����� � ������ ��������--//
//---------------------------------------------------------------------------
class TTimerExist : public TThread
{
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType;     // must be 0x1000
    LPCSTR szName;    // pointer to name (in user addr space)
    DWORD dwThreadID; // thread ID (-1=caller thread)
    DWORD dwFlags;    // reserved for future use, must be zero
  } THREADNAME_INFO;
private:
  void SetName();
protected:
    void __fastcall Execute();
public:
    void __fastcall EM();
    void __fastcall Timer1ms();
    __fastcall TTimerExist(bool CreateSuspended);
} *TimerExist;
//---------------------------------------------------------------------------
//--����������� ������� ����� ������--//
//---------------------------------------------------------------------------
__fastcall TTimerExist::TTimerExist(bool CreateSuspended)
    : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void TTimerExist::SetName()
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = "TimerExist";
    info.dwThreadID = -1;
    info.dwFlags = 0;
    __try
    {
         RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD),(DWORD*)&info );
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
}
//---------------------------------------------------------------------------
void __fastcall TTimerExist::Execute()
{
    SetName();
    //---- Place thread code here ----
    FreeOnTerminate = true; // ���������� ������ ����� ����������
    LARGE_INTEGER
        TimeNew, TimeOld, TimeFreq;
    // ����������� ������� ����������
    QueryPerformanceFrequency(&TimeFreq);
    TimeFreq.QuadPart /= 1000;
    // ����������� �������� �������
    QueryPerformanceCounter(&TimeOld);
    unsigned long
        timeOld = GetTickCount(),
        timeNew;
    // ����������� �������� �������
    while ( !Terminated )
    {
        // ����������� ������ �������� �������
        timeNew = GetTickCount();
        // ��������� ������ ������� � ���������� �� ������� � 1 ��
        if ( timeNew != timeOld )
        {
            for ( unsigned long i = 0 ; i < ( timeNew - timeOld ) ; i++ )
                // ��������� ��������� �����
                Synchronize(Timer1ms);
            // �������� "������" �����
            timeOld = timeNew;
        }
        // ����������� ������ �������� �������
        QueryPerformanceCounter(&TimeNew);
        // ��������� ������ ������� � ���������� �� ������� � 1 ��
        if (((TimeNew.QuadPart - TimeOld.QuadPart)/TimeFreq.QuadPart)>=1)
        {
            // ��������� ��������� ���������� ����
            Synchronize(EM);
            // �������� "������" �����
            TimeOld = TimeNew;
        }
    }
}

//---------------------------------------------------------------------------
//--��������� ������ �� ����������� �������--//
//---------------------------------------------------------------------------
void TForm1::ExternalManager()
{

  	// ���� ������� ������� �������
	if(pr_otl) return;

    // ������ ISO-P32C32(1)
    if ( externalTask & 0x01 )
    {
        // ��������
        externalError = ISO_P32C32_1( 0 , zin );
        // ������ ������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x01);
                // ����� ������
                externalTask &= (~0x01);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x01;
                // ����� ������
                externalTask &= (~0x01);
            }; break;
        }
    }
     // ������ ISO-P32C32(2)
    else if ( externalTask & 0x02 )
    {
        // ��������
        externalError = ISO_P32C32_2( 0 , zin );
        // ������ ������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x02);
                // ����� ������
                externalTask &= (~0x02);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x02;
                // ����� ������
                externalTask &= (~0x02);
            }; break;
        }
    }
    // ������ ACL-7250
    else if( externalTask & 0x04 )
    {
        // ��������
        externalError = ACL7250(0,zin);
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x10);
                // ����� ������
                externalTask &=(~0x04);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x10;
                // ����� ������
                externalTask &= (~0x04);
            }; break;
        }
    }

    // ������ ���������� ������� �������� � ISO-813
    else if(externalTask & 0x08)
    {
        // ��������
        externalError = ISO_813(aik , AIK_COUNT * 8);
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x04);
                // ����� ������
                externalTask &= (~0x08);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x04;
                // ����� ������
                externalTask &= (~0x08);
            }; break;
        }
    }

    // �������� � ISO-P32�32(1)
    else if(externalTask & 0x10)
    {
         // ��������
         externalError = ISO_P32C32_1( 1 , out );
        // ������ �������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x01);
                // ���� ��� ������ ����� ����� ������
                externalTask &= (~0x10);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x01;
                // ����� ������
                externalTask &= (~0x10);
            }; break;
        }
    }
    // �������� � ISO-P32�32(2)
    else if(externalTask & 0x20)
    {
         // ��������
         externalError = ISO_P32C32_2( 1 , out );
        // ������ �������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x02);
                // ���� ��� ������ ����� ����� ������
                externalTask &= (~0x20);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x02;
                // ����� ������
                externalTask &= (~0x20);
            }; break;
        }
    }
    // �������� � ACL7250
    else if(externalTask & 0x40 )
    {
        // ��������
        externalError = ACL7250( 1 , out );
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x10);
                // ����� ������
                externalTask &= (~0x40);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x10;
                // ����� ������
                externalTask &= (~0x40);
            }; break;
        }
    }
    // ������ � ISO-DA16
    else if( externalTask & 0x80 )
    {
        for( int i = 0 ; i < AOUT_COUNT * 4 ; i++ )
        {
            externalError = ISO_DA16 ( 0 , aout[i] , 0 , i );
            // ������ ������
            switch ( externalError )
            {
                case 0:
                {
                    // ����� �����������
                    diagnS[1] &= (~0x08);
                }; break;
                default:
                {
                    // ��������� �����������
                    diagnS[1] |= 0x08;
                    // ����� ������
                    externalTask &= (~0x80);
                }; break;
            }
        }
        externalTask &= (~0x80);
    }
    else
    {
        externalTask = 0xFF;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ �������� �������--//
void TForm1::VisualFormat()
{    // �����
   unsigned int mask;
    unsigned char i=0,j=0;
    // ������������ ���������� ������ (ISO P32C32)
    for(i=0;i<ZIN_COUNT;i++)
    {
        mask = 0x0001;
        for(j=0;j<16;j++)
        {
             if((zin[i]&mask)!=(zin_Z[i]&mask))
             {
                if(zin[i]&mask)
                        Check_Zin[i][j] -> Picture = check1->Picture;
                else
                        Check_Zin[i][j] -> Picture = check0->Picture;
             }
             mask <<= 1;
        }
        zin_Z[i] = zin[i];
    }

        // ������������ ���������� ������� (ISO P32C32)
    for(i=0;i<OUT_COUNT;i++)
    {
        mask = 0x0001;
        for(j=0;j<16;j++)
        {
             if((out[i]&mask)!=(out_Z[i]&mask))
             {
                if(out[i] & mask)
                Check_Out[i][j] -> Picture = check1->Picture;
                else
                Check_Out[i][j] -> Picture = check0->Picture;
             }
             mask <<= 1;
        }
        out_Z[i] = out[i];
    }

    // ������������ ���������� ������
    for(i=0;i<AIK_COUNT;i++)
    { for(j=0;j<8;j++)
      {
          Dec_Ain[i][j] -> Text = IntToStr(aik[i*8+j]);
          CG_Ain[i][j] -> Progress = aik[i*8+j];
          UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/4095.0 * 10.0,  ffFixed, 5, 3);
      }
    }
    // ������������ ���������� �������
    for(i=0;i<AOUT_COUNT;i++)
    { for(j=0;j<4;j++)
      {   if(aoutKon[i*4+j] < 8192)  { aoutKon[i*4+j] = 8192;  }
          if(aoutKon[i*4+j] > 16384) { aoutKon[i*4+j] = 16384; }
          if(aout[i*4+j] < 8192)     { aout[i*4+j] = 8192;     }
          if(aout[i*4+j] > 16383)    { aout[i*4+j] = 16383;    }

          Dec_Aout[i][j] -> Text = IntToStr( aoutKon[i*4+j] );
          UV_Aout[i][j] -> Text = FloatToStrF( float(aout[i*4+j]-8192) / 8191.0 * 10.0, ffFixed, 5, 3);
          CG_Aout[i][j] -> Progress = aout[i*4+j];
      }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

    // ������������� ������� ����������� ������� ��������
    serTemp[0] = Series1;
    serTemp[1] = Series2;
    serTemp[2] = Series3;
    serTemp[3] = Series4;
    serTemp[4] = Series5;
    serTemp[5] = Series6;
    serTemp[6] = Series7;
    serTemp[7] = Series8;
    serTemp[8] = Series9;
    serTemp[9] = Series10;
    serTemp[10] = Series20;

    // ������������� ������� ����������� �������� ��������
    serArh[0] = Series11;
    serArh[1] = Series12;
    serArh[2] = Series13;
    serArh[3] = Series14;
    serArh[4] = Series15;
    serArh[5] = Series16;
    serArh[6] = Series17;
    serArh[7] = Series18;
    serArh[8] = Series19;
    serArh[9] = Series21;
    serArh[10] = Series22;

	// ���������������� ������� ���
    InitObjectsRRG();   //���������� RRG.cpp
    InitObjectsKl();
    // ��������� ������ ��� ����������� ��������
    TSWork -> DoubleBuffered = true;
    EditSHRName -> DoubleBuffered = true;
    Pnl_Work -> DoubleBuffered = true;
    LBError -> DoubleBuffered = true;
	PanDIAGvak -> DoubleBuffered = true;

    // ����� �������� ������
    int
        fileCount,
        rezult;
    TSearchRec SR;

    // ����������
    fileCount = 0;
    ListBoxLibrary -> Clear();
    rezult = FindFirst("LIB\\*.txt", faAnyFile, SR);

    while ( !rezult )
    {
        fileCount++;
        SR.Name.SetLength( SR.Name.Length() - 4 );
        ListBoxLibrary -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };
    ListBoxLibrary -> Sorted;

    // ���������� � �����������
    fileCount = 0;
    ListBoxStatArh -> Clear();
    rezult = FindFirst("Stat\\*.txt", faAnyFile, SR);
    while ( !rezult ){
        fileCount++;
        SR.Name.SetLength(10);
        ListBoxStatArh -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };
    EdtArhStat -> Text = fileCount;
    ListBoxStatArh -> Sorted;

    // �������
    fileCount = 0;
    ListBoxGraphArh -> Clear();
    rezult = FindFirst("Graph\\*.txt", faAnyFile, SR);
        while ( !rezult ){
        fileCount++;
        SR.Name.SetLength(10);
        ListBoxGraphArh -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };
    EdtArhGraph -> Text = fileCount;
    ListBoxGraphArh -> Sorted;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Btn_ExitClick(TObject *Sender)
{
    if(MessageDlg("����� �� ���������?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
    Close();
}
//---------------------------------------------------------------------------
//--������������ ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualMnemo()
{
    if(!ust_ready) return;

    VisualColorElement();                   //  ������������ ��������� ����������
    VisualVoda();                           //  ����������� ������� ����
    VisualParam();                          //  ������������ ���������� ����������
    VisualDiagn();                          //  ����������� ���������� �� ����������
    VisualButtons();                        //  ����������� ������
    VisualZagol();                          //  ������������ ����� ���������
    VisualOperatorDlg();                    //  ������������ ������� ���������

    if(PR_KLASTER) Visual_IM();    // ����������� � �������
}

//----��������� / ����������
void __fastcall TForm1::PCMainChange(TObject *Sender)
{

  if(PCMain -> ActivePage == TSNalad)
  {
        Pnl_Work -> Visible = false;
        Pnl_Work -> Parent = TabSheet1;
        Pnl_Work -> Top = -4;
        Pnl_Work -> Left = +4;


        Panel90->BringToFront();  // ������������ ���� / ������
        Panel75->BringToFront();
        Panel317->BringToFront(); // ���������
        PnlMnemoParam -> Height = 423; //��������� ������ �������
        Pnl_GK->BringToFront();

        //�������� ����� ��������
        Panel67 -> Visible = false;
        Panel69 -> Visible = false;
        Panel72 -> Visible = false;
        Panel73 -> Visible = false;

        //�������� �������
        Panel4 -> Visible = false;
        Panel33 -> Visible = false;

        VisualMnemo();
        Pnl_Work -> Visible = true;
        Plast->Visible=false;
        State->Visible=false;

  }
  else if(PCMain -> ActivePage == TSWork)
  {     Pnl_Work -> Visible = false;
        Pnl_Work -> Parent = TSWork;

       
        Pnl_Work -> Top = 32;
        Pnl_Work -> Left = 8;

        PnlMnemoParam -> Height = 474; //����������� ������ �������

        //��������� ����� ��������
        Panel67 -> Visible = true;
        Panel69 -> Visible = true;
        Panel72 -> Visible = true;
        Panel73 -> Visible = true;

        //���������� �������
        Panel4 -> Visible = true;
        Panel33 -> Visible = true;

        VisualMnemo();
        Pnl_Work -> Visible = true;
        State->Visible=true;
        if(!PR_KLASTER)
        Plast->Visible=true;
  }
}
//---------------------------------------
/////// ��������� ��������� �������
//---------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
//��������� PR_KLASTER
Klaster();

//���������� ������ ��������� �� ������ �������
APanel -> Left = 600;
APanel -> Top = 340;
PnlDiagm ->Left=655;
PnlDiagm ->Top=70;
PnlCondition->Left=650;
PnlCondition->Top=180;

//��������� �������� �����
LoadGasData();
RenameGases();

Params->Cells[0][0]="���������";
Params->Cells[0][1]="������ ���1";
Params->Cells[0][2]="������ ���2";
Params->Cells[0][3]="������ ���3";
Params->Cells[0][4]="������ ���4";
Params->Cells[0][5]="������ ���5";
Params->Cells[0][6]="������ ���6";
Params->Cells[0][7]="��������";
Params->Cells[0][8]="�������� ��� ��";
Params->Cells[0][9]="���. ����. �����";
Params->Cells[0][10]="���. ����. �����";
Params->Cells[0][11]="�������� ��� �/�";
Params->Cells[0][12]="��������";
Params->Cells[0][13]="����� ��������";
Params->Cells[0][14]="� ���������?";

Units->Cells[0][0]="��.���.";
Units->Cells[0][1]="�/�";
Units->Cells[0][2]="�/�";
Units->Cells[0][3]="�/�";
Units->Cells[0][4]="�/�";
Units->Cells[0][5]="�/�";
Units->Cells[0][6]="�/�";
Units->Cells[0][7]="��";
Units->Cells[0][8]="��";
Units->Cells[0][9]="�";
Units->Cells[0][10]="�";
Units->Cells[0][11]="��";
Units->Cells[0][12]="�";
Units->Cells[0][13]="���";
Units->Cells[0][14]="�";

unsigned char i=0,j=0;

TGroupBox *ZinParents[ZIN_COUNT*2] =
   { Form1->GB_zin0_1,
     Form1->GB_zin0_2,
     Form1->GB_zin1_1,
     Form1->GB_zin1_2,
     Form1->GB_zin2_1,
     Form1->GB_zin2_2,
     Form1->GB_zin3_1,
     Form1->GB_zin3_2,
     Form1->GB_zin4_1,
     Form1->GB_zin4_2
   };

 /////////////////////////////////////////////////////////////////////////////
   for(i=0;i<ZIN_COUNT;i++)
   { // ���� �� ���-�� ��� � ����������
     for(j=0;j<16;j++)
     {  //  �������� ���������� �������� ZIN
        Title_Zin[i][j] = new TLabel(this);
        // ������������, �������, ��������
        if(j<=7)
        { Title_Zin[i][j] -> Parent = ZinParents[2*i];
          Title_Zin[i][j] -> Top = 37 + 36 * j;
        }
        else
        { Title_Zin[i][j] -> Parent = ZinParents[2*i+1];
          Title_Zin[i][j] -> Top = 37 + 36 * (j-8);
        }
        Title_Zin[i][j] -> Left = 18;
        Title_Zin[i][j] -> Font -> Name = "Arial";
        Title_Zin[i][j] -> Font -> Size = 14;
        Title_Zin[i][j] -> Font -> Color = clBlack;
        Title_Zin[i][j] -> Caption = zinNames[i*16+j];
        Title_Zin[i][j] -> Transparent = true;
        Title_Zin[i][j] -> Height = 23;
        Title_Zin[i][j] -> Width = 440;
        Title_Zin[i][j] -> Layout = tlTop;

        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Check_Zin[i][j] = new TImage(this);
        // ������������, �������
        if(j<=7)
        { Check_Zin[i][j] -> Parent = ZinParents[2*i];
          Check_Zin[i][j] -> Top = 36 + 36 * j;
        }
        else
        { Check_Zin[i][j] -> Parent = ZinParents[2*i+1];
          Check_Zin[i][j] -> Top = 36 + 36 * (j-8);
        }
        Check_Zin[i][j] -> Left = 462;
        Check_Zin[i][j] -> Height = 25;
        Check_Zin[i][j] -> Width = 25;
        Check_Zin[i][j] -> Picture=check0->Picture;
        Check_Zin[i][j] -> Transparent = true;
        Check_Zin[i][j] -> Hint = IntToStr(int(pow(2,j)));
        Check_Zin[i][j] -> OnClick = SetZinClick;
        Check_Zin[i][j] -> OnDblClick = SetZinClick;
     }
   }
   /////////////////////////////////////////////////////////////////////////////
   TGroupBox *OutParents[OUT_COUNT*2] =
   { Form1->GB_out0_1,
     Form1->GB_out0_2,
     Form1->GB_out1_1,
     Form1->GB_out1_2,
     Form1->GB_out2_1,
     Form1->GB_out2_2,
     Form1->GB_out3_1,
     Form1->GB_out3_2
   };
   /////////////////////////////////////////////////////////////////////////////
   // ���� �� ���������� ����������� ��� �����������
   for(i=0;i<OUT_COUNT;i++)
   { // ���� �� ���������� ��� � ����������
     for(j=0;j<16;j++)
     {  // �������� ���������� ��������
        Title_Out[i][j] = new TLabel(this);
        // ������������, �������, ��������
        if(j<=7)
        { Title_Out[i][j] -> Parent = OutParents[2*i];
          Title_Out[i][j] -> Top = 37 + 36 * j;
        }
        else
        { Title_Out[i][j] -> Parent = OutParents[2*i+1];
          Title_Out[i][j] -> Top = 37 + 36 * (j-8);
        }
        Title_Out[i][j] -> Left = 18;
        Title_Out[i][j] -> Font -> Name = "Arial";
        Title_Out[i][j] -> Font -> Size = 14;
        Title_Out[i][j] -> Font -> Color = clBlack;
        Title_Out[i][j] -> Caption = outNames[i*16+j];
        Title_Out[i][j] -> Transparent = true;
        Title_Out[i][j] -> Height = 23;
        Title_Out[i][j] -> Width = 440;
        Title_Out[i][j] -> Layout = tlTop;
		
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Check_Out[i][j] = new TImage(this);
        // ������������, �������
        if(j<=7)
        { Check_Out[i][j] -> Parent = OutParents[2*i];
          Check_Out[i][j] -> Top = 36 + 36 * j;
        }
        else
        {
        Check_Out[i][j] -> Parent = OutParents[2*i+1];
        Check_Out[i][j] -> Top = 36 + 36 * (j-8);
        }
        Check_Out[i][j] -> Left = 462;
        Check_Out[i][j] -> Height = 25;
        Check_Out[i][j] -> Width = 25;
        Check_Out[i][j] -> Picture=check0->Picture;

        Check_Out[i][j] -> Hint = IntToStr(int(pow(2,j)));
        Check_Out[i][j] -> OnClick = SetOutClick;
     }
   }

 /////////////////
  TGroupBox *AinParents[AIK_COUNT] =

   { Form1->GB_ain0,
     Form1->GB_ain1,
     Form1->GB_ain2
   };
   /////////////////////////////////////////////////////////////////////////////
   // ���� �� ���������� ����������� ��� �����������
   for(i=0;i<AIK_COUNT;i++)
   { // ���� �� ���������� ��� � ����������
     for(j=0;j<8;j++)
     {  // �������� ���������� ��������
        Title_Ain[i][j] = new TLabel(this);
        // ������������, �������, ��������
        Title_Ain[i][j] -> Parent = AinParents[i];
        Title_Ain[i][j] -> Top = 37 + 36 * j;
        Title_Ain[i][j] -> Left = 18;
        Title_Ain[i][j] -> Font -> Name = "Arial";
        Title_Ain[i][j] -> Font -> Size = 14;
        Title_Ain[i][j] -> Font -> Color = clBlack;
        Title_Ain[i][j] -> Caption = aikNames[i*8+j];
        Title_Ain[i][j] -> Transparent = true;
        Title_Ain[i][j] -> Height = 23;
        Title_Ain[i][j] -> Width = 430;
        Title_Ain[i][j] -> Layout = tlTop;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Dec_Ain[i][j] = new TEdit(this);
        // ������������, �������
        Dec_Ain[i][j] -> Parent = AinParents[i];
        Dec_Ain[i][j] -> BevelInner = bvSpace;
        Dec_Ain[i][j] -> BevelKind = bkFlat;
        Dec_Ain[i][j] -> BorderStyle = bsNone;
        Dec_Ain[i][j] -> Left = 469;
        Dec_Ain[i][j] -> Top = 35 + 36 * j;
        Dec_Ain[i][j] -> Font -> Name = "Arial";
        Dec_Ain[i][j] -> Font -> Size = 16;
        Dec_Ain[i][j] -> Font -> Color = clBlack;
        Dec_Ain[i][j] -> Width = 60;
        Dec_Ain[i][j] -> Height = 27;
        Dec_Ain[i][j] -> Color = clWhite;
        Dec_Ain[i][j] -> AutoSize = false;
        Dec_Ain[i][j] -> Text = "0";
        Dec_Ain[i][j] -> ReadOnly = true;
        Dec_Ain[i][j] -> MaxLength = 5;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        UV_Ain[i][j] = new TEdit(this);
        // ������������, �������
        UV_Ain[i][j] -> Parent = AinParents[i];
        UV_Ain[i][j] -> BevelInner = bvSpace;
        UV_Ain[i][j] -> BevelKind = bkFlat;
        UV_Ain[i][j] -> BorderStyle = bsNone;
        UV_Ain[i][j] -> Left = 531;
        UV_Ain[i][j] -> Top = 35 + 36 * j;
        UV_Ain[i][j] -> Font -> Name = "Arial";
        UV_Ain[i][j] -> Font -> Size = 16;
        UV_Ain[i][j] -> Font -> Color = clBlack;
        UV_Ain[i][j] -> Width = 72;
        UV_Ain[i][j] -> Height = 27;
        UV_Ain[i][j] -> Color = 0x00EAD999;
        UV_Ain[i][j] -> AutoSize = false;
        UV_Ain[i][j] -> Text = "0";
        UV_Ain[i][j] -> ReadOnly = true;
        UV_Ain[i][j] -> MaxLength = 5;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        CG_Ain[i][j] = new TCGauge(this);
        // ������������, �������
        CG_Ain[i][j] -> Parent = AinParents[i];
        CG_Ain[i][j] -> Left = 605;
        CG_Ain[i][j] -> Font -> Name = "Arial"; //"Arial Narrow"; //"Arial";
        CG_Ain[i][j] -> Font -> Size = 12;
        CG_Ain[i][j] -> Font -> Height = -16;
        CG_Ain[i][j] -> Top = 35 + 36 * j;
        CG_Ain[i][j] -> ForeColor = 0x0046F064;
        CG_Ain[i][j] -> BackColor = clGray;
        CG_Ain[i][j] -> Width = 202;
        CG_Ain[i][j] -> Height = 27;
        CG_Ain[i][j] -> BorderStyle = bsSingle;
        CG_Ain[i][j] -> ShowText = false;
        CG_Ain[i][j] -> MaxValue = 0x0FFF;
     }
      }
 /////////////////////////////////////////////////////////////////////////////
   TGroupBox *AoutParents[AOUT_COUNT] =
   { Form1->GB_aout0,
     Form1->GB_aout1,
     Form1->GB_aout2
   };
   /////////////////////////////////////////////////////////////////////////////
   // ���� �� ���������� ����������� ��� �����������
   for(i=0;i<AOUT_COUNT;i++)
   { // ���� �� ���������� ��� � ����������
     for(j=0;j<4;j++)
     {  // �������� ���������� ��������
        Title_Aout[i][j] = new TLabel(this);
        // ������������, �������, ��������
        Title_Aout[i][j] -> Parent = AoutParents[i];
        Title_Aout[i][j] -> Top = 37 + 72 * j;
        Title_Aout[i][j] -> Left = 18;
        Title_Aout[i][j] -> Font -> Name = "Arial";
        Title_Aout[i][j] -> Font -> Size = 14;
        Title_Aout[i][j] -> Font -> Color = clBlack;
        Title_Aout[i][j] -> Caption = aoutNames[i*4+j];
        Title_Aout[i][j] -> Transparent = true;
        Title_Aout[i][j] -> Height = 23;
        Title_Aout[i][j] -> Width = 430;
        Title_Aout[i][j] -> Layout = tlTop;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Dec_Aout[i][j] = new TEdit(this);
        // ������������, �������
        Dec_Aout[i][j] -> Parent = AoutParents[i];
        Dec_Aout[i][j] -> BevelInner = bvSpace;
        Dec_Aout[i][j] -> BevelKind = bkFlat;
        Dec_Aout[i][j] -> BorderStyle = bsNone;
        Dec_Aout[i][j] -> Left = 464;
        Dec_Aout[i][j] -> Top = 35 + 72 * j;
        Dec_Aout[i][j] -> Font -> Name = "Arial";
        Dec_Aout[i][j] -> Font -> Size = 16;
        Dec_Aout[i][j] -> Font -> Color = clBlack;
        Dec_Aout[i][j] -> Width = 65;
        Dec_Aout[i][j] -> Height = 27;
        Dec_Aout[i][j] -> Color = clWhite;
        Dec_Aout[i][j] -> AutoSize = false;
        Dec_Aout[i][j] -> Text = "0";
        Dec_Aout[i][j] -> ReadOnly = true;
        Dec_Aout[i][j] -> MaxLength = 5;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        UV_Aout[i][j] = new TEdit(this);
        // ������������, �������
        UV_Aout[i][j] -> Parent = AoutParents[i];
        UV_Aout[i][j] -> BevelInner = bvSpace;
        UV_Aout[i][j] -> BevelKind = bkFlat;
        UV_Aout[i][j] -> BorderStyle = bsNone;
        UV_Aout[i][j] -> Left = 531;
        UV_Aout[i][j] -> Top = 35 + 72 * j;
        UV_Aout[i][j] -> Font -> Name = "Arial";
        UV_Aout[i][j] -> Font -> Size = 16;
        UV_Aout[i][j] -> Font -> Color = clBlack;
        UV_Aout[i][j] -> Width = 72;
        UV_Aout[i][j] -> Height = 27;
        UV_Aout[i][j] -> Color = clWhite;
        UV_Aout[i][j] -> AutoSize = false;
        UV_Aout[i][j] -> Text = "0";
        UV_Aout[i][j] -> ReadOnly = true;
        UV_Aout[i][j] -> MaxLength = 5;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Dec_Aout_zad[i][j] = new TEdit(this);
        // ������������, �������
        Dec_Aout_zad[i][j] -> Parent = AoutParents[i];
        Dec_Aout_zad[i][j] -> BevelInner = bvSpace;
        Dec_Aout_zad[i][j] -> BevelKind = bkFlat;
        Dec_Aout_zad[i][j] -> BorderStyle = bsNone;
        Dec_Aout_zad[i][j] -> Left = 464;
        Dec_Aout_zad[i][j] -> Top = 68 + 72 * j;
        Dec_Aout_zad[i][j] -> Font -> Name = "Arial";
        Dec_Aout_zad[i][j] -> Font -> Size = 16;
        Dec_Aout_zad[i][j] -> Font -> Color = clBlack;
        Dec_Aout_zad[i][j] -> Width = 65;
        Dec_Aout_zad[i][j] -> Height = 27;
        Dec_Aout_zad[i][j] -> Color = 0x00EAD999;
        Dec_Aout_zad[i][j] -> AutoSize = false;
        Dec_Aout_zad[i][j] -> Text = "0";
        Dec_Aout_zad[i][j] -> ReadOnly = true;
        Dec_Aout_zad[i][j] -> MaxLength = 4;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        UV_Aout_zad[i][j] = new TEdit(this);
        // ������������, �������
        UV_Aout_zad[i][j] -> Parent = AoutParents[i];
        UV_Aout_zad[i][j] -> BevelInner = bvSpace;
        UV_Aout_zad[i][j] -> BevelKind = bkFlat;
        UV_Aout_zad[i][j] -> BorderStyle = bsNone;
        UV_Aout_zad[i][j] -> Left = 531;
        UV_Aout_zad[i][j] -> Top = 68 + 72 * j;
        UV_Aout_zad[i][j] -> Font -> Name = "Arial";
        UV_Aout_zad[i][j] -> Font -> Size = 16;
        UV_Aout_zad[i][j] -> Font -> Color = clBlack;
        UV_Aout_zad[i][j] -> Width = 72;
        UV_Aout_zad[i][j] -> Height = 27;
        UV_Aout_zad[i][j] -> Color = 0x00EAD999;
        UV_Aout_zad[i][j] -> AutoSize = false;
        UV_Aout_zad[i][j] -> Text = "0";
        UV_Aout_zad[i][j] -> ReadOnly = true;
        UV_Aout_zad[i][j] -> MaxLength = 4;
        /////////////////////////////////////////////////////////////////////////
        // �������� ����������
        CG_Aout[i][j] = new TCGauge(this);
        // ������������, �������
        CG_Aout[i][j] -> Parent = AoutParents[i];
        CG_Aout[i][j] -> Left = 605;
        CG_Aout[i][j] -> Top = 35 + 72 * j;
        CG_Aout[i][j] -> ForeColor = 0x0046F064;
        CG_Aout[i][j] -> BackColor = clGray;
        CG_Aout[i][j] -> Width = 202;
        CG_Aout[i][j] -> Height = 27;
        CG_Aout[i][j] -> BorderStyle = bsSingle;
        CG_Aout[i][j] -> ShowText = false;
        CG_Aout[i][j] -> MaxValue = 16383;
        CG_Aout[i][j] -> MinValue = 8192;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        CG_Aout_zad[i][j] = new TCGauge(this);
        // ������������, �������
        CG_Aout_zad[i][j] -> Parent = AoutParents[i];
        CG_Aout_zad[i][j] -> BorderStyle = bsSingle;
        CG_Aout_zad[i][j] -> Left = 605;
        CG_Aout_zad[i][j] -> Top = 68 + 72 * j;
        CG_Aout_zad[i][j] -> ForeColor = 0x00EAD999;
        CG_Aout_zad[i][j] -> BackColor = clGray;
        CG_Aout_zad[i][j] -> Width = 202;
        CG_Aout_zad[i][j] -> Height = 27;
        CG_Aout_zad[i][j] -> ShowText = false;
        CG_Aout_zad[i][j] -> MaxValue = 16383;
        CG_Aout_zad[i][j] -> MinValue = 8192;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Zero_Aout[i][j] = new TButton(this);
        // ������������, �������
        Zero_Aout[i][j] -> Parent = AoutParents[i];
        Zero_Aout[i][j] -> Left = 16;
        Zero_Aout[i][j] -> Top = 70 + 72 * j;
        Zero_Aout[i][j] -> Width = 25;
        Zero_Aout[i][j] -> Height = 25;
        Zero_Aout[i][j] -> Caption = "0";
        Zero_Aout[i][j] -> Hint = IntToStr(i*4 + j);
        Zero_Aout[i][j] -> OnClick = ZeroAoutClick;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Zad_Aout[i][j] = new TScrollBar(this);
        // ������������, �������
        Zad_Aout[i][j] -> Parent = AoutParents[i];
        Zad_Aout[i][j] -> Left = 42;
        Zad_Aout[i][j] -> Ctl3D = true;
        Zad_Aout[i][j] -> Top = 70 + 72 * j;
        Zad_Aout[i][j] -> Width = 390;
        Zad_Aout[i][j] -> Height = 25;
        Zad_Aout[i][j] -> Max = 16383;
        Zad_Aout[i][j] -> Min = 8192;
        Zad_Aout[i][j] -> Hint = IntToStr(i*4 + j);
        Zad_Aout[i][j] -> ShowHint = false;
        Zad_Aout[i][j] -> OnChange = SetAoutChange;
        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Ent_Aout[i][j] = new TButton(this);
        // ������������, �������
        Ent_Aout[i][j] -> Parent = AoutParents[i];
        Ent_Aout[i][j] -> Left = 433;
        Ent_Aout[i][j] -> Top = 70 + 72 * j;
        Ent_Aout[i][j] -> Width = 25;
        Ent_Aout[i][j] -> Height = 25;
        Ent_Aout[i][j] -> Caption = "�";
        Ent_Aout[i][j] -> Hint = IntToStr(i*4 + j);
        Ent_Aout[i][j] -> OnClick = EntAoutClick;
  }
 }

    // ������ ����������� ������ � ������
    // ������� �����
    LogicThread = new TLogicThread(true);
    // ����� ������������ ����� ����������
    LogicThread -> FreeOnTerminate = true;
    // ��������� ������ ������
    LogicThread -> Priority = tpLower;
    // ��������� �����
    LogicThread -> Resume();

    // ������ ������������� ������� � ������
    // ������� �����
    TimerExist = new TTimerExist(true);
    // ����� ������������ ����� ����������
    TimerExist -> FreeOnTerminate = true;
    // ��������� ������ ������
    TimerExist -> Priority = tpLower;
    // ��������� �����
    TimerExist -> Resume();
	
	// ��������� ��������� ����
    OpenISO_P32C32(); // ������� ��������� � ��������� ISO-P32C32
    OpenACL_7250();   // ������� ��������� � ��������� ACL-7250
    OpenISO_813 ();    // ������� ��������� � ��������� ISO-813
    OpenISO_DA16();    // ������� ��������� � ��������� ISO-DA16

    PCMain  -> DoubleBuffered = true;
    Form1-> DoubleBuffered = true;
    KASSETA -> DoubleBuffered = true;
    Pnl_Title -> DoubleBuffered = true;
    // ������������ ����-�������
    Label_Time -> Caption = TimeToStr(Time());
    Label_Date -> Caption = DateToStr(Date());
    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> LoadFromFile("Nasmod\\Nasmod.txt");
    EditNastrTo0  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](0));
    EditNastrTo1  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](1));
    EditNastrTo2  -> Text = MemoNasmod -> Lines -> operator [](2);
    EditNastrTo13  -> Text = MemoNasmod -> Lines -> operator [](3);
    EditNastrTo3  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](4));
    EditNastrTo4  -> Text = MemoNasmod -> Lines -> operator [](5);
    EditNastrTo5  -> Text = MemoNasmod -> Lines -> operator [](6);
    EditNastrTo6  -> Text = MemoNasmod -> Lines -> operator [](7);
    EditNastrTo14  -> Text = MemoNasmod -> Lines -> operator [](8);
    EditNastrTo7  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](9));
    EditNastrTo8  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](10));
    EditNastrTo11  -> Text = MemoNasmod -> Lines -> operator [](11);
    EditNastrTo12  -> Text = MemoNasmod -> Lines -> operator [](12);
    EditNastrTo17  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](13));
    MemoNasmod -> Lines -> Clear();

    // ��������� �������� T
    MemoT -> Lines -> LoadFromFile("Nasmod\\Mex.txt");
    EdtTRed1  -> Text = MemoT -> Lines -> operator [](0);
    EdtTRed2  -> Text = MemoT -> Lines -> operator [](1);
    EdtTRed3  -> Text = MemoT -> Lines -> operator [](2);
    EdtTRed4  -> Text = MemoT -> Lines -> operator [](3);
    EdtTRed5  -> Text = MemoT -> Lines -> operator [](4);
    MemoT -> Lines -> Clear();

    BtnNastrDaClick(BtnNastrDa);
    BtnAutoDaClick(BtnAutoDa);
    BtnTDaClick(BtnTDa);
    BtnRDaClick(BtnRDa);

    Init_SComport();
	
	Comport[0]->Reser_Port(Comport[0]->BTN_reset);  // ��������� �����
    Comport[1]->Reser_Port(Comport[1]->BTN_reset);  // ��������� �����
    Comport[2]->Reser_Port(Comport[2]->BTN_reset);  // ��������� �����
    Comport[3]->Reser_Port(Comport[3]->BTN_reset);  // ��������� �����

    Init_DZaslVAT();
    Init_DatMERA();
    Init_TRMD();
    Init_DatMTM9D();
    if(!PR_KLASTER) Init_SAZ_drive();
    else Init_SIntMod();
	
    if(!PR_KLASTER) AZdrive_Load();	// �������� ���������� ���������

    // ����������� �������
    LoadData2();

    pas_str = "";
    for(int i=0;iniPAS.pass[i]!=0;i++)
    pas_str = pas_str + iniPAS.pass[i];
    Edit_Acc_UserPas -> Text = pas_str;

    // �������� �������
    PCNalad->Pages[0]->TabVisible = iniPAS.state[0];
    CB_Acc_V1->Checked = !iniPAS.state[0];
    PCNalad->Pages[1]->TabVisible = iniPAS.state[1];
    CB_Acc_V2->Checked = !iniPAS.state[1];
    PCNalad->Pages[2]->TabVisible = iniPAS.state[2];
    CB_Acc_V3->Checked = !iniPAS.state[2];
    PCNalad->Pages[3]->TabVisible = iniPAS.state[3];
    CB_Acc_V4->Checked = !iniPAS.state[3];
    PCNalad->Pages[4]->TabVisible = iniPAS.state[4];
    CB_Acc_V5->Checked = !iniPAS.state[4];
    PCNalad->Pages[5]->TabVisible = iniPAS.state[5];
    CB_Acc_V6->Checked = !iniPAS.state[5];
    PCNalad->Pages[6]->TabVisible = iniPAS.state[6];
    CB_Acc_V7->Checked = !iniPAS.state[6];
    PCNalad->Pages[7]->TabVisible = iniPAS.state[7];
    CB_Acc_V8->Checked = !iniPAS.state[7];

    // ���� ���������� �������������, ������������� �� ������
    if(!PCNalad->Pages[0]->TabVisible)
		PCNalad->ActivePage = TSNaladAcc;

    SetOut(1,1,0x02);				// ��������� ���� ����������

    ust_ready = 1;	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ��������� ����������--//
TColor TForm1::SetPopeColor(bool value)
{
    if(value) return clWhite;
    else      return 0x0064F046;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--����������� ������� �������� ����������--//
void TForm1::VisualParam()
{
    // ����� ������
    if(shr[3]||shr[4])
    EditNST -> Caption = N_ST;
    else
    EditNST -> Caption = "";

    //��
    dz_tek  -> Caption = FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,0) + "%";
    //����������� ������
    temp_ko -> Caption = FloatToStrF((float(TEK_TEMP1)/10.0),ffFixed,4,1);

    //����������� �����
    //temp_tr -> Caption = FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1);

    //����������� ������������ ��.������
	if(nasmod[5]&&nasmod[6])
    {
		coef_ip_zad->Caption = FloatToStrF(1000.0/(float)nasmod[5],ffFixed,5,0); // �������� �����. ���� ��
		coef_pd_zad->Caption = FloatToStrF(1000.0/(float)nasmod[6],ffFixed,5,0); // �������� �����. ���� �/�
    }
    //����������� ������������ �������
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(shr[29]) {
    if(N_TEK_GIR!=0) { coef_ip_tek->Caption = FloatToStrF(1000.0/(float)N_TEK_GIR,ffFixed,5,0); } // ���. �����. ���� ��
    else             { coef_ip_tek->Caption = 0;                                                }    }
    else coef_ip_tek->Caption = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if((shr[27])||(shr[28])) {
    if(N_TEK_GIS!=0) { coef_pd_tek->Caption = FloatToStrF(1000.0/(float)N_TEK_GIS,ffFixed,5,0); } // ���. �����. ���� �/�
    else             { coef_pd_tek->Caption = 0;                                                }  }
    else coef_pd_tek->Caption = 0;

    //�������
    if((D_D1>=0)&&(D_D1<=10000))  { d_d1->Caption=FloatToStrF(133.3*pow(10,(float)D_D1/1000.0-6.0),ffExponent,3,8); }
    if((D_D2>=0)&&(D_D2<=10000))  { d_d2->Caption=FloatToStrF(133.3*pow(10,(float)D_D2/1000.0-6.0),ffExponent,3,8); }

	if(D_D3 < 0) d_d3->Caption = "0";
    else if(D_D3 > 10000) d_d3->Caption = FloatToStrF(DAVL_MAX,ffFixed,5,1);
    else d_d3->Caption=FloatToStrF((float)D_D3/10000*DAVL_MAX,ffFixed,5,1);
	
    if((D_D4>=0)&&(D_D4<=10000))  { d_d4->Caption=FloatToStrF(100.0*pow(10.0,(float(D_D4)/1000.0-6.8)/0.6),ffExponent,3,8); }
    if((D_D5>=0)&&(D_D5<=10000))  { d_d5->Caption=FloatToStrF(133.3*pow(10,(float)D_D5/1000.0-6.0),ffExponent,3,8); }

    if(PCMain -> ActivePage == TSNalad) // �������
    {

    EdtZadA00 -> Text = EdtRKon0_0 -> Text;  // ������ ���1
    EdtZadA01 -> Text = EdtRKon0_1 -> Text;  // ������ ���2
    EdtZadA02 -> Text = EdtRKon0_2 -> Text;  // ������ ���3
    EdtZadA03 -> Text = EdtRKon0_3 -> Text;  // ������ ���4
    EdtZadA04 -> Text = EdtRKon0_4 -> Text;  // ������ ���5
    EdtZadA05 -> Text = EdtRKon0_5 -> Text;  // ������ ���6
    if(shr[4])
        EdtZadA06 -> Text = FloatToStrF((float)nasmod[2]/4095.0*RRG7_MAX,ffFixed,5,1);  // ������ ���7
    else
        EdtZadA06 -> Text = FloatToStrF((float)par[0][6]/4095.0*RRG7_MAX,ffFixed,5,1);  
    EdtZadA07 -> Text = EdtRKon0_8 -> Text;  // ��� ���. ��� ��
    EdtZadA11 -> Text = EdtRKon0_11 -> Text; // ��� ���. ��� �/�
    EdtZadA13 -> Text = EdtRKon0_7 -> Text;  // ��������
    EdtZadA16 -> Text = EdtRKon0_12 -> Text;  // ��������

    EdtTekA00 -> Text = FloatToStrF((float)aik[5]*RRG1_MAX/4095.0,  ffFixed, 6, 1); // ������ ���1
    EdtTekA01 -> Text = FloatToStrF((float)aik[6]*RRG2_MAX/4095.0,  ffFixed, 6, 1); // ������ ���2
    EdtTekA02 -> Text = FloatToStrF((float)aik[7]*RRG3_MAX/4095.0,  ffFixed, 6, 1); // ������ ���3
    EdtTekA03 -> Text = FloatToStrF((float)aik[8]*RRG4_MAX/4095.0,  ffFixed, 6, 1); // ������ ���4
    EdtTekA04 -> Text = FloatToStrF((float)aik[9]*RRG5_MAX/4095.0,  ffFixed, 6, 1); // ������ ���5
    EdtTekA05 -> Text = FloatToStrF((float)aik[10]*RRG6_MAX/4095.0, ffFixed, 6, 1); // ������ ���6
    EdtTekA06 -> Text = FloatToStrF((float)aik[11]*RRG7_MAX/4095.0, ffFixed, 6, 1); // ������ ���7
    EdtTekA07 -> Text = FloatToStrF((float)aik[12]*CESAR_MAX_IP/4095.0, ffFixed, 6, 0); // �������� �������� ��� ��
    EdtTekA08 -> Text = FloatToStrF((float)aik[13]*CESAR_MAX_IP/4095.0, ffFixed, 6, 0); // ���������� �������� ��� ��
    EdtTekA09 -> Text = FloatToStrF((float)aik[17]*US_MAX/4095.0, ffFixed, 6, 2); // ��������� ������������ �� �����
    EdtTekA10 -> Text = FloatToStrF((float)aik[16]*US_MAX/4095.0, ffFixed, 6, 2); // ��������� ������������ �� �����
    EdtTekA11 -> Text = FloatToStrF((float)aik[14]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // �������� �������� ��� ��
    EdtTekA12 -> Text = FloatToStrF((float)aik[15]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // ���������� �������� ��� ��
    EdtTekA13-> Text = FloatToStrF((float)D_D3/10000*DAVL_MAX,ffFixed,5,1);  // ��������

    EdtTekA16-> Text = FloatToStrF((float)aik[18]*SMESH_MAX_USER/4095.0, ffFixed, 6, 1);  // ��������

    }
    else // ����������
    {

    if(N_ST>0)
    {
      EdtZadA00 -> Text = FloatToStrF((float)par[N_ST][0]/4095.0*RRG1_MAX,        ffFixed,5,1);   //������ ���1
      EdtZadA01 -> Text = FloatToStrF((float)par[N_ST][1]/4095.0*RRG2_MAX,        ffFixed,5,1);   //������ ���2
      EdtZadA02 -> Text = FloatToStrF((float)par[N_ST][2]/4095.0*RRG3_MAX,        ffFixed,5,1);   //������ ���3
      EdtZadA03 -> Text = FloatToStrF((float)par[N_ST][3]/4095.0*RRG4_MAX,        ffFixed,5,1);   //������ ���4
      EdtZadA04 -> Text = FloatToStrF((float)par[N_ST][4]/4095.0*RRG5_MAX,        ffFixed,5,1);   //������ ���5
      EdtZadA05 -> Text = FloatToStrF((float)par[N_ST][5]/4095.0*RRG6_MAX,        ffFixed,5,1);   //������ ���6
      EdtZadA07 -> Text = FloatToStrF((float)par[N_ST][8]/4095.0*CESAR_MAX_IP,    ffFixed,5,0);   //�������� �������� CEASR ��
      EdtZadA09  -> Text = FloatToStrF((float)par[N_ST][9]/4095.0*US_MAX,          ffFixed,5,2);  //��������� ������������ �����
      EdtZadA10  -> Text = FloatToStrF((float)par[N_ST][10]/4095.0*US_MAX,         ffFixed,5,2);  //��������� ������������ �����
      EdtZadA11  -> Text = FloatToStrF((float)par[N_ST][11]/4095.0*CESAR_MAX_PD,   ffFixed,5,0);  //�������� �������� CEASR ��
      EdtZadA13 -> Text = FloatToStrF((float)par[N_ST][7]/10000.0*DAVL_MAX,       ffFixed,8,1);   //��������
      EdtZadA14 -> Text = FloatToStrF((float)par[N_ST][13],                       ffFixed,5,0);   //����� ��������
      EdtZadA16 -> Text = FloatToStrF((float)par[N_ST][12]/4095.0*SMESH_MAX_USER,  ffFixed,5,1);   //��������
      if(par[N_ST][14]) {EdtZadA15 -> Text = "��";}                                               // ��������
      else              {EdtZadA15 -> Text = "���";}
    }
    else
    {
      EdtZadA00 -> Text = "0,0";
      EdtZadA01 -> Text = "0,0";
      EdtZadA02 -> Text = "0,0";
      EdtZadA03 -> Text = "0,0";
      EdtZadA04 -> Text = "0,0";
      EdtZadA05 -> Text = "0,0";
      EdtZadA07 -> Text = "0";
      EdtZadA09  -> Text = "0,00";
      EdtZadA10  -> Text = "0,00";
      EdtZadA11  -> Text = "0";
      EdtZadA13 -> Text = "0,0";
      EdtZadA14 -> Text = "0";
      EdtZadA15 -> Text = "�";
      EdtZadA16 -> Text = "0,0";
    }

    if(shr[4])
        EdtZadA06 -> Text = FloatToStrF((float)nasmod[2]/4095.0*RRG7_MAX,        ffFixed,5,1);      //������ ���7
    else
        EdtZadA06 -> Text = "0,0";

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // ������ ���1
      if(shr[20]&&(shr[4]&&!PR_NALADKA)) { EdtTekA00 -> Text = FloatToStrF((float)aik[5]*RRG1_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA00 -> Text = "0,0";}
      // ������ ���2
      if(shr[21]&&(shr[4]&&!PR_NALADKA)) { EdtTekA01 -> Text = FloatToStrF((float)aik[6]*RRG2_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA01 -> Text = "0,0";}
      // ������ ���3
      if(shr[22]&&(shr[4]&&!PR_NALADKA)) { EdtTekA02 -> Text = FloatToStrF((float)aik[7]*RRG3_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA02 -> Text = "0,0";}
      // ������ ���4
      if(shr[23]&&(shr[4]&&!PR_NALADKA)) { EdtTekA03 -> Text = FloatToStrF((float)aik[8]*RRG4_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA03-> Text = "0,0";}
      // ������ ���5
      if(shr[24]&&(shr[4]&&!PR_NALADKA)) { EdtTekA04 -> Text = FloatToStrF((float)aik[9]*RRG5_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA04 -> Text = "0,0";}
      // ������ ���6
      if(shr[25]&&(shr[4]&&!PR_NALADKA)) { EdtTekA05 -> Text = FloatToStrF((float)aik[10]*RRG6_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA05 -> Text = "0,0";}
      // ������ ���7
      if(shr[26]&&shr[4]) { EdtTekA06 -> Text = FloatToStrF((float)aik[11]*RRG7_MAX/4095.0,ffFixed,5,1);}
      else        { EdtTekA06 -> Text = "0,0";}
      // �������� �������� <CESAR> ��
      if(shr[29]&&(shr[4]&&!PR_NALADKA)) { EdtTekA07 -> Text = FloatToStrF((float)aik[12]*CESAR_MAX_IP/4095.0,ffFixed,5,1);}
      else        { EdtTekA07 -> Text = "0";}
      // ���������� �������� <CESAR> ��
      if(shr[29]&&(shr[4]&&!PR_NALADKA)) { EdtTekA08 -> Text = FloatToStrF((float)aik[13]*CESAR_MAX_IP/4095.0,ffFixed,5,1); }
      else        { EdtTekA08 -> Text = "0";}
      // ��������� ������������ �� �����   //
      EdtTekA09  -> Text = FloatToStrF((float)aik[17]*US_MAX/4095.0,ffFixed,5,2);
      // ��������� ������������ �� �����   //
      EdtTekA10  -> Text = FloatToStrF((float)aik[16]*US_MAX/4095.0,ffFixed,5,2);
      // �������� �������� <CESAR> ��      //
      if((shr[27]||shr[28])&&(shr[4]&&!PR_NALADKA)) { EdtTekA11  -> Text = FloatToStrF((float)aik[14]*CESAR_MAX_PD/4095.0,ffFixed,5,1);}
      else                 { EdtTekA11  -> Text = "0";}
      // ���������� �������� <CESAR> ��    //
      if((shr[27]||shr[28])&&(shr[4]&&!PR_NALADKA)) { EdtTekA12  -> Text = FloatToStrF((float)aik[15]*CESAR_MAX_PD/4095.0,ffFixed,5,1); }
      else                 { EdtTekA12  -> Text = "0";}
      //��������
      if(shr[27]||shr[28])
      EdtTekA16 -> Text = FloatToStrF((float)aik[18]*SMESH_MAX_USER/4095.0, ffFixed, 6, 1);
      else
      EdtTekA16  -> Text = "0,0";
      // ��������
      if(shr[17]&&(shr[4]&&!PR_NALADKA)) { EdtTekA13-> Text = FloatToStrF((float)D_D3/10000*DAVL_MAX,ffFixed,5,1);}
      else        { EdtTekA13 -> Text = "0,0";}
      // ����� ��������
      if((shr[4]&&!PR_NALADKA)) {  EdtTekA14 -> Text = IntToStr(T_PROC);  }
      else       {  EdtTekA14 -> Text = "0";  }
      // ��������
      if((shr[4]&&!PR_NALADKA)) {
      if(par[N_ST][14])
        EdtTekA15 -> Text = "��";
      else
        EdtTekA15 -> Text = "���";
      }
      else
      {
        EdtTekA15 -> Text = "�";
      }

    }

    //���������� �������
	zad_pl -> Caption = IntToStr(par[N_ST][20]);
	tek_pl -> Caption = N_PL;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--����������� ������--//
void TForm1::VisualButtons()
{
    // ������� ������
    OtkKam    -> Font -> Color = SetPopeColor(shr[1]);
    // ������� ����
    PnlPVS    -> Font -> Color = SetPopeColor(shr[3]);
    // ����� ��
    PnlRC    -> Font -> Color = SetPopeColor(shr[5]);
    // ���������� ���������
    PnlUstOff   -> Font -> Color = SetPopeColor(shr[7]);
    // ���� �������
    PnlSbor -> Font -> Color = SetPopeColor(shr[6]);

    ///////////////////////////////////////////////////////////////////////////
    // ���1 (���.)
    PnlRRG1On   -> Font -> Color = SetPopeColor(shr[20]);
    // ���2 (���.)
    PnlRRG2On   -> Font -> Color = SetPopeColor(shr[21]);
    // ���3 (���.)
    PnlRRG3On   -> Font -> Color = SetPopeColor(shr[22]);
    // ���4 (���.)
    PnlRRG4On   -> Font -> Color = SetPopeColor(shr[23]);
    // ���5 (���.)
    PnlRRG5On   -> Font -> Color = SetPopeColor(shr[24]);
    // ���6 (���.)
    PnlRRG6On   -> Font -> Color = SetPopeColor(shr[25]);
    // ���7 ����� (���.)
    PnlRRG7OnNasos  -> Font -> Color = SetPopeColor(shr[26]&&!PR_HEL);
    // ���7 ������ (���.)
    PnlRRG7OnChamber-> Font -> Color = SetPopeColor(shr[26]&&PR_HEL);
    // ������� ��
    PnlSZOn     -> Font -> Color = SetPopeColor(shr[10]);
    // ������� ��
    PnlSZOff    -> Font -> Color = SetPopeColor(shr[11]);
    // �� �������
    PnlDZOn     -> Font -> Color = SetPopeColor(shr[18]);
    // �� �������
    PnlDZOff    -> Font -> Color = SetPopeColor(shr[19]);
    // �� �� ����
    PnlDZUgol   -> Font -> Color = SetPopeColor(shr[17]);
    // ��������� ��� ��
    PnlCESAROn  -> Font -> Color = SetPopeColor(shr[29]);
    // ��������� ��� ��
    PnlVCHPdOn     -> Font -> Color = SetPopeColor(shr[27]);
    // ��������� ��� ��������
    PnlVCHPdSmesh     -> Font -> Color = SetPopeColor(shr[28]);
    // ������ ������ �������� (���.)
    PnlKamOn     -> Font -> Color = SetPopeColor(shr[33]);
    // ������ ������ �������� (����.)
    PnlKamOff     -> Font -> Color = SetPopeColor(shr[34]);
    // ������ ������ ��������� (���.)
    PnlKamRazOn     -> Font -> Color = SetPopeColor(shr[35]);
    // ������ ������ ��������� (����.)
    PnlKamRazOff     -> Font -> Color = SetPopeColor(shr[36]);

    // ������ ����������
    Pnl_KasH -> Font -> Color = SetPopeColor(shr[39]);
    Pnl_KasS-> Font -> Color = SetPopeColor(shr[40]);
    Pnl_PerH -> Font -> Color = SetPopeColor(shr[12]);
    Pnl_PerS -> Font -> Color = SetPopeColor(shr[13]);
    Pnl_PovH -> Font -> Color = SetPopeColor(shr[14]);
    Pnl_PovS -> Font -> Color = SetPopeColor(shr[15]);

    // ������������ ����
    if(shr[9])
    {   if(PR_TRTEST)
        {   PnlTTOn  -> Font -> Color = SetPopeColor(0);
            PnlTTOff -> Font -> Color = SetPopeColor(1);
        }
        else
        {   PnlTTOn  -> Font -> Color = SetPopeColor(1);
            PnlTTOff -> Font -> Color = SetPopeColor(0);
        }
    }
    else
    {   PnlTTOn  -> Font -> Color = SetPopeColor(0);
        PnlTTOff -> Font -> Color = SetPopeColor(0);
    }
    // ������ �����
    Panel278     -> Font -> Color = SetPopeColor(shr[37]);
    // ������ ����
    Panel277     -> Font -> Color = SetPopeColor(shr[38]);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ����������� ����--//
void TForm1::VisualVoda()
{
 //���� ���������� �/�
if(zin[0]&0x01)
PnlKan00->Color=0x00EAD999;
else
PnlKan00->Color=0x003030FF;

 //���� ���������� ��� ���
if(zin[0]&0x02)
PnlKan01->Color=0x00EAD999;
else
PnlKan01->Color=0x003030FF;

 //���� ���������� ���
if(zin[0]&0x04)
PnlKan02->Color=0x00EAD999;
else
PnlKan02->Color=0x003030FF;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ����������� ���� ���. �/�
    PnlKan00 -> Caption = FloatToStrF((((float)aik[0]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " 'C";
    if(((((float)aik[0]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan00 -> Caption = "0,0 �C"; }
    // ����������� ���� ���������� ��� ���
    PnlKan01 -> Caption = FloatToStrF((((float)aik[1]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " 'C";
    if(((((float)aik[1]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan01 -> Caption = "0,0 �C"; }
    // ����������� ���� ���������� ���
    PnlKan02 -> Caption = FloatToStrF((((float)aik[2]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " 'C";
    if(((((float)aik[2]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan02 -> Caption = "0,0 �C"; }
}
//---------------------------------------------------------------------------
//--������������ ��������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualColorElement()
{
        anim_fase = !anim_fase;

            // �������
    if((diagn[14])|| (diagn[28]))
	{
		SetOut(1,1,0x80); //
	}
    else
	{
		SetOut(0,1,0x80); //
	}

    // ������
    if((pr_yel)||(!shr[1]&&!shr[2]&&!shr[3]&&!shr[5]&&!shr[6]&&!shr[7]&&!shr[9]))
	{
		SetOut(1,1,0x40); //
	}
    else
	{
		SetOut(0,1,0x40); //
	}

    // �������
    if(shr[1]||shr[2]||shr[3]||shr[4]||shr[5]||shr[6]||shr[7]||shr[9])
	{
		SetOut(1,1,0x20); //
	}
    else
	{
		SetOut(0,1,0x20); //

	}

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[20]) { rrg1 -> Visible = true;  kl1_tube -> Visible = true;  }  // ���1
  else        { rrg1 -> Visible = false; kl1_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x01)  // ��-���1
  { kl_rrg1 -> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    if(shr[20]) { kl1_tube -> Visible = true; }
  }
  else
  { kl_rrg1 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[21]) { rrg2 -> Visible = true;  kl2_tube -> Visible = true;  }  // ���2
  else        { rrg2 -> Visible = false; kl2_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x02)  // ��-���2
  { kl_rrg2 -> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    if(shr[21]) { kl2_tube -> Visible = true; }
  }
  else
  { kl_rrg2 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[22]) { rrg3 -> Visible = true;  kl3_tube -> Visible = true;  }  // ���3
  else        { rrg3 -> Visible = false; kl3_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x04)  // ��-���3
  { kl_rrg3 -> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    if(shr[22]) { kl3_tube -> Visible = true; }
  }
  else
  { kl_rrg3 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[23]) { rrg4 -> Visible = true;  kl4_tube -> Visible = true;  }  // ���4
  else        { rrg4 -> Visible = false; kl4_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x08)  // ��-���4
  { kl_rrg4 -> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    if(shr[23]) { kl4_tube -> Visible = true; }
  }
  else
  { kl_rrg4 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[24]) { rrg5 -> Visible = true;  kl5_tube -> Visible = true;  }  // ���5
  else        { rrg5 -> Visible = false; kl5_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x10)  // ��-���5
  { kl_rrg5 -> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    if(shr[24]) { kl5_tube -> Visible = true; }
  }
  else
  { kl_rrg5 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[25]) { rrg6 -> Visible = true;  kl6_tube -> Visible = true;  }  // ���6
  else        { rrg6 -> Visible = false; kl6_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x20)  // ��-���6
  { kl_rrg6 -> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    if(shr[25]) { kl6_tube -> Visible = true; }
  }
  else
  { kl_rrg6 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[26]) { rrg7 -> Visible = true;  rrg7_tube -> Visible = true;  }  // ���7
  else        { rrg7 -> Visible = false; rrg7_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x4000)  // ��3
  { kl3 -> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
    if(shr[26]) { rrg7_tube -> Visible = true; }
  }
  else
  { kl3 -> Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[26]&&(out[3]&0x4000)) // ��3
    kl3_kam_tube ->Visible=true;
  else
    kl3_kam_tube ->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x2000){//��4
  kl4->Picture->Bitmap = e_klg_o->Picture->Bitmap; }
  else kl4->Picture->Bitmap = 0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[17])  // ��
  dz->Picture->Bitmap = dross->Picture->Bitmap;
  else{
  switch(zin[1]&0x30)
  { case 0x00: { dz->Picture->Bitmap = zasl_grey->Picture->Bitmap;  break; } // �� ����������
    case 0x10: { dz->Picture->Bitmap = zasl_white->Picture->Bitmap; break; } // ������
    case 0x20: { dz->Picture->Bitmap = 0;                           break; } // ������
    case 0x30: { dz->Picture->Bitmap = zasl_grey->Picture->Bitmap;  break; } // ������������
  } }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(diagn[21]&0x08) { fn_kam->Picture->Bitmap = FN_red->Picture->Bitmap; fn_kam_tube -> Visible = false;} //�������� ������
  else
  { if(zin[2]&0x10)
    { if(zin[2]&0x02)      { fn_kam->Picture->Bitmap = FN_dry->Picture->Bitmap;  }
      else                 { fn_kam->Picture->Bitmap = 0;                        }
    }
    else
    { fn_kam->Picture->Bitmap = FN_yellow->Picture->Bitmap; }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(((out[3]&0x2000)&&(rrg7_tube -> Visible))||(zin[2]&0x02))
  fn_kam_tube -> Visible = true;
  else
  fn_kam_tube -> Visible = false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[2]&0x01){//�������� �����
    fn_shl->Picture->Bitmap = FN_SHL_o->Picture->Bitmap; fn_shl_tube->Visible=true;}
  else{
    fn_shl->Picture->Bitmap = 0; fn_shl_tube->Visible=false;}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((shr[20]&&(out[2]&0x01))||(shr[21]&&(out[2]&0x02))||(shr[22]&&(out[2]&0x04))||
  (shr[23]&&(out[2]&0x08))||(shr[24]&&(out[2]&0x10))||(shr[25]&&(out[2]&0x20))) { kl_kam_tube -> Visible = true;  } // ����� ���1-���4
  else                                                                          { kl_kam_tube -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    switch(zin[0]&0xC000)  // ��-�4
  { case 0x0000: { kl_d4->Picture->Bitmap = e_klg_n->Picture->Bitmap; break; }       // ������������
    case 0x4000: { kl_d4->Picture->Bitmap = e_klg_o->Picture->Bitmap; break; }       // ������
    case 0x8000: { kl_d4->Picture->Bitmap = 0;                             break; }  // ������
    case 0xC000: { kl_d4->Picture->Bitmap = e_klg_n->Picture->Bitmap; break; }       // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    switch(zin[1]&0x03)  // ��-�5
  { case 0x00: { kl_d5->Picture->Bitmap = e_klg_n->Picture->Bitmap; break; }       // ������������
    case 0x01: { kl_d5->Picture->Bitmap = e_klg_o->Picture->Bitmap; break; }       // ������
    case 0x02: { kl_d5->Picture->Bitmap = 0;                        break; }       // ������
    case 0x03: { kl_d5->Picture->Bitmap = e_klg_n->Picture->Bitmap; break; }       // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x800) //��-���3
  { kl_nap3->Picture->Bitmap = e_klg_o->Picture->Bitmap; }
  else
  { kl_nap3->Picture->Bitmap = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x100) //��-���1
  {kl_nap1->Picture->Bitmap = e_klg_o->Picture->Bitmap;kl_nap3_tube -> Visible=true;}
  else
  {kl_nap1->Picture->Bitmap = 0;kl_nap3_tube -> Visible=false;}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x400) //��-���2
  kl_nap2->Picture->Bitmap = e_klg_o->Picture->Bitmap;
  else
  kl_nap2->Picture->Bitmap = 0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((out[3]&0x800)||(out[3]&0x400)||((out[0]&0x1000)||((zin[0]&0x100)&&(!(zin[0]&0x100)))&&(zin[2]&0x01))) //����� �������� �����
  fk_shl_tube->Visible=true;
  else
  fk_shl_tube->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[0]&0x1000)//��-�� ����
  fk_shl1->Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
  else
  fk_shl1->Picture->Bitmap = 0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    switch(zin[0]&0x3000)  //��-���
  { case 0x0000: { fk_tmn->Picture->Bitmap = e_klg_n->Picture->Bitmap; tmn_kl5_tube->Visible=false; break; }       // ������������
    case 0x1000: { fk_tmn->Picture->Bitmap = e_klg_o->Picture->Bitmap; tmn_kl5_tube->Visible=true; break; }       // ������
    case 0x2000: { fk_tmn->Picture->Bitmap = 0; tmn_kl5_tube->Visible=false;                             break; }  // ������
    case 0x3000: { fk_tmn->Picture->Bitmap = e_klg_n->Picture->Bitmap; tmn_kl5_tube->Visible=false; break; }       // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    switch(zin[0]&0x300)  // ��-��
  { case 0x000: { fk_shl2->Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break; }       // ������������
    case 0x100: { fk_shl2->Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; break; }       // ������
    case 0x200: { fk_shl2->Picture->Bitmap = 0;                             break; }  // ������
    case 0x300: { fk_shl2->Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break; }       // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch(zin[0]&0xC00)  // ��-���
  { case 0x000: { fk_kam->Picture->Bitmap = e_klg_n->Picture->Bitmap; Image23->Visible=false;   break; }       // ������������
    case 0x400: { fk_kam->Picture->Bitmap = e_klg_o->Picture->Bitmap;Image23->Visible=true; break; }       // ������
    case 0x800: { fk_kam->Picture->Bitmap = 0; Image23->Visible=false;                        break; }  // ������
    case 0xC00: { fk_kam->Picture->Bitmap = e_klg_n->Picture->Bitmap; Image23->Visible=false;     break; }       // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x1000){//��5
  kl5->Picture->Bitmap = e_klg_o->Picture->Bitmap; tmn_kl9_tube ->Visible=true;}
  else{
  kl5->Picture->Bitmap = 0; tmn_kl9_tube ->Visible=false;}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //���
  if(zin[2]&0x200)       { tmn->Picture->Bitmap=tmn_red->Picture->Bitmap;    } // ������
  else if(zin[2]&0x4000) { tmn->Picture->Bitmap=tmn_yellow->Picture->Bitmap;    } // ��������������
  else if(zin[2]&0x400||zin[2]&0x1000) // ������/����������
  { if(anim_fase) { tmn->Picture->Bitmap=tmn_white->Picture->Bitmap; }
    else          { tmn->Picture->Bitmap = 0;                        }
  }
  else if(zin[2]&0x800)  { tmn->Picture->Bitmap=tmn_white->Picture->Bitmap; }  // �����
  else                   { tmn->Picture->Bitmap = 0;                         }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[2]&0x400) { str_down -> Visible = true;  }  //����������
  else             { str_down -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[2]&0x1000) { str_up -> Visible = true;  }  //������
  else              { str_up -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((zin[2]&0x400)||(zin[2]&0x800)||(zin[2]&0x1000)) //����� ���
  dz_tube->Visible=true;
  else
  dz_tube->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(PR_KLASTER)
  {
        switch(KOM_MOD & 0x300) //������� ������
        { case 0x00: { zatvor->Picture->Bitmap = zatvor_grey->Picture->Bitmap;  break; } // ������������
        case 0x100: { zatvor->Picture->Bitmap = 0;                                break; } // ������
        case 0x200: { zatvor->Picture->Bitmap = zatvor_green->Picture->Bitmap; break; } // ������
        case 0x300: { zatvor->Picture->Bitmap = zatvor_grey->Picture->Bitmap;  break; } // ������������
        }
  }
  else
  {
        switch(zin[1]&0x0C) //������� ������
        { case 0x00: { zatvor->Picture->Bitmap = zatvor_grey->Picture->Bitmap;  break; } // ������������
        case 0x04: { zatvor->Picture->Bitmap = 0;                                break; } // ������
        case 0x08: { zatvor->Picture->Bitmap = zatvor_green->Picture->Bitmap; break; } // ������
        case 0x0C: { zatvor->Picture->Bitmap = zatvor_grey->Picture->Bitmap;  break; } // ������������
        }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[3]&0x4000)//����� �����
  door->Visible=true;
  else
  door->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[3]&0x20) { datchik1 -> Visible = true;  }  //���� ��������� � �����
  else            { datchik1 -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[3]&0x40) { datchik5 -> Visible = true;  }  //���� ��������� � ���
  else            { datchik5 -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[4]&0x4000) { sensor_l -> Visible = true;  }  //���� �������� ����� ��������
  else              { sensor_l -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[4]&0x8000) { sensor_r -> Visible = true;  }  //���� �������� ����� �������
  else              { sensor_r -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch(zin[1]&0xC0) //�����
  { case 0x00: { pin->Picture->Bitmap = pin_grey->Picture->Bitmap;  break; }              // ������������
    case 0x40: { pin->Picture->Bitmap = pin_green->Picture->Bitmap; pin->Top=326;break; } // ������
    case 0x80: { pin->Picture->Bitmap = pin_green->Picture->Bitmap; pin->Top=338; break; }// �����
    case 0xC0: { pin->Picture->Bitmap = pin_grey->Picture->Bitmap;  break; }              // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[3]&0x1000) //������ � HOME
  prizhim_null->Picture->Bitmap = prizhim_home->Picture->Bitmap;
  else
  prizhim_null->Picture->Bitmap = prizhim->Picture->Bitmap;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[33]) //������ ������
  nagr_ko->Visible=true;
  else
  nagr_ko->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[33]) //������ ������
  nagr_kr->Visible=true;
  else
  nagr_kr->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((shr[27])||(shr[28])) //��� �����
  Image8->Visible=true;
  else
  Image8->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[29]) //��� ��������
  Image11->Visible=true;
  else
  Image11->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[20]||shr[21]||shr[22]||shr[23]||shr[24]||shr[25]) //�������
  Image10->Visible=true;
  else
  Image10->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((shr[27]&&VRGIS)||(shr[28]&&VRGIS)) //������ ���
  {
    if(anim_fase)
        Image9->Visible=true;
    else
        Image9->Visible=false;
  }
  else
    Image9->Visible=false;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(shr[29]&&VRGIR) //������ ����
  {
    if(anim_fase)
        Image12->Visible=true;
    else
        Image12->Visible=false;
  }
  else
    Image12->Visible=false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x200)//��1
  kl1->Picture->Bitmap = e_klg_o->Picture->Bitmap;
  else
  kl1->Picture->Bitmap = 0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x400)//��2
  kl2->Picture->Bitmap = e_klg_o->Picture->Bitmap;
  else
  kl2->Picture->Bitmap = 0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ��������� ��������� ����������
  if(zin[4]&0x40)//�������
  {kasseta_null->Picture->Bitmap =kasseta_home->Picture->Bitmap; }
  else
  {kasseta_null->Picture->Bitmap =kasseta_green->Picture->Bitmap;}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 if(zin[4]&0x1000)//�������
    {man_active->Color=clWhite; }
  else
    {man_active->Color= 0x0064F046;}   
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[4]&0x200)//�����������
    {man_per->Color=clWhite; }
  else
   { man_per->Color= 0x0064F046; }  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

//---------------------------------------------------------------------------
//--���������� ���������� ��������--//
//---------------------------------------------------------------------------
void SetOut(bool value, unsigned char outNmb, unsigned int outMask)
{   // ���������� �������� ������ ���������
    value ? out[outNmb] |= outMask : out[outNmb] &= (~outMask);
}
//---------------------------------------------------------------------------
//--������� ����������� ������--//
//------------------------------+---------------------------------------------
void A_OUT(unsigned int Nmb,unsigned int Value)
{ // �������� �������� ���������� �������
  aout[Nmb] = Value;
  aoutKon[Nmb] = Value;
}
//---------------------------------------------------------------------------
void SetZin(bool value, unsigned char outNmb, unsigned int outMask)
{   // ���������� �������� ������ ���������
    value ? zin[outNmb] |= outMask : zin[outNmb] &= (~outMask);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� ������ �������--//
void __fastcall TForm1::SetOutClick(TObject *Sender)
{   // ��������� OUT��
    unsigned char bitNmb  = PCFormatOut -> TabIndex;
    unsigned int  bitMask = StrToInt(((TButton*)Sender)->Hint);
    // ��������� ������
    out[bitNmb] & bitMask ? SetOut(0, bitNmb, bitMask) : SetOut(1, bitNmb, bitMask);
    // ������������ ������
    //VisualFormat();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::ZeroAoutClick(TObject *Sender)
{ // ��������� � ���� ����������� ������
  unsigned int
   bitNmb = StrToInt(((TImage*)Sender)->Hint),
   i = bitNmb / 4 ,
   j = bitNmb % 4 ;
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   Zad_Aout[i][j] -> Position = Zad_Aout[i][j] -> Min;
   A_OUT(bitNmb, Zad_Aout[i][j] -> Min);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::SetZinClick(TObject *Sender)
{
    if(pr_otl)
    {
        // ��������� OUT��
        unsigned char bitNmb = PCFormatZin -> TabIndex;
        unsigned int  bitMask = StrToInt(((TButton*)Sender)->Hint);
        // ��������� ������
        zin[bitNmb] & bitMask ? SetZin(0, bitNmb, bitMask) : SetZin(1, bitNmb, bitMask);
        // ������������ ������
        VisualFormat();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::SetAoutChange(TObject *Sender)
{ // ��������� ��������� ����������� ������
  unsigned int
   bitNmb = StrToInt(((TImage*)Sender)->Hint),
   i = bitNmb / 4,
   j = bitNmb % 4;
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   Dec_Aout_zad[i][j] -> Text = FloatToStrF(float(((TScrollBar*)Sender)->Position), ffFixed, 5, 0);
   // ����������� ������� �������� ���������� ���������
   UV_Aout_zad[i][j] -> Text = FloatToStrF(float(((TScrollBar*)Sender)->Position-8192) * 10.0 / 8191.0, ffFixed, 5, 3);
   // ������������ ���������� �����������
   CG_Aout_zad[i][j] -> Progress = Zad_Aout[i][j] -> Position;
   // ������������ ������
   //VisualFormat();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::EntAoutClick(TObject *Sender)
{ // ��������� ����������� �����
  unsigned int
   bitNmb = StrToInt(((TImage*)Sender)->Hint),
   i = bitNmb / 4,
   j = bitNmb % 4;
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   A_OUT(bitNmb, Zad_Aout[i][j] -> Position);
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void __fastcall TForm1::Timer_250Timer(TObject *Sender)
{
   VisualFormat();
   VisualDebug();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--������������ ���������� ��������--//
//---------------------------------------------------------------------------
void TForm1::VisualParA()
{

//���������� �������
EdtAKon1_20 -> Text = FloatToStrF((float)par[1][20], ffFixed, 5, 0);

//1 ������
EdtAKon1_0 -> Text = FloatToStrF((float)par[1][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon1_1 -> Text = FloatToStrF((float)par[1][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon1_2 -> Text = FloatToStrF((float)par[1][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon1_3 -> Text = FloatToStrF((float)par[1][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon1_4 -> Text = FloatToStrF((float)par[1][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon1_5 -> Text = FloatToStrF((float)par[1][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon1_7 -> Text = FloatToStrF((float)par[1][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon1_8 -> Text = FloatToStrF((float)par[1][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon1_9 -> Text = FloatToStrF((float)par[1][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon1_10 -> Text = FloatToStrF((float)par[1][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon1_11 -> Text = FloatToStrF((float)par[1][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon1_12 -> Text = FloatToStrF((float)par[1][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon1_13 -> Text = FloatToStrF((float)par[1][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon1_14 -> Text = ( par[1][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//2 ������
EdtAKon2_0 -> Text = FloatToStrF((float)par[2][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon2_1 -> Text = FloatToStrF((float)par[2][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon2_2 -> Text = FloatToStrF((float)par[2][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon2_3 -> Text = FloatToStrF((float)par[2][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon2_4 -> Text = FloatToStrF((float)par[2][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon2_5 -> Text = FloatToStrF((float)par[2][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon2_7 -> Text = FloatToStrF((float)par[2][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon2_8 -> Text = FloatToStrF((float)par[2][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon2_9 -> Text = FloatToStrF((float)par[2][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon2_10 -> Text = FloatToStrF((float)par[2][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon2_11 -> Text = FloatToStrF((float)par[2][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon2_12 -> Text = FloatToStrF((float)par[2][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon2_13 -> Text = FloatToStrF((float)par[2][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon2_14 -> Text = ( par[2][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//3 ������
EdtAKon3_0 -> Text = FloatToStrF((float)par[3][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon3_1 -> Text = FloatToStrF((float)par[3][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon3_2 -> Text = FloatToStrF((float)par[3][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon3_3 -> Text = FloatToStrF((float)par[3][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon3_4 -> Text = FloatToStrF((float)par[3][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon3_5 -> Text = FloatToStrF((float)par[3][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon3_7 -> Text = FloatToStrF((float)par[3][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon3_8 -> Text = FloatToStrF((float)par[3][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon3_9 -> Text = FloatToStrF((float)par[3][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon3_10 -> Text = FloatToStrF((float)par[3][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon3_11 -> Text = FloatToStrF((float)par[3][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon3_12 -> Text = FloatToStrF((float)par[3][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon3_13 -> Text = FloatToStrF((float)par[3][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon3_14 -> Text = ( par[3][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//4 ������
EdtAKon4_0 -> Text = FloatToStrF((float)par[4][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon4_1 -> Text = FloatToStrF((float)par[4][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon4_2 -> Text = FloatToStrF((float)par[4][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon4_3 -> Text = FloatToStrF((float)par[4][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon4_4 -> Text = FloatToStrF((float)par[4][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon4_5 -> Text = FloatToStrF((float)par[4][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon4_7 -> Text = FloatToStrF((float)par[4][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon4_8 -> Text = FloatToStrF((float)par[4][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon4_9 -> Text = FloatToStrF((float)par[4][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon4_10 -> Text = FloatToStrF((float)par[4][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon4_11 -> Text = FloatToStrF((float)par[4][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon4_12 -> Text = FloatToStrF((float)par[4][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon4_13 -> Text = FloatToStrF((float)par[4][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon4_14 -> Text = ( par[4][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//5 ������
EdtAKon5_0 -> Text = FloatToStrF((float)par[5][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon5_1 -> Text = FloatToStrF((float)par[5][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon5_2 -> Text = FloatToStrF((float)par[5][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon5_3 -> Text = FloatToStrF((float)par[5][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon5_4 -> Text = FloatToStrF((float)par[5][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon5_5 -> Text = FloatToStrF((float)par[5][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon5_7 -> Text = FloatToStrF((float)par[5][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon5_8 -> Text = FloatToStrF((float)par[5][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon5_9 -> Text = FloatToStrF((float)par[5][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon5_10 -> Text = FloatToStrF((float)par[5][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon5_11 -> Text = FloatToStrF((float)par[5][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon5_12 -> Text = FloatToStrF((float)par[5][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon5_13 -> Text = FloatToStrF((float)par[5][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon5_14 -> Text = ( par[5][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//6 ������
EdtAKon6_0 -> Text = FloatToStrF((float)par[6][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon6_1 -> Text = FloatToStrF((float)par[6][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon6_2 -> Text = FloatToStrF((float)par[6][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon6_3 -> Text = FloatToStrF((float)par[6][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon6_4 -> Text = FloatToStrF((float)par[6][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon6_5 -> Text = FloatToStrF((float)par[6][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon6_7 -> Text = FloatToStrF((float)par[6][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon6_8 -> Text = FloatToStrF((float)par[6][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon6_9 -> Text = FloatToStrF((float)par[6][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon6_10 -> Text = FloatToStrF((float)par[6][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon6_11 -> Text = FloatToStrF((float)par[6][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon6_12 -> Text = FloatToStrF((float)par[6][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon6_13 -> Text = FloatToStrF((float)par[6][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon6_14 -> Text = ( par[6][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//7 ������
EdtAKon7_0 -> Text = FloatToStrF((float)par[7][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon7_1 -> Text = FloatToStrF((float)par[7][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon7_2 -> Text = FloatToStrF((float)par[7][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon7_3 -> Text = FloatToStrF((float)par[7][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon7_4 -> Text = FloatToStrF((float)par[7][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon7_5 -> Text = FloatToStrF((float)par[7][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon7_7 -> Text = FloatToStrF((float)par[7][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon7_8 -> Text = FloatToStrF((float)par[7][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon7_9 -> Text = FloatToStrF((float)par[7][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon7_10 -> Text = FloatToStrF((float)par[7][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon7_11 -> Text = FloatToStrF((float)par[7][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon7_12 -> Text = FloatToStrF((float)par[7][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon7_13 -> Text = FloatToStrF((float)par[7][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon7_14 -> Text = ( par[7][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//8 ������
EdtAKon8_0 -> Text = FloatToStrF((float)par[8][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon8_1 -> Text = FloatToStrF((float)par[8][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon8_2 -> Text = FloatToStrF((float)par[8][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon8_3 -> Text = FloatToStrF((float)par[8][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon8_4 -> Text = FloatToStrF((float)par[8][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon8_5 -> Text = FloatToStrF((float)par[8][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon8_7 -> Text = FloatToStrF((float)par[8][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon8_8 -> Text = FloatToStrF((float)par[8][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon8_9 -> Text = FloatToStrF((float)par[8][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon8_10 -> Text = FloatToStrF((float)par[8][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon8_11 -> Text = FloatToStrF((float)par[8][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon8_12 -> Text = FloatToStrF((float)par[8][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon8_13 -> Text = FloatToStrF((float)par[8][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon8_14 -> Text = ( par[8][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//9 ������
EdtAKon9_0 -> Text = FloatToStrF((float)par[9][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon9_1 -> Text = FloatToStrF((float)par[9][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon9_2 -> Text = FloatToStrF((float)par[9][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon9_3 -> Text = FloatToStrF((float)par[9][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon9_4 -> Text = FloatToStrF((float)par[9][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon9_5 -> Text = FloatToStrF((float)par[9][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon9_7 -> Text = FloatToStrF((float)par[9][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon9_8 -> Text = FloatToStrF((float)par[9][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon9_9 -> Text = FloatToStrF((float)par[9][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon9_10 -> Text = FloatToStrF((float)par[9][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon9_11 -> Text = FloatToStrF((float)par[9][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon9_12 -> Text = FloatToStrF((float)par[9][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon9_13 -> Text = FloatToStrF((float)par[9][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon9_14 -> Text = ( par[9][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//10 ������
EdtAKon10_0 -> Text = FloatToStrF((float)par[10][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon10_1 -> Text = FloatToStrF((float)par[10][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon10_2 -> Text = FloatToStrF((float)par[10][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon10_3 -> Text = FloatToStrF((float)par[10][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon10_4 -> Text = FloatToStrF((float)par[10][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon10_5 -> Text = FloatToStrF((float)par[10][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon10_7 -> Text = FloatToStrF((float)par[10][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon10_8 -> Text = FloatToStrF((float)par[10][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon10_9 -> Text = FloatToStrF((float)par[10][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon10_10 -> Text = FloatToStrF((float)par[10][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon10_11 -> Text = FloatToStrF((float)par[10][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon10_12 -> Text = FloatToStrF((float)par[10][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon10_13 -> Text = FloatToStrF((float)par[10][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon10_14 -> Text = ( par[10][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//11 ������
EdtAKon11_0 -> Text = FloatToStrF((float)par[11][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon11_1 -> Text = FloatToStrF((float)par[11][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon11_2 -> Text = FloatToStrF((float)par[11][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon11_3 -> Text = FloatToStrF((float)par[11][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon11_4 -> Text = FloatToStrF((float)par[11][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon11_5 -> Text = FloatToStrF((float)par[11][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon11_7 -> Text = FloatToStrF((float)par[11][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon11_8 -> Text = FloatToStrF((float)par[11][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon11_9 -> Text = FloatToStrF((float)par[11][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon11_10 -> Text = FloatToStrF((float)par[11][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon11_11 -> Text = FloatToStrF((float)par[11][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon11_12 -> Text = FloatToStrF((float)par[11][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon11_13 -> Text = FloatToStrF((float)par[11][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon11_14 -> Text = ( par[11][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//12 ������
EdtAKon12_0 -> Text = FloatToStrF((float)par[12][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon12_1 -> Text = FloatToStrF((float)par[12][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon12_2 -> Text = FloatToStrF((float)par[12][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon12_3 -> Text = FloatToStrF((float)par[12][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon12_4 -> Text = FloatToStrF((float)par[12][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon12_5 -> Text = FloatToStrF((float)par[12][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon12_7 -> Text = FloatToStrF((float)par[12][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon12_8 -> Text = FloatToStrF((float)par[12][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon12_9 -> Text = FloatToStrF((float)par[12][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon12_10 -> Text = FloatToStrF((float)par[12][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon12_11 -> Text = FloatToStrF((float)par[12][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon12_12 -> Text = FloatToStrF((float)par[12][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon12_13 -> Text = FloatToStrF((float)par[12][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon12_14 -> Text = ( par[12][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//13 ������
EdtAKon13_0 -> Text = FloatToStrF((float)par[13][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon13_1 -> Text = FloatToStrF((float)par[13][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon13_2 -> Text = FloatToStrF((float)par[13][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon13_3 -> Text = FloatToStrF((float)par[13][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon13_4 -> Text = FloatToStrF((float)par[13][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon13_5 -> Text = FloatToStrF((float)par[13][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon13_7 -> Text = FloatToStrF((float)par[13][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon13_8 -> Text = FloatToStrF((float)par[13][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon13_9 -> Text = FloatToStrF((float)par[13][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon13_10 -> Text = FloatToStrF((float)par[13][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon13_11 -> Text = FloatToStrF((float)par[13][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon13_12 -> Text = FloatToStrF((float)par[13][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon13_13 -> Text = FloatToStrF((float)par[13][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon13_14 -> Text = ( par[13][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//14 ������
EdtAKon14_0 -> Text = FloatToStrF((float)par[14][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon14_1 -> Text = FloatToStrF((float)par[14][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon14_2 -> Text = FloatToStrF((float)par[14][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon14_3 -> Text = FloatToStrF((float)par[14][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon14_4 -> Text = FloatToStrF((float)par[14][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon14_5 -> Text = FloatToStrF((float)par[14][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon14_7 -> Text = FloatToStrF((float)par[14][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon14_8 -> Text = FloatToStrF((float)par[14][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon14_9 -> Text = FloatToStrF((float)par[14][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon14_10 -> Text = FloatToStrF((float)par[14][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon14_11 -> Text = FloatToStrF((float)par[14][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon14_12 -> Text = FloatToStrF((float)par[14][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon14_13 -> Text = FloatToStrF((float)par[14][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon14_14 -> Text = ( par[14][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//15 ������
EdtAKon15_0 -> Text = FloatToStrF((float)par[15][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon15_1 -> Text = FloatToStrF((float)par[15][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon15_2 -> Text = FloatToStrF((float)par[15][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon15_3 -> Text = FloatToStrF((float)par[15][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon15_4 -> Text = FloatToStrF((float)par[15][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon15_5 -> Text = FloatToStrF((float)par[15][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon15_7 -> Text = FloatToStrF((float)par[15][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon15_8 -> Text = FloatToStrF((float)par[15][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon15_9 -> Text = FloatToStrF((float)par[15][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon15_10 -> Text = FloatToStrF((float)par[15][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon15_11 -> Text = FloatToStrF((float)par[15][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon15_12 -> Text = FloatToStrF((float)par[15][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon15_13 -> Text = FloatToStrF((float)par[15][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon15_14 -> Text = ( par[15][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//16 ������
EdtAKon16_0 -> Text = FloatToStrF((float)par[16][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon16_1 -> Text = FloatToStrF((float)par[16][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon16_2 -> Text = FloatToStrF((float)par[16][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon16_3 -> Text = FloatToStrF((float)par[16][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon16_4 -> Text = FloatToStrF((float)par[16][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon16_5 -> Text = FloatToStrF((float)par[16][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon16_7 -> Text = FloatToStrF((float)par[16][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon16_8 -> Text = FloatToStrF((float)par[16][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon16_9 -> Text = FloatToStrF((float)par[16][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon16_10 -> Text = FloatToStrF((float)par[16][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon16_11 -> Text = FloatToStrF((float)par[16][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon16_12 -> Text = FloatToStrF((float)par[16][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon16_13 -> Text = FloatToStrF((float)par[16][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon16_14 -> Text = ( par[16][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//17 ������
EdtAKon17_0 -> Text = FloatToStrF((float)par[17][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon17_1 -> Text = FloatToStrF((float)par[17][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon17_2 -> Text = FloatToStrF((float)par[17][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon17_3 -> Text = FloatToStrF((float)par[17][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon17_4 -> Text = FloatToStrF((float)par[17][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon17_5 -> Text = FloatToStrF((float)par[17][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon17_7 -> Text = FloatToStrF((float)par[17][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon17_8 -> Text = FloatToStrF((float)par[17][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon17_9 -> Text = FloatToStrF((float)par[17][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon17_10 -> Text = FloatToStrF((float)par[17][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon17_11 -> Text = FloatToStrF((float)par[17][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon17_12 -> Text = FloatToStrF((float)par[17][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon17_13 -> Text = FloatToStrF((float)par[17][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon17_14 -> Text = ( par[17][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//18 ������
EdtAKon18_0 -> Text = FloatToStrF((float)par[18][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon18_1 -> Text = FloatToStrF((float)par[18][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon18_2 -> Text = FloatToStrF((float)par[18][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon18_3 -> Text = FloatToStrF((float)par[18][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon18_4 -> Text = FloatToStrF((float)par[18][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon18_5 -> Text = FloatToStrF((float)par[18][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon18_7 -> Text = FloatToStrF((float)par[18][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon18_8 -> Text = FloatToStrF((float)par[18][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon18_9 -> Text = FloatToStrF((float)par[18][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon18_10 -> Text = FloatToStrF((float)par[18][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon18_11 -> Text = FloatToStrF((float)par[18][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon18_12 -> Text = FloatToStrF((float)par[18][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon18_13 -> Text = FloatToStrF((float)par[18][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon18_14 -> Text = ( par[18][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//19 ������
EdtAKon19_0 -> Text = FloatToStrF((float)par[19][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon19_1 -> Text = FloatToStrF((float)par[19][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon19_2 -> Text = FloatToStrF((float)par[19][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon19_3 -> Text = FloatToStrF((float)par[19][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon19_4 -> Text = FloatToStrF((float)par[19][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon19_5 -> Text = FloatToStrF((float)par[19][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon19_7 -> Text = FloatToStrF((float)par[19][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon19_8 -> Text = FloatToStrF((float)par[19][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon19_9 -> Text = FloatToStrF((float)par[19][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon19_10 -> Text = FloatToStrF((float)par[19][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon19_11 -> Text = FloatToStrF((float)par[19][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon19_12 -> Text = FloatToStrF((float)par[19][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon19_13 -> Text = FloatToStrF((float)par[19][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon19_14 -> Text = ( par[19][14] ? "��" : "���" );                                    //� ���������?
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//20 ������
EdtAKon20_0 -> Text = FloatToStrF((float)par[20][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
EdtAKon20_1 -> Text = FloatToStrF((float)par[20][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
EdtAKon20_2 -> Text = FloatToStrF((float)par[20][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
EdtAKon20_3 -> Text = FloatToStrF((float)par[20][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
EdtAKon20_4 -> Text = FloatToStrF((float)par[20][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
EdtAKon20_5 -> Text = FloatToStrF((float)par[20][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
EdtAKon20_7 -> Text = FloatToStrF((float)par[20][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);     //��������
EdtAKon20_8 -> Text = FloatToStrF((float)par[20][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
EdtAKon20_9 -> Text = FloatToStrF((float)par[20][9]/4095.0*US_MAX, ffFixed, 5, 2);        //��������� ���� �����
EdtAKon20_10 -> Text = FloatToStrF((float)par[20][10]/4095.0*US_MAX, ffFixed, 5, 2);      //��������� ���� �����
EdtAKon20_11 -> Text = FloatToStrF((float)par[20][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
EdtAKon20_12 -> Text = FloatToStrF((float)par[20][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);// ��������
EdtAKon20_13 -> Text = FloatToStrF((float)par[20][13], ffFixed, 5, 0);                    //����� ��������
EdtAKon20_14 -> Text = ( par[20][14] ? "��" : "���" );                                    //� ���������?

//������ �� ���� �������� ������� �������� ��� �/� ��� ���������� ��������
    if (EdtARed1_12->Text!=0){EdtARed1_11->Text=0;EdtARed1_11->Enabled=false;}
    else {EdtARed1_11->Enabled=true;}
    if (EdtARed2_12->Text!=0){EdtARed2_11->Text=0;EdtARed2_11->Enabled=false;}
    else {EdtARed2_11->Enabled=true;}
    if (EdtARed3_12->Text!=0){EdtARed3_11->Text=0;EdtARed3_11->Enabled=false;}
    else {EdtARed3_11->Enabled=true;}
    if (EdtARed4_12->Text!=0){EdtARed4_11->Text=0;EdtARed4_11->Enabled=false;}
    else {EdtARed4_11->Enabled=true;}
    if (EdtARed5_12->Text!=0){EdtARed5_11->Text=0;EdtARed5_11->Enabled=false;}
    else {EdtARed5_11->Enabled=true;}
    if (EdtARed6_12->Text!=0){EdtARed6_11->Text=0;EdtARed6_11->Enabled=false;}
    else {EdtARed6_11->Enabled=true;}
    if (EdtARed7_12->Text!=0){EdtARed7_11->Text=0;EdtARed7_11->Enabled=false;}
    else {EdtARed7_11->Enabled=true;}
    if (EdtARed8_12->Text!=0){EdtARed8_11->Text=0;EdtARed8_11->Enabled=false;}
    else {EdtARed8_11->Enabled=true;}
    if (EdtARed9_12->Text!=0){EdtARed9_11->Text=0;EdtARed9_11->Enabled=false;}
    else {EdtARed9_11->Enabled=true;}
    if (EdtARed10_12->Text!=0){EdtARed10_11->Text=0;EdtARed10_11->Enabled=false;}
    else {EdtARed10_11->Enabled=true;}
    if (EdtARed11_12->Text!=0){EdtARed11_11->Text=0;EdtARed11_11->Enabled=false;}
    else {EdtARed11_11->Enabled=true;}
    if (EdtARed12_12->Text!=0){EdtARed12_11->Text=0;EdtARed12_11->Enabled=false;}
    else {EdtARed12_11->Enabled=true;}
    if (EdtARed13_12->Text!=0){EdtARed13_11->Text=0;EdtARed13_11->Enabled=false;}
    else {EdtARed13_11->Enabled=true;}
    if (EdtARed14_12->Text!=0){EdtARed14_11->Text=0;EdtARed14_11->Enabled=false;}
    else {EdtARed14_11->Enabled=true;}
    if (EdtARed15_12->Text!=0){EdtARed15_11->Text=0;EdtARed15_11->Enabled=false;}
    else {EdtARed15_11->Enabled=true;}
    if (EdtARed16_12->Text!=0){EdtARed16_11->Text=0;EdtARed16_11->Enabled=false;}
    else {EdtARed16_11->Enabled=true;}
    if (EdtARed17_12->Text!=0){EdtARed17_11->Text=0;EdtARed17_11->Enabled=false;}
    else {EdtARed17_11->Enabled=true;}
    if (EdtARed18_12->Text!=0){EdtARed18_11->Text=0;EdtARed18_11->Enabled=false;}
    else {EdtARed18_11->Enabled=true;}
    if (EdtARed19_12->Text!=0){EdtARed19_11->Text=0;EdtARed19_11->Enabled=false;}
    else {EdtARed19_11->Enabled=true;}
    if (EdtARed20_12->Text!=0){EdtARed20_11->Text=0;EdtARed20_11->Enabled=false;}
    else {EdtARed20_11->Enabled=true;}

    //������ �� ���� �������� ������� ��������� ��� �/� ��� ���������� ��������
    if (EdtARed1_11->Text!=0){EdtARed1_12->Text=0;EdtARed1_12->Enabled=false;}
    else {EdtARed1_12->Enabled=true;}
    if (EdtARed2_11->Text!=0){EdtARed2_12->Text=0;EdtARed2_12->Enabled=false;}
    else {EdtARed2_12->Enabled=true;}
    if (EdtARed3_11->Text!=0){EdtARed3_12->Text=0;EdtARed3_12->Enabled=false;}
    else {EdtARed3_12->Enabled=true;}
    if (EdtARed4_11->Text!=0){EdtARed4_12->Text=0;EdtARed4_12->Enabled=false;}
    else {EdtARed4_12->Enabled=true;}
    if (EdtARed5_11->Text!=0){EdtARed5_12->Text=0;EdtARed5_12->Enabled=false;}
    else {EdtARed5_12->Enabled=true;}
    if (EdtARed6_11->Text!=0){EdtARed6_12->Text=0;EdtARed6_12->Enabled=false;}
    else {EdtARed6_12->Enabled=true;}
    if (EdtARed7_11->Text!=0){EdtARed7_12->Text=0;EdtARed7_12->Enabled=false;}
    else {EdtARed7_12->Enabled=true;}
    if (EdtARed8_11->Text!=0){EdtARed8_12->Text=0;EdtARed8_12->Enabled=false;}
    else {EdtARed8_12->Enabled=true;}
    if (EdtARed9_11->Text!=0){EdtARed9_12->Text=0;EdtARed9_12->Enabled=false;}
    else {EdtARed9_12->Enabled=true;}
    if (EdtARed10_11->Text!=0){EdtARed10_12->Text=0;EdtARed10_12->Enabled=false;}
    else {EdtARed10_12->Enabled=true;}
    if (EdtARed11_11->Text!=0){EdtARed11_11->Text=0;EdtARed11_12->Enabled=false;}
    else {EdtARed11_12->Enabled=true;}
    if (EdtARed12_11->Text!=0){EdtARed12_12->Text=0;EdtARed12_12->Enabled=false;}
    else {EdtARed12_12->Enabled=true;}
    if (EdtARed13_11->Text!=0){EdtARed13_12->Text=0;EdtARed13_12->Enabled=false;}
    else {EdtARed13_12->Enabled=true;}
    if (EdtARed14_11->Text!=0){EdtARed14_12->Text=0;EdtARed14_12->Enabled=false;}
    else {EdtARed14_12->Enabled=true;}
    if (EdtARed15_11->Text!=0){EdtARed15_12->Text=0;EdtARed15_12->Enabled=false;}
    else {EdtARed15_12->Enabled=true;}
    if (EdtARed16_11->Text!=0){EdtARed16_12->Text=0;EdtARed16_12->Enabled=false;}
    else {EdtARed16_12->Enabled=true;}
    if (EdtARed17_11->Text!=0){EdtARed17_12->Text=0;EdtARed17_12->Enabled=false;}
    else {EdtARed17_12->Enabled=true;}
    if (EdtARed18_11->Text!=0){EdtARed18_12->Text=0;EdtARed18_12->Enabled=false;}
    else {EdtARed18_12->Enabled=true;}
    if (EdtARed19_11->Text!=0){EdtARed19_12->Text=0;EdtARed19_12->Enabled=false;}
    else {EdtARed19_12->Enabled=true;}
    if (EdtARed20_11->Text!=0){EdtARed20_12->Text=0;EdtARed20_12->Enabled=false;}
    else {EdtARed20_12->Enabled=true;}
}
//---------------------------------------------------------------------------
//--�������� ���������� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParAClick(TObject *Sender)
{
       PanelParA -> Visible = true;
}
//---------------------------------------------------------------------------
//--����� �� �������� ���������� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnAutoNetClick(TObject *Sender)
{
    PanelParA -> Visible = false;
}
//---------------------------------------------------------------------------
//--������������� �������� ���������� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnAutoDaClick(TObject *Sender)
{
    // ������ ������������� �������� ������
    PanelParA -> Visible = false;


//���������� �������
par[0][20] = StrToInt( EdtARed1_20->Text );
par[1][20] = StrToInt( EdtARed1_20->Text );
par[2][20] = StrToInt( EdtARed1_20->Text );
par[3][20] = StrToInt( EdtARed1_20->Text );
par[4][20] = StrToInt( EdtARed1_20->Text );
par[5][20] = StrToInt( EdtARed1_20->Text );
par[6][20] = StrToInt( EdtARed1_20->Text );
par[7][20] = StrToInt( EdtARed1_20->Text );
par[8][20] = StrToInt( EdtARed1_20->Text );
par[9][20] = StrToInt( EdtARed1_20->Text );
par[10][20] = StrToInt( EdtARed1_20->Text );
par[11][20] = StrToInt( EdtARed1_20->Text );
par[12][20] = StrToInt( EdtARed1_20->Text );
par[13][20] = StrToInt( EdtARed1_20->Text );
par[14][20] = StrToInt( EdtARed1_20->Text );
par[15][20] = StrToInt( EdtARed1_20->Text );
par[16][20] = StrToInt( EdtARed1_20->Text );
par[17][20] = StrToInt( EdtARed1_20->Text );
par[18][20] = StrToInt( EdtARed1_20->Text );
par[19][20] = StrToInt( EdtARed1_20->Text );

//1 ������
par[1][0] = StrToFloat( EdtARed1_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[1][1] = StrToFloat( EdtARed1_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[1][2] = StrToFloat( EdtARed1_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[1][3] = StrToFloat( EdtARed1_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[1][4] = StrToFloat( EdtARed1_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[1][5] = StrToFloat( EdtARed1_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[1][7] = StrToFloat( EdtARed1_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[1][8] = StrToFloat( EdtARed1_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[1][9] = StrToFloat(EdtARed1_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[1][10] = StrToFloat(EdtARed1_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[1][11] = StrToFloat( EdtARed1_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[1][12] = StrToFloat( EdtARed1_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[1][13] = StrToInt( EdtARed1_13->Text );                             //����� ��������
EdtARed1_14 -> Text == "��" ? par[1][14] = 1 : par[1][14] = 0;          //� ���������?

//2 ������
par[2][0] = StrToFloat( EdtARed2_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[2][1] = StrToFloat( EdtARed2_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[2][2] = StrToFloat( EdtARed2_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[2][3] = StrToFloat( EdtARed2_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[2][4] = StrToFloat( EdtARed2_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[2][5] = StrToFloat( EdtARed2_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[2][7] = StrToFloat( EdtARed2_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[2][8] = StrToFloat( EdtARed2_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[2][9] = StrToFloat(EdtARed2_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[2][10] = StrToFloat(EdtARed2_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[2][11] = StrToFloat( EdtARed2_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[2][12] = StrToFloat( EdtARed2_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[2][13] = StrToInt( EdtARed2_13->Text );                             //����� ��������
EdtARed2_14 -> Text == "��" ? par[2][14] = 1 : par[2][14] = 0;          //� ���������?

//3 ������
par[3][0] = StrToFloat( EdtARed3_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[3][1] = StrToFloat( EdtARed3_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[3][2] = StrToFloat( EdtARed3_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[3][3] = StrToFloat( EdtARed3_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[3][4] = StrToFloat( EdtARed3_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[3][5] = StrToFloat( EdtARed3_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[3][7] = StrToFloat( EdtARed3_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[3][8] = StrToFloat( EdtARed3_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[3][9] = StrToFloat(EdtARed3_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[3][10] = StrToFloat(EdtARed3_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[3][11] = StrToFloat( EdtARed3_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[3][12] = StrToFloat( EdtARed3_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[3][13] = StrToInt( EdtARed3_13->Text );                             //����� ��������
EdtARed3_14 -> Text == "��" ? par[3][14] = 1 : par[3][14] = 0;          //� ���������?

//4 ������
par[4][0] = StrToFloat( EdtARed4_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[4][1] = StrToFloat( EdtARed4_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[4][2] = StrToFloat( EdtARed4_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[4][3] = StrToFloat( EdtARed4_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[4][4] = StrToFloat( EdtARed4_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[4][5] = StrToFloat( EdtARed4_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[4][7] = StrToFloat( EdtARed4_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[4][8] = StrToFloat( EdtARed4_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[4][9] = StrToFloat(EdtARed4_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[4][10] = StrToFloat(EdtARed4_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[4][11] = StrToFloat( EdtARed4_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[4][12] = StrToFloat( EdtARed4_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[4][13] = StrToInt( EdtARed4_13->Text );                             //����� ��������
EdtARed4_14 -> Text == "��" ? par[4][14] = 1 : par[4][14] = 0;          //� ���������?

//5 ������
par[5][0] = StrToFloat( EdtARed5_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[5][1] = StrToFloat( EdtARed5_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[5][2] = StrToFloat( EdtARed5_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[5][3] = StrToFloat( EdtARed5_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[5][4] = StrToFloat( EdtARed5_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[5][5] = StrToFloat( EdtARed5_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[5][7] = StrToFloat( EdtARed5_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[5][8] = StrToFloat( EdtARed5_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[5][9] = StrToFloat(EdtARed5_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[5][10] = StrToFloat(EdtARed5_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[5][11] = StrToFloat( EdtARed5_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[5][12] = StrToFloat( EdtARed5_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[5][13] = StrToInt( EdtARed5_13->Text );                             //����� ��������
EdtARed5_14 -> Text == "��" ? par[5][14] = 1 : par[5][14] = 0;          //� ���������?

//6 ������
par[6][0] = StrToFloat( EdtARed6_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[6][1] = StrToFloat( EdtARed6_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[6][2] = StrToFloat( EdtARed6_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[6][3] = StrToFloat( EdtARed6_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[6][4] = StrToFloat( EdtARed6_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[6][5] = StrToFloat( EdtARed6_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[6][7] = StrToFloat( EdtARed6_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[6][8] = StrToFloat( EdtARed6_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[6][9] = StrToFloat(EdtARed6_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[6][10] = StrToFloat(EdtARed6_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[6][11] = StrToFloat( EdtARed6_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[6][12] = StrToFloat( EdtARed6_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[6][13] = StrToInt( EdtARed6_13->Text );                             //����� ��������
EdtARed6_14 -> Text == "��" ? par[6][14] = 1 : par[6][14] = 0;          //� ���������?

//7 ������
par[7][0] = StrToFloat( EdtARed7_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[7][1] = StrToFloat( EdtARed7_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[7][2] = StrToFloat( EdtARed7_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[7][3] = StrToFloat( EdtARed7_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[7][4] = StrToFloat( EdtARed7_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[7][5] = StrToFloat( EdtARed7_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[7][7] = StrToFloat( EdtARed7_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[7][8] = StrToFloat( EdtARed7_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[7][9] = StrToFloat(EdtARed7_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[7][10] = StrToFloat(EdtARed7_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[7][11] = StrToFloat( EdtARed7_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[7][12] = StrToFloat( EdtARed7_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[7][13] = StrToInt( EdtARed7_13->Text );                             //����� ��������
EdtARed7_14 -> Text == "��" ? par[7][14] = 1 : par[7][14] = 0;          //� ���������?

//8 ������
par[8][0] = StrToFloat( EdtARed8_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[8][1] = StrToFloat( EdtARed8_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[8][2] = StrToFloat( EdtARed8_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[8][3] = StrToFloat( EdtARed8_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[8][4] = StrToFloat( EdtARed8_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[8][5] = StrToFloat( EdtARed8_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[8][7] = StrToFloat( EdtARed8_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[8][8] = StrToFloat( EdtARed8_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[8][9] = StrToFloat(EdtARed8_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[8][10] = StrToFloat(EdtARed8_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[8][11] = StrToFloat( EdtARed8_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[8][12] = StrToFloat( EdtARed8_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[8][13] = StrToInt( EdtARed8_13->Text );                             //����� ��������
EdtARed8_14 -> Text == "��" ? par[8][14] = 1 : par[8][14] = 0;          //� ���������?

//9 ������
par[9][0] = StrToFloat( EdtARed9_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[9][1] = StrToFloat( EdtARed9_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[9][2] = StrToFloat( EdtARed9_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[9][3] = StrToFloat( EdtARed9_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[9][4] = StrToFloat( EdtARed9_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[9][5] = StrToFloat( EdtARed9_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[9][7] = StrToFloat( EdtARed9_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[9][8] = StrToFloat( EdtARed9_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[9][9] = StrToFloat(EdtARed9_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[9][10] = StrToFloat(EdtARed9_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[9][11] = StrToFloat( EdtARed9_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[9][12] = StrToFloat( EdtARed9_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[9][13] = StrToInt( EdtARed9_13->Text );                             //����� ��������
EdtARed9_14 -> Text == "��" ? par[9][14] = 1 : par[9][14] = 0;          //� ���������?

//10 ������
par[10][0] = StrToFloat( EdtARed10_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[10][1] = StrToFloat( EdtARed10_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[10][2] = StrToFloat( EdtARed10_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[10][3] = StrToFloat( EdtARed10_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[10][4] = StrToFloat( EdtARed10_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[10][5] = StrToFloat( EdtARed10_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[10][7] = StrToFloat( EdtARed10_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[10][8] = StrToFloat( EdtARed10_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[10][9] = StrToFloat(EdtARed10_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[10][10] = StrToFloat(EdtARed10_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[10][11] = StrToFloat( EdtARed10_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[10][12] = StrToFloat( EdtARed10_12->Text ) * 4095.0 / SMESH_MAX_USER; //��������
par[10][13] = StrToInt( EdtARed10_13->Text );                             //����� ��������
EdtARed10_14 -> Text == "��" ? par[10][14] = 1 : par[10][14] = 0;          //� ���������?

//11 ������
par[11][0] = StrToFloat( EdtARed11_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[11][1] = StrToFloat( EdtARed11_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[11][2] = StrToFloat( EdtARed11_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[11][3] = StrToFloat( EdtARed11_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[11][4] = StrToFloat( EdtARed11_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[11][5] = StrToFloat( EdtARed11_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[11][7] = StrToFloat( EdtARed11_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[11][8] = StrToFloat( EdtARed11_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[11][9] = StrToFloat(EdtARed11_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[11][10] = StrToFloat(EdtARed11_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[11][11] = StrToFloat( EdtARed11_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[11][12] = StrToFloat( EdtARed11_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[11][13] = StrToInt( EdtARed11_13->Text );                             //����� ��������
EdtARed11_14 -> Text == "��" ? par[11][14] = 1 : par[11][14] = 0;          //� ���������?

//12 ������
par[12][0] = StrToFloat( EdtARed12_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[12][1] = StrToFloat( EdtARed12_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[12][2] = StrToFloat( EdtARed12_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[12][3] = StrToFloat( EdtARed12_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[12][4] = StrToFloat( EdtARed12_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[12][5] = StrToFloat( EdtARed12_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[12][7] = StrToFloat( EdtARed12_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[12][8] = StrToFloat( EdtARed12_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[12][9] = StrToFloat(EdtARed12_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[12][10] = StrToFloat(EdtARed12_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[12][11] = StrToFloat( EdtARed12_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[12][12] = StrToFloat( EdtARed12_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[12][13] = StrToInt( EdtARed12_13->Text );                             //����� ��������
EdtARed12_14 -> Text == "��" ? par[12][14] = 1 : par[12][14] = 0;          //� ���������?

//13 ������
par[13][0] = StrToFloat( EdtARed13_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[13][1] = StrToFloat( EdtARed13_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[13][2] = StrToFloat( EdtARed13_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[13][3] = StrToFloat( EdtARed13_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[13][4] = StrToFloat( EdtARed13_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[13][5] = StrToFloat( EdtARed13_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[13][7] = StrToFloat( EdtARed13_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[13][8] = StrToFloat( EdtARed13_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[13][9] = StrToFloat(EdtARed13_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[13][10] = StrToFloat(EdtARed13_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[13][11] = StrToFloat( EdtARed13_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[13][12] = StrToFloat( EdtARed13_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[13][13] = StrToInt( EdtARed13_13->Text );                             //����� ��������
EdtARed13_14 -> Text == "��" ? par[13][14] = 1 : par[13][14] = 0;          //� ���������?

//14 ������
par[14][0] = StrToFloat( EdtARed14_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[14][1] = StrToFloat( EdtARed14_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[14][2] = StrToFloat( EdtARed14_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[14][3] = StrToFloat( EdtARed14_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[14][4] = StrToFloat( EdtARed14_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[14][5] = StrToFloat( EdtARed14_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[14][7] = StrToFloat( EdtARed14_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[14][8] = StrToFloat( EdtARed14_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[14][9] = StrToFloat(EdtARed14_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[14][10] = StrToFloat(EdtARed14_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[14][11] = StrToFloat( EdtARed14_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[14][12] = StrToFloat( EdtARed14_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[14][13] = StrToInt( EdtARed14_13->Text );                             //����� ��������
EdtARed14_14 -> Text == "��" ? par[14][14] = 1 : par[14][14] = 0;          //� ���������?

//15 ������
par[15][0] = StrToFloat( EdtARed15_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[15][1] = StrToFloat( EdtARed15_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[15][2] = StrToFloat( EdtARed15_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[15][3] = StrToFloat( EdtARed15_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[15][4] = StrToFloat( EdtARed15_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[15][5] = StrToFloat( EdtARed15_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[15][7] = StrToFloat( EdtARed15_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[15][8] = StrToFloat( EdtARed15_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[15][9] = StrToFloat(EdtARed15_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[15][10] = StrToFloat(EdtARed15_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[15][11] = StrToFloat( EdtARed15_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[15][12] = StrToFloat( EdtARed15_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[15][13] = StrToInt( EdtARed15_13->Text );                             //����� ��������
EdtARed15_14 -> Text == "��" ? par[15][14] = 1 : par[15][14] = 0;          //� ���������?

//16 ������
par[16][0] = StrToFloat( EdtARed16_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[16][1] = StrToFloat( EdtARed16_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[16][2] = StrToFloat( EdtARed16_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[16][3] = StrToFloat( EdtARed16_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[16][4] = StrToFloat( EdtARed16_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[16][5] = StrToFloat( EdtARed16_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[16][7] = StrToFloat( EdtARed16_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[16][8] = StrToFloat( EdtARed16_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[16][9] = StrToFloat(EdtARed16_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[16][10] = StrToFloat(EdtARed16_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[16][11] = StrToFloat( EdtARed16_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[16][12] = StrToFloat( EdtARed16_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[16][13] = StrToInt( EdtARed16_13->Text );                             //����� ��������
EdtARed16_14 -> Text == "��" ? par[16][14] = 1 : par[16][14] = 0;          //� ���������?

//17 ������
par[17][0] = StrToFloat( EdtARed17_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[17][1] = StrToFloat( EdtARed17_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[17][2] = StrToFloat( EdtARed17_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[17][3] = StrToFloat( EdtARed17_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[17][4] = StrToFloat( EdtARed17_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[17][5] = StrToFloat( EdtARed17_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[17][7] = StrToFloat( EdtARed17_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[17][8] = StrToFloat( EdtARed17_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[17][9] = StrToFloat(EdtARed17_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[17][10] = StrToFloat(EdtARed17_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[17][11] = StrToFloat( EdtARed17_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[17][12] = StrToFloat( EdtARed17_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[17][13] = StrToInt( EdtARed17_13->Text );                             //����� ��������
EdtARed17_14 -> Text == "��" ? par[17][14] = 1 : par[17][14] = 0;          //� ���������?

//18 ������
par[18][0] = StrToFloat( EdtARed18_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[18][1] = StrToFloat( EdtARed18_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[18][2] = StrToFloat( EdtARed18_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[18][3] = StrToFloat( EdtARed18_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[18][4] = StrToFloat( EdtARed18_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[18][5] = StrToFloat( EdtARed18_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[18][7] = StrToFloat( EdtARed18_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[18][8] = StrToFloat( EdtARed18_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[18][9] = StrToFloat(EdtARed18_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[18][10] = StrToFloat(EdtARed18_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[18][11] = StrToFloat( EdtARed18_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[18][12] = StrToFloat( EdtARed18_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[18][13] = StrToInt( EdtARed18_13->Text );                             //����� ��������
EdtARed18_14 -> Text == "��" ? par[18][14] = 1 : par[18][14] = 0;          //� ���������?

//19 ������
par[19][0] = StrToFloat( EdtARed19_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[19][1] = StrToFloat( EdtARed19_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[19][2] = StrToFloat( EdtARed19_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[19][3] = StrToFloat( EdtARed19_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[19][4] = StrToFloat( EdtARed19_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[19][5] = StrToFloat( EdtARed19_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[19][7] = StrToFloat( EdtARed19_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[19][8] = StrToFloat( EdtARed19_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[19][9] = StrToFloat(EdtARed19_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[19][10] = StrToFloat(EdtARed19_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[19][11] = StrToFloat( EdtARed19_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[19][12] = StrToFloat( EdtARed19_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[19][13] = StrToInt( EdtARed19_13->Text );                             //����� ��������
EdtARed19_14 -> Text == "��" ? par[19][14] = 1 : par[19][14] = 0;          //� ���������?

//20 ������
par[20][0] = StrToFloat( EdtARed20_0->Text ) / RRG1_MAX * 4095.0;         //������ ���1
par[20][1] = StrToFloat( EdtARed20_1->Text ) / RRG2_MAX * 4095.0;         //������ ���2
par[20][2] = StrToFloat( EdtARed20_2->Text ) / RRG3_MAX * 4095.0;         //������ ���3
par[20][3] = StrToFloat( EdtARed20_3->Text ) / RRG4_MAX * 4095.0;         //������ ���4
par[20][4] = StrToFloat( EdtARed20_4->Text ) / RRG5_MAX * 4095.0;         //������ ���5
par[20][5] = StrToFloat( EdtARed20_5->Text ) / RRG6_MAX * 4095.0;         //������ ���6
par[20][7] = StrToFloat( EdtARed20_7->Text ) / DAVL_MAX * 10000.0;        //��������
par[20][8] = StrToFloat( EdtARed20_8->Text ) * 4095.0 / CESAR_MAX_IP;     //�������� ��� ��
par[20][9] = StrToFloat(EdtARed20_9 ->Text)*4095.0/US_MAX;                //��������� ���� �����
par[20][10] = StrToFloat(EdtARed20_10 ->Text)*4095.0/US_MAX;              //��������� ���� �����
par[20][11] = StrToFloat( EdtARed20_11->Text ) * 4095.0 / CESAR_MAX_PD;   //�������� ��� �/�
par[20][12] = StrToFloat( EdtARed20_12->Text ) * 4095.0 / SMESH_MAX_USER; //�������� ��� �/�
par[20][13] = StrToInt( EdtARed20_13->Text );                             //����� ��������
EdtARed20_14 -> Text == "��" ? par[20][14] = 1 : par[20][14] = 0;          //� ���������?


    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ��������� �������������� ������:");

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("1 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //1 ������
    if ( EdtAKon1_0 -> Text != EdtARed1_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon1_0 -> Text + " -> " + EdtARed1_0 -> Text );
    if ( EdtAKon1_1 -> Text != EdtARed1_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon1_1 -> Text + " -> " + EdtARed1_1 -> Text );
    if ( EdtAKon1_2 -> Text != EdtARed1_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon1_2 -> Text + " -> " + EdtARed1_2 -> Text );
    if ( EdtAKon1_3 -> Text != EdtARed1_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon1_3 -> Text + " -> " + EdtARed1_3 -> Text );
    if ( EdtAKon1_4 -> Text != EdtARed1_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon1_4 -> Text + " -> " + EdtARed1_4 -> Text );
    if ( EdtAKon1_5 -> Text != EdtARed1_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon1_5 -> Text + " -> " + EdtARed1_5 -> Text );
    if ( EdtAKon1_7 -> Text != EdtARed1_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon1_7 -> Text + " -> " + EdtARed1_7 -> Text );
    if ( EdtAKon1_8 -> Text != EdtARed1_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon1_8 -> Text + " -> " + EdtARed1_8 -> Text );
    if ( EdtAKon1_9 -> Text != EdtARed1_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon1_9 -> Text + " -> " + EdtARed1_9 -> Text );
    if ( EdtAKon1_10 -> Text != EdtARed1_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon1_10 -> Text + " -> " + EdtARed1_10 -> Text );
    if ( EdtAKon1_11 -> Text != EdtARed1_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon1_11 -> Text + " -> " + EdtARed1_11 -> Text );
    if ( EdtAKon1_12 -> Text != EdtARed1_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon1_12 -> Text + " -> " + EdtARed1_12 -> Text );
    if ( EdtAKon1_13 -> Text != EdtARed1_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon1_13 -> Text + " -> " + EdtARed1_13 -> Text );
    if ( EdtAKon1_14 -> Text != EdtARed1_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon1_14 -> Text + " -> " + EdtARed1_14 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("2 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //2 ������
    if ( EdtAKon2_0 -> Text != EdtARed2_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon2_0 -> Text + " -> " + EdtARed2_0 -> Text );
    if ( EdtAKon2_1 -> Text != EdtARed2_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon2_1 -> Text + " -> " + EdtARed2_1 -> Text );
    if ( EdtAKon2_2 -> Text != EdtARed2_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon2_2 -> Text + " -> " + EdtARed2_2 -> Text );
    if ( EdtAKon2_3 -> Text != EdtARed2_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon2_3 -> Text + " -> " + EdtARed2_3 -> Text );
    if ( EdtAKon2_4 -> Text != EdtARed2_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon2_4 -> Text + " -> " + EdtARed2_4 -> Text );
    if ( EdtAKon2_5 -> Text != EdtARed2_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon2_5 -> Text + " -> " + EdtARed2_5 -> Text );
    if ( EdtAKon2_7 -> Text != EdtARed2_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon2_7 -> Text + " -> " + EdtARed2_7 -> Text );
    if ( EdtAKon2_8 -> Text != EdtARed2_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon2_8 -> Text + " -> " + EdtARed2_8 -> Text );
    if ( EdtAKon2_9 -> Text != EdtARed2_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon2_9 -> Text + " -> " + EdtARed2_9 -> Text );
    if ( EdtAKon2_10 -> Text != EdtARed2_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon2_10 -> Text + " -> " + EdtARed2_10 -> Text );
    if ( EdtAKon2_11 -> Text != EdtARed2_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon2_11 -> Text + " -> " + EdtARed2_11 -> Text );
    if ( EdtAKon2_12 -> Text != EdtARed2_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon2_12 -> Text + " -> " + EdtARed2_12 -> Text );
    if ( EdtAKon2_13 -> Text != EdtARed2_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon2_13 -> Text + " -> " + EdtARed2_13 -> Text );
    if ( EdtAKon2_14 -> Text != EdtARed2_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon2_14 -> Text + " -> " + EdtARed2_14 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("3 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //3 ������
    if ( EdtAKon3_0 -> Text != EdtARed3_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon3_0 -> Text + " -> " + EdtARed3_0 -> Text );
    if ( EdtAKon3_1 -> Text != EdtARed3_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon3_1 -> Text + " -> " + EdtARed3_1 -> Text );
    if ( EdtAKon3_2 -> Text != EdtARed3_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon3_2 -> Text + " -> " + EdtARed3_2 -> Text );
    if ( EdtAKon3_3 -> Text != EdtARed3_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon3_3 -> Text + " -> " + EdtARed3_3 -> Text );
    if ( EdtAKon3_4 -> Text != EdtARed3_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon3_4 -> Text + " -> " + EdtARed3_4 -> Text );
    if ( EdtAKon3_5 -> Text != EdtARed3_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon3_5 -> Text + " -> " + EdtARed3_5 -> Text );
    if ( EdtAKon3_7 -> Text != EdtARed3_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon3_7 -> Text + " -> " + EdtARed3_7 -> Text );
    if ( EdtAKon3_8 -> Text != EdtARed3_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon3_8 -> Text + " -> " + EdtARed3_8 -> Text );
    if ( EdtAKon3_9 -> Text != EdtARed3_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon3_9 -> Text + " -> " + EdtARed3_9 -> Text );
    if ( EdtAKon3_10 -> Text != EdtARed3_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon3_10 -> Text + " -> " + EdtARed3_10 -> Text );
    if ( EdtAKon3_11 -> Text != EdtARed3_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon3_11 -> Text + " -> " + EdtARed3_11 -> Text );
    if ( EdtAKon3_12 -> Text != EdtARed3_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon3_12 -> Text + " -> " + EdtARed3_12 -> Text );
    if ( EdtAKon3_13 -> Text != EdtARed3_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon3_13 -> Text + " -> " + EdtARed3_13 -> Text );
    if ( EdtAKon3_14 -> Text != EdtARed3_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon3_14 -> Text + " -> " + EdtARed3_14 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("4 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //4 ������
    if ( EdtAKon4_0 -> Text != EdtARed4_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon4_0 -> Text + " -> " + EdtARed4_0 -> Text );
    if ( EdtAKon4_1 -> Text != EdtARed4_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon4_1 -> Text + " -> " + EdtARed4_1 -> Text );
    if ( EdtAKon4_2 -> Text != EdtARed4_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon4_2 -> Text + " -> " + EdtARed4_2 -> Text );
    if ( EdtAKon4_3 -> Text != EdtARed4_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon4_3 -> Text + " -> " + EdtARed4_3 -> Text );
    if ( EdtAKon4_4 -> Text != EdtARed4_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon4_4 -> Text + " -> " + EdtARed4_4 -> Text );
    if ( EdtAKon4_5 -> Text != EdtARed4_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon4_5 -> Text + " -> " + EdtARed4_5 -> Text );
    if ( EdtAKon4_7 -> Text != EdtARed4_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon4_7 -> Text + " -> " + EdtARed4_7 -> Text );
    if ( EdtAKon4_8 -> Text != EdtARed4_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon4_8 -> Text + " -> " + EdtARed4_8 -> Text );
    if ( EdtAKon4_9 -> Text != EdtARed4_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon4_9 -> Text + " -> " + EdtARed4_9 -> Text );
    if ( EdtAKon4_10 -> Text != EdtARed4_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon4_10 -> Text + " -> " + EdtARed4_10 -> Text );
    if ( EdtAKon4_11 -> Text != EdtARed4_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon4_11 -> Text + " -> " + EdtARed4_11 -> Text );
    if ( EdtAKon4_12 -> Text != EdtARed4_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon4_12 -> Text + " -> " + EdtARed4_12 -> Text );
    if ( EdtAKon4_13 -> Text != EdtARed4_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon4_13 -> Text + " -> " + EdtARed4_13 -> Text );
    if ( EdtAKon4_14 -> Text != EdtARed4_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon4_14 -> Text + " -> " + EdtARed4_14 -> Text );
     
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("5 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //5 ������
    if ( EdtAKon5_0 -> Text != EdtARed5_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon5_0 -> Text + " -> " + EdtARed5_0 -> Text );
    if ( EdtAKon5_1 -> Text != EdtARed5_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon5_1 -> Text + " -> " + EdtARed5_1 -> Text );
    if ( EdtAKon5_2 -> Text != EdtARed5_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon5_2 -> Text + " -> " + EdtARed5_2 -> Text );
    if ( EdtAKon5_3 -> Text != EdtARed5_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon5_3 -> Text + " -> " + EdtARed5_3 -> Text );
    if ( EdtAKon5_4 -> Text != EdtARed5_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon5_4 -> Text + " -> " + EdtARed5_4 -> Text );
    if ( EdtAKon5_5 -> Text != EdtARed5_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon5_5 -> Text + " -> " + EdtARed5_5 -> Text );
    if ( EdtAKon5_7 -> Text != EdtARed5_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon5_7 -> Text + " -> " + EdtARed5_7 -> Text );
    if ( EdtAKon5_8 -> Text != EdtARed5_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon5_8 -> Text + " -> " + EdtARed5_8 -> Text );
    if ( EdtAKon5_9 -> Text != EdtARed5_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon5_9 -> Text + " -> " + EdtARed5_9 -> Text );
    if ( EdtAKon5_10 -> Text != EdtARed5_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon5_10 -> Text + " -> " + EdtARed5_10 -> Text );
    if ( EdtAKon5_11 -> Text != EdtARed5_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon5_11 -> Text + " -> " + EdtARed5_11 -> Text );
    if ( EdtAKon5_12 -> Text != EdtARed5_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon5_12 -> Text + " -> " + EdtARed5_12 -> Text );
    if ( EdtAKon5_13 -> Text != EdtARed5_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon5_13 -> Text + " -> " + EdtARed5_13 -> Text );
    if ( EdtAKon5_14 -> Text != EdtARed5_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon5_14 -> Text + " -> " + EdtARed5_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("6 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //6 ������
    if ( EdtAKon6_0 -> Text != EdtARed6_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon6_0 -> Text + " -> " + EdtARed6_0 -> Text );
    if ( EdtAKon6_1 -> Text != EdtARed6_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon6_1 -> Text + " -> " + EdtARed6_1 -> Text );
    if ( EdtAKon6_2 -> Text != EdtARed6_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon6_2 -> Text + " -> " + EdtARed6_2 -> Text );
    if ( EdtAKon6_3 -> Text != EdtARed6_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon6_3 -> Text + " -> " + EdtARed6_3 -> Text );
    if ( EdtAKon6_4 -> Text != EdtARed6_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon6_4 -> Text + " -> " + EdtARed6_4 -> Text );
    if ( EdtAKon6_5 -> Text != EdtARed6_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon6_5 -> Text + " -> " + EdtARed6_5 -> Text );
    if ( EdtAKon6_7 -> Text != EdtARed6_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon6_7 -> Text + " -> " + EdtARed6_7 -> Text );
    if ( EdtAKon6_8 -> Text != EdtARed6_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon6_8 -> Text + " -> " + EdtARed6_8 -> Text );
    if ( EdtAKon6_9 -> Text != EdtARed6_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon6_9 -> Text + " -> " + EdtARed6_9 -> Text );
    if ( EdtAKon6_10 -> Text != EdtARed6_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon6_10 -> Text + " -> " + EdtARed6_10 -> Text );
    if ( EdtAKon6_11 -> Text != EdtARed6_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon6_11 -> Text + " -> " + EdtARed6_11 -> Text );
    if ( EdtAKon6_12 -> Text != EdtARed6_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon6_12 -> Text + " -> " + EdtARed6_12 -> Text );
    if ( EdtAKon6_13 -> Text != EdtARed6_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon6_13 -> Text + " -> " + EdtARed6_13 -> Text );
    if ( EdtAKon6_14 -> Text != EdtARed6_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon6_14 -> Text + " -> " + EdtARed6_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("7 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //7 ������
    if ( EdtAKon7_0 -> Text != EdtARed7_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon7_0 -> Text + " -> " + EdtARed7_0 -> Text );
    if ( EdtAKon7_1 -> Text != EdtARed7_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon7_1 -> Text + " -> " + EdtARed7_1 -> Text );
    if ( EdtAKon7_2 -> Text != EdtARed7_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon7_2 -> Text + " -> " + EdtARed7_2 -> Text );
    if ( EdtAKon7_3 -> Text != EdtARed7_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon7_3 -> Text + " -> " + EdtARed7_3 -> Text );
    if ( EdtAKon7_4 -> Text != EdtARed7_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon7_4 -> Text + " -> " + EdtARed7_4 -> Text );
    if ( EdtAKon7_5 -> Text != EdtARed7_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon7_5 -> Text + " -> " + EdtARed7_5 -> Text );
    if ( EdtAKon7_7 -> Text != EdtARed7_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon7_7 -> Text + " -> " + EdtARed7_7 -> Text );
    if ( EdtAKon7_8 -> Text != EdtARed7_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon7_8 -> Text + " -> " + EdtARed7_8 -> Text );
    if ( EdtAKon7_9 -> Text != EdtARed7_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon7_9 -> Text + " -> " + EdtARed7_9 -> Text );
    if ( EdtAKon7_10 -> Text != EdtARed7_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon7_10 -> Text + " -> " + EdtARed7_10 -> Text );
    if ( EdtAKon7_11 -> Text != EdtARed7_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon7_11 -> Text + " -> " + EdtARed7_11 -> Text );
    if ( EdtAKon7_12 -> Text != EdtARed7_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon7_12 -> Text + " -> " + EdtARed7_12 -> Text );
    if ( EdtAKon7_13 -> Text != EdtARed7_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon7_13 -> Text + " -> " + EdtARed7_13 -> Text );
    if ( EdtAKon7_14 -> Text != EdtARed7_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon7_14 -> Text + " -> " + EdtARed7_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("8 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //8 ������
    if ( EdtAKon8_0 -> Text != EdtARed8_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon8_0 -> Text + " -> " + EdtARed8_0 -> Text );
    if ( EdtAKon8_1 -> Text != EdtARed8_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon8_1 -> Text + " -> " + EdtARed8_1 -> Text );
    if ( EdtAKon8_2 -> Text != EdtARed8_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon8_2 -> Text + " -> " + EdtARed8_2 -> Text );
    if ( EdtAKon8_3 -> Text != EdtARed8_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon8_3 -> Text + " -> " + EdtARed8_3 -> Text );
    if ( EdtAKon8_4 -> Text != EdtARed8_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon8_4 -> Text + " -> " + EdtARed8_4 -> Text );
    if ( EdtAKon8_5 -> Text != EdtARed8_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon8_5 -> Text + " -> " + EdtARed8_5 -> Text );
    if ( EdtAKon8_7 -> Text != EdtARed8_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon8_7 -> Text + " -> " + EdtARed8_7 -> Text );
    if ( EdtAKon8_8 -> Text != EdtARed8_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon8_8 -> Text + " -> " + EdtARed8_8 -> Text );
    if ( EdtAKon8_9 -> Text != EdtARed8_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon8_9 -> Text + " -> " + EdtARed8_9 -> Text );
    if ( EdtAKon8_10 -> Text != EdtARed8_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon8_10 -> Text + " -> " + EdtARed8_10 -> Text );
    if ( EdtAKon8_11 -> Text != EdtARed8_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon8_11 -> Text + " -> " + EdtARed8_11 -> Text );
    if ( EdtAKon8_12 -> Text != EdtARed8_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon8_12 -> Text + " -> " + EdtARed8_12 -> Text );
    if ( EdtAKon8_13 -> Text != EdtARed8_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon8_13 -> Text + " -> " + EdtARed8_13 -> Text );
    if ( EdtAKon8_14 -> Text != EdtARed8_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon8_14 -> Text + " -> " + EdtARed8_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("9 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //9 ������
    if ( EdtAKon9_0 -> Text != EdtARed9_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon9_0 -> Text + " -> " + EdtARed9_0 -> Text );
    if ( EdtAKon9_1 -> Text != EdtARed9_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon9_1 -> Text + " -> " + EdtARed9_1 -> Text );
    if ( EdtAKon9_2 -> Text != EdtARed9_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon9_2 -> Text + " -> " + EdtARed9_2 -> Text );
    if ( EdtAKon9_3 -> Text != EdtARed9_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon9_3 -> Text + " -> " + EdtARed9_3 -> Text );
    if ( EdtAKon9_4 -> Text != EdtARed9_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon9_4 -> Text + " -> " + EdtARed9_4 -> Text );
    if ( EdtAKon9_5 -> Text != EdtARed9_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon9_5 -> Text + " -> " + EdtARed9_5 -> Text );
    if ( EdtAKon9_7 -> Text != EdtARed9_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon9_7 -> Text + " -> " + EdtARed9_7 -> Text );
    if ( EdtAKon9_8 -> Text != EdtARed9_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon9_8 -> Text + " -> " + EdtARed9_8 -> Text );
    if ( EdtAKon9_9 -> Text != EdtARed9_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon9_9 -> Text + " -> " + EdtARed9_9 -> Text );
    if ( EdtAKon9_10 -> Text != EdtARed9_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon9_10 -> Text + " -> " + EdtARed9_10 -> Text );
    if ( EdtAKon9_11 -> Text != EdtARed9_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon9_11 -> Text + " -> " + EdtARed9_11 -> Text );
    if ( EdtAKon9_12 -> Text != EdtARed9_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon9_12 -> Text + " -> " + EdtARed9_12 -> Text );
    if ( EdtAKon9_13 -> Text != EdtARed9_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon9_13 -> Text + " -> " + EdtARed9_13 -> Text );
    if ( EdtAKon9_14 -> Text != EdtARed9_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon9_14 -> Text + " -> " + EdtARed9_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("10 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //10 ������
    if ( EdtAKon10_0 -> Text != EdtARed10_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon10_0 -> Text + " -> " + EdtARed10_0 -> Text );
    if ( EdtAKon10_1 -> Text != EdtARed10_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon10_1 -> Text + " -> " + EdtARed10_1 -> Text );
    if ( EdtAKon10_2 -> Text != EdtARed10_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon10_2 -> Text + " -> " + EdtARed10_2 -> Text );
    if ( EdtAKon10_3 -> Text != EdtARed10_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon10_3 -> Text + " -> " + EdtARed10_3 -> Text );
    if ( EdtAKon10_4 -> Text != EdtARed10_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon10_4 -> Text + " -> " + EdtARed10_4 -> Text );
    if ( EdtAKon10_5 -> Text != EdtARed10_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon10_5 -> Text + " -> " + EdtARed10_5 -> Text );
    if ( EdtAKon10_7 -> Text != EdtARed10_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon10_7 -> Text + " -> " + EdtARed10_7 -> Text );
    if ( EdtAKon10_8 -> Text != EdtARed10_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon10_8 -> Text + " -> " + EdtARed10_8 -> Text );
    if ( EdtAKon10_9 -> Text != EdtARed10_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon10_9 -> Text + " -> " + EdtARed10_9 -> Text );
    if ( EdtAKon10_10 -> Text != EdtARed10_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon10_10 -> Text + " -> " + EdtARed10_10 -> Text );
    if ( EdtAKon10_11 -> Text != EdtARed10_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon10_11 -> Text + " -> " + EdtARed10_11 -> Text );
    if ( EdtAKon10_12 -> Text != EdtARed10_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon10_12 -> Text + " -> " + EdtARed10_12 -> Text );
    if ( EdtAKon10_13 -> Text != EdtARed10_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon10_13 -> Text + " -> " + EdtARed10_13 -> Text );
    if ( EdtAKon10_14 -> Text != EdtARed10_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon10_14 -> Text + " -> " + EdtARed10_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("11 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //11 ������
    if ( EdtAKon11_0 -> Text != EdtARed11_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon11_0 -> Text + " -> " + EdtARed11_0 -> Text );
    if ( EdtAKon11_1 -> Text != EdtARed11_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon11_1 -> Text + " -> " + EdtARed11_1 -> Text );
    if ( EdtAKon11_2 -> Text != EdtARed11_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon11_2 -> Text + " -> " + EdtARed11_2 -> Text );
    if ( EdtAKon11_3 -> Text != EdtARed11_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon11_3 -> Text + " -> " + EdtARed11_3 -> Text );
    if ( EdtAKon11_4 -> Text != EdtARed11_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon11_4 -> Text + " -> " + EdtARed11_4 -> Text );
    if ( EdtAKon11_5 -> Text != EdtARed11_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon11_5 -> Text + " -> " + EdtARed11_5 -> Text );
    if ( EdtAKon11_7 -> Text != EdtARed11_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon11_7 -> Text + " -> " + EdtARed11_7 -> Text );
    if ( EdtAKon11_8 -> Text != EdtARed11_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon11_8 -> Text + " -> " + EdtARed11_8 -> Text );
    if ( EdtAKon11_9 -> Text != EdtARed11_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon11_9 -> Text + " -> " + EdtARed11_9 -> Text );
    if ( EdtAKon11_10 -> Text != EdtARed11_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon11_10 -> Text + " -> " + EdtARed11_10 -> Text );
    if ( EdtAKon11_11 -> Text != EdtARed11_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon11_11 -> Text + " -> " + EdtARed11_11 -> Text );
    if ( EdtAKon11_12 -> Text != EdtARed11_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon11_12 -> Text + " -> " + EdtARed11_12 -> Text );
    if ( EdtAKon11_13 -> Text != EdtARed11_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon11_13 -> Text + " -> " + EdtARed11_13 -> Text );
    if ( EdtAKon11_14 -> Text != EdtARed11_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon11_14 -> Text + " -> " + EdtARed11_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("12 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //12 ������
    if ( EdtAKon12_0 -> Text != EdtARed12_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon12_0 -> Text + " -> " + EdtARed12_0 -> Text );
    if ( EdtAKon12_1 -> Text != EdtARed12_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon12_1 -> Text + " -> " + EdtARed12_1 -> Text );
    if ( EdtAKon12_2 -> Text != EdtARed12_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon12_2 -> Text + " -> " + EdtARed12_2 -> Text );
    if ( EdtAKon12_3 -> Text != EdtARed12_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon12_3 -> Text + " -> " + EdtARed12_3 -> Text );
    if ( EdtAKon12_4 -> Text != EdtARed12_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon12_4 -> Text + " -> " + EdtARed12_4 -> Text );
    if ( EdtAKon12_5 -> Text != EdtARed12_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon12_5 -> Text + " -> " + EdtARed12_5 -> Text );
    if ( EdtAKon12_7 -> Text != EdtARed12_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon12_7 -> Text + " -> " + EdtARed12_7 -> Text );
    if ( EdtAKon12_8 -> Text != EdtARed12_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon12_8 -> Text + " -> " + EdtARed12_8 -> Text );
    if ( EdtAKon12_9 -> Text != EdtARed12_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon12_9 -> Text + " -> " + EdtARed12_9 -> Text );
    if ( EdtAKon12_10 -> Text != EdtARed12_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon12_10 -> Text + " -> " + EdtARed12_10 -> Text );
    if ( EdtAKon12_11 -> Text != EdtARed12_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon12_11 -> Text + " -> " + EdtARed12_11 -> Text );
    if ( EdtAKon12_12 -> Text != EdtARed12_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon12_12 -> Text + " -> " + EdtARed12_12 -> Text );
    if ( EdtAKon12_13 -> Text != EdtARed12_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon12_13 -> Text + " -> " + EdtARed12_13 -> Text );
    if ( EdtAKon12_14 -> Text != EdtARed12_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon12_14 -> Text + " -> " + EdtARed12_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("13 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //13 ������
    if ( EdtAKon13_0 -> Text != EdtARed13_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon13_0 -> Text + " -> " + EdtARed13_0 -> Text );
    if ( EdtAKon13_1 -> Text != EdtARed13_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon13_1 -> Text + " -> " + EdtARed13_1 -> Text );
    if ( EdtAKon13_2 -> Text != EdtARed13_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon13_2 -> Text + " -> " + EdtARed13_2 -> Text );
    if ( EdtAKon13_3 -> Text != EdtARed13_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon13_3 -> Text + " -> " + EdtARed13_3 -> Text );
    if ( EdtAKon13_4 -> Text != EdtARed13_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon13_4 -> Text + " -> " + EdtARed13_4 -> Text );
    if ( EdtAKon13_5 -> Text != EdtARed13_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon13_5 -> Text + " -> " + EdtARed13_5 -> Text );
    if ( EdtAKon13_7 -> Text != EdtARed13_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon13_7 -> Text + " -> " + EdtARed13_7 -> Text );
    if ( EdtAKon13_8 -> Text != EdtARed13_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon13_8 -> Text + " -> " + EdtARed13_8 -> Text );
    if ( EdtAKon13_9 -> Text != EdtARed13_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon13_9 -> Text + " -> " + EdtARed13_9 -> Text );
    if ( EdtAKon13_10 -> Text != EdtARed13_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon13_10 -> Text + " -> " + EdtARed13_10 -> Text );
    if ( EdtAKon13_11 -> Text != EdtARed13_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon13_11 -> Text + " -> " + EdtARed13_11 -> Text );
    if ( EdtAKon13_12 -> Text != EdtARed13_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon13_12 -> Text + " -> " + EdtARed13_12 -> Text );
    if ( EdtAKon13_13 -> Text != EdtARed13_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon13_13 -> Text + " -> " + EdtARed13_13 -> Text );
    if ( EdtAKon13_14 -> Text != EdtARed13_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon13_14 -> Text + " -> " + EdtARed13_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("14 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //14 ������
    if ( EdtAKon14_0 -> Text != EdtARed14_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon14_0 -> Text + " -> " + EdtARed14_0 -> Text );
    if ( EdtAKon14_1 -> Text != EdtARed14_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon14_1 -> Text + " -> " + EdtARed14_1 -> Text );
    if ( EdtAKon14_2 -> Text != EdtARed14_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon14_2 -> Text + " -> " + EdtARed14_2 -> Text );
    if ( EdtAKon14_3 -> Text != EdtARed14_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon14_3 -> Text + " -> " + EdtARed14_3 -> Text );
    if ( EdtAKon14_4 -> Text != EdtARed14_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon14_4 -> Text + " -> " + EdtARed14_4 -> Text );
    if ( EdtAKon14_5 -> Text != EdtARed14_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon14_5 -> Text + " -> " + EdtARed14_5 -> Text );
    if ( EdtAKon14_7 -> Text != EdtARed14_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon14_7 -> Text + " -> " + EdtARed14_7 -> Text );
    if ( EdtAKon14_8 -> Text != EdtARed14_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon14_8 -> Text + " -> " + EdtARed14_8 -> Text );
    if ( EdtAKon14_9 -> Text != EdtARed14_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon14_9 -> Text + " -> " + EdtARed14_9 -> Text );
    if ( EdtAKon14_10 -> Text != EdtARed14_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon14_10 -> Text + " -> " + EdtARed14_10 -> Text );
    if ( EdtAKon14_11 -> Text != EdtARed14_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon14_11 -> Text + " -> " + EdtARed14_11 -> Text );
    if ( EdtAKon14_12 -> Text != EdtARed14_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon14_12 -> Text + " -> " + EdtARed14_12 -> Text );
    if ( EdtAKon14_13 -> Text != EdtARed14_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon14_13 -> Text + " -> " + EdtARed14_13 -> Text );
    if ( EdtAKon14_14 -> Text != EdtARed14_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon14_14 -> Text + " -> " + EdtARed14_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("15 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //15 ������
    if ( EdtAKon15_0 -> Text != EdtARed15_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon15_0 -> Text + " -> " + EdtARed15_0 -> Text );
    if ( EdtAKon15_1 -> Text != EdtARed15_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon15_1 -> Text + " -> " + EdtARed15_1 -> Text );
    if ( EdtAKon15_2 -> Text != EdtARed15_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon15_2 -> Text + " -> " + EdtARed15_2 -> Text );
    if ( EdtAKon15_3 -> Text != EdtARed15_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon15_3 -> Text + " -> " + EdtARed15_3 -> Text );
    if ( EdtAKon15_4 -> Text != EdtARed15_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon15_4 -> Text + " -> " + EdtARed15_4 -> Text );
    if ( EdtAKon15_5 -> Text != EdtARed15_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon15_5 -> Text + " -> " + EdtARed15_5 -> Text );
    if ( EdtAKon15_7 -> Text != EdtARed15_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon15_7 -> Text + " -> " + EdtARed15_7 -> Text );
    if ( EdtAKon15_8 -> Text != EdtARed15_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon15_8 -> Text + " -> " + EdtARed15_8 -> Text );
    if ( EdtAKon15_9 -> Text != EdtARed15_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon15_9 -> Text + " -> " + EdtARed15_9 -> Text );
    if ( EdtAKon15_10 -> Text != EdtARed15_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon15_10 -> Text + " -> " + EdtARed15_10 -> Text );
    if ( EdtAKon15_11 -> Text != EdtARed15_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon15_11 -> Text + " -> " + EdtARed15_11 -> Text );
    if ( EdtAKon15_12 -> Text != EdtARed15_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon15_12 -> Text + " -> " + EdtARed15_12 -> Text );
    if ( EdtAKon15_13 -> Text != EdtARed15_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon15_13 -> Text + " -> " + EdtARed15_13 -> Text );
    if ( EdtAKon15_14 -> Text != EdtARed15_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon15_14 -> Text + " -> " + EdtARed15_14 -> Text );
   
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("16 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //16 ������
    if ( EdtAKon16_0 -> Text != EdtARed16_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon16_0 -> Text + " -> " + EdtARed16_0 -> Text );
    if ( EdtAKon16_1 -> Text != EdtARed16_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon16_1 -> Text + " -> " + EdtARed16_1 -> Text );
    if ( EdtAKon16_2 -> Text != EdtARed16_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon16_2 -> Text + " -> " + EdtARed16_2 -> Text );
    if ( EdtAKon16_3 -> Text != EdtARed16_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon16_3 -> Text + " -> " + EdtARed16_3 -> Text );
    if ( EdtAKon16_4 -> Text != EdtARed16_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon16_4 -> Text + " -> " + EdtARed16_4 -> Text );
    if ( EdtAKon16_5 -> Text != EdtARed16_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon16_5 -> Text + " -> " + EdtARed16_5 -> Text );
    if ( EdtAKon16_7 -> Text != EdtARed16_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon16_7 -> Text + " -> " + EdtARed16_7 -> Text );
    if ( EdtAKon16_8 -> Text != EdtARed16_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon16_8 -> Text + " -> " + EdtARed16_8 -> Text );
    if ( EdtAKon16_9 -> Text != EdtARed16_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon16_9 -> Text + " -> " + EdtARed16_9 -> Text );
    if ( EdtAKon16_10 -> Text != EdtARed16_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon16_10 -> Text + " -> " + EdtARed16_10 -> Text );
    if ( EdtAKon16_11 -> Text != EdtARed16_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon16_11 -> Text + " -> " + EdtARed16_11 -> Text );
    if ( EdtAKon16_12 -> Text != EdtARed16_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon16_12 -> Text + " -> " + EdtARed16_12 -> Text );
    if ( EdtAKon16_13 -> Text != EdtARed16_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon16_13 -> Text + " -> " + EdtARed16_13 -> Text );
    if ( EdtAKon16_14 -> Text != EdtARed16_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon16_14 -> Text + " -> " + EdtARed16_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("17 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //17 ������
    if ( EdtAKon17_0 -> Text != EdtARed17_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon17_0 -> Text + " -> " + EdtARed17_0 -> Text );
    if ( EdtAKon17_1 -> Text != EdtARed17_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon17_1 -> Text + " -> " + EdtARed17_1 -> Text );
    if ( EdtAKon17_2 -> Text != EdtARed17_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon17_2 -> Text + " -> " + EdtARed17_2 -> Text );
    if ( EdtAKon17_3 -> Text != EdtARed17_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon17_3 -> Text + " -> " + EdtARed17_3 -> Text );
    if ( EdtAKon17_4 -> Text != EdtARed17_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon17_4 -> Text + " -> " + EdtARed17_4 -> Text );
    if ( EdtAKon17_5 -> Text != EdtARed17_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon17_5 -> Text + " -> " + EdtARed17_5 -> Text );
    if ( EdtAKon17_7 -> Text != EdtARed17_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon17_7 -> Text + " -> " + EdtARed17_7 -> Text );
    if ( EdtAKon17_8 -> Text != EdtARed17_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon17_8 -> Text + " -> " + EdtARed17_8 -> Text );
    if ( EdtAKon17_9 -> Text != EdtARed17_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon17_9 -> Text + " -> " + EdtARed17_9 -> Text );
    if ( EdtAKon17_10 -> Text != EdtARed17_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon17_10 -> Text + " -> " + EdtARed17_10 -> Text );
    if ( EdtAKon17_11 -> Text != EdtARed17_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon17_11 -> Text + " -> " + EdtARed17_11 -> Text );
    if ( EdtAKon17_12 -> Text != EdtARed17_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon17_12 -> Text + " -> " + EdtARed17_12 -> Text );
    if ( EdtAKon17_13 -> Text != EdtARed17_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon17_13 -> Text + " -> " + EdtARed17_13 -> Text );
    if ( EdtAKon17_14 -> Text != EdtARed17_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon17_14 -> Text + " -> " + EdtARed17_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("18 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //18 ������
    if ( EdtAKon18_0 -> Text != EdtARed18_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon18_0 -> Text + " -> " + EdtARed18_0 -> Text );
    if ( EdtAKon18_1 -> Text != EdtARed18_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon18_1 -> Text + " -> " + EdtARed18_1 -> Text );
    if ( EdtAKon18_2 -> Text != EdtARed18_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon18_2 -> Text + " -> " + EdtARed18_2 -> Text );
    if ( EdtAKon18_3 -> Text != EdtARed18_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon18_3 -> Text + " -> " + EdtARed18_3 -> Text );
    if ( EdtAKon18_4 -> Text != EdtARed18_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon18_4 -> Text + " -> " + EdtARed18_4 -> Text );
    if ( EdtAKon18_5 -> Text != EdtARed18_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon18_5 -> Text + " -> " + EdtARed18_5 -> Text );
    if ( EdtAKon18_7 -> Text != EdtARed18_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon18_7 -> Text + " -> " + EdtARed18_7 -> Text );
    if ( EdtAKon18_8 -> Text != EdtARed18_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon18_8 -> Text + " -> " + EdtARed18_8 -> Text );
    if ( EdtAKon18_9 -> Text != EdtARed18_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon18_9 -> Text + " -> " + EdtARed18_9 -> Text );
    if ( EdtAKon18_10 -> Text != EdtARed18_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon18_10 -> Text + " -> " + EdtARed18_10 -> Text );
    if ( EdtAKon18_11 -> Text != EdtARed18_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon18_11 -> Text + " -> " + EdtARed18_11 -> Text );
    if ( EdtAKon18_12 -> Text != EdtARed18_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon18_12 -> Text + " -> " + EdtARed18_12 -> Text );
    if ( EdtAKon18_13 -> Text != EdtARed18_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon18_13 -> Text + " -> " + EdtARed18_13 -> Text );
    if ( EdtAKon18_14 -> Text != EdtARed18_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon18_14 -> Text + " -> " + EdtARed18_14 -> Text );
    
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("19 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //19 ������
    if ( EdtAKon19_0 -> Text != EdtARed19_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon19_0 -> Text + " -> " + EdtARed19_0 -> Text );
    if ( EdtAKon19_1 -> Text != EdtARed19_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon19_1 -> Text + " -> " + EdtARed19_1 -> Text );
    if ( EdtAKon19_2 -> Text != EdtARed19_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon19_2 -> Text + " -> " + EdtARed19_2 -> Text );
    if ( EdtAKon19_3 -> Text != EdtARed19_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon19_3 -> Text + " -> " + EdtARed19_3 -> Text );
    if ( EdtAKon19_4 -> Text != EdtARed19_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon19_4 -> Text + " -> " + EdtARed19_4 -> Text );
    if ( EdtAKon19_5 -> Text != EdtARed19_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon19_5 -> Text + " -> " + EdtARed19_5 -> Text );
    if ( EdtAKon19_7 -> Text != EdtARed19_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon19_7 -> Text + " -> " + EdtARed19_7 -> Text );
    if ( EdtAKon19_8 -> Text != EdtARed19_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon19_8 -> Text + " -> " + EdtARed19_8 -> Text );
    if ( EdtAKon19_9 -> Text != EdtARed19_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon19_9 -> Text + " -> " + EdtARed19_9 -> Text );
    if ( EdtAKon19_10 -> Text != EdtARed19_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon19_10 -> Text + " -> " + EdtARed19_10 -> Text );
    if ( EdtAKon19_11 -> Text != EdtARed19_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon19_11 -> Text + " -> " + EdtARed19_11 -> Text );
    if ( EdtAKon19_12 -> Text != EdtARed19_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon19_12 -> Text + " -> " + EdtARed19_12 -> Text );
    if ( EdtAKon19_13 -> Text != EdtARed19_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon19_13 -> Text + " -> " + EdtARed19_13 -> Text );
    if ( EdtAKon19_14 -> Text != EdtARed19_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon19_14 -> Text + " -> " + EdtARed19_14 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("20 ������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //20 ������
    if ( EdtAKon20_0 -> Text != EdtARed20_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon20_0 -> Text + " -> " + EdtARed20_0 -> Text );
    if ( EdtAKon20_1 -> Text != EdtARed20_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon20_1 -> Text + " -> " + EdtARed20_1 -> Text );
    if ( EdtAKon20_2 -> Text != EdtARed20_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtAKon20_2 -> Text + " -> " + EdtARed20_2 -> Text );
    if ( EdtAKon20_3 -> Text != EdtARed20_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtAKon20_3 -> Text + " -> " + EdtARed20_3 -> Text );
    if ( EdtAKon20_4 -> Text != EdtARed20_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtAKon20_4 -> Text + " -> " + EdtARed20_4 -> Text );
    if ( EdtAKon20_5 -> Text != EdtARed20_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtAKon20_5 -> Text + " -> " + EdtARed20_5 -> Text );
    if ( EdtAKon20_7 -> Text != EdtARed20_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon20_7 -> Text + " -> " + EdtARed20_7 -> Text );
    if ( EdtAKon20_8 -> Text != EdtARed20_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtAKon20_8 -> Text + " -> " + EdtARed20_8 -> Text );
    if ( EdtAKon20_9 -> Text != EdtARed20_9 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon20_9 -> Text + " -> " + EdtARed20_9 -> Text );
    if ( EdtAKon20_10 -> Text != EdtARed20_10 -> Text )
        MemoStat -> Lines -> Add("��������� ������������ �����: " + EdtAKon20_10 -> Text + " -> " + EdtARed20_10 -> Text );
    if ( EdtAKon20_11 -> Text != EdtARed20_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon20_11 -> Text + " -> " + EdtARed20_11 -> Text );
    if ( EdtAKon20_12 -> Text != EdtARed20_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon20_12 -> Text + " -> " + EdtARed20_12 -> Text );
    if ( EdtAKon20_13 -> Text != EdtARed20_13 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon20_13 -> Text + " -> " + EdtARed20_13 -> Text );
    if ( EdtAKon20_14 -> Text != EdtARed20_14 -> Text )
        MemoStat -> Lines -> Add("� ���������?: " + EdtAKon20_14 -> Text + " -> " + EdtARed20_14 -> Text );

    // ����������� ���������� ���������
    EdtARed1_0 -> Color = clWhite;
    EdtARed1_1 -> Color = clWhite;
	EdtARed1_2 -> Color = clWhite;
	EdtARed1_3 -> Color = clWhite;
	EdtARed1_4 -> Color = clWhite;
	EdtARed1_5 -> Color = clWhite;
	EdtARed1_7 -> Color = clWhite;
	EdtARed1_8 -> Color = clWhite;
	EdtARed1_9 -> Color = clWhite;
	EdtARed1_10 -> Color = clWhite;
	EdtARed1_11 -> Color = clWhite;
    EdtARed1_12 -> Color = clWhite;
	EdtARed1_13 -> Color = clWhite;
	EdtARed1_14 -> Color = clWhite;

    EdtARed2_0 -> Color = clWhite;
    EdtARed2_1 -> Color = clWhite;
	EdtARed2_2 -> Color = clWhite;
	EdtARed2_3 -> Color = clWhite;
	EdtARed2_4 -> Color = clWhite;
	EdtARed2_5 -> Color = clWhite;
	EdtARed2_7 -> Color = clWhite;
	EdtARed2_8 -> Color = clWhite;
	EdtARed2_9 -> Color = clWhite;
	EdtARed2_10 -> Color = clWhite;
	EdtARed2_11 -> Color = clWhite;
    EdtARed2_12 -> Color = clWhite;
	EdtARed2_13 -> Color = clWhite;
	EdtARed2_14 -> Color = clWhite;

    EdtARed3_0 -> Color = clWhite;
    EdtARed3_1 -> Color = clWhite;
	EdtARed3_2 -> Color = clWhite;
	EdtARed3_3 -> Color = clWhite;
	EdtARed3_4 -> Color = clWhite;
	EdtARed3_5 -> Color = clWhite;
	EdtARed3_7 -> Color = clWhite;
	EdtARed3_8 -> Color = clWhite;
	EdtARed3_9 -> Color = clWhite;
	EdtARed3_10 -> Color = clWhite;
	EdtARed3_11 -> Color = clWhite;
    EdtARed3_12 -> Color = clWhite;
	EdtARed3_13 -> Color = clWhite;
	EdtARed3_14 -> Color = clWhite;

    EdtARed4_0 -> Color = clWhite;
    EdtARed4_1 -> Color = clWhite;
	EdtARed4_2 -> Color = clWhite;
	EdtARed4_3 -> Color = clWhite;
	EdtARed4_4 -> Color = clWhite;
	EdtARed4_5 -> Color = clWhite;
	EdtARed4_7 -> Color = clWhite;
	EdtARed4_8 -> Color = clWhite;
	EdtARed4_9 -> Color = clWhite;
	EdtARed4_10 -> Color = clWhite;
	EdtARed4_11 -> Color = clWhite;
    EdtARed4_12 -> Color = clWhite;
	EdtARed4_13 -> Color = clWhite;
	EdtARed4_14 -> Color = clWhite;

    EdtARed5_0 -> Color = clWhite;
    EdtARed5_1 -> Color = clWhite;
	EdtARed5_2 -> Color = clWhite;
	EdtARed5_3 -> Color = clWhite;
	EdtARed5_4 -> Color = clWhite;
	EdtARed5_5 -> Color = clWhite;
	EdtARed5_7 -> Color = clWhite;
	EdtARed5_8 -> Color = clWhite;
	EdtARed5_9 -> Color = clWhite;
	EdtARed5_10 -> Color = clWhite;
	EdtARed5_11 -> Color = clWhite;
    EdtARed5_12 -> Color = clWhite;
	EdtARed5_13 -> Color = clWhite;
	EdtARed5_14 -> Color = clWhite;

    EdtARed6_0 -> Color = clWhite;
    EdtARed6_1 -> Color = clWhite;
	EdtARed6_2 -> Color = clWhite;
	EdtARed6_3 -> Color = clWhite;
	EdtARed6_4 -> Color = clWhite;
	EdtARed6_5 -> Color = clWhite;
	EdtARed6_7 -> Color = clWhite;
	EdtARed6_8 -> Color = clWhite;
	EdtARed6_9 -> Color = clWhite;
	EdtARed6_10 -> Color = clWhite;
	EdtARed6_11 -> Color = clWhite;
    EdtARed6_12 -> Color = clWhite;
	EdtARed6_13 -> Color = clWhite;
	EdtARed6_14 -> Color = clWhite;

    EdtARed7_0 -> Color = clWhite;
    EdtARed7_1 -> Color = clWhite;
	EdtARed7_2 -> Color = clWhite;
	EdtARed7_3 -> Color = clWhite;
	EdtARed7_4 -> Color = clWhite;
	EdtARed7_5 -> Color = clWhite;
	EdtARed7_7 -> Color = clWhite;
	EdtARed7_8 -> Color = clWhite;
	EdtARed7_9 -> Color = clWhite;
	EdtARed7_10 -> Color = clWhite;
	EdtARed7_11 -> Color = clWhite;
    EdtARed7_12 -> Color = clWhite;
	EdtARed7_13 -> Color = clWhite;
	EdtARed7_14 -> Color = clWhite;

    EdtARed8_0 -> Color = clWhite;
    EdtARed8_1 -> Color = clWhite;
	EdtARed8_2 -> Color = clWhite;
	EdtARed8_3 -> Color = clWhite;
	EdtARed8_4 -> Color = clWhite;
	EdtARed8_5 -> Color = clWhite;
	EdtARed8_7 -> Color = clWhite;
	EdtARed8_8 -> Color = clWhite;
	EdtARed8_9 -> Color = clWhite;
	EdtARed8_10 -> Color = clWhite;
	EdtARed8_11 -> Color = clWhite;
    EdtARed8_12 -> Color = clWhite;
	EdtARed8_13 -> Color = clWhite;
	EdtARed8_14 -> Color = clWhite;

    EdtARed9_0 -> Color = clWhite;
    EdtARed9_1 -> Color = clWhite;
	EdtARed9_2 -> Color = clWhite;
	EdtARed9_3 -> Color = clWhite;
	EdtARed9_4 -> Color = clWhite;
	EdtARed9_5 -> Color = clWhite;
	EdtARed9_7 -> Color = clWhite;
	EdtARed9_8 -> Color = clWhite;
	EdtARed9_9 -> Color = clWhite;
	EdtARed9_10 -> Color = clWhite;
	EdtARed9_11 -> Color = clWhite;
    EdtARed9_12 -> Color = clWhite;
	EdtARed9_13 -> Color = clWhite;
	EdtARed9_14 -> Color = clWhite;

    EdtARed10_0 -> Color = clWhite;
    EdtARed10_1 -> Color = clWhite;
	EdtARed10_2 -> Color = clWhite;
	EdtARed10_3 -> Color = clWhite;
	EdtARed10_4 -> Color = clWhite;
	EdtARed10_5 -> Color = clWhite;
	EdtARed10_7 -> Color = clWhite;
	EdtARed10_8 -> Color = clWhite;
	EdtARed10_9 -> Color = clWhite;
	EdtARed10_10 -> Color = clWhite;
	EdtARed10_11 -> Color = clWhite;
    EdtARed10_12 -> Color = clWhite;
	EdtARed10_13 -> Color = clWhite;
	EdtARed10_14 -> Color = clWhite;

    EdtARed11_0 -> Color = clWhite;
    EdtARed11_1 -> Color = clWhite;
	EdtARed11_2 -> Color = clWhite;
	EdtARed11_3 -> Color = clWhite;
	EdtARed11_4 -> Color = clWhite;
	EdtARed11_5 -> Color = clWhite;
	EdtARed11_7 -> Color = clWhite;
	EdtARed11_8 -> Color = clWhite;
	EdtARed11_9 -> Color = clWhite;
	EdtARed11_10 -> Color = clWhite;
	EdtARed11_11 -> Color = clWhite;
    EdtARed11_12 -> Color = clWhite;
	EdtARed11_13 -> Color = clWhite;
	EdtARed11_14 -> Color = clWhite;

    EdtARed12_0 -> Color = clWhite;
    EdtARed12_1 -> Color = clWhite;
	EdtARed12_2 -> Color = clWhite;
	EdtARed12_3 -> Color = clWhite;
	EdtARed12_4 -> Color = clWhite;
	EdtARed12_5 -> Color = clWhite;
	EdtARed12_7 -> Color = clWhite;
	EdtARed12_8 -> Color = clWhite;
	EdtARed12_9 -> Color = clWhite;
	EdtARed12_10 -> Color = clWhite;
	EdtARed12_11 -> Color = clWhite;
    EdtARed12_12 -> Color = clWhite;
	EdtARed12_13 -> Color = clWhite;
	EdtARed12_14 -> Color = clWhite;

    EdtARed13_0 -> Color = clWhite;
    EdtARed13_1 -> Color = clWhite;
	EdtARed13_2 -> Color = clWhite;
	EdtARed13_3 -> Color = clWhite;
	EdtARed13_4 -> Color = clWhite;
	EdtARed13_5 -> Color = clWhite;
	EdtARed13_7 -> Color = clWhite;
	EdtARed13_8 -> Color = clWhite;
	EdtARed13_9 -> Color = clWhite;
	EdtARed13_10 -> Color = clWhite;
	EdtARed13_11 -> Color = clWhite;
    EdtARed13_12 -> Color = clWhite;
	EdtARed13_13 -> Color = clWhite;
	EdtARed13_14 -> Color = clWhite;

    EdtARed14_0 -> Color = clWhite;
    EdtARed14_1 -> Color = clWhite;
	EdtARed14_2 -> Color = clWhite;
	EdtARed14_3 -> Color = clWhite;
	EdtARed14_4 -> Color = clWhite;
	EdtARed14_5 -> Color = clWhite;
	EdtARed14_7 -> Color = clWhite;
	EdtARed14_8 -> Color = clWhite;
	EdtARed14_9 -> Color = clWhite;
	EdtARed14_10 -> Color = clWhite;
	EdtARed14_11 -> Color = clWhite;
    EdtARed14_12 -> Color = clWhite;
	EdtARed14_13 -> Color = clWhite;
	EdtARed14_14 -> Color = clWhite;

    EdtARed15_0 -> Color = clWhite;
    EdtARed15_1 -> Color = clWhite;
	EdtARed15_2 -> Color = clWhite;
	EdtARed15_3 -> Color = clWhite;
	EdtARed15_4 -> Color = clWhite;
	EdtARed15_5 -> Color = clWhite;
	EdtARed15_7 -> Color = clWhite;
	EdtARed15_8 -> Color = clWhite;
	EdtARed15_9 -> Color = clWhite;
	EdtARed15_10 -> Color = clWhite;
	EdtARed15_11 -> Color = clWhite;
    EdtARed15_12 -> Color = clWhite;
	EdtARed15_13 -> Color = clWhite;
	EdtARed15_14 -> Color = clWhite;

    EdtARed16_0 -> Color = clWhite;
    EdtARed16_1 -> Color = clWhite;
	EdtARed16_2 -> Color = clWhite;
	EdtARed16_3 -> Color = clWhite;
	EdtARed16_4 -> Color = clWhite;
	EdtARed16_5 -> Color = clWhite;
	EdtARed16_7 -> Color = clWhite;
	EdtARed16_8 -> Color = clWhite;
	EdtARed16_9 -> Color = clWhite;
	EdtARed16_10 -> Color = clWhite;
	EdtARed16_11 -> Color = clWhite;
    EdtARed16_12 -> Color = clWhite;
	EdtARed16_13 -> Color = clWhite;
	EdtARed16_14 -> Color = clWhite;

    EdtARed17_0 -> Color = clWhite;
    EdtARed17_1 -> Color = clWhite;
	EdtARed17_2 -> Color = clWhite;
	EdtARed17_3 -> Color = clWhite;
	EdtARed17_4 -> Color = clWhite;
	EdtARed17_5 -> Color = clWhite;
	EdtARed17_7 -> Color = clWhite;
	EdtARed17_8 -> Color = clWhite;
	EdtARed17_9 -> Color = clWhite;
	EdtARed17_10 -> Color = clWhite;
	EdtARed17_11 -> Color = clWhite;
    EdtARed17_12 -> Color = clWhite;
	EdtARed17_13 -> Color = clWhite;
	EdtARed17_14 -> Color = clWhite;

    EdtARed18_0 -> Color = clWhite;
    EdtARed18_1 -> Color = clWhite;
	EdtARed18_2 -> Color = clWhite;
	EdtARed18_3 -> Color = clWhite;
	EdtARed18_4 -> Color = clWhite;
	EdtARed18_5 -> Color = clWhite;
	EdtARed18_7 -> Color = clWhite;
	EdtARed18_8 -> Color = clWhite;
	EdtARed18_9 -> Color = clWhite;
	EdtARed18_10 -> Color = clWhite;
	EdtARed18_11 -> Color = clWhite;
    EdtARed18_12 -> Color = clWhite;
	EdtARed18_13 -> Color = clWhite;
	EdtARed18_14 -> Color = clWhite;

    EdtARed19_0 -> Color = clWhite;
    EdtARed19_1 -> Color = clWhite;
	EdtARed19_2 -> Color = clWhite;
	EdtARed19_3 -> Color = clWhite;
	EdtARed19_4 -> Color = clWhite;
	EdtARed19_5 -> Color = clWhite;
	EdtARed19_7 -> Color = clWhite;
	EdtARed19_8 -> Color = clWhite;
	EdtARed19_9 -> Color = clWhite;
	EdtARed19_10 -> Color = clWhite;
	EdtARed19_11 -> Color = clWhite;
    EdtARed19_12 -> Color = clWhite;
	EdtARed19_13 -> Color = clWhite;
	EdtARed19_14 -> Color = clWhite;

    EdtARed20_0 -> Color = clWhite;
    EdtARed20_1 -> Color = clWhite;
	EdtARed20_2 -> Color = clWhite;
	EdtARed20_3 -> Color = clWhite;
	EdtARed20_4 -> Color = clWhite;
	EdtARed20_5 -> Color = clWhite;
	EdtARed20_7 -> Color = clWhite;
	EdtARed20_8 -> Color = clWhite;
	EdtARed20_9 -> Color = clWhite;
	EdtARed20_10 -> Color = clWhite;
	EdtARed20_11 -> Color = clWhite;
    EdtARed20_12 -> Color = clWhite;
	EdtARed20_13 -> Color = clWhite;
	EdtARed20_14 -> Color = clWhite;

    // �������� ��������
    VisualParA();
    ParametersGrigAutomate();
}

//---------------------------------------------------------------------------
//--��������� ���������� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::EdtARed1Exit(TObject *Sender)
{
// �������� ����� �� �������
    AnsiString
        text = ((TEdit*)Sender)->Text;
    for ( unsigned char i = 1 ; i < text.Length(); i++)
        if (text[i] == '.') text[i] = ',';
    unsigned char
        format; // ���-�� ������ ����� �������
    float
        valueText = StrToFloat(text);
    // �������� ������
    ((TEdit*)Sender)->Color = clSilver;


// ������ ���1
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_0" )||
            (((TEdit*)Sender)->Name == "EdtARed2_0" )||
            (((TEdit*)Sender)->Name == "EdtARed3_0" )||
            (((TEdit*)Sender)->Name == "EdtARed4_0" )||
            (((TEdit*)Sender)->Name == "EdtARed5_0" )||
            (((TEdit*)Sender)->Name == "EdtARed6_0" )||
            (((TEdit*)Sender)->Name == "EdtARed7_0" )||
            (((TEdit*)Sender)->Name == "EdtARed8_0" )||
            (((TEdit*)Sender)->Name == "EdtARed9_0" )||
            (((TEdit*)Sender)->Name == "EdtARed10_0" )||
            (((TEdit*)Sender)->Name == "EdtARed11_0" )||
            (((TEdit*)Sender)->Name == "EdtARed12_0" )||
            (((TEdit*)Sender)->Name == "EdtARed13_0" )||
            (((TEdit*)Sender)->Name == "EdtARed14_0" )||
            (((TEdit*)Sender)->Name == "EdtARed15_0" )||
            (((TEdit*)Sender)->Name == "EdtARed16_0" )||
            (((TEdit*)Sender)->Name == "EdtARed17_0" )||
            (((TEdit*)Sender)->Name == "EdtARed18_0" )||
            (((TEdit*)Sender)->Name == "EdtARed19_0" )||
            (((TEdit*)Sender)->Name == "EdtARed20_0" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_0" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG1_MIN)
        {
            valueText = RRG1_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG1_MAX)
        {
            valueText = RRG1_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���2
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_1" )||
            (((TEdit*)Sender)->Name == "EdtARed2_1" )||
            (((TEdit*)Sender)->Name == "EdtARed3_1" )||
            (((TEdit*)Sender)->Name == "EdtARed4_1" )||
            (((TEdit*)Sender)->Name == "EdtARed5_1" )||
            (((TEdit*)Sender)->Name == "EdtARed6_1" )||
            (((TEdit*)Sender)->Name == "EdtARed7_1" )||
            (((TEdit*)Sender)->Name == "EdtARed8_1" )||
            (((TEdit*)Sender)->Name == "EdtARed9_1" )||
            (((TEdit*)Sender)->Name == "EdtARed10_1" )||
            (((TEdit*)Sender)->Name == "EdtARed11_1" )||
            (((TEdit*)Sender)->Name == "EdtARed12_1" )||
            (((TEdit*)Sender)->Name == "EdtARed13_1" )||
            (((TEdit*)Sender)->Name == "EdtARed14_1" )||
            (((TEdit*)Sender)->Name == "EdtARed15_1" )||
            (((TEdit*)Sender)->Name == "EdtARed16_1" )||
            (((TEdit*)Sender)->Name == "EdtARed17_1" )||
            (((TEdit*)Sender)->Name == "EdtARed18_1" )||
            (((TEdit*)Sender)->Name == "EdtARed19_1" )||
            (((TEdit*)Sender)->Name == "EdtARed20_1" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_1" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG2_MIN)
        {
            valueText = RRG2_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG2_MAX)
        {
            valueText = RRG2_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���3
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_2" )||
            (((TEdit*)Sender)->Name == "EdtARed2_2" )||
            (((TEdit*)Sender)->Name == "EdtARed3_2" )||
            (((TEdit*)Sender)->Name == "EdtARed4_2" )||
            (((TEdit*)Sender)->Name == "EdtARed5_2" )||
            (((TEdit*)Sender)->Name == "EdtARed6_2" )||
            (((TEdit*)Sender)->Name == "EdtARed7_2" )||
            (((TEdit*)Sender)->Name == "EdtARed8_2" )||
            (((TEdit*)Sender)->Name == "EdtARed9_2" )||
            (((TEdit*)Sender)->Name == "EdtARed10_2" )||
            (((TEdit*)Sender)->Name == "EdtARed11_2" )||
            (((TEdit*)Sender)->Name == "EdtARed12_2" )||
            (((TEdit*)Sender)->Name == "EdtARed13_2" )||
            (((TEdit*)Sender)->Name == "EdtARed14_2" )||
            (((TEdit*)Sender)->Name == "EdtARed15_2" )||
            (((TEdit*)Sender)->Name == "EdtARed16_2" )||
            (((TEdit*)Sender)->Name == "EdtARed17_2" )||
            (((TEdit*)Sender)->Name == "EdtARed18_2" )||
            (((TEdit*)Sender)->Name == "EdtARed19_2" )||
            (((TEdit*)Sender)->Name == "EdtARed20_2" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_2" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG3_MIN)
        {
            valueText = RRG3_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG3_MAX)
        {
            valueText = RRG3_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���4
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_3" )||
            (((TEdit*)Sender)->Name == "EdtARed2_3" )||
            (((TEdit*)Sender)->Name == "EdtARed3_3" )||
            (((TEdit*)Sender)->Name == "EdtARed4_3" )||
            (((TEdit*)Sender)->Name == "EdtARed5_3" )||
            (((TEdit*)Sender)->Name == "EdtARed6_3" )||
            (((TEdit*)Sender)->Name == "EdtARed7_3" )||
            (((TEdit*)Sender)->Name == "EdtARed8_3" )||
            (((TEdit*)Sender)->Name == "EdtARed9_3" )||
            (((TEdit*)Sender)->Name == "EdtARed10_3" )||
            (((TEdit*)Sender)->Name == "EdtARed11_3" )||
            (((TEdit*)Sender)->Name == "EdtARed12_3" )||
            (((TEdit*)Sender)->Name == "EdtARed13_3" )||
            (((TEdit*)Sender)->Name == "EdtARed14_3" )||
            (((TEdit*)Sender)->Name == "EdtARed15_3" )||
            (((TEdit*)Sender)->Name == "EdtARed16_3" )||
            (((TEdit*)Sender)->Name == "EdtARed17_3" )||
            (((TEdit*)Sender)->Name == "EdtARed18_3" )||
            (((TEdit*)Sender)->Name == "EdtARed19_3" )||
            (((TEdit*)Sender)->Name == "EdtARed20_3" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_3" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG4_MIN)
        {
            valueText = RRG4_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG4_MAX)
        {
            valueText = RRG4_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���5
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_4" )||
            (((TEdit*)Sender)->Name == "EdtARed2_4" )||
            (((TEdit*)Sender)->Name == "EdtARed3_4" )||
            (((TEdit*)Sender)->Name == "EdtARed4_4" )||
            (((TEdit*)Sender)->Name == "EdtARed5_4" )||
            (((TEdit*)Sender)->Name == "EdtARed6_4" )||
            (((TEdit*)Sender)->Name == "EdtARed7_4" )||
            (((TEdit*)Sender)->Name == "EdtARed8_4" )||
            (((TEdit*)Sender)->Name == "EdtARed9_4" )||
            (((TEdit*)Sender)->Name == "EdtARed10_4" )||
            (((TEdit*)Sender)->Name == "EdtARed11_4" )||
            (((TEdit*)Sender)->Name == "EdtARed12_4" )||
            (((TEdit*)Sender)->Name == "EdtARed13_4" )||
            (((TEdit*)Sender)->Name == "EdtARed14_4" )||
            (((TEdit*)Sender)->Name == "EdtARed15_4" )||
            (((TEdit*)Sender)->Name == "EdtARed16_4" )||
            (((TEdit*)Sender)->Name == "EdtARed17_4" )||
            (((TEdit*)Sender)->Name == "EdtARed18_4" )||
            (((TEdit*)Sender)->Name == "EdtARed19_4" )||
            (((TEdit*)Sender)->Name == "EdtARed20_4" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_4" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG5_MIN)
        {
            valueText = RRG5_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG5_MAX)
        {
            valueText = RRG5_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���6
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_5" )||
            (((TEdit*)Sender)->Name == "EdtARed2_5" )||
            (((TEdit*)Sender)->Name == "EdtARed3_5" )||
            (((TEdit*)Sender)->Name == "EdtARed4_5" )||
            (((TEdit*)Sender)->Name == "EdtARed5_5" )||
            (((TEdit*)Sender)->Name == "EdtARed6_5" )||
            (((TEdit*)Sender)->Name == "EdtARed7_5" )||
            (((TEdit*)Sender)->Name == "EdtARed8_5" )||
            (((TEdit*)Sender)->Name == "EdtARed9_5" )||
            (((TEdit*)Sender)->Name == "EdtARed10_5" )||
            (((TEdit*)Sender)->Name == "EdtARed11_5" )||
            (((TEdit*)Sender)->Name == "EdtARed12_5" )||
            (((TEdit*)Sender)->Name == "EdtARed13_5" )||
            (((TEdit*)Sender)->Name == "EdtARed14_5" )||
            (((TEdit*)Sender)->Name == "EdtARed15_5" )||
            (((TEdit*)Sender)->Name == "EdtARed16_5" )||
            (((TEdit*)Sender)->Name == "EdtARed17_5" )||
            (((TEdit*)Sender)->Name == "EdtARed18_5" )||
            (((TEdit*)Sender)->Name == "EdtARed19_5" )||
            (((TEdit*)Sender)->Name == "EdtARed20_5" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_5" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG6_MIN)
        {
            valueText = RRG6_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG6_MAX)
        {
            valueText = RRG6_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���7
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_6" )||
            (((TEdit*)Sender)->Name == "EditNastrTo2")
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < RRG7_MIN)
        {
            valueText = RRG7_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG7_MAX)
        {
            valueText = RRG7_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ��������
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_7" )||
            (((TEdit*)Sender)->Name == "EdtARed2_7" )||
            (((TEdit*)Sender)->Name == "EdtARed3_7" )||
            (((TEdit*)Sender)->Name == "EdtARed4_7" )||
            (((TEdit*)Sender)->Name == "EdtARed5_7" )||
            (((TEdit*)Sender)->Name == "EdtARed6_7" )||
            (((TEdit*)Sender)->Name == "EdtARed7_7" )||
            (((TEdit*)Sender)->Name == "EdtARed8_7" )||
            (((TEdit*)Sender)->Name == "EdtARed9_7" )||
            (((TEdit*)Sender)->Name == "EdtARed10_7" )||
            (((TEdit*)Sender)->Name == "EdtARed11_7" )||
            (((TEdit*)Sender)->Name == "EdtARed12_7" )||
            (((TEdit*)Sender)->Name == "EdtARed13_7" )||
            (((TEdit*)Sender)->Name == "EdtARed14_7" )||
            (((TEdit*)Sender)->Name == "EdtARed15_7" )||
            (((TEdit*)Sender)->Name == "EdtARed16_7" )||
            (((TEdit*)Sender)->Name == "EdtARed17_7" )||
            (((TEdit*)Sender)->Name == "EdtARed18_7" )||
            (((TEdit*)Sender)->Name == "EdtARed19_7" )||
            (((TEdit*)Sender)->Name == "EdtARed20_7" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_7" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < DAVL_MIN)
        {
            valueText = DAVL_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > DAVL_MAX)
        {
            valueText = DAVL_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� ��� ��
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_8" )||
            (((TEdit*)Sender)->Name == "EdtARed2_8" )||
            (((TEdit*)Sender)->Name == "EdtARed3_8" )||
            (((TEdit*)Sender)->Name == "EdtARed4_8" )||
            (((TEdit*)Sender)->Name == "EdtARed5_8" )||
            (((TEdit*)Sender)->Name == "EdtARed6_8" )||
            (((TEdit*)Sender)->Name == "EdtARed7_8" )||
            (((TEdit*)Sender)->Name == "EdtARed8_8" )||
            (((TEdit*)Sender)->Name == "EdtARed9_8" )||
            (((TEdit*)Sender)->Name == "EdtARed10_8" )||
            (((TEdit*)Sender)->Name == "EdtARed11_8" )||
            (((TEdit*)Sender)->Name == "EdtARed12_8" )||
            (((TEdit*)Sender)->Name == "EdtARed13_8" )||
            (((TEdit*)Sender)->Name == "EdtARed14_8" )||
            (((TEdit*)Sender)->Name == "EdtARed15_8" )||
            (((TEdit*)Sender)->Name == "EdtARed16_8" )||
            (((TEdit*)Sender)->Name == "EdtARed17_8" )||
            (((TEdit*)Sender)->Name == "EdtARed18_8" )||
            (((TEdit*)Sender)->Name == "EdtARed19_8" )||
            (((TEdit*)Sender)->Name == "EdtARed20_8" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_8" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < CESAR_MIN_IP)
        {
            valueText = CESAR_MIN_IP;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > CESAR_MAX_IP)
        {
            valueText = CESAR_MAX_IP;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ��������� ���� �����
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_9" )||
            (((TEdit*)Sender)->Name == "EdtARed2_9" )||
            (((TEdit*)Sender)->Name == "EdtARed3_9" )||
            (((TEdit*)Sender)->Name == "EdtARed4_9" )||
            (((TEdit*)Sender)->Name == "EdtARed5_9" )||
            (((TEdit*)Sender)->Name == "EdtARed6_9" )||
            (((TEdit*)Sender)->Name == "EdtARed7_9" )||
            (((TEdit*)Sender)->Name == "EdtARed8_9" )||
            (((TEdit*)Sender)->Name == "EdtARed9_9" )||
            (((TEdit*)Sender)->Name == "EdtARed10_9" )||
            (((TEdit*)Sender)->Name == "EdtARed11_9" )||
            (((TEdit*)Sender)->Name == "EdtARed12_9" )||
            (((TEdit*)Sender)->Name == "EdtARed13_9" )||
            (((TEdit*)Sender)->Name == "EdtARed14_9" )||
            (((TEdit*)Sender)->Name == "EdtARed15_9" )||
            (((TEdit*)Sender)->Name == "EdtARed16_9" )||
            (((TEdit*)Sender)->Name == "EdtARed17_9" )||
            (((TEdit*)Sender)->Name == "EdtARed18_9" )||
            (((TEdit*)Sender)->Name == "EdtARed19_9" )||
            (((TEdit*)Sender)->Name == "EdtARed20_9" )
        )
    {
        // ���-�� ������ ����� ������� 2
        format = 2;
        // ��������� �� �������
        if (valueText < US_MIN_USER)
        {
            valueText = US_MIN_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > US_MAX_USER)
        {
            valueText = US_MAX_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ��������� ���� �����
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_10" )||
            (((TEdit*)Sender)->Name == "EdtARed2_10" )||
            (((TEdit*)Sender)->Name == "EdtARed3_10" )||
            (((TEdit*)Sender)->Name == "EdtARed4_10" )||
            (((TEdit*)Sender)->Name == "EdtARed5_10" )||
            (((TEdit*)Sender)->Name == "EdtARed6_10" )||
            (((TEdit*)Sender)->Name == "EdtARed7_10" )||
            (((TEdit*)Sender)->Name == "EdtARed8_10" )||
            (((TEdit*)Sender)->Name == "EdtARed9_10" )||
            (((TEdit*)Sender)->Name == "EdtARed10_10" )||
            (((TEdit*)Sender)->Name == "EdtARed11_10" )||
            (((TEdit*)Sender)->Name == "EdtARed12_10" )||
            (((TEdit*)Sender)->Name == "EdtARed13_10" )||
            (((TEdit*)Sender)->Name == "EdtARed14_10" )||
            (((TEdit*)Sender)->Name == "EdtARed15_10" )||
            (((TEdit*)Sender)->Name == "EdtARed16_10" )||
            (((TEdit*)Sender)->Name == "EdtARed17_10" )||
            (((TEdit*)Sender)->Name == "EdtARed18_10" )||
            (((TEdit*)Sender)->Name == "EdtARed19_10" )||
            (((TEdit*)Sender)->Name == "EdtARed20_10" )
        )
    {
        // ���-�� ������ ����� ������� 2
        format = 2;
        // ��������� �� �������
        if (valueText < US_MIN_USER)
        {
            valueText = US_MIN_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > US_MAX_USER)
        {
            valueText = US_MAX_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� ��� �/�
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_11" )||
            (((TEdit*)Sender)->Name == "EdtARed2_11" )||
            (((TEdit*)Sender)->Name == "EdtARed3_11" )||
            (((TEdit*)Sender)->Name == "EdtARed4_11" )||
            (((TEdit*)Sender)->Name == "EdtARed5_11" )||
            (((TEdit*)Sender)->Name == "EdtARed6_11" )||
            (((TEdit*)Sender)->Name == "EdtARed7_11" )||
            (((TEdit*)Sender)->Name == "EdtARed8_11" )||
            (((TEdit*)Sender)->Name == "EdtARed9_11" )||
            (((TEdit*)Sender)->Name == "EdtARed10_11" )||
            (((TEdit*)Sender)->Name == "EdtARed11_11" )||
            (((TEdit*)Sender)->Name == "EdtARed12_11" )||
            (((TEdit*)Sender)->Name == "EdtARed13_11" )||
            (((TEdit*)Sender)->Name == "EdtARed14_11" )||
            (((TEdit*)Sender)->Name == "EdtARed15_11" )||
            (((TEdit*)Sender)->Name == "EdtARed16_11" )||
            (((TEdit*)Sender)->Name == "EdtARed17_11" )||
            (((TEdit*)Sender)->Name == "EdtARed18_11" )||
            (((TEdit*)Sender)->Name == "EdtARed19_11" )||
            (((TEdit*)Sender)->Name == "EdtARed20_11" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_11" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
       // switch(TPControl->)
        if (EdtAKon1_12->Text == 0){}
        // ��������� �� �������
        if (valueText < CESAR_MIN_PD)
        {
            valueText = CESAR_MIN_PD;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > CESAR_MAX_PD)
        {
            valueText = CESAR_MAX_PD;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ��������
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_12" )||
            (((TEdit*)Sender)->Name == "EdtARed2_12" )||
            (((TEdit*)Sender)->Name == "EdtARed3_12" )||
            (((TEdit*)Sender)->Name == "EdtARed4_12" )||
            (((TEdit*)Sender)->Name == "EdtARed5_12" )||
            (((TEdit*)Sender)->Name == "EdtARed6_12" )||
            (((TEdit*)Sender)->Name == "EdtARed7_12" )||
            (((TEdit*)Sender)->Name == "EdtARed8_12" )||
            (((TEdit*)Sender)->Name == "EdtARed9_12" )||
            (((TEdit*)Sender)->Name == "EdtARed10_12" )||
            (((TEdit*)Sender)->Name == "EdtARed11_12" )||
            (((TEdit*)Sender)->Name == "EdtARed12_12" )||
            (((TEdit*)Sender)->Name == "EdtARed13_12" )||
            (((TEdit*)Sender)->Name == "EdtARed14_12" )||
            (((TEdit*)Sender)->Name == "EdtARed15_12" )||
            (((TEdit*)Sender)->Name == "EdtARed16_12" )||
            (((TEdit*)Sender)->Name == "EdtARed17_12" )||
            (((TEdit*)Sender)->Name == "EdtARed18_12" )||
            (((TEdit*)Sender)->Name == "EdtARed19_12" )||
            (((TEdit*)Sender)->Name == "EdtARed20_12" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_12" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < SMESH_MIN_USER)
        {
            valueText = SMESH_MIN_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > SMESH_MIN_USER && valueText < 20)
        {
            valueText = SMESH_MIN_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > SMESH_MAX_USER)
        {
            valueText = SMESH_MAX_USER;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }


    // ����� ��������
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_13" )||
            (((TEdit*)Sender)->Name == "EdtARed2_13" )||
            (((TEdit*)Sender)->Name == "EdtARed3_13" )||
            (((TEdit*)Sender)->Name == "EdtARed4_13" )||
            (((TEdit*)Sender)->Name == "EdtARed5_13" )||
            (((TEdit*)Sender)->Name == "EdtARed6_13" )||
            (((TEdit*)Sender)->Name == "EdtARed7_13" )||
            (((TEdit*)Sender)->Name == "EdtARed8_13" )||
            (((TEdit*)Sender)->Name == "EdtARed9_13" )||
            (((TEdit*)Sender)->Name == "EdtARed10_13" )||
            (((TEdit*)Sender)->Name == "EdtARed11_13" )||
            (((TEdit*)Sender)->Name == "EdtARed12_13" )||
            (((TEdit*)Sender)->Name == "EdtARed13_13" )||
            (((TEdit*)Sender)->Name == "EdtARed14_13" )||
            (((TEdit*)Sender)->Name == "EdtARed15_13" )||
            (((TEdit*)Sender)->Name == "EdtARed16_13" )||
            (((TEdit*)Sender)->Name == "EdtARed17_13" )||
            (((TEdit*)Sender)->Name == "EdtARed18_13" )||
            (((TEdit*)Sender)->Name == "EdtARed19_13" )||
            (((TEdit*)Sender)->Name == "EdtARed20_13" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < TIME_MIN)
        {
            valueText = TIME_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > TIME_MAX)
        {
            valueText = TIME_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
	
	// ���� ������������
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_15" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_17" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_18" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < -4000000.0)
        {
            valueText = -4000000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 4000000.0)
        {
            valueText = 4000000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

//----------------
//----���������---
//----------------

    // ����� �������� ��� ���������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo13" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 300)
        {
            valueText = 300;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 10000)
        {
            valueText = 10000;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���������� ��������� ��
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo4" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 1)
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 13)
        {
            valueText = 13;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //���������� ����������� ������������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo5" )||
            (((TEdit*)Sender)->Name == "EditNastrTo6" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 4)
        {
            valueText = 4;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 20)
        {
            valueText = 20;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //�������� �� �������� ������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo14" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 1800)
        {
            valueText = 1800;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //����������� ��������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo11" )||
            (((TEdit*)Sender)->Name == "EditNastrTo12" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > 0&&valueText < 30)
        {
            valueText = 30;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 60)
        {
            valueText = 60;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }


     // ���������
    if  (
            (((TEdit*)Sender)->Name == "EdtTRed1" )||
            (((TEdit*)Sender)->Name == "EdtTRed2" )||
            (((TEdit*)Sender)->Name == "EdtTRed3" )||
            (((TEdit*)Sender)->Name == "EdtTRed4" )||
            (((TEdit*)Sender)->Name == "EdtTRed5" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > MEX_MAX)
        {
            valueText = MEX_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

         // ���������� �������
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_20" )
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 25)
        {
            valueText = 25;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� �� ���������
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 5, format);

    VisualParA();
    ParametersGrigAutomate();
}

//---------------------------------------------------------------------------
//--�������� ���������� ������� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParRClick(TObject *Sender)
{
    PanelParR -> Visible = true;
}
//---------------------------------------------------------------------------
//--����� �� �������� ���������� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnRNetClick(TObject *Sender)
{
    // ������ ������������� �������� ������
    PanelParR -> Visible = false;
}
//---------------------------------------------------------------------------
//--������������� �������� ���������� �������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnRDaClick(TObject *Sender)
{

    // ������ ������������� �������� ������
    PanelParR -> Visible = false;

//������ ���1	
par[0][0] = StrToFloat( EdtRRed0_0 -> Text )/ RRG1_MAX * 4095.0 ;
//������ ���2
par[0][1] = StrToFloat( EdtRRed0_1 -> Text )/ RRG2_MAX* 4095.0 ;
//������ ���3
par[0][2] = StrToFloat( EdtRRed0_2 -> Text )/ RRG3_MAX* 4095.0 ;
//������ ���4
par[0][3] = StrToFloat( EdtRRed0_3 -> Text )/ RRG4_MAX* 4095.0 ;
//������ ���5
par[0][4] = StrToFloat( EdtRRed0_4 -> Text )/ RRG5_MAX* 4095.0 ;
//������ ���6
par[0][5] = StrToFloat( EdtRRed0_5 -> Text )/ RRG6_MAX* 4095.0 ;
//������ ���7
par[0][6] = StrToFloat( EdtRRed0_6 -> Text )/ RRG7_MAX* 4095.0 ;
//�������� ��� ��
par[0][8] = StrToFloat( EdtRRed0_8 -> Text )* 4095.0 / CESAR_MAX_IP;
//�������� ��� ��
par[0][11] = StrToFloat( EdtRRed0_11 -> Text )* 4095.0 / CESAR_MAX_PD;
//��������
par[0][12] = StrToFloat( EdtRRed0_12 -> Text )* 4095.0 / SMESH_MAX_USER;
//��������
par[0][7] = StrToFloat( EdtRRed0_7->Text ) / DAVL_MAX * 10000.0;
//����������� ������������
par[0][15] = StrToInt( EdtRRed0_15->Text );
//������� ������������
par[0][17] = StrToInt( EdtRRed0_17->Text );
//����������� �������
par[0][18] = StrToInt( EdtRRed0_18->Text );
//�������� ��������
if(EdtRRed0_16 -> ItemIndex == 0)       { par[0][16]=0; }
else if(EdtRRed0_16 -> ItemIndex == 1)  { par[0][16]=1; }
else if(EdtRRed0_16 -> ItemIndex == 2)  { par[0][16]=2; }

    MemoStat -> Lines -> Add("");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ���������� ���������:");
    MemoStat -> Lines -> Add("");

    if ( EdtRKon0_0 -> Text != EdtRRed0_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtRKon0_0 -> Text + " -> " + EdtRRed0_0 -> Text );
    if ( EdtRKon0_1 -> Text != EdtRRed0_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtRKon0_1 -> Text + " -> " + EdtRRed0_1 -> Text );
    if ( EdtRKon0_2 -> Text != EdtRRed0_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtRKon0_2 -> Text + " -> " + EdtRRed0_2 -> Text );
    if ( EdtRKon0_3 -> Text != EdtRRed0_3 -> Text )
        MemoStat -> Lines -> Add("������ ���4: " + EdtRKon0_3 -> Text + " -> " + EdtRRed0_3 -> Text );
    if ( EdtRKon0_4 -> Text != EdtRRed0_4 -> Text )
        MemoStat -> Lines -> Add("������ ���5: " + EdtRKon0_4 -> Text + " -> " + EdtRRed0_4 -> Text );
    if ( EdtRKon0_5 -> Text != EdtRRed0_5 -> Text )
        MemoStat -> Lines -> Add("������ ���6: " + EdtRKon0_5 -> Text + " -> " + EdtRRed0_5 -> Text );
    if ( EdtRKon0_6 -> Text != EdtRRed0_6 -> Text )
        MemoStat -> Lines -> Add("������ ���7: " + EdtRKon0_6 -> Text + " -> " + EdtRRed0_6 -> Text );
    if ( EdtRKon0_8 -> Text != EdtRRed0_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� ��: " + EdtRKon0_8 -> Text + " -> " + EdtRRed0_8 -> Text );
    if ( EdtRKon0_11 -> Text != EdtRRed0_11 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtRKon0_11 -> Text + " -> " + EdtRRed0_11 -> Text );
    if ( EdtRKon0_12 -> Text != EdtRRed0_12 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtRKon0_12 -> Text + " -> " + EdtRRed0_12 -> Text );
    if ( EdtRKon0_7 -> Text != EdtRRed0_7 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtRKon0_7 -> Text + " -> " + EdtRRed0_7 -> Text );
    if ( EdtRKon0_15 -> Text != EdtRRed0_15 -> Text )
        MemoStat -> Lines -> Add("����������� ������������: " + EdtRKon0_15 -> Text + " -> " + EdtRRed0_15 -> Text );
    if ( EdtRKon0_17 -> Text != EdtRRed0_17 -> Text )
        MemoStat -> Lines -> Add("������� ������������: " + EdtRKon0_17 -> Text + " -> " + EdtRRed0_17 -> Text );
    if ( EdtRKon0_18 -> Text != EdtRRed0_18 -> Text )
        MemoStat -> Lines -> Add("����������� �������: " + EdtRKon0_18 -> Text + " -> " + EdtRRed0_18 -> Text );
    if ( EdtRKon0_16 -> Text != EdtRRed0_16 -> Text )
        MemoStat -> Lines -> Add("�������� ��������: " + EdtRKon0_16 -> Text + " -> " + EdtRRed0_16 -> Text );
		
    // ����������� ���������� ���������
    EdtRRed0_0 -> Color = clWhite;
    EdtRRed0_1 -> Color = clWhite;
    EdtRRed0_2 -> Color = clWhite;
    EdtRRed0_3 -> Color = clWhite;
    EdtRRed0_4 -> Color = clWhite;
    EdtRRed0_5 -> Color = clWhite;
    EdtRRed0_6 -> Color = clWhite;
    EdtRRed0_8 -> Color = clWhite;
    EdtRRed0_11 -> Color = clWhite;
    EdtRRed0_12 -> Color = clWhite;
    EdtRRed0_7 -> Color = clWhite;
    EdtRRed0_15 -> Color = clWhite;
    EdtRRed0_16 -> Color = clWhite;
    EdtRRed0_17 -> Color = clWhite;
    EdtRRed0_18 -> Color = clWhite;

    // �������� ��������
    VisualParR();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ParametersGrigLib() //������� ���������� ����������
{
    Par_Tab -> Color = clPurple;
    Par_Tab -> FixedColor = clPurple;
    Par_Tab -> Font->Color = clWhite;

    int count = 0;
    for(int i=0;i<21;i++)
    {
        Par_Tab->Cells[i][0] = i+1; //������� ����� ������

        for(int j=1; j<=14;j++)
        {
         Par_Tab->Cells[i][j] = MemoLib -> Lines -> operator [](count);
         ++count;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ParametersGrigAutomate() //������� ����������
{

    Par_Tab -> Color = 0x00EAD999;
    Par_Tab -> FixedColor = 0x00EAD999;
    Par_Tab -> Font->Color = clBlack;

    for(int i=1;i<21;i++)
    {
        Par_Tab->Cells[i-1][0] = i; //������� ����� ������

        Par_Tab->Cells[i-1][1] = FloatToStrF((float)par[i][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);
        Par_Tab->Cells[i-1][2] = FloatToStrF((float)par[i][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);
        Par_Tab->Cells[i-1][3] = FloatToStrF((float)par[i][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);
        Par_Tab->Cells[i-1][4] = FloatToStrF((float)par[i][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);
        Par_Tab->Cells[i-1][5] = FloatToStrF((float)par[i][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);
        Par_Tab->Cells[i-1][6] = FloatToStrF((float)par[i][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);
        Par_Tab->Cells[i-1][7] = FloatToStrF((float)par[i][7]/10000.0*DAVL_MAX, ffFixed, 8, 1);
        Par_Tab->Cells[i-1][8] = FloatToStrF((float)par[i][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);
        Par_Tab->Cells[i-1][9] = FloatToStrF((float)par[i][9]/4095.0*US_MAX, ffFixed, 5, 2);
        Par_Tab->Cells[i-1][10] = FloatToStrF((float)par[i][10]/4095.0*US_MAX, ffFixed, 5, 2);
        Par_Tab->Cells[i-1][11] = FloatToStrF((float)par[i][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);
        Par_Tab->Cells[i-1][12] = FloatToStrF((float)par[i][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);
        Par_Tab->Cells[i-1][13] = FloatToStrF((float)par[i][13], ffFixed, 5, 0);
        if(par[i][14]) Par_Tab->Cells[i-1][14] = "��";
        else Par_Tab->Cells[i-1][14] = "���";
    }
}
//---------------------------------------------------------------------------
//--����� ����� �� ������ ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryClick(TObject *Sender)
{

libNmb = ListBoxLibrary -> ItemIndex;
// ���� ������� ������������ ���������
if ( libNmb != -1 )
{
AnsiString fName = "LIB\\" + ListBoxLibrary->Items->operator[](libNmb) + ".txt";
if ( FileExists ( fName ) )
{
MemoLib -> Lines -> LoadFromFile( fName );
LblPresentName -> Caption = "���������� ���: " + ListBoxLibrary->Items->operator[](libNmb);
// ��������� � ��������� �������� ������������ ������ ��� ������


EdtALib1_0 -> Text = MemoLib -> Lines -> operator [](0);
EdtALib1_1 -> Text = MemoLib -> Lines -> operator [](1);
EdtALib1_2 -> Text = MemoLib -> Lines -> operator [](2);
EdtALib1_3 -> Text = MemoLib -> Lines -> operator [](3);
EdtALib1_4 -> Text = MemoLib -> Lines -> operator [](4);
EdtALib1_5 -> Text = MemoLib -> Lines -> operator [](5);
EdtALib1_7 -> Text = MemoLib -> Lines -> operator [](6);
EdtALib1_8 -> Text = MemoLib -> Lines -> operator [](7);
EdtALib1_9 -> Text = MemoLib -> Lines -> operator [](8);
EdtALib1_10 -> Text = MemoLib -> Lines -> operator [](9);
EdtALib1_11 -> Text = MemoLib -> Lines -> operator [](10);
EdtALib1_12 -> Text = MemoLib -> Lines -> operator [](11);
EdtALib1_13 -> Text = MemoLib -> Lines -> operator [](12);
EdtALib1_14 -> Text = MemoLib -> Lines -> operator [](13);

EdtALib2_0 -> Text = MemoLib -> Lines -> operator [](14);
EdtALib2_1 -> Text = MemoLib -> Lines -> operator [](15);
EdtALib2_2 -> Text = MemoLib -> Lines -> operator [](16);
EdtALib2_3 -> Text = MemoLib -> Lines -> operator [](17);
EdtALib2_4 -> Text = MemoLib -> Lines -> operator [](18);
EdtALib2_5 -> Text = MemoLib -> Lines -> operator [](19);
EdtALib2_7 -> Text = MemoLib -> Lines -> operator [](20);
EdtALib2_8 -> Text = MemoLib -> Lines -> operator [](21);
EdtALib2_9 -> Text = MemoLib -> Lines -> operator [](22);
EdtALib2_10 -> Text = MemoLib -> Lines -> operator [](23);
EdtALib2_11 -> Text = MemoLib -> Lines -> operator [](24);
EdtALib2_12 -> Text = MemoLib -> Lines -> operator [](25);
EdtALib2_13 -> Text = MemoLib -> Lines -> operator [](26);
EdtALib2_14 -> Text = MemoLib -> Lines -> operator [](27);

EdtALib3_0 -> Text = MemoLib -> Lines -> operator [](28);
EdtALib3_1 -> Text = MemoLib -> Lines -> operator [](29);
EdtALib3_2 -> Text = MemoLib -> Lines -> operator [](30);
EdtALib3_3 -> Text = MemoLib -> Lines -> operator [](31);
EdtALib3_4 -> Text = MemoLib -> Lines -> operator [](32);
EdtALib3_5 -> Text = MemoLib -> Lines -> operator [](33);
EdtALib3_7 -> Text = MemoLib -> Lines -> operator [](34);
EdtALib3_8 -> Text = MemoLib -> Lines -> operator [](35);
EdtALib3_9 -> Text = MemoLib -> Lines -> operator [](36);
EdtALib3_10 -> Text = MemoLib -> Lines -> operator [](37);
EdtALib3_11 -> Text = MemoLib -> Lines -> operator [](38);
EdtALib3_12 -> Text = MemoLib -> Lines -> operator [](39);
EdtALib3_13 -> Text = MemoLib -> Lines -> operator [](40);
EdtALib3_14 -> Text = MemoLib -> Lines -> operator [](41);

EdtALib4_0 -> Text = MemoLib -> Lines -> operator [](42);
EdtALib4_1 -> Text = MemoLib -> Lines -> operator [](43);
EdtALib4_2 -> Text = MemoLib -> Lines -> operator [](44);
EdtALib4_3 -> Text = MemoLib -> Lines -> operator [](45);
EdtALib4_4 -> Text = MemoLib -> Lines -> operator [](46);
EdtALib4_5 -> Text = MemoLib -> Lines -> operator [](47);
EdtALib4_7 -> Text = MemoLib -> Lines -> operator [](48);
EdtALib4_8 -> Text = MemoLib -> Lines -> operator [](49);
EdtALib4_9 -> Text = MemoLib -> Lines -> operator [](50);
EdtALib4_10 -> Text = MemoLib -> Lines -> operator [](51);
EdtALib4_11 -> Text = MemoLib -> Lines -> operator [](52);
EdtALib4_12 -> Text = MemoLib -> Lines -> operator [](53);
EdtALib4_13 -> Text = MemoLib -> Lines -> operator [](54);
EdtALib4_14 -> Text = MemoLib -> Lines -> operator [](55);

EdtALib5_0 -> Text = MemoLib -> Lines -> operator [](56);
EdtALib5_1 -> Text = MemoLib -> Lines -> operator [](57);
EdtALib5_2 -> Text = MemoLib -> Lines -> operator [](58);
EdtALib5_3 -> Text = MemoLib -> Lines -> operator [](59);
EdtALib5_4 -> Text = MemoLib -> Lines -> operator [](60);
EdtALib5_5 -> Text = MemoLib -> Lines -> operator [](61);
EdtALib5_7 -> Text = MemoLib -> Lines -> operator [](62);
EdtALib5_8 -> Text = MemoLib -> Lines -> operator [](63);
EdtALib5_9 -> Text = MemoLib -> Lines -> operator [](64);
EdtALib5_10 -> Text = MemoLib -> Lines -> operator [](65);
EdtALib5_11 -> Text = MemoLib -> Lines -> operator [](66);
EdtALib5_12 -> Text = MemoLib -> Lines -> operator [](67);
EdtALib5_13 -> Text = MemoLib -> Lines -> operator [](68);
EdtALib5_14 -> Text = MemoLib -> Lines -> operator [](69);

EdtALib6_0 -> Text = MemoLib -> Lines -> operator [](70);
EdtALib6_1 -> Text = MemoLib -> Lines -> operator [](71);
EdtALib6_2 -> Text = MemoLib -> Lines -> operator [](72);
EdtALib6_3 -> Text = MemoLib -> Lines -> operator [](73);
EdtALib6_4 -> Text = MemoLib -> Lines -> operator [](74);
EdtALib6_5 -> Text = MemoLib -> Lines -> operator [](75);
EdtALib6_7 -> Text = MemoLib -> Lines -> operator [](76);
EdtALib6_8 -> Text = MemoLib -> Lines -> operator [](77);
EdtALib6_9 -> Text = MemoLib -> Lines -> operator [](78);
EdtALib6_10 -> Text = MemoLib -> Lines -> operator [](79);
EdtALib6_11 -> Text = MemoLib -> Lines -> operator [](80);
EdtALib6_12 -> Text = MemoLib -> Lines -> operator [](81);
EdtALib6_13 -> Text = MemoLib -> Lines -> operator [](82);
EdtALib6_14 -> Text = MemoLib -> Lines -> operator [](83);

EdtALib7_0 -> Text = MemoLib -> Lines -> operator [](84);
EdtALib7_1 -> Text = MemoLib -> Lines -> operator [](85);
EdtALib7_2 -> Text = MemoLib -> Lines -> operator [](86);
EdtALib7_3 -> Text = MemoLib -> Lines -> operator [](87);
EdtALib7_4 -> Text = MemoLib -> Lines -> operator [](88);
EdtALib7_5 -> Text = MemoLib -> Lines -> operator [](89);
EdtALib7_7 -> Text = MemoLib -> Lines -> operator [](90);
EdtALib7_8 -> Text = MemoLib -> Lines -> operator [](91);
EdtALib7_9 -> Text = MemoLib -> Lines -> operator [](92);
EdtALib7_10 -> Text = MemoLib -> Lines -> operator [](93);
EdtALib7_11 -> Text = MemoLib -> Lines -> operator [](94);
EdtALib7_12 -> Text = MemoLib -> Lines -> operator [](95);
EdtALib7_13 -> Text = MemoLib -> Lines -> operator [](96);
EdtALib7_14 -> Text = MemoLib -> Lines -> operator [](97);

EdtALib8_0 -> Text = MemoLib -> Lines -> operator [](98);
EdtALib8_1 -> Text = MemoLib -> Lines -> operator [](99);
EdtALib8_2 -> Text = MemoLib -> Lines -> operator [](100);
EdtALib8_3 -> Text = MemoLib -> Lines -> operator [](101);
EdtALib8_4 -> Text = MemoLib -> Lines -> operator [](102);
EdtALib8_5 -> Text = MemoLib -> Lines -> operator [](103);
EdtALib8_7 -> Text = MemoLib -> Lines -> operator [](104);
EdtALib8_8 -> Text = MemoLib -> Lines -> operator [](105);
EdtALib8_9 -> Text = MemoLib -> Lines -> operator [](106);
EdtALib8_10 -> Text = MemoLib -> Lines -> operator [](107);
EdtALib8_11 -> Text = MemoLib -> Lines -> operator [](108);
EdtALib8_12 -> Text = MemoLib -> Lines -> operator [](109);
EdtALib8_13 -> Text = MemoLib -> Lines -> operator [](110);
EdtALib8_14 -> Text = MemoLib -> Lines -> operator [](111);

EdtALib9_0 -> Text = MemoLib -> Lines -> operator [](112);
EdtALib9_1 -> Text = MemoLib -> Lines -> operator [](113);
EdtALib9_2 -> Text = MemoLib -> Lines -> operator [](114);
EdtALib9_3 -> Text = MemoLib -> Lines -> operator [](115);
EdtALib9_4 -> Text = MemoLib -> Lines -> operator [](116);
EdtALib9_5 -> Text = MemoLib -> Lines -> operator [](117);
EdtALib9_7 -> Text = MemoLib -> Lines -> operator [](118);
EdtALib9_8 -> Text = MemoLib -> Lines -> operator [](119);
EdtALib9_9 -> Text = MemoLib -> Lines -> operator [](120);
EdtALib9_10 -> Text = MemoLib -> Lines -> operator [](121);
EdtALib9_11 -> Text = MemoLib -> Lines -> operator [](122);
EdtALib9_12 -> Text = MemoLib -> Lines -> operator [](123);
EdtALib9_13 -> Text = MemoLib -> Lines -> operator [](124);
EdtALib9_14 -> Text = MemoLib -> Lines -> operator [](125);

EdtALib10_0 -> Text = MemoLib -> Lines -> operator [](126);
EdtALib10_1 -> Text = MemoLib -> Lines -> operator [](127);
EdtALib10_2 -> Text = MemoLib -> Lines -> operator [](128);
EdtALib10_3 -> Text = MemoLib -> Lines -> operator [](129);
EdtALib10_4 -> Text = MemoLib -> Lines -> operator [](130);
EdtALib10_5 -> Text = MemoLib -> Lines -> operator [](131);
EdtALib10_7 -> Text = MemoLib -> Lines -> operator [](132);
EdtALib10_8 -> Text = MemoLib -> Lines -> operator [](133);
EdtALib10_9 -> Text = MemoLib -> Lines -> operator [](134);
EdtALib10_10 -> Text = MemoLib -> Lines -> operator [](135);
EdtALib10_11 -> Text = MemoLib -> Lines -> operator [](136);
EdtALib10_12 -> Text = MemoLib -> Lines -> operator [](137);
EdtALib10_13 -> Text = MemoLib -> Lines -> operator [](138);
EdtALib10_14 -> Text = MemoLib -> Lines -> operator [](139);

EdtALib11_0 -> Text = MemoLib -> Lines -> operator [](140);
EdtALib11_1 -> Text = MemoLib -> Lines -> operator [](141);
EdtALib11_2 -> Text = MemoLib -> Lines -> operator [](142);
EdtALib11_3 -> Text = MemoLib -> Lines -> operator [](143);
EdtALib11_4 -> Text = MemoLib -> Lines -> operator [](144);
EdtALib11_5 -> Text = MemoLib -> Lines -> operator [](145);
EdtALib11_7 -> Text = MemoLib -> Lines -> operator [](146);
EdtALib11_8 -> Text = MemoLib -> Lines -> operator [](147);
EdtALib11_9 -> Text = MemoLib -> Lines -> operator [](148);
EdtALib11_10 -> Text = MemoLib -> Lines -> operator [](149);
EdtALib11_11 -> Text = MemoLib -> Lines -> operator [](150);
EdtALib11_12 -> Text = MemoLib -> Lines -> operator [](151);
EdtALib11_13 -> Text = MemoLib -> Lines -> operator [](152);
EdtALib11_14 -> Text = MemoLib -> Lines -> operator [](153);

EdtALib12_0 -> Text = MemoLib -> Lines -> operator [](154);
EdtALib12_1 -> Text = MemoLib -> Lines -> operator [](155);
EdtALib12_2 -> Text = MemoLib -> Lines -> operator [](156);
EdtALib12_3 -> Text = MemoLib -> Lines -> operator [](157);
EdtALib12_4 -> Text = MemoLib -> Lines -> operator [](158);
EdtALib12_5 -> Text = MemoLib -> Lines -> operator [](159);
EdtALib12_7 -> Text = MemoLib -> Lines -> operator [](160);
EdtALib12_8 -> Text = MemoLib -> Lines -> operator [](161);
EdtALib12_9 -> Text = MemoLib -> Lines -> operator [](162);
EdtALib12_10 -> Text = MemoLib -> Lines -> operator [](163);
EdtALib12_11 -> Text = MemoLib -> Lines -> operator [](164);
EdtALib12_12 -> Text = MemoLib -> Lines -> operator [](165);
EdtALib12_13 -> Text = MemoLib -> Lines -> operator [](166);
EdtALib12_14 -> Text = MemoLib -> Lines -> operator [](167);

EdtALib13_0 -> Text = MemoLib -> Lines -> operator [](168);
EdtALib13_1 -> Text = MemoLib -> Lines -> operator [](169);
EdtALib13_2 -> Text = MemoLib -> Lines -> operator [](170);
EdtALib13_3 -> Text = MemoLib -> Lines -> operator [](171);
EdtALib13_4 -> Text = MemoLib -> Lines -> operator [](172);
EdtALib13_5 -> Text = MemoLib -> Lines -> operator [](173);
EdtALib13_7 -> Text = MemoLib -> Lines -> operator [](174);
EdtALib13_8 -> Text = MemoLib -> Lines -> operator [](175);
EdtALib13_9 -> Text = MemoLib -> Lines -> operator [](176);
EdtALib13_10 -> Text = MemoLib -> Lines -> operator [](177);
EdtALib13_11 -> Text = MemoLib -> Lines -> operator [](178);
EdtALib13_12 -> Text = MemoLib -> Lines -> operator [](179);
EdtALib13_13 -> Text = MemoLib -> Lines -> operator [](180);
EdtALib13_14 -> Text = MemoLib -> Lines -> operator [](181);

EdtALib14_0 -> Text = MemoLib -> Lines -> operator [](182);
EdtALib14_1 -> Text = MemoLib -> Lines -> operator [](183);
EdtALib14_2 -> Text = MemoLib -> Lines -> operator [](184);
EdtALib14_3 -> Text = MemoLib -> Lines -> operator [](185);
EdtALib14_4 -> Text = MemoLib -> Lines -> operator [](186);
EdtALib14_5 -> Text = MemoLib -> Lines -> operator [](187);
EdtALib14_7 -> Text = MemoLib -> Lines -> operator [](188);
EdtALib14_8 -> Text = MemoLib -> Lines -> operator [](189);
EdtALib14_9 -> Text = MemoLib -> Lines -> operator [](190);
EdtALib14_10 -> Text = MemoLib -> Lines -> operator [](191);
EdtALib14_11 -> Text = MemoLib -> Lines -> operator [](192);
EdtALib14_12 -> Text = MemoLib -> Lines -> operator [](193);
EdtALib14_13 -> Text = MemoLib -> Lines -> operator [](194);
EdtALib14_14 -> Text = MemoLib -> Lines -> operator [](195);

EdtALib15_0 -> Text = MemoLib -> Lines -> operator [](196);
EdtALib15_1 -> Text = MemoLib -> Lines -> operator [](197);
EdtALib15_2 -> Text = MemoLib -> Lines -> operator [](198);
EdtALib15_3 -> Text = MemoLib -> Lines -> operator [](199);
EdtALib15_4 -> Text = MemoLib -> Lines -> operator [](200);
EdtALib15_5 -> Text = MemoLib -> Lines -> operator [](201);
EdtALib15_7 -> Text = MemoLib -> Lines -> operator [](202);
EdtALib15_8 -> Text = MemoLib -> Lines -> operator [](203);
EdtALib15_9 -> Text = MemoLib -> Lines -> operator [](204);
EdtALib15_10 -> Text = MemoLib -> Lines -> operator [](205);
EdtALib15_11 -> Text = MemoLib -> Lines -> operator [](206);
EdtALib15_12 -> Text = MemoLib -> Lines -> operator [](207);
EdtALib15_13 -> Text = MemoLib -> Lines -> operator [](208);
EdtALib15_14 -> Text = MemoLib -> Lines -> operator [](209);

EdtALib16_0 -> Text = MemoLib -> Lines -> operator [](210);
EdtALib16_1 -> Text = MemoLib -> Lines -> operator [](211);
EdtALib16_2 -> Text = MemoLib -> Lines -> operator [](212);
EdtALib16_3 -> Text = MemoLib -> Lines -> operator [](213);
EdtALib16_4 -> Text = MemoLib -> Lines -> operator [](214);
EdtALib16_5 -> Text = MemoLib -> Lines -> operator [](215);
EdtALib16_7 -> Text = MemoLib -> Lines -> operator [](216);
EdtALib16_8 -> Text = MemoLib -> Lines -> operator [](217);
EdtALib16_9 -> Text = MemoLib -> Lines -> operator [](218);
EdtALib16_10 -> Text = MemoLib -> Lines -> operator [](219);
EdtALib16_11 -> Text = MemoLib -> Lines -> operator [](220);
EdtALib16_12 -> Text = MemoLib -> Lines -> operator [](221);
EdtALib16_13 -> Text = MemoLib -> Lines -> operator [](222);
EdtALib16_14 -> Text = MemoLib -> Lines -> operator [](223);

EdtALib17_0 -> Text = MemoLib -> Lines -> operator [](224);
EdtALib17_1 -> Text = MemoLib -> Lines -> operator [](225);
EdtALib17_2 -> Text = MemoLib -> Lines -> operator [](226);
EdtALib17_3 -> Text = MemoLib -> Lines -> operator [](227);
EdtALib17_4 -> Text = MemoLib -> Lines -> operator [](228);
EdtALib17_5 -> Text = MemoLib -> Lines -> operator [](229);
EdtALib17_7 -> Text = MemoLib -> Lines -> operator [](230);
EdtALib17_8 -> Text = MemoLib -> Lines -> operator [](231);
EdtALib17_9 -> Text = MemoLib -> Lines -> operator [](232);
EdtALib17_10 -> Text = MemoLib -> Lines -> operator [](233);
EdtALib17_11 -> Text = MemoLib -> Lines -> operator [](234);
EdtALib17_12 -> Text = MemoLib -> Lines -> operator [](235);
EdtALib17_13 -> Text = MemoLib -> Lines -> operator [](236);
EdtALib17_14 -> Text = MemoLib -> Lines -> operator [](237);

EdtALib18_0 -> Text = MemoLib -> Lines -> operator [](238);
EdtALib18_1 -> Text = MemoLib -> Lines -> operator [](239);
EdtALib18_2 -> Text = MemoLib -> Lines -> operator [](240);
EdtALib18_3 -> Text = MemoLib -> Lines -> operator [](241);
EdtALib18_4 -> Text = MemoLib -> Lines -> operator [](242);
EdtALib18_5 -> Text = MemoLib -> Lines -> operator [](243);
EdtALib18_7 -> Text = MemoLib -> Lines -> operator [](244);
EdtALib18_8 -> Text = MemoLib -> Lines -> operator [](245);
EdtALib18_9 -> Text = MemoLib -> Lines -> operator [](246);
EdtALib18_10 -> Text = MemoLib -> Lines -> operator [](247);
EdtALib18_11 -> Text = MemoLib -> Lines -> operator [](248);
EdtALib18_12 -> Text = MemoLib -> Lines -> operator [](249);
EdtALib18_13 -> Text = MemoLib -> Lines -> operator [](250);
EdtALib18_14 -> Text = MemoLib -> Lines -> operator [](251);

EdtALib19_0 -> Text = MemoLib -> Lines -> operator [](252);
EdtALib19_1 -> Text = MemoLib -> Lines -> operator [](253);
EdtALib19_2 -> Text = MemoLib -> Lines -> operator [](254);
EdtALib19_3 -> Text = MemoLib -> Lines -> operator [](255);
EdtALib19_4 -> Text = MemoLib -> Lines -> operator [](256);
EdtALib19_5 -> Text = MemoLib -> Lines -> operator [](257);
EdtALib19_7 -> Text = MemoLib -> Lines -> operator [](258);
EdtALib19_8 -> Text = MemoLib -> Lines -> operator [](259);
EdtALib19_9 -> Text = MemoLib -> Lines -> operator [](260);
EdtALib19_10 -> Text = MemoLib -> Lines -> operator [](261);
EdtALib19_11 -> Text = MemoLib -> Lines -> operator [](262);
EdtALib19_12 -> Text = MemoLib -> Lines -> operator [](263);
EdtALib19_13 -> Text = MemoLib -> Lines -> operator [](264);
EdtALib19_14 -> Text = MemoLib -> Lines -> operator [](265);

EdtALib20_0 -> Text = MemoLib -> Lines -> operator [](266);
EdtALib20_1 -> Text = MemoLib -> Lines -> operator [](267);
EdtALib20_2 -> Text = MemoLib -> Lines -> operator [](268);
EdtALib20_3 -> Text = MemoLib -> Lines -> operator [](269);
EdtALib20_4 -> Text = MemoLib -> Lines -> operator [](270);
EdtALib20_5 -> Text = MemoLib -> Lines -> operator [](271);
EdtALib20_7 -> Text = MemoLib -> Lines -> operator [](272);
EdtALib20_8 -> Text = MemoLib -> Lines -> operator [](273);
EdtALib20_9 -> Text = MemoLib -> Lines -> operator [](274);
EdtALib20_10 -> Text = MemoLib -> Lines -> operator [](275);
EdtALib20_11 -> Text = MemoLib -> Lines -> operator [](276);
EdtALib20_12 -> Text = MemoLib -> Lines -> operator [](277);
EdtALib20_13 -> Text = MemoLib -> Lines -> operator [](278);
EdtALib20_14 -> Text = MemoLib -> Lines -> operator [](279);

ParametersGrigLib();


BitBtnLoad -> Enabled = true;
}
else
{
// ��������� � ��������� �������� ������������ ������ ��� ������
BitBtnLoad -> Enabled = false;
LblPresentName -> Caption = "���������� ���: ";
}
}

}
//---------------------------------------------------------------------------
//--������� �������������� ���������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryMouseDown(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
libNmb = -1;
}
//---------------------------------------------------------------------------
//--����� ���������� �� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryMouseUp(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{

// ���� ������� �������������� ���������
if ( libNmb == -1 )
{
// ������� ������ �� ���������� ����������
BitBtnLoad -> Enabled = false;
// ����������� ������������ ������������� �� �������
LblPresentName -> Caption = "���������� ���: ";
// ������ ����� � ���������� ������ ���������
ListBoxLibrary -> ItemIndex = -1;
ParametersGrigAutomate(); // ��������� �� �������������� �������

// �������� ������ ������������ ����������
EdtALib1_0 -> Text = "";
EdtALib1_1 -> Text = "";
EdtALib1_2 -> Text = "";
EdtALib1_3 -> Text = "";
EdtALib1_4 -> Text = "";
EdtALib1_5 -> Text = "";
EdtALib1_7 -> Text = "";
EdtALib1_8 -> Text = "";
EdtALib1_9 -> Text = "";
EdtALib1_10 -> Text = "";
EdtALib1_11 -> Text = "";
EdtALib1_12 -> Text = "";
EdtALib1_13 -> Text = "";
EdtALib1_14 -> Text = "";

EdtALib2_0 -> Text = "";
EdtALib2_1 -> Text = "";
EdtALib2_2 -> Text = "";
EdtALib2_3 -> Text = "";
EdtALib2_4 -> Text = "";
EdtALib2_5 -> Text = "";
EdtALib2_7 -> Text = "";
EdtALib2_8 -> Text = "";
EdtALib2_9 -> Text = "";
EdtALib2_10 -> Text = "";
EdtALib2_11 -> Text = "";
EdtALib2_12 -> Text = "";
EdtALib2_13 -> Text = "";
EdtALib2_14 -> Text = "";

EdtALib3_0 -> Text = "";
EdtALib3_1 -> Text = "";
EdtALib3_2 -> Text = "";
EdtALib3_3 -> Text = "";
EdtALib3_4 -> Text = "";
EdtALib3_5 -> Text = "";
EdtALib3_7 -> Text = "";
EdtALib3_8 -> Text = "";
EdtALib3_9 -> Text = "";
EdtALib3_10 -> Text = "";
EdtALib3_11 -> Text = "";
EdtALib3_12 -> Text = "";
EdtALib3_13 -> Text = "";
EdtALib3_14 -> Text = "";

EdtALib4_0 -> Text = "";
EdtALib4_1 -> Text = "";
EdtALib4_2 -> Text = "";
EdtALib4_3 -> Text = "";
EdtALib4_4 -> Text = "";
EdtALib4_5 -> Text = "";
EdtALib4_7 -> Text = "";
EdtALib4_8 -> Text = "";
EdtALib4_9 -> Text = "";
EdtALib4_10 -> Text = "";
EdtALib4_11 -> Text = "";
EdtALib4_12 -> Text = "";
EdtALib4_13 -> Text = "";
EdtALib4_14 -> Text = "";

EdtALib5_0 -> Text = "";
EdtALib5_1 -> Text = "";
EdtALib5_2 -> Text = "";
EdtALib5_3 -> Text = "";
EdtALib5_4 -> Text = "";
EdtALib5_5 -> Text = "";
EdtALib5_7 -> Text = "";
EdtALib5_8 -> Text = "";
EdtALib5_9 -> Text = "";
EdtALib5_10 -> Text = "";
EdtALib5_11 -> Text = "";
EdtALib5_12 -> Text = "";
EdtALib5_13 -> Text = "";
EdtALib5_14 -> Text = "";

EdtALib6_0 -> Text = "";
EdtALib6_1 -> Text = "";
EdtALib6_2 -> Text = "";
EdtALib6_3 -> Text = "";
EdtALib6_4 -> Text = "";
EdtALib6_5 -> Text = "";
EdtALib6_7 -> Text = "";
EdtALib6_8 -> Text = "";
EdtALib6_9 -> Text = "";
EdtALib6_10 -> Text = "";
EdtALib6_11 -> Text = "";
EdtALib6_12 -> Text = "";
EdtALib6_13 -> Text = "";
EdtALib6_14 -> Text = "";

EdtALib7_0 -> Text = "";
EdtALib7_1 -> Text = "";
EdtALib7_2 -> Text = "";
EdtALib7_3 -> Text = "";
EdtALib7_4 -> Text = "";
EdtALib7_5 -> Text = "";
EdtALib7_7 -> Text = "";
EdtALib7_8 -> Text = "";
EdtALib7_9 -> Text = "";
EdtALib7_10 -> Text = "";
EdtALib7_11 -> Text = "";
EdtALib7_12 -> Text = "";
EdtALib7_13 -> Text = "";
EdtALib7_14 -> Text = "";

EdtALib8_0 -> Text = "";
EdtALib8_1 -> Text = "";
EdtALib8_2 -> Text = "";
EdtALib8_3 -> Text = "";
EdtALib8_4 -> Text = "";
EdtALib8_5 -> Text = "";
EdtALib8_7 -> Text = "";
EdtALib8_8 -> Text = "";
EdtALib8_9 -> Text = "";
EdtALib8_10 -> Text = "";
EdtALib8_11 -> Text = "";
EdtALib8_12 -> Text = "";
EdtALib8_13 -> Text = "";
EdtALib8_14 -> Text = "";

EdtALib9_0 -> Text = "";
EdtALib9_1 -> Text = "";
EdtALib9_2 -> Text = "";
EdtALib9_3 -> Text = "";
EdtALib9_4 -> Text = "";
EdtALib9_5 -> Text = "";
EdtALib9_7 -> Text = "";
EdtALib9_8 -> Text = "";
EdtALib9_9 -> Text = "";
EdtALib9_10 -> Text = "";
EdtALib9_11 -> Text = "";
EdtALib9_12 -> Text = "";
EdtALib9_13 -> Text = "";
EdtALib9_14 -> Text = "";

EdtALib10_0 -> Text = "";
EdtALib10_1 -> Text = "";
EdtALib10_2 -> Text = "";
EdtALib10_3 -> Text = "";
EdtALib10_4 -> Text = "";
EdtALib10_5 -> Text = "";
EdtALib10_7 -> Text = "";
EdtALib10_8 -> Text = "";
EdtALib10_9 -> Text = "";
EdtALib10_10 -> Text = "";
EdtALib10_11 -> Text = "";
EdtALib10_12 -> Text = "";
EdtALib10_13 -> Text = "";
EdtALib10_14 -> Text = "";

EdtALib11_0 -> Text = "";
EdtALib11_1 -> Text = "";
EdtALib11_2 -> Text = "";
EdtALib11_3 -> Text = "";
EdtALib11_4 -> Text = "";
EdtALib11_5 -> Text = "";
EdtALib11_7 -> Text = "";
EdtALib11_8 -> Text = "";
EdtALib11_9 -> Text = "";
EdtALib11_10 -> Text = "";
EdtALib11_11 -> Text = "";
EdtALib11_12 -> Text = "";
EdtALib11_13 -> Text = "";
EdtALib11_14 -> Text = "";

EdtALib12_0 -> Text = "";
EdtALib12_1 -> Text = "";
EdtALib12_2 -> Text = "";
EdtALib12_3 -> Text = "";
EdtALib12_4 -> Text = "";
EdtALib12_5 -> Text = "";
EdtALib12_7 -> Text = "";
EdtALib12_8 -> Text = "";
EdtALib12_9 -> Text = "";
EdtALib12_10 -> Text = "";
EdtALib12_11 -> Text = "";
EdtALib12_12 -> Text = "";
EdtALib12_13 -> Text = "";
EdtALib12_14 -> Text = "";

EdtALib13_0 -> Text = "";
EdtALib13_1 -> Text = "";
EdtALib13_2 -> Text = "";
EdtALib13_3 -> Text = "";
EdtALib13_4 -> Text = "";
EdtALib13_5 -> Text = "";
EdtALib13_7 -> Text = "";
EdtALib13_8 -> Text = "";
EdtALib13_9 -> Text = "";
EdtALib13_10 -> Text = "";
EdtALib13_11 -> Text = "";
EdtALib13_12 -> Text = "";
EdtALib13_13 -> Text = "";
EdtALib13_14 -> Text = "";

EdtALib14_0 -> Text = "";
EdtALib14_1 -> Text = "";
EdtALib14_2 -> Text = "";
EdtALib14_3 -> Text = "";
EdtALib14_4 -> Text = "";
EdtALib14_5 -> Text = "";
EdtALib14_7 -> Text = "";
EdtALib14_8 -> Text = "";
EdtALib14_9 -> Text = "";
EdtALib14_10 -> Text = "";
EdtALib14_11 -> Text = "";
EdtALib14_12 -> Text = "";
EdtALib14_13 -> Text = "";
EdtALib14_14 -> Text = "";

EdtALib15_0 -> Text = "";
EdtALib15_1 -> Text = "";
EdtALib15_2 -> Text = "";
EdtALib15_3 -> Text = "";
EdtALib15_4 -> Text = "";
EdtALib15_5 -> Text = "";
EdtALib15_7 -> Text = "";
EdtALib15_8 -> Text = "";
EdtALib15_9 -> Text = "";
EdtALib15_10 -> Text = "";
EdtALib15_11 -> Text = "";
EdtALib15_12 -> Text = "";
EdtALib15_13 -> Text = "";
EdtALib15_14 -> Text = "";

EdtALib16_0 -> Text = "";
EdtALib16_1 -> Text = "";
EdtALib16_2 -> Text = "";
EdtALib16_3 -> Text = "";
EdtALib16_4 -> Text = "";
EdtALib16_5 -> Text = "";
EdtALib16_7 -> Text = "";
EdtALib16_8 -> Text = "";
EdtALib16_9 -> Text = "";
EdtALib16_10 -> Text = "";
EdtALib16_11 -> Text = "";
EdtALib16_12 -> Text = "";
EdtALib16_13 -> Text = "";
EdtALib16_14 -> Text = "";

EdtALib17_0 -> Text = "";
EdtALib17_1 -> Text = "";
EdtALib17_2 -> Text = "";
EdtALib17_3 -> Text = "";
EdtALib17_4 -> Text = "";
EdtALib17_5 -> Text = "";
EdtALib17_7 -> Text = "";
EdtALib17_8 -> Text = "";
EdtALib17_9 -> Text = "";
EdtALib17_10 -> Text = "";
EdtALib17_11 -> Text = "";
EdtALib17_12 -> Text = "";
EdtALib17_13 -> Text = "";
EdtALib17_14 -> Text = "";

EdtALib18_0 -> Text = "";
EdtALib18_1 -> Text = "";
EdtALib18_2 -> Text = "";
EdtALib18_3 -> Text = "";
EdtALib18_4 -> Text = "";
EdtALib18_5 -> Text = "";
EdtALib18_7 -> Text = "";
EdtALib18_8 -> Text = "";
EdtALib18_9 -> Text = "";
EdtALib18_10 -> Text = "";
EdtALib18_11 -> Text = "";
EdtALib18_12 -> Text = "";
EdtALib18_13 -> Text = "";
EdtALib18_14 -> Text = "";

EdtALib19_0 -> Text = "";
EdtALib19_1 -> Text = "";
EdtALib19_2 -> Text = "";
EdtALib19_3 -> Text = "";
EdtALib19_4 -> Text = "";
EdtALib19_5 -> Text = "";
EdtALib19_7 -> Text = "";
EdtALib19_8 -> Text = "";
EdtALib19_9 -> Text = "";
EdtALib19_10 -> Text = "";
EdtALib19_11 -> Text = "";
EdtALib19_12 -> Text = "";
EdtALib19_13 -> Text = "";
EdtALib19_14 -> Text = "";

EdtALib20_0 -> Text = "";
EdtALib20_1 -> Text = "";
EdtALib20_2 -> Text = "";
EdtALib20_3 -> Text = "";
EdtALib20_4 -> Text = "";
EdtALib20_5 -> Text = "";
EdtALib20_7 -> Text = "";
EdtALib20_8 -> Text = "";
EdtALib20_9 -> Text = "";
EdtALib20_10 -> Text = "";
EdtALib20_11 -> Text = "";
EdtALib20_12 -> Text = "";
EdtALib20_13 -> Text = "";
EdtALib20_14 -> Text = "";

}
}
//---------------------------------------------------------------------------
//--������ � ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnSaveClick(TObject *Sender)
{

MemoLib -> Lines -> Clear();

MemoLib -> Lines -> Add ( EdtARed1_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed1_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed2_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed2_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed3_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed3_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed4_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed4_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed5_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed5_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed6_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed6_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed7_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed7_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed8_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed8_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed9_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed9_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed10_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed10_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed11_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed11_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed12_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed12_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed13_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed13_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed14_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed14_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed15_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed15_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed16_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed16_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed17_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed17_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed18_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed18_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed19_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed19_14 -> Text );

MemoLib -> Lines -> Add ( EdtARed20_0 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_1 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_2 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_3 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_4 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_5 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_7 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_8 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_9 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_10 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_11 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_12 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_13 -> Text );
MemoLib -> Lines -> Add ( EdtARed20_14 -> Text );


// ����������� ����������� ����
GBSaveDialog -> Visible = true;

}
//---------------------------------------------------------------------------
//--������ �� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnLoadClick(TObject *Sender)
{

EdtARed1_0 -> Text = EdtALib1_0 -> Text;
EdtARed1_1 -> Text = EdtALib1_1 -> Text;
EdtARed1_2 -> Text = EdtALib1_2 -> Text;
EdtARed1_3 -> Text = EdtALib1_3 -> Text;
EdtARed1_4 -> Text = EdtALib1_4 -> Text;
EdtARed1_5 -> Text = EdtALib1_5 -> Text;
EdtARed1_7 -> Text = EdtALib1_7 -> Text;
EdtARed1_8 -> Text = EdtALib1_8 -> Text;
EdtARed1_9 -> Text = EdtALib1_9 -> Text;
EdtARed1_10 -> Text = EdtALib1_10 -> Text;
EdtARed1_11 -> Text = EdtALib1_11 -> Text;
EdtARed1_12 -> Text = EdtALib1_12 -> Text;
EdtARed1_13 -> Text = EdtALib1_13 -> Text;
if(EdtALib1_14 -> Text == "��") EdtARed1_14 -> ItemIndex = 1;
else EdtARed1_14 -> ItemIndex = 0;

EdtARed2_0 -> Text = EdtALib2_0 -> Text;
EdtARed2_1 -> Text = EdtALib2_1 -> Text;
EdtARed2_2 -> Text = EdtALib2_2 -> Text;
EdtARed2_3 -> Text = EdtALib2_3 -> Text;
EdtARed2_4 -> Text = EdtALib2_4 -> Text;
EdtARed2_5 -> Text = EdtALib2_5 -> Text;
EdtARed2_7 -> Text = EdtALib2_7 -> Text;
EdtARed2_8 -> Text = EdtALib2_8 -> Text;
EdtARed2_9 -> Text = EdtALib2_9 -> Text;
EdtARed2_10 -> Text = EdtALib2_10 -> Text;
EdtARed2_11 -> Text = EdtALib2_11 -> Text;
EdtARed2_12 -> Text = EdtALib2_12 -> Text;
EdtARed2_13 -> Text = EdtALib2_13 -> Text;
if(EdtALib2_14 -> Text == "��") EdtARed2_14 -> ItemIndex = 1;
else EdtARed2_14 -> ItemIndex = 0;

EdtARed3_0 -> Text = EdtALib3_0 -> Text;
EdtARed3_1 -> Text = EdtALib3_1 -> Text;
EdtARed3_2 -> Text = EdtALib3_2 -> Text;
EdtARed3_3 -> Text = EdtALib3_3 -> Text;
EdtARed3_4 -> Text = EdtALib3_4 -> Text;
EdtARed3_5 -> Text = EdtALib3_5 -> Text;
EdtARed3_7 -> Text = EdtALib3_7 -> Text;
EdtARed3_8 -> Text = EdtALib3_8 -> Text;
EdtARed3_9 -> Text = EdtALib3_9 -> Text;
EdtARed3_10 -> Text = EdtALib3_10 -> Text;
EdtARed3_11 -> Text = EdtALib3_11 -> Text;
EdtARed3_12 -> Text = EdtALib3_12 -> Text;
EdtARed3_13 -> Text = EdtALib3_13 -> Text;
if(EdtALib3_14 -> Text == "��") EdtARed3_14 -> ItemIndex = 1;
else EdtARed3_14 -> ItemIndex = 0;

EdtARed4_0 -> Text = EdtALib4_0 -> Text;
EdtARed4_1 -> Text = EdtALib4_1 -> Text;
EdtARed4_2 -> Text = EdtALib4_2 -> Text;
EdtARed4_3 -> Text = EdtALib4_3 -> Text;
EdtARed4_4 -> Text = EdtALib4_4 -> Text;
EdtARed4_5 -> Text = EdtALib4_5 -> Text;
EdtARed4_7 -> Text = EdtALib4_7 -> Text;
EdtARed4_8 -> Text = EdtALib4_8 -> Text;
EdtARed4_9 -> Text = EdtALib4_9 -> Text;
EdtARed4_10 -> Text = EdtALib4_10 -> Text;
EdtARed4_11 -> Text = EdtALib4_11 -> Text;
EdtARed4_12 -> Text = EdtALib4_12 -> Text;
EdtARed4_13 -> Text = EdtALib4_13 -> Text;
if(EdtALib4_14 -> Text == "��") EdtARed4_14 -> ItemIndex = 1;
else EdtARed4_14 -> ItemIndex = 0;

EdtARed5_0 -> Text = EdtALib5_0 -> Text;
EdtARed5_1 -> Text = EdtALib5_1 -> Text;
EdtARed5_2 -> Text = EdtALib5_2 -> Text;
EdtARed5_3 -> Text = EdtALib5_3 -> Text;
EdtARed5_4 -> Text = EdtALib5_4 -> Text;
EdtARed5_5 -> Text = EdtALib5_5 -> Text;
EdtARed5_7 -> Text = EdtALib5_7 -> Text;
EdtARed5_8 -> Text = EdtALib5_8 -> Text;
EdtARed5_9 -> Text = EdtALib5_9 -> Text;
EdtARed5_10 -> Text = EdtALib5_10 -> Text;
EdtARed5_11 -> Text = EdtALib5_11 -> Text;
EdtARed5_12 -> Text = EdtALib5_12 -> Text;
EdtARed5_13 -> Text = EdtALib5_13 -> Text;
if(EdtALib5_14 -> Text == "��") EdtARed5_14 -> ItemIndex = 1;
else EdtARed5_14 -> ItemIndex = 0;

EdtARed6_0 -> Text = EdtALib6_0 -> Text;
EdtARed6_1 -> Text = EdtALib6_1 -> Text;
EdtARed6_2 -> Text = EdtALib6_2 -> Text;
EdtARed6_3 -> Text = EdtALib6_3 -> Text;
EdtARed6_4 -> Text = EdtALib6_4 -> Text;
EdtARed6_5 -> Text = EdtALib6_5 -> Text;
EdtARed6_7 -> Text = EdtALib6_7 -> Text;
EdtARed6_8 -> Text = EdtALib6_8 -> Text;
EdtARed6_9 -> Text = EdtALib6_9 -> Text;
EdtARed6_10 -> Text = EdtALib6_10 -> Text;
EdtARed6_11 -> Text = EdtALib6_11 -> Text;
EdtARed6_12 -> Text = EdtALib6_12 -> Text;
EdtARed6_13 -> Text = EdtALib6_13 -> Text;
if(EdtALib6_14 -> Text == "��") EdtARed6_14 -> ItemIndex = 1;
else EdtARed6_14 -> ItemIndex = 0;

EdtARed7_0 -> Text = EdtALib7_0 -> Text;
EdtARed7_1 -> Text = EdtALib7_1 -> Text;
EdtARed7_2 -> Text = EdtALib7_2 -> Text;
EdtARed7_3 -> Text = EdtALib7_3 -> Text;
EdtARed7_4 -> Text = EdtALib7_4 -> Text;
EdtARed7_5 -> Text = EdtALib7_5 -> Text;
EdtARed7_7 -> Text = EdtALib7_7 -> Text;
EdtARed7_8 -> Text = EdtALib7_8 -> Text;
EdtARed7_9 -> Text = EdtALib7_9 -> Text;
EdtARed7_10 -> Text = EdtALib7_10 -> Text;
EdtARed7_11 -> Text = EdtALib7_11 -> Text;
EdtARed7_12 -> Text = EdtALib7_12 -> Text;
EdtARed7_13 -> Text = EdtALib7_13 -> Text;
if(EdtALib7_14 -> Text == "��") EdtARed7_14 -> ItemIndex = 1;
else EdtARed7_14 -> ItemIndex = 0;

EdtARed8_0 -> Text = EdtALib8_0 -> Text;
EdtARed8_1 -> Text = EdtALib8_1 -> Text;
EdtARed8_2 -> Text = EdtALib8_2 -> Text;
EdtARed8_3 -> Text = EdtALib8_3 -> Text;
EdtARed8_4 -> Text = EdtALib8_4 -> Text;
EdtARed8_5 -> Text = EdtALib8_5 -> Text;
EdtARed8_7 -> Text = EdtALib8_7 -> Text;
EdtARed8_8 -> Text = EdtALib8_8 -> Text;
EdtARed8_9 -> Text = EdtALib8_9 -> Text;
EdtARed8_10 -> Text = EdtALib8_10 -> Text;
EdtARed8_11 -> Text = EdtALib8_11 -> Text;
EdtARed8_12 -> Text = EdtALib8_12 -> Text;
EdtARed8_13 -> Text = EdtALib8_13 -> Text;
if(EdtALib8_14 -> Text == "��") EdtARed8_14 -> ItemIndex = 1;
else EdtARed8_14 -> ItemIndex = 0;

EdtARed9_0 -> Text = EdtALib9_0 -> Text;
EdtARed9_1 -> Text = EdtALib9_1 -> Text;
EdtARed9_2 -> Text = EdtALib9_2 -> Text;
EdtARed9_3 -> Text = EdtALib9_3 -> Text;
EdtARed9_4 -> Text = EdtALib9_4 -> Text;
EdtARed9_5 -> Text = EdtALib9_5 -> Text;
EdtARed9_7 -> Text = EdtALib9_7 -> Text;
EdtARed9_8 -> Text = EdtALib9_8 -> Text;
EdtARed9_9 -> Text = EdtALib9_9 -> Text;
EdtARed9_10 -> Text = EdtALib9_10 -> Text;
EdtARed9_11 -> Text = EdtALib9_11 -> Text;
EdtARed9_12 -> Text = EdtALib9_12 -> Text;
EdtARed9_13 -> Text = EdtALib9_13 -> Text;
if(EdtALib9_14 -> Text == "��") EdtARed9_14 -> ItemIndex = 1;
else EdtARed9_14 -> ItemIndex = 0;

EdtARed10_0 -> Text = EdtALib10_0 -> Text;
EdtARed10_1 -> Text = EdtALib10_1 -> Text;
EdtARed10_2 -> Text = EdtALib10_2 -> Text;
EdtARed10_3 -> Text = EdtALib10_3 -> Text;
EdtARed10_4 -> Text = EdtALib10_4 -> Text;
EdtARed10_5 -> Text = EdtALib10_5 -> Text;
EdtARed10_7 -> Text = EdtALib10_7 -> Text;
EdtARed10_8 -> Text = EdtALib10_8 -> Text;
EdtARed10_9 -> Text = EdtALib10_9 -> Text;
EdtARed10_10 -> Text = EdtALib10_10 -> Text;
EdtARed10_11 -> Text = EdtALib10_11 -> Text;
EdtARed10_12 -> Text = EdtALib10_12 -> Text;
EdtARed10_13 -> Text = EdtALib10_13 -> Text;
if(EdtALib10_14 -> Text == "��") EdtARed10_14 -> ItemIndex = 1;
else EdtARed10_14 -> ItemIndex = 0;

EdtARed11_0 -> Text = EdtALib11_0 -> Text;
EdtARed11_1 -> Text = EdtALib11_1 -> Text;
EdtARed11_2 -> Text = EdtALib11_2 -> Text;
EdtARed11_3 -> Text = EdtALib11_3 -> Text;
EdtARed11_4 -> Text = EdtALib11_4 -> Text;
EdtARed11_5 -> Text = EdtALib11_5 -> Text;
EdtARed11_7 -> Text = EdtALib11_7 -> Text;
EdtARed11_8 -> Text = EdtALib11_8 -> Text;
EdtARed11_9 -> Text = EdtALib11_9 -> Text;
EdtARed11_10 -> Text = EdtALib11_10 -> Text;
EdtARed11_11 -> Text = EdtALib11_11 -> Text;
EdtARed11_12 -> Text = EdtALib11_12 -> Text;
EdtARed11_13 -> Text = EdtALib11_13 -> Text;
if(EdtALib11_14 -> Text == "��") EdtARed11_14 -> ItemIndex = 1;
else EdtARed11_14 -> ItemIndex = 0;

EdtARed12_0 -> Text = EdtALib12_0 -> Text;
EdtARed12_1 -> Text = EdtALib12_1 -> Text;
EdtARed12_2 -> Text = EdtALib12_2 -> Text;
EdtARed12_3 -> Text = EdtALib12_3 -> Text;
EdtARed12_4 -> Text = EdtALib12_4 -> Text;
EdtARed12_5 -> Text = EdtALib12_5 -> Text;
EdtARed12_7 -> Text = EdtALib12_7 -> Text;
EdtARed12_8 -> Text = EdtALib12_8 -> Text;
EdtARed12_9 -> Text = EdtALib12_9 -> Text;
EdtARed12_10 -> Text = EdtALib12_10 -> Text;
EdtARed12_11 -> Text = EdtALib12_11 -> Text;
EdtARed12_12 -> Text = EdtALib12_12 -> Text;
EdtARed12_13 -> Text = EdtALib12_13 -> Text;
if(EdtALib12_14 -> Text == "��") EdtARed12_14 -> ItemIndex = 1;
else EdtARed12_14 -> ItemIndex = 0;

EdtARed13_0 -> Text = EdtALib13_0 -> Text;
EdtARed13_1 -> Text = EdtALib13_1 -> Text;
EdtARed13_2 -> Text = EdtALib13_2 -> Text;
EdtARed13_3 -> Text = EdtALib13_3 -> Text;
EdtARed13_4 -> Text = EdtALib13_4 -> Text;
EdtARed13_5 -> Text = EdtALib13_5 -> Text;
EdtARed13_7 -> Text = EdtALib13_7 -> Text;
EdtARed13_8 -> Text = EdtALib13_8 -> Text;
EdtARed13_9 -> Text = EdtALib13_9 -> Text;
EdtARed13_10 -> Text = EdtALib13_10 -> Text;
EdtARed13_11 -> Text = EdtALib13_11 -> Text;
EdtARed13_12 -> Text = EdtALib13_12 -> Text;
EdtARed13_13 -> Text = EdtALib13_13 -> Text;
if(EdtALib13_14 -> Text == "��") EdtARed13_14 -> ItemIndex = 1;
else EdtARed13_14 -> ItemIndex = 0;

EdtARed14_0 -> Text = EdtALib14_0 -> Text;
EdtARed14_1 -> Text = EdtALib14_1 -> Text;
EdtARed14_2 -> Text = EdtALib14_2 -> Text;
EdtARed14_3 -> Text = EdtALib14_3 -> Text;
EdtARed14_4 -> Text = EdtALib14_4 -> Text;
EdtARed14_5 -> Text = EdtALib14_5 -> Text;
EdtARed14_7 -> Text = EdtALib14_7 -> Text;
EdtARed14_8 -> Text = EdtALib14_8 -> Text;
EdtARed14_9 -> Text = EdtALib14_9 -> Text;
EdtARed14_10 -> Text = EdtALib14_10 -> Text;
EdtARed14_11 -> Text = EdtALib14_11 -> Text;
EdtARed14_12 -> Text = EdtALib14_12 -> Text;
EdtARed14_13 -> Text = EdtALib14_13 -> Text;
if(EdtALib14_14 -> Text == "��") EdtARed14_14 -> ItemIndex = 1;
else EdtARed14_14 -> ItemIndex = 0;

EdtARed15_0 -> Text = EdtALib15_0 -> Text;
EdtARed15_1 -> Text = EdtALib15_1 -> Text;
EdtARed15_2 -> Text = EdtALib15_2 -> Text;
EdtARed15_3 -> Text = EdtALib15_3 -> Text;
EdtARed15_4 -> Text = EdtALib15_4 -> Text;
EdtARed15_5 -> Text = EdtALib15_5 -> Text;
EdtARed15_7 -> Text = EdtALib15_7 -> Text;
EdtARed15_8 -> Text = EdtALib15_8 -> Text;
EdtARed15_9 -> Text = EdtALib15_9 -> Text;
EdtARed15_10 -> Text = EdtALib15_10 -> Text;
EdtARed15_11 -> Text = EdtALib15_11 -> Text;
EdtARed15_12 -> Text = EdtALib15_12 -> Text;
EdtARed15_13 -> Text = EdtALib15_13 -> Text;
if(EdtALib15_14 -> Text == "��") EdtARed15_14 -> ItemIndex = 1;
else EdtARed15_14 -> ItemIndex = 0;

EdtARed16_0 -> Text = EdtALib16_0 -> Text;
EdtARed16_1 -> Text = EdtALib16_1 -> Text;
EdtARed16_2 -> Text = EdtALib16_2 -> Text;
EdtARed16_3 -> Text = EdtALib16_3 -> Text;
EdtARed16_4 -> Text = EdtALib16_4 -> Text;
EdtARed16_5 -> Text = EdtALib16_5 -> Text;
EdtARed16_7 -> Text = EdtALib16_7 -> Text;
EdtARed16_8 -> Text = EdtALib16_8 -> Text;
EdtARed16_9 -> Text = EdtALib16_9 -> Text;
EdtARed16_10 -> Text = EdtALib16_10 -> Text;
EdtARed16_11 -> Text = EdtALib16_11 -> Text;
EdtARed16_12 -> Text = EdtALib16_12 -> Text;
EdtARed16_13 -> Text = EdtALib16_13 -> Text;
if(EdtALib16_14 -> Text == "��") EdtARed16_14 -> ItemIndex = 1;
else EdtARed16_14 -> ItemIndex = 0;

EdtARed17_0 -> Text = EdtALib17_0 -> Text;
EdtARed17_1 -> Text = EdtALib17_1 -> Text;
EdtARed17_2 -> Text = EdtALib17_2 -> Text;
EdtARed17_3 -> Text = EdtALib17_3 -> Text;
EdtARed17_4 -> Text = EdtALib17_4 -> Text;
EdtARed17_5 -> Text = EdtALib17_5 -> Text;
EdtARed17_7 -> Text = EdtALib17_7 -> Text;
EdtARed17_8 -> Text = EdtALib17_8 -> Text;
EdtARed17_9 -> Text = EdtALib17_9 -> Text;
EdtARed17_10 -> Text = EdtALib17_10 -> Text;
EdtARed17_11 -> Text = EdtALib17_11 -> Text;
EdtARed17_12 -> Text = EdtALib17_12 -> Text;
EdtARed17_13 -> Text = EdtALib17_13 -> Text;
if(EdtALib17_14 -> Text == "��") EdtARed17_14 -> ItemIndex = 1;
else EdtARed17_14 -> ItemIndex = 0;

EdtARed18_0 -> Text = EdtALib18_0 -> Text;
EdtARed18_1 -> Text = EdtALib18_1 -> Text;
EdtARed18_2 -> Text = EdtALib18_2 -> Text;
EdtARed18_3 -> Text = EdtALib18_3 -> Text;
EdtARed18_4 -> Text = EdtALib18_4 -> Text;
EdtARed18_5 -> Text = EdtALib18_5 -> Text;
EdtARed18_7 -> Text = EdtALib18_7 -> Text;
EdtARed18_8 -> Text = EdtALib18_8 -> Text;
EdtARed18_9 -> Text = EdtALib18_9 -> Text;
EdtARed18_10 -> Text = EdtALib18_10 -> Text;
EdtARed18_11 -> Text = EdtALib18_11 -> Text;
EdtARed18_12 -> Text = EdtALib18_12 -> Text;
EdtARed18_13 -> Text = EdtALib18_13 -> Text;
if(EdtALib18_14 -> Text == "��") EdtARed18_14 -> ItemIndex = 1;
else EdtARed18_14 -> ItemIndex = 0;

EdtARed19_0 -> Text = EdtALib19_0 -> Text;
EdtARed19_1 -> Text = EdtALib19_1 -> Text;
EdtARed19_2 -> Text = EdtALib19_2 -> Text;
EdtARed19_3 -> Text = EdtALib19_3 -> Text;
EdtARed19_4 -> Text = EdtALib19_4 -> Text;
EdtARed19_5 -> Text = EdtALib19_5 -> Text;
EdtARed19_7 -> Text = EdtALib19_7 -> Text;
EdtARed19_8 -> Text = EdtALib19_8 -> Text;
EdtARed19_9 -> Text = EdtALib19_9 -> Text;
EdtARed19_10 -> Text = EdtALib19_10 -> Text;
EdtARed19_11 -> Text = EdtALib19_11 -> Text;
EdtARed19_12 -> Text = EdtALib19_12 -> Text;
EdtARed19_13 -> Text = EdtALib19_13 -> Text;
if(EdtALib19_14 -> Text == "��") EdtARed19_14 -> ItemIndex = 1;
else EdtARed19_14 -> ItemIndex = 0;

EdtARed20_0 -> Text = EdtALib20_0 -> Text;
EdtARed20_1 -> Text = EdtALib20_1 -> Text;
EdtARed20_2 -> Text = EdtALib20_2 -> Text;
EdtARed20_3 -> Text = EdtALib20_3 -> Text;
EdtARed20_4 -> Text = EdtALib20_4 -> Text;
EdtARed20_5 -> Text = EdtALib20_5 -> Text;
EdtARed20_7 -> Text = EdtALib20_7 -> Text;
EdtARed20_8 -> Text = EdtALib20_8 -> Text;
EdtARed20_9 -> Text = EdtALib20_9 -> Text;
EdtARed20_10 -> Text = EdtALib20_10 -> Text;
EdtARed20_11 -> Text = EdtALib20_11 -> Text;
EdtARed20_12 -> Text = EdtALib20_12 -> Text;
EdtARed20_13 -> Text = EdtALib20_13 -> Text;
if(EdtALib20_14 -> Text == "��") EdtARed20_14 -> ItemIndex = 1;
else EdtARed20_14 -> ItemIndex = 0;


EdtARed1_0 -> Color = clSilver;
EdtARed1_1 -> Color = clSilver;
EdtARed1_2 -> Color = clSilver;
EdtARed1_3 -> Color = clSilver;
EdtARed1_4 -> Color = clSilver;
EdtARed1_5 -> Color = clSilver;
EdtARed1_7 -> Color = clSilver;
EdtARed1_8 -> Color = clSilver;
EdtARed1_9 -> Color = clSilver;
EdtARed1_10 -> Color = clSilver;
EdtARed1_11 -> Color = clSilver;
EdtARed1_12 -> Color = clSilver;
EdtARed1_13 -> Color = clSilver;
EdtARed1_14 -> Color = clSilver;

EdtARed2_0 -> Color = clSilver;
EdtARed2_1 -> Color = clSilver;
EdtARed2_2 -> Color = clSilver;
EdtARed2_3 -> Color = clSilver;
EdtARed2_4 -> Color = clSilver;
EdtARed2_5 -> Color = clSilver;
EdtARed2_7 -> Color = clSilver;
EdtARed2_8 -> Color = clSilver;
EdtARed2_9 -> Color = clSilver;
EdtARed2_10 -> Color = clSilver;
EdtARed2_11 -> Color = clSilver;
EdtARed2_12 -> Color = clSilver;
EdtARed2_13 -> Color = clSilver;
EdtARed2_14 -> Color = clSilver;

EdtARed3_0 -> Color = clSilver;
EdtARed3_1 -> Color = clSilver;
EdtARed3_2 -> Color = clSilver;
EdtARed3_3 -> Color = clSilver;
EdtARed3_4 -> Color = clSilver;
EdtARed3_5 -> Color = clSilver;
EdtARed3_7 -> Color = clSilver;
EdtARed3_8 -> Color = clSilver;
EdtARed3_9 -> Color = clSilver;
EdtARed3_10 -> Color = clSilver;
EdtARed3_11 -> Color = clSilver;
EdtARed3_12 -> Color = clSilver;
EdtARed3_13 -> Color = clSilver;
EdtARed3_14 -> Color = clSilver;

EdtARed4_0 -> Color = clSilver;
EdtARed4_1 -> Color = clSilver;
EdtARed4_2 -> Color = clSilver;
EdtARed4_3 -> Color = clSilver;
EdtARed4_4 -> Color = clSilver;
EdtARed4_5 -> Color = clSilver;
EdtARed4_7 -> Color = clSilver;
EdtARed4_8 -> Color = clSilver;
EdtARed4_9 -> Color = clSilver;
EdtARed4_10 -> Color = clSilver;
EdtARed4_11 -> Color = clSilver;
EdtARed4_12 -> Color = clSilver;
EdtARed4_13 -> Color = clSilver;
EdtARed4_14 -> Color = clSilver;

EdtARed5_0 -> Color = clSilver;
EdtARed5_1 -> Color = clSilver;
EdtARed5_2 -> Color = clSilver;
EdtARed5_3 -> Color = clSilver;
EdtARed5_4 -> Color = clSilver;
EdtARed5_5 -> Color = clSilver;
EdtARed5_7 -> Color = clSilver;
EdtARed5_8 -> Color = clSilver;
EdtARed5_9 -> Color = clSilver;
EdtARed5_10 -> Color = clSilver;
EdtARed5_11 -> Color = clSilver;
EdtARed5_12 -> Color = clSilver;
EdtARed5_13 -> Color = clSilver;
EdtARed5_14 -> Color = clSilver;

EdtARed6_0 -> Color = clSilver;
EdtARed6_1 -> Color = clSilver;
EdtARed6_2 -> Color = clSilver;
EdtARed6_3 -> Color = clSilver;
EdtARed6_4 -> Color = clSilver;
EdtARed6_5 -> Color = clSilver;
EdtARed6_7 -> Color = clSilver;
EdtARed6_8 -> Color = clSilver;
EdtARed6_9 -> Color = clSilver;
EdtARed6_10 -> Color = clSilver;
EdtARed6_11 -> Color = clSilver;
EdtARed6_12 -> Color = clSilver;
EdtARed6_13 -> Color = clSilver;
EdtARed6_14 -> Color = clSilver;

EdtARed7_0 -> Color = clSilver;
EdtARed7_1 -> Color = clSilver;
EdtARed7_2 -> Color = clSilver;
EdtARed7_3 -> Color = clSilver;
EdtARed7_4 -> Color = clSilver;
EdtARed7_5 -> Color = clSilver;
EdtARed7_7 -> Color = clSilver;
EdtARed7_8 -> Color = clSilver;
EdtARed7_9 -> Color = clSilver;
EdtARed7_10 -> Color = clSilver;
EdtARed7_11 -> Color = clSilver;
EdtARed7_12 -> Color = clSilver;
EdtARed7_13 -> Color = clSilver;
EdtARed7_14 -> Color = clSilver;

EdtARed8_0 -> Color = clSilver;
EdtARed8_1 -> Color = clSilver;
EdtARed8_2 -> Color = clSilver;
EdtARed8_3 -> Color = clSilver;
EdtARed8_4 -> Color = clSilver;
EdtARed8_5 -> Color = clSilver;
EdtARed8_7 -> Color = clSilver;
EdtARed8_8 -> Color = clSilver;
EdtARed8_9 -> Color = clSilver;
EdtARed8_10 -> Color = clSilver;
EdtARed8_11 -> Color = clSilver;
EdtARed8_12 -> Color = clSilver;
EdtARed8_13 -> Color = clSilver;
EdtARed8_14 -> Color = clSilver;

EdtARed9_0 -> Color = clSilver;
EdtARed9_1 -> Color = clSilver;
EdtARed9_2 -> Color = clSilver;
EdtARed9_3 -> Color = clSilver;
EdtARed9_4 -> Color = clSilver;
EdtARed9_5 -> Color = clSilver;
EdtARed9_7 -> Color = clSilver;
EdtARed9_8 -> Color = clSilver;
EdtARed9_9 -> Color = clSilver;
EdtARed9_10 -> Color = clSilver;
EdtARed9_11 -> Color = clSilver;
EdtARed9_12 -> Color = clSilver;
EdtARed9_13 -> Color = clSilver;
EdtARed9_14 -> Color = clSilver;

EdtARed10_0 -> Color = clSilver;
EdtARed10_1 -> Color = clSilver;
EdtARed10_2 -> Color = clSilver;
EdtARed10_3 -> Color = clSilver;
EdtARed10_4 -> Color = clSilver;
EdtARed10_5 -> Color = clSilver;
EdtARed10_7 -> Color = clSilver;
EdtARed10_8 -> Color = clSilver;
EdtARed10_9 -> Color = clSilver;
EdtARed10_10 -> Color = clSilver;
EdtARed10_11 -> Color = clSilver;
EdtARed10_12 -> Color = clSilver;
EdtARed10_13 -> Color = clSilver;
EdtARed10_14 -> Color = clSilver;

EdtARed11_0 -> Color = clSilver;
EdtARed11_1 -> Color = clSilver;
EdtARed11_2 -> Color = clSilver;
EdtARed11_3 -> Color = clSilver;
EdtARed11_4 -> Color = clSilver;
EdtARed11_5 -> Color = clSilver;
EdtARed11_7 -> Color = clSilver;
EdtARed11_8 -> Color = clSilver;
EdtARed11_9 -> Color = clSilver;
EdtARed11_10 -> Color = clSilver;
EdtARed11_11 -> Color = clSilver;
EdtARed11_12 -> Color = clSilver;
EdtARed11_13 -> Color = clSilver;
EdtARed11_14 -> Color = clSilver;

EdtARed12_0 -> Color = clSilver;
EdtARed12_1 -> Color = clSilver;
EdtARed12_2 -> Color = clSilver;
EdtARed12_3 -> Color = clSilver;
EdtARed12_4 -> Color = clSilver;
EdtARed12_5 -> Color = clSilver;
EdtARed12_7 -> Color = clSilver;
EdtARed12_8 -> Color = clSilver;
EdtARed12_9 -> Color = clSilver;
EdtARed12_10 -> Color = clSilver;
EdtARed12_11 -> Color = clSilver;
EdtARed12_12 -> Color = clSilver;
EdtARed12_13 -> Color = clSilver;
EdtARed12_14 -> Color = clSilver;

EdtARed13_0 -> Color = clSilver;
EdtARed13_1 -> Color = clSilver;
EdtARed13_2 -> Color = clSilver;
EdtARed13_3 -> Color = clSilver;
EdtARed13_4 -> Color = clSilver;
EdtARed13_5 -> Color = clSilver;
EdtARed13_7 -> Color = clSilver;
EdtARed13_8 -> Color = clSilver;
EdtARed13_9 -> Color = clSilver;
EdtARed13_10 -> Color = clSilver;
EdtARed13_11 -> Color = clSilver;
EdtARed13_12 -> Color = clSilver;
EdtARed13_13 -> Color = clSilver;
EdtARed13_14 -> Color = clSilver;

EdtARed14_0 -> Color = clSilver;
EdtARed14_1 -> Color = clSilver;
EdtARed14_2 -> Color = clSilver;
EdtARed14_3 -> Color = clSilver;
EdtARed14_4 -> Color = clSilver;
EdtARed14_5 -> Color = clSilver;
EdtARed14_7 -> Color = clSilver;
EdtARed14_8 -> Color = clSilver;
EdtARed14_9 -> Color = clSilver;
EdtARed14_10 -> Color = clSilver;
EdtARed14_11 -> Color = clSilver;
EdtARed14_12 -> Color = clSilver;
EdtARed14_13 -> Color = clSilver;
EdtARed14_14 -> Color = clSilver;

EdtARed15_0 -> Color = clSilver;
EdtARed15_1 -> Color = clSilver;
EdtARed15_2 -> Color = clSilver;
EdtARed15_3 -> Color = clSilver;
EdtARed15_4 -> Color = clSilver;
EdtARed15_5 -> Color = clSilver;
EdtARed15_7 -> Color = clSilver;
EdtARed15_8 -> Color = clSilver;
EdtARed15_9 -> Color = clSilver;
EdtARed15_10 -> Color = clSilver;
EdtARed15_11 -> Color = clSilver;
EdtARed15_12 -> Color = clSilver;
EdtARed15_13 -> Color = clSilver;
EdtARed15_14 -> Color = clSilver;

EdtARed16_0 -> Color = clSilver;
EdtARed16_1 -> Color = clSilver;
EdtARed16_2 -> Color = clSilver;
EdtARed16_3 -> Color = clSilver;
EdtARed16_4 -> Color = clSilver;
EdtARed16_5 -> Color = clSilver;
EdtARed16_7 -> Color = clSilver;
EdtARed16_8 -> Color = clSilver;
EdtARed16_9 -> Color = clSilver;
EdtARed16_10 -> Color = clSilver;
EdtARed16_11 -> Color = clSilver;
EdtARed16_12 -> Color = clSilver;
EdtARed16_13 -> Color = clSilver;
EdtARed16_14 -> Color = clSilver;

EdtARed17_0 -> Color = clSilver;
EdtARed17_1 -> Color = clSilver;
EdtARed17_2 -> Color = clSilver;
EdtARed17_3 -> Color = clSilver;
EdtARed17_4 -> Color = clSilver;
EdtARed17_5 -> Color = clSilver;
EdtARed17_7 -> Color = clSilver;
EdtARed17_8 -> Color = clSilver;
EdtARed17_9 -> Color = clSilver;
EdtARed17_10 -> Color = clSilver;
EdtARed17_11 -> Color = clSilver;
EdtARed17_12 -> Color = clSilver;
EdtARed17_13 -> Color = clSilver;
EdtARed17_14 -> Color = clSilver;

EdtARed18_0 -> Color = clSilver;
EdtARed18_1 -> Color = clSilver;
EdtARed18_2 -> Color = clSilver;
EdtARed18_3 -> Color = clSilver;
EdtARed18_4 -> Color = clSilver;
EdtARed18_5 -> Color = clSilver;
EdtARed18_7 -> Color = clSilver;
EdtARed18_8 -> Color = clSilver;
EdtARed18_9 -> Color = clSilver;
EdtARed18_10 -> Color = clSilver;
EdtARed18_11 -> Color = clSilver;
EdtARed18_12 -> Color = clSilver;
EdtARed18_13 -> Color = clSilver;
EdtARed18_14 -> Color = clSilver;

EdtARed19_0 -> Color = clSilver;
EdtARed19_1 -> Color = clSilver;
EdtARed19_2 -> Color = clSilver;
EdtARed19_3 -> Color = clSilver;
EdtARed19_4 -> Color = clSilver;
EdtARed19_5 -> Color = clSilver;
EdtARed19_7 -> Color = clSilver;
EdtARed19_8 -> Color = clSilver;
EdtARed19_9 -> Color = clSilver;
EdtARed19_10 -> Color = clSilver;
EdtARed19_11 -> Color = clSilver;
EdtARed19_12 -> Color = clSilver;
EdtARed19_13 -> Color = clSilver;
EdtARed19_14 -> Color = clSilver;

EdtARed20_0 -> Color = clSilver;
EdtARed20_1 -> Color = clSilver;
EdtARed20_2 -> Color = clSilver;
EdtARed20_3 -> Color = clSilver;
EdtARed20_4 -> Color = clSilver;
EdtARed20_5 -> Color = clSilver;
EdtARed20_7 -> Color = clSilver;
EdtARed20_8 -> Color = clSilver;
EdtARed20_9 -> Color = clSilver;
EdtARed20_10 -> Color = clSilver;
EdtARed20_11 -> Color = clSilver;
EdtARed20_12 -> Color = clSilver;
EdtARed20_13 -> Color = clSilver;
EdtARed20_14 -> Color = clSilver;

}
//---------------------------------------------------------------------------
//--�������� ���������� ���������� � ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnSaveNoClick(TObject *Sender)
{
EdtNewName -> Text = "";
GBSaveDialog -> Visible = false;
}
//---------------------------------------------------------------------------
//--������������� ���������� ���������� � ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnSaveYesClick(TObject *Sender)
{
// �������� �� ������������� ����������
if ( ! DirectoryExists("LIB") ) CreateDir("LIB");
AnsiString fileName = "LIB\\" + EdtNewName -> Text + ".txt";
if ( ! FileExists( fileName ) )
{
// ������������ �������� ����������� �����
if ( MessageDlg("������������� ������ ���������?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
// ���� �������� ��������� ������ ������������
if ( libNmb != -1 )
DeleteFileA( "LIB\\" + ListBoxLibrary -> Items -> operator[](libNmb) + ".txt" );
int fileID = FileCreate ( fileName );
FileClose( fileID );
}
else
{
if ( MessageDlg("���� � �������� ��������� ��� ����������, ������������?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
}
MemoLib -> Lines -> SaveToFile( "LIB\\" + EdtNewName -> Text + ".txt" );
GBSaveDialog -> Visible = false;

// ����� �������� ������
int
fileCount = 0,
rezult;
TSearchRec SR;
// ����������
fileCount = 0;
Form1 -> ListBoxLibrary -> Clear();
rezult = FindFirst("LIB\\*.txt", faAnyFile, SR);
while ( !rezult ){
fileCount++;
SR.Name.SetLength( SR.Name.Length() - 4 );
Form1 -> ListBoxLibrary -> Items -> Add( SR.Name );
rezult = FindNext(SR);
};
Form1 -> ListBoxLibrary -> Sorted;
EdtNewName -> Text = "";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//--������������ ���������� �������--//
//---------------------------------------------------------------------------
void TForm1::VisualParR()
{
    EdtRKon0_0 -> Text = FloatToStrF((float)par[0][0]/4095.0*RRG1_MAX, ffFixed, 5, 1);      //������ ���1
    EdtRKon0_1 -> Text = FloatToStrF((float)par[0][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //������ ���2
    EdtRKon0_2 -> Text = FloatToStrF((float)par[0][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //������ ���3
    EdtRKon0_3 -> Text = FloatToStrF((float)par[0][3]/4095.0*RRG4_MAX, ffFixed, 5, 1);      //������ ���4
    EdtRKon0_4 -> Text = FloatToStrF((float)par[0][4]/4095.0*RRG5_MAX, ffFixed, 5, 1);      //������ ���5
    EdtRKon0_5 -> Text = FloatToStrF((float)par[0][5]/4095.0*RRG6_MAX, ffFixed, 5, 1);      //������ ���6
    EdtRKon0_6 -> Text = FloatToStrF((float)par[0][6]/4095.0*RRG7_MAX, ffFixed, 5, 1);      //������ ���7
    EdtRKon0_8 -> Text = FloatToStrF((float)par[0][8]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);  //�������� ��� ��
    EdtRKon0_11 -> Text = FloatToStrF((float)par[0][11]/4095.0*CESAR_MAX_PD, ffFixed, 5, 0);//�������� ��� �/�
    EdtRKon0_12 -> Text = FloatToStrF((float)par[0][12]/4095.0*SMESH_MAX_USER, ffFixed, 5, 0);//��������
    EdtRKon0_7 -> Text = FloatToStrF((float)par[0][7]/10000.0*DAVL_MAX, ffFixed, 5, 1);     //��������
    EdtRKon0_15 -> Text = FloatToStrF((float)par[0][15], ffFixed, 5, 0);                    //����������� ������������
    EdtRKon0_17 -> Text = FloatToStrF((float)par[0][17], ffFixed, 5, 0);                    //������� ������������
    EdtRKon0_18 -> Text = FloatToStrF((float)par[0][18], ffFixed, 5, 0);                    //����������� �������
    if(par[0][16]==0)      { EdtRKon0_16->Text="�������";  } //�������� ������������
    else if(par[0][16]==1) { EdtRKon0_16->Text="�����";    } //�������� ������������
    else if(par[0][16]==2) { EdtRKon0_16->Text="��������"; } //�������� ������������

}

//---------------------------------------------------------------------------
//--������������ �������� �������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::PCPeremChange(TObject *Sender)
{
    // ��� �� ������������ ���� ������
    VisualDebug();
    // ������� �������� ������ ����������� ����������
    PnlDebugValues -> Parent = PCPerem -> ActivePage;
    // �������� �������� ���������� ���������
    EditOTLzad1 -> Text = "";
    EditOTLzad2 -> Text = "";
    EditOTLzad3 -> Text = "";
    EditOTLzad4 -> Text = "";
    EditOTLzad5 -> Text = "";
    EditOTLzad6 -> Text = "";
    EditOTLzad7 -> Text = "";
    EditOTLzad8 -> Text = "";
    EditOTLzad9 -> Text = "";
    EditOTLzad10-> Text = "";
    EditOTLzad11-> Text = "";
    EditOTLzad12-> Text = "";
    EditOTLzad13-> Text = "";
    EditOTLzad14-> Text = "";
    EditOTLzad15-> Text = "";
    EditOTLzad16-> Text = "";
    EditOTLzad17 -> Text = "";
    EditOTLzad18 -> Text = "";
    EditOTLzad19 -> Text = "";
    EditOTLzad20 -> Text = "";
    EditOTLzad21 -> Text = "";
    EditOTLzad22 -> Text = "";
    EditOTLzad23 -> Text = "";
    EditOTLzad24 -> Text = "";
    EditOTLzad25 -> Text = "";
    EditOTLzad26-> Text = "";
    EditOTLzad27-> Text = "";
    EditOTLzad28-> Text = "";
    EditOTLzad29-> Text = "";
    EditOTLzad30-> Text = "";
}
//---------------------------------------------------------------------------
void TForm1::VisualDebug()
{
    TEdit *EdtDebugValues[30] =
    {
            EditOTLnam1,
            EditOTLnam2,
            EditOTLnam3,
            EditOTLnam4,
            EditOTLnam5,
            EditOTLnam6,
            EditOTLnam7,
            EditOTLnam8,
            EditOTLnam9,
            EditOTLnam10,
            EditOTLnam11,
            EditOTLnam12,
            EditOTLnam13,
            EditOTLnam14,
            EditOTLnam15,
            EditOTLnam16,
            EditOTLnam17,
            EditOTLnam18,
            EditOTLnam19,
            EditOTLnam20,
            EditOTLnam21,
            EditOTLnam22,
            EditOTLnam23,
            EditOTLnam24,
            EditOTLnam25,
            EditOTLnam26,
            EditOTLnam27,
            EditOTLnam28,
            EditOTLnam29,
            EditOTLnam30
    };

AnsiString valuesNames[] =
    {
    	//0 ��������

	"sh_",
	"shr[1]",
	"sh[1]",
	"shr[2]",
	"sh[2]",
	"shr[3]",
	"sh[3]",
	"shr[4]",
	"sh[4]",
	"shr[5]",
	"sh[5]",
	"shr[6]",
	"sh[6]",
	"shr[7]",
	"sh[7]",
	"shr[8]",
	"sh[8]",
	"shr[9]",
	"sh[9]",
	"shr[10]",
	"sh[10]",
	"shr[11]",
	"sh[11]",
	"shr[12]",
	"sh[12]",
	"shr[13]",
	"sh[13]",
	"shr[14]",
	"sh[14]",
	"shr[15]",
	//1 ��������

	"sh[15]",
	"shr[17]",
	"sh[17]",
	"shr[18]",
	"sh[18]",
	"shr[19]",
	"sh[19]",
	"shr[20]",
	"sh[20]",
	"shr[21]",
	"sh[21]",
	"shr[22]",
	"sh[22]",
	"shr[23]",
	"sh[23]",
	"shr[24]",
	"sh[24]",
	"shr[25]",
	"sh[25]",
	"shr[26]",
	"sh[26]",
	"shr[27]",
	"sh[27]",
	"shr[28]",
	"sh[28]",
    "shr[29]",
	"sh[29]",
	"shr[30]",
	"sh[30]",
	"shr[31]",

	//2 ��������

    "sh[31]",
	"shr[32]",
	"sh[32]",
	"shr[33]",
	"sh[33]",
	"shr[34]",
	"sh[34]",
	"shr[35]",
	"sh[35]",
	"shr[36]",
	"sh[36]",
	"shr[37]",
	"sh[37]",
	"shr[38]",
	"sh[38]",
	"shr[39]",
	"sh[39]",
	"shr[40]",
	"sh[40]",
	"zshr3",
	"norma",
	"qkk",
	"diagn[0]",
	"diagn[1]",
	"diagn[2]",
	"diagn[3]",
	"diagn[4]",
	"diagn[5]",
	"diagn[6]",
	"diagn[7]",

	//3 ��������

    "diagn[8]",
	"diagn[9]",
	"diagn[10]",
	"diagn[11]",
	"diagn[12]",
	"diagn[13]",
	"diagn[14]",
	"diagn[15]",
	"diagn[16]",
	"diagn[17]",
	"diagn[18]",
	"diagn[19]",
	"diagn[20]",
	"diagn[21]",
	"diagn[22]",
	"diagn[23]",
	"diagn[24]",
	"diagn[25]",
	"diagn[26]",
	"diagn[27]",
	"diagn[28]",
	"diagnS[0]",
	"diagnS[1]",
	"diagnS[2]",
	"out[0]",
	"out[1]",
	"out[2]",
	"out[3]",
	"zin[0]",
	"zin[1]",
	//4 ��������

	"zin[2]",
	"zin[3]",
	"zin[4]",
	"aik[0]",
	"aik[1]",
	"aik[2]",
	"aik[3]",
	"aik[4]",
	"aik[5]",
	"aik[6]",
	"aik[7]",
	"aik[8]",
	"aik[9]",
	"aik[10]",
	"aik[11]",
	"aik[12]",
	"aik[13]",
	"aik[14]",
	"aik[15]",
	"aik[16]",
	"aik[17]",
    "aik[18]",
	"aout[0]",
	"aout[1]",
	"aout[2]",
	"aout[3]",
	"aout[4]",
	"aout[5]",
	"aout[6]",
	"aout[7]",
    //5 ��������

    "aout[8]",
	"aout[9]",
	"aout[10]",
	"D_D1",
	"D_D2",
	"D_D3",
	"D_D4",
	"D_D5",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	"POROG_DAVL",
	"UVAKV_KAM",
	"UVAKN_KAM",
	"UVAKN_TMN",
	"UVAKV_TMN",
	"UVAK_SHL",
	"UVAK_SHL_MO",
	"UVAK_SHL_MN",
	"UVAK_ZTMN",
	"UATM_D1",
	"UATM_D4",
	" ",
	"nasmod[0]",
	"nasmod[1]",
	"nasmod[2]",
	//6 ��������

	"nasmod[3]",
	"nasmod[4]",
	"nasmod[5]",
	"nasmod[6]",
	"nasmod[7]",
	"nasmod[8]",
	"nasmod[9]",
	"nasmod[10]",
	"nasmod[11]",
	"nasmod[12]",
	"nasmod[13]",
	"nasmod[14]",
	"nasmod[15]",
	"nasmod[16]",
	"nasmod[17]",
	"",
	"par_t[0]",
	"par_t[1]",
	"par_t[2]",
	"par_t[3]",
	"par_t[4]",
	"par_t[5]",
	"par[0][0]",
	"par[0][1]",
	"par[0][2]",
	"par[0][3]",
	"par[0][4]",
	"par[0][5]",
	"par[0][6]",
	"par[0][7]",
	//7 ��������

	"par[0][8]",
	"par[0][9]",
	"par[0][10]",
	"par[0][11]",
	"par[0][12]",
	"par[0][13]",
	"par[0][14]",
	"par[0][15]",
	"par[0][16]",
	"par[0][17]",
	"par[0][18]",
    "par[0][20]",
	"par[1][0]",
	"par[1][1]",
	"par[1][2]",
	"par[1][3]",
	"par[1][4]",
	"par[1][5]",
	"par[1][6]",
	"par[1][7]",
	"par[1][8]",
	"par[1][9]",
	"par[1][10]",
	"par[1][11]",
	"par[1][12]",
	"par[1][13]",
	"par[1][14]",
	"par[2][0]",
	"par[2][1]",
	"par[2][2]",
	
	//8 ��������
    "par[2][3]",
	"par[2][4]",
	"par[2][5]",
	"par[2][6]",
	"par[2][7]",
	"par[2][8]",
	"par[2][9]",
	"par[2][10]",
	"par[2][11]",
	"par[2][12]",
	"par[2][13]",
	"par[2][14]",
	"par[3][0]",
	"par[3][1]",
	"par[3][2]",
	"par[3][3]",
	"par[3][4]",
	"par[3][5]",
	"par[3][6]",
	"par[3][7]",
	"par[3][8]",
	"par[3][9]",
	"par[3][10]",
	"par[3][11]",
	"par[3][12]",
	"par[3][13]",
	"par[3][14]",
	"par[4][0]",
	"par[4][1]",
	"par[4][2]",
	
	//9 ��������
    "par[4][3]",
	"par[4][4]",
	"par[4][5]",
	"par[4][6]",
	"par[4][7]",
	"par[4][8]",
	"par[4][9]",
	"par[4][10]",
	"par[4][11]",
	"par[4][12]",
	"par[4][13]",
	"par[4][14]",
	"par[5][0]",
	"par[5][1]",
	"par[5][2]",
	"par[5][3]",
	"par[5][4]",
	"par[5][5]",
	"par[5][6]",
	"par[5][7]",
	"par[5][8]",
	"par[5][9]",
	"par[5][10]",
	"par[5][11]",
	"par[5][12]",
	"par[5][13]",
	"par[5][14]",
	"par[6][0]",
	"par[6][1]",
	"par[6][2]",
	
	//10 ��������
    "par[6][3]",
	"par[6][4]",
	"par[6][5]",
	"par[6][6]",
	"par[6][7]",
	"par[6][8]",
	"par[6][9]",
	"par[6][10]",
	"par[6][11]",
	"par[6][12]",
	"par[6][13]",
	"par[6][14]",
	"par[7][0]",
	"par[7][1]",
	"par[7][2]",
	"par[7][3]",
	"par[7][4]",
	"par[7][5]",
	"par[7][6]",
	"par[7][7]",
	"par[7][8]",
	"par[7][9]",
	"par[7][10]",
	"par[7][11]",
	"par[7][12]",
	"par[7][13]",
	"par[7][14]",
	"par[8][0]",
	"par[8][1]",
	"par[8][2]",
	
	//11 ��������
    "par[8][3]",
	"par[8][4]",
	"par[8][5]",
	"par[8][6]",
	"par[8][7]",
	"par[8][8]",
	"par[8][9]",
	"par[8][10]",
	"par[8][11]",
	"par[8][12]",
	"par[8][13]",
	"par[8][14]",
	"par[9][0]",
	"par[9][1]",
	"par[9][2]",
	"par[9][3]",
	"par[9][4]",
	"par[9][5]",
	"par[9][6]",
	"par[9][7]",
	"par[9][8]",
	"par[9][9]",
	"par[9][10]",
	"par[9][11]",
	"par[9][12]",
	"par[9][13]",
	"par[9][14]",
	"par[10][0]",
	"par[10][1]",
	"par[10][2]",
	"par[10][3]",
	//12 ��������

	"par[10][4]",
	"par[10][5]",
	"par[10][6]",
	"par[10][7]",
	"par[10][8]",
	"par[10][9]",
	"par[10][10]",
	"par[10][11]",
	"par[10][12]",
	"par[10][13]",
	"par[10][14]",
	"par[11][0]",
	"par[11][1]",
	"par[11][2]",
	"par[11][3]",
	"par[11][4]",
	"par[11][5]",
	"par[11][6]",
	"par[11][7]",
	"par[11][8]",
	"par[11][9]",
	"par[11][10]",
	"par[11][11]",
	"par[11][12]",
	"par[11][13]",
	"par[11][14]",
	"par[12][0]",
	"par[12][1]",
	"par[12][2]",
	
	//13 ��������
    "par[12][3]",
	"par[12][4]",
	"par[12][5]",
	"par[12][6]",
	"par[12][7]",
	"par[12][8]",
	"par[12][9]",
	"par[12][10]",
	"par[12][11]",
	"par[12][12]",
	"par[12][13]",
	"par[12][14]",
	"par[13][0]",
	"par[13][1]",
	"par[13][2]",
	"par[13][3]",
	"par[13][4]",
	"par[13][5]",
	"par[13][6]",
	"par[13][7]",
	"par[13][8]",
	"par[13][9]",
	"par[13][10]",
	"par[13][11]",
	"par[13][12]",
	"par[13][13]",
	"par[13][14]",
	"par[14][0]",
	"par[14][1]",
	"par[14][2]",
	
	//14 ��������
    "par[14][3]",
	"par[14][4]",
	"par[14][5]",
	"par[14][6]",
	"par[14][7]",
	"par[14][8]",
	"par[14][9]",
	"par[14][10]",
	"par[14][11]",
	"par[14][12]",
	"par[14][13]",
	"par[14][14]",
	"par[15][0]",
	"par[15][1]",
	"par[15][2]",
	"par[15][3]",
	"par[15][4]",
	"par[15][5]",
	"par[15][6]",
	"par[15][7]",
	"par[15][8]",
	"par[15][9]",
	"par[15][10]",
	"par[15][11]",
	"par[15][12]",
	"par[15][13]",
	"par[15][14]",
	"par[16][0]",
	"par[16][1]",
	"par[16][2]",
	
	//15 ��������
    "par[16][3]",
	"par[16][4]",
	"par[16][5]",
	"par[16][6]",
	"par[16][7]",
	"par[16][8]",
	"par[16][9]",
	"par[16][10]",
	"par[16][11]",
	"par[16][12]",
	"par[16][13]",
	"par[16][14]",
	"par[17][0]",
	"par[17][1]",
	"par[17][2]",
	"par[17][3]",
	"par[17][4]",
	"par[17][5]",
	"par[17][6]",
	"par[17][7]",
	"par[17][8]",
	"par[17][9]",
	"par[17][10]",
	"par[17][11]",
	"par[17][12]",
	"par[17][13]",
	"par[17][14]",
	"par[18][0]",
	"par[18][1]",
	"par[18][2]",
	
	//16 ��������
    "par[18][3]",
	"par[18][4]",
	"par[18][5]",
	"par[18][6]",
	"par[18][7]",
	"par[18][8]",
	"par[18][9]",
	"par[18][10]",
	"par[18][11]",
	"par[18][12]",
	"par[18][13]",
	"par[18][14]",
	"par[19][0]",
	"par[19][1]",
	"par[19][2]",
	"par[19][3]",
	"par[19][4]",
	"par[19][5]",
	"par[19][6]",
	"par[19][7]",
	"par[19][8]",
	"par[19][9]",
	"par[19][10]",
	"par[19][11]",
	"par[19][12]",
	"par[19][13]",
	"par[19][14]",
	"par[20][0]",
	"par[20][1]",
	"par[20][2]",
	
	//17 ��������
    "par[20][3]",
	"par[20][4]",
	"par[20][5]",
	"par[20][6]",
	"par[20][7]",
	"par[20][8]",
	"par[20][9]",
	"par[20][10]",
	"par[20][11]",
	"par[20][12]",
	"par[20][13]",
	"par[20][14]",
	"T_VHG",
	"T_ZAD_DVS",
	"T_PROC",
	"T_KTMN_RAZGON",
	"T_VKL_BPN",
	"T_KKAM_V",
	"T_VODA",
	"T_STOP",
	"T_DVIJ",
	"T_KDVIJ_SU",
	"T_KSUT",
	"T_KKAM",
	"T_KTMN",
	"T_KPRIJ",
	"T_KPRST",
	"T_KPR",
	"T_KSHL",
	"T_KNAP",
	
	//18 ��������
    "T_NAPUSK",
	"T_SBROSHE",
	"T_KSHL_MO",
	"T_TMN",
	"CT_VHG",
	"CT_VODA_STOL",
	"CT_VODA_IP",
	"CT_PER",
	"CT_POV",
	"CT_PRIJ",
	"CT_KAS",
	"CT_TEMP1",
	"CT_TEMP2",
	" ",
	"CT_DVIJ_GIR_g",
	"CT_DVIJ_GIR_t",
	"CT_SUT_g",
	"CT_SUT_t",
	"CT_T1",
	"CT_T20",
	"CT_1",
	"CT_2",
	"CT_3",
	"CT_4",
	"CT_5",
	"CT_6",
	"CT_7",
	"CT_9",
	"CT_17",
	"CT17K1",
	
	//19 ��������
    "CT_27",
	"CT27K1",
	"CT_28",
	"CT28K1",
	"CT_29",
	"CT29K1",
	"CT_30T",
	"CT_33",
	"CT33K1",
	"CT_35",
	"CT35K1",
	"PR_TRTEST",
	"PR_OTK",
	"PR_FK_KAM",
	"PR_NASOS",
	"PR_NALADKA",
	"PR_NPL",
	"PR_PRIJ",
	"otvet",
	"N_PL",
	"N_ST_MAX",
	"N_ST",
	"PR_HEL",
	"DATA_DZASL",
	"PAR_DZASL",
	"ZPAR_DZASL",
	"X_TDZASL",
	"VRDZASL",
	"E_TDZASL",
	"DELDZASL",
	
	//20 ��������
    "LIM1DZASL",
	"LIM2DZASL",
	"T_VRDZASL",
	"T_KDZASL",
	"DOPDZASL",
	"KOM_DZASL",
	"TEK_DAVL_DZASL",
	"TEK_POZ_DZASL",
	"PR_DZASL",
	"CT_DZASL",
	"OTVET_DZASL",
	"DAVL_DZASL",
	"VRGIS",
	"K_SOGL_GIS",
	"NAPRS_GIS",
	"X_TGIS",
	"E_TGIS",
	"DELGIS",
	"DOPGIS",
	"PAR_GIS",
	"N_TEK_GIS",
	"LIM1GIS",
	"LIM2GIS",
	"T_VRGIS",
	"T_KGIS",
	"prDvijGir_g",
	"prDvijGir_t",
	"DOP_SU",
	"T_SM_NAPR",
	"DOP_DV_IP",
	
	//21 ��������
    "X_TGIS_SM",
	"E_TGIS_SM",
	"DELGIS_SM",
	"DOPGIS_SM",
	"PAR_GIS_SM",
	"N_TEK_GIS_SM",
	"LIM1GIS_SM",
	"LIM2GIS_SM",
	"T_VRGIS_SM",
	"T_KGIS_SM",
	"VRGIR",
	"K_SOGL_GIR",
	"NAPRS_GIR",
	"X_TGIR",
	"E_TGIR",
	"DOPGIR",
	"PAR_GIR",
	"T_VRGIR",
	"T_KGIR",
	"N_TEK_GIR",
	"KOM_PER",
	"OTVET_PER",
	"V_PER",
	"TYPE_PER",
	"PR_PER",
	"HOME_PER",
	"PUT_PER",
	"TEK_ABS_PER",
	"TEK_OTN_PER",
	"KOM_POV",
	
	//22 ��������
    "OTVET_POV",
	"V_POV",
	"TYPE_POV",
	"PR_POV",
	"HOME_POV",
	"PUT_POV",
	"TEK_ABS_POV",
	"TEK_OTN_POV",
	"KOM_KAS",
	"OTVET_KAS",
	"V_KAS",
	"TYPE_KAS",
	"PR_KAS",
	"HOME_KAS",
	"PUT_KAS",
	"TEK_ABS_KAS",
	"TEK_OTN_KAS",
	"PR_TEMP",
	"KOM_TEMP",
	"ZAD_TEMP1",
	"PAR_TEMP1",
	"ZPAR_TEMP1",
	"X_TEMP1",
	"VRTEMP1",
	"E_TEMP1",
	"DELTEMP1",
	"LIM1TEMP1",
	"LIM2TEMP1",
	"DOPTEMP1",
	"TEK_TEMP1",
	
	//23 ��������
    "T_VRTEMP",
	"T_KTEMP",
	"PR_TEMP",
	"KOM_TEMP",
	"ZAD_TEMP2",
	"PAR_TEMP2",
	"ZPAR_TEMP2",
	"X_TEMP2",
	"VRTEMP2",
	"E_TEMP2",
	"DELTEMP2",
	"LIM1TEMP2",
	"LIM2TEMP2",
	"DOPTEMP2",
	"TEK_TEMP2",
	"KOM_MOD",
	"OTVET_MOD",
	"PR_KLASTER",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
    };
	// ������� ����� �������� ��������
    unsigned char pageCount = StrToInt ( PCPerem -> ActivePage -> Hint );
    // ��������� ����� ���������� �� ��������
    for ( unsigned int i = 30 * pageCount; i < ( 30 * ( pageCount + 1  ) ); i++)
        EdtDebugValues[i%30] -> Text = valuesNames[i];

switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
{
//0 ��������
case 0:
{
		EditOTLtek1->Text = IntToStr(sh_);
		EditOTLtek2->Text = IntToStr(shr[1]);
		EditOTLtek3->Text = IntToStr(sh[1]);
		EditOTLtek4->Text = IntToStr(shr[2]);
		EditOTLtek5->Text = IntToStr(sh[2]);
		EditOTLtek6->Text = IntToStr(shr[3]);
		EditOTLtek7->Text = IntToStr(sh[3]);
		EditOTLtek8->Text = IntToStr(shr[4]);
		EditOTLtek9->Text = IntToStr(sh[4]);
		EditOTLtek10->Text = IntToStr(shr[5]);
		EditOTLtek11->Text = IntToStr(sh[5]);
		EditOTLtek12->Text = IntToStr(shr[6]);
		EditOTLtek13->Text = IntToStr(sh[6]);
		EditOTLtek14->Text = IntToStr(shr[7]);
		EditOTLtek15->Text = IntToStr(sh[7]);
		EditOTLtek16->Text = IntToStr(shr[8]);
		EditOTLtek17->Text = IntToStr(sh[8]);
		EditOTLtek18->Text = IntToStr(shr[9]);
		EditOTLtek19->Text = IntToStr(sh[9]);
		EditOTLtek20->Text = IntToStr(shr[10]);
		EditOTLtek21->Text = IntToStr(sh[10]);
		EditOTLtek22->Text = IntToStr(shr[11]);
		EditOTLtek23->Text = IntToStr(sh[11]);
		EditOTLtek24->Text = IntToStr(shr[12]);
		EditOTLtek25->Text = IntToStr(sh[12]);
		EditOTLtek26->Text = IntToStr(shr[13]);
		EditOTLtek27->Text = IntToStr(sh[13]);
		EditOTLtek28->Text = IntToStr(shr[14]);
		EditOTLtek29->Text = IntToStr(sh[14]);
		EditOTLtek30->Text = IntToStr(shr[15]);
}; break;
//1 ��������
case 1:
{
		EditOTLtek1->Text = IntToStr(sh[15]);
		EditOTLtek2->Text = IntToStr(shr[17]);
		EditOTLtek3->Text = IntToStr(sh[17]);
		EditOTLtek4->Text = IntToStr(shr[18]);
		EditOTLtek5->Text = IntToStr(sh[18]);
		EditOTLtek6->Text = IntToStr(shr[19]);
		EditOTLtek7->Text = IntToStr(sh[19]);
		EditOTLtek8->Text = IntToStr(shr[20]);
		EditOTLtek9->Text = IntToStr(sh[20]);
		EditOTLtek10->Text = IntToStr(shr[21]);
		EditOTLtek11->Text = IntToStr(sh[21]);
		EditOTLtek12->Text = IntToStr(shr[22]);
		EditOTLtek13->Text = IntToStr(sh[22]);
		EditOTLtek14->Text = IntToStr(shr[23]);
		EditOTLtek15->Text = IntToStr(sh[23]);
		EditOTLtek16->Text = IntToStr(shr[24]);
		EditOTLtek17->Text = IntToStr(sh[24]);
		EditOTLtek18->Text = IntToStr(shr[25]);
		EditOTLtek19->Text = IntToStr(sh[25]);
		EditOTLtek20->Text = IntToStr(shr[26]);
		EditOTLtek21->Text = IntToStr(sh[26]);
		EditOTLtek22->Text = IntToStr(shr[27]);
		EditOTLtek23->Text = IntToStr(sh[27]);
        EditOTLtek24->Text = IntToStr(shr[28]);
		EditOTLtek25->Text = IntToStr(sh[28]);
		EditOTLtek26->Text = IntToStr(shr[29]);
		EditOTLtek27->Text = IntToStr(sh[29]);
		EditOTLtek28->Text = IntToStr(shr[30]);
		EditOTLtek29->Text = IntToStr(sh[30]);
		EditOTLtek30->Text = IntToStr(shr[31]);

}; break;
//2 ��������
case 2:
{
		EditOTLtek1->Text = IntToStr(sh[31]);
		EditOTLtek2->Text = IntToStr(shr[32]);
		EditOTLtek3->Text = IntToStr(sh[32]);
		EditOTLtek4->Text = IntToStr(shr[33]);
		EditOTLtek5->Text = IntToStr(sh[33]);
		EditOTLtek6->Text = IntToStr(shr[34]);
		EditOTLtek7->Text = IntToStr(sh[34]);
		EditOTLtek8->Text = IntToStr(shr[35]);
		EditOTLtek9->Text = IntToStr(sh[35]);
		EditOTLtek10->Text = IntToStr(shr[36]);
		EditOTLtek11->Text = IntToStr(sh[36]);
		EditOTLtek12->Text = IntToStr(shr[37]);
		EditOTLtek13->Text = IntToStr(sh[37]);
		EditOTLtek14->Text = IntToStr(shr[38]);
		EditOTLtek15->Text = IntToStr(sh[38]);
		EditOTLtek16->Text = IntToStr(shr[39]);
		EditOTLtek17->Text = IntToStr(sh[39]);
		EditOTLtek18->Text = IntToStr(shr[40]);
		EditOTLtek19->Text = IntToStr(sh[40]);
		EditOTLtek20->Text = IntToStr(zshr3);
		EditOTLtek21->Text = IntToStr(norma);
		EditOTLtek22->Text = IntToStr(qkk);
		EditOTLtek23->Text = IntToStr(diagn[0]);
		EditOTLtek24->Text = IntToStr(diagn[1]);
		EditOTLtek25->Text = IntToStr(diagn[2]);
		EditOTLtek26->Text = IntToStr(diagn[3]);
		EditOTLtek27->Text = IntToStr(diagn[4]);
		EditOTLtek28->Text = IntToStr(diagn[5]);
		EditOTLtek29->Text = IntToStr(diagn[6]);
		EditOTLtek30->Text = IntToStr(diagn[7]);

}; break;
//3 ��������
case 3:
{
		EditOTLtek1->Text = IntToStr(diagn[8]);
		EditOTLtek2->Text = IntToStr(diagn[9]);
		EditOTLtek3->Text = IntToStr(diagn[10]);
		EditOTLtek4->Text = IntToStr(diagn[11]);
		EditOTLtek5->Text = IntToStr(diagn[12]);
		EditOTLtek6->Text = IntToStr(diagn[13]);
		EditOTLtek7->Text = IntToStr(diagn[14]);
		EditOTLtek8->Text = IntToStr(diagn[15]);
		EditOTLtek9->Text = IntToStr(diagn[16]);
		EditOTLtek10->Text = IntToStr(diagn[17]);
		EditOTLtek11->Text = IntToStr(diagn[18]);
		EditOTLtek12->Text = IntToStr(diagn[19]);
		EditOTLtek13->Text = IntToStr(diagn[20]);
		EditOTLtek14->Text = IntToStr(diagn[21]);
		EditOTLtek15->Text = IntToStr(diagn[22]);
		EditOTLtek16->Text = IntToStr(diagn[23]);
		EditOTLtek17->Text = IntToStr(diagn[24]);
		EditOTLtek18->Text = IntToStr(diagn[25]);
		EditOTLtek19->Text = IntToStr(diagn[26]);
		EditOTLtek20->Text = IntToStr(diagn[27]);
		EditOTLtek21->Text = IntToStr(diagn[28]);
		EditOTLtek22->Text = IntToStr(diagnS[0]);
		EditOTLtek23->Text = IntToStr(diagnS[1]);
		EditOTLtek24->Text = IntToStr(diagnS[2]);
		EditOTLtek25->Text = IntToStr(out[0]);
		EditOTLtek26->Text = IntToStr(out[1]);
		EditOTLtek27->Text = IntToStr(out[2]);
		EditOTLtek28->Text = IntToStr(out[3]);
		EditOTLtek29->Text = IntToStr(zin[0]);
		EditOTLtek30->Text = IntToStr(zin[1]);
}; break;
//4 ��������
case 4:
{
		EditOTLtek1->Text = IntToStr(zin[2]);
		EditOTLtek2->Text = IntToStr(zin[3]);
		EditOTLtek3->Text = IntToStr(zin[4]);
		EditOTLtek4->Text = IntToStr(aik[0]);
		EditOTLtek5->Text = IntToStr(aik[1]);
		EditOTLtek6->Text = IntToStr(aik[2]);
		EditOTLtek7->Text = IntToStr(aik[3]);
		EditOTLtek8->Text = IntToStr(aik[4]);
		EditOTLtek9->Text = IntToStr(aik[5]);
		EditOTLtek10->Text = IntToStr(aik[6]);
		EditOTLtek11->Text = IntToStr(aik[7]);
		EditOTLtek12->Text = IntToStr(aik[8]);
		EditOTLtek13->Text = IntToStr(aik[9]);
		EditOTLtek14->Text = IntToStr(aik[10]);
		EditOTLtek15->Text = IntToStr(aik[11]);
		EditOTLtek16->Text = IntToStr(aik[12]);
		EditOTLtek17->Text = IntToStr(aik[13]);
		EditOTLtek18->Text = IntToStr(aik[14]);
		EditOTLtek19->Text = IntToStr(aik[15]);
		EditOTLtek20->Text = IntToStr(aik[16]);
		EditOTLtek21->Text = IntToStr(aik[17]);
        EditOTLtek22->Text = IntToStr(aik[18]);
		EditOTLtek23->Text = IntToStr(aout[0]);
		EditOTLtek24->Text = IntToStr(aout[1]);
		EditOTLtek25->Text = IntToStr(aout[2]);
		EditOTLtek26->Text = IntToStr(aout[3]);
		EditOTLtek27->Text = IntToStr(aout[4]);
		EditOTLtek28->Text = IntToStr(aout[5]);
		EditOTLtek29->Text = IntToStr(aout[6]);
		EditOTLtek30->Text = IntToStr(aout[7]);
}; break;
//5 ��������
case 5:
{
        EditOTLtek1->Text = IntToStr(aout[8]);
		EditOTLtek2->Text = IntToStr(aout[9]);
		EditOTLtek3->Text = IntToStr(aout[10]);
		EditOTLtek4->Text = IntToStr(D_D1);
		EditOTLtek5->Text = IntToStr(D_D2);
		EditOTLtek6->Text = IntToStr(D_D3);
		EditOTLtek7->Text = IntToStr(D_D4);
		EditOTLtek8->Text = IntToStr(D_D5);
		EditOTLtek9->Text = IntToStr(0);
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(0);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(0);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(POROG_DAVL);
		EditOTLtek17->Text = IntToStr(UVAKV_KAM);
		EditOTLtek18->Text = IntToStr(UVAKN_KAM);
		EditOTLtek19->Text = IntToStr(UVAKN_TMN);
		EditOTLtek20->Text = IntToStr(UVAKV_TMN);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(UVAK_SHL_MO);
		EditOTLtek23->Text = IntToStr(UVAK_SHL_MN);
		EditOTLtek24->Text = IntToStr(UVAK_ZTMN);
		EditOTLtek25->Text = IntToStr(UATM_D1);
		EditOTLtek26->Text = IntToStr(UATM_D4);
		EditOTLtek27->Text = IntToStr(0);
		EditOTLtek28->Text = IntToStr(nasmod[0]);
		EditOTLtek29->Text = IntToStr(nasmod[1]);
		EditOTLtek30->Text = IntToStr(nasmod[2]);
}; break;
//6 ��������
case 6:
{
		EditOTLtek1->Text = IntToStr(nasmod[3]);
		EditOTLtek2->Text = IntToStr(nasmod[4]);
		EditOTLtek3->Text = IntToStr(nasmod[5]);
		EditOTLtek4->Text = IntToStr(nasmod[6]);
		EditOTLtek5->Text = IntToStr(nasmod[7]);
		EditOTLtek6->Text = IntToStr(nasmod[8]);
		EditOTLtek7->Text = IntToStr(nasmod[9]);
		EditOTLtek8->Text = IntToStr(nasmod[10]);
		EditOTLtek9->Text = IntToStr(nasmod[11]);
		EditOTLtek10->Text = IntToStr(nasmod[12]);
		EditOTLtek11->Text = IntToStr(nasmod[13]);
		EditOTLtek12->Text = IntToStr(nasmod[14]);
		EditOTLtek13->Text = IntToStr(nasmod[15]);
		EditOTLtek14->Text = IntToStr(nasmod[16]);
		EditOTLtek15->Text = IntToStr(nasmod[17]);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(par_t[0]);
		EditOTLtek18->Text = IntToStr(par_t[1]);
		EditOTLtek19->Text = IntToStr(par_t[2]);
		EditOTLtek20->Text = IntToStr(par_t[3]);
		EditOTLtek21->Text = IntToStr(par_t[4]);
		EditOTLtek22->Text = IntToStr(par_t[5]);
		EditOTLtek23->Text = IntToStr(par[0][0]);
		EditOTLtek24->Text = IntToStr(par[0][1]);
		EditOTLtek25->Text = IntToStr(par[0][2]);
		EditOTLtek26->Text = IntToStr(par[0][3]);
		EditOTLtek27->Text = IntToStr(par[0][4]);
		EditOTLtek28->Text = IntToStr(par[0][5]);
		EditOTLtek29->Text = IntToStr(par[0][6]);
		EditOTLtek30->Text = IntToStr(par[0][7]);
}; break;
//7 ��������
case 7:
{
		EditOTLtek1->Text = IntToStr(par[0][8]);
		EditOTLtek2->Text = IntToStr(par[0][9]);
		EditOTLtek3->Text = IntToStr(par[0][10]);
		EditOTLtek4->Text = IntToStr(par[0][11]);
		EditOTLtek5->Text = IntToStr(par[0][12]);
		EditOTLtek6->Text = IntToStr(par[0][13]);
		EditOTLtek7->Text = IntToStr(par[0][14]);
		EditOTLtek8->Text = IntToStr(par[0][15]);
		EditOTLtek9->Text = IntToStr(par[0][16]);
		EditOTLtek10->Text = IntToStr(par[0][17]);
		EditOTLtek11->Text = IntToStr(par[0][18]);
        EditOTLtek12->Text = IntToStr(par[0][20]);
		EditOTLtek13->Text = IntToStr(par[1][0]);
		EditOTLtek14->Text = IntToStr(par[1][1]);
		EditOTLtek15->Text = IntToStr(par[1][2]);
		EditOTLtek16->Text = IntToStr(par[1][3]);
		EditOTLtek17->Text = IntToStr(par[1][4]);
		EditOTLtek18->Text = IntToStr(par[1][5]);
		EditOTLtek19->Text = IntToStr(par[1][6]);
		EditOTLtek20->Text = IntToStr(par[1][7]);
		EditOTLtek21->Text = IntToStr(par[1][8]);
		EditOTLtek22->Text = IntToStr(par[1][9]);
		EditOTLtek23->Text = IntToStr(par[1][10]);
		EditOTLtek24->Text = IntToStr(par[1][11]);
		EditOTLtek25->Text = IntToStr(par[1][12]);
		EditOTLtek26->Text = IntToStr(par[1][13]);
		EditOTLtek27->Text = IntToStr(par[1][14]);
		EditOTLtek28->Text = IntToStr(par[2][0]);
		EditOTLtek29->Text = IntToStr(par[2][1]);
		EditOTLtek30->Text = IntToStr(par[2][2]);
		
}; break;
//8 ��������
case 8:
{       EditOTLtek1->Text = IntToStr(par[2][3]);
		EditOTLtek2->Text = IntToStr(par[2][4]);
		EditOTLtek3->Text = IntToStr(par[2][5]);
		EditOTLtek4->Text = IntToStr(par[2][6]);
		EditOTLtek5->Text = IntToStr(par[2][7]);
		EditOTLtek6->Text = IntToStr(par[2][8]);
		EditOTLtek7->Text = IntToStr(par[2][9]);
		EditOTLtek8->Text = IntToStr(par[2][10]);
		EditOTLtek9->Text = IntToStr(par[2][11]);
		EditOTLtek10->Text = IntToStr(par[2][12]);
		EditOTLtek11->Text = IntToStr(par[2][13]);
		EditOTLtek12->Text = IntToStr(par[2][14]);
		EditOTLtek13->Text = IntToStr(par[3][0]);
		EditOTLtek14->Text = IntToStr(par[3][1]);
		EditOTLtek15->Text = IntToStr(par[3][2]);
		EditOTLtek16->Text = IntToStr(par[3][3]);
		EditOTLtek17->Text = IntToStr(par[3][4]);
		EditOTLtek18->Text = IntToStr(par[3][5]);
		EditOTLtek19->Text = IntToStr(par[3][6]);
		EditOTLtek20->Text = IntToStr(par[3][7]);
		EditOTLtek21->Text = IntToStr(par[3][8]);
		EditOTLtek22->Text = IntToStr(par[3][9]);
		EditOTLtek23->Text = IntToStr(par[3][10]);
		EditOTLtek24->Text = IntToStr(par[3][11]);
		EditOTLtek25->Text = IntToStr(par[3][12]);
		EditOTLtek26->Text = IntToStr(par[3][13]);
		EditOTLtek27->Text = IntToStr(par[3][14]);
		EditOTLtek28->Text = IntToStr(par[4][0]);
		EditOTLtek29->Text = IntToStr(par[4][1]);
		EditOTLtek30->Text = IntToStr(par[4][2]);

}; break;
//9 ��������
case 9:
{       EditOTLtek1->Text = IntToStr(par[4][3]);
		EditOTLtek2->Text = IntToStr(par[4][4]);
		EditOTLtek3->Text = IntToStr(par[4][5]);
		EditOTLtek4->Text = IntToStr(par[4][6]);
		EditOTLtek5->Text = IntToStr(par[4][7]);
		EditOTLtek6->Text = IntToStr(par[4][8]);
		EditOTLtek7->Text = IntToStr(par[4][9]);
		EditOTLtek8->Text = IntToStr(par[4][10]);
		EditOTLtek9->Text = IntToStr(par[4][11]);
		EditOTLtek10->Text = IntToStr(par[4][12]);
		EditOTLtek11->Text = IntToStr(par[4][13]);
		EditOTLtek12->Text = IntToStr(par[4][14]);
		EditOTLtek13->Text = IntToStr(par[5][0]);
		EditOTLtek14->Text = IntToStr(par[5][1]);
		EditOTLtek15->Text = IntToStr(par[5][2]);
		EditOTLtek16->Text = IntToStr(par[5][3]);
		EditOTLtek17->Text = IntToStr(par[5][4]);
		EditOTLtek18->Text = IntToStr(par[5][5]);
		EditOTLtek19->Text = IntToStr(par[5][6]);
		EditOTLtek20->Text = IntToStr(par[5][7]);
		EditOTLtek21->Text = IntToStr(par[5][8]);
		EditOTLtek22->Text = IntToStr(par[5][9]);
		EditOTLtek23->Text = IntToStr(par[5][10]);
		EditOTLtek24->Text = IntToStr(par[5][11]);
		EditOTLtek25->Text = IntToStr(par[5][12]);
		EditOTLtek26->Text = IntToStr(par[5][13]);
		EditOTLtek27->Text = IntToStr(par[5][14]);
		EditOTLtek28->Text = IntToStr(par[6][0]);
		EditOTLtek29->Text = IntToStr(par[6][1]);
		EditOTLtek30->Text = IntToStr(par[6][2]);
		
}; break;
//10 ��������
case 10:
{       EditOTLtek1->Text = IntToStr(par[6][3]);
		EditOTLtek2->Text = IntToStr(par[6][4]);
		EditOTLtek3->Text = IntToStr(par[6][5]);
		EditOTLtek4->Text = IntToStr(par[6][6]);
		EditOTLtek5->Text = IntToStr(par[6][7]);
		EditOTLtek6->Text = IntToStr(par[6][8]);
		EditOTLtek7->Text = IntToStr(par[6][9]);
		EditOTLtek8->Text = IntToStr(par[6][10]);
		EditOTLtek9->Text = IntToStr(par[6][11]);
		EditOTLtek10->Text = IntToStr(par[6][12]);
		EditOTLtek11->Text = IntToStr(par[6][13]);
		EditOTLtek12->Text = IntToStr(par[6][14]);
		EditOTLtek13->Text = IntToStr(par[7][0]);
		EditOTLtek14->Text = IntToStr(par[7][1]);
		EditOTLtek15->Text = IntToStr(par[7][2]);
		EditOTLtek16->Text = IntToStr(par[7][3]);
		EditOTLtek17->Text = IntToStr(par[7][4]);
		EditOTLtek18->Text = IntToStr(par[7][5]);
		EditOTLtek19->Text = IntToStr(par[7][6]);
		EditOTLtek20->Text = IntToStr(par[7][7]);
		EditOTLtek21->Text = IntToStr(par[7][8]);
		EditOTLtek22->Text = IntToStr(par[7][9]);
		EditOTLtek23->Text = IntToStr(par[7][10]);
		EditOTLtek24->Text = IntToStr(par[7][11]);
		EditOTLtek25->Text = IntToStr(par[7][12]);
		EditOTLtek26->Text = IntToStr(par[7][13]);
		EditOTLtek27->Text = IntToStr(par[7][14]);
		EditOTLtek28->Text = IntToStr(par[8][0]);
		EditOTLtek29->Text = IntToStr(par[8][1]);
		EditOTLtek30->Text = IntToStr(par[8][2]);
		
}; break;
//11 ��������
case 11:
{       EditOTLtek1->Text = IntToStr(par[8][3]);
		EditOTLtek2->Text = IntToStr(par[8][4]);
		EditOTLtek3->Text = IntToStr(par[8][5]);
		EditOTLtek4->Text = IntToStr(par[8][6]);
		EditOTLtek5->Text = IntToStr(par[8][7]);
		EditOTLtek6->Text = IntToStr(par[8][8]);
		EditOTLtek7->Text = IntToStr(par[8][9]);
		EditOTLtek8->Text = IntToStr(par[8][10]);
		EditOTLtek9->Text = IntToStr(par[8][11]);
		EditOTLtek10->Text = IntToStr(par[8][12]);
		EditOTLtek11->Text = IntToStr(par[8][13]);
		EditOTLtek12->Text = IntToStr(par[8][14]);
		EditOTLtek13->Text = IntToStr(par[9][0]);
		EditOTLtek14->Text = IntToStr(par[9][1]);
		EditOTLtek15->Text = IntToStr(par[9][2]);
		EditOTLtek16->Text = IntToStr(par[9][3]);
		EditOTLtek17->Text = IntToStr(par[9][4]);
		EditOTLtek18->Text = IntToStr(par[9][5]);
		EditOTLtek19->Text = IntToStr(par[9][6]);
		EditOTLtek20->Text = IntToStr(par[9][7]);
		EditOTLtek21->Text = IntToStr(par[9][8]);
		EditOTLtek22->Text = IntToStr(par[9][9]);
		EditOTLtek23->Text = IntToStr(par[9][10]);
		EditOTLtek24->Text = IntToStr(par[9][11]);
		EditOTLtek25->Text = IntToStr(par[9][12]);
		EditOTLtek26->Text = IntToStr(par[9][13]);
		EditOTLtek27->Text = IntToStr(par[9][14]);
		EditOTLtek28->Text = IntToStr(par[10][0]);
		EditOTLtek29->Text = IntToStr(par[10][1]);
		EditOTLtek30->Text = IntToStr(par[10][2]);

}; break;
//12 ��������
case 12:
{       EditOTLtek1->Text = IntToStr(par[10][3]);
		EditOTLtek2->Text = IntToStr(par[10][4]);
		EditOTLtek3->Text = IntToStr(par[10][5]);
		EditOTLtek4->Text = IntToStr(par[10][6]);
		EditOTLtek5->Text = IntToStr(par[10][7]);
		EditOTLtek6->Text = IntToStr(par[10][8]);
		EditOTLtek7->Text = IntToStr(par[10][9]);
		EditOTLtek8->Text = IntToStr(par[10][10]);
		EditOTLtek9->Text = IntToStr(par[10][11]);
		EditOTLtek10->Text = IntToStr(par[10][12]);
		EditOTLtek11->Text = IntToStr(par[10][13]);
		EditOTLtek12->Text = IntToStr(par[10][14]);
		EditOTLtek13->Text = IntToStr(par[11][0]);
		EditOTLtek14->Text = IntToStr(par[11][1]);
		EditOTLtek15->Text = IntToStr(par[11][2]);
		EditOTLtek16->Text = IntToStr(par[11][3]);
		EditOTLtek17->Text = IntToStr(par[11][4]);
		EditOTLtek18->Text = IntToStr(par[11][5]);
		EditOTLtek19->Text = IntToStr(par[11][6]);
		EditOTLtek20->Text = IntToStr(par[11][7]);
		EditOTLtek21->Text = IntToStr(par[11][8]);
		EditOTLtek22->Text = IntToStr(par[11][9]);
		EditOTLtek23->Text = IntToStr(par[11][10]);
		EditOTLtek24->Text = IntToStr(par[11][11]);
		EditOTLtek25->Text = IntToStr(par[11][12]);
		EditOTLtek26->Text = IntToStr(par[11][13]);
		EditOTLtek27->Text = IntToStr(par[11][14]);
		EditOTLtek28->Text = IntToStr(par[12][0]);
		EditOTLtek29->Text = IntToStr(par[12][1]);
		EditOTLtek30->Text = IntToStr(par[12][2]);

}; break;
//13 ��������
case 13:
{       EditOTLtek1->Text = IntToStr(par[12][3]);
		EditOTLtek2->Text = IntToStr(par[12][4]);
		EditOTLtek3->Text = IntToStr(par[12][5]);
		EditOTLtek4->Text = IntToStr(par[12][6]);
		EditOTLtek5->Text = IntToStr(par[12][7]);
		EditOTLtek6->Text = IntToStr(par[12][8]);
		EditOTLtek7->Text = IntToStr(par[12][9]);
		EditOTLtek8->Text = IntToStr(par[12][10]);
		EditOTLtek9->Text = IntToStr(par[12][11]);
		EditOTLtek10->Text = IntToStr(par[12][12]);
		EditOTLtek11->Text = IntToStr(par[12][13]);
		EditOTLtek12->Text = IntToStr(par[12][14]);
		EditOTLtek13->Text = IntToStr(par[13][0]);
		EditOTLtek14->Text = IntToStr(par[13][1]);
		EditOTLtek15->Text = IntToStr(par[13][2]);
		EditOTLtek16->Text = IntToStr(par[13][3]);
		EditOTLtek17->Text = IntToStr(par[13][4]);
		EditOTLtek18->Text = IntToStr(par[13][5]);
		EditOTLtek19->Text = IntToStr(par[13][6]);
		EditOTLtek20->Text = IntToStr(par[13][7]);
		EditOTLtek21->Text = IntToStr(par[13][8]);
		EditOTLtek22->Text = IntToStr(par[13][9]);
		EditOTLtek23->Text = IntToStr(par[13][10]);
		EditOTLtek24->Text = IntToStr(par[13][11]);
		EditOTLtek25->Text = IntToStr(par[13][12]);
		EditOTLtek26->Text = IntToStr(par[13][13]);
		EditOTLtek27->Text = IntToStr(par[13][14]);
		EditOTLtek28->Text = IntToStr(par[14][0]);
		EditOTLtek29->Text = IntToStr(par[14][1]);
		EditOTLtek30->Text = IntToStr(par[14][2]);

}; break;
//14 ��������
case 14:
{       EditOTLtek1->Text = IntToStr(par[14][3]);
		EditOTLtek2->Text = IntToStr(par[14][4]);
		EditOTLtek3->Text = IntToStr(par[14][5]);
		EditOTLtek4->Text = IntToStr(par[14][6]);
		EditOTLtek5->Text = IntToStr(par[14][7]);
		EditOTLtek6->Text = IntToStr(par[14][8]);
		EditOTLtek7->Text = IntToStr(par[14][9]);
		EditOTLtek8->Text = IntToStr(par[14][10]);
		EditOTLtek9->Text = IntToStr(par[14][11]);
		EditOTLtek10->Text = IntToStr(par[14][12]);
		EditOTLtek11->Text = IntToStr(par[14][13]);
		EditOTLtek12->Text = IntToStr(par[14][14]);
		EditOTLtek13->Text = IntToStr(par[15][0]);
		EditOTLtek14->Text = IntToStr(par[15][1]);
		EditOTLtek15->Text = IntToStr(par[15][2]);
		EditOTLtek16->Text = IntToStr(par[15][3]);
		EditOTLtek17->Text = IntToStr(par[15][4]);
		EditOTLtek18->Text = IntToStr(par[15][5]);
		EditOTLtek19->Text = IntToStr(par[15][6]);
		EditOTLtek20->Text = IntToStr(par[15][7]);
		EditOTLtek21->Text = IntToStr(par[15][8]);
		EditOTLtek22->Text = IntToStr(par[15][9]);
		EditOTLtek23->Text = IntToStr(par[15][10]);
		EditOTLtek24->Text = IntToStr(par[15][11]);
		EditOTLtek25->Text = IntToStr(par[15][12]);
		EditOTLtek26->Text = IntToStr(par[15][13]);
		EditOTLtek27->Text = IntToStr(par[15][14]);
		EditOTLtek28->Text = IntToStr(par[16][0]);
		EditOTLtek29->Text = IntToStr(par[16][1]);
		EditOTLtek30->Text = IntToStr(par[16][2]);

}; break;
//15 ��������
case 15:
{       EditOTLtek1->Text = IntToStr(par[16][3]);
		EditOTLtek2->Text = IntToStr(par[16][4]);
		EditOTLtek3->Text = IntToStr(par[16][5]);
		EditOTLtek4->Text = IntToStr(par[16][6]);
		EditOTLtek5->Text = IntToStr(par[16][7]);
		EditOTLtek6->Text = IntToStr(par[16][8]);
		EditOTLtek7->Text = IntToStr(par[16][9]);
		EditOTLtek8->Text = IntToStr(par[16][10]);
		EditOTLtek9->Text = IntToStr(par[16][11]);
		EditOTLtek10->Text = IntToStr(par[16][12]);
		EditOTLtek11->Text = IntToStr(par[16][13]);
		EditOTLtek12->Text = IntToStr(par[16][14]);
		EditOTLtek13->Text = IntToStr(par[17][0]);
		EditOTLtek14->Text = IntToStr(par[17][1]);
		EditOTLtek15->Text = IntToStr(par[17][2]);
		EditOTLtek16->Text = IntToStr(par[17][3]);
		EditOTLtek17->Text = IntToStr(par[17][4]);
		EditOTLtek18->Text = IntToStr(par[17][5]);
		EditOTLtek19->Text = IntToStr(par[17][6]);
		EditOTLtek20->Text = IntToStr(par[17][7]);
		EditOTLtek21->Text = IntToStr(par[17][8]);
		EditOTLtek22->Text = IntToStr(par[17][9]);
		EditOTLtek23->Text = IntToStr(par[17][10]);
		EditOTLtek24->Text = IntToStr(par[17][11]);
		EditOTLtek25->Text = IntToStr(par[17][12]);
		EditOTLtek26->Text = IntToStr(par[17][13]);
		EditOTLtek27->Text = IntToStr(par[17][14]);
		EditOTLtek28->Text = IntToStr(par[18][0]);
		EditOTLtek29->Text = IntToStr(par[18][1]);
		EditOTLtek30->Text = IntToStr(par[18][2]);

}; break;
//16 ��������
case 16:
{       EditOTLtek1->Text = IntToStr(par[18][3]);
		EditOTLtek2->Text = IntToStr(par[18][4]);
		EditOTLtek3->Text = IntToStr(par[18][5]);
		EditOTLtek4->Text = IntToStr(par[18][6]);
		EditOTLtek5->Text = IntToStr(par[18][7]);
		EditOTLtek6->Text = IntToStr(par[18][8]);
		EditOTLtek7->Text = IntToStr(par[18][9]);
		EditOTLtek8->Text = IntToStr(par[18][10]);
		EditOTLtek9->Text = IntToStr(par[18][11]);
		EditOTLtek10->Text = IntToStr(par[18][12]);
		EditOTLtek11->Text = IntToStr(par[18][13]);
		EditOTLtek12->Text = IntToStr(par[18][14]);
		EditOTLtek13->Text = IntToStr(par[19][0]);
		EditOTLtek14->Text = IntToStr(par[19][1]);
		EditOTLtek15->Text = IntToStr(par[19][2]);
		EditOTLtek16->Text = IntToStr(par[19][3]);
		EditOTLtek17->Text = IntToStr(par[19][4]);
		EditOTLtek18->Text = IntToStr(par[19][5]);
		EditOTLtek19->Text = IntToStr(par[19][6]);
		EditOTLtek20->Text = IntToStr(par[19][7]);
		EditOTLtek21->Text = IntToStr(par[19][8]);
		EditOTLtek22->Text = IntToStr(par[19][9]);
		EditOTLtek23->Text = IntToStr(par[19][10]);
		EditOTLtek24->Text = IntToStr(par[19][11]);
		EditOTLtek25->Text = IntToStr(par[19][12]);
		EditOTLtek26->Text = IntToStr(par[19][13]);
		EditOTLtek27->Text = IntToStr(par[19][14]);
		EditOTLtek28->Text = IntToStr(par[20][0]);
		EditOTLtek29->Text = IntToStr(par[20][1]);
		EditOTLtek30->Text = IntToStr(par[20][2]);
		
}; break;
//17 ��������
case 17:
{       EditOTLtek1->Text = IntToStr(par[20][3]);
		EditOTLtek2->Text = IntToStr(par[20][4]);
		EditOTLtek3->Text = IntToStr(par[20][5]);
		EditOTLtek4->Text = IntToStr(par[20][6]);
		EditOTLtek5->Text = IntToStr(par[20][7]);
		EditOTLtek6->Text = IntToStr(par[20][8]);
		EditOTLtek7->Text = IntToStr(par[20][9]);
		EditOTLtek8->Text = IntToStr(par[20][10]);
		EditOTLtek9->Text = IntToStr(par[20][11]);
		EditOTLtek10->Text = IntToStr(par[20][12]);
		EditOTLtek11->Text = IntToStr(par[20][13]);
		EditOTLtek12->Text = IntToStr(par[20][14]);
		EditOTLtek13->Text = IntToStr(T_VHG);
		EditOTLtek14->Text = IntToStr(T_ZAD_DVS);
		EditOTLtek15->Text = IntToStr(T_PROC);
		EditOTLtek16->Text = IntToStr(T_KTMN_RAZGON);
		EditOTLtek17->Text = IntToStr(T_VKL_BPN);
		EditOTLtek18->Text = IntToStr(T_KKAM_V);
		EditOTLtek19->Text = IntToStr(T_VODA);
		EditOTLtek20->Text = IntToStr(T_STOP);
		EditOTLtek21->Text = IntToStr(T_DVIJ);
		EditOTLtek22->Text = IntToStr(T_KDVIJ_SU);
		EditOTLtek23->Text = IntToStr(T_KSUT);
		EditOTLtek24->Text = IntToStr(T_KKAM);
		EditOTLtek25->Text = IntToStr(T_KTMN);
		EditOTLtek26->Text = IntToStr(T_KPRIJ);
		EditOTLtek27->Text = IntToStr(T_KPRST);
		EditOTLtek28->Text = IntToStr(T_KPR);
		EditOTLtek29->Text = IntToStr(T_KSHL);
		EditOTLtek30->Text = IntToStr(T_KNAP);

}; break;
//18 ��������
case 18:
{       EditOTLtek1->Text = IntToStr(T_NAPUSK);
		EditOTLtek2->Text = IntToStr(T_SBROSHE);
		EditOTLtek3->Text = IntToStr(T_KSHL_MO);
		EditOTLtek4->Text = IntToStr(T_TMN);
		EditOTLtek5->Text = IntToStr(CT_VHG);
		EditOTLtek6->Text = IntToStr(CT_VODA_STOL);
		EditOTLtek7->Text = IntToStr(CT_VODA_IP);
		EditOTLtek8->Text = IntToStr(CT_PER);
		EditOTLtek9->Text = IntToStr(CT_POV);
		EditOTLtek10->Text = IntToStr(CT_PRIJ);
		EditOTLtek11->Text = IntToStr(CT_KAS);
		EditOTLtek12->Text = IntToStr(CT_TEMP1);
		EditOTLtek13->Text = IntToStr(CT_TEMP2);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(CT_DVIJ_GIR_g);
		EditOTLtek16->Text = IntToStr(CT_DVIJ_GIR_t);
		EditOTLtek17->Text = IntToStr(CT_SUT_g);
		EditOTLtek18->Text = IntToStr(CT_SUT_t);
		EditOTLtek19->Text = IntToStr(CT_T1);
		EditOTLtek20->Text = IntToStr(CT_T20);
		EditOTLtek21->Text = IntToStr(CT_1);
		EditOTLtek22->Text = IntToStr(CT_2);
		EditOTLtek23->Text = IntToStr(CT_3);
		EditOTLtek24->Text = IntToStr(CT_4);
		EditOTLtek25->Text = IntToStr(CT_5);
		EditOTLtek26->Text = IntToStr(CT_6);
		EditOTLtek27->Text = IntToStr(CT_7);
		EditOTLtek28->Text = IntToStr(CT_9);
		EditOTLtek29->Text = IntToStr(CT_17);
		EditOTLtek30->Text = IntToStr(CT17K1);

}; break;
//19 ��������
case 19:
{       EditOTLtek1->Text = IntToStr(CT_27);
		EditOTLtek2->Text = IntToStr(CT27K1);
		EditOTLtek3->Text = IntToStr(CT_28);
		EditOTLtek4->Text = IntToStr(CT28K1);
		EditOTLtek5->Text = IntToStr(CT_29);
		EditOTLtek6->Text = IntToStr(CT29K1);
		EditOTLtek7->Text = IntToStr(CT_30T);
		EditOTLtek8->Text = IntToStr(CT_33);
		EditOTLtek9->Text = IntToStr(CT33K1);
		EditOTLtek10->Text = IntToStr(CT_35);
		EditOTLtek11->Text = IntToStr(CT35K1);
		EditOTLtek12->Text = IntToStr(PR_TRTEST);
		EditOTLtek13->Text = IntToStr(PR_OTK);
		EditOTLtek14->Text = IntToStr(PR_FK_KAM);
		EditOTLtek15->Text = IntToStr(PR_NASOS);
		EditOTLtek16->Text = IntToStr(PR_NALADKA);
		EditOTLtek17->Text = IntToStr(PR_NPL);
		EditOTLtek18->Text = IntToStr(PR_PRIJ);
		EditOTLtek19->Text = IntToStr(otvet);
		EditOTLtek20->Text = IntToStr(N_PL);
		EditOTLtek21->Text = IntToStr(N_ST_MAX);
		EditOTLtek22->Text = IntToStr(N_ST);
		EditOTLtek23->Text = IntToStr(PR_HEL);
		EditOTLtek24->Text = IntToStr(DATA_DZASL);
		EditOTLtek25->Text = IntToStr(PAR_DZASL);
		EditOTLtek26->Text = IntToStr(ZPAR_DZASL);
		EditOTLtek27->Text = IntToStr(X_TDZASL);
		EditOTLtek28->Text = IntToStr(VRDZASL);
		EditOTLtek29->Text = IntToStr(E_TDZASL);
		EditOTLtek30->Text = IntToStr(DELDZASL);

}; break;
//20 ��������
case 20:
{       EditOTLtek1->Text = IntToStr(LIM1DZASL);
		EditOTLtek2->Text = IntToStr(LIM2DZASL);
		EditOTLtek3->Text = IntToStr(T_VRDZASL);
		EditOTLtek4->Text = IntToStr(T_KDZASL);
		EditOTLtek5->Text = IntToStr(DOPDZASL);
		EditOTLtek6->Text = IntToStr(KOM_DZASL);
		EditOTLtek7->Text = IntToStr(TEK_DAVL_DZASL);
		EditOTLtek8->Text = IntToStr(TEK_POZ_DZASL);
		EditOTLtek9->Text = IntToStr(PR_DZASL);
		EditOTLtek10->Text = IntToStr(CT_DZASL);
		EditOTLtek11->Text = IntToStr(OTVET_DZASL);
		EditOTLtek12->Text = IntToStr(DAVL_DZASL);
		EditOTLtek13->Text = IntToStr(VRGIS);
		EditOTLtek14->Text = IntToStr(K_SOGL_GIS);
		EditOTLtek15->Text = IntToStr(NAPRS_GIS);
		EditOTLtek16->Text = IntToStr(X_TGIS);
		EditOTLtek17->Text = IntToStr(E_TGIS);
		EditOTLtek18->Text = IntToStr(DELGIS);
		EditOTLtek19->Text = IntToStr(DOPGIS);
		EditOTLtek20->Text = IntToStr(PAR_GIS);
		EditOTLtek21->Text = IntToStr(N_TEK_GIS);
		EditOTLtek22->Text = IntToStr(LIM1GIS);
		EditOTLtek23->Text = IntToStr(LIM2GIS);
		EditOTLtek24->Text = IntToStr(T_VRGIS);
		EditOTLtek25->Text = IntToStr(T_KGIS);
		EditOTLtek26->Text = IntToStr(prDvijGir_g);
		EditOTLtek27->Text = IntToStr(prDvijGir_t);
		EditOTLtek28->Text = IntToStr(DOP_SU);
		EditOTLtek29->Text = IntToStr(T_SM_NAPR);
		EditOTLtek30->Text = IntToStr(DOP_DV_IP);

}; break;
//21 ��������
case 21:
{       EditOTLtek1->Text = IntToStr(X_TGIS_SM);
		EditOTLtek2->Text = IntToStr(E_TGIS_SM);
		EditOTLtek3->Text = IntToStr(DELGIS_SM);
		EditOTLtek4->Text = IntToStr(DOPGIS_SM);
		EditOTLtek5->Text = IntToStr(PAR_GIS_SM);
		EditOTLtek6->Text = IntToStr(N_TEK_GIS_SM);
		EditOTLtek7->Text = IntToStr(LIM1GIS_SM);
		EditOTLtek8->Text = IntToStr(LIM2GIS_SM);
		EditOTLtek9->Text = IntToStr(T_VRGIS_SM);
		EditOTLtek10->Text = IntToStr(T_KGIS_SM);
		EditOTLtek11->Text = IntToStr(VRGIR);
		EditOTLtek12->Text = IntToStr(K_SOGL_GIR);
		EditOTLtek13->Text = IntToStr(NAPRS_GIR);
		EditOTLtek14->Text = IntToStr(X_TGIR);
		EditOTLtek15->Text = IntToStr(E_TGIR);
		EditOTLtek16->Text = IntToStr(DOPGIR);
		EditOTLtek17->Text = IntToStr(PAR_GIR);
		EditOTLtek18->Text = IntToStr(T_VRGIR);
		EditOTLtek19->Text = IntToStr(T_KGIR);
		EditOTLtek20->Text = IntToStr(N_TEK_GIR);
		EditOTLtek21->Text = IntToStr(KOM_PER);
		EditOTLtek22->Text = IntToStr(OTVET_PER);
		EditOTLtek23->Text = IntToStr(V_PER);
		EditOTLtek24->Text = IntToStr(TYPE_PER);
		EditOTLtek25->Text = IntToStr(PR_PER);
		EditOTLtek26->Text = IntToStr(HOME_PER);
		EditOTLtek27->Text = IntToStr(PUT_PER);
		EditOTLtek28->Text = IntToStr(TEK_ABS_PER);
		EditOTLtek29->Text = IntToStr(TEK_OTN_PER);
		EditOTLtek30->Text = IntToStr(KOM_POV);

}; break;
//22 ��������
case 22:
{       EditOTLtek1->Text = IntToStr(OTVET_POV);
		EditOTLtek2->Text = IntToStr(V_POV);
		EditOTLtek3->Text = IntToStr(TYPE_POV);
		EditOTLtek4->Text = IntToStr(PR_POV);
		EditOTLtek5->Text = IntToStr(HOME_POV);
		EditOTLtek6->Text = IntToStr(PUT_POV);
		EditOTLtek7->Text = IntToStr(TEK_ABS_POV);
		EditOTLtek8->Text = IntToStr(TEK_OTN_POV);
		EditOTLtek9->Text = IntToStr(KOM_KAS);
		EditOTLtek10->Text = IntToStr(OTVET_KAS);
		EditOTLtek11->Text = IntToStr(V_KAS);
		EditOTLtek12->Text = IntToStr(TYPE_KAS);
		EditOTLtek13->Text = IntToStr(PR_KAS);
		EditOTLtek14->Text = IntToStr(HOME_KAS);
		EditOTLtek15->Text = IntToStr(PUT_KAS);
		EditOTLtek16->Text = IntToStr(TEK_ABS_KAS);
		EditOTLtek17->Text = IntToStr(TEK_OTN_KAS);
		EditOTLtek18->Text = IntToStr(PR_TEMP);
		EditOTLtek19->Text = IntToStr(KOM_TEMP);
		EditOTLtek20->Text = IntToStr(ZAD_TEMP1);
		EditOTLtek21->Text = IntToStr(PAR_TEMP1);
		EditOTLtek22->Text = IntToStr(ZPAR_TEMP1);
		EditOTLtek23->Text = IntToStr(X_TEMP1);
		EditOTLtek24->Text = IntToStr(VRTEMP1);
		EditOTLtek25->Text = IntToStr(E_TEMP1);
		EditOTLtek26->Text = IntToStr(DELTEMP1);
		EditOTLtek27->Text = IntToStr(LIM1TEMP1);
		EditOTLtek28->Text = IntToStr(LIM2TEMP1);
		EditOTLtek29->Text = IntToStr(DOPTEMP1);
		EditOTLtek30->Text = IntToStr(TEK_TEMP1);

}; break;
//23 ��������
case 23:
{       EditOTLtek1->Text = IntToStr(T_VRTEMP);
		EditOTLtek2->Text = IntToStr(T_KTEMP);
		EditOTLtek3->Text = IntToStr(PR_TEMP);
		EditOTLtek4->Text = IntToStr(KOM_TEMP);
		EditOTLtek5->Text = IntToStr(ZAD_TEMP2);
		EditOTLtek6->Text = IntToStr(PAR_TEMP2);
		EditOTLtek7->Text = IntToStr(ZPAR_TEMP2);
		EditOTLtek8->Text = IntToStr(X_TEMP2);
		EditOTLtek9->Text = IntToStr(VRTEMP2);
		EditOTLtek10->Text = IntToStr(E_TEMP2);
		EditOTLtek11->Text = IntToStr(DELTEMP2);
		EditOTLtek12->Text = IntToStr(LIM1TEMP2);
		EditOTLtek13->Text = IntToStr(LIM2TEMP2);
		EditOTLtek14->Text = IntToStr(DOPTEMP2);
		EditOTLtek15->Text = IntToStr(TEK_TEMP2);
		EditOTLtek16->Text = IntToStr(KOM_MOD);
		EditOTLtek17->Text = IntToStr(OTVET_MOD);
		EditOTLtek18->Text = IntToStr(PR_KLASTER);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(0);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(0);
		EditOTLtek23->Text = IntToStr(0);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(0);
		EditOTLtek26->Text = IntToStr(0);
		EditOTLtek27->Text = IntToStr(0);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(0);
}; break;
}

}
//---------------------------------------------------------------------------

//���������� ��������
//---------------------------------------------------------------------------
//--��������� �������� �1--//
void __fastcall TForm1::SBD1DebugChange(TObject *Sender)
{   // �������� ��� ��������
    EdtD1Code -> Text = IntToStr(SBD1Debug->Position);
    // ����������� �������� ��������
    EdtD1Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD1Debug->Position)/1000.0-6.8)/0.6),ffExponent,3,8);
}
//---------------------------------------------------------------------------
//--��������� �������� �1--//
void __fastcall TForm1::EdtD1CodeChange(TObject *Sender)
{   // �������� ��� ��������
    SBD1Debug -> Position = StrToInt(EdtD1Code->Text);
    // ����������� �������� ��������
   EdtD1Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD1Debug->Position)/1000.0-6.8)/0.6),ffExponent,3,8);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� �������� �2--//
void __fastcall TForm1::SBD2DebugChange(TObject *Sender)
{   // �������� ��� ��������
    EdtD2Code -> Text = IntToStr(SBD2Debug->Position);
    // ����������� �������� ��������
    EdtD2Davl -> Text = FloatToStrF(133.3*pow(10,(float)SBD2Debug->Position/1000.0-6.0),ffExponent,3,8);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� �������� �2--//
void __fastcall TForm1::EdtD2CodeChange(TObject *Sender)
{   // �������� ��� ��������
    SBD2Debug -> Position = StrToInt(EdtD2Code->Text);
    // ����������� �������� ��������
    EdtD2Davl -> Text = FloatToStrF(133.3*pow(10,(float)SBD2Debug->Position/1000.0-6.0),ffExponent,3,8);
}

//---------------------------------------------------------------------------
//--�������� ����������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNasModClick(TObject *Sender)
{
  PanelParNastr -> Visible = true;
}
//---------------------------------------------------------------------------
//--������������ ����������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualNasmod()
{
    //���������� ������� ��������������� ������� ������
    EditNastrIn0 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[0])/1000.0-6.8)/0.6),ffExponent,3,8);
    // ������� ������
    EditNastrIn1 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[1])/1000.0-6.8)/0.6),ffExponent,3,8);
    // ������ ���7
    EditNastrIn2 -> Text = FloatToStrF((float)nasmod[2]/4095.0*RRG7_MAX,ffFixed,5,1);
    //����� �������� ��� ���������
    EditNastrIn13 -> Text =FloatToStrF(133.3*pow(10,(float)nasmod[13]/1000.0-6.0),ffFixed,5,0);
    //���������������� �� ���������?
    EditNastrIn3 -> Text = ( nasmod[3] ? "��" : "���" );
    //���������� ��������� ��
    EditNastrIn4 ->Text = IntToStr(int(((float)nasmod[4]-8192.0-819.2)/204.8+2.0));
    //���������� ����������� ������������ ��� ��
    EditNastrIn5 -> Text = FloatToStrF( 1000.0 / float(nasmod[5]), ffFixed, 6, 0 );
    //���������� ����������� ������������ ��� �/�
    EditNastrIn6 -> Text = FloatToStrF( 1000.0 / float(nasmod[6]), ffFixed, 6, 0 );
    //�������� �� �������� ������
    EditNastrIn14 -> Text = IntToStr( nasmod[14] );
    //������� ������� ������ ����� ���������������� ��������
    EditNastrIn7 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[7])/1000.0-6.8)/0.6),ffExponent,3,8);
    //������� ���� � ����������� ���������������� ��������
    EditNastrIn8 -> Text = ( nasmod[8] ? "��" : "���" );
    //����������� �������� ������
    EditNastrIn11 -> Text = IntToStr((unsigned int)ceil(nasmod[11]/10.0));
    //����������� �������� ������������
    EditNastrIn12 -> Text = IntToStr((unsigned int)ceil(nasmod[12]/10.0));
    //������������ ���� � ����������
    EditNastrIn17 -> Text = ( nasmod[17] ? "��" : "���" );
}

//---------------------------------------------------------------------------
//--������������� �������� ����������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrDaClick(TObject *Sender)
{
    // ������ ������
    PanelParNastr -> Visible = false;

    //���������� ������� ��������������� ������� ������
    nasmod[0] = int((0.6*log10(StrToFloat(EditNastrTo0->Text)/100.0)+6.8)*1000.0);
    // ������� ������
    nasmod[1] = int((0.6*log10(StrToFloat(EditNastrTo1->Text)/100.0)+6.8)*1000.0);
    // ������ ���7
    nasmod[2]=StrToFloat(EditNastrTo2->Text)/RRG7_MAX*4095.0;
    //����� �������� ��� ���������
    nasmod[13]=(log10(StrToFloat(EditNastrTo13->Text)/133.3)+6.0)*1000.0;
    //���������������� �� ���������?
    EditNastrTo3 -> Text == "��" ? nasmod[3] = 1 : nasmod[3] = 0;
    //���������� ��������� ��
    nasmod[4]=int((float)(StrToInt(EditNastrTo4->Text)-1)*204.8+819.2+8192.0);
    //���������� ����������� ������������ ��� ��
    nasmod[5]=(unsigned int)(1000.0/StrToFloat(EditNastrTo5->Text));
    //���������� ����������� ������������ ��� �/�
    nasmod[6]=(unsigned int)(1000.0/StrToFloat(EditNastrTo6->Text));
    //�������� �� �������� ������
    nasmod[14] = StrToInt(EditNastrTo14 -> Text);
    //������� ������� ������ ����� ���������������� ��������
    nasmod[7]=int((0.6*log10(StrToFloat(EditNastrTo7->Text)/100.0)+6.8)*1000.0);
    //������� ���� � ����������� ���������������� ��������
    EditNastrTo8 -> Text == "��" ? nasmod[8] = 1 : nasmod[8] = 0;
    //����������� �������� ������
    nasmod[11] = StrToFloat(EditNastrTo11->Text)*10;
    //����������� �������� ������������
    nasmod[12] = StrToFloat(EditNastrTo12->Text)*10;
    //������������ ���� � ����������
    EditNastrTo17 -> Text == "��" ? nasmod[17] = 1 : nasmod[17] = 0;


    // ��������� �������� � ������
    MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time -> Caption + " : �������� �������� ������������ ������� : ");
    if ( EditNastrTo0 -> Text != EditNastrIn0 -> Text )
        MemoStat -> Lines -> Add( "���������� ������� ��������������� ������� ������ : " + EditNastrIn0 -> Text + " -> " + EditNastrTo0 -> Text );
    if ( EditNastrTo1 -> Text != EditNastrIn1 -> Text )
        MemoStat -> Lines -> Add( "������� ������ : " + EditNastrIn1 -> Text + " -> " + EditNastrTo1 -> Text );
    if ( EditNastrTo2 -> Text != EditNastrIn2 -> Text )
        MemoStat -> Lines -> Add( "������ ���7 : " + EditNastrIn2 -> Text + " -> " + EditNastrTo2 -> Text );
     if ( EditNastrTo13 -> Text != EditNastrIn13 -> Text )
        MemoStat -> Lines -> Add( "����� �������� ��� ��������� : " + EditNastrIn13 -> Text + " -> " + EditNastrTo13 -> Text );
    if ( EditNastrTo3 -> Text != EditNastrIn3 -> Text )
        MemoStat -> Lines -> Add( "���������������� �� ���������? : " + EditNastrIn3 -> Text + " -> " + EditNastrTo3 -> Text );
    if ( EditNastrTo4 -> Text != EditNastrIn4 -> Text )
        MemoStat -> Lines -> Add( "���������� ��������� �� : " + EditNastrIn4 -> Text + " -> " + EditNastrTo4 -> Text );
    if ( EditNastrTo5 -> Text != EditNastrIn5 -> Text )
        MemoStat -> Lines -> Add( "���������� ����������� ������������ ��� �� : " + EditNastrIn5 -> Text + " -> " + EditNastrTo5 -> Text );
    if ( EditNastrTo6 -> Text != EditNastrIn6 -> Text )
        MemoStat -> Lines -> Add( "���������� ����������� ������������ ��� �/� : " + EditNastrIn6 -> Text + " -> " + EditNastrTo6 -> Text );
    if ( EditNastrTo14 -> Text != EditNastrIn14 -> Text )
        MemoStat -> Lines -> Add( "�������� �� �������� ������ ����� ���������������� �������� : " + EditNastrIn14 -> Text + " -> " + EditNastrTo14 -> Text );
    if ( EditNastrTo7 -> Text != EditNastrIn7 -> Text )
        MemoStat -> Lines -> Add( "������� ������� ������ ����� ���������������� �������� : " + EditNastrIn7 -> Text + " -> " + EditNastrTo7 -> Text );
    if ( EditNastrTo8 -> Text != EditNastrIn8 -> Text )
        MemoStat -> Lines -> Add( "������� ���� � ����������� ���������������� �������� : " + EditNastrIn8 -> Text + " -> " + EditNastrTo8 -> Text );
    if ( EditNastrTo11 -> Text != EditNastrIn11 -> Text )
        MemoStat -> Lines -> Add( "����������� �������� ������ : " + EditNastrIn11 -> Text + " -> " + EditNastrTo11 -> Text );
    if ( EditNastrTo12 -> Text != EditNastrIn12 -> Text )
        MemoStat -> Lines -> Add( "����������� �������� ������������ : " + EditNastrIn12 -> Text + " -> " + EditNastrTo12 -> Text );
    if ( EditNastrTo17 -> Text != EditNastrIn17 -> Text )
        MemoStat -> Lines -> Add( "������������ ���� � ���������? : " + EditNastrIn17 -> Text + " -> " + EditNastrTo17 -> Text );

    // ������������ ����� ������ ���������� ����

    EditNastrTo0 -> Color = clWhite;
    EditNastrTo1 -> Color = clWhite;
    EditNastrTo2 -> Color = clWhite;
    EditNastrTo13 -> Color = clWhite;
    EditNastrTo3 -> Color = clWhite;
    EditNastrTo4 -> Color = clWhite;
    EditNastrTo5 -> Color = clWhite;
    EditNastrTo6 -> Color = clWhite;
    EditNastrTo14 -> Color = clWhite;
    EditNastrTo7 -> Color = clWhite;
    EditNastrTo8 -> Color = clWhite;
    EditNastrTo11 -> Color = clWhite;
    EditNastrTo12 -> Color = clWhite;
    EditNastrTo17 -> Color = clWhite;

    // ������������ ����������� ����������

    VisualNasmod();
    VisualParA();
    ParametersGrigAutomate();
    VisualParR();
    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> Clear();

    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo0->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo1->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo2->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo13->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo3->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo4->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo5->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo6->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo14->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo7->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo8->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo11->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo12->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo17->ItemIndex));

    MemoNasmod -> Lines -> SaveToFile("Nasmod\\Nasmod.txt");
}
//---------------------------------------------------------------------------
//--����� �� �������� ����������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrNetClick(TObject *Sender)
{
  PanelParNastr -> Visible = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--������� ������������ ������� ������--//
//---------------------------------------------------------------------------
void __fastcall TLogicThread::LM()
{
    // ������� ���� ������
    LogicMain();

    logic_time = (float)(TimeNew.QuadPart-TimeOld.QuadPart)/(float)(TimeFreq.QuadPart);
    logicPerSecond++;
}
//---------------------------------------------------------------------------
//--������� ������������ ������� ������� 1 ��--//
//---------------------------------------------------------------------------
void __fastcall TTimerExist::EM()
{
    // ��������� ������� ����
    Form1 -> ExternalManager();
}
//---------------------------------------------------------------------------
//--������� ������������ ������� ������� 1 ��--//
//---------------------------------------------------------------------------
void __fastcall TTimerExist::Timer1ms()
{  
    if(!ust_ready) return;
    // ������� ������
    TIME();

    Comport[0]->Dev_Timer++;
    Comport[1]->Dev_Timer++;
    Comport[2]->Dev_Timer++;
    Comport[3]->Dev_Timer++;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//--��������� ���-�� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ChBoxGraphTemp1Click(TObject *Sender)
{
 // ����������� �������������� ������� ������� ��������
    if(StrToInt(((TCheckBox*)Sender)->Hint)<SERIES_COUNT)
    { serTemp[StrToInt(((TCheckBox*)Sender)->Hint)] -> Active = ((TCheckBox*)Sender) -> Checked; }
    // �������� �������
    else
    { serArh[StrToInt(((TCheckBox*)Sender)->Hint)-SERIES_COUNT]->Active=((TCheckBox*)Sender)->Checked; }
}
//---------------------------------------------------------------------------
//--������ ������ ������ ��������--//
//---------------------------------------------------------------------------
void ArhToGraph (AnsiString graphStr)
{
    // ������ �������� - ����, ��������� - ������
    AnsiString str[SERIES_COUNT+1];
    // ������� �������
    for ( int i = 0 ; i < SERIES_COUNT+1 ; i++ ) str[i] = "";
    int byteNmb = 1;
    // ��������� ������ �� ���������
    for ( int i = 0 ; i < SERIES_COUNT+1 ; i++ )
    {
        while (graphStr[byteNmb]!=';')
        {
            str[i] += AnsiString(graphStr[byteNmb]);
            byteNmb++;
        }
        byteNmb++;
    }
    // ���������� �������� ����������
    for ( int i = 1 ; i < SERIES_COUNT+1 ; i++ )
        serArh[i-1] -> AddY(StrToFloat(str[i]),str[0]);
}
//---------------------------------------------------------------------------
//--��������� �������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxGraphArhClick(TObject *Sender)
{
    // ������� ��������
    for ( int i = 0 ; i < SERIES_COUNT ; i++ )
        serArh[i] -> Clear();

    int itemNmb = ListBoxGraphArh -> ItemIndex;

    AnsiString fName = "Graph\\" + ListBoxGraphArh->Items->operator[](itemNmb) + ".txt";
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile (fName);

    for ( int i = 0 ; i < MemoTemp -> Lines -> Count ; i++ )
        ArhToGraph( MemoTemp -> Lines -> operator [](i) );
}
//---------------------------------------------------------------------------
//--���������� ����������� � ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxStatArhClick(TObject *Sender)
{
    // ��������� ����������
    int itemNmb = ListBoxStatArh -> ItemIndex;
    AnsiString fName = "Stat\\" + ListBoxStatArh->Items->operator[](itemNmb) + ".txt";
    MemoStatArh -> Lines -> LoadFromFile (fName);
    fName = "Diag\\" + ListBoxStatArh->Items->operator[](itemNmb) + ".txt";
    // ��������� �� ����������, ���������� ����� �� ����
    if ( FileExists ( fName ) )
        MemoDiagArh -> Lines -> LoadFromFile (fName);
}
//---------------------------------------------------------------------------
//--������������ ��������--//
//---------------------------------------------------------------------------
void TForm1::VisualGraph()
{

    AnsiString graphTemp = "";

    if((shr[4]==0)||PR_NALADKA) return;    // �� ������

    // �����
    graphTemp = Label_Time -> Caption + ";";

    // ������ ���1
    if(shr[20])
    {
        graphTemp = graphTemp + FloatToStrF(float(aik[5])*RRG1_MAX/4095.0,ffFixed,5,1) + ";";
        serTemp[0] -> AddY(float(aik[5])*RRG1_MAX/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[0] -> AddY(0.0,Label_Time -> Caption);
    }

    // ������ ���2
    if(shr[21])
    {
        graphTemp = graphTemp + FloatToStrF(float(aik[6])*RRG2_MAX/4095.0,ffFixed,5,1) + ";";
        serTemp[1] -> AddY(float(aik[6])*RRG2_MAX/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[1] -> AddY(0.0,Label_Time -> Caption);
    }

    // ������ ���3
    if(shr[22])
    {
        graphTemp = graphTemp + FloatToStrF(float(aik[7])*RRG3_MAX/4095.0,ffFixed,5,1) + ";";
        serTemp[2] -> AddY(float(aik[7])*RRG3_MAX/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[2] -> AddY(0.0,Label_Time -> Caption);
    }

    // ������ ���4
    if(shr[23])
    {
        graphTemp = graphTemp + FloatToStrF(float(aik[8])*RRG4_MAX/4095.0,ffFixed,5,1) + ";";
        serTemp[3] -> AddY(float(aik[8])*RRG4_MAX/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[3] -> AddY(0.0,Label_Time -> Caption);
    }

    // ������ ���5
    if(shr[24])
    {
        graphTemp = graphTemp + FloatToStrF(float(aik[9])*RRG5_MAX/4095.0,ffFixed,5,1) + ";";
        serTemp[4] -> AddY(float(aik[9])*RRG5_MAX/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[4] -> AddY(0.0,Label_Time -> Caption);
    }

    // ������ ���6
    if(shr[25])
    {
        graphTemp = graphTemp + FloatToStrF(float(aik[10])*RRG6_MAX/4095.0,ffFixed,5,1) + ";";
        serTemp[5] -> AddY(float(aik[10])*RRG6_MAX/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[5] -> AddY(0.0,Label_Time -> Caption);
    }
    // ���. �������� ��� ��
    if(shr[29])
    {

        graphTemp = graphTemp + FloatToStrF(float(aik[12])*CESAR_MAX_IP/4095.0,ffFixed,5,1) + ";";
        serTemp[6] -> AddY(float(aik[12])*CESAR_MAX_IP/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[6] -> AddY(0.0,Label_Time -> Caption);
    }
    // ���. �������� ��� ��
    if(shr[29])
    {

        graphTemp = graphTemp + FloatToStrF(float(aik[13])*CESAR_MAX_IP/4095.0,ffFixed,5,1) + ";";
        serTemp[7] -> AddY(float(aik[13])*CESAR_MAX_IP/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[7] -> AddY(0.0,Label_Time -> Caption);
    }
    // ���. �������� ��� �/�
    if(shr[27]||shr[28])
    {

        graphTemp = graphTemp + FloatToStrF(float(aik[14])*CESAR_MAX_PD/4095.0,ffFixed,5,1) + ";";
        serTemp[8] -> AddY(float(aik[14])*CESAR_MAX_PD/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[8] -> AddY(0.0,Label_Time -> Caption);
    }
    // ���. �������� ��� �/�
    if(shr[27]||shr[28])
    {

        graphTemp = graphTemp + FloatToStrF(float(aik[15])*CESAR_MAX_PD/4095.0,ffFixed,5,1) + ";";
        serTemp[9] -> AddY(float(aik[15])*CESAR_MAX_PD/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[9] -> AddY(0.0,Label_Time -> Caption);
    }
    // ��������
    if(shr[17])
    {

        graphTemp = graphTemp + FloatToStrF(float(D_D3)/10000*DAVL_MAX,ffFixed,5,1) + ";";
        serTemp[10] -> AddY(float(D_D3)/10000*DAVL_MAX, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[10] -> AddY(0.0,Label_Time -> Caption);
    }


    // �������� ������
    MemoGraph -> Lines -> Add ( graphTemp );
}
//---------------------------------------------------------------------------
//--�������: ���/����--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDebugClick(TObject *Sender)
{
    LabelOTLreg -> Visible = (bool) StrToInt( ((TButton*)Sender) -> Hint );
    LabelOTLreg -> Visible = pr_otl = (bool)StrToInt(((TButton*)Sender)->Hint);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//--���������� ��������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ImgFkKamClick(TObject *Sender)
{
    // ����������� � ����� �������� ������ �����, ���� � �������������� �� �����
    if ( PCMain -> ActivePage == TSWork ) return;
    // ������������ ������
    if  (
                (((TImage*)Sender)->Name) == "fn_shl" ||
                (((TImage*)Sender)->Name) == "fn_kam" ||
                (((TImage*)Sender)->Name) == "tmn"
        )
    {
        BtnDeviceOn -> Caption = "���.";
        BtnDeviceOff -> Caption = "����.";
    }
    else    if  (
                (((TImage*)Sender)->Name) == "pin"
                )
    {
        BtnDeviceOn -> Caption = "�����";
        BtnDeviceOff -> Caption = "����";
    }
    else
    {
        BtnDeviceOn -> Caption = "����.";
        BtnDeviceOff -> Caption = "����.";
    }
    // ���������� ������ ����������
    LblDeviceName -> Caption = ((TImage*)Sender) -> Hint;
    PnlDevice -> Hint = ((TImage*)Sender) -> Name;

    if((((TImage*)Sender)->Name) == "kl1")
        PnlDevice -> Top = ((TImage*)Sender)->Top - 50;
    else if((((TImage*)Sender)->Name) == "kl10")
        PnlDevice -> Top = ((TImage*)Sender)->Top +10;
    else
        PnlDevice -> Top = ((TImage*)Sender)->Top - 90;
    PnlDevice -> Left = ((TImage*)Sender)->Left - 90;
    PnlDevice -> Visible = true;
    PnlDevice -> BringToFront();
}
//---------------------------------------------------------------------------
//--���������� ������������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDeviceOnClick(TObject *Sender)
{
    // ������ ���������� ������ ���������� ���������� ����������
    ((TButton*)Sender) -> Parent -> Visible = false;
    // ���� ������ ������� �����, �� �����
    if (((TButton*)Sender) -> Name == "BtnDeviceExit" ) return;
    if ( ((TButton*)Sender) -> Parent -> Hint == "kl1" )       SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x200);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x400);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl3" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x4000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl4" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x2000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl5" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x1000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_shl1" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x1000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_shl2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_tmn" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x04);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_d4" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x08);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_d5" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x10);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "tmn" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x40);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fn_kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x10);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_nap1" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x100);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_nap2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x400);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_nap3" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x800);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fn_shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x08);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_rrg1" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_rrg2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_rrg3" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x04);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_rrg4" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x08);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_rrg5" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x10);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_rrg6" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x20);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "pin" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 0, 0x400);
            SetOut(1, 0, 0x200);
        }
        else
        {
            SetOut(1, 0, 0x400);
            SetOut(0, 0, 0x200);
        }


    }
    // ��������� �������� � ������
    MemoStat -> Lines -> Add( DateToStr(Date()) + " " + TimeToStr(Time()) + " : ���������� ��������� : " + LblDeviceName -> Caption + " : " + ((TButton*)Sender)->Caption);
}
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ���������--//
void TForm1::VisualZagol()
{
    int i=0;
    AnsiString TempStr = "";
    // �����
    EditNormName -> Text = NormNames[norma];
    // ���������� �������, ��� ���� ���� ����������
    #define SHR_VALUE_COUNT 10
    // ������� ���������� �������� ����� (���� �����)
    unsigned char SHRValue[SHR_VALUE_COUNT] = {8,5,3,7,1,4,6,10,2,9};
    // ����������� ���������� ������� � ������� �������� ����������
    for(i=0; i<SHR_VALUE_COUNT; i++)
        if(shr[SHRValue[i]])
        {   Form1 -> EditRName -> Text = SHRNames[SHRValue[i]];
            switch(SHRValue[i])
            {   case 1: Form1 -> EditSHRName -> Text = SHR1Names[shr[SHRValue[i]]]; break;
                case 2: Form1 -> EditSHRName -> Text = SHR2Names[shr[SHRValue[i]]]; break;
                case 3: Form1 -> EditSHRName -> Text = SHR3Names[shr[SHRValue[i]]]; break;
                case 4: Form1 -> EditSHRName -> Text = SHR4Names[shr[SHRValue[i]]]; break;
                case 5: Form1 -> EditSHRName -> Text = SHR5Names[shr[SHRValue[i]]]; break;
                case 6: Form1 -> EditSHRName -> Text = SHR6Names[shr[SHRValue[i]]]; break;
                case 7: Form1 -> EditSHRName -> Text = SHR7Names[shr[SHRValue[i]]]; break;
                case 8: Form1 -> EditSHRName -> Text = SHR8Names[shr[SHRValue[i]]]; break;
                case 9: Form1 -> EditSHRName -> Text = SHR9Names[shr[SHRValue[i]]]; break;
                case 10: Form1 -> EditSHRName -> Text = SHR10Names[shr[SHRValue[i]]]; break;
            }


            // � ������ 8 ��� 3
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==3))
            {
                if(shr[5]==6)
					Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
                else if (shr[5]==7)
                {
                    if(shr[6]==25)
                        Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                    else if(shr[6]==5)
                    {
                        if(shr[10]==13)
                            Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                        else
                            Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
                    }
                    else
                        Form1 -> EditSHRName -> Text = SHR6Names[shr[6]];
                }
                else
                    Form1 -> EditSHRName -> Text = SHR5Names[shr[5]];
            }

            // � ������ 8 ��� 4 ������ ���� ������ 7
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==4))
               Form1 -> EditSHRName -> Text = SHR7Names[shr[7]];

            // � ������ 5 ��� 6
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==6))
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];

            // � ������ 5 ��� 7
            if ((SHRValue[i]==5)&&(shr[SHRValue[i]]==7))
            {
                if(shr[6]==25)
                    Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                else if(shr[6]==5)
                {
                    if(shr[10]==13)
                        Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                    else
                        Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
                }
                else
                    Form1 -> EditSHRName -> Text = SHR6Names[shr[6]];
            }

            //� ������ 6 � ���� 5
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==5))
            {
                if(shr[10]==13)
                    Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                else
                    Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
            }

            // � ������ 6 ��� 25 ������ ���� ������ 2
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==25))
            Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];

            // � ������ 10 ��� 13 ������ ���� ������ 2
            if((SHRValue[i]==10)&&(shr[SHRValue[i]]==13))
            {   Form1 -> EditSHRName -> Text = SHR2Names[shr[2]]; }

            // � ������ 3 ��� 2 ������ ���� ������ 1
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==2))
            {   Form1 -> EditSHRName -> Text = SHR1Names[shr[1]]; }
            // � ������ 3 ��� 9 ������ ���� ������ 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==9))
            {   Form1 -> EditSHRName -> Text = SHR2Names[shr[2]]; }
            // � ������ 3 ��� 12 ������ ���� ������ 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==12))
            {   Form1 -> EditSHRName -> Text = SHR2Names[shr[2]]; }
            // � ������ 3 ��� 33 ������ ���� ������ 4
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==33))
            {   Form1 -> EditSHRName -> Text = SHR4Names[shr[4]]; }
            // � ������ 3 ��� 52 ������ ���� ������ 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==52))
            {   Form1 -> EditSHRName -> Text = SHR2Names[shr[2]]; }

            // ����� ���������� ������

            // � ������ 4 ��� 28 ������� �����
            if(shr[4]==28)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(T_SBROSHE-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 13 ������� �����
            if(shr[4]==13)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][13]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 16 ������� �����
            if(shr[4]==16)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][13]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 19 ������� �����
            if(shr[4]==19)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[14]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }

            return;
        }

    // ��� �������� ������� �������� ����
    Form1 -> EditRName   -> Text = "";
    Form1 -> EditSHRName -> Text = "";

}
//---------------------------------------------------------------------------
//--����������� ����������� �� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualDiagn ()
{
        #define LB_ERROR_COUNT 6
        TLabel *LbError[LB_ERROR_COUNT] =
        {
                Form1 -> LbError1 ,
                Form1 -> LbError2 ,
                Form1 -> LbError3 ,
                Form1 -> LbError4 ,
                Form1 -> LbError5 ,
                Form1 -> LbError6
        };

        // ��������� ��������� ��� ������������ �����������
        BYTE mask = 0x01;
        for ( int i = 0 ; i < DIAGN_S_COUNT ; i++ )
        {
                if ( diagnS[i] != diagnSOld[i] )
                {
                        mask = 0x01;
                        for ( int j = 0 ; j < 8 ; j++ )
                        {
                                if ((diagnS[i]&mask)!=(diagnSOld[i]&mask))
                                {
                                        if ( diagnS[i]&mask )
                                        {
                                                Form1->MemoDiag->Font->Color = clRed;
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : ���������� : "+DiagnSNames[i*8+j]);
                                        }
                                        else
                                        {
                                                Form1->MemoDiag->Font->Color = clLime;
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : ����� : "+DiagnSNames[i*8+j]);
                                        }
                                }
                                mask <<= 1;
                        }
                        diagnSOld[i] = diagnS[i];
                }
            }
        for ( int i = 0 ; i < DIAGN_COUNT ; i++ )
        {
                if ( diagn[i] != diagnOld[i] )
                {
                        mask = 0x01;
                        for ( int j = 0 ; j < 8 ; j++ )
                        {
                                if ((diagn[i]&mask)!=(diagnOld[i]&mask))
                                {
                                        if ( diagn[i]&mask )
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : ���������� : "+DiagnNames[i*8+j]);
                                        else
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : ����� : "+DiagnNames[i*8+j]);
                                }
                                mask <<= 1;
                        }
                        diagnOld[i] = diagn[i];
                }
        }

        for ( int i = 0 ; i < LB_ERROR_COUNT ; i++ )
        {
                LbError[i] -> Caption = "";
        };

        int ErrNmb = 0;

        Form1 -> LBError -> Clear();

        for ( int i = 0 ; i < DIAGN_COUNT ; i++ )
        {
                mask = 0x01;
                for ( int j = 0 ; j < 8 ; j++ )
                {
                        if ( diagn[i] & mask )
                        {
                                Form1 -> LBError -> Items -> Add( DiagnNames[i*8+j] );
                                if ( ErrNmb < LB_ERROR_COUNT )
                                {
                                        LbError[ErrNmb] -> Caption = DiagnNames[i*8+j];
                                        ErrNmb++;
                                }
                        }
                        mask <<= 1;
                }
        }
        for ( int i = 0 ; i < DIAGN_S_COUNT ; i++ )
        {
                mask = 0x01;
                for ( int j = 0 ; j < 8 ; j++ )
                {
                        if ( diagnS[i] & mask )
                        {
                                Form1 -> LBError -> Items -> Add( DiagnSNames[i*8+j] );
                                if ( ErrNmb < LB_ERROR_COUNT )
                                {
                                        LbError[ErrNmb] -> Caption = DiagnSNames[i*8+j];
                                        ErrNmb++;
                                }
                        }
                        mask <<= 1;
                }
        }
        if ( Form1 -> LBError -> Items -> Count > LB_ERROR_COUNT )   Form1 -> BitBdVall -> Visible = true;
        else                                                            Form1 -> BitBdVall -> Visible = false;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ������� ���������--//
void TForm1::VisualOperatorDlg()
{

    if(shr[3]==7)
    {
        APanel_String1 -> Caption = "������ ��������";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "����� ������ ���������";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Caption = "���� � �������� ?";
        APanel_String3 -> Visible = true;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[3]==8)
    {
        APanel_String1 -> Caption = "�������� ���� ?";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[3]==51)
    {
        APanel_String1 -> Caption = "������ ������ � ���� ?";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }

    else if(shr[6]==23)
    {
        APanel_String1 -> Caption = "������ ��������";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "��������� �������";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Caption = "�������� ���� ?";
        APanel_String3 -> Visible = true;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else
    {
        APanel -> Visible = false;
        pr_yel = 0;
    }


}

void __fastcall TForm1::APanel_ButClick(TObject *Sender)
{
    switch(StrToInt(((TPanel*)Sender)->Hint))
    {
        case 1: { otvet=1; break; }
        case 2: { otvet=2; break; }
    }
    APanel -> Visible = false;
    pr_yel = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--����������� ����� ������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBdVallClick(TObject *Sender)
{
    PnlDiagm -> Visible = true;
}
//---------------------------------------------------------------------------
//--������� ����� ������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDiagmClick(TObject *Sender)
{
    ((TButton*)Sender) -> Parent -> Visible = false;
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������ �������--//
void __fastcall TForm1::PanelRCClick(TObject *Sender)
{  // �������� ������� �������
    ListBoxCondition -> Items -> Clear();
    // ������ ������� �������
    switch (StrToInt(((TPanel*)Sender)->Hint))
    {
        case 1:
        {
             LblRejim -> Caption = "������� ������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ���������� ���-������
            if(!(zin[0]&0x02))  { ListBoxCondition -> Items -> Add("��� ���������� ��� ������"); }
            // ���� ���������� ���
            if(!(zin[0]&0x04))  { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
		    // ���� ����� � �/���������
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
            // ���� �������� ������ ���-������
            if(!(zin[2]&0x20))  { ListBoxCondition -> Items -> Add("��� ��������� ������� ��� ������"); }
            // ���� �������� ������ ���
            if(!(zin[2]&0x2000))  { ListBoxCondition -> Items -> Add("��� ��������� ������� ���"); }
            // ���� ������� ���
            if(!(zin[2]&0x100))  { ListBoxCondition -> Items -> Add("��� ������� ���"); }
            // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������ �����������"); }
		    // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������ ��������"); }
		    // ���� ����� � ������������ ����������� �������
		    if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ �������"); }
		    // �� ������� �� ���� �����
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++) // �� ������������� R_33(); R_35();
                if((i!=33)&&(i!=35))
                    if(shr[i])
                        ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;
		case 3:
		{   LblRejim -> Caption = "������� ���� (��)";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ���������� ���-������
            if(!(zin[0]&0x02))  { ListBoxCondition -> Items -> Add("��� ���������� ��������� ������"); }
            // ���� ���������� ���
            if(!(zin[0]&0x04))  { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
            // ���� ���������� �������
		    if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� �/���������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
		    // ���� ����� � �2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � �2"); }
            // ���� ����� � �3
		    if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
		    // ���� ����� � �/���������
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
            // ���� �������� ������ ���-������
            if(!(zin[2]&0x20))  { ListBoxCondition -> Items -> Add("��� ��������� ������� ��������� ������"); }
            // ���� �������� ������ ���
            if(!(zin[2]&0x2000))  { ListBoxCondition -> Items -> Add("��� ��������� ������� ���"); }
            // ���� ������� ���
            if(!(zin[2]&0x100))  { ListBoxCondition -> Items -> Add("��� ������� ���"); }
		     // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� ������������"); }
		    // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ �������� ������������"); }
		    // ���� ����� � ������������ ����������� �������
		    if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� �������"); }
		    // �� ������� �� ���� �����
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++) // �� ������������� R_33(); R_35();
                if((i!=33)&&(i!=35))
                    if(shr[i])
                        ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;
        case 5:
        {
             LblRejim -> Caption = "����� ��";
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if(!shr[3]&&!shr[4])  { ListBoxCondition -> Items -> Add("�� ������� ����������"); }
		    // �� ������� ����� 5
		    if(shr[5])  { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
        }; break;
        case 6:
		{   LblRejim -> Caption = "���� �������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
		    // ���� ����� � �2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � �2"); }
            // ���� ����� � �3
		    if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
		    // ���� ����� � �/���������
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
		    // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������"); }
            // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ �������� ������������"); }
		    // ���� ����� � ������������ ����������� �������
		    if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� �������"); }
		    // �� ������� �� ���� �����
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++) // �� ������������� R_33(); R_35();
                if((i!=33)&&(i!=35))
                    if(shr[i])
                        ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;
       case 7:
		{   LblRejim -> Caption = "���������� ���������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
		    // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������"); }
            // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ �������� ������������"); }
		    // ���� ����� � ������������ ����������� �������
		    if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� �������"); }
		    // �� ������� �� ���� �����
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++) // �� ������������� R_33(); R_35();
                if((i!=33)&&(i!=35))
                    if(shr[i])
                        ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;
         case 9:
		{   LblRejim -> Caption = "�������� ������������ ����";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
		    // ������ ������
		    if((zin[1]&0x0C)!=0x04) { ListBoxCondition -> Items -> Add("������� ������ �� ������"); }
		    // ������ ������� ������� � HOME
		    if(!(zin[4]&0x40)) { ListBoxCondition -> Items -> Add("������ ������� ������� �� � HOME"); }
            // ������ ����������� ������������ � HOME
		    if(!(zin[4]&0x200)) { ListBoxCondition -> Items -> Add("������ ������������ ����������� �� � HOME"); }
            // ������ �������� ������������ � HOME
		    if(!(zin[4]&0x1000)) { ListBoxCondition -> Items -> Add("������ �������� ������������ �� � HOME"); }
            // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� ������������"); }
		    // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ �������� ������������"); }
		    // ���� ����� � ������������ ����������� �������
		    if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� �������"); }
		    // �� ������� �� ���� �����
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++) // �� ������������� R_33(); R_35();
                if((i!=33)&&(i!=35))
                    if(shr[i])
                        ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;
        case 109:
		{   LblRejim -> Caption = "��������� ������������ ����";
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� �����  9
		    if(!shr[9]) { ListBoxCondition -> Items -> Add("�� ������� ����� " + SHRNames[9]); }
		}; break;
       	case 10:
		{   LblRejim -> Caption = "������� ������� ������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
		    // ���� ����� � �2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � �2"); }
            // ���� ����� � �3
		    if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
       		    // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 10
		    if(shr[10]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[10]); }
		}; break;
        case 11:
		{   LblRejim -> Caption = "������� ������� ������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
 		    // ���� ����������� � HOME
		    if(!(zin[4]&0x200)) { ListBoxCondition -> Items -> Add("������ ������������ ����������� �� � HOME"); }
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
       		    // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 11
		    if(shr[11]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[11]); }
		}; break;
        case 12:
        {   LblRejim -> Caption = "����������� ����������� � HOME";
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ����������� ������������"); }
		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
       		    // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
                    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
       		    // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            }; break;

            case 13:
            {   LblRejim -> Caption = "����������� ����������� ������/�����";
		    // ���� ����� � ������������ ����������� ������������
		    if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������"); }
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // ������ ������
		    if((zin[1]&0x0C)!=0x04) { ListBoxCondition -> Items -> Add("������� ������ �� ������"); }
		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
            // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
            // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
            // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
            // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // ������ � home
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("������ ������� �� � HOME"); }
            }; break;

		case 113:
		{   LblRejim -> Caption = "���� ����������";  // STOP ���������� -- ������� ������� ������
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
       		    // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
                }; break;
		case 213:
		{   LblRejim -> Caption = "C���� ������ ����������";
		}; break;
        case 17:
		{   LblRejim -> Caption = "��������������� ��";
		    // ���� ����� � ��
		    if(diagnS[0]&0x80) { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		}; break;
        case 18:
		{   LblRejim -> Caption = "������� ��";
		    // ���� ����� � ��
		    if(diagnS[0]&0x40) { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		}; break;
		case 19:
		{   LblRejim -> Caption = "������� ��";
		    // ���� ����� � ��
		    if(diagnS[0]&0x80) { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		}; break;
        case 14:
		{   LblRejim -> Caption = "����������� �������� � HOME";

            // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������ ��������"); }
		    // ������ ����������� ������������ � HOME
		    if(!(zin[4]&0x200)) { ListBoxCondition -> Items -> Add("������ ������������ ����������� �� � HOME"); }
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
		}; break;

        case 15:
		{   LblRejim -> Caption = "����������� �������� ������/�����";

		    // ���� ����� � ������������ �������� ������������
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ������������ ������������ ��������"); }
		    // ������ ����������� ������������ � HOME
		    if(!(zin[4]&0x200)) { ListBoxCondition -> Items -> Add("������ ������������ ����������� �� � HOME"); }
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
		}; break;

        case 20:
		{   LblRejim -> Caption = "�������� ���1";
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 20
		    if(shr[20]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[20]); }
		}; break;
		case 120:
		{   LblRejim -> Caption = "��������� ���1";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 20
		    if(!shr[20]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[20]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 21:
		{   LblRejim -> Caption = "�������� ���2";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 21
		    if(shr[21]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[21]); }
		}; break;
		case 121:
		{   LblRejim -> Caption = "��������� ���2";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 21
		    if(!shr[21]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[21]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 22:
		{   LblRejim -> Caption = "�������� ���3";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 22
		    if(shr[22]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[22]); }
		}; break;
		case 122:
		{   LblRejim -> Caption = "��������� ���3";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 22
		    if(!shr[22]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[22]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 23:
		{   LblRejim -> Caption = "�������� ���4";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 23
		    if(shr[23]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[23]); }
		}; break;
		case 123:
		{   LblRejim -> Caption = "��������� ���4";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 23
		    if(!shr[23]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[23]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 24:
		{   LblRejim -> Caption = "�������� ���5";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 24
		    if(shr[24]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[24]); }
		}; break;
		case 124:
		{   LblRejim -> Caption = "��������� ���5";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 24
		    if(!shr[24]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[24]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 25:
		{   LblRejim -> Caption = "�������� ���6";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 25
		    if(shr[25]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[25]); }
		}; break;
		case 125:
		{   LblRejim -> Caption = "��������� ���6";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 25
		    if(!shr[25]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[25]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 26:
		{   LblRejim -> Caption = "�������� ���7 � �����";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
		}; break;
		case 126:
		{   LblRejim -> Caption = "�������� ���7 � ������";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
       		// �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
		}; break;
		case 226:
		{   LblRejim -> Caption = "��������� ���7";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 26
		    if(!shr[26]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[26]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;

        case 27:
		{   LblRejim -> Caption = "�������� ��� �����";
  		    // ���� ���������� �/���������
		    if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� �/���������"); }
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 27
		    if(shr[27]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[27]); }
		}; break;
		case 127:
		{   LblRejim -> Caption = "��������� ��� �����";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 27 ��� 28
            if((!shr[27])&&(!shr[28])) { ListBoxCondition -> Items -> Add("��� ����� �� �������"); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
                    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
         case 28:
		{   LblRejim -> Caption = "�������� ��� �� ��������";
  		    // ���� ���������� �/���������
		    if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� �/���������"); }
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 28
		    if(shr[28]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[28]); }
		}; break;
        case 29:
		{   LblRejim -> Caption = "�������� ��� ��������";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 29
		    if(shr[29]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]); }
		}; break;
		case 129:
		{   LblRejim -> Caption = "��������� ��� ��������";
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // ������� ����� 29
                    if(!shr[29]) { ListBoxCondition -> Items -> Add("��� �������� �� �������"); }
            // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
                    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
        case 33:
		{   LblRejim -> Caption = "�������� ������ ������";
            // �� ������� ����� 33
		    if(shr[33]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[33]); }
		}; break;
		case 34:
		{   LblRejim -> Caption = "��������� ������ ������";
		    // ������� ����� 33
            if(!shr[33]) { ListBoxCondition -> Items -> Add("�� ������� ����� <������ ������>"); }
		}; break;
		case 35:
		{   LblRejim -> Caption = "�������� ������ ������������";
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
		}; break;
		case 36:
		{   LblRejim -> Caption = "��������� ������ ������������";
		    // ������� ����� 35
            if(!shr[35]) { ListBoxCondition -> Items -> Add("�� ������� ����� <������ ������������>"); }
		}; break;
		case 37:
		{   LblRejim -> Caption = "������ � HOME";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
       		    // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
		}; break;
		case 38:
		{   LblRejim -> Caption = "������ ����";
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
       		    // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
       		    // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // ��������� �����
            if((zin[1]&0xC0)!=0x80) { ListBoxCondition -> Items -> Add("��������� �� �����"); }
            // ����������� � HOME
            if(!(zin[4]&0x200)&&!PR_KLASTER) { ListBoxCondition -> Items -> Add("������ ������������ ����������� �� � HOME"); }
		}; break;
        case 39:
		{   LblRejim -> Caption = "������� � HOME";
            // ���� ����� � �������� �������
		    if(diagnS[2]&0x04) { ListBoxCondition -> Items -> Add("��� ����� � �������� �������"); }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
		}; break;

            case 40:
		{   LblRejim -> Caption = "������� �����/����";
            // ���� ����� � �������� �������
		    if(diagnS[2]&0x04) { ListBoxCondition -> Items -> Add("��� ����� � �������� �������"); }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
       		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
       		    // �� ������� ����� 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[6]); }
       		    // �� ������� ����� 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[7]); }
       		    // �� ������� ����� 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[8]); }
       		    // �� ������� ����� 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
		}; break;

        case 100:
        {
             LblRejim -> Caption = "����� �����";
             ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
             if(!shr[5]&&shr[4])  { ListBoxCondition -> Items -> Add("��� ����������� ������: ����� ��"); }
		    // �� ������� ����� 3
		    if(shr[3])  { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
        }; break;

        default: return;     // ����������� �������
        }
    // ������������ �������� ����������� �����
    if ( StrToInt(((TPanel*)Sender)->Hint) != 140 )
        if ( MessageDlg("������������� ������ ������: " + LblRejim -> Caption + "?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) != mrYes ) return;
    // ���� �� ��������� ������� �������
    if ( ListBoxCondition -> Items -> Count )
        PnlCondition -> Visible = true;
    // ���� ��� ������� ��������� �������� �������
    else
    {
        // �������� ��� �������
        qkk = StrToInt(((TPanel*)Sender)->Hint);
        // ��������� �������� � ������
        MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time->Caption + ": ������� �����: <" + LblRejim -> Caption + ">" );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������, ����������� ��� �������� ����������--//
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	// ���������� � ��������� ������������ ������
	TimerExist  -> Terminate();
	LogicThread -> Terminate();
    MemoStat -> Lines -> Add( "<<< " + Label_Time -> Caption + " | ��������� ��������� >>>");
	// ���������� �������
	Save_Stat();
    SaveGasData();
    //AZdrive_Save();

	// ���������� ����
    ISO_DriverClose();
    ISO813_DriverClose();
    ISODA_DriverClose();

    if(Comport[0]->State)
    {
        Comport[0]->Port.Close();
        Comport[0]->State = 0;
    }
    if(Comport[1]->State)
    {
        Comport[1]->Port.Close();
        Comport[1]->State = 0;
    }
    if(Comport[2]->State)
    {
        Comport[2]->Port.Close();
        Comport[2]->State = 0;
    }
    if(Comport[3]->State)
    {
        Comport[3]->Port.Close();
        Comport[3]->State = 0;
    }
}
//---------------------------------------------------------------------------
// ���������� ����������, ����������� � ����
void TForm1::Save_Stat()
{
    // ��������� �����
    AnsiString fileName;

    // ����������� ��������� � �������� ������
    if ( ! DirectoryExists("Diag") ) CreateDir("Diag");
    fileName = "Diag\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    if ( ! FileExists( fileName ) )
    {
        int fileID = FileCreate ( fileName );
        FileClose( fileID );
    }
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile( fileName );
    for ( int i = 0 ; i < MemoDiag -> Lines -> Count ; i++ )
        MemoTemp -> Lines -> Add( MemoDiag -> Lines -> operator [](i) );
    MemoTemp -> Lines -> SaveToFile( fileName );

    

    // �������� ������������ � �������� ������
    if ( ! DirectoryExists("Stat") ) CreateDir("Stat");
    fileName = "Stat\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    if ( ! FileExists( fileName ) )
    {
        int fileID = FileCreate ( fileName );
        FileClose( fileID );
    }
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile( fileName );
    for ( int i = 0 ; i < MemoStat -> Lines -> Count ; i++ )
        MemoTemp -> Lines -> Add( MemoStat -> Lines -> operator [](i) );
    MemoTemp -> Lines -> SaveToFile( fileName );

    // ������� ������� ���������� ��������
    if ( ! DirectoryExists("Graph") ) CreateDir("Graph");
    fileName = "Graph\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    if ( ! FileExists( fileName ) )
    {
        int fileID = FileCreate ( fileName );
        FileClose( fileID );
    }
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile( fileName );
    for ( int i = 0 ; i < ( MemoGraph -> Lines -> Count - 1 ); i++ )
        MemoTemp -> Lines -> Add( MemoGraph -> Lines -> operator [](i) );
    MemoTemp -> Lines -> SaveToFile( fileName );

}
//---------------------------------------------------------------------------
//--����������� ���������� ��� ��������--//
//---------------------------------------------------------------------------
#define kodKlL 37
#define kodKlV 38
#define kodKlP 39
#define kodKlN 40
//---------------------------------------------------------------------------
//--����������� ���������� ��� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)

{

    if ( Key == kodKlP )
    {
        klGir_tV = 1; // ������� ������� ������� > "������"

    }
    if ( Key == kodKlL )
    {
        klGir_tN = 1; // ������� ������� ������� < "�����"

    }

        if ( Key == kodKlV )
    {
        klGir_gV = 1; // ������� ������� ������� > "�����"

    }
    if ( Key == kodKlN )
    {
        klGir_gN = 1; // ������� ������� ������� < "����"

    }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{

     if ( Key == kodKlP )
    {
        klGir_tV = 0; // ������� ������� ������� > "������"

    }
    if ( Key == kodKlL )
    {
        klGir_tN = 0; // ������� ������� ������� < "�����"

    }

        if ( Key == kodKlV )
    {
        klGir_gV = 0; // ������� ������� ������� > "�����"

    }
    if ( Key == kodKlN )
    {
        klGir_gN = 0; // ������� ������� ������� < "����"

    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TPControlChange(TObject *Sender)
{
    BtnParA->Visible=!(TPControl -> ActivePage == TabSheet15);
}
//---------------------------------------------------------------------------
void __fastcall SComport::Timer_Com1_Timer(TObject *Sender)
{
    //return;
try
{
    Comport[0]->CB_status->Checked = State;
	CB_nal->Checked = Pr_nal;
	LBL_otl->Visible = (!State || Pr_nal);

	// ��������� ��������� � ���� �������
	if(!State||!ust_ready) return;

	// ����������� ������/��������
	RB_prd->Checked = !DevState;
	RB_prm->Checked = DevState;

	// ���� ������ �������
	if((DZaslVAT[0]->RCom)&&(!DevState)) PortTask |= 0x100; //

	if(!PortTask && !Pr_nal) PortTask |= 0x01; // ��������� �������������� �������

	if(PortTask & 0x100)
	{
		DevErr = DZaslVAT[0]->DZaslVAT_Manage(DevState,1);
		if(DevState > 1)
		{
			DevErr ? diagnS[DZaslVAT[0]->diagnS_byte] |= DZaslVAT[0]->diagnS_mask : diagnS[DZaslVAT[0]->diagnS_byte] &= (~DZaslVAT[0]->diagnS_mask);
			PortTask &= (~0x100);
			DZaslVAT[0]->RCom = 0;
			DevState = 0;
		}
		return;
	}
	else if(PortTask & 0x01)
	{
		DevErr = DZaslVAT[0]->DZaslVAT_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[DZaslVAT[0]->diagnS_byte] |= DZaslVAT[0]->diagnS_mask : diagnS[DZaslVAT[0]->diagnS_byte] &= (~DZaslVAT[0]->diagnS_mask);
			PortTask &= (~0x01);
			DevState = 0;
		}
		return;
	}
    }
catch (Exception &exception)
{
        if(!com1_err_alarm)
        {
                com1_err_alarm = 1;
                if(State)
                {
                        State = 0;
			            Port.Close();
			            BTN_reset->Caption = "���� �����";
                }
                ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void __fastcall SComport::Timer_Com2_Timer(TObject *Sender)
{
    //return;
try
{
    Comport[1]->CB_status->Checked = State;
	CB_nal->Checked = Pr_nal;
	LBL_otl->Visible = (!State || Pr_nal);

	// ��������� ��������� � ���� �������
	if(!State||!ust_ready) return;

	// ����������� ������/��������
	RB_prd->Checked = !DevState;
	RB_prm->Checked = DevState;

	if(!PortTask && !Pr_nal)
    {
        PortTask |= 0x0D; // ��������� �������������� �������
        if(!PR_KLASTER) PortTask |= 0x02; // �1 ������ � ��������� ��������
    }

    if(PortTask & 0x01)
	{
		DevErr = TRMD[0]->TRMD_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[TRMD[0]->diagnS_byte] |= TRMD[0]->diagnS_mask : diagnS[TRMD[0]->diagnS_byte] &= (~TRMD[0]->diagnS_mask);
			PortTask &= (~0x01);
			DevState = 0;
		}
		return;
	}

    else if(PortTask & 0x02)
	{
		DevErr = Dat_MERA[0]->DatMERA_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[Dat_MERA[0]->diagnS_byte] |= Dat_MERA[0]->diagnS_mask : diagnS[Dat_MERA[0]->diagnS_byte] &= (~Dat_MERA[0]->diagnS_mask);
			PortTask &= (~0x02);
			DevState = 0;
		}
		return;
	}

    else if(PortTask & 0x04)
	{
		DevErr = Dat_MERA[1]->DatMERA_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[Dat_MERA[1]->diagnS_byte] |= Dat_MERA[1]->diagnS_mask : diagnS[Dat_MERA[1]->diagnS_byte] &= (~Dat_MERA[1]->diagnS_mask);
			PortTask &= (~0x04);
			DevState = 0;
		}
		return;
	}

    else if(PortTask & 0x08)
	{
		DevErr = Dat_MERA[2]->DatMERA_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[Dat_MERA[2]->diagnS_byte] |= Dat_MERA[2]->diagnS_mask : diagnS[Dat_MERA[2]->diagnS_byte] &= (~Dat_MERA[2]->diagnS_mask);
			PortTask &= (~0x08);
			DevState = 0;
		}
		return;
	}
    }
catch (Exception &exception)
{
        if(!com2_err_alarm)
        {
                com2_err_alarm = 1;
                if(State)
                {
                        State = 0;
			            Port.Close();
			            BTN_reset->Caption = "���� �����";
                }
                ShowMessage("���������� ������. Com2 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void __fastcall SComport::Timer_Com3_Timer(TObject *Sender)
{
    //return;
try
{
        if(com3_err_alarm)
        {
            if(Port.Open(PortName.c_str(),B_Rate,Data8Bit,P_Rate,OneStopBit))
            {
                State = 1;
				BTN_reset->Caption = "���� �����";
                com3_err_alarm = 0;
            }
        }


    Comport[2]->CB_status->Checked = State;
	CB_nal->Checked = Pr_nal;
	LBL_otl->Visible = (!State || Pr_nal);

	// ��������� ��������� � ���� �������
	if(!State||!ust_ready) return;

	// ����������� ������/��������
	RB_prd->Checked = !DevState;
	RB_prm->Checked = DevState;

	// ���� ���� ������ �� �������� ������-�� ���������
    if(!PR_KLASTER)
    {
        if((Comport[2]->DevState==0) &&
            ( *AZ_drive[0]->Pr_AZ ||
            *AZ_drive[1]->Pr_AZ ||
            *AZ_drive[2]->Pr_AZ ))
        {
            if(!(*AZ_drive[0]->Pr_AZ)) Comport[2]->PortTask &= (~0x01);
            if(!(*AZ_drive[1]->Pr_AZ)) Comport[2]->PortTask &= (~0x02);
            if(!(*AZ_drive[2]->Pr_AZ)) Comport[2]->PortTask &= (~0x04);
            if(Comport[2]->PortTask == 0)
            {
                if(*AZ_drive[0]->Pr_AZ) Comport[2]->PortTask |= 0x01;
                if(*AZ_drive[1]->Pr_AZ) Comport[2]->PortTask |= 0x02;
                if(*AZ_drive[2]->Pr_AZ) Comport[2]->PortTask |= 0x04;
            }
        }
        else
        {
            if(!(Comport[2]->PortTask))
                Comport[2]->PortTask |= 0x07;	// 3 ���������
        }
    }
    else
    {
        if(!(Comport[2]->PortTask))
            Comport[2]->PortTask |= 0x08;	// 1 ����������
    }

	if(PortTask & 0x01)
	{
		DevErr = AZ_drive[0]->AZ_manage(DevState);
		if(DevState > 1)
		{
			DevErr ? diagnS[AZ_drive[0]->diagnS_byte] |= AZ_drive[0]->diagnS_mask : diagnS[AZ_drive[0]->diagnS_byte] &= (~AZ_drive[0]->diagnS_mask);
			PortTask &= (~0x01);
			DevState = 0;
		}
		return;
	}

	else if(PortTask & 0x02)
	{
		DevErr = AZ_drive[1]->AZ_manage(DevState);
		if(DevState > 1)
		{
			DevErr ? diagnS[AZ_drive[1]->diagnS_byte] |= AZ_drive[1]->diagnS_mask : diagnS[AZ_drive[1]->diagnS_byte] &= (~AZ_drive[1]->diagnS_mask);
			PortTask &= (~0x02);
			DevState = 0;
		}
		return;
	}

	else if(PortTask & 0x04)
	{
		DevErr = AZ_drive[2]->AZ_manage(DevState);
		if(DevState > 1)
		{
			DevErr ? diagnS[AZ_drive[2]->diagnS_byte] |= AZ_drive[2]->diagnS_mask : diagnS[AZ_drive[2]->diagnS_byte] &= (~AZ_drive[2]->diagnS_mask);
			PortTask &= (~0x04);
			DevState = 0;
		}
		return;
	}
    else if(PortTask & 0x08)
	{
		DevErr = IntMod[0]->IM_manage(DevState);
                DevErr ? diagnS[IntMod[0]->diagnS_byte] |= IntMod[0]->diagnS_mask : diagnS[IntMod[0]->diagnS_byte] &= (~IntMod[0]->diagnS_mask);
		if(DevState > 1)
		{
			DevErr ? diagnS[IntMod[0]->diagnS_byte] |= IntMod[0]->diagnS_mask : diagnS[IntMod[0]->diagnS_byte] &= (~IntMod[0]->diagnS_mask);
			PortTask &= (~0x08);
			DevState = 0;
		}
		return;
	}
    }
catch (Exception &exception)
{
        if(!com3_err_alarm)
        {
                com3_err_alarm = 1;
                if(State)
                {
                        com3_err++;
                        Form1->Lbl_Com3Err->Caption = "������ �����: " + IntToStr(com3_err);
                        Port.Close();
                        BTN_reset->Caption = "���� �����";
                }
                // ShowMessage("���������� ������. Com3 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void __fastcall SComport::Timer_Com4_Timer(TObject *Sender)
{
    //return;
try
{
    Comport[3]->CB_status->Checked = State;
	CB_nal->Checked = Pr_nal;
	LBL_otl->Visible = (!State || Pr_nal);

	// ��������� ��������� � ���� �������
	if(!State||!ust_ready) return;

	// ����������� ������/��������
	RB_prd->Checked = !DevState;
	RB_prm->Checked = DevState;

    if(!PortTask && !Pr_nal)
        PortTask |= 0x01; // ��������� �������������� �������

    if(PortTask & 0x01)
	{
		DevErr = Dat_MTM9D[0]->DatMTM9D_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[Dat_MTM9D[0]->diagnS_byte] |= Dat_MTM9D[0]->diagnS_mask : diagnS[Dat_MTM9D[0]->diagnS_byte] &= (~Dat_MTM9D[0]->diagnS_mask);
			PortTask &= (~0x01);
			DevState = 0;
		}
		return;
	}
    }
catch (Exception &exception)
{
        if(!com4_err_alarm)
        {
                com4_err_alarm = 1;
                if(State)
                {
                        State = 0;
			            Port.Close();
			            BTN_reset->Caption = "���� �����";
                }
                ShowMessage("���������� ������. Com4 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
// ���� �������� ����������
void __fastcall TForm1::BtnOtl1Click(TObject *Sender)
{
	// ��������� �������� ���������� �� ��������
	switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
	{
     //0 ��������
case 0:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: sh_ = StrToInt(EditOTLzad1->Text); break; 
		case 2: shr[1] = StrToInt(EditOTLzad2->Text); break;
		case 3: sh[1] = StrToInt(EditOTLzad3->Text); break; 
		case 4: shr[2] = StrToInt(EditOTLzad4->Text); break; 
		case 5: sh[2] = StrToInt(EditOTLzad5->Text); break; 
		case 6: shr[3] = StrToInt(EditOTLzad6->Text); break; 
		case 7: sh[3] = StrToInt(EditOTLzad7->Text); break; 
		case 8: shr[4] = StrToInt(EditOTLzad8->Text); break; 
		case 9: sh[4] = StrToInt(EditOTLzad9->Text); break; 
		case 10: shr[5] = StrToInt(EditOTLzad10->Text); break; 
		case 11: sh[5] = StrToInt(EditOTLzad11->Text); break; 
		case 12: shr[6] = StrToInt(EditOTLzad12->Text); break; 
		case 13: sh[6] = StrToInt(EditOTLzad13->Text); break; 
		case 14: shr[7] = StrToInt(EditOTLzad14->Text); break; 
		case 15: sh[7] = StrToInt(EditOTLzad15->Text); break; 
		case 16: shr[8] = StrToInt(EditOTLzad16->Text); break; 
		case 17: sh[8] = StrToInt(EditOTLzad17->Text); break; 
		case 18: shr[9] = StrToInt(EditOTLzad18->Text); break; 
		case 19: sh[9] = StrToInt(EditOTLzad19->Text); break; 
		case 20: shr[10] = StrToInt(EditOTLzad20->Text); break; 
		case 21: sh[10] = StrToInt(EditOTLzad21->Text); break; 
		case 22: shr[11] = StrToInt(EditOTLzad22->Text); break; 
		case 23: sh[11] = StrToInt(EditOTLzad23->Text); break; 
		case 24: shr[12] = StrToInt(EditOTLzad24->Text); break; 
		case 25: sh[12] = StrToInt(EditOTLzad25->Text); break; 
		case 26: shr[13] = StrToInt(EditOTLzad26->Text); break; 
		case 27: sh[13] = StrToInt(EditOTLzad27->Text); break; 
		case 28: shr[14] = StrToInt(EditOTLzad28->Text); break; 
		case 29: sh[14] = StrToInt(EditOTLzad29->Text); break; 
		case 30: shr[15] = StrToInt(EditOTLzad30->Text); break; 
	}
}; break;
//1 ��������
case 1:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: sh[15] = StrToInt(EditOTLzad1->Text); break; 
		case 2: shr[17] = StrToInt(EditOTLzad2->Text); break; 
		case 3: sh[17] = StrToInt(EditOTLzad3->Text); break; 
		case 4: shr[18] = StrToInt(EditOTLzad4->Text); break; 
		case 5: sh[18] = StrToInt(EditOTLzad5->Text); break; 
		case 6: shr[19] = StrToInt(EditOTLzad6->Text); break; 
		case 7: sh[19] = StrToInt(EditOTLzad7->Text); break; 
		case 8: shr[20] = StrToInt(EditOTLzad8->Text); break; 
		case 9: sh[20] = StrToInt(EditOTLzad9->Text); break; 
		case 10: shr[21] = StrToInt(EditOTLzad10->Text); break; 
		case 11: sh[21] = StrToInt(EditOTLzad11->Text); break; 
		case 12: shr[22] = StrToInt(EditOTLzad12->Text); break; 
		case 13: sh[22] = StrToInt(EditOTLzad13->Text); break; 
		case 14: shr[23] = StrToInt(EditOTLzad14->Text); break; 
		case 15: sh[23] = StrToInt(EditOTLzad15->Text); break; 
		case 16: shr[24] = StrToInt(EditOTLzad16->Text); break; 
		case 17: sh[24] = StrToInt(EditOTLzad17->Text); break; 
		case 18: shr[25] = StrToInt(EditOTLzad18->Text); break; 
		case 19: sh[25] = StrToInt(EditOTLzad19->Text); break; 
		case 20: shr[26] = StrToInt(EditOTLzad20->Text); break; 
		case 21: sh[26] = StrToInt(EditOTLzad21->Text); break; 
		case 22: shr[27] = StrToInt(EditOTLzad22->Text); break;
		case 23: sh[27] = StrToInt(EditOTLzad23->Text); break;
        case 24: shr[28] = StrToInt(EditOTLzad24->Text); break;
		case 25: sh[28] = StrToInt(EditOTLzad25->Text); break;
		case 26: shr[29] = StrToInt(EditOTLzad26->Text); break;
		case 27: sh[29] = StrToInt(EditOTLzad27->Text); break;
		case 28: shr[30] = StrToInt(EditOTLzad28->Text); break;
		case 29: sh[30] = StrToInt(EditOTLzad29->Text); break;
		case 30: shr[31] = StrToInt(EditOTLzad30->Text); break;
		
	}
}; break;
//2 ��������
case 2:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
        case 1: sh[31] = StrToInt(EditOTLzad1->Text); break;
		case 2: shr[32] = StrToInt(EditOTLzad2->Text); break;
		case 3: sh[32] = StrToInt(EditOTLzad3->Text); break;
		case 4: shr[33] = StrToInt(EditOTLzad4->Text); break;
		case 5: sh[33] = StrToInt(EditOTLzad5->Text); break;
		case 6: shr[34] = StrToInt(EditOTLzad6->Text); break;
		case 7: sh[34] = StrToInt(EditOTLzad7->Text); break;
		case 8: shr[35] = StrToInt(EditOTLzad8->Text); break;
		case 9: sh[35] = StrToInt(EditOTLzad9->Text); break;
		case 10: shr[36] = StrToInt(EditOTLzad10->Text); break;
		case 11: sh[36] = StrToInt(EditOTLzad11->Text); break;
		case 12: shr[37] = StrToInt(EditOTLzad12->Text); break;
		case 13: sh[37] = StrToInt(EditOTLzad13->Text); break;
		case 14: shr[38] = StrToInt(EditOTLzad14->Text); break;
		case 15: sh[38] = StrToInt(EditOTLzad15->Text); break;
		case 16: shr[39] = StrToInt(EditOTLzad16->Text); break;
		case 17: sh[39] = StrToInt(EditOTLzad17->Text); break;
		case 18: shr[40] = StrToInt(EditOTLzad18->Text); break;
		case 19: sh[40] = StrToInt(EditOTLzad19->Text); break;
		case 20: zshr3 = StrToInt(EditOTLzad20->Text); break;
		case 21: norma = StrToInt(EditOTLzad21->Text); break;
		case 22: qkk = StrToInt(EditOTLzad22->Text); break;
		case 23: diagn[0] = StrToInt(EditOTLzad23->Text); break;
		case 24: diagn[1] = StrToInt(EditOTLzad24->Text); break;
		case 25: diagn[2] = StrToInt(EditOTLzad25->Text); break;
		case 26: diagn[3] = StrToInt(EditOTLzad26->Text); break;
		case 27: diagn[4] = StrToInt(EditOTLzad27->Text); break;
		case 28: diagn[5] = StrToInt(EditOTLzad28->Text); break;
		case 29: diagn[6] = StrToInt(EditOTLzad29->Text); break;
		case 30: diagn[7] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//3 ��������
case 3:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
        case 1: diagn[8] = StrToInt(EditOTLzad1->Text); break;
		case 2: diagn[9] = StrToInt(EditOTLzad2->Text); break;
		case 3: diagn[10] = StrToInt(EditOTLzad3->Text); break;
		case 4: diagn[11] = StrToInt(EditOTLzad4->Text); break;
		case 5: diagn[12] = StrToInt(EditOTLzad5->Text); break;
		case 6: diagn[13] = StrToInt(EditOTLzad6->Text); break;
		case 7: diagn[14] = StrToInt(EditOTLzad7->Text); break;
		case 8: diagn[15] = StrToInt(EditOTLzad8->Text); break;
		case 9: diagn[16] = StrToInt(EditOTLzad9->Text); break;
		case 10: diagn[17] = StrToInt(EditOTLzad10->Text); break;
		case 11: diagn[18] = StrToInt(EditOTLzad11->Text); break;
		case 12: diagn[19] = StrToInt(EditOTLzad12->Text); break;
		case 13: diagn[20] = StrToInt(EditOTLzad13->Text); break;
		case 14: diagn[21] = StrToInt(EditOTLzad14->Text); break;
		case 15: diagn[22] = StrToInt(EditOTLzad15->Text); break;
		case 16: diagn[23] = StrToInt(EditOTLzad16->Text); break;
		case 17: diagn[24] = StrToInt(EditOTLzad17->Text); break;
		case 18: diagn[25] = StrToInt(EditOTLzad18->Text); break;
		case 19: diagn[26] = StrToInt(EditOTLzad19->Text); break;
		case 20: diagn[27] = StrToInt(EditOTLzad20->Text); break;
		case 21: diagn[28] = StrToInt(EditOTLzad21->Text); break;
		case 22: diagnS[0] = StrToInt(EditOTLzad22->Text); break;
		case 23: diagnS[1] = StrToInt(EditOTLzad23->Text); break;
		case 24: diagnS[2] = StrToInt(EditOTLzad24->Text); break;
		case 25: out[0] = StrToInt(EditOTLzad25->Text); break;
		case 26: out[1] = StrToInt(EditOTLzad26->Text); break;
		case 27: out[2] = StrToInt(EditOTLzad27->Text); break;
		case 28: out[3] = StrToInt(EditOTLzad28->Text); break;
		case 29: zin[0] = StrToInt(EditOTLzad29->Text); break;
		case 30: zin[1] = StrToInt(EditOTLzad30->Text); break; 
	}
}; break;
//4 ��������
case 4:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: zin[2] = StrToInt(EditOTLzad1->Text); break; 
		case 2: zin[3] = StrToInt(EditOTLzad2->Text); break; 
		case 3: zin[4] = StrToInt(EditOTLzad3->Text); break; 
		case 4: aik[0] = StrToInt(EditOTLzad4->Text); break; 
		case 5: aik[1] = StrToInt(EditOTLzad5->Text); break; 
		case 6: aik[2] = StrToInt(EditOTLzad6->Text); break; 
		case 7: aik[3] = StrToInt(EditOTLzad7->Text); break; 
		case 8: aik[4] = StrToInt(EditOTLzad8->Text); break;
		case 9: aik[5] = StrToInt(EditOTLzad9->Text); break; 
		case 10: aik[6] = StrToInt(EditOTLzad10->Text); break; 
		case 11: aik[7] = StrToInt(EditOTLzad11->Text); break; 
		case 12: aik[8] = StrToInt(EditOTLzad12->Text); break; 
		case 13: aik[9] = StrToInt(EditOTLzad13->Text); break; 
		case 14: aik[10] = StrToInt(EditOTLzad14->Text); break; 
		case 15: aik[11] = StrToInt(EditOTLzad15->Text); break; 
		case 16: aik[12] = StrToInt(EditOTLzad16->Text); break; 
		case 17: aik[13] = StrToInt(EditOTLzad17->Text); break; 
		case 18: aik[14] = StrToInt(EditOTLzad18->Text); break; 
		case 19: aik[15] = StrToInt(EditOTLzad19->Text); break; 
		case 20: aik[16] = StrToInt(EditOTLzad20->Text); break;
		case 21: aik[17] = StrToInt(EditOTLzad21->Text); break;
        case 22: aik[18] = StrToInt(EditOTLzad22->Text); break;
		case 23: aout[0] = StrToInt(EditOTLzad23->Text); break;
		case 24: aout[1] = StrToInt(EditOTLzad24->Text); break;
		case 25: aout[2] = StrToInt(EditOTLzad25->Text); break;
		case 26: aout[3] = StrToInt(EditOTLzad26->Text); break;
		case 27: aout[4] = StrToInt(EditOTLzad27->Text); break;
		case 28: aout[5] = StrToInt(EditOTLzad28->Text); break;
		case 29: aout[6] = StrToInt(EditOTLzad29->Text); break;
		case 30: aout[7] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//5 ��������
case 5:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
        case 1: aout[8] = StrToInt(EditOTLzad1->Text); break;
		case 2: aout[9] = StrToInt(EditOTLzad2->Text); break;
		case 3: aout[10] = StrToInt(EditOTLzad3->Text); break;
		case 4: D_D1 = StrToInt(EditOTLzad4->Text); break;
		case 5: D_D2 = StrToInt(EditOTLzad5->Text); break;
		case 6: D_D3 = StrToInt(EditOTLzad6->Text); break;
		case 7: D_D4 = StrToInt(EditOTLzad7->Text); break;
		case 8: D_D5 = StrToInt(EditOTLzad8->Text); break;
		//case 8: UVAK_OTK_D1 = StrToInt(EditOTLzad8->Text); break;
		//case 9: UVAK_OTK_D2 = StrToInt(EditOTLzad9->Text); break;
		//case 10: UVAK_OTK_D3 = StrToInt(EditOTLzad10->Text); break;
		//case 11: UATM = StrToInt(EditOTLzad11->Text); break;
		//case 12: UVAK_TMN_V = StrToInt(EditOTLzad12->Text); break;
		//case 13: UVAK_TMN_N = StrToInt(EditOTLzad13->Text); break; 
		//case 14: UVAK_ATM = StrToInt(EditOTLzad14->Text); break;
		//case 15: UVAK_KAM = StrToInt(EditOTLzad15->Text); break;
		case 16: POROG_DAVL = StrToInt(EditOTLzad16->Text); break;
		case 17: UVAKV_KAM = StrToInt(EditOTLzad17->Text); break;
		case 18: UVAKN_KAM = StrToInt(EditOTLzad18->Text); break; 
		case 19: UVAKN_TMN = StrToInt(EditOTLzad19->Text); break; 
		case 20: UVAKV_TMN = StrToInt(EditOTLzad20->Text); break;
		//case 21: UVAK_SHL = StrToInt(""); break;
		case 22: UVAK_SHL_MO = StrToInt(EditOTLzad22->Text); break;
		case 23: UVAK_SHL_MN = StrToInt(EditOTLzad23->Text); break; 
		case 24: UVAK_ZTMN = StrToInt(EditOTLzad24->Text); break;
		case 25: UATM_D1 = StrToInt(EditOTLzad25->Text); break; 
		case 26: UATM_D4 = StrToInt(EditOTLzad26->Text); break; 
		//case 27: UVAK_BUST = StrToInt(EditOTLzad27->Text); break;
		case 28: nasmod[0] = StrToInt(EditOTLzad28->Text); break; 
		case 29: nasmod[1] = StrToInt(EditOTLzad29->Text); break; 
		case 30: nasmod[2] = StrToInt(EditOTLzad30->Text); break; 
	}
}; break;
//6 ��������
case 6:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: nasmod[3] = StrToInt(EditOTLzad1->Text); break; 
		case 2: nasmod[4] = StrToInt(EditOTLzad2->Text); break; 
		case 3: nasmod[5] = StrToInt(EditOTLzad3->Text); break; 
		case 4: nasmod[6] = StrToInt(EditOTLzad4->Text); break; 
		case 5: nasmod[7] = StrToInt(EditOTLzad5->Text); break; 
		case 6: nasmod[8] = StrToInt(EditOTLzad6->Text); break; 
		case 7: nasmod[9] = StrToInt(EditOTLzad7->Text); break; 
		case 8: nasmod[10] = StrToInt(EditOTLzad8->Text); break; 
		case 9: nasmod[11] = StrToInt(EditOTLzad9->Text); break; 
		case 10: nasmod[12] = StrToInt(EditOTLzad10->Text); break; 
		case 11: nasmod[13] = StrToInt(EditOTLzad11->Text); break; 
		case 12: nasmod[14] = StrToInt(EditOTLzad12->Text); break; 
		case 13: nasmod[15] = StrToInt(EditOTLzad13->Text); break; 
		case 14: nasmod[16] = StrToInt(EditOTLzad14->Text); break; 
		case 15: nasmod[17] = StrToInt(EditOTLzad15->Text); break; 
		//case 16: nasmod[18] = StrToInt(EditOTLzad16->Text); break;
		case 17: par_t[0] = StrToInt(EditOTLzad17->Text); break; 
		case 18: par_t[1] = StrToInt(EditOTLzad18->Text); break; 
		case 19: par_t[2] = StrToInt(EditOTLzad19->Text); break; 
		case 20: par_t[3] = StrToInt(EditOTLzad20->Text); break; 
		case 21: par_t[4] = StrToInt(EditOTLzad21->Text); break; 
		case 22: par_t[5] = StrToInt(EditOTLzad22->Text); break; 
		case 23: par[0][0] = StrToInt(EditOTLzad23->Text); break; 
		case 24: par[0][1] = StrToInt(EditOTLzad24->Text); break; 
		case 25: par[0][2] = StrToInt(EditOTLzad25->Text); break; 
		case 26: par[0][3] = StrToInt(EditOTLzad26->Text); break; 
		case 27: par[0][4] = StrToInt(EditOTLzad27->Text); break; 
		case 28: par[0][5] = StrToInt(EditOTLzad28->Text); break; 
		case 29: par[0][6] = StrToInt(EditOTLzad29->Text); break; 
		case 30: par[0][7] = StrToInt(EditOTLzad30->Text); break; 
	}
}; break;
//7 ��������
case 7:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: par[0][8] = StrToInt(EditOTLzad1->Text); break; 
		case 2: par[0][9] = StrToInt(EditOTLzad2->Text); break; 
		case 3: par[0][10] = StrToInt(EditOTLzad3->Text); break; 
		case 4: par[0][11] = StrToInt(EditOTLzad4->Text); break; 
		case 5: par[0][12] = StrToInt(EditOTLzad5->Text); break; 
		case 6: par[0][13] = StrToInt(EditOTLzad6->Text); break; 
		case 7: par[0][14] = StrToInt(EditOTLzad7->Text); break; 
		case 8: par[0][15] = StrToInt(EditOTLzad8->Text); break; 
		case 9: par[0][16] = StrToInt(EditOTLzad9->Text); break; 
		case 10: par[0][17] = StrToInt(EditOTLzad10->Text); break; 
		case 11: par[0][18] = StrToInt(EditOTLzad11->Text); break;
        case 12: par[0][20] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[1][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[1][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[1][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[1][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[1][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[1][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[1][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[1][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[1][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[1][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[1][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[1][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[1][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[1][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[1][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[2][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[2][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[2][2] = StrToInt(EditOTLzad30->Text); break;
		
	}
}; break;
//8 ��������
case 8:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[2][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[2][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[2][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[2][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[2][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[2][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[2][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[2][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[2][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[2][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[2][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[2][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[3][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[3][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[3][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[3][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[3][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[3][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[3][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[3][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[3][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[3][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[3][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[3][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[3][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[3][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[3][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[4][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[4][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[4][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//9 ��������
case 9:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[4][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[4][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[4][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[4][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[4][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[4][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[4][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[4][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[4][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[4][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[4][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[4][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[5][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[5][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[5][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[5][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[5][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[5][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[5][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[5][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[5][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[5][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[5][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[5][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[5][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[5][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[5][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[6][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[6][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[6][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//10 ��������
case 10:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[6][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[6][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[6][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[6][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[6][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[6][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[6][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[6][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[6][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[6][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[6][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[6][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[7][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[7][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[7][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[7][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[7][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[7][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[7][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[7][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[7][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[7][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[7][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[7][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[7][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[7][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[7][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[8][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[8][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[8][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//11 ��������
case 11:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[8][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[8][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[8][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[8][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[8][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[8][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[8][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[8][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[8][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[8][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[8][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[8][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[9][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[9][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[9][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[9][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[9][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[9][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[9][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[9][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[9][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[9][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[9][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[9][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[9][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[9][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[9][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[10][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[10][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[10][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//12 ��������
case 12:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[10][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[10][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[10][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[10][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[10][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[10][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[10][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[10][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[10][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[10][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[10][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[10][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[11][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[11][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[11][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[11][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[11][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[11][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[11][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[11][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[11][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[11][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[11][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[11][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[11][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[11][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[11][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[12][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[12][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[12][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//13 ��������
case 13:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[12][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[12][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[12][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[12][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[12][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[12][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[12][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[12][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[12][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[12][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[12][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[12][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[13][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[13][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[13][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[13][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[13][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[13][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[13][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[13][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[13][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[13][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[13][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[13][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[13][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[13][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[13][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[14][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[14][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[14][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//14 ��������
case 14:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[14][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[14][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[14][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[14][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[14][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[14][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[14][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[14][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[14][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[14][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[14][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[14][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[15][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[15][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[15][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[15][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[15][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[15][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[15][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[15][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[15][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[15][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[15][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[15][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[15][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[15][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[15][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[16][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[16][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[16][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//15 ��������
case 15:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[16][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[16][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[16][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[16][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[16][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[16][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[16][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[16][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[16][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[16][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[16][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[16][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[17][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[17][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[17][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[17][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[17][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[17][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[17][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[17][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[17][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[17][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[17][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[17][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[17][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[17][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[17][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[18][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[18][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[18][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//16 ��������
case 16:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[18][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[18][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[18][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[18][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[18][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[18][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[18][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[18][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[18][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[18][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[18][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[18][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[19][0] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[19][1] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[19][2] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[19][3] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[19][4] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[19][5] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[19][6] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[19][7] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[19][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[19][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[19][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[19][11] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[19][12] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[19][13] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[19][14] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[20][0] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[20][1] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[20][2] = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//17 ��������
case 17:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: par[20][3] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[20][4] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[20][5] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[20][6] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[20][7] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[20][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[20][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[20][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[20][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[20][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[20][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[20][14] = StrToInt(EditOTLzad12->Text); break;
		case 13: T_VHG = StrToInt(EditOTLzad13->Text); break;
		case 14: T_ZAD_DVS = StrToInt(EditOTLzad14->Text); break;
		case 15: T_PROC = StrToInt(EditOTLzad15->Text); break;
		case 16: T_KTMN_RAZGON = StrToInt(EditOTLzad16->Text); break;
		case 17: T_VKL_BPN = StrToInt(EditOTLzad17->Text); break;
		case 18: T_KKAM_V = StrToInt(EditOTLzad18->Text); break;
		case 19: T_VODA = StrToInt(EditOTLzad19->Text); break;
		case 20: T_STOP = StrToInt(EditOTLzad20->Text); break;
		case 21: T_DVIJ = StrToInt(EditOTLzad21->Text); break;
		case 22: T_KDVIJ_SU = StrToInt(EditOTLzad22->Text); break;
		case 23: T_KSUT = StrToInt(EditOTLzad23->Text); break;
		case 24: T_KKAM = StrToInt(EditOTLzad24->Text); break;
		case 25: T_KTMN = StrToInt(EditOTLzad25->Text); break;
		case 26: T_KPRIJ = StrToInt(EditOTLzad26->Text); break;
		case 27: T_KPRST = StrToInt(EditOTLzad27->Text); break;
		case 28: T_KPR = StrToInt(EditOTLzad28->Text); break;
		case 29: T_KSHL = StrToInt(EditOTLzad29->Text); break;
		case 30: T_KNAP = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//18 ��������
case 18:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: T_NAPUSK = StrToInt(EditOTLzad1->Text); break;
		case 2: T_SBROSHE = StrToInt(EditOTLzad2->Text); break;
		case 3: T_KSHL_MO = StrToInt(EditOTLzad3->Text); break;
		case 4: T_TMN = StrToInt(EditOTLzad4->Text); break;
		case 5: CT_VHG = StrToInt(EditOTLzad5->Text); break;
		case 6: CT_VODA_STOL = StrToInt(EditOTLzad6->Text); break;
		case 7: CT_VODA_IP = StrToInt(EditOTLzad7->Text); break;
		case 8: CT_PER = StrToInt(EditOTLzad8->Text); break;
		case 9: CT_POV = StrToInt(EditOTLzad9->Text); break;
		case 10: CT_PRIJ = StrToInt(EditOTLzad10->Text); break;
		case 11: CT_KAS = StrToInt(EditOTLzad11->Text); break;
		case 12: CT_TEMP1 = StrToInt(EditOTLzad12->Text); break;
		case 13: CT_TEMP2 = StrToInt(EditOTLzad13->Text); break;
		//case 14: CT_MP = StrToInt(EditOTLzad14->Text); break;
		case 15: CT_DVIJ_GIR_g = StrToInt(EditOTLzad15->Text); break;
		case 16: CT_DVIJ_GIR_t = StrToInt(EditOTLzad16->Text); break;
		case 17: CT_SUT_g = StrToInt(EditOTLzad17->Text); break;
		case 18: CT_SUT_t = StrToInt(EditOTLzad18->Text); break;
		case 19: CT_T1 = StrToInt(EditOTLzad19->Text); break;
		case 20: CT_T20 = StrToInt(EditOTLzad20->Text); break;
		case 21: CT_1 = StrToInt(EditOTLzad21->Text); break;
		case 22: CT_2 = StrToInt(EditOTLzad22->Text); break;
		case 23: CT_3 = StrToInt(EditOTLzad23->Text); break;
		case 24: CT_4 = StrToInt(EditOTLzad24->Text); break;
		case 25: CT_5 = StrToInt(EditOTLzad25->Text); break;
		case 26: CT_6 = StrToInt(EditOTLzad26->Text); break;
		case 27: CT_7 = StrToInt(EditOTLzad27->Text); break;
		case 28: CT_9 = StrToInt(EditOTLzad28->Text); break;
		case 29: CT_17 = StrToInt(EditOTLzad29->Text); break;
		case 30: CT17K1 = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//19 ��������
case 19:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: CT_27 = StrToInt(EditOTLzad1->Text); break;
		case 2: CT27K1 = StrToInt(EditOTLzad2->Text); break;
		case 3: CT_28 = StrToInt(EditOTLzad3->Text); break;
		case 4: CT28K1 = StrToInt(EditOTLzad4->Text); break;
		case 5: CT_29 = StrToInt(EditOTLzad5->Text); break;
		case 6: CT29K1 = StrToInt(EditOTLzad6->Text); break;
		case 7: CT_30T = StrToInt(EditOTLzad7->Text); break;
		case 8: CT_33 = StrToInt(EditOTLzad8->Text); break;
		case 9: CT33K1 = StrToInt(EditOTLzad9->Text); break;
		case 10: CT_35 = StrToInt(EditOTLzad10->Text); break;
		case 11: CT35K1 = StrToInt(EditOTLzad11->Text); break;
		case 12: PR_TRTEST = StrToInt(EditOTLzad12->Text); break;
		case 13: PR_OTK = StrToInt(EditOTLzad13->Text); break;
		case 14: PR_FK_KAM = StrToInt(EditOTLzad14->Text); break;
		case 15: PR_NASOS = StrToInt(EditOTLzad15->Text); break;
		case 16: PR_NALADKA = StrToInt(EditOTLzad16->Text); break;
		case 17: PR_NPL = StrToInt(EditOTLzad17->Text); break;
		case 18: PR_PRIJ = StrToInt(EditOTLzad18->Text); break;
		case 19: otvet = StrToInt(EditOTLzad19->Text); break;
		case 20: N_PL = StrToInt(EditOTLzad20->Text); break;
		case 21: N_ST_MAX = StrToInt(EditOTLzad21->Text); break;
		case 22: N_ST = StrToInt(EditOTLzad22->Text); break;
		case 23: PR_HEL = StrToInt(EditOTLzad23->Text); break;
		case 24: DATA_DZASL = StrToInt(EditOTLzad24->Text); break;
		case 25: PAR_DZASL = StrToInt(EditOTLzad25->Text); break;
		case 26: ZPAR_DZASL = StrToInt(EditOTLzad26->Text); break;
		case 27: X_TDZASL = StrToInt(EditOTLzad27->Text); break;
		case 28: VRDZASL = StrToInt(EditOTLzad28->Text); break;
		case 29: E_TDZASL = StrToInt(EditOTLzad29->Text); break;
		case 30: DELDZASL = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//20 ��������
case 20:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: LIM1DZASL = StrToInt(EditOTLzad1->Text); break;
		case 2: LIM2DZASL = StrToInt(EditOTLzad2->Text); break;
		case 3: T_VRDZASL = StrToInt(EditOTLzad3->Text); break;
		case 4: T_KDZASL = StrToInt(EditOTLzad4->Text); break;
		case 5: DOPDZASL = StrToInt(EditOTLzad5->Text); break;
		case 6: KOM_DZASL = StrToInt(EditOTLzad6->Text); break;
		case 7: TEK_DAVL_DZASL = StrToInt(EditOTLzad7->Text); break;
		case 8: TEK_POZ_DZASL = StrToInt(EditOTLzad8->Text); break;
		case 9: PR_DZASL = StrToInt(EditOTLzad9->Text); break;
		case 10: CT_DZASL = StrToInt(EditOTLzad10->Text); break;
		case 11: OTVET_DZASL = StrToInt(EditOTLzad11->Text); break;
		case 12: DAVL_DZASL = StrToInt(EditOTLzad12->Text); break;
		case 13: VRGIS = StrToInt(EditOTLzad13->Text); break;
		case 14: K_SOGL_GIS = StrToInt(EditOTLzad14->Text); break;
		case 15: NAPRS_GIS = StrToInt(EditOTLzad15->Text); break;
		case 16: X_TGIS = StrToInt(EditOTLzad16->Text); break;
		case 17: E_TGIS = StrToInt(EditOTLzad17->Text); break;
		case 18: DELGIS = StrToInt(EditOTLzad18->Text); break;
		case 19: DOPGIS = StrToInt(EditOTLzad19->Text); break;
		case 20: PAR_GIS = StrToInt(EditOTLzad20->Text); break;
		case 21: N_TEK_GIS = StrToInt(EditOTLzad21->Text); break;
		case 22: LIM1GIS = StrToInt(EditOTLzad22->Text); break;
		case 23: LIM2GIS = StrToInt(EditOTLzad23->Text); break;
		case 24: T_VRGIS = StrToInt(EditOTLzad24->Text); break;
		case 25: T_KGIS = StrToInt(EditOTLzad25->Text); break;
		case 26: prDvijGir_g = StrToInt(EditOTLzad26->Text); break;
		case 27: prDvijGir_t = StrToInt(EditOTLzad27->Text); break;
		case 28: DOP_SU = StrToInt(EditOTLzad28->Text); break;
		case 29: T_SM_NAPR = StrToInt(EditOTLzad29->Text); break;
		case 30: DOP_DV_IP = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//21 ��������
case 21:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: X_TGIS_SM = StrToInt(EditOTLzad1->Text); break;
		case 2: E_TGIS_SM = StrToInt(EditOTLzad2->Text); break;
		case 3: DELGIS_SM = StrToInt(EditOTLzad3->Text); break;
		case 4: DOPGIS_SM = StrToInt(EditOTLzad4->Text); break;
		case 5: PAR_GIS_SM = StrToInt(EditOTLzad5->Text); break;
		case 6: N_TEK_GIS_SM = StrToInt(EditOTLzad6->Text); break;
		case 7: LIM1GIS_SM = StrToInt(EditOTLzad7->Text); break;
		case 8: LIM2GIS_SM = StrToInt(EditOTLzad8->Text); break;
		case 9: T_VRGIS_SM = StrToInt(EditOTLzad9->Text); break;
		case 10: T_KGIS_SM = StrToInt(EditOTLzad10->Text); break;
		case 11: VRGIR = StrToInt(EditOTLzad11->Text); break;
		case 12: K_SOGL_GIR = StrToInt(EditOTLzad12->Text); break;
		case 13: NAPRS_GIR = StrToInt(EditOTLzad13->Text); break;
		case 14: X_TGIR = StrToInt(EditOTLzad14->Text); break;
		case 15: E_TGIR = StrToInt(EditOTLzad15->Text); break;
		case 16: DOPGIR = StrToInt(EditOTLzad16->Text); break;
		case 17: PAR_GIR = StrToInt(EditOTLzad17->Text); break;
		case 18: T_VRGIR = StrToInt(EditOTLzad18->Text); break;
		case 19: T_KGIR = StrToInt(EditOTLzad19->Text); break;
		case 20: N_TEK_GIR = StrToInt(EditOTLzad20->Text); break;
	case 21: KOM_PER = StrToInt(EditOTLzad21->Text); break;
		case 22: OTVET_PER = StrToInt(EditOTLzad22->Text); break;
		case 23: V_PER = StrToInt(EditOTLzad23->Text); break;
		case 24: TYPE_PER = StrToInt(EditOTLzad24->Text); break;
		case 25: PR_PER = StrToInt(EditOTLzad25->Text); break;
		case 26: HOME_PER = StrToInt(EditOTLzad26->Text); break;
		case 27: PUT_PER = StrToInt(EditOTLzad27->Text); break;
		case 28: TEK_ABS_PER = StrToInt(EditOTLzad28->Text); break;
		case 29: TEK_OTN_PER = StrToInt(EditOTLzad29->Text); break;
		case 30: KOM_POV = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//22 ��������
case 22:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: OTVET_POV = StrToInt(EditOTLzad1->Text); break;
		case 2: V_POV = StrToInt(EditOTLzad2->Text); break;
		case 3: TYPE_POV = StrToInt(EditOTLzad3->Text); break;
		case 4: PR_POV = StrToInt(EditOTLzad4->Text); break;
		case 5: HOME_POV = StrToInt(EditOTLzad5->Text); break;
		case 6: PUT_POV = StrToInt(EditOTLzad6->Text); break;
		case 7: TEK_ABS_POV = StrToInt(EditOTLzad7->Text); break;
		case 8: TEK_OTN_POV = StrToInt(EditOTLzad8->Text); break;
		case 9: KOM_KAS = StrToInt(EditOTLzad9->Text); break;
		case 10: OTVET_KAS = StrToInt(EditOTLzad10->Text); break;
		case 11: V_KAS = StrToInt(EditOTLzad11->Text); break;
		case 12: TYPE_KAS = StrToInt(EditOTLzad12->Text); break;
		case 13: PR_KAS = StrToInt(EditOTLzad13->Text); break;
		case 14: HOME_KAS = StrToInt(EditOTLzad14->Text); break;
		case 15: PUT_KAS = StrToInt(EditOTLzad15->Text); break;
		case 16: TEK_ABS_KAS = StrToInt(EditOTLzad16->Text); break;
		case 17: TEK_OTN_KAS = StrToInt(EditOTLzad17->Text); break;
		case 18: PR_TEMP = StrToInt(EditOTLzad18->Text); break;
		case 19: KOM_TEMP = StrToInt(EditOTLzad19->Text); break;
		case 20: ZAD_TEMP1 = StrToInt(EditOTLzad20->Text); break;
		case 21: PAR_TEMP1 = StrToInt(EditOTLzad21->Text); break;
		case 22: ZPAR_TEMP1 = StrToInt(EditOTLzad22->Text); break;
		case 23: X_TEMP1 = StrToInt(EditOTLzad23->Text); break;
		case 24: VRTEMP1 = StrToInt(EditOTLzad24->Text); break;
		case 25: E_TEMP1 = StrToInt(EditOTLzad25->Text); break;
		case 26: DELTEMP1 = StrToInt(EditOTLzad26->Text); break;
		case 27: LIM1TEMP1 = StrToInt(EditOTLzad27->Text); break;
		case 28: LIM2TEMP1 = StrToInt(EditOTLzad28->Text); break;
		case 29: DOPTEMP1 = StrToInt(EditOTLzad29->Text); break;
		case 30: TEK_TEMP1 = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
//23 ��������
case 23:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{   case 1: T_VRTEMP = StrToInt(EditOTLzad1->Text); break;
		case 2: T_KTEMP = StrToInt(EditOTLzad2->Text); break;
		case 3: PR_TEMP = StrToInt(EditOTLzad3->Text); break;
		case 4: KOM_TEMP = StrToInt(EditOTLzad4->Text); break;
		case 5: ZAD_TEMP2 = StrToInt(EditOTLzad5->Text); break;
		case 6: PAR_TEMP2 = StrToInt(EditOTLzad6->Text); break;
		case 7: ZPAR_TEMP2 = StrToInt(EditOTLzad7->Text); break;
		case 8: X_TEMP2 = StrToInt(EditOTLzad8->Text); break;
		case 9: VRTEMP2 = StrToInt(EditOTLzad9->Text); break;
		case 10: E_TEMP2 = StrToInt(EditOTLzad10->Text); break;
		case 11: DELTEMP2 = StrToInt(EditOTLzad11->Text); break;
		case 12: LIM1TEMP2 = StrToInt(EditOTLzad12->Text); break;
		case 13: LIM2TEMP2 = StrToInt(EditOTLzad13->Text); break;
		case 14: DOPTEMP2 = StrToInt(EditOTLzad14->Text); break;
		case 15: TEK_TEMP2 = StrToInt(EditOTLzad15->Text); break;
        case 16: KOM_MOD = StrToInt(EditOTLzad16->Text); break;
		case 17: OTVET_MOD = StrToInt(EditOTLzad17->Text); break;
        case 18: PR_KLASTER = StrToInt(EditOTLzad18->Text); break;
	}
}; break;

    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------- ��������� �������� ����� ------------------------------------------
void __fastcall TForm1::Gas_Name0Click(TObject *Sender)
{
	int i=0;
	AnsiString temp_str = "";
  
	if(PCMain -> ActivePage == TSNalad) // ��������� ����� ������ � ������� ���� �����
    {
      Pnl_GK -> Visible = true;
      GK_n = StrToInt(((TLabel*)Sender)->Hint);
      //////////////////////////////////////////////////////////////////////////////////////////////////
      Pnl_GK -> Top = ((TLabel*)Sender) -> Top-10;
      Pnl_GK -> Left = ((TLabel*)Sender) -> Left + 20;
      //////////////////////////////////////////////////////////////////////////////////////////////////
      for(i=0;GasNames.Gas_names[GK_n][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[GK_n][i]; }
      //////////////////////////////////////////////////////////////////////////////////////////////////
      Edit_GK -> Text = temp_str; Pnl_GK -> Visible = true;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::BtnGKOnClick(TObject *Sender)
{
	int i=0;
	AnsiString temp_str;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ������������� ��� ������ �����
	if((bool)StrToInt(((TButton*)Sender)->Hint))
	{ temp_str = Edit_GK->Text;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(i=0;i<8;i++) { GasNames.Gas_names[GK_n][i] = 0; }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(i=1;i<=temp_str.Length();i++) { GasNames.Gas_names[GK_n][i-1] = temp_str[i]; }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    RenameGases();
	}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Pnl_GK -> Visible = false;
}
//----------------------------------------------------------------------------------------------
void TForm1::LoadGasData()
{
	int SizeOfIniFile=(int)sizeof(GasNames);

	if(!DirectoryExists("Data")) { CreateDir("Data"); }
	FILE *im0;
	im0=fopen(loc_udb,"rb");
	if(im0)       { fread(&GasNames,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� ��������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//----------------------------------------------------------------------------------------------
void TForm1::SaveGasData()
{
	int SizeOfIniFile=(int)sizeof(GasNames);

	if(!DirectoryExists("Data")) { CreateDir("Data"); }
	FILE *im0;
	im0=fopen(loc_udb,"wb");
	if(im0)       { fwrite(&GasNames,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TForm1::RenameGases()   // �������� �������� �����
{ int i=0;
  AnsiString temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[0][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[0][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name0 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[1][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[1][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name1 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[2][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[2][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name2 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[3][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[3][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name3 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[4][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[4][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name4 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[5][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[5][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name5 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[6][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[6][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name6 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[7][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[7][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name7 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i=0;GasNames.Gas_names[8][i]!=0;i++) { temp_str = temp_str + GasNames.Gas_names[8][i]; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Form1->Gas_Name8 -> Caption = temp_str; temp_str = "";
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_500Timer(TObject *Sender)
{
        if(!ust_ready) return;
        // ������������ ����-�������
        Label_Time -> Caption = TimeToStr(Time());
        Label_Date -> Caption = DateToStr(Date());
        // ����������� ����������
        VisualMnemo();

        // ����� ������� ������
        Form1 -> EditTLogic -> Text = FloatToStrF(logic_time,ffFixed,6,3);

        // ���� ����� ������ 10:00 ������� ������� 0 ��� �������
        if ( Label_Time -> Caption.Length() == 7 )
        {
            Label_Time -> Caption = "0" + Label_Time -> Caption;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1secTimer(TObject *Sender)
{
    VisualGraph();                          //  ����������� ��������
    Save_Stat_24H();
}
//���������� � ����� ����� ���� ����������
void TForm1::Save_Stat_24H()
{
    //������ �������� �������
    AnsiString date_now;
    unsigned short date_hour, date_minutes, date_seconds, date_miliseconds;
    date_now=Now().TimeString();
    DecodeTime(date_now,date_hour,date_minutes,date_seconds,date_miliseconds);

    if((date_hour==23)&&(date_minutes==59)&&((date_seconds>=40)&&(date_seconds<=49)))
    {

        Save_Stat();                    //���������� � ���� ���� ��� MEMO

        MemoDiag-> Lines -> Clear();    //������� ����������
        MemoStat-> Lines -> Clear();    //������� ����������
        MemoGraph-> Lines -> Clear();   //������� ��������

        //�������� ������� ��������
        serTemp[0]  -> Clear();
        serTemp[1]  -> Clear();
        serTemp[2]  -> Clear();
        serTemp[3]  -> Clear();
        serTemp[4]  -> Clear();
        serTemp[5]  -> Clear();
        serTemp[6]  -> Clear();
        serTemp[7]   -> Clear();
        serTemp[8]  -> Clear();
        serTemp[9]  -> Clear();
        serTemp[10]  -> Clear();

        //�������� �������� ��������
        serArh[0]->Clear();
        serArh[1]->Clear();
        serArh[2]->Clear();
        serArh[3]->Clear();
        serArh[4]->Clear();
        serArh[5]->Clear();
        serArh[6]->Clear();
        serArh[7]->Clear();
        serArh[8]->Clear();
        serArh[9]->Clear();
        serArh[10]->Clear();

        //������� �������� ���������
        MemoDiagArh    -> Lines -> Clear();
        MemoStatArh -> Lines -> Clear();


        //���������� ������ �������
        int
            fileCount,
            rezult;
            TSearchRec SR;
        // ���������� � �����������
        fileCount = 0;
        ListBoxStatArh -> Clear();
        rezult = FindFirst("Stat\\*.txt", faAnyFile, SR);
        while ( !rezult ){
            fileCount++;
            SR.Name.SetLength(10);
            ListBoxStatArh -> Items -> Add( SR.Name );
            rezult = FindNext(SR);
        };
        EdtArhStat -> Text = fileCount;
        ListBoxStatArh -> Sorted;
        // �������
        fileCount = 0;
        ListBoxGraphArh -> Clear();
        rezult = FindFirst("Graph\\*.txt", faAnyFile, SR);
        while ( !rezult ){
            fileCount++;
            SR.Name.SetLength(10);
            ListBoxGraphArh -> Items -> Add( SR.Name );
            rezult = FindNext(SR);
        };
        EdtArhGraph -> Text = fileCount;
        ListBoxGraphArh -> Sorted;
    }
}
//---------------------------------------------------------------------------
//--������������ ���������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualParT()
{
EdtTKon1 -> Text = FloatToStrF((float)par_t[1], ffFixed, 5, 0);
EdtTKon2 -> Text = FloatToStrF((float)par_t[2], ffFixed, 5, 0);
EdtTKon3 -> Text = FloatToStrF((float)par_t[3], ffFixed, 5, 0);
EdtTKon4 -> Text = FloatToStrF((float)par_t[4], ffFixed, 5, 0);
EdtTKon5 -> Text = FloatToStrF((float)par_t[5], ffFixed, 5, 0);

}
//---------------------------------------------------------------------------
//--�������� ���������� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParTClick(TObject *Sender)
{
       PanelParT -> Visible = true;
}
//---------------------------------------------------------------------------
//--����� �� �������� ���������� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnTNetClick(TObject *Sender)
{
    PanelParT -> Visible = false;
}
//---------------------------------------------------------------------------
//--������������� �������� ���������� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnTDaClick(TObject *Sender)
{
    // ������ ������������� �������� ������
    PanelParT -> Visible = false;

    par_t[1]  = StrToInt(EdtTRed1->Text);
    par_t[2]  = StrToInt(EdtTRed2->Text);
    par_t[3]  = StrToInt(EdtTRed3->Text);
    par_t[4]  = StrToInt(EdtTRed4->Text);
    par_t[5]  = StrToInt(EdtTRed5->Text);

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "  �������� ��������� ����������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    if ( EdtTKon1 -> Text != EdtTRed1 -> Text )
        MemoStat -> Lines -> Add("���� ������������ �� ��������� ������� (HOME) �� ����������� �������� � �������: " + EdtTKon1 -> Text + " -> " + EdtTRed1 -> Text );
    if ( EdtTKon2 -> Text != EdtTRed2 -> Text )
        MemoStat -> Lines -> Add("���� ������������ �� ��������� ������� (HOME) �� ����������� �������� � ������: " + EdtTKon2 -> Text + " -> " + EdtTRed2 -> Text );
    if ( EdtTKon3 -> Text != EdtTRed3 -> Text )
        MemoStat -> Lines -> Add("���� �������� ������������ �� ��������� ������� (HOME) �� ����������� �������� � �������: " + EdtTKon3 -> Text + " -> " + EdtTRed3 -> Text );
    if ( EdtTKon4 -> Text != EdtTRed4 -> Text )
        MemoStat -> Lines -> Add("��� �������: " + EdtTKon4 -> Text + " -> " + EdtTRed4 -> Text );
    if ( EdtTKon5 -> Text != EdtTRed5 -> Text )
        MemoStat -> Lines -> Add("���� ������� ��� ����������� ��������: " + EdtTKon5 -> Text + " -> " + EdtTRed5 -> Text );

    // ����������� ���������� ���������
    EdtTRed1 -> Color = clWhite;
    EdtTRed2 -> Color = clWhite;
	EdtTRed3 -> Color = clWhite;
	EdtTRed4 -> Color = clWhite;
	EdtTRed5 -> Color = clWhite;

    // �������� ��������
    VisualParT();
    MemoT -> Lines -> Clear();
    MemoT -> Lines -> Add(EdtTKon1->Text);
    MemoT -> Lines -> Add(EdtTKon2->Text);
    MemoT -> Lines -> Add(EdtTKon3->Text);
    MemoT -> Lines -> Add(EdtTKon4->Text);
    MemoT -> Lines -> Add(EdtTKon5->Text);

    MemoT -> Lines -> SaveToFile("Nasmod\\Mex.txt");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_MechTimer(TObject *Sender)
{   // ������������ ���������� � �� �����
    // �����������
    if(!ust_ready) return;
/*
    if(zin[4]&0x200)
    {
        Img_per_kas->Picture->Bitmap = man_left_1_e->Picture->Bitmap;
        Img_per_mid->Picture->Bitmap = man_mid_1_e->Picture->Bitmap;
        Img_per_kam->Picture->Bitmap = man_right_1_e->Picture->Bitmap;
    }
    else
    {
        Img_per_kas->Picture->Bitmap = man_left_0_e->Picture->Bitmap;
        Img_per_mid->Picture->Bitmap = man_mid_0_e->Picture->Bitmap;
        Img_per_kam->Picture->Bitmap = man_right_0_e->Picture->Bitmap;
    }
    if(TEK_ABS_POV == 0)
    {
        Img_per_mid->Left = 260;
        Img_per_mid->Width = 50 + int(155.0*float(TEK_ABS_PER)/float(par_t[2]));
        Img_per_kas->Left = Img_per_mid->Left - 5;
        Img_per_kam->Left = Img_per_mid->Left + Img_per_mid->Width;
    }
    if(TEK_ABS_POV == par_t[3])
    {
        Img_per_mid->Width = 50 + int(90.0*float(TEK_ABS_PER)/float(par_t[1]));
        Img_per_mid->Left = 260 + 50 - Img_per_mid->Width;
        Img_per_kas->Left = Img_per_mid->Left - 5;
        Img_per_kam->Left = Img_per_mid->Left + Img_per_mid->Width;
    }
    // �������
    if(zin[4]&0x1000)
        Img_pov->Picture->Bitmap = man_mid_1_e->Picture->Bitmap;
    else
        Img_pov->Picture->Bitmap = man_mid_0_e->Picture->Bitmap;
    Img_pov->Left = 265 + int(35.0*float(TEK_ABS_POV)/float(par_t[3]));
    //������� � HOME
    if(zin[4]&0x02)
        kasseta_null->Picture->Bitmap = kasseta_home->Picture->Bitmap;
    else
        kasseta_null->Picture->Bitmap = kasseta_green->Picture->Bitmap;
    kasseta_null->Top = 301 + int(24.0*float(-TEK_ABS_KAS)/float(par_t[4]+par_t[5]));

    // �������� ����
    Edt_AZ_1_1mn -> Text = IntToStr(par[0][15]);
    Edt_AZ_2_1mn -> Text = IntToStr(par[0][17]);
    Edt_AZ_3_1mn -> Text = IntToStr(par[0][18]);
    // ���������� ����
    Edt_AZ_1_2mn -> Text = IntToStr(TEK_ABS_PER);
    Edt_AZ_2_2mn -> Text = IntToStr(TEK_ABS_POV);
    Edt_AZ_3_2mn -> Text = IntToStr(TEK_ABS_KAS);
    // ������������� ����
    Edt_AZ_1_3mn -> Text = IntToStr(TEK_OTN_PER);
    Edt_AZ_2_3mn -> Text = IntToStr(TEK_OTN_POV);
    Edt_AZ_3_3mn -> Text = IntToStr(TEK_OTN_KAS);

    */
}
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_OptClick(TObject *Sender)
{
  // ��� ���������� ����������
  // �������� �������
  iniPAS.state[0] = !CB_Acc_V1->Checked;
  PCNalad->Pages[0]->TabVisible = !CB_Acc_V1->Checked;
  iniPAS.state[1] = !CB_Acc_V2->Checked;
  PCNalad->Pages[1]->TabVisible = !CB_Acc_V2->Checked;
  iniPAS.state[2] = !CB_Acc_V3->Checked;
  PCNalad->Pages[2]->TabVisible = !CB_Acc_V3->Checked;
  iniPAS.state[3] = !CB_Acc_V4->Checked;
  PCNalad->Pages[3]->TabVisible = !CB_Acc_V4->Checked;
  iniPAS.state[4] = !CB_Acc_V5->Checked;
  PCNalad->Pages[4]->TabVisible = !CB_Acc_V5->Checked;
  iniPAS.state[5] = !CB_Acc_V6->Checked;
  PCNalad->Pages[5]->TabVisible = !CB_Acc_V6->Checked;
  iniPAS.state[6] = !CB_Acc_V7->Checked;
  PCNalad->Pages[6]->TabVisible = !CB_Acc_V7->Checked;
  iniPAS.state[7] = !CB_Acc_V8->Checked;
  PCNalad->Pages[7]->TabVisible = !CB_Acc_V8->Checked;

  // �������� ������ � �������, ������ ���� �� ������
  SaveData2();

   Vtekpas_str = "";
   Vnew1pas_str = "";
   Vnew2pas_str = "";

   Pnl_Acc_Pas -> Visible = true;
   Pnl_Acc_Opt -> Visible = false;
   Pnl_Acc_SetPas -> Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Edit_Acc_TekPasKeyPress(TObject *Sender,
      char &Key)
{
  // ���� �������� ������ ��� �����
  if((Key == 8) && (Vtekpas_str.Length() > 0)) // ��������
  {
       Vtekpas_str[Vtekpas_str.Length()] = 0;     // ������� ��������� ������ � ������� ������
       Vtekpas_str.SetLength(Vtekpas_str.Length() - 1);
  }
  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vtekpas_str += Key; // ��������� ������ � ������� ������
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit_Acc_NewPas1KeyPress(TObject *Sender,
      char &Key)
{
  // ���� ������ ������ 1 ��� �����
  if((Key == 8) && (Vnew1pas_str.Length() > 0)) // ��������
  {
       Vnew1pas_str[Vnew1pas_str.Length()] = 0;     // ������� ��������� ������ � ������� ������
       Vnew1pas_str.SetLength(Vnew1pas_str.Length() - 1);
  }
  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vnew1pas_str += Key; // ��������� ������ � ������� ������
  }    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit_Acc_NewPas2KeyPress(TObject *Sender,
      char &Key)
{
  // ���� ������ ������ 2 ��� �����
  if((Key == 8) && (Vnew2pas_str.Length() > 0)) // ��������
  {
       Vnew2pas_str[Vnew2pas_str.Length()] = 0;     // ������� ��������� ������ � ������� ������
       Vnew2pas_str.SetLength(Vnew2pas_str.Length() - 1);
  }
  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vnew2pas_str += Key; // ��������� ������ � ������� ������
  }    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_NewPasClick(TObject *Sender)
{
   // ������������ ������ ���������� ������
   if((Vtekpas_str != Edit_Acc_DefPas->Text)&&((Vtekpas_str != Edit_Acc_UserPas->Text)||(Edit_Acc_UserPas->Text == "")))
   {
        Vtekpas_str = "";
        Vnew1pas_str = "";
        Vnew2pas_str = "";
        // ����������� ���� "������� ������ ������ �� �����"
        MessageBox(NULL, "������� ������ ������ �� �����", "������" , MB_OK | MB_ICONERROR);
        return;
   }
   if(Vnew1pas_str != Vnew2pas_str)
   {
        Vtekpas_str = "";
        Vnew1pas_str = "";
        Vnew2pas_str = "";
        // ����������� ���� "��������� ������ �� ���������"
        MessageBox(NULL, "��������� ������ �� ���������", "������" , MB_OK | MB_ICONERROR);
        return;
   }
   if((Vnew1pas_str.Length()<5)||(Vnew1pas_str.Length()>20))
   {
        Vtekpas_str = "";
        Vnew1pas_str = "";
        Vnew2pas_str = "";
         // ����������� ���� "����� ������ �� ������������� �����������"
        MessageBox(NULL, "����� ������ �� ������������� �����������", "������" , MB_OK | MB_ICONERROR);
        return;
   }

   Edit_Acc_UserPas->Text = Vnew1pas_str;
   for(int i=1;i<= 30;i++)
        iniPAS.pass[i-1] = 0;
   for(int i=1; i<= Vnew1pas_str.Length();i++)
        iniPAS.pass[i-1] = Vnew1pas_str[i];

   SaveData2();

   Vtekpas_str = "";
   Vnew1pas_str = "";
   Vnew2pas_str = "";

   Pnl_Acc_Pas -> Visible = true;
   Pnl_Acc_Opt -> Visible = false;
   Pnl_Acc_SetPas -> Visible = false;

   // ����������� ���� "����� ������ ����������"
   MessageBox(NULL, "����� ������ ����������", "�������������" , MB_OK );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Edit_Acc_VPasKeyPress(TObject *Sender,
      char &Key)
{
  // ���� ������ ��� �����
  if((Key == 8) && (Vpas_str.Length() > 0)) // ��������
  {
       Vpas_str[Vpas_str.Length()] = 0;     // ������� ��������� ������ � ������� ������
       Vpas_str.SetLength(Vpas_str.Length() - 1);
  }

  else if(Key == 13) // ����
  {
        // ��������� ���������
        But_Acc_VPasClick(But_Acc_VPas);
  }

  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vpas_str += Key; // ��������� ������ � ������� ������
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_VPasClick(TObject *Sender)
{
        // ��������� ������
        // ������� ���
  if((Edit_Acc_DefPas->Text == Vpas_str)||((Edit_Acc_UserPas->Text != "")&&(Edit_Acc_UserPas->Text == Vpas_str)))
        {
                Vpas_str = "";
                Pnl_Acc_Pas -> Visible = false;
                Pnl_Acc_Opt -> Visible = true;
                Pnl_Acc_SetPas -> Visible = true;
        }
  else
  {
        // ����������� ���� "������ ������ �� �����"
        MessageBox(NULL, "������ ������ �� �����!", "������" , MB_OK | MB_ICONERROR);
                Vpas_str = "";
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_AccTimer(TObject *Sender)
{
 // ������ ��� ������ ��������� � ���� ����� �������
    // �������� ����������� ��������� ������
    pas_str = "";
    if(Vpas_str.Length() > 0)
    {
        for(int i=1;i < Vpas_str.Length();i++)
          pas_str += '*';
        if(Edit_Acc_VPas->Focused())
                pas_str += Vpas_str[Vpas_str.Length()];
        else
                pas_str += '*';
    }
    Edit_Acc_VPas -> Text = pas_str;
    Edit_Acc_VPas -> SelStart = pas_str.Length();
    // �������� ����������� �������� ������
    pas_str = "";
    if(Vtekpas_str.Length() > 0)
    {
        for(int i=1;i < Vtekpas_str.Length();i++)
          pas_str += '*';
        if(Edit_Acc_TekPas->Focused())
                pas_str += Vtekpas_str[Vtekpas_str.Length()];
        else
                pas_str += '*';
    }
    Edit_Acc_TekPas -> Text = pas_str;
    Edit_Acc_TekPas -> SelStart = pas_str.Length();
    // �������� ����������� ������ ������ 1
    pas_str = "";
    if(Vnew1pas_str.Length() > 0)
    {
        for(int i=1;i < Vnew1pas_str.Length();i++)
          pas_str += '*';
        if(Edit_Acc_NewPas1->Focused())
                pas_str += Vnew1pas_str[Vnew1pas_str.Length()];
        else
                pas_str += '*';
    }
    Edit_Acc_NewPas1 -> Text = pas_str;
    Edit_Acc_NewPas1 -> SelStart = pas_str.Length();
    // �������� ����������� ������ ������ 2
    pas_str = "";
    if(Vnew2pas_str.Length() > 0)
    {
        for(int i=1;i < Vnew2pas_str.Length();i++)
          pas_str += '*';
        if(Edit_Acc_NewPas2->Focused())
                pas_str += Vnew2pas_str[Vnew2pas_str.Length()];
        else
                pas_str += '*';
    }
    Edit_Acc_NewPas2 -> Text = pas_str;
    Edit_Acc_NewPas2 -> SelStart = pas_str.Length();
}
//------------------------------------------------------------------------------
void TForm1::SaveData2()
{
	int SizeOfIniFile=(int)sizeof(iniPAS);

	if(!DirectoryExists("Data")) { CreateDir("Data"); }
	FILE *im0;
	im0=fopen(loc_acc_udb,"wb");
	if(im0)       { fwrite(&iniPAS,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::LoadData2()
{
	int SizeOfIniFile=(int)sizeof(iniPAS);

	if(!DirectoryExists("Data")) { CreateDir("Data"); }
	FILE *im0;
	im0=fopen(loc_acc_udb,"rb");
	if(im0)       { fread(&iniPAS,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� ��������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------

void TForm1::Klaster()
{
    if(PR_KLASTER) // ��������� ��� �������
    {
    // ������
    PnlPVS -> Visible = false;
    PnlSbor -> Visible = false;
    PnlUstOff -> Left = 315;
    PnlRC -> Left = 650;

    // ������� �� ����������
    KASSETA -> Visible = false;
    Gas_Name8 -> Visible = false;

    Plast -> Visible = false; // ���������� ������� �� ����������

    // ���������� ������� � ����������
    Panel2047 -> Visible = false;
    Panel2048 -> Visible = false;
    Panel2052 -> Visible = false;
    Panel2054 -> Visible = false;

    // ������������ �� ���������� �������
    Panel317 -> Visible = false;
    Panel90 -> Visible = false;
    Panel324 -> Visible = false;

    // ��������� �������
    Panel89 -> Visible = false;
    Panel279 -> Visible = false;
    Panel309 -> Visible = false;
    Panel289 -> Visible = false;
    Panel91 -> Visible = false;
    Panel282 -> Visible = false;
    Panel311 -> Visible = false;
    Panel290 -> Visible = false;
    Panel286 -> Visible = false;
    Panel283 -> Visible = false;
    Panel312 -> Visible = false;
    Panel291 -> Visible = false;
    Panel287 -> Visible = false;
    Panel306 -> Visible = false;
    Panel314 -> Visible = false;
    Panel292 -> Visible = false;
    Panel288 -> Visible = false;
    Panel308 -> Visible = false;
    Panel315 -> Visible = false;
    Panel293 -> Visible = false;

    // ���������
    Label58 -> Visible = false;
    EditNastrIn17 -> Visible = false;
    EditNastrTo17 -> Visible = false;
    Label42 -> Visible = false;
    

    }
}



