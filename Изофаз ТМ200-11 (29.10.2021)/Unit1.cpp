//---------------------------------------------------------------------------
#include <vcl.h>
#include <vfw.h>
#include <Clipbrd.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#pragma hdrstop

#include "Unit1.h"
#include "Header.h"
#include "Logic.cpp"

#include "Modules\Com.cpp"
#include "Modules\DZaslVAT\DZaslVAT.cpp"
#include "Modules\MERA\MERA.cpp"
#include "Modules\TRMD12\TRMD12.cpp"
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
void __fastcall TLogicThread::LM()
{
    // ������� ���� ������
    LogicMain();
    // ����� �� ����� ������� �������
    logic_time = float(TimeNew.QuadPart-TimeOld.QuadPart)/float(TimeFreq.QuadPart);
}
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

///////////////////////////////////////////////////////////////////////////////////
//--����������� ������� ����� ������--//
__fastcall TTimerExist::TTimerExist(bool CreateSuspended) : TThread(CreateSuspended)
{
}
///////////////////////////////////////////////////////////////////////////////////
void TTimerExist::SetName()
{   THREADNAME_INFO info;
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
///////////////////////////////////////////////////////////////////////////////////
void __fastcall TTimerExist::Execute()
{   SetName();
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
        timeNew,i=0;
    // ����������� �������� �������
    while(!Terminated)
    {   // ����������� ������ �������� �������
        timeNew = GetTickCount();
        // ��������� ������ ������� � ���������� �� ������� � 1 ��
        if(timeNew!=timeOld)
        {   for(i=0;i<(timeNew-timeOld);i++) { Synchronize(Timer1ms); }  // ��������� ��������� �����
            // �������� "������" �����
            timeOld = timeNew;
        }
        // ����������� ������ �������� �������
        QueryPerformanceCounter(&TimeNew);
        // ��������� ������ ������� � ���������� �� ������� � 1 ��
        if(((TimeNew.QuadPart - TimeOld.QuadPart)/TimeFreq.QuadPart)>=1)
        {   // ��������� ��������� ���������� ����
            Synchronize(EM);
            // �������� "������" �����
            TimeOld = TimeNew;
        }
    }
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
//--��������� ������ �� ����������� �������--//
//---------------------------------------------------------------------------
void TForm1::ExternalManager()
{
	// ���� ��������� � ������ ������� ��������� ������ Zin
    if ( pr_otl) return;
	
	// ������ ISO-P32C32_1
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
	// ������ ISO-P32C32_2
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
        externalError = ACL7250( 0 , zin);
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

    // ������ PCI-1730U
    else if( externalTask & 0x08 )
    {
        // ��������
        externalError = PCI1730( 0 , zin);
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x20);
                // ����� ������
                externalTask &=(~0x08);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x20;
                // ����� ������
                externalTask &= (~0x08);
            }; break;
        }
    }

    // ������ ���������� ������� �������� � ISO-813
    else if(externalTask & 0x10)
    {
        // ��������
        externalError = ISO_813(aik , AIK_COUNT * 8); // DI15 - � 7017 ��������
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x04);
                // ����� ������
                externalTask &= (~0x10);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x04;
				// ����� ������
                externalTask &= (~0x10);
            }; break;
        }
    }

    // OUT ------------------------------------------------
    // �������� ���������� �������� ������� � ISO-P32�32(1)
    else if(externalTask & 0x20)
    {
         // ��������
         externalError = ISO_P32C32_1 ( 1 , out );
        // ������ �������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x01);
                // ���� ��� ������ ����� ����� ������
                externalTask &= (~0x20);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x01;
				// ����� ������
                externalTask &= (~0x20);
            }; break;
        }
    }
    // �������� ���������� �������� ������� � ISO-P32�32(2)
    else if(externalTask & 0x40)
    {
         // ��������
         externalError = ISO_P32C32_2 ( 1 , out );
        // ������ �������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x02);
                // ���� ��� ������ ����� ����� ������
                externalTask &= (~0x40);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x02;
				// ����� ������
                externalTask &= (~0x40);
            }; break;
        }
    }

    // �������� ���������� �������� ������� � ACL7250
    else if(externalTask & 0x80 )
    {
        // ��������
        externalError = ACL7250 ( 1 , out );
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x10);
                // ����� ������
                externalTask &= (~0x80);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x10;
				// ����� ������
                externalTask &= (~0x80);
            }; break;
        }
    }

    // ������ ������� PCI1730
    else if(externalTask & 0x100 )
    {
        // ��������
        externalError = PCI1730( 1 , out );
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x20);
                // ����� ������
                externalTask &= (~0x100);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x20;
				// ����� ������
                externalTask &= (~0x100);
            }; break;
        }
    }

    // ������ ���������� �������� �������� � ISO-DA16
    else if( externalTask & 0x200 )
    {
        for( int i = 0 ; i < A_OUT_COUNT * 4 ; i++ )
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
                }; break;
            }
        }
        externalTask &= (~0x200);
    }
	// �������������� �������
	else
		externalTask = 0x3FF;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ �������� �������--//
