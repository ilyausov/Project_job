//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "stdio.h"
#include "ctype.h"

#include "Logic.cpp"
#include "Header.h"

#include "Modules\Com\Com.cpp"
#include "Modules\DZaslVAT\DZaslVAT.cpp"
#include "Modules\TRMD\TRMD.cpp"
#include "Modules\DatMPT200\DatMPT200.cpp"
#include "Modules\DatPPT200\DatPPT200.cpp"
#include "Modules\AZdrive\AZdrive.cpp"
#include "Modules\BPM_HP\BPM_HP.cpp"
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

    // ������ PISO-P32C32(1)
    if ( externalTask & 0x01 )
    {
        // ��������
        externalError = PISO_P32C32U_1( 0 , zin );
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
     // ������ PISO-P32C32(2)
    if ( externalTask & 0x02 )
    {
        // ��������
        externalError = PISO_P32C32U_2( 0 , zin );
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
    if( externalTask & 0x04 )
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

    // ������ ���������� ������� �������� � PISO-813
    if(externalTask & 0x08)
    {
        // ��������
        externalError = PISO_813U(aik , AIK_COUNT * 8);
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

    // �������� � PISO-P32�32(1)
    if(externalTask & 0x10)
    {
         // ��������
         externalError = PISO_P32C32U_1( 1 , out );
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
    // �������� � PISO-P32�32(2)
    if(externalTask & 0x20)
    {
         // ��������
         externalError = PISO_P32C32U_2( 1 , out );
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
    if(externalTask & 0x40)
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
    if( externalTask & 0x80 )
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


    // ������������� ������� ����������� �������� ��������
    serArh[0] = Series11;
    serArh[1] = Series12;
    serArh[2] = Series13;
    serArh[3] = Series14;
    serArh[4] = Series15;
    serArh[5] = Series16;
    serArh[6] = Series17;
    serArh[7] = Series18;


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
    // ������ ������� ����������
    if( FileExists( "Res\\Res.txt" ) )
    {
        MemoRes -> Lines -> LoadFromFile("Res\\Res.txt");
        magnRes1 = StrToFloat( MemoRes -> Lines -> operator [](0) );
    }
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
        if(shr[40]||shr[41])
            DrawPaint->Visible=true;



        PnlMnemoParam -> Height = 344; //��������� ������ �������
        Pnl_GK->BringToFront();

        //�������� ����� ��������
        Panel663 -> Visible = false;
        Panel665 -> Visible = false;
        EdtZadA12 -> Visible = false;
        EdtTekA12 -> Visible = false;
        State     -> Visible = false;
        Panel317->BringToFront();


        VisualMnemo();
        Pnl_Work -> Visible = true;
  }
  else if(PCMain -> ActivePage == TSWork)
  {     Pnl_Work -> Visible = false;
        Pnl_Work -> Parent = TSWork;

        DrawPaint->Visible=false;
        Pnl_Work -> Top = 32;
        Pnl_Work -> Left = 8;

        PnlMnemoParam -> Height = 370; //����������� ������ �������

        //��������� ����� ��������
        Panel663 -> Visible = true;
        Panel665 -> Visible = true;
        EdtZadA12 -> Visible = true;
        EdtTekA12 -> Visible = true;
        State     -> Visible = true;



        VisualMnemo();
        Pnl_Work -> Visible = true;
  }
}
//---------------------------------------
/////// ��������� ��������� �������
//---------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
FI = 30.96*M_PI/180.0;
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
        Check_Out[i][j] -> Transparent = true;
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
     Form1->GB_aout1
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
        CG_Aout[i][j] -> MinValue = 8193;
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
        CG_Aout_zad[i][j] -> MinValue = 8193;
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
        Zad_Aout[i][j] -> Min = 8193;
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
    OpenPISO_P32C32(); // ������� ��������� � ��������� ISO-P32C32
    OpenACL_7250();   // ������� ��������� � ��������� ACL-7250
    OpenPISO_813 ();    // ������� ��������� � ��������� ISO-813
    OpenISO_DA16();    // ������� ��������� � ��������� ISO-DA16
         
    PCMain  -> DoubleBuffered = true;
    Form1-> DoubleBuffered = true;
    Pnl_Title -> DoubleBuffered = true;

    Label_Time -> Caption = FormatDateTime("hh:mm:ss",Time());
    Label_Date -> Caption = FormatDateTime("dd.mm.yyyy",Date());
    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> LoadFromFile("Nasmod\\Nasmod.txt");
    EditNastrTo0  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](0));
    EditNastrTo1  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](1));
    EditNastrTo2  -> Text = MemoNasmod -> Lines -> operator [](2);
    EditNastrTo3  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](3));
    EditNastrTo4  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](4));
    EditNastrTo5  -> Text = MemoNasmod -> Lines -> operator [](5);
    EditNastrTo6  -> Text = MemoNasmod -> Lines -> operator [](6);
    EditNastrTo7  -> Text = MemoNasmod -> Lines -> operator [](7);
    EditNastrTo8  -> Text = MemoNasmod -> Lines -> operator [](8);
    EditNastrTo9  -> Text = MemoNasmod -> Lines -> operator [](9);
    EditNastrTo10  -> Text = MemoNasmod -> Lines -> operator [](10);
    EditNastrTo11  -> Text = MemoNasmod -> Lines -> operator [](11);
    EditNastrTo12  -> Text = MemoNasmod -> Lines -> operator [](12);
    EditNastrTo13  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](13));
    EditNastrTo14  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](14));
    EditNastrTo15  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](15));
    EditNastrTo16  -> Text = MemoNasmod -> Lines -> operator [](16);
    EditNastrTo17  -> Text = MemoNasmod -> Lines -> operator [](17);

    MemoNasmod -> Lines -> Clear();


    // ��������� �������� T
    MemoT -> Lines -> LoadFromFile("Nasmod\\Mex.txt");
    EdtTRed1  -> Text = MemoT -> Lines -> operator [](0);
    MemoT -> Lines -> Clear();

    BtnNastrDaClick(BtnNastrDa);
    BtnAutoDaClick(BtnAutoDa);
    BtnTrDaClick(BtnTrDa);
    BtnTDaClick(BtnTDa);
    BtnRDaClick(BtnRDa);

    Init_SComport();
	Comport[0]->Reser_Port(Comport[0]->BTN_reset);  // ��������� �����
    Comport[1]->Reser_Port(Comport[1]->BTN_reset);  // ��������� �����
    Comport[2]->Reser_Port(Comport[2]->BTN_reset);  // ��������� �����
    Comport[3]->Reser_Port(Comport[3]->BTN_reset);  // ��������� �����
    Comport[4]->Reser_Port(Comport[4]->BTN_reset);  // ��������� �����
    Comport[5]->Reser_Port(Comport[5]->BTN_reset);  // ��������� �����

    Init_DZaslVAT();
    Init_DatMPT200();
    Init_DatPPT200();
    Init_TRMD();
    Init_BU_IVE();
    Init_SAZ_drive();
    AZdrive_Load();	// �������� ���������� ���������

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
    PCNalad->Pages[8]->TabVisible = iniPAS.state[8];
    CB_Acc_V9->Checked = !iniPAS.state[8];

    // ���� ���������� �������������, ������������� �� ������
    if(!PCNalad->Pages[0]->TabVisible)
		PCNalad->ActivePage = TSNaladAcc;

    SetOut(1,2,0x40);				// ��������� ���� ����������

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
    //�������
    if((D_D1>=0)&&(D_D1<=10000))  { d_d1->Caption=FloatToStrF(pow(10,(float)D_D1/1000.0*1.667-9.333),ffExponent,3,8); } //���200
    if(D_D2<1000) d_d2->Caption = "0";
    else if(D_D2>9000) d_d2->Caption = "10,0";
    else { d_d2->Caption=FloatToStrF((float(D_D2) - 1000.0)*DAVL_MAX/8000.0,ffFixed,5,1);}  //��������
    if((D_D3>=0)&&(D_D3<=10000))  { d_d3->Caption=FloatToStrF(pow(10,(float)D_D3/1000.0*1.667-9.333),ffExponent,3,8); } //���200
    if((D_D4>=0)&&(D_D4<=10000))  { d_d4->Caption=FloatToStrF(pow(10,(float)D_D4/1000.0*1.667-9.333),ffExponent,3,8); } //���200
    if((D_D5>=0)&&(D_D5<=10000))  { d_d5->Caption=FloatToStrF(pow(10,(float)D_D5/1000.0-3.5),ffExponent,3,8); } //���200
    if((D_D6>=0)&&(D_D6<=10000))  { d_d6->Caption=FloatToStrF(pow(10,(float)D_D6/1000.0-3.5),ffExponent,3,8); } //���200

    if(zin[4]&0x02)
        datch->Visible=true;
    else
        datch->Visible=false;
    //����������� ������������ �������
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ������������ � ������ ������� ���������� 1
    if((shr[33])&& (( OTVET_BMH1[6] * 6144.0/1024.0 ) > 100 ) )
        magnRes1 += StrToFloat((float)OTVET_BMH1[6]/1024.0*6144.0/3600.0/1000.0);
    Form1->EditRESm1->Text = FloatToStrF(magnRes1,ffFixed,6,3);

    if(shr[28]) 
    {
        if(N_TEK_GIS!=0) { coef_pd_tek->Caption = FloatToStrF(1000.0/(float)N_TEK_GIS,ffFixed,5,0); } // ���. �����. ����
        else             { coef_pd_tek->Caption = 0;}
    }
    else coef_pd_tek->Caption = 0;
    //����������� ������������ ��.������
    if(nasmod[14])
        coef_pd_zad->Caption = FloatToStrF(1000.0/(float)nasmod[14],ffFixed,5,0); // �������� �����. ���� �/�
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ����� ������
    if(shr[3]||shr[4]||shr[2])
    switch (N_ST)
    {
        case 1: {EditNST -> Caption = "������";             break;}
        case 2: {EditNST -> Caption = "��-�������";         break;}
        case 3: {EditNST -> Caption = "�����";     break;}
        case 4: {EditNST -> Caption = "���������";     break;}
        case 5: {EditNST -> Caption = "���������� �";     break;}
        default: {EditNST -> Caption = "";                  break;}
    }
    else
    EditNST -> Caption = "";
    //--------------------------------------------------------------------------
    //�����������
    Temp_PD -> Caption = FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1)+ "��";//�/�

    
    Temp_K  -> Caption = FloatToStrF((float(TEK_TEMP1)/10.0),ffFixed,4,1)+ "��";//������
    //--------------------------------------------------------------------------
    //��
    poz_DZ_k  -> Caption = FloatToStrF((float(TEK_POZ_DZASL1)/10000.0*100.0),ffFixed,3,0) + "%";
    poz_DZ_m  -> Caption = FloatToStrF((float(TEK_POZ_DZASL2)/10000.0*100.0),ffFixed,3,0) + "%";
    //--------------------------------------------------------------------------



        

    if(PCMain -> ActivePage == TSNalad) // �������
    {
    //����������� �����
    Gas_one->Caption = IntToStr(par[0][15]) + "/"+IntToStr(par[0][16]);
    if(shr[43])
    {

        if(aik[15])
            Gas_two->Caption = IntToStr(par[0][15]) + "/"+FloatToStrF((float(aik[16])*(float)par[0][15]/(float)aik[15]),ffFixed,5,0);
    }
    else
    {
        
        Gas_two->Caption =0;
    }
    
    //**********************������� �������*************************************
    EdtZadA00-> Text =FloatToStrF((float)nasmod[6]/10.0,ffFixed,5,1);//����������� ������� �/�
    EdtZadA01-> Text =FloatToStrF((float)par[0][0]* RRG1_MAX / 4095.0, ffFixed, 5, 2);//������ ���1
    EdtZadA02-> Text =FloatToStrF((float)par[0][1]* RRG2_MAX / 4095.0, ffFixed, 5, 2);//������ ���2
    EdtZadA03-> Text =FloatToStrF((float)par[0][2]* RRG3_MAX / 4095.0, ffFixed, 5, 1);//������ ���3
    EdtZadA04-> Text =FloatToStrF((float)par[0][8]/ 4095.0 * CESAR_MAX_PD, ffFixed, 5, 0);//���
    EdtZadA07-> Text =FloatToStrF((float)par[0][6]/ 4095.0 * 6000, ffFixed, 5, 0);//�
    EdtZadA10-> Text =FloatToStrF((float)par[0][7]/4095.0*600.0, ffFixed, 5, 0);
    EdtZadA11-> Text =FloatToStrF(((float)par[0][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);//��������
    EdtZadA12-> Text ="0";
    //**********************������� �������*************************************
    EdtTekA00-> Text =FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1);         //����������� �/�
    EdtTekA01-> Text =FloatToStrF((float)aik[6]*RRG1_MAX/4095.0,  ffFixed, 6, 1); // ������ ���1
    EdtTekA02-> Text =FloatToStrF((float)aik[7]*RRG2_MAX/4095.0,  ffFixed, 6, 1); // ������ ���2
    EdtTekA03-> Text =FloatToStrF((float)aik[8]*RRG3_MAX/4095.0,  ffFixed, 6, 1); // ������ ���3
    EdtTekA04-> Text =FloatToStrF((float)aik[10]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // �������� �������� ��� ��
    EdtTekA05-> Text =FloatToStrF((float)aik[11]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // ���������� �������� ��� ��
    EdtTekA06-> Text =FloatToStrF((float)aik[14]*SMESH_MAX_USER/4095.0, ffFixed, 6, 0); // ��������
    EdtTekA07-> Text =FloatToStrF((float)OTVET_BMH1[6]*6144.0/1023.0,ffFixed,6,0);//�������� �
    EdtTekA08-> Text =FloatToStrF((float)OTVET_BMH1[5]*750.0/1023.0,ffFixed,6,0); // ���������� �
    EdtTekA09-> Text =FloatToStrF((float)OTVET_BMH1[4]*10.24/1023.0,ffFixed,6,1); // ��� �
    EdtTekA10-> Text =FloatToStrF((float)OTVET_BMH1[7]*614.4/1023.0,ffFixed,6,0);//���������� ��� �
    if(shr[27])EdtTekA11-> Text =FloatToStrF(((float)D_D2 - 1000)/8000*DAVL_MAX,ffFixed,5,1);  //��������
    else       EdtTekA11-> Text ="0,0";
    EdtTekA12-> Text ="0";
    }
    else // ����������
    {    Gas_one->Caption = IntToStr(par[4][15]) + "/"+IntToStr(par[4][16]);
        //����������� �����
    if((shr[43])&&(shr[3]))
    {

        if(aik[15])
            Gas_two->Caption = IntToStr(par[N_ST][15]) + "/"+FloatToStrF((float(aik[16])*(float)par[N_ST][15]/(float)aik[15]),ffFixed,5,0);
    }
    else
    {
        
        Gas_two->Caption =0;
    }
  //***************************������� �������**********************************
        //������ �/�
        EdtZadA00-> Text =FloatToStrF((float)nasmod[6]/10.0,ffFixed,5,1);//����������� ������� �/�

        //���1
        if(((shr[3]&&!PR_NALADKA)||shr[2])&&shr[20]) { EdtZadA01-> Text = FloatToStrF((float)par[N_ST][0]/4095.0*RRG1_MAX,    ffFixed,5,2); }
        else EdtZadA01-> Text = "0,00";
        //���2
        if((shr[3]&&!PR_NALADKA)&&shr[21])EdtZadA02-> Text =FloatToStrF((float)par[N_ST][1]/4095.0*RRG2_MAX,        ffFixed,5,2);
        else EdtZadA02-> Text = "0,00";
        //���3
        if((shr[3]&&!PR_NALADKA)&&shr[22])EdtZadA03-> Text =FloatToStrF((float)nasmod[2]/4095.0*RRG3_MAX,        ffFixed,5,1);
        else EdtZadA03-> Text = "0,0";

        //�������� �������� ��� �/�
        if(shr[3]&&((N_ST==2)||(N_ST==4))&&!PR_NALADKA)
            EdtZadA04-> Text =FloatToStrF((float)par[N_ST][8]/ 4095.0* CESAR_MAX_PD, ffFixed, 5, 0);
        else
            EdtZadA04-> Text ="0";
        //�������� �
        if(((shr[3])&&((N_ST==3)||(N_ST==4)))||((shr[2])&&((N_ST==5))))
            EdtZadA07-> Text =FloatToStrF((float)par[N_ST][6]/4095.0*6000 , ffFixed, 5, 0);
        else EdtZadA07-> Text =0;
        //���������� ��� �
        if((shr[3])&&(N_ST==4))
            EdtZadA10-> Text =FloatToStrF((float)par[N_ST][7]/4095.0*600.0, ffFixed, 5, 0);
        else EdtZadA10-> Text =0;
        //��������
        if(((shr[3])&&((N_ST>1)&&(N_ST<5)))||((shr[2])&&((N_ST==5))))
            EdtZadA11-> Text =FloatToStrF(((float)par[N_ST][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);
        else EdtZadA11-> Text ="0,0";
        //����� ��������
        if(shr[2]||shr[3])
            EdtZadA12-> Text =FloatToStrF((float)par[N_ST][12],ffFixed,5,0);   //����� ��������
        else
            EdtZadA12-> Text ="0,0";
  //******************������� �������*******************************************
        if(shr[31])EdtTekA00-> Text =FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1);         //����������� �/�
        else                EdtTekA00-> Text ="0,0";
        //���1
        if(shr[20]&&((shr[3]&&!PR_NALADKA)||(shr[2]))) { EdtTekA01-> Text = FloatToStrF((float)aik[6]*RRG1_MAX/4095.0,ffFixed,5,1); }
        else        { EdtTekA01-> Text = "0,0"; }
        //���2
        if(shr[21]&&(shr[3]&&!PR_NALADKA)) { EdtTekA02-> Text = FloatToStrF((float)aik[7]*RRG2_MAX/4095.0,ffFixed,5,1); }
        else        { EdtTekA02-> Text = "0,0"; }
        //���3
        if(shr[22]&&(shr[3]&&!PR_NALADKA)) { EdtTekA03-> Text = FloatToStrF((float)aik[8]*RRG3_MAX/4095.0,ffFixed,5,1); }
        else        { EdtTekA03-> Text = "0,0"; }
        if(shr[28])
        {
            EdtTekA04-> Text =FloatToStrF((float)aik[10]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // �������� �������� ��� ��
            EdtTekA05-> Text =FloatToStrF((float)aik[11]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // ���������� �������� ��� ��
            EdtTekA06-> Text =FloatToStrF((float)aik[14]*SMESH_MAX_USER/4095.0, ffFixed, 6, 0); // �������� ��� ��
        }
        else
        {
            EdtTekA04-> Text ="0"; // �������� �������� ��� ��
            EdtTekA05-> Text ="0"; // ���������� �������� ��� ��
            EdtTekA06-> Text ="0"; // �������� ��� ��
        }
        //�
        if(shr[33]&&((shr[3]&&!PR_NALADKA)||shr[2]))
        {
            EdtTekA07-> Text =FloatToStrF((float)OTVET_BMH1[6]*6144.0/1023.0,ffFixed,6,0);//�������� �
            EdtTekA08-> Text =FloatToStrF((float)OTVET_BMH1[5]*750.0/1023.0,ffFixed,6,0); // ���������� �
            EdtTekA09-> Text =FloatToStrF((float)OTVET_BMH1[4]*10.24/1023.0,ffFixed,6,1); // ��� �
            EdtTekA10-> Text =FloatToStrF((float)OTVET_BMH1[7]*614.4/1023.0,ffFixed,6,0);//���������� ��� �

        }
        else
        {
            EdtTekA07-> Text =0;
            EdtTekA08-> Text =0;
            EdtTekA09-> Text =0;
            EdtTekA10-> Text ="0";//��� �������� �
        }
        if((shr[3]||shr[2])&&shr[27])
            EdtTekA11-> Text =FloatToStrF(((float)D_D2-1000)/8000*DAVL_MAX,ffFixed,5,1);  //��������
        else
            EdtTekA11-> Text ="0,0";
        if(((shr[4]||shr[2])&&!PR_NALADKA))EdtTekA12 -> Text = IntToStr(T_PROC);
        else                     EdtTekA12 -> Text ="0";

    }

              


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
    //����������
    PnlTR  -> Font -> Color = SetPopeColor(shr[2]);


    ///////////////////////////////////////////////////////////////////////////




    // ���1 (���.)
    Pan_RRG1_On     -> Font -> Color = SetPopeColor(shr[20]);
    // ���2 (���.)
    Pan_RRG2_On     -> Font -> Color = SetPopeColor(shr[21]);
    // ���3 ���(���.)
    Pan_RRG3_Kam    -> Font -> Color = SetPopeColor(shr[22]&&!PR_RG3);
    // ���3 ���(���.)
    Pan_RRG3_Nas    -> Font -> Color = SetPopeColor(shr[22]&&PR_RG3);
//******************************************************************************
    //�� ��� �������
    Pan_Dz_Kam_Open     -> Font -> Color = SetPopeColor(shr[25]);
    //�� ��� �������
    Pan_Dz_Kam_Close    -> Font -> Color = SetPopeColor(shr[26]);
    //�� ��� ����
    Pan_Dz_Kam_Dross    -> Font -> Color = SetPopeColor(shr[27]);
    //�� ���� �������
    Pan_Dz_Mass_Open    -> Font -> Color = SetPopeColor(shr[37]);
    //�� ���� �������
    Pan_Dz_Mass_Close   -> Font -> Color = SetPopeColor(shr[38]);
    //�� ���� �� ����
    Pan_Dz_Mass_Ugol    -> Font -> Color = SetPopeColor(shr[39]);
    //��� ���
    Pan_Vchg_On         -> Font -> Color = SetPopeColor(shr[28]);
    //��� ���
    Pan_Bpm_On          -> Font -> Color = SetPopeColor(shr[33]);
    //��� ����
    Pan_Bpm_Off         -> Font -> Color = SetPopeColor(shr[34]);
    //���� ���� ���
    Pan_VrashMagn_On    -> Font -> Color = SetPopeColor((shr[47])||(zin[2]&0x200));
    //���� ���� ����
    Pan_VrashMagn_Off   -> Font -> Color = SetPopeColor(shr[48]);
    //������ ��� ���
    Pan_Nagr_Kam_On     -> Font -> Color = SetPopeColor(shr[29]);
    //������ ��� ����
    Pan_Nagr_Kam_Off    -> Font -> Color = SetPopeColor(shr[30]);
    //������ �/� ���
    Pan_Nagr_Pd_On      -> Font -> Color = SetPopeColor(shr[31]);
    //������ �/� ����
    Pan_Nagr_Pd_Off     -> Font -> Color = SetPopeColor(shr[32]);
    //�� �������
    Pan_ShZatv_On       -> Font -> Color = SetPopeColor(shr[10]);
    //�� �������
    Pan_ShZatv_Off      -> Font -> Color = SetPopeColor(shr[11]);
    //����������� � HOME
    Pan_Man_Home        -> Font -> Color = SetPopeColor(shr[12]);
    //����������� � ������
    Pan_Man_Kam         -> Font -> Color = SetPopeColor(shr[13]);
    //������ �/� � HOME
    Pan_Pod_Home        -> Font -> Color = SetPopeColor(shr[14]);
    //������ �/� � ��� �����
    Pan_Pod_Nap         -> Font -> Color = SetPopeColor((shr[18])&&(!PRR_POD));
    //������ �/� � ��� �����
    Pan_Pod_Izm         -> Font -> Color = SetPopeColor((shr[18])&&(PRR_POD));
    //����� ������ ���� �����
    Pan_Izm_Perv        -> Font -> Color = SetPopeColor(shr[40]);
    //����� ������ � ���� �����
    Pan_Izm_Sled        -> Font -> Color = SetPopeColor(shr[41]);
    //������� ����� ���
    Pan_Gas_On          -> Font -> Color = SetPopeColor(shr[43]);
    //����������� ����� ����
    Pan_Gas_Off         -> Font -> Color = SetPopeColor(shr[44]);
    //�������� ���� ���
    Pan_Otk_On          -> Font -> Color = SetPopeColor(shr[45]);
    //�������� ���� ����
    Pan_Otk_Off         -> Font -> Color = SetPopeColor(shr[46]);
    //������������ ���� ���
    Pan_Tr_On           -> Font -> Color = SetPopeColor((shr[9])&&(!PR_TRTEST));
    //������������ ���� ����
    Pan_Tr_Off          -> Font -> Color = SetPopeColor((shr[9])&&(PR_TRTEST));
    //��� ������ ���
    Pan_TMN_On          -> Font -> Color = SetPopeColor(shr[49]);
    //��� ������ ����
    Pan_TMN_Off         -> Font -> Color = SetPopeColor(shr[50]);

    //������ �/� � HOME
    Pnl_PodH            -> Font -> Color = SetPopeColor(shr[14]);
    //������ �/� �����
    Pnl_PodS            -> Font -> Color = SetPopeColor(shr[15]);
    //�������� �/� � HOME
    Pnl_VrH             -> Font -> Color = SetPopeColor(shr[16]);
    //�������� �/� �����
    Pnl_VrS             -> Font -> Color = SetPopeColor(shr[17]);
    //������� �������� � HOME
    Pnl_PovH            -> Font -> Color = SetPopeColor(shr[35]);
    //������� �������� �����
    Pnl_PovS            -> Font -> Color = SetPopeColor(shr[36]);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--������������ ����������� ����--//
void TForm1::VisualVoda()
{


 //���� ���������� 1
if(zin[0]&0x01)
PnlKan01->Color=0x00EAD999;
else
PnlKan01->Color=0x003030FF;

 //���� ���������� 2
if(zin[0]&0x02)
PnlKan02->Color=0x00EAD999;
else
PnlKan02->Color=0x003030FF;

//���� ���������� 3
if(zin[0]&0x04)
PnlKan03->Color=0x00EAD999;
else
PnlKan03->Color=0x003030FF;

//���� ���������� 4
if(zin[0]&0x08)
PnlKan04->Color=0x00EAD999;
else
PnlKan04->Color=0x003030FF;




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // ����������� ���� ���������� 1
    PnlKan01 -> Caption = FloatToStrF((((float)aik[0]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "�C";
    if(((((float)aik[0]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan01 -> Caption = "0,0�C"; }
    // ����������� ���� ���������� 2
    PnlKan02 -> Caption = FloatToStrF((((float)aik[1]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "�C";
    if(((((float)aik[1]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan02 -> Caption = "0,0�C"; }
    // ����������� ���� ���������� 3
    PnlKan03 -> Caption = FloatToStrF((((float)aik[2]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "�C";
    if(((((float)aik[2]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan03 -> Caption = "0,0�C"; }
    // ����������� ���� ���������� 4
    PnlKan04 -> Caption = FloatToStrF((((float)aik[3]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "�C";
    if(((((float)aik[3]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan04 -> Caption = "0,0�C"; }


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
		SetOut(1,1,0x40); //
	}
    else
	{
		SetOut(0,1,0x40); //
	}

    // ������
    if((pr_yel)||(!shr[1]&&!shr[2]&&!shr[3]&&!shr[4]&&!shr[5]&&!shr[6]&&!shr[7]&&!shr[9]))
	{
		SetOut(1,1,0x20); //
	}
    else
	{
		SetOut(0,1,0x20); //
	}

    // �������
    if(shr[1]||shr[2]||shr[3]||shr[4]||shr[5]||shr[6]||shr[7]||shr[9])
	{
		SetOut(1,1,0x10); //
	}
    else
	{
		SetOut(0,1,0x10); //

	}

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[20]) { rrg1 -> Visible = true;   }  // ���1
  else        { rrg1 -> Visible = false;  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[21]) { rrg2 -> Visible = true;   }  // ���2
  else        { rrg2 -> Visible = false;  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[22]) { rrg3 -> Visible = true;   }  // ���3
  else        { rrg3 -> Visible = false;  }

//------�������---------------------------------------------------------------//
//��1
    if(out[1]&0x100)
        kl1-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        kl1-> Picture->Bitmap = 0;
//��2
    if(out[1]&0x200)
        kl2-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        kl2-> Picture->Bitmap = 0;
//��3
    if(out[1]&0x1000)
        kl3-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
    else
        kl3-> Picture->Bitmap = 0;
//��4
    if(out[1]&0x2000)
        kl4-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        kl4-> Picture->Bitmap = 0;
//��5
    if(out[1]&0x800)
        kl5-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
    else
        kl5-> Picture->Bitmap = 0;
//��6
    if(out[2]&0x04)
        kl6-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        kl6-> Picture->Bitmap = 0;
//��-�� ����
    if(out[0]&0x200)
        fk_shl_m-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
    else
        fk_shl_m-> Picture->Bitmap = 0;
//��-���
    switch(zin[0]&0x300)
    {
        case 0x000:{fk_kam-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap;   }break;
        case 0x100:{fk_kam-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;   }break;
        case 0x200:{fk_kam-> Picture->Bitmap = 0;                               }break;
        case 0x300:{fk_kam-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap;   }break;
    }
//��-���
    switch(zin[0]&0xC00)
    {
        case 0x000:{fk_tmn-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
        case 0x400:{fk_tmn-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; }break;
        case 0x800:{fk_tmn-> Picture->Bitmap = 0;                        }break;
        case 0xC00:{fk_tmn-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
    }
//��-��
    switch(zin[0]&0xC000)
    {
        case 0x0000:{fk_shl-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
        case 0x4000:{fk_shl-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; }break;
        case 0x8000:{fk_shl-> Picture->Bitmap = 0;                        }break;
        case 0xC000:{fk_shl-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
    }
//��-��
    switch(zin[1]&0x300)
    {
        case 0x000:{vk_shl-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
        case 0x100:{vk_shl-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; }break;
        case 0x200:{vk_shl-> Picture->Bitmap = 0;                        }break;
        case 0x300:{vk_shl-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
    }
//��-��
    switch(zin[2]&0xC0)
    {
        case 0x00:{fk_op-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
        case 0x40:{fk_op-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; }break;
        case 0x80:{fk_op-> Picture->Bitmap = 0;                        }break;
        case 0xC0:{fk_op-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
    }
//��-���1
    if(out[2]&0x02)
        kl_nap1-> Picture->Bitmap =e_klg_o->Picture->Bitmap;
    else
        kl_nap1-> Picture->Bitmap =0;
//��-���2
    if(out[2]&0x01)
        kl_nap2-> Picture->Bitmap =e_klg_o_vert->Picture->Bitmap;
    else
        kl_nap2-> Picture->Bitmap =0;
//---���������----------------------------------------------------------------//
    //����� �����
    if(zin[1]&0x8000)
        door->Visible=true;
    else
        door->Visible=false;
    //������� ������
    switch(zin[1]&0xC00)
    {
        case 0x000:{ShZatv-> Picture->Bitmap =ShZatvNo-> Picture->Bitmap;  break; }
        case 0x400:{ShZatv-> Picture->Bitmap =0;                           break; }
        case 0x800:{ShZatv-> Picture->Bitmap =ShZatvCl-> Picture->Bitmap;  break; }
        case 0xC00:{ShZatv-> Picture->Bitmap =ShZatvNo-> Picture->Bitmap;  break; }
    }
    //�����������
    if(zin[3]&0x200)
        Man-> Picture->Bitmap =man_home-> Picture->Bitmap;
    else
        Man-> Picture->Bitmap =man_norm-> Picture->Bitmap;
    switch (POL_PER)
    {
        case 0:
        {
            Man->Width=80;
            str_right->Visible=0;
            str_left->Visible=0;
            break;
        }
     /*   case 1:
        {
            str_right->Visible=anim_fase;
            str_left->Visible=0;
            Man->Width=190;
            break;
        }
        case 2:
        {
            str_left->Visible=anim_fase;
            str_right->Visible=0;
            Man->Width=190;
            break;
        }    */
        case 3:
        {
            Man->Width=336;
            str_right->Visible=0;
            str_left->Visible=0;
            break;
        }
    }
    //������ �/�
    if(shr[31])
        p_pd-> Picture->Bitmap =p_pd_home-> Picture->Bitmap;
    else
        p_pd-> Picture->Bitmap =p_pd_N-> Picture->Bitmap;
    //������
    if(shr[29])
    {
        lamp1->Visible=true;
        lamp2->Visible=true;
    }
    else
    {
        lamp1->Visible=false;
        lamp2->Visible=false;
    }
    //������ �/�
    if(zin[3]&0x1000)
        Vchg-> Picture->Bitmap =Vchg_on-> Picture->Bitmap;
    else
        Vchg-> Picture->Bitmap =Vchg_off-> Picture->Bitmap;
    //��� ��������
    if(shr[33]&&VRBMH1)
        anim->Visible=(bool)anim_fase;
    else
        anim->Visible=false;
    //��� ��������
    if(shr[28])
        anim_vchg->Visible=(bool)!anim_fase;
    else
        anim_vchg->Visible=false;
    //�������� �/�
    if(PR_VR)
        Vrash->Visible=(bool)anim_fase;
    else
        Vrash->Visible=false;
    //�������� �/�  home
    if(zin[3]&0x02)
        Home_Vr_pd->Visible=true;
    else
        Home_Vr_pd->Visible=false;
    //�������� ����
    if(zin[2]&0x200)
        Vrash_Magn->Visible=(bool)anim_fase;
    else
        Vrash_Magn->Visible=false;


    //���
    if(shr[33])
        Bpm->Visible=true;
    else
        Bpm->Visible=false;
    //�� ���
    if(shr[27]) Dz_kam-> Picture->Bitmap= dross_v-> Picture->Bitmap;
    else{
    switch(zin[0]&0x3000)
    {
        case 0x0000:{Dz_kam-> Picture->Bitmap= zasl_grey_v-> Picture->Bitmap;break;}
        case 0x1000:{Dz_kam-> Picture->Bitmap= zasl_white_v-> Picture->Bitmap; break;}
        case 0x2000:{Dz_kam-> Picture->Bitmap= 0; break;}
        case 0x3000:{Dz_kam-> Picture->Bitmap= zasl_grey_v-> Picture->Bitmap; break;}
    }    }
    //�� ����
    if(shr[39]) Dz_m-> Picture->Bitmap= dross-> Picture->Bitmap;
    else{
    switch(zin[0]&0x30)
    {
        case 0x00:{Dz_m-> Picture->Bitmap= zasl_grey-> Picture->Bitmap;break;}
        case 0x10:{Dz_m-> Picture->Bitmap= zasl_white-> Picture->Bitmap; break;}
        case 0x20:{Dz_m-> Picture->Bitmap= 0; break;}
        case 0x30:{Dz_m-> Picture->Bitmap= zasl_grey-> Picture->Bitmap; break;}
    }    }
    //��� ���
    if(!(zin[1]&0x10))
    {
        tmn-> Picture->Bitmap =tmn_red-> Picture->Bitmap;
        str_up->Visible=false;
        str_down->Visible=false;
    } //������
    else if(!(zin[1]&0x08))//��������������
    {
        tmn-> Picture->Bitmap =tmn_yellow-> Picture->Bitmap;//����
        str_down->Visible=false;
        str_up->Visible=false;
    }
    else if((zin[1]&0x2020)==0x2020)//������
    {
        str_down->Visible=false;
        str_up->Visible=true;
        if(anim_fase)tmn-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
        else         tmn-> Picture->Bitmap =0;
    }
    else if((zin[1]&0x1000))//����������
    {
        str_down->Visible=true;
        str_up->Visible=false;
        if(anim_fase)tmn-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
        else         tmn-> Picture->Bitmap =0;
    }
    else if((zin[1]&0x24)==0x24)//�����
    {
        str_down->Visible=false;
        str_up->Visible=false;
        tmn-> Picture->Bitmap =tmn_white-> Picture->Bitmap;

    }

    else
    {
        tmn-> Picture->Bitmap =0;//����
        str_down->Visible=false;
        str_up->Visible=false;
    }
    //��� ��
    if(zin[4]&0x80)//����� �� �����
        Tmn_otk-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
    else if(out[3]&0x80)//�������
    {
        if(anim_fase)Tmn_otk-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
        else         Tmn_otk-> Picture->Bitmap =0;
    }
    else
        Tmn_otk-> Picture->Bitmap =0;
    //��� ���
    if(zin[1]&0x02)
        fvn_kam-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
    else
        fvn_kam-> Picture->Bitmap =0;
    //��� ��
    if(zin[1]&0x01)
        Fvn_Shl-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
    else
        Fvn_Shl-> Picture->Bitmap =0;
    //��� ��
    if(out[3]&0x40)
        fvn_otk-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
    else
        fvn_otk-> Picture->Bitmap =0;
    //����
    if(zin[4]&0x01)
        mass-> Visible =true;
    else
        mass-> Visible =false;
    //�����**********************************************************************
    //1
    if(
    ((tube_2->Visible==true)&&((out[0]&0x200)||(zin[0]&0x4000)))   ||
    ((tube_16->Visible==true)&&(zin[1]&0x100))  ||
    ((out[2]&0x02))
    )
        tube_1->Visible=true;
    else
        tube_1->Visible=false;
    //2
    if(
        ((tube_1->Visible==true)&&((out[0]&0x200)||(zin[0]&0x4000)))||
        (zin[1]&0x01)
        )
        tube_2->Visible=true;
    else
        tube_2->Visible=false;
    //3
    if(out[2]&0x01)
        tube_3->Visible=true;
    else
        tube_3->Visible=false;
    //4
    if(((tube_5->Visible==true)&&(zin[2]&0x40))||(zin[0]&0x10))
        tube_4->Visible=true;
    else
        tube_4->Visible=false;
    //5
    if(out[3]&0xC0)
        tube_5->Visible=true;
    else
        tube_5->Visible=false;
    //6
    if(out[3]&0x40)
        tube_6->Visible=true;
    else
        tube_6->Visible=false;
    //7
    if(
       ((out[1]&0x1000)&&(tube_8->Visible==true))
    )
        tube_7->Visible=true;
    else
        tube_7->Visible=false;
    //7_1
    if(
       ((out[1]&0x1000)&&(tube_8->Visible==true))
    )
        {
            tube_7_open->Visible=true;
            tube_7_close->Visible=false;

        }
    else
        {
            tube_7_open->Visible=false;
            tube_7_close->Visible=true;
            
        }


    //8
    if(
        ((out[1]&0x2000)&&(tube_9->Visible==true))||
        (shr[22])
       )
        tube_8->Visible=true;
    else
        tube_8->Visible=false;
    //9
    if(
        ((tube_8->Visible==true)&&(out[1]&0x2000))||
        (zin[1]&0x02)
    )
        tube_9->Visible=true;
    else
        tube_9->Visible=false;
    //10
    if(
        ((tube_11->Visible==true)&&(out[1]&0x100))||
        ((tube_12->Visible==true)&&(out[1]&0x200))
        )
        tube_10->Visible=true;
    else
        tube_10->Visible=false;
    //11
    if(
        (shr[20])||
        ((tube_10->Visible==true)&&(out[1]&0x100))
        )
        tube_11->Visible=true;
    else
        tube_11->Visible=false;
    //12
    if(
        (shr[21])||
        ((tube_10->Visible==true)&&(out[1]&0x200))
        )
        tube_12->Visible=true;
    else
        tube_12->Visible=false;
    //13
    if((out[1]&0x800)&&(tube_10->Visible==true))
        tube_13->Visible=true;
    else
        tube_13->Visible=false;
    //14
    if((tube_9->Visible==true)&&(zin[0]&0x100))
        tube_14->Visible=true;
    else
        tube_14->Visible=false;
    //15
    if((out[2]&0x04)&&(tube_9->Visible==true))
        tube_15->Visible=true;
    else
        tube_15->Visible=false;
    //16
    if
        (
            ((tube_1->Visible==true)&&(zin[1]&0x100))||
            ((tube_9->Visible==true)&&(zin[0]&0x400))||
            (zin[0]&0x1000)
        )
    {
        tube_16->Visible=true;
        tube_17->Visible=true;
    }
    else
    {
        tube_16->Visible=false;
        tube_17->Visible=false;
    }

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
   if((shr[40]||shr[41])&&(PCMain -> ActivePage == TSNalad))
   {
        DrawPaint->Visible=true;
        float a,b;
        a= TEK_ABS_VR*360/Max_Vr;
        b= TEK_ABS_POV*360/Max_Pov;
        if(b>=120)
            drawpaint(a+180,150);
        else if(b<=0)
            drawpaint(a+180,240);
        else
            drawpaint(a+180,240-b);
   }
   else
        DrawPaint->Visible=false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--������������ ���������� ��������--//
//---------------------------------------------------------------------------
void TForm1::VisualParA()
{
//N_ST=1**********************************************************************************************************

    EdtAKon1_12 -> Text =FloatToStrF((float)par[1][12], ffFixed, 5, 0);                     //����� ��������
//N_ST=2**********************************************************************************************************

    EdtAKon2_0  -> Text =FloatToStrF((float)par[2][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);      //������ ���1
    EdtAKon2_4  -> Text =FloatToStrF(((float)par[2][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);                     //��������
    EdtAKon2_8  -> Text =FloatToStrF((float)par[2][8]/ 4095.0* CESAR_MAX_PD, ffFixed, 5, 0);//�������� ���
    EdtAKon2_12 -> Text =FloatToStrF((float)par[2][12], ffFixed, 5, 0);                     //����� ��������
//N_ST=3**********************************************************************************************************
    EdtAKon3_0  -> Text =FloatToStrF((float)par[3][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);      //������ ���1
    EdtAKon3_4  -> Text =FloatToStrF(((float)par[3][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);          //�������� ??
    EdtAKon3_6  -> Text =FloatToStrF((float)par[3][6]/4095.0*6000 , ffFixed, 5, 0);         //�������� �  ???
    EdtAKon3_12 -> Text =FloatToStrF((float)par[3][12], ffFixed, 5, 0);                     //����� ��������
//N_ST=4**********************************************************************************************************
    EdtAKon4_0  -> Text =FloatToStrF((float)par[4][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);      //������ ���1
    EdtAKon4_1  -> Text =FloatToStrF((float)par[4][1]/4095.0*RRG2_MAX, ffFixed, 5, 2);      //������ ���2
    EdtAKon4_15  -> Text =FloatToStrF((float)par[4][15], ffFixed, 5, 0);                    //����������� Ar
    EdtAKon4_16  -> Text =FloatToStrF((float)par[4][16], ffFixed, 5, 0);                    //����������� O2
    EdtAKon4_4  -> Text =FloatToStrF(((float)par[4][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);          //��������
    EdtAKon4_7  -> Text =FloatToStrF((float)par[4][7]/4095.0*600.0, ffFixed, 5, 0);                      //���������� ���
    EdtAKon4_6  -> Text =FloatToStrF((float)par[4][6]/4095.0*6000 , ffFixed, 5, 0);         //�������� �
    EdtAKon4_8  -> Text =FloatToStrF((float)par[2][8]/ 4095.0* CESAR_MAX_PD, ffFixed, 5, 0);//�������� ���
    EdtAKon4_17  -> Text =FloatToStrF((float)par[4][17]/10000.0*100.0 , ffFixed, 5, 1);     //������� �������� ��
    EdtAKon4_12 -> Text =FloatToStrF((float)par[4][12] , ffFixed, 5, 0);                    //����� ��������
//N_ST=5*************************************************************************************************************
    EdtAKon5_0  -> Text =FloatToStrF((float)par[5][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);      //������ ���1
    EdtAKon5_4  -> Text =FloatToStrF(((float)par[5][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);          //��������
    EdtAKon5_6  -> Text =FloatToStrF((float)par[5][6]/4095.0*6000 , ffFixed, 5, 0);         //�������� �
    EdtAKon5_12 -> Text =FloatToStrF((float)par[5][12], ffFixed, 5, 0);                     //����� ��������
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
    //N_ST=1 ������------------------------------------------------------------------------------
    par[1][12]= StrToInt    ( EdtARed1_12->Text );                          //����� ��������
    //N_ST=2 ��-�������--------------------------------------------------------------------------
    par[2][0]=  StrToFloat  ( EdtARed2_0->Text ) / RRG1_MAX * 4095.0;       //���1
    par[2][4]=  StrToFloat    ( EdtARed2_4->Text )*8000/DAVL_MAX + 1000;          //��������
    par[2][8]=  StrToFloat  ( EdtARed2_8->Text ) * 4095.0 / CESAR_MAX_PD;   //���
    par[2][12]= StrToInt    ( EdtARed2_12->Text );                          //����� ��������
    //N_ST=3 �����-------------------------------------------------------------------------------
    par[3][0]=  StrToFloat  ( EdtARed3_0->Text ) / RRG1_MAX * 4095.0;       //���1
    par[3][4]=  StrToFloat    ( EdtARed3_4->Text )*8000/DAVL_MAX + 1000;             //��������
    par[3][6]=  StrToFloat  ( EdtARed3_6-> Text) * 4095.0 / 6000;           //�������� �
    par[3][7]=  100.0/600.0*4095;             //��������� ��������� ����
    par[3][12]= StrToInt    ( EdtARed3_12->Text );                          //����� ��������
    //N_ST=4 ���������---------------------------------------------------------------------------
    par[4][0]=  StrToFloat  ( EdtARed4_0->Text ) / RRG1_MAX * 4095.0;       //���1
    par[4][1]=  StrToFloat  ( EdtARed4_1->Text ) / RRG2_MAX * 4095.0;       //���2
    par[4][15]= StrToInt    ( EdtARed4_15->Text );                          //����������� Ar
    par[4][16]= StrToInt    ( EdtARed4_16->Text );                          //����������� O2
    par[4][4]=  StrToFloat    ( EdtARed4_4->Text )*8000/DAVL_MAX + 1000;           //��������
    par[4][7]=  StrToFloat    ( EdtARed4_7->Text )/600.0*4095;             //��������� ��������� ����
    par[4][6]=  StrToFloat  ( EdtARed4_6-> Text) * 4095.0 / 6000;           //�������� �
    par[4][8]=  StrToFloat  ( EdtARed4_8->Text ) * 4095.0 / CESAR_MAX_PD;   //���
    par[4][17]=  StrToFloat  ( EdtARed4_17->Text ) *10000.0/100.0;          //���� �������� ��
    par[4][12]= StrToInt    ( EdtARed4_12->Text );                          //����� ��������




    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ��������� �������������� ������:");

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //1 ������
    if ( EdtAKon1_12 -> Text != EdtARed1_12 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon1_12 -> Text + " -> " + EdtARed1_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("��-�������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //2 ������
    if ( EdtAKon2_0 -> Text != EdtARed2_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon2_0 -> Text + " -> " + EdtARed2_0 -> Text );
    if ( EdtAKon2_4 -> Text != EdtARed2_4 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon2_4 -> Text + " -> " + EdtARed2_4 -> Text );
    if ( EdtAKon2_8 -> Text != EdtARed2_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon2_8 -> Text + " -> " + EdtARed2_8 -> Text );
    if ( EdtAKon2_12 -> Text != EdtARed2_12 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon2_12 -> Text + " -> " + EdtARed2_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("�����:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //3 ������
    if ( EdtAKon3_0 -> Text != EdtARed3_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon3_0 -> Text + " -> " + EdtARed3_0 -> Text );
    if ( EdtAKon3_4 -> Text != EdtARed3_4 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon3_4 -> Text + " -> " + EdtARed3_4 -> Text );
    if ( EdtAKon3_6 -> Text != EdtARed3_6 -> Text )
        MemoStat -> Lines -> Add("�������� �: " + EdtAKon3_6 -> Text + " -> " + EdtARed3_6 -> Text );
    if ( EdtAKon3_12 -> Text != EdtARed3_12 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon3_12 -> Text + " -> " + EdtARed3_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("���������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //4 ������
    if ( EdtAKon4_0 -> Text != EdtARed4_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon4_0 -> Text + " -> " + EdtARed4_0 -> Text );
    if ( EdtAKon4_1 -> Text != EdtARed4_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtAKon4_1 -> Text + " -> " + EdtARed4_1 -> Text );
    if ( EdtAKon4_15 -> Text != EdtARed4_15 -> Text )
        MemoStat -> Lines -> Add("����������� Ar: " + EdtAKon4_15 -> Text + " -> " + EdtARed4_15 -> Text );
    if ( EdtAKon4_16 -> Text != EdtARed4_16 -> Text )
        MemoStat -> Lines -> Add("����������� �2: " + EdtAKon4_16 -> Text + " -> " + EdtARed4_16 -> Text );
    if ( EdtAKon4_4 -> Text != EdtARed4_4 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon4_4 -> Text + " -> " + EdtARed4_4 -> Text );
    if ( EdtAKon4_7 -> Text != EdtARed4_7 -> Text )
        MemoStat -> Lines -> Add("���������� ��� �: " + EdtAKon4_7 -> Text + " -> " + EdtARed4_7 -> Text );
    if ( EdtAKon4_6 -> Text != EdtARed4_6 -> Text )
        MemoStat -> Lines -> Add("�������� �: " + EdtAKon4_6 -> Text + " -> " + EdtARed4_6 -> Text );
    if ( EdtAKon4_8 -> Text != EdtARed4_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtAKon4_8 -> Text + " -> " + EdtARed4_8 -> Text );
    if ( EdtAKon4_17 -> Text != EdtARed4_17 -> Text )
        MemoStat -> Lines -> Add("���� �������� �� ����-������������: " + EdtAKon4_17 -> Text + " -> " + EdtARed4_17 -> Text );
    if ( EdtAKon4_12 -> Text != EdtARed4_12 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon4_12 -> Text + " -> " + EdtARed4_12 -> Text );



    // ����������� ���������� ���������
    EdtARed1_12 -> Color = clWhite;

    EdtARed2_0 -> Color = clWhite;
    EdtARed2_4 -> Color = clWhite;
    EdtARed2_8 -> Color = clWhite;
    EdtARed2_12 -> Color = clWhite;

    EdtARed3_0 -> Color = clWhite;
    EdtARed3_4 -> Color = clWhite;
    EdtARed3_6 -> Color = clWhite;
    EdtARed3_12 -> Color = clWhite;

    EdtARed4_0 -> Color = clWhite;
    EdtARed4_1 -> Color = clWhite;
    EdtARed4_15 -> Color = clWhite;
    EdtARed4_16 -> Color = clWhite;
    EdtARed4_4 -> Color = clWhite;
    EdtARed4_7 -> Color = clWhite;
    EdtARed4_6 -> Color = clWhite;
    EdtARed4_8 -> Color = clWhite;
    EdtARed4_17 -> Color = clWhite;
    EdtARed4_12 -> Color = clWhite;


    // �������� ��������
    VisualParA();
   
 /* int i=0,j=0,k=0,n=0,n0=0,n1=0,n2=0,n3=0,iIzmeneniya=0,ii=0;
    // ����������������� ���������� ���1 - ���3 � ������ par_V[][]
    par_V[1][0]=par[1][0]; par_V[1][1]=par[1][1]; par_V[1][2]=par[1][2];
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(k=0;k<3;k++)
    { i=1; j=7; n=1;

      do
      { if(par[j][k])
        { par_V[j][k]=par[j][k]; i++; }
        else
        { if(i!=1)
          { if(j==7) { par_V[7][k]=par_V[2][k];   }
            else     { par_V[j][k]=par_V[j+1][k]; }
            i++;
          }
          else
          { n++; }
        }
        j--;
        if(j<2) { j=7; }
      }
      while((i<7)&&(n<7));

      if(n==7)
      {
        for(i=1;i<=7;i++)
            par_V[i][k] = 0;
      }

      if(!par[1][k]) { par_V[1][k]=par_V[2][k]; }
      else           { par_V[1][k]=par[1][k];   }
    }
    */
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
 // ���������� ��������� ��������� �������� �/�
    if  (
            (((TEdit*)Sender)->Name == "EdtTRed1" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < -2000000)
        {
            valueText = -2000000;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 2000000)
        {
            valueText = 2000000;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

// ������ ���1
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_0" )||
            (((TEdit*)Sender)->Name == "EdtARed3_0" )||
            (((TEdit*)Sender)->Name == "EdtARed4_0" )||
            (((TEdit*)Sender)->Name == "EdtARed5_0" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_0" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 2;
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
            (((TEdit*)Sender)->Name == "EdtARed4_1" )||
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
            (((TEdit*)Sender)->Name == "EdtRRed0_2" )||
            (((TEdit*)Sender)->Name == "EditNastrTo2" )
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
// ����� ��������    1
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_12" )||
            (((TEdit*)Sender)->Name == "EdtARed3_12" )||
            (((TEdit*)Sender)->Name == "EdtARed4_12" )||
            (((TEdit*)Sender)->Name == "EdtARed5_12" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 3600)
        {
            valueText = 3600;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ����� ��������   2
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_12" )
        )
    {
        // ���-�� ������ ����� ������� 1
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
// �������� 1
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_4" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.5)
        {
            valueText = 0.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 4.0)
        {
            valueText = 4.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// �������� 2
    if  (

            (((TEdit*)Sender)->Name == "EdtARed3_4" )||
            (((TEdit*)Sender)->Name == "EdtARed4_4" )||

            (((TEdit*)Sender)->Name == "EdtARed5_4" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.3)
        {
            valueText = 0.3;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 1.5)
        {
            valueText = 1.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// �������� 3
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_4" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.3)
        {
            valueText = 0.3;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 4.0)
        {
            valueText = 4.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// �������� �
    if  (
            (((TEdit*)Sender)->Name == "EdtARed3_6" )||
            (((TEdit*)Sender)->Name == "EdtARed5_6" )||
            (((TEdit*)Sender)->Name == "EdtARed4_6" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_6" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 6000)
        {
            valueText = 6000;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// �������� ��� �/�
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_8" )||
            (((TEdit*)Sender)->Name == "EdtARed4_8" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_8" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 300)
        {
            valueText = 300;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ����������� �����
    if  (
            (((TEdit*)Sender)->Name == "EdtARed4_15" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_15" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 1)
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 1000)
        {
            valueText = 1000;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ����������� �����
    if  (

            (((TEdit*)Sender)->Name == "EdtARed4_16" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_16" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 1)
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ��������� ��������� ����
    if  (
            (((TEdit*)Sender)->Name == "EdtARed4_7" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_7" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if ((valueText>100)&&(valueText<300))
        {
            valueText = 300;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 600)
        {
            valueText = 600;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ���� �������� ��
    if  (
            (((TEdit*)Sender)->Name == "EdtARed4_17" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_17" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 90)
        {
            valueText = 90;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ���������� �� ������
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_13" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 75)
        {
            valueText = 75;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ���� �������� �/�
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_14" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 360)
        {
            valueText = 360;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ���� ������������
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_10" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_18" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_19" )
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
//���������
// ����������� ������� ��������������� ������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo5" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText <= 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if ((valueText<=30)&&(valueText>0))
        {
            valueText = 30;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 150)
        {
            valueText = 150;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ����������� ������� �/�
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo6" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText <= 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if ((valueText<=100)&&(valueText>0))
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 300)
        {
            valueText = 300;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ������������ ���������� �� ����������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo7" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 750)
        {
            valueText = 750;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ������������ �������� ���������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo8" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 2)
        {
            valueText = 2;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 4098)
        {
            valueText = 4098;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ������������ ����� ����� ���������
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo9" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText <= 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if ((valueText<=0.3)&&(valueText>0))
        {
            valueText = 0.3;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 1023)
        {
            valueText = 1023;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// ���������� ����������� ������������ ��� �/�
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo10" )
        )
    {
        // ���-�� ������ ����� ������� 1
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
// ���� ������� �/�
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo11" )||
            (((TEdit*)Sender)->Name == "EditNastrTo12" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }

    }
//  ������� ���������� ���������� ���
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo16" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 60)
        {
            valueText = 60;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 383)
        {
            valueText = 383;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//  ������� ���� ���������� ���
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo17" )
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText < 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 640)
        {
            valueText = 640;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

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
    par[0][0] =StrToFloat( EdtRRed0_0 -> Text )/ RRG1_MAX * 4095.0 ;
    //������ ���2
    par[0][1] =StrToFloat( EdtRRed0_1 -> Text )/ RRG2_MAX * 4095.0 ;
    //������ ���3
    par[0][2] =StrToFloat( EdtRRed0_2 -> Text )/ RRG3_MAX * 4095.0 ;
    //��������
    par[0][4] =StrToFloat    ( EdtRRed0_4->Text )*8000/DAVL_MAX + 1000;
    //�������� �
    par[0][6] =StrToFloat  ( EdtRRed0_6-> Text) * 4095.0 / 6000;
    //��������� ��������� ����
    par[0][7] =StrToFloat  ( EdtRRed0_7-> Text) * 4095.0/600.0;
    //�������� ��� �/�
    par[0][8] =StrToFloat  ( EdtRRed0_8->Text ) * 4095.0 / CESAR_MAX_PD;
    //����������� Ar
    par[0][15]=StrToFloat  ( EdtRRed0_15-> Text);
    //����������� �2
    par[0][16]=StrToFloat  ( EdtRRed0_16-> Text);
    //���� �������� �� ����-������������
    par[0][17]=StrToFloat  ( EdtRRed0_17->Text ) *10000.0/100.0;
    //������ �����������������
    par[0][10]=StrToFloat  ( EdtRRed0_10-> Text);
    //�������� �����������������
    par[0][18]=StrToFloat  ( EdtRRed0_18-> Text);
    //������� ��������
    par[0][19]=StrToFloat  ( EdtRRed0_19-> Text);
    //�������� �������
    if(EdtRRed0_11 -> ItemIndex == 0)       { par[0][11]=0; }
    else if(EdtRRed0_11 -> ItemIndex == 1)  { par[0][11]=1; }
    else if(EdtRRed0_11 -> ItemIndex == 2)  { par[0][11]=2; }
    //���������� �� ������
    par[0][13]=StrToFloat  ( EdtRRed0_13-> Text);
    //���� �������� �/�
    par[0][14]=StrToFloat  ( EdtRRed0_14-> Text)* Max_Vr / 360;;






    MemoStat -> Lines -> Add("");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ���������� ���������:");
    MemoStat -> Lines -> Add("");

    if ( EdtRKon0_0 -> Text != EdtRRed0_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtRKon0_0 -> Text + " -> " + EdtRRed0_0 -> Text );
    if ( EdtRKon0_1 -> Text != EdtRRed0_1 -> Text )
        MemoStat -> Lines -> Add("������ ���2: " + EdtRKon0_1 -> Text + " -> " + EdtRRed0_1 -> Text );
    if ( EdtRKon0_2 -> Text != EdtRRed0_2 -> Text )
        MemoStat -> Lines -> Add("������ ���3: " + EdtRKon0_2 -> Text + " -> " + EdtRRed0_2 -> Text );
    if ( EdtRKon0_4 -> Text != EdtRRed0_4 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtRKon0_4 -> Text + " -> " + EdtRRed0_4 -> Text );
    if ( EdtRKon0_6 -> Text != EdtRRed0_6 -> Text )
        MemoStat -> Lines -> Add("�������� �: " + EdtRKon0_6 -> Text + " -> " + EdtRRed0_6 -> Text );
    if ( EdtRKon0_7 -> Text != EdtRRed0_7 -> Text )
        MemoStat -> Lines -> Add("���������� ��� �: " + EdtRKon0_7 -> Text + " -> " + EdtRRed0_7 -> Text );
    if ( EdtRKon0_8 -> Text != EdtRRed0_8 -> Text )
        MemoStat -> Lines -> Add("�������� ��� �/�: " + EdtRKon0_8 -> Text + " -> " + EdtRRed0_8 -> Text );
    if ( EdtRKon0_15 -> Text != EdtRRed0_15 -> Text )
        MemoStat -> Lines -> Add("����������� Ar: " + EdtRKon0_15 -> Text + " -> " + EdtRRed0_15 -> Text );
    if ( EdtRKon0_16 -> Text != EdtRRed0_16 -> Text )
        MemoStat -> Lines -> Add("����������� O2: " + EdtRKon0_16 -> Text + " -> " + EdtRRed0_16 -> Text );
    if ( EdtRKon0_17 -> Text != EdtRRed0_17 -> Text )
        MemoStat -> Lines -> Add("���� �������� �� ����-������������: " + EdtRKon0_17 -> Text + " -> " + EdtRRed0_17 -> Text );
    if ( EdtRKon0_10 -> Text != EdtRRed0_10 -> Text )
        MemoStat -> Lines -> Add("������ ������������������: " + EdtRKon0_10 -> Text + " -> " + EdtRRed0_10 -> Text );
    if ( EdtRKon0_18 -> Text != EdtRRed0_18 -> Text )
        MemoStat -> Lines -> Add("�������� ������������������: " + EdtRKon0_18 -> Text + " -> " + EdtRRed0_18 -> Text );
    if ( EdtRKon0_19 -> Text != EdtRRed0_19 -> Text )
        MemoStat -> Lines -> Add("������� ��������: " + EdtRKon0_19 -> Text + " -> " + EdtRRed0_19 -> Text );
    if ( EdtRKon0_11 -> Text != EdtRRed0_11 -> Text )
        MemoStat -> Lines -> Add("�������� �������: " + EdtRKon0_11 -> Text + " -> " + EdtRRed0_11 -> Text );
    if ( EdtRKon0_13 -> Text != EdtRRed0_13 -> Text )
        MemoStat -> Lines -> Add("���������� �� ������: " + EdtRKon0_13 -> Text + " -> " + EdtRRed0_13 -> Text );
    if ( EdtRKon0_14 -> Text != EdtRRed0_14 -> Text )
        MemoStat -> Lines -> Add("���� �������� �/�: " + EdtRKon0_14 -> Text + " -> " + EdtRRed0_14 -> Text );


    // ����������� ���������� ���������
    EdtRRed0_0 -> Color = clWhite;
    EdtRRed0_1 -> Color = clWhite;
    EdtRRed0_2 -> Color = clWhite;
    EdtRRed0_4 -> Color = clWhite;
    EdtRRed0_6 -> Color = clWhite;
    EdtRRed0_7 -> Color = clWhite;
    EdtRRed0_8 -> Color = clWhite;
    EdtRRed0_15 -> Color = clWhite;
    EdtRRed0_16 -> Color = clWhite;
    EdtRRed0_17 -> Color = clWhite;
    EdtRRed0_10 -> Color = clWhite;
    EdtRRed0_18 -> Color = clWhite;
    EdtRRed0_19 -> Color = clWhite;
    EdtRRed0_11 -> Color = clWhite;
    EdtRRed0_13 -> Color = clWhite;
    EdtRRed0_14 -> Color = clWhite;

    // �������� ��������
    VisualParR();

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

//N_ST=1

EdtALib1_12 -> Text = MemoLib -> Lines -> operator [](0);
//N_ST=2
EdtALib2_0 -> Text = MemoLib -> Lines -> operator [](1);
EdtALib2_4 -> Text = MemoLib -> Lines -> operator [](2);
EdtALib2_8 -> Text = MemoLib -> Lines -> operator [](3);
EdtALib2_12 -> Text = MemoLib -> Lines -> operator [](4);
//N_ST=3
EdtALib3_0 -> Text = MemoLib -> Lines -> operator [](5);
EdtALib3_4 -> Text = MemoLib -> Lines -> operator [](6);
EdtALib3_6 -> Text = MemoLib -> Lines -> operator [](7);
EdtALib3_12 -> Text = MemoLib -> Lines -> operator [](8);
//N_ST=4
EdtALib4_0 -> Text = MemoLib -> Lines -> operator [](9);
EdtALib4_1 -> Text = MemoLib -> Lines -> operator [](10);
EdtALib4_15 -> Text = MemoLib -> Lines -> operator [](11);
EdtALib4_16 -> Text = MemoLib -> Lines -> operator [](12);
EdtALib4_4 -> Text = MemoLib -> Lines -> operator [](13);
EdtALib4_7 -> Text = MemoLib -> Lines -> operator [](14);
EdtALib4_6 -> Text = MemoLib -> Lines -> operator [](15);
EdtALib4_8 -> Text = MemoLib -> Lines -> operator [](16);
EdtALib4_17 -> Text = MemoLib -> Lines -> operator [](17);
EdtALib4_12 -> Text = MemoLib -> Lines -> operator [](18);








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


// �������� ������ ������������ ����������

EdtALib1_12 -> Text = "";

EdtALib2_0 -> Text = "";
EdtALib2_4 -> Text = "";
EdtALib2_8 -> Text = "";
EdtALib2_12 -> Text = "";

EdtALib3_0 -> Text = "";
EdtALib3_4 -> Text = "";
EdtALib3_6 -> Text = "";
EdtALib3_12 -> Text = "";

EdtALib4_0 -> Text = "";
EdtALib4_1 -> Text = "";
EdtALib4_15 -> Text = "";
EdtALib4_16 -> Text = "";
EdtALib4_4 -> Text = "";
EdtALib4_7 -> Text = "";
EdtALib4_6 -> Text = "";
EdtALib4_8 -> Text = "";
EdtALib4_17 -> Text = "";
EdtALib4_12 -> Text = "";




}

}
//---------------------------------------------------------------------------
//--������ � ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnSaveClick(TObject *Sender)
{

MemoLib -> Lines -> Clear();
//N_ST=1
    MemoLib -> Lines -> Add ( EdtARed1_12 -> Text );
//N_ST=2
    MemoLib -> Lines -> Add ( EdtARed2_0 -> Text );
    MemoLib -> Lines -> Add ( EdtARed2_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed2_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed2_12 -> Text );
//N_ST=3
    MemoLib -> Lines -> Add ( EdtARed3_0 -> Text );
    MemoLib -> Lines -> Add ( EdtARed3_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed3_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed3_12 -> Text );
//N_ST=4
    MemoLib -> Lines -> Add ( EdtARed4_0 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_16 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_17 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_12 -> Text );


// ����������� ����������� ����
GBSaveDialog -> Visible = true;

}
//---------------------------------------------------------------------------
//--������ �� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnLoadClick(TObject *Sender)
{
//N_ST=1
EdtARed1_12 -> Text = EdtALib1_12 -> Text;
EdtARed1_12 -> Color = clSilver;
//N_ST=2
EdtARed2_0 -> Text = EdtALib2_0 -> Text;
EdtARed2_0 -> Color = clSilver;
EdtARed2_4 -> Text = EdtALib2_4 -> Text;
EdtARed2_4 -> Color = clSilver;
EdtARed2_8 -> Text = EdtALib2_8 -> Text;
EdtARed2_8 -> Color = clSilver;
EdtARed2_12 -> Text = EdtALib2_12 -> Text;
EdtARed2_12 -> Color = clSilver;
//N_ST=3
EdtARed3_0 -> Text = EdtALib3_0 -> Text;
EdtARed3_0 -> Color = clSilver;
EdtARed3_4 -> Text = EdtALib3_4 -> Text;
EdtARed3_4 -> Color = clSilver;                                   
EdtARed3_6 -> Text = EdtALib3_6 -> Text;
EdtARed3_6 -> Color = clSilver;
EdtARed3_12 -> Text = EdtALib3_12 -> Text;
EdtARed3_12 -> Color = clSilver;
//N_ST=4
EdtARed4_0 -> Text = EdtALib4_0 -> Text;
EdtARed4_0 -> Color = clSilver;
EdtARed4_1 -> Text = EdtALib4_1 -> Text;
EdtARed4_1 -> Color = clSilver;
EdtARed4_15 -> Text = EdtALib4_15 -> Text;
EdtARed4_15 -> Color = clSilver;
EdtARed4_16 -> Text = EdtALib4_16 -> Text;
EdtARed4_16 -> Color = clSilver;
EdtARed4_4 -> Text = EdtALib4_4 -> Text;
EdtARed4_4 -> Color = clSilver;
EdtARed4_7 -> Text = EdtALib4_7 -> Text;
EdtARed4_7 -> Color = clSilver;
EdtARed4_6 -> Text = EdtALib4_6 -> Text;
EdtARed4_6 -> Color = clSilver;
EdtARed4_8 -> Text = EdtALib4_8 -> Text;
EdtARed4_8 -> Color = clSilver;
EdtARed4_17 -> Text = EdtALib4_17 -> Text;
EdtARed4_17 -> Color = clSilver;
EdtARed4_12 -> Text = EdtALib4_12 -> Text;
EdtARed4_12 -> Color = clSilver;




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
    EdtRKon0_0 -> Text =FloatToStrF((float)par[0][0]* RRG1_MAX / 4095.0, ffFixed, 5, 2);//������ ���1
    EdtRKon0_1 -> Text =FloatToStrF((float)par[0][1]* RRG2_MAX / 4095.0, ffFixed, 5, 2);//������ ���2
    EdtRKon0_2 -> Text =FloatToStrF((float)par[0][2]* RRG3_MAX / 4095.0, ffFixed, 5, 1);//������ ���3
    EdtRKon0_4 -> Text =FloatToStrF(((float)par[0][4]-1000)/8000*DAVL_MAX,ffFixed,5,1);//��������
    EdtRKon0_6 -> Text =FloatToStrF((float)par[0][6]/ 4095.0 * 6000, ffFixed, 5, 0);//�
    EdtRKon0_7-> Text =FloatToStrF((float)par[0][7]/4095.0*600, ffFixed, 5, 0);//��������� ��������� ����
    EdtRKon0_8 -> Text =FloatToStrF((float)par[0][8]/ 4095.0 * CESAR_MAX_PD, ffFixed, 5, 0);//���
    EdtRKon0_15-> Text =FloatToStrF((float)par[0][15], ffFixed, 5, 0);//����������� Ar
    EdtRKon0_16-> Text =FloatToStrF((float)par[0][16], ffFixed, 5, 0);//����������� �2
    EdtRKon0_17-> Text =FloatToStrF((float)par[0][17]/10000.0*100.0, ffFixed, 5, 1);//������� ��
    EdtRKon0_10-> Text =FloatToStrF((float)par[0][10], ffFixed, 5, 0);//������ �����������������
    EdtRKon0_18-> Text =FloatToStrF((float)par[0][18], ffFixed, 5, 0);//�������� �����������������
    EdtRKon0_19-> Text =FloatToStrF((float)par[0][19], ffFixed, 5, 0);//������� ��������
    if(par[0][11]==0)       { EdtRKon0_11 -> Text ="�������"; }
    else if(par[0][11]==1)  { EdtRKon0_11 -> Text ="�����"; }
    else if(par[0][11]==2)  { EdtRKon0_11 -> Text ="��������";}
    EdtRKon0_13-> Text =FloatToStrF((float)par[0][13], ffFixed, 5, 0);//���������� �� ������
    EdtRKon0_14-> Text =FloatToStrF((float)par[0][14]/ Max_Vr * 360, ffFixed, 5, 0);//���� �������� �/�


    




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
        "",
        "sh[1]",
        "shr[1]",
        "sh[2]",
        "shr[2]",
        "sh[3]",
        "shr[3]",
        "sh[4]",
        "shr[4]",
        "sh[5]",
        "shr[5]",
        "sh[6]",
        "shr[6]",
        "sh[7]",
        "shr[7]",
        "sh[8]",
        "shr[8]",
        "sh[9]",
        "shr[9]",
        "sh[10]",
        "shr[10]",
        "sh[11]",
        "shr[11]",
        "sh[12]",
        "shr[12]",
        "sh[13]",
        "shr[13]",
        "sh[14]",
        "shr[14]",


        //1 ��������
        "sh[15]",
        "shr[15]",
        "sh[16]",
        "shr[16]",
        "sh[17]",
        "shr[17]",
        "sh[18]",
        "shr[18]",
        "sh[19]",
        "shr[19]",
        "sh[20]",
        "shr[20]",
        "sh[21]",
        "shr[21]",
        "sh[22]",
        "shr[22]",
        "sh[23]",
        "shr[23]",
        "sh[24]",
        "shr[24]",
        "sh[25]",
        "shr[25]",
        "sh[26]",
        "shr[26]",
        "sh[27]",
        "shr[27]",
        "sh[28]",
        "shr[28]",
        "sh[29]",
        "shr[29]",

        //2 ��������
        "sh[30]",
        "shr[30]",
        "sh[31]",
        "shr[31]",
        "sh[32]",
        "shr[32]",
        "sh[33]",
        "shr[33]",
        "sh[34]",
        "shr[34]",
        "sh[35]",
        "shr[35]",
        "sh[36]",
        "shr[36]",
        "sh[37]",
        "shr[37]",
        "sh[38]",
        "shr[38]",
        "sh[39]",
        "shr[39]",
        "sh[40]",
        "shr[40]",
        "sh[41]",
        "shr[41]",
        "sh[42]",
        "shr[42]",
        "sh[43]",
        "shr[43]",
        "sh[44]",
        "shr[44]",
        //3
        "sh[45]",
        "shr[45]",
        "sh[46]",
        "shr[46]",
        "sh[47]",
        "shr[47]",
        "sh[48]",
        "shr[48]",
        "sh[49]",
        "shr[49]",
        "sh[50]",
        "shr[50]",
        "",
        "zshr3",
        "norma",
        "qkk",
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
        //4 ��������
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

        //5 ��������
        "diagn[30]",
        "",
        "diagnS[0]",
        "diagnS[1]",
        "diagnS[2]",
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

        //6 ��������
        "out[0]",
        "out[1]",
        "out[2]",
        "out[3]",
        "",
        "zin[0]",
        "zin[1]",
        "zin[2]",
        "zin[3]",
        "zin[4]",
        "",
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
        "",
        "",

        //7 ��������
        "aout[0]",
        "aout[1]",
        "aout[2]",
        "aout[3]",
        "aout[4]",
        "aout[5]",
        "aout[6]",
        "",
        "D_D1",
        "D_D2",
        "D_D3",
        "D_D4",
        "D_D5",
        "D_D6",
        "",
        "UVAK_KAM",
        "UVAKV_KAM",
        "UVAKN_KAM",
        "POROG_DAVL",
        "UVAK_TMNOP",
        "UVVAK_TMNOP",
        "UVVAK_KAM",
        "UVVAK_SHL",
        "POROG_M",
        "UVAKN_TMN",
        "UVAKV_TMN",
        "UVAK_SHL",
        "UVAK_SHL_MO",
        "UVAK_ZTMN",
        "UATM",

        //8 ��������
        "nasmod[0]",
        "nasmod[1]",
        "nasmod[2]",
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
        "nasmod[18]",
        "nasmod[19]",
        "nasmod[20]",
        "",
        "",
        "",
        "par_t[0]",
        "",
        "",
        "",
        "",
        "",

        //9 ��������
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
        "par[0][14]",
        "par[0][15]",
        "par[0][16]",
        "par[0][17]",
        "par[0][18]",
        "par[0][19]",
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

        //10 ��������
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
        "par[1][15]",
        "par[1][16]",
        "par[1][17]",
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

        //11 ��������
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
        "par[2][14]",
        "par[2][15]",
        "par[2][16]",
        "par[2][17]",
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

        //12 ��������
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
        "par[3][15]",
        "par[3][16]",
        "par[3][17]",
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

        //13 ��������
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
        "par[4][14]",
        "par[4][15]",
        "par[4][16]",
        "par[4][17]",
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

        //14 ��������
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
        "par[5][15]",
        "par[5][16]",
        "par[5][17]",
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



        //15 ��������
        "CT_T1",
        "CT_T20",
        "",
        "CT_1",
        "CT_2",
        "CT_3",
        "CT_4",
        "CT_5",
        "CT_6",
        "CT_7",
        "CT_9",
        "CT_19",
        "CT_27",
        "CT27K1",
        "CT_28",
        "CT28K1",
        "CT_29",
        "CT29K1",
        "CT_31",
        "CT31K1",
        "CT_33",
        "CT33K1",
        "CT_39",
        "CT_43",
        "CT43K1",
        "CT_46",
        "CT_47",
        "CT_48",
        "",
        "",


        //16 ��������
        "CT_VHG",
        "CT_PER",
        "CT_BMH1",
        "CT_TEMP1",
        "CT_TEMP2",
        "CT_TMN",
        "CT_IST",
        "CT_VODA_BM",
        "CT_VODA_II",
        "CT_KZ_BMH1",
        "CT_VAK",
        "",
        "CT_Kl6",
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

        //17 ��������
        "T_VHG",
        "T_PROC",
        "T_KTMN_RAZGON",
        "T_VKL_BPN",
        "T_VODA",
        "T_KKAM",
        "T_KTMN",
        "T_KPER",
        "T_KPRST",
        "T_KPR",
        "T_KSHL",
        "T_KNAP",
        "T_NAPUSK",
        "T_SBROSHE",
        "T_KSHL_MO",
        "T_KKAV_V",
        "T_KSHL_V",
        "T_OSTANOV_TMNOP",
        "T_M_VR",
        "T_RAZGON_TMN",
        "T_OTKL_TMN",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",

        //18 ��������
        "",
        "PR_TRTEST",
        "PR_NALADKA",
        "N_ST_MAX",
        "N_ST",
        "PR_RG3",
        "otvet",
        "",
        "PR_FOTK_SHL",
        "PR_TREN",
        "VRSO2",
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

        //19 ��������
        "PR_DZASL1",
        "OTVET_DZASL1",
        "",
        "",
        "DAVL_DZASL1",
        "DATA_DZASL1",
        "",
        "",
        "X_TDZASL1",
        "VRDZASL1",
        "E_TDZASL1",
        "DELDZASL1",
        "LIM1DZASL1",
        "LIM2DZASL1",
        "DOPDZASL1",
        "",
        "KOM_DZASL1",
        "",
        "",
        "",
        "",
        "CT_DZASL1",
        "T_KDZASL1",
        "T_VRDZASL1",
        "",
        "PAR_DZASL1",
        "ZPAR_DZASL1",
        "",
        "TEK_DAVL_DZASL1",
        "TEK_POZ_DZASL1",

        //20 ��������
        "PR_DZASL2",
        "OTVET_DZASL2",
        "",
        "",
        "DAVL_DZASL2",
        "DATA_DZASL2",
        "",
        "",
        "X_TDZASL2",
        "VRDZASL2",
        "E_TDZASL2",
        "DELDZASL2",
        "LIM1DZASL2",
        "LIM2DZASL2",
        "DOPDZASL2",
        "",
        "KOM_DZASL2",
        "",
        "",
        "",
        "",
        "CT_DZASL2",
        "T_KDZASL2",
        "T_VRDZASL2",
        "",
        "PAR_DZASL2",
        "ZPAR_DZASL2",
        "",
        "TEK_DAVL_DZASL2",
        "TEK_POZ_DZASL2",

        //21 ��������
        "VRGIS",
        "",
        "K_SOGL_GIS",
        "NAPRS_GIS",
        "X_TGIS",
        "E_TGIS",
        "DELGIS",
        "DOPGIS",
        "N_TEK_GIS",
        "LIM1GIS",
        "LIM2GIS",
        "T_VRGIS",
        "T_KGIS",
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
        
        //22 ��������
        "KOM_TEMP",
        "PR_TEMP",
        "TEK_TEMP3",
        "",
        "",
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
        "",
        "ZAD_TEMP2",
        "PAR_TEMP2",
        "X_TEMP2",
        "VRTEMP2",
        "E_TEMP2",
        "DELTEMP2",
        "LIM1TEMP2",
        "LIM2TEMP2",
        "DOPTEMP2",
        "TEK_TEMP2",
        "",
        "T_VRTEMP",
        "T_KTEMP",

        //23 ��������
        "PR_PER",
        "CT_PER",
        "",
        "T_KPER",
        "T_KPR",
        "T_KPRST",
        "",
        "POL_PER",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CT_POD",
        "KOM_POD",
        "OTVET_POD",
        "TYPE_POD",
        "",
        "PR_POD",
        "HOME_POD",
        "",
        "PUT_POD",
        "PAR_POD",
        "V_POD",
        "",
        "TEK_ABS_POD",
        "TEK_OTN_POD",

        //24 ��������
        "CT_VR",
        "KOM_VR",
        "OTVET_VR",
        "TYPE_VR",
        "",
        "PR_VR",
        "HOME_VR",
        "",
        "PUT_VR",
        "PAR_VR",
        "V_VR",
        "",
        "TEK_ABS_VR",
        "TEK_OTN_VR",
        "",
        "",
        "CT_POV",
        "KOM_POV",
        "OTVET_POV",
        "TYPE_POV",
        "",
        "PR_POV",
        "HOME_POV",
        "",
        "PUT_POV",
        "PAR_POV",
        "V_POV",
        "",
        "TEK_ABS_POV",
        "TEK_OTN_POV",



        //25 ��������
        "VRBMH1",
        "",
        "PR_SV_BMH1",
        "BMH1_mode",
        "UST_BMH1",
        "X_TBMH1",
        "E_TBMH1",
        "DELBMH1",
        "DOPBMH1",
        "PAR_BMH1_I",
        "PAR_BMH1_P",
        "",
        "LIM1BMH1",
        "LIM2BMH1",
        "N_PROBBMH",
        "T_VRBMH",
        "T_KBMH",
        "",
        "CT_KZ_BMH1",
        "PR_KZ_BMH1",
        "N_KZ_BMH1",
        "",
        "CT_BMH1",
        "",
        "",
        "",
        "",
        "",
        "",
        "",


        //26 ��������
        "OTVET_BMH1[0]",
        "OTVET_BMH1[1]",
        "OTVET_BMH1[2]",
        "OTVET_BMH1[3]",
        "OTVET_BMH1[4]",
        "OTVET_BMH1[5]",
        "OTVET_BMH1[6]",
        "OTVET_BMH1[7]",
        "OTVET_BMH1[8]",
        "OTVET_BMH1[9]",
        "OTVET_BMH1[10]",
        "OTVET_BMH1[11]",
        "OTVET_BMH1[12]",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "KOM_BMH1[0]",
        "KOM_BMH1[1]",
        "KOM_BMH1[2]",
        "KOM_BMH1[3]",
        "KOM_BMH1[4]",
        "KOM_BMH1[5]",
        "KOM_BMH1[6]",
        "KOM_BMH1[7]",
        "KOM_BMH1[8]",

        //27 ��������
        "VRSG",
        "X_TSG",
        "E_TSG",
        "DELSG",
        "E_PSG",
        "K_PSG",
        "K_ISG",
        "U_PSG",
        "A_VIH",
        "LIMPSG",
        "LIMISG",
        "LIM1SG",
        "LIM2SG",
        "LIMUSG",
        "LIMU_SG",
        "LIMUPR_SG",
        "PORCNV_SG",
        "PORCPR_SG",
        "PROBSG",
        "T_VRSG",
        "T_KSG",
        "T_VREJ_SG",
        "T_VPRB_SG",
        "T_REQSG",
        "CT_VRSG",
        "CT_PR_SG",
        "CT_REQSG",
        "SOSG",
        "DOPSG",
        "PAR_SG",

        

    };
	// ������� ����� �������� ��������
    unsigned char pageCount = StrToInt ( PCPerem -> ActivePage -> Hint );
    // ��������� ����� ���������� �� ��������
    for ( unsigned int i = 30 * pageCount; i < ( 30 * ( pageCount + 1  ) ); i++)
        EdtDebugValues[i%30] -> Text = valuesNames[i];

switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
{


case 0:
{
		EditOTLtek1->Text = IntToStr(sh_);
		EditOTLtek2->Text = IntToStr(0);
		EditOTLtek3->Text = IntToStr(sh[1]);
		EditOTLtek4->Text = IntToStr(shr[1]);
		EditOTLtek5->Text = IntToStr(sh[2]);
		EditOTLtek6->Text = IntToStr(shr[2]);
		EditOTLtek7->Text = IntToStr(sh[3]);
		EditOTLtek8->Text = IntToStr(shr[3]);
		EditOTLtek9->Text = IntToStr(sh[4]);
		EditOTLtek10->Text = IntToStr(shr[4]);
		EditOTLtek11->Text = IntToStr(sh[5]);
		EditOTLtek12->Text = IntToStr(shr[5]);
		EditOTLtek13->Text = IntToStr(sh[6]);
		EditOTLtek14->Text = IntToStr(shr[6]);
		EditOTLtek15->Text = IntToStr(sh[7]);
		EditOTLtek16->Text = IntToStr(shr[7]);
		EditOTLtek17->Text = IntToStr(sh[8]);
		EditOTLtek18->Text = IntToStr(shr[8]);
		EditOTLtek19->Text = IntToStr(sh[9]);
		EditOTLtek20->Text = IntToStr(shr[9]);
		EditOTLtek21->Text = IntToStr(sh[10]);
		EditOTLtek22->Text = IntToStr(shr[10]);
		EditOTLtek23->Text = IntToStr(sh[11]);
		EditOTLtek24->Text = IntToStr(shr[11]);
		EditOTLtek25->Text = IntToStr(sh[12]);
		EditOTLtek26->Text = IntToStr(shr[12]);
		EditOTLtek27->Text = IntToStr(sh[13]);
		EditOTLtek28->Text = IntToStr(shr[13]);
		EditOTLtek29->Text = IntToStr(sh[14]);
		EditOTLtek30->Text = IntToStr(shr[14]);
}; break;
case 1:
{
		EditOTLtek1->Text = IntToStr(sh[15]);
		EditOTLtek2->Text = IntToStr(shr[15]);
		EditOTLtek3->Text = IntToStr(sh[16]);
		EditOTLtek4->Text = IntToStr(shr[16]);
		EditOTLtek5->Text = IntToStr(sh[17]);
		EditOTLtek6->Text = IntToStr(shr[17]);
		EditOTLtek7->Text = IntToStr(sh[18]);
		EditOTLtek8->Text = IntToStr(shr[18]);
		EditOTLtek9->Text = IntToStr(sh[19]);
		EditOTLtek10->Text = IntToStr(shr[19]);
		EditOTLtek11->Text = IntToStr(sh[20]);
		EditOTLtek12->Text = IntToStr(shr[20]);
		EditOTLtek13->Text = IntToStr(sh[21]);
		EditOTLtek14->Text = IntToStr(shr[21]);
		EditOTLtek15->Text = IntToStr(sh[22]);
		EditOTLtek16->Text = IntToStr(shr[22]);
		EditOTLtek17->Text = IntToStr(sh[23]);
		EditOTLtek18->Text = IntToStr(shr[23]);
		EditOTLtek19->Text = IntToStr(sh[24]);
		EditOTLtek20->Text = IntToStr(shr[24]);
		EditOTLtek21->Text = IntToStr(sh[25]);
		EditOTLtek22->Text = IntToStr(shr[25]);
		EditOTLtek23->Text = IntToStr(sh[26]);
		EditOTLtek24->Text = IntToStr(shr[26]);
		EditOTLtek25->Text = IntToStr(sh[27]);
		EditOTLtek26->Text = IntToStr(shr[27]);
		EditOTLtek27->Text = IntToStr(sh[28]);
		EditOTLtek28->Text = IntToStr(shr[28]);
		EditOTLtek29->Text = IntToStr(sh[29]);
		EditOTLtek30->Text = IntToStr(shr[29]);
}; break;
case 2:
{
		EditOTLtek1->Text = IntToStr(sh[30]);
		EditOTLtek2->Text = IntToStr(shr[30]);
		EditOTLtek3->Text = IntToStr(sh[31]);
		EditOTLtek4->Text = IntToStr(shr[31]);
		EditOTLtek5->Text = IntToStr(sh[32]);
		EditOTLtek6->Text = IntToStr(shr[32]);
		EditOTLtek7->Text = IntToStr(sh[33]);
		EditOTLtek8->Text = IntToStr(shr[33]);
		EditOTLtek9->Text = IntToStr(sh[34]);
		EditOTLtek10->Text = IntToStr(shr[34]);
		EditOTLtek11->Text = IntToStr(sh[35]);
		EditOTLtek12->Text = IntToStr(shr[35]);
		EditOTLtek13->Text = IntToStr(sh[36]);
		EditOTLtek14->Text = IntToStr(shr[36]);
		EditOTLtek15->Text = IntToStr(sh[37]);
		EditOTLtek16->Text = IntToStr(shr[37]);
		EditOTLtek17->Text = IntToStr(sh[38]);
		EditOTLtek18->Text = IntToStr(shr[38]);
		EditOTLtek19->Text = IntToStr(sh[39]);
		EditOTLtek20->Text = IntToStr(shr[39]);
		EditOTLtek21->Text = IntToStr(sh[40]);
		EditOTLtek22->Text = IntToStr(shr[40]);
		EditOTLtek23->Text = IntToStr(sh[41]);
		EditOTLtek24->Text = IntToStr(shr[41]);
		EditOTLtek25->Text = IntToStr(sh[42]);
		EditOTLtek26->Text = IntToStr(shr[42]);
		EditOTLtek27->Text = IntToStr(sh[43]);
		EditOTLtek28->Text = IntToStr(shr[43]);
		EditOTLtek29->Text = IntToStr(shr[44]);
		EditOTLtek30->Text = IntToStr(shr[44]);
}; break;
case 3:
{
		EditOTLtek1->Text = IntToStr(sh[45]);
		EditOTLtek2->Text = IntToStr(shr[45]);
		EditOTLtek3->Text = IntToStr(sh[46]);
		EditOTLtek4->Text = IntToStr(shr[46]);
		EditOTLtek5->Text = IntToStr(sh[47]);
		EditOTLtek6->Text = IntToStr(shr[47]);
		EditOTLtek7->Text = IntToStr(sh[48]);
		EditOTLtek8->Text = IntToStr(shr[48]);
		EditOTLtek9->Text = IntToStr(sh[49]);
		EditOTLtek10->Text = IntToStr(shr[49]);
		EditOTLtek11->Text = IntToStr(sh[50]);
		EditOTLtek12->Text = IntToStr(shr[50]);
		EditOTLtek13->Text = IntToStr(0);
		EditOTLtek14->Text = IntToStr(zshr3);
		EditOTLtek15->Text = IntToStr(norma);
		EditOTLtek16->Text = IntToStr(qkk);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
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
case 4:
{
		EditOTLtek1->Text = IntToStr(diagn[0]);
		EditOTLtek2->Text = IntToStr(diagn[1]);
		EditOTLtek3->Text = IntToStr(diagn[2]);
		EditOTLtek4->Text = IntToStr(diagn[3]);
		EditOTLtek5->Text = IntToStr(diagn[4]);
		EditOTLtek6->Text = IntToStr(diagn[5]);
		EditOTLtek7->Text = IntToStr(diagn[6]);
		EditOTLtek8->Text = IntToStr(diagn[7]);
		EditOTLtek9->Text = IntToStr(diagn[8]);
		EditOTLtek10->Text = IntToStr(diagn[9]);
		EditOTLtek11->Text = IntToStr(diagn[10]);
		EditOTLtek12->Text = IntToStr(diagn[11]);
		EditOTLtek13->Text = IntToStr(diagn[12]);
		EditOTLtek14->Text = IntToStr(diagn[13]);
		EditOTLtek15->Text = IntToStr(diagn[14]);
		EditOTLtek16->Text = IntToStr(diagn[15]);
		EditOTLtek17->Text = IntToStr(diagn[16]);
		EditOTLtek18->Text = IntToStr(diagn[17]);
		EditOTLtek19->Text = IntToStr(diagn[18]);
		EditOTLtek20->Text = IntToStr(diagn[19]);
		EditOTLtek21->Text = IntToStr(diagn[20]);
		EditOTLtek22->Text = IntToStr(diagn[21]);
		EditOTLtek23->Text = IntToStr(diagn[22]);
		EditOTLtek24->Text = IntToStr(diagn[23]);
		EditOTLtek25->Text = IntToStr(diagn[24]);
		EditOTLtek26->Text = IntToStr(diagn[25]);
		EditOTLtek27->Text = IntToStr(diagn[26]);
		EditOTLtek28->Text = IntToStr(diagn[27]);
		EditOTLtek29->Text = IntToStr(diagn[28]);
		EditOTLtek30->Text = IntToStr(diagn[29]);
}; break;
case 5:
{
		EditOTLtek1->Text = IntToStr(diagn[30]);
		EditOTLtek2->Text = IntToStr(0);
		EditOTLtek3->Text = IntToStr(diagnS[0]);
		EditOTLtek4->Text = IntToStr(diagnS[1]);
		EditOTLtek5->Text = IntToStr(diagnS[2]);
		EditOTLtek6->Text = IntToStr(0);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(0);
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(0);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(0);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
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
case 6:
{
		EditOTLtek1->Text = IntToStr(out[0]);
		EditOTLtek2->Text = IntToStr(out[1]);
		EditOTLtek3->Text = IntToStr(out[2]);
		EditOTLtek4->Text = IntToStr(out[3]);
		EditOTLtek5->Text = IntToStr(0);
		EditOTLtek6->Text = IntToStr(zin[0]);
		EditOTLtek7->Text = IntToStr(zin[1]);
		EditOTLtek8->Text = IntToStr(zin[2]);
		EditOTLtek9->Text = IntToStr(zin[3]);
		EditOTLtek10->Text = IntToStr(zin[4]);
		EditOTLtek11->Text = IntToStr(0);
		EditOTLtek12->Text = IntToStr(aik[0]);
		EditOTLtek13->Text = IntToStr(aik[1]);
		EditOTLtek14->Text = IntToStr(aik[2]);
		EditOTLtek15->Text = IntToStr(aik[3]);
		EditOTLtek16->Text = IntToStr(aik[4]);
		EditOTLtek17->Text = IntToStr(aik[5]);
		EditOTLtek18->Text = IntToStr(aik[6]);
		EditOTLtek19->Text = IntToStr(aik[7]);
		EditOTLtek20->Text = IntToStr(aik[8]);
		EditOTLtek21->Text = IntToStr(aik[9]);
		EditOTLtek22->Text = IntToStr(aik[10]);
		EditOTLtek23->Text = IntToStr(aik[11]);
		EditOTLtek24->Text = IntToStr(aik[12]);
		EditOTLtek25->Text = IntToStr(aik[13]);
		EditOTLtek26->Text = IntToStr(aik[14]);
		EditOTLtek27->Text = IntToStr(aik[15]);
		EditOTLtek28->Text = IntToStr(aik[16]);
		EditOTLtek29->Text = IntToStr(0);
	   	EditOTLtek30->Text = IntToStr(0);
}; break;
case 7:
{
		EditOTLtek1->Text = IntToStr(aout[0]);
		EditOTLtek2->Text = IntToStr(aout[1]);
		EditOTLtek3->Text = IntToStr(aout[2]);
		EditOTLtek4->Text = IntToStr(aout[3]);
		EditOTLtek5->Text = IntToStr(aout[4]);
		EditOTLtek6->Text = IntToStr(aout[5]);
		EditOTLtek7->Text = IntToStr(aout[6]);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(D_D1);
		EditOTLtek10->Text = IntToStr(D_D2);
		EditOTLtek11->Text = IntToStr(D_D3);
		EditOTLtek12->Text = IntToStr(D_D4);
		EditOTLtek13->Text = IntToStr(D_D5);
		EditOTLtek14->Text = IntToStr(D_D6);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(UVAK_KAM);
		EditOTLtek17->Text = IntToStr(UVAKV_KAM);
		EditOTLtek18->Text = IntToStr(UVAKN_KAM);
		EditOTLtek19->Text = IntToStr(POROG_DAVL);
		EditOTLtek20->Text = IntToStr(UVAK_TMNOP);
		EditOTLtek21->Text = IntToStr(UVVAK_TMNOP);
		EditOTLtek22->Text = IntToStr(UVVAK_KAM);
		EditOTLtek23->Text = IntToStr(UVVAK_SHL);
		EditOTLtek24->Text = IntToStr(POROG_M);
		EditOTLtek25->Text = IntToStr(UVAKN_TMN);
		EditOTLtek26->Text = IntToStr(UVAKV_TMN);
		EditOTLtek27->Text = IntToStr(UVAK_SHL);
		EditOTLtek28->Text = IntToStr(UVAK_SHL_MO);
		EditOTLtek29->Text = IntToStr(UVAK_ZTMN);
		EditOTLtek30->Text = IntToStr(UATM);
}; break;
case 8:
{
		EditOTLtek1->Text = IntToStr(nasmod[0]);
		EditOTLtek2->Text = IntToStr(nasmod[1]);
		EditOTLtek3->Text = IntToStr(nasmod[2]);
		EditOTLtek4->Text = IntToStr(nasmod[3]);
		EditOTLtek5->Text = IntToStr(nasmod[4]);
		EditOTLtek6->Text = IntToStr(nasmod[5]);
		EditOTLtek7->Text = IntToStr(nasmod[6]);
		EditOTLtek8->Text = IntToStr(nasmod[7]);
		EditOTLtek9->Text = IntToStr(nasmod[8]);
		EditOTLtek10->Text = IntToStr(nasmod[9]);
		EditOTLtek11->Text = IntToStr(nasmod[10]);
		EditOTLtek12->Text = IntToStr(nasmod[11]);
		EditOTLtek13->Text = IntToStr(nasmod[12]);
		EditOTLtek14->Text = IntToStr(nasmod[13]);
		EditOTLtek15->Text = IntToStr(nasmod[14]);
		EditOTLtek16->Text = IntToStr(nasmod[15]);
		EditOTLtek17->Text = IntToStr(nasmod[16]);
		EditOTLtek18->Text = IntToStr(nasmod[17]);
		EditOTLtek19->Text = IntToStr(nasmod[18]);
		EditOTLtek20->Text = IntToStr(nasmod[19]);
		EditOTLtek21->Text = IntToStr(nasmod[20]);
		EditOTLtek22->Text = IntToStr(0);
		EditOTLtek23->Text = IntToStr(0);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(par_t[0]);
		EditOTLtek26->Text = IntToStr(0);
		EditOTLtek27->Text = IntToStr(0);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(0);
}; break;
case 9:
{
		EditOTLtek1->Text = IntToStr(par[0][0]);
		EditOTLtek2->Text = IntToStr(par[0][1]);
		EditOTLtek3->Text = IntToStr(par[0][2]);
		EditOTLtek4->Text = IntToStr(par[0][3]);
		EditOTLtek5->Text = IntToStr(par[0][4]);
		EditOTLtek6->Text = IntToStr(par[0][5]);
		EditOTLtek7->Text = IntToStr(par[0][6]);
		EditOTLtek8->Text = IntToStr(par[0][7]);
		EditOTLtek9->Text = IntToStr(par[0][8]);
		EditOTLtek10->Text = IntToStr(par[0][9]);
		EditOTLtek11->Text = IntToStr(par[0][10]);
		EditOTLtek12->Text = IntToStr(par[0][11]);
		EditOTLtek13->Text = IntToStr(par[0][12]);
		EditOTLtek14->Text = IntToStr(par[0][13]);
		EditOTLtek15->Text = IntToStr(par[0][14]);
		EditOTLtek16->Text = IntToStr(par[0][15]);
		EditOTLtek17->Text = IntToStr(par[0][16]);
		EditOTLtek18->Text = IntToStr(par[0][17]);
		EditOTLtek19->Text = IntToStr(par[0][18]);
		EditOTLtek20->Text = IntToStr(par[0][19]);
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
case 10:
{
		EditOTLtek1->Text = IntToStr(par[1][0]);
		EditOTLtek2->Text = IntToStr(par[1][1]);
		EditOTLtek3->Text = IntToStr(par[1][2]);
		EditOTLtek4->Text = IntToStr(par[1][3]);
		EditOTLtek5->Text = IntToStr(par[1][4]);
		EditOTLtek6->Text = IntToStr(par[1][5]);
		EditOTLtek7->Text = IntToStr(par[1][6]);
		EditOTLtek8->Text = IntToStr(par[1][7]);
		EditOTLtek9->Text = IntToStr(par[1][8]);
		EditOTLtek10->Text = IntToStr(par[1][9]);
		EditOTLtek11->Text = IntToStr(par[1][10]);
		EditOTLtek12->Text = IntToStr(par[1][11]);
		EditOTLtek13->Text = IntToStr(par[1][12]);
		EditOTLtek14->Text = IntToStr(par[1][13]);
		EditOTLtek15->Text = IntToStr(par[1][14]);
		EditOTLtek16->Text = IntToStr(par[1][15]);
		EditOTLtek17->Text = IntToStr(par[1][16]);
		EditOTLtek18->Text = IntToStr(par[1][17]);
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
case 11:
{
		EditOTLtek1->Text = IntToStr(par[2][0]);
		EditOTLtek2->Text = IntToStr(par[2][1]);
		EditOTLtek3->Text = IntToStr(par[2][2]);
		EditOTLtek4->Text = IntToStr(par[2][3]);
		EditOTLtek5->Text = IntToStr(par[2][4]);
		EditOTLtek6->Text = IntToStr(par[2][5]);
		EditOTLtek7->Text = IntToStr(par[2][6]);
		EditOTLtek8->Text = IntToStr(par[2][7]);
		EditOTLtek9->Text = IntToStr(par[2][8]);
		EditOTLtek10->Text = IntToStr(par[2][9]);
		EditOTLtek11->Text = IntToStr(par[2][10]);
		EditOTLtek12->Text = IntToStr(par[2][11]);
		EditOTLtek13->Text = IntToStr(par[2][12]);
		EditOTLtek14->Text = IntToStr(par[2][13]);
		EditOTLtek15->Text = IntToStr(par[2][14]);
		EditOTLtek16->Text = IntToStr(par[2][15]);
		EditOTLtek17->Text = IntToStr(par[2][16]);
		EditOTLtek18->Text = IntToStr(par[2][17]);
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
case 12:
{
		EditOTLtek1->Text = IntToStr(par[3][0]);
		EditOTLtek2->Text = IntToStr(par[3][1]);
		EditOTLtek3->Text = IntToStr(par[3][2]);
		EditOTLtek4->Text = IntToStr(par[3][3]);
		EditOTLtek5->Text = IntToStr(par[3][4]);
		EditOTLtek6->Text = IntToStr(par[3][5]);
		EditOTLtek7->Text = IntToStr(par[3][6]);
		EditOTLtek8->Text = IntToStr(par[3][7]);
		EditOTLtek9->Text = IntToStr(par[3][8]);
		EditOTLtek10->Text = IntToStr(par[3][9]);
		EditOTLtek11->Text = IntToStr(par[3][10]);
		EditOTLtek12->Text = IntToStr(par[3][11]);
		EditOTLtek13->Text = IntToStr(par[3][12]);
		EditOTLtek14->Text = IntToStr(par[3][13]);
		EditOTLtek15->Text = IntToStr(par[3][14]);
		EditOTLtek16->Text = IntToStr(par[3][15]);
		EditOTLtek17->Text = IntToStr(par[3][16]);
		EditOTLtek18->Text = IntToStr(par[3][17]);
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
case 13:
{
		EditOTLtek1->Text = IntToStr(par[4][0]);
		EditOTLtek2->Text = IntToStr(par[4][1]);
		EditOTLtek3->Text = IntToStr(par[4][2]);
		EditOTLtek4->Text = IntToStr(par[4][3]);
		EditOTLtek5->Text = IntToStr(par[4][4]);
		EditOTLtek6->Text = IntToStr(par[4][5]);
		EditOTLtek7->Text = IntToStr(par[4][6]);
		EditOTLtek8->Text = IntToStr(par[4][7]);
		EditOTLtek9->Text = IntToStr(par[4][8]);
		EditOTLtek10->Text = IntToStr(par[4][9]);
		EditOTLtek11->Text = IntToStr(par[4][10]);
		EditOTLtek12->Text = IntToStr(par[4][11]);
		EditOTLtek13->Text = IntToStr(par[4][12]);
		EditOTLtek14->Text = IntToStr(par[4][13]);
		EditOTLtek15->Text = IntToStr(par[4][14]);
		EditOTLtek16->Text = IntToStr(par[4][15]);
		EditOTLtek17->Text = IntToStr(par[4][16]);
		EditOTLtek18->Text = IntToStr(par[4][17]);
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
case 14:
{
		EditOTLtek1->Text = IntToStr(par[5][0]);
		EditOTLtek2->Text = IntToStr(par[5][1]);
		EditOTLtek3->Text = IntToStr(par[5][2]);
		EditOTLtek4->Text = IntToStr(par[5][3]);
		EditOTLtek5->Text = IntToStr(par[5][4]);
		EditOTLtek6->Text = IntToStr(par[5][5]);
		EditOTLtek7->Text = IntToStr(par[5][6]);
		EditOTLtek8->Text = IntToStr(par[5][7]);
		EditOTLtek9->Text = IntToStr(par[5][8]);
		EditOTLtek10->Text = IntToStr(par[5][9]);
		EditOTLtek11->Text = IntToStr(par[5][10]);
		EditOTLtek12->Text = IntToStr(par[5][11]);
		EditOTLtek13->Text = IntToStr(par[5][12]);
		EditOTLtek14->Text = IntToStr(par[5][13]);
		EditOTLtek15->Text = IntToStr(par[5][14]);
		EditOTLtek16->Text = IntToStr(par[5][15]);
		EditOTLtek17->Text = IntToStr(par[5][16]);
		EditOTLtek18->Text = IntToStr(par[5][17]);
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

case 15:
{
		EditOTLtek1->Text = IntToStr(CT_T1);
		EditOTLtek2->Text = IntToStr(CT_T20);
		EditOTLtek3->Text = IntToStr(0);
		EditOTLtek4->Text = IntToStr(CT_1);
		EditOTLtek5->Text = IntToStr(CT_2);
		EditOTLtek6->Text = IntToStr(CT_3);
		EditOTLtek7->Text = IntToStr(CT_4);
		EditOTLtek8->Text = IntToStr(CT_5);
		EditOTLtek9->Text = IntToStr(CT_6);
		EditOTLtek10->Text = IntToStr(CT_7);
		EditOTLtek11->Text = IntToStr(CT_9);
		EditOTLtek12->Text = IntToStr(CT_19);
		EditOTLtek13->Text = IntToStr(CT_27);
		EditOTLtek14->Text = IntToStr(CT27K1);
		EditOTLtek15->Text = IntToStr(CT_28);
		EditOTLtek16->Text = IntToStr(CT28K1);
		EditOTLtek17->Text = IntToStr(CT_29);
		EditOTLtek18->Text = IntToStr(CT29K1);
		EditOTLtek19->Text = IntToStr(CT_31);
		EditOTLtek20->Text = IntToStr(CT31K1);
		EditOTLtek21->Text = IntToStr(CT_33);
		EditOTLtek22->Text = IntToStr(CT33K1);
		EditOTLtek23->Text = IntToStr(CT_39);
		EditOTLtek24->Text = IntToStr(CT_43);
		EditOTLtek25->Text = IntToStr(CT43K1);
		EditOTLtek26->Text = IntToStr(CT_46);
		EditOTLtek27->Text = IntToStr(CT_47);
		EditOTLtek28->Text = IntToStr(CT_48);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(0);
}; break;
case 16:
{
		EditOTLtek1->Text = IntToStr(CT_VHG);
		EditOTLtek2->Text = IntToStr(CT_PER);
		EditOTLtek3->Text = IntToStr(CT_BMH1);
		EditOTLtek4->Text = IntToStr(CT_TEMP1);
		EditOTLtek5->Text = IntToStr(CT_TEMP2);
		EditOTLtek6->Text = IntToStr(CT_TMN);
		EditOTLtek7->Text = IntToStr(CT_IST);
		EditOTLtek8->Text = IntToStr(CT_VODA_BM);
		EditOTLtek9->Text = IntToStr(CT_VODA_II);
		EditOTLtek10->Text = IntToStr(CT_KZ_BMH1);
		EditOTLtek11->Text = IntToStr(CT_VAK);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(CT_Kl6);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
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
case 17:
{
		EditOTLtek1->Text = IntToStr(T_VHG);
		EditOTLtek2->Text = IntToStr(T_PROC);
		EditOTLtek3->Text = IntToStr(T_KTMN_RAZGON);
		EditOTLtek4->Text = IntToStr(T_VKL_BPN);
		EditOTLtek5->Text = IntToStr(T_VODA);
		EditOTLtek6->Text = IntToStr(T_KKAM);
		EditOTLtek7->Text = IntToStr(T_KTMN);
		EditOTLtek8->Text = IntToStr(T_KPER);
		EditOTLtek9->Text = IntToStr(T_KPRST);
		EditOTLtek10->Text = IntToStr(T_KPR);
		EditOTLtek11->Text = IntToStr(T_KSHL);
		EditOTLtek12->Text = IntToStr(T_KNAP);
		EditOTLtek13->Text = IntToStr(T_NAPUSK);
		EditOTLtek14->Text = IntToStr(T_SBROSHE);
		EditOTLtek15->Text = IntToStr(T_KSHL_MO);
		EditOTLtek16->Text = IntToStr(T_KKAV_V);
		EditOTLtek17->Text = IntToStr(T_KSHL_V);
		EditOTLtek18->Text = IntToStr(T_OSTANOV_TMNOP);
		EditOTLtek19->Text = IntToStr(T_M_VR);
		EditOTLtek20->Text = IntToStr(T_RAZGON_TMN);
		EditOTLtek21->Text = IntToStr(T_OTKL_TMN);
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
case 18:
{
		EditOTLtek1->Text = IntToStr(zshr3);
		EditOTLtek2->Text = IntToStr(PR_TRTEST);
		EditOTLtek3->Text = IntToStr(PR_NALADKA);
		EditOTLtek4->Text = IntToStr(N_ST_MAX);
		EditOTLtek5->Text = IntToStr(N_ST);
		EditOTLtek6->Text = IntToStr(PR_RG3);
		EditOTLtek7->Text = IntToStr(otvet);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(PR_FOTK_SHL);
		EditOTLtek10->Text = IntToStr(PR_TREN);
		EditOTLtek11->Text = IntToStr(VRSO2);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(0);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
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
case 19:
{
		EditOTLtek1->Text = IntToStr(PR_DZASL1);
		EditOTLtek2->Text = IntToStr(OTVET_DZASL1);
		EditOTLtek3->Text = IntToStr(0);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(DAVL_DZASL1);
		EditOTLtek6->Text = IntToStr(DATA_DZASL1);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(X_TDZASL1);
		EditOTLtek10->Text = IntToStr(VRDZASL1);
		EditOTLtek11->Text = IntToStr(E_TDZASL1);
		EditOTLtek12->Text = IntToStr(DELDZASL1);
		EditOTLtek13->Text = IntToStr(LIM1DZASL1);
		EditOTLtek14->Text = IntToStr(LIM2DZASL1);
		EditOTLtek15->Text = IntToStr(DOPDZASL1);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(KOM_DZASL1);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(0);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(CT_DZASL1);
		EditOTLtek23->Text = IntToStr(T_KDZASL1);
		EditOTLtek24->Text = IntToStr(T_VRDZASL1);
		EditOTLtek25->Text = IntToStr(0);
		EditOTLtek26->Text = IntToStr(PAR_DZASL1);
		EditOTLtek27->Text = IntToStr(ZPAR_DZASL1);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(TEK_DAVL_DZASL1);
		EditOTLtek30->Text = IntToStr(TEK_POZ_DZASL1);
}; break;
case 20:
{
		EditOTLtek1->Text = IntToStr(PR_DZASL2);
		EditOTLtek2->Text = IntToStr(OTVET_DZASL2);
		EditOTLtek3->Text = IntToStr(0);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(DAVL_DZASL2);
		EditOTLtek6->Text = IntToStr(DATA_DZASL2);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(X_TDZASL2);
		EditOTLtek10->Text = IntToStr(VRDZASL2);
		EditOTLtek11->Text = IntToStr(E_TDZASL2);
		EditOTLtek12->Text = IntToStr(DELDZASL2);
		EditOTLtek13->Text = IntToStr(LIM1DZASL2);
		EditOTLtek14->Text = IntToStr(LIM2DZASL2);
		EditOTLtek15->Text = IntToStr(DOPDZASL2);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(KOM_DZASL2);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(0);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(CT_DZASL2);
		EditOTLtek23->Text = IntToStr(T_KDZASL2);
		EditOTLtek24->Text = IntToStr(T_VRDZASL2);
		EditOTLtek25->Text = IntToStr(0);
		EditOTLtek26->Text = IntToStr(PAR_DZASL2);
		EditOTLtek27->Text = IntToStr(ZPAR_DZASL2);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(TEK_DAVL_DZASL2);
		EditOTLtek30->Text = IntToStr(TEK_POZ_DZASL2);
}; break;
case 21:
{
		EditOTLtek1->Text = IntToStr(VRGIS);
		EditOTLtek2->Text = IntToStr(0);
		EditOTLtek3->Text = IntToStr(K_SOGL_GIS);
		EditOTLtek4->Text = IntToStr(NAPRS_GIS);
		EditOTLtek5->Text = IntToStr(X_TGIS);
		EditOTLtek6->Text = IntToStr(E_TGIS);
		EditOTLtek7->Text = IntToStr(DELGIS);
		EditOTLtek8->Text = IntToStr(DOPGIS);
		EditOTLtek9->Text = IntToStr(N_TEK_GIS);
		EditOTLtek10->Text = IntToStr(LIM1GIS);
		EditOTLtek11->Text = IntToStr(LIM2GIS);
		EditOTLtek12->Text = IntToStr(T_VRGIS);
		EditOTLtek13->Text = IntToStr(T_KGIS);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
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
case 22:
{
		EditOTLtek1->Text = IntToStr(KOM_TEMP);
		EditOTLtek2->Text = IntToStr(PR_TEMP);
		EditOTLtek3->Text = IntToStr(TEK_TEMP3);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(0);
		EditOTLtek6->Text = IntToStr(ZAD_TEMP1);
		EditOTLtek7->Text = IntToStr(PAR_TEMP1);
		EditOTLtek8->Text = IntToStr(ZPAR_TEMP1);
		EditOTLtek9->Text = IntToStr(X_TEMP1);
		EditOTLtek10->Text = IntToStr(VRTEMP1);
		EditOTLtek11->Text = IntToStr(E_TEMP1);
		EditOTLtek12->Text = IntToStr(DELTEMP1);
		EditOTLtek13->Text = IntToStr(LIM1TEMP1);
		EditOTLtek14->Text = IntToStr(LIM2TEMP1);
		EditOTLtek15->Text = IntToStr(DOPTEMP1);
		EditOTLtek16->Text = IntToStr(TEK_TEMP1);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(ZAD_TEMP2);
		EditOTLtek19->Text = IntToStr(PAR_TEMP2);
		EditOTLtek20->Text = IntToStr(X_TEMP2);
		EditOTLtek21->Text = IntToStr(VRTEMP2);
		EditOTLtek22->Text = IntToStr(E_TEMP2);
		EditOTLtek23->Text = IntToStr(DELTEMP2);
		EditOTLtek24->Text = IntToStr(LIM1TEMP2);
		EditOTLtek25->Text = IntToStr(LIM2TEMP2);
		EditOTLtek26->Text = IntToStr(DOPTEMP2);
		EditOTLtek27->Text = IntToStr(TEK_TEMP2);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(T_VRTEMP);
		EditOTLtek30->Text = IntToStr(T_KTEMP);
}; break;


case 23:
{
		EditOTLtek1->Text = IntToStr(PR_PER);
		EditOTLtek2->Text = IntToStr(CT_PER);
		EditOTLtek3->Text = IntToStr(0);
		EditOTLtek4->Text = IntToStr(T_KPER);
		EditOTLtek5->Text = IntToStr(T_KPR);
		EditOTLtek6->Text = IntToStr(T_KPRST);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(POL_PER);
		EditOTLtek9->Text = IntToStr(0);
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(0);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(0);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(CT_POD);
		EditOTLtek18->Text = IntToStr(KOM_POD);
		EditOTLtek19->Text = IntToStr(OTVET_POD);
		EditOTLtek20->Text = IntToStr(TYPE_POD);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(PR_POD);
		EditOTLtek23->Text = IntToStr(HOME_POD);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(PUT_POD);
		EditOTLtek26->Text = IntToStr(PAR_POD);
		EditOTLtek27->Text = IntToStr(V_POD);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(TEK_ABS_POD);
		EditOTLtek30->Text = IntToStr(TEK_OTN_POD);
}; break;
case 24:
{
		EditOTLtek1->Text = IntToStr(CT_VR);
		EditOTLtek2->Text = IntToStr(KOM_VR);
		EditOTLtek3->Text = IntToStr(OTVET_VR);
		EditOTLtek4->Text = IntToStr(TYPE_VR);
		EditOTLtek5->Text = IntToStr(0);
		EditOTLtek6->Text = IntToStr(PR_VR);
		EditOTLtek7->Text = IntToStr(HOME_VR);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(PUT_VR);
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(V_VR);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(TEK_ABS_VR);
		EditOTLtek14->Text = IntToStr(TEK_OTN_VR);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(CT_POV);
		EditOTLtek18->Text = IntToStr(KOM_POV);
		EditOTLtek19->Text = IntToStr(OTVET_POV);
		EditOTLtek20->Text = IntToStr(TYPE_POV);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(PR_POV);
		EditOTLtek23->Text = IntToStr(HOME_POV);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(PUT_POV);
		EditOTLtek26->Text = IntToStr(0);
		EditOTLtek27->Text = IntToStr(V_POV);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(TEK_ABS_POV);
		EditOTLtek30->Text = IntToStr(TEK_OTN_POV);
}; break;
case 25:
{
  		EditOTLtek1->Text = IntToStr(VRBMH1);
		EditOTLtek2->Text = IntToStr(0);
		EditOTLtek3->Text = IntToStr(PR_SV_BMH1);
		EditOTLtek4->Text = IntToStr(BMH1_mode);
		EditOTLtek5->Text = IntToStr(UST_BMH1);
		EditOTLtek6->Text = IntToStr(X_TBMH1);
		EditOTLtek7->Text = IntToStr(E_TBMH1);
		EditOTLtek8->Text = IntToStr(DELBMH1);
		EditOTLtek9->Text = IntToStr(DOPBMH1);
		EditOTLtek10->Text = IntToStr(PAR_BMH1_I);
		EditOTLtek11->Text = IntToStr(PAR_BMH1_P);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(LIM1BMH1);
		EditOTLtek14->Text = IntToStr(LIM2BMH1);
		EditOTLtek15->Text = IntToStr(N_PROBBMH);
		EditOTLtek16->Text = IntToStr(T_VRBMH);
		EditOTLtek17->Text = IntToStr(T_KBMH);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(CT_KZ_BMH1);
		EditOTLtek20->Text = IntToStr(PR_KZ_BMH1);
		EditOTLtek21->Text = IntToStr(N_KZ_BMH1);
		EditOTLtek22->Text = IntToStr(0);
		EditOTLtek23->Text = IntToStr(CT_BMH1);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(0);
		EditOTLtek26->Text = IntToStr(0);
		EditOTLtek27->Text = IntToStr(0);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(0);
}; break;

case 26:
{
 		EditOTLtek1->Text = IntToStr(OTVET_BMH1[0]);
		EditOTLtek2->Text = IntToStr(OTVET_BMH1[1]);
		EditOTLtek3->Text = IntToStr(OTVET_BMH1[2]);
		EditOTLtek4->Text = IntToStr(OTVET_BMH1[3]);
		EditOTLtek5->Text = IntToStr(OTVET_BMH1[4]);
		EditOTLtek6->Text = IntToStr(OTVET_BMH1[5]);
		EditOTLtek7->Text = IntToStr(OTVET_BMH1[6]);
		EditOTLtek8->Text = IntToStr(OTVET_BMH1[7]);
		EditOTLtek9->Text = IntToStr(OTVET_BMH1[8]);
		EditOTLtek10->Text = IntToStr(OTVET_BMH1[9]);
		EditOTLtek11->Text = IntToStr(OTVET_BMH1[10]);
		EditOTLtek12->Text = IntToStr(OTVET_BMH1[11]);
		EditOTLtek13->Text = IntToStr(OTVET_BMH1[12]);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(0);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(KOM_BMH1[0]);
		EditOTLtek23->Text = IntToStr(KOM_BMH1[1]);
		EditOTLtek24->Text = IntToStr(KOM_BMH1[2]);
		EditOTLtek25->Text = IntToStr(KOM_BMH1[3]);
		EditOTLtek26->Text = IntToStr(KOM_BMH1[4]);
		EditOTLtek27->Text = IntToStr(KOM_BMH1[5]);
		EditOTLtek28->Text = IntToStr(KOM_BMH1[6]);
		EditOTLtek29->Text = IntToStr(KOM_BMH1[7]);
		EditOTLtek30->Text = IntToStr(KOM_BMH1[8]);
}; break;
case 27:
{
		EditOTLtek1->Text = IntToStr(VRSG);
		EditOTLtek2->Text = IntToStr(X_TSG);
		EditOTLtek3->Text = IntToStr(E_TSG);
		EditOTLtek4->Text = IntToStr(DELSG);
		EditOTLtek5->Text = IntToStr(E_PSG);
		EditOTLtek6->Text = IntToStr(K_PSG);
		EditOTLtek7->Text = IntToStr(K_ISG);
		EditOTLtek8->Text = IntToStr(U_PSG);
		EditOTLtek9->Text = IntToStr(A_VIH);
		EditOTLtek10->Text = IntToStr(LIMPSG);
		EditOTLtek11->Text = IntToStr(LIMISG);
		EditOTLtek12->Text = IntToStr(LIM1SG);
		EditOTLtek13->Text = IntToStr(LIM2SG);
		EditOTLtek14->Text = IntToStr(LIMUSG);
		EditOTLtek15->Text = IntToStr(LIMU_SG);
		EditOTLtek16->Text = IntToStr(LIMUPR_SG);
		EditOTLtek17->Text = IntToStr(PORCNV_SG);
		EditOTLtek18->Text = IntToStr(PORCPR_SG);
		EditOTLtek19->Text = IntToStr(PROBSG);
		EditOTLtek20->Text = IntToStr(T_VRSG);
		EditOTLtek21->Text = IntToStr(T_KSG);
		EditOTLtek22->Text = IntToStr(T_VREJ_SG);
		EditOTLtek23->Text = IntToStr(T_VPRB_SG);
		EditOTLtek24->Text = IntToStr(T_REQSG);
		EditOTLtek25->Text = IntToStr(CT_VRSG);
		EditOTLtek26->Text = IntToStr(CT_PR_SG);
		EditOTLtek27->Text = IntToStr(CT_REQSG);
		EditOTLtek28->Text = IntToStr(SOSG);
		EditOTLtek29->Text = IntToStr(DOPSG);
		EditOTLtek30->Text = IntToStr(PAR_SG);
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
    EditNastrIn0 -> Text =FloatToStrF(pow(10,(float)nasmod[0]/1000.0*1.667-9.333),ffExponent,3,8);
    EditNastrIn1 -> Text =FloatToStrF(pow(10,(float)nasmod[1]/1000.0*1.667-9.333),ffExponent,3,8);
    EditNastrIn2 -> Text =FloatToStrF((float)nasmod[2]/4095.0*RRG3_MAX,ffFixed,5,1);
    EditNastrIn3 -> Text =( nasmod[3] ? "��" : "���" );
    EditNastrIn4 -> Text =( nasmod[4] ? "��" : "���" );
    EditNastrIn5 -> Text =FloatToStrF((float)nasmod[5]/10.0,ffFixed,5,0);
    EditNastrIn6 -> Text =FloatToStrF((float)nasmod[6]/10.0,ffFixed,5,0);
    EditNastrIn7 -> Text =FloatToStrF((float)nasmod[10]/256.0*750.0,ffFixed,5,0);
    EditNastrIn8 -> Text =FloatToStrF((float)nasmod[8],ffFixed,5,0);
    EditNastrIn9 -> Text =FloatToStrF((float)nasmod[9]/4095.0*1023.0 + 0.05,ffFixed,5,1);
    EditNastrIn10 -> Text =FloatToStrF( 1000.0 / float(nasmod[14]), ffFixed, 6, 0 );
    EditNastrIn11 -> Text =FloatToStrF(((float)nasmod[16]/200.0), ffFixed, 5, 0 );
    EditNastrIn12 -> Text =FloatToStrF(((float)nasmod[17]/200.0), ffFixed, 5, 0 );
    EditNastrIn13 -> Text =( nasmod[7] ? "��" : "���" );
    EditNastrIn14 -> Text =( nasmod[15] ? "��" : "���" );
    EditNastrIn15 -> Text =( nasmod[18] ? "��" : "���" );
    EditNastrIn16 -> Text =FloatToStrF((float)nasmod[19]/255.0*383.0,ffFixed,5,0);
    EditNastrIn17 -> Text =FloatToStrF((float)nasmod[20]/255.0*640.0,ffFixed,5,0);


}

//---------------------------------------------------------------------------
//--������������� �������� ����������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrDaClick(TObject *Sender)
{

    // ������ ������
    PanelParNastr -> Visible = false;
    // ���������� ������� ��������������� ������� ������
    nasmod[0]=int(((log10(StrToFloat(EditNastrTo0->Text)))*0.6+5.6)*1000.0);
    // ���������� ������� ��������������� ������� �����
    nasmod[1]=int(((log10(StrToFloat(EditNastrTo1->Text)))*0.6+5.6)*1000.0);
    // ������ ���3 (������ He ��� ��������)
    nasmod[2]=StrToFloat(EditNastrTo2->Text)/RRG3_MAX*4095.0;
    // ������� ���� � ����������� ���������������� ��������?
    EditNastrTo3 -> Text == "��" ? nasmod[3] = 1 : nasmod[3] = 0;
    // �������� ������?
    EditNastrTo4 -> Text == "��" ? nasmod[4] = 1 : nasmod[4] = 0;
    // ����������� ������� ��������������� ������
    nasmod[5]=StrToFloat(EditNastrTo5->Text)*10;
    // ����������� ������� �/�
    nasmod[6]=StrToFloat(EditNastrTo6->Text)*10;
    // ������������ ���������� �� ����������
    nasmod[10]=int(StrToFloat(EditNastrTo7->Text)*255.0/750.0);
    // ������������ �������� ���������
    nasmod[8]=StrToInt(EditNastrTo8 -> Text);
    // ������������ ����� ����� ��������� ����������
    nasmod[9]=int(StrToFloat(EditNastrTo9->Text)*4095.0/1023.0);
    // ���������� ����������� ������������ ��� �/�
    nasmod[14]=(unsigned int)(1000.0/StrToFloat(EditNastrTo10->Text));
    // ���� ������� ����������������� �� ������� ���������
    nasmod[16]=StrToFloat(EditNastrTo11->Text)*200;
    // ���� ������� ����������������� ��� ��������� �������������� �������������
    nasmod[17]=StrToFloat(EditNastrTo12->Text)*200;
    // ������� ���� � ���������� ��� ��������� �������������� �������������
    EditNastrTo13 -> Text == "��" ? nasmod[7] = 1 : nasmod[7] = 0;
    // ������������ ���� � ��������� ?
    EditNastrTo14 -> Text == "��" ? nasmod[15] = 1 : nasmod[15] = 0;
    // ������ ��� ����-������������ ?
    EditNastrTo15 -> Text == "��" ? nasmod[18] = 1 : nasmod[18] = 0;
    // ������� ���������� ���������� ���
    nasmod[19]=int(StrToFloat(EditNastrTo16->Text)*255.0/383.0);
    // ������� ���� ���������� ���
    nasmod[20]=int(StrToFloat(EditNastrTo17->Text)*255.0/640.0);




    // ��������� �������� � ������
    MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time -> Caption + " : �������� �������� ������������ ������� : ");
    if ( EditNastrTo0 -> Text != EditNastrIn0 -> Text )
        MemoStat -> Lines -> Add( "���������� ������� ��������������� ������� ������: " + EditNastrIn0 -> Text + " -> " + EditNastrTo0 -> Text );
    if ( EditNastrTo1 -> Text != EditNastrIn1 -> Text )
        MemoStat -> Lines -> Add( "���������� ������� ��������������� ������� �����: " + EditNastrIn1 -> Text + " -> " + EditNastrTo1 -> Text );
    if ( EditNastrTo2 -> Text != EditNastrIn2 -> Text )
        MemoStat -> Lines -> Add( "������ ���3 (������ He ��� ��������): " + EditNastrIn2 -> Text + " -> " + EditNastrTo2 -> Text );
    if ( EditNastrTo3 -> Text != EditNastrIn3 -> Text )
        MemoStat -> Lines -> Add( "������� ���� � ����������� ���������������� ��������?: " + EditNastrIn3 -> Text + " -> " + EditNastrTo3 -> Text );
    if ( EditNastrTo4 -> Text != EditNastrIn4 -> Text )
        MemoStat -> Lines -> Add( "�������� ������?: " + EditNastrIn4 -> Text + " -> " + EditNastrTo4 -> Text );
    if ( EditNastrTo5 -> Text != EditNastrIn5 -> Text )
        MemoStat -> Lines -> Add( "����������� ������� ��������������� ������: " + EditNastrIn5 -> Text + " -> " + EditNastrTo5 -> Text );
    if ( EditNastrTo6 -> Text != EditNastrIn6 -> Text )
        MemoStat -> Lines -> Add( "����������� ������� �/�: " + EditNastrIn6 -> Text + " -> " + EditNastrTo6 -> Text );
    if ( EditNastrTo7 -> Text != EditNastrIn7 -> Text )
        MemoStat -> Lines -> Add( "������������ ���������� �� ����������: " + EditNastrIn7 -> Text + " -> " + EditNastrTo7 -> Text );
    if ( EditNastrTo8 -> Text != EditNastrIn8 -> Text )
        MemoStat -> Lines -> Add( "������������ �������� ���������: " + EditNastrIn8 -> Text + " -> " + EditNastrTo8 -> Text );
    if ( EditNastrTo9 -> Text != EditNastrIn9 -> Text )
        MemoStat -> Lines -> Add( "������������ ����� ����� ��������� ����������: " + EditNastrIn9 -> Text + " -> " + EditNastrTo9 -> Text );
    if ( EditNastrTo10 -> Text != EditNastrIn10 -> Text )
        MemoStat -> Lines -> Add( "���������� ����������� ������������ ��� �/�: " + EditNastrIn10 -> Text + " -> " + EditNastrTo10 -> Text );
    if ( EditNastrTo11 -> Text != EditNastrIn11 -> Text )
        MemoStat -> Lines -> Add( "���� ������� ����������������� �� ������� ���������: " + EditNastrIn11 -> Text + " -> " + EditNastrTo11 -> Text );
    if ( EditNastrTo12 -> Text != EditNastrIn12 -> Text )
        MemoStat -> Lines -> Add( "���� ������� ����������������� ��� ��������� �������������� �������������: " + EditNastrIn12 -> Text + " -> " + EditNastrTo12 -> Text );
    if ( EditNastrTo13 -> Text != EditNastrIn13 -> Text )
        MemoStat -> Lines -> Add( "������� ���� � ���������� ��� ��������� �������������� �������������: " + EditNastrIn13 -> Text + " -> " + EditNastrTo13 -> Text );
    if ( EditNastrTo14 -> Text != EditNastrIn14 -> Text )
        MemoStat -> Lines -> Add( "������������ ���� � ��������� ?: " + EditNastrIn14 -> Text + " -> " + EditNastrTo14 -> Text );
    if ( EditNastrTo15 -> Text != EditNastrIn15 -> Text )
        MemoStat -> Lines -> Add( "������ ��� ����-������������ ?: " + EditNastrIn15 -> Text + " -> " + EditNastrTo15 -> Text );
    if ( EditNastrTo16 -> Text != EditNastrIn16 -> Text )
        MemoStat -> Lines -> Add( "������� ���������� ���������� ���: " + EditNastrIn16 -> Text + " -> " + EditNastrTo16 -> Text );
    if ( EditNastrTo17 -> Text != EditNastrIn17 -> Text )
        MemoStat -> Lines -> Add( "������� ���� ���������� ���: " + EditNastrIn17 -> Text + " -> " + EditNastrTo17 -> Text );



    // ������������ ����� ������ ���������� ����

    EditNastrTo0 -> Color = clWhite;
    EditNastrTo1 -> Color = clWhite;
    EditNastrTo2 -> Color = clWhite;
    EditNastrTo3 -> Color = clWhite;
    EditNastrTo4 -> Color = clWhite;
    EditNastrTo5 -> Color = clWhite;
    EditNastrTo6 -> Color = clWhite;
    EditNastrTo7 -> Color = clWhite;
    EditNastrTo8 -> Color = clWhite;
    EditNastrTo9 -> Color = clWhite;
    EditNastrTo10 -> Color = clWhite;
    EditNastrTo11 -> Color = clWhite;
    EditNastrTo12 -> Color = clWhite;
    EditNastrTo13 -> Color = clWhite;
    EditNastrTo14 -> Color = clWhite;
    EditNastrTo15 -> Color = clWhite;
    EditNastrTo16 -> Color = clWhite;
    EditNastrTo17 -> Color = clWhite;






    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> Clear();

    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo0->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo1->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo2->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo3->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo4->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo5->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo6->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo7->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo8->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo9->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo10->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo11->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo12->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo13->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo14->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo15->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo16->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo17->Text);

    MemoNasmod -> Lines -> SaveToFile("Nasmod\\Nasmod.txt");




    VisualNasmod();
    VisualParA();

    VisualParR();
    // ������������ ����������� ����������
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
    Comport[4]->Dev_Timer++;
    Comport[5]->Dev_Timer++;
}
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

    if((shr[3]==0&&shr[4]==0)||PR_NALADKA) return;    // �� ������

    // �����
    graphTemp = Label_Time -> Caption + ";";

    // ����������� ������� �/�
    if(shr[31])
    {
        graphTemp = graphTemp + FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1) + ";";
        serTemp[0] -> AddY(float(TEK_TEMP2)/10.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[0] -> AddY(0.0,Label_Time -> Caption);
    }

    // ���1
    if(shr[20])
    {
        graphTemp = graphTemp + FloatToStrF((float)aik[6]*RRG1_MAX/4095.0,  ffFixed, 6, 1) + ";";
        serTemp[1] -> AddY(((float)aik[6]*RRG1_MAX/4095.0), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[1] -> AddY(0.0,Label_Time -> Caption);
    }

    // ���2
    if(shr[21])
    {
        graphTemp = graphTemp + FloatToStrF((float)aik[7]*RRG2_MAX/4095.0,  ffFixed, 6, 1) + ";";
        serTemp[2] -> AddY(((float)aik[7]*RRG2_MAX/4095.0), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[2] -> AddY(0.0,Label_Time -> Caption);
    }

    // ���3
    if(shr[22])
    {
        graphTemp = graphTemp + FloatToStrF((float)aik[8]*RRG3_MAX/4095.0,  ffFixed, 6, 1) + ";";
        serTemp[3] -> AddY(((float)aik[8]*RRG3_MAX/4095.0), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[3] -> AddY(0.0,Label_Time -> Caption);
    }


    // ���. �������� ��� ��
    if(shr[28])
    {

        graphTemp = graphTemp + FloatToStrF((float)aik[10]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0) + ";";
        serTemp[4] -> AddY((float)aik[10]*CESAR_MAX_PD/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[4] -> AddY(0.0,Label_Time -> Caption);
    }
    // ���. �������� ��� ��
    if(shr[28])
    {

        graphTemp = graphTemp + FloatToStrF((float)aik[11]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0) + ";";
        serTemp[5] -> AddY((float)aik[11]*CESAR_MAX_PD/4095.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[5] -> AddY(0.0,Label_Time -> Caption);
    }
    // �������� �
    if(shr[33])
    {
        graphTemp = graphTemp + FloatToStrF((float)OTVET_BMH1[6]*6144.0/1024.0,ffFixed,6,0) + ";";
        serTemp[6] -> AddY((float)OTVET_BMH1[6]*6144.0/1024.0, Label_Time->Caption );

    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[6] -> AddY(0.0,Label_Time -> Caption);
    }


    // ����
    if(shr[27])
    {                                              /*FloatToStrF((float)D_D2/10000*DAVL_MAX,ffFixed,5,1);      */

        graphTemp = graphTemp + FloatToStrF(((float)D_D2-1000)/8000*DAVL_MAX,ffFixed,5,1) + ";";
        serTemp[7] -> AddY(((float)D_D2-1000)/8000*DAVL_MAX, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[7] -> AddY(0.0,Label_Time -> Caption);
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
//--���������� ��������� ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ImgFkKamClick(TObject *Sender)
{
    // ����������� � ����� �������� ������ �����, ���� � �������������� �� �����
    if ( PCMain -> ActivePage == TSWork ) return;
    // ������������ ������                                      
    if  (
                (((TImage*)Sender)->Name) == "Fvn_Shl" ||
                (((TImage*)Sender)->Name) == "fvn_kam" ||
                (((TImage*)Sender)->Name) == "tmn"     ||
                (((TImage*)Sender)->Name) == "fvn_otk" ||
                (((TImage*)Sender)->Name) == "Tmn_otk"
        )
    {
        BtnDeviceOn -> Caption = "���.";
        BtnDeviceOff -> Caption = "����.";
    }

    else    if  (
                (((TImage*)Sender)->Name) == "pp"
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

    PnlDevice -> Top = ((TImage*)Sender)->Top - 90;
    PnlDevice -> Left = ((TImage*)Sender)->Left - 110;
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
    if ( ((TButton*)Sender) -> Parent -> Hint == "kl1" )       SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x100);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x200);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl3" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x1000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl4" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x2000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl5" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x800);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl6" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x04);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_nap1" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_nap2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x01);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_tmn" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x300);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_shl_m" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x200);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fk_op" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x80);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "vk_shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x40);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fvn_Shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x20);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fvn_kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x10);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fvn_otk" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x40);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Tmn_otk" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x80);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "pp" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 0, 0x8000);
            SetOut(1, 0, 0x4000);
        }
        else
        {
            SetOut(1, 0, 0x8000);
            SetOut(0, 0, 0x4000);
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
    #define SHR_VALUE_COUNT 11
    // ������� ���������� �������� ����� (���� �����)
    unsigned char SHRValue[SHR_VALUE_COUNT] = {8,5,3,2,7,1,4,6,10,19,9};
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
                case 19: Form1 -> EditSHRName -> Text = SHR19Names[shr[SHRValue[i]]]; break;
            }
            // � ������ 8 ��� 4
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==4))
            {
                if(shr[5]==26)
                    Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
                else if(shr[5]==35)
                    Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
                else
                    Form1 -> EditSHRName -> Text = SHR5Names[shr[5]];
            }
            // � ������ 6 ��� 13
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==13))
            {
                if(shr[10]==14)
                    Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
                else
                    Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
            }
            // � ������ 8 ��� 5
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==5))
                Form1 -> EditSHRName -> Text = SHR7Names[shr[7]];
            // � ������ 2 ��� 2
            if((SHRValue[i]==2)&&(shr[SHRValue[i]]==2))
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            // � ������ 3 ��� 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==2))
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            // � ������ 3 ��� 8
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==8))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            // � ������ 3 ��� 19
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==19))
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            // � ������ 3 ��� 27
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==27))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            // � ������ 5 ��� 26
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==26))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            // � ������ 5 ��� 35
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==35))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            // � ������ 6 ��� 11
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==11))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            // � ������ 10 ��� 14
            if((SHRValue[i]==10)&&(shr[SHRValue[i]]==14))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];

            // � ������ 2 ��� 8 ������� �����
            if(shr[2]==8)
            {
                TempStr = SHR2Names[shr[2]]+IntToStr(par[N_ST][12]-CT_2);
                Form1 -> EditSHRName -> Text = TempStr;
            }

            // � ������ 3 ��� 5 ������� �����
            if(shr[3]==5)
            {
                TempStr = SHR3Names[shr[3]]+IntToStr(T_NAPUSK-CT_3);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 4 ������� �����
            if(shr[4]==4)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(T_SBROSHE-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 5 ������� �����
            if(shr[4]==5)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 9 ������� �����
            if(shr[4]==9)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 17 ������� �����
            if(shr[4]==17)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 4 ��� 36 ������� �����
            if(shr[4]==36)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 5 ��� 32 ������� �����
            if(shr[5]==32)
            {
                TempStr = SHR5Names[shr[5]]+IntToStr(T_NAPUSK-CT_5);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 6 ��� 8 ������� �����
            if(shr[6]==8)
            {
                TempStr = SHR6Names[shr[6]]+IntToStr(T_NAPUSK-CT_6);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // � ������ 10 ��� 18 ������� �����
            if(shr[10]==18)
            {
                TempStr = SHR10Names[shr[10]]+IntToStr(T_NAPUSK-CT_10);
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

    if(shr[3]==6)
    {
        APanel_String1 -> Caption = "������ ��������.";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "��������-��������";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Caption = "���������";
        APanel_String3 -> Visible = true;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[3]==7)
    {
        APanel_String1 -> Caption = "������ �������.";
        APanel_String1 -> Visible = true;
         APanel_String2 -> Caption = "���� � ���������?";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[4]==45)
    {
        APanel_String1 -> Caption = "��������� �� ���������� ��������.";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "����������� ��������� �������. ���������.";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="��������� ��������";
        APanel_NetBut -> Caption="�������� ��������";
        if(PCMain -> ActivePage == TSNalad)
            APanel -> Visible = false;
        else
            APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[5]==17)
    {
        APanel_String1 -> Caption = "���� � ���������?";
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

    else if(shr[5]==33)
    {
        APanel_String1 -> Caption = "������ ��������.";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "��������� ��������.";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[5]==34)
    {
        APanel_String1 -> Caption = "������ �������?";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }

    else if(shr[6]==4)
    {
        APanel_String1 -> Caption = "���� �������� � �����?";
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
    else if(shr[6]==9)
    {
        APanel_String1 -> Caption = "������ ��������.";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "��������� ��������.";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="��";
        APanel_NetBut -> Caption="���";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[6]==10)
    {
        APanel_String1 -> Caption = "�������� ����?";
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
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            //���� ���������� ���
            if(!(zin[0]&0x02)) { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
            //���� ���������� �����
            if(!(zin[0]&0x04)) { ListBoxCondition -> Items -> Add("��� ���������� �����"); }
            //���� ����� � �1
            if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
            //���� ����� � �3
            if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
            //���� ����� � �4
            if((diagnS[0]&0x08)&&(nasmod[18]==0))   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
            //���� ����� � �5
            if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
            //���� ����� � �6
            if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("��� ����� � �6"); }
            //���� ����� � ����������� ������� �/�
            if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� ������� �/�"); }
            //���� ����� � ����������� �������� �/�
            if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � �������� �/�"); }
            //���� ����� � ����������� �������� ��������
            if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �������� ��������"); }
            //���� ����� � ����������
            if((diagnS[2]&0x40)&&(nasmod[4]==1))   { ListBoxCondition -> Items -> Add("��� ����� � ����������"); }
            //��� ����������� �� ������ ������ ����� R_31
            for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((shr[i])&&(i!=31)) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;

        case 2:
        {
            LblRejim -> Caption = "����������";
            // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            //���� ���������� ����������
            if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� ����������"); }
            //���� ���������� ���
            if(!(zin[0]&0x02)) { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
            //���� ���������� �����
            if(!(zin[0]&0x04)) { ListBoxCondition -> Items -> Add("��� ���������� �����"); }
            //���� ����� � �1
            if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
            //���� ����� � �3
            if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
            //���� ����� � �4
            if((diagnS[0]&0x08)&&(nasmod[18]==0))   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
            //���� ����� � �5
            if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
            //���� ����� � �6
            if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("��� ����� � �6"); }
            //���� ����� � ����������� ������� �/�
            if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� ������� �/�"); }
            //���� ����� � ����������� �������� �/�
            if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� �/�"); }
            //���� ����� � ����������� �������� ��������
            if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� ��������"); }
            //���� ����� � ����������
            if(diagnS[2]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � ����������"); }
            //���� ����� � �� ������
            if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �� ������"); }
            //��� ����������� �� ������ ������ ����� R_31
            for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((shr[i])&&(i!=31)) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;

		case 3:
		{   LblRejim -> Caption = "������� ���� (��)";
            // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            //���� ���������� ����������
            if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� ����������"); }
            //���� ���������� ���
            if(!(zin[0]&0x02)) { ListBoxCondition -> Items -> Add("��� ���������� ���"); }
            //���� ���������� �����
            if(!(zin[0]&0x04)) { ListBoxCondition -> Items -> Add("��� ���������� �����"); }
            //���� ����� � �1
            if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
            //���� ����� � �3
            if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
            //���� ����� � �4
            if((diagnS[0]&0x08)&&(nasmod[18]==0))   { ListBoxCondition -> Items -> Add("��� ����� � �4"); }
            //���� ����� � �5
            if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � �5"); }
            //���� ����� � �6
            if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("��� ����� � �6"); }
            //���� ����� � ����������� ������� �/�
            if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� ������� �/�"); }
            //���� ����� � ����������� �������� �/�
            if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� �/�"); }
            //���� ����� � ����������� �������� ��������
            if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� ��������"); }
            //���� ����� � ����������
            if(diagnS[2]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � ����������"); }
            //���� ����� � �� ������
            if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �� ������"); }
            //���� ����� � �� ����-������������
            if((diagnS[2]&0x80)&&(nasmod[18]==0))   { ListBoxCondition -> Items -> Add("��� ����� � �� ����-������������"); }
            //��� ����������� �� ������ ������ ����� R_31
            for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((shr[i])&&(i!=31)) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;

        case 5:  
        {
             LblRejim -> Caption = "����� ��";
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if(!shr[2]&&!shr[3])  { ListBoxCondition -> Items -> Add("�� �������� �� ��� ����������"); }
		    // �� ������� ����� 5
		    if(shr[5])  { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
        }; break;

        case 6:   
		{   LblRejim -> Caption = "���� �������";
            // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
            // ���� ����� � �3
		    if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
            // ���� ����� � �6
		    if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("��� ����� � �6"); }
            //���� ����� � ����������� ������� �/�
            if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� ������� �/�"); }
            //���� ����� � ����������� �������� �/�
            if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� �/�"); }
            //���� ����� � ����������� �������� ��������
            if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� ��������"); }
            //��� ����������� �� ������ ������
            for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if(shr[i]) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;

       case 7:
		{   LblRejim -> Caption = "���������� ���������";
            // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            //���� ����� � ����������� ������� �/�
            if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� ������� �/�"); }
            //���� ����� � ����������� �������� �/�
            if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� �/�"); }
            //���� ����� � ����������� �������� ��������
            if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� ��������"); }
            //��� ����������� �� ������ ������ ����� R_1 R_27 R_39 R_29 R_31
            for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((shr[i])&&(i!=1)&&(i!=27)&&(i!=39)&&(i!=29)&&(i!=31))
                ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
		}; break;

         case 9:
		{   LblRejim -> Caption = "�������� ������������ ����";  
            // �� ������
            if((zin[1]&0xC00)!=0x400) { ListBoxCondition -> Items -> Add("������� ������ �� ������"); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
            // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            //���� ����� � ����������� ������� �/�
            if(diagnS[2]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� ������� �/�"); }
            //���� ����� � ����������� �������� �/�
            if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� �/�"); }
            //���� ����� � ����������� �������� ��������
            if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � ����������� �������� ��������"); }
            //��� ����������� �� ������ ������
            for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if(shr[i])ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
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
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ���� ����� � �1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("��� ����� � �1"); }
            // ���� ����� � �3
		    if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("��� ����� � �3"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 10
		    if(shr[10]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[10]); }
		}; break;

        case 11:
		{   LblRejim -> Caption = "������� ������� ������";
            // ���� �������� � ���������� �� zin
		    if(!(zin[0]&0x40)) { ListBoxCondition -> Items -> Add("��� �������� � ����������"); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 11
		    if(shr[11]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[11]); }
		}; break;

        case 12:
        {   LblRejim -> Caption = "����������� � HOME";
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
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
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
        }; break;

        case 13:
        {   LblRejim -> Caption = "����������� � ������";
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
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
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������
            if((zin[1]&0xC00)!=0x400) { ListBoxCondition -> Items -> Add("������� ������ �� ������"); }
            // ������ �/� � HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("������ �/� �� � HOME"); }
            // �������� �/� � HOME
		    if(!(zin[3]&0x02)) { ListBoxCondition -> Items -> Add("�������� �/� �� � HOME"); }
        }; break;

		case 101:
		{   LblRejim -> Caption = "���� ����������";  // STOP ���������� -- ������� ������� ������
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////
            // �� ������� ����� 1
            if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
            if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            
            // �� ������� ����� 19
            if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;

        case 14:
        {   LblRejim -> Caption = "������ �/� � HOME";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }

        }; break;
        case 15:
        {   LblRejim -> Caption = "������ �/� � �����";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }

        }; break;
        case 16:
        {   LblRejim -> Caption = "�������� �/� ���.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
            // ������ �/� � HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("������ �/� �� � HOME"); }
        }; break;

        case 17:
        {   LblRejim -> Caption = "�������� �/� ����.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
        }; break;

        case 18:
        {   LblRejim -> Caption = "������ �/� � ������� ��������� (���������)";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
            // �������� �/� � HOME
		    if(!(zin[3]&0x10)) { ListBoxCondition -> Items -> Add("�������� �/� �� � HOME"); }
        }; break;
        case 118:
        {   LblRejim -> Caption = "������ �/� � ������� ��������� (��������� �������������)";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
            // �������� �/� � HOME
		    if(!(zin[3]&0x10)) { ListBoxCondition -> Items -> Add("�������� �/� �� � HOME"); }
        }; break;

        case 20:
        {   LblRejim -> Caption = "���1 ���.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 20
		    if(shr[20]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[20]); }
        }; break;

        case 120:
        {   LblRejim -> Caption = "���1 ����.";
            // ������� ����� 20
		    if(!(shr[20])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[20]); }
            // �� ������� ����� 3 ��� ������� � �������
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
        }; break;

        case 21:
        {   LblRejim -> Caption = "���2 ���.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 21
		    if(shr[21]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[21]); }
        }; break;

        case 121:
        {   LblRejim -> Caption = "���2 ����.";
            // ������� ����� 21
		    if(!(shr[21])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[21]); }
            // �� ������� ����� 3 ��� ������� � �������
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
        }; break;

        case 22:
        {   LblRejim -> Caption = "���3 ���. � ������";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;

        case 222:
        {   LblRejim -> Caption = "���3 ���. � �����";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;

        case 122:
        {   LblRejim -> Caption = "���3 ����.";
            // ������� ����� 22
		    if(!(shr[22])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[22]); }
            // �� ������� ����� 3 ��� ������� � �������
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
        }; break;

        case 25:
        {   LblRejim -> Caption = "������� �� ������";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 25
		    if(shr[25]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[25]); }
        }; break;

        case 26:
        {   LblRejim -> Caption = "������� �� ������";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 26
		    if(shr[26]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[26]); }
        }; break;

        case 27:
        {   LblRejim -> Caption = "�� ������ ��������������";
            // ���� ����� � ��
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("��� ����� � �� ������"); }
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 27
		    if(shr[27]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[27]); }
        }; break;

        case 28:
        {   LblRejim -> Caption = "��� �/� ���.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 28
		    if(shr[28]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[28]); }
        }; break;

        case 128:
        {   LblRejim -> Caption = "��� �/� ����.";
            // �� ������� ����� 3 ��� ������� � �������
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
            // �� ������� ����� 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]); }
            // �� ������� ����� 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]); }
            // ������� ����� 28
		    if(!(shr[28])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[28]); }
        }; break;

        case 29:
        {   LblRejim -> Caption = "������ ������ ���.";
            // �� ������� ����� 29
		    if(shr[29]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]); }
            // ���� ����� � ��������
		    if(diagnS[2]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � ��������"); }
            //���� ����������
            if(!(nasmod[4])){ ListBoxCondition -> Items -> Add("��� ���������� �������" ); }
            //���� ����������
            if(!(nasmod[5])){ ListBoxCondition -> Items -> Add("�� ���������� ����������� �������" ); }

        }; break;
        case 30:
        {   LblRejim -> Caption = "������ ������ ����.";
            // ������� ����� 29
		    if(!(shr[29])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[29]); }

        }; break;
        case 31:
        {   LblRejim -> Caption = "������ �/� ���.";
            // ������� ����� 31
		    if(shr[31]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[31]); }
            // ���� ����� � ��������
		    if(diagnS[2]&0x10)   { ListBoxCondition -> Items -> Add("��� ����� � ��������"); }
            //���� ����������
            if(!(nasmod[4])){ ListBoxCondition -> Items -> Add("��� ���������� �������" ); }
            //���� ����������
            if(!(nasmod[6])){ ListBoxCondition -> Items -> Add("�� ���������� ����������� ������� �/�");  }
        }; break;
        case 32:
        {   LblRejim -> Caption = "������ �/� ����.";
            // �� ������� ����� 31
		    if(!(shr[31])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[31]); }
        }; break;
        case 33:
        {   LblRejim -> Caption = "��� ���.";
            //���� �������� ����������
            if(!(zin[2]&0x200)) { ListBoxCondition -> Items -> Add("��� �������� ����������"); }
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 33
		    if(shr[33]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[33]); }
            // �� ������� ����� 34
		    if(shr[34]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[34]); }
            //���� ���������� ����������
            if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("��� ���������� ����������"); }
            // ���� ����� � ���
		    if(diagnS[2]&0x20)   { ListBoxCondition -> Items -> Add("��� ����� � ���"); }

        }; break;
        case 34:
        {   LblRejim -> Caption = "��� ����.";
            // ������� ����� 33
		    if(!(shr[33])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[33]); }
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 34
		    if(shr[34]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[34]); }
        }; break;
        case 35:
        {   LblRejim -> Caption = "������� �������� � HOME";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
            // ������ �/� � HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("������ �/� �� � HOME"); }
        }; break;
        case 36:
        {   LblRejim -> Caption = "������� �������� �� ����";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[12]); }
            // �� ������� ����� 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[13]); }
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
            // ��� � HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("����������� �� � HOME"); }
            // ������ �/� � HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("������ �/� �� � HOME"); }
        }; break;

        case 37:
        {   LblRejim -> Caption = "������� �� ����-������������";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[37]); }
        }; break;

        case 38:
        {   LblRejim -> Caption = "������� �� ����-������������";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[38]); }
        }; break;

        case 39:
        {   LblRejim -> Caption = "�� ����-������������ �� ����";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 39
		    if(shr[39]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[39]); }
            // ���� ����� � �� ����-������������
		    if(diagnS[2]&0x80)   { ListBoxCondition -> Items -> Add("��� ����� � �� ����-������������"); }
        }; break;

        case 40:
        {   LblRejim -> Caption = "���������� ������������� (1 �����)";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)&&(shr[4]!=45)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
        }; break;

        case 41:
        {   LblRejim -> Caption = "���������� ������������� (��������� �����)";
        // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)&&(shr[4]!=45)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[14]); }
            // �� ������� ����� 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[15]); }
            // �� ������� ����� 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[16]); }
            // �� ������� ����� 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[17]); }
            // �� ������� ����� 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[18]); }
            // �� ������� ����� 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[35]); }
            // �� ������� ����� 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[36]); }
            // �� ������� ����� 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]); }
            // �� ������� ����� 41
		    if(shr[41]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]); }
        }; break;

        case 43:
        {   LblRejim -> Caption = "����������� ����������� ����� ���.";
            //���� ����������
            if(nasmod[18]!=0) { ListBoxCondition -> Items -> Add("��������� ������ �� ����-������������ "); }
            //��-�� ������
            if((zin[2]&0xC0)!=0x40){ ListBoxCondition -> Items -> Add("��-�� �� ������"); }
            //���� ������ � ����-������������
            if(!(zin[4]&0x02))     { ListBoxCondition -> Items -> Add("��� ������� � ����-������������"); }
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;
        case 44:
        {   LblRejim -> Caption = "����������� ����������� ����� ����.";
            // ������� ����� 43
		    if(!(shr[43])) { ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[43]); }
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;
        case 45:
        {   LblRejim -> Caption = "�������� ���� ���.";

        }; break;
        case 46:
        {   LblRejim -> Caption = "�������� ���� ����.";

        }; break;
        case 47:
        {   LblRejim -> Caption = "�������� ���������� ���.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
            // �� ������� ����� 47
		    if(shr[47]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[47]); }
        }; break;
        case 48:
        {   LblRejim -> Caption = "�������� ���������� ����.";
            // �� ������� ����� 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]); }
            // �� ������� ����� 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // ���������� ����� ��������
            if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("���������� ����� ��������"); }
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;
        case 147:
        {   LblRejim -> Caption = "���� �������� ����������";

        }; break;
        case 49:
        {   LblRejim -> Caption = "��� ������ ���.";
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;
        case 50:
        {   LblRejim -> Caption = "��� ������ ����.";
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
            // �� ������� ����� 19
		    if(shr[19]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[19]); }
        }; break;
        case 213:
        {   LblRejim -> Caption = "����� ������ ����������";

        }; break;
        
        case 100:
        {   LblRejim -> Caption = "����� �����";
            // �� ������� ����� 1
		    if(shr[2]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]); }
            // �� ������� ����� 1
		    if(shr[3]) { ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]); }

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

	// ���������� �������
	Save_Stat();
    SaveGasData();
    MemoRes -> Lines -> Clear();
    MemoRes -> Lines -> Add(FloatToStr(magnRes1));
    MemoRes -> Lines -> SaveToFile("Res\\Res.txt");
	// ���������� ����
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
    if(Comport[4]->State)
    {
        Comport[4]->Port.Close();
        Comport[4]->State = 0;
    }
    if(Comport[5]->State)
    {
        Comport[5]->Port.Close();
        Comport[5]->State = 0;
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

    // ��������� �������� � ������
    MemoStat -> Lines -> Add( "<<< " + Label_Time -> Caption + " | ��������� ��������� >>>");

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
void Timer_Com1()
{
// return;

try
{
    if(Comport[0]->port_err)
    {
        if(Comport[0]->port_ct > 30)
        {
            if(Comport[0]->Port.Open(Comport[0]->PortName.c_str(),Comport[0]->B_Rate,Data8Bit,Comport[0]->P_Rate,OneStopBit))
            {
                Comport[0]->State = 1;
                Comport[0]->BTN_reset->Caption = "���� �����";
                Comport[0]->port_err = 0;
            }
            else
            {
                Comport[0]->port_ct = 0;
            }
        }
        else Comport[0]->port_ct++;
    }

    Comport[0]->CB_status->Checked = Comport[0]->State;
	Comport[0]->CB_nal->Checked = Comport[0]->Pr_nal;
	Comport[0]->LBL_otl->Visible = (!(Comport[0]->State) || (Comport[0]->Pr_nal));

	// ��������� ��������� � ���� �������
	if(!(Comport[0]->State)||!ust_ready) return;

	// ����������� ������/��������
	Comport[0]->RB_prd->Checked = !(Comport[0]->DevState);
	Comport[0]->RB_prm->Checked = Comport[0]->DevState;

    // ���� ������ �������
	    if((DZaslVAT[0]->RCom)&&(!(Comport[0]->DevState))) Comport[0]->PortTask |= 0x100;
	    if(!(Comport[0]->PortTask)&&!(Comport[0]->Pr_nal)) Comport[0]->PortTask |= 0x01; // ��������� �������������� �������


        if((Comport[0]->PortTask) & 0x100)
	    {
		    Comport[0]->DevErr = DZaslVAT[0]->DZaslVAT_Manage(Comport[0]->DevState,1);
		    if((Comport[0]->DevState) > 1)
		    {
			    (Comport[0]->DevErr) ? diagnS[DZaslVAT[0]->diagnS_byte] |= DZaslVAT[0]->diagnS_mask : diagnS[DZaslVAT[0]->diagnS_byte] &= (~DZaslVAT[0]->diagnS_mask);
			    (Comport[0]->PortTask) &= (~0x100);
			    DZaslVAT[0]->RCom = 0;
			    Comport[0]->DevState = 0;
		    }
		    return;
	    }
	    else if((Comport[0]->PortTask)&0x01)
	    {
		    Comport[0]->DevErr = DZaslVAT[0]->DZaslVAT_Manage(Comport[0]->DevState,0);
		    if((Comport[0]->DevState)>1)
		    {
			    (Comport[0]->DevErr) ? diagnS[DZaslVAT[0]->diagnS_byte] |= DZaslVAT[0]->diagnS_mask : diagnS[DZaslVAT[0]->diagnS_byte] &= (~DZaslVAT[0]->diagnS_mask);
			    (Comport[0]->PortTask) &= (~0x01);
			    Comport[0]->DevState = 0;
		    }
		    return;
	    }
}
catch (Exception &exception)
{
    if(!(Comport[0]->port_err))
    {
        Comport[0]->port_err = 1;
        if(Comport[0]->State)
        {
            Comport[0]->State = 0;
            Comport[0]->Port.Close();
            Comport[0]->BTN_reset->Caption = "���� �����";
            Comport[0]->port_ct = 0;
            }
            // ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
    }
}
//---------------------------------------------------------------------------
void Timer_Com2()
{
// return;

try
{
    if(Comport[1]->port_err)
    {
        if(Comport[1]->port_ct > 30)
        {
            if(Comport[1]->Port.Open(Comport[1]->PortName.c_str(),Comport[1]->B_Rate,Data8Bit,Comport[1]->P_Rate,OneStopBit))
            {
                Comport[1]->State = 1;
                Comport[1]->BTN_reset->Caption = "���� �����";
                Comport[1]->port_err = 0;
            }
            else
            {
                Comport[1]->port_ct = 0;
            }
        }
        else Comport[1]->port_ct++;
    }

    Comport[1]->CB_status->Checked = Comport[1]->State;
	Comport[1]->CB_nal->Checked = Comport[1]->Pr_nal;
	Comport[1]->LBL_otl->Visible = (!(Comport[1]->State) || (Comport[1]->Pr_nal));

	// ��������� ��������� � ���� �������
	if(!(Comport[1]->State)||!ust_ready) return;

	// ����������� ������/��������
	Comport[1]->RB_prd->Checked = !(Comport[1]->DevState);
	Comport[1]->RB_prm->Checked = Comport[1]->DevState;

    // ���� ������ �������
	    if((DZaslVAT[1]->RCom)&&(!(Comport[1]->DevState))) Comport[1]->PortTask |= 0x100;
	    if(!(Comport[1]->PortTask)&&!(Comport[1]->Pr_nal)) Comport[1]->PortTask |= 0x01; // ��������� �������������� �������


        if((Comport[1]->PortTask) & 0x100)
	    {
		    Comport[1]->DevErr = DZaslVAT[1]->DZaslVAT_Manage(Comport[1]->DevState,1);
		    if((Comport[1]->DevState) > 1)
		    {
			    (Comport[1]->DevErr) ? diagnS[DZaslVAT[1]->diagnS_byte] |= DZaslVAT[1]->diagnS_mask : diagnS[DZaslVAT[1]->diagnS_byte] &= (~DZaslVAT[1]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x100);
			    DZaslVAT[1]->RCom = 0;
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
	    else if((Comport[1]->PortTask)&0x01)
	    {
		    Comport[1]->DevErr = DZaslVAT[1]->DZaslVAT_Manage(Comport[1]->DevState,0);
		    if((Comport[1]->DevState)>1)
		    {
			    (Comport[1]->DevErr) ? diagnS[DZaslVAT[1]->diagnS_byte] |= DZaslVAT[1]->diagnS_mask : diagnS[DZaslVAT[1]->diagnS_byte] &= (~DZaslVAT[1]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x01);
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
}
catch (Exception &exception)
{
        if(!(Comport[1]->port_err))
        {
                Comport[1]->port_err = 1;
                if(Comport[1]->State)
                {
                        Comport[1]->State = 0;
			            Comport[1]->Port.Close();
			            Comport[1]->BTN_reset->Caption = "���� �����";
                        Comport[1]->port_ct = 0;
                }
                // ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void Timer_Com3()
{
// return;

try
{
    if(Comport[2]->port_err)
    {
        if(Comport[2]->port_ct > 30)
        {
            if(Comport[2]->Port.Open(Comport[2]->PortName.c_str(),Comport[2]->B_Rate,Data8Bit,Comport[2]->P_Rate,OneStopBit))
            {
                Comport[2]->State = 1;
                Comport[2]->BTN_reset->Caption = "���� �����";
                Comport[2]->port_err = 0;
            }
            else
            {
                Comport[2]->port_ct = 0;
            }
        }
        else Comport[2]->port_ct++;
    }

    Comport[2]->CB_status->Checked = Comport[2]->State;
	Comport[2]->CB_nal->Checked = Comport[2]->Pr_nal;
	Comport[2]->LBL_otl->Visible = (!(Comport[2]->State) || (Comport[2]->Pr_nal));

	// ��������� ��������� � ���� �������
	if(!(Comport[2]->State)||!ust_ready) return;

	// ����������� ������/��������
	Comport[2]->RB_prd->Checked = !(Comport[2]->DevState);
	Comport[2]->RB_prm->Checked = Comport[2]->DevState;

	if(!(Comport[2]->PortTask)&&!(Comport[2]->Pr_nal)) Comport[2]->PortTask |= 0x1f; // ��������� �������������� �������
    
    if(Comport[2]->PortTask & 0x01)
	{
		Comport[2]->DevErr = Dat_PPT200[0]->DatPPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_PPT200[0]->diagnS_byte] |= Dat_PPT200[0]->diagnS_mask : diagnS[Dat_PPT200[0]->diagnS_byte] &= (~Dat_PPT200[0]->diagnS_mask);
			Comport[2]->PortTask &= (~0x01);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x02)
	{
		Comport[2]->DevErr = Dat_PPT200[1]->DatPPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_PPT200[1]->diagnS_byte] |= Dat_PPT200[1]->diagnS_mask : diagnS[Dat_PPT200[1]->diagnS_byte] &= (~Dat_PPT200[1]->diagnS_mask);
			Comport[2]->PortTask &= (~0x02);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x04)
	{
		Comport[2]->DevErr = Dat_MPT200[0]->DatMPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_MPT200[0]->diagnS_byte] |= Dat_MPT200[0]->diagnS_mask : diagnS[Dat_MPT200[0]->diagnS_byte] &= (~Dat_MPT200[0]->diagnS_mask);
			Comport[2]->PortTask &= (~0x04);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x08)
	{
		Comport[2]->DevErr = Dat_MPT200[1]->DatMPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_MPT200[1]->diagnS_byte] |= Dat_MPT200[1]->diagnS_mask : diagnS[Dat_MPT200[1]->diagnS_byte] &= (~Dat_MPT200[1]->diagnS_mask);
			Comport[2]->PortTask &= (~0x08);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x10)
	{
		Comport[2]->DevErr = Dat_MPT200[2]->DatMPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_MPT200[2]->diagnS_byte] |= Dat_MPT200[2]->diagnS_mask : diagnS[Dat_MPT200[2]->diagnS_byte] &= (~Dat_MPT200[2]->diagnS_mask);
			Comport[2]->PortTask &= (~0x10);
			Comport[2]->DevState = 0;
		}
		return;
	}
}
catch (Exception &exception)
{
        if(!(Comport[2]->port_err))
        {
                Comport[2]->port_err = 1;
                if(Comport[2]->State)
                {
                        Comport[2]->State = 0;
			            Comport[2]->Port.Close();
			            Comport[2]->BTN_reset->Caption = "���� �����";
                        Comport[2]->port_ct = 0;
                }
                // ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void Timer_Com4()
{
// return;

try
{
    if(Comport[3]->port_err)
    {
        if(Comport[3]->port_ct > 30)
        {
            if(Comport[3]->Port.Open(Comport[3]->PortName.c_str(),Comport[3]->B_Rate,Data8Bit,Comport[3]->P_Rate,OneStopBit))
            {
                Comport[3]->State = 1;
                Comport[3]->BTN_reset->Caption = "���� �����";
                Comport[3]->port_err = 0;
            }
            else
            {
                Comport[3]->port_ct = 0;
            }
        }
        else Comport[3]->port_ct++;
    }

    Comport[3]->CB_status->Checked = Comport[3]->State;
	Comport[3]->CB_nal->Checked = Comport[3]->Pr_nal;
	Comport[3]->LBL_otl->Visible = (!(Comport[3]->State) || (Comport[3]->Pr_nal));

	// ��������� ��������� � ���� �������
	if(!(Comport[3]->State)||!ust_ready) return;

	// ����������� ������/��������
	Comport[3]->RB_prd->Checked = !(Comport[3]->DevState);
	Comport[3]->RB_prm->Checked = Comport[3]->DevState;

    if(!(Comport[3]->PortTask)&&!(Comport[3]->Pr_nal))
    {
        if(nasmod[4])
            Comport[3]->PortTask |= 0x01;
        else
            diagnS[TRMD[0]->diagnS_byte] &= ~(TRMD[0]->diagnS_mask);
    }

    if(Comport[3]->PortTask & 0x01)
	{
		Comport[3]->DevErr = TRMD[0]->TRMD_Manage(Comport[3]->DevState,0);
		if(Comport[3]->DevState > 1)
		{
			Comport[3]->DevErr ? diagnS[TRMD[0]->diagnS_byte] |= TRMD[0]->diagnS_mask : diagnS[TRMD[0]->diagnS_byte] &= (~TRMD[0]->diagnS_mask);
			Comport[3]->PortTask &= (~0x01);
			Comport[3]->DevState = 0;
		}
		return;
	}
}
catch (Exception &exception)
{
        if(!(Comport[3]->port_err))
        {
                Comport[3]->port_err = 1;
                if(Comport[3]->State)
                {
                        Comport[3]->State = 0;
			            Comport[3]->Port.Close();
			            Comport[3]->BTN_reset->Caption = "���� �����";
                        Comport[3]->port_ct = 0;
                }
                // ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void Timer_Com5()
{
// return;
try
{
    if(Comport[4]->port_err)
    {
        if(Comport[4]->port_ct > 30)
        {
            if(Comport[4]->Port.Open(Comport[4]->PortName.c_str(),Comport[4]->B_Rate,Data8Bit,Comport[4]->P_Rate,OneStopBit))
            {
                Comport[4]->State = 1;
                Comport[4]->BTN_reset->Caption = "���� �����";
                Comport[4]->port_err = 0;
            }
            else
            {
                Comport[4]->port_ct = 0;
            }
        }
        else Comport[4]->port_ct++;
    }
    Comport[4]->CB_status->Checked = Comport[4]->State;
	Comport[4]->CB_nal->Checked = Comport[4]->Pr_nal;
	Comport[4]->LBL_otl->Visible = (!(Comport[4]->State) || (Comport[4]->Pr_nal));

	// ��������� ��������� � ���� �������
	if(!(Comport[4]->State)||!ust_ready) return;

	// ����������� ������/��������
	Comport[4]->RB_prd->Checked = !(Comport[4]->DevState);
	Comport[4]->RB_prm->Checked = Comport[4]->DevState;

    if((Comport[4]->DevState == 0) &&
        ( *AZ_drive[0]->Pr_AZ ||
        *AZ_drive[1]->Pr_AZ ||
        *AZ_drive[2]->Pr_AZ))
    {
        if(!(*AZ_drive[0]->Pr_AZ)) Comport[4]->PortTask &= (~0x01);
        if(!(*AZ_drive[1]->Pr_AZ)) Comport[4]->PortTask &= (~0x02);
        if(!(*AZ_drive[2]->Pr_AZ)) Comport[4]->PortTask &= (~0x04);
        if(Comport[4]->PortTask == 0)
        {
                if(*AZ_drive[0]->Pr_AZ) Comport[4]->PortTask |= 0x01;
                if(*AZ_drive[1]->Pr_AZ) Comport[4]->PortTask |= 0x02;
                if(*AZ_drive[2]->Pr_AZ) Comport[4]->PortTask |= 0x04;
        }
    }
    else
    {
        if(!(Comport[4]->PortTask)) Comport[4]->PortTask |= 0x07;	// 3 ����������
    }
	
	
    if(Comport[4]->PortTask & 0x01)
	{
		Comport[4]->DevErr = AZ_drive[0]->AZ_manage(Comport[4]->DevState);
		if(Comport[4]->DevState > 1)
		{
			Comport[4]->DevErr ? diagnS[AZ_drive[0]->diagnS_byte] |= AZ_drive[0]->diagnS_mask : diagnS[AZ_drive[0]->diagnS_byte] &= (~AZ_drive[0]->diagnS_mask);
			Comport[4]->PortTask &= (~0x01);
			Comport[4]->DevState = 0;
		}
		return;
	}
    else if(Comport[4]->PortTask & 0x02)
	{
		Comport[4]->DevErr = AZ_drive[1]->AZ_manage(Comport[4]->DevState);
		if(Comport[4]->DevState > 1)
		{
			Comport[4]->DevErr ? diagnS[AZ_drive[1]->diagnS_byte] |= AZ_drive[1]->diagnS_mask : diagnS[AZ_drive[1]->diagnS_byte] &= (~AZ_drive[1]->diagnS_mask);
			Comport[4]->PortTask &= (~0x02);
			Comport[4]->DevState = 0;
		}
		return;
	}
    else if(Comport[4]->PortTask & 0x04)
	{
		Comport[4]->DevErr = AZ_drive[2]->AZ_manage(Comport[4]->DevState);
		if(Comport[4]->DevState > 1)
		{
			Comport[4]->DevErr ? diagnS[AZ_drive[2]->diagnS_byte] |= AZ_drive[2]->diagnS_mask : diagnS[AZ_drive[2]->diagnS_byte] &= (~AZ_drive[2]->diagnS_mask);
			Comport[4]->PortTask &= (~0x04);
			Comport[4]->DevState = 0;
		}
		return;
	}
}
catch (Exception &exception)
{
        if(!(Comport[4]->port_err))
        {
                Comport[4]->port_err = 1;
                if(Comport[4]->State)
                {
                        Comport[4]->State = 0;
			            Comport[4]->Port.Close();
			            Comport[4]->BTN_reset->Caption = "���� �����";
                        Comport[4]->port_ct = 0;
                }
                // ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void Timer_Com6()
{
// return;

try
{
    if(Comport[5]->port_err)
    {
        if(Comport[5]->port_ct > 30)
        {
            if(Comport[5]->Port.Open(Comport[5]->PortName.c_str(),Comport[5]->B_Rate,Data8Bit,Comport[5]->P_Rate,OneStopBit))
            {
                Comport[5]->State = 1;
                Comport[5]->BTN_reset->Caption = "���� �����";
                Comport[5]->port_err = 0;
            }
            else
            {
                Comport[5]->port_ct = 0;
            }
        }
        else Comport[5]->port_ct++;
    }
    Comport[5]->CB_status->Checked = Comport[5]->State;
	Comport[5]->CB_nal->Checked = Comport[5]->Pr_nal;
	Comport[5]->LBL_otl->Visible = (!(Comport[5]->State) || (Comport[5]->Pr_nal));

	// ��������� ��������� � ���� �������
	if(!(Comport[5]->State)||!ust_ready) return;

	// ����������� ������/��������
	Comport[5]->RB_prd->Checked = !(Comport[5]->DevState);
	Comport[5]->RB_prm->Checked = Comport[5]->DevState;

    if(!(Comport[5]->PortTask)&&!(Comport[5]->Pr_nal)) Comport[5]->PortTask |= 0x01; // ��������� �������������� �������
    // ���� ������ �������
    if((BPM_HP[0]->RCom)&&(!(Comport[5]->DevState))) Comport[5]->PortTask |= 0x100; //

	if(Comport[5]->PortTask & 0x100)
	{
		Comport[5]->DevErr = BPM_HP[0]->BU_IVE_Manage(Comport[5]->DevState,1);
		if(Comport[5]->DevState > 1)
		{
			Comport[5]->DevErr ? diagnS[BPM_HP[0]->diagnS_byte] |= BPM_HP[0]->diagnS_mask : diagnS[BPM_HP[0]->diagnS_byte] &= (~BPM_HP[0]->diagnS_mask);
			Comport[5]->PortTask &= (~0x100);
			BPM_HP[0]->RCom = 0;
			Comport[5]->DevState = 0;
		}
		return;
	}
	else if(Comport[5]->PortTask & 0x01)
	{
		Comport[5]->DevErr = BPM_HP[0]->BU_IVE_Manage(Comport[5]->DevState,0);
		if(Comport[5]->DevState > 1)
		{
			Comport[5]->DevErr ? diagnS[BPM_HP[0]->diagnS_byte] |= BPM_HP[0]->diagnS_mask : diagnS[BPM_HP[0]->diagnS_byte] &= (~BPM_HP[0]->diagnS_mask);
			Comport[5]->PortTask &= (~0x01);
			Comport[5]->DevState = 0;
		}
		return;
	}
}
catch (Exception &exception)
{
        if(!(Comport[5]->port_err))
        {
                Comport[5]->port_err = 1;
                if(Comport[5]->State)
                {
                        Comport[5]->State = 0;
			            Comport[5]->Port.Close();
			            Comport[5]->BTN_reset->Caption = "���� �����";
                        Comport[5]->port_ct = 0;
                }
                // ShowMessage("���������� ������. Com1 ��������!");
        }
        return;
}
}
//---------------------------------------------------------------------------
void __fastcall SComport::Com_Timer(TObject *Sender)
{
    if(((TTimer*)Sender)->Name == "ComTimer1") Timer_Com1();
    if(((TTimer*)Sender)->Name == "ComTimer2") Timer_Com2();
    if(((TTimer*)Sender)->Name == "ComTimer3") Timer_Com3();
    if(((TTimer*)Sender)->Name == "ComTimer4") Timer_Com4();
    if(((TTimer*)Sender)->Name == "ComTimer5") Timer_Com5();
    if(((TTimer*)Sender)->Name == "ComTimer6") Timer_Com6();
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
		case 1 :sh_         = StrToInt(EditOTLzad1->Text) ; break;
//		case 2:            = StrToInt(EditOTLzad2->Text); break;
		case 3 :sh[1]       = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :shr[1]      = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :sh[2]       = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :shr[2]      = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :sh[3]       = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :shr[3]      = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :sh[4]       = StrToInt(EditOTLzad9->Text) ; break;
		case 10 :shr[4]      = StrToInt(EditOTLzad10->Text) ; break;
		case 11:sh[5]       = StrToInt(EditOTLzad11->Text); break;
		case 12:shr[5]      = StrToInt(EditOTLzad12->Text); break;
		case 13:sh[6]       = StrToInt(EditOTLzad13->Text); break;
		case 14:shr[6]      = StrToInt(EditOTLzad14->Text); break;
		case 15:sh[7]       = StrToInt(EditOTLzad15->Text); break;
		case 16:shr[7]      = StrToInt(EditOTLzad16->Text); break;
		case 17:sh[8]       = StrToInt(EditOTLzad17->Text); break;
		case 18:shr[8]      = StrToInt(EditOTLzad18->Text); break;
		case 19:sh[9]       = StrToInt(EditOTLzad19->Text); break;
		case 20:shr[9]      = StrToInt(EditOTLzad20->Text); break;
		case 21:sh[10]      = StrToInt(EditOTLzad21->Text); break;
		case 22:shr[10]     = StrToInt(EditOTLzad22->Text); break;
		case 23:sh[11]      = StrToInt(EditOTLzad23->Text); break;
		case 24:shr[11]     = StrToInt(EditOTLzad24->Text); break;
		case 25:sh[12]      = StrToInt(EditOTLzad25->Text); break;
		case 26:shr[12]     = StrToInt(EditOTLzad26->Text); break;
		case 27:sh[13]      = StrToInt(EditOTLzad27->Text); break;
		case 28:shr[13]     = StrToInt(EditOTLzad28->Text); break;
		case 29:sh[14]      = StrToInt(EditOTLzad29->Text); break;
		case 30:shr[14]     = StrToInt(EditOTLzad30->Text); break;

	}
}; break;
     //1 ��������
case 1:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :sh[15]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :shr[15]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :sh[16]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :shr[16]     = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :sh[17]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :shr[17]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :sh[18]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :shr[18]     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :sh[19]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:shr[19]     = StrToInt(EditOTLzad10->Text); break;
		case 11:sh[20]      = StrToInt(EditOTLzad11->Text); break;
		case 12:shr[20]     = StrToInt(EditOTLzad12->Text); break;
		case 13:sh[21]      = StrToInt(EditOTLzad13->Text); break;
		case 14:shr[21]     = StrToInt(EditOTLzad14->Text); break;
		case 15:sh[22]      = StrToInt(EditOTLzad15->Text); break;
		case 16:shr[22]     = StrToInt(EditOTLzad16->Text); break;
		case 17:sh[23]      = StrToInt(EditOTLzad17->Text); break;
		case 18:shr[23]     = StrToInt(EditOTLzad18->Text); break;
		case 19:sh[24]      = StrToInt(EditOTLzad19->Text); break;
		case 20:shr[24]     = StrToInt(EditOTLzad20->Text); break;
		case 21:sh[25]      = StrToInt(EditOTLzad21->Text); break;
		case 22:shr[25]     = StrToInt(EditOTLzad22->Text); break;
		case 23:sh[26]      = StrToInt(EditOTLzad23->Text); break;
		case 24:shr[26]     = StrToInt(EditOTLzad24->Text); break;
		case 25:sh[27]      = StrToInt(EditOTLzad25->Text); break;
		case 26:shr[27]     = StrToInt(EditOTLzad26->Text); break;
		case 27:sh[28]      = StrToInt(EditOTLzad27->Text); break;
		case 28:shr[28]     = StrToInt(EditOTLzad28->Text); break;
		case 29:sh[29]      = StrToInt(EditOTLzad29->Text); break;
		case 30:shr[29]     = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
 //2 ��������
case 2:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :sh[30]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :shr[30]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :sh[31]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :shr[31]     = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :sh[32]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :shr[32]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :sh[33]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :shr[33]     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :sh[34]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:shr[34]     = StrToInt(EditOTLzad10->Text); break;
		case 11:sh[35]      = StrToInt(EditOTLzad11->Text); break;
		case 12:shr[35]     = StrToInt(EditOTLzad12->Text); break;
		case 13:sh[36]      = StrToInt(EditOTLzad13->Text); break;
		case 14:shr[36]     = StrToInt(EditOTLzad14->Text); break;
		case 15:sh[37]      = StrToInt(EditOTLzad15->Text); break;
		case 16:shr[37]     = StrToInt(EditOTLzad16->Text); break;
		case 17:sh[38]      = StrToInt(EditOTLzad17->Text); break;
		case 18:shr[38]     = StrToInt(EditOTLzad18->Text); break;
		case 19:sh[39]      = StrToInt(EditOTLzad19->Text); break;
		case 20:shr[39]     = StrToInt(EditOTLzad20->Text); break;
		case 21:sh[40]      = StrToInt(EditOTLzad21->Text); break;
		case 22:shr[40]     = StrToInt(EditOTLzad22->Text); break;
		case 23:sh[41]      = StrToInt(EditOTLzad23->Text); break;
		case 24:shr[41]     = StrToInt(EditOTLzad24->Text); break;
		case 25:sh[42]      = StrToInt(EditOTLzad25->Text); break;
		case 26:shr[42]     = StrToInt(EditOTLzad26->Text); break;
		case 27:sh[43]      = StrToInt(EditOTLzad27->Text); break;
		case 28:shr[43]     = StrToInt(EditOTLzad28->Text); break;
		case 29:sh[44]      = StrToInt(EditOTLzad29->Text); break;
		case 30:shr[44]     = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
 //3 ��������
case 3:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :sh[45]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :shr[45]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :sh[46]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :shr[46]     = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :sh[47]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :shr[47]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :sh[48]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :shr[48]     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :sh[49]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:shr[49]     = StrToInt(EditOTLzad10->Text); break;
		case 11:sh[50]      = StrToInt(EditOTLzad11->Text); break;
		case 12:shr[50]     = StrToInt(EditOTLzad12->Text); break;
//		case 13:     = StrToInt(EditOTLzad13->Text); break;
		case 14:zshr3     = StrToInt(EditOTLzad14->Text); break;
		case 15:norma     = StrToInt(EditOTLzad15->Text); break;
		case 16:qkk    = StrToInt(EditOTLzad16->Text); break;
//		case 17:      = StrToInt(EditOTLzad17->Text); break;
//		case 18:     = StrToInt(EditOTLzad18->Text); break;
//		case 19:      = StrToInt(EditOTLzad19->Text); break;
//		case 20:    = StrToInt(EditOTLzad20->Text); break;
//		case 21:      = StrToInt(EditOTLzad21->Text); break;
//		case 22:    = StrToInt(EditOTLzad22->Text); break;
//		case 23:      = StrToInt(EditOTLzad23->Text); break;
//		case 24:     = StrToInt(EditOTLzad24->Text); break;
//		case 25:     = StrToInt(EditOTLzad25->Text); break;
//		case 26:    = StrToInt(EditOTLzad26->Text); break;
//		case 27:      = StrToInt(EditOTLzad27->Text); break;
//		case 28:     = StrToInt(EditOTLzad28->Text); break;
//		case 29:      = StrToInt(EditOTLzad29->Text); break;
//		case 30:    = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //4 ��������
case 4:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :diagn[0]    = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :diagn[1]    = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :diagn[2]    = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :diagn[3]    = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :diagn[4]    = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :diagn[5]    = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :diagn[6]    = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :diagn[7]    = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :diagn[8]    = StrToInt(EditOTLzad9->Text) ; break;
		case 10:diagn[9]    = StrToInt(EditOTLzad10->Text); break;
		case 11:diagn[10]   = StrToInt(EditOTLzad11->Text); break;
		case 12:diagn[11]   = StrToInt(EditOTLzad12->Text); break;
		case 13:diagn[12]   = StrToInt(EditOTLzad13->Text); break;
		case 14:diagn[13]   = StrToInt(EditOTLzad14->Text); break;
		case 15:diagn[14]   = StrToInt(EditOTLzad15->Text); break;
		case 16:diagn[15]   = StrToInt(EditOTLzad16->Text); break;
		case 17:diagn[16]   = StrToInt(EditOTLzad17->Text); break;
		case 18:diagn[17]   = StrToInt(EditOTLzad18->Text); break;
		case 19:diagn[18]   = StrToInt(EditOTLzad19->Text); break;
		case 20:diagn[19]   = StrToInt(EditOTLzad20->Text); break;
		case 21:diagn[20]   = StrToInt(EditOTLzad21->Text); break;
		case 22:diagn[21]   = StrToInt(EditOTLzad22->Text); break;
		case 23:diagn[22]   = StrToInt(EditOTLzad23->Text); break;
		case 24:diagn[23]   = StrToInt(EditOTLzad24->Text); break;
		case 25:diagn[24]   = StrToInt(EditOTLzad25->Text); break;
		case 26:diagn[25]   = StrToInt(EditOTLzad26->Text); break;
		case 27:diagn[26]   = StrToInt(EditOTLzad27->Text); break;
		case 28:diagn[27]   = StrToInt(EditOTLzad28->Text); break;
		case 29:diagn[28]   = StrToInt(EditOTLzad29->Text); break;
		case 30:diagn[29]   = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //5 ��������
case 5:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :diagn[30]   = StrToInt(EditOTLzad1->Text) ; break;
//		case 2 := StrToInt(EditOTLzad2->Text) ; break;
		case 3 :diagnS[0]   = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :diagnS[1]   = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :diagnS[2]   = StrToInt(EditOTLzad5->Text) ; break;
//		case 6 := StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
//		case 9 := StrToInt(EditOTLzad9->Text) ; break;
//		case 10:= StrToInt(EditOTLzad10->Text); break;
//		case 11:= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
//		case 13:= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //6 ��������
case 6:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :out[0]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :out[1]      = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :out[2]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :out[3]      = StrToInt(EditOTLzad4->Text) ; break;
//		case 5 :      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :zin[0]      = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :zin[1]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :zin[2]      = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :zin[3]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:zin[4]      = StrToInt(EditOTLzad10->Text); break;
//		case 11:     = StrToInt(EditOTLzad11->Text); break;
		case 12:aik[0]      = StrToInt(EditOTLzad12->Text); break;
		case 13:aik[1]      = StrToInt(EditOTLzad13->Text); break;
		case 14:aik[2]      = StrToInt(EditOTLzad14->Text); break;
		case 15:aik[3]      = StrToInt(EditOTLzad15->Text); break;
		case 16:aik[4]      = StrToInt(EditOTLzad16->Text); break;
		case 17:aik[5]      = StrToInt(EditOTLzad17->Text); break;
		case 18:aik[6]      = StrToInt(EditOTLzad18->Text); break;
		case 19:aik[7]      = StrToInt(EditOTLzad19->Text); break;
		case 20:aik[8]      = StrToInt(EditOTLzad20->Text); break;
		case 21:aik[9]      = StrToInt(EditOTLzad21->Text); break;
		case 22:aik[10]     = StrToInt(EditOTLzad22->Text); break;
		case 23:aik[11]     = StrToInt(EditOTLzad23->Text); break;
		case 24:aik[12]     = StrToInt(EditOTLzad24->Text); break;
		case 25:aik[13]     = StrToInt(EditOTLzad25->Text); break;
		case 26:aik[14]     = StrToInt(EditOTLzad26->Text); break;
		case 27:aik[15]     = StrToInt(EditOTLzad27->Text); break;
		case 28:aik[16]     = StrToInt(EditOTLzad28->Text); break;
//		case 29:     = StrToInt(EditOTLzad29->Text); break;
//	    case 30:     = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //7 ��������
case 7:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :aout[0]     = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :aout[1]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :aout[2]     = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :aout[3]     = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :aout[4]     = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :aout[5]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :aout[6]     = StrToInt(EditOTLzad7->Text) ; break;
//		case 8 :     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :D_D1        = StrToInt(EditOTLzad9->Text) ; break;
		case 10:D_D2        = StrToInt(EditOTLzad10->Text); break;
		case 11:D_D3        = StrToInt(EditOTLzad11->Text); break;
		case 12:D_D4        = StrToInt(EditOTLzad12->Text); break;
		case 13:D_D5        = StrToInt(EditOTLzad13->Text); break;
		case 14:D_D6        = StrToInt(EditOTLzad14->Text); break;
//		case 15:        = StrToInt(EditOTLzad15->Text); break;
		case 16:UVAK_KAM    = StrToInt(EditOTLzad16->Text); break;
		case 17:UVAKV_KAM   = StrToInt(EditOTLzad17->Text); break;
		case 18:UVAKN_KAM   = StrToInt(EditOTLzad18->Text); break;
		case 19:POROG_DAVL  = StrToInt(EditOTLzad19->Text); break;
		case 20:UVAK_TMNOP  = StrToInt(EditOTLzad20->Text); break;
		case 21:UVVAK_TMNOP   = StrToInt(EditOTLzad21->Text); break;
		case 22:UVVAK_KAM   = StrToInt(EditOTLzad22->Text); break;
		case 23:UVVAK_SHL    = StrToInt(EditOTLzad23->Text); break;
   	    case 24:POROG_M = StrToInt(EditOTLzad24->Text); break;
		case 25:UVAKN_TMN   = StrToInt(EditOTLzad25->Text); break;
		case 26:UVAKV_TMN        = StrToInt(EditOTLzad26->Text); break;
		case 27:UVAK_SHL   = StrToInt(EditOTLzad27->Text); break;
		case 28:UVAK_SHL_MO= StrToInt(EditOTLzad28->Text); break;
		case 29:UVAK_ZTMN= StrToInt(EditOTLzad29->Text); break;
		case 30:UATM= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //8 ��������
case 8:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :nasmod[0]   = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :nasmod[1]   = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :nasmod[2]   = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :nasmod[3]   = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :nasmod[4]   = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :nasmod[5]   = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :nasmod[6]   = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :nasmod[7]   = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :nasmod[8]   = StrToInt(EditOTLzad9->Text) ; break;
		case 10:nasmod[9]   = StrToInt(EditOTLzad10->Text); break;
		case 11:nasmod[10]  = StrToInt(EditOTLzad11->Text); break;
		case 12:nasmod[11]  = StrToInt(EditOTLzad12->Text); break;
		case 13:nasmod[12]  = StrToInt(EditOTLzad13->Text); break;
		case 14:nasmod[13]  = StrToInt(EditOTLzad14->Text); break;
		case 15:nasmod[14]  = StrToInt(EditOTLzad15->Text); break;
		case 16:nasmod[15]  = StrToInt(EditOTLzad16->Text); break;
		case 17:nasmod[16]  = StrToInt(EditOTLzad17->Text); break;
		case 18:nasmod[17]  = StrToInt(EditOTLzad18->Text); break;
		case 19:nasmod[18]  = StrToInt(EditOTLzad19->Text); break;
		case 20:nasmod[19]  = StrToInt(EditOTLzad20->Text); break;
		case 21:nasmod[20]  = StrToInt(EditOTLzad21->Text); break;
//		case 22:  = StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:par_t[0]    = StrToInt(EditOTLzad25->Text); break;
//		case 26:par_t[1]    = StrToInt(EditOTLzad26->Text); break;
//		case 27:par_t[2]    = StrToInt(EditOTLzad27->Text); break;
//		case 28:par_t[3]    = StrToInt(EditOTLzad28->Text); break;
//		case 29:par_t[4]    = StrToInt(EditOTLzad29->Text); break;
//		case 30:par_t[5]    = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //9 ��������
case 9:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[0][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[0][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[0][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[0][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[0][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[0][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[0][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[0][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[0][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[0][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[0][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[0][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[0][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[0][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[0][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[0][15]= StrToInt(EditOTLzad16->Text); break;
		case 17:par[0][16]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[0][17]= StrToInt(EditOTLzad18->Text); break;
		case 19:par[0][18]= StrToInt(EditOTLzad19->Text); break;
		case 20:par[0][19]= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //10 ��������
case 10:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[1][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[1][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[1][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[1][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[1][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[1][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[1][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[1][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[1][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[1][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[1][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[1][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[1][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[1][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[1][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[1][15]= StrToInt(EditOTLzad16->Text); break;
		case 17:par[1][16]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[1][17]= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //11 ��������
case 11:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[2][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[2][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[2][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[2][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[2][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[2][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[2][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[2][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[2][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[2][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[2][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[2][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[2][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[2][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[2][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[2][15]= StrToInt(EditOTLzad16->Text); break;
		case 17:par[2][16]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[2][17]= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //12 ��������
case 12:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[3][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[3][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[3][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[3][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[3][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[3][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[3][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[3][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[3][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[3][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[3][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[3][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[3][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[3][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[3][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[3][15]= StrToInt(EditOTLzad16->Text); break;
		case 17:par[3][16]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[3][17]= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //13 ��������
case 13:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[4][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[4][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[4][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[4][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[4][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[4][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[4][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[4][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[4][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[4][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[4][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[4][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[4][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[4][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[4][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[4][15]= StrToInt(EditOTLzad16->Text); break;
		case 17:par[4][16]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[4][17]= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //14 ��������
case 14:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[5][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[5][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[5][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[5][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[5][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[5][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[5][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[5][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[5][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[5][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[5][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[5][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[5][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[5][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[5][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[5][15]= StrToInt(EditOTLzad16->Text); break;
		case 17:par[5][16]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[5][17]= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;

     //15 ��������
case 15:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :CT_T1= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :CT_T20= StrToInt(EditOTLzad2->Text) ; break;
//		case 3 := StrToInt(EditOTLzad3->Text) ; break;
		case 4 :CT_1= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :CT_2= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :CT_3= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :CT_4= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :CT_5= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :CT_6= StrToInt(EditOTLzad9->Text) ; break;
		case 10:CT_7= StrToInt(EditOTLzad10->Text); break;
		case 11:CT_9= StrToInt(EditOTLzad11->Text); break;
		case 12:CT_19= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_27= StrToInt(EditOTLzad13->Text); break;
		case 14:CT27K1= StrToInt(EditOTLzad14->Text); break;
		case 15:CT_28= StrToInt(EditOTLzad15->Text); break;
		case 16:CT28K1= StrToInt(EditOTLzad16->Text); break;
		case 17:CT_29= StrToInt(EditOTLzad17->Text); break;
		case 18:CT29K1= StrToInt(EditOTLzad18->Text); break;
		case 19:CT_31= StrToInt(EditOTLzad19->Text); break;
		case 20:CT31K1= StrToInt(EditOTLzad20->Text); break;
		case 21:CT_33= StrToInt(EditOTLzad21->Text); break;
		case 22:CT33K1= StrToInt(EditOTLzad22->Text); break;
		case 23:CT_39= StrToInt(EditOTLzad23->Text); break;
		case 24:CT_43= StrToInt(EditOTLzad24->Text); break;
		case 25:CT43K1= StrToInt(EditOTLzad25->Text); break;
		case 26:CT_46= StrToInt(EditOTLzad26->Text); break;
		case 27:CT_47= StrToInt(EditOTLzad27->Text); break;
		case 28:CT_48= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //16 ��������
case 16:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
   		case 1 :CT_VHG= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :CT_PER= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :CT_BMH1= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :CT_TEMP1= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :CT_TEMP2= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :CT_TMN= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :CT_IST= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :CT_VODA_BM= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :CT_VODA_II= StrToInt(EditOTLzad9->Text) ; break;
		case 10:CT_KZ_BMH1= StrToInt(EditOTLzad10->Text); break;
		case 11:CT_VAK= StrToInt(EditOTLzad11->Text); break;
// 		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_Kl6= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //17 ��������
case 17:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
   		case 1 :T_VHG= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :T_PROC= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :T_KTMN_RAZGON= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :T_VKL_BPN= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :T_VODA= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :T_KKAM= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :T_KTMN= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :T_KPER= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :T_KPRST= StrToInt(EditOTLzad9->Text) ; break;
		case 10:T_KPR= StrToInt(EditOTLzad10->Text); break;
		case 11:T_KSHL= StrToInt(EditOTLzad11->Text); break;
		case 12:T_KNAP= StrToInt(EditOTLzad12->Text); break;
		case 13:T_NAPUSK= StrToInt(EditOTLzad13->Text); break;
		case 14:T_SBROSHE= StrToInt(EditOTLzad14->Text); break;
		case 15:T_KSHL_MO= StrToInt(EditOTLzad15->Text); break;
		case 16:T_KKAV_V= StrToInt(EditOTLzad16->Text); break;
		case 17:T_KSHL_V= StrToInt(EditOTLzad17->Text); break;
	 	case 18:T_OSTANOV_TMNOP= StrToInt(EditOTLzad18->Text); break;
		case 19:T_M_VR= StrToInt(EditOTLzad19->Text); break;
		case 20:T_RAZGON_TMN= StrToInt(EditOTLzad20->Text); break;
		case 21:T_OTKL_TMN= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;

     //18 ��������
case 18:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :zshr3= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :PR_TRTEST= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_NALADKA= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :N_ST_MAX= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :N_ST= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :PR_RG3= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :otvet= StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :PR_FOTK_SHL= StrToInt(EditOTLzad9->Text) ; break;
		case 10:PR_TREN= StrToInt(EditOTLzad10->Text); break;
		case 11:VRSO2= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
//		case 13:= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
 //		case 18:= StrToInt(EditOTLzad18->Text); break;
 //		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //19 ��������
case 19:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_DZASL1= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_DZASL1= StrToInt(EditOTLzad2->Text) ; break;
//		case 3 := StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
		case 5 :DAVL_DZASL1= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :DATA_DZASL1= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :X_TDZASL1= StrToInt(EditOTLzad9->Text) ; break;
		case 10:VRDZASL1= StrToInt(EditOTLzad10->Text); break;
		case 11:E_TDZASL1= StrToInt(EditOTLzad11->Text); break;
		case 12:DELDZASL1= StrToInt(EditOTLzad12->Text); break;
		case 13:LIM1DZASL1= StrToInt(EditOTLzad13->Text); break;
		case 14:LIM2DZASL1= StrToInt(EditOTLzad14->Text); break;
		case 15:DOPDZASL1= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:KOM_DZASL1= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:CT_DZASL1= StrToInt(EditOTLzad22->Text); break;
		case 23:T_KDZASL1= StrToInt(EditOTLzad23->Text); break;
		case 24:T_VRDZASL1= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
		case 26:PAR_DZASL1= StrToInt(EditOTLzad26->Text); break;
		case 27:ZPAR_DZASL1= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
		case 29:TEK_DAVL_DZASL1= StrToInt(EditOTLzad29->Text); break;
		case 30:TEK_POZ_DZASL1= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //20 ��������
case 20:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_DZASL2= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_DZASL2= StrToInt(EditOTLzad2->Text) ; break;
//		case 3 := StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
		case 5 :DAVL_DZASL2= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :DATA_DZASL2= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :X_TDZASL2= StrToInt(EditOTLzad9->Text) ; break;
		case 10:VRDZASL2= StrToInt(EditOTLzad10->Text); break;
		case 11:E_TDZASL2= StrToInt(EditOTLzad11->Text); break;
		case 12:DELDZASL2= StrToInt(EditOTLzad12->Text); break;
		case 13:LIM1DZASL2= StrToInt(EditOTLzad13->Text); break;
		case 14:LIM2DZASL2= StrToInt(EditOTLzad14->Text); break;
		case 15:DOPDZASL2= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:KOM_DZASL2= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:CT_DZASL2= StrToInt(EditOTLzad22->Text); break;
		case 23:T_KDZASL2= StrToInt(EditOTLzad23->Text); break;
		case 24:T_VRDZASL2= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
		case 26:PAR_DZASL2= StrToInt(EditOTLzad26->Text); break;
		case 27:ZPAR_DZASL2= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
		case 29:TEK_DAVL_DZASL2= StrToInt(EditOTLzad29->Text); break;
		case 30:TEK_POZ_DZASL2= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //21 ��������
case 21:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :VRGIS= StrToInt(EditOTLzad1->Text) ; break;
//		case 2 := StrToInt(EditOTLzad2->Text) ; break;
		case 3 :K_SOGL_GIS= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :NAPRS_GIS= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :X_TGIS= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :E_TGIS= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :DELGIS= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :DOPGIS= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :N_TEK_GIS= StrToInt(EditOTLzad9->Text) ; break;
		case 10:LIM1GIS= StrToInt(EditOTLzad10->Text); break;
		case 11:LIM2GIS= StrToInt(EditOTLzad11->Text); break;
		case 12:T_VRGIS= StrToInt(EditOTLzad12->Text); break;
		case 13:T_KGIS= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //22 ��������
case 22:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :KOM_TEMP= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :PR_TEMP= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :TEK_TEMP3= StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
//		case 5 := StrToInt(EditOTLzad5->Text) ; break;
		case 6 :ZAD_TEMP1= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :PAR_TEMP1= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :ZPAR_TEMP1= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :X_TEMP1= StrToInt(EditOTLzad9->Text) ; break;
		case 10:VRTEMP1= StrToInt(EditOTLzad10->Text); break;
		case 11:E_TEMP1= StrToInt(EditOTLzad11->Text); break;
		case 12:DELTEMP1= StrToInt(EditOTLzad12->Text); break;
		case 13:LIM1TEMP1= StrToInt(EditOTLzad13->Text); break;
		case 14:LIM2TEMP1= StrToInt(EditOTLzad14->Text); break;
		case 15:DOPTEMP1= StrToInt(EditOTLzad15->Text); break;
		case 16:TEK_TEMP1= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
		case 18:ZAD_TEMP2= StrToInt(EditOTLzad18->Text); break;
		case 19:PAR_TEMP2= StrToInt(EditOTLzad19->Text); break;
		case 20:X_TEMP2= StrToInt(EditOTLzad20->Text); break;
		case 21:VRTEMP2= StrToInt(EditOTLzad21->Text); break;
		case 22:E_TEMP2= StrToInt(EditOTLzad22->Text); break;
		case 23:DELTEMP2= StrToInt(EditOTLzad23->Text); break;
		case 24:LIM1TEMP2= StrToInt(EditOTLzad24->Text); break;
		case 25:LIM2TEMP2= StrToInt(EditOTLzad25->Text); break;
		case 26:DOPTEMP2= StrToInt(EditOTLzad26->Text); break;
		case 27:TEK_TEMP2= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
		case 29:T_VRTEMP= StrToInt(EditOTLzad29->Text); break;
		case 30:T_KTEMP= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //23 ��������

case 23:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_PER= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :CT_PER= StrToInt(EditOTLzad2->Text) ; break;
		//case 3 :V_PER= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :T_KPER= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :T_KPR= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :T_KPRST= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
		case 8 :POL_PER= StrToInt(EditOTLzad8->Text) ; break;
//		case 9 := StrToInt(EditOTLzad9->Text) ; break;
//		case 10:= StrToInt(EditOTLzad10->Text); break;
//		case 11:= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
//		case 13:= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:CT_POD= StrToInt(EditOTLzad17->Text); break;
		case 18:KOM_POD= StrToInt(EditOTLzad18->Text); break;
		case 19:OTVET_POD= StrToInt(EditOTLzad19->Text); break;
		case 20:TYPE_POD= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:PR_POD= StrToInt(EditOTLzad22->Text); break;
		case 23:HOME_POD= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:PUT_POD= StrToInt(EditOTLzad25->Text); break;
		case 26:PAR_POD= StrToInt(EditOTLzad26->Text); break;
		case 27:V_POD= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
		case 29:TEK_ABS_POD= StrToInt(EditOTLzad29->Text); break;
		case 30:TEK_OTN_POD= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //24 ��������
case 24:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
   		case 1 :CT_VR= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :KOM_VR= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :OTVET_VR= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :TYPE_VR= StrToInt(EditOTLzad4->Text) ; break;
//		case 5 := StrToInt(EditOTLzad5->Text) ; break;
		case 6 :PR_VR= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :HOME_VR= StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :PUT_VR= StrToInt(EditOTLzad9->Text) ; break;
//		case 10:= StrToInt(EditOTLzad10->Text); break;
		case 11:V_VR= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:TEK_ABS_VR= StrToInt(EditOTLzad13->Text); break;
		case 14:TEK_OTN_VR= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:CT_POV= StrToInt(EditOTLzad17->Text); break;
		case 18:KOM_POV= StrToInt(EditOTLzad18->Text); break;
		case 19:OTVET_POV= StrToInt(EditOTLzad19->Text); break;
		case 20:TYPE_POV= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:PR_POV= StrToInt(EditOTLzad22->Text); break;
		case 23:HOME_POV= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:PUT_POV= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
		case 27:V_POV= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
		case 29:TEK_ABS_POV= StrToInt(EditOTLzad29->Text); break;
		case 30:TEK_OTN_POV= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //25 ��������
case 25:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
  		case 1 :VRBMH1= StrToInt(EditOTLzad1->Text) ; break;
//		case 2 := StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_SV_BMH1= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :BMH1_mode= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :UST_BMH1= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :X_TBMH1= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :E_TBMH1= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :DELBMH1= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :DOPBMH1= StrToInt(EditOTLzad9->Text) ; break;
		case 10:PAR_BMH1_I= StrToInt(EditOTLzad10->Text); break;
		case 11:PAR_BMH1_P= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:LIM1BMH1= StrToInt(EditOTLzad13->Text); break;
		case 14:LIM2BMH1= StrToInt(EditOTLzad14->Text); break;
		case 15:N_PROBBMH= StrToInt(EditOTLzad15->Text); break;
		case 16:T_VRBMH= StrToInt(EditOTLzad16->Text); break;
		case 17:T_KBMH= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
		case 19:CT_KZ_BMH1= StrToInt(EditOTLzad19->Text); break;
		case 20:PR_KZ_BMH1= StrToInt(EditOTLzad20->Text); break;
 		case 21:N_KZ_BMH1= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
		case 23:CT_BMH1= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //26 ��������
case 26:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
  		case 1 :OTVET_BMH1[0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_BMH1[1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :OTVET_BMH1[2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :OTVET_BMH1[3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :OTVET_BMH1[4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :OTVET_BMH1[5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :OTVET_BMH1[6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :OTVET_BMH1[7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :OTVET_BMH1[8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:OTVET_BMH1[9]= StrToInt(EditOTLzad10->Text); break;
		case 11:OTVET_BMH1[10]= StrToInt(EditOTLzad11->Text); break;
		case 12:OTVET_BMH1[11]= StrToInt(EditOTLzad12->Text); break;
		case 13:OTVET_BMH1[12]= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:KOM_BMH1[0]= StrToInt(EditOTLzad22->Text); break;
		case 23:KOM_BMH1[1]= StrToInt(EditOTLzad23->Text); break;
		case 24:KOM_BMH1[2]= StrToInt(EditOTLzad24->Text); break;
		case 25:KOM_BMH1[3]= StrToInt(EditOTLzad25->Text); break;
		case 26:KOM_BMH1[4]= StrToInt(EditOTLzad26->Text); break;
		case 27:KOM_BMH1[5]= StrToInt(EditOTLzad27->Text); break;
		case 28:KOM_BMH1[6]= StrToInt(EditOTLzad28->Text); break;
		case 29:KOM_BMH1[7]= StrToInt(EditOTLzad29->Text); break;
		case 30:KOM_BMH1[8]= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //27 ��������
case 27:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
  		case 1 :VRSG= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :X_TSG= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :E_TSG= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :DELSG= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :DELSG= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :K_PSG= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :K_ISG= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :U_PSG= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :A_VIH= StrToInt(EditOTLzad9->Text) ; break;
		case 10:LIMPSG= StrToInt(EditOTLzad10->Text); break;
		case 11:LIMISG= StrToInt(EditOTLzad11->Text); break;
		case 12:LIM1SG= StrToInt(EditOTLzad12->Text); break;
		case 13:LIM2SG= StrToInt(EditOTLzad13->Text); break;
		case 14:LIMUSG= StrToInt(EditOTLzad14->Text); break;
		case 15:LIMU_SG= StrToInt(EditOTLzad15->Text); break;
		case 16:LIMUPR_SG= StrToInt(EditOTLzad16->Text); break;
		case 17:PORCNV_SG= StrToInt(EditOTLzad17->Text); break;
		case 18:PORCPR_SG= StrToInt(EditOTLzad18->Text); break;
		case 19:PROBSG= StrToInt(EditOTLzad19->Text); break;
		case 20:T_VRSG= StrToInt(EditOTLzad20->Text); break;
		case 21:T_KSG= StrToInt(EditOTLzad21->Text); break;
		case 22:T_VREJ_SG= StrToInt(EditOTLzad22->Text); break;
		case 23:T_VPRB_SG= StrToInt(EditOTLzad23->Text); break;
		case 24:T_REQSG= StrToInt(EditOTLzad24->Text); break;
		case 25:CT_VRSG= StrToInt(EditOTLzad25->Text); break;
		case 26:CT_PR_SG= StrToInt(EditOTLzad26->Text); break;
		case 27:CT_REQSG= StrToInt(EditOTLzad27->Text); break;
		case 28:SOSG= StrToInt(EditOTLzad28->Text); break;
		case 29:DOPSG= StrToInt(EditOTLzad29->Text); break;
		case 30:PAR_SG= StrToInt(EditOTLzad30->Text); break;
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
    }
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_500Timer(TObject *Sender)
{
    // ����������� ����������

        VisualMnemo();
        // ������������ ����-�������

        Label_Time -> Caption = FormatDateTime("hh:mm:ss",Time());
        Label_Date -> Caption = FormatDateTime("dd.mm.yyyy",Date());

        // ����� ������� ������
        Form1 -> EditTLogic -> Text = FloatToStrF(logic_time,ffFixed,6,3);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1secTimer(TObject *Sender)
{
    VisualGraph();                          //  ����������� ��������



}
//---------------------------------------------------------------------------
//--������������ ���������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualParT()
{
EdtTKon1 -> Text = FloatToStrF((float)par_t[0], ffFixed, 5, 0);


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

    par_t[0]  = StrToInt(EdtTRed1->Text);


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "  �������� ��������� ����������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    if ( EdtTKon1 -> Text != EdtTRed1 -> Text )
        MemoStat -> Lines -> Add("���������� ��������� ��������� �������� �/�: " + EdtTKon1 -> Text + " -> " + EdtTRed1 -> Text );


    // ����������� ���������� ���������
    EdtTRed1 -> Color = clWhite;


    // �������� ��������
    VisualParT();
    MemoT -> Lines -> Clear();
    MemoT -> Lines -> Add(EdtTKon1->Text);


    MemoT -> Lines -> SaveToFile("Nasmod\\Mex.txt");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_MechTimer(TObject *Sender)
{   // ������������ ���������� � �� �����
    // �����������
    if(!ust_ready) return;
    //������ �/�
    if(TEK_ABS_POD<=0)
        p_pd->Top=243;
    else if((TEK_ABS_POD>0)&&(TEK_ABS_POD<=nasmod[17]))
        p_pd->Top=243 - int(25.0*float(TEK_ABS_POD)/(float)(nasmod[17]));
    else if((TEK_ABS_POD>nasmod[17])&&(TEK_ABS_POD<=nasmod[16]))
        p_pd->Top=218 - int(22.0*float(TEK_ABS_POD-nasmod[17])/(float)(nasmod[16]-nasmod[17]));
    else
        p_pd->Top=196;
    //������� ��������
    if(zin[3]&0x10)
        Zasl->Picture->Bitmap=ZaslPD_home-> Picture->Bitmap;
    else
        Zasl->Picture->Bitmap=ZaslPD_None-> Picture->Bitmap;

    if(TEK_ABS_POV<=par_t[0])
        Zasl->Width=181;
    else if((TEK_ABS_POV>par_t[0])&&(TEK_ABS_POV<=0))
        Zasl->Width=60 + int(121.0*float(TEK_ABS_POV)/(float)(par_t[0]));
    else
        Zasl->Width=60;
    Zasl->Left=796-Zasl->Width;
    tube_7_open->Top=p_pd->Top;
    tube_7_close->Top=p_pd->Top;

// �������� ����
    Edt_AZ_1_1mn -> Text = IntToStr(par[0][10]);
    Edt_AZ_2_1mn -> Text = IntToStr(par[0][18]);
    Edt_AZ_3_1mn -> Text = IntToStr(par[0][19]);

    // ���������� ����
    Edt_AZ_1_2mn -> Text = IntToStr(TEK_ABS_POD);
    Edt_AZ_2_2mn -> Text = IntToStr(TEK_ABS_VR);
    Edt_AZ_3_2mn -> Text = IntToStr(TEK_ABS_POV);

    // ������������� ����
    Edt_AZ_1_3mn -> Text = IntToStr(TEK_OTN_POD);
    Edt_AZ_2_3mn -> Text = IntToStr(TEK_OTN_VR);
    Edt_AZ_3_3mn -> Text = IntToStr(TEK_OTN_POV);

    //���������
    switch(zin[1]&0xC0)
    {
        case 0x00:{ pp->Top=p_pd->Top+8;pp-> Picture->Bitmap =pp_n-> Picture->Bitmap;break;}
        case 0x40:{ pp->Top=p_pd->Top-6;pp-> Picture->Bitmap =pp_home-> Picture->Bitmap;break;}
        case 0x80:{ pp->Top=p_pd->Top+8;pp-> Picture->Bitmap =pp_home-> Picture->Bitmap;break;}
        case 0xC0:{ pp->Top=p_pd->Top+8;pp-> Picture->Bitmap =pp_n-> Picture->Bitmap;break;}
    }
    Vchg->Top=p_pd->Top+8;
    anim_vchg->Top=p_pd->Top-22;

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
  iniPAS.state[8] = !CB_Acc_V9->Checked;
  PCNalad->Pages[8]->TabVisible = !CB_Acc_V9->Checked;

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

























void __fastcall TForm1::BtnTrDaClick(TObject *Sender)
{
     // ������ ������������� �������� ������
    PanelParTr -> Visible = false;
    //N_ST=5 ------------------------------------------------------------------------------
    par[5][0]=StrToFloat  ( EdtARed5_0->Text ) / RRG1_MAX * 4095.0; //������ ���1
    par[5][4]=StrToFloat    ( EdtARed5_4->Text )*8000/DAVL_MAX + 1000;     //��������
    par[5][6]=StrToFloat  ( EdtARed5_6-> Text) * 4095.0 / 6000;     //�������� �
    par[5][12]=StrToInt   ( EdtARed5_12->Text );                    //����� ��������
     par[5][7]=  100.0/600.0*4095;             //��������� ��������� ����





    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ��������� ����������:");

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("���������� �:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //10 ������
    if ( EdtAKon5_0 -> Text != EdtARed5_0 -> Text )
        MemoStat -> Lines -> Add("������ ���1: " + EdtAKon5_0 -> Text + " -> " + EdtARed5_0 -> Text );
    if ( EdtAKon5_4 -> Text != EdtARed5_4 -> Text )
        MemoStat -> Lines -> Add("��������: " + EdtAKon5_4 -> Text + " -> " + EdtARed5_4 -> Text );
    if ( EdtAKon5_6 -> Text != EdtARed5_6 -> Text )
        MemoStat -> Lines -> Add("�������� �: " + EdtAKon5_6 -> Text + " -> " + EdtARed5_6 -> Text );
    if ( EdtAKon5_12 -> Text != EdtARed5_12 -> Text )
        MemoStat -> Lines -> Add("����� ��������: " + EdtAKon5_12 -> Text + " -> " + EdtARed5_12 -> Text );


    // ����������� ���������� ���������
    EdtARed5_0 -> Color = clWhite;
    EdtARed5_4 -> Color = clWhite;
    EdtARed5_6 -> Color = clWhite;
    EdtARed5_12 -> Color = clWhite;


    // �������� ��������
    VisualParA();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnParTrClick(TObject *Sender)
{
    PanelParTr -> Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnTrNetClick(TObject *Sender)
{
    PanelParTr -> Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnYesMagn13ResClick(TObject *Sender)
{
    // ����� ���������� ��� ���� ������ �������,
    // "��" ����� hint = ��������
    if ( ((TButton*)Sender)->Hint == "��������" )

            magnRes1 = 0;


    // � ����� ������� ������ ������� ���������
    ((TButton*)Sender) -> Parent -> Visible = false;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::EdtD3CodeChange(TObject *Sender)
{
   // �������� ��� ��������
    SBD3Debug -> Position = StrToInt(EdtD3Code->Text);
    // ����������� �������� ��������
    EdtD3Davl -> Text = FloatToStrF(pow(10,(float)SBD3Debug->Position/1000.0-3.5),ffExponent,3,8);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SBD3DebugChange(TObject *Sender)
{
    // �������� ��� ��������
    EdtD3Code -> Text = IntToStr(SBD3Debug->Position);
    // ����������� �������� ��������
    EdtD3Davl -> Text = FloatToStrF(pow(10,(float)SBD3Debug->Position/1000.0-3.5),ffExponent,3,8);
    //(3.5+log10(StrToFloat(EditNastrTo13->Text)))*1000.0
    //FloatToStrF(pow(10,(float)D_D1/1000.0-3.5),ffExponent,3,8);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SBD4DebugChange(TObject *Sender)
{
    // �������� ��� ��������
    EdtD4Code -> Text = IntToStr(SBD4Debug->Position);
    // ����������� �������� ��������
    EdtD4Davl -> Text = FloatToStrF(pow(10,(float)SBD4Debug->Position/1000.0*1.667-9.333),ffExponent,3,8);
    //FloatToStrF(pow(10,(float)D_D4/1000.0*1.667-9.333),ffExponent,3,8);
    //int(((log10(StrToFloat(EditNastrTo21->Text)))*0.6+5.6)*1000.0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::EdtD4CodeChange(TObject *Sender)
{
    // �������� ��� ��������
    SBD4Debug -> Position = StrToInt(EdtD4Code->Text);
    // ����������� �������� ��������
    EdtD4Davl -> Text = FloatToStrF(pow(10,(float)SBD4Debug->Position/1000.0*1.667-9.333),ffExponent,3,8);
}
//---------------------------------------------------------------------------
void TForm1::drawpaint(float alfa,float beta)
{
    double
        x1,y1,x2,y2,        //�������� ����������
        cx1=80,cy1=310,     //���������� ������ ��������
        r1=75,              //������ ��������
        cx2,cy2,            //��������� ���������� ������ �������� ��������
        r2=175;             //������ �������� ��������
    //������� ������ �������� ��������
    cx2=cx1-cos((150)*3.14159265/180)*r2;
    cy2=cy1-sin((150)*3.14159265/180)*r2;
    //�������� ����� �� ���������� ����
    DrawPaint->Picture->Bitmap= Image2 ->Picture->Bitmap;

    //������ ��������� ����� �����
    x2=cx1+cos((alfa+30)*3.14159265/180)*r1;
    y2=cy1+sin((alfa+30)*3.14159265/180)*r1;
    x1=cx1+cos((alfa-30)*3.14159265/180)*r1;
    y1=cy1+sin((alfa-30)*3.14159265/180)*r1;

    //�������� �����
    DrawPaint->Canvas->Pen->Color=(TColor)RGB(70,83,72);  //����  RGB
    DrawPaint->Canvas->Pen->Width=2;      //������ �����
    DrawPaint->Canvas->Arc(cx1-r1,cy1-r1,cx1+r1,cy1+r1,x1,y1,x2,y2);

    //����� �����
    DrawPaint->Canvas->MoveTo(x1,y1);
    DrawPaint->Canvas->LineTo(x2,y2);
    //��������� �������� ��������
    DrawPaint->Canvas->Pen->Color=clBlue;
    x1=cx2+cos((beta)*3.14159265/180)*r2;
    y1=cy2+sin((beta)*3.14159265/180)*r2;
    DrawPaint->Canvas->MoveTo(cx2,cy2);
    DrawPaint->Canvas->LineTo(x1,y1);

    //��������� �������
    DrawPaint->Canvas->Pen->Color=clWhite;
    DrawPaint->Canvas->Rectangle(x1-2,y1-2,x1+2,y1+2);

}








void __fastcall TForm1::BtnMagn1ResClick(TObject *Sender)
{
  // ���������� ��������� ������
    int Bnum = StrToInt(((TButton*)Sender)->Hint);

    if(Bnum == 5) // ������
    {
        Pnl_ResMVvod -> Visible = false;
        flagSBres = 0;
    }
    else if(Bnum == 4) // ����
    {
        if(flagSBres == 1)
        {
            magnRes1 = StrToFloat(EditRESmVvod->Text);
            MemoStat -> Lines -> Add(Label_Time -> Caption + " | ������ ���������� 1 ������� ��: " + EditRESmVvod->Text);
        }
        /*else if(flagSBres == 2)
        {
            magnRes2 = StrToFloat(EditRESmVvod->Text);
            MemoStat -> Lines -> Add(Label_Time -> Caption + " | ������ ���������� 2 ������� ��: " + EditRESmVvod->Text);
        }
        else if(flagSBres == 3)
        {
            magnRes3 = StrToFloat(EditRESmVvod->Text);
            MemoStat -> Lines -> Add(Label_Time -> Caption + " | ������ ���������� 3 ������� ��: " + EditRESmVvod->Text);
        } */
        Pnl_ResMVvod->Visible=false;
    }
    else
    {
        // ������ �� ��������� �������
        flagSBres = Bnum;
        EditRESmVvod->Text = "0";
        Pnl_ResMVvod -> Visible = true;
    }
}
//---------------------------------------------------------------------------