void TForm1::VisualFormat()
{
    // �����
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
          if ((i*8+j) == 15 ) { UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/32767.0 * 10.0, ffFixed, 5, 3); }
          else               { UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/4095.0 * 10.0,  ffFixed, 5, 3); }
          //UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/4095.0 * 10.0,  ffFixed, 5, 3);
      }
    }
    // ������������ ���������� �������
    for(i=0;i<A_OUT_COUNT;i++)
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
/////////////////////////////////////////////////////////

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
        klGir_gV = 1; // ������� ������� ������� > "������"
    }
    if ( Key == kodKlN )
    {
        klGir_gN = 1; // ������� ������� ������� < "�����"
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
        klGir_gV = 0; // ������� ������� ������� > "������"
    }
    if ( Key == kodKlN )
    {
        klGir_gN = 0; // ������� ������� ������� < "�����"
    }
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
// ������������� ������� ����������� ������� ��������
    serTemp[1] = Series1;
    serTemp[2] = Series2;
    serTemp[3] = Series3;
    serTemp[4] = Series4;
    serTemp[5] = Series5;
    serTemp[6] = Series6;
    serTemp[7] = Series7;
    serTemp[8] = Series8;
    serTemp[9] = Series9;

    // ������������� ������� ����������� �������� ��������
    serArh[1] = Series11;
    serArh[2] = Series12;
    serArh[3] = Series13;
    serArh[4] = Series14;
    serArh[5] = Series15;
    serArh[6] = Series16;
    serArh[7] = Series17;
    serArh[8] = Series18;
    serArh[9] = Series19;


    // ���������������� ������� ���
   InitObjectsRRG();   //���������� RRG.cpp
   InitObjectsKl();
   InitObjectsBPN();

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

    // ������������ ����-�������
    Label_Time -> Caption = TimeToStr(Time());
    Label_Date -> Caption = DateToStr(Date());

    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> LoadFromFile("Nasmod\\Nasmod.txt");
    EditNastrTo0  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](0));
    EditNastrTo1  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](1));
    EditNastrTo2  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](2));
    EditNastrTo3  -> Text = MemoNasmod -> Lines -> operator [](3);
    EditNastrTo4  -> Text = MemoNasmod -> Lines -> operator [](4);
    EditNastrTo5  -> Text = MemoNasmod -> Lines -> operator [](5);
    EditNastrTo6  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](6));
    EditNastrTo7  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](7));
    EditNastrTo17  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](8));
    EditNastrTo18  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](9));
    MemoNasmod -> Lines -> Clear();     
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
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Btn_ExitClick(TObject *Sender)
{
    if(MessageDlg("����� �� ���������?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
    Close();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_500_1Timer(TObject *Sender)
{
    
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
//--������������ ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualMnemo()
{   if(!ust_ready) return;
    VisualColorElement();                   //  ������������ ��������� ����������
    VisualVoda();                           //  ����������� ������� ����
    VisualParam();                          //  ������������ ���������� ����������
    VisualDiagn();                          //  ����������� ���������� �� ����������
    VisualButtons();                        //  ����������� ������
    VisualZagol();                          //  ������������ ����� ���������
    //VisualFormat();                         //  ������������ �������
    //VisualDebug();                          //  ����������� �������� �������
    //VisualGraph();                          //  ����������� ��������
    VisualOperatorDlg();                    //  ������������ ������� ���������

    if(PR_KLASTER && ust_ready) Visual_IM();    // ����������� � �������
}

//----��������� / ����������
void __fastcall TForm1::PCMainChange(TObject *Sender)
{
  if(PCMain -> ActivePage == TSNalad)
  {
        mnemoInAuto = 0;

        Pnl_Work -> Visible = false;

        Pnl_Work -> Parent = TabSheet1;
        Pnl_Work -> Top = -4;
        Pnl_Work -> Left = +4;

        //������� ����� ��������
        Panel69 -> Visible = false;
        Panel72 -> Visible = false;
        Panel79 -> Visible = false;
        Panel148 -> Visible = false;

        //�������� ������ �������
        PnlMnemoParam -> Height = 344;

        //������� ���-�� ������
        Cycles -> Visible = false;
        Pnl_Stage -> Visible = false;
        Pnl_Proc  -> Visible = false;
        Plast->Top=231;
        PnlMnemoParam->Top=263;


     VisualMnemo();
     Pnl_Work -> Visible = true;
     Panel317->BringToFront();
  }
  else if(PCMain -> ActivePage == TSWork)
  {
        mnemoInAuto = 1;

        Pnl_Work -> Visible = false;

        Pnl_Work -> Parent = TSWork;

        Pnl_Work -> Top = 32;
        Pnl_Work -> Left = 8;

        //��������� ����� ��������
        Panel69 -> Visible = true;
        Panel79 -> Visible = true;
        Panel72 -> Visible = true;
        Panel148 -> Visible = true;

        //�������� ������ �������
        PnlMnemoParam -> Height = 396;

        //��������� ���-�� ������
        Cycles -> Visible = true;
        Pnl_Stage -> Visible = true;
        Pnl_Proc  -> Visible = true;
        Plast->Top=295;
        PnlMnemoParam->Top=359;


    VisualMnemo();
    Pnl_Work -> Visible = true;
  }
}
//---------------------------------------
/////// ��������� ��������� �������
//---------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
Klaster();
//������� �� �������� ���� �������� ��������
vp1->BringToFront();
vp2->BringToFront();
vp3->BringToFront();
vp4->BringToFront();
vp5->BringToFront();
vp6->BringToFront();
vp7->BringToFront();
vp8->BringToFront();
vp9->BringToFront();
vp10->BringToFront();
vp11->BringToFront();
vp13->BringToFront();
vp15->BringToFront();
vp16->BringToFront();
vp17->BringToFront();
vp18->BringToFront();
vp19->BringToFront();
vp20->BringToFront();
vp21->BringToFront();
vp22->BringToFront();
vp23->BringToFront();
ve1->BringToFront();
kl_nap1->BringToFront();
kl_nap2->BringToFront();

fk_kam->BringToFront();
zasl->BringToFront();
zatv->BringToFront();
zatvor->BringToFront();
kl_d2->BringToFront();
kl_d4->BringToFront();
fk_tmn->BringToFront();
fn_kam->BringToFront();
tmn->BringToFront();
fn_shl->BringToFront();
coef_zad->BringToFront();
coef_tek->BringToFront();
    Prek_1->BringToFront();
    Prek_2->BringToFront();
    Prek_3->BringToFront();
    Prek_4->BringToFront();
 Panel317->BringToFront();
 Pnl_GK->BringToFront();
//���������� ������ ��������� �� ������ �������
APanel -> Left = 600;
APanel -> Top = 340;
NPanel -> Left = 600;
NPanel -> Top = 340;
PnlDiagm ->Left=655;
PnlDiagm ->Top=70;
PnlCondition->Left=650;
PnlCondition->Top=180;
// ������ ����������� ������ � ������
   // ������� �����
   LogicThread = new TLogicThread(true);
   // ����� ������������ ����� ����������
   LogicThread -> FreeOnTerminate = true;
   // ��������� ������ ������
   LogicThread -> Priority = tpLower;
   // ��������� �����
   LogicThread -> Resume();
   /////////////////////////////////////////////////////////////////////////////
   // ������ ������������� ������� � ������
   // ������� �����
   TimerExist = new TTimerExist(true);
   // ����� ������������ ����� ����������
   TimerExist -> FreeOnTerminate = true;
   // ��������� ������ ������
   TimerExist -> Priority = tpLower;
   // ��������� �����
   TimerExist -> Resume();
   /////////////////////////////////////////////////////////////////////////////


    unsigned char i=0,j=0;

  /////////////////////////////////////////////////////////////////////////////
   TGroupBox *ZinParents[ZIN_COUNT*5] =
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
        Title_Zin[i][j] -> Width = 430;
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
   TGroupBox *OutParents[OUT_COUNT*4] =
   { Form1->GB_out0_1,
     Form1->GB_out0_2,
     Form1->GB_out1_1,
     Form1->GB_out1_2,
     Form1->GB_out2_1,
     Form1->GB_out2_2,
     Form1->GB_out3_1,
     Form1->GB_out3_2,
     Form1->GB_out4_1,
     Form1->GB_out4_2
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
        Title_Out[i][j] -> Width = 430;
        Title_Out[i][j] -> Layout = tlTop;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // �������� ����������
        Check_Out[i][j] = new TImage(this);
        // ������������, �������
        if(j<=7)
        { Check_Out[i][j] -> Parent = OutParents[2*i];
          Check_Out[i][j] -> Top = 36 + 36 * j;
        }
        else
        { Check_Out[i][j] -> Parent = OutParents[2*i+1];
          Check_Out[i][j] -> Top = 36 + 36 * (j-8);
        }
        Check_Out[i][j] -> Left = 462;
        Check_Out[i][j] -> Height = 25;
        Check_Out[i][j] -> Width = 25;
        Check_Out[i][j] -> Picture=check0->Picture;
        Check_Out[i][j] -> Transparent =true;

        Check_Out[i][j] -> Hint = IntToStr(int(pow(2,j)));
        Check_Out[i][j] -> OnClick = SetOutClick;
        Check_Out[i][j] -> OnDblClick = SetOutClick;
     }
   }
   /////////////////////////////////////////////////////////////////////////////
   TGroupBox *AinParents[AIK_COUNT] =
   { Form1->GB_ain0,
     Form1->GB_ain1,
     Form1->GB_ain2
   };
   /////////////////////////////////////////////////////////////////////////////
   // ���� �� ���������� ����������� ��� �����������
   for(i=0;i<AIK_COUNT;i++)
   {
        LDec_Ain[i] = new TLabel(this);
        // ������������, �������, ��������
        LDec_Ain[i] -> Parent = AinParents[i];
        LDec_Ain[i] -> Left = 484;
        LDec_Ain[i] -> Top = 13;
        LDec_Ain[i] -> Font -> Name = "Arial";
        LDec_Ain[i] -> Font -> Size = 13;
        LDec_Ain[i] -> Font -> Color = clBlack;
        LDec_Ain[i] -> Caption = "���";
        LDec_Ain[i] -> Transparent = true;
        LDec_Ain[i] -> Width = 30;
        LDec_Ain[i] -> Height = 19;
        LDec_Ain[i] -> Layout = tlTop;

        LUv_Ain[i] = new TLabel(this);
        // ������������, �������, ��������
        LUv_Ain[i] -> Parent = AinParents[i];
        LUv_Ain[i] -> Left = 551;
        LUv_Ain[i] -> Top = 13;
        LUv_Ain[i] -> Font -> Name = "Arial";
        LUv_Ain[i] -> Font -> Size = 13;
        LUv_Ain[i] -> Font -> Color = clBlack;
        LUv_Ain[i] -> Caption = "U,�";
        LUv_Ain[i] -> Transparent = true;
        LUv_Ain[i] -> Width = 32;
        LUv_Ain[i] -> Height = 19;
        LUv_Ain[i] -> Layout = tlTop;

     // ���� �� ���������� ��� � ����������
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
        if((i*8+j) == 15) CG_Ain[i][j] -> MaxValue = 0x7FFF;
        else CG_Ain[i][j] -> MaxValue = 0x0FFF;
     }
   }
   /////////////////////////////////////////////////////////////////////////////
   TGroupBox *AoutParents[A_OUT_COUNT] =
   { Form1->GB_aout0,
     Form1->GB_aout1,
     Form1->GB_aout2
   };
   /////////////////////////////////////////////////////////////////////////////
   // ���� �� ���������� ����������� ��� �����������
   for(i=0;i<A_OUT_COUNT;i++)
   {
        LDec_Aout[i] = new TLabel(this);
        // ������������, �������, ��������
        LDec_Aout[i] -> Parent = AoutParents[i];
        LDec_Aout[i] -> Left = 481;
        LDec_Aout[i] -> Top = 13;
        LDec_Aout[i] -> Font -> Name = "Arial";
        LDec_Aout[i] -> Font -> Size = 13;
        LDec_Aout[i] -> Font -> Color = clBlack;
        LDec_Aout[i] -> Caption = "���";
        LDec_Aout[i] -> Transparent = true;
        LDec_Aout[i] -> Width = 30;
        LDec_Aout[i] -> Height = 19;
        LDec_Aout[i] -> Layout = tlTop;

        LUv_Aout[i] = new TLabel(this);
        // ������������, �������, ��������
        LUv_Aout[i] -> Parent = AoutParents[i];
        LUv_Aout[i] -> Left = 551;
        LUv_Aout[i] -> Top = 13;
        LUv_Aout[i] -> Font -> Name = "Arial";
        LUv_Aout[i] -> Font -> Size = 13;
        LUv_Aout[i] -> Font -> Color = clBlack;
        LUv_Aout[i] -> Caption = "U,�";
        LUv_Aout[i] -> Transparent = true;
        LUv_Aout[i] -> Width = 32;
        LUv_Aout[i] -> Height = 19;
        LUv_Aout[i] -> Layout = tlTop;

    // ���� �� ���������� ��� � ����������
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
        UV_Aout[i][j] -> Text = "0,000";
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
        UV_Aout_zad[i][j] -> Text = "0,000";
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
	
	// ���� �� ���������� ����������� ��� �����������
    for(i=0;i<PAR_NAGR;i++)
    {
        Zad_Nagr[i] = new TLabel(this);
        Zad_Nagr[i] -> Parent = Pnl_Work;
        Zad_Nagr[i] -> Top = 10 + 30 * i;
        Zad_Nagr[i] -> Left = 28;
        Zad_Nagr[i] -> Font -> Name = "Arial";
        Zad_Nagr[i] -> Font -> Size = 14;
        Zad_Nagr[i] -> Font -> Color = clWhite;
        Zad_Nagr[i] -> Caption = "0";
        Zad_Nagr[i] -> AutoSize = false;
        Zad_Nagr[i] -> Alignment = taCenter;
        Zad_Nagr[i] -> Transparent = true;
        Zad_Nagr[i] -> Height = 23;
        Zad_Nagr[i] -> Width = 37;
        Zad_Nagr[i] -> Layout = tlTop;

        Tek_Nagr[i] = new TLabel(this);
        Tek_Nagr[i] -> Parent = Pnl_Work;
        Tek_Nagr[i] -> Top = 10 + 30 * i;
        Tek_Nagr[i] -> Left = 66;
        Tek_Nagr[i] -> Font -> Name = "Arial";
        Tek_Nagr[i] -> Font -> Size = 14;
        Tek_Nagr[i] -> Font -> Color = clBlack;
        Tek_Nagr[i] -> Caption = "0";
        Tek_Nagr[i] -> AutoSize = false;
        Tek_Nagr[i] -> Alignment = taCenter;
        Tek_Nagr[i] -> Transparent = true;
        Tek_Nagr[i] -> Height = 23;
        Tek_Nagr[i] -> Width = 37;
        Tek_Nagr[i] -> Layout = tlTop;
    }
    Zad_Nagr[2] -> Visible = false;
    Tek_Nagr[2] -> Visible = false;
    Zad_Nagr[3] -> Visible = false;
    Tek_Nagr[3] -> Visible = false;
    Zad_Nagr[21] -> Visible = false;
  

    
     // ��������� ������ ��� ����������� ��������
    TSWork -> DoubleBuffered = true;
    EditSHRName -> DoubleBuffered = true;
    Pnl_Work -> DoubleBuffered = true;
    LBError -> DoubleBuffered = true;
    PCNalad -> DoubleBuffered = true;
    PCMain -> DoubleBuffered = true;
    KASSETA -> DoubleBuffered = true;
//------------------------
// ����������� �������
//-------------------------
    Load_Data();
    pas_str = "";
    for(int i=0;iniID.pass[i]!=0;i++)
    pas_str = pas_str + iniID.pass[i];
    Edit_Acc_UserPas -> Text = pas_str;

    // �������� �������
    PCNalad->Pages[0]->TabVisible = iniID.state[0];
    CB_Acc_V1->Checked = !iniID.state[0];
    PCNalad->Pages[1]->TabVisible = iniID.state[1];
    CB_Acc_V2->Checked = !iniID.state[1];
    PCNalad->Pages[2]->TabVisible = iniID.state[2];
    CB_Acc_V3->Checked = !iniID.state[2];
    PCNalad->Pages[3]->TabVisible = iniID.state[3];
    CB_Acc_V4->Checked = !iniID.state[3];
    PCNalad->Pages[4]->TabVisible = iniID.state[4];
    CB_Acc_V5->Checked = !iniID.state[4];
    PCNalad->Pages[5]->TabVisible = iniID.state[5];
    CB_Acc_V6->Checked = !iniID.state[5];
    PCNalad->Pages[6]->TabVisible = iniID.state[6];
    CB_Acc_V7->Checked = !iniID.state[6];

    // ���� ���������� �������������, ������������� �� ������
    if(!PCNalad->Pages[0]->TabVisible)
    PCNalad->ActivePage = TabSheet52;

    // ��������� ������� ���������� ��������
    //Label_Rec -> Caption = IntToStr(ActCell_Rec);
    //Label_Step -> Caption = IntToStr(ActCell_Step);
    
    // ����������� ������� �������
    //SG_Recept->Cells[0][0] = " �������� ����";
    //SG_Recept->Cells[1][0] = " #";
    //SG_Steps->Cells[0][0] = " #";
    //SG_Steps->Cells[1][0] = " �������� ����";

    //SG_Steps->ColWidths[0] = 10;
    //SG_Steps->ColWidths[1] = 20;



    LibListRefresh();   // �������� ���� ���������

    BtnRDaClick(BtnRDa);

    // �������� ����������� �������� �������
    AnsiString fName = "ParN\\ParN.udb";
    if(FileExists(fName))
    {
        int SizeOfIniFile=(int)sizeof(iniParN);
        FILE *im0;
        im0=fopen(fName.c_str(),"rb");
        if(im0)
        {
            fread(&iniParN,SizeOfIniFile,1,im0); fclose(im0);

            EdtNRed0->Text = IntToStr(iniParN.par_n[0]/10);
            EdtNRed1->Text = IntToStr(iniParN.par_n[1]/10);

            EdtNRed4->Text = IntToStr(iniParN.par_n[4]/10);
            EdtNRed5->Text = IntToStr(iniParN.par_n[5]/10);
            EdtNRed6->Text = IntToStr(iniParN.par_n[6]/10);
            EdtNRed7->Text = IntToStr(iniParN.par_n[7]/10);
            EdtNRed8->Text = IntToStr(iniParN.par_n[8]/10);
            EdtNRed9->Text = IntToStr(iniParN.par_n[9]/10);
            EdtNRed10->Text = IntToStr(iniParN.par_n[10]/10);
            EdtNRed11->Text = IntToStr(iniParN.par_n[11]/10);
            EdtNRed12->Text = IntToStr(iniParN.par_n[12]/10);
            EdtNRed13->Text = IntToStr(iniParN.par_n[13]/10);
            EdtNRed14->Text = IntToStr(iniParN.par_n[14]/10);
            EdtNRed15->Text = IntToStr(iniParN.par_n[15]/10);
            EdtNRed16->Text = IntToStr(iniParN.par_n[16]/10);
            EdtNRed17->Text = IntToStr(iniParN.par_n[17]/10);
            EdtNRed18->Text = IntToStr(iniParN.par_n[18]/10);
            EdtNRed19->Text = IntToStr(iniParN.par_n[19]/10);
            EdtNRed20->Text = IntToStr(iniParN.par_n[20]/10);
            EdtNRed22->Text = IntToStr(iniParN.par_n[22]/10);
            EdtNRed23->Text = IntToStr(iniParN.par_n[23]/10);
            BtnParNDaClick(BtnParNDa);
        }
        else{ MessageBox(NULL, "���������� ��������� ����������", "������", MB_OK | MB_ICONSTOP); }
    }

    // ������ ��������� ����
    OpenISO_P32C32(); // ������� ��������� � ��������� ISO-P32C32
    OpenACL_7250();   // ������� ��������� � ��������� ACL-7250
    OpenISO_813();    // ������� ��������� � ��������� ISO-813
    OpenISO_DA16();   // ������� ��������� � ��������� ISO-DA16   

    // �������� ���������
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

    SetOut(1,1,0x02); // ���������� ������ "���� ����������"

    LoadGasNames();
    //��������� �������� �����
    RenameGases();
    ////////////////////////////////////////////////////////
    //�������� ��������� "�������� ����"

    for(int i=0;i<KL_COUNT;i++)
    {
        Lb_Klapan[i] = new TLabel(this);
        Lb_Klapan[i] -> Parent = Panel_sost_kl;
        if(i<=10)
        {
            Lb_Klapan[i] -> Top = 50 + 30 * i;
            Lb_Klapan[i] -> Left = 10;
        }
        else
        {
             Lb_Klapan[i] -> Top = 50 + 30 * (i-11);
             Lb_Klapan[i] -> Left = 210;
        }
        Lb_Klapan[i] -> Font -> Name = "Arial";
        Lb_Klapan[i] -> Font -> Size = 14;
        Lb_Klapan[i] -> Font -> Color = clBlack;
        Lb_Klapan[i] -> Caption = kl_names[i];
        Lb_Klapan[i] -> AutoSize = true;
        Lb_Klapan[i] -> Alignment = taCenter;
        Lb_Klapan[i] -> Transparent = true;
        Lb_Klapan[i] -> Height = 23;
        Lb_Klapan[i] -> Layout = tlTop;
        ///////////////////////////////////////
        Cb_Klapan[i] = new TImage(this);
        Cb_Klapan[i] -> Parent = Panel_sost_kl;
        if(i<=10)
        {
            Cb_Klapan[i] -> Top = 50 + 30 * i;
            Cb_Klapan[i] -> Left = 60;
        }
        else
        {
            Cb_Klapan[i] -> Top = 50 + 30 * (i-11);
            Cb_Klapan[i] -> Left = 260;
        }
        Cb_Klapan[i] ->Transparent=true;
        Cb_Klapan[i] ->Picture=check0->Picture;
        Cb_Klapan[i] -> OnClick = check1Click;
        Cb_Klapan[i] -> OnDblClick = check1Click;
        Cb_Klapan[i] ->Hint=0;
    }
    for(int i=0;i<KL_COUNT;i++)
    {
        Lb_Klapan_Info[i] = new TLabel(this);
        Lb_Klapan_Info[i] -> Parent = Panel_kl_info;
            Lb_Klapan_Info[i] -> Top = 50 + 35 * i;
            Lb_Klapan_Info[i] -> Left = 5;
        Lb_Klapan_Info[i] -> Font -> Name = "Arial";
        Lb_Klapan_Info[i] -> Font -> Size = 12;
        Lb_Klapan_Info[i] -> Font -> Color = clBlack;
        Lb_Klapan_Info[i] -> Caption = kl_names[i];
        Lb_Klapan_Info[i] -> AutoSize = true;
        Lb_Klapan_Info[i] -> Alignment = taCenter;
        Lb_Klapan_Info[i] -> Transparent = true;
        Lb_Klapan_Info[i] -> Height = 23;
        Lb_Klapan_Info[i] -> Layout = tlTop;
        ///////////////////////////////////////
        Cb_Klapan_Info[i] = new TImage(this);
        Cb_Klapan_Info[i] -> Parent = Panel_kl_info;
            Cb_Klapan_Info[i] -> Top = 50 + 35 * i;
            Cb_Klapan_Info[i] -> Left = 55;
        Cb_Klapan_Info[i] ->Transparent=true;
        Cb_Klapan_Info[i] ->Picture=check0->Picture;
    }


    RefreshShTree();
    pr_lib=0;
    if(ComboBox1->Items->Count>0)
    ComboBox1->ItemIndex=0;
    StringGrid1->Cells[0][0]="�";
    StringGrid1->Cells[1][0]="�������� ����";
    LibListRefresh();
    StringGrid1->Selection.Top=1;

    Refresh_kl_info("Clear");
    SetOut(1,4,0x2000);
    
	ust_ready = 1;
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ��������� ����������--//
TColor TForm1::SetPopeColor(bool value)
{
	if(value) return clWhite;
    else      return clLime;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--����������� ������� �������� ����������--//
void TForm1::VisualParam()
{
    // �������������� �����
    if(PCMain -> ActivePage == TSWork) // �������������� ����������
    {
        if(N_ST)
        {
            //�������� ��������
            EdtZadA00 -> Text = FloatToStrF((float)par[N_ST][0]/4095.0*18.0,       ffFixed,5,2);   //������ ���1
            EdtZadA01 -> Text = FloatToStrF((float)par[N_ST][1]/4095.0*9.0,        ffFixed,5,2);   //������ ���2
            EdtZadA02 -> Text = FloatToStrF((float)par[N_ST][2]/4095.0*9.0,        ffFixed,5,2);   //������ ���3
            EdtZadA03 -> Text = FloatToStrF((float)par[N_ST][3]/4095.0*3.6,        ffFixed,5,2);   //������ ���4
            EdtZadA04 -> Text = FloatToStrF((float)par[N_ST][4]/4095.0*9.0,        ffFixed,5,2);   //������ ���5
            EdtZadA05 -> Text = FloatToStrF((float)par[N_ST][5]/4095.0*18.0,       ffFixed,5,2);   //������ ���6
            EdtZadA06 -> Text = FloatToStrF((float)par[N_ST][6]/4095.0*18.0,       ffFixed,5,2);   //������ ���7
            EdtZadA07 -> Text = FloatToStrF((float)par[N_ST][7]/4095.0*CESAR_MAX_IP,    ffFixed,5,0);     //�������� ��������
            EdtZadA11 -> Text = FloatToStrF((float)par[N_ST][10]/10000*13.3,             ffFixed,   5,1);    //��������

            EdtZadA12 -> Text = IntToStr(int(par[N_ST][11]/3600000)) +
            ":" + IntToStr(int((par[N_ST][11]%3600000)/60000)) +
            ":" + IntToStr(int((par[N_ST][11]%60000)/1000)) +
            "." + IntToStr(int(par[N_ST][11]%1000));

            EdtZadA09 -> Text = FloatToStrF((float)par[N_ST][8]/4095.0*10.0, ffFixed,5,2);
            EdtZadA10 -> Text = FloatToStrF((float)par[N_ST][9]/4095.0*10.0, ffFixed,5,2);

            PnlStageName->Caption = StringGrid1->Cells[1][N_ST];
            PnlProcName ->Caption = Tek_Rec_Name;
        }
        else
        {
            EdtZadA00 -> Text = "0,00";
            EdtZadA01 -> Text = "0,00";
            EdtZadA02 -> Text = "0,00";
            EdtZadA03 -> Text = "0,00";
            EdtZadA04 -> Text = "0,00";
            EdtZadA05 -> Text = "0,00";
            EdtZadA06 -> Text = "0,00";
            EdtZadA07 -> Text = "0";
            EdtZadA11 -> Text = "0,0";
            EdtZadA12 -> Text = "0:0:0.0";
            EdtZadA09 -> Text = "0,0";
            EdtZadA10 -> Text = "0,0";

            PnlStageName->Caption = "";
            PnlProcName ->Caption = "";
        }

      if(ZN_ST)
      {
        Panel63->Caption = IntToStr(par[ZN_ST-1][12]);
        Panel64->Caption = IntToStr(N_ZICL);
      }
      else
      {
        Panel63->Caption = "0";
        Panel64->Caption = "0";
      }
      zad_pl -> Caption = IntToStr(par[N_ST][20]);
      tek_pl -> Caption = N_PL;
      //��������� ��������
      dz_tek  -> Caption = FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,0) + "%";
        //������� ��������
        // ������ ���1
        if(shr[20]) { EdtTekA00 -> Text = FloatToStrF((float)aik[5]*18.0/4095.0,ffFixed,5,2);      }
        else        { EdtTekA00 -> Text = "0,00"; }
        // ������ ���2
      if(shr[21]) { EdtTekA01 -> Text = FloatToStrF((float)aik[6]*9.0/4095.0,ffFixed,5,2);      }
      else        { EdtTekA01 -> Text = "0,00"; }
      // ������ ���3
      if(shr[22]) { EdtTekA02 -> Text = FloatToStrF((float)aik[7]*9.0/4095.0,ffFixed,5,2);      }
      else        { EdtTekA02 -> Text = "0,00"; }
      // ������ ���4
      if(shr[23]) { EdtTekA03 -> Text = FloatToStrF((float)aik[8]*3.6/4095.0,ffFixed,5,2);      }
      else        { EdtTekA03 -> Text = "0,00"; }
      // ������ ���5
      if(shr[24]) { EdtTekA04 -> Text = FloatToStrF((float)aik[9]*9.0/4095.0,ffFixed,5,2);      }
      else        { EdtTekA04 -> Text = "0,00"; }
      // ������ ���6
      if(shr[25]) { EdtTekA05 -> Text = FloatToStrF((float)aik[10]*18.0/4095.0,ffFixed,5,2);      }
      else        { EdtTekA05 -> Text = "0,00"; }
      // ������ ���7
      if(shr[26]) { EdtTekA06 -> Text = FloatToStrF((float)aik[11]*18.0/4095.0,ffFixed,5,2);      }
      else        { EdtTekA06 -> Text = "0,00"; }
       // �������� ��������
      if(shr[29]) { EdtTekA07 -> Text = FloatToStrF((float)aik[12]*CESAR_MAX_IP/4095.0, ffFixed, 6, 0); }
      else      { EdtTekA07 -> Text = "0"; }
      // ��������� ��������
      if(shr[29]) { EdtTekA08 -> Text = FloatToStrF((float)aik[13]*CESAR_MAX_IP/4095.0, ffFixed, 6, 0); }
      else      { EdtTekA08 -> Text = "0"; }
      // ��������� ������������ �����
      EdtTekA09 -> Text = FloatToStrF((float)aik[17]/4095.0*10.0,ffFixed,5,2);
      // ��������� ������������ �����
      EdtTekA10 -> Text = FloatToStrF((float)aik[16]/4095.0*10.0,ffFixed,5,2);
      // ��������
      if(shr[17]) { EdtTekA11 -> Text = FloatToStrF((float)D_D3/10000*13.3,ffFixed,   5,2); }
      else      { EdtTekA11 -> Text = "0,00"; }
      // ����� ��������
      if(shr[4])
        {
        EdtTekA12 -> Text = IntToStr(int(T_PROC/3600000)) +
        ":" + IntToStr(int((T_PROC%3600000)/60000)) +
        ":" + IntToStr(int((T_PROC%60000)/1000)) +
        "." + IntToStr(int(T_PROC%1000));
        }
      else      { EdtTekA12 -> Text = "0:0:0.0"; }
      if(PR_KLASTER==0)  Plast->Visible=true;
    }
    if(PCMain -> ActivePage == TSNalad) //�������
    {
        Plast->Visible=false;
        //�������� ��������
        EdtZadA00 -> Text = FloatToStrF((float)par[0][0]/4095.0*18.0,ffFixed,5,2);   //������ ���1
        EdtZadA01 -> Text = FloatToStrF((float)par[0][1]/4095.0*9.0,ffFixed,5,2);   //������ ���2
        EdtZadA02 -> Text = FloatToStrF((float)par[0][2]/4095.0*9.0,ffFixed,5,2);   //������ ���3
        EdtZadA03 -> Text = FloatToStrF((float)par[0][3]/4095.0*3.6,ffFixed,5,2);   //������ ���4
        EdtZadA04 -> Text = FloatToStrF((float)par[0][4]/4095.0*9.0,ffFixed,5,2);   //������ ���5
        EdtZadA05 -> Text = FloatToStrF((float)par[0][5]/4095.0*18.0,ffFixed,5,2);   //������ ���6
        EdtZadA06 -> Text = FloatToStrF((float)par[0][6]/4095.0*18.0,ffFixed,5,2);   //������ ���7
        EdtZadA07 -> Text =  EdtRKon0_7 -> Text;     //�������� ��������
        EdtZadA11 -> Text = FloatToStrF((float)par[0][10]/10000*13.3,ffFixed,5,1);    //��������
        EdtZadA09 -> Text = "�";
        EdtZadA10 -> Text = "�";
        

    //������� ��������
    //��������� ��������
      dz_tek  -> Caption = FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,0) + "%";
    // ������ ���1
    EdtTekA00 -> Text = FloatToStrF((float)aik[5]*18.0/4095.0,ffFixed,5,2);
    // ������ ���2
    EdtTekA01 -> Text = FloatToStrF((float)aik[6]*9.0/4095.0,ffFixed,5,2);
    // ������ ���3
    EdtTekA02 -> Text = FloatToStrF((float)aik[7]*9.0/4095.0,ffFixed,5,2);
    // ������ ���4
    EdtTekA03 -> Text = FloatToStrF((float)aik[8]*3.6/4095.0,ffFixed,5,2);
    // ������ ���5
    EdtTekA04 -> Text = FloatToStrF((float)aik[9]*9.0/4095.0,ffFixed,5,2);
    // ������ ���6
    EdtTekA05 -> Text = FloatToStrF((float)aik[10]*18.0/4095.0,ffFixed,5,2);
    // ������ ���7
    EdtTekA06 -> Text = FloatToStrF((float)aik[11]*18.0/4095.0,ffFixed,5,2);
    // �������� ��������
    EdtTekA07 -> Text = FloatToStrF((float)aik[12]*CESAR_MAX_IP/4095.0, ffFixed, 6, 0);
    // ��������� ��������
    EdtTekA08 -> Text = FloatToStrF((float)aik[13]*CESAR_MAX_IP/4095.0, ffFixed, 6, 0);
    // ��������� ������������ �����
    EdtTekA09 -> Text = FloatToStrF((float)aik[17]/4095.0*10.0,ffFixed,5,2);
    // ��������� ������������ �����
    EdtTekA10 -> Text = FloatToStrF((float)aik[16]/4095.0*10.0,ffFixed,5,2);
    // ��������
    EdtTekA11 -> Text = FloatToStrF((float)D_D3/10000*13.3,ffFixed,5,1);
  }
  
  // ����������� ���������� ������������
  for(int i=0;i<PAR_NAGR;i++)
    {
        Zad_Nagr[i]->Caption = IntToStr(int(float(par_n[i])/10.0));
        Tek_Nagr[i]->Caption = IntToStr(int(float(ObjBPN[i]->tekBPN)/10.0));
    }

    // �������� ���������� �� ��������
    davl_pr1 -> Caption = FloatToStrF((float)aik[18]/4095.0*13332,ffExponent,3,8);
    davl_pr2 -> Caption = FloatToStrF((float)aik[19]/4095.0*13332,ffExponent,3,8);
    davl_pr3 -> Caption = FloatToStrF((float)aik[20]/4095.0*133320,ffExponent,3,8);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--����������� ������--//
void TForm1::VisualButtons()
{   // ������� ������
    PnlPVS    -> Font -> Color = SetPopeColor(shr[1]);
    // ������� ����
    PnlRC     -> Font -> Color = SetPopeColor(shr[3]);
    // ���������� ���������
    PnlUstOff   -> Font -> Color = SetPopeColor(shr[7]);
    // ���� �������
    PnlSP       -> Font -> Color = SetPopeColor(shr[6]);
    // ����� ��
    PnlSRC    -> Font -> Color = SetPopeColor(shr[5]);

    /////////////////////////////////////////
    //������� ������ (���.)
    OtkKamOn -> Font -> Color = SetPopeColor(shr[1]);
    //������� ������ (����.)
    OtkKamOff -> Font -> Color = SetPopeColor(shr[7]);
    // ���1 (���.)
    PnlRRG1On   -> Font -> Color = SetPopeColor(shr[20]);
    // ���1 (����.)
    //PnlRRG1Off   -> Font -> Color = SetPopeColor(shr[120]);
    // ���2 (���.)
    PnlRRG2On   -> Font -> Color = SetPopeColor(shr[21]);
    // ���2 (����.)
    //PnlRRG1Off   -> Font -> Color = SetPopeColor(shr[121]);
    // ���3 (���.)
    PnlRRG3On   -> Font -> Color = SetPopeColor(shr[22]);
    // ���3 (����.)
    //PnlRRG3Off   -> Font -> Color = SetPopeColor(shr[122]);
    // ���4 (���.)
    PnlRRG4On   -> Font -> Color = SetPopeColor(shr[23]);
    // ���4 (����.)
    //PnlRRG4Off   -> Font -> Color = SetPopeColor(shr[123]);
    // ���5 (���.)
    PnlRRG5On   -> Font -> Color = SetPopeColor(shr[24]);
    // ���5 (����.)
    //PnlRRG5Off   -> Font -> Color = SetPopeColor(shr[124]);
    // ���6 (���.)
    PnlRRG6On   -> Font -> Color = SetPopeColor(shr[25]);
    // ���6 (����.)
    //PnlRRG6Off   -> Font -> Color = SetPopeColor(shr[125]);
    // ���7 (���.)
    PnlRRG7On   -> Font -> Color = SetPopeColor(shr[26]);
    // ���7 (����.)
    //PnlRRG7Off   -> Font -> Color = SetPopeColor(shr[126]);
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
    // ��� (���)
    VchgOn   -> Font -> Color = SetPopeColor(shr[29]);
    // ��� (����)
    VchgOff   -> Font -> Color = SetPopeColor(shr[39]);
    // ������ �/� (���.)
    PnlPDOn   -> Font -> Color = SetPopeColor(shr[33]);
    // ������ �/� (����.)
    PnlPDOff   -> Font -> Color = SetPopeColor(shr[34]);

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
       ///////////////////////////////////////////////////////////////////////////
    Pnl_KasH -> Font -> Color = SetPopeColor(shr[37]);
    Pnl_KasS-> Font -> Color = SetPopeColor(shr[38]);
    Pnl_PerH -> Font -> Color = SetPopeColor(shr[12]);
    Pnl_PerS -> Font -> Color = SetPopeColor(shr[13]);
    Pnl_PovH -> Font -> Color = SetPopeColor(shr[14]);
    Pnl_PovS -> Font -> Color = SetPopeColor(shr[15]);
    // ���� ����������
    Panel284 -> Font -> Color = SetPopeColor(out[1]&0x02);
    // ����� ������ ����������
    Panel336 -> Font -> Color = SetPopeColor(out[1]&0x08);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ����������� ����--//
void TForm1::VisualVoda()
{     
//���� ���������� ������ �/�
  if(zin[0]&0x01) { PnlKan0->Color=0x00EAD999; }
  else            { PnlKan0->Color=0x003030FF; }
  //���� ���������� ������. ���
  if(zin[0]&0x02) { PnlKan1->Color=0x00EAD999; }
  else            { PnlKan1->Color=0x003030FF; }
  //���� ���������� ���
  if(zin[0]&0x04) { PnlKan2->Color=0x00EAD999; }
  else            { PnlKan2->Color=0x003030FF; }
  //���� ���������� ��
  if(zin[0]&0x08) { PnlKan3->Color=0x00EAD999; }
  else            { PnlKan3->Color=0x003030FF; }
  //���� ���������� �������
  if(zin[0]&0x10) { PnlKan4->Color=0x00EAD999; }
  else            { PnlKan4->Color=0x003030FF; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ����������� ���� ���. ������ �/�
  PnlKan00 -> Caption = FloatToStrF((((float)aik[0]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " �C";
  if(((((float)aik[0]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan00 -> Caption = "0,0 �C"; }
  // ����������� ���� ���������� ��������� ������
  PnlKan01 -> Caption = FloatToStrF((((float)aik[1]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " �C";
  if(((((float)aik[1]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan01 -> Caption = "0,0 �C"; }
  // ����������� ���� ���������� ���
  PnlKan02 -> Caption = FloatToStrF((((float)aik[2]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " �C";
  if(((((float)aik[2]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan02 -> Caption = "0,0 �C"; }
  // ����������� ���� ���������� ��
  PnlKan03 -> Caption = FloatToStrF((((float)aik[3]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " �C";
  if(((((float)aik[3]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan03 -> Caption = "0,0 �C"; }
  // ����������� ���� ���������� �������
  PnlKan04 -> Caption = FloatToStrF((((float)aik[4]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + " �C";
  if(((((float)aik[4]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan04 -> Caption = "0,0 �C"; }
}
//---------------------------------------------------------------------------
//--������������ ��������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualColorElement()
{
anim_fase = !anim_fase;
        // �������
    if((diagn[14])|| (diagn[33]&0x03))
	{
		SetOut(1,1,0x80); //
	}
    else
	{
		SetOut(0,1,0x80); //
	}

    // �����
    if((pr_yel)||(!shr[1]&&!shr[2]&&!shr[3]&&!shr[5]&&!shr[6]&&!shr[7]&&!shr[9]))
	{
		SetOut(1,1,0x40); //
	}
    else
	{
		SetOut(0,1,0x40); //
	}

    // ������
    if(shr[1]||shr[2]||shr[4]||shr[5]||shr[6]||shr[7]||shr[9])
	{
		SetOut(1,1,0x20); //
	}
    else
	{
		SetOut(0,1,0x20); //

	}
coef_zad->Caption = FloatToStrF(1000.0/(float)nasmod[5],ffFixed,5,0);                      // �������� ����������� ������������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(N_TEK_GIR!=0) { coef_tek->Caption = FloatToStrF(1000.0/(float)N_TEK_GIR,ffFixed,5,0); } // ������� ����������� ������������
else             { coef_tek->Caption = 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if((D_D1 >= 0)&&(D_D1 <= 10000)) { davl_D1 -> Caption = FloatToStrF(133.332*pow(10.0,(float)D_D1/1000.0-6.0),ffExponent,3,8); } // ��������� ������� �1
if((D_D2 >= 0)&&(D_D2 <= 10000)) { davl_D2 -> Caption = FloatToStrF(133.332*pow(10.0,(float)D_D2/1000.0-6.0),ffExponent,3,8); } // ��������� ������� �2
if((D_D3 >= 0)&&(D_D3 <= 10000)) { davl_D3 -> Caption = FloatToStrF((float)D_D3/10000*13.3,                 ffFixed,   5,1); } // ��������� ������� �3                                            }
if((D_D4 >= 0)&&(D_D4 <= 10000)) davl_D4 -> Caption=FloatToStrF(100.0*pow(10.0,(float(D_D4)/1000.0-6.8)/0.6),ffExponent,3,8); // ��������� ������� �4

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//����������� ��������
if((shr[40])||(shr[41])){d1 -> Visible = true;} else {d1 -> Visible = false;} //D1
if((shr[42])||(shr[43])){d2 -> Visible = true;} else {d2 -> Visible = false;} //D2
if((shr[84])||(shr[85])){d5_1 -> Visible = true; d5_2 -> Visible = true;} else {d5_1 -> Visible = false; d5_2 -> Visible = false;} //D5
if((shr[48])||(shr[49])){d6_1 -> Visible = true; d6_2 -> Visible = true;} else {d6_1 -> Visible = false; d6_2 -> Visible = false;} //D6
if((shr[50])||(shr[51])){d7 -> Visible = true;} else {d7 -> Visible = false;} //D7
if((shr[52])||(shr[53])){d8 -> Visible = true;} else {d8 -> Visible = false;} //D8
if((shr[54])||(shr[55])){d9 -> Visible = true;} else {d9 -> Visible = false;} //D9
if((shr[56])||(shr[57])){d10 -> Visible = true;} else {d10 -> Visible = false;} //D10
if((shr[58])||(shr[59])){d11 -> Visible = true;} else {d11 -> Visible = false;} //D11
if((shr[60])||(shr[61])){d12 -> Visible = true;} else {d12 -> Visible = false;} //D12
if((shr[62])||(shr[63])){d13 -> Visible = true;} else {d13 -> Visible = false;} //D13
if((shr[64])||(shr[65])){d14 -> Visible = true;} else {d14 -> Visible = false;} //D14
if((shr[66])||(shr[67])){d15 -> Visible = true;} else {d15 -> Visible = false;} //D15
if((shr[68])||(shr[69])){d16 -> Visible = true;} else {d16 -> Visible = false;} //D16
if((shr[70])||(shr[71])){d17 -> Visible = true;} else {d17 -> Visible = false;} //D17
if((shr[72])||(shr[73])){d18 -> Visible = true;} else {d18 -> Visible = false;} //D18
if((shr[74])||(shr[75])){d19 -> Visible = true;} else {d19 -> Visible = false;} //D19
if((shr[76])||(shr[77])){d20 -> Visible = true;} else {d20 -> Visible = false;} //D20
if((shr[78])||(shr[79])){d21 -> Visible = true;} else {d21 -> Visible = false;} //D21
if((shr[80])||(shr[81])){d23 -> Visible = true;} else {d23 -> Visible = false;} //D23
if((shr[82])||(shr[83])){d24 -> Visible = true;} else {d24 -> Visible = false;} //D24

    d1s -> Visible = d1 -> Visible;
    d2s -> Visible = d2 -> Visible;
    d5s -> Visible = d5_1 -> Visible;
    d6s -> Visible = d6_1 -> Visible;
    d7s -> Visible = d7 -> Visible;
    d8s -> Visible = d8 -> Visible;
    d9s -> Visible = d9 -> Visible;
    d10s -> Visible = d10 -> Visible;
    d11s -> Visible = d11 -> Visible;
    d12s -> Visible = d12 -> Visible;
    d13s -> Visible = d13 -> Visible;
    d14s -> Visible = d14 -> Visible;
    d15s -> Visible = d15 -> Visible;
    d16s -> Visible = d16 -> Visible;
    d17s -> Visible = d17 -> Visible;
    d18s -> Visible = d18 -> Visible;
    d19s -> Visible = d19 -> Visible;
    d20s -> Visible = d20 -> Visible;
    d21s -> Visible = d21 -> Visible;
    d23s -> Visible = d23 -> Visible;
    d24s -> Visible = d24 -> Visible;


    //�����
////////////////////////////////////////////////////////////////////////////////
if((shr[20])||((out[2]&0x01)&&(tube_5->Visible)))tube_10->Visible=true;
else tube_10->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if((shr[21])||((out[2]&0x02)&&(tube_5->Visible)))tube_9->Visible=true;
else tube_9->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if((shr[22])||((out[2]&0x04)&&(tube_5->Visible)))tube_8->Visible=true;
else tube_8->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if(((shr[23])&&(tube_7->Visible))||((out[2]&0x08)&&(tube_5->Visible)))tube_27->Visible=true;
else tube_27->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if(((shr[24])&&(tube_6->Visible))||((out[2]&0x10)&&(tube_5->Visible)))tube_28->Visible=true;
else tube_28->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if((out[2]&0x80)||(out[2]&0x200))tube_7->Visible=true;
else tube_7->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if((out[2]&0x100)||(out[2]&0x400))tube_6->Visible=true;
else tube_6->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (
     ((out[2]&0x01)&&(shr[20]))||
     ((out[2]&0x02)&&(shr[21]))||
     ((out[2]&0x04)&&(shr[22]))||
     ((out[2]&0x08)&&(shr[23]))||
     ((out[2]&0x10)&&(shr[24]))||
     ((out[3]&0x04)&&(tube_29->Visible))
    )
    tube_5->Visible=true;
else tube_5->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if (tube_15->Visible)
    tube_1->Visible=true;
else
    tube_1->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if ((out[2]&0x1000)&&(tube_16->Visible))
    tube_2->Visible=true;
else
    tube_2->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if ((out[2]&0x800)&&(tube_34->Visible))
    tube_3->Visible=true;
else
    tube_3->Visible=false;
////////////////////////////////////////////////////////////////////////////////

if  (out[4]&0x1000)
    tube_11->Visible=true;
else
    tube_11->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  ((shr[26])||((out[2]&0x40)&&(tube_15->Visible)))
    tube_12->Visible=true;
else
    tube_12->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  ((shr[25])||((out[2]&0x20)&&(tube_15->Visible)))
    tube_13->Visible=true;
else
    tube_13->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if (
    ((out[3]&0x08)&&(tube_19->Visible))||
    ((out[2]&0x20)&&(tube_13->Visible))||
    ((out[3]&0x10)&&(tube_29->Visible))||
    ((out[3]&0x800)&&(tube_33->Visible))
    )
    tube_4->Visible=true;
else
    tube_4->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if(out[3]&0x01)
    tube_14->Visible=true;
else
    tube_14->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if(
    ((out[2]&0x40)&&(tube_12->Visible))||
    ((out[3]&0x400)&&(tube_29->Visible))
  )
    tube_15->Visible=true;
else
    tube_15->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (
    ((out[3]&0x02)&&(tube_5->Visible))||
    ((out[3]&0x08)&&(tube_4->Visible))
    )
    tube_19->Visible=true;
else
    tube_19->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (zin[0]&0x4000)
    tube_21->Visible=true;
else
    tube_21->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if (out[4]&0x400)
    tube_22->Visible=true;
else
    tube_22->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  ((zin[0]&0x1000)&&(tube_29->Visible))
    tube_23->Visible=true;
else
    tube_23->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (((zin[0]&0x400)&&(tube_29->Visible))||
    ((zin[1]&0x100)&&(tube_26->Visible))
    )
    tube_24->Visible=true;
else
    tube_24->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (zin[1]&0x10)
    tube_25->Visible=true;
else
    tube_25->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (((zin[1]&0x100)&&(tube_24->Visible))||
    (tube_23->Visible))
    tube_26->Visible=true;
else
    tube_26->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (
    (out[2]&0x4000)||
    (zin[2]&0x02)||
    ((zin[0]&0x400)&&(tube_24->Visible)&&(zin[1]&0x10)) ||
    ((out[3]&0x04)&&(tube_5->Visible)&&((shr[20])||(shr[21])||(shr[22])||(shr[23])||(shr[24])))      ||
    ((out[3]&0x400)&&(tube_15->Visible)&&(shr[5]))    ||
    ((out[3]&0x10)&&(tube_4->Visible)&&(shr[6]))      ||
    ((zin[0]&0x1000)&&(tube_23->Visible)&&(zin[1]&0x10))
    )
    tube_29->Visible=true;
else
    tube_29->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  ((!(zin[1]&0x02))&&(tube_23->Visible))
    tube_30->Visible=true;
else
    tube_30->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (tube_15->Visible)
    tube_32->Visible=true;
else
    tube_32->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if ((out[3]&0x800)&&(tube_4->Visible))
    {tube_16->Visible=true;
    tube_17->Visible=true;
    tube_31->Visible=true;
    tube_33->Visible=true; }

else
    {tube_16->Visible=false;
    tube_17->Visible=false;
    tube_31->Visible=false;
    tube_33->Visible=false; }
////////////////////////////////////////////////////////////////////////////////
if  ((out[3]&0x20)&&(tube_4->Visible))
    {tube_34->Visible=true;
    tube_35->Visible=true;
    tube_36->Visible=true;
    tube_37->Visible=true;}
else
    {tube_34->Visible=false;
    tube_35->Visible=false;
    tube_36->Visible=false;
    tube_37->Visible=false;}
////////////////////////////////////////////////////////////////////////////////
if((zin[2]&0x01)||((out[0]&0x03)&&(tube_shl->Visible)))
    tube_fvn_shl->Visible=true;
else
    tube_fvn_shl->Visible=false;
////////////////////////////////////////////////////////////////////////////////
if  (
    (out[4]&0x808)||
    ((out[0]&0x03)&&(tube_fvn_shl->Visible))
    )
    tube_shl->Visible=true;
else
    tube_shl->Visible=false;
////////////////////////////////////////////////////////////////////////////////
//�������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[20])  rrg1 -> Visible = true;     // ���1
  else         rrg1 -> Visible = false;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x01)  // VP1
   vp1 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
    vp1 -> Picture->Bitmap = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[21])  rrg2 -> Visible = true;      // ���2
  else         rrg2 -> Visible = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x02)  // VP2
    vp2 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
   vp2 -> Picture->Bitmap = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[22])  rrg3 -> Visible = true;      // ���3
  else         rrg3 -> Visible = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x04)  // VP3
   vp3 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
   vp3 -> Picture->Bitmap = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[23])  rrg4 -> Visible = true;   // ���4
  else         rrg4 -> Visible = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x08)  // VP4
   vp6 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
   vp6 -> Picture->Bitmap = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[24])  rrg5 -> Visible = true;    // ���5
  else         rrg5 -> Visible = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x10)  // VP7
    vp7 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
    vp7 -> Picture->Bitmap = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[25])  rrg6 -> Visible = true;    // ���6
  else         rrg6 -> Visible = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x20)  // VP10
   vp10 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
   vp10 -> Picture->Bitmap = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[26])  rrg7 -> Visible = true;    // ���7
  else         rrg7 -> Visible = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x40)  // VP11
   vp11 -> Picture->Bitmap = rrg_white->Picture->Bitmap;
  else
   vp11 -> Picture->Bitmap = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    switch(zin[0]&0xC000)  // ��-�4
  { case 0x0000: { kl_d4->Picture->Bitmap = rrg_grey->Picture->Bitmap;   break; }; // ������������
    case 0x4000: { kl_d4->Picture->Bitmap = rrg_white->Picture->Bitmap;  break; }; // ������
    case 0x8000: { kl_d4->Picture->Bitmap = 0;                           break; }; // ������
    case 0xC000: { kl_d4->Picture->Bitmap = rrg_grey->Picture->Bitmap;   break; }; // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[4]&0x400)  kl_nap1->Picture->Bitmap = rrg_white->Picture->Bitmap;    // ��-���-1
  else             kl_nap1->Picture->Bitmap = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[4]&0x08)  kl_nap2->Picture->Bitmap = rrg_white->Picture->Bitmap;   // ��-���-2
  else             kl_nap2->Picture->Bitmap = 0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch(zin[0]&0x300)  // ��-��
  { case 0x000: { fk_shl2->Picture->Bitmap = rrg_grey->Picture->Bitmap; break; } // ������������
    case 0x100: { fk_shl2->Picture->Bitmap = rrg_white->Picture->Bitmap; break; } // ������
    case 0x200: { fk_shl2->Picture->Bitmap = 0;                             break; } // ������
    case 0x300: { fk_shl2->Picture->Bitmap = rrg_grey->Picture->Bitmap; break; } // ������������
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[2]&0x01) { fn_shl->Picture->Bitmap = fn_kam_white->Picture->Bitmap;   } // �������� �����
  else            { fn_shl->Picture->Bitmap = 0;                          }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[3]&0x20) { datchik1->Visible=true;  } // ���� ��������� � �����
  else            { datchik1->Visible=false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(zin[3]&0x40) { datchik2->Visible=true;  } //���� ��������� � ���
  else            { datchik2->Visible=false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //����� �����
  if(zin[3]&0x4000) { cap -> Visible = true;  }
  else              { cap -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //�������� ����� ��������
  if(zin[4]&0x4000) { sensor_l -> Visible = true;  }
  else              { sensor_l -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //�������� ����� �������
  if(zin[4]&0x8000) { sensor_r -> Visible = true;  }
  else              { sensor_r -> Visible = false; }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(PR_KLASTER)
  {
        switch(KOM_MOD & 0x300) //������� ������
        {
                case 0x00: { door->Picture->Bitmap = door_grey->Picture->Bitmap; break; } // ������������
		case 0x100: { door->Picture->Bitmap = 0;                      break; } // ������
		case 0x200: { door->Picture->Bitmap = door_green->Picture->Bitmap; break; } // ������
		case 0x300: { door->Picture->Bitmap = door_grey->Picture->Bitmap; break; } // ������������
        }
  }
  else
  {
	switch(zin[1]&0x0C) //������� ������
	{ case 0x00: { door->Picture->Bitmap = door_grey->Picture->Bitmap; break; } // ������������
		case 0x04: { door->Picture->Bitmap = 0;                      break; } // ������
		case 0x08: { door->Picture->Bitmap = door_green->Picture->Bitmap; break; } // ������
		case 0x0c: { door->Picture->Bitmap = door_grey->Picture->Bitmap; break; } // ������������
	}
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch(zin[1]&0xC00) //������ ��
  { case 0x000: { zatvor->Picture->Bitmap = zatvor_grey->Picture->Bitmap; break; } // ������������
    case 0x400: { zatvor->Picture->Bitmap = 0;                       break; } // ������
    case 0x800: { zatvor->Picture->Bitmap = zatvor_green->Picture->Bitmap; break; } // ������
    case 0xC00: { zatvor->Picture->Bitmap = zatvor_grey->Picture->Bitmap; break; } // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�������� ������
  if(!(diagn[14]&0x02))//��� ������
  {if(zin[2]&0x10) //��� ��������������
  {if(zin[2]&0x02){fn_kam->Picture->Bitmap = fn_kam_white->Picture->Bitmap;} //����� �������
  else{fn_kam->Picture->Bitmap = 0;}} //���� ����� �� �������
  else{fn_kam->Picture->Bitmap = fn_kam_yellow->Picture->Bitmap;}} //������ ��������������
  else{fn_kam->Picture->Bitmap = fn_kam_red->Picture->Bitmap;} //������ ������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x4000) { vp13->Picture->Bitmap = rrg_v_white->Picture->Bitmap; }  // VP13
  else            { vp13->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x80) { vp4->Picture->Bitmap = rrg_white->Picture->Bitmap;  } // vp4
  else            { vp4->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x200) { vp8->Picture->Bitmap = rrg_white->Picture->Bitmap;  } // vp8
  else            { vp8->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x100) { vp5->Picture->Bitmap = rrg_white->Picture->Bitmap;  } // vp5
  else            { vp5->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x400) { vp9->Picture->Bitmap = rrg_white->Picture->Bitmap;  } // vp9
  else            { vp9->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[4]&0x1000) {ve1->Picture->Bitmap = rrg_white->Picture->Bitmap;   } // VE1
  else              {ve1->Picture->Bitmap = 0; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x08) {vp18->Picture->Bitmap = rrg_v_white->Picture->Bitmap;} // VP18
  else            {vp18->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x02) {vp19->Picture->Bitmap = rrg_v_white->Picture->Bitmap;} // VP19
  else            {vp19->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x4000) {vp13->Picture->Bitmap = rrg_v_white->Picture->Bitmap;} // VP13
  else              {vp13->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x04) {vp20->Picture->Bitmap = rrg_v_white->Picture->Bitmap;} // VP20
  else            {vp20->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x400) {vp21->Picture->Bitmap = rrg_v_white->Picture->Bitmap;} // VP21
  else             {vp21->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x10) {vp22->Picture->Bitmap = rrg_v_white->Picture->Bitmap;} // VP22
  else            {vp22->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x800){vp17->Picture->Bitmap = rrg_white->Picture->Bitmap;} // VP17
  else            {vp17->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[2]&0x1000){vp15->Picture->Bitmap = rrg_white->Picture->Bitmap;} // VP15
  else           {vp15->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
switch(zin[0]&0x3000)  // ��-���
  { case 0x0000: { fk_tmn->Picture->Bitmap = rrg_grey->Picture->Bitmap;   break; }; // ������������
    case 0x1000: { fk_tmn->Picture->Bitmap = rrg_white->Picture->Bitmap;  break; }; // ������
    case 0x2000: { fk_tmn->Picture->Bitmap = 0;                           break; }; // ������
    case 0x3000: { fk_tmn->Picture->Bitmap = rrg_grey->Picture->Bitmap;   break; }; // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 switch(zin[1]&0x03)  // ��-�2
  { case 0x00: { kl_d2->Picture->Bitmap = rrg_grey->Picture->Bitmap;   break; }; // ������������
    case 0x01: { kl_d2->Picture->Bitmap = rrg_white->Picture->Bitmap;  break; }; // ������
    case 0x02: { kl_d2->Picture->Bitmap = 0;                           break; }; // ������
    case 0x03: { kl_d2->Picture->Bitmap = rrg_grey->Picture->Bitmap;   break; }; // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
switch(zin[0]&0xC00)  // ��-���
  { case 0x000: { fk_kam->Picture->Bitmap = rrg_grey->Picture->Bitmap; break; }; // ������������
    case 0x400: { fk_kam->Picture->Bitmap = rrg_white->Picture->Bitmap;break; }; // ������
    case 0x800: { fk_kam->Picture->Bitmap = 0; break; }; // ������
    case 0xC00: { fk_kam->Picture->Bitmap = rrg_grey->Picture->Bitmap; break; }; // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���
  if(zin[2]&0x200)       { tmn->Picture->Bitmap=tmn_red->Picture->Bitmap;    } // ������
  else if(zin[2]&0x4000) { tmn->Picture->Bitmap=tmn_yellow->Picture->Bitmap; } // ��������������
  else if(zin[2]&0x1400) // ������/����������
  { if(anim_fase) { tmn->Picture->Bitmap=tmn_white->Picture->Bitmap; }
    else          { tmn->Picture->Bitmap = 0;                        }
  }
  else if(zin[2]&0x800)  { tmn->Picture->Bitmap=tmn_white->Picture->Bitmap; }  // �����
  else                   { tmn->Picture->Bitmap = 0;                        }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//����������
  if(zin[2]&0x400) { str_down -> Visible = true;  }
  else             { str_down -> Visible = false; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //������
  if(zin[2]&0x1000) { str_up -> Visible = true;  }
  else              { str_up -> Visible = false; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
switch(zin[1]&0x300)  // ������ ������
  { case 0x000: { zatv->Picture->Bitmap = rrg_v_grey->Picture->Bitmap; break; }; // ������������
    case 0x100: { zatv->Picture->Bitmap = rrg_v_white->Picture->Bitmap;break; }; // ������
    case 0x200: { zatv->Picture->Bitmap = 0; break; }; // ������
    case 0x300: { zatv->Picture->Bitmap = rrg_v_grey->Picture->Bitmap; break; }; // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[17])
  zasl->Picture->Bitmap = dross->Picture->Bitmap;
  else
  {
  switch(zin[1]&0x30) // ��
  { case 0x00: { zasl->Picture->Bitmap = zasl_grey->Picture->Bitmap;    break; } // ������������
    case 0x10: { zasl->Picture->Bitmap = zasl_white->Picture->Bitmap;   break; } // ������
    case 0x20: { zasl->Picture->Bitmap = 0;                             break; } // ������
    case 0x30: { zasl->Picture->Bitmap = zasl_grey->Picture->Bitmap;    break; } // ������������
  }
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[64]||shr[65]) { warm->Visible = true;  } //������ ��������
  else        { warm->Visible = false; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch(zin[1]&0xC0) // �����(��������� ����������)
  { case 0x00: { pin_null->Picture->Bitmap = pin_grey ->Picture->Bitmap;                    break; } // ������������
    case 0x40: { pin_null->Picture->Bitmap = pin_green->Picture->Bitmap; pin_null->Top=554; break; } // ��������� ������
    case 0x80: { pin_null->Picture->Bitmap = pin_green->Picture->Bitmap; pin_null->Top=572; break; } // ��������� �����
    case 0xC0: { pin_null->Picture->Bitmap = pin_grey ->Picture->Bitmap;                    break; } // ������������
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x01) {vp23->Picture->Bitmap = rrg_v_green->Picture->Bitmap; } // VP23
  else            {vp23->Picture->Bitmap = 0; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x20) {vp16->Picture->Bitmap = rrg_white->Picture->Bitmap;} // VP16
  else {vp16->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x800) {vp14->Picture->Bitmap = rrg_white->Picture->Bitmap;} // VP14
  else {vp14->Picture->Bitmap = 0;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[3]&0x1000){arrow_10->Visible=true;} // VP28
  else           {arrow_10->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[3]&0x8000){arrow_9->Visible=true;} // VP41
  else           {arrow_9->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[3]&0x2000){arrow_8->Visible=true;} // VP29
  else           {arrow_8->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[3]&0x4000){arrow_7->Visible=true;} // VP40
  else           {arrow_7->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[2]&0x8000){arrow_6->Visible=true;} // VP39
  else           {arrow_6->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x40) {arrow_5->Visible=true;} // VP24
  else            {arrow_5->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x80) {arrow_4->Visible=true;} // VP26
  else            {arrow_4->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x100) {arrow_3->Visible=true;} // VP25
  else             {arrow_3->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[3]&0x200) {arrow_2->Visible=true;} // VP27
  else             {arrow_2->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(out[2]&0x2000) {arrow_1->Visible=true;} // VP12
  else              {arrow_1->Visible=false;}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[0]&0x02) fk_shl1->Picture->Bitmap = rrg_white->Picture->Bitmap;
else            fk_shl1->Picture->Bitmap = 0;       //��-�� (������ �������)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(out[4]&0x800) kl_nap3->Picture->Bitmap = rrg_white->Picture->Bitmap;//��-���3
else            kl_nap3->Picture->Bitmap = 0;
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
  if(shr[29])
    vcg->Picture->Bitmap =vcg_on->Picture->Bitmap;
  else
    vcg->Picture->Bitmap =vcg_off->Picture->Bitmap;

    // �������� ������
  if(VRGIR)
  {
    if(anim_fase) plazma->Picture->Bitmap = plazma1_e->Picture->Bitmap;
    else plazma->Picture->Bitmap = plazma2_e->Picture->Bitmap;
  }
  else plazma->Picture->Bitmap = 0;
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� ������ �������--//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetZin(bool value, unsigned char outNmb, unsigned int outMask)
{   // ���������� �������� ������ ���������
    value ? zin[outNmb] |= outMask : zin[outNmb] &= (~outMask);
}
//---------------------------------------------------------------------------

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

///////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[0] c �������--//
void __fastcall TForm1::ButtonOT0Click(TObject *Sender)
{   if(CheckBoxZ0_0->Checked) { zin[0] |= 0x0001;    }
    else                      { zin[0] &= (~0x0001); }
    if(CheckBoxZ0_1->Checked) { zin[0] |= 0x0002;    }
    else                      { zin[0] &= (~0x0002); }
    if(CheckBoxZ0_2->Checked) { zin[0] |= 0x0004;    }
    else                      { zin[0] &= (~0x0004); }
    if(CheckBoxZ0_3->Checked) { zin[0] |= 0x0008;    }
    else                      { zin[0] &= (~0x0008); }
    if(CheckBoxZ0_4->Checked) { zin[0] |= 0x0010;    }
    else                      { zin[0] &= (~0x0010); }
    if(CheckBoxZ0_5->Checked) { zin[0] |= 0x0020;    }
    else                      { zin[0] &= (~0x0020); }
    if(CheckBoxZ0_6->Checked) { zin[0] |= 0x0040;    }
    else                      { zin[0] &= (~0x0040); }
    if(CheckBoxZ0_7->Checked) { zin[0] |= 0x0080;    }
    else                      { zin[0] &= (~0x0080); }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[0] c �������--//
void __fastcall TForm1::Button3Click(TObject *Sender)
{   if(CheckBoxZ0_8 ->Checked) { zin[0] |= 0x0100;    }
    else                       { zin[0] &= (~0x0100); }
    if(CheckBoxZ0_9 ->Checked) { zin[0] |= 0x0200;    }
    else                       { zin[0] &= (~0x0200); }
    if(CheckBoxZ0_10->Checked) { zin[0] |= 0x0400;    }
    else                       { zin[0] &= (~0x0400); }
    if(CheckBoxZ0_11->Checked) { zin[0] |= 0x0800;    }
    else                       { zin[0] &= (~0x0800); }
    if(CheckBoxZ0_12->Checked) { zin[0] |= 0x1000;    }
    else                       { zin[0] &= (~0x1000); }
    if(CheckBoxZ0_13->Checked) { zin[0] |= 0x2000;    }
    else                       { zin[0] &= (~0x2000); }
    if(CheckBoxZ0_14->Checked) { zin[0] |= 0x4000;    }
    else                       { zin[0] &= (~0x4000); }
    if(CheckBoxZ0_15->Checked) { zin[0] |= 0x8000;    }
    else                       { zin[0] &= (~0x8000); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[1] c �������--//
void __fastcall TForm1::ButtonOT1Click(TObject *Sender)
{   if(CheckBoxZ1_0->Checked) { zin[1] |= 0x0001;    }
    else                      { zin[1] &= (~0x0001); }
    if(CheckBoxZ1_1->Checked) { zin[1] |= 0x0002;    }
    else                      { zin[1] &= (~0x0002); }
    if(CheckBoxZ1_2->Checked) { zin[1] |= 0x0004;    }
    else                      { zin[1] &= (~0x0004); }
    if(CheckBoxZ1_3->Checked) { zin[1] |= 0x0008;    }
    else                      { zin[1] &= (~0x0008); }
    if(CheckBoxZ1_4->Checked) { zin[1] |= 0x0010;    }
    else                      { zin[1] &= (~0x0010); }
    if(CheckBoxZ1_5->Checked) { zin[1] |= 0x0020;    }
    else                      { zin[1] &= (~0x0020); }
    if(CheckBoxZ1_6->Checked) { zin[1] |= 0x0040;    }
    else                      { zin[1] &= (~0x0040); }
    if(CheckBoxZ1_7->Checked) { zin[1] |= 0x0080;    }
    else                      { zin[1] &= (~0x0080); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[1] c �������--//
void __fastcall TForm1::Button9Click(TObject *Sender)
{   if(CheckBoxZ1_8 ->Checked) { zin[1] |= 0x0100;    }
    else                       { zin[1] &= (~0x0100); }
    if(CheckBoxZ1_9 ->Checked) { zin[1] |= 0x0200;    }
    else                       { zin[1] &= (~0x0200); }
    if(CheckBoxZ1_10->Checked) { zin[1] |= 0x0400;    }
    else                       { zin[1] &= (~0x0400); }
    if(CheckBoxZ1_11->Checked) { zin[1] |= 0x0800;    }
    else                       { zin[1] &= (~0x0800); }
    if(CheckBoxZ1_12->Checked) { zin[1] |= 0x1000;    }
    else                       { zin[1] &= (~0x1000); }
    if(CheckBoxZ1_13->Checked) { zin[1] |= 0x2000;    }
    else                       { zin[1] &= (~0x2000); }
    if(CheckBoxZ1_14->Checked) { zin[1] |= 0x4000;    }
    else                       { zin[1] &= (~0x4000); }
    if(CheckBoxZ1_15->Checked) { zin[1] |= 0x8000;    }
    else                       { zin[1] &= (~0x8000); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[2] c �������--//
void __fastcall TForm1::Button55Click(TObject *Sender)
{   if(CheckBoxZ2_0->Checked) { zin[2] |= 0x0001;    }
    else                      { zin[2] &= (~0x0001); }
    if(CheckBoxZ2_1->Checked) { zin[2] |= 0x0002;    }
    else                      { zin[2] &= (~0x0002); }
    if(CheckBoxZ2_2->Checked) { zin[2] |= 0x0004;    }
    else                      { zin[2] &= (~0x0004); }
    if(CheckBoxZ2_3->Checked) { zin[2] |= 0x0008;    }
    else                      { zin[2] &= (~0x0008); }
    if(CheckBoxZ2_4->Checked) { zin[2] |= 0x0010;    }
    else                      { zin[2] &= (~0x0010); }
    if(CheckBoxZ2_5->Checked) { zin[2] |= 0x0020;    }
    else                      { zin[2] &= (~0x0020); }
    if(CheckBoxZ2_6->Checked) { zin[2] |= 0x0040;    }
    else                      { zin[2] &= (~0x0040); }
    if(CheckBoxZ2_7->Checked) { zin[2] |= 0x0080;    }
    else                      { zin[2] &= (~0x0080); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[2] c �������--//
void __fastcall TForm1::Button2Click(TObject *Sender)
{   if(CheckBoxZ2_8 ->Checked) { zin[2] |= 0x0100;    }
    else                       { zin[2] &= (~0x0100); }
    if(CheckBoxZ2_9 ->Checked) { zin[2] |= 0x0200;    }
    else                       { zin[2] &= (~0x0200); }
    if(CheckBoxZ2_10->Checked) { zin[2] |= 0x0400;    }
    else                       { zin[2] &= (~0x0400); }
    if(CheckBoxZ2_11->Checked) { zin[2] |= 0x0800;    }
    else                       { zin[2] &= (~0x0800); }
    if(CheckBoxZ2_12->Checked) { zin[2] |= 0x1000;    }
    else                       { zin[2] &= (~0x1000); }
    if(CheckBoxZ2_13->Checked) { zin[2] |= 0x2000;    }
    else                       { zin[2] &= (~0x2000); }
    if(CheckBoxZ2_14->Checked) { zin[2] |= 0x4000;    }
    else                       { zin[2] &= (~0x4000); }
    if(CheckBoxZ2_15->Checked) { zin[2] |= 0x8000;    }
    else                       { zin[2] &= (~0x8000); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[3] c �������--//
void __fastcall TForm1::ButtonOT3Click(TObject *Sender)
{   if(CheckBoxZ3_0->Checked) { zin[3] |= 0x0001;    }
    else                      { zin[3] &= (~0x0001); }
    if(CheckBoxZ3_1->Checked) { zin[3] |= 0x0002;    }
    else                      { zin[3] &= (~0x0002); }
    if(CheckBoxZ3_2->Checked) { zin[3] |= 0x0004;    }
    else                      { zin[3] &= (~0x0004); }
    if(CheckBoxZ3_3->Checked) { zin[3] |= 0x0008;    }
    else                      { zin[3] &= (~0x0008); }
    if(CheckBoxZ3_4->Checked) { zin[3] |= 0x0010;    }
    else                      { zin[3] &= (~0x0010); }
    if(CheckBoxZ3_5->Checked) { zin[3] |= 0x0020;    }
    else                      { zin[3] &= (~0x0020); }
    if(CheckBoxZ3_6->Checked) { zin[3] |= 0x0040;    }
    else                      { zin[3] &= (~0x0040); }
    if(CheckBoxZ3_7->Checked) { zin[3] |= 0x0080;    }
    else                      { zin[3] &= (~0x0080); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[3] c �������--//
void __fastcall TForm1::ButtonOT32Click(TObject *Sender)
{   if(CheckBoxZ3_8 ->Checked) { zin[3] |= 0x0100;    }
    else                       { zin[3] &= (~0x0100); }
    if(CheckBoxZ3_9 ->Checked) { zin[3] |= 0x0200;    }
    else                       { zin[3] &= (~0x0200); }
    if(CheckBoxZ3_10->Checked) { zin[3] |= 0x0400;    }
    else                       { zin[3] &= (~0x0400); }
    if(CheckBoxZ3_11->Checked) { zin[3] |= 0x0800;    }
    else                       { zin[3] &= (~0x0800); }
    if(CheckBoxZ3_12->Checked) { zin[3] |= 0x1000;    }
    else                       { zin[3] &= (~0x1000); }
    if(CheckBoxZ3_13->Checked) { zin[3] |= 0x2000;    }
    else                       { zin[3] &= (~0x2000); }
    if(CheckBoxZ3_14->Checked) { zin[3] |= 0x4000;    }
    else                       { zin[3] &= (~0x4000); }
    if(CheckBoxZ3_15->Checked) { zin[3] |= 0x8000;    }
    else                       { zin[3] &= (~0x8000); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[4] c �������--//
void __fastcall TForm1::ButtonOT4Click(TObject *Sender)
{   if(CheckBoxZ4_0->Checked) { zin[4] |= 0x0001;    }
    else                      { zin[4] &= (~0x0001); }
    if(CheckBoxZ4_1->Checked) { zin[4] |= 0x0002;    }
    else                      { zin[4] &= (~0x0002); }
    if(CheckBoxZ4_2->Checked) { zin[4] |= 0x0004;    }
    else                      { zin[4] &= (~0x0004); }
    if(CheckBoxZ4_3->Checked) { zin[4] |= 0x0008;    }
    else                      { zin[4] &= (~0x0008); }
    if(CheckBoxZ4_4->Checked) { zin[4] |= 0x0010;    }
    else                      { zin[4] &= (~0x0010); }
    if(CheckBoxZ4_5->Checked) { zin[4] |= 0x0020;    }
    else                      { zin[4] &= (~0x0020); }
    if(CheckBoxZ4_6->Checked) { zin[4] |= 0x0040;    }
    else                      { zin[4] &= (~0x0040); }
    if(CheckBoxZ4_7->Checked) { zin[4] |= 0x0080;    }
    else                      { zin[4] &= (~0x0080); }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� zin[4] c �������--//
void __fastcall TForm1::ButtonOT42Click(TObject *Sender)
{   if(CheckBoxZ4_8 ->Checked) { zin[4] |= 0x0100;    }
    else                       { zin[4] &= (~0x0100); }
    if(CheckBoxZ4_9 ->Checked) { zin[4] |= 0x0200;    }
    else                       { zin[4] &= (~0x0200); }
    if(CheckBoxZ4_10->Checked) { zin[4] |= 0x0400;    }
    else                       { zin[4] &= (~0x0400); }
    if(CheckBoxZ4_11->Checked) { zin[4] |= 0x0800;    }
    else                       { zin[4] &= (~0x0800); }
    if(CheckBoxZ4_12->Checked) { zin[4] |= 0x1000;    }
    else                       { zin[4] &= (~0x1000); }
    if(CheckBoxZ4_13->Checked) { zin[4] |= 0x2000;    }
    else                       { zin[4] &= (~0x2000); }
    if(CheckBoxZ4_14->Checked) { zin[4] |= 0x4000;    }
    else                       { zin[4] &= (~0x4000); }
    if(CheckBoxZ4_15->Checked) { zin[4] |= 0x8000;    }
    else                       { zin[4] &= (~0x8000); }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_250Timer(TObject *Sender)
{
   VisualFormat();
   VisualDebug();
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
    if(((TEdit*)Sender)->Name == "BtnAutoDa")
    {

        // �������� �������� �� ���������� � ����������� ������
        for(int j=1;j<StringGrid1->RowCount-1;j++)
            for(int i=0;i<PAR_COUNT;i++)
            {
                if(j <= Memo_receipt->Lines->Count)
                    par[j][i] = par_temp[j][i];
                else
                    par[j][i] = 0;
                // ������ �������
            }
        for(int i=0;i<Memo_receipt->Lines->Count;i++)
        par[i][20] = StrToInt( EdtARed20->Text );

        VisualParA();
    }
    DrawEditColor(EdtAKon20,EdtARed20);

    PanelParA->Visible = false;
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

//--------------------
//-------������ �����-
//--------------------

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
if  (
            (((TEdit*)Sender)->Name == "EdtTRed1" )||
            (((TEdit*)Sender)->Name == "EdtTRed2" )||
            (((TEdit*)Sender)->Name == "EdtTRed3" )||
            (((TEdit*)Sender)->Name == "EdtTRed4" )||
            (((TEdit*)Sender)->Name == "EdtTRed5" )
        )
    {
        // // ���-�� ������ ����� ������� 0
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
// ������ ���1
    if  (((TEdit*)Sender)->Name == "EdtRRed0_0" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 18.0)
        {
            valueText = 18.0;
            ((TEdit*)Sender)->Color = clYellow;
        }}
        // ������ ���2
    if  (((TEdit*)Sender)->Name == "EdtRRed0_1" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 9.0)
        {
            valueText = 9.0;
            ((TEdit*)Sender)->Color = clYellow;
        }}

        // ������ ���3
    if  (((TEdit*)Sender)->Name == "EdtRRed0_2" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 9.0)
        {
            valueText = 9.0;
            ((TEdit*)Sender)->Color = clYellow;
        }}

        // ������ ���4
    if  (((TEdit*)Sender)->Name == "EdtRRed0_3" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 3.6)
        {
            valueText = 3.6;
            ((TEdit*)Sender)->Color = clYellow;
        }}

    // ������ ���5
    if  (((TEdit*)Sender)->Name == "EdtRRed0_4" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 9.0)
        {
            valueText = 9.0;
            ((TEdit*)Sender)->Color = clYellow;
        }}

    // ������ ���6
    if  (((TEdit*)Sender)->Name == "EdtRRed0_5" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 18.0)
        {
            valueText = 18.0;
            ((TEdit*)Sender)->Color = clYellow;
        }}

    // ������ ���7
    if  (((TEdit*)Sender)->Name == "EdtRRed0_6" )
        { // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 18.0)
        {
            valueText = 18.0;
            ((TEdit*)Sender)->Color = clYellow;
        }}

        // ��������
    if  (((TEdit*)Sender)->Name == "EdtRRed0_10" )
        { // ���-�� ������ ����� ������� 1
        format = 2;
        // ��������� �� �������
        if (valueText < 1.0)
        {
            valueText = 1.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 13.3)
        {
            valueText = 13.3;
            ((TEdit*)Sender)->Color = clYellow;
        }
        }

    // �������� ��
    if  (((TEdit*)Sender)->Name == "EdtRRed0_7" )
        { // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > CESAR_MAX_IP)
        {
            valueText = CESAR_MAX_IP;
            ((TEdit*)Sender)->Color = clYellow;
        }
        }

   



//���������

  if  (((TEdit*)Sender)->Name == "EditNastrTo3" )
        { // ���-�� ������ ����� ������� 1
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
        }}

    if  (((TEdit*)Sender)->Name == "EditNastrTo4" )
        { // ���-�� ������ ����� ������� 1
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
        }}
       if  (((TEdit*)Sender)->Name == "EditNastrTo5" )
        { // ���-�� ������ ����� ������� 1
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
        }}

    // �������� �� ���������
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 5, format);

    VisualParA();
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
par[0][0] = StrToFloat( EdtRRed0_0 -> Text )* 4095.0 / 18.0;
//������ ���2
par[0][1] = StrToFloat( EdtRRed0_1 -> Text )* 4095.0 / 9.0;
//������ ���3
par[0][2] = StrToFloat( EdtRRed0_2 -> Text )* 4095.0 / 9.0;
//������ ���4
par[0][3] = StrToFloat( EdtRRed0_3 -> Text )* 4095.0 / 3.6;
//������ ���5
par[0][4] = StrToFloat( EdtRRed0_4 -> Text )* 4095.0 / 9.0;
//������ ���6
par[0][5] = StrToFloat( EdtRRed0_5 -> Text )* 4095.0 / 18.0;
//������ ���7
par[0][6] = StrToFloat( EdtRRed0_6 -> Text )* 4095.0 / 18.0;
//�������� ��
par[0][7] = StrToFloat( EdtRRed0_7 -> Text )* 4095.0 / CESAR_MAX_IP;
//��������
par[0][10] = StrToFloat(EdtRRed0_10->Text)*10000.0/13.3;
//����������� ������������
par[0][15] = StrToFloat( EdtRRed0_15 -> Text );
//������� ������������
par[0][17] = StrToFloat( EdtRRed0_17 -> Text );
//����������� �������
par[0][18] = StrToFloat( EdtRRed0_18 -> Text );
//�������� ��������
par[0][16] = EdtRRed0_16 -> ItemIndex;
//���������� �������
par[0][20]=StrToInt( EdtARed20->Text );
MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ���������� ���������:");

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
    if ( EdtRKon0_7 -> Text != EdtRRed0_7 -> Text )
        MemoStat -> Lines -> Add("�������� ��: " + EdtRKon0_7 -> Text + " -> " + EdtRRed0_7 -> Text );
    if ( EdtRKon0_10 -> Text != EdtRRed0_10 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtRKon0_10 -> Text + " -> " + EdtRRed0_10 -> Text );
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
    EdtRRed0_7 -> Color = clWhite;
    EdtRRed0_10 -> Color = clWhite;
    EdtRRed0_15 -> Color = clWhite;
    EdtRRed0_17 -> Color = clWhite;
    EdtRRed0_18 -> Color = clWhite;
    EdtRRed0_16 -> Color = clWhite;
    // �������� ��������
    VisualParR();
}
//---------------------------------------------------------------------------
//--������������ ���������� �������--//
//---------------------------------------------------------------------------
void TForm1::VisualParR()
{
	//������ ���1
    EdtRKon0_0 -> Text = FloatToStrF((float)par[0][0]/4095.0*18.0, ffFixed, 5, 1);
	// ������ ���2
    EdtRKon0_1 -> Text = FloatToStrF((float)par[0][1]/4095.0*9.0, ffFixed, 5, 1);
	// ������ ���3
    EdtRKon0_2 -> Text = FloatToStrF((float)par[0][2]/4095.0*9.0, ffFixed, 5, 1);
	// ������ ���4
    EdtRKon0_3 -> Text = FloatToStrF((float)par[0][3]/4095.0*3.6, ffFixed, 5, 1);
    // ������ ���5
    EdtRKon0_4 -> Text = FloatToStrF((float)par[0][4]/4095.0*9.0, ffFixed, 5, 1);
    // ������ ���6
    EdtRKon0_5 -> Text = FloatToStrF((float)par[0][5]/4095.0*18.0, ffFixed, 5, 1);
    // ������ ���7
    EdtRKon0_6 -> Text = FloatToStrF((float)par[0][6]/4095.0*18.0, ffFixed, 5, 1);
    // �������� ��
    EdtRKon0_7 -> Text = FloatToStrF((float)par[0][7]/4095.0*CESAR_MAX_IP, ffFixed, 5, 0);
	// ��������
    EdtRKon0_10 -> Text = FloatToStrF((float)par[0][10]/10000.0*13.3, ffFixed, 5, 2);

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
{   // ��� �� ������������ ���� ������
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
    EditOTLzad24-> Text = "";
    EditOTLzad25-> Text = "";
    EditOTLzad26-> Text = "";
    EditOTLzad27-> Text = "";
    EditOTLzad28-> Text = "";
    EditOTLzad29-> Text = "";
    EditOTLzad30-> Text = "";
}
//---------------------------------------------------------------------------
void TForm1::VisualDebug()
{
    // ������������ zin[0]
    unsigned int mask = 0x01;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[0] & mask )
             Form1 -> StringGridZl0 -> Cells[0][i] = "1";
        else Form1 -> StringGridZl0 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x0100;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[0] & mask )
             Form1 -> StringGridZh0 -> Cells[0][i] = "1";
        else Form1 -> StringGridZh0 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x01;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[1] & mask )
             Form1 -> StringGridZl1 -> Cells[0][i] = "1";
        else Form1 -> StringGridZl1 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x100;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[1] & mask )
             Form1 -> StringGridZh1 -> Cells[0][i] = "1";
        else Form1 -> StringGridZh1 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x01;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[2] & mask )
             Form1 -> StringGridZl2 -> Cells[0][i] = "1";
        else Form1 -> StringGridZl2 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x100;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[2] & mask )
             Form1 -> StringGridZh2 -> Cells[0][i] = "1";
        else Form1 -> StringGridZh2 -> Cells[0][i] = "0";
        mask <<= 1;
    }
        mask = 0x01;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[3] & mask )
             Form1 -> StringGridZl3 -> Cells[0][i] = "1";
        else Form1 -> StringGridZl3 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x100;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[3] & mask )
             Form1 -> StringGridZh3 -> Cells[0][i] = "1";
        else Form1 -> StringGridZh3 -> Cells[0][i] = "0";
        mask <<= 1;
    }
        mask = 0x01;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[4] & mask )
             Form1 -> StringGridZl4 -> Cells[0][i] = "1";
        else Form1 -> StringGridZl4 -> Cells[0][i] = "0";
        mask <<= 1;
    }
    mask = 0x100;
    for ( int i = 0 ; i < 8 ; i ++ )
    {
        if ( zin[4] & mask )
             Form1 -> StringGridZh4 -> Cells[0][i] = "1";
        else Form1 -> StringGridZh4 -> Cells[0][i] = "0";
        mask <<= 1;
    }
      /////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
    AnsiString valuesNames[] =
    {
        "sh_",	  //0
        "norma",
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
         //1
        "shr[15]",
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
		"shr[29]",
		"sh[29]",
		"shr[30]",
		"sh[30]",
		"shr[31]",
		"sh[31]",
		"shr[32]",
		"sh[32]",
        //2
        "shr[33]",
		"sh[33]",
		"shr[34]",
		"sh[34]",
		"shr[37]",
		"sh[37]",
        "shr[38]",
		"sh[38]",
		"shr[40]",
		"sh[40]",
		"shr[41]",
		"sh[41]",
		"shr[42]",
		"sh[42]",
		"shr[43]",
		"sh[43]",
		"shr[44]",
		"sh[44]",
		"shr[45]",
		"sh[45]",
		"shr[46]",
		"sh[46]",
		"shr[47]",
		"sh[47]",
		"shr[48]",
		"sh[48]",
		"shr[49]",
		"sh[49]",
		"shr[50]",
		"sh[50]",

         //3
        "shr[51]",
		"sh[51]",
        "shr[52]",
		"sh[52]",
		"shr[53]",
		"sh[53]",
		"shr[54]",	
		"sh[54]",
		"shr[55]",
		"sh[55]",
		"shr[56]",
		"sh[56]",
		"shr[57]",
		"sh[57]",
		"shr[58]",
		"sh[58]",
		"shr[59]",
		"sh[59]",
		"shr[60]",
		"sh[60]",
		"shr[61]",
		"sh[61]",
		"shr[62]",
		"sh[62]",
		"shr[63]",
		"sh[63]",
		"shr[64]",
		"sh[64]",
		"shr[65]",
		"sh[65]",

        //4
        "shr[66]",
		"sh[66]",
        "shr[67]",
		"sh[67]",
		"shr[68]",
		"sh[68]",
		"shr[69]",	
		"sh[69]",
		"shr[70]",
		"sh[70]",
		"shr[71]",
		"sh[71]",
		"shr[72]",
		"sh[72]",
		"shr[73]",
		"sh[73]",
		"shr[74]",
		"sh[74]",
		"shr[75]",
		"sh[75]",
		"shr[76]",
		"sh[76]",
		"shr[77]",
		"sh[77]",
		"shr[78]",
		"sh[78]",
		"shr[79]",
		"sh[79]",
		"shr[80]",
		"sh[80]",


		//5
        "shr[81]",
		"sh[81]",
		"shr[82]",
		"sh[82]",
		"shr[83]",
		"sh[83]",
        "shr[84]",
		"sh[84]",
        "shr[85]",
		"sh[85]",
        "",
		"diagn[0]",
		"diagn[1]",
		"diagn[2]",
		"diagn[3]",
		"diagn[4]",
		"diagn[5]",
		"diagn[6]",
		"diagn[7]",
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
		



		//6

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
		"diagn[29]",
		"diagn[30]",
		"diagn[31]",
		"diagn[32]",
		"diagn[33]",
		"diagn[34]",
		"diagn[35]",
		"diagnS[0]",
		"diagnS[1]",
        "diagnS[2]",
		"out[0]",
		"out[1]",
		"out[2]",
		"out[3]",
		"out[4]",
		"zin[0]",
		"zin[1]",
		"zin[2]",
		"zin[3]",
		"zin[4]",
		
		
		
		"aik[0]",	//7
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
		"aik[19]",
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
		
		//8

		"aout[0]",
		"aout[1]",
		"aout[2]",
		"aout[3]",
		"aout[4]",
		"aout[5]",
		"aout[6]",
		"aout[7]",
		"aout[8]",
		"aout[9]",
		"aout[10]",
		"aout[11]",
		"aout[12]",
		"",
        "",			
		"",
		"D_D1",
		"D_D2",
		"D_D3",
		"D_D4",
		"",
		"UVAK_KAM",
		"POROG_DAVL",
		"UVAKV_KAM",
		"UVAKN_KAM",
		"UVAK_SHL_MO",
		"UVAK_SHL_MN",
		"UVAK_ZTMN",
		"UATM_D1",
		"UATM_D4",
		
		"nasmod[0]",	//9
		"nasmod[1]",
		"nasmod[2]",
		"nasmod[3]",
		"nasmod[4]",
		"nasmod[5]",
		"nasmod[6]",
		"nasmod[7]",
		"nasmod[17]",
		"nasmod[18]",
		"",
		"",
		"par[0][0]",
		"par[0][1]",
		"par[0][2]",
		"par[0][3]",
		"par[0][4]",
        "par[0][5]",
        "par[0][6]",
        "par[0][7]",
        "par[0][8]",
        "par[0][9]",
        "par[0][10]",
        "par[0][11]",
        "par[0][12]",
        "par[0][13]",
        "par[0][15]",
        "par[0][16]",
        "par[0][17]",
        "par[0][18]",
		
		"par[1][0]",	//10
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
        "",
        "",
        "par[2][0]",
		"par[2][1]",
		"par[2][2]",  
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
		
        "par[3][0]",	//11
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
        "",
        "par[4][0]",
		"par[4][1]",
		"par[4][2]",  
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
		"",
        
		"par[5][0]",	//12
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
        "",
        "par[6][0]",
		"par[6][1]",
		"par[6][2]",  
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
		"",
       
	    "par[7][0]",	//13
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
        "",
        "par[8][0]",
		"par[8][1]",
		"par[8][2]",  
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
		"",
		
		"par[9][0]",	//14
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
        "",
		"par[10][0]",
		"par[10][1]",
		"par[10][2]",
        "par[10][3]",
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
		"",
		
		"par_n[0]",	//15
		"par_n[1]",
		"par_n[2]",  
        "par_n[3]",
        "par_n[4]",
        "par_n[5]",
        "par_n[6]",
        "par_n[7]",
        "par_n[8]",
        "par_n[9]",
        "par_n[10]",
        "par_n[11]",
        "par_n[12]",
        "par_n[13]",
        "par_n[14]",
        "par_n[15]",
		"par_n[16]",
		"par_n[17]",  
        "par_n[18]",
        "par_n[19]",
        "par_n[20]",
        "par_n[21]",
        "par_n[22]",
        "par_n[23]",
        "par_t[0]",
	    "par_t[1]",
	    "par_t[2]",
	    "par_t[3]",
	    "par_t[4]",
	    "par_t[5]",
	
		
		"T_ZAD_DVS",	//16
		"T_PROC",
		"T_KTMN_RAZGON",
		"T_KKAM_V",
		"T_VODA",
		"T_STOP",
		"T_DVIJ",
		"T_KDVIJ_SU",
		"T_KSUT",
		"T_KKAM",
		"T_KTMN",
		"T_KSHL",
		"T_KNAP",
		"T_NAPUSK",
		"T_VHG",
        "T_KSHL_MO",
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
		"",
		"",
		"",


		"CT_VHG",	//17
		"CT_VODA_IP",
		"",
		"",
		"",
		"CT_TEMP",
		"CT_DVIJ_GIR_g",
		"CT_DVIJ_GIR_t",
		"CT_SUT_g",
		"CT_SUT_t",
		"",
		"",
		"",
		"",
		"",
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
		"CT_29",
		"CT29K1",
		"CT_30T",

        "PR_KLASTER",	//18
		"otvet",
		"zshr3",
		"PR_TRTEST",
		"PR_OTK",
		"PR_FK_KAM",
		"PR_NASOS",
		"PR_NALADKA",
		"N_PL",
		"N_ST_MAX",
		"N_ST",
		"N_ZICL",
		"ZN_ST",
		"",
		"KOM_MOD",
		"OTVET_MOD",
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
		"",
		"",
		"",
		
		"DATA_DZASL",	//19
        "PAR_DZASL",   	   
        "ZPAR_DZASL",    
        "X_TDZASL",
        "VRDZASL",   
        "E_TDZASL",
        "DELDZASL",      
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

		"prDvijGir_g",	//20
		"prDvijGir_t",
		"DOP_SU",
		"T_SM_NAPR",
		"DOP_DV_IP",
		"klGir_gV",
		"klGir_gN",
		"klGir_tV",
		"klGir_tN",
		"VRGIR",
		"K_SOGL_GIR",
		"NAPRS_GIR",
		"X_TGIR",
		"E_TGIR",
		"DELGIR",
		"DOPGIR",
		"PAR_GIR",
		"N_TEK_GIR",
		"LIM1GIR",
		"LIM2GIR",
		"T_VRGIR",
		"T_KGIR",
		"N_PRED_GIR",
		"",
		"",
		"",
		"",
		"",
		"",
		"",

        "PR_PER",	//21
		"KOM_PER",
		"OTVET_PER",
		"V_PER",
		"HOME_PER",
		"TEK_OTN_PER",
		"TEK_ABS_PER",
		"PUT_PER",
		"CT_PER",
		"",
		"",
		"PR_KAS",
		"KOM_KAS",
		"OTVET_KAS",
		"V_KAS",
		"HOME_KAS",
		"TEK_OTN_KAS",
		"TEK_ABS_KAS",
		"PUT_KAS",
		"CT_KAS",
		"",
		"PR_POV",
		"KOM_POV",
		"OTVET_POV",
		"V_POV",
		"HOME_POV",
		"TEK_OTN_POV",
		"TEK_ABS_POV",
		"PUT_POV",
		"CT_POV",

		"PR_BPN1",	//22
		"VBPN1",
		"ZAD_TEMP1",
		"TEK_TEMP1",
        "PR_BPN2",
		"VBPN2",
		"ZAD_TEMP2",
		"TEK_TEMP2",
        "PR_BPN5",
		"VBPN5",
		"ZAD_TEMP5",
		"TEK_TEMP5",
		"PR_BPN6",
		"VBPN6",
		"ZAD_TEMP6",
		"TEK_TEMP6",
        "PR_BPN7",
		"VBPN7",
		"ZAD_TEMP7",
		"TEK_TEMP7",
        "PR_BPN8",
		"VBPN8",
		"ZAD_TEMP8",
		"TEK_TEMP8",
        "PR_BPN9",
		"VBPN9",
        "ZAD_TEMP9",
		"TEK_TEMP9",
		
		"",
		"",
        //23
        "PR_BPN10",
		"VBPN10",
		"ZAD_TEMP10",
		"TEK_TEMP10",
        "PR_BPN11",
		"VBPN11",
		"ZAD_TEMP11",
		"TEK_TEMP11",
        "PR_BPN12",
		"VBPN12",
		"ZAD_TEMP12",
		"TEK_TEMP12",
        "PR_BPN13",
		"VBPN13",
		"ZAD_TEMP13",
		"TEK_TEMP13",
        "PR_BPN14",
		"VBPN14",
		"ZAD_TEMP14",
		"TEK_TEMP14",
        "PR_BPN15",
		"VBPN15",
		"ZAD_TEMP15",
		"TEK_TEMP15",
        "PR_BPN16",
        "VBPN16",
		"ZAD_TEMP16",
		"TEK_TEMP16",

		"",
		"",
        //24
        "PR_BPN17",
        "VBPN17",
		"ZAD_TEMP17",
		"TEK_TEMP17",
        "PR_BPN18",
		"VBPN18",
		"ZAD_TEMP18",
		"TEK_TEMP18",
		"PR_BPN19",
		"VBPN19",
		"ZAD_TEMP19",
		"TEK_TEMP19",
		"PR_BPN20",
		"VBPN20",
		"ZAD_TEMP20",
		"TEK_TEMP20",
		"PR_BPN21",
		"VBPN21",
		"ZAD_TEMP21",
		"TEK_TEMP21",
		"PR_BPN22",
        "VBPN22",
		"ZAD_TEMP22",
		"TEK_TEMP22",
		"PR_BPN23",
		"VBPN23",
		"ZAD_TEMP23",
		"TEK_TEMP23",

        "",
		"",
        //25
        "PR_BPN24",
		"VBPN24",
		"ZAD_TEMP24",
		"TEK_TEMP24",
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
		"",
		"",
        "",
		""
         

};
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
	// ������� ����� �������� ��������
    unsigned char pageCount = StrToInt ( PCPerem -> ActivePage -> Hint );
    // ��������� ����� ���������� �� ��������
    for ( unsigned int i = 30 * pageCount; i < ( 30 * ( pageCount + 1  ) ); i++)
        EdtDebugValues[i%30] -> Text = valuesNames[i];

	// ��������� �������� ���������� �� ��������
	switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
    {   case 0:  // 0 ��������
        {   EditOTLtek1  -> Text = IntToStr(sh_);
            EditOTLtek2  -> Text = IntToStr(norma);
            EditOTLtek3  -> Text = IntToStr(shr[1]);
            EditOTLtek4  -> Text = IntToStr(sh[1]);
            EditOTLtek5  -> Text = IntToStr(shr[2]);
            EditOTLtek6  -> Text = IntToStr(sh[2]);
            EditOTLtek7  -> Text = IntToStr(shr[3]);
            EditOTLtek8  -> Text = IntToStr(sh[3]);
            EditOTLtek9  -> Text = IntToStr(shr[4]);
            EditOTLtek10 -> Text = IntToStr(sh[4]);
            EditOTLtek11 -> Text = IntToStr(shr[5]);
            EditOTLtek12 -> Text = IntToStr(sh[5]);
            EditOTLtek13 -> Text = IntToStr(shr[6]);
            EditOTLtek14 -> Text = IntToStr(sh[6]);
            EditOTLtek15 -> Text = IntToStr(shr[7]);
            EditOTLtek16 -> Text = IntToStr(sh[7]);
            EditOTLtek17 -> Text = IntToStr(shr[8]);
            EditOTLtek18 -> Text = IntToStr(sh[8]);
            EditOTLtek19 -> Text = IntToStr(shr[9]);
            EditOTLtek20 -> Text = IntToStr(sh[9]);
			EditOTLtek21 -> Text = IntToStr(shr[10]);
            EditOTLtek22 -> Text = IntToStr(sh[10]);
            EditOTLtek23 -> Text = IntToStr(shr[11]);
            EditOTLtek24 -> Text = IntToStr(sh[11]);
            EditOTLtek25 -> Text = IntToStr(shr[12]);
            EditOTLtek26 -> Text = IntToStr(sh[12]);
            EditOTLtek27 -> Text = IntToStr(shr[13]);
            EditOTLtek28 -> Text = IntToStr(sh[13]);
            EditOTLtek29 -> Text = IntToStr(shr[14]);
            EditOTLtek30 -> Text = IntToStr(sh[14]);
        }; break;
        case 1:  // 1 ��������
        {   EditOTLtek1  -> Text = IntToStr(shr[15]);
            EditOTLtek2  -> Text = IntToStr(sh[15]);
            EditOTLtek3  -> Text = IntToStr(shr[17]);
            EditOTLtek4  -> Text = IntToStr(sh[17]);
            EditOTLtek5  -> Text = IntToStr(shr[18]);
            EditOTLtek6  -> Text = IntToStr(sh[18]);
            EditOTLtek7  -> Text = IntToStr(shr[19]);
            EditOTLtek8  -> Text = IntToStr(sh[19]);
            EditOTLtek9  -> Text = IntToStr(shr[20]);
            EditOTLtek10 -> Text = IntToStr(sh[20]);
            EditOTLtek11 -> Text = IntToStr(shr[21]);
            EditOTLtek12 -> Text = IntToStr(sh[21]);
            EditOTLtek13 -> Text = IntToStr(shr[22]);
            EditOTLtek14 -> Text = IntToStr(sh[22]);
            EditOTLtek15 -> Text = IntToStr(shr[23]);
            EditOTLtek16 -> Text = IntToStr(sh[23]);
            EditOTLtek17 -> Text = IntToStr(shr[24]);
            EditOTLtek18 -> Text = IntToStr(sh[24]);
            EditOTLtek19 -> Text = IntToStr(shr[25]);
            EditOTLtek20 -> Text = IntToStr(sh[25]);
			EditOTLtek21 -> Text = IntToStr(shr[26]);
            EditOTLtek22 -> Text = IntToStr(sh[26]);
            EditOTLtek23 -> Text = IntToStr(shr[29]);
            EditOTLtek24 -> Text = IntToStr(sh[29]);
            EditOTLtek25 -> Text = IntToStr(shr[30]);
            EditOTLtek26 -> Text = IntToStr(sh[30]);
            EditOTLtek27 -> Text = IntToStr(shr[31]);
            EditOTLtek28 -> Text = IntToStr(sh[31]);
            EditOTLtek29 -> Text = IntToStr(shr[32]);
            EditOTLtek30 -> Text = IntToStr(sh[32]);
        }; break;
        case 2:  // 2 ��������
        {   EditOTLtek1  -> Text = IntToStr(shr[33]);
            EditOTLtek2  -> Text = IntToStr(sh[33]);
            EditOTLtek3  -> Text = IntToStr(shr[34]);
            EditOTLtek4  -> Text = IntToStr(sh[34]);
            EditOTLtek5  -> Text = IntToStr(shr[37]);
            EditOTLtek6  -> Text = IntToStr(sh[37]);
            EditOTLtek7  -> Text = IntToStr(shr[38]);
            EditOTLtek8  -> Text = IntToStr(sh[38]);
            EditOTLtek9  -> Text = IntToStr(shr[40]);
            EditOTLtek10 -> Text = IntToStr(sh[40]);
            EditOTLtek11 -> Text = IntToStr(shr[41]);
            EditOTLtek12 -> Text = IntToStr(sh[41]);
            EditOTLtek13 -> Text = IntToStr(shr[42]);
            EditOTLtek14 -> Text = IntToStr(sh[42]);
            EditOTLtek15 -> Text = IntToStr(shr[43]);
            EditOTLtek16 -> Text = IntToStr(sh[43]);
            EditOTLtek17 -> Text = IntToStr(shr[44]);
            EditOTLtek18 -> Text = IntToStr(sh[44]);
            EditOTLtek19 -> Text = IntToStr(shr[45]);
            EditOTLtek20 -> Text = IntToStr(sh[45]);
			EditOTLtek21 -> Text = IntToStr(shr[46]);
            EditOTLtek22 -> Text = IntToStr(sh[46]);
            EditOTLtek23 -> Text = IntToStr(shr[47]);
            EditOTLtek24 -> Text = IntToStr(sh[47]);
            EditOTLtek25 -> Text = IntToStr(shr[48]);
            EditOTLtek26 -> Text = IntToStr(sh[48]);
            EditOTLtek27 -> Text = IntToStr(shr[49]);
            EditOTLtek28 -> Text = IntToStr(sh[49]);
            EditOTLtek29 -> Text = IntToStr(shr[50]);
            EditOTLtek30 -> Text = IntToStr(sh[50]);
        }; break;
        case 3:  // 3 ��������
        {   EditOTLtek1  -> Text = IntToStr(shr[51]);
            EditOTLtek2  -> Text = IntToStr(sh[51]);
            EditOTLtek3  -> Text = IntToStr(shr[52]);
            EditOTLtek4  -> Text = IntToStr(sh[52]);
            EditOTLtek5  -> Text = IntToStr(shr[53]);
            EditOTLtek6  -> Text = IntToStr(sh[53]);
            EditOTLtek7  -> Text = IntToStr(shr[54]);
            EditOTLtek8  -> Text = IntToStr(sh[54]);
            EditOTLtek9  -> Text = IntToStr(shr[55]);
            EditOTLtek10 -> Text = IntToStr(sh[55]);
            EditOTLtek11 -> Text = IntToStr(shr[56]);
            EditOTLtek12 -> Text = IntToStr(sh[56]);
            EditOTLtek13 -> Text = IntToStr(shr[57]);
            EditOTLtek14 -> Text = IntToStr(sh[57]);
            EditOTLtek15 -> Text = IntToStr(shr[58]);
            EditOTLtek16 -> Text = IntToStr(sh[58]);
            EditOTLtek17 -> Text = IntToStr(shr[59]);
            EditOTLtek18 -> Text = IntToStr(sh[59]);
            EditOTLtek19 -> Text = IntToStr(shr[60]);
            EditOTLtek20 -> Text = IntToStr(sh[60]);
			EditOTLtek21 -> Text = IntToStr(shr[61]);
            EditOTLtek22 -> Text = IntToStr(sh[61]);
            EditOTLtek23 -> Text = IntToStr(shr[62]);
            EditOTLtek24 -> Text = IntToStr(sh[62]);
            EditOTLtek25 -> Text = IntToStr(shr[63]);
            EditOTLtek26 -> Text = IntToStr(sh[63]);
            EditOTLtek27 -> Text = IntToStr(shr[64]);
            EditOTLtek28 -> Text = IntToStr(sh[64]);
            EditOTLtek29 -> Text = IntToStr(shr[65]);
            EditOTLtek30 -> Text = IntToStr(sh[65]);
        }; break;
		case 4:  // 4 ��������
        {   EditOTLtek1  -> Text = IntToStr(shr[66]);
            EditOTLtek2  -> Text = IntToStr(sh[66]);
            EditOTLtek3  -> Text = IntToStr(shr[67]);
            EditOTLtek4  -> Text = IntToStr(sh[67]);
            EditOTLtek5  -> Text = IntToStr(shr[68]);
            EditOTLtek6  -> Text = IntToStr(sh[68]);
            EditOTLtek7  -> Text = IntToStr(shr[69]);
            EditOTLtek8  -> Text = IntToStr(sh[69]);
            EditOTLtek9  -> Text = IntToStr(shr[70]);
            EditOTLtek10 -> Text = IntToStr(sh[70]);
            EditOTLtek11 -> Text = IntToStr(shr[71]);
            EditOTLtek12 -> Text = IntToStr(sh[71]);
            EditOTLtek13 -> Text = IntToStr(shr[72]);
            EditOTLtek14 -> Text = IntToStr(sh[72]);
            EditOTLtek15 -> Text = IntToStr(shr[73]);
            EditOTLtek16 -> Text = IntToStr(sh[73]);
            EditOTLtek17 -> Text = IntToStr(shr[74]);
            EditOTLtek18 -> Text = IntToStr(sh[74]);
            EditOTLtek19 -> Text = IntToStr(shr[75]);
            EditOTLtek20 -> Text = IntToStr(sh[75]);
			EditOTLtek21 -> Text = IntToStr(shr[76]);
            EditOTLtek22 -> Text = IntToStr(sh[76]);
            EditOTLtek23 -> Text = IntToStr(shr[77]);
            EditOTLtek24 -> Text = IntToStr(sh[77]);
            EditOTLtek25 -> Text = IntToStr(shr[78]);
            EditOTLtek26 -> Text = IntToStr(sh[78]);
            EditOTLtek27 -> Text = IntToStr(shr[79]);
            EditOTLtek28 -> Text = IntToStr(sh[79]);
            EditOTLtek29 -> Text = IntToStr(shr[80]);
            EditOTLtek30 -> Text = IntToStr(sh[80]);
        }; break;
        case 5:  // 5 ��������
        {   EditOTLtek1  -> Text = IntToStr(shr[81]);
            EditOTLtek2  -> Text = IntToStr(sh[81]);
            EditOTLtek3  -> Text = IntToStr(shr[82]);
            EditOTLtek4  -> Text = IntToStr(sh[82]);
            EditOTLtek5  -> Text = IntToStr(shr[83]);
            EditOTLtek6  -> Text = IntToStr(sh[83]);
            EditOTLtek7  -> Text = IntToStr(shr[84]);
            EditOTLtek8  -> Text = IntToStr(sh[84]);
            EditOTLtek9  -> Text = IntToStr(shr[85]);
            EditOTLtek10 -> Text = IntToStr(sh[85]);
            EditOTLtek11 -> Text = IntToStr(0);
            EditOTLtek12 -> Text = IntToStr(diagn[0]);
            EditOTLtek13 -> Text = IntToStr(diagn[1]);
            EditOTLtek14 -> Text = IntToStr(diagn[2]);
            EditOTLtek15 -> Text = IntToStr(diagn[3]);
            EditOTLtek16 -> Text = IntToStr(diagn[4]);
            EditOTLtek17 -> Text = IntToStr(diagn[5]);
            EditOTLtek18 -> Text = IntToStr(diagn[6]);
            EditOTLtek19 -> Text = IntToStr(diagn[7]);
            EditOTLtek20 -> Text = IntToStr(diagn[8]);
			EditOTLtek21 -> Text = IntToStr(diagn[9]);
            EditOTLtek22 -> Text = IntToStr(diagn[10]);
            EditOTLtek23 -> Text = IntToStr(diagn[11]);
            EditOTLtek24 -> Text = IntToStr(diagn[12]);
            EditOTLtek25 -> Text = IntToStr(diagn[13]);
            EditOTLtek26 -> Text = IntToStr(diagn[14]);
            EditOTLtek27 -> Text = IntToStr(diagn[15]);
            EditOTLtek28 -> Text = IntToStr(diagn[16]);
            EditOTLtek29 -> Text = IntToStr(diagn[17]);
            EditOTLtek30 -> Text = IntToStr(diagn[18]);
        }; break;

        case 6:  // 6 ��������
        {   EditOTLtek1  -> Text = IntToStr(diagn[19]);
            EditOTLtek2  -> Text = IntToStr(diagn[20]);
            EditOTLtek3  -> Text = IntToStr(diagn[21]);
            EditOTLtek4  -> Text = IntToStr(diagn[22]);
            EditOTLtek5  -> Text = IntToStr(diagn[23]);
            EditOTLtek6  -> Text = IntToStr(diagn[24]);
            EditOTLtek7  -> Text = IntToStr(diagn[25]);
            EditOTLtek8  -> Text = IntToStr(diagn[26]);
            EditOTLtek9  -> Text = IntToStr(diagn[27]);
            EditOTLtek10 -> Text = IntToStr(diagn[28]);
            EditOTLtek11 -> Text = IntToStr(diagn[29]);
            EditOTLtek12 -> Text = IntToStr(diagn[30]);
            EditOTLtek13 -> Text = IntToStr(diagn[31]);
            EditOTLtek14 -> Text = IntToStr(diagn[32]);
            EditOTLtek15 -> Text = IntToStr(diagn[33]);
            EditOTLtek16 -> Text = IntToStr(diagn[34]);
            EditOTLtek17 -> Text = IntToStr(diagn[35]);
            EditOTLtek18 -> Text = IntToStr(diagnS[0]);
            EditOTLtek19 -> Text = IntToStr(diagnS[1]);
            EditOTLtek20 -> Text = IntToStr(diagnS[2]);
			EditOTLtek21 -> Text = IntToStr(out[0]);
            EditOTLtek22 -> Text = IntToStr(out[1]);
            EditOTLtek23 -> Text = IntToStr(out[2]);
            EditOTLtek24 -> Text = IntToStr(out[3]);
            EditOTLtek25 -> Text = IntToStr(out[4]);
            EditOTLtek26 -> Text = IntToStr(zin[0]);
            EditOTLtek27 -> Text = IntToStr(zin[1]);
            EditOTLtek28 -> Text = IntToStr(zin[2]);
            EditOTLtek29 -> Text = IntToStr(zin[3]);
            EditOTLtek30 -> Text = IntToStr(zin[4]);
        }; break;
		 case 7:  // 7 ��������
        {   EditOTLtek1  -> Text = IntToStr(aik[0]);
            EditOTLtek2  -> Text = IntToStr(aik[1]);
            EditOTLtek3  -> Text = IntToStr(aik[2]);
            EditOTLtek4  -> Text = IntToStr(aik[3]);
            EditOTLtek5  -> Text = IntToStr(aik[4]);
            EditOTLtek6  -> Text = IntToStr(aik[5]);
            EditOTLtek7  -> Text = IntToStr(aik[6]);
            EditOTLtek8  -> Text = IntToStr(aik[7]);
            EditOTLtek9  -> Text = IntToStr(aik[8]);
            EditOTLtek10 -> Text = IntToStr(aik[9]);
            EditOTLtek11 -> Text = IntToStr(aik[10]);
            EditOTLtek12 -> Text = IntToStr(aik[11]);
            EditOTLtek13 -> Text = IntToStr(aik[12]);
            EditOTLtek14 -> Text = IntToStr(aik[13]);
            EditOTLtek15 -> Text = IntToStr(aik[14]);
            EditOTLtek16 -> Text = IntToStr(aik[15]);
            EditOTLtek17 -> Text = IntToStr(aik[16]);
            EditOTLtek18 -> Text = IntToStr(aik[17]);
            EditOTLtek19 -> Text = IntToStr(aik[18]);
            EditOTLtek20 -> Text = IntToStr(aik[19]);
			EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(0);
            EditOTLtek23 -> Text = IntToStr(0);
            EditOTLtek24 -> Text = IntToStr(0);
            EditOTLtek25 -> Text = IntToStr(0);
            EditOTLtek26 -> Text = IntToStr(0);
            EditOTLtek27 -> Text = IntToStr(0);
            EditOTLtek28 -> Text = IntToStr(0);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 8:  // 8 ��������
        {   EditOTLtek1  -> Text = IntToStr(aout[0]);
            EditOTLtek2  -> Text = IntToStr(aout[1]);
            EditOTLtek3  -> Text = IntToStr(aout[2]);
            EditOTLtek4  -> Text = IntToStr(aout[3]);
            EditOTLtek5  -> Text = IntToStr(aout[4]);
            EditOTLtek6  -> Text = IntToStr(aout[5]);
            EditOTLtek7  -> Text = IntToStr(aout[6]);
            EditOTLtek8  -> Text = IntToStr(aout[7]);
            EditOTLtek9  -> Text = IntToStr(aout[8]);
            EditOTLtek10 -> Text = IntToStr(aout[9]);
            EditOTLtek11 -> Text = IntToStr(aout[10]);
            EditOTLtek12 -> Text = IntToStr(aout[11]);
            EditOTLtek13 -> Text = IntToStr(aout[12]);
            EditOTLtek14 -> Text = IntToStr(0);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(0);
            EditOTLtek17 -> Text = IntToStr(D_D1);
            EditOTLtek18 -> Text = IntToStr(D_D2);
            EditOTLtek19 -> Text = IntToStr(D_D3);
            EditOTLtek20 -> Text = IntToStr(D_D4);
			EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(UVAK_KAM);
            EditOTLtek23 -> Text = IntToStr(POROG_DAVL);
            EditOTLtek24 -> Text = IntToStr(UVAKV_KAM);
            EditOTLtek25 -> Text = IntToStr(UVAKN_KAM);
            EditOTLtek26 -> Text = IntToStr(UVAK_SHL_MO);
            EditOTLtek27 -> Text = IntToStr(UVAK_SHL_MN);
            EditOTLtek28 -> Text = IntToStr(UVAK_ZTMN);
            EditOTLtek29 -> Text = IntToStr(UATM_D1);
            EditOTLtek30 -> Text = IntToStr(UATM_D4);
        }; break;
        case 9:  // 9 ��������
        {   EditOTLtek1  -> Text = IntToStr(nasmod[0]);
            EditOTLtek2  -> Text = IntToStr(nasmod[1]);
            EditOTLtek3  -> Text = IntToStr(nasmod[2]);
            EditOTLtek4  -> Text = IntToStr(nasmod[3]);
            EditOTLtek5  -> Text = IntToStr(nasmod[4]);
            EditOTLtek6  -> Text = IntToStr(nasmod[5]);
            EditOTLtek7  -> Text = IntToStr(nasmod[6]);
            EditOTLtek8  -> Text = IntToStr(nasmod[7]);
            EditOTLtek9  -> Text = IntToStr(nasmod[17]);
            EditOTLtek10 -> Text = IntToStr(nasmod[18]);
            EditOTLtek11 -> Text = IntToStr(0);
            EditOTLtek12 -> Text = IntToStr(0);
            EditOTLtek13 -> Text = IntToStr(par[0][0]);
            EditOTLtek14 -> Text = IntToStr(par[0][1]);
            EditOTLtek15 -> Text = IntToStr(par[0][2]);
            EditOTLtek16 -> Text = IntToStr(par[0][3]);
            EditOTLtek17 -> Text = IntToStr(par[0][4]);
            EditOTLtek18 -> Text = IntToStr(par[0][5]);
            EditOTLtek19 -> Text = IntToStr(par[0][6]);
            EditOTLtek20 -> Text = IntToStr(par[0][7]);
            EditOTLtek21 -> Text = IntToStr(par[0][8]);
            EditOTLtek22 -> Text = IntToStr(par[0][9]);
            EditOTLtek23 -> Text = IntToStr(par[0][10]);
            EditOTLtek24 -> Text = IntToStr(par[0][11]);
            EditOTLtek25 -> Text = IntToStr(par[0][12]);
            EditOTLtek26 -> Text = IntToStr(par[0][13]);
            EditOTLtek27 -> Text = IntToStr(par[0][15]);
            EditOTLtek28 -> Text = IntToStr(par[0][16]);
            EditOTLtek29 -> Text = IntToStr(par[0][17]);
            EditOTLtek30 -> Text = IntToStr(par[0][18]);
        }; break;
        case 10:  // 10 ��������
        {   EditOTLtek1  -> Text = IntToStr(par[1][0]);
            EditOTLtek2  -> Text = IntToStr(par[1][1]);
            EditOTLtek3  -> Text = IntToStr(par[1][2]);
            EditOTLtek4  -> Text = IntToStr(par[1][3]);
            EditOTLtek5  -> Text = IntToStr(par[1][4]);
            EditOTLtek6  -> Text = IntToStr(par[1][5]);
            EditOTLtek7  -> Text = IntToStr(par[1][6]);
            EditOTLtek8  -> Text = IntToStr(par[1][7]);
            EditOTLtek9  -> Text = IntToStr(par[1][8]);
            EditOTLtek10 -> Text = IntToStr(par[1][9]);
            EditOTLtek11 -> Text = IntToStr(par[1][10]);
            EditOTLtek12 -> Text = IntToStr(par[1][11]);
            EditOTLtek13 -> Text = IntToStr(par[1][12]);
            EditOTLtek14 -> Text = IntToStr(par[1][13]);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(0);
			EditOTLtek17 -> Text = IntToStr(par[2][0]);
            EditOTLtek18 -> Text = IntToStr(par[2][1]);
            EditOTLtek19 -> Text = IntToStr(par[2][2]);
            EditOTLtek20 -> Text = IntToStr(par[2][3]);
            EditOTLtek21 -> Text = IntToStr(par[2][4]);
            EditOTLtek22 -> Text = IntToStr(par[2][5]);
            EditOTLtek23 -> Text = IntToStr(par[2][6]);
            EditOTLtek24 -> Text = IntToStr(par[2][7]);
            EditOTLtek25 -> Text = IntToStr(par[2][8]);
            EditOTLtek26 -> Text = IntToStr(par[2][9]);
            EditOTLtek27 -> Text = IntToStr(par[2][10]);
            EditOTLtek28 -> Text = IntToStr(par[2][11]);
            EditOTLtek29 -> Text = IntToStr(par[2][12]);
            EditOTLtek30 -> Text = IntToStr(par[2][13]);
        }; break;
		case 11:  // 11 ��������
        {   EditOTLtek1  -> Text = IntToStr(par[3][0]);
            EditOTLtek2  -> Text = IntToStr(par[3][1]);
            EditOTLtek3  -> Text = IntToStr(par[3][2]);
            EditOTLtek4  -> Text = IntToStr(par[3][3]);
            EditOTLtek5  -> Text = IntToStr(par[3][4]);
            EditOTLtek6  -> Text = IntToStr(par[3][5]);
            EditOTLtek7  -> Text = IntToStr(par[3][6]);
            EditOTLtek8  -> Text = IntToStr(par[3][7]);
            EditOTLtek9  -> Text = IntToStr(par[3][8]);
            EditOTLtek10 -> Text = IntToStr(par[3][9]);
            EditOTLtek11 -> Text = IntToStr(par[3][10]);
            EditOTLtek12 -> Text = IntToStr(par[3][11]);
            EditOTLtek13 -> Text = IntToStr(par[3][12]);
            EditOTLtek14 -> Text = IntToStr(par[3][13]);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(par[4][0]);
            EditOTLtek17 -> Text = IntToStr(par[4][1]);
            EditOTLtek18 -> Text = IntToStr(par[4][2]);
            EditOTLtek19 -> Text = IntToStr(par[4][3]);
            EditOTLtek20 -> Text = IntToStr(par[4][4]);
            EditOTLtek21 -> Text = IntToStr(par[4][5]);
            EditOTLtek22 -> Text = IntToStr(par[4][6]);
            EditOTLtek23 -> Text = IntToStr(par[4][7]);
            EditOTLtek24 -> Text = IntToStr(par[4][8]);
            EditOTLtek25 -> Text = IntToStr(par[4][9]);
            EditOTLtek26 -> Text = IntToStr(par[4][10]);
            EditOTLtek27 -> Text = IntToStr(par[4][11]);
            EditOTLtek28 -> Text = IntToStr(par[4][12]);
            EditOTLtek29 -> Text = IntToStr(par[4][13]);
			EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 12:  // 12 ��������
        {   EditOTLtek1  -> Text = IntToStr(par[5][0]);
            EditOTLtek2  -> Text = IntToStr(par[5][1]);
            EditOTLtek3  -> Text = IntToStr(par[5][2]);
            EditOTLtek4  -> Text = IntToStr(par[5][3]);
            EditOTLtek5  -> Text = IntToStr(par[5][4]);
            EditOTLtek6  -> Text = IntToStr(par[5][5]);
            EditOTLtek7  -> Text = IntToStr(par[5][6]);
            EditOTLtek8  -> Text = IntToStr(par[5][7]);
            EditOTLtek9  -> Text = IntToStr(par[5][8]);
            EditOTLtek10 -> Text = IntToStr(par[5][9]);
            EditOTLtek11 -> Text = IntToStr(par[5][10]);
            EditOTLtek12 -> Text = IntToStr(par[5][11]);
            EditOTLtek13 -> Text = IntToStr(par[5][12]);
            EditOTLtek14 -> Text = IntToStr(par[5][13]);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(par[6][0]);
            EditOTLtek17 -> Text = IntToStr(par[6][1]);
            EditOTLtek18 -> Text = IntToStr(par[6][2]);
            EditOTLtek19 -> Text = IntToStr(par[6][3]);
            EditOTLtek20 -> Text = IntToStr(par[6][4]);
            EditOTLtek21 -> Text = IntToStr(par[6][5]);
            EditOTLtek22 -> Text = IntToStr(par[6][6]);
            EditOTLtek23 -> Text = IntToStr(par[6][7]);
            EditOTLtek24 -> Text = IntToStr(par[6][8]);
            EditOTLtek25 -> Text = IntToStr(par[6][9]);
            EditOTLtek26 -> Text = IntToStr(par[6][10]);
            EditOTLtek27 -> Text = IntToStr(par[6][11]);
            EditOTLtek28 -> Text = IntToStr(par[6][12]);
            EditOTLtek29 -> Text = IntToStr(par[6][13]);
			EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 13:  // 13 ��������
        {   EditOTLtek1  -> Text = IntToStr(par[7][0]);
            EditOTLtek2  -> Text = IntToStr(par[7][1]);
            EditOTLtek3  -> Text = IntToStr(par[7][2]);
            EditOTLtek4  -> Text = IntToStr(par[7][3]);
            EditOTLtek5  -> Text = IntToStr(par[7][4]);
            EditOTLtek6  -> Text = IntToStr(par[7][5]);
            EditOTLtek7  -> Text = IntToStr(par[7][6]);
            EditOTLtek8  -> Text = IntToStr(par[7][7]);
            EditOTLtek9  -> Text = IntToStr(par[7][8]);
            EditOTLtek10 -> Text = IntToStr(par[7][9]);
            EditOTLtek11 -> Text = IntToStr(par[7][10]);
            EditOTLtek12 -> Text = IntToStr(par[7][11]);
            EditOTLtek13 -> Text = IntToStr(par[7][12]);
            EditOTLtek14 -> Text = IntToStr(par[7][13]);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(par[8][0]);
            EditOTLtek17 -> Text = IntToStr(par[8][1]);
            EditOTLtek18 -> Text = IntToStr(par[8][2]);
            EditOTLtek19 -> Text = IntToStr(par[8][3]);
            EditOTLtek20 -> Text = IntToStr(par[8][4]);
            EditOTLtek21 -> Text = IntToStr(par[8][5]);
            EditOTLtek22 -> Text = IntToStr(par[8][6]);
            EditOTLtek23 -> Text = IntToStr(par[8][7]);
            EditOTLtek24 -> Text = IntToStr(par[8][8]);
            EditOTLtek25 -> Text = IntToStr(par[8][9]);
            EditOTLtek26 -> Text = IntToStr(par[8][10]);
            EditOTLtek27 -> Text = IntToStr(par[8][11]);
            EditOTLtek28 -> Text = IntToStr(par[8][12]);
            EditOTLtek29 -> Text = IntToStr(par[8][13]);
			EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 14:  // 14 ��������
        {   EditOTLtek1  -> Text = IntToStr(par[9][0]);
            EditOTLtek2  -> Text = IntToStr(par[9][1]);
            EditOTLtek3  -> Text = IntToStr(par[9][2]);
            EditOTLtek4  -> Text = IntToStr(par[9][3]);
            EditOTLtek5  -> Text = IntToStr(par[9][4]);
            EditOTLtek6  -> Text = IntToStr(par[9][5]);
            EditOTLtek7  -> Text = IntToStr(par[9][6]);
            EditOTLtek8  -> Text = IntToStr(par[9][7]);
            EditOTLtek9  -> Text = IntToStr(par[9][8]);
            EditOTLtek10 -> Text = IntToStr(par[9][9]);
            EditOTLtek11 -> Text = IntToStr(par[9][10]);
            EditOTLtek12 -> Text = IntToStr(par[9][11]);
            EditOTLtek13 -> Text = IntToStr(par[9][12]);
            EditOTLtek14 -> Text = IntToStr(par[9][13]);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(par[10][0]);
            EditOTLtek17 -> Text = IntToStr(par[10][1]);
            EditOTLtek18 -> Text = IntToStr(par[10][2]);
            EditOTLtek19 -> Text = IntToStr(par[10][3]);
            EditOTLtek20 -> Text = IntToStr(par[10][4]);
            EditOTLtek21 -> Text = IntToStr(par[10][5]);
            EditOTLtek22 -> Text = IntToStr(par[10][6]);
            EditOTLtek23 -> Text = IntToStr(par[10][7]);
            EditOTLtek24 -> Text = IntToStr(par[10][8]);
            EditOTLtek25 -> Text = IntToStr(par[10][9]);
            EditOTLtek26 -> Text = IntToStr(par[10][10]);
            EditOTLtek27 -> Text = IntToStr(par[10][11]);
            EditOTLtek28 -> Text = IntToStr(par[10][12]);
            EditOTLtek29 -> Text = IntToStr(par[10][13]);
			EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 15:  // 15 ��������
        {   EditOTLtek1  -> Text = IntToStr(par_n[0]);
            EditOTLtek2  -> Text = IntToStr(par_n[1]);
            EditOTLtek3  -> Text = IntToStr(par_n[2]);
            EditOTLtek4  -> Text = IntToStr(par_n[3]);
            EditOTLtek5  -> Text = IntToStr(par_n[4]);
            EditOTLtek6  -> Text = IntToStr(par_n[5]);
            EditOTLtek7  -> Text = IntToStr(par_n[6]);
            EditOTLtek8  -> Text = IntToStr(par_n[7]);
            EditOTLtek9  -> Text = IntToStr(par_n[8]);
            EditOTLtek10 -> Text = IntToStr(par_n[9]);
            EditOTLtek11 -> Text = IntToStr(par_n[10]);
            EditOTLtek12 -> Text = IntToStr(par_n[11]);
            EditOTLtek13 -> Text = IntToStr(par_n[12]);
            EditOTLtek14 -> Text = IntToStr(par_n[13]);
            EditOTLtek15 -> Text = IntToStr(par_n[14]);
            EditOTLtek16 -> Text = IntToStr(par_n[15]);
            EditOTLtek17 -> Text = IntToStr(par_n[16]);
            EditOTLtek18 -> Text = IntToStr(par_n[17]);
            EditOTLtek19 -> Text = IntToStr(par_n[18]);
            EditOTLtek20 -> Text = IntToStr(par_n[19]);
			EditOTLtek21 -> Text = IntToStr(par_n[20]);
            EditOTLtek22 -> Text = IntToStr(par_n[21]);
            EditOTLtek23 -> Text = IntToStr(par_n[22]);
            EditOTLtek24 -> Text = IntToStr(par_n[23]);
            EditOTLtek25 -> Text = IntToStr(par_t[0]);
            EditOTLtek26 -> Text = IntToStr(par_t[1]);
            EditOTLtek27 -> Text = IntToStr(par_t[2]);
            EditOTLtek28 -> Text = IntToStr(par_t[3]);
            EditOTLtek29 -> Text = IntToStr(par_t[4]);
            EditOTLtek30 -> Text = IntToStr(par_t[5]);
        }; break;
        case 16:  // 16 ��������
        {   EditOTLtek1  -> Text = IntToStr(T_ZAD_DVS);
            EditOTLtek2  -> Text = IntToStr(T_PROC);
            EditOTLtek3  -> Text = IntToStr(T_KTMN_RAZGON);
            EditOTLtek4  -> Text = IntToStr(T_KKAM_V);
            EditOTLtek5  -> Text = IntToStr(T_VODA);
            EditOTLtek6  -> Text = IntToStr(T_STOP);
            EditOTLtek7  -> Text = IntToStr(T_DVIJ);
            EditOTLtek8 -> Text = IntToStr(T_KDVIJ_SU);
            EditOTLtek9 -> Text = IntToStr(T_KSUT);
            EditOTLtek10 -> Text = IntToStr(T_KKAM);
            EditOTLtek11 -> Text = IntToStr(T_KTMN);
            EditOTLtek12 -> Text = IntToStr(T_KSHL);
            EditOTLtek13 -> Text = IntToStr(T_KNAP);
            EditOTLtek14 -> Text = IntToStr(T_NAPUSK);
            EditOTLtek15 -> Text = IntToStr(T_VHG);
            EditOTLtek16 -> Text = IntToStr(T_KSHL_MO);
            EditOTLtek17 -> Text = IntToStr(0);
            EditOTLtek18 -> Text = IntToStr(0);
            EditOTLtek19 -> Text = IntToStr(0);
            EditOTLtek20 -> Text = IntToStr(0);
			EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(0);
            EditOTLtek23 -> Text = IntToStr(0);
            EditOTLtek24 -> Text = IntToStr(0);
            EditOTLtek25 -> Text = IntToStr(0);
            EditOTLtek26 -> Text = IntToStr(0);
            EditOTLtek27 -> Text = IntToStr(0);
            EditOTLtek28 -> Text = IntToStr(0);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 17:  // 17 ��������
        {   EditOTLtek1  -> Text = IntToStr(CT_VHG);
            EditOTLtek2  -> Text = IntToStr(CT_VODA_IP);
			EditOTLtek3  -> Text = IntToStr(0);
            EditOTLtek4  -> Text = IntToStr(0);
            EditOTLtek5  -> Text = IntToStr(0);
            EditOTLtek6  -> Text = IntToStr(CT_TEMP);
            EditOTLtek7  -> Text = IntToStr(CT_DVIJ_GIR_g);
            EditOTLtek8  -> Text = IntToStr(CT_DVIJ_GIR_t);
            EditOTLtek9  -> Text = IntToStr(CT_SUT_g);
            EditOTLtek10 -> Text = IntToStr(CT_SUT_t);
            EditOTLtek11 -> Text = IntToStr(0);
            EditOTLtek12 -> Text = IntToStr(0);
            EditOTLtek13 -> Text = IntToStr(0);
            EditOTLtek14 -> Text = IntToStr(0);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(CT_T1);
            EditOTLtek17 -> Text = IntToStr(CT_T20);
            EditOTLtek18 -> Text = IntToStr(CT_1);
            EditOTLtek19 -> Text = IntToStr(CT_2);
            EditOTLtek20 -> Text = IntToStr(CT_3);
            EditOTLtek21 -> Text = IntToStr(CT_4);
            EditOTLtek22 -> Text = IntToStr(CT_5);
			EditOTLtek23 -> Text = IntToStr(CT_6);
            EditOTLtek24 -> Text = IntToStr(CT_7);
            EditOTLtek25 -> Text = IntToStr(CT_9);
            EditOTLtek26 -> Text = IntToStr(CT_17);
            EditOTLtek27 -> Text = IntToStr(CT17K1);
            EditOTLtek28 -> Text = IntToStr(CT_29);
            EditOTLtek29 -> Text = IntToStr(CT29K1);
            EditOTLtek30 -> Text = IntToStr(CT_30T);
        }; break;
        case 18:  // 18 ��������
        {   EditOTLtek1  -> Text = IntToStr(PR_KLASTER);
            EditOTLtek2  -> Text = IntToStr(otvet);
			EditOTLtek3  -> Text = IntToStr(zshr3);
            EditOTLtek4  -> Text = IntToStr(PR_TRTEST);
            EditOTLtek5  -> Text = IntToStr(PR_OTK);
            EditOTLtek6  -> Text = IntToStr(PR_FK_KAM);
            EditOTLtek7  -> Text = IntToStr(PR_NASOS);
            EditOTLtek8  -> Text = IntToStr(PR_NALADKA);
            EditOTLtek9  -> Text = IntToStr(N_PL);
            EditOTLtek10 -> Text = IntToStr(N_ST_MAX);
            EditOTLtek11 -> Text = IntToStr(N_ST);
            EditOTLtek12 -> Text = IntToStr(N_ZICL);
            EditOTLtek13 -> Text = IntToStr(ZN_ST);
            EditOTLtek14 -> Text = IntToStr(0);
            EditOTLtek15 -> Text = IntToStr(KOM_MOD);
            EditOTLtek16 -> Text = IntToStr(OTVET_MOD);
            EditOTLtek17 -> Text = IntToStr(0);
            EditOTLtek18 -> Text = IntToStr(0);
            EditOTLtek19 -> Text = IntToStr(0);
            EditOTLtek20 -> Text = IntToStr(0);
            EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(0);
			EditOTLtek23 -> Text = IntToStr(0);
            EditOTLtek24 -> Text = IntToStr(0);
            EditOTLtek25 -> Text = IntToStr(0);
            EditOTLtek26 -> Text = IntToStr(0);
            EditOTLtek27 -> Text = IntToStr(0);
            EditOTLtek28 -> Text = IntToStr(0);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 19:  // 19 ��������
        {   EditOTLtek1  -> Text = IntToStr(DATA_DZASL);
            EditOTLtek2  -> Text = IntToStr(PAR_DZASL);
            EditOTLtek3  -> Text = IntToStr(ZPAR_DZASL);
            EditOTLtek4  -> Text = IntToStr(X_TDZASL);
            EditOTLtek5  -> Text = IntToStr(VRDZASL);
            EditOTLtek6  -> Text = IntToStr(E_TDZASL);
            EditOTLtek7  -> Text = IntToStr(DELDZASL);
            EditOTLtek8  -> Text = IntToStr(LIM1DZASL);
            EditOTLtek9  -> Text = IntToStr(LIM2DZASL);
            EditOTLtek10 -> Text = IntToStr(T_VRDZASL);
            EditOTLtek11 -> Text = IntToStr(T_KDZASL);
            EditOTLtek12 -> Text = IntToStr(DOPDZASL);
            EditOTLtek13 -> Text = IntToStr(KOM_DZASL);
            EditOTLtek14 -> Text = IntToStr(TEK_DAVL_DZASL);
            EditOTLtek15 -> Text = IntToStr(TEK_POZ_DZASL);
            EditOTLtek16 -> Text = IntToStr(PR_DZASL);
            EditOTLtek17 -> Text = IntToStr(CT_DZASL);
            EditOTLtek18 -> Text = IntToStr(OTVET_DZASL);
            EditOTLtek19 -> Text = IntToStr(DAVL_DZASL);
            EditOTLtek20 -> Text = IntToStr(0);
			EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(0);
            EditOTLtek23 -> Text = IntToStr(0);
            EditOTLtek24 -> Text = IntToStr(0);
            EditOTLtek25 -> Text = IntToStr(0);
            EditOTLtek26 -> Text = IntToStr(0);
            EditOTLtek27 -> Text = IntToStr(0);
            EditOTLtek28 -> Text = IntToStr(0);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 20:   // 20 ��������
        {   EditOTLtek1  -> Text = IntToStr(prDvijGir_g);
            EditOTLtek2  -> Text = IntToStr(prDvijGir_t);
            EditOTLtek3  -> Text = IntToStr(DOP_SU);
            EditOTLtek4  -> Text = IntToStr(T_SM_NAPR);
            EditOTLtek5  -> Text = IntToStr(DOP_DV_IP);
            EditOTLtek6  -> Text = IntToStr(klGir_gV);
            EditOTLtek7  -> Text = IntToStr(klGir_gN);
            EditOTLtek8  -> Text = IntToStr(klGir_tV);
            EditOTLtek9 -> Text = IntToStr(klGir_tN);
            EditOTLtek10 -> Text = IntToStr(VRGIR);
            EditOTLtek11 -> Text = IntToStr(K_SOGL_GIR);
            EditOTLtek12 -> Text = IntToStr(NAPRS_GIR);
            EditOTLtek13 -> Text = IntToStr(X_TGIR);
            EditOTLtek14 -> Text = IntToStr(E_TGIR);
            EditOTLtek15 -> Text = IntToStr(DELGIR);
            EditOTLtek16 -> Text = IntToStr(DOPGIR);
            EditOTLtek17 -> Text = IntToStr(PAR_GIR);
            EditOTLtek18 -> Text = IntToStr(N_TEK_GIR);
            EditOTLtek19 -> Text = IntToStr(LIM1GIR);
			EditOTLtek20 -> Text = IntToStr(LIM2GIR);
            EditOTLtek21 -> Text = IntToStr(T_VRGIR);
            EditOTLtek22 -> Text = IntToStr(T_KGIR);
            EditOTLtek23 -> Text = IntToStr(N_PRED_GIR);
            EditOTLtek24 -> Text = IntToStr(0);
            EditOTLtek25 -> Text = IntToStr(0);
            EditOTLtek26 -> Text = IntToStr(0);
            EditOTLtek27 -> Text = IntToStr(0);
            EditOTLtek28 -> Text = IntToStr(0);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 21:   // 21 ��������
        {   EditOTLtek1  -> Text = IntToStr(PR_PER);
            EditOTLtek2  -> Text = IntToStr(KOM_PER);
            EditOTLtek3  -> Text = IntToStr(OTVET_PER);
            EditOTLtek4  -> Text = IntToStr(V_PER);
            EditOTLtek5  -> Text = IntToStr(HOME_PER);
            EditOTLtek6  -> Text = IntToStr(TEK_OTN_PER);
            EditOTLtek7  -> Text = IntToStr(TEK_ABS_PER);
            EditOTLtek8  -> Text = IntToStr(PUT_PER);
            EditOTLtek9 -> Text = IntToStr(CT_PER);
            EditOTLtek10 -> Text = IntToStr(0);
            EditOTLtek11 -> Text = IntToStr(0);
            EditOTLtek12 -> Text = IntToStr(PR_KAS);
            EditOTLtek13 -> Text = IntToStr(KOM_KAS);
            EditOTLtek14 -> Text = IntToStr(OTVET_KAS);
            EditOTLtek15 -> Text = IntToStr(V_KAS);
            EditOTLtek16 -> Text = IntToStr(HOME_KAS);
            EditOTLtek17 -> Text = IntToStr(TEK_OTN_KAS);
            EditOTLtek18 -> Text = IntToStr(TEK_ABS_KAS);
            EditOTLtek19 -> Text = IntToStr(PUT_KAS);
			EditOTLtek20 -> Text = IntToStr(CT_KAS);
            EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(PR_POV);
            EditOTLtek23 -> Text = IntToStr(KOM_POV);
            EditOTLtek24 -> Text = IntToStr(OTVET_POV);
            EditOTLtek25 -> Text = IntToStr(V_POV);
            EditOTLtek26 -> Text = IntToStr(HOME_POV);
            EditOTLtek27 -> Text = IntToStr(TEK_OTN_POV);
            EditOTLtek28 -> Text = IntToStr(TEK_ABS_POV);
            EditOTLtek29 -> Text = IntToStr(PUT_POV);
            EditOTLtek30 -> Text = IntToStr(CT_POV);
        }; break;
        case 22:   // 22 ��������
        {   EditOTLtek1  -> Text = IntToStr(ObjBPN[0]->prBPN);
            EditOTLtek2  -> Text = IntToStr(ObjBPN[0]->vBPN);
            EditOTLtek3  -> Text = IntToStr(ObjBPN[0]->zadBPN);
            EditOTLtek4  -> Text = IntToStr(ObjBPN[0]->tekBPN);
            EditOTLtek5  -> Text = IntToStr(ObjBPN[1]->prBPN);
            EditOTLtek6  -> Text = IntToStr(ObjBPN[1]->vBPN);
            EditOTLtek7  -> Text = IntToStr(ObjBPN[1]->zadBPN);
            EditOTLtek8  -> Text = IntToStr(ObjBPN[1]->tekBPN);
            EditOTLtek9  -> Text = IntToStr(ObjBPN[4]->prBPN);
            EditOTLtek10 -> Text = IntToStr(ObjBPN[4]->vBPN);
            EditOTLtek11 -> Text = IntToStr(ObjBPN[4]->zadBPN);
            EditOTLtek12 -> Text = IntToStr(ObjBPN[4]->tekBPN);
            EditOTLtek13 -> Text = IntToStr(ObjBPN[5]->prBPN);
            EditOTLtek14 -> Text = IntToStr(ObjBPN[5]->vBPN);
            EditOTLtek15 -> Text = IntToStr(ObjBPN[5]->zadBPN);
            EditOTLtek16 -> Text = IntToStr(ObjBPN[5]->tekBPN);
            EditOTLtek17 -> Text = IntToStr(ObjBPN[6]->prBPN);
            EditOTLtek18 -> Text = IntToStr(ObjBPN[6]->vBPN);
            EditOTLtek19 -> Text = IntToStr(ObjBPN[6]->zadBPN);
            EditOTLtek20 -> Text = IntToStr(ObjBPN[6]->tekBPN);
			EditOTLtek21 -> Text = IntToStr(ObjBPN[7]->prBPN);
            EditOTLtek22 -> Text = IntToStr(ObjBPN[7]->vBPN);
            EditOTLtek23 -> Text = IntToStr(ObjBPN[7]->zadBPN);
            EditOTLtek24 -> Text = IntToStr(ObjBPN[7]->tekBPN);
            EditOTLtek25 -> Text = IntToStr(ObjBPN[8]->prBPN);
            EditOTLtek26 -> Text = IntToStr(ObjBPN[8]->vBPN);
            EditOTLtek27 -> Text = IntToStr(ObjBPN[8]->zadBPN);
            EditOTLtek28 -> Text = IntToStr(ObjBPN[8]->tekBPN);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 23:   // 23 ��������
        {   EditOTLtek1  -> Text = IntToStr(ObjBPN[9]->prBPN);
            EditOTLtek2  -> Text = IntToStr(ObjBPN[9]->vBPN);
            EditOTLtek3  -> Text = IntToStr(ObjBPN[9]->zadBPN);
            EditOTLtek4  -> Text = IntToStr(ObjBPN[9]->tekBPN);
            EditOTLtek5  -> Text = IntToStr(ObjBPN[10]->prBPN);
            EditOTLtek6  -> Text = IntToStr(ObjBPN[10]->vBPN);
            EditOTLtek7  -> Text = IntToStr(ObjBPN[10]->zadBPN);
            EditOTLtek8  -> Text = IntToStr(ObjBPN[10]->tekBPN);
            EditOTLtek9  -> Text = IntToStr(ObjBPN[11]->prBPN);
            EditOTLtek10 -> Text = IntToStr(ObjBPN[11]->vBPN);
            EditOTLtek11 -> Text = IntToStr(ObjBPN[11]->zadBPN);
            EditOTLtek12 -> Text = IntToStr(ObjBPN[11]->tekBPN);
            EditOTLtek13 -> Text = IntToStr(ObjBPN[12]->prBPN);
            EditOTLtek14 -> Text = IntToStr(ObjBPN[12]->vBPN);
            EditOTLtek15 -> Text = IntToStr(ObjBPN[12]->zadBPN);
            EditOTLtek16 -> Text = IntToStr(ObjBPN[12]->tekBPN);
            EditOTLtek17 -> Text = IntToStr(ObjBPN[13]->prBPN);
            EditOTLtek18 -> Text = IntToStr(ObjBPN[13]->vBPN);
            EditOTLtek19 -> Text = IntToStr(ObjBPN[13]->zadBPN);
            EditOTLtek20 -> Text = IntToStr(ObjBPN[13]->tekBPN);
			EditOTLtek21 -> Text = IntToStr(ObjBPN[14]->prBPN);
            EditOTLtek22 -> Text = IntToStr(ObjBPN[14]->vBPN);
            EditOTLtek23 -> Text = IntToStr(ObjBPN[14]->zadBPN);
            EditOTLtek24 -> Text = IntToStr(ObjBPN[14]->tekBPN);
            EditOTLtek25 -> Text = IntToStr(ObjBPN[15]->prBPN);
            EditOTLtek26 -> Text = IntToStr(ObjBPN[15]->vBPN);
            EditOTLtek27 -> Text = IntToStr(ObjBPN[15]->zadBPN);
            EditOTLtek28 -> Text = IntToStr(ObjBPN[15]->tekBPN);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
		case 24:   // 24 ��������
        {   EditOTLtek1  -> Text = IntToStr(ObjBPN[16]->prBPN);
            EditOTLtek2  -> Text = IntToStr(ObjBPN[16]->vBPN);
            EditOTLtek3  -> Text = IntToStr(ObjBPN[16]->zadBPN);
            EditOTLtek4  -> Text = IntToStr(ObjBPN[16]->tekBPN);
            EditOTLtek5  -> Text = IntToStr(ObjBPN[17]->prBPN);
            EditOTLtek6  -> Text = IntToStr(ObjBPN[17]->vBPN);
            EditOTLtek7  -> Text = IntToStr(ObjBPN[17]->zadBPN);
            EditOTLtek8  -> Text = IntToStr(ObjBPN[17]->tekBPN);
            EditOTLtek9  -> Text = IntToStr(ObjBPN[18]->prBPN);
            EditOTLtek10 -> Text = IntToStr(ObjBPN[18]->vBPN);
            EditOTLtek11 -> Text = IntToStr(ObjBPN[18]->zadBPN);
            EditOTLtek12 -> Text = IntToStr(ObjBPN[18]->tekBPN);
            EditOTLtek13 -> Text = IntToStr(ObjBPN[19]->prBPN);
            EditOTLtek14 -> Text = IntToStr(ObjBPN[19]->vBPN);
            EditOTLtek15 -> Text = IntToStr(ObjBPN[19]->zadBPN);
            EditOTLtek16 -> Text = IntToStr(ObjBPN[19]->tekBPN);
            EditOTLtek17 -> Text = IntToStr(ObjBPN[20]->prBPN);
            EditOTLtek18 -> Text = IntToStr(ObjBPN[20]->vBPN);
            EditOTLtek19 -> Text = IntToStr(ObjBPN[20]->vBPN);
            EditOTLtek20 -> Text = IntToStr(ObjBPN[20]->zadBPN);
			EditOTLtek21 -> Text = IntToStr(ObjBPN[21]->prBPN);
            EditOTLtek22 -> Text = IntToStr(ObjBPN[21]->vBPN);
            EditOTLtek23 -> Text = IntToStr(ObjBPN[21]->zadBPN);
            EditOTLtek24 -> Text = IntToStr(ObjBPN[21]->tekBPN);
            EditOTLtek25 -> Text = IntToStr(ObjBPN[22]->prBPN);
            EditOTLtek26 -> Text = IntToStr(ObjBPN[22]->vBPN);
            EditOTLtek27 -> Text = IntToStr(ObjBPN[22]->zadBPN);
            EditOTLtek28 -> Text = IntToStr(ObjBPN[22]->tekBPN);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
        case 25:   // 25 ��������
        {   EditOTLtek1  -> Text = IntToStr(ObjBPN[23]->prBPN);
            EditOTLtek2  -> Text = IntToStr(ObjBPN[23]->vBPN);
            EditOTLtek3  -> Text = IntToStr(ObjBPN[23]->zadBPN);
            EditOTLtek4  -> Text = IntToStr(ObjBPN[23]->tekBPN);
            EditOTLtek5  -> Text = IntToStr(0);
            EditOTLtek6  -> Text = IntToStr(0);
            EditOTLtek7  -> Text = IntToStr(0);
            EditOTLtek8  -> Text = IntToStr(0);
            EditOTLtek9  -> Text = IntToStr(0);
            EditOTLtek10 -> Text = IntToStr(0);
            EditOTLtek11 -> Text = IntToStr(0);
            EditOTLtek12 -> Text = IntToStr(0);
            EditOTLtek13 -> Text = IntToStr(0);
            EditOTLtek14 -> Text = IntToStr(0);
            EditOTLtek15 -> Text = IntToStr(0);
            EditOTLtek16 -> Text = IntToStr(0);
            EditOTLtek17 -> Text = IntToStr(0);
            EditOTLtek18 -> Text = IntToStr(0);
            EditOTLtek19 -> Text = IntToStr(0);
            EditOTLtek20 -> Text = IntToStr(0);
			EditOTLtek21 -> Text = IntToStr(0);
            EditOTLtek22 -> Text = IntToStr(0);
            EditOTLtek23 -> Text = IntToStr(0);
            EditOTLtek24 -> Text = IntToStr(0);
            EditOTLtek25 -> Text = IntToStr(0);
            EditOTLtek26 -> Text = IntToStr(0);
            EditOTLtek27 -> Text = IntToStr(0);
            EditOTLtek28 -> Text = IntToStr(0);
            EditOTLtek29 -> Text = IntToStr(0);
            EditOTLtek30 -> Text = IntToStr(0);
        }; break;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//--������ �������� ������������ ����������--//
void __fastcall TForm1::BtnOtl1Click(TObject *Sender)
{   // ��������� �������� ���������� �� ��������
    switch(StrToInt(PCPerem -> ActivePage -> Hint))
    {   case 0:  // 0 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  sh_      = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  norma      = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  shr[1]   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  sh[1]    = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  shr[2]   = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  sh[2]    = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  shr[3]   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  sh[3]    = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  shr[4]   = StrToInt(EditOTLzad9  -> Text); break;
                case 10: sh[4]    = StrToInt(EditOTLzad10 -> Text); break;
                case 11: shr[5]   = StrToInt(EditOTLzad11 -> Text); break;
                case 12: sh[5]    = StrToInt(EditOTLzad12 -> Text); break;
                case 13: shr[6]   = StrToInt(EditOTLzad13 -> Text); break;
                case 14: sh[6]    = StrToInt(EditOTLzad14 -> Text); break;
                case 15: shr[7]   = StrToInt(EditOTLzad15 -> Text); break;
                case 16: sh[7]    = StrToInt(EditOTLzad16 -> Text); break;
                case 17: shr[8]   = StrToInt(EditOTLzad17 -> Text); break;
                case 18: sh[8]    = StrToInt(EditOTLzad18 -> Text); break;
                case 19: shr[9]   = StrToInt(EditOTLzad19 -> Text); break;
                case 20: sh[9]    = StrToInt(EditOTLzad20 -> Text); break;
				case 21: shr[10]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: sh[10]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: shr[11]  = StrToInt(EditOTLzad23 -> Text); break;
                case 24: sh[11]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: shr[12]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: sh[12]   = StrToInt(EditOTLzad26 -> Text); break;
                case 27: shr[13]  = StrToInt(EditOTLzad27 -> Text); break;
                case 28: sh[13]   = StrToInt(EditOTLzad28 -> Text); break;
                case 29: shr[14]  = StrToInt(EditOTLzad29 -> Text); break;
                case 30: sh[14]   = StrToInt(EditOTLzad30 -> Text); break;
           }
        }; break;
        case 1:  // 1 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  shr[15]  = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  sh[15]   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  shr[17]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  sh[17]   = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  shr[18]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  sh[18]   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  shr[19]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  sh[19]   = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  shr[20]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: sh[20]   = StrToInt(EditOTLzad10 -> Text); break;
                case 11: shr[21]  = StrToInt(EditOTLzad11 -> Text); break;
                case 12: sh[21]   = StrToInt(EditOTLzad12 -> Text); break;
                case 13: shr[22]  = StrToInt(EditOTLzad13 -> Text); break;
                case 14: sh[22]   = StrToInt(EditOTLzad14 -> Text); break;
                case 15: shr[23]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: sh[23]   = StrToInt(EditOTLzad16 -> Text); break;
                case 17: shr[24]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: sh[24]   = StrToInt(EditOTLzad18 -> Text); break;
                case 19: shr[25]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: sh[25]   = StrToInt(EditOTLzad20 -> Text); break;
				case 21: shr[26]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: sh[26]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: shr[29]  = StrToInt(EditOTLzad23 -> Text); break;
                case 24: sh[29]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: shr[30]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: sh[30]   = StrToInt(EditOTLzad26 -> Text); break;
                case 27: shr[31]  = StrToInt(EditOTLzad27 -> Text); break;
                case 28: sh[31]   = StrToInt(EditOTLzad28 -> Text); break;
                case 29: shr[32]  = StrToInt(EditOTLzad29 -> Text); break;
                case 30: sh[32]   = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 2:  // 2 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {
				case 1:  shr[33]  = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  sh[33]   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  shr[34]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  sh[34]   = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  shr[37]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  sh[37]   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  shr[38]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  sh[38]   = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  shr[40]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: sh[40]   = StrToInt(EditOTLzad10 -> Text); break;
                case 11: shr[41]  = StrToInt(EditOTLzad11 -> Text); break;
                case 12: sh[41]   = StrToInt(EditOTLzad12 -> Text); break;
                case 13: shr[42]  = StrToInt(EditOTLzad13 -> Text); break;
                case 14: sh[42]   = StrToInt(EditOTLzad14 -> Text); break;
                case 15: shr[43]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: sh[43]   = StrToInt(EditOTLzad16 -> Text); break;
                case 17: shr[44]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: sh[44]   = StrToInt(EditOTLzad18 -> Text); break;
                case 19: shr[45]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: sh[45]   = StrToInt(EditOTLzad20 -> Text); break;
				case 21: shr[46]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: sh[46]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: shr[47]  = StrToInt(EditOTLzad23 -> Text); break;
                case 24: sh[47]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: shr[48]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: sh[48]   = StrToInt(EditOTLzad26 -> Text); break;
                case 27: shr[49]  = StrToInt(EditOTLzad27 -> Text); break;
                case 28: sh[49]   = StrToInt(EditOTLzad28 -> Text); break;
                case 29: shr[50]  = StrToInt(EditOTLzad29 -> Text); break;
                case 30: sh[50]   = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		 case 3:  // 3 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  shr[51]  = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  sh[51]   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  shr[52]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  sh[52]   = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  shr[53]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  sh[53]   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  shr[54]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  sh[54]   = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  shr[55]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: sh[55]   = StrToInt(EditOTLzad10 -> Text); break;
                case 11: shr[56]  = StrToInt(EditOTLzad11 -> Text); break;
                case 12: sh[56]   = StrToInt(EditOTLzad12 -> Text); break;
                case 13: shr[57]  = StrToInt(EditOTLzad13 -> Text); break;
                case 14: sh[57]   = StrToInt(EditOTLzad14 -> Text); break;
                case 15: shr[58]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: sh[58]   = StrToInt(EditOTLzad16 -> Text); break;
                case 17: shr[59]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: sh[59]   = StrToInt(EditOTLzad18 -> Text); break;
                case 19: shr[60]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: sh[60]   = StrToInt(EditOTLzad20 -> Text); break;
				case 21: shr[61]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: sh[61]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: shr[62]  = StrToInt(EditOTLzad23 -> Text); break;
                case 24: sh[62]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: shr[63]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: sh[63]   = StrToInt(EditOTLzad26 -> Text); break;
                case 27: shr[64]  = StrToInt(EditOTLzad27 -> Text); break;
                case 28: sh[64]   = StrToInt(EditOTLzad28 -> Text); break;
                case 29: shr[65]  = StrToInt(EditOTLzad29 -> Text); break;
                case 30: sh[65]   = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		 case 4:  // 4 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  shr[66]  = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  sh[66]   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  shr[67]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  sh[67]   = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  shr[68]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  sh[68]   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  shr[69]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  sh[69]   = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  shr[70]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: sh[70]   = StrToInt(EditOTLzad10 -> Text); break;
                case 11: shr[71]  = StrToInt(EditOTLzad11 -> Text); break;
                case 12: sh[71]   = StrToInt(EditOTLzad12 -> Text); break;
                case 13: shr[72]  = StrToInt(EditOTLzad13 -> Text); break;
                case 14: sh[72]   = StrToInt(EditOTLzad14 -> Text); break;
                case 15: shr[73]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: sh[73]   = StrToInt(EditOTLzad16 -> Text); break;
                case 17: shr[74]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: sh[74]   = StrToInt(EditOTLzad18 -> Text); break;
                case 19: shr[75]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: sh[75]   = StrToInt(EditOTLzad20 -> Text); break;
				case 21: shr[76]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: sh[76]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: shr[77]    = StrToInt(EditOTLzad23 -> Text); break;
                case 24: sh[77]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: shr[78]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: sh[78]   = StrToInt(EditOTLzad26 -> Text); break;
                case 27: shr[79]  = StrToInt(EditOTLzad27 -> Text); break;
                case 28: sh[79]   = StrToInt(EditOTLzad28 -> Text); break;
                case 29: shr[80]  = StrToInt(EditOTLzad29 -> Text); break;
                case 30: sh[80]   = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 5:  // 5 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {
				case 1:  shr[81]  = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  sh[81]  = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  shr[82]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  sh[82]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  shr[83]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  sh[83]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  shr[84]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  sh[84]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  shr[85]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: sh[85]  = StrToInt(EditOTLzad10 -> Text); break;
                //case 11:   = StrToInt(EditOTLzad11 -> Text); break;
                case 12: diagn[0]  = StrToInt(EditOTLzad12 -> Text); break;
                case 13: diagn[1] = StrToInt(EditOTLzad13 -> Text); break;
                case 14: diagn[2] = StrToInt(EditOTLzad14 -> Text); break;
                case 15: diagn[3] = StrToInt(EditOTLzad15 -> Text); break;
                case 16: diagn[4] = StrToInt(EditOTLzad16 -> Text); break;
				case 17: diagn[5] = StrToInt(EditOTLzad17 -> Text); break;
                case 18: diagn[6] = StrToInt(EditOTLzad18 -> Text); break;
                case 19: diagn[7] = StrToInt(EditOTLzad19 -> Text); break;
                case 20: diagn[8] = StrToInt(EditOTLzad20 -> Text); break;
				case 21: diagn[9] = StrToInt(EditOTLzad21 -> Text); break;
                case 22: diagn[10] = StrToInt(EditOTLzad22 -> Text); break;
                case 23: diagn[11] = StrToInt(EditOTLzad23 -> Text); break;
                case 24: diagn[12] = StrToInt(EditOTLzad24 -> Text); break;
                case 25: diagn[13] = StrToInt(EditOTLzad25 -> Text); break;
                case 26: diagn[14] = StrToInt(EditOTLzad26 -> Text); break;
                case 27: diagn[15] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: diagn[16] = StrToInt(EditOTLzad28 -> Text); break;
                case 29: diagn[17] = StrToInt(EditOTLzad29 -> Text); break;
                case 30: diagn[18] = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 6:  // 6 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {
				case 1:  diagn[19]  = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  diagn[20]  = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  diagn[21]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  diagn[22]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  diagn[23]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  diagn[24]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  diagn[25]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  diagn[26]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  diagn[27]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: diagn[28]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: diagn[29]  = StrToInt(EditOTLzad11 -> Text); break;
                case 12: diagn[30] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: diagn[31] = StrToInt(EditOTLzad13 -> Text); break;
                case 14: diagn[32] = StrToInt(EditOTLzad14 -> Text); break;
                case 15: diagn[33] = StrToInt(EditOTLzad15 -> Text); break;
                case 16: diagn[34] = StrToInt(EditOTLzad16 -> Text); break;
				case 17: diagn[35] = StrToInt(EditOTLzad17 -> Text); break;
                case 18: diagnS[0] = StrToInt(EditOTLzad18 -> Text); break;
                case 19: diagnS[1] = StrToInt(EditOTLzad19 -> Text); break;
                case 20: diagnS[2] = StrToInt(EditOTLzad20 -> Text); break;
				case 21: out[0] = StrToInt(EditOTLzad21 -> Text); break;
                case 22: out[1] = StrToInt(EditOTLzad22 -> Text); break;
                case 23: out[2] = StrToInt(EditOTLzad23 -> Text); break;
                case 24: out[3] = StrToInt(EditOTLzad24 -> Text); break;
                case 25: out[4] = StrToInt(EditOTLzad25 -> Text); break;
                case 26: zin[0] = StrToInt(EditOTLzad26 -> Text); break;
                case 27: zin[1] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: zin[2] = StrToInt(EditOTLzad28 -> Text); break;
                case 29: zin[3] = StrToInt(EditOTLzad29 -> Text); break;
                case 30: zin[4] = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 7:  // 7 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  aik[0]   = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  aik[1]   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  aik[2]   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  aik[3]   = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  aik[4]   = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  aik[5]   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  aik[6]   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  aik[7]   = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  aik[8]   = StrToInt(EditOTLzad9  -> Text); break;
                case 10: aik[9]   = StrToInt(EditOTLzad10 -> Text); break;
                case 11: aik[10]  = StrToInt(EditOTLzad11 -> Text); break;
                case 12: aik[11]  = StrToInt(EditOTLzad12 -> Text); break;
                case 13: aik[12]  = StrToInt(EditOTLzad13 -> Text); break;
                case 14: aik[13]  = StrToInt(EditOTLzad14 -> Text); break;
                case 15: aik[14]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: aik[15]  = StrToInt(EditOTLzad16 -> Text); break;
				case 17: aik[16]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: aik[17]  = StrToInt(EditOTLzad18 -> Text); break;
                case 19: aik[18]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: aik[19]  = StrToInt(EditOTLzad20 -> Text); break;
				//case 21: out[0]   = StrToInt(EditOTLzad1  -> Text); break;
                //case 22: out[1] = StrToInt(EditOTLzad2  -> Text); break;
                //case 23: out[2] = StrToInt(EditOTLzad3  -> Text); break;
                //case 24: out[3] = StrToInt(EditOTLzad4  -> Text); break;
                //case 25: out[4] = StrToInt(EditOTLzad5  -> Text); break;
                //case 26: zin[0] = StrToInt(EditOTLzad6  -> Text); break;
                //case 27: zin[1] = StrToInt(EditOTLzad7  -> Text); break;
                //case 28: zin[2] = StrToInt(EditOTLzad8  -> Text); break;
                //case 29: zin[3] = StrToInt(EditOTLzad9  -> Text); break;
                //case 30: zin[4] = StrToInt(EditOTLzad10 -> Text); break;
            }
        }; break;
        case 8:  // 8 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:	  aout[0] = StrToInt(EditOTLzad1 -> Text); break;
				case 2:	  aout[1] = StrToInt(EditOTLzad2 -> Text); break;
				case 3:	  aout[2] = StrToInt(EditOTLzad3 -> Text); break;
				case 4:   aout[3] = StrToInt(EditOTLzad4 -> Text); break;
				case 5:   aout[4] = StrToInt(EditOTLzad5  -> Text); break;
                case 6:   aout[5] = StrToInt(EditOTLzad6  -> Text); break;
                case 7:   aout[6] = StrToInt(EditOTLzad7  -> Text); break;
                case 8:   aout[7] = StrToInt(EditOTLzad8  -> Text); break;
                case 9:   aout[8] = StrToInt(EditOTLzad9  -> Text); break;
                case 10:  aout[9] = StrToInt(EditOTLzad10 -> Text); break;
                case 11:  aout[10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12:  aout[11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13:  aout[12] = StrToInt(EditOTLzad13 -> Text); break;
                //case 14:   = StrToInt(EditOTLzad14 -> Text); break;
                //case 15:  = StrToInt(EditOTLzad15 -> Text); break;
                //case 16:  = StrToInt(EditOTLzad16 -> Text); break;
                case 17: D_D1   = StrToInt(EditOTLzad17 -> Text); break;
                case 18: D_D2   = StrToInt(EditOTLzad18 -> Text); break;
                case 19: D_D3  	= StrToInt(EditOTLzad19 -> Text); break;
				case 20: D_D4 	= StrToInt(EditOTLzad20 -> Text); break;
                //case 21:  	  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: UVAK_KAM       = StrToInt(EditOTLzad22 -> Text); break;
                case 23: POROG_DAVL     = StrToInt(EditOTLzad23 -> Text); break;
				case 24: UVAKV_KAM      = StrToInt(EditOTLzad24  -> Text); break;
                case 25: UVAKN_KAM      = StrToInt(EditOTLzad25  -> Text); break;
                case 26: UVAK_SHL_MO    = StrToInt(EditOTLzad26  -> Text); break;
                case 27: UVAK_SHL_MN    = StrToInt(EditOTLzad27  -> Text); break;
                case 28: UVAK_ZTMN      = StrToInt(EditOTLzad28  -> Text); break;
                case 29: UATM_D1        = StrToInt(EditOTLzad29  -> Text); break;
                case 30: UATM_D4        = StrToInt(EditOTLzad30  -> Text); break;
            }
        }; break;
        case 9:  // 9 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1: nasmod[0]   = StrToInt(EditOTLzad1  -> Text); break;
                case 2: nasmod[1]   = StrToInt(EditOTLzad2  -> Text); break;
                case 3: nasmod[2]   = StrToInt(EditOTLzad3  -> Text); break;
                case 4: nasmod[3]   = StrToInt(EditOTLzad4  -> Text); break;
                case 5: nasmod[4]   = StrToInt(EditOTLzad5  -> Text); break;
                case 6: nasmod[5]   = StrToInt(EditOTLzad6  -> Text); break;
                case 7: nasmod[6]   = StrToInt(EditOTLzad7  -> Text); break;
                case 8: nasmod[7]   = StrToInt(EditOTLzad8  -> Text); break;
                case 9: nasmod[17]   = StrToInt(EditOTLzad9  -> Text); break;
                case 10: nasmod[18]  = StrToInt(EditOTLzad10 -> Text); break;
                //case 11: nasmod[10] = StrToInt(EditOTLzad11 -> Text); break;
                //case 12: nasmod[11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par[0][0] = StrToInt(EditOTLzad13 -> Text); break;
                case 14: par[0][1] = StrToInt(EditOTLzad14 -> Text); break;
                case 15: par[0][2] = StrToInt(EditOTLzad15 -> Text); break;
				case 16: par[0][3] = StrToInt(EditOTLzad16 -> Text); break;
				case 17: par[0][4] = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par[0][5] = StrToInt(EditOTLzad18 -> Text); break;
                case 19: par[0][6] = StrToInt(EditOTLzad19 -> Text); break;
				case 20: par[0][7] = StrToInt(EditOTLzad20 -> Text); break;
                case 21: par[0][8] = StrToInt(EditOTLzad21 -> Text); break;
                case 22: par[0][9] = StrToInt(EditOTLzad22 -> Text); break;
                case 23: par[0][10] = StrToInt(EditOTLzad23 -> Text); break;
                case 24: par[0][11] = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par[0][12] = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par[0][13] = StrToInt(EditOTLzad26 -> Text); break;
                case 27: par[0][15] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par[0][16] = StrToInt(EditOTLzad28 -> Text); break;
                case 29: par[0][17] = StrToInt(EditOTLzad29 -> Text); break;
				case 30: par[0][18] = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 10:  // 10 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  par[1][0]  = StrToInt(EditOTLzad1  -> Text); break;
				case 2:  par[1][1]  = StrToInt(EditOTLzad2  -> Text); break;
				case 3:  par[1][2]  = StrToInt(EditOTLzad3  -> Text); break;
				case 4:  par[1][3]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  par[1][4]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  par[1][5]  = StrToInt(EditOTLzad6  -> Text); break;
				case 7:  par[1][6]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  par[1][7]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  par[1][8]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: par[1][9]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: par[1][10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12: par[1][11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par[1][12] = StrToInt(EditOTLzad13 -> Text); break;
                case 14: par[1][13] = StrToInt(EditOTLzad14 -> Text); break;
              //case 15: par[][]    = StrToInt(EditOTLzad15 -> Text); break;
			  //case 16: par[][]    = StrToInt(EditOTLzad16 -> Text); break;
			    case 17: par[2][0]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par[2][1]  = StrToInt(EditOTLzad18 -> Text); break;
                case 19: par[2][2]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: par[2][3]  = StrToInt(EditOTLzad20 -> Text); break;
				case 21: par[2][4]  = StrToInt(EditOTLzad21 -> Text); break;
				case 22: par[2][5]  = StrToInt(EditOTLzad22 -> Text); break;
				case 23: par[2][6]  = StrToInt(EditOTLzad23 -> Text); break;
				case 24: par[2][7]  = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par[2][8]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par[2][9]  = StrToInt(EditOTLzad26 -> Text); break;
				case 27: par[2][10] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par[2][11] = StrToInt(EditOTLzad28 -> Text); break;
                case 29: par[2][12] = StrToInt(EditOTLzad29 -> Text); break;
                case 30: par[2][13] = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 11:  // 11 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  par[3][0]  = StrToInt(EditOTLzad1  -> Text); break;
				case 2:  par[3][1]  = StrToInt(EditOTLzad2  -> Text); break;
				case 3:  par[3][2]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  par[3][3]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  par[3][4]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  par[3][5]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  par[3][6]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  par[3][7]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  par[3][8]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: par[3][9]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: par[3][10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12: par[3][11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par[3][12] = StrToInt(EditOTLzad13 -> Text); break;
				case 14: par[3][13] = StrToInt(EditOTLzad14 -> Text); break;
				//case 15: par[][]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: par[4][0]  = StrToInt(EditOTLzad16 -> Text); break;
                case 17: par[4][1]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par[4][2]  = StrToInt(EditOTLzad18 -> Text); break;
				case 19: par[4][3]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: par[4][4]  = StrToInt(EditOTLzad20 -> Text); break;
                case 21: par[4][5]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: par[4][6]  = StrToInt(EditOTLzad22 -> Text); break;
                case 23: par[4][7]  = StrToInt(EditOTLzad23 -> Text); break;
				case 24: par[4][8]  = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par[4][9]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par[4][10] = StrToInt(EditOTLzad26 -> Text); break;
                case 27: par[4][11] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par[4][12] = StrToInt(EditOTLzad28 -> Text); break;
				case 29: par[4][13] = StrToInt(EditOTLzad29 -> Text); break;
                //case 30: par[][]  = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		case 12:  // 12 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  par[5][0]  = StrToInt(EditOTLzad1  -> Text); break;
				case 2:  par[5][1]  = StrToInt(EditOTLzad2  -> Text); break;
				case 3:  par[5][2]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  par[5][3]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  par[5][4]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  par[5][5]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  par[5][6]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  par[5][7]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  par[5][8]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: par[5][9]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: par[5][10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12: par[5][11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par[5][12] = StrToInt(EditOTLzad13 -> Text); break;
				case 14: par[5][13] = StrToInt(EditOTLzad14 -> Text); break;
				//case 15: par[][]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: par[6][0]  = StrToInt(EditOTLzad16 -> Text); break;
                case 17: par[6][1]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par[6][2]  = StrToInt(EditOTLzad18 -> Text); break;
				case 19: par[6][3]  = StrToInt(EditOTLzad19 -> Text); break;
                case 20: par[6][4]  = StrToInt(EditOTLzad20 -> Text); break;
                case 21: par[6][5]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: par[6][6]  = StrToInt(EditOTLzad22 -> Text); break;
                case 23: par[6][7]  = StrToInt(EditOTLzad23 -> Text); break;
				case 24: par[6][8]  = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par[6][9]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par[6][10] = StrToInt(EditOTLzad26 -> Text); break;
                case 27: par[6][11] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par[6][12] = StrToInt(EditOTLzad28 -> Text); break;
				case 29: par[6][13] = StrToInt(EditOTLzad29 -> Text); break;
                //case 30: par[][]  = StrToInt(EditOTLzad30 -> Text); break;
            }
		}; break;
        case 13:  // 13 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  par[7][0]  = StrToInt(EditOTLzad1  -> Text); break;
				case 2:  par[7][1]  = StrToInt(EditOTLzad2  -> Text); break;
				case 3:  par[7][2]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  par[7][3]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  par[7][4]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  par[7][5]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  par[7][6]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  par[7][7]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  par[7][8]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: par[7][9]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: par[7][10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12: par[7][11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par[7][12] = StrToInt(EditOTLzad13 -> Text); break;
				case 14: par[7][13] = StrToInt(EditOTLzad14 -> Text); break;
				//case 15: par[][]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: par[8][0]  = StrToInt(EditOTLzad16 -> Text); break;
                case 17: par[8][1]  = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par[8][2]  = StrToInt(EditOTLzad18 -> Text); break;
				case 19: par[8][3]  = StrToInt(EditOTLzad19  -> Text); break;
                case 20: par[8][4]  = StrToInt(EditOTLzad20 -> Text); break;
                case 21: par[8][5]  = StrToInt(EditOTLzad21 -> Text); break;
                case 22: par[8][6]  = StrToInt(EditOTLzad22 -> Text); break;
                case 23: par[8][7]  = StrToInt(EditOTLzad23 -> Text); break;
				case 24: par[8][8]  = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par[8][9]  = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par[8][10] = StrToInt(EditOTLzad26 -> Text); break;
                case 27: par[8][11] = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par[8][12] = StrToInt(EditOTLzad28 -> Text); break;
				case 29: par[8][13] = StrToInt(EditOTLzad29 -> Text); break;
                //case 30: par[][]  = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		case 14:  // 14 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  par[9][0]  = StrToInt(EditOTLzad1  -> Text); break;
				case 2:  par[9][1]  = StrToInt(EditOTLzad2  -> Text); break;
				case 3:  par[9][2]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  par[9][3]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  par[9][4]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  par[9][5]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  par[9][6]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  par[9][7]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  par[9][8]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: par[9][9]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: par[9][10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12: par[9][11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par[9][12] = StrToInt(EditOTLzad13 -> Text); break;
				case 14: par[9][13] = StrToInt(EditOTLzad14 -> Text); break;
				//case 15: par[][]  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: par[10][0]   = StrToInt(EditOTLzad16 -> Text); break;
                case 17: par[10][1]   = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par[10][2]   = StrToInt(EditOTLzad18 -> Text); break;
				case 19: par[10][3]   = StrToInt(EditOTLzad19  -> Text); break;
                case 20: par[10][4]   = StrToInt(EditOTLzad20 -> Text); break;
                case 21: par[10][5]   = StrToInt(EditOTLzad21 -> Text); break;
                case 22: par[10][6]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: par[10][7]   = StrToInt(EditOTLzad23 -> Text); break;
				case 24: par[10][8]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par[10][9]   = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par[10][10]  = StrToInt(EditOTLzad26 -> Text); break;
                case 27: par[10][11]  = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par[10][12]  = StrToInt(EditOTLzad28 -> Text); break;
				case 29: par[10][13]  = StrToInt(EditOTLzad29 -> Text); break;
                //case 30: par[][]  = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		case 15:  // 15 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  par_n[0]  = StrToInt(EditOTLzad1  -> Text); break;
				case 2:  par_n[1]  = StrToInt(EditOTLzad2  -> Text); break;
				case 3:  par_n[2]  = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  par_n[3]  = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  par_n[4]  = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  par_n[5]  = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  par_n[6]  = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  par_n[7]  = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  par_n[8]  = StrToInt(EditOTLzad9  -> Text); break;
                case 10: par_n[9]  = StrToInt(EditOTLzad10 -> Text); break;
                case 11: par_n[10] = StrToInt(EditOTLzad11 -> Text); break;
                case 12: par_n[11] = StrToInt(EditOTLzad12 -> Text); break;
                case 13: par_n[12] = StrToInt(EditOTLzad13 -> Text); break;
				case 14: par_n[13] = StrToInt(EditOTLzad14 -> Text); break;
				case 15: par_n[14] = StrToInt(EditOTLzad15 -> Text); break;
                case 16: par_n[15] = StrToInt(EditOTLzad16 -> Text); break;
                case 17: par_n[16] = StrToInt(EditOTLzad17 -> Text); break;
                case 18: par_n[17] = StrToInt(EditOTLzad18 -> Text); break;
				case 19: par_n[18] = StrToInt(EditOTLzad19  -> Text); break;
                case 20: par_n[19] = StrToInt(EditOTLzad20 -> Text); break;
                case 21: par_n[20] = StrToInt(EditOTLzad21 -> Text); break;
                case 22: par_n[21]   = StrToInt(EditOTLzad22 -> Text); break;
                case 23: par_n[22]   = StrToInt(EditOTLzad23 -> Text); break;
				case 24: par_n[23]   = StrToInt(EditOTLzad24 -> Text); break;
                case 25: par_t[0]   = StrToInt(EditOTLzad25 -> Text); break;
                case 26: par_t[1]   = StrToInt(EditOTLzad26 -> Text); break;
                case 27: par_t[2]   = StrToInt(EditOTLzad27 -> Text); break;
                case 28: par_t[3]   = StrToInt(EditOTLzad28 -> Text); break;
				case 29: par_t[4]   = StrToInt(EditOTLzad29 -> Text); break;
                case 30: par_t[5]   = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 16:  // 16 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  T_ZAD_DVS     = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  T_PROC  	   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  T_KTMN_RAZGON = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  T_KKAM_V      = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  T_VODA        = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  T_STOP        = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  T_DVIJ 	   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  T_KDVIJ_SU    = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  T_KSUT        = StrToInt(EditOTLzad9  -> Text); break;
                case 10: T_KKAM        = StrToInt(EditOTLzad10 -> Text); break;
                case 11: T_KTMN        = StrToInt(EditOTLzad11 -> Text); break;
                case 12: T_KSHL        = StrToInt(EditOTLzad12 -> Text); break;
                case 13: T_KNAP        = StrToInt(EditOTLzad13 -> Text); break;
                case 14: T_NAPUSK      = StrToInt(EditOTLzad14 -> Text); break;
                case 15: T_VHG         = StrToInt(EditOTLzad15 -> Text); break;
                case 16: T_KSHL_MO    = StrToInt(EditOTLzad16 -> Text); break;
                //case 17: 	   = StrToInt(EditOTLzad17 -> Text); break;
                //case 18:     = StrToInt(EditOTLzad18 -> Text); break;
                //case 19:  	   = StrToInt(EditOTLzad19 -> Text); break;
                //case 20:  	   = StrToInt(EditOTLzad20 -> Text); break;
                //case 21:  = StrToInt(EditOTLzad21 -> Text); break;
                //case 22:  = StrToInt(EditOTLzad22 -> Text); break;
				//case 23: 	   = StrToInt(EditOTLzad23 -> Text); break;
				//case 24: 	   = StrToInt(EditOTLzad24 -> Text); break;
				//case 23: par[]   	   = StrToInt(EditOTLzad23 -> Text); break;
				//case 24: par[]       = StrToInt(EditOTLzad24 -> Text); break;
                //case 25: par[]       = StrToInt(EditOTLzad25 -> Text); break;
                //case 26: par[]       = StrToInt(EditOTLzad26 -> Text); break;
                //case 27: par[]       = StrToInt(EditOTLzad27 -> Text); break;
                //case 28: par[]       = StrToInt(EditOTLzad28 -> Text); break;
				//case 29: par[]       = StrToInt(EditOTLzad29 -> Text); break;
                //case 30: par[]       = StrToInt(EditOTLzad30 -> Text); break;
			}
        }; break;
        case 17:  // 17 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1: CT_VHG           = StrToInt(EditOTLzad1  -> Text); break;
                case 2: CT_VODA_IP       = StrToInt(EditOTLzad2  -> Text); break;
                //case 3: CT_PER           = StrToInt(EditOTLzad3  -> Text); break;
               // case 4: CT_POV           = StrToInt(EditOTLzad4  -> Text); break;
                //case 5: CT_KAS           = StrToInt(EditOTLzad5  -> Text); break;
                case 6: CT_TEMP          = StrToInt(EditOTLzad6  -> Text); break;
                case 7: CT_DVIJ_GIR_g    = StrToInt(EditOTLzad7  -> Text); break;
                case 8: CT_DVIJ_GIR_t    = StrToInt(EditOTLzad8  -> Text); break;
				case 9: CT_SUT_g         = StrToInt(EditOTLzad9  -> Text); break;
                case 10: CT_SUT_t 	     = StrToInt(EditOTLzad10 -> Text); break;
                //case 11:    = StrToInt(EditOTLzad11 -> Text); break;
                //case 12:    = StrToInt(EditOTLzad12 -> Text); break;
                //case 13:    = StrToInt(EditOTLzad13 -> Text); break;
                //case 14:    = StrToInt(EditOTLzad14 -> Text); break;
                //case 15:    = StrToInt(EditOTLzad15 -> Text); break;
                case 16: CT_T1      = StrToInt(EditOTLzad16 -> Text); break;
                case 17: CT_T20     = StrToInt(EditOTLzad17 -> Text); break;
                case 18: CT_1       = StrToInt(EditOTLzad18 -> Text); break;
                case 19: CT_2       = StrToInt(EditOTLzad19 -> Text); break;
                case 20: CT_3       = StrToInt(EditOTLzad20 -> Text); break;
				case 21: CT_4       = StrToInt(EditOTLzad21 -> Text); break;
				case 22: CT_5	    = StrToInt(EditOTLzad22 -> Text); break;
				case 23: CT_6       = StrToInt(EditOTLzad23 -> Text); break;
				case 24: CT_7       = StrToInt(EditOTLzad24 -> Text); break;
                case 25: CT_9       = StrToInt(EditOTLzad25 -> Text); break;
                case 26: CT_17      = StrToInt(EditOTLzad26 -> Text); break;
                case 27: CT17K1     = StrToInt(EditOTLzad27 -> Text); break;
                case 28: CT_29      = StrToInt(EditOTLzad28 -> Text); break;
				case 29: CT29K1 	= StrToInt(EditOTLzad29 -> Text); break;
                case 30: CT_30T     = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;

        case 18:  // 18 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1: PR_KLASTER  = StrToInt(EditOTLzad1  -> Text); break;
                case 2: otvet       = StrToInt(EditOTLzad2  -> Text); break;
                case 3: zshr3       = StrToInt(EditOTLzad3  -> Text); break;
                case 4: PR_TRTEST   = StrToInt(EditOTLzad4  -> Text); break;
                case 5: PR_OTK      = StrToInt(EditOTLzad5  -> Text); break;
                case 6: PR_FK_KAM   = StrToInt(EditOTLzad6  -> Text); break;
                case 7: PR_NASOS    = StrToInt(EditOTLzad7  -> Text); break;
                case 8: PR_NALADKA  = StrToInt(EditOTLzad8  -> Text); break;
				case 9: N_PL        = StrToInt(EditOTLzad9  -> Text); break;
                case 10: N_ST_MAX   = StrToInt(EditOTLzad10 -> Text); break;
                case 11: N_ST       = StrToInt(EditOTLzad11 -> Text); break;
                case 12: N_ZICL     = StrToInt(EditOTLzad12 -> Text); break;
                case 13: ZN_ST      = StrToInt(EditOTLzad13 -> Text); break;
                //case 14:    = StrToInt(EditOTLzad14 -> Text); break;
                case 15: KOM_MOD = StrToInt(EditOTLzad15 -> Text); break;
                case 16: OTVET_MOD = StrToInt(EditOTLzad16 -> Text); break;
                //case 17:      = StrToInt(EditOTLzad17 -> Text); break;
                //case 18:        = StrToInt(EditOTLzad18 -> Text); break;
                //case 19:        = StrToInt(EditOTLzad19 -> Text); break;
                //case 20:        = StrToInt(EditOTLzad20 -> Text); break;
				//case 21:        = StrToInt(EditOTLzad21 -> Text); break;
				//case 22: 	    = StrToInt(EditOTLzad22 -> Text); break;
				//case 23:        = StrToInt(EditOTLzad23 -> Text); break;
				//case 24:        = StrToInt(EditOTLzad24 -> Text); break;
                //case 25:        = StrToInt(EditOTLzad25 -> Text); break;
                //case 26:       = StrToInt(EditOTLzad26 -> Text); break;
                //case 27:      = StrToInt(EditOTLzad27 -> Text); break;
                //case 28:       = StrToInt(EditOTLzad28 -> Text); break;
				//case 29:  	= StrToInt(EditOTLzad29 -> Text); break;
                //case 30:      = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;

        case 19:  // 19 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  DATA_DZASL       = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  PAR_DZASL   	   = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  ZPAR_DZASL    = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  X_TDZASL     = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  VRDZASL     = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  E_TDZASL       = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  DELDZASL      = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  LIM1DZASL        = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  LIM2DZASL         = StrToInt(EditOTLzad9  -> Text); break;
                case 10: T_VRDZASL        = StrToInt(EditOTLzad10 -> Text); break;
                case 11: T_KDZASL        = StrToInt(EditOTLzad11 -> Text); break;
                case 12: DOPDZASL       = StrToInt(EditOTLzad12 -> Text); break;
                case 13: KOM_DZASL       = StrToInt(EditOTLzad13 -> Text); break;
                case 14: TEK_DAVL_DZASL       = StrToInt(EditOTLzad14 -> Text); break;
                case 15: TEK_POZ_DZASL        = StrToInt(EditOTLzad15 -> Text); break;
                case 16: PR_DZASL        = StrToInt(EditOTLzad16 -> Text); break;
                case 17: CT_DZASL	   = StrToInt(EditOTLzad17 -> Text); break;
                case 18: OTVET_DZASL = StrToInt(EditOTLzad18 -> Text); break;
                case 19: DAVL_DZASL  = StrToInt(EditOTLzad19 -> Text); break;
                //case 20: CT35K1 	   = StrToInt(EditOTLzad20 -> Text); break;
				//case 21:    = StrToInt(EditOTLzad21  -> Text); break;
                //case 22:    = StrToInt(EditOTLzad22  -> Text); break;
                //case 23:         = StrToInt(EditOTLzad23  -> Text); break;
                //case 24:      = StrToInt(EditOTLzad24  -> Text); break;
                //case 25:      = StrToInt(EditOTLzad25  -> Text); break;
                //case 26:   	   = StrToInt(EditOTLzad26  -> Text); break;
                //case 27:       = StrToInt(EditOTLzad27  -> Text); break;
                //case 28:       = StrToInt(EditOTLzad28  -> Text); break;
                //case 29:       = StrToInt(EditOTLzad29  -> Text); break;
                //case 30: CT17K1      = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        case 20:  // 20 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  prDvijGir_g       = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  prDvijGir_t  = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  DOP_SU   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  T_SM_NAPR		 = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  DOP_DV_IP      = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  klGir_gV   	 = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  klGir_gN    	 = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  klGir_tV  	 = StrToInt(EditOTLzad8  -> Text); break;
                case 9:  klGir_tN   = StrToInt(EditOTLzad9  -> Text); break;
                case 10: VRGIR     = StrToInt(EditOTLzad10 -> Text); break;
                case 11: K_SOGL_GIR     = StrToInt(EditOTLzad11 -> Text); break;
                case 12: NAPRS_GIR     = StrToInt(EditOTLzad12 -> Text); break;
                case 13: X_TGIR      = StrToInt(EditOTLzad13 -> Text); break;
                case 14: E_TGIR  = StrToInt(EditOTLzad14 -> Text); break;
                case 15: DELGIR  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: DOPGIR	 = StrToInt(EditOTLzad16 -> Text); break;
                case 17: PAR_GIR 	 = StrToInt(EditOTLzad17 -> Text); break;
                case 18: N_TEK_GIR	 = StrToInt(EditOTLzad18 -> Text); break;
                case 19: LIM1GIR 	 = StrToInt(EditOTLzad19 -> Text); break;
				case 20: LIM2GIR    = StrToInt(EditOTLzad20 -> Text); break;
                case 21: T_VRGIR		 = StrToInt(EditOTLzad21 -> Text); break;
                case 22: T_KGIR	     = StrToInt(EditOTLzad22 -> Text); break;
                case 23: N_PRED_GIR	 	 = StrToInt(EditOTLzad23 -> Text); break;
                //case 24:      = StrToInt(EditOTLzad24 -> Text); break;
                //case 25:   	 = StrToInt(EditOTLzad25 -> Text); break;
                //case 26:     = StrToInt(EditOTLzad26 -> Text); break;
                //case 27:   	 = StrToInt(EditOTLzad27 -> Text); break;
                //case 28:     = StrToInt(EditOTLzad28 -> Text); break;
				//case 29:      = StrToInt(EditOTLzad29 -> Text); break;
                //case 30:   = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;

        case 21:  // 21 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1: PR_PER       = StrToInt(EditOTLzad1  -> Text); break;
                case 2: KOM_PER      = StrToInt(EditOTLzad2  -> Text); break;
                case 3: OTVET_PER    = StrToInt(EditOTLzad3  -> Text); break;
                case 4: V_PER 		 = StrToInt(EditOTLzad4  -> Text); break;
                case 5: HOME_PER     = StrToInt(EditOTLzad5  -> Text); break;
                case 6: TEK_OTN_PER  = StrToInt(EditOTLzad6  -> Text); break;
                case 7: TEK_ABS_PER  = StrToInt(EditOTLzad7  -> Text); break;
                case 8: PUT_PER   	 = StrToInt(EditOTLzad8  -> Text); break;
                case 9: CT_PER       = StrToInt(EditOTLzad9  -> Text); break;
                //case 10:      = StrToInt(EditOTLzad10 -> Text); break;
                //case 11:     = StrToInt(EditOTLzad11 -> Text); break;
                case 12: PR_KAS      = StrToInt(EditOTLzad12 -> Text); break;
                case 13: KOM_KAS     = StrToInt(EditOTLzad13 -> Text); break;
                case 14: OTVET_KAS   = StrToInt(EditOTLzad14 -> Text); break;
                case 15: V_KAS       = StrToInt(EditOTLzad15 -> Text); break;
                case 16: HOME_KAS	 = StrToInt(EditOTLzad16 -> Text); break;
                case 17: TEK_OTN_KAS = StrToInt(EditOTLzad17 -> Text); break;
                case 18: TEK_ABS_KAS = StrToInt(EditOTLzad18 -> Text); break;
                case 19: PUT_KAS 	 = StrToInt(EditOTLzad19 -> Text); break;
				case 20: CT_KAS      = StrToInt(EditOTLzad20 -> Text); break;
                //case 21: 		 = StrToInt(EditOTLzad21 -> Text); break;
                case 22: PR_POV	     = StrToInt(EditOTLzad22 -> Text); break;
                case 23: KOM_POV	 	 = StrToInt(EditOTLzad23 -> Text); break;
                case 24: OTVET_POV     = StrToInt(EditOTLzad24 -> Text); break;
                case 25: V_POV  	 = StrToInt(EditOTLzad25 -> Text); break;
                case 26: HOME_POV    = StrToInt(EditOTLzad26 -> Text); break;
                case 27: TEK_OTN_POV  	 = StrToInt(EditOTLzad27 -> Text); break;
                case 28: TEK_ABS_POV    = StrToInt(EditOTLzad28 -> Text); break;
				case 29: PUT_POV     = StrToInt(EditOTLzad29 -> Text); break;
                case 30: CT_POV  = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;

        case 22:  // 22 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  ObjBPN[0]->prBPN 	 = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  ObjBPN[0]->vBPN     = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  ObjBPN[0]->zadBPN   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  ObjBPN[0]->tekBPN       = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  ObjBPN[1]->prBPN    = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  ObjBPN[1]->vBPN   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  ObjBPN[1]->zadBPN   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  ObjBPN[1]->tekBPN = StrToInt(EditOTLzad8  -> Text); break;
				case 9:  ObjBPN[4]->prBPN = StrToInt(EditOTLzad9  -> Text); break;
				case 10: ObjBPN[4]->vBPN       = StrToInt(EditOTLzad10 -> Text); break;
                case 11: ObjBPN[4]->zadBPN        = StrToInt(EditOTLzad11 -> Text); break;
                case 12: ObjBPN[4]->tekBPN        = StrToInt(EditOTLzad12 -> Text); break;
                case 13: ObjBPN[5]->prBPN      = StrToInt(EditOTLzad13 -> Text); break;
                case 14: ObjBPN[5]->vBPN = StrToInt(EditOTLzad14 -> Text); break;
				case 15: ObjBPN[5]->zadBPN  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: ObjBPN[5]->tekBPN   	 = StrToInt(EditOTLzad16 -> Text); break;
                case 17: ObjBPN[6]->prBPN 	     = StrToInt(EditOTLzad17 -> Text); break;
                case 18: ObjBPN[6]->vBPN    = StrToInt(EditOTLzad18 -> Text); break;
                case 19: ObjBPN[6]->zadBPN 	 = StrToInt(EditOTLzad19 -> Text); break;
                case 20: ObjBPN[6]->tekBPN        = StrToInt(EditOTLzad20 -> Text); break;
				case 21: ObjBPN[7]->prBPN     = StrToInt(EditOTLzad21 -> Text); break;
                case 22: ObjBPN[7]->vBPN       = StrToInt(EditOTLzad22 -> Text); break;
                case 23: ObjBPN[7]->zadBPN      = StrToInt(EditOTLzad23 -> Text); break;
                case 24: ObjBPN[7]->tekBPN        = StrToInt(EditOTLzad24 -> Text); break;
                case 25: ObjBPN[8]->prBPN         = StrToInt(EditOTLzad25 -> Text); break;
                case 26: ObjBPN[8]->vBPN        = StrToInt(EditOTLzad26 -> Text); break;
                case 27: ObjBPN[8]->zadBPN        = StrToInt(EditOTLzad27 -> Text); break;
                case 28: ObjBPN[8]->tekBPN       = StrToInt(EditOTLzad28 -> Text); break;
				//case 29: LIM2ZSL       = StrToInt(EditOTLzad29 -> Text); break;
				//case 30: T_VRZSL       = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		case 23:  // 23 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  ObjBPN[9]->prBPN 	 = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  ObjBPN[9]->vBPN     = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  ObjBPN[9]->zadBPN   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  ObjBPN[9]->tekBPN       = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  ObjBPN[10]->prBPN    = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  ObjBPN[10]->vBPN   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  ObjBPN[10]->zadBPN   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  ObjBPN[10]->tekBPN = StrToInt(EditOTLzad8  -> Text); break;
				case 9:  ObjBPN[11]->prBPN = StrToInt(EditOTLzad9  -> Text); break;
				case 10: ObjBPN[11]->vBPN       = StrToInt(EditOTLzad10 -> Text); break;
                case 11: ObjBPN[11]->zadBPN        = StrToInt(EditOTLzad11 -> Text); break;
                case 12: ObjBPN[11]->tekBPN        = StrToInt(EditOTLzad12 -> Text); break;
                case 13: ObjBPN[12]->prBPN      = StrToInt(EditOTLzad13 -> Text); break;
                case 14: ObjBPN[12]->vBPN = StrToInt(EditOTLzad14 -> Text); break;
				case 15: ObjBPN[12]->zadBPN  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: ObjBPN[12]->tekBPN   	 = StrToInt(EditOTLzad16 -> Text); break;
                case 17: ObjBPN[13]->prBPN 	     = StrToInt(EditOTLzad17 -> Text); break;
                case 18: ObjBPN[13]->vBPN    = StrToInt(EditOTLzad18 -> Text); break;
                case 19: ObjBPN[13]->zadBPN 	 = StrToInt(EditOTLzad19 -> Text); break;
                case 20: ObjBPN[13]->tekBPN        = StrToInt(EditOTLzad20 -> Text); break;
				case 21: ObjBPN[14]->prBPN     = StrToInt(EditOTLzad21 -> Text); break;
                case 22: ObjBPN[14]->vBPN       = StrToInt(EditOTLzad22 -> Text); break;
                case 23: ObjBPN[14]->zadBPN      = StrToInt(EditOTLzad23 -> Text); break;
                case 24: ObjBPN[14]->tekBPN        = StrToInt(EditOTLzad24 -> Text); break;
                case 25: ObjBPN[15]->prBPN         = StrToInt(EditOTLzad25 -> Text); break;
                case 26: ObjBPN[15]->vBPN        = StrToInt(EditOTLzad26 -> Text); break;
                case 27: ObjBPN[15]->zadBPN        = StrToInt(EditOTLzad27 -> Text); break;
                case 28: ObjBPN[15]->tekBPN       = StrToInt(EditOTLzad28 -> Text); break;
				//case 29: LIM2ZSL       = StrToInt(EditOTLzad29 -> Text); break;
				//case 30: T_VRZSL       = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
		case 24:  // 24 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  ObjBPN[16]->prBPN 	 = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  ObjBPN[16]->vBPN     = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  ObjBPN[16]->zadBPN   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  ObjBPN[16]->tekBPN       = StrToInt(EditOTLzad4  -> Text); break;
                case 5:  ObjBPN[17]->prBPN    = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  ObjBPN[17]->vBPN   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  ObjBPN[17]->zadBPN   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  ObjBPN[17]->tekBPN = StrToInt(EditOTLzad8  -> Text); break;
				case 9:  ObjBPN[18]->prBPN = StrToInt(EditOTLzad9  -> Text); break;
				case 10: ObjBPN[18]->vBPN       = StrToInt(EditOTLzad10 -> Text); break;
                case 11: ObjBPN[18]->zadBPN        = StrToInt(EditOTLzad11 -> Text); break;
                case 12: ObjBPN[18]->tekBPN        = StrToInt(EditOTLzad12 -> Text); break;
                case 13: ObjBPN[19]->prBPN      = StrToInt(EditOTLzad13 -> Text); break;
                case 14: ObjBPN[19]->vBPN = StrToInt(EditOTLzad14 -> Text); break;
				case 15: ObjBPN[19]->zadBPN  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: ObjBPN[19]->tekBPN   	 = StrToInt(EditOTLzad16 -> Text); break;
                case 17: ObjBPN[20]->tekBPN 	     = StrToInt(EditOTLzad17 -> Text); break;
                case 18: ObjBPN[20]->zadBPN    = StrToInt(EditOTLzad18 -> Text); break;
                case 19: ObjBPN[20]->vBPN 	 = StrToInt(EditOTLzad19 -> Text); break;
                case 20: ObjBPN[20]->prBPN        = StrToInt(EditOTLzad20 -> Text); break;
				case 21: ObjBPN[21]->prBPN     = StrToInt(EditOTLzad21 -> Text); break;
                case 22: ObjBPN[21]->vBPN       = StrToInt(EditOTLzad22 -> Text); break;
                case 23: ObjBPN[21]->zadBPN      = StrToInt(EditOTLzad23 -> Text); break;
                case 24: ObjBPN[21]->tekBPN        = StrToInt(EditOTLzad24 -> Text); break;
                case 25: ObjBPN[22]->prBPN          = StrToInt(EditOTLzad25 -> Text); break;
                case 26: ObjBPN[22]->vBPN         = StrToInt(EditOTLzad26 -> Text); break;
                case 27: ObjBPN[22]->zadBPN         = StrToInt(EditOTLzad27 -> Text); break;
                case 28: ObjBPN[22]->tekBPN       = StrToInt(EditOTLzad28 -> Text); break;
				//case 29: LIM2ZSL       = StrToInt(EditOTLzad29 -> Text); break;
				//case 30: T_VRZSL       = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
        	case 25:  // 25 ��������
        {   switch(StrToInt(((TButton*)Sender)->Hint))
            {   case 1:  ObjBPN[23]->prBPN 	 = StrToInt(EditOTLzad1  -> Text); break;
                case 2:  ObjBPN[23]->vBPN     = StrToInt(EditOTLzad2  -> Text); break;
                case 3:  ObjBPN[23]->zadBPN   = StrToInt(EditOTLzad3  -> Text); break;
                case 4:  ObjBPN[23]->tekBPN       = StrToInt(EditOTLzad4  -> Text); break;
                /*case 5:  ObjBPN[17]->prBPN    = StrToInt(EditOTLzad5  -> Text); break;
                case 6:  ObjBPN[17]->vBPN   = StrToInt(EditOTLzad6  -> Text); break;
                case 7:  ObjBPN[17]->zadBPN   = StrToInt(EditOTLzad7  -> Text); break;
                case 8:  ObjBPN[17]->tekBPN = StrToInt(EditOTLzad8  -> Text); break;
				case 9:  ObjBPN[18]->prBPN = StrToInt(EditOTLzad9  -> Text); break;
				case 10: ObjBPN[18]->vBPN       = StrToInt(EditOTLzad10 -> Text); break;
                case 11: ObjBPN[18]->zadBPN        = StrToInt(EditOTLzad11 -> Text); break;
                case 12: ObjBPN[18]->tekBPN        = StrToInt(EditOTLzad12 -> Text); break;
                case 13: ObjBPN[19]->prBPN      = StrToInt(EditOTLzad13 -> Text); break;
                case 14: ObjBPN[19]->vBPN = StrToInt(EditOTLzad14 -> Text); break;
				case 15: ObjBPN[19]->zadBPN  = StrToInt(EditOTLzad15 -> Text); break;
                case 16: ObjBPN[19]->tekBPN   	 = StrToInt(EditOTLzad16 -> Text); break;
                case 17: ObjBPN[20]->tekBPN 	     = StrToInt(EditOTLzad17 -> Text); break;
                case 18: ObjBPN[20]->zadBPN    = StrToInt(EditOTLzad18 -> Text); break;
                case 19: ObjBPN[20]->vBPN 	 = StrToInt(EditOTLzad19 -> Text); break;
                case 20: ObjBPN[20]->prBPN        = StrToInt(EditOTLzad20 -> Text); break;
				case 21: ObjBPN[21]->prBPN     = StrToInt(EditOTLzad21 -> Text); break;
                case 22: ObjBPN[21]->vBPN       = StrToInt(EditOTLzad22 -> Text); break;
                case 23: ObjBPN[21]->zadBPN      = StrToInt(EditOTLzad23 -> Text); break;
                case 24: ObjBPN[21]->tekBPN        = StrToInt(EditOTLzad24 -> Text); break;
                case 25: ObjBPN[22]->prBPN          = StrToInt(EditOTLzad25 -> Text); break;
                case 26: ObjBPN[22]->vBPN         = StrToInt(EditOTLzad26 -> Text); break;
                case 27: ObjBPN[22]->zadBPN         = StrToInt(EditOTLzad27 -> Text); break;
                case 28: ObjBPN[22]->tekBPN       = StrToInt(EditOTLzad28 -> Text); break; */
				//case 29: LIM2ZSL       = StrToInt(EditOTLzad29 -> Text); break;
				//case 30: T_VRZSL       = StrToInt(EditOTLzad30 -> Text); break;
            }
        }; break;
    }
}
///////////////////////////////////////////////////////////////////////////////////

//���������� ��������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� �������� �1--//
void __fastcall TForm1::SBD1DebugChange(TObject *Sender)
{   // �������� ��� ��������
    EdtD1Code -> Text = IntToStr(SBD1Debug->Position);
    // ����������� �������� ��������
    EdtD1Davl -> Text = FloatToStrF(133.3*pow(10,(float)SBD1Debug->Position/1000.0-6.0),ffExponent,3,8);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� �������� �1--//
void __fastcall TForm1::EdtD1CodeChange(TObject *Sender)
{   // �������� ��� ��������
    SBD1Debug -> Position = StrToInt(EdtD1Code->Text);
    // ����������� �������� ��������
    EdtD1Davl -> Text = FloatToStrF(133.3*pow(10,(float)SBD1Debug->Position/1000.0-6.0),ffExponent,3,8);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� �������� �2--//
void __fastcall TForm1::SBD2DebugChange(TObject *Sender)
{   // �������� ��� ��������
    EdtD2Code -> Text = IntToStr(SBD2Debug->Position);
    // ����������� �������� ��������
    EdtD2Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD2Debug->Position)/1000.0-6.8)/0.6),ffExponent,3,8);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������� �������� �2--//
void __fastcall TForm1::EdtD2CodeChange(TObject *Sender)
{   // �������� ��� ��������
    SBD2Debug -> Position = StrToInt(EdtD2Code->Text);
    // ����������� �������� ��������
   EdtD2Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD2Debug->Position)/1000.0-6.8)/0.6),ffExponent,3,8);
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
 	// 1 - ���������� ������� ��������������� ������� ������
    EditNastrIn0 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[0])/1000.0-6.8)/0.6),ffExponent,3,8);
	// 2 - ������� ������(������� ������)
    EditNastrIn1 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[1])/1000.0-6.8)/0.6),ffExponent,3,8);
	// 3 - ���������������� �� ���������?
    EditNastrIn2 -> Text = ( nasmod[3] ? "��" : "���" );
	// 4 - ���������� ��������� ��
    EditNastrIn3 -> Text = IntToStr(int(float(nasmod[4]-8192.0-820.0)/205.0+1.5));
	// 5 - ���������� ����������� ������������ ��
    EditNastrIn4 -> Text = IntToStr(int(1000.0/float(nasmod[5])+0.5));
	// 6 - �������� �� �������� ������ ����� ���������������� ��������
    EditNastrIn5 -> Text = IntToStr( nasmod[14] );
	// 7 - ������� ������� ������ ����� ���������������� ��������
    EditNastrIn6 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[7])/1000.0-6.8)/0.6),ffExponent,3,8);
    // 8 - ������� ���� � ����������� ���������������� ��������
    EditNastrIn7 -> Text = ( nasmod[8] ? "��" : "���" );
    //������������ ���� � ����������
    EditNastrIn17 -> Text = ( nasmod[17] ? "��" : "���" );
    //��������������� ������� � ���������� � ������?
    EditNastrIn18 -> Text = ( nasmod[18] ? "��" : "���" );
}

//---------------------------------------------------------------------------
//--������������� �������� ����������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrDaClick(TObject *Sender)
{
    // ������ ������
    PanelParNastr -> Visible = false;

    // 1 - ���������� ������� ��������������� ������� ������
    nasmod[0] = int((0.6*log10(StrToFloat(EditNastrTo0->Text)/100.0)+6.8)*1000.0);
    // 2 - ������� ������(������� ������)
    nasmod[1] = int((0.6*log10(StrToFloat(EditNastrTo1->Text)/100.0)+6.8)*1000.0);
    // 3 - ���������������� �� ���������?
    EditNastrTo2 -> Text == "��" ? nasmod[3] = 1 : nasmod[3] = 0;
    // 4 - ���������� ��������� ��
    nasmod[4] = 8192 + 820 + (StrToInt(EditNastrTo3->Text)-1)*205;
    // 5 - ���������� ����������� ������������ ��
    nasmod[5] = 1000/StrToInt(EditNastrTo4->Text);
    // 6 - �������� �� �������� ������ ����� ���������������� ��������
    nasmod[14] = StrToInt(EditNastrTo5->Text);
    // 7 - ������� ������� ������ ����� ���������������� ��������
    nasmod[7] = int((0.6*log10(StrToFloat(EditNastrTo6->Text)/100.0)+6.8)*1000.0);
    // 8 - ������� ���� � ����������� ���������������� ��������
    EditNastrTo7 -> Text == "��" ? nasmod[8] = 1 : nasmod[8] = 0;
    //������������ ���� � ����������
    EditNastrTo17 -> Text == "��" ? nasmod[17] = 1 : nasmod[17] = 0;
    //��������������� ������� � ���������� � ������?
    EditNastrTo18 -> Text == "��" ? nasmod[18] = 1 : nasmod[18] = 0;
   MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    // ��������� �������� � ������
      MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time -> Caption + " : �������� �������� ������������ ������� : ");
    if ( EditNastrTo0 -> Text != EditNastrIn0 -> Text )
        MemoStat -> Lines -> Add( "���������� ������� ��������������� ������� ������ : " + EditNastrIn0 -> Text + " -> " + EditNastrTo0 -> Text );
    if ( EditNastrTo1 -> Text != EditNastrIn1 -> Text )
        MemoStat -> Lines -> Add( "������� ������(������� ������) : " + EditNastrIn1 -> Text + " -> " + EditNastrTo1 -> Text );
    if ( EditNastrTo2 -> Text != EditNastrIn2 -> Text )
        MemoStat -> Lines -> Add( "���������������� �� ���������? : " + EditNastrIn2 -> Text + " -> " + EditNastrTo2 -> Text );
    if ( EditNastrTo3 -> Text != EditNastrIn3 -> Text )
        MemoStat -> Lines -> Add( "���������� ��������� �� : " + EditNastrIn3 -> Text + " -> " + EditNastrTo3 -> Text );
    if ( EditNastrTo4 -> Text != EditNastrIn4 -> Text )
        MemoStat -> Lines -> Add( "���������� ����������� ������������ �� : " + EditNastrIn4 -> Text + " -> " + EditNastrTo4 -> Text );
    if ( EditNastrTo5 -> Text != EditNastrIn5 -> Text )
        MemoStat -> Lines -> Add( "�������� �� �������� ������ ����� ���������������� �������� : " + EditNastrIn5 -> Text + " -> " + EditNastrTo5 -> Text );
    if ( EditNastrTo6 -> Text != EditNastrIn6 -> Text )
        MemoStat -> Lines -> Add( "������� ������� ������ ����� ���������������� �������� : " + EditNastrIn6 -> Text + " -> " + EditNastrTo6 -> Text );
    if ( EditNastrTo7 -> Text != EditNastrIn7 -> Text )
        MemoStat -> Lines -> Add( "������� ���� � ����������� ���������������� �������� : " + EditNastrIn7 -> Text + " -> " + EditNastrTo7 -> Text );
    if ( EditNastrTo17 -> Text != EditNastrIn17 -> Text )
        MemoStat -> Lines -> Add( "������������ ���� � ���������? : " + EditNastrIn17 -> Text + " -> " + EditNastrTo17 -> Text );
    if ( EditNastrTo18 -> Text != EditNastrIn18 -> Text )
        MemoStat -> Lines -> Add( "��������������� ������� � ���������� � ������? : " + EditNastrIn18 -> Text + " -> " + EditNastrTo18 -> Text );
    // ������������ ����� ������ ���������� �����
    EditNastrTo0 -> Color = clWhite;
    EditNastrTo1 -> Color = clWhite;
    EditNastrTo2 -> Color = clWhite;
    EditNastrTo3 -> Color = clWhite;
    EditNastrTo4 -> Color = clWhite;
    EditNastrTo5 -> Color = clWhite;
    EditNastrTo6 -> Color = clWhite;
    EditNastrTo7 -> Color = clWhite;
    EditNastrTo17 -> Color = clWhite;
    EditNastrTo18 -> Color = clWhite;
    // ������������ ����������� ����������
    VisualNasmod();
    VisualParA();
    VisualParR();
    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> Clear();
    if(EditNastrIn0->Text=="9,96E-3")       MemoNasmod -> Lines -> Add(0);
    else if(EditNastrIn0->Text=="4,99E-3")  MemoNasmod -> Lines -> Add(1);
    else if(EditNastrIn0->Text=="9,96E-4")  MemoNasmod -> Lines -> Add(2);
    else                                    MemoNasmod -> Lines -> Add(3);

    if(EditNastrIn1->Text=="9,96E-3")       MemoNasmod -> Lines -> Add(0);
    else if(EditNastrIn1->Text=="4,99E-3")  MemoNasmod -> Lines -> Add(1);
    else if(EditNastrIn1->Text=="9,96E-4")  MemoNasmod -> Lines -> Add(2);
    else                                    MemoNasmod -> Lines -> Add(3);
       // ��� ���������� ����!
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo2->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrIn3->Text);
    MemoNasmod -> Lines -> Add(EditNastrIn4->Text);
    MemoNasmod -> Lines -> Add(EditNastrIn5->Text);

    if(EditNastrIn6->Text=="9,96E-3")       MemoNasmod -> Lines -> Add(0);
    else if(EditNastrIn6->Text=="4,99E-3")  MemoNasmod -> Lines -> Add(1);
    else if(EditNastrIn6->Text=="9,96E-4")  MemoNasmod -> Lines -> Add(2);
    else                                    MemoNasmod -> Lines -> Add(3);
    
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo7->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo17->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo18->ItemIndex));
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
//------------------------------------------------------------------------------
// ������� ����������� �������
//------------------------------------------------------------------------------
void TForm1::Save_Data()
{
	int SizeOfIniFile=(int)sizeof(iniID);

	if(!DirectoryExists("Modules")) { CreateDir("Modules"); }
	FILE *im0;
	im0=fopen(loc_udb,"wb");
	if(im0)       { fwrite(&iniID,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::Load_Data()
{
	int SizeOfIniFile=(int)sizeof(iniID);

	if(!DirectoryExists("Modules")) { CreateDir("Modules"); }
	FILE *im0;
	im0=fopen(loc_udb,"rb");
	if(im0)       { fread(&iniID,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� ��������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
//-------------------  ������ �� ��������� �������  -------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::Edit_Acc_VPasKeyPress(TObject *Sender, char &Key)
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
void __fastcall TForm1::Edit_Acc_TekPasKeyPress(TObject *Sender, char &Key)
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
        iniID.pass[i-1] = 0;
   for(int i=1; i<= Vnew1pas_str.Length();i++)
        iniID.pass[i-1] = Vnew1pas_str[i];
   Save_Data();

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
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_OptClick(TObject *Sender)
{
  // ��� ���������� ����������
  // �������� �������
  iniID.state[0] = !CB_Acc_V1->Checked;
  PCNalad->Pages[0]->TabVisible = !CB_Acc_V1->Checked;
  iniID.state[1] = !CB_Acc_V2->Checked;
  PCNalad->Pages[1]->TabVisible = !CB_Acc_V2->Checked;
  iniID.state[2] = !CB_Acc_V3->Checked;
  PCNalad->Pages[2]->TabVisible = !CB_Acc_V3->Checked;
  iniID.state[3] = !CB_Acc_V4->Checked;
  PCNalad->Pages[3]->TabVisible = !CB_Acc_V4->Checked;
  iniID.state[4] = !CB_Acc_V5->Checked;
  PCNalad->Pages[4]->TabVisible = !CB_Acc_V5->Checked;
  iniID.state[5] = !CB_Acc_V6->Checked;
  PCNalad->Pages[5]->TabVisible = !CB_Acc_V6->Checked;
  iniID.state[6] = !CB_Acc_V7->Checked;
  PCNalad->Pages[6]->TabVisible = !CB_Acc_V7->Checked;    

  // �������� ������ � �������, ������ ���� �� ������
  Save_Data();

   Vtekpas_str = "";
   Vnew1pas_str = "";
   Vnew2pas_str = "";

   Pnl_Acc_Pas -> Visible = true;
   Pnl_Acc_Opt -> Visible = false;
   Pnl_Acc_SetPas -> Visible = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--��������� ���-�� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ChBoxGraphTemp1Click(TObject *Sender)
{
    // ����������� �������������� ������� ������� ��������
    if ( StrToInt(((TCheckBox*)Sender)->Hint) <= SERIES_COUNT )
        serTemp[StrToInt(((TCheckBox*)Sender)->Hint)] -> Active = ((TCheckBox*)Sender) -> Checked;
    // �������� �������
    else
        serArh[StrToInt(((TCheckBox*)Sender)->Hint) - 10] -> Active = ((TCheckBox*)Sender) -> Checked;
}
//---------------------------------------------------------------------------
//--������ ������ ������ ��������--//
//---------------------------------------------------------------------------
void ArhToGraph (AnsiString graphStr)
{
    // ������ �������� - ����, ��������� - ������
    AnsiString str[SERIES_COUNT];
    // ������� �������
    for ( int i = 0 ; i < SERIES_COUNT ; i++ ) str[i] = "";
    int byteNmb = 1;
    // ��������� ������ �� ���������
    for ( int i = 0 ; i < SERIES_COUNT ; i++ )
    {
        while (graphStr[byteNmb]!=';')
        {
            str[i] += AnsiString(graphStr[byteNmb]);
            byteNmb++;
        }
        byteNmb++;
    }
    // ���������� �������� ����������
    for ( int i = 1 ; i < SERIES_COUNT ; i++ )
        serArh[i] -> AddY(StrToFloat(str[i]),str[0]);
}
//---------------------------------------------------------------------------
//--��������� �������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxGraphArhClick(TObject *Sender)
{
    // ������� ��������
    for ( int i = 1 ; i < SERIES_COUNT ; i++ )
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
    else
        MemoDiagArh -> Lines ->Clear();
}
//---------------------------------------------------------------------------
//--������������ ��������--//
//---------------------------------------------------------------------------
void TForm1::VisualGraph()
{
 AnsiString temp_value = "";
    AnsiString graphTemp = "";

     if(shr[4])
    {
        // �����
        graphTemp = Label_Time -> Caption + ";";

        // ���1
        if(shr[20])
            temp_value = EdtTekA00->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[1] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        // ���2
        if(shr[21])
            temp_value = EdtTekA01->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[2] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //���3
        if(shr[22])
            temp_value = EdtTekA02->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[3] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //���4
        if(shr[23])
            temp_value = EdtTekA03->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[4] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //���5
        if(shr[24])
            temp_value = EdtTekA04->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[5] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //��� ��������
        if(shr[29])
            temp_value = EdtTekA07->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[6] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //��� ��������
        if(shr[29])
            temp_value = EdtTekA08->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[7] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //��������
        if(shr[17])
            temp_value = EdtTekA11->Text;
        else
            temp_value = "0,0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[8] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        //�����������
        if(shr[64])
            temp_value = Tek_Nagr[13]->Caption;
        else
            temp_value = "0";

        graphTemp = graphTemp + temp_value + ";";
        serTemp[9] -> AddY(StrToFloat ( temp_value ), Label_Time -> Caption );

        // �������� ������
        MemoGraph -> Lines -> Add ( graphTemp );
    }

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
                (((TImage*)Sender)->Name) == "tmn" ||
                (((TImage*)Sender)->Name) == "fn_shl" ||
                (((TImage*)Sender)->Name) == "fn_kam"
        )
    {
        BtnDeviceOn -> Caption = "���.";
        BtnDeviceOff -> Caption = "����.";
    }
	else if((((TImage*)Sender)->Name) == "pin_null")
	{
		BtnDeviceOn -> Caption = "�����";
        BtnDeviceOff -> Caption = "����";
	}
    	else if((((TImage*)Sender)->Name) == "vp23")
	{
		BtnDeviceOn -> Caption = "����.";
        BtnDeviceOff -> Caption = "����.";
	}
    else
    {
        BtnDeviceOn -> Caption = "����.";
        BtnDeviceOff -> Caption = "����.";
    }
    // ���������� ������ ����������
    LblDeviceName -> Caption = ((TImage*)Sender) -> Hint;
    PnlDevice -> Hint = ((TImage*)Sender) -> Name;
    if(((((TImage*)Sender)->Top) > 90)&&((((TImage*)Sender)->Top)<600))
        PnlDevice -> Top = ((TImage*)Sender)->Top + 40;
    else if ((((TImage*)Sender)->Top) < 90)
        PnlDevice -> Top = ((TImage*)Sender)->Top + 40;
    else
        PnlDevice -> Top = ((TImage*)Sender)->Top - 90;
    PnlDevice -> Left = ((TImage*)Sender)->Left - 90;
    if ((((TImage*)Sender)->Parent->Name) == "KASSETA")
    {
      PnlDevice -> Top +=300 ;
      PnlDevice -> Left +=123 ;
    }
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
 if(((TButton*)Sender) -> Name == "BtnDeviceExit" ) { return; }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(((TButton*)Sender) -> Parent -> Hint == "vp1")      { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x01);       }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp2") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x02);       }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp3") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x04);       }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp4") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x80);       }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp5")   { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x100);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp6") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x08);       }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp7") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x10);       }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp8")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x200);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp9")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x400);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp10")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x20);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp11")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x40);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp13") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x4000);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp14")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x800);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp15") { SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x1000);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp19")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x02);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp20")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x04);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp18")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x08);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp22")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x10);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp16")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x20);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp24")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x40);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp26")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x80);     }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp25")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x100);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp27")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x200);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp21")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x400);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp17")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x800);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp28")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x1000);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "vp29")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x2000);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "fk_shl2")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x01);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "fk_shl1")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x02);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "fk_tmn")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x04);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "kl_d4")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x08);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "kl_d2")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x10);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "ve1")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x1000);    }
  else if(((TButton*)Sender) -> Parent -> Hint == "kl_nap1")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x400); }
  else if(((TButton*)Sender) -> Parent -> Hint == "kl_nap2")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x08); }
  else if(((TButton*)Sender) -> Parent -> Hint == "kl_nap3")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x800); }
  else if(((TButton*)Sender) -> Parent -> Hint == "fn_shl")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x20);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "fn_kam")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x10);   }
  else if(((TButton*)Sender) -> Parent -> Hint == "fk_kam")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x4000); }
  else if(((TButton*)Sender) -> Parent -> Hint == "zatv")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x8000); }
  else if(((TButton*)Sender) -> Parent -> Hint == "zatvor")  { SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x2000); }

  else if(((TButton*)Sender) -> Parent -> Hint == "tmn")
  { if(StrToInt(((TButton*)Sender)->Hint)) { SetOut(1,4,0x40); }
    else                                   { SetOut(0,4,0x40); }
  }
  else if(((TButton*)Sender) -> Parent -> Hint == "pin_null")
  { if(StrToInt(((TButton*)Sender)->Hint)) { SetOut(1,0,0x200); SetOut(0,0,0x400); }
    else                                   { SetOut(0,0,0x200); SetOut(1,0,0x400); }
  }
   else if(((TButton*)Sender) -> Parent -> Hint == "vp23")
  { if(StrToInt(((TButton*)Sender)->Hint)) { SetOut(0,3,0x01);}
    else                                   { SetOut(1,3,0x01);}
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
            // � ������ 8 ��� 4
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==4))
            {
                if (shr[5]==5)
                    Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
                else if (shr[5]==6)
                {
                    if(shr[6]==8)
                    {
                        if(shr[10]==14)
                            Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                        else
                            Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
                    }
                    else if(shr[6]==28)
                        Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                    else
                        Form1 -> EditSHRName -> Text = SHR6Names[shr[6]];

                }
                else
                    Form1 -> EditSHRName -> Text = SHR5Names[shr[5]];

            }
            // � ������ 5 ��� 5 ������ ���� ������ 4
            if ((SHRValue[i]==5)&&(shr[SHRValue[i]]==5))
                    Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            // � ������ 5 ��� 6
            if ((SHRValue[i]==5)&&(shr[SHRValue[i]]==6))
            {
                    if(shr[6]==8)
                    {
                        if(shr[10]==14)
                            Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                        else
                            Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
                    }
                    else if(shr[6]==28)
                        Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                    else
                        Form1 -> EditSHRName -> Text = SHR6Names[shr[6]];

            }
            // � ������ 6 ��� 8
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==8))
            {
                if(shr[10]==14)
                    Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                else
                    Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
            }
            // � ������ 6 ��� 28 ������ ���� ������ 2
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==28))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // � ������ 10 ��� 14 ������ ���� ������ 2
            if((SHRValue[i]==10)&&(shr[SHRValue[i]]==14))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];

            // � ������ 8 ��� 5 ������ ���� ������ 7
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==5))
                Form1 -> EditSHRName -> Text = SHR7Names[shr[7]];
            // � ������ 3 ��� 2 ������ ���� ������ 1
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==2))
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            // � ������ 3 ��� 9 ������ ���� ������ 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==9))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // � ������ 3 ��� 12 ������ ���� ������ 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==12))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // � ������ 3 ��� 33 ������ ���� ������ 4
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==33))
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            // � ������ 3 ��� 52 ������ ���� ������ 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==52))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // � ������ 5 ��� 5 ������ ���� ������ 4
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==5))
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            // � ������ 5 ��� 6 ������ ���� ������ 6
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==6))
                Form1 -> EditSHRName -> Text = SHR6Names[shr[6]];
            // � ������ 10 ��� 14 ������ ���� ������ 2
            if((SHRValue[i]==10)&&(shr[SHRValue[i]]==14))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
			 // � ������ 4 ��� 13 ������� �����
            if(shr[4]==13)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr((par[N_ST][11]-CT_4)/1000);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 17 ������� �����
            if(shr[4]==17)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[14]-CT_4/1000);
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
        for ( int i = 0 ; i < 2 ; i++ )
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


if(shr[3] == 7)
{
APanel_String1 -> Caption = "������ ��������";
APanel_String1 -> Visible = true;
APanel_String2 -> Caption = "����� ������ ���������";
APanel_String2 -> Visible = true;
APanel_String3 -> Caption = "���� � ��������?";
APanel_String3 -> Visible = true;
APanel_DaBut -> Visible = true;
APanel_NetBut -> Visible = true;

NPanel_String1 -> Caption = "������ ��������";
NPanel_String1 -> Visible = true;
NPanel_String2 -> Caption = "����� ������ ���������";
NPanel_String2 -> Visible = true;
NPanel_String3 -> Caption = "���� � ��������?";
NPanel_String3 -> Visible = true;
NPanel_DaBut -> Visible = true;
NPanel_NetBut -> Visible = true;

APanel -> Visible = true;
NPanel -> Visible = true;
}


else if(shr[3] == 8)
{
APanel_String1 -> Caption = "�������� ����?";
APanel_String1 -> Visible = true;
APanel_String2 -> Visible = false;
APanel_String3 -> Visible = false;
APanel_DaBut -> Visible = true;
APanel_NetBut -> Visible = true;

NPanel_String1 -> Caption = "�������� ����?";
NPanel_String1 -> Visible = true;
NPanel_String2 -> Visible = false;
NPanel_String3 -> Visible = false;
NPanel_DaBut -> Visible = true;
NPanel_NetBut -> Visible = true;

APanel -> Visible = true;
NPanel -> Visible = true;
}

else if(shr[3] == 51)
{
APanel_String1 -> Caption = "������ ������ � ����?";
APanel_String1 -> Visible = true;
APanel_String2 -> Visible = false;
APanel_String3 -> Visible = false;
APanel_DaBut -> Visible = true;
APanel_NetBut -> Visible = true;

NPanel_String1 -> Caption = "������ ������ � ����?";
NPanel_String1 -> Visible = true;
NPanel_String2 -> Visible = false;
NPanel_String3 -> Visible = false;
NPanel_DaBut -> Visible = true;
NPanel_NetBut -> Visible = true;

APanel -> Visible = true;
NPanel -> Visible = true;
}

else if(shr[6]==26)
{
APanel_String1 -> Caption = "������ ��������";
APanel_String1 -> Visible = true;
APanel_String2 -> Caption = "��������� �������";
APanel_String2 -> Visible = true;
APanel_String3 -> Caption = "�������� ����?";
APanel_String3 -> Visible = true;
APanel_DaBut -> Visible = true;
APanel_NetBut -> Visible = true;

NPanel_String1 -> Caption = "������ ��������";
NPanel_String1 -> Visible = true;
NPanel_String2 -> Caption = "��������� �������";
NPanel_String2 -> Visible = true;
APanel_String3 -> Caption = "�������� ����?";
NPanel_String3 -> Visible = true;
NPanel_DaBut -> Visible = true;
NPanel_NetBut -> Visible = true;

APanel -> Visible = true;
NPanel -> Visible = true;
}

else if(shr[4]==21)
{
APanel_String1 -> Caption = "���������� ";
APanel_String1 -> Visible = true;
APanel_String2 -> Caption = "��������������� �������?";
APanel_String2 -> Visible = true;
APanel_String3 -> Visible = false;
APanel_DaBut -> Visible = true;
APanel_NetBut -> Visible = false;

NPanel_String1 -> Caption = "���������� ";
NPanel_String1 -> Visible = true;
NPanel_String2 -> Caption = "��������������� �������?";
NPanel_String2 -> Visible = true;
NPanel_String3 -> Visible = false;
NPanel_DaBut -> Visible = true;
NPanel_NetBut -> Visible = false;

APanel -> Visible = true;
NPanel -> Visible = true;
}

else
{
APanel -> Visible = false;
NPanel -> Visible = false;
}
}


void __fastcall TForm1::APanel_ButClick(TObject *Sender)
{
switch(StrToInt(((TPanel*)Sender)->Hint))
  { case 1: { otvet=1; break; }
    case 2: { otvet=2; break; }
  }
  APanel -> Visible = false;
  NPanel -> Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NPanel_ButClick(TObject *Sender)
{
 switch(StrToInt(((TPanel*)Sender)->Hint))
  { case 1: { otvet=1; break; }
    case 2: { otvet=2; break; }
  }
  APanel -> Visible = false;
  NPanel -> Visible = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--����������� ����� ������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBdVallClick(TObject *Sender)
{
    PnlDiagm -> Visible = !(PnlDiagm -> Visible);
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
{
    unsigned char i=0;
    // �������� ������� �������
	ListBoxCondition -> Items -> Clear();
	// ������ ������� �������
	switch(StrToInt(((TPanel*)Sender)->Hint))
	{
        case 1:
		{   LblRejim -> Caption = "������� ������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ���������� ���-������
            if(!(zin[0]&0x02))  { ListBoxCondition -> Items -> Add("��� ���������� ��� ������"); }
            // ���� ���������� ���
            if(!(zin[0]&0x04))  { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � �2"); }
		    // ���� ����� � �/���������
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
            // ���� �������� ������ ���-������
            if(!(zin[2]&0x20))  { ListBoxCondition -> Items -> Add("��� ����. ������� ��� ������"); }
            // ���� ������� ���
            if(!(zin[2]&0x100))  { ListBoxCondition -> Items -> Add("��� ������� ���"); }
            // ���� ��������� ������ ���
            if(!(zin[2]&0x2000))  { ListBoxCondition -> Items -> Add("��� ����. ������� ���"); }
		    // �� ������� �� ���� �����
		    for(i=1;i<40;i++) // �� ������������� R_33();
		    { if(i!=33) { if(shr[i]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]); } } }
		}; break;
		case 3:
		{   LblRejim -> Caption = "������� ���� (��)";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ���������� �������
		    if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� �/���������"); }
            // ���� ���������� ���
            if(!(zin[0]&0x04))  { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
            // ���� ���������� ��
		    if(!(zin[0]&0x08)) { ListBoxCondition -> Items -> Add("��� ���������� ��"); }
            // ���� ���������� �������
		    if(!(zin[0]&0x10)) { ListBoxCondition -> Items -> Add("��� ���������� �������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
		    // ���� ����� � �2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � �2"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �/���������
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
            // ���� �������� ������ ���-������
            if(!(zin[2]&0x20))  { ListBoxCondition -> Items -> Add("��� ����. ������� ��� ������"); }
            // ���� ������� ���
            if(!(zin[2]&0x100))  { ListBoxCondition -> Items -> Add("��� ������� ���"); }
            // ���� ��������� ������ ���
            if(!(zin[2]&0x2000))  { ListBoxCondition -> Items -> Add("��� ����. ������� ���"); }
            // �� ������� �� ���� �����
		    for(i=1;i<40;i++) // �� ������������� R_33();
		    { if(i!=33) { if(shr[i]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]); } } }
            
		}; break;
		case 5:
		{   LblRejim -> Caption = "����� ��";
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if((!shr[3])&&(!shr[4])) { ListBoxCondition -> Items -> Add("�� ������� ����� ���. �������"); }
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
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
		    // ���� ����� � �/���������
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
            // �� ������� �� ���� �����
		    for(i=1;i<40;i++) // �� ������������� R_33();
		    { if(i!=33) { if(shr[i]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]); } } }
		}; break;
		case 7:
		{   LblRejim -> Caption = "���������� ���������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // �� ������� �� ���� �����
		    for(i=1;i<40;i++) // �� ������������� R_33();
		    { if(i!=33) { if(shr[i]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]); } } }
		}; break;
        case 9:
		{   LblRejim -> Caption = "���. ������������ ����";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
		    // ������ ������
		    if(!(zin[1]&0x04)) { ListBoxCondition -> Items -> Add("������� ������ �� ������"); }
		    // ������ ����������� ������������ � HOME
		    if(!(zin[4]&0x200)) { ListBoxCondition -> Items -> Add("������ ������������ �� � HOME"); }
            // �� ������� �� ���� �����
		    for(i=1;i<40;i++) // �� ������������� R_33();
		    { if(i!=33) { if(shr[i]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]); } } }
		}; break;
		case 109:
		{   LblRejim -> Caption = "����. ������������ ����";
		    // ������� �����  9
		    if(!shr[9]) { ListBoxCondition -> Items -> Add("�� ������� ����� " + SHRNames[9]); }
		}; break;
		case 10:
		{   LblRejim -> Caption = "������� ������� ������";
		    // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x20)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
		    // ���� ����� � �4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
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
		    if(!(zin[4]&0x200)) { ListBoxCondition -> Items -> Add("������ ����������� ������������ �� � HOME"); }
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
        }; break;
        case 13:
        {   LblRejim -> Caption = "START ������������ �����������";
            // ������ ������
		    if(!(zin[1]&0x04)) { ListBoxCondition -> Items -> Add("������� ������ �� ������"); }
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
        }; break;
		case 113:
		{   LblRejim -> Caption = "���� ����������";  // STOP ���������� -- ������� ������� ������
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
		case 100:
		{   LblRejim -> Caption = "����� �����";
		    if(!shr[5]&&shr[4])  { ListBoxCondition -> Items -> Add("��� ����������� ������: ����� ��"); }
		    // �� ������� ����� 3
		    if(shr[3])  { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }
		}; break;
		case 17:
		{   LblRejim -> Caption = "��������������� ��";
		    // ���� ����� � ��
		    if(diagnS[0]&0x40) { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
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
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
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
		}; break;

		case 18:
		{   LblRejim -> Caption = "������� ��";
		    // ���� ����� � ��
		    if(diagnS[0]&0x40) { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
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
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
		}; break;
		case 19:
		{   LblRejim -> Caption = "������� ��";
		    // ���� ����� � ��
		    if(diagnS[0]&0x40) { ListBoxCondition -> Items -> Add("��� ����� � �/����"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
		}; break;
		case 20:
		{   LblRejim -> Caption = "���. ���1";
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
		{   LblRejim -> Caption = "����. ���1";
		    // ������� ����� 20
		    if(!shr[20]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[20]); }
		   // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 21:
		{   LblRejim -> Caption = "���. ���2";
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
		{   LblRejim -> Caption = "����. ���2";
		    // ������� ����� 21
		    if(!shr[21]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[21]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 22:
		{   LblRejim -> Caption = "���. ���3";
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
		{   LblRejim -> Caption = "����. ���3";
		    // ������� ����� 22
		    if(!shr[22]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[22]); }
		   // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 23:
		{   LblRejim -> Caption = "���. ���4";
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
		{   LblRejim -> Caption = "����. ���4";
		    // ������� ����� 23
		    if(!shr[23]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[23]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 24:
		{   LblRejim -> Caption = "���. ���5";
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
		{   LblRejim -> Caption = "����. ���5";
		    // ������� ����� 24
		    if(!shr[24]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[24]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 25:
		{   LblRejim -> Caption = "���. ���6";
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
		{   LblRejim -> Caption = "����. ���6";
		    // ������� ����� 25
		    if(!shr[25]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[25]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 26:
		{   LblRejim -> Caption = "���. ���7";
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
            // �� ������� ����� 26
		    if(shr[26]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[26]); }
		}; break;
		case 126:
		{   LblRejim -> Caption = "����. ���7";
		    // ������� ����� 26
		    if(!shr[26]) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[26]); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
		    // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 29:
		{   LblRejim -> Caption = "���. ��� ��������";
       		    // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
       		    // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
		    /// �� ������� ����� 3 ��� 4 ��� ������� � �������
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
            // ���� ���������� ��
            if(!(zin[0]&0x08))  { ListBoxCondition -> Items -> Add("��� ���������� ��"); }
		}; break;
		case 129:
		{   LblRejim -> Caption = "����. ��� ��������";
		    // ������� ����� 29
            if(!shr[29]) { ListBoxCondition -> Items -> Add("��� �������� �� �������"); }
		    // �� ������� ����� 3 ��� 4 ��� ������� � �������
		    if(!((shr[4]&&PR_NALADKA)||(!shr[3]&&!shr[4]))) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
		}; break;
		case 33:
		{   LblRejim -> Caption = "���. ������ ���������";
		    // ������� ����� 33
            if(shr[33]) { ListBoxCondition -> Items -> Add("������� ����� <������ ���������>"); }
		}; break;
		case 34:
		{   LblRejim -> Caption = "����. ������ ���������";
		    // ������� ����� 33
            if(!shr[33]) { ListBoxCondition -> Items -> Add("�� ������� ����� <������ ���������>"); }
		}; break;

        case 37:
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
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
		}; break;

            case 38:
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
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
		}; break;

        default: return;     // ����������� �������
	}
	// ������������ �������� ����������� �����
	if(StrToInt(((TPanel*)Sender)->Hint)!=140)
	{ if(MessageDlg("������������� ������ ������: " + LblRejim -> Caption + "?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) != mrYes ) return; }
	// ���� �� ��������� ������� �������
	if(ListBoxCondition -> Items -> Count)
	{ PnlCondition -> Visible = true; }
	// ���� ��� ������� ��������� �������� �������
	else
	{ // �������� ��� �������
	  qkk = StrToInt(((TPanel*)Sender)->Hint);
	  // ��������� �������� � ������
          MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
	  MemoStat -> Lines -> Add(Label_Date -> Caption + " " + Label_Time -> Caption + ": ������� �����: <" + LblRejim -> Caption + ">" );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--��������, ����������� ��� �������� ����������--//
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    // ���������� � ��������� ������������ ������
    TimerExist  -> Terminate();
    LogicThread -> Terminate();

    // ��������� ����� � ������� ISO
    ISO_DriverClose();
    ISO813_DriverClose();
    ISODA_DriverClose();

    // ������� Comport'�
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

    // ��������� �����
    AnsiString fileName;
    ///////////////////////////////////////////////////////////////////////////////////////////
    // ����������� ��������� � �������� ������
    if(!DirectoryExists("Diag")) { CreateDir("Diag"); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    fileName = "Diag\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    ///////////////////////////////////////////////////////////////////////////////////////////
    if(!FileExists(fileName))
    {   int fileID = FileCreate(fileName);
        FileClose(fileID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<MemoDiag -> Lines -> Count; i++)
    {   MemoTemp -> Lines -> Add(MemoDiag -> Lines -> operator[](i)); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> SaveToFile( fileName );
    ///////////////////////////////////////////////////////////////////////////////////////////
    // �������� ������������ � �������� ������
    if(!DirectoryExists("Stat")) { CreateDir("Stat"); }
    fileName = "Stat\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    if(!FileExists(fileName))
    {   int fileID = FileCreate(fileName);
        FileClose(fileID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<MemoStat -> Lines -> Count; i++)
    {   MemoTemp -> Lines -> Add(MemoStat -> Lines -> operator[](i)); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> SaveToFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    // ������� ������� ���������� ��������
    if(!DirectoryExists("Graph")) { CreateDir("Graph"); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    fileName = "Graph\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    ///////////////////////////////////////////////////////////////////////////////////////////
    if(!FileExists(fileName))
    {   int fileID=FileCreate(fileName);
        FileClose(fileID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<(MemoGraph -> Lines -> Count - 1); i++)
    {  MemoTemp -> Lines -> Add(MemoGraph -> Lines -> operator[](i)); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> SaveToFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    // ��������� �������� � ������
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time -> Caption + " : ��������� ��������� ");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EdtNRed0Exit(TObject *Sender)
{
    // �������� ���������� �������� ���������� �������

    // �������� ����� �� �������
    AnsiString
        text = ((TEdit*)Sender)->Text;
    for ( unsigned char i = 1 ; i < text.Length(); i++)
        if (text[i] == '.') text[i] = ',';
    unsigned char
        format; // ���-�� ������ ����� �������
    float
        valueText = StrToFloat(text);

    ((TEdit*)Sender)->Color = clSilver;

    if(((TEdit*)Sender)->Hint == "1")
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
        else if (valueText > 100.0)
        {
            valueText = 100.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    if(((TEdit*)Sender)->Hint == "2")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if(valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if(valueText > 40.0)
        {
            valueText = 40.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    if(((TEdit*)Sender)->Hint == "3")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        if(valueText == 0.0)
        { }
        // ��������� �� �������
        else if(valueText < 25.0)
        {
            valueText = 25.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if(valueText > 140.0)
        {
            valueText = 140.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    if(((TEdit*)Sender)->Hint == "4")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if(valueText == 0.0)
        { }
        else if(valueText < 25.0)
        {
            valueText = 25.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if(valueText > 400.0)
        {
            valueText = 400.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� �� ���������
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 5, format);

    VisualParN();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// �������� ���������� ���������
void __fastcall TForm1::EdtARed0Exit(TObject *Sender)
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
    //((TEdit*)Sender)->Color = clSilver;

    //  ������ ���1
    if  (((TEdit*)Sender)->Name == "EdtARed0")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG1_MAX)
        {
            valueText = RRG1_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ������ ���2
    if  (((TEdit*)Sender)->Name == "EdtARed1")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG2_MAX)
        {
            valueText = RRG2_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ������ ���3
    if  (((TEdit*)Sender)->Name == "EdtARed2")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG3_MAX)
        {
            valueText = RRG3_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ������ ���4
    if  (((TEdit*)Sender)->Name == "EdtARed3")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG4_MAX)
        {
            valueText = RRG4_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ������ ���5
    if  (((TEdit*)Sender)->Name == "EdtARed4")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG5_MAX)
        {
            valueText = RRG5_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ������ ���6
    if  (((TEdit*)Sender)->Name == "EdtARed5")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG6_MAX)
        {
            valueText = RRG6_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ������ ���7
    if  (((TEdit*)Sender)->Name == "EdtARed6")
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > RRG7_MAX)
        {
            valueText = RRG7_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  �������� ��
    if  (((TEdit*)Sender)->Name == "EdtARed7")
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
        else if (valueText > P_IP_MAX)
        {
            valueText = P_IP_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ��������� ������������
    if  ((((TEdit*)Sender)->Name == "EdtARed8") ||
        (((TEdit*)Sender)->Name == "EdtARed9"))
    {
        // ���-�� ������ ����� ������� 1
        format = 2;
        // ��������� �� �������
        if (valueText < 0.5)
        {
            valueText = 0.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 9.5)
        {
            valueText = 9.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ��������
    if  (((TEdit*)Sender)->Name == "EdtARed10")
    {
        // ���-�� ������ ����� ������� 1
        format = 2;
        // ��������� �� �������
        if(valueText == 0.0)
        { }
        else if (valueText < 1.0)
        {
            valueText = 1.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 13.3)
        {
            valueText = 13.3;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ����� �������� (�)
    if  (((TEdit*)Sender)->Name == "EdtARed11_3")
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
        else if (valueText > 1000.0)
        {
            valueText = 1000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ����� �������� (��� � ���)
    if  ((((TEdit*)Sender)->Name == "EdtARed11_2")  ||
        (((TEdit*)Sender)->Name == "EdtARed11_1"))
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
        else if (valueText > 59.0)
        {
            valueText = 59.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ����� �������� (����)
    if  (((TEdit*)Sender)->Name == "EdtARed11_0")
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
        else if (valueText > 999.0)
        {
            valueText = 999.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //  ���������� ������
    if (((TEdit*)Sender)->Name == "EdtARed12")
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
        else if (valueText > 1000.0)
        {
            valueText = 1000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    //  ���������� �������
    if (((TEdit*)Sender)->Name == "EdtARed20")
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 1.0)
        {
            valueText = 1.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 25.0)
        {
            valueText = 25.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���� � �����������
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 5, format);
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;

    // ���������� �� ��������� ������
    Save_parA_temp();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void TForm1::DrawEditColor(TMaskEdit *Kon, TMaskEdit *Red)
{
    // ������� ���������� �����
    if(Kon->Text!=Red->Text) Red->Color = clSilver;
    else Red->Color = clWhite;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TForm1::BtnParNClick(TObject *Sender)
{
    // ���� ���������� �������
    PnlParN -> Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParNDaClick(TObject *Sender)
{
    // �������� ���������� �������
    if(((TEdit*)Sender)->Name == "BtnParNDa")
    {
        // �������� �������� �� ���������� � ����������� ������
        par_n[0] = iniParN.par_n[0] = int(StrToFloat(EdtNRed0 -> Text)*10.0);
        par_n[1] = iniParN.par_n[1] = int(StrToFloat(EdtNRed1 -> Text)*10.0);
        par_n[4] = iniParN.par_n[4] = int(StrToFloat(EdtNRed4 -> Text)*10.0);
        par_n[5] = iniParN.par_n[5] = int(StrToFloat(EdtNRed5 -> Text)*10.0);
        par_n[6] = iniParN.par_n[6] = int(StrToFloat(EdtNRed6 -> Text)*10.0);
        par_n[7] = iniParN.par_n[7] = int(StrToFloat(EdtNRed7 -> Text)*10.0);
        par_n[8] = iniParN.par_n[8] = int(StrToFloat(EdtNRed8 -> Text)*10.0);
        par_n[9] = iniParN.par_n[9] = int(StrToFloat(EdtNRed9 -> Text)*10.0);
        par_n[10] = iniParN.par_n[10] = int(StrToFloat(EdtNRed10 -> Text)*10.0);
        par_n[11] = iniParN.par_n[11] = int(StrToFloat(EdtNRed11 -> Text)*10.0);
        par_n[12] = iniParN.par_n[12] = int(StrToFloat(EdtNRed12 -> Text)*10.0);
        par_n[13] = iniParN.par_n[13] = int(StrToFloat(EdtNRed13 -> Text)*10.0);
        par_n[14] = iniParN.par_n[14] = int(StrToFloat(EdtNRed14 -> Text)*10.0);
        par_n[15] = iniParN.par_n[15] = int(StrToFloat(EdtNRed15 -> Text)*10.0);
        par_n[16] = iniParN.par_n[16] = int(StrToFloat(EdtNRed16 -> Text)*10.0);
        par_n[17] = iniParN.par_n[17] = int(StrToFloat(EdtNRed17 -> Text)*10.0);
        par_n[18] = iniParN.par_n[18] = int(StrToFloat(EdtNRed18 -> Text)*10.0);
        par_n[19] = iniParN.par_n[19] = int(StrToFloat(EdtNRed19 -> Text)*10.0);
        par_n[20] = iniParN.par_n[20] = int(StrToFloat(EdtNRed20 -> Text)*10.0);
        par_n[22] = iniParN.par_n[22] = int(StrToFloat(EdtNRed22 -> Text)*10.0);
        par_n[23] = iniParN.par_n[23] = int(StrToFloat(EdtNRed23 -> Text)*10.0);
        // ������� ��������� ��������� �����
        EdtNRed0 -> Color = clWhite;
        EdtNRed1 -> Color = clWhite;
        EdtNRed4 -> Color = clWhite;
        EdtNRed5 -> Color = clWhite;
        EdtNRed6 -> Color = clWhite;
        EdtNRed7 -> Color = clWhite;
        EdtNRed8 -> Color = clWhite;
        EdtNRed9 -> Color = clWhite;
        EdtNRed10 -> Color = clWhite;
        EdtNRed11 -> Color = clWhite;
        EdtNRed12 -> Color = clWhite;
        EdtNRed13 -> Color = clWhite;
        EdtNRed14 -> Color = clWhite;
        EdtNRed15 -> Color = clWhite;
        EdtNRed16 -> Color = clWhite;
        EdtNRed17 -> Color = clWhite;
        EdtNRed18 -> Color = clWhite;
        EdtNRed19 -> Color = clWhite;
        EdtNRed20 -> Color = clWhite;
        EdtNRed22 -> Color = clWhite;
        EdtNRed23 -> Color = clWhite;
        MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ��������� �������");
        VisualParN();

        // ���������� ���������� �������
        int SizeOfIniFile=(int)sizeof(iniParN);
	    if(!DirectoryExists("ParN")) { CreateDir("ParN"); }
        AnsiString fName = "ParN//ParN.udb";
	    FILE *im0;
	    im0=fopen(fName.c_str(),"wb");
	    if(im0)       { fwrite(&iniParN,SizeOfIniFile,1,im0); fclose(im0); }
	    else if(!im0) { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
    }

    PnlParN->Visible = false;
}
//---------------------------------------------------------------------------
void TForm1::VisualParN()
{
    //--������������ ���������� �������
    EdtNKon0->Text = IntToStr(int(float(par_n[0])/10.0));
    EdtNKon1->Text = IntToStr(int(float(par_n[1])/10.0));
    EdtNKon4->Text = IntToStr(int(float(par_n[4])/10.0));
    EdtNKon5->Text = IntToStr(int(float(par_n[5])/10.0));
    EdtNKon6->Text = IntToStr(int(float(par_n[6])/10.0));
    EdtNKon7->Text = IntToStr(int(float(par_n[7])/10.0));
    EdtNKon8->Text = IntToStr(int(float(par_n[8])/10.0));
    EdtNKon9->Text = IntToStr(int(float(par_n[9])/10.0));
    EdtNKon10->Text = IntToStr(int(float(par_n[10])/10.0));
    EdtNKon11->Text = IntToStr(int(float(par_n[11])/10.0));
    EdtNKon12->Text = IntToStr(int(float(par_n[12])/10.0));
    EdtNKon13->Text = IntToStr(int(float(par_n[13])/10.0));
    EdtNKon14->Text = IntToStr(int(float(par_n[14])/10.0));
    EdtNKon15->Text = IntToStr(int(float(par_n[15])/10.0));
    EdtNKon16->Text = IntToStr(int(float(par_n[16])/10.0));
    EdtNKon17->Text = IntToStr(int(float(par_n[17])/10.0));
    EdtNKon18->Text = IntToStr(int(float(par_n[18])/10.0));
    EdtNKon19->Text = IntToStr(int(float(par_n[19])/10.0));
    EdtNKon20->Text = IntToStr(int(float(par_n[20])/10.0));
    EdtNKon22->Text = IntToStr(int(float(par_n[22])/10.0));
    EdtNKon23->Text = IntToStr(int(float(par_n[23])/10.0));
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_500_2Timer(TObject *Sender)
{
    VisualGraph();                          //  ����������� ��������
    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Prek_1Click(TObject *Sender)
{
    // ����� ���� �����
   if(mnemoInAuto) return;
   // ��������� ����� ������ � ������� ���� �����
   GK_n = StrToInt(((TLabel*)Sender)->Hint);
   if(GK_n>9)   Pnl_GK -> Left = ((TLabel*)Sender) -> Left - 150;
   else         Pnl_GK -> Left = ((TLabel*)Sender) -> Left - 110;

   Pnl_GK -> Top = ((TLabel*)Sender) -> Top + 40;
   AnsiString temp_str = "";
   for(int i=0;iniGAS.Gas_names[GK_n][i]!=0;i++)
   temp_str = temp_str + iniGAS.Gas_names[GK_n][i];
   Edit_GK -> Text = temp_str;

   Pnl_GK -> Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnGKOnClick(TObject *Sender)
{
   // ������������� ��� ������ �����
   if((bool)StrToInt(((TButton*)Sender)->Hint))
   {
        AnsiString temp_str;
        temp_str = Edit_GK->Text;

        for(int i=0;i<8;i++)
        iniGAS.Gas_names[GK_n][i] = 0;

        for(int i=1; i<= temp_str.Length();i++)
        iniGAS.Gas_names[GK_n][i-1] = temp_str[i];
        RenameGases();
        SaveGasNames();
   }

   Pnl_GK -> Visible = false;
}
//---------------------------------------------------------------------------
void TForm1::RenameGases()   // �������� �������� �����
{
 AnsiString temp_str = "";
 for(int i=0;iniGAS.Gas_names[0][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[0][i];
 Form1->Prek_1 -> Caption = temp_str;
 temp_str = "";
 for(int i=0;iniGAS.Gas_names[1][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[1][i];
 Form1->Prek_2 -> Caption = temp_str;
 temp_str = "";
 for(int i=0;iniGAS.Gas_names[2][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[2][i];
 Form1->Prek_3 -> Caption = temp_str;
 temp_str = "";
 for(int i=0;iniGAS.Gas_names[3][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[3][i];
 Form1->Prek_4 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[4][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[4][i];
 Form1->Gas_Name0 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[5][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[5][i];
 Form1->Gas_Name1 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[6][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[6][i];
 Form1->Gas_Name2 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[7][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[7][i];
 Form1->Gas_Name3 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[8][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[8][i];
 Form1->Gas_Name4 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[9][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[9][i];
 Form1->Gas_Name5 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[10][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[10][i];
 Form1->Gas_Name6 -> Caption = temp_str;
 temp_str = "";

 for(int i=0;iniGAS.Gas_names[11][i]!=0;i++)
 temp_str = temp_str + iniGAS.Gas_names[11][i];
 Form1->Gas_Name7 -> Caption = temp_str;

}
//---------------------------------------------------------------------------
void TForm1::SaveGasNames()
{
	int SizeOfIniFile=(int)sizeof(iniGAS);

	if(!DirectoryExists("Modules")) { CreateDir("Modules"); }
	FILE *im0;
	im0=fopen(loc_gas_udb,"wb");
	if(im0)       { fwrite(&iniGAS,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::LoadGasNames()
{
	int SizeOfIniFile=(int)sizeof(iniGAS);

	if(!DirectoryExists("Modules")) { CreateDir("Modules"); }
	FILE *im0;
	im0=fopen(loc_gas_udb,"rb");
	if(im0)       { fread(&iniGAS,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "���������� ��������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::Klaster()
{
    if(PR_KLASTER) // ��������� ��� �������
    {
    // ������
    PnlRC  -> Visible = false;
    PnlSP  -> Visible = false;
    Panel86->Visible =false;
    Panel378->Visible =false;
    // ������� �� ����������
    KASSETA -> Visible = false;
    // ���������� ������� �� ����������
    Plast -> Visible = false; 
    // ���������� ������� � ����������
    Panel2047 -> Visible = false;
    Panel2048 -> Visible = false;
    Panel2052 -> Visible = false;
    Panel2054  -> Visible = false;
    // ������������ �� ���������� �������
    Panel317 -> Visible = false;
    // ��������� �������
    Panel301 -> Visible = false;
    Panel303 -> Visible = false;
    Panel306 -> Visible = false;
    Panel307 -> Visible = false;
    Panel309 -> Visible = false;

    Panel311 -> Visible = false;
    Panel312 -> Visible = false;
    Panel313 -> Visible = false;
    Panel314 -> Visible = false;
    Panel315 -> Visible = false;

    Panel316 -> Visible = false;
    Panel319 -> Visible = false;
    Panel320 -> Visible = false;
    Panel324 -> Visible = false;
    Panel328 -> Visible = false;

    Panel302 -> Visible = false;
    Panel304 -> Visible = false;
    Panel305 -> Visible = false;
    Panel308 -> Visible = false;
    Panel310 -> Visible = false;
    // ���������
    Label48 -> Visible = false;
    EditNastrIn17 -> Visible = false;
    EditNastrTo17 -> Visible = false;
    Label54 -> Visible = false;
    }
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
			Application->ShowException(&exception);
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
		if(!PR_KLASTER) PortTask |= 0x01; // �1 ������ � ��������� ��������
		PortTask |= 0x0E; // ��������� �������������� �������
    }

	if(PortTask & 0x01)
	{
		DevErr = Dat_MERA[0]->DatMERA_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[Dat_MERA[0]->diagnS_byte] |= Dat_MERA[0]->diagnS_mask : diagnS[Dat_MERA[0]->diagnS_byte] &= (~Dat_MERA[0]->diagnS_mask);
			PortTask &= (~0x01);
			DevState = 0;
		}
		return;
	}

    else if(PortTask & 0x04)
	{
		DevErr = TRMD[0]->TRMD_Manage(DevState);
		if(DevState > 1)
		{
			DevErr ? diagnS[TRMD[0]->diagnS_byte] |= TRMD[0]->diagnS_mask : diagnS[TRMD[0]->diagnS_byte] &= (~TRMD[0]->diagnS_mask);
			PortTask &= (~0x04);
			DevState = 0;
		}
		return;
	}

    else if(PortTask & 0x02)
	{
		DevErr = Dat_MERA[1]->DatMERA_Manage(DevState,0);
		if(DevState > 1)
		{
			DevErr ? diagnS[Dat_MERA[1]->diagnS_byte] |= Dat_MERA[1]->diagnS_mask : diagnS[Dat_MERA[1]->diagnS_byte] &= (~Dat_MERA[1]->diagnS_mask);
			PortTask &= (~0x02);
			DevState = 0;
		}
		return;
	}

    else if(PortTask & 0x08)
	{
		DevErr = TRMD[1]->TRMD_Manage(DevState);
		if(DevState > 1)
		{
			DevErr ? diagnS[TRMD[1]->diagnS_byte] |= TRMD[1]->diagnS_mask : diagnS[TRMD[1]->diagnS_byte] &= (~TRMD[1]->diagnS_mask);
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
			Application->ShowException(&exception);
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
//�������� �����
//---------------------------------------------------------------------------
void __fastcall TForm1::check1Click(TObject *Sender)
{
    if (StrToInt(((TImage*)Sender)->Hint)==0)
      ((TImage*)Sender)->Hint=1;
    else
      ((TImage*)Sender)->Hint=0;
    for(int j=0;j<=20;j++)
    {
        if(Cb_Klapan[j] ->Hint==0)
        {
            Cb_Klapan[j]        ->Picture->Bitmap=check0->Picture->Bitmap;
        }
        else
        {
            Cb_Klapan[j]        ->Picture->Bitmap=check1->Picture->Bitmap;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
   //����� ����
void __fastcall TForm1::TreeSelect(TObject *Sender)
{

    Button8->Enabled=true;
    AnsiString buf=((TTreeView*)Sender)->Selected->Text;
    Button1->Enabled=false;
    if(((TTreeView*)Sender)->Selected->Level==1)
    { Button1->Enabled=true;

        Edit1->Text=((TTreeView*)Sender)->Selected->Text;
        Edit1->Text=Edit1->Text.Delete(Edit1->Text.Length(),6);
        Edit2->Text="";
        int i =1;


        while (buf[i]!='(')i++;
            i++;
        Edit2->Text=buf[i];
        Edit2->Text=Edit2->Text+buf[i+1];
        Edit2->Text=Edit2->Text+buf[i+2];
        Edit2->Text=StrToInt(Edit2->Text);

        i =StrToInt(Edit2->Text);

        buf=klinfo[i].adr;
        for(int j =0;j<=20;j++)
            buf=buf+IntToStr(klinfo[i].opcl[j]);
        Edit1->Text=klinfo[i].name;
        ComboBox1->ItemIndex=klinfo[i].adr;
        for(int j=0;j<=20;j++)
        {
            Cb_Klapan[j] ->Hint=IntToStr(klinfo[i].opcl[j]);
            if(Cb_Klapan[j] ->Hint==0)
               Cb_Klapan[j]->Picture->Bitmap=check0->Picture->Bitmap;
            else Cb_Klapan[j]->Picture->Bitmap=check1->Picture->Bitmap;
        }
    }
    else
    {
        ComboBox1->ItemIndex=((TTreeView*)Sender)->Selected->Index;
        Edit3->Text=((TTreeView*)Sender)->Selected->Text;

        Edit1->Text="";
        Edit2->Text="";
    }
}
//---------------------------------------------------------------------------
  //�������� ����
void __fastcall TForm1::DeleteStep(TObject *Sender)
{
if((Edit1->Text!="")&&(ComboBox1->ItemIndex!=0))
{
int i =StrToInt(Edit2->Text);
if (i+1==Memo_Steps->Lines->Count)
{
    Memo_Steps->Lines->Delete(i);
}
else
{
    AnsiString buf =klinfo[i].adr;
    buf=buf+"000000000000000000000";
    buf=buf+" - ";
    buf=buf+"@"+Now().DateString()+Now().TimeString();
    Memo_Steps->Lines->Strings[i]=buf;
}
Memo_Steps->Lines->SaveToFile("libs_1\\Steps.udb");
AnsiString buf =ComboBox1->ItemIndex;
RefreshShTree();
ComboBox1->ItemIndex=StrToInt(buf);

Button8->Enabled=false;
}
else if(ComboBox1->ItemIndex!=0)
{
AnsiString buf;
for(int i =0;i<Memo_tree->Lines->Count;i++)
  {
    if(Memo_tree->Lines->Strings[i]==Edit3->Text)
        {
            Memo_tree->Lines->Delete(i);
            for(int j=0;j<Memo_Steps->Lines->Count;j++)
            {
                buf=Memo_Steps->Lines->Strings[j];
                if(i==StrToInt(buf[1]))
                {
                    Memo_Steps->Lines->Delete(j);
                    j=-1;
                }
                if(i<StrToInt(buf[1]))
                {

                    int a =StrToInt(buf[1])-1;
                    buf[1]=(IntToStr(a))[1];
                    Memo_Steps->Lines->Strings[j]=buf;

                }
            }
        }
  }
  Memo_Steps->Lines->SaveToFile("libs_1\\Steps.udb");
  Memo_tree->Lines->SaveToFile("libs_1\\treenode.udb");
  buf =ComboBox1->ItemIndex;
  RefreshShTree();
  ComboBox1->ItemIndex=StrToInt(buf);
  Button8->Enabled=false;
}
}

//---------------------------------------------------------------------------
void TForm1::RefreshShTree() //���������� ������ � ������
{
    ///////////////������� �������///////////////////////////
    Memo_Steps->Clear();
    for(int i=0;i<step_count_max;i++)
    {
        klinfo[i].name="";
        klinfo[i].hash="";
    }

    /////////////������� ������////////////////////////////
    TreeView2->Items->Clear();
    ComboBox1->Items->Clear();
    Memo_tree->Lines->LoadFromFile("libs_1\\treenode.udb");
    for(int i=0;i<Memo_tree->Lines->Count;i++)
    {
        TreeView2->Items->Add(NULL,Memo_tree->Lines->Strings[i]);
        Node[i]= TreeView2->Items->Item[i];
        ComboBox1->Items->Add(Memo_tree->Lines->Strings[i]);
    }
    ////////////������ �����///////////////////////////////
    Memo_Steps->Lines->LoadFromFile("libs_1\\Steps.udb");
   for(int i=0;i<Memo_Steps->Lines->Count;i++)
   {
        AnsiString buf =Memo_Steps->Lines->Strings[i];
        klinfo[i].adr=StrToInt(buf[1]);        //������ ��������� � ������
        int j;
        for(j=1;j<=21;j++)
            klinfo[i].opcl[j-1]=StrToInt(buf[j+1]); //������ ��������� ��������
        for(j=22;buf[j+1]!='@';j++)
            klinfo[i].name+=buf[j+1];                 //������ �����
        j=j+2;
        for(j;j<=buf.Length();j++)
            klinfo[i].hash=klinfo[i].hash+buf[j];
   }
   //////////////���������� � ������ ����� ���������////////
   for(int i=0;i<Memo_Steps->Lines->Count;i++)
   {
        AnsiString num ="";
        if(i>=10)
        {
            if(i>=100) num=IntToStr(i);
            else num="0"+IntToStr(i);
        }
        else num="00"+IntToStr(i);
        AnsiString name=klinfo[i].name+" ("+num+")";
        TreeView2->Items->AddChild(Node[klinfo[i].adr],name);

   }
   TreeView2->FullExpand();//��������� ������//

}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//save
void __fastcall TForm1::SaveStep(TObject *Sender)
{

if(ComboBox1->ItemIndex!=0)
{
    //���������
    if (state==0)
    {
        Button10->Enabled=true;
        int i=StrToInt(Edit2->Text);   //����� ����������� ����
        AnsiString buf;
        buf=ComboBox1->ItemIndex;
        for(int z=0;z<KL_COUNT;z++)   //���������� ��������� ��������
        {
            buf=buf+Cb_Klapan[z]->Hint;
        }

        buf=buf+Edit1->Text;//���������� ����� ����
        buf=buf+"@"+Now().DateString()+Now().TimeString();
        Memo_Steps->Lines->Strings[i]=buf;
        Memo_Steps->Lines->SaveToFile("libs_1\\Steps.udb");
        buf=ComboBox1->ItemIndex;
        RefreshShTree(); //���������� ������ � ������ �����
        ComboBox1->ItemIndex=StrToInt(buf);
    }
    //�����
    else
    {
        Button10->Enabled=false;
        AnsiString buf;
        if(ComboBox1->ItemIndex>=0)
        {
        buf=ComboBox1->ItemIndex;
        for(int z=0;z<KL_COUNT;z++) //���������� ��������� ��������
          {
            buf=buf+Cb_Klapan[z]->Hint;
          }

        buf=buf+Edit1->Text;//���������� ����� ����
        buf=buf+"@"+Now().DateString()+Now().TimeString();
        Memo_Steps->Lines->Add(buf);
        Memo_Steps->Lines->SaveToFile("libs_1\\Steps.udb");
        RefreshShTree();//���������� ������ � ������ �����
        ComboBox1->ItemIndex=StrToInt(buf[1]);
        }
    }
}
}
//---------------------------------------------------------------------------
int TForm1::bintodec(AnsiString a)
{
    int b=0;
    for (int i=0; i<21;i++)
    {
            b+=StrToInt(a[i+1])*pow(2,(i));

    }
    return b;
}
AnsiString TForm1::dectobin(int a)
{
    AnsiString b="";

    while(b.Length()!=21)
    {
     b=b+IntToStr(a%2);
     a=a/2;
    }
    b=b+IntToStr(a);
    return b;
}
//////////////////////������/���������////////////////////////////////////////
void __fastcall TForm1::Rb_saveClick(TObject *Sender)
{
if((((TRadioButton*)Sender)->Name)=="Rb_save")
{
    state=1;
    if(Edit1->Text=="")
        Button10->Enabled=false;

}
else
{
    state=0;
    Button10->Enabled=true;
    if((Edit2->Text=="")||(Edit1->Text==""))Button10->Enabled=false;

}
}
//---------------------------------------------------------------------------
//��������� ������ � �������� ������ ����//
void __fastcall TForm1::NameStepChange(TObject *Sender)
{
    if(Edit1->Text=="")
        Button10->Enabled=false;
    else
        Button10->Enabled=true;
    if((state==0)&&(Edit2->Text==""))
       Button10->Enabled=false;
}
//---------------------------------------------------------------------------


//�������� ����� �����//
void __fastcall TForm1::CreateTree(TObject *Sender)
{
    for(int i =0;i<Memo_tree->Lines->Count;i++)
    {
        if(Memo_tree->Lines->Strings[i]==Edit3->Text)
            return;
    }
    Memo_tree->Lines->Add(Edit3->Text);
    Memo_tree->Lines->SaveToFile("libs_1\\treenode.udb");
    AnsiString buf=ComboBox1->ItemIndex;
    RefreshShTree();
    ComboBox1->ItemIndex=StrToInt(buf);
}
//---------------------------------------------------------------------------

//���������� ���� � ������//
void __fastcall TForm1::AddStepToReceipe(TObject *Sender)
{
    if((Edit2->Text!="")&&(StringGrid1->RowCount-2<N_ST_MAX))
    {

        int index=StringGrid1->Selection.Top;
        ActCell_Rec=index;
        StringGrid1->RowCount++;
        Memo_receipt->Lines->Add(0);
        for(int j =StringGrid1->RowCount;j!=index;j--)
        {
            StringGrid1->Cells[0][j]=StringGrid1->Cells[0][j-1];
            StringGrid1->Cells[1][j]=StringGrid1->Cells[1][j-1];
        }
        if(StrToInt(Edit2->Text)>=100)StringGrid1->Cells[0][index]=Edit2->Text;
        else if(StrToInt(Edit2->Text)>=10)StringGrid1->Cells[0][index]="0"+Edit2->Text;
        else StringGrid1->Cells[0][index]="00"+Edit2->Text;
        StringGrid1->Cells[1][index]=klinfo[StrToInt(Edit2->Text)].name;

        for(int j =Memo_receipt->Lines->Count;j!=index;j--)
        {
            Memo_receipt->Lines->Strings[j-1]=Memo_receipt->Lines->Strings[j-2];
        }

        AnsiString buf ="";
        buf=        StringGrid1->Cells[0][index]    +   "#";//����� ����
        buf=buf+    klinfo[StrToInt(StringGrid1->Cells[0][index])].hash +   "#";//��� ����
        buf=buf+    klinfo[StrToInt(StringGrid1->Cells[0][index])].name +   "#";//��� ����
        buf=buf+    "0"   +   "#";//������ ���1
        buf=buf+    "0"   +   "#";//������ ���2
        buf=buf+    "0"   +   "#";//������ ���3
        buf=buf+    "0"   +   "#";//������ ���4
        buf=buf+    "0"   +   "#";//������ ���5
        buf=buf+    "0"   +   "#";//������ ���6
        buf=buf+    "0"   +   "#";//������ ���7
        buf=buf+    "0"   +   "#";//�������� ��
        buf=buf+    "0.5"   +   "#";//��������� ������������ �����
        buf=buf+    "0.5"   +   "#";//��������� ������������ �����
        buf=buf+    "0"   +   "#";//��������
        buf=buf+    "0"   +   "#";//����
        buf=buf+    "0"   +   "#";//������
        buf=buf+    "0"   +   "#";//�������
        buf=buf+    "0"   +   "#";//�����������
        buf=buf+    "0"   +   "#";//����������� ������
        Memo_receipt->Lines->Strings[index-1]=buf;
        buf="";
        for(int i =0;i<KL_COUNT;i++)
            buf=buf+IntToStr(klinfo[StrToInt(StringGrid1->Cells[0][index])].opcl[i]);
        for(int i=StringGrid1->RowCount;i>index;i--)
        {
            for(int j=0;j<14;j++)
                {par_temp[i][j]=par_temp[i-1][j];
                par_temp[i-1][j]=0;}
        }

        if(StrToInt(StringGrid1->Cells[0][index])==0)
            par_temp[index][13]=3000001;
        else if(StrToInt(StringGrid1->Cells[0][index])==1)
            par_temp[index][13]=3000002;
        else par_temp[index][13]=bintodec(buf);
        Label_Rec->Caption=" ������ : ����� ������";
        Label_Rec->Color=clBtnFace;
        Label_Rec->Font->Color=clBlack;
    }

    pr_lib=0;
    VisualParA();

}
//---------------------------------------------------------------------------
//����� ���� �� �������//
void __fastcall TForm1::ChoiceStepInRecipe(TObject *Sender)
{
if(StringGrid1->Selection.Top!=StringGrid1->RowCount-1)
{   ActCell_Rec=StringGrid1->Selection.Top;
    Refresh_kl_info("Clear");
    pr_cikl=0;
    pr_rtrn=0;
    if(StringGrid1->Cells[0][StringGrid1->Selection.Top]=="000")pr_cikl=1;
    else if(StringGrid1->Cells[0][StringGrid1->Selection.Top]=="001")pr_rtrn=1;
    else Refresh_kl_info(dectobin(par_temp[ActCell_Rec][13]));
    VisualParA();
    

}

}
//---------------------------------------------------------------------------
//�������� ���� �� �������//
void __fastcall TForm1::DeleteStepFromRecipe(TObject *Sender)
{
if(StringGrid1->Selection.Top!=StringGrid1->RowCount-1)
{
    for(int j =StringGrid1->Selection.Top;j<StringGrid1->RowCount;j++)
    {
        StringGrid1->Cells[0][j]=StringGrid1->Cells[0][j+1];
        StringGrid1->Cells[1][j]=StringGrid1->Cells[1][j+1];
    }
    Memo_receipt->Lines->Delete(StringGrid1->Selection.Top-1);

    for(int i = ActCell_Rec; i < StringGrid1->RowCount-1; i++)
        for(int j=0;j<PAR_COUNT;j++)
        {
            par_temp[i][j] = par_temp[i+1][j];
            par_temp[i+1][j] = 0;
            iniLib.par_lib[i-1][j]=iniLib.par_lib[i][j];
            iniLib.par_lib[i][j]="0";

        }
    StringGrid1->RowCount--;
    pr_lib=0;
    VisualParA();
    Label_Rec->Caption=" ������ : ����� ������";
    Label_Rec->Color=clBtnFace;
    Label_Rec->Font->Color=clBlack;
    //Refresh_kl_info("Clear");
}

}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveRecipe(TObject *Sender) //���������� �������//
{
    if((StringGrid1->RowCount>2)&&(Edit4->Text!=""))
    {
        for(int i=0;i<Memo_receipt->Lines->Count;i++)
        {
            AnsiString buf=Memo_receipt->Lines->Strings[i];
            AnsiString buf1[19]={""};
            int stage=0;
            for(int j=1;j<buf.Length();j++)
            {
                if(buf[j]=='#'){stage++;continue;}
                buf1[stage]=buf1[stage]+buf[j];
            }
            buf=buf1[0]+"#"+buf1[1]+"#"+buf1[2]+"#";
            // ������� ���
            buf =buf+ FloatToStrF((float)par_temp[i+1][0]*RRG1_MAX/4095.0,ffFixed,5,1)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][1]*RRG2_MAX/4095.0,ffFixed,5,1)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][2]*RRG3_MAX/4095.0,ffFixed,5,1)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][3]*RRG4_MAX/4095.0,ffFixed,5,1)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][4]*RRG5_MAX/4095.0,ffFixed,5,1)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][5]*RRG6_MAX/4095.0,ffFixed,5,1)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][6]*RRG7_MAX/4095.0,ffFixed,5,1)+"#";
            // �������� ��
            buf =buf+ FloatToStrF((float)par_temp[i+1][7]*P_IP_MAX/4095.0,ffFixed,5,0)+"#";
            // ��������� �������������
            buf =buf+ FloatToStrF((float)par_temp[i+1][8]*10.0/4095.0,ffFixed,5,2)+"#";
            buf =buf+ FloatToStrF((float)par_temp[i+1][9]*10.0/4095.0,ffFixed,5,2)+"#";
            // ��������
            buf =buf+ FloatToStrF((float)par_temp[i+1][10]*13.3/10000.0,ffFixed,5,2)+"#";
            // ����� ��������
            buf =buf+ int(par_temp[i+1][11]/3600000)+"#";
            buf =buf+ int((par_temp[i+1][11]%3600000)/60000)+"#";
            buf =buf+ int((par_temp[i+1][11]%60000)/1000)+"#";
            buf =buf+ int(par_temp[i+1][11]%1000)+"#";
            // ���������� ������
            buf =buf+ IntToStr(par_temp[i+1][12])+"#";


            Memo_receipt->Lines->Strings[i]=buf;
            stage=0;
            for(int j=0;j<19;j++)
                buf1[j]="";
            for(int j=1;j<buf.Length();j++)
            {
                if(buf[j]=='#'){stage++;continue;}
                buf1[stage]=buf1[stage]+buf[j];
            }
            for(int j =0;j<13;j++)
            {
                iniLib.par_lib[i][j]=buf1[3+j];
                if(j==11)
                    iniLib.par_lib[i][j]=
                    StrToInt(buf1[14])*3600000
                    +StrToInt(buf1[15])*60000
                    +StrToInt(buf1[16])*1000
                    +StrToInt(buf1[17]);
                if(j==12)
                   iniLib.par_lib[i][j]=buf1[18];
                
            }
        }
        Memo_receipt->Lines->SaveToFile("libs_1\\Libs\\"+Edit4->Text+".udb");
        LibListRefresh();

        pr_lib=1;
        VisualParA();
    }
}
//---------------------------------------------------------------------------
 void TForm1::LibListRefresh() // �������� ������� ���������
{
    // ����� �������� ������
    int
        fileCount,
        rezult;
    TSearchRec SR;

    // �������� ������ ���������
    fileCount = 0;
    ListBoxLibrary -> Clear();
    rezult = FindFirst("libs_1\\Libs\\*.udb", faAnyFile, SR);
    while ( !rezult )
    {
        fileCount++;
        SR.Name.SetLength( SR.Name.Length() - 4 );
        ListBoxLibrary -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };

    ListBoxLibrary -> Sorted;
}
//����� �������/////////////////////////////////////////////////////////////////
void __fastcall TForm1::ListBoxLibraryClick(TObject *Sender)
{   pr_err=0;
    Tek_LibRec_Name=ListBoxLibrary->Items->operator[](ListBoxLibrary -> ItemIndex);
    for(int i =1;i<StringGrid1->RowCount;i++)
    {StringGrid1->Cells[0][i]="";
    StringGrid1->Cells[1][i]="";}
    StringGrid1->RowCount=2;
    ActCell_Rec=1;
    if(StringGrid1->Cells[0][ActCell_Rec]=="000")pr_cikl=1;
    if(StringGrid1->Cells[0][ActCell_Rec]=="001")pr_rtrn=1;
    StringGrid1->Selection.Top=1;
    libNmb = ListBoxLibrary -> ItemIndex;
    Edit4->Text=ListBoxLibrary->Items->operator[](libNmb);
    Memo_receipt->Lines->Clear();
    Memo_receipt->Lines->LoadFromFile("libs_1\\Libs\\"+ListBoxLibrary->Items->operator[](libNmb)+".udb");
    int err_rec=0;
    for(int i =0;i<Memo_receipt->Lines->Count;i++)
    {
        AnsiString buf =Memo_receipt->Lines->Strings[i];
        AnsiString buf1[19]={""};
        int stage=0;
        for(int j=1;j<buf.Length();j++)
        {
            if(buf[j]=='#'){stage++;continue;}
            buf1[stage]=buf1[stage]+buf[j];

        }

        if(buf1[1]==klinfo[StrToInt(buf1[0])].hash)err_rec=0;
        else {err_rec=1;pr_err=1;}
            StringGrid1->Cells[0][i+1]=buf1[0];
        if(!err_rec)
            StringGrid1->Cells[1][i+1]=buf1[2];
        else
            StringGrid1->Cells[1][i+1]="(������) "+buf1[2];
        StringGrid1->RowCount++;
        if(!err_rec)
        {
            for(int j =0;j<14;j++)
            {

                iniLib.par_lib[i][j]=buf1[3+j];
                if(j==11)
                    iniLib.par_lib[i][j]=
                    StrToInt(buf1[14])*3600000
                    +StrToInt(buf1[15])*60000
                    +StrToInt(buf1[16])*1000
                    +StrToInt(buf1[17]);
                if(j==12)
                   iniLib.par_lib[i][j]=buf1[18];
                if(j==13)
                {    par_temp[i+1][j]=0;
                    AnsiString b="";
                    if( StrToInt(buf1[0])==0)
                        par_temp[i+1][j]=3000001;
                    else if( StrToInt(buf1[0])==1)
                        par_temp[i+1][j]=3000002;
                    else
                    {
                    for(int z=0;z<KL_COUNT;z++)
                        b=b+IntToStr(klinfo[StrToInt(buf1[0])].opcl[z]);
                    par_temp[i+1][j]=bintodec(b);
                    }
                }
            }
        }
        else
        {
            for(int j =0;j<14;j++)
            {
                if((j==8)||(j==9))iniLib.par_lib[i][j]="0.5";
                else if(j==13)par_temp[i+1][j]=0;
                else iniLib.par_lib[i][j]="0";


            }
        }
    }
    if(!pr_err)
    {
        Label_Rec->Caption="������: "+ListBoxLibrary->Items->Strings[libNmb];
        Label_Rec->Color=0x00400080;
        Label_Rec->Font->Color=clWhite;
    }
    else
    {
        Label_Rec->Caption="������: (������) "+ListBoxLibrary->Items->Strings[libNmb];
        Label_Rec->Color=0x00400080;
        Label_Rec->Font->Color=clWhite;
    }
    Refresh_kl_info("Clear");
    pr_lib=1;
    VisualParA();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RenameTree(TObject *Sender)
{
    if((Edit3->Text!="")&&(ComboBox1->ItemIndex!=0))
    {Memo_tree->Lines->Strings[ComboBox1->ItemIndex]=Edit3->Text;
    Memo_tree->Lines->SaveToFile("libs_1\\treenode.udb");
    RefreshShTree();}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::mode_select(TObject *Sender)
{
    if(((TButton*)Sender)->Hint==1)//����� �������
    {
        Panel_lib_rec   ->Visible   =true;
        Panel_rec       ->Visible   =true;
        Panel_lib_step  ->Visible   =false;
        Panel_sost_kl   ->Visible   =false;

        mode_1       ->Font->Color  =clSilver;
        mode_2       ->Font->Color  =clBlack;
        mode_3       ->Font->Color  =clBlack;
        Button1->Enabled=false;

    }
    else if(((TButton*)Sender)->Hint==2)//�������� �������
    {
        Panel_lib_rec   ->Visible   =false;
        Panel_rec       ->Visible   =true;
        Panel_lib_step  ->Visible   =true;
        Panel_sost_kl   ->Visible   =false;

        mode_1       ->Font->Color  =clBlack;
        mode_2       ->Font->Color  =clSilver;
        mode_3       ->Font->Color  =clBlack;

    }
    else//�������� ����
    {
        Panel_lib_rec   ->Visible   =false;
        Panel_rec       ->Visible   =false;
        Panel_lib_step  ->Visible   =true;
        Panel_sost_kl   ->Visible   =true;

        mode_1       ->Font->Color  =clBlack;
        mode_2       ->Font->Color  =clBlack;
        mode_3       ->Font->Color  =clSilver;
    }
}

//---------------------------------------------------------------------------

void __fastcall TForm1::CancelReceipe(TObject *Sender)
{

    Edit4->Text="";
    Memo_receipt->Lines->Clear();
    for(int i =1;i<StringGrid1->RowCount;i++)
    {
        StringGrid1->Cells[0][i]="";
        StringGrid1->Cells[1][i]="";
    }
    StringGrid1->RowCount=2;
    Label_Rec->Caption=" ������ : ����� ������";
    Label_Rec->Color=clBtnFace;
    Label_Rec->Font->Color=clBlack;

    VisualParA();
    //Refresh_kl_info("Clear");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DeleteLib(TObject *Sender)
{
    // �������� ����������
    // ���� ������� ������������ ���������
    if ( libNmb != -1 )
    {
        pr_lib = false;
        Refresh_kl_info("Clear");
        AnsiString fName = "libs_1\\Libs\\" + ListBoxLibrary->Items->operator[](libNmb) + ".udb";
        if(FileExists(fName))
        {
            DeleteFile(fName);
        }
    }
    Edit4->Text="";
    Memo_receipt->Lines->Clear();
    for(int i =1;i<StringGrid1->RowCount;i++)
    {
        StringGrid1->Cells[0][i]="";
        StringGrid1->Cells[1][i]="";
    }
    StringGrid1->RowCount=2;
    Label_Rec->Caption=" ������ : ����� ������";
    Label_Rec->Color=clBtnFace;
    Label_Rec->Font->Color=clBlack;
    LibListRefresh();
    Refresh_kl_info("Clear");
    VisualParA();
}
//---------------------------------------------------------------------------
void TForm1::VisualParA() // ����������� ���������� ��������
{
    // ����������� �������� ��������

    // �������� � ����������� �������
    EdtAKon20 -> Text = FloatToStrF((float)par[0][20], ffFixed, 5, 0);

    if((pr_cikl)&&(!pr_rtrn))
    {
        EdtARed0->Enabled=false;
        EdtARed1->Enabled=false;
        EdtARed2->Enabled=false;
        EdtARed3->Enabled=false;
        EdtARed4->Enabled=false;
        EdtARed5->Enabled=false;
        EdtARed6->Enabled=false;
        EdtARed7->Enabled=false;
        EdtARed8->Enabled=false;
        EdtARed9->Enabled=false;
        EdtARed10->Enabled=false;
        EdtARed11_3->Enabled=false;
        EdtARed11_2->Enabled=false;
        EdtARed11_1->Enabled=false;
        EdtARed11_0->Enabled=false;
        EdtARed12->Enabled=true;
    }
    if((!pr_cikl)&&(!pr_rtrn))
    {
        EdtARed0->Enabled=true;
        EdtARed1->Enabled=true;
        EdtARed2->Enabled=true;
        EdtARed3->Enabled=true;
        EdtARed4->Enabled=true;
        EdtARed5->Enabled=true;
        EdtARed6->Enabled=true;
        EdtARed7->Enabled=true;
        EdtARed8->Enabled=true;
        EdtARed9->Enabled=true;
        EdtARed10->Enabled=true;
        EdtARed11_3->Enabled=true;
        EdtARed11_2->Enabled=true;
        EdtARed11_1->Enabled=true;
        EdtARed11_0->Enabled=true;
        EdtARed12->Enabled=false;
    }
    if((!pr_cikl)&&(pr_rtrn))
    {
        EdtARed0->Enabled=false;
        EdtARed1->Enabled=false;
        EdtARed2->Enabled=false;
        EdtARed3->Enabled=false;
        EdtARed4->Enabled=false;
        EdtARed5->Enabled=false;
        EdtARed6->Enabled=false;
        EdtARed7->Enabled=false;
        EdtARed8->Enabled=false;
        EdtARed9->Enabled=false;
        EdtARed10->Enabled=false;
        EdtARed11_3->Enabled=false;
        EdtARed11_2->Enabled=false;
        EdtARed11_1->Enabled=false;
        EdtARed11_0->Enabled=false;
        EdtARed12->Enabled=false;
    }
     if((pr_lib)&&(pr_err))
    {
        EdtARed0->Enabled=false;
        EdtARed1->Enabled=false;
        EdtARed2->Enabled=false;
        EdtARed3->Enabled=false;
        EdtARed4->Enabled=false;
        EdtARed5->Enabled=false;
        EdtARed6->Enabled=false;
        EdtARed7->Enabled=false;
        EdtARed8->Enabled=false;
        EdtARed9->Enabled=false;
        EdtARed10->Enabled=false;
        EdtARed11_3->Enabled=false;
        EdtARed11_2->Enabled=false;
        EdtARed11_1->Enabled=false;
        EdtARed11_0->Enabled=false;
        EdtARed12->Enabled=false;
    }
    for(int i=0;i<Memo_receipt->Lines->Count;i++)
    {
        if(StringGrid1->Cells[0][i+1]=="000")
            StringGrid1->Cells[1][i+1]="���������� ������: "+iniLib.par_lib[i][12];
    }

    // ����������� ���������� ����������
    // ������� ���
    EdtAKon0->Text = FloatToStrF((float)par[ActCell_Rec][0]*RRG1_MAX/4095.0,ffFixed,5,1);
    EdtAKon1->Text = FloatToStrF((float)par[ActCell_Rec][1]*RRG2_MAX/4095.0,ffFixed,5,1);
    EdtAKon2->Text = FloatToStrF((float)par[ActCell_Rec][2]*RRG3_MAX/4095.0,ffFixed,5,1);
    EdtAKon3->Text = FloatToStrF((float)par[ActCell_Rec][3]*RRG4_MAX/4095.0,ffFixed,5,1);
    EdtAKon4->Text = FloatToStrF((float)par[ActCell_Rec][4]*RRG5_MAX/4095.0,ffFixed,5,1);
    EdtAKon5->Text = FloatToStrF((float)par[ActCell_Rec][5]*RRG6_MAX/4095.0,ffFixed,5,1);
    EdtAKon6->Text = FloatToStrF((float)par[ActCell_Rec][6]*RRG7_MAX/4095.0,ffFixed,5,1);
    // �������� ��
    EdtAKon7->Text = FloatToStrF((float)par[ActCell_Rec][7]*P_IP_MAX/4095.0,ffFixed,5,0);
    // ��������� �������������
    EdtAKon8->Text = FloatToStrF((float)par[ActCell_Rec][8]*10.0/4095.0,ffFixed,5,2);
    EdtAKon9->Text = FloatToStrF((float)par[ActCell_Rec][9]*10.0/4095.0,ffFixed,5,2);
    // ��������
    EdtAKon10->Text = FloatToStrF((float)par[ActCell_Rec][10]*13.3/10000.0,ffFixed,5,2);
    // ����� ��������
    EdtAKon11_3->Text = IntToStr(int(par[ActCell_Rec][11]/3600000));
    EdtAKon11_2->Text = IntToStr(int((par[ActCell_Rec][11]%3600000)/60000));
    EdtAKon11_1->Text = IntToStr(int((par[ActCell_Rec][11]%60000)/1000));
    EdtAKon11_0->Text = IntToStr(int(par[ActCell_Rec][11]%1000));
    // ���������� ������
    EdtAKon12->Text = IntToStr(par[ActCell_Rec][12]);

    // ����� ���� � ����������
    EdtAKon13 -> Text = IntToStr(par[ActCell_Rec][13]);

    // ����������� ��������� ����������
    // ������� ���
    EdtARed0 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][0]*RRG1_MAX/4095.0,ffFixed,5,1);
    EdtARed1 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][1]*RRG2_MAX/4095.0,ffFixed,5,1);
    EdtARed2 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][2]*RRG3_MAX/4095.0,ffFixed,5,1);
    EdtARed3 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][3]*RRG4_MAX/4095.0,ffFixed,5,1);
    EdtARed4 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][4]*RRG5_MAX/4095.0,ffFixed,5,1);
    EdtARed5 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][5]*RRG6_MAX/4095.0,ffFixed,5,1);
    EdtARed6 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][6]*RRG7_MAX/4095.0,ffFixed,5,1);
    // �������� ��
    EdtARed7 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][7]*P_IP_MAX/4095.0,ffFixed,5,0);
    // ��������� �������������
    EdtARed8 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][8]*10.0/4095.0,ffFixed,5,2);
    EdtARed9 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][9]*10.0/4095.0,ffFixed,5,2);
    // ��������
    EdtARed10 -> Text = FloatToStrF((float)par_temp[ActCell_Rec][10]*13.3/10000.0,ffFixed,5,2);
    // ����� ��������
    EdtARed11_3 -> Text = int(par_temp[ActCell_Rec][11]/3600000);
    EdtARed11_2 -> Text = int((par_temp[ActCell_Rec][11]%3600000)/60000);
    EdtARed11_1 -> Text = int((par_temp[ActCell_Rec][11]%60000)/1000);
    EdtARed11_0 -> Text = int(par_temp[ActCell_Rec][11]%1000);
    // ���������� ������
    EdtARed12 -> Text = IntToStr(par_temp[ActCell_Rec][12]);
    // ����� ���� � ����������
   EdtARed13 -> Text = par_temp[ActCell_Rec][13];
    
    // �������� ����� � �����

    DrawEditColor(EdtAKon0,EdtARed0);
    DrawEditColor(EdtAKon1,EdtARed1);
    DrawEditColor(EdtAKon2,EdtARed2);
    DrawEditColor(EdtAKon3,EdtARed3);
    DrawEditColor(EdtAKon4,EdtARed4);
    DrawEditColor(EdtAKon5,EdtARed5);
    DrawEditColor(EdtAKon6,EdtARed6);
    DrawEditColor(EdtAKon7,EdtARed7);
    DrawEditColor(EdtAKon8,EdtARed8);
    DrawEditColor(EdtAKon9,EdtARed9);
    DrawEditColor(EdtAKon10,EdtARed10);
    DrawEditColor(EdtAKon11_0,EdtARed11_0);
    DrawEditColor(EdtAKon11_1,EdtARed11_1);
    DrawEditColor(EdtAKon11_2,EdtARed11_2);
    DrawEditColor(EdtAKon11_3,EdtARed11_3);
    DrawEditColor(EdtAKon12,EdtARed12);

    // ����������� ������������ ����������
    if((pr_lib)&&(!pr_err))
    {
        EdtALib0 -> Text = iniLib.par_lib[ActCell_Rec-1][0];
        EdtALib1 -> Text = iniLib.par_lib[ActCell_Rec-1][1];
        EdtALib2 -> Text = iniLib.par_lib[ActCell_Rec-1][2];
        EdtALib3 -> Text = iniLib.par_lib[ActCell_Rec-1][3];
        EdtALib4 -> Text = iniLib.par_lib[ActCell_Rec-1][4];
        EdtALib5 -> Text = iniLib.par_lib[ActCell_Rec-1][5];
        EdtALib6 -> Text = iniLib.par_lib[ActCell_Rec-1][6];
        // �������� ��
        EdtALib7 -> Text = iniLib.par_lib[ActCell_Rec-1][7];
        // ��������� �������������
        EdtALib8 -> Text = iniLib.par_lib[ActCell_Rec-1][8];
        EdtALib9 -> Text = iniLib.par_lib[ActCell_Rec-1][9];
        // ��������
        EdtALib10 -> Text = iniLib.par_lib[ActCell_Rec-1][10];
        // ����� ��������
        EdtALib11_3 -> Text = StrToInt(iniLib.par_lib[ActCell_Rec-1][11])/3600000;
        EdtALib11_2 -> Text = (StrToInt(iniLib.par_lib[ActCell_Rec-1][11])%3600000)/60000;
        EdtALib11_1 -> Text = (StrToInt(iniLib.par_lib[ActCell_Rec-1][11])%60000)/1000;
        EdtALib11_0 -> Text = StrToInt(iniLib.par_lib[ActCell_Rec-1][11])%1000;
        // ���������� ������
        EdtALib12 -> Text = iniLib.par_lib[ActCell_Rec-1][12];
        // ����� ���� � ����������
        //EdtALib13 -> Text = iniLib.par_lib[ActCell_Rec-1][13];

    }
    else
    {
        // �������� ������ ������������ ����������
        EdtALib0 -> Text = "";
        EdtALib1 -> Text = "";
        EdtALib2 -> Text = "";
        EdtALib3 -> Text = "";
        EdtALib4 -> Text = "";
        EdtALib5 -> Text = "";
        EdtALib6 -> Text = "";
        EdtALib7 -> Text = "";
        EdtALib8 -> Text = "";
        EdtALib9 -> Text = "";
        EdtALib10 -> Text = "";
        EdtALib11_0 -> Text = "";
        EdtALib11_1 -> Text = "";
        EdtALib11_2 -> Text = "";
        EdtALib11_3-> Text = "";
        EdtALib12-> Text = "";
        EdtALib13 -> Text = "";


    }
    if((pr_err)||(pr_cikl)||(pr_rtrn))
        Refresh_kl_info("Clear");
}
void TForm1::Save_parA_temp() // ���������� ���������� �� ��������� ������
{
    // ������� ���
    par_temp[ActCell_Rec][0] = int(StrToFloat(EdtARed0->Text)*4095.0/RRG1_MAX);
    par_temp[ActCell_Rec][1] = int(StrToFloat(EdtARed1->Text)*4095.0/RRG2_MAX);
    par_temp[ActCell_Rec][2] = int(StrToFloat(EdtARed2->Text)*4095.0/RRG3_MAX);
    par_temp[ActCell_Rec][3] = int(StrToFloat(EdtARed3->Text)*4095.0/RRG4_MAX);
    par_temp[ActCell_Rec][4] = int(StrToFloat(EdtARed4->Text)*4095.0/RRG5_MAX);
    par_temp[ActCell_Rec][5] = int(StrToFloat(EdtARed5->Text)*4095.0/RRG6_MAX);
    par_temp[ActCell_Rec][6] = int(StrToFloat(EdtARed6->Text)*4095.0/RRG7_MAX);
    // �������� ��
    par_temp[ActCell_Rec][7] = int(StrToFloat(EdtARed7 ->Text)*4095.0/P_IP_MAX);
    // ��������� �������������
    par_temp[ActCell_Rec][8] = int(StrToFloat(EdtARed8 ->Text)*4095.0/10.0);
    par_temp[ActCell_Rec][9] = int(StrToFloat(EdtARed9 ->Text)*4095.0/10.0);
    // ��������
    par_temp[ActCell_Rec][10] = int(StrToFloat(EdtARed10 ->Text)*10000.0/13.3);
    // ����� ��������
    par_temp[ActCell_Rec][11] = StrToInt(EdtARed11_0 ->Text) +
                                StrToInt(EdtARed11_1 ->Text)*1000 +
                                StrToInt(EdtARed11_2 ->Text)*60000 +
                                StrToInt(EdtARed11_3 ->Text)*3600000;
    // ���������� ������
    par_temp[ActCell_Rec][12] = int(StrToFloat(EdtARed12 ->Text));
    // ���������� �������
    par_temp[ActCell_Rec][20] = int(StrToFloat(EdtARed20 ->Text));
}








void __fastcall TForm1::ReceptToWork(TObject *Sender)
{
    if(pr_err)return;
    pr_lib = false;
    SHarr_size = SHarr_lib_size;
    Tek_Rec_Name = Tek_LibRec_Name;
    for(int i=1;i<StringGrid1->RowCount-1;i++)
    {
        // ������� ���
        par_temp[i][0] = int(StrToFloat(iniLib.par_lib[i-1][0])*4095.0/RRG1_MAX);
        par_temp[i][1] = int(StrToFloat(iniLib.par_lib[i-1][1])*4095.0/RRG2_MAX);
        par_temp[i][2] = int(StrToFloat(iniLib.par_lib[i-1][2])*4095.0/RRG3_MAX);
        par_temp[i][3] = int(StrToFloat(iniLib.par_lib[i-1][3])*4095.0/RRG4_MAX);
        par_temp[i][4] = int(StrToFloat(iniLib.par_lib[i-1][4])*4095.0/RRG5_MAX);
        par_temp[i][5] = int(StrToFloat(iniLib.par_lib[i-1][5])*4095.0/RRG6_MAX);
        par_temp[i][6] = int(StrToFloat(iniLib.par_lib[i-1][6])*4095.0/RRG7_MAX);
        // �������� ��
        par_temp[i][7] = int(StrToFloat(iniLib.par_lib[i-1][7])*4095.0/P_IP_MAX);
        // ��������� �������������
        par_temp[i][8] = int(StrToFloat(iniLib.par_lib[i-1][8])*4095.0/10.0);
        par_temp[i][9] = int(StrToFloat(iniLib.par_lib[i-1][9])*4095.0/10.0);
        // ��������
        par_temp[i][10] = int(StrToFloat(iniLib.par_lib[i-1][10])*10000.0/13.3);
        // ����� ��������
        par_temp[i][11] =   StrToInt(iniLib.par_lib[i-1][11]) ;
        // ���������� ������
        par_temp[i][12] = int(StrToFloat(iniLib.par_lib[i-1][12]));
       
    }
    Label_Rec->Color=clBtnFace;
    Label_Rec->Font->Color=clBlack;
    VisualParA();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClearAllStepInRecept(TObject *Sender)
{
    ActCell_Rec=1;
    pr_lib=0;
    Refresh_kl_info("Clear");
    for(int i=1;i<StringGrid1->RowCount-1;i++)
    {
        StringGrid1->Cells[0][i]="";
        StringGrid1->Cells[1][i]="";
    }
    for(int i=1;i<StringGrid1->RowCount-1;i++)
        for(int j=0;j<13;j++)
            iniLib.par_lib[i-1][j]="0";

    StringGrid1->RowCount=2;
    VisualParA();
    Label_Rec->Caption=" ������ : ����� ������";
    Label_Rec->Color=clBtnFace;
    Label_Rec->Font->Color=clBlack;
    Memo_receipt->Lines->Clear();

}
//---------------------------------------------------------------------------
void TForm1::Refresh_kl_info(AnsiString a)
{
    if(a!="Clear")
    {
        for(int i=1; i<=KL_COUNT;i++)
        {
            if(i<=a.Length())
            {
                if(a[i]=='1') Cb_Klapan_Info[i-1]->Picture->Bitmap=check1->Picture->Bitmap;
                else          Cb_Klapan_Info[i-1]->Picture->Bitmap=check0->Picture->Bitmap;
            }
            else              Cb_Klapan_Info[i-1]->Picture->Bitmap=check0->Picture->Bitmap;
        }
    }
    else
    {
        for(int i=0; i<21;i++)
        {
           Cb_Klapan_Info[i]->Picture->Bitmap=check2->Picture->Bitmap;
        }
    }

}


void __fastcall TForm1::Timer1SecTimer(TObject *Sender)
{
    AnsiString date_now;
    unsigned short date_hour, date_minutes, date_seconds, date_miliseconds;
    date_now=Now().TimeString();
    DecodeTime(date_now,date_hour,date_minutes,date_seconds,date_miliseconds);

    if((date_hour==23)&&(date_minutes==59)&&((date_seconds>=40)&&(date_seconds<=49)))
    {
        // ��������� �����
    AnsiString fileName;
    ///////////////////////////////////////////////////////////////////////////////////////////
    // ����������� ��������� � �������� ������
    if(!DirectoryExists("Diag")) { CreateDir("Diag"); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    fileName = "Diag\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    ///////////////////////////////////////////////////////////////////////////////////////////
    if(!FileExists(fileName))
    {   int fileID = FileCreate(fileName);
        FileClose(fileID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<MemoDiag -> Lines -> Count; i++)
    {   MemoTemp -> Lines -> Add(MemoDiag -> Lines -> operator[](i)); }


    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> SaveToFile( fileName );
    ///////////////////////////////////////////////////////////////////////////////////////////
    // �������� ������������ � �������� ������
    if(!DirectoryExists("Stat")) { CreateDir("Stat"); }
    fileName = "Stat\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    if(!FileExists(fileName))
    {   int fileID = FileCreate(fileName);
        FileClose(fileID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<MemoStat -> Lines -> Count; i++)
    {   MemoTemp -> Lines -> Add(MemoStat -> Lines -> operator[](i)); }

    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> SaveToFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    // ������� ������� ���������� ��������
    if(!DirectoryExists("Graph")) { CreateDir("Graph"); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    fileName = "Graph\\" + FormatDateTime("yyyy_mm_dd",Date()) + ".txt";
    ///////////////////////////////////////////////////////////////////////////////////////////
    if(!FileExists(fileName))
    {   int fileID=FileCreate(fileName);
        FileClose(fileID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> Clear();
    MemoTemp -> Lines -> LoadFromFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<(MemoGraph -> Lines -> Count - 1); i++)
    {  MemoTemp -> Lines -> Add(MemoGraph -> Lines -> operator[](i)); }
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines -> SaveToFile(fileName);
    ///////////////////////////////////////////////////////////////////////////////////////////
    MemoTemp -> Lines ->  Clear();


    serTemp[1]->Clear();
    serTemp[2]->Clear();
    serTemp[3]->Clear();
    serTemp[4]->Clear();
    serTemp[5]->Clear();
    serTemp[6]->Clear();
    serTemp[7]->Clear();
    serTemp[8]->Clear();
    serTemp[9]->Clear();

    MemoDiag    -> Lines -> Clear();
    MemoStat -> Lines -> Clear();
    MemoGraph -> Lines -> Clear();
    
    serArh[1]->Clear();
    serArh[2]->Clear();
    serArh[3]->Clear();
    serArh[4]->Clear();
    serArh[5]->Clear();
    serArh[6]->Clear();
    serArh[7]->Clear();
    serArh[8]->Clear();
    serArh[9]->Clear();

    MemoDiagArh    -> Lines -> Clear();
    MemoStatArh -> Lines -> Clear();
//    MemoGraphArh -> Lines -> Clear();
    // ����� �������� ������
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

