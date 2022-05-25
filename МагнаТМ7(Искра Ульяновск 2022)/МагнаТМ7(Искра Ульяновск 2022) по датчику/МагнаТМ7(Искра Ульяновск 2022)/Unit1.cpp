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
#include "Modules\Com\Com.cpp"
#include "Modules\IVE\IVE.cpp"
#include "Modules\TRMD\TRMD.cpp"
#include "Modules\DatMTM9D\DatMTM9D.cpp"
#include "Modules\DatMTP4D\DatMTP4D.cpp"
#include "Modules\AZdrive\AZdrive.cpp"
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
//---- �������� � �������� ����� --------------------------------------------
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // ���������� ���� � �����
    Label_Time -> Caption = FormatDateTime("hh:mm:ss",Time());
    Label_Date -> Caption = FormatDateTime("dd.mm.yyyy",Date());

	//���������� ������ ��������� �� ������ �������
    PanelDiagm ->Left = 680;
    PanelDiagm ->Top = 190;
    PnlCondition->Left = 680;
    PnlCondition->Top = 330;

    PnlMnemo->DoubleBuffered = true;
    PanDIAGvak->DoubleBuffered = true;

    unsigned char i=0,j=0;

	// �������� ��������� �������
    TGroupBox *ZinParents[ZIN_COUNT*2] =
    {
        Form1->GB_zin0_1,
        Form1->GB_zin0_2,
        Form1->GB_zin1_1,
        Form1->GB_zin1_2,
        Form1->GB_zin2_1,
        Form1->GB_zin2_2
    };

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

        // �������� ����������
        Check_Zin[i][j] = new TImage(this);
        // ������������, �������
        if(j<=7)
        { Check_Zin[i][j] -> Parent = ZinParents[2*i];
          Check_Zin[i][j] -> Top = 36 + 36 * j;
        }
        else
        {
        Check_Zin[i][j] -> Parent = ZinParents[2*i+1];
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

	TGroupBox *OutParents[OUT_COUNT*2] =
	{
     Form1->GB_out0_1,
     Form1->GB_out0_2,
     Form1->GB_out1_1,
     Form1->GB_out1_2,
     Form1->GB_out2_1,
     Form1->GB_out2_2
	};

	// ���� �� ���������� ����������� ��� �����������
	for(i=0;i<OUT_COUNT;i++)
	{ // ���� �� ���������� ��� � ����������
	for(j=0;j<16;j++)
		{
        // �������� ���������� ��������
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
        Check_Out[i][j] -> OnDblClick = SetOutClick;
		}
	}

	TGroupBox *AinParents[AIK_COUNT] =
	{
		Form1->GB_ain0,
        Form1->GB_ain1
	};

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
        Title_Ain[i][j] -> Width = 440;
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
   TGroupBox *AoutParents[AOUT_COUNT] =
	{
        Form1->GB_aout0,
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
        Title_Aout[i][j] -> Width = 440;
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

    // ��������� ������ ��� ����������� ��������

    PnlMnemo -> DoubleBuffered = true;
    PCMain -> DoubleBuffered = true;

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
    serTemp[10] = Series10;
    serTemp[11] = Series21;
    serTemp[12] = Series23;
    // ������������� ������� ����������� �������� ��������
    serArh[1] = Series11;;
    serArh[2] = Series12;
    serArh[3] = Series13;
    serArh[4] = Series14;
    serArh[5] = Series15;
    serArh[6] = Series16;
    serArh[7] = Series17;
    serArh[8] = Series18;
    serArh[9] = Series19;
    serArh[10] = Series20;
    serArh[11] = Series22;
    serArh[12] = Series24;

    // ������ ������� ����������
    MemoRes -> Lines -> LoadFromFile("Res\\Res.txt");
    magnRes1 = StrToFloat( MemoRes -> Lines -> operator [](0) );
    magnRes2 = StrToFloat( MemoRes -> Lines -> operator [](1) );
    magnRes3 = StrToFloat( MemoRes -> Lines -> operator [](2) );

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
    PCNalad->ActivePage = TabSheet18;

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
    Label_ArhStat -> Caption = "�����: " + IntToStr(fileCount);
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
    Label_ArhGraph -> Caption = "�����: " + IntToStr(fileCount);
    ListBoxGraphArh -> Sorted;

    // ��������� ������ ���������
    fileCount = 0;
    ListBoxLibrary -> Clear();
    rezult = FindFirst("Lib\\*.txt", faAnyFile, SR);

    while ( !rezult )
    {
        fileCount++;
        SR.Name.SetLength( SR.Name.Length() - 4 );
        ListBoxLibrary -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };
    ListBoxLibrary -> Sorted;

    fileCount = 0;
    ListBoxLibraryT -> Clear();
    rezult = FindFirst("LibT\\*.txt", faAnyFile, SR);

    while ( !rezult )
    {
        fileCount++;
        SR.Name.SetLength( SR.Name.Length() - 4 );
        ListBoxLibraryT -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };
    ListBoxLibraryT -> Sorted;

    LoadGasNames();
    RenameGases();

    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> LoadFromFile("Nasmod\\Nasmod.txt");
    EditNastrTo0  -> Text = MemoNasmod -> Lines -> operator [](0);
    EditNastrTo22  -> Text = MemoNasmod -> Lines -> operator [](1);
    EditNastrTo2  -> Text = MemoNasmod -> Lines -> operator [](2);
    EditNastrTo1  -> Text = MemoNasmod -> Lines -> operator [](3);
    EditNastrTo4  -> Text = MemoNasmod -> Lines -> operator [](4);
    EditNastrTo5  -> Text = MemoNasmod -> Lines -> operator [](5);
    EditNastrTo6  -> Text = MemoNasmod -> Lines -> operator [](6);
    EditNastrTo7  -> Text = MemoNasmod -> Lines -> operator [](7);
    EditNastrTo29  -> Text = MemoNasmod -> Lines -> operator [](8);
    EditNastrTo30  -> Text = MemoNasmod -> Lines -> operator [](9);
    EditNastrTo31  -> Text = MemoNasmod -> Lines -> operator [](10);
    EditNastrTo11  -> Text = MemoNasmod -> Lines -> operator [](11);
    EditNastrTo23  -> Text = MemoNasmod -> Lines -> operator [](12);
    EditNastrTo15  -> Text = MemoNasmod -> Lines -> operator [](13);
    EditNastrTo12  -> Text = MemoNasmod -> Lines -> operator [](14);
    EditNastrTo16  -> Text = MemoNasmod -> Lines -> operator [](15);
    EditNastrTo8  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](16));
    EditNastrTo9  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](17));
    EditNastrTo10  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](18));
    EditNastrTo19  -> Text = MemoNasmod -> Lines -> operator [](19);
    EditNastrTo3  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](20));
    EditNastrTo25  -> Text = MemoNasmod -> Lines -> operator [](21);
    EditNastrTo35  -> Text = MemoNasmod -> Lines -> operator [](22);
    EditNastrTo36  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](23));
    EditNastrTo37  -> Text = MemoNasmod -> Lines -> operator [](24);
    EditNastrTo38  -> Text = MemoNasmod -> Lines -> operator [](25);
    EditNastrTo39  -> Text = MemoNasmod -> Lines -> operator [](26);
    EditNastrTo26  -> Text = MemoNasmod -> Lines -> operator [](27);
    EditNastrTo27  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](28));
    EditNastrTo34  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](29));
    EditNastrTo33  -> Text = MemoNasmod -> Lines -> operator [](30);
    MemoNasmod -> Lines -> Clear();
  /*
    // ��������� ��������� ����
    OpenISO_P32C32(); // ������� ��������� � ��������� ISO-P32C32
    OpenPISO_813();    // ������� ��������� � ��������� PISO-813
    OpenISO_DA16();    // ������� ��������� � ��������� ISO-DA16
    OpenPISO_DIO();

    OpenPCI1784();
       */
    Init_SComport();
    Comport[0]->Reser_Port(Comport[0]->BTN_reset);  // ��������� �����
    Comport[1]->Reser_Port(Comport[1]->BTN_reset);  // ��������� �����
    Comport[2]->Reser_Port(Comport[2]->BTN_reset);  // ��������� �����

    Init_BU_IVE();
    Init_TRMD();
    Init_DatMTM9D();
    Init_DatMTP4D();
    Init_SAZ_drive();
    AZdrive_Load();

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

    InitObjectsRRG();
    InitObjectsKl();

    // ��������� �������� � ������
    MemoStat -> Lines -> Add( "<<< " + Label_Time->Caption + " | ��������� �������� >>>");

        // ��������� �������� T
    MemoT -> Lines -> LoadFromFile("Nasmod\\Mex.txt");
    EdtTRed1  -> Text = MemoT -> Lines -> operator [](0);
    MemoT -> Lines -> Clear();

    // ������� ������� �������� ������������ �������
    BtnAzDaClick(BtnAzDa);
    BtnAutoDaClick(BtnAutoDa);
    BtnNastrDaClick(BtnNastrDa);
    BtnNalDaClick(BtnNalDa);
    BtnTrDaClick(BtnTrDa);
	
    ust_ready = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Btn_ExitClick(TObject *Sender)
{
  Form1->Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    // ���������� � ��������� ������������ ������
    TimerExist -> Terminate();
    LogicThread-> Terminate();

 /*   // ��������� ����� � ������� ISO
    ISO_DriverClose();
    ISODA_DriverClose();
    PISO813_DriverClose();

    ClosePCI1784(); // ������� PCI-1784
                        */
    // �������� ������
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

    Save_Stat();
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

    // ��������� �������� ������� ����������
    MemoRes -> Lines -> Clear();
    MemoRes -> Lines -> Add(FloatToStr(magnRes1));
    MemoRes -> Lines -> Add(FloatToStr(magnRes2));
    MemoRes -> Lines -> Add(FloatToStr(magnRes3));
    MemoRes -> Lines -> SaveToFile("Res\\Res.txt");
}
//---------------------------------------------------------------------------
//---- ������� -----------------------------------------------------
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
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer250Timer(TObject *Sender)
{
	// ����������� ���� � �������
    Label_Time -> Caption = FormatDateTime("hh:mm:ss",Time());
    Label_Date -> Caption = FormatDateTime("dd.mm.yyyy",Date());

    // ���� ��� �������� ��������������� �������� ���������� ��
    float
        ugol = (float)zaslUgolAbs - (float)nasmod[2];
    while ( ugol > zaslAngle360 ) ugol -= zaslAngle360;
    while ( ugol < 0 ) ugol += zaslAngle360;
    if ( ! zaslPrNeopr)
        Form1 -> DrawZasl(((int)(180.0-(((int)(ugol/zaslAngle360*360.0 + 135.0))%360)))%360);
    else
        Form1 -> DrawZasl(135);
}
//--------------------------------------------------------------------------
void __fastcall TForm1::Timer500Timer(TObject *Sender)
{
	if(!ust_ready) return;
	// ������������ ����������
	VisualMnemo();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1secTimer(TObject *Sender)
{
	if(!ust_ready) return;
    VisualGraph();
}
//---------------------------------------------------------------------------
void TForm1::ExternalManager()  // ��������� ���� �����/������
{       /*
	// ���� ������� ������� �������
	if(pr_otl) return;
    
    // ���������� PortState ��������� ������� ������
    // ������ ISO-P32C32
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
                // ��������� ��� ������
            }; break;
       }
    }
    // ������ PEX-P16R16(1)
    if ( externalTask & 0x02 )
    {
        // ��������
        externalError = PISO_P16R16U_1( 0 , zin );
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
                // ��������� ��� ������
            }; break;
       }
    }
    // ������ PEX-P16R16(2)
    if ( externalTask & 0x04 )
    {
        // ��������
        externalError = PISO_P16R16U_2( 0 , zin );
        // ������ ������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x04);
                // ����� ������
                externalTask &= (~0x04);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x04;
                // ��������� ��� ������
            }; break;
       }
    }

    // ������ ���������� ������� �������� � ISO-813
    else if(externalTask & 0x08)
    {
        // ��������
        externalError = PISO_813U(aik , AIK_COUNT * 8);
        // ������ ������
        switch ( externalError )
        {
            case 0:
            {
                // ����� �����������
                diagnS[1] &= (~0x08);
                // ����� ������
                externalTask &= (~0x08);
            }; break;
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x08;
            }; break;
        }
    }
	
    // �������� � ISO-P32�32
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
            }; break;
        }
    }
    // �������� � PEX-P16R16(1)
    else if(externalTask & 0x20)
    {
         // ��������
         externalError = PISO_P16R16U_1( 1 , out );
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
            }; break;
        }
    }
    // �������� � PEX-P16R16(2)
    else if(externalTask & 0x40)
    {
         // ��������
         externalError = PISO_P16R16U_2( 1 , out );
        // ������ �������
        switch ( externalError )
        {
            // ������ ���
            case 0:
            {
                // �������� �����������
                diagnS[1] &= (~0x04);
                // ���� ��� ������ ����� ����� ������
                externalTask &= (~0x40);
            }; break;
            // ���� ������ �����
            default:
            {
                // ��������� �����������
                diagnS[1] |= 0x04;
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
                    diagnS[1] &= (~0x10);
                }; break;
                default:
                {
                    // ��������� �����������
                    diagnS[1] |= 0x10;
                }; break;
            }
        }
        externalTask &= (~0x80);
    }
    else
    {
        externalTask = 0xFF;
    }               */
}
//---------------------------------------------------------------------------
//---- ������������ ������� -------------------------------------------------
//---------------------------------------------------------------------------
void TForm1::VisualMnemo() // 500��
{
    if(!ust_ready) return;
    // ������������ ���-�� �������� Logic
    Form1 -> EdtLogicPerSecond -> Text = IntToStr(2*logicPerSecond);
    logicPerSecond = 0;
    // ����� �� ����� ������� �������
    Form1 -> EditTLogic -> Text = FloatToStrF(logic_time,ffFixed,6,3);

    VisualZagol();      // ������������ ��������� ���������
    VisualVoda();       // ������������ ����
    VisualColorElement(); // ������������ ��������� ����������
    VisualParam();      // ����������� ������� �������� ����������
    VisualButtons();    // ����������� ������
    VisualDiagn();      // ����������� ���������� �� ����������
	VisualOperatorDlg();// ������������ ������� ���������
    VisualFormat();     // ������������ �������
    VisualNasmod();     // ������������ �������� ������������ �������
    VisualDebug();      // ������������ �������� �������
}
//---------------------------------------------------------------------------
//--������������ ���������--//
//---------------------------------------------------------------------------
void TForm1::VisualZagol()
{
    AnsiString TempStr = "";
    // �����
    EditNormName -> Text = NormNames[norma];
    // ���������� �������, ��� ���� ���� ����������
    #define SHR_VALUE_COUNT 6
    // ������� ���������� �������� ����� (���� �����)
    unsigned char SHRValue[SHR_VALUE_COUNT] = {2, 3, 9, 4, 5, 1};
    // ����������� ���������� ������� � ������� �������� ����������
    for ( int i = 0 ; i < SHR_VALUE_COUNT ; i++ )
        if ( shr[SHRValue[i]] )
        {
            Form1 -> EditRName -> Text = SHRNames[SHRValue[i]];
            switch ( SHRValue[i] )
            {
                case 1: Form1 -> EditSHRName -> Text = SHR1Names[shr[SHRValue[i]]] ;break;
                case 2: Form1 -> EditSHRName -> Text = SHR2Names[shr[SHRValue[i]]] ;break;
                case 3: Form1 -> EditSHRName -> Text = SHR3Names[shr[SHRValue[i]]] ;break;
                case 4: Form1 -> EditSHRName -> Text = SHR4Names[shr[SHRValue[i]]] ;break;
                case 5: Form1 -> EditSHRName -> Text = SHR5Names[shr[SHRValue[i]]] ;break;
                case 9: Form1 -> EditSHRName -> Text = SHR9Names[shr[SHRValue[i]]] ;break;
            }
        // ����������� ����� ������ �������
            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 7 ) )
            {
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            }
            if ( ( SHRValue[i] == 3 ) && ( shr[SHRValue[i]] == 2 ) )
            {
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            }
            if ( ( SHRValue[i] == 9 ) && ( shr[SHRValue[i]] == 5 ) )
            {
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            }
            if ( ( SHRValue[i] == 9 ) && ( shr[SHRValue[i]] == 6 ) )
            {
                Form1 -> EditSHRName -> Text = SHR5Names[shr[5]];
            }
        // ����������� ��������

          /*  if ( shr[1] == 15 )
            {
                TempStr = SHR1Names[15] + IntToStr(int((nasmod[33]-CT_KN)/60)) + " ��� ";
                TempStr = TempStr + IntToStr(nasmod[33] - CT_KN - 60 * int( float(nasmod[33]- CT_KN) / 60.0)) + " ���";
                Form1 -> EditSHRName -> Text = TempStr;
            } */

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 10 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - CT_2 );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 16 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 23 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 30 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( nasmod[5] - CT_2 );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 33 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr (  par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 37 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( nasmod[7] - CT_2 );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 40 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr (  par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 46 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( nasmod[29] - CT_2 );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 48 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }
            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 68 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

           /* if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 49 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }     */

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 54 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 2 ) && ( shr[SHRValue[i]] == 57 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 3 ) && ( shr[SHRValue[i]] == 7 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 3 ) && ( shr[SHRValue[i]] == 13 ) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }

            if ( ( SHRValue[i] == 3 ) && ( shr[SHRValue[i]] == 21) )
            {
                TempStr = Form1 -> EditSHRName -> Text;
                Form1 -> EditSHRName -> Text = TempStr + IntToStr ( par[N_ST][5] - T_PROC );
            }
            return;
            
        }
    // ��� �������� ������� �������� ����
    Form1 -> EditRName -> Text = "";
    Form1 -> EditSHRName -> Text = "";
}
//---------------------------------------------------------------------------
//--������������ ����������� ����--//
//---------------------------------------------------------------------------
TColor TForm1::VodaActivityColor( bool value )
{
    if ( value ) return 0x00EAD999;
    else         return 0x004040FF;
}
//---------------------------------------------------------------------------
void TForm1::VisualVoda()
{

    // ���������� ����� 1
    PnlKan0 -> Color = VodaActivityColor( zin[0] & 0x01 );
    // ���������� ����� 2
    PnlKan10 -> Color = VodaActivityColor( zin[0] & 0x02 );
    // ���������� ����� 3
    PnlKan20 -> Color = VodaActivityColor( zin[0] & 0x04 );
    // ���������� ����� 4
    PnlKan30 -> Color = VodaActivityColor( zin[0] & 0x08 );
    // ���������� ����� 5
    PnlKan40 -> Color = VodaActivityColor( zin[0] & 0x400 );
    // �������� �� �����
    if(aik[10] < 540)
        PnlKan50 -> Color = VodaActivityColor( 0 );
    else   PnlKan50 -> Color = VodaActivityColor( 1);

    // �������� ����� 1
    PnlKan00 -> Caption = FloatToStrF( ( (float)aik[0] * 10.0 - 4095.0 ) / 163.8, ffFixed, 5, 0 ) + " �C";
    if((((float)aik[0] * 10.0 - 4095.0 ) / 163.8 ) < 0 ) PnlKan00->Caption = "0 �C";
    // �������� ����� 2
    PnlKan01 -> Caption = FloatToStrF( ( (float)aik[1] * 10.0 - 4095.0 ) / 163.8, ffFixed, 5, 0 ) + " �C";
    if((((float)aik[1] * 10.0 - 4095.0 ) / 163.8 ) < 0 ) PnlKan01->Caption = "0 �C";
    // �������� ����� 3
    PnlKan02 -> Caption = FloatToStrF( ( (float)aik[2] * 10.0 - 4095.0 ) / 163.8, ffFixed, 5, 0 ) + " �C";
    if((((float)aik[2] * 10.0 - 4095.0 ) / 163.8 ) < 0 ) PnlKan02->Caption = "0 �C";
    // �������� ����� 4
    PnlKan03 -> Caption = FloatToStrF( ( (float)aik[3] * 10.0 - 4095.0 ) / 163.8, ffFixed, 5, 0 ) + " �C";
    if((((float)aik[3] * 10.0 - 4095.0 ) / 163.8 ) < 0 ) PnlKan03->Caption = "0 �C";
    // �������� ����� 5
    PnlKan04 -> Caption = FloatToStrF( ( (float)aik[9] * 10.0 - 4095.0 ) / 163.8, ffFixed, 5, 0 ) + " �C";
    if((((float)aik[9] * 10.0 - 4095.0 ) / 163.8 ) < 0 ) PnlKan04->Caption = "0 �C";

    // �������� ����� ���� ���� �� �����
    PnlKan05 -> Caption = FloatToStrF((((float)aik[10]/409.5) - 2.032)/8.128 * 9.8, ffFixed, 5, 2) + " ���";
    if(((((float)aik[10]/409.5) - 2.032)/8.128 * 9.8 ) < 0 ) PnlKan05->Caption = "0 ���";
    

    // �������� ����
    Edt_AZ_1_1mn -> Text = IntToStr(par[0][13]);

    // ���������� ����
    Edt_AZ_1_2mn -> Text = IntToStr(TEK_ABS_DZ);

    // ������������� ����
    Edt_AZ_1_3mn -> Text = IntToStr(TEK_OTN_DZ);
}
//---------------------------------------------------------------------------
//--������������ ���������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualParT()
{
    EdtTKon1 -> Text = FloatToStrF((float)par_t[0], ffFixed, 5, 0);
}
//---------------------------------------------------------------------------
//--�������� ���������� ���������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParAzClick(TObject *Sender)
{
       PanelParAz -> Visible = true;
}
//---------------------------------------------------------------------------
//--����� �� �������� ���������� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnAzNetClick(TObject *Sender)
{
    PanelParAz -> Visible = false;
}
//---------------------------------------------------------------------------
//--������������� �������� ���������� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnAzDaClick(TObject *Sender)
{
    // ������ ������������� �������� ������
    PanelParAz -> Visible = false;

    par_t[0]  = StrToInt(EdtTRed1->Text);


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "  �������� ��������� ����������:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    if ( EdtTKon1 -> Text != EdtTRed1 -> Text )
        MemoStat -> Lines -> Add("���� ������������ ������� �� ��������� ���������: " + EdtTKon1 -> Text + " -> " + EdtTRed1 -> Text );


    // ����������� ���������� ���������
    EdtTRed1 -> Color = clWhite;


    // �������� ��������
    VisualParT();
    MemoT -> Lines -> Clear();
    MemoT -> Lines -> Add(EdtTKon1->Text);


    MemoT -> Lines -> SaveToFile("Nasmod\\Mex.txt");
}
//---------------------------------------------------------------------------
//--������������ ��������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualColorElement()
{
    anim_fase = !anim_fase;   // ����� ���� ��������
    //�������� ������������
            // �������
    if((diagn[11]))
	{
		SetOut(1,2,0x400); //
	}
    else
	{
		SetOut(0,2,0x400); //
	}

    // ������
    if((pr_yel)||(!shr[1]&&!shr[2]&&!shr[3]&&!shr[4]&&!shr[5]&&!shr[6]&&!shr[7]&&!shr[9]))
	{
		SetOut(1,2,0x200); //
	}
    else
	{
		SetOut(0,2,0x200); //
	}

    // �������
    if(shr[1]||shr[2]||shr[3]||shr[4]||shr[5]||shr[6]||shr[7]||shr[9])
	{
		SetOut(1,2,0x100); //
	}
    else
	{
		SetOut(0,2,0x100); //

	}



    // ������� � �����

    // ��1
    if ( out[1] & 0x20 ) kl_1 -> Picture = e_klg_o_vert -> Picture;
    else                 kl_1 -> Picture = 0;
    // ��2
    if ( out[1] & 0x40 ) kl_2 -> Picture = e_klg_o_vert -> Picture;
    else                 kl_2 -> Picture = 0;
    // ��3
    if ( out[1] & 0x080 ) kl_3 -> Picture = e_klg_o_vert -> Picture;
    else                  kl_3 -> Picture = 0;
    // ��4
    if ( out[1] & 0x2000) kl_4 -> Picture = e_klg_o_vert -> Picture;
    else                  kl_4 -> Picture = 0;
    // ��5
    if ( out[1] & 0x4000) kl_5 -> Picture = e_klg_o_vert -> Picture;
    else                  kl_5 -> Picture = 0;
    // ��-���
    if ( out[1] & 0x8000 ) kl_nap -> Picture = e_klg_o -> Picture;
    else                   kl_nap -> Picture = 0;

    //������� �������
	if(VRTEMP&&shr[27])
	{
		if(anim_fase) { plazma->Visible=true;  }
		else          { plazma->Visible=false; }
	}
	else { plazma -> Visible = false; }

     // ��-���
        switch ( zin[0] & 0x3000 )
    {
        // �� ����������
        case 0x0000: kl_kam -> Picture = e_klg_n_vert -> Picture; break;
        // ������
        case 0x1000: kl_kam -> Picture = e_klg_o_vert -> Picture; break;
        // ������
        case 0x2000: kl_kam -> Picture = 0; break;
        // ������������
        case 0x3000: kl_kam -> Picture = e_klg_n_vert -> Picture; break;
    }
    // ��-��
        switch ( zin[0] & 0xC000 )
    {
        // �� ����������
        case 0x0000: kl_kn -> Picture = e_klg_n -> Picture; break;
        // ������
        case 0x4000: kl_kn -> Picture = e_klg_o -> Picture; break;
        // ������
        case 0x8000: kl_kn -> Picture = 0; break;
        // ������������
        case 0xC000: kl_kn -> Picture = e_klg_n -> Picture; break;
    }
    // ��
    if(shr[39])
    {
     dz -> Picture = dros -> Picture;
    }
    else
    {
        switch ( zin[1] & 0xC000 )
    {
        // �� ����������
        case 0x0000: dz -> Picture = e_dz_n -> Picture; break;
        // ������
        case 0x4000: dz -> Picture = e_dz -> Picture; break;
        // ������
        case 0x8000: dz -> Picture = 0; break;
        // ������������
        case 0xC000: dz -> Picture = e_dz_n -> Picture; break;
    }
    }
    // ��
    /*  ����
    if(zin[2] & 0x02) // ������
       tmn -> Picture = e_tmn_a -> Picture;
    else
    {   if(shr[1]>7&&shr[1]<=15)
        {
        if(anim_fase) { tmn -> Picture = e_tmn -> Picture;  }
		else          { tmn -> Picture = tmn_e -> Picture; }

        }
        else if ( zin[2] & 0x01 )            // �������
                tmn -> Picture = e_tmn -> Picture;
        else
                tmn -> Picture = tmn_e -> Picture;
    }  */
        if((out[2] & 0x01) && (aik[15] > nasmod[33]))
        {
            if(anim_fase) { Kn -> Picture = e_tmn -> Picture; }
		    else          { Kn -> Picture = tmn_e -> Picture; }

        }
        else if ( (out[2] & 0x01) && (aik[15] <= nasmod[33]))            // �������
                Kn -> Picture = e_tmn -> Picture;
        else
                Kn -> Picture = tmn_e -> Picture;
    //������
    if(shr[27])
        nagr -> Visible = true;
    else
        nagr -> Visible = false;

    // ���
    if ( zin[1] & 0x01 ) fvn -> Picture = e_fvn -> Picture;
    else                 fvn -> Picture = 0;
    // ���1
    if ( shr[20]||shr[23] ) rrg1 -> Visible = true;
    else           rrg1 -> Visible = false;
    // ���2
    if ( shr[21] ) rrg2 -> Visible = true;
    else           rrg2 -> Visible = false;

    // ��� 1
    if ( shr[29] || shr[7] ) m1 -> Visible = true;
    else           m1 -> Visible = false;
    // ��� 1 ��������
    if ( (shr[29]||shr[7]) && VRBM )
    {
        if(anim_fase) m1_a -> Visible = true;
        else m1_a -> Visible = false;
    }
    else m1_a -> Visible = false;
    // ��� 2
    if ( shr[30] || shr[8] ) m2 -> Visible = true;
    else           m2 -> Visible = false;
    // ��� 2 ��������
    if ( (shr[30]||shr[8]) && VRBM )
    {
        if(anim_fase) m2_a -> Visible = true;
        else m2_a -> Visible = false;
    }
    else m2_a -> Visible = false;
    // ��� 3
    if ( shr[34] || shr[10] ) vchm -> Visible = true;
    else           vchm -> Visible = false;
    // ��� 3 ��������
    if ( (shr[34]|| shr[10]) && VRGIS )
    {
        if(anim_fase) vchm_a -> Visible = true;
        else vchm_a -> Visible = false;
    }
    else vchm_a -> Visible = false;
    // ��
    if ( shr[32] ) ii -> Visible = true;
    else           ii -> Visible = false;
    // �� ��������
    if ( shr[32] && VRII )
    {
        if(anim_fase) ii_a -> Visible = true;
        else ii_a -> Visible = false;
    }
    else ii_a -> Visible = false;
    // �������� ��������
    if(out[0] & 0x02)
       strelka -> Visible = true;
    else
       strelka -> Visible = false;
    // �������� ��������
    if(zin[1] & 0x04)
       ish -> Visible = true;
    else
       ish -> Visible = false;
    // ������ �����
    if(zin[1] & 0x10)
       kr_d -> Visible = true;
    else
       kr_d -> Visible = false;
    // ������ ������
    if(zin[1] & 0x20)
       kr_u -> Visible = true;
    else
       kr_u -> Visible = false;
    // ������� ������
    if(zin[1] & 0x40)
       kr_rot -> Visible = true;
    else
       kr_rot -> Visible = false;
    // �������� ������
    if(shr[44] == 2)
        kr_napr -> Picture = e_kr_up -> Picture;
    else if(shr[45] == 2)
        kr_napr -> Picture = e_kr_down -> Picture;
    else kr_napr -> Picture = 0;

    // ������������
    if( shr[20] ||shr[23]) tube_01 -> Visible = true;
    else          tube_01 -> Visible = false;
    if( shr[21] ) tube_02 -> Visible = true;
    else          tube_02 -> Visible = false;

    if(((out[1]&0x20)&&(tube_01->Visible))||
       ((out[1]&0x40)&&(tube_02->Visible))||shr[23])
                tube_03 -> Visible = true;
    else        tube_03 ->  Visible = false;

    if(out[1]&0x80&&(tube_01->Visible))
                tube_05 -> Visible = true;
    else        tube_05 -> Visible = false;

    if(zin[1]&0x01 || out[1]&0x2000)
                tube_11 -> Visible = true;
    else        tube_11 -> Visible = false;

    if((zin[0]&0x1000)&&(tube_11->Visible)){
                tube_09_1 -> Visible = true;
                tube_09_2 -> Visible = true;
                tube_09_3 -> Visible = true;
    }
    else{
                tube_09_1 -> Visible = false;
                tube_09_2 -> Visible = false;
                tube_09_3 -> Visible = false;
    }
    if((zin[0]&0x4000)||(zin[2] & 0x01))
                tube_10 -> Visible = true;
    else        tube_10 -> Visible = false;

    if(!(zin[1]&0x8000)&&tube_10 -> Visible)
                tube_08 -> Visible = true;
    else        tube_08 -> Visible = false;

    if((out[1]&0x8000)||(out[1]&0x4000)){
                tube_07_1 -> Visible = true;
                tube_07_2 -> Visible = true;
    }
    else{
                tube_07_1 -> Visible = false;
                tube_07_2 -> Visible = false;
    }
}
//---------------------------------------------------------------------------
//--������������ �������� �������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualParam()
{

    // ����������� ������� �������� ����������
    // �������� �1 (MTM9D)
    if ((D_D1 >= 0) && (D_D1 <= 10000))
    davl_D1 -> Caption = FloatToStrF(100.0*pow(10.0,(float(D_D1)/1000.0-6.8)/0.6),ffExponent,3,8);
    // �������� �2 (MTP4D)
    if ((D_D2 > 1) && (D_D2 <= 10000)) davl_D2 -> Caption = FloatToStrF(100.0*pow(10.0,(float(D_D2)/1000.0-5.5)),ffExponent,3,8);
    else davl_D2 -> Caption = "��� ����.";

    //�������� � ������ ���1/���2
    d_drrg1 -> Caption = FloatToStrF((((float)aik[7]/409.5) - 2.032)/8.128 * 9.8, ffFixed, 5, 2) + " ���";
    d_drrg2 -> Caption = FloatToStrF((((float)aik[8]/409.5) - 2.032)/8.128 * 9.8, ffFixed, 5, 2) + " ���";

    // ������������ � ������ ������� ���������� 1
    if ( shr[29] && ( ( X_TBM * BPM1_P_MAX / 4095 ) > 100 ) )
        magnRes1 += StrToFloat((float)X_TBM/4095.0*BPM1_P_MAX/3600.0/1000.0);
    Form1->EditRESm1->Text = FloatToStrF(magnRes1,ffFixed,6,3);
    // ������������ � ������ ������� ���������� 2
    if ( shr[30] && ( ( X_TBM * BPM2_P_MAX / 4095 ) > 100 ) )
        magnRes2 += StrToFloat((float)X_TBM/4095.0*BPM2_P_MAX/3600.0/1000.0);
    Form1->EditRESm2->Text = FloatToStrF(magnRes2,ffFixed,6,3);

    // ������������ � ������ ������� ���������� ���
    if ( shr[34] && ( ( aik[13] * COMET_MAX_PD / 4095 ) > 30 ) )
        magnRes3 += StrToFloat((float)aik[13]/4095.0*COMET_MAX_PD/3600.0/1000.0);
    Form1->EditRESVCHM->Text = FloatToStrF(magnRes3,ffFixed,6,3);

    COEF_SOGL_ZAD->Caption = FloatToStrF(1000.0/(float)nasmod[35],ffFixed,5,0);       // �������� ����������� ������������ ���

  //����������� ������������ �������

  if(shr[34]) 
  {
        if(N_TEK_GIS!=0) { COEF_SOGL_TEK->Caption = FloatToStrF(1000.0/(float)N_TEK_GIS,ffFixed,5,0); } // ������� ����������� ������������
        else             { COEF_SOGL_TEK->Caption = 0;}
  }
  else             { COEF_SOGL_TEK->Caption = 0;}
  //nasmod[35] = (unsigned int)(1000.0/(float)StrToFloat(EditNastrTo35->Text));
  //N_TEK_GIS
                                              
  // ����������� ��
    LbKN -> Caption = FloatToStrF(float(aik[15])*350/2047.0,ffFixed,5,0);

     // �������
    if ( !mnemoInAuto )
    {
        // �������
        ProcessName -> Caption = " ";
        
		// ������� ����� ������ ���� �� �� � ����������
		if(!shr[2]&&!shr[3]&&!shr[4])	
		{
			// ���1
			if(shr[20])
				EdtZadA00 -> Text = FloatToStrF((float)ObjRRG[0]->parRRG*RRG1_MAX/4095, ffFixed, 6, 2);
			else if ( shr[23] )
				EdtZadA00 -> Text = FloatToStrF((float)U_PUN*RRG1_MAX/4095, ffFixed, 6, 2); //U_PUN
			else
				EdtZadA00 -> Text = "0,00";					
			// ���2
			EdtZadA01 -> Text = EdtRTek1 -> Text;			
			// �����������
			EdtZadA02 -> Text = EdtRTek12 -> Text;								
			// ��� ��
			EdtZadA14 -> Text = EdtRTek7 -> Text;
			// �������� �1
			EdtZadA03 -> Text = EdtRTek8 -> Text;
			// �������� �2
			EdtZadA04 -> Text = EdtRTek9 -> Text;
			// ��� �������� ���
			EdtZadA12 -> Text = EdtRTek10 -> Text;
			// ��������
			EdtZadA09 -> Text = EdtRTek2 -> Text;
			// ������� �������� ��
			EdtZadA10 -> Text = EdtRTek6 -> Text;
		}
		else
		{
           ProcessName -> Caption = " "; ///

			EdtZadA00 -> Text = "";
			EdtZadA01 -> Text = "";
			EdtZadA02 -> Text = "";
			EdtZadA14 -> Text = "";
			EdtZadA03 -> Text = "";
			EdtZadA04 -> Text = "";
			EdtZadA12 -> Text = "";
			EdtZadA09 -> Text = "";
			EdtZadA10 -> Text = "";
		}

		// ������� ����� ������
		
		// ������ ���1
		EdtTekA00 -> Text = FloatToStrF((float)aik[4]*RRG1_MAX/4095, ffFixed, 6, 2);
		// ������ ���2
		EdtTekA01 -> Text = FloatToStrF((float)aik[5]*RRG2_MAX/4095, ffFixed, 6, 2);
		// �����������
		EdtTekA02 -> Text = FloatToStrF((float)TEK_TEMP/10.0, ffFixed, 6, 1);
		// ��� ��
        if(shr[32])
		    EdtTekA14 -> Text = FloatToStrF((float)OTVET_II[5]*512.0/1023, ffFixed, 6, 0);
        else
            EdtTekA14 -> Text = "0";


        // ���������� ��
        if(shr[32])
		    EdtTekA15 -> Text = FloatToStrF((float)OTVET_II[4]*3072/1024, ffFixed, 6, 0); //���������� ��
        else
            EdtTekA15 -> Text = "0";


		// �������� �1
		if ( shr[29] || shr[7] )
            EdtTekA03 -> Text = FloatToStrF(OTVET_BM[6]*3072.0/1023.0, ffFixed, 6, 0);
        else
            EdtTekA03 -> Text = "0";
		// �������� �2
        if ( shr[30] || shr[8] )
		    EdtTekA04 -> Text = FloatToStrF(OTVET_BM[6]*3072.0/1023.0, ffFixed, 6, 0);
        else
            EdtTekA04 -> Text = "0";
		// ���������� �
		if( shr[7] || shr[8] ||shr[10] || shr[29] || shr[30] || shr[34])
			EdtTekA06 -> Text = FloatToStrF((float)OTVET_BM[4]*665.6/1023.0, ffFixed, 6, 0);
		else
			EdtTekA06 -> Text = "0";
		// ��� �
		if( shr[7] || shr[8] ||shr[10] || shr[29] || shr[30] || shr[34])
			EdtTekA07 -> Text = FloatToStrF((float)OTVET_BM[5]*10230.0/1023.0/1000.0, ffFixed, 8, 1);
		else
			EdtTekA07 -> Text = "0,0";


		// ��� �������� ���
        if ( shr[34] || shr[10] )
		    EdtTekA12 -> Text = FloatToStrF((float)aik[13]*COMET_MAX_PD/4095.0, ffFixed, 6, 0); // �������� �������� ��� ��
        else
            EdtTekA12 -> Text = "0";
        // ��� �������� ���
        if ( shr[34] || shr[10] )
		    EdtTekA13 -> Text = FloatToStrF((float)aik[14]*COMET_MAX_PD/4095.0, ffFixed, 6, 0); // ���������� �������� ��� ��
        else
            EdtTekA13 -> Text = "0";


		// �������������
		EdtTekA08 -> Text = FloatToStrF ((float)SOPR/10.0, ffFixed, 7, 1);
		// ��������
		if(shr[23])
			EdtTekA09 -> Text = FloatToStrF(100.0*pow(10.0,(float(D_D1)/1000.0-6.8)/0.6),ffFixed, 5, 1);
		else
			EdtTekA09 -> Text = "0,00";
		// ���� �������� �� // ���������� ������, x�� 100% ��� par_t[0], ���� �������
		EdtTekA10 -> Text = FloatToStrF(((float) TEK_ABS_DZ*100.0/par_t[0]),ffFixed, 5, 1);  // ���� TEK_ABS_DZ/10000*100

	}
	
    // �������
    else
    {
        if(shr[2]||shr[3]||shr[4])
        {
            switch(N_ST)
            {
                case 0: {
                ProcessName -> Caption = " ";
                }; break;
                case 1: {
                ProcessName -> Caption = "��������������� ������";
                }; break;
                case 2: {
                ProcessName -> Caption = "������ �������";
                }; break;
                case 3: {
                ProcessName -> Caption = "������";
                }; break;
                case 4: {
                ProcessName -> Caption = "���������: ���� 1";
                }; break;
                case 5: {
                ProcessName -> Caption = "���������: ���� 2";
                }; break;
                case 6: {
                ProcessName -> Caption = "���������: ���� 3";
                }; break;
                case 7: {
                ProcessName -> Caption = "�����";
                }; break;
                case 8: {
                ProcessName -> Caption = "���������";
                }; break;
                case 9: {
                ProcessName -> Caption = "���������� �1";
                }; break;
                case 10: {
                ProcessName -> Caption = "���������� �2";
                }; break;
                case 11: {
                ProcessName -> Caption = "���������� ���";
                }; break;
                default:
                ProcessName -> Caption = " ";
                break;
            };
			
			// �������
			
			// ������ ���1 (� ���)
			if(shr[20])
				EdtZadA00 -> Text = FloatToStrF((float)ObjRRG[0]->parRRG*RRG1_MAX/4095, ffFixed, 6, 2);
			else if(shr[23])
				EdtZadA00 -> Text = FloatToStrF((float)PAR_UN*RRG1_MAX/4095, ffFixed, 6, 2);   //U_PUN
			else EdtZadA00 -> Text = "";
			
			// ������ ���2
			if(par[N_ST][1]&&N_ST)
				EdtZadA01 -> Text = FloatToStrF((float)par[N_ST][1]*RRG2_MAX/4095, ffFixed, 6, 2);
			else
				EdtZadA01 -> Text = "";

			// �����������
			if((nasmod[27])&&(N_ST==3))
				EdtZadA02 -> Text = FloatToStrF((float)par[3][12]/10.0, ffFixed, 5, 1);
			else if(par[N_ST][12] && N_ST)
				EdtZadA02 -> Text = FloatToStrF((float)par[N_ST][12]/10.0, ffFixed, 5, 1);
			else
				EdtZadA02 -> Text = "";

			// ��� ��
			if(par[N_ST][3] && N_ST)
				EdtZadA14 -> Text = FloatToStrF((float)par[N_ST][3]/4095.0*I_MAX, ffFixed, 5, 0);
			else
				EdtZadA14 -> Text = "";

			// �������� �1
			if(par[N_ST][8] && N_ST)
				EdtZadA03 -> Text = FloatToStrF((float)PAR_BM1/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
			else
				EdtZadA03 -> Text = " ";
			
			// �������� �2
			if(par[N_ST][9] && N_ST)
				EdtZadA04 -> Text = FloatToStrF((float)PAR_BM2/4095.0*BPM2_P_MAX, ffFixed, 5, 0);
			else
                EdtZadA04 -> Text = "";

			// ��� �������� ���
			if(par[N_ST][10] && N_ST)
				EdtZadA12 -> Text = FloatToStrF((float)PAR_GIS /4095.0*COMET_MAX_PD, ffFixed, 5, 0);
			else
                EdtZadA12 -> Text = "";

			// �������������
			if(par[N_ST][17] && N_ST)
                EdtZadA08 -> Text = FloatToStrF ((float)par[N_ST][17]/10.0, ffFixed, 7, 1);
			else
                EdtZadA08 -> Text = "";
			
			// ��������
          /*  if(par[N_ST][2] && N_ST )
                EdtZadA09 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[N_ST][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
		
            else
                EdtZadA09 -> Text = ""; */
            if(par[N_ST][2] && N_ST && (PR_DAVL_PODJ == 0))
                EdtZadA09 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[N_ST][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
			else if(par[N_ST][15] && N_ST && (PR_DAVL_PODJ == 1))
                EdtZadA09 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[N_ST][15])/1000.0-6.8)/0.6),ffFixed, 5, 1);
            else
                EdtZadA09 -> Text = "";
			
			// ������� �������� ��
			if(par[N_ST][6] && N_ST)
                EdtZadA10 -> Text = FloatToStrF((float)par[N_ST][6], ffFixed, 5, 1);      //*100.0/par_t[0]   (float)par[N_ST][6]/100.0
			else
                EdtZadA10 -> Text = "";
			
			// ����� ��������
			if(par[N_ST][5] && N_ST)
                EdtZadA11 -> Text = FloatToStrF((float)par[N_ST][5], ffFixed, 5, 0);
			else
                EdtZadA11 -> Text = "";

			// �������			
			// ������ ���1
			if(shr[20]||shr[23])
				EdtTekA00 -> Text = FloatToStrF((float)aik[4]*RRG1_MAX/4095, ffFixed, 6, 2);
			else EdtTekA00 -> Text = "0,00";
			// ������ ���2
			if(shr[21])
				EdtTekA01 -> Text = FloatToStrF((float)aik[5]*RRG2_MAX/4095, ffFixed, 6, 2);
			else EdtTekA01 -> Text = "0,00";			
			// �����������
			EdtTekA02 -> Text = FloatToStrF((float)TEK_TEMP/10.0, ffFixed, 6, 1);
			// ��� ��
			if ( shr[32] || (!mnemoInAuto) )
				EdtTekA14 -> Text = FloatToStrF((float)OTVET_II[5]*512.0/1023, ffFixed, 6, 0);
			else EdtTekA14 -> Text = "0";
            // ���������� ��
			if ( shr[32] || (!mnemoInAuto) )
				EdtTekA15 -> Text = FloatToStrF((float)OTVET_II[4]*3072/1024, ffFixed, 6, 0);
			else EdtTekA15 -> Text = "0";
			// �������� �1
			if ( shr[29] || shr[7] )
				EdtTekA03 -> Text = FloatToStrF(OTVET_BM[6]*3072.0/1023.0, ffFixed, 6, 0);
			else EdtTekA03 -> Text = "0";
			// �������� �2
			if ( shr[30] || shr[8] )
				EdtTekA04 -> Text = FloatToStrF(OTVET_BM[6]*3072.0/1023.0, ffFixed, 6, 0);
			else EdtTekA04 -> Text = "0";
			// ���������� �
			if ( shr[7] || shr[8] ||shr[10] || shr[29] || shr[30] || shr[34] || (!mnemoInAuto) )
				EdtTekA06 -> Text = FloatToStrF((float)OTVET_BM[4]*665.6/1023.0, ffFixed, 6, 0);
			else EdtTekA06 -> Text = "0";
			// ��� �
			if ( shr[7] || shr[8] ||shr[10] || shr[29] || shr[30] || shr[34] || (!mnemoInAuto) )
				EdtTekA07 -> Text = FloatToStrF((float)OTVET_BM[5]*10230.0/1023.0/1000.0, ffFixed, 8, 1);
			else EdtTekA07 -> Text = "0,0";
			// ��� �������� ���
			if ( shr[34] || shr[10] )
				EdtTekA12 -> Text = FloatToStrF((float)aik[13]*COMET_MAX_PD/4095.0,ffFixed,5,1);
			else EdtTekA12 -> Text = "0";
            // ��� �������� ���
			if ( shr[34] || shr[10] )
				EdtTekA13 -> Text = FloatToStrF((float)aik[14]*COMET_MAX_PD/4095.0,ffFixed,5,1);
			else EdtTekA13 -> Text = "0";
			// �������������
			if((N_ST >= 4) && (out[0]&0x04))
				EdtTekA08 -> Text = FloatToStrF ((float)SOPR/10.0, ffFixed, 7, 1);
			else EdtTekA08 -> Text = "0,0";
			// ��������
			if(shr[23])
				EdtTekA09 -> Text = FloatToStrF(100.0*pow(10.0,(float(D_D1)/1000.0-6.8)/0.6),ffFixed, 5, 1);
			else
				EdtTekA09 -> Text = "0,0";
			// ������� �������� ��
		   //	EdtTekA10 -> Text = IntToStr(int(TEK_ABS_DZ/10000.0*100.0));
           // ���� �������� �� // ���������� ������, x�� 100% ��� par_t[0], ���� �������
		EdtTekA10 -> Text = FloatToStrF(((float) TEK_ABS_DZ*100.0/par_t[0]),ffFixed, 5, 1);  // ���� TEK_ABS_DZ/10000*100
		}
		else
		{
			ProcessName -> Caption = " ";
			
			EdtZadA00 -> Text = "";
			EdtZadA01 -> Text = "";
			EdtZadA02 -> Text = "";
			EdtZadA14 -> Text = "";
			EdtZadA03 -> Text = "";
			EdtZadA04 -> Text = "";
			EdtZadA12 -> Text = "";
            EdtZadA08 -> Text = "";
			EdtZadA09 -> Text = "";
			EdtZadA10 -> Text = "";
            EdtZadA11 -> Text = "";
			
			EdtTekA00 -> Text = "0";
			EdtTekA01 -> Text = "0";
			EdtTekA02 -> Text = "0,0";
			EdtTekA14 -> Text = "0";
            EdtTekA15 -> Text = "0";
			EdtTekA03 -> Text = "0,0";
			EdtTekA04 -> Text = "0,0";
			EdtTekA06 -> Text = "0,0";
            EdtTekA07 -> Text = "0,0";
            EdtTekA12 -> Text = "0,0";
            EdtTekA13 -> Text = "0,0";
            EdtTekA08 -> Text = "0,0";
			EdtTekA09 -> Text = "0,0";
			EdtTekA10 -> Text = "0,0";
            EdtTekA11 -> Text = "0";
		}	
    }

    // ������� �������� ��
    pol_DZ -> Caption = IntToStr(int(TEK_ABS_DZ*100.0/par_t[0]));//IntToStr(int(TEK_ABS_DZ/10000.0*100.0));

    // ����� ��������
    if( shr[2] || shr[3] || shr[4] )
        EdtTekA11 -> Text = IntToStr(T_PROC);
    else
        EdtTekA11 -> Text = "0";

    // ���������
    // ��������
    Zasl_Zad -> Text = EdtRTek11 -> Text;
    Zasl_Tek -> Text = FloatToStrF(zaslUgolTek/zaslAngle360*360.0, ffFixed, 5, 1);
    float
        ugol = (float)zaslUgolAbs - (float)nasmod[2];
    while ( ugol > zaslAngle360 ) ugol -= zaslAngle360;
    while ( ugol < 0 ) ugol += zaslAngle360;
    if ( zaslPrNeopr )
        Zasl_Abs -> Text = "�� ���.";
    else
        Zasl_Abs -> Text = FloatToStrF(ugol/zaslAngle360*360.0, ffFixed, 5, 1);
    // �������
    ugol = (float)pderjPutTek - (float)nasmod[1];
    while ( ugol > pderjAngle360 ) ugol -= pderjAngle360;
    while ( ugol < 0 ) ugol += pderjAngle360;
    if ( pderjPrNeopr )
        Bar_Abs -> Text = "�� ���.";
    else
        Bar_Abs -> Text = FloatToStrF(ugol/pderjAngle360*360.0, ffFixed, 5, 1);
    Bar_Spd -> Text = FloatToStrF((float)pderjSpeed/10.0, ffFixed, 5, 1);

}
//---------------------------------------------------------------------------
//--����������� ������--//
//---------------------------------------------------------------------------
TColor TForm1::SetPopeColor( bool value )
{
    if ( value ) return clWhite;
    else         return clLime;
}
//---------------------------------------------------------------------------
void TForm1::VisualButtons()
{
	// ������� ����
    PnlRC -> Font -> Color = SetPopeColor( shr[2] );
    // ����������
    TrnPnl -> Font -> Color = SetPopeColor( shr[3] );
    // ���������� ���������
    PnlUstOff -> Font -> Color = SetPopeColor( shr[5] );
    // ����� ��
    PnlSRC -> Font -> Color = SetPopeColor( shr[4] );

    // ������� (���.)
    PnlOtkOn -> Font -> Color = SetPopeColor( shr[1] );
    PnlOtkOnA -> Font -> Color = SetPopeColor( shr[1] );
    // ������� (����.)
    PnlOtkOff -> Font -> Color = SetPopeColor( shr[5] );
    PnlOtkOffA -> Font -> Color = SetPopeColor( shr[5] );
    // ���1 (� ���.)
    PnlKam -> Font -> Color = SetPopeColor( shr[20]&&!PR_Ar );
    // ���1 (� ��.)
    PnlII -> Font -> Color = SetPopeColor( shr[20]&&PR_Ar );
    // ���2 (���.)
    PnlRRG2On -> Font -> Color = SetPopeColor( shr[21] );
    // ���
    UunOn -> Font -> Color = SetPopeColor( shr[23] );
    //������
    NagrOn -> Font -> Color = SetPopeColor( shr[27] );
    //���� ������
    NagrOff -> Font -> Color = SetPopeColor( shr[28] );
    //��
    IiOn -> Font -> Color = SetPopeColor( shr[32] );
    //���� ��
    IiOff -> Font -> Color = SetPopeColor( shr[33] );
    // ���1
    PnlBPM1On -> Font -> Color = SetPopeColor( shr[29] );
    // ���2
    PnlBPM2On -> Font -> Color = SetPopeColor( shr[30] );
    // ���
    PnlVCHMOn -> Font -> Color = SetPopeColor( shr[34] );
    // ����. ���
    PnlBPMOff -> Font -> Color = SetPopeColor( shr[31] );
    // �/�������� �������
    PnlDZOn -> Font -> Color = SetPopeColor( shr[37] );
    // �/�������� �������
    PnlDZOff -> Font -> Color = SetPopeColor( shr[38] );
    // �/�������� ���������������
    PnlDZDr -> Font -> Color = SetPopeColor( shr[39] );
    // ������ �����
    KrVverh -> Font -> Color = SetPopeColor( shr[44] );
    KrVverhA -> Font -> Color = SetPopeColor( shr[44] );
    Lbl_KR -> Font -> Color = SetPopeColor( shr[44] );
    // ������ ����
    KrVniz -> Font -> Color = SetPopeColor( shr[45] );
    KrVnizA -> Font -> Color = SetPopeColor( shr[45] );
    // �������� � ���.
    Zasl_Ish -> Font -> Color = SetPopeColor(shr[42]);
    // �������� ������
    Zasl_Vrash -> Font -> Color = SetPopeColor(shr[43]);
    // �������� ����
    Zasl_Count -> Font -> Color = SetPopeColor(shr[50]);
    // ������� � ���.
    Bar_Ish -> Font -> Color = SetPopeColor(shr[40]);
    // ������� ������
    Bar_Vrash -> Font -> Color = SetPopeColor( shr[41] );
    // �� HOME
    Pnl_PdH -> Font -> Color = SetPopeColor( shr[38] );
    // �� ������-�����
    Pnl_PdS -> Font -> Color = SetPopeColor( shr[53] );
}
//---------------------------------------------------------------------------
//-- ������������ ���� ���������� --//
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
                        Form1->MemoDiag->Lines->Add(Form1->Label_Time->Caption+" >> ���������� : "+DiagnSNames[i*8+j]);
                    }
                    else
                    {
                        Form1->MemoDiag->Lines->Add(Form1->Label_Time->Caption+" << ����� : "+DiagnSNames[i*8+j]);
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
                    {
                        Form1->MemoDiag->Lines->Add(Form1->Label_Time->Caption+" >> ���������� : "+DiagnNames[i*8+j]);
                    }
                    else
                    {
                        Form1->MemoDiag->Lines->Add(Form1->Label_Time->Caption+" << ����� : "+DiagnNames[i*8+j]);
                    }
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
    else Form1 -> BitBdVall -> Visible = false;
};
//---------------------------------------------------------------------------
//--������������ ������� ���������--//
//---------------------------------------------------------------------------
void TForm1::VisualOperatorDlg()
{
    if ( shr[2] == 5 )
    {
        APanel_String1 -> Caption = "������ ��������, ����������� ��������";
        APanel_String2 -> Caption = "������ �������?";
        APanel_DaBut -> Visible = true;
        APanel_DaBut -> Left= 194;
        APanel -> Visible = true;
        pr_yel=1;
    }
    else if ( shr[2] == 64 )
    {
        APanel_String1 -> Caption = "������ ��������";
        APanel_String2 -> Caption = "����������� ��������";
        APanel_DaBut -> Visible = false;

        APanel -> Visible = true;
        pr_yel=1;
    }
    else if ( shr[4] == 9 )
    {
        APanel_String1 -> Caption = "";
        APanel_String2 -> Caption = "������ ������?";
        APanel_DaBut -> Visible = true;
        APanel_DaBut -> Left= 106;
        APanel_NetBut -> Visible = true;
        APanel_NetBut -> Left= 272;
        APanel -> Visible = true;
        pr_yel=1;
    }
        else if ( shr[4] == 14 )
    {
        APanel_String1 -> Caption = "������ ��������";
        APanel_String2 -> Caption = "����������� ��������";
        APanel_DaBut -> Visible = false;
        APanel -> Visible = true;
        pr_yel=1;
    }
    else {APanel -> Visible = false;pr_yel=0; }
}
//---------------------------------------------------------------------------
//--������������ ���������� ��������--//
//---------------------------------------------------------------------------
void TForm1::VisualParA()
{
     //��������������� ������

     //�����������
     EdtAKon1_12 -> Text = FloatToStrF((float)par[1][12]/10, ffFixed, 5, 0);
     //����� ��������
     EdtAKon1_5  -> Text = FloatToStrF((float)par[1][5], ffFixed, 5, 0);

     //������ �������

    // ������ ���1
    EdtAKon2_0 -> Text = FloatToStrF((float)par[2][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);
    // ��� ��
    EdtAKon2_3 -> Text = FloatToStrF((float)par[2][3]/4095.0*I_MAX, ffFixed, 5, 0);
    // ����� ��������
    EdtAKon2_5 -> Text = FloatToStrF((float)par[2][5], ffFixed, 5, 0);
    // ���� �������� ��
    EdtAKon2_6 -> Text = FloatToStrF((float)par[2][6], ffFixed, 5, 0);  //  /100   *100.0/par_t[0]

    // ������
    // �����������
    EdtAKon3_12 -> Text = FloatToStrF((float)par[3][12]/10.0, ffFixed, 5, 1);
    // ����� ��������
    EdtAKon3_5 -> Text = FloatToStrF((float)par[3][5], ffFixed, 5, 0);

    //��������� 1
    // �����������
    EdtAKon4_12 -> Text = FloatToStrF((float)par[4][12]/10.0, ffFixed, 5, 0);
    // ������ ���2
    EdtAKon4_1 -> Text = FloatToStrF((float)par[4][1]/4095.0*RRG2_MAX, ffFixed, 5, 2);
    // �������� �1
    EdtAKon4_8 -> Text = FloatToStrF((float)par[4][8]/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
    // �������� �2
    EdtAKon4_9 -> Text = FloatToStrF((float)par[4][9]/4095.0*BPM2_P_MAX, ffFixed, 5, 0);
    // ��� �������� ���
    EdtAKon4_10 -> Text = FloatToStrF((float)par[4][10]/4095.0*COMET_MAX_PD, ffFixed, 5, 0);   //������
    // �������� ������� ��� ���
    if(par[4][15] == 0)
        EdtAKon4_15 -> Text = "0,0";
    else
        EdtAKon4_15 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[4][15])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ��������
    if(par[4][2] == 0)
        EdtAKon4_2 -> Text = "0,0";
    else
        EdtAKon4_2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[4][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // �������������
    EdtAKon4_17 -> Text = FloatToStrF((float)par[4][17]/10.0, ffFixed, 7, 0);
    // ����� ��������
    EdtAKon4_5 -> Text = FloatToStrF((float)par[4][5], ffFixed, 5, 0);
    // ���� �������� ��
    EdtAKon4_6 -> Text = FloatToStrF((float)par[4][6], ffFixed, 5, 1);

    //��������� 2
    // �����������
    EdtAKon5_12 -> Text = FloatToStrF((float)par[5][12]/10.0, ffFixed, 5, 0);
    // ������ ���2
    EdtAKon5_1 -> Text = FloatToStrF((float)par[5][1]/4095.0*RRG2_MAX, ffFixed, 5, 2);
    // �������� �1
    EdtAKon5_8 -> Text = FloatToStrF((float)par[5][8]/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
    // �������� �2
    EdtAKon5_9 -> Text = FloatToStrF((float)par[5][9]/4095.0*BPM2_P_MAX, ffFixed, 5, 0);
    // ��� �������� ���
    EdtAKon5_10 -> Text = FloatToStrF((float)par[5][10]/4095.0*COMET_MAX_PD, ffFixed, 5, 0);              //������
    // �������� ������� ��� ���
    if(par[5][15] == 0)
        EdtAKon5_15 -> Text = "0,0";
    else
        EdtAKon5_15 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[5][15])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ��������
    if(par[5][2] == 0)
        EdtAKon5_2 -> Text = "0,0";
    else
        EdtAKon5_2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[5][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // �������������
    EdtAKon5_17 -> Text = FloatToStrF((float)par[5][17]/10.0, ffFixed, 7, 0);
    // ����� ��������
    EdtAKon5_5 -> Text = FloatToStrF((float)par[5][5], ffFixed, 5, 0);
    // ���� �������� ��
    EdtAKon5_6 -> Text = FloatToStrF((float)par[5][6], ffFixed, 5, 1);

    //��������� 3
    // �����������
    EdtAKon6_12 -> Text = FloatToStrF((float)par[6][12]/10.0, ffFixed, 5, 0);
    // ������ ���2
    EdtAKon6_1 -> Text = FloatToStrF((float)par[6][1]/4095.0*RRG2_MAX, ffFixed, 5, 2);
    // �������� �1
    EdtAKon6_8 -> Text = FloatToStrF((float)par[6][8]/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
    // �������� �2
    EdtAKon6_9 -> Text = FloatToStrF((float)par[6][9]/4095.0*BPM2_P_MAX, ffFixed, 5, 0);
    // ��� �������� ���
    EdtAKon6_10 -> Text = FloatToStrF((float)par[6][10]/4095.0*COMET_MAX_PD, ffFixed, 5, 0);         //������
    // �������� ������� ��� ���
    if(par[6][15] == 0)
        EdtAKon6_15 -> Text = "0,0";
    else
        EdtAKon6_15 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[6][15])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ��������
    if(par[6][2] == 0)
        EdtAKon6_2 -> Text = "0,0";
    else
        EdtAKon6_2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[6][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // �������������
    EdtAKon6_17 -> Text = FloatToStrF((float)par[6][17]/10.0, ffFixed, 7, 0);
    // ����� ��������
    EdtAKon6_5 -> Text = FloatToStrF((float)par[6][5], ffFixed, 5, 0);
    // ���� �������� ��
    EdtAKon6_6 -> Text = FloatToStrF((float)par[6][6], ffFixed, 5, 1);


    // �����
    // �����������
    EdtAKon7_12 -> Text = FloatToStrF((float)par[7][12]/10.0, ffFixed, 5, 0);
    // ����� ��������
    EdtAKon7_5 -> Text = FloatToStrF((float)par[7][5], ffFixed, 5, 0);

    // ���������
    // �����������
    EdtAKon8_12 -> Text = FloatToStrF((float)par[8][12]/10.0, ffFixed, 5, 0);
    // ����� ��������
    EdtAKon8_5 -> Text = FloatToStrF((float)par[8][5], ffFixed, 5, 0);



    // ���������� �1
    // �������� �1
    EdtTKon9_8 -> Text = FloatToStrF((float)par[9][8]/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
    // ��������
    EdtTKon9_2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[9][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ����� ��������
    EdtTKon9_5 -> Text = FloatToStrF((float)par[9][5], ffFixed, 5, 0);
    // ������� ��
    EdtTKon9_6 -> Text = FloatToStrF((float)par[9][6], ffFixed, 5, 0);

    // ���������� �2
    // �������� �2
    EdtTKon10_9 -> Text = FloatToStrF((float)par[10][9]/4095.0*BPM2_P_MAX, ffFixed, 5, 0);
    // ��������
    EdtTKon10_2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[10][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ����� ��������
    EdtTKon10_5 -> Text = FloatToStrF((float)par[10][5], ffFixed, 5, 0);
    // ������� ��
    EdtTKon10_6 -> Text = FloatToStrF((float)par[10][6], ffFixed, 5, 0);

    // ���������� ���
    // ��� �������� ���
    EdtTKon11_10 -> Text = FloatToStrF((float)par[11][10]/4095.0*COMET_MAX_PD, ffFixed, 5, 0);             //������
    // �������� ������� ��� ���
    if(par[11][15] == 0)
        EdtTKon11_15 -> Text = "0,0";
    else
        EdtTKon11_15 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[11][15])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ��������
    EdtTKon11_2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[11][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ����� ��������
    EdtTKon11_5 -> Text = FloatToStrF((float)par[11][5], ffFixed, 5, 0);
    // ������� ��
    EdtTKon11_6 -> Text = FloatToStrF((float)par[11][6], ffFixed, 5, 0);

    // ���������� �����
    if(!nasmod[8]) // ��������
    {
        EdtARed1_12->Text = "0,0";
        EdtARed1_5->Text = "0";
        EdtARed3_12->Text = "0,0";
        EdtARed3_5->Text = "0";
        EdtARed4_12->Text = "0,0";
        EdtARed5_12->Text = "0,0";
        EdtARed6_12->Text = "0,0";
        EdtARed7_12->Text = "20";
        EdtARed8_12->Text = "0,0";
    }
    EdtARed1_12->Enabled = nasmod[8];
    EdtARed1_5->Enabled = nasmod[8];
    EdtARed3_12->Enabled = nasmod[8];
    EdtARed3_5->Enabled = nasmod[8];
    EdtARed4_12->Enabled = (nasmod[8]&&nasmod[10]);
    EdtARed5_12->Enabled = (nasmod[8]&&nasmod[10]);
    EdtARed6_12->Enabled = (nasmod[8]&&nasmod[10]);
    EdtARed7_12->Enabled = nasmod[8];
    EdtARed7_5->Enabled = nasmod[8];
    EdtARed8_12->Enabled = nasmod[8];
  //  EdtARed8_5->Enabled = nasmod[8];

    if(!nasmod[9]) // ��
    {
        EdtARed2_0->Text = "0,00";
        EdtARed2_3->Text = "0";
        EdtARed2_5->Text = "0";
        EdtARed2_6->Text = "0";
    }

    EdtARed2_0->Enabled = nasmod[9];
    EdtARed2_3->Enabled = nasmod[9];
    EdtARed2_5->Enabled = nasmod[9];
    EdtARed2_6->Enabled = nasmod[9];

    if(!nasmod[10]) // ���
    {
        EdtARed4_8->Text = "0";
        EdtARed4_9->Text = "0";
        EdtARed4_10->Text = "0";
        EdtARed4_15->Text = "0,4";
        EdtARed5_8->Text = "0";
        EdtARed5_9->Text = "0";
        EdtARed5_10->Text = "0";
        EdtARed5_15->Text = "0,4";
        EdtARed6_8->Text = "0";
        EdtARed6_9->Text = "0";
        EdtARed6_10->Text = "0";
        EdtARed6_15->Text = "0,4";
    }

    EdtARed4_1->Enabled = nasmod[10];
    EdtARed4_2->Enabled = nasmod[10];
    EdtARed4_15->Enabled = nasmod[10];
    EdtARed4_17->Enabled = ((EdtARed4_5->Text == "0")&&nasmod[10]);
    EdtARed4_5->Enabled = ((EdtARed4_17->Text == "0")&&nasmod[10]);
    EdtARed4_6->Enabled = nasmod[10];

    EdtARed5_1->Enabled = nasmod[10];
    EdtARed5_2->Enabled = nasmod[10];
    EdtARed5_15->Enabled = nasmod[10];
    EdtARed5_17->Enabled = ((EdtARed5_5->Text == "0")&&nasmod[10]);
    EdtARed5_5->Enabled = ((EdtARed5_17->Text == "0")&&nasmod[10]);
    EdtARed5_6->Enabled = nasmod[10];

    EdtARed6_1->Enabled = nasmod[10];
    EdtARed6_2->Enabled = nasmod[10];
    EdtARed6_15->Enabled = nasmod[10];
    EdtARed6_17->Enabled = ((EdtARed6_5->Text == "0")&&nasmod[10]);
    EdtARed6_5->Enabled = ((EdtARed6_17->Text == "0")&&nasmod[10]);
    EdtARed6_6->Enabled = nasmod[10];

    EdtARed4_8->Enabled = ((EdtARed4_9->Text == "0")&&(EdtARed4_10->Text == "0")&&nasmod[10]);
    EdtARed4_9->Enabled = ((EdtARed4_8->Text == "0")&&(EdtARed4_10->Text == "0")&&nasmod[10]);
    EdtARed4_10->Enabled = ((EdtARed4_8->Text == "0")&&(EdtARed4_9->Text == "0")&&nasmod[10]);

    EdtARed5_8->Enabled = ((EdtARed5_9->Text == "0")&&(EdtARed5_10->Text == "0")&&nasmod[10]);
    EdtARed5_9->Enabled = ((EdtARed5_8->Text == "0")&&(EdtARed5_10->Text == "0")&&nasmod[10]);
    EdtARed5_10->Enabled = ((EdtARed5_8->Text == "0")&&(EdtARed5_9->Text == "0")&&nasmod[10]);

    EdtARed6_8->Enabled = ((EdtARed6_9->Text == "0")&&(EdtARed6_10->Text == "0")&&nasmod[10]);
    EdtARed6_9->Enabled = ((EdtARed6_8->Text == "0")&&(EdtARed6_10->Text == "0")&&nasmod[10]);
    EdtARed6_10->Enabled = ((EdtARed6_8->Text == "0")&&(EdtARed6_9->Text == "0")&&nasmod[10]);     

}
//---------------------------------------------------------------------------
//--������������ ���������� �������--//
//---------------------------------------------------------------------------
void TForm1::VisualParR()
{

    // ������ ���1
    EdtRTek0 -> Text = FloatToStrF((float)par[0][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);
    // ������ ���2
    EdtRTek1 -> Text = FloatToStrF((float)par[0][1]/4095.0*RRG2_MAX, ffFixed, 5, 2);
    // �����������
    EdtRTek12 -> Text = FloatToStrF((float)par[0][12]/10.0, ffFixed, 5, 1);
    // ��� ��
    EdtRTek7 -> Text = FloatToStrF((float)par[0][3]/4095.0*I_MAX, ffFixed, 5, 0);
    // �������� �1
    EdtRTek8 -> Text = FloatToStrF((float)par[0][8]/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
    // �������� �2
    EdtRTek9 -> Text = FloatToStrF((float)par[0][9]/4095.0*BPM2_P_MAX, ffFixed, 5, 0);
    // ��� �������� ���
    EdtRTek10 -> Text = FloatToStrF((float)par[0][10]/4095.0*COMET_MAX_PD, ffFixed, 5, 0);        //������
    // ��������
    EdtRTek2 -> Text = FloatToStrF(100.0*pow(10.0,(float(par[0][2])/1000.0-6.8)/0.6),ffFixed, 5, 1);
    // ���� �������� ��������
    EdtRTek11 -> Text = FloatToStrF((float)par[0][11]/zaslAngle360*UGOL_DZ_MAX, ffFixed, 5, 0);
    // ������� �������� ��
    EdtRTek6 -> Text = FloatToStrF((float)par[0][6], ffFixed, 5, 0);   //
    // ���� ��
    EdtRTek13 -> Text = FloatToStrF((int)par[0][13], ffFixed, 5, 0);
    // �������� ��������
        if(par[0][14]==0)      { EdtRTek14->Text="�������";  } //�������� ������������
    else if(par[0][14]==1) { EdtRTek14->Text="�����";    } //�������� ������������
    else if(par[0][14]==2) { EdtRTek14->Text="��������"; } //�������� ������������
}
//---------------------------------------------------------------------------
//--������������ ����������� ����������--//
//---------------------------------------------------------------------------
void TForm1::VisualNasmod()
{
    // ���������� ������� ��������������� �������������
    EditNastrIn0 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[0])/1000.0-6.8)/0.6),ffExponent,2,8);
    //  �������  ������� ������ ����� ��������
    EditNastrIn22 -> Text = FloatToStrF(100.0*pow(10,(float(nasmod[22])/1000.0-6.8)/0.6),ffExponent,2,8);
    // ���� �������� ��������
    EditNastrIn2 -> Text = FloatToStrF((float)nasmod[2]*360.0/zaslAngle360, ffFixed, 5, 1);
     // ���� �������� ��������
    EditNastrIn1 -> Text = FloatToStrF((float)nasmod[1]*360.0/pderjAngle360, ffFixed, 5, 1);
    // �������� ���������� �������� �1 ��� ������
    EditNastrIn4 -> Text = FloatToStrF((float)nasmod[4]*12.0*BPM1_P_MAX/4095.0, ffFixed, 5, 0);
    // ����� ������ �1
    EditNastrIn5 -> Text = FloatToStrF((float)nasmod[5], ffFixed, 5, 0);
    // �������� ���������� �������� �2 ��� ������
    EditNastrIn6 -> Text = FloatToStrF((float)nasmod[6]*12.0*BPM2_P_MAX/4095.0, ffFixed, 5, 0);
    // ����� ������ �2
    EditNastrIn7 -> Text = FloatToStrF((float)nasmod[7], ffFixed, 5, 0);
    // ����� ������ ���
    EditNastrIn29 -> Text = FloatToStrF((float)nasmod[29], ffFixed, 5, 0);
    //������� ����������� �1
    EditNastrIn30 -> Text = FloatToStrF((float)nasmod[30]*40.0/255.0, ffFixed , 5, 0);
    //������� ����������� �2
    EditNastrIn31 -> Text = FloatToStrF((float)nasmod[31]*40.0/255.0, ffFixed , 5, 0);
    // ������� �������� �������� ��������
    EditNastrIn11 -> Text = IntToStr( ( nasmod[11] - 8192 - 820 ) / 205 + 1 );
    // ������� �������� �������� ��������(������)
    EditNastrIn23 -> Text = IntToStr( ( nasmod[23] - 8192 - 820 ) / 205 + 1 );
    // ����������� �������� �������� ��������
    EditNastrIn15 -> Text = IntToStr( ( nasmod[15] - 8192 - 820 ) / 205 + 1 );
    // ������� �������� �������� ��������
    EditNastrIn12 -> Text = IntToStr( ( nasmod[12] - 8192 - 820 ) / 205 + 1 );
    // ����������� �������� �������� ��������
    EditNastrIn16 -> Text = IntToStr( ( nasmod[16] - 8192 - 820 ) / 205 + 1 );
    //�������� ���� ������� �����������?
    if ( nasmod[8] ) EditNastrIn8 -> Text = "��";
    else             EditNastrIn8 -> Text = "���";
    //�������� ���� ��?
    if ( nasmod[9] ) EditNastrIn9 -> Text = "��";
    else             EditNastrIn9 -> Text = "���";
    //�������� ���� ������� ����������?
    if ( nasmod[10] ) EditNastrIn10 -> Text = "��";
    else              EditNastrIn10 -> Text = "���";
    // ������������ ���������� �� ���
    EditNastrIn19 -> Text = FloatToStrF(float(nasmod[19])*BPM_U_MAX/4095.0, ffFixed , 5, 0);
    //�������� ��������� �������
    EditNastrIn3 -> Text = IntToStr(int(100*pow(10,nasmod[3])));
    //���������� �������� ��� ���������� � ��������� �������������
    EditNastrIn25 -> Text = FloatToStrF((float)nasmod[25]/4095.0*BPM1_P_MAX, ffFixed, 5, 0);
    //���������� �������� ��� ���������� � ��������� �������������
    EditNastrIn37 -> Text = FloatToStrF((float)nasmod[37]/4095.0*COMET_MAX_PD, ffFixed, 5, 0);
    //������������ �������� � ����� ���1
    EditNastrIn38 -> Text = FloatToStrF((((float)nasmod[38]/409.5) - 2.032)/8.128 *9.8, ffFixed, 5, 1);   // *9,8
    //������������ �������� � ����� ���2
    EditNastrIn39 -> Text = FloatToStrF((((float)nasmod[39]/409.5) - 2.032)/8.128 *9.8, ffFixed, 5, 1);   // *9,8
    //������������ �������� �� ���������� ��������
    EditNastrIn26 -> Text = FloatToStrF((float)nasmod[26]/10.0, ffFixed, 6, 0);
    //������ � �������� � ������ �������� �����
    if ( nasmod[27] ) EditNastrIn27 -> Text = "��";
    else              EditNastrIn27 -> Text = "���";
    //������ � �������� � ����� �������� �����
    if ( nasmod[34] ) EditNastrIn34 -> Text = "��";
    else              EditNastrIn34 -> Text = "���";
    //����������� ��
    EditNastrIn33 -> Text = FloatToStrF((float)nasmod[33]*350/2047.0, ffFixed, 5, 0);

    //������ � ������ ������
    if ( nasmod[36] ) EditNastrIn36 -> Text = "��";
    else              EditNastrIn36 -> Text = "���";

    // ������������ ���
    EditNastrIn35 -> Text = IntToStr((unsigned int)(1000.0/(double)nasmod[35]));                  // ���������� ����������� ������������

  /*
    // �������� ���
    if ( nasmod[8] ) EditNastrIn8 -> Text = "��";
    else             EditNastrIn8 -> Text = "���";
    EdtARed1_12 -> Enabled = (bool)nasmod[8];
    EdtARed3_12 -> Enabled = (bool)nasmod[8];
    EdtARed3_5 -> Enabled = (bool)nasmod[8];
    EdtARed5_12 -> Enabled = (bool)nasmod[8];
    EdtARed5_5 -> Enabled = (bool)nasmod[8];

    // �������� ����
    if ( nasmod[9] ) EditNastrIn9 -> Text = "��";
    else             EditNastrIn9 -> Text = "���";
    EdtARed2_15 -> Enabled = (bool)nasmod[9];
    EdtARed2_3 -> Enabled = (bool)nasmod[9];
    EdtARed2_5 -> Enabled = (bool)nasmod[9];
    EdtARed2_6 -> Enabled = (bool)nasmod[9];

    // �������� ���
    if ( nasmod[10] ) EditNastrIn10 -> Text = "��";
    else              EditNastrIn10 -> Text = "���";
    EdtARed4_1 -> Enabled = (bool)nasmod[10];
    //EdtARed4_8 -> Enabled = (bool)nasmod[10];
    //EdtARed4_9 -> Enabled = (bool)nasmod[10];
    EdtARed4_2 -> Enabled = (bool)nasmod[10];
    //EdtARed4_17 -> Enabled = (bool)nasmod[10];
    //EdtARed4_5 -> Enabled = (bool)nasmod[10];
    EdtARed4_6 -> Enabled = (bool)nasmod[10];


    // �� � ��������
    if ( nasmod[24] ) EditNastrIn24 -> Text = "��";
    else              EditNastrIn24 -> Text = "���";      */
}
//---------------------------------------------------------------------------
//--������������ �������� �������--//
//---------------------------------------------------------------------------
void TForm1::VisualDebug()
{
    TEdit
        *EdtDebugValues[30] =
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
		"shr[7]",
		"sh[7]",
		"shr[8]",
		"sh[8]",
		"shr[9]",
		"sh[9]",
		"shr[20]",
		"sh[20]",
		"shr[21]",
		"sh[21]",
		"shr[23]",
		"sh[23]",
		"shr[27]",
		"sh[27]",
		"shr[28]",
		"sh[28]",
		"shr[29]",
		"sh[29]",
		"shr[30]",
		"sh[30]",

		"shr[31]",
		"sh[31]",
		"shr[32]",
		"sh[32]",
		"shr[33]",
		"sh[33]",
		"shr[34]",
		"sh[34]",
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
		"shr[48]",
		"sh[48]",

		"shr[49]",
		"sh[49]",
		"shr[50]",
		"sh[50]",
		"shr[52]",
		"sh[52]",
		"shr[53]",
		"sh[53]",
        "",
        "otvet",
 		"norma",
		"sh_",
		"qkk",
        "",
        "D_D1",
		"D_D2",
        "",
		"UVAK_KN",
		"UVAK_KAM_V",
        "UVAK_KAM_N",
        "UVAK_KAM",
        "UVAK_ATM",
        "POROG_DAVL",
        "",
        "zin[0]",
        "zin[1]",
        "zin[2]",
		"out[0]",
        "out[1]",
        "out[2]",

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
		"diagn[19]",
		"diagn[20]",
		"diagn[21]",
        " ",
		"diagnS[0]",
		"diagnS[1]",
        " ",
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
		"nasmod[15]",
		"nasmod[16]",
		"nasmod[19]",
		"nasmod[22]",
		"nasmod[23]",
		"nasmod[25]",
        "nasmod[26]",
        "nasmod[27]",
        "nasmod[29]",
        "nasmod[30]",
        "nasmod[31]",
        "nasmod[33]",
        "nasmod[34]",
        "nasmod[35]",
        "nasmod[36]",
        "nasmod[37]",
        "nasmod[38]",
        "nasmod[39]",
        " ",
        " ",
        " ",
        
		"par[0][0]",
		"par[0][1]",
		"par[0][2]",
		"par[0][3]",
		"par[0][6]",
		"par[0][8]",
		"par[0][9]",
		"par[0][10]",
		"par[0][11]",
		"par[0][12]",
		"par[0][13]",
		"par[0][14]",
        "",
		"par[1][5]",
		"par[1][12]",
        "",
		"par[2][0]",
		"par[2][3]",
		"par[2][5]",
		"par[2][6]",
        "",
		"par[3][5]",
		"par[3][12]",
        "",
		"par[4][1]",
		"par[4][2]",
		"par[4][5]",
		"par[4][6]",
		"par[4][8]",
		"par[4][9]",

		"par[4][10]",
		"par[4][12]",
		"par[4][15]",
		"par[4][17]",
        "",
		"par[5][1]",
		"par[5][2]",
		"par[5][5]",
		"par[5][6]",
		"par[5][8]",
		"par[5][9]",
		"par[5][10]",
		"par[5][12]",
		"par[5][15]",
		"par[5][17]",
        "",
		"par[6][1]",
		"par[6][2]",
		"par[6][5]",
		"par[6][6]",
		"par[6][8]",
		"par[6][9]",
		"par[6][10]",
		"par[6][12]",
		"par[6][15]",
		"par[6][17]",
        "",
		"par[7][5]",
		"par[7][12]",
        "",

		"par[8][5]",
		"par[8][12]",
        "",
		"par[9][2]",
		"par[9][5]",
		"par[9][6]",
		"par[9][8]",
        "",
		"par[10][2]",
		"par[10][5]",
		"par[10][6]",
		"par[10][9]",
        "",
		"par[11][2]",
		"par[11][5]",
		"par[11][6]",
		"par[11][10]",
		"par[11][15]",
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

		"CT_T1",				
		"CT_T20",
		"CT_1",
		"CT_2",
		"CT_3",
		"CT_7",
		"CT_8",
    	"CT_10",
		"CT_23",
		"CT23K1",
		"CT_27",
		"CT27K1",
    	"CT_28",
		"CT_29",
		"CT29K1",
		"CT_30",
		"CT30K1",
		"CT_32",
		"CT32K1",
		"CT_34",
        "CT34K1",
        "CT_39",
        "ct48",
        "ct49",
        "ctVrashPderj",
        "ctPderjSpd",
		"",
		"",
		"",
		"",

        "T_VODA",
        "T_KTMN_OTK",
        "T_KKAM_V",
        "T_KKAM",
        "T_PROC",
        "T_KUSTBM",
        "T_KZASL",
        "T_VKL_BPN",
        "T_KL",
        "TK_TMN1",
        "TK_TMN2",
        "TK_TMN3",
        "T_KKR",
        "T_KSOPR",
        "T_KVRASH",
        "T_KOST",
        "T_KKN_OTK",
        "T_ZASL",
        "T_KRAZGON",
        "T_VHG",
        "tk48",
        "tk49",
        "tkVrashPderj",
		"zaslUgolAbs",
		"pderjPutTek"
        "",
        "",
        "",
        "",
        "",
        "",

		"CT_VODA_BM",
		"CT_VODA_II",
    	"CT_VODA_KN",
		"CT_TEMP",
		"CT_BM",
		"CT_II",
    	"CT_IST",
		"CT_TMN",
		"CT_KR",
		"CT_KR_T",
    	"CT_BAR",
    	"CT_NAP",
    	"CT_KSOPR",
		"CT_VRASH_BAR",
		"CT_VKL_KN",
		"CT_KN",
		"CT_VHG",
		"PR_RAZKN",
		"PR_RC",
		"PR_DAVL_PODJ",
		"N_ST",
		"N_ST_MAX",
		"SOPR",
		"PR_VTMN",
		"PR_SOPR_BM",
		"Z_SOPR",
		"ZU_SOPR",
		"K_OSHIB",
		"LogicT",
		"PR_Ar",


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
		"",
        "",
	    "KOM_DZ",
	    "OTVET_DZ",
	    "TYPE_DZ",
	    "PR_DZ",
	    "HOME_DZ",
	    "PUT_DZ",
	    "V_DZ",
	    "TEK_ABS_DZ",
	    "TEK_OTN_DZ",
	    "CT_DZ",
        "",
        "",
        "",
        "",
        "",


		"VRUN",
		"X_TUN",
		"E_TUN",
		"DELUN",
		"E_PUN",
		"K_PUN",
		"K_IUN",
		"U_PUN",
		"T_REQUN",
		"LIMPUN",
		"LIMIUN",
		"LIM1UN",
		"LIM2UN",
		"LIMUUN",
		"LIMU_UN",
		"LIMUPR_UN",
		"PORCNV_UN",
		"PORCPR_UN",
		"PROBUN",
        "",
        "",
        "M1_N",
		"M1_V",
		"M2_N",
		"M2_V",
		"VCHM_N",
		"VCHM_V",
        "",
        "",
        "",



		"PR_TEMP",
		"KOM_TEMP",
		"ZAD_TEMP",
		"PAR_TEMP",
		"ZPAR_TEMP",
		"X_TEMP",
		"VRTEMP",
		"E_TEMP",
		"DELTEMP",
		"LIM1TEMP",
		"LIM2TEMP",
		"T_VRTEMP",
		"T_KTEMP",
		"DOPTEMP",
		"TEK_TEMP",
		"TEK_TEMP1",
		"TEK_TEMP2",
		"TEK_TEMP3",
		"TEK_TEMP4",
		"TEK_TEMP_PIR",
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



		"VRBM",
		"UST_BM",
		"X_TBM",
		"E_TBM",
		"DELBM",
		"DOPBM",
		"PAR_BM1",
		"PAR_BM2",
		"LIM1BM",
		"LIM2BM",
		"PR_SV_BM",
		"KOM_BM[0]",
		"KOM_BM[1]",
		"KOM_BM[2]",
		"KOM_BM[3]",
		"KOM_BM[4]",
		"OTVET_BM[0]",
		"OTVET_BM[1]",
		"OTVET_BM[2]",
		"OTVET_BM[3]",
		"OTVET_BM[4]",
		"OTVET_BM[5]",
		"OTVET_BM[6]",
		"OTVET_BM[7]",
		"OTVET_BM[8]",
		"OTVET_BM[9]",
        "",
        "",
        "",
        "",

		"VRII",
		"X_TII",
		"E_TII",
		"DELII",
		"DOPII",
		"PAR_II",
		"LIM1II",
		"LIM2II",
		"T_VRII",
		"T_KII",
        "PR_SV_II",
		"KOM_II[0]",
		"KOM_II[1]",
		"KOM_II[2]",
		"KOM_II[3]",
		"KOM_II[4]",
		"OTVET_II[0]",
		"OTVET_II[1]",
		"OTVET_II[2]",
		"OTVET_II[3]",
		"OTVET_II[4]",
		"OTVET_II[5]",
		"OTVET_II[6]",
		"OTVET_II[7]",
		"OTVET_II[8]",
		"OTVET_II[9]",
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

	// ��������� �������� ���������� �� ��������
	switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
	{
   //0 ��������

case 0:
{
EditOTLtek1->Text = IntToStr(shr[1]);
EditOTLtek2->Text = IntToStr(sh[1]);
EditOTLtek3->Text = IntToStr(shr[2]);
EditOTLtek4->Text = IntToStr(sh[2]);
EditOTLtek5->Text = IntToStr(shr[3]);
EditOTLtek6->Text = IntToStr(sh[3]);
EditOTLtek7->Text = IntToStr(shr[4]);
EditOTLtek8->Text = IntToStr(sh[4]);
EditOTLtek9->Text = IntToStr(shr[5]);
EditOTLtek10->Text = IntToStr(sh[5]);
EditOTLtek11->Text = IntToStr(shr[7]);
EditOTLtek12->Text = IntToStr(sh[7]);
EditOTLtek13->Text = IntToStr(shr[8]);
EditOTLtek14->Text = IntToStr(sh[8]);
EditOTLtek15->Text = IntToStr(shr[9]);
EditOTLtek16->Text = IntToStr(sh[9]);
EditOTLtek17->Text = IntToStr(shr[20]);
EditOTLtek18->Text = IntToStr(sh[20]);
EditOTLtek19->Text = IntToStr(shr[21]);
EditOTLtek20->Text = IntToStr(sh[21]);
EditOTLtek21->Text = IntToStr(shr[23]);
EditOTLtek22->Text = IntToStr(sh[23]);
EditOTLtek23->Text = IntToStr(shr[27]);
EditOTLtek24->Text = IntToStr(sh[27]);
EditOTLtek25->Text = IntToStr(shr[28]);
EditOTLtek26->Text = IntToStr(sh[28]);
EditOTLtek27->Text = IntToStr(shr[29]);
EditOTLtek28->Text = IntToStr(sh[29]);
EditOTLtek29->Text = IntToStr(shr[30]);
EditOTLtek30->Text = IntToStr(sh[30]);
}; break;

//1 ��������

case 1:
{
EditOTLtek1->Text = IntToStr(shr[31]);
EditOTLtek2->Text = IntToStr(sh[31]);
EditOTLtek3->Text = IntToStr(shr[32]);
EditOTLtek4->Text = IntToStr(sh[32]);
EditOTLtek5->Text = IntToStr(shr[33]);
EditOTLtek6->Text = IntToStr(sh[33]);
EditOTLtek7->Text = IntToStr(shr[34]);
EditOTLtek8->Text = IntToStr(sh[34]);
EditOTLtek9->Text = IntToStr(shr[36]);
EditOTLtek10->Text = IntToStr(sh[36]);
EditOTLtek11->Text = IntToStr(shr[37]);
EditOTLtek12->Text = IntToStr(sh[37]);
EditOTLtek13->Text = IntToStr(shr[38]);
EditOTLtek14->Text = IntToStr(sh[38]);
EditOTLtek15->Text = IntToStr(shr[39]);
EditOTLtek16->Text = IntToStr(sh[39]);
EditOTLtek17->Text = IntToStr(shr[40]);
EditOTLtek18->Text = IntToStr(sh[40]);
EditOTLtek19->Text = IntToStr(shr[41]);
EditOTLtek20->Text = IntToStr(sh[41]);
EditOTLtek21->Text = IntToStr(shr[42]);
EditOTLtek22->Text = IntToStr(sh[42]);
EditOTLtek23->Text = IntToStr(shr[43]);
EditOTLtek24->Text = IntToStr(sh[43]);
EditOTLtek25->Text = IntToStr(shr[44]);
EditOTLtek26->Text = IntToStr(sh[44]);
EditOTLtek27->Text = IntToStr(shr[45]);
EditOTLtek28->Text = IntToStr(sh[45]);
EditOTLtek29->Text = IntToStr(shr[48]);
EditOTLtek30->Text = IntToStr(sh[48]);
}; break;

//2 ��������

case 2:
{
EditOTLtek1->Text = IntToStr(shr[49]);
EditOTLtek2->Text = IntToStr(sh[49]);
EditOTLtek3->Text = IntToStr(shr[50]);
EditOTLtek4->Text = IntToStr(sh[50]);
EditOTLtek5->Text = IntToStr(shr[52]);
EditOTLtek6->Text = IntToStr(sh[52]);
EditOTLtek7->Text = IntToStr(shr[53]);
EditOTLtek8->Text = IntToStr(sh[53]);
EditOTLtek9->Text = IntToStr(0);
EditOTLtek10->Text = IntToStr(otvet);
EditOTLtek11->Text = IntToStr(norma);
EditOTLtek12->Text = IntToStr(sh_);
EditOTLtek13->Text = IntToStr(qkk);
EditOTLtek14->Text = IntToStr(0);
EditOTLtek15->Text = IntToStr(D_D1);
EditOTLtek16->Text = IntToStr(D_D2);
EditOTLtek17->Text = IntToStr(0);
EditOTLtek18->Text = IntToStr(UVAK_KN);
EditOTLtek19->Text = IntToStr(UVAK_KAM_V);
EditOTLtek20->Text = IntToStr(UVAK_KAM_N);
EditOTLtek21->Text = IntToStr(UVAK_KAM);
EditOTLtek22->Text = IntToStr(UVAK_ATM);
EditOTLtek23->Text = IntToStr(POROG_DAVL);
EditOTLtek24->Text = IntToStr(0);
EditOTLtek25->Text = IntToStr(zin[0]);
EditOTLtek26->Text = IntToStr(zin[1]);
EditOTLtek27->Text = IntToStr(zin[2]);
EditOTLtek28->Text = IntToStr(out[0]);
EditOTLtek29->Text = IntToStr(out[1]);
EditOTLtek30->Text = IntToStr(out[2]);
}; break;

//3 ��������

case 3:
{
EditOTLtek1->Text = IntToStr(aout[0]);
EditOTLtek2->Text = IntToStr(aout[1]);
EditOTLtek3->Text = IntToStr(aout[2]);
EditOTLtek4->Text = IntToStr(aout[3]);
EditOTLtek5->Text = IntToStr(aout[4]);
EditOTLtek6->Text = IntToStr(aout[5]);
EditOTLtek7->Text = IntToStr(aout[6]);
EditOTLtek8->Text = IntToStr(aout[7]);
EditOTLtek9->Text = IntToStr(aout[8]);
EditOTLtek10->Text = IntToStr(aout[9]);
EditOTLtek11->Text = IntToStr(aout[10]);
EditOTLtek12->Text = IntToStr(aout[11]);
EditOTLtek13->Text = IntToStr(0);
EditOTLtek14->Text = IntToStr(aik[0]);
EditOTLtek15->Text = IntToStr(aik[1]);
EditOTLtek16->Text = IntToStr(aik[2]);
EditOTLtek17->Text = IntToStr(aik[3]);
EditOTLtek18->Text = IntToStr(aik[4]);
EditOTLtek19->Text = IntToStr(aik[5]);
EditOTLtek20->Text = IntToStr(aik[6]);
EditOTLtek21->Text = IntToStr(aik[7]);
EditOTLtek22->Text = IntToStr(aik[8]);
EditOTLtek23->Text = IntToStr(aik[9]);
EditOTLtek24->Text = IntToStr(aik[10]);
EditOTLtek25->Text = IntToStr(aik[11]);
EditOTLtek26->Text = IntToStr(aik[12]);
EditOTLtek27->Text = IntToStr(aik[13]);
EditOTLtek28->Text = IntToStr(aik[14]);
EditOTLtek29->Text = IntToStr(aik[15]);
EditOTLtek30->Text = IntToStr(0);
}; break;

//4 ��������

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
EditOTLtek23->Text = IntToStr(0);
EditOTLtek24->Text = IntToStr(diagnS[0]);
EditOTLtek25->Text = IntToStr(diagnS[1]);
EditOTLtek26->Text = IntToStr(0);
EditOTLtek27->Text = IntToStr(nasmod[0]);
EditOTLtek28->Text = IntToStr(nasmod[1]);
EditOTLtek29->Text = IntToStr(nasmod[2]);
EditOTLtek30->Text = IntToStr(nasmod[3]);
}; break;

//5 ��������

case 5:
{
EditOTLtek1->Text = IntToStr(nasmod[4]);
EditOTLtek2->Text = IntToStr(nasmod[5]);
EditOTLtek3->Text = IntToStr(nasmod[6]);
EditOTLtek4->Text = IntToStr(nasmod[7]);
EditOTLtek5->Text = IntToStr(nasmod[8]);
EditOTLtek6->Text = IntToStr(nasmod[9]);
EditOTLtek7->Text = IntToStr(nasmod[10]);
EditOTLtek8->Text = IntToStr(nasmod[11]);
EditOTLtek9->Text = IntToStr(nasmod[12]);
EditOTLtek10->Text = IntToStr(nasmod[15]);
EditOTLtek11->Text = IntToStr(nasmod[16]);
EditOTLtek12->Text = IntToStr(nasmod[19]);
EditOTLtek13->Text = IntToStr(nasmod[22]);
EditOTLtek14->Text = IntToStr(nasmod[23]);
EditOTLtek15->Text = IntToStr(nasmod[25]);
EditOTLtek16->Text = IntToStr(nasmod[26]);
EditOTLtek17->Text = IntToStr(nasmod[27]);
EditOTLtek18->Text = IntToStr(nasmod[29]);
EditOTLtek19->Text = IntToStr(nasmod[30]);
EditOTLtek20->Text = IntToStr(nasmod[31]);
EditOTLtek21->Text = IntToStr(nasmod[33]);
EditOTLtek22->Text = IntToStr(nasmod[34]);
EditOTLtek23->Text = IntToStr(nasmod[35]);
EditOTLtek24->Text = IntToStr(nasmod[36]);
EditOTLtek25->Text = IntToStr(nasmod[37]);
EditOTLtek26->Text = IntToStr(nasmod[38]);
EditOTLtek27->Text = IntToStr(nasmod[39]);
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;

//6 ��������

case 6:
{
EditOTLtek1->Text = IntToStr(par[0][0]);
EditOTLtek2->Text = IntToStr(par[0][1]);
EditOTLtek3->Text = IntToStr(par[0][2]);
EditOTLtek4->Text = IntToStr(par[0][3]);
EditOTLtek5->Text = IntToStr(par[0][6]);
EditOTLtek6->Text = IntToStr(par[0][8]);
EditOTLtek7->Text = IntToStr(par[0][9]);
EditOTLtek8->Text = IntToStr(par[0][10]);
EditOTLtek9->Text = IntToStr(par[0][11]);
EditOTLtek10->Text = IntToStr(par[0][12]);
EditOTLtek11->Text = IntToStr(par[0][13]);
EditOTLtek12->Text = IntToStr(par[0][14]);
EditOTLtek13->Text = IntToStr(0);
EditOTLtek14->Text = IntToStr(par[1][5]);
EditOTLtek15->Text = IntToStr(par[1][12]);
EditOTLtek16->Text = IntToStr(0);
EditOTLtek17->Text = IntToStr(par[2][0]);
EditOTLtek18->Text = IntToStr(par[2][3]);
EditOTLtek19->Text = IntToStr(par[2][5]);
EditOTLtek20->Text = IntToStr(par[2][6]);
EditOTLtek21->Text = IntToStr(0);
EditOTLtek22->Text = IntToStr(par[3][5]);
EditOTLtek23->Text = IntToStr(par[3][12]);
EditOTLtek24->Text = IntToStr(0);
EditOTLtek25->Text = IntToStr(par[4][1]);
EditOTLtek26->Text = IntToStr(par[4][2]);
EditOTLtek27->Text = IntToStr(par[4][5]);
EditOTLtek28->Text = IntToStr(par[4][6]);
EditOTLtek29->Text = IntToStr(par[4][8]);
EditOTLtek30->Text = IntToStr(par[4][9]);
}; break;

//7 ��������

case 7:
{
EditOTLtek1->Text = IntToStr(par[4][10]);
EditOTLtek2->Text = IntToStr(par[4][12]);
EditOTLtek3->Text = IntToStr(par[4][15]);
EditOTLtek4->Text = IntToStr(par[4][17]);
EditOTLtek5->Text = IntToStr(0);
EditOTLtek6->Text = IntToStr(par[5][1]);
EditOTLtek7->Text = IntToStr(par[5][2]);
EditOTLtek8->Text = IntToStr(par[5][5]);
EditOTLtek9->Text = IntToStr(par[5][6]);
EditOTLtek10->Text = IntToStr(par[5][8]);
EditOTLtek11->Text = IntToStr(par[5][9]);
EditOTLtek12->Text = IntToStr(par[5][10]);
EditOTLtek13->Text = IntToStr(par[5][12]);
EditOTLtek14->Text = IntToStr(par[5][15]);
EditOTLtek15->Text = IntToStr(par[5][17]);
EditOTLtek16->Text = IntToStr(0);
EditOTLtek17->Text = IntToStr(par[6][1]);
EditOTLtek18->Text = IntToStr(par[6][2]);
EditOTLtek19->Text = IntToStr(par[6][5]);
EditOTLtek20->Text = IntToStr(par[6][6]);
EditOTLtek21->Text = IntToStr(par[6][8]);
EditOTLtek22->Text = IntToStr(par[6][9]);
EditOTLtek23->Text = IntToStr(par[6][10]);
EditOTLtek24->Text = IntToStr(par[6][12]);
EditOTLtek25->Text = IntToStr(par[6][15]);
EditOTLtek26->Text = IntToStr(par[6][17]);
EditOTLtek27->Text = IntToStr(0);
EditOTLtek28->Text = IntToStr(par[7][5]);
EditOTLtek29->Text = IntToStr(par[7][12]);
EditOTLtek30->Text = IntToStr(0);
}; break;

//8 ��������

case 8:
{
EditOTLtek1->Text = IntToStr(par[8][5]);
EditOTLtek2->Text = IntToStr(par[8][12]);
EditOTLtek3->Text = IntToStr(0);
EditOTLtek4->Text = IntToStr(par[9][2]);
EditOTLtek5->Text = IntToStr(par[9][5]);
EditOTLtek6->Text = IntToStr(par[9][6]);
EditOTLtek7->Text = IntToStr(par[9][8]);
EditOTLtek8->Text = IntToStr(0);
EditOTLtek9->Text = IntToStr(par[10][2]);
EditOTLtek10->Text = IntToStr(par[10][5]);
EditOTLtek11->Text = IntToStr(par[10][6]);
EditOTLtek12->Text = IntToStr(par[10][9]);
EditOTLtek13->Text = IntToStr(0);
EditOTLtek14->Text = IntToStr(par[11][2]);
EditOTLtek15->Text = IntToStr(par[11][5]);
EditOTLtek16->Text = IntToStr(par[11][6]);
EditOTLtek17->Text = IntToStr(par[11][10]);
EditOTLtek18->Text = IntToStr(par[11][15]);
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

//9 ��������

case 9:
{
EditOTLtek1->Text = IntToStr(CT_T1);
EditOTLtek2->Text = IntToStr(CT_T20);
EditOTLtek3->Text = IntToStr(CT_1);
EditOTLtek4->Text = IntToStr(CT_2);
EditOTLtek5->Text = IntToStr(CT_3);
EditOTLtek6->Text = IntToStr(CT_7);
EditOTLtek7->Text = IntToStr(CT_8);
EditOTLtek8->Text = IntToStr(CT_10);
EditOTLtek9->Text = IntToStr(CT_23);
EditOTLtek10->Text = IntToStr(CT23K1);
EditOTLtek11->Text = IntToStr(CT_27);
EditOTLtek12->Text = IntToStr(CT27K1);
EditOTLtek13->Text = IntToStr(CT_28);
EditOTLtek14->Text = IntToStr(CT_29);
EditOTLtek15->Text = IntToStr(CT29K1);
EditOTLtek16->Text = IntToStr(CT_30);
EditOTLtek17->Text = IntToStr(CT30K1);
EditOTLtek18->Text = IntToStr(CT_32);
EditOTLtek19->Text = IntToStr(CT32K1);
EditOTLtek20->Text = IntToStr(CT_34);
EditOTLtek21->Text = IntToStr(CT34K1);
EditOTLtek22->Text = IntToStr(CT_39);
EditOTLtek23->Text = IntToStr(ct48);
EditOTLtek24->Text = IntToStr(ct49);
EditOTLtek25->Text = IntToStr(ctVrashPderj);
EditOTLtek26->Text = IntToStr(ctPderjSpd);
EditOTLtek27->Text = IntToStr(0);
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;

//10 ��������

case 10:
{
EditOTLtek1->Text = IntToStr(T_VODA);
EditOTLtek2->Text = IntToStr(T_KTMN_OTK);
EditOTLtek3->Text = IntToStr(T_KKAM_V);
EditOTLtek4->Text = IntToStr(T_KKAM);
EditOTLtek5->Text = IntToStr(T_PROC);
EditOTLtek6->Text = IntToStr(T_KUSTBM);
EditOTLtek7->Text = IntToStr(T_KZASL);
EditOTLtek8->Text = IntToStr(T_VKL_BPN);
EditOTLtek9->Text = IntToStr(T_KL);
EditOTLtek10->Text = IntToStr(TK_TMN1);
EditOTLtek11->Text = IntToStr(TK_TMN2);
EditOTLtek12->Text = IntToStr(TK_TMN3);
EditOTLtek13->Text = IntToStr(T_KKR);
EditOTLtek14->Text = IntToStr(T_KSOPR);
EditOTLtek15->Text = IntToStr(T_KVRASH);
EditOTLtek16->Text = IntToStr(T_KOST);
EditOTLtek17->Text = IntToStr(T_KKN_OTK);
EditOTLtek18->Text = IntToStr(T_ZASL);
EditOTLtek19->Text = IntToStr(T_KRAZGON);
EditOTLtek20->Text = IntToStr(T_VHG);
EditOTLtek21->Text = IntToStr(tk48);
EditOTLtek22->Text = IntToStr(tk49);
EditOTLtek23->Text = IntToStr(tkVrashPderj);
EditOTLtek24->Text = IntToStr(zaslUgolAbs);
EditOTLtek25->Text = IntToStr(pderjPutTek);
EditOTLtek26->Text = IntToStr(0);
EditOTLtek27->Text = IntToStr(0);
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;

//11 ��������

case 11:
{
EditOTLtek1->Text = IntToStr(CT_VODA_BM);
EditOTLtek2->Text = IntToStr(CT_VODA_II);
EditOTLtek3->Text = IntToStr(CT_VODA_KN);
EditOTLtek4->Text = IntToStr(CT_TEMP);
EditOTLtek5->Text = IntToStr(CT_BM);
EditOTLtek6->Text = IntToStr(CT_II);
EditOTLtek7->Text = IntToStr(CT_IST);
EditOTLtek8->Text = IntToStr(CT_TMN);
EditOTLtek9->Text = IntToStr(CT_KR);
EditOTLtek10->Text = IntToStr(CT_KR_T);
EditOTLtek11->Text = IntToStr(CT_BAR);
EditOTLtek12->Text = IntToStr(CT_NAP);
EditOTLtek13->Text = IntToStr(CT_KSOPR);
EditOTLtek14->Text = IntToStr(CT_VRASH_BAR);
EditOTLtek15->Text = IntToStr(CT_VKL_KN);
EditOTLtek16->Text = IntToStr(CT_KN);
EditOTLtek17->Text = IntToStr(CT_VHG);
EditOTLtek18->Text = IntToStr(PR_RAZKN);
EditOTLtek19->Text = IntToStr(PR_RC);
EditOTLtek20->Text = IntToStr(PR_DAVL_PODJ);
EditOTLtek21->Text = IntToStr(N_ST);
EditOTLtek22->Text = IntToStr(N_ST_MAX);
EditOTLtek23->Text = IntToStr(SOPR);
EditOTLtek24->Text = IntToStr(PR_VTMN);
EditOTLtek25->Text = IntToStr(PR_SOPR_BM);
EditOTLtek26->Text = IntToStr(Z_SOPR);
EditOTLtek27->Text = IntToStr(ZU_SOPR);
EditOTLtek28->Text = IntToStr(K_OSHIB);
EditOTLtek29->Text = FloatToStr(LogicT);
EditOTLtek30->Text = IntToStr(PR_Ar);
}; break;

//12 ��������

case 12:
{
EditOTLtek1->Text = IntToStr(VRGIS);
EditOTLtek2->Text = IntToStr(K_SOGL_GIS);
EditOTLtek3->Text = IntToStr(NAPRS_GIS);
EditOTLtek4->Text = IntToStr(X_TGIS);
EditOTLtek5->Text = IntToStr(E_TGIS);
EditOTLtek6->Text = IntToStr(DELGIS);
EditOTLtek7->Text = IntToStr(DOPGIS);
EditOTLtek8->Text = IntToStr(PAR_GIS);
EditOTLtek9->Text = IntToStr(N_TEK_GIS);
EditOTLtek10->Text = IntToStr(LIM1GIS);
EditOTLtek11->Text = IntToStr(LIM2GIS);
EditOTLtek12->Text = IntToStr(T_VRGIS);
EditOTLtek13->Text = IntToStr(T_KGIS);
EditOTLtek14->Text = IntToStr(0);
EditOTLtek15->Text = IntToStr(0);
EditOTLtek16->Text = IntToStr(KOM_DZ);
EditOTLtek17->Text = IntToStr(OTVET_DZ);
EditOTLtek18->Text = IntToStr(TYPE_DZ);
EditOTLtek19->Text = IntToStr(PR_DZ);
EditOTLtek20->Text = IntToStr(HOME_DZ);
EditOTLtek21->Text = IntToStr(PUT_DZ);
EditOTLtek22->Text = IntToStr(V_DZ);
EditOTLtek23->Text = IntToStr(TEK_ABS_DZ);
EditOTLtek24->Text = IntToStr(TEK_OTN_DZ);
EditOTLtek25->Text = IntToStr(CT_DZ);
EditOTLtek26->Text = IntToStr(0);
EditOTLtek27->Text = IntToStr(0);
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;

//13 ��������

case 13:
{
EditOTLtek1->Text = IntToStr(VRUN);
EditOTLtek2->Text = IntToStr(X_TUN);
EditOTLtek3->Text = IntToStr(E_TUN);
EditOTLtek4->Text = IntToStr(DELUN);
EditOTLtek5->Text = IntToStr(E_PUN);
EditOTLtek6->Text = IntToStr(K_PUN);
EditOTLtek7->Text = IntToStr(K_IUN);
EditOTLtek8->Text = IntToStr(U_PUN);
EditOTLtek9->Text = IntToStr(T_REQUN);
EditOTLtek10->Text = IntToStr(LIMPUN);
EditOTLtek11->Text = IntToStr(LIMIUN);
EditOTLtek12->Text = IntToStr(LIM1UN);
EditOTLtek13->Text = IntToStr(LIM2UN);
EditOTLtek14->Text = IntToStr(LIMUUN);
EditOTLtek15->Text = IntToStr(LIMU_UN);
EditOTLtek16->Text = IntToStr(LIMUPR_UN);
EditOTLtek17->Text = IntToStr(PORCNV_UN);
EditOTLtek18->Text = IntToStr(PORCPR_UN);
EditOTLtek19->Text = IntToStr(PROBUN);
EditOTLtek20->Text = IntToStr(0);
EditOTLtek21->Text = IntToStr(0);
EditOTLtek22->Text = IntToStr(int(360.0*M1_N/pderjAngle360));
EditOTLtek23->Text = IntToStr(int(360.0*M1_V/pderjAngle360));
EditOTLtek24->Text = IntToStr(int(360.0*M2_N/pderjAngle360));
EditOTLtek25->Text = IntToStr(int(360.0*M2_V/pderjAngle360));
EditOTLtek26->Text = IntToStr(int(360.0*VCHM_N/pderjAngle360));
EditOTLtek27->Text = IntToStr(int(360.0*VCHM_V/pderjAngle360));
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;

//14 ��������

case 14:
{
EditOTLtek1->Text = IntToStr(PR_TEMP);
EditOTLtek2->Text = IntToStr(KOM_TEMP);
EditOTLtek3->Text = IntToStr(ZAD_TEMP);
EditOTLtek4->Text = IntToStr(PAR_TEMP);
EditOTLtek5->Text = IntToStr(ZPAR_TEMP);
EditOTLtek6->Text = IntToStr(X_TEMP);
EditOTLtek7->Text = IntToStr(VRTEMP);
EditOTLtek8->Text = IntToStr(E_TEMP);
EditOTLtek9->Text = IntToStr(DELTEMP);
EditOTLtek10->Text = IntToStr(LIM1TEMP);
EditOTLtek11->Text = IntToStr(LIM2TEMP);
EditOTLtek12->Text = IntToStr(T_VRTEMP);
EditOTLtek13->Text = IntToStr(T_KTEMP);
EditOTLtek14->Text = IntToStr(DOPTEMP);
EditOTLtek15->Text = IntToStr(TEK_TEMP);
EditOTLtek16->Text = IntToStr(TEK_TEMP1);
EditOTLtek17->Text = IntToStr(TEK_TEMP2);
EditOTLtek18->Text = IntToStr(TEK_TEMP3);
EditOTLtek19->Text = IntToStr(TEK_TEMP4);
EditOTLtek20->Text = IntToStr(TEK_TEMP_PIR);
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

//15 ��������

case 15:
{
EditOTLtek1->Text = IntToStr(VRBM);
EditOTLtek2->Text = IntToStr(UST_BM);
EditOTLtek3->Text = IntToStr(X_TBM);
EditOTLtek4->Text = IntToStr(E_TBM);
EditOTLtek5->Text = IntToStr(DELBM);
EditOTLtek6->Text = IntToStr(DOPBM);
EditOTLtek7->Text = IntToStr(PAR_BM1);
EditOTLtek8->Text = IntToStr(PAR_BM2);
EditOTLtek9->Text = IntToStr(LIM1BM);
EditOTLtek10->Text = IntToStr(LIM2BM);
EditOTLtek11->Text = IntToStr(PR_SV_BM);
EditOTLtek12->Text = IntToStr(KOM_BM[0]);
EditOTLtek13->Text = IntToStr(KOM_BM[1]);
EditOTLtek14->Text = IntToStr(KOM_BM[2]);
EditOTLtek15->Text = IntToStr(KOM_BM[3]);
EditOTLtek16->Text = IntToStr(KOM_BM[4]);
EditOTLtek17->Text = IntToStr(OTVET_BM[0]);
EditOTLtek18->Text = IntToStr(OTVET_BM[1]);
EditOTLtek19->Text = IntToStr(OTVET_BM[2]);
EditOTLtek20->Text = IntToStr(OTVET_BM[3]);
EditOTLtek21->Text = IntToStr(OTVET_BM[4]);
EditOTLtek22->Text = IntToStr(OTVET_BM[5]);
EditOTLtek23->Text = IntToStr(OTVET_BM[6]);
EditOTLtek24->Text = IntToStr(OTVET_BM[7]);
EditOTLtek25->Text = IntToStr(OTVET_BM[8]);
EditOTLtek26->Text = IntToStr(OTVET_BM[9]);
EditOTLtek27->Text = IntToStr(0);
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;

//16 ��������

case 16:
{
EditOTLtek1->Text = IntToStr(VRII);
EditOTLtek2->Text = IntToStr(X_TII);
EditOTLtek3->Text = IntToStr(E_TII);
EditOTLtek4->Text = IntToStr(DELII);
EditOTLtek5->Text = IntToStr(DOPII);
EditOTLtek6->Text = IntToStr(PAR_II);
EditOTLtek7->Text = IntToStr(LIM1II);
EditOTLtek8->Text = IntToStr(LIM2II);
EditOTLtek9->Text = IntToStr(T_VRII);
EditOTLtek10->Text = IntToStr(T_KII);
EditOTLtek11->Text = IntToStr(PR_SV_II);
EditOTLtek12->Text = IntToStr(KOM_II[0]);
EditOTLtek13->Text = IntToStr(KOM_II[1]);
EditOTLtek14->Text = IntToStr(KOM_II[2]);
EditOTLtek15->Text = IntToStr(KOM_II[3]);
EditOTLtek16->Text = IntToStr(KOM_II[4]);
EditOTLtek17->Text = IntToStr(OTVET_II[0]);
EditOTLtek18->Text = IntToStr(OTVET_II[1]);
EditOTLtek19->Text = IntToStr(OTVET_II[2]);
EditOTLtek20->Text = IntToStr(OTVET_II[3]);
EditOTLtek21->Text = IntToStr(OTVET_II[4]);
EditOTLtek22->Text = IntToStr(OTVET_II[5]);
EditOTLtek23->Text = IntToStr(OTVET_II[6]);
EditOTLtek24->Text = IntToStr(OTVET_II[7]);
EditOTLtek25->Text = IntToStr(OTVET_II[8]);
EditOTLtek26->Text = IntToStr(OTVET_II[9]);
EditOTLtek27->Text = IntToStr(0);
EditOTLtek28->Text = IntToStr(0);
EditOTLtek29->Text = IntToStr(0);
EditOTLtek30->Text = IntToStr(0);
}; break;
    }
}
//---------------------------------------------------------------------------
//--������������ ��������--//
//---------------------------------------------------------------------------
void TForm1::VisualGraph()
{
    AnsiString graphTemp = "";

    if(!shr[2]&&!shr[4]) return;    // �� ������ ������ ���� ��� �� ��� ������ ��

    // �����
    graphTemp = Form1 -> Label_Time -> Caption + ";";
    // ������ ���1
    if(shr[20]) graphTemp = graphTemp + Form1 -> EdtTekA00 -> Text + ";";
    else graphTemp = graphTemp + "0,00;";
    // ������ ���2
    graphTemp = graphTemp + Form1 -> EdtTekA01 -> Text + ";";
    // �����������
    graphTemp = graphTemp + Form1 -> EdtTekA02 -> Text + ";";
    // ��� ��
    graphTemp = graphTemp + Form1 -> EdtTekA13 -> Text + ";";
    // �������� �1
    graphTemp = graphTemp + Form1 -> EdtTekA03 -> Text + ";";
    // �������� �2
    graphTemp = graphTemp + Form1 -> EdtTekA04 -> Text + ";";
    // ���������� �
    graphTemp = graphTemp + Form1 -> EdtTekA06 -> Text + ";";
    // ��� �
    graphTemp = graphTemp + Form1 -> EdtTekA07 -> Text + ";";

    // ��� �������� ���
    graphTemp = graphTemp + Form1 -> EdtTekA12 -> Text + ";";
    // ��� �������� ���
    graphTemp = graphTemp + Form1 -> EdtTekA13 -> Text + ";";

    // ��������
    graphTemp = graphTemp + Form1 -> EdtTekA09 -> Text + ";";
    
    // �������������
    if(SOPR < 9000) graphTemp = graphTemp + Form1 -> EdtTekA08 -> Text + ";";
    else graphTemp = graphTemp + "0,0;";
    // �������� ������
    Form1 -> MemoGraph -> Lines -> Add ( graphTemp );

    // ������ ���1
    if(shr[20]) serTemp[1] -> AddY(StrToFloat ( Form1 -> EdtTekA00 -> Text ), Form1 -> Label_Time -> Caption );
    else serTemp[1] -> AddY(0.00, Form1 -> Label_Time -> Caption );
    // ������ ���2
    serTemp[2] -> AddY(StrToFloat ( Form1 -> EdtTekA01 -> Text ), Form1 -> Label_Time -> Caption );
    // �����������
    serTemp[3] -> AddY(StrToFloat ( Form1 -> EdtTekA02 -> Text ), Form1 -> Label_Time -> Caption );
    // ��� ��
    serTemp[4] -> AddY(StrToFloat ( Form1 -> EdtTekA13 -> Text ), Form1 -> Label_Time -> Caption );
    // �������� �1
    serTemp[5] -> AddY(StrToFloat ( Form1 -> EdtTekA03 -> Text ),Form1 -> Label_Time -> Caption );
    // �������� �2
    serTemp[6] -> AddY(StrToFloat ( Form1 -> EdtTekA04 -> Text ),Form1 -> Label_Time -> Caption );
    // ���������� �
    serTemp[7] -> AddY(StrToFloat ( Form1 -> EdtTekA06 -> Text ), Form1 -> Label_Time -> Caption );
    // ��� �
    serTemp[8] -> AddY(StrToFloat ( Form1 -> EdtTekA07 -> Text ), Form1 -> Label_Time -> Caption );
    // ��� �������� ���
    serTemp[9] -> AddY(StrToFloat ( Form1 -> EdtTekA12 -> Text ), Form1 -> Label_Time -> Caption );
    // ��� �������� ���
    serTemp[10] -> AddY(StrToFloat ( Form1 -> EdtTekA13 -> Text ), Form1 -> Label_Time -> Caption );
    // ��������
    serTemp[11] -> AddY(StrToFloat ( Form1 -> EdtTekA09 -> Text ), Form1 -> Label_Time -> Caption );

    // �������������
    if(SOPR < 9000)
    serTemp[12] -> AddY(StrToFloat ( Form1 -> EdtTekA08 -> Text ), Form1 -> Label_Time -> Caption );
    else serTemp[12] -> AddY(0.0, Form1 -> Label_Time -> Caption );
}
//---------------------------------------------------------------------------
//--������������ �������� �������--//
//---------------------------------------------------------------------------
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
      {   Dec_Ain[i][j] -> Text = IntToStr(aik[i*8+j]);
          CG_Ain[i][j] -> Progress = aik[i*8+j];
          if ((i*8+j) == 15) { UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/32767.0 * 10.0, ffFixed, 5, 3); }
          else               { UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/4095.0 * 10.0,  ffFixed, 5, 3); }
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
//---------------------------------------------------------------------------
//--���������� ���������� ��������--//
//---------------------------------------------------------------------------
void SetOut(bool value, unsigned char outNmb, unsigned int outMask)
{   // ���������� �������� ������ ���������
    value ? out[outNmb] |= outMask : out[outNmb] &= (~outMask);
}
//---------------------------------------------------------------------------
void SetZin(bool value, unsigned char outNmb, unsigned int outMask)
{   // ���������� �������� ������ ���������
    value ? zin[outNmb] |= outMask : zin[outNmb] &= (~outMask);
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
void __fastcall TForm1::SetOutClick(TObject *Sender)
{   // ��������� OUT��
    unsigned char bitNmb  = PC_Out -> TabIndex;
    unsigned int  bitMask = StrToInt(((TButton*)Sender)->Hint);
    // ��������� ������
    out[bitNmb] & bitMask ? SetOut(0, bitNmb, bitMask) : SetOut(1, bitNmb, bitMask);
    // ������������ ������
    VisualFormat();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SetZinClick(TObject *Sender)
{
    if(pr_otl)
    {
        // ��������� OUT��
        unsigned char bitNmb = PC_Zin -> TabIndex;
        unsigned int  bitMask = StrToInt(((TButton*)Sender)->Hint);
        // ��������� ������
        zin[bitNmb] & bitMask ? SetZin(0, bitNmb, bitMask) : SetZin(1, bitNmb, bitMask);
        // ������������ ������
        VisualFormat();
    }
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
   VisualFormat();
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
void __fastcall TForm1::BtnDebugClick(TObject *Sender)
{
  // ���������/���������� ����������� ������
  LabelOTLreg -> Visible = pr_otl = (bool)StrToInt(((TButton*)Sender)->Hint);
}
//---------------------------------------------------------------------------
// �����������/������� ����� ������� ����������
void __fastcall TForm1::BitBdVallClick(TObject *Sender)
{
    PanelDiagm -> Visible = (bool)StrToInt(((TButton*)Sender)->Hint);
}
//---------------------------------------------------------------------------
// ���� ���������� ����������
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
    ((TEdit*)Sender)->Color = clSilver;

        // �����������
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_12") ||
            (((TEdit*)Sender)->Name == "EdtARed3_12") ||
            (((TEdit*)Sender)->Name == "EdtARed4_12") ||
            (((TEdit*)Sender)->Name == "EdtARed5_12") ||
            (((TEdit*)Sender)->Name == "EdtARed6_12")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 1;
        // ��������� �� �������
        if ((valueText < 20.0) && (valueText != 0.0))
        {
            valueText = 20.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 350.0)
        {
            valueText = 350.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ����������� ������
    else if  (
            (((TEdit*)Sender)->Name == "EdtARed7_12")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 20.0)
        {
            valueText = 20.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 350.0)
        {
            valueText = 350.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ����� ��������
    else if  (
            (((TEdit*)Sender)->Name == "EdtARed1_5") ||
            (((TEdit*)Sender)->Name == "EdtARed2_5") ||
            (((TEdit*)Sender)->Name == "EdtARed4_5") ||
            (((TEdit*)Sender)->Name == "EdtARed5_5") ||
            (((TEdit*)Sender)->Name == "EdtARed6_5") ||
            (((TEdit*)Sender)->Name == "EdtARed7_5") ||
            (((TEdit*)Sender)->Name == "EdtTRed9_5") ||
            (((TEdit*)Sender)->Name == "EdtTRed10_5") ||
            (((TEdit*)Sender)->Name == "EdtTRed11_5")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>3600.0)
        {
            valueText = 3600.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ����� ��������
    else if  (
            (((TEdit*)Sender)->Name == "EdtARed3_5")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>600.0)
        {
            valueText = 600.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������ ���
    else if  (
              (((TEdit*)Sender)->Name == "EdtARed2_0") ||
              (((TEdit*)Sender)->Name == "EdtARed4_1") ||
              (((TEdit*)Sender)->Name == "EdtARed5_1") ||
              (((TEdit*)Sender)->Name == "EdtARed6_1") ||
              (((TEdit*)Sender)->Name == "EdtRZad0") ||
              (((TEdit*)Sender)->Name == "EdtRZad1")

              )
    {
        // ���-�� ������ ����� ������� 2
        format = 2;
        // ��������� �� �������
        if (valueText<0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>0.90)
        {
            valueText = 0.90;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ��� ��
    else if  (
              ((TEdit*)Sender)->Name == "EdtARed2_3" ||
              (((TEdit*)Sender)->Name == "EdtRZad7")
              )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>200.0)
        {
            valueText = 200.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // ������� �������� ��
    else if (
                (((TEdit*)Sender)->Name == "EdtARed2_6") ||
                (((TEdit*)Sender)->Name == "EdtARed4_6") ||
                (((TEdit*)Sender)->Name == "EdtARed5_6") ||
                (((TEdit*)Sender)->Name == "EdtARed6_6") ||
                (((TEdit*)Sender)->Name == "EdtTRed9_6") ||
                (((TEdit*)Sender)->Name == "EdtTRed10_6") ||
                (((TEdit*)Sender)->Name == "EdtTRed11_6") ||
                (((TEdit*)Sender)->Name == "EdtRZad6")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 1;
        // ��������� �� �������
        if (valueText<0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>100.0)
        {
            valueText = 100.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� �1 M2
    else if (
                (((TEdit*)Sender)->Name == "EdtARed4_8") ||
                (((TEdit*)Sender)->Name == "EdtARed5_8") ||
                (((TEdit*)Sender)->Name == "EdtARed6_8") ||
                (((TEdit*)Sender)->Name == "EdtARed4_9") ||
                (((TEdit*)Sender)->Name == "EdtARed5_9") ||
                (((TEdit*)Sender)->Name == "EdtARed6_9") ||
                (((TEdit*)Sender)->Name == "EdtTRed9_8") ||
                (((TEdit*)Sender)->Name == "EdtTRed10_9") ||
                (((TEdit*)Sender)->Name == "EdtRZad8") ||
                (((TEdit*)Sender)->Name == "EdtRZad9")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<BPM1_P_MIN)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 3000)
        {
            valueText = 3000;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }



    // ��� �������� ���
    else if (
                (((TEdit*)Sender)->Name == "EdtARed4_10") ||
                (((TEdit*)Sender)->Name == "EdtARed5_10") ||
                (((TEdit*)Sender)->Name == "EdtARed6_10") ||
                (((TEdit*)Sender)->Name == "EdtTRed11_10") ||
                (((TEdit*)Sender)->Name == "EdtRZad10")


            )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > COMET_MAX_PD)
        {
            valueText = COMET_MAX_PD;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���� ��
    else if (
                (((TEdit*)Sender)->Name == "EdtARed4_13")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText > 13)
        {
            valueText = 13;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� ������
    else if (
                (((TEdit*)Sender)->Name == "EdtRZad2")
            )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText<0.2)
        {
            valueText = 0.2;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>1.5)
        {
            valueText = 1.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� [���������] ��������
    else if (
                (((TEdit*)Sender)->Name == "EdtARed4_2")||
                (((TEdit*)Sender)->Name == "EdtARed5_2")||
                (((TEdit*)Sender)->Name == "EdtARed6_2")

            )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if ((valueText<0.4)&&(valueText!=0.0))
        {
            valueText = 0.4;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>1.5)
        {
            valueText = 1.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� ��� ���
    else if (
                (((TEdit*)Sender)->Name == "EdtARed4_15")||
                (((TEdit*)Sender)->Name == "EdtARed5_15")||
                (((TEdit*)Sender)->Name == "EdtARed6_15")||
                (((TEdit*)Sender)->Name == "EdtTRed11_15")
            )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if ((valueText < 0.4)  )
        {
            valueText = 0.4;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>1.5)
        {
            valueText = 1.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� [����������]
    else if (
                (((TEdit*)Sender)->Name == "EdtARed9_2")||
                (((TEdit*)Sender)->Name == "EdtARed10_2")||
                (((TEdit*)Sender)->Name == "EdtARed11_2")


            )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if ((valueText<0.2)&&(valueText!=0.0))
        {
            valueText = 0.2;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>1.1)
        {
            valueText = 1.1;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������������
    else if  (
            (((TEdit*)Sender)->Name == "EdtARed4_17") ||
            (((TEdit*)Sender)->Name == "EdtARed5_17") ||
            (((TEdit*)Sender)->Name == "EdtARed6_17")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if ((valueText < 1.0)&&(valueText != 0.0))
        {
            valueText = 1.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 10000.0)
        {
            valueText = 10000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // ����������� [���������]
    else if  (
            (((TEdit*)Sender)->Name == "EdtARed8_12")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 1;
        // ��������� �� �������
        if ((valueText < 20.0) && (valueText != 0.0))
        {
            valueText = 20.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 200.0)
        {
            valueText = 200.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // ����� �������� [���������]
    else if (
                (((TEdit*)Sender)->Name == "EdtARed8_5")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>9000.0)
        {
            valueText = 9000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���������� ��������
    else if  (
            (((TEdit*)Sender)->Name == "EdtTRed9_2") ||
            (((TEdit*)Sender)->Name == "EdtTRed10_2") ||
            (((TEdit*)Sender)->Name == "EdtTRed11_2")
        )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText<0.2)
        {
            valueText = 0.2;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>1.1)
        {
            valueText = 1.1;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // ������� �������� ��������
    else if (
                (((TEdit*)Sender)->Name == "EdtRZad11")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>360)
        {
            valueText = 360;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // ����������� �������
    else if (
                (((TEdit*)Sender)->Name == "EdtRZad12")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 1;
        // ��������� �� �������
        if (valueText<20.0)
        {
            valueText = 20.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>350.0)
        {
            valueText = 350.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���� �� �������
    else if (
                (((TEdit*)Sender)->Name == "EdtRZad13")
            )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<-90000.0)
        {
            valueText = -90000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>90000.0)
        {
            valueText = 90000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // ������������ ���������� �� �����������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo19")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<100.0)
        {
            valueText = 100.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>650.0)
        {
            valueText = 650.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }


    // �������� ���������� �������� ��� ������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo4") ||
            (((TEdit*)Sender)->Name == "EditNastrTo6")
        )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<250.0)
        {
            valueText = 250.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>4000.0)
        {
            valueText = 4000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // �������� ��������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo11") ||
            (((TEdit*)Sender)->Name == "EditNastrTo23") ||
            (((TEdit*)Sender)->Name == "EditNastrTo15") ||
            (((TEdit*)Sender)->Name == "EditNastrTo12") ||
            (((TEdit*)Sender)->Name == "EditNastrTo16")

             )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<1.0)
        {
            valueText = 1.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>13.0)
        {
            valueText = 13.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������� ����������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo30") ||
            (((TEdit*)Sender)->Name == "EditNastrTo31")
             )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if ((valueText < 4.0) && (valueText != 0.0))
        {
            valueText = 4.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 40.0)
        {
            valueText = 40.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���� ���������� �� �����������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo19")

             )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText<100.0)
        {
            valueText = 100.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>650.0)
        {
            valueText = 650.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }



    // ���������� ��������  ���
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo25")
             )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if ((valueText < 100.0) && (valueText != 0.0))
        {
            valueText = 100.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 500.0)
        {
            valueText = 500.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ���������� ��������  ���
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo37")
             )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if ((valueText < 100.0) && (valueText != 0.0))
        {
            valueText = 100.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText> 300.0)
        {
            valueText = 300.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // ������������� ��������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo26")

             )
    {
        // ���-�� ������ ����� ������� 1
        format = 0;
        // ��������� �� �������
        if (valueText<0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // ��������� �� ��������
        else if (valueText>10000.0)
        {
            valueText = 10000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }


    // ���� �������� �������� / ��������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo2") ||
            (((TEdit*)Sender)->Name == "EditNastrTo1")
             )
    {
        // ���-�� ������ ����� ������� 1
        format = 1;
        // ��������� �� �������
        if (valueText < 0.0)
        {
            valueText = 0.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        if (valueText > 360.0)
        {
            valueText = 360.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //����� ������
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo5") ||
            (((TEdit*)Sender)->Name == "EditNastrTo7") ||
            (((TEdit*)Sender)->Name == "EditNastrTo29")
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
        else if (valueText > 4000.0)
        {
            valueText = 4000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //���������� �� ���
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo35")
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
        else if (valueText > 20)
        {
            valueText = 20;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //������������ �������� ���1/���2
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo38") ||
            (((TEdit*)Sender)->Name == "EditNastrTo39") ||
            (((TEdit*)Sender)->Name == "EditNastrTo29")
             )
    {
        // ���-�� ������ ����� ������� 0
        format = 1;
        // ��������� �� �������
        if (valueText < 0.8)
        {
            valueText = 0.8;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > 2.0)
        {
            valueText = 2.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    //����������� ��
    else if  (
            (((TEdit*)Sender)->Name == "EditNastrTo33")
             )
    {
        // ���-�� ������ ����� ������� 0
        format = 0;
        // ��������� �� �������
        if (valueText < 15)
        {
            valueText = 15;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > 22)
        {
            valueText = 22;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }



    // �������� �� ���������
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 5, format);

    VisualParA();
}
//---------------------------------------------------------------------------
// �������� ���������� ��������
void __fastcall TForm1::BtnAutoDaClick(TObject *Sender)
{
    if(((TButton*)Sender)->Hint == "2")  // ����� ���� �������������
    {
        PanelParA -> Visible = true;
        return;
    }

    if(((TButton*)Sender)->Hint == "0")  // ������
    {
        PanelParA -> Visible = false;
        return;
    }

    PanelParA -> Visible = false;

    // �����������
    par[1][12] = int(StrToFloat(EdtARed1_12 -> Text) * 10.0);
    // ����� ��������
    par[1][5] = StrToInt(EdtARed1_5->Text);

    // ������ �������
    // ������ ���1
    par[2][0] = int(StrToFloat(EdtARed2_0 -> Text) * 4095.0 / RRG1_MAX);
    // ��� ��
    par[2][3] = int(StrToFloat(EdtARed2_3 -> Text) * 4095.0 / I_MAX);
    // ����� ��������
    par[2][5] = StrToInt(EdtARed2_5->Text);
    // ������� �������� ��
    par[2][6] = int(StrToFloat(EdtARed2_6 -> Text));

    // ������
    // �����������
    par[3][12] = int(StrToFloat(EdtARed3_12 -> Text) * 10.0);
    // ����� ��������
    par[3][5] = StrToInt(EdtARed3_5->Text);

    // ��������� 1
    // �����������
    par[4][12] = int(StrToFloat(EdtARed4_12 -> Text) * 10.0);
    // ������ ���2
    par[4][1] = int(StrToFloat(EdtARed4_1 -> Text) * 4095.0 / RRG2_MAX);
    // �������� �1
    par[4][8] = int(StrToFloat(EdtARed4_8 -> Text) * 4095.0 / BPM1_P_MAX);
    // �������� �2
    par[4][9] = int(StrToFloat(EdtARed4_9 -> Text) * 4095.0 / BPM2_P_MAX);
    // ��� �������� ���
    par[4][10] = int(StrToFloat(EdtARed4_10 -> Text) * 4095.0 / COMET_MAX_PD);                    //������

    // �������� ������� ��� ���
    if(StrToFloat(EdtARed4_15->Text) == 0)
        par[4][15] = 0;
    else
        par[4][15] = int((0.6*log10(StrToFloat(EdtARed4_15->Text)/100.0)+6.8)*1000.0);
    // ��������
    if(StrToFloat(EdtARed4_2->Text) == 0)
        par[4][2] = 0;
    else
        par[4][2] = int((0.6*log10(StrToFloat(EdtARed4_2->Text)/100.0)+6.8)*1000.0);
    // �������������
    par[4][17] = int(StrToFloat(EdtARed4_17 -> Text) * 10.0);
    // ����� ��������
    par[4][5] = StrToInt(EdtARed4_5->Text);
    // ������� �������� ��
    par[4][6] = int(StrToFloat(EdtARed4_6 -> Text) );

    // ��������� 2
    // �����������
    par[5][12] = int(StrToFloat(EdtARed5_12 -> Text) * 10.0);
    // ������ ���2
    par[5][1] = int(StrToFloat(EdtARed5_1 -> Text) * 4095.0 / RRG2_MAX);
    // �������� �1
    par[5][8] = int(StrToFloat(EdtARed5_8 -> Text) * 4095.0 / BPM1_P_MAX);
    // �������� �2
    par[5][9] = int(StrToFloat(EdtARed5_9 -> Text) * 4095.0 / BPM2_P_MAX);
    // ��� �������� ���
    par[5][10] = int(StrToFloat(EdtARed5_10 -> Text) * 4095.0 / COMET_MAX_PD);                     //������
    // �������� ������� ��� ���
    if(StrToFloat(EdtARed5_15->Text) == 0)
        par[5][15] = 0;
    else
        par[5][15] = int((0.6*log10(StrToFloat(EdtARed5_15->Text)/100.0)+6.8)*1000.0);
    // ��������
    if(StrToFloat(EdtARed5_2->Text) == 0)
        par[5][2] = 0;
    else
        par[5][2] = int((0.6*log10(StrToFloat(EdtARed5_2->Text)/100.0)+6.8)*1000.0);
    // �������������
    par[5][17] = int(StrToFloat(EdtARed5_17 -> Text) * 10.0);
    // ����� ��������
    par[5][5] = StrToInt(EdtARed5_5->Text);
    // ������� �������� ��
    par[5][6] = int(StrToFloat(EdtARed5_6 -> Text) );

    // ��������� 3
    // �����������
    par[6][12] = int(StrToFloat(EdtARed6_12 -> Text) * 10.0);
    // ������ ���2
    par[6][1] = int(StrToFloat(EdtARed6_1 -> Text) * 4095.0 / RRG2_MAX);
    // �������� �1
    par[6][8] = int(StrToFloat(EdtARed6_8 -> Text) * 4095.0 / BPM1_P_MAX);
    // �������� �2
    par[6][9] = int(StrToFloat(EdtARed6_9 -> Text) * 4095.0 / BPM2_P_MAX);
    // ��� �������� ���
    par[6][10] = int(StrToFloat(EdtARed6_10 -> Text) * 4095.0 / COMET_MAX_PD);                             //������
    // �������� ������� ��� ���
    if(StrToFloat(EdtARed6_15->Text) == 0)
        par[6][15] = 0;
    else
        par[6][15] = int((0.6*log10(StrToFloat(EdtARed6_15->Text)/100.0)+6.8)*1000.0);
    // ��������
    if(StrToFloat(EdtARed6_2->Text) == 0)
        par[6][2] = 0;
    else
        par[6][2] = int((0.6*log10(StrToFloat(EdtARed6_2->Text)/100.0)+6.8)*1000.0);
    // �������������
    par[6][17] = int(StrToFloat(EdtARed6_17 -> Text) * 10.0);
    // ����� ��������
    par[6][5] = StrToInt(EdtARed6_5->Text);
    // ������� �������� ��
    par[6][6] = int(StrToFloat(EdtARed6_6 -> Text));



    // �����
    // �����������
    par[7][12] = int(StrToFloat(EdtARed7_12 -> Text) * 10.0);
    // ����� ��������
    par[7][5] = StrToInt(EdtARed7_5->Text);

    // ���������
    // �����������
    par[8][12] = int(StrToFloat(EdtARed8_12 -> Text) * 10.0);
    // ����� ��������
    par[8][5] = StrToInt(EdtARed8_5->Text);


    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ��������� �������������� ������:");

    // ��������������� ������
    // �����������
    if ( EdtAKon1_12 -> Text != EdtARed1_12 -> Text )
        MemoStat -> Lines -> Add("��������������� ������: �����������:" + EdtAKon1_12 -> Text + " -> " + EdtARed1_12 -> Text );
    if ( EdtAKon1_5 -> Text != EdtARed1_5 -> Text )
        MemoStat -> Lines -> Add("��������������� ������: ����� ��������:" + EdtAKon1_5 -> Text + " -> " + EdtARed1_5 -> Text );

    // ������ �������
    // ������ ���1
    if ( EdtAKon2_0 -> Text != EdtARed2_0 -> Text )
        MemoStat -> Lines -> Add("������ �������: ������ ���1:" + EdtAKon2_0 -> Text + " -> " + EdtARed2_0 -> Text );
    // ��� ��
    if ( EdtAKon2_3 -> Text != EdtARed2_3 -> Text )
        MemoStat -> Lines -> Add("������ �������: ��� ��:" + EdtAKon2_3 -> Text + " -> " + EdtARed2_3 -> Text );
    // ����� ��������
    if ( EdtAKon2_5 -> Text != EdtARed2_5 -> Text )
        MemoStat -> Lines -> Add("������ �������: ����� ��������:" + EdtAKon2_5 -> Text + " -> " + EdtARed2_5 -> Text );
    // ������� �������� ��
    if ( EdtAKon2_6 -> Text != EdtARed2_6 -> Text )
        MemoStat -> Lines -> Add("������ �������: ������� �������� ��:" + EdtAKon2_6 -> Text + " -> " + EdtARed2_6 -> Text );

    // ������
    // �����������
    if ( EdtAKon3_12 -> Text != EdtARed3_12 -> Text )
        MemoStat -> Lines -> Add("������: �����������:" + EdtAKon3_12 -> Text + " -> " + EdtARed3_12 -> Text );
    // ����� ��������
    if ( EdtAKon3_5 -> Text != EdtARed3_5 -> Text )
        MemoStat -> Lines -> Add("������: ����� ��������:" + EdtAKon3_5 -> Text + " -> " + EdtARed3_5 -> Text );

    // ��������� 1 ����
    if ( EdtAKon4_12 -> Text != EdtARed4_12 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: �����������:" + EdtAKon4_12 -> Text + " -> " + EdtARed4_12 -> Text );
    // ������ ���2
    if ( EdtAKon4_1 -> Text != EdtARed4_1 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: ������ ���2:" + EdtAKon4_1 -> Text + " -> " + EdtARed4_1 -> Text );
    // �������� �1
    if ( EdtAKon4_8 -> Text != EdtARed4_8 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: �������� �1:" + EdtAKon4_8 -> Text + " -> " + EdtARed4_8 -> Text );
    // �������� �2
    if ( EdtAKon4_9 -> Text != EdtARed4_9 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: �������� �2:" + EdtAKon4_9 -> Text + " -> " + EdtARed4_9 -> Text );
    // ��� �������� ���
    if ( EdtAKon4_10 -> Text != EdtARed4_10 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: ���. �������� ���:" + EdtAKon4_10 -> Text + " -> " + EdtARed4_10 -> Text );
    // �������� ������� ��� ���
    if ( EdtAKon4_15 -> Text != EdtARed4_15 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: �������� ������� ��� ���:" + EdtAKon4_15 -> Text + " -> " + EdtARed4_15 -> Text );
    // ��������
    if ( EdtAKon4_2 -> Text != EdtARed4_2 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: ��������:" + EdtAKon4_2 -> Text + " -> " + EdtARed4_2 -> Text );
    // �������������
    if ( EdtAKon4_17 -> Text != EdtARed4_17 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: �������������:" + EdtAKon4_17 -> Text + " -> " + EdtARed4_17 -> Text );
    // ����� ��������
    if ( EdtAKon4_5 -> Text != EdtARed4_5 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: ����� ��������:" + EdtAKon4_5 -> Text + " -> " + EdtARed4_5 -> Text );
    // ������� �������� ��
    if ( EdtAKon4_6 -> Text != EdtARed4_6 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 1: ������� �������� ��:" + EdtAKon4_6 -> Text + " -> " + EdtARed4_6 -> Text );

    // ��������� 2 ����
    if ( EdtAKon5_12 -> Text != EdtARed5_12 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: �����������:" + EdtAKon5_12 -> Text + " -> " + EdtARed5_12 -> Text );
    // ������ ���2
    if ( EdtAKon5_1 -> Text != EdtARed5_1 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: ������ ���2:" + EdtAKon5_1 -> Text + " -> " + EdtARed5_1 -> Text );
    // �������� �1
    if ( EdtAKon5_8 -> Text != EdtARed5_8 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: �������� �1:" + EdtAKon5_8 -> Text + " -> " + EdtARed5_8 -> Text );
    // �������� �2
    if ( EdtAKon5_9 -> Text != EdtARed5_9 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: �������� �2:" + EdtAKon5_9 -> Text + " -> " + EdtARed5_9 -> Text );
    // ��� �������� ���
    if ( EdtAKon5_10 -> Text != EdtARed5_10 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: ���. �������� ���:" + EdtAKon5_10 -> Text + " -> " + EdtARed5_10 -> Text );
    // �������� ������� ��� ���
    if ( EdtAKon5_15 -> Text != EdtARed5_15 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: �������� ������� ��� ���:" + EdtAKon5_15 -> Text + " -> " + EdtARed5_15 -> Text );
    // ��������
    if ( EdtAKon5_2 -> Text != EdtARed5_2 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: ��������:" + EdtAKon5_2 -> Text + " -> " + EdtARed5_2 -> Text );
    // �������������
    if ( EdtAKon5_17 -> Text != EdtARed5_17 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: �������������:" + EdtAKon5_17 -> Text + " -> " + EdtARed5_17 -> Text );
    // ����� ��������
    if ( EdtAKon5_5 -> Text != EdtARed5_5 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: ����� ��������:" + EdtAKon5_5 -> Text + " -> " + EdtARed5_5 -> Text );
    // ������� �������� ��
    if ( EdtAKon5_6 -> Text != EdtARed5_6 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 2: ������� �������� ��:" + EdtAKon5_6 -> Text + " -> " + EdtARed5_6 -> Text );

    // ��������� 3 ����
    if ( EdtAKon6_12 -> Text != EdtARed6_12 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: �����������:" + EdtAKon6_12 -> Text + " -> " + EdtARed6_12 -> Text );
    // ������ ���2
    if ( EdtAKon6_1 -> Text != EdtARed6_1 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: ������ ���2:" + EdtAKon6_1 -> Text + " -> " + EdtARed6_1 -> Text );
    // �������� �1
    if ( EdtAKon6_8 -> Text != EdtARed6_8 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: �������� �1:" + EdtAKon6_8 -> Text + " -> " + EdtARed6_8 -> Text );
    // �������� �2
    if ( EdtAKon6_9 -> Text != EdtARed6_9 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: �������� �2:" + EdtAKon6_9 -> Text + " -> " + EdtARed6_9 -> Text );
    // ��� �������� ���
    if ( EdtAKon6_10 -> Text != EdtARed6_10 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: ���. �������� ���:" + EdtAKon6_10 -> Text + " -> " + EdtARed6_10 -> Text );
    // �������� ������� ��� ���
    if ( EdtAKon6_15 -> Text != EdtARed6_15 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: �������� ������� ��� ���:" + EdtAKon6_15 -> Text + " -> " + EdtARed6_15 -> Text );
    // ��������
    if ( EdtAKon6_2 -> Text != EdtARed6_2 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: ��������:" + EdtAKon6_2 -> Text + " -> " + EdtARed6_2 -> Text );
    // �������������
    if ( EdtAKon6_17 -> Text != EdtARed6_17 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: �������������:" + EdtAKon6_17 -> Text + " -> " + EdtARed6_17 -> Text );
    // ����� ��������
    if ( EdtAKon6_5 -> Text != EdtARed6_5 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: ����� ��������:" + EdtAKon6_5 -> Text + " -> " + EdtARed6_5 -> Text );
    // ������� �������� ��
    if ( EdtAKon6_6 -> Text != EdtARed6_6 -> Text )
        MemoStat -> Lines -> Add("��������� ���� 3: ������� �������� ��:" + EdtAKon6_6 -> Text + " -> " + EdtARed6_6 -> Text );

    // �����
    // �����������
    if ( EdtAKon7_12 -> Text != EdtARed7_12 -> Text )
        MemoStat -> Lines -> Add("�����: �����������:" + EdtAKon7_12 -> Text + " -> " + EdtARed7_12 -> Text );
    // ����� ��������
    if ( EdtAKon7_5 -> Text != EdtARed7_5 -> Text )
        MemoStat -> Lines -> Add("�����: ����� ��������:" + EdtAKon7_5 -> Text + " -> " + EdtARed7_5 -> Text );

    // ���������
    // �����������
    if ( EdtAKon8_12 -> Text != EdtARed8_12 -> Text )
        MemoStat -> Lines -> Add("���������: �����������:" + EdtAKon8_12 -> Text + " -> " + EdtARed8_12 -> Text );
    // ����� ��������
    if ( EdtAKon8_5 -> Text != EdtARed8_5 -> Text )
        MemoStat -> Lines -> Add("���������: ����� ��������:" + EdtAKon8_5 -> Text + " -> " + EdtARed8_5 -> Text );


    // ����������� ���������� ���������
    EdtARed1_12 -> Color = clWhite;
    EdtARed1_5 -> Color = clWhite;

    EdtARed2_0 -> Color = clWhite;
    EdtARed2_3 -> Color = clWhite;
    EdtARed2_5 -> Color = clWhite;
    EdtARed2_6 -> Color = clWhite;

    EdtARed3_12 -> Color = clWhite;
    EdtARed3_5 -> Color = clWhite;

    EdtARed4_12 -> Color = clWhite;
    EdtARed4_1 -> Color = clWhite;
    EdtARed4_8 -> Color = clWhite;
    EdtARed4_9 -> Color = clWhite;
    EdtARed4_10 -> Color = clWhite;
    EdtARed4_15 -> Color = clWhite;
    EdtARed4_2 -> Color = clWhite;
    EdtARed4_17 -> Color = clWhite;
    EdtARed4_5 -> Color = clWhite;
    EdtARed4_6 -> Color = clWhite;

    EdtARed5_12 -> Color = clWhite;
    EdtARed5_1 -> Color = clWhite;
    EdtARed5_8 -> Color = clWhite;
    EdtARed5_9 -> Color = clWhite;
    EdtARed5_10 -> Color = clWhite;
    EdtARed5_15 -> Color = clWhite;
    EdtARed5_2 -> Color = clWhite;
    EdtARed5_17 -> Color = clWhite;
    EdtARed5_5 -> Color = clWhite;
    EdtARed5_6 -> Color = clWhite;

    EdtARed6_12 -> Color = clWhite;
    EdtARed6_1 -> Color = clWhite;
    EdtARed6_8 -> Color = clWhite;
    EdtARed6_9 -> Color = clWhite;
    EdtARed6_10 -> Color = clWhite;
    EdtARed6_15 -> Color = clWhite;
    EdtARed6_2 -> Color = clWhite;
    EdtARed6_17 -> Color = clWhite;
    EdtARed6_5 -> Color = clWhite;
    EdtARed6_6 -> Color = clWhite;

    EdtARed7_12 -> Color = clWhite;
    EdtARed7_5 -> Color = clWhite;

    EdtARed8_12 -> Color = clWhite;
    EdtARed8_5 -> Color = clWhite;

    // �������� ��������
    VisualParA();              
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CB_stageChange(TObject *Sender)
{
  // ������� ������� ����� ���� � ������� ������ � ����������� ����
  if(CB_stage -> ItemIndex == 0)
  {
        Pnl_Nap4 -> Visible = true;
        Pnl_Nap5 -> Visible = false;
        Pnl_Nap6 -> Visible = false;
  }
  else if(CB_stage -> ItemIndex == 1)
  {
        Pnl_Nap4 -> Visible = false;
        Pnl_Nap5 -> Visible = true;
        Pnl_Nap6 -> Visible = false;
  }
  else if(CB_stage -> ItemIndex == 2)
  {
        Pnl_Nap4 -> Visible = false;
        Pnl_Nap5 -> Visible = false;
        Pnl_Nap6 -> Visible = true;
  }

}
//---------------------------------------------------------------------------
// �������� ������������ �������
void __fastcall TForm1::BtnNastrDaClick(TObject *Sender)
{

    if(((TButton*)Sender)->Hint == "2")  // ����� ���� �������������
    {
        PanelParNastr -> Visible = true;
        return;
    }
    if(((TButton*)Sender)->Hint == "0")  // ������
    {
        PanelParNastr -> Visible = false;
        return;
    }
    PanelParNastr -> Visible = false;

    // �������� ������������ �������
    // 1 - ���������� ������� ��������������� ������� ������
    nasmod[0] = int((0.6*log10(StrToFloat(EditNastrTo0->Text)/100.0)+6.8)*1000.0);
    // 2 - �������  ������� ������ ����� ��������
    nasmod[22] = int((0.6*log10(StrToFloat(EditNastrTo22->Text)/100.0)+6.8)*1000.0);
    // 3 - ���� �������� ����.
    nasmod[2] = int(StrToFloat(EditNastrTo2->Text)*zaslAngle360/360.0);
    // 4 - ���� �������� ��������
    nasmod[1] = int(StrToFloat(EditNastrTo1->Text)*pderjAngle360/360.0);
    // 5 - �������� ���������� �������� �1 ��� ������
    nasmod[4] = int(StrToFloat(EditNastrTo4->Text)*4095.0/12.0/BPM1_P_MAX);
    // 6 - ����� ������ �1
    nasmod[5] = StrToInt(EditNastrTo5 -> Text);
    // 7 - �������� ���������� �������� �2 ��� ������
    nasmod[6] = int(StrToFloat(EditNastrTo6 -> Text)*4095.0/12.0/BPM2_P_MAX);
    // 8 - ����� ������ �2
    nasmod[7] = StrToInt(EditNastrTo7 -> Text);
    // 9 - ����� ������ ���
    nasmod[29] = StrToInt(EditNastrTo29 -> Text);
    // 10 - ������� ���������� �1
    nasmod[30] = int(StrToFloat(EditNastrTo30->Text)*255.0/40.0);
    // 11 - ������� ���������� �2
    nasmod[31] = int(StrToFloat(EditNastrTo31->Text)*255.0/40.0);
    // 12 - ������� �������� �������� ��������
    nasmod[11] = 8192 + 820 + 205 * ( StrToInt(EditNastrTo11 -> Text) - 1 );
    // 13 - ������� �������� �������� �������� ������
    nasmod[23] = 8192 + 820 + 205 * ( StrToInt(EditNastrTo23 -> Text) - 1 );
    // 14 - ����������� �������� �������� ��������
    nasmod[15] = 8192 + 820 + 205 * ( StrToInt(EditNastrTo15 -> Text) - 1 );
    // 15 - ������� �������� �������� ��������
    nasmod[12] = 8192 + 820 + 205 * ( StrToInt(EditNastrTo12 -> Text) - 1 );
    // 16 - ����������� �������� �������� ��������
    nasmod[16] = 8192 + 820 + 205 * ( StrToInt(EditNastrTo16 -> Text) - 1 );
     // 17 - �������� ���?
    EditNastrTo8 -> Text == "��" ? nasmod[8] = 1 : nasmod[8] = 0;
    // 18 - �������� ����?
    EditNastrTo9 -> Text == "��" ? nasmod[9] = 1 : nasmod[9] = 0;
    // 19 - �������� ���?
    EditNastrTo10 -> Text == "��" ? nasmod[10] = 1 : nasmod[10] = 0;
    // 20 - ������������ ���������� �� ����������
    nasmod[19] = int(StrToFloat(EditNastrTo19->Text)*4095.0/BPM_U_MAX);
    // 23 - ������
    nasmod[3] = EditNastrTo3->ItemIndex;  
    // 22 - ���������� �������� ���
    nasmod[25] = int(StrToFloat(EditNastrTo25 -> Text) * 4095.0 / BPM1_P_MAX);
    //���������� ���� ������������ ���
    nasmod[35] = (unsigned int)(1000.0/(float)StrToFloat(EditNastrTo35->Text));
    // 24 - ������ � ������ ������?
    EditNastrTo36 -> Text == "��" ? nasmod[36] = 1 : nasmod[36] = 0;
    // 25 - ���������� �������� ���
    nasmod[37] = int(StrToFloat(EditNastrTo37 -> Text) * 4095.0 / COMET_MAX_PD);
    // 26 - ���� �������� Ar � ����� ���1
    nasmod[38] = int(((StrToFloat(EditNastrTo38 -> Text) * 8.128/9.8) + 2.032)*409.5);  // ������ // *9,8
    // 27 - ���� �������� N2 � ����� ���2
    nasmod[39] = int(((StrToFloat(EditNastrTo39 -> Text) * 8.128/9.8) + 2.032)*409.5);
    // 28 - ���������� �������������
    nasmod[26] = int(StrToFloat(EditNastrTo26 -> Text) * 10.0);
    // 26 - ������ � �������� � ������ ��
    EditNastrTo27 -> Text == "��" ? nasmod[27] = 1 : nasmod[27] = 0;
    // 30 - ������ � �������� � ����� ��
    EditNastrTo34 -> Text == "��" ? nasmod[34] = 1 : nasmod[34] = 0;
    // 31 - ����������� ��
    nasmod[33] = int(StrToInt(EditNastrTo33 -> Text)*2047/350);



    // ��������� �������� � �������
    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ����������� ���������:");
    if ( EditNastrIn0 -> Text != EditNastrTo0 -> Text )
        MemoStat -> Lines -> Add("���������� ������� ��������������� ������� ������: " + EditNastrIn0 -> Text + " -> " + EditNastrTo0 -> Text );
    if ( EditNastrIn22 -> Text != EditNastrTo22 -> Text )
        MemoStat -> Lines -> Add("������� ������� ������ ����� ��������: " + EditNastrIn22 -> Text + " -> " + EditNastrTo22 -> Text );
    if ( EditNastrIn2 -> Text != EditNastrTo2 -> Text )
        MemoStat -> Lines -> Add("���� �������� �������� �� ��������� ������� �� ������� ��: " + EditNastrIn2 -> Text + " -> " + EditNastrTo2 -> Text );
    if ( EditNastrIn1 -> Text != EditNastrTo1 -> Text )
        MemoStat -> Lines -> Add("���� �������� �������� �� ��������� �� ������� ��: " + EditNastrIn1 -> Text + " -> " + EditNastrTo1 -> Text );
    if ( EditNastrIn4 -> Text != EditNastrTo4 -> Text )
        MemoStat -> Lines -> Add("�������� ���������� �������� �1 ��� ������: " + EditNastrIn4 -> Text + " -> " + EditNastrTo4 -> Text );
    if ( EditNastrIn5 -> Text != EditNastrTo5 -> Text )
        MemoStat -> Lines -> Add("����� ������ �1: " + EditNastrIn5 -> Text + " -> " + EditNastrTo5 -> Text );
    if ( EditNastrIn6 -> Text != EditNastrTo6 -> Text )
        MemoStat -> Lines -> Add("�������� ���������� �������� �2 ��� ������: " + EditNastrIn6 -> Text + " -> " + EditNastrTo6 -> Text );
    if ( EditNastrIn7 -> Text != EditNastrTo7 -> Text )
        MemoStat -> Lines -> Add("����� ������ �2: " + EditNastrIn7 -> Text + " -> " + EditNastrTo7 -> Text );
    if ( EditNastrIn29 -> Text != EditNastrTo29 -> Text )
        MemoStat -> Lines -> Add("����� ������ ���: " + EditNastrIn29 -> Text + " -> " + EditNastrTo29 -> Text );
    if ( EditNastrIn30 -> Text != EditNastrTo30 -> Text )
        MemoStat -> Lines -> Add("������� ���������� �1: " + EditNastrIn30 -> Text + " -> " + EditNastrTo30 -> Text );
    if ( EditNastrIn31 -> Text != EditNastrTo31 -> Text )
        MemoStat -> Lines -> Add("������� ���������� �2: " + EditNastrIn31 -> Text + " -> " + EditNastrTo31 -> Text );
    if ( EditNastrIn11 -> Text != EditNastrTo11 -> Text )
        MemoStat -> Lines -> Add("������� �������� �������� ��� ���������: " + EditNastrIn11 -> Text + " -> " + EditNastrTo11 -> Text );
    if ( EditNastrIn23 -> Text != EditNastrTo23 -> Text )
        MemoStat -> Lines -> Add("�������� �������� �������� ��� �������, �������, �������: " + EditNastrIn23 -> Text + " -> " + EditNastrTo23 -> Text );
    if ( EditNastrIn15 -> Text != EditNastrTo15 -> Text )
        MemoStat -> Lines -> Add("����������� �������� �������� ��������: " + EditNastrIn15 -> Text + " -> " + EditNastrTo15 -> Text );
    if ( EditNastrIn12 -> Text != EditNastrTo12 -> Text )
        MemoStat -> Lines -> Add("������� �������� �������� ��������: " + EditNastrIn12 -> Text + " -> " + EditNastrTo12 -> Text );
    if ( EditNastrIn16 -> Text != EditNastrTo16 -> Text )
        MemoStat -> Lines -> Add("����������� �������� �������� ��������: " + EditNastrIn16 -> Text + " -> " + EditNastrTo16 -> Text );
    if ( EditNastrIn8 -> Text != EditNastrTo8 -> Text )
        MemoStat -> Lines -> Add("�������� ����� ������� �����������: " + EditNastrIn8 -> Text + " -> " + EditNastrTo8 -> Text );
    if ( EditNastrIn9 -> Text != EditNastrTo9 -> Text )
        MemoStat -> Lines -> Add("�������� ����� ������� ��������� �����: " + EditNastrIn9 -> Text + " -> " + EditNastrTo9 -> Text );
    if ( EditNastrIn10 -> Text != EditNastrTo10 -> Text )
        MemoStat -> Lines -> Add("�������� ���� ������� ����������: " + EditNastrIn10 -> Text + " -> " + EditNastrTo10 -> Text );
    if ( EditNastrIn19 -> Text != EditNastrTo19 -> Text )
        MemoStat -> Lines -> Add("������������ ���������� �� �����������: " + EditNastrIn19 -> Text + " -> " + EditNastrTo19 -> Text );
    if ( EditNastrIn3 -> Text != EditNastrTo3 -> Text )
        MemoStat -> Lines -> Add("�������� ��������� �������: " + EditNastrIn3 -> Text + " -> " + EditNastrTo3 -> Text );
    if ( EditNastrIn25 -> Text != EditNastrTo25 -> Text )
        MemoStat -> Lines -> Add("���������� �������� ��� ��� ������� � ��������� �������������: " + EditNastrIn25 -> Text + " -> " + EditNastrTo25 -> Text );
    if ( EditNastrIn35 -> Text != EditNastrTo35 -> Text )
        MemoStat -> Lines -> Add("���������� ����������� ������������ ���: " + EditNastrIn35 -> Text + " -> " + EditNastrTo35 -> Text );
    if ( EditNastrIn36 -> Text != EditNastrTo36 -> Text )
        MemoStat -> Lines -> Add("������ � ������ ������: " + EditNastrIn36 -> Text + " -> " + EditNastrTo36 -> Text );
    if ( EditNastrIn37 -> Text != EditNastrTo37 -> Text )
        MemoStat -> Lines -> Add("���������� �������� ��� ��� ������� � ��������� �������������: " + EditNastrIn37 -> Text + " -> " + EditNastrTo37 -> Text );
    if ( EditNastrIn38 -> Text != EditNastrTo38 -> Text )
        MemoStat -> Lines -> Add("������������ �������� Ar � ����� ���1: " + EditNastrIn38 -> Text + " -> " + EditNastrTo38 -> Text );
    if ( EditNastrIn39 -> Text != EditNastrTo39 -> Text )
        MemoStat -> Lines -> Add("������������ �������� N2 � ����� ���2: " + EditNastrIn39 -> Text + " -> " + EditNastrTo39 -> Text );
    if ( EditNastrIn26 -> Text != EditNastrTo26 -> Text )
        MemoStat -> Lines -> Add("������������� �������� �� ���������� ��������: " + EditNastrIn26 -> Text + " -> " + EditNastrTo26 -> Text );
    if ( EditNastrIn27 -> Text != EditNastrTo27 -> Text )
        MemoStat -> Lines -> Add("������ � �������� � ������ �������� �����: " + EditNastrIn27 -> Text + " -> " + EditNastrTo27 -> Text );
    if ( EditNastrIn34 -> Text != EditNastrTo34 -> Text )
        MemoStat -> Lines -> Add("������ � �������� � ����� �������� �����: " + EditNastrIn34 -> Text + " -> " + EditNastrTo34 -> Text );
    if ( EditNastrIn33 -> Text != EditNastrTo33 -> Text )
        MemoStat -> Lines -> Add("����������� ��: " + EditNastrIn33 -> Text + " -> " + EditNastrTo33 -> Text );

    // ������� ������ - �������� ���� �����, ��� ����������
    EditNastrTo0 -> Color = clWhite;
    EditNastrTo22 -> Color = clWhite;
    EditNastrTo2 -> Color = clWhite;
    EditNastrTo1 -> Color = clWhite;
    EditNastrTo4 -> Color = clWhite;
    EditNastrTo5 -> Color = clWhite;
    EditNastrTo6 -> Color = clWhite;
    EditNastrTo7 -> Color = clWhite;
    EditNastrTo29 -> Color = clWhite;
    EditNastrTo30 -> Color = clWhite;
    EditNastrTo31 -> Color = clWhite;
    EditNastrTo11 -> Color = clWhite;
    EditNastrTo23 -> Color = clWhite;
    EditNastrTo15 -> Color = clWhite;
    EditNastrTo12 -> Color = clWhite;
    EditNastrTo16 -> Color = clWhite;
    EditNastrTo8 -> Color = clWhite;
    EditNastrTo9 -> Color = clWhite;
    EditNastrTo19 -> Color = clWhite;
    EditNastrTo3 -> Color = clWhite;
    EditNastrTo25 -> Color = clWhite;
    EditNastrTo35 -> Color = clWhite;
    EditNastrTo36 -> Color = clWhite;
    EditNastrTo37 -> Color = clWhite;
    EditNastrTo38 -> Color = clWhite;
    EditNastrTo39 -> Color = clWhite;
    EditNastrTo26 -> Color = clWhite;
    EditNastrTo27 -> Color = clWhite;
    EditNastrTo34 -> Color = clWhite;
    EditNastrTo33 -> Color = clWhite;

    // �������� ����������� �������� ������������ �������
    VisualNasmod();
    VisualParA();

    // ��������� �������� ������������ �������
    MemoNasmod -> Lines -> Clear();

    MemoNasmod -> Lines -> Add(EditNastrTo0->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo22->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo2->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo1->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo4->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo5->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo6->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo7->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo29->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo30->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo31->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo11->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo23->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo15->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo12->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo16->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo8->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo9->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo10->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo19->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo3->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo25->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo35->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo36->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo37->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo38->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo39->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo26->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo27->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo34->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo33->Text);
    MemoNasmod -> Lines -> SaveToFile("Nasmod\\Nasmod.txt");
}
//---------------------------------------------------------------------------
// ������������ ������� ����������
void __fastcall TForm1::PCMainChange(TObject *Sender)
{
if ( PCMain -> ActivePage == TSMnemo )
    {
        // ���������� �� �������� �������������� ������
        mnemoInAuto = true;
		PnlMnemo->Visible = false;

        PnlPod->Visible = false;

        PnlMnemo->Parent = TSMnemo;
        PnlMnemo->Left = 4;
        PnlMnemo->Top = 36;

        PnlMnemoParam->Height = 455;


        Panel47->Visible = true;
		Panel48->Visible = true;
        Panel55->Visible = true;
        Panel62->Visible = true;

        State -> Visible = true;


        //���������� �������������
        Panel22 -> Visible = true;
        Panel24 -> Visible = true;
        Panel235 -> Visible = true;
        Panel57 -> Visible = true;

        //������ ���������� ������� � �������� ��
        Panel28 -> Top = 370;
        Panel40 -> Top = 370;
        Panel51 -> Top = 370;
        Panel58 -> Top = 370;

        Panel44 -> Top = 398;
        Panel45 -> Top = 398;
        Panel52 -> Top = 398;
        Panel60 -> Top = 398;

        Pnl_GK -> Visible = false;


		// ������������ ����������
		VisualMnemo();
		
		PnlMnemo->Visible = true;
    }
    else if ( ( PCMain -> ActivePage == TSNalad ) && ( PCNalad -> ActivePage == TSNaladMnemo ) )
    {
        // ���������� �� �������� ������ ������
        mnemoInAuto = false;
		PnlMnemo->Visible = false;

        PnlPod->Visible = true;

        PnlMnemo -> Parent = TSNaladMnemo;
        PnlMnemo->Left = 0;
        PnlMnemo->Top = 0;

        PnlMnemoParam->Height = 455 - 56;


        Panel47->Visible = false;
		Panel48->Visible = false;
        Panel55->Visible = false;
        Panel62->Visible = false;

        //�������� �������������
        Panel22 -> Visible = false;
        Panel24 -> Visible = false;
        Panel235 -> Visible = false;
        Panel57 -> Visible = false;

        //������ ���������� ������� � �������� ��
        Panel28 -> Top = 342;
        Panel40 -> Top = 342;
        Panel51 -> Top = 342;
        Panel58 -> Top = 342;

        Panel44 -> Top = 370;
        Panel45 -> Top = 370;
        Panel52 -> Top = 370;
        Panel60 -> Top = 370;

        Panel21 ->BringToFront();
		// ������������ ����������
		VisualMnemo();

        // ������� ������
        State -> Visible = false;
		
		PnlMnemo->Visible = true;
    }
}
//---------------------------------------------------------------------------
// ������������ ���������� �������
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
    EditOTLzad17-> Text = "";
    EditOTLzad18-> Text = "";
    EditOTLzad19-> Text = "";
    EditOTLzad20-> Text = "";
    EditOTLzad21-> Text = "";
    EditOTLzad22-> Text = "";
    EditOTLzad23-> Text = "";
    EditOTLzad24-> Text = "";
    EditOTLzad25-> Text = "";
    EditOTLzad26-> Text = "";
    EditOTLzad27-> Text = "";
    EditOTLzad28-> Text = "";
    EditOTLzad29-> Text = "";
    EditOTLzad30-> Text = "";
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
		case 1: shr[1] = StrToInt(EditOTLzad1->Text); break;
		case 2: sh[1] = StrToInt(EditOTLzad2->Text); break;
		case 3: shr[2] = StrToInt(EditOTLzad3->Text); break;
		case 4: sh[2] = StrToInt(EditOTLzad4->Text); break;
		case 5: shr[3] = StrToInt(EditOTLzad5->Text); break;
		case 6: sh[3] = StrToInt(EditOTLzad6->Text); break;
		case 7: shr[4] = StrToInt(EditOTLzad7->Text); break;
		case 8: sh[4] = StrToInt(EditOTLzad8->Text); break;
		case 9: shr[5] = StrToInt(EditOTLzad9->Text); break;
		case 10: sh[5] = StrToInt(EditOTLzad10->Text); break;
		case 11: shr[7] = StrToInt(EditOTLzad11->Text); break;
		case 12: sh[7] = StrToInt(EditOTLzad12->Text); break;
		case 13: shr[8] = StrToInt(EditOTLzad13->Text); break;
		case 14: sh[8] = StrToInt(EditOTLzad14->Text); break;
		case 15: shr[9] = StrToInt(EditOTLzad15->Text); break;
		case 16: sh[9] = StrToInt(EditOTLzad16->Text); break;
		case 17: shr[20] = StrToInt(EditOTLzad17->Text); break;
		case 18: sh[20] = StrToInt(EditOTLzad18->Text); break;
		case 19: shr[21] = StrToInt(EditOTLzad19->Text); break;
		case 20: sh[21] = StrToInt(EditOTLzad20->Text); break;
		case 21: shr[23] = StrToInt(EditOTLzad21->Text); break;
		case 22: sh[23] = StrToInt(EditOTLzad22->Text); break;
		case 23: shr[27] = StrToInt(EditOTLzad23->Text); break;
		case 24: sh[27] = StrToInt(EditOTLzad24->Text); break;
		case 25: shr[28] = StrToInt(EditOTLzad25->Text); break;
		case 26: sh[28] = StrToInt(EditOTLzad26->Text); break;
		case 27: shr[29] = StrToInt(EditOTLzad27->Text); break;
		case 28: sh[29] = StrToInt(EditOTLzad28->Text); break;
		case 29: shr[30] = StrToInt(EditOTLzad29->Text); break;
		case 30: sh[30] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//1 ��������
case 1:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: shr[31] = StrToInt(EditOTLzad1->Text); break;
		case 2: sh[31] = StrToInt(EditOTLzad2->Text); break;
		case 3: shr[32] = StrToInt(EditOTLzad3->Text); break;
		case 4: sh[32] = StrToInt(EditOTLzad4->Text); break;
		case 5: shr[33] = StrToInt(EditOTLzad5->Text); break;
		case 6: sh[33] = StrToInt(EditOTLzad6->Text); break;
		case 7: shr[34] = StrToInt(EditOTLzad7->Text); break;
		case 8: sh[34] = StrToInt(EditOTLzad8->Text); break;
		case 9: shr[36] = StrToInt(EditOTLzad9->Text); break;
		case 10: sh[36] = StrToInt(EditOTLzad10->Text); break;
		case 11: shr[37] = StrToInt(EditOTLzad11->Text); break;
		case 12: sh[37] = StrToInt(EditOTLzad12->Text); break;
		case 13: shr[38] = StrToInt(EditOTLzad13->Text); break;
		case 14: sh[38] = StrToInt(EditOTLzad14->Text); break;
		case 15: shr[39] = StrToInt(EditOTLzad15->Text); break;
		case 16: sh[39] = StrToInt(EditOTLzad16->Text); break;
		case 17: shr[40] = StrToInt(EditOTLzad17->Text); break;
		case 18: sh[40] = StrToInt(EditOTLzad18->Text); break;
		case 19: shr[41] = StrToInt(EditOTLzad19->Text); break;
		case 20: sh[41] = StrToInt(EditOTLzad20->Text); break;
		case 21: shr[42] = StrToInt(EditOTLzad21->Text); break;
		case 22: sh[42] = StrToInt(EditOTLzad22->Text); break;
		case 23: shr[43] = StrToInt(EditOTLzad23->Text); break;
		case 24: sh[43] = StrToInt(EditOTLzad24->Text); break;
		case 25: shr[44] = StrToInt(EditOTLzad25->Text); break;
		case 26: sh[44] = StrToInt(EditOTLzad26->Text); break;
		case 27: shr[45] = StrToInt(EditOTLzad27->Text); break;
		case 28: sh[45] = StrToInt(EditOTLzad28->Text); break;
		case 29: shr[48] = StrToInt(EditOTLzad29->Text); break; 
		case 30: sh[48] = StrToInt(EditOTLzad30->Text); break; 
	}
}; break;
//2 ��������
case 2:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: shr[49] = StrToInt(EditOTLzad1->Text); break; 
		case 2: sh[49] = StrToInt(EditOTLzad2->Text); break; 
		case 3: shr[50] = StrToInt(EditOTLzad3->Text); break; 
		case 4: sh[50] = StrToInt(EditOTLzad4->Text); break;
		case 5: shr[52] = StrToInt(EditOTLzad5->Text); break;
		case 6: sh[52] = StrToInt(EditOTLzad6->Text); break;
		case 7: shr[53] = StrToInt(EditOTLzad7->Text); break;
		case 8: sh[53] = StrToInt(EditOTLzad8->Text); break;
		//case 9: diagn[2] = StrToInt(EditOTLzad9->Text); break;
		case 10: otvet = StrToInt(EditOTLzad10->Text); break;
		case 11: norma = StrToInt(EditOTLzad11->Text); break;
		case 12: sh_ = StrToInt(EditOTLzad12->Text); break;
		case 13: qkk = StrToInt(EditOTLzad13->Text); break;
		//case 14: diagn[7] = StrToInt(EditOTLzad14->Text); break;
		case 15: D_D1 = StrToInt(EditOTLzad15->Text); break;
		case 16: D_D2 = StrToInt(EditOTLzad16->Text); break;
		//case 17: diagn[10] = StrToInt(EditOTLzad17->Text); break;
		case 18: UVAK_KN = StrToInt(EditOTLzad18->Text); break;
		case 19: UVAK_KAM_V = StrToInt(EditOTLzad19->Text); break;
		case 20: UVAK_KAM_N = StrToInt(EditOTLzad20->Text); break;
		case 21: UVAK_KAM = StrToInt(EditOTLzad21->Text); break;
		case 22: UVAK_ATM = StrToInt(EditOTLzad22->Text); break;
		case 23: POROG_DAVL = StrToInt(EditOTLzad23->Text); break;
		//case 24: diagn[17] = StrToInt(EditOTLzad24->Text); break;
		case 25: zin[0] = StrToInt(EditOTLzad25->Text); break;
		case 26: zin[1] = StrToInt(EditOTLzad26->Text); break;
		case 27: zin[2] = StrToInt(EditOTLzad27->Text); break;
		case 28: out[0] = StrToInt(EditOTLzad28->Text); break;
		case 29: out[1] = StrToInt(EditOTLzad29->Text); break;
		case 30: out[2] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//3 ��������
case 3:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: aout[0] = StrToInt(EditOTLzad1->Text); break;
		case 2: aout[1] = StrToInt(EditOTLzad2->Text); break;
		case 3: aout[2] = StrToInt(EditOTLzad3->Text); break;
		case 4: aout[3] = StrToInt(EditOTLzad4->Text); break;
		case 5: aout[4] = StrToInt(EditOTLzad5->Text); break;
		case 6: aout[5] = StrToInt(EditOTLzad6->Text); break;
		case 7: aout[6] = StrToInt(EditOTLzad7->Text); break;
		case 8: aout[7] = StrToInt(EditOTLzad8->Text); break;
		case 9: aout[8] = StrToInt(EditOTLzad9->Text); break;
		case 10: aout[9] = StrToInt(EditOTLzad10->Text); break;
		case 11: aout[10] = StrToInt(EditOTLzad11->Text); break;
		case 12: aout[11] = StrToInt(EditOTLzad12->Text); break;
		//case 13: aik[5] = StrToInt(EditOTLzad13->Text); break;
		case 14: aik[0] = StrToInt(EditOTLzad14->Text); break;
		case 15: aik[1] = StrToInt(EditOTLzad15->Text); break;
		case 16: aik[2] = StrToInt(EditOTLzad16->Text); break;
		case 17: aik[3] = StrToInt(EditOTLzad17->Text); break;
		case 18: aik[4] = StrToInt(EditOTLzad18->Text); break;
		case 19: aik[5] = StrToInt(EditOTLzad19->Text); break;
		case 20: aik[6] = StrToInt(EditOTLzad20->Text); break;
		case 21: aik[7] = StrToInt(EditOTLzad21->Text); break;
		case 22: aik[8] = StrToInt(EditOTLzad22->Text); break;
		case 23: aik[9] = StrToInt(EditOTLzad23->Text); break;
		case 24: aik[10] = StrToInt(EditOTLzad24->Text); break;
		case 25: aik[11] = StrToInt(EditOTLzad25->Text); break;
		case 26: aik[12] = StrToInt(EditOTLzad26->Text); break;
		case 27: aik[13] = StrToInt(EditOTLzad27->Text); break;
		case 28: aik[14] = StrToInt(EditOTLzad28->Text); break;
		case 29: aik[15] = StrToInt(EditOTLzad29->Text); break;
		//case 30: nasmod[6] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//4 ��������
case 4:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: diagn[0] = StrToInt(EditOTLzad1->Text); break;
		case 2: diagn[1] = StrToInt(EditOTLzad2->Text); break;
		case 3: diagn[2] = StrToInt(EditOTLzad3->Text); break;
		case 4: diagn[3] = StrToInt(EditOTLzad4->Text); break;
		case 5: diagn[4] = StrToInt(EditOTLzad5->Text); break;
		case 6: diagn[5] = StrToInt(EditOTLzad6->Text); break;
		case 7: diagn[6] = StrToInt(EditOTLzad7->Text); break;
		case 8: diagn[7] = StrToInt(EditOTLzad8->Text); break;
		case 9: diagn[8] = StrToInt(EditOTLzad9->Text); break;
		case 10: diagn[9] = StrToInt(EditOTLzad10->Text); break;
		case 11: diagn[10] = StrToInt(EditOTLzad11->Text); break;
		case 12: diagn[11] = StrToInt(EditOTLzad12->Text); break;
		case 13: diagn[12] = StrToInt(EditOTLzad13->Text); break;
		case 14: diagn[13] = StrToInt(EditOTLzad14->Text); break;
		case 15: diagn[14] = StrToInt(EditOTLzad15->Text); break;
		case 16: diagn[15] = StrToInt(EditOTLzad16->Text); break;
		case 17: diagn[16] = StrToInt(EditOTLzad17->Text); break;
		case 18: diagn[17] = StrToInt(EditOTLzad18->Text); break;
		case 19: diagn[18] = StrToInt(EditOTLzad19->Text); break;
		case 20: diagn[19] = StrToInt(EditOTLzad20->Text); break;
		case 21: diagn[20] = StrToInt(EditOTLzad21->Text); break;
		case 22: diagn[21] = StrToInt(EditOTLzad22->Text); break;
		//case 23: diagn[29] = StrToInt(EditOTLzad23->Text); break;
		case 24: diagnS[0] = StrToInt(EditOTLzad24->Text); break;
		case 25: diagnS[1] = StrToInt(EditOTLzad25->Text); break;
		//case 26: diagn[32] = StrToInt(EditOTLzad26->Text); break;
		case 27: nasmod[0] = StrToInt(EditOTLzad27->Text); break;
		case 28: nasmod[1] = StrToInt(EditOTLzad28->Text); break;
		case 29: nasmod[2] = StrToInt(EditOTLzad29->Text); break;
		case 30: nasmod[3] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//5 ��������
case 5:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: nasmod[4] = StrToInt(EditOTLzad1->Text); break;
		case 2: nasmod[5] = StrToInt(EditOTLzad2->Text); break;
		case 3: nasmod[6] = StrToInt(EditOTLzad3->Text); break;
		case 4: nasmod[7] = StrToInt(EditOTLzad4->Text); break;
		case 5: nasmod[8] = StrToInt(EditOTLzad5->Text); break;
		case 6: nasmod[9] = StrToInt(EditOTLzad6->Text); break;
		case 7: nasmod[10] = StrToInt(EditOTLzad7->Text); break;
		case 8: nasmod[11] = StrToInt(EditOTLzad8->Text); break;
		case 9: nasmod[12] = StrToInt(EditOTLzad9->Text); break;
		case 10: nasmod[15] = StrToInt(EditOTLzad10->Text); break;
		case 11: nasmod[16] = StrToInt(EditOTLzad11->Text); break;
		case 12: nasmod[19] = StrToInt(EditOTLzad12->Text); break;
		case 13: nasmod[22] = StrToInt(EditOTLzad13->Text); break;
		case 14: nasmod[23] = StrToInt(EditOTLzad14->Text); break;
		case 15: nasmod[25] = StrToInt(EditOTLzad15->Text); break;
		case 16: nasmod[26] = StrToInt(EditOTLzad16->Text); break;
		case 17: nasmod[27] = StrToInt(EditOTLzad17->Text); break;
		case 18: nasmod[29] = StrToInt(EditOTLzad18->Text); break;
		case 19: nasmod[30] = StrToInt(EditOTLzad19->Text); break;
		case 20: nasmod[31] = StrToInt(EditOTLzad20->Text); break;
		case 21: nasmod[33] = StrToInt(EditOTLzad21->Text); break;
		case 22: nasmod[34] = StrToInt(EditOTLzad22->Text); break;
		case 23: nasmod[35] = StrToInt(EditOTLzad23->Text); break;
		case 24: nasmod[36] = StrToInt(EditOTLzad24->Text); break;
		case 25: nasmod[37] = StrToInt(EditOTLzad25->Text); break;
		case 26: nasmod[38] = StrToInt(EditOTLzad26->Text); break;
		case 27: nasmod[39] = StrToInt(EditOTLzad27->Text); break;
		//case 28: par[5][18] = StrToInt(EditOTLzad28->Text); break;
		//case 29: par[5][2] = StrToInt(EditOTLzad29->Text); break;
		//case 30: par[5][17] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//6 ��������
case 6:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: par[0][0] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[0][1] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[0][2] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[0][3] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[0][6] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[0][8] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[0][9] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[0][10] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[0][11] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[0][12] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[0][13] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[0][14] = StrToInt(EditOTLzad12->Text); break;
		//case 13: par[7][1] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[1][5] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[1][12] = StrToInt(EditOTLzad15->Text); break;
		//case 16: par[7][18] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[2][0] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[2][3] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[2][5] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[2][6] = StrToInt(EditOTLzad20->Text); break;
		//case 21: par[8][12] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[3][5] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[3][12] = StrToInt(EditOTLzad23->Text); break;
		//case 24: par[8][9] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[4][1] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[4][2] = StrToInt(EditOTLzad26->Text); break;
		case 27: par[4][5] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[4][6] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[4][8] = StrToInt(EditOTLzad29->Text); break;
		case 30: par[4][9] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//7 ��������
case 7:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: par[4][10] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[4][12] = StrToInt(EditOTLzad2->Text); break;
		case 3: par[4][15] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[4][17] = StrToInt(EditOTLzad4->Text); break;
		//case 5: par[9][2] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[5][1] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[5][2] = StrToInt(EditOTLzad7->Text); break;
		case 8: par[5][5] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[5][6] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[5][8] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[5][9] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[5][10] = StrToInt(EditOTLzad12->Text); break;
		case 13: par[5][12] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[5][15] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[5][17] = StrToInt(EditOTLzad15->Text); break;
		//case 16: par[10][5] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[6][1] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[6][2] = StrToInt(EditOTLzad18->Text); break;
		case 19: par[6][5] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[6][6] = StrToInt(EditOTLzad20->Text); break;
		case 21: par[6][8] = StrToInt(EditOTLzad21->Text); break;
		case 22: par[6][9] = StrToInt(EditOTLzad22->Text); break;
		case 23: par[6][10] = StrToInt(EditOTLzad23->Text); break;
		case 24: par[6][12] = StrToInt(EditOTLzad24->Text); break;
		case 25: par[6][15] = StrToInt(EditOTLzad25->Text); break;
		case 26: par[6][17] = StrToInt(EditOTLzad26->Text); break;
		//case 27: par[12][12] = StrToInt(EditOTLzad27->Text); break;
		case 28: par[7][5] = StrToInt(EditOTLzad28->Text); break;
		case 29: par[7][12] = StrToInt(EditOTLzad29->Text); break;
		//case 30: par[12][9] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//8 ��������
case 8:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: par[8][5] = StrToInt(EditOTLzad1->Text); break;
		case 2: par[8][12] = StrToInt(EditOTLzad2->Text); break;
		//case 3: par[12][17] = StrToInt(EditOTLzad3->Text); break;
		case 4: par[9][2] = StrToInt(EditOTLzad4->Text); break;
		case 5: par[9][5] = StrToInt(EditOTLzad5->Text); break;
		case 6: par[9][6] = StrToInt(EditOTLzad6->Text); break;
		case 7: par[9][8] = StrToInt(EditOTLzad7->Text); break;
		//case 8: par[14][12] = StrToInt(EditOTLzad8->Text); break;
		case 9: par[10][2] = StrToInt(EditOTLzad9->Text); break;
		case 10: par[10][5] = StrToInt(EditOTLzad10->Text); break;
		case 11: par[10][6] = StrToInt(EditOTLzad11->Text); break;
		case 12: par[10][9] = StrToInt(EditOTLzad12->Text); break;
		//case 13: par[15][6] = StrToInt(EditOTLzad13->Text); break;
		case 14: par[11][2] = StrToInt(EditOTLzad14->Text); break;
		case 15: par[11][5] = StrToInt(EditOTLzad15->Text); break;
		case 16: par[11][6] = StrToInt(EditOTLzad16->Text); break;
		case 17: par[11][10] = StrToInt(EditOTLzad17->Text); break;
		case 18: par[11][15] = StrToInt(EditOTLzad18->Text); break;
		/*case 19: par[17][2] = StrToInt(EditOTLzad19->Text); break;
		case 20: par[17][5] = StrToInt(EditOTLzad20->Text); break; 
		case 21: par[17][6] = StrToInt(EditOTLzad21->Text); break; 
		case 22: D_D1 = StrToInt(EditOTLzad22->Text); break; 
		case 23: D_D2 = StrToInt(EditOTLzad23->Text); break;
		case 24: UVAK_KAM_V = StrToInt(EditOTLzad24->Text); break; 
		case 25: UVAK_KAM_N = StrToInt(EditOTLzad25->Text); break; 
		case 26: UVAK_KAM = StrToInt(EditOTLzad26->Text); break; 
		case 27: UVAK_ATM = StrToInt(EditOTLzad27->Text); break; 
		case 28: POROG_DAVL = StrToInt(EditOTLzad28->Text); break; 
		case 29: T_VODA = StrToInt(EditOTLzad29->Text); break; 
		case 30: T_KTMN_OTK = StrToInt(EditOTLzad30->Text); break;  */
	}
}; break;
//9 ��������
case 9:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: CT_T1 = StrToInt(EditOTLzad1->Text); break;
		case 2: CT_T20 = StrToInt(EditOTLzad2->Text); break;
		case 3: CT_1 = StrToInt(EditOTLzad3->Text); break;
		case 4: CT_2 = StrToInt(EditOTLzad4->Text); break;
		case 5: CT_3 = StrToInt(EditOTLzad5->Text); break;
		case 6: CT_7 = StrToInt(EditOTLzad6->Text); break;
		case 7: CT_8 = StrToInt(EditOTLzad7->Text); break;
		case 8: CT_10 = StrToInt(EditOTLzad8->Text); break;
		case 9: CT_23 = StrToInt(EditOTLzad9->Text); break;
		case 10: CT23K1 = StrToInt(EditOTLzad10->Text); break;
		case 11: CT_27 = StrToInt(EditOTLzad11->Text); break;
		case 12: CT27K1 = StrToInt(EditOTLzad12->Text); break;
		case 13: CT_28 = StrToInt(EditOTLzad13->Text); break;
		case 14: CT_29 = StrToInt(EditOTLzad14->Text); break;
		case 15: CT29K1 = StrToInt(EditOTLzad15->Text); break;
		case 16: CT_30 = StrToInt(EditOTLzad16->Text); break;
		case 17: CT30K1 = StrToInt(EditOTLzad17->Text); break;
		case 18: CT_32 = StrToInt(EditOTLzad18->Text); break;
		case 19: CT32K1 = StrToInt(EditOTLzad19->Text); break;
		case 20: CT_34 = StrToInt(EditOTLzad20->Text); break;
		case 21: CT34K1 = StrToInt(EditOTLzad21->Text); break;
		case 22: CT_39 = StrToInt(EditOTLzad22->Text); break;
		case 23: ct48 = StrToInt(EditOTLzad23->Text); break;
		case 24: ct49 = StrToInt(EditOTLzad24->Text); break;
		case 25: ctVrashPderj = StrToInt(EditOTLzad25->Text); break;
		case 26: ctPderjSpd = StrToInt(EditOTLzad26->Text); break;
		//case 27: CT_NAP = StrToInt(EditOTLzad27->Text); break;
		//case 28: CT_KSOPR = StrToInt(EditOTLzad28->Text); break;
		//case 29: CT_VRASH_BAR = StrToInt(EditOTLzad29->Text); break;
		//case 30: CT_VKL_KN = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//10 ��������
case 10:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: T_VODA = StrToInt(EditOTLzad1->Text); break;
		case 2: T_KTMN_OTK = StrToInt(EditOTLzad2->Text); break;
		case 3: T_KKAM_V = StrToInt(EditOTLzad3->Text); break;
		case 4: T_KKAM = StrToInt(EditOTLzad4->Text); break;
		case 5: T_PROC = StrToInt(EditOTLzad5->Text); break;
		case 6: T_KUSTBM = StrToInt(EditOTLzad6->Text); break;
		case 7: T_KZASL = StrToInt(EditOTLzad7->Text); break;
		case 8: T_VKL_BPN = StrToInt(EditOTLzad8->Text); break;
		case 9: T_KL = StrToInt(EditOTLzad9->Text); break;
		case 10: TK_TMN1 = StrToInt(EditOTLzad10->Text); break;
		case 11: TK_TMN2 = StrToInt(EditOTLzad11->Text); break;
		case 12: TK_TMN3 = StrToInt(EditOTLzad12->Text); break;
		case 13: T_KKR = StrToInt(EditOTLzad13->Text); break;
		case 14: T_KSOPR = StrToInt(EditOTLzad14->Text); break;
		case 15: T_KVRASH = StrToInt(EditOTLzad15->Text); break;
		case 16: T_KOST = StrToInt(EditOTLzad16->Text); break;
		case 17: T_KKN_OTK = StrToInt(EditOTLzad17->Text); break;
		case 18: T_ZASL = StrToInt(EditOTLzad18->Text); break;
		case 19: T_KRAZGON = StrToInt(EditOTLzad19->Text); break;
		case 20: T_VHG = StrToInt(EditOTLzad20->Text); break;
		case 21: tk48 = StrToInt(EditOTLzad21->Text); break;
		case 22: tk49 = StrToInt(EditOTLzad22->Text); break;
		case 23: tkVrashPderj = StrToInt(EditOTLzad23->Text); break;
		case 24: zaslUgolAbs = StrToInt(EditOTLzad24->Text); break;
		case 25: pderjPutTek = StrToInt(EditOTLzad25->Text); break;
		//case 26: N_ST = StrToInt(EditOTLzad26->Text); break;
		//case 27: N_ST_MAX = StrToInt(EditOTLzad27->Text); break;
		//case 28: SOPR = StrToInt(EditOTLzad28->Text); break;
		//case 29: PR_VTMN = StrToInt(EditOTLzad29->Text); break;
		//case 30: PR_SOPR_BM = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//11 ��������
case 11:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: CT_VODA_BM = StrToInt(EditOTLzad1->Text); break;
		case 2: CT_VODA_II = StrToInt(EditOTLzad2->Text); break;
		case 3: CT_VODA_KN = StrToInt(EditOTLzad3->Text); break;
		case 4: CT_TEMP = StrToInt(EditOTLzad4->Text); break;
		case 5: CT_BM = StrToInt(EditOTLzad5->Text); break;
		case 6: CT_II = StrToInt(EditOTLzad6->Text); break;
		case 7: CT_IST = StrToInt(EditOTLzad7->Text); break;
		case 8: CT_TMN = StrToInt(EditOTLzad8->Text); break;
		case 9: CT_KR = StrToInt(EditOTLzad9->Text); break;
		case 10: CT_KR_T = StrToInt(EditOTLzad10->Text); break;
		case 11: CT_BAR = StrToInt(EditOTLzad11->Text); break;
		case 12: CT_NAP = StrToInt(EditOTLzad12->Text); break;
		case 13: CT_KSOPR = StrToInt(EditOTLzad13->Text); break;
		case 14: CT_VRASH_BAR = StrToInt(EditOTLzad14->Text); break;
		case 15: CT_VKL_KN = StrToInt(EditOTLzad15->Text); break;
		case 16: CT_KN = StrToInt(EditOTLzad16->Text); break;
		case 17: CT_VHG = StrToInt(EditOTLzad17->Text); break;
		case 18: PR_RAZKN = StrToInt(EditOTLzad18->Text); break;
		case 19: PR_RC = StrToInt(EditOTLzad19->Text); break;
		case 20: PR_DAVL_PODJ = StrToInt(EditOTLzad20->Text); break;
		case 21: N_ST = StrToInt(EditOTLzad21->Text); break;
		case 22: N_ST_MAX = StrToInt(EditOTLzad22->Text); break;
		case 23: SOPR = StrToInt(EditOTLzad23->Text); break;
		case 24: PR_VTMN = StrToInt(EditOTLzad24->Text); break;
		case 25: PR_SOPR_BM = StrToInt(EditOTLzad25->Text); break;
		case 26: Z_SOPR = StrToInt(EditOTLzad26->Text); break;
		case 27: ZU_SOPR = StrToInt(EditOTLzad27->Text); break;
		case 28: K_OSHIB = StrToInt(EditOTLzad28->Text); break;
		case 29: LogicT = StrToFloat(EditOTLzad29->Text); break;
		case 30: PR_Ar = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//12 ��������
case 12:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: VRGIS = StrToInt(EditOTLzad1->Text); break;
		case 2: K_SOGL_GIS = StrToInt(EditOTLzad2->Text); break;
		case 3: NAPRS_GIS = StrToInt(EditOTLzad3->Text); break;
		case 4: X_TGIS = StrToInt(EditOTLzad4->Text); break;
		case 5: E_TGIS = StrToInt(EditOTLzad5->Text); break;
		case 6: DELGIS = StrToInt(EditOTLzad6->Text); break;
		case 7: DOPGIS = StrToInt(EditOTLzad7->Text); break;
		case 8: PAR_GIS = StrToInt(EditOTLzad8->Text); break;
		case 9: N_TEK_GIS = StrToInt(EditOTLzad9->Text); break;
		case 10: LIM1GIS = StrToInt(EditOTLzad10->Text); break;
		case 11: LIM2GIS = StrToInt(EditOTLzad11->Text); break;
		case 12: T_VRGIS = StrToInt(EditOTLzad12->Text); break;
		case 13: T_KGIS = StrToInt(EditOTLzad13->Text); break;
		//case 14: PR_TEMP = StrToInt(EditOTLzad14->Text); break;
		//case 15: KOM_TEMP = StrToInt(EditOTLzad15->Text); break;
		case 16: KOM_DZ = StrToInt(EditOTLzad16->Text); break;
		case 17: OTVET_DZ = StrToInt(EditOTLzad17->Text); break;
		case 18: TYPE_DZ = StrToInt(EditOTLzad18->Text); break;
		case 19: PR_DZ = StrToInt(EditOTLzad19->Text); break;
		case 20: HOME_DZ = StrToInt(EditOTLzad20->Text); break;
		case 21: PUT_DZ = StrToInt(EditOTLzad21->Text); break;
		case 22: V_DZ = StrToInt(EditOTLzad22->Text); break;
		case 23: TEK_ABS_DZ = StrToInt(EditOTLzad23->Text); break;
		case 24: TEK_OTN_DZ = StrToInt(EditOTLzad24->Text); break;
		case 25: CT_DZ = StrToInt(EditOTLzad25->Text); break;
		//case 26: T_KTEMP = StrToInt(EditOTLzad26->Text); break;
		//case 27: DOPTEMP = StrToInt(EditOTLzad27->Text); break;
		//case 28: TEK_TEMP = StrToInt(EditOTLzad28->Text); break;
		//case 29: TEK_TEMP1 = StrToInt(EditOTLzad29->Text); break;
		//case 30: TEK_TEMP2 = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
//13 ��������
case 13:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: VRUN = StrToInt(EditOTLzad1->Text); break;
		case 2: X_TUN = StrToInt(EditOTLzad2->Text); break;
		case 3: E_TUN = StrToInt(EditOTLzad3->Text); break;
		case 4: DELUN = StrToInt(EditOTLzad4->Text); break;
		case 5: E_PUN = StrToInt(EditOTLzad5->Text); break;
		case 6: K_PUN = StrToInt(EditOTLzad6->Text); break;
		case 7: K_IUN = StrToInt(EditOTLzad7->Text); break;
		case 8: U_PUN = StrToInt(EditOTLzad8->Text); break;
		case 9: T_REQUN = StrToInt(EditOTLzad9->Text); break;
		case 10: LIMPUN = StrToInt(EditOTLzad10->Text); break;
		case 11: LIMIUN = StrToInt(EditOTLzad11->Text); break;
		case 12: LIM1UN = StrToInt(EditOTLzad12->Text); break;
		case 13: LIM2UN = StrToInt(EditOTLzad13->Text); break;
		case 14: LIMUUN = StrToInt(EditOTLzad14->Text); break;
		case 15: LIMU_UN = StrToInt(EditOTLzad15->Text); break;
		case 16: LIMUPR_UN = StrToInt(EditOTLzad16->Text); break;
		case 17: PORCNV_UN = StrToInt(EditOTLzad17->Text); break;
		case 18: PORCPR_UN = StrToInt(EditOTLzad18->Text); break;
		case 19: PROBUN = StrToInt(EditOTLzad19->Text); break;
		//case 20: KOM_BM[4] = StrToInt(EditOTLzad20->Text); break;
		//case 21: OTVET_BM[0] = StrToInt(EditOTLzad21->Text); break;
		case 22: M1_N = StrToInt(EditOTLzad22->Text); break;
		case 23: M1_V = StrToInt(EditOTLzad23->Text); break;
		case 24: M2_N = StrToInt(EditOTLzad24->Text); break;
		case 25: M2_V = StrToInt(EditOTLzad25->Text); break;
		case 26: VCHM_N = StrToInt(EditOTLzad26->Text); break;
		case 27: VCHM_V = StrToInt(EditOTLzad27->Text); break;
		//case 28: OTVET_BM[7] = StrToInt(EditOTLzad28->Text); break;
		//case 29: OTVET_BM[8] = StrToInt(EditOTLzad29->Text); break;
		//case 30: OTVET_BM[9] = StrToInt(EditOTLzad30->Text); break;
	}
}; break;

//14 ��������
case 14:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: PR_TEMP = StrToInt(EditOTLzad1->Text); break;
		case 2: KOM_TEMP = StrToInt(EditOTLzad2->Text); break;
		case 3: ZAD_TEMP = StrToInt(EditOTLzad3->Text); break;
		case 4: PAR_TEMP = StrToInt(EditOTLzad4->Text); break;
		case 5: ZPAR_TEMP = StrToInt(EditOTLzad5->Text); break;
		case 6: X_TEMP = StrToInt(EditOTLzad6->Text); break;
		case 7: VRTEMP = StrToInt(EditOTLzad7->Text); break;
		case 8: E_TEMP = StrToInt(EditOTLzad8->Text); break;
		case 9: DELTEMP = StrToInt(EditOTLzad9->Text); break;
		case 10: LIM1TEMP = StrToInt(EditOTLzad10->Text); break;
		case 11: LIM2TEMP = StrToInt(EditOTLzad11->Text); break;
		case 12: T_VRTEMP = StrToInt(EditOTLzad12->Text); break;
		case 13: T_KTEMP = StrToInt(EditOTLzad13->Text); break;
		case 14: DOPTEMP = StrToInt(EditOTLzad14->Text); break;
		case 15: TEK_TEMP = StrToInt(EditOTLzad15->Text); break;
		case 16: TEK_TEMP1 = StrToInt(EditOTLzad16->Text); break;
		case 17: TEK_TEMP2 = StrToInt(EditOTLzad17->Text); break;
		case 18: TEK_TEMP3 = StrToInt(EditOTLzad18->Text); break;
		case 19: TEK_TEMP4 = StrToInt(EditOTLzad19->Text); break;
		case 20: TEK_TEMP_PIR = StrToInt(EditOTLzad20->Text); break;
		/*case 21: OTVET_II[4] = StrToInt(EditOTLzad21->Text); break;
		case 22: OTVET_II[5] = StrToInt(EditOTLzad22->Text); break;
		case 23: OTVET_II[6] = StrToInt(EditOTLzad23->Text); break;
		case 24: OTVET_II[7] = StrToInt(EditOTLzad24->Text); break;
		case 25: OTVET_II[8] = StrToInt(EditOTLzad25->Text); break;
		case 26: OTVET_II[9] = StrToInt(EditOTLzad26->Text); break;
		case 27: zaslUgolAbs = StrToInt(EditOTLzad27->Text); break;
		case 28: shr[52] = StrToInt(EditOTLzad28->Text); break;
		case 29: sh[52] = StrToInt(EditOTLzad29->Text); break;
		case 30: pderjPutTek = StrToInt(EditOTLzad30->Text); break;   */
	}
}; break;

//15 ��������
case 15:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: VRBM = StrToInt(EditOTLzad1->Text); break;
		case 2: UST_BM = StrToInt(EditOTLzad2->Text); break;
		case 3: X_TBM = StrToInt(EditOTLzad3->Text); break;
		case 4: E_TBM = StrToInt(EditOTLzad4->Text); break;
		case 5: DELBM = StrToInt(EditOTLzad5->Text); break;
		case 6: DOPBM = StrToInt(EditOTLzad6->Text); break;
		case 7: PAR_BM1 = StrToInt(EditOTLzad7->Text); break;
		case 8: PAR_BM2 = StrToInt(EditOTLzad8->Text); break;
		case 9: LIM1BM = StrToInt(EditOTLzad9->Text); break;
		case 10: LIM2BM = StrToInt(EditOTLzad10->Text); break;
		case 11: PR_SV_BM = StrToInt(EditOTLzad11->Text); break;
		case 12: KOM_BM[0] = StrToInt(EditOTLzad12->Text); break;
		case 13: KOM_BM[1] = StrToInt(EditOTLzad13->Text); break;
		case 14: KOM_BM[2] = StrToInt(EditOTLzad14->Text); break;
		case 15: KOM_BM[3] = StrToInt(EditOTLzad15->Text); break;
		case 16: KOM_BM[4] = StrToInt(EditOTLzad16->Text); break;
		case 17: OTVET_BM[0] = StrToInt(EditOTLzad17->Text); break;
		case 18: OTVET_BM[1] = StrToInt(EditOTLzad18->Text); break;
		case 19: OTVET_BM[2] = StrToInt(EditOTLzad19->Text); break;
		case 20: OTVET_BM[3] = StrToInt(EditOTLzad20->Text); break;
		case 21: OTVET_BM[4] = StrToInt(EditOTLzad21->Text); break;
		case 22: OTVET_BM[5] = StrToInt(EditOTLzad22->Text); break;
		case 23: OTVET_BM[6] = StrToInt(EditOTLzad23->Text); break;
		case 24: OTVET_BM[7] = StrToInt(EditOTLzad24->Text); break;
		case 25: OTVET_BM[8] = StrToInt(EditOTLzad25->Text); break;
		case 26: OTVET_BM[9] = StrToInt(EditOTLzad26->Text); break;
		//case 27: zaslUgolAbs = StrToInt(EditOTLzad27->Text); break;
		//case 28: shr[52] = StrToInt(EditOTLzad28->Text); break;
		//case 29: sh[52] = StrToInt(EditOTLzad29->Text); break;
		//case 30: pderjPutTek = StrToInt(EditOTLzad30->Text); break;
	}
}; break;


//16 ��������
case 16:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1: VRII = StrToInt(EditOTLzad1->Text); break;
		case 2: X_TII = StrToInt(EditOTLzad2->Text); break;
		case 3: E_TII = StrToInt(EditOTLzad3->Text); break;
		case 4: DELII = StrToInt(EditOTLzad4->Text); break;
		case 5: DOPII = StrToInt(EditOTLzad5->Text); break;
		case 6: PAR_II = StrToInt(EditOTLzad6->Text); break;
		case 7: LIM1II = StrToInt(EditOTLzad7->Text); break;
		case 8: LIM2II = StrToInt(EditOTLzad8->Text); break;
		case 9: T_VRII = StrToInt(EditOTLzad9->Text); break;
		case 10: T_KII = StrToInt(EditOTLzad10->Text); break;
		case 11: PR_SV_II = StrToInt(EditOTLzad11->Text); break;
		case 12: KOM_II[0] = StrToInt(EditOTLzad12->Text); break;
		case 13: KOM_II[1] = StrToInt(EditOTLzad13->Text); break;
		case 14: KOM_II[2] = StrToInt(EditOTLzad14->Text); break;
		case 15: KOM_II[3] = StrToInt(EditOTLzad15->Text); break;
		case 16: KOM_II[4] = StrToInt(EditOTLzad16->Text); break;
		case 17: OTVET_II[0] = StrToInt(EditOTLzad17->Text); break;
		case 18: OTVET_II[1] = StrToInt(EditOTLzad18->Text); break;
		case 19: OTVET_II[2] = StrToInt(EditOTLzad19->Text); break;
		case 20: OTVET_II[3] = StrToInt(EditOTLzad20->Text); break;
		case 21: OTVET_II[4] = StrToInt(EditOTLzad21->Text); break;
		case 22: OTVET_II[5] = StrToInt(EditOTLzad22->Text); break;
		case 23: OTVET_II[6] = StrToInt(EditOTLzad23->Text); break;
		case 24: OTVET_II[7] = StrToInt(EditOTLzad24->Text); break;
		case 25: OTVET_II[8] = StrToInt(EditOTLzad25->Text); break;
		case 26: OTVET_II[9] = StrToInt(EditOTLzad26->Text); break;
		//case 27: zaslUgolAbs = StrToInt(EditOTLzad27->Text); break;
		//case 28: shr[52] = StrToInt(EditOTLzad28->Text); break;
		//case 29: sh[52] = StrToInt(EditOTLzad29->Text); break;
		//case 30: pderjPutTek = StrToInt(EditOTLzad30->Text); break;
	}
}; break;

    }
}
//---------------------------------------------------------------------------
// ��������� ������ ������� �������
void __fastcall TForm1::PnlPVSClick(TObject *Sender)
{
    AnsiString Rejim_name;
    int posX = 800 , posY = 750;

    // �������� ������� �������
    ListBoxCondition -> Items -> Clear();

    // ������ ������� �������
    switch (StrToInt(((TPanel*)Sender)->Hint))
    {
        case 1:
        {
            LblRejim -> Caption = "������� ������";
            // ���� ����� � �1
            if ( diagnS[0] & 0x01 ) ListBoxCondition -> Items -> Add(DiagnSNames[0]);
            // ���� ����� � �2
            if ( diagnS[0] & 0x02 ) ListBoxCondition -> Items -> Add(DiagnSNames[1]);
            // ���� ����� � ���������
            if ( diagnS[0] & 0x20 ) ListBoxCondition -> Items -> Add(DiagnSNames[5]);
            // ���� ����� � ���
            if ( nasmod[8] && (diagnS[0] & 0x04) ) ListBoxCondition -> Items -> Add(DiagnSNames[2]);
            // ���� ����� � ���
            if ( nasmod[10] && (diagnS[0] & 0x08) ) ListBoxCondition -> Items -> Add(DiagnSNames[3]);
            // ���� ����� � ����
            if ( nasmod[10] && (diagnS[0] & 0x10) ) ListBoxCondition -> Items -> Add(DiagnSNames[4]);
            // ���� �������� � ����������
            if ( ! ( zin[0] & 0x40 ) ) ListBoxCondition -> Items -> Add("��� �������� � ����������");
            // ���� ���������� ��
            if ( ! ( zin[0] & 0x400 ) ) ListBoxCondition -> Items -> Add("��� ���������� ����������� ��");
            // ������ �����
            if ( ! ( zin[1] & 0x10 ) ) ListBoxCondition -> Items -> Add("������ �� �����");
            // ���� ���� ���� �� �����
            if ( diagn[9] & 0x01 ) ListBoxCondition -> Items -> Add("��� �������� ���� �� ����� ���������");

            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if(shr[i]) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;
        case 2:
        {
            LblRejim -> Caption = "������� ����";
             // ���� ����� � �1
            if ( diagnS[0] & 0x01 ) ListBoxCondition -> Items -> Add(DiagnSNames[0]);
            // ���� ����� � �2
            if ( diagnS[0] & 0x02 ) ListBoxCondition -> Items -> Add(DiagnSNames[1]);
            // ���� ����� � ���������
            if ( diagnS[0] & 0x20 ) ListBoxCondition -> Items -> Add(DiagnSNames[5]);
            // ���� ����� � ���
            if ( nasmod[8] && (diagnS[0] & 0x04) ) ListBoxCondition -> Items -> Add(DiagnSNames[2]);
            // ���� ����� � ���
            if ( nasmod[10] && (diagnS[0] & 0x08) ) ListBoxCondition -> Items -> Add(DiagnSNames[3]);
            // ���� ����� � ����
            if ( nasmod[10] && (diagnS[0] & 0x10) ) ListBoxCondition -> Items -> Add(DiagnSNames[4]);
            // ���� �������� � ����������
            if ( ! ( zin[0] & 0x40 ) ) ListBoxCondition -> Items -> Add("��� �������� � ����������");
            // ���� ���������� ��
            if ( ! ( zin[0] & 0x400 ) ) ListBoxCondition -> Items -> Add("��� ���������� ����������� ��");
            // ������ �����
            if ( ! ( zin[1] & 0x10 ) ) ListBoxCondition -> Items -> Add("������ �� �����");
            // ���� ���� ���� �� �����
            if ( diagn[9] & 0x01 ) ListBoxCondition -> Items -> Add("��� �������� ���� �� ����� ���������");
            // ���� ���� ���� � ���. �����������
            if ( diagn[20] & 0x30 ) ListBoxCondition -> Items -> Add("�������� ���� Ar �� � �����");
            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if(shr[i]) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);

            // ���� ���������� �����������
            if ( ! ( zin[0] & 0x01 ) ) ListBoxCondition -> Items -> Add("��� ���������� �����������");
            // ���� ���������� ������� ��
            if ( ! ( zin[0] & 0x02 ) ) ListBoxCondition -> Items -> Add("��� ���������� ������� ��");
            // ���� ���������� ������� � ������� ������ ������
            if ( ! ( zin[0] & 0x04 ) ) ListBoxCondition -> Items -> Add("��� ���������� ������� � ����. ������ ������");
            // ���� ���������� ������ ������ ������
            if ( ! ( zin[0] & 0x08 ) ) ListBoxCondition -> Items -> Add("��� ���������� ������ ������ ������");


        }; break;
        case 3:
        {
            LblRejim -> Caption = "����������";
            // ���� ����� � �1
            if ( diagnS[0] & 0x01 ) ListBoxCondition -> Items -> Add(DiagnSNames[0]);
            // ���� ����� � �2
            if ( diagnS[0] & 0x02 ) ListBoxCondition -> Items -> Add(DiagnSNames[1]);
            // ���� ����� � ���������
            if ( diagnS[0] & 0x20 ) ListBoxCondition -> Items -> Add(DiagnSNames[5]);
            // ���� ����� � ���
            if ( nasmod[10] && (diagnS[0] & 0x08) ) ListBoxCondition -> Items -> Add(DiagnSNames[3]);
            // ���� �������� � ����������
            if ( ! ( zin[0] & 0x40 ) ) ListBoxCondition -> Items -> Add("��� �������� � ����������");
            // ���� ���������� ��
            if ( ! ( zin[0] & 0x400 ) ) ListBoxCondition -> Items -> Add("��� ���������� ����������� ��");
            // ������ �����
            if ( ! ( zin[1] & 0x10 ) ) ListBoxCondition -> Items -> Add("������ �� �����");
            // ���� ���� ���� �� �����
            if ( diagn[9] & 0x01 ) ListBoxCondition -> Items -> Add("��� �������� ���� �� ��. ���������");
            // ���� ���� ���� � ���. �����������
            if (  diagn[20] & 0x30 ) ListBoxCondition -> Items -> Add("�������� ���� Ar �� � �����");
            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if(shr[i]) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);

                    // ���� ���������� �����������
            if ( ! ( zin[0] & 0x01 ) ) ListBoxCondition -> Items -> Add("��� ���������� �����������");
            // ���� ���������� ������� ��
            if ( ! ( zin[0] & 0x02 ) ) ListBoxCondition -> Items -> Add("��� ���������� ������� ��");
            // ���� ���������� ������� � ������� ������ ������
            if ( ! ( zin[0] & 0x04 ) ) ListBoxCondition -> Items -> Add("��� ���������� ������� � ����. ������ ������");
            // ���� ���������� ������ ������ ������
            if ( ! ( zin[0] & 0x08 ) ) ListBoxCondition -> Items -> Add("��� ���������� ������ ������ ������");   

        }; break;
        case 4:
        {
            LblRejim -> Caption = "����� ��";
            // ������� ����� 2 ��� 3
            if (!shr[2] && !shr[3]) ListBoxCondition -> Items -> Add("�� ������� ����� �� ��� ����������");
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
        }; break;
        case 5:
        {
            LblRejim -> Caption = "���������� ���������";
            // �� ������� �� ���� �����, ����� �������
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if((i!=1)&&shr[i]) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;
        case 6:
        {
            LblRejim -> Caption = "����� �����";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 27
            if ( shr[27] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[27]);
            // �� ������� ����� 29
            if ( shr[29] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]);
            // �� ������� ����� 30
            if ( shr[30] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[30]);
            // �� ������� ����� 32
            if ( shr[32] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[32]);
        }; break;
        case 20:
        {
            LblRejim -> Caption = "���1 � ���.";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 20
            if ( shr[20] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[20]);
              // �� ������� ����� 23
            if ( shr[23] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[23]);

            // �������  �������� � ���. �����������
            if (  diagn[20] & 0x20 ) ListBoxCondition -> Items -> Add("������� �������� Ar");
            //  ������ �������� � ���. �����������
            if (  diagn[20] & 0x10 ) ListBoxCondition -> Items -> Add("������ �������� Ar");

        }; break;
         case 220:
        {
            LblRejim -> Caption = "���1 � ��.";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 20
            if ( shr[20] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[20]);
              // �� ������� ����� 23
            if ( shr[23] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[23]);

        }; break;
        case 120:
        {
            LblRejim -> Caption = "����� ���1";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� 20
            if ( ! shr[20] ) ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[20]);
        }; break;
        case 21:
        {
            LblRejim -> Caption = "���2";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 21
            if ( shr[21] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[21]);
            //  ������ �������� � ���. �����������
            if (  diagn[20] & 0x40 ) ListBoxCondition -> Items -> Add("������ �������� N2");
            // �������   �������� � ���. �����������
            if (  diagn[20] & 0x80 ) ListBoxCondition -> Items -> Add("������� �������� N2");
        }; break;
        case 121:
        {
            LblRejim -> Caption = "����� ���2";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� 21
            if ( ! shr[21] ) ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[21]);
        }; break;
        case 23:
        {
            LblRejim -> Caption = "���";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 20
            if ( shr[20] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[20]);
            // �� ������� ����� 23
            if ( shr[23] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[23]);

            // �������  �������� � ���. �����������
            if (  diagn[20] & 0x20 ) ListBoxCondition -> Items -> Add("������� �������� Ar");
            //  ������ �������� � ���. �����������
            if (  diagn[20] & 0x10 ) ListBoxCondition -> Items -> Add("������ �������� Ar");

        }; break;
        case 123:
        {
            LblRejim -> Caption = "����� ���";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� 23
            if ( ! shr[23] ) ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[23]);
        }; break;
        case 27:
        {
            LblRejim -> Caption = "������";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 27
            if ( shr[27] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[27]);
            // �� ������� ����� 32
            if ( shr[32] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[32]);
            // ���� ���������� ���� ������
            if (!(zin[0] & 0x04)) ListBoxCondition -> Items -> Add("��� ���������� ������� ������ ������");
            // ���� ���������� ������ ������
            if (!(zin[0] & 0x08)) ListBoxCondition -> Items -> Add("��� ���������� ������ ������ ������");
        }; break;


        case 29:
        {
            LblRejim -> Caption = "M1 ���.";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 29
            if ( shr[29] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]);
            // �� ������� ����� 30
            if ( shr[30] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[30]);
            // �� ������� ����� 31
            if ( shr[31] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[31]);
            // �� ������� ����� 32
            if ( shr[32] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[32]);
            // �� ������� ����� 34
            if ( shr[34] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[34]);
            // ���� ����� � ���
            if ( diagnS[0] & 0x08 ) ListBoxCondition -> Items -> Add(DiagnSNames[3]);
            // ���� ���������� �����������
            if (!(zin[0] & 0x01)) ListBoxCondition -> Items -> Add("��� ���������� �����������");

        }; break;
        case 30:
        {
            LblRejim -> Caption = "M2 ���.";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 29
            if ( shr[29] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]);
            // �� ������� ����� 30
            if ( shr[30] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[30]);
            // �� ������� ����� 31
            if ( shr[31] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[31]);
            // �� ������� ����� 32
            if ( shr[32] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[32]);
            // �� ������� ����� 34
            if ( shr[34] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[34]);
            // ���� ����� � ���
            if ( diagnS[0] & 0x08 ) ListBoxCondition -> Items -> Add(DiagnSNames[3]);
            // ���� ���������� �����������
            if (!(zin[0] & 0x01)) ListBoxCondition -> Items -> Add("��� ���������� �����������");
        }; break;
        case 31:
        {
            LblRejim -> Caption = "����� ���";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� ���1 ��� ���2
            if ((!shr[29]) && !(shr[30])) ListBoxCondition -> Items -> Add("�� ������� ����� �1 ��� �2");
        }; break;
        case 32:
        {
            LblRejim -> Caption = "��";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 27
            if ( shr[27] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[27]);
            // �� ������� ����� 29
            if ( shr[29] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]);
            // �� ������� ����� 30
            if ( shr[30] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[30]);
            // �� ������� ����� 32
            if ( shr[32] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[32]);
            // �� ������� ����� 33
            if ( shr[33] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[33]);
            // �� ������� ����� 34
            if ( shr[34] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[34]);
            // ���� ����� � ����
            if ( diagnS[0] & 0x10 ) ListBoxCondition -> Items -> Add(DiagnSNames[4]);
            // ���� ���������� ������� ��
            if (!(zin[0] & 0x02)) ListBoxCondition -> Items -> Add("��� ���������� ������� ��");
        }; break;
        case 33:
        {
            LblRejim -> Caption = "����� ��";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� ��
            if ( !shr[32] ) ListBoxCondition -> Items -> Add("�� ������� ����� ��");
        }; break;
         case 34:
        {
            LblRejim -> Caption = "���";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 29
            if ( shr[29] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[29]);
            // �� ������� ����� 30
            if ( shr[30] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[30]);
            // �� ������� ����� 32
            if ( shr[32] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[32]);
            // �� ������� ����� 34
            if ( shr[34] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[34]);
            // ���� ���������� �����������
            if (!(zin[0] & 0x01)) ListBoxCondition -> Items -> Add("��� ���������� �����������");
        }; break;
         case 134:
        {
            LblRejim -> Caption = "����� ���";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� 34
            if ( ! shr[34] ) ListBoxCondition -> Items -> Add("�� ������� �����: " + SHRNames[34]);

        }; break;

        case 37:
        {
            LblRejim -> Caption = "������� ��";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
        }; break;
        case 38:
        {
            LblRejim -> Caption = "������� �� (HOME)";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
        }; break;
        case 39:
        {
            LblRejim -> Caption = "�� � ���������������";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
        }; break;

        case 36:
        {
            LblRejim -> Caption = "����. ������ (����)";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // ������� ����� ������
            if ( !shr[27] ) ListBoxCondition -> Items -> Add("�� ������� ����� ������");
        }; break;
        case 40:
        {
            LblRejim -> Caption = "������� � ���.";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 40
            if ( shr[40] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[40]);
            // �� ������� ����� 41
            if ( shr[41] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[41]);
            // �� ������� ����� 49
            if ( shr[49] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[49]);
            // �� ������� ����� 52
            if ( shr[52] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[52]);
        }; break;
        case 140:
        {
            LblRejim -> Caption = "���� ����������";
            /*
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� �������� ������ ����������
            if (!shr[40] && !shr[41] && !shr[42] && !shr[43] && !shr[44] && !shr[45]) ListBoxCondition -> Items -> Add("�� �������� ������ ����������");  */
        }; break;
        case 41:
        {
            LblRejim -> Caption = "������� ������";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 40
            if ( shr[40] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[40]);
            // �� ������� ����� 41
            if ( shr[41] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[41]);
            // �� ������� ����� 49
            if ( shr[49] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[49]);
            // �� ������� ����� 52
            if ( shr[52] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[52]);
        }; break;
        case 42:
        {
            LblRejim -> Caption = "�������� � ���.";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 42
            if ( shr[42] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[42]);
            // �� ������� ����� 43
            if ( shr[43] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[43]);
            // �� ������� ����� 48
            if ( shr[48] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[48]);
            // �� ������� ����� 50
            if ( shr[50] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[50]);
        }; break;
        case 43:
        {
            LblRejim -> Caption = "�������� ������";
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 42
            if ( shr[42] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[42]);
            // �� ������� ����� 43
            if ( shr[43] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[43]);
            // �� ������� ����� 48
            if ( shr[48] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[48]);
            // �� ������� ����� 50
            if ( shr[50] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[50]);
        }; break;
        case 44:
        {
            LblRejim -> Caption = "������ �����";
            // ��� ������� �������� ������
            if (!(zin[1] & 0x40)) ListBoxCondition -> Items -> Add("��� ������� �������� ������");
            // �� ������� ����� 2
            if ((shr[2])&&(shr[2]!=64)&&(shr[2]!=5)) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( (shr[4]) && (shr[4]!=14) ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 5
            if ( shr[5] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 44
            if ( shr[44] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[44]);
            // �� ������� ����� 45
            if ( shr[45] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[45]);
           
        }; break;
        case 144:
        {
            LblRejim -> Caption = "������ ����� ��� ��������";
            // �� ������� ����� 2
            if ((shr[2])&&(shr[2]!=64)&&(shr[2]!=5)) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( (shr[4]) && (shr[4]!=14) ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 5
            if ( shr[5] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 44
            if ( shr[44] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[44]);
            // �� ������� ����� 45
            if ( shr[45] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[45]);
        }; break;
        case 45:
        {
            LblRejim -> Caption = "������ ����";
            // ��� ������� �������� ������
            if (!(zin[1] & 0x40)) ListBoxCondition -> Items -> Add("��� ������� �������� ������");
            // �� ������� ����� 2
            if ((shr[2])&&(shr[2]!=6)&&(shr[2]!=5)) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 5
            if ( shr[5] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[5]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
            // �� ������� ����� 44
            if ( shr[44] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[44]);
            // �� ������� ����� 45
            if ( shr[45] ) ListBoxCondition -> Items -> Add("��� ������� �����: " + SHRNames[45]);

        }; break;
        case 48:
        {
            LblRejim -> Caption = "��. ���� ��������";
            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if ( shr[i] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;
        case 49:
        {
            LblRejim -> Caption = "��. ���� ��������";
            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if ( shr[i] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;

        case 50:
        {
            LblRejim -> Caption = "������� ��������";
            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if ( shr[i] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;

        case 52:
        {
            LblRejim -> Caption = "������� ��������";
            // �� ������� �� ���� �����
            for ( unsigned char i = 1 ; i < ( SHR_NAMES_COUNT + 1 ) ; i++ )
                if ( shr[i] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[i]);
        }; break;

        case 53:
        {
            LblRejim -> Caption = "�� ������/�����";
            // ���� ����� � ���������
            if ( diagnS[0] & 0x20 ) ListBoxCondition -> Items -> Add(DiagnSNames[5]);
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
        }; break;

        case 101:
        {
            LblRejim -> Caption = "���� ������� ��";
            // �� ������� ����� 1
            if ( shr[1] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[1]);
            // �� ������� ����� 2
            if ( shr[2] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[2]);
            // �� ������� ����� 3
            if ( shr[3] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[3]);
            // �� ������� ����� 4
            if ( shr[4] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[4]);
            // �� ������� ����� 9
            if ( shr[9] ) ListBoxCondition -> Items -> Add("������� �����: " + SHRNames[9]);
        }; break;

        case 213:
        {
            LblRejim -> Caption = "����� ������";

        }; break;

        
        default:return;     // ����������� �������
    }

    if(StrToInt(((TPanel*)Sender)->Hint)!= 140) // ����� ��� �������������
    {
        if(MessageDlgPos("��������� �����: " + LblRejim -> Caption + "?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,0,posX,posY) != mrYes ) return;
        // ���� �� ��������� ������� �������
        if(ListBoxCondition -> Items -> Count)
        {
            //LblRejim -> Caption = Rejim_name;
            PnlCondition -> Visible = true;
        }
        // ���� ��� ������� ��������� �������� �������
        else
	    { // �������� ��� �������
	        qkk = StrToInt(((TPanel*)Sender)->Hint);
	        // ��������� �������� � ������
            MemoStat -> Lines -> Add( Label_Time -> Caption + " | ������� ����� : <" + LblRejim -> Caption + ">" );
        }
    }
    else
    {
        // ���� �� ��������� ������� �������
	    if(ListBoxCondition -> Items -> Count)
	    {
            //LblRejim -> Caption = LblRejim -> Caption;
            PnlCondition -> Visible = true;
        }
	    // ���� ��� ������� ��������� �������� �������
	    else
	    {
            // �������� ��� �������
	        qkk = StrToInt(((TPanel*)Sender)->Hint);
	        // ��������� �������� � ������
            MemoStat -> Lines -> Add( Label_Time -> Caption + " | ������� ����� : <" + LblRejim -> Caption + ">" );
        }
    }
}
//---------------------------------------------------------------------------
// �������� ����
void __fastcall TForm1::BtnConditionClick(TObject *Sender)
{
    PnlCondition -> Visible = false;
}
//---------------------------------------------------------------------------
// ��������� ������� �������
// ������ � ������������
//---------------------------------------------------------------------------
// ���� ������� ������������ ���������
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
            EdtALib1_12  -> Text = MemoLib -> Lines -> operator [](0);
            EdtALib1_5  -> Text = MemoLib -> Lines -> operator [](1);
            EdtALib2_0  -> Text = MemoLib -> Lines -> operator [](2);
            EdtALib2_3  -> Text = MemoLib -> Lines -> operator [](3);
            EdtALib2_5  -> Text = MemoLib -> Lines -> operator [](4);
            EdtALib2_6  -> Text = MemoLib -> Lines -> operator [](5);
            EdtALib3_12  -> Text = MemoLib -> Lines -> operator [](6);
            EdtALib3_5  -> Text = MemoLib -> Lines -> operator [](7);

            EdtALib4_12  -> Text = MemoLib -> Lines -> operator [](8);
            EdtALib4_1  -> Text = MemoLib -> Lines -> operator [](9);
            EdtALib4_8  -> Text = MemoLib -> Lines -> operator [](10);
            EdtALib4_9  -> Text = MemoLib -> Lines -> operator [](11);
            EdtALib4_10  -> Text = MemoLib -> Lines -> operator [](12);
            EdtALib4_15  -> Text = MemoLib -> Lines -> operator [](13);
            EdtALib4_2  -> Text = MemoLib -> Lines -> operator [](14);
            EdtALib4_17  -> Text = MemoLib -> Lines -> operator [](15);
            EdtALib4_5  -> Text = MemoLib -> Lines -> operator [](16);
            EdtALib4_6  -> Text = MemoLib -> Lines -> operator [](17);

            EdtALib5_12  -> Text = MemoLib -> Lines -> operator [](18);
            EdtALib5_1  -> Text = MemoLib -> Lines -> operator [](19);
            EdtALib5_8  -> Text = MemoLib -> Lines -> operator [](20);
            EdtALib5_9  -> Text = MemoLib -> Lines -> operator [](21);
            EdtALib5_10  -> Text = MemoLib -> Lines -> operator [](22);
            EdtALib5_15  -> Text = MemoLib -> Lines -> operator [](23);
            EdtALib5_2  -> Text = MemoLib -> Lines -> operator [](24);
            EdtALib5_17  -> Text = MemoLib -> Lines -> operator [](25);
            EdtALib5_5  -> Text = MemoLib -> Lines -> operator [](26);
            EdtALib5_6  -> Text = MemoLib -> Lines -> operator [](27);

            EdtALib6_12  -> Text = MemoLib -> Lines -> operator [](28);
            EdtALib6_1  -> Text = MemoLib -> Lines -> operator [](29);
            EdtALib6_8  -> Text = MemoLib -> Lines -> operator [](30);
            EdtALib6_9  -> Text = MemoLib -> Lines -> operator [](31);
            EdtALib6_10  -> Text = MemoLib -> Lines -> operator [](32);
            EdtALib6_15  -> Text = MemoLib -> Lines -> operator [](33);
            EdtALib6_2  -> Text = MemoLib -> Lines -> operator [](34);
            EdtALib6_17  -> Text = MemoLib -> Lines -> operator [](35);
            EdtALib6_5  -> Text = MemoLib -> Lines -> operator [](36);
            EdtALib6_6  -> Text = MemoLib -> Lines -> operator [](37);

            EdtALib7_12  -> Text = MemoLib -> Lines -> operator [](38);
            EdtALib7_5  -> Text = MemoLib -> Lines -> operator [](39);

            EdtALib8_12  -> Text = MemoLib -> Lines -> operator [](40);
            EdtALib8_5  -> Text = MemoLib -> Lines -> operator [](41);

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
// ������� �������������� ���������
void __fastcall TForm1::ListBoxLibraryMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    libNmb = -1;
}
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
            EdtALib1_12  -> Text = "";
            EdtALib1_5  -> Text = "";
            EdtALib2_0  -> Text = "";
            EdtALib2_3  -> Text = "";
            EdtALib2_5  -> Text = "";
            EdtALib2_6  -> Text = "";
            EdtALib3_12  -> Text = "";
            EdtALib3_5  -> Text = "";

            EdtALib4_12  -> Text = "";
            EdtALib4_1  -> Text = "";
            EdtALib4_8  -> Text = "";
            EdtALib4_9  -> Text = "";
            EdtALib4_10  -> Text = "";
            EdtALib4_15  -> Text = "";
            EdtALib4_2  -> Text = "";
            EdtALib4_17  -> Text = "";
            EdtALib4_5  -> Text = "";
            EdtALib4_6  -> Text = "";

            EdtALib5_12  -> Text = "";
            EdtALib5_1  -> Text = "";
            EdtALib5_8  -> Text = "";
            EdtALib5_9  -> Text = "";
            EdtALib5_10  -> Text = "";
            EdtALib5_15  -> Text = "";
            EdtALib5_2  -> Text = "";
            EdtALib5_17  -> Text = "";
            EdtALib5_5  -> Text = "";
            EdtALib5_6  -> Text = "";

            EdtALib6_12  -> Text = "";
            EdtALib6_1  -> Text = "";
            EdtALib6_8  -> Text = "";
            EdtALib6_9  -> Text = "";
            EdtALib6_10  -> Text = "";
            EdtALib6_15  -> Text = "";
            EdtALib6_2  -> Text = "";
            EdtALib6_17  -> Text = "";
            EdtALib6_5  -> Text = "";
            EdtALib6_6  -> Text = "";

            EdtALib7_12  -> Text = "";
            EdtALib7_5  -> Text = "";

            EdtALib8_12  -> Text = "";
            EdtALib8_5  -> Text = "";
    }
}
//---------------------------------------------------------------------------
// ������ ���������� � ����������
void __fastcall TForm1::BitBtnSaveClick(TObject *Sender)
{
    // ���������� ��������� ������
    int Bnum = StrToInt(((TButton*)Sender)->Hint);

    if(Bnum == 0) // ������
    {
        EdtNewName -> Text = "";
        GBSaveDialog -> Visible = false;
    }
    else if(Bnum == 2) // �����������
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
		while ( !rezult )
		{
			fileCount++;
			SR.Name.SetLength( SR.Name.Length() - 4 );
			Form1 -> ListBoxLibrary -> Items -> Add( SR.Name );
			rezult = FindNext(SR);
		};
		Form1 -> ListBoxLibrary -> Sorted;
		EdtNewName -> Text = "";
    }
    else
    {
		// ���������� ���������� � ������������� ������
		MemoLib -> Lines -> Clear();
		MemoLib -> Lines -> Add ( EdtARed1_12 -> Text );
		MemoLib -> Lines -> Add ( EdtARed1_5 -> Text );

		MemoLib -> Lines -> Add ( EdtARed2_0 -> Text );
		MemoLib -> Lines -> Add ( EdtARed2_3 -> Text );
		MemoLib -> Lines -> Add ( EdtARed2_5 -> Text );
		MemoLib -> Lines -> Add ( EdtARed2_6 -> Text );

		MemoLib -> Lines -> Add ( EdtARed3_12 -> Text );
		MemoLib -> Lines -> Add ( EdtARed3_5 -> Text );

		MemoLib -> Lines -> Add ( EdtARed4_12 -> Text );
		MemoLib -> Lines -> Add ( EdtARed4_1 -> Text );
		MemoLib -> Lines -> Add ( EdtARed4_8 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_9 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_10 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_15 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_2 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_17 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_5 -> Text );
        MemoLib -> Lines -> Add ( EdtARed4_6 -> Text );

        MemoLib -> Lines -> Add ( EdtARed5_12 -> Text );
		MemoLib -> Lines -> Add ( EdtARed5_1 -> Text );
		MemoLib -> Lines -> Add ( EdtARed5_8 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_9 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_10 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_15 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_2 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_17 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_5 -> Text );
        MemoLib -> Lines -> Add ( EdtARed5_6 -> Text );

        MemoLib -> Lines -> Add ( EdtARed6_12 -> Text );
		MemoLib -> Lines -> Add ( EdtARed6_1 -> Text );
		MemoLib -> Lines -> Add ( EdtARed6_8 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_9 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_10 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_15 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_2 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_17 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_5 -> Text );
        MemoLib -> Lines -> Add ( EdtARed6_6 -> Text );

        MemoLib -> Lines -> Add ( EdtARed7_12 -> Text );
        MemoLib -> Lines -> Add ( EdtARed7_5 -> Text );

        MemoLib -> Lines -> Add ( EdtARed8_12 -> Text );
        MemoLib -> Lines -> Add ( EdtARed8_5 -> Text );

		// ����������� ����������� ����
		GBSaveDialog -> Visible = true;
    }
}
//---------------------------------------------------------------------------
// ������ ���������� �� ����������
void __fastcall TForm1::BitBtnLoadClick(TObject *Sender)
{
    // �������� ��������
    EdtARed1_12 -> Text = EdtALib1_12 -> Text;
    EdtARed1_5 -> Text = EdtALib1_5 -> Text;

    EdtARed2_0 -> Text = EdtALib2_0 -> Text;
    EdtARed2_3 -> Text = EdtALib2_3 -> Text;
    EdtARed2_5 -> Text = EdtALib2_5 -> Text;
    EdtARed2_6 -> Text = EdtALib2_6 -> Text;

    EdtARed3_12 -> Text = EdtALib3_12 -> Text;
    EdtARed3_5 -> Text = EdtALib3_5 -> Text;

    EdtARed4_12 -> Text = EdtALib4_12 -> Text;
    EdtARed4_1 -> Text = EdtALib4_1 -> Text;
    EdtARed4_8 -> Text = EdtALib4_8 -> Text;
    EdtARed4_9 -> Text = EdtALib4_9 -> Text;
    EdtARed4_10 -> Text = EdtALib4_10 -> Text;
    EdtARed4_15 -> Text = EdtALib4_15 -> Text;
    EdtARed4_2 -> Text = EdtALib4_2 -> Text;
    EdtARed4_17 -> Text = EdtALib4_17 -> Text;
    EdtARed4_5 -> Text = EdtALib4_5 -> Text;
    EdtARed4_6 -> Text = EdtALib4_6 -> Text;

    EdtARed5_12 -> Text = EdtALib5_12 -> Text;
    EdtARed5_1 -> Text = EdtALib5_1 -> Text;
    EdtARed5_8 -> Text = EdtALib5_8 -> Text;
    EdtARed5_9 -> Text = EdtALib5_9 -> Text;
    EdtARed5_10 -> Text = EdtALib5_10 -> Text;
    EdtARed5_15 -> Text = EdtALib5_15 -> Text;
    EdtARed5_2 -> Text = EdtALib5_2 -> Text;
    EdtARed5_17 -> Text = EdtALib5_17 -> Text;
    EdtARed5_5 -> Text = EdtALib5_5 -> Text;
    EdtARed5_6 -> Text = EdtALib5_6 -> Text;

    EdtARed6_12 -> Text = EdtALib6_12 -> Text;
    EdtARed6_1 -> Text = EdtALib6_1 -> Text;
    EdtARed6_8 -> Text = EdtALib6_8 -> Text;
    EdtARed6_9 -> Text = EdtALib6_9 -> Text;
    EdtARed6_10 -> Text = EdtALib6_10 -> Text;
    EdtARed6_15 -> Text = EdtALib6_15 -> Text;
    EdtARed6_2 -> Text = EdtALib6_2 -> Text;
    EdtARed6_17 -> Text = EdtALib6_17 -> Text;
    EdtARed6_5 -> Text = EdtALib6_5 -> Text;
    EdtARed6_6 -> Text = EdtALib6_6 -> Text;

    EdtARed7_12 -> Text = EdtALib7_12 -> Text;
    EdtARed7_5 -> Text = EdtALib7_5 -> Text;

    EdtARed8_12 -> Text = EdtALib8_12 -> Text;
    EdtARed8_5 -> Text = EdtALib8_5 -> Text;

   // �����������
   EdtARed1_12  -> Color = clSilver;
   EdtARed1_5  -> Color = clSilver;
   EdtARed2_0  -> Color = clSilver;
   EdtARed2_3  -> Color = clSilver;
   EdtARed2_5  -> Color = clSilver;
   EdtARed2_6  -> Color = clSilver;
   EdtARed3_12  -> Color = clSilver;
   EdtARed3_5  -> Color = clSilver;

   EdtARed4_12  -> Color = clSilver;
   EdtARed4_1  -> Color = clSilver;
   EdtARed4_8  -> Color = clSilver;
   EdtARed4_9  -> Color = clSilver;
   EdtARed4_10  -> Color = clSilver;
   EdtARed4_15  -> Color = clSilver;
   EdtARed4_2  -> Color = clSilver;
   EdtARed4_17  -> Color = clSilver;
   EdtARed4_5  -> Color = clSilver;
   EdtARed4_6  -> Color = clSilver;

   EdtARed5_12  -> Color = clSilver;
   EdtARed5_1  -> Color = clSilver;
   EdtARed5_8  -> Color = clSilver;
   EdtARed5_9  -> Color = clSilver;
   EdtARed5_10  -> Color = clSilver;
   EdtARed5_15  -> Color = clSilver;
   EdtARed5_2  -> Color = clSilver;
   EdtARed5_17  -> Color = clSilver;
   EdtARed5_5  -> Color = clSilver;
   EdtARed5_6  -> Color = clSilver;

   EdtARed6_12  -> Color = clSilver;
   EdtARed6_1  -> Color = clSilver;
   EdtARed6_8  -> Color = clSilver;
   EdtARed6_9  -> Color = clSilver;
   EdtARed6_10  -> Color = clSilver;
   EdtARed6_15  -> Color = clSilver;
   EdtARed6_2  -> Color = clSilver;
   EdtARed6_17  -> Color = clSilver;
   EdtARed6_5  -> Color = clSilver;
   EdtARed6_6  -> Color = clSilver;

   EdtARed7_12  -> Color = clSilver;
   EdtARed7_5  -> Color = clSilver;

   EdtARed8_12  -> Color = clSilver;
   EdtARed8_5  -> Color = clSilver;
}
//---------------------------------------------------------------------------
//--��������� ���-�� ��������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ChBoxGraphTemp1Click(TObject *Sender)
{
    // ����������� �������������� ������� ������� ��������
    if ( StrToInt(((TCheckBox*)Sender)->Hint) < SERIES_COUNT )
        serTemp[StrToInt(((TCheckBox*)Sender)->Hint)] -> Active = ((TCheckBox*)Sender) -> Checked;
    // �������� �������
    else
        serArh[StrToInt(((TCheckBox*)Sender)->Hint) - 20] -> Active = ((TCheckBox*)Sender) -> Checked;
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
}
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
//---------------------------------------------------------------------------
//--��������� �������� �2--//
void __fastcall TForm1::SBD2DebugChange(TObject *Sender)
{   // �������� ��� ��������
    EdtD2Code -> Text = IntToStr(SBD2Debug->Position);
    // ����������� �������� ��������
    EdtD2Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD2Debug->Position)/1000.0-5.5)),ffExponent,3,8);
}
//---------------------------------------------------------------------------
//--��������� �������� �2--//
void __fastcall TForm1::EdtD2CodeChange(TObject *Sender)
{   // �������� ��� ��������
    SBD2Debug -> Position = StrToInt(EdtD2Code->Text);
    // ����������� �������� ��������
    EdtD2Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD2Debug->Position)/1000.0-5.5)),ffExponent,3,8);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditNastrTo0Exit(TObject *Sender)
{
    // ��������� ���������� ������� � ���������
    if(((TEdit*)Sender)->Name == "EditNastrTo0")
    {
        EditNastrTo0->Text = FloatToStrF(StrToFloat(EditNastrTo0->Text),ffExponent,3,8);
    }
	else if(((TEdit*)Sender)->Name == "EditNastrTo1")
    {
        EditNastrTo1->Text = FloatToStrF(StrToFloat(EditNastrTo1->Text),ffExponent,3,8);
    }

    // �������� ������
    ((TEdit*)Sender)->Color = clSilver;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNalDaClick(TObject *Sender)
{
    if(((TButton*)Sender)->Hint == "2")  // ����� ���� �������������
    {
        PanelParR -> Visible = true;
        return;
    }

    if(((TButton*)Sender)->Hint == "0")  // ������
    {
        PanelParR -> Visible = false;
        return;
    }

    PanelParR -> Visible = false;

    // ������ ���1
    par[0][0] = int(StrToFloat( EdtRZad0->Text ) / RRG1_MAX * 4095.0);
    // ������ ���2
    par[0][1] = int(StrToFloat( EdtRZad1->Text ) / RRG2_MAX * 4095.0);
    // �����������
    par[0][12] = int(StrToFloat(EdtRZad12->Text) * 10.0);
    // ��� ��
    par[0][3] = int(StrToFloat( EdtRZad7->Text ) * 4095.0 / I_MAX);
    // �������� �1
    par[0][8] = int(StrToFloat( EdtRZad8->Text ) * 4095.0 / BPM1_P_MAX + 0.5);
    // �������� �2
    par[0][9] = int(StrToFloat( EdtRZad9->Text ) * 4095.0 / BPM2_P_MAX + 0.5);
    // ��� �������� ���
    par[0][10] = int(StrToFloat( EdtRZad10->Text ) * 4095.0 / COMET_MAX_PD + 0.5);
    // ��������
    par[0][2] = int((0.6*log10(StrToFloat(EdtRZad2->Text)/100.0)+6.8)*1000.0);
    // ���� �������� ��������
    par[0][11] = int(StrToFloat( EdtRZad11->Text ) / UGOL_DZ_MAX * zaslAngle360);
    // ������� �������� ��
    par[0][6] = int(StrToFloat( EdtRZad6->Text ));
    // ���� ��
    par[0][13] = StrToInt( EdtRZad13->Text );
    // �������� ��������
    if(EdtRZad14 -> ItemIndex == 0)       { par[0][14]=0; }
    else if(EdtRZad14 -> ItemIndex == 1)  { par[0][14]=1; }
    else if(EdtRZad14 -> ItemIndex == 2)  { par[0][14]=2; }


    MemoStat -> Lines -> Add(Label_Time -> Caption + "�������� ���������� ���������:");
    if ( EdtRTek0 -> Text != EdtRZad0 -> Text )
        MemoStat -> Lines -> Add("������ ���1:" + EdtRTek0 -> Text + " -> " + EdtRZad0 -> Text );
    if ( EdtRTek1 -> Text != EdtRZad1 -> Text )
        MemoStat -> Lines -> Add("������ ���2:" + EdtRTek1 -> Text + " -> " + EdtRZad1 -> Text );
    if ( EdtRTek12 -> Text != EdtRZad12 -> Text )
        MemoStat -> Lines -> Add("�����������:" + EdtRTek12 -> Text + " -> " + EdtRZad12 -> Text );
    if ( EdtRTek7 -> Text != EdtRZad7 -> Text )
        MemoStat -> Lines -> Add("��� ��:" + EdtRTek7 -> Text + " -> " + EdtRZad7 -> Text );
    if ( EdtRTek8 -> Text != EdtRZad8 -> Text )
        MemoStat -> Lines -> Add("�������� �1:" + EdtRTek8 -> Text + " -> " + EdtRZad8 -> Text );
    if ( EdtRTek9 -> Text != EdtRZad9 -> Text )
        MemoStat -> Lines -> Add("�������� �2:" + EdtRTek9 -> Text + " -> " + EdtRZad9 -> Text );
    if ( EdtRTek10 -> Text != EdtRZad10 -> Text )
        MemoStat -> Lines -> Add("�������� �3:" + EdtRTek10 -> Text + " -> " + EdtRZad10 -> Text );
    if ( EdtRTek2 -> Text != EdtRZad2 -> Text )
        MemoStat -> Lines -> Add("��������:" + EdtRTek2 -> Text + " -> " + EdtRZad2 -> Text );
    if ( EdtRTek11 -> Text != EdtRZad11 -> Text )
        MemoStat -> Lines -> Add("���� �������� ��������:" + EdtRTek11 -> Text + " -> " + EdtRZad11 -> Text );
    if ( EdtRTek6 -> Text != EdtRZad6 -> Text )
        MemoStat -> Lines -> Add("������� �������� ��:" + EdtRTek6 -> Text + " -> " + EdtRZad6 -> Text );

    // ����������� ���������� ���������
    EdtRZad0 -> Color = clWhite;
    EdtRZad1 -> Color = clWhite;
    EdtRZad12 -> Color = clWhite;
    EdtRZad7 -> Color = clWhite;
    EdtRZad8 -> Color = clWhite;
    EdtRZad9 -> Color = clWhite;
    EdtRZad10 -> Color = clWhite;
    EdtRZad2 -> Color = clWhite;
    EdtRZad11 -> Color = clWhite;
    EdtRZad6 -> Color = clWhite;

    // �������� ��������
    VisualParR();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::kl_d1Click(TObject *Sender)
{
    // ��������� ��������/�������� ��������� � ���������� ��������

    // ������������ ������
    if  (
            ( (((TImage*)Sender)->Name) == "fvn" ) ||
            ( (((TImage*)Sender)->Name) == "Kn" )
        )
    {
        BtnDevOn -> Caption = "���";
        BtnDevOff -> Caption = "����";
    }
    else
    {
        BtnDevOn -> Caption = "����";
        BtnDevOff -> Caption = "����";
    }
    // ���� ��������� �� �������� ������� ����������
    if ( ( PCMain -> ActivePage != TSNalad ) || ( PCNalad -> ActivePage != TSNaladMnemo ) ) return;
    // ���������� ������ ����������
    LblDeviceName -> Caption = ((TImage*)Sender) -> Hint;
    PnlDevice -> Hint = ((TImage*)Sender) -> Name;

    PnlDevice -> Top = ((TImage*)Sender)->Top - 41;

    if (
        ( (((TImage*)Sender)->Name) == "fvn" )
    )    PnlDevice -> Left = ((TImage*)Sender)->Left - 110;
    else
        PnlDevice -> Left = ((TImage*)Sender)->Left - 90;  

    PnlDevice -> Visible = true;
    PnlDevice -> BringToFront();

}
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDevOnClick(TObject *Sender)
{
    // ������ ���������� ������ ���������� ���������� ����������
    ((TButton*)Sender) -> Parent -> Visible = false;
    // ���� ������ ������� �����, �� �����
    if (((TButton*)Sender) -> Name == "BtnDevExit" ) return;
    if ( ((TButton*)Sender) -> Parent -> Hint == "kl_1" )         SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x20);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_2" )    SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x40);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_3" )    SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x80);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_4" )    SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x2000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_5" )    SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x4000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_nap" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x8000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x10);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "kl_kn" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x20);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "fvn" )     SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kn" )     SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x01);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::APanel_DaButClick(TObject *Sender)
{
    otvet = StrToInt(((TButton*)Sender)->Hint);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnTrDaClick(TObject *Sender)
{
    PanelParTr -> Visible = false;

    // ���������� �1
    // �������� �1
    par[9][8] = int(StrToFloat(EdtTRed9_8 -> Text) * 4095.0 / BPM1_P_MAX);
    // ��������
    par[9][2] = int((0.6*log10(StrToFloat(EdtTRed9_2->Text)/100.0)+6.8)*1000.0);
    // ����� ��������
    par[9][5] = StrToInt(EdtTRed9_5->Text);
    // ������� �������� ��
    par[9][6] = int(StrToFloat(EdtTRed9_6 -> Text) );

    // ���������� �2
    // �������� �2
    par[10][9] = int(StrToFloat(EdtTRed10_9 -> Text) * 4095.0 / BPM2_P_MAX);
    // ��������
    par[10][2] = int((0.6*log10(StrToFloat(EdtTRed10_2->Text)/100.0)+6.8)*1000.0);
    // ����� ��������
    par[10][5] = StrToInt(EdtTRed10_5->Text);
    // ������� �������� ��
    par[10][6] = int(StrToFloat(EdtTRed10_6 -> Text)  );

     // ���������� ���
    // ��� �������� ���
    par[11][10] = int(StrToFloat(EdtTRed11_10 -> Text) * 4095.0 / COMET_MAX_PD);


        // �������� ������� ��� ���
  //  if(StrToFloat(EdtTRed11_15->Text) == 0)
  //      par[11][15] = 0;
  //  else
        par[11][15] = int((0.6*log10(StrToFloat(EdtTRed11_15->Text)/100.0)+6.8)*1000.0);

    // ��������
    par[11][2] = int((0.6*log10(StrToFloat(EdtTRed11_2->Text)/100.0)+6.8)*1000.0);
    // ����� ��������
    par[11][5] = StrToInt(EdtTRed11_5->Text);
    // ������� �������� ��
    par[11][6] = int(StrToFloat(EdtTRed11_6 -> Text) );

    MemoStat -> Lines -> Add(Label_Time->Caption + "�������� ��������� ����������:");
    // ���������� �1
    // �������� �1
    if ( EdtTKon9_8 -> Text != EdtTRed9_8 -> Text )
        MemoStat -> Lines -> Add("���������� �1: �������� �1:" + EdtTKon9_8 -> Text + " -> " + EdtTRed9_8 -> Text );
    // ��������
    if ( EdtTKon9_2 -> Text != EdtTRed9_2 -> Text )
        MemoStat -> Lines -> Add("���������� �1: ��������:" + EdtTKon9_2 -> Text + " -> " + EdtTRed9_2 -> Text );
    // ����� ��������
    if ( EdtTKon9_5 -> Text != EdtTRed9_5 -> Text )
        MemoStat -> Lines -> Add("���������� �1: ����� ��������:" + EdtTKon9_5 -> Text + " -> " + EdtTRed9_5 -> Text );
    // ������� �������� ��
    if ( EdtTKon9_6 -> Text != EdtTRed9_6 -> Text )
        MemoStat -> Lines -> Add("���������� �1: ������� �������� ��:" + EdtTKon9_6 -> Text + " -> " + EdtTRed9_6 -> Text );
    // ���������� �2
    // �������� �2
    if ( EdtTKon10_9 -> Text != EdtTRed10_9 -> Text )
        MemoStat -> Lines -> Add("���������� �2: �������� �2:" + EdtTKon10_9 -> Text + " -> " + EdtTRed10_9 -> Text );
    // ��������
    if ( EdtTKon10_2 -> Text != EdtTRed10_2 -> Text )
        MemoStat -> Lines -> Add("���������� �2: ��������:" + EdtTKon10_2 -> Text + " -> " + EdtTRed10_2 -> Text );
    // ����� ��������
    if ( EdtTKon10_5 -> Text != EdtTRed10_5 -> Text )
        MemoStat -> Lines -> Add("���������� �2: ����� ��������:" + EdtTKon10_5 -> Text + " -> " + EdtTRed10_5 -> Text );
    // ������� �������� ��
    if ( EdtTKon10_6 -> Text != EdtTRed10_6 -> Text )
        MemoStat -> Lines -> Add("���������� �2: ������� �������� ��:" + EdtTKon10_6 -> Text + " -> " + EdtTRed10_6 -> Text );
    // ���������� ���
    // ��� �������� ���
    if ( EdtTKon11_10 -> Text != EdtTRed11_10 -> Text )
        MemoStat -> Lines -> Add("���������� ���: ���. �������� ���:" + EdtTKon11_10 -> Text + " -> " + EdtTRed11_10 -> Text );
    // �������� ������� ��� ���
    if ( EdtTKon11_15 -> Text != EdtTRed11_15 -> Text )
        MemoStat -> Lines -> Add("���������� ���: �������� ������� ��� ���:" + EdtTKon11_15 -> Text + " -> " + EdtTRed11_15 -> Text );
    // ��������
    if ( EdtTKon11_2 -> Text != EdtTRed11_2 -> Text )
        MemoStat -> Lines -> Add("���������� ���: ��������:" + EdtTKon11_2 -> Text + " -> " + EdtTRed11_2 -> Text );
    // ����� ��������
    if ( EdtTKon11_5 -> Text != EdtTRed11_5 -> Text )
        MemoStat -> Lines -> Add("���������� ���: ����� ��������:" + EdtTKon11_5 -> Text + " -> " + EdtTRed11_5 -> Text );
    // ������� �������� ��
    if ( EdtTKon11_6 -> Text != EdtTRed11_6 -> Text )
        MemoStat -> Lines -> Add("���������� ���: ������� �������� ��:" + EdtTKon11_6 -> Text + " -> " + EdtTRed11_6 -> Text );

    // ����������� ���������� ���������
    EdtTRed9_8 -> Color = clWhite;
    EdtTRed9_2 -> Color = clWhite;
    EdtTRed9_5 -> Color = clWhite;
    EdtTRed9_6 -> Color = clWhite;

    EdtTRed10_9 -> Color = clWhite;
    EdtTRed10_2 -> Color = clWhite;
    EdtTRed10_5 -> Color = clWhite;
    EdtTRed10_6 -> Color = clWhite;

    EdtTRed11_10 -> Color = clWhite;
    EdtTRed11_15 -> Color = clWhite;
    EdtTRed11_2 -> Color = clWhite;
    EdtTRed11_5 -> Color = clWhite;
    EdtTRed11_6 -> Color = clWhite;

    // �������� ��������
    VisualParA();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnTrNetClick(TObject *Sender)
{
    PanelParTr -> Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button43Click(TObject *Sender)
{
    PanelParTr -> Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnSaveTYesClick(TObject *Sender)
{
    // �������� �� ������������� ����������
    if ( ! DirectoryExists("LibT") ) CreateDir("LibT");
    AnsiString fileName = "LibT\\" + EdtNewNameT -> Text + ".txt";
    if ( ! FileExists( fileName ) )
    {
        // ������������ �������� ����������� �����
        if ( MessageDlg("������������� ������ ���������?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
        // ���� �������� ��������� ������ ������������
        if ( libNmb != -1 )
            DeleteFileA( "LibT\\" + ListBoxLibraryT -> Items -> operator[](libNmb) + ".txt" );
        int fileID = FileCreate ( fileName );
        FileClose( fileID );
    }
    else
    {
        if ( MessageDlg("���� � �������� ��������� ��� ����������, ������������?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
    }
    MemoLibT -> Lines -> SaveToFile( "LibT\\" + EdtNewNameT -> Text + ".txt" );
    GBSaveTDialog -> Visible = false;

    // ����� �������� ������
    int
        fileCount = 0,
        rezult;
    TSearchRec SR;
    // ����������
    fileCount = 0;
    Form1 -> ListBoxLibraryT -> Clear();
    rezult = FindFirst("LibT\\*.txt", faAnyFile, SR);
        while ( !rezult ){
        fileCount++;
        SR.Name.SetLength( SR.Name.Length() - 4 );
        Form1 -> ListBoxLibraryT -> Items -> Add( SR.Name );
        rezult = FindNext(SR);
    };
    Form1 -> ListBoxLibraryT -> Sorted;
    EdtNewNameT -> Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnSaveTNoClick(TObject *Sender)
{
    EdtNewNameT -> Text = "";
    GBSaveTDialog -> Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BitBtnSaveTClick(TObject *Sender)
{
    // ���������� ���������� � ������������� ������
    MemoLibT -> Lines -> Clear();
    MemoLibT -> Lines -> Add ( EdtTRed9_8 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed9_2 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed9_5 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed9_6 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed10_9 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed10_2 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed10_5 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed10_6 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed11_10 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed11_15 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed11_2 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed11_5 -> Text );
    MemoLibT -> Lines -> Add ( EdtTRed11_6 -> Text );

    // ����������� ����������� ����
    GBSaveTDialog -> Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BitBtnLoadTClick(TObject *Sender)
{
   EdtTRed9_8  -> Text = EdtTLib9_8  -> Text;
   EdtTRed9_2  -> Text = EdtTLib9_2  -> Text;
   EdtTRed9_5  -> Text = EdtTLib9_5  -> Text;
   EdtTRed9_6  -> Text = EdtTLib9_6  -> Text;
   EdtTRed10_9  -> Text = EdtTLib10_9  -> Text;
   EdtTRed10_2  -> Text = EdtTLib10_2  -> Text;
   EdtTRed10_5  -> Text = EdtTLib10_5  -> Text;
   EdtTRed10_6  -> Text = EdtTLib10_6  -> Text;
   EdtTRed11_10  -> Text = EdtTLib11_10  -> Text;
   EdtTRed11_15  -> Text = EdtTLib11_15  -> Text;
   EdtTRed11_2  -> Text = EdtTLib11_2  -> Text;
   EdtTRed11_5  -> Text = EdtTLib11_5  -> Text;
   EdtTRed11_6  -> Text = EdtTLib11_6  -> Text;

   EdtTRed9_8  -> Color = clSilver;
   EdtTRed9_2  -> Color = clSilver;
   EdtTRed9_5  -> Color = clSilver;
   EdtTRed9_6  -> Color = clSilver;
   EdtTRed10_9  -> Color = clSilver;
   EdtTRed10_2  -> Color = clSilver;
   EdtTRed10_5  -> Color = clSilver;
   EdtTRed10_6  -> Color = clSilver;
   EdtTRed11_10  -> Color = clSilver;
   EdtTRed11_15  -> Color = clSilver;
   EdtTRed11_2  -> Color = clSilver;
   EdtTRed11_5  -> Color = clSilver;
   EdtTRed11_6  -> Color = clSilver;
}
//---------------------------------------------------------------------------
//--����� ����� �� ������ ����������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryTClick(TObject *Sender)
{
    libNmb = ListBoxLibraryT -> ItemIndex;
    // ���� ������� ������������ ���������
    if ( libNmb != -1 )
    {
        AnsiString fName = "LibT\\" + ListBoxLibraryT->Items->operator[](libNmb) + ".txt";
        if ( FileExists ( fName ) )
        {
            MemoLibT -> Lines -> LoadFromFile( fName );
            LblPresentNameT -> Caption = "���������� ���: " + ListBoxLibraryT->Items->operator[](libNmb);
            // ��������� � ��������� �������� ������������ ������ ��� ������
            EdtTLib9_8  -> Text = MemoLibT -> Lines -> operator [](0);
            EdtTLib9_2  -> Text = MemoLibT -> Lines -> operator [](1);
            EdtTLib9_5  -> Text = MemoLibT -> Lines -> operator [](2);
            EdtTLib9_6  -> Text = MemoLibT -> Lines -> operator [](3);
            EdtTLib10_9  -> Text = MemoLibT -> Lines -> operator [](4);
            EdtTLib10_2  -> Text = MemoLibT -> Lines -> operator [](5);
            EdtTLib10_5  -> Text = MemoLibT -> Lines -> operator [](6);
            EdtTLib10_6  -> Text = MemoLibT -> Lines -> operator [](7);
            EdtTLib11_10  -> Text = MemoLibT -> Lines -> operator [](8);
            EdtTLib11_15  -> Text = MemoLibT -> Lines -> operator [](9);
            EdtTLib11_2  -> Text = MemoLibT -> Lines -> operator [](10);
            EdtTLib11_5  -> Text = MemoLibT -> Lines -> operator [](11);
            EdtTLib11_6  -> Text = MemoLibT -> Lines -> operator [](12);

            BitBtnLoadT -> Enabled = true;
        }
        else
        {
            // ��������� � ��������� �������� ������������ ������ ��� ������
            BitBtnLoadT -> Enabled = false;
            LblPresentNameT -> Caption = "���������� ���: ";
        }
    }
}
//---------------------------------------------------------------------------
//--������� �������������� ���������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryTMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    libNmb = -1;
}
//---------------------------------------------------------------------------
//--����� ���������� �� ������--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryTMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    // ���� ������� �������������� ���������
    if ( libNmb == -1 )
    {
        // ������� ������ �� ���������� ����������
        BitBtnLoadT -> Enabled = false;
        // ����������� ������������ ������������� �� �������
        LblPresentNameT -> Caption = "���������� ���: ";
        // ������ ����� � ���������� ������ ���������
        ListBoxLibraryT -> ItemIndex = -1;
        // �������� ������ ������������ ����������
            EdtTLib9_8  -> Text = "";
            EdtTLib9_2  -> Text = "";
            EdtTLib9_5  -> Text = "";
            EdtTLib9_6  -> Text = "";
            EdtTLib10_9  -> Text = "";
            EdtTLib10_2  -> Text = "";
            EdtTLib10_5  -> Text = "";
            EdtTLib10_6  -> Text = "";
            EdtTLib11_10  -> Text = "";
            EdtTLib11_15  -> Text = "";
            EdtTLib11_2  -> Text = "";
            EdtTLib11_5  -> Text = "";
            EdtTLib11_6  -> Text = "";
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//--������������ ��������� ����������--//
//---------------------------------------------------------------------------
void TForm1::DrawZasl(float alfa)
{
	// ��������� ��������

	static int
        alfaREM=-1;  //���������� ����
    int centrX=407;  //���������� ������ ������� X
    int centrY=375;  //���������� ������ ������� Y

    int KrugX1=407 - 100;
    int KrugY1=375 - 100;
    int KrugX2=407 + 100;
    int KrugY2=375 + 100;

    int RadX1=0;
    int RadY1=0;
    int RadX2=0;
    int RadY2=0;

    double
        x,y;

    long cZSL;
//----�������� ���������----------------
if(zin[1] & 0x0008)     cZSL=clWhite;
else if ( zaslPrNeopr ) cZSL=clSilver;
else                    cZSL=(TColor)RGB(61,237,105);
//--------------------------------------

    x=sin((alfa+22)*3.14159265/180)*180;
    y=cos((alfa+22)*3.14159265/180)*180;
    RadX2=centrX-x;
    RadY2=centrY-y;

    x=sin((alfa-22)*3.14159265/180)*180;
    y=cos((alfa-22)*3.14159265/180)*180;
    RadX1=centrX-x;
    RadY1=centrY-y;

    mnemo_fon->Canvas->Pen->Color=TColor(cZSL);  //����
    mnemo_fon->Canvas->Pen->Width=6;      //������ �����
    mnemo_fon->Canvas->Arc(KrugX1,KrugY1,KrugX2,KrugY2,RadX2,RadY2,RadX1,RadY1);


    mnemo_fon->Canvas->Pen->Color=(TColor)RGB(70,83,72);  //����  RGB
    mnemo_fon->Canvas->Pen->Width=8;      //������ �����
    mnemo_fon->Canvas->Arc(KrugX1,KrugY1,KrugX2,KrugY2,RadX1,RadY1,RadX2,RadY2);

    /*
    x=sin((alfa - 90 + 20)*3.14159265/180)*180;
    y=cos((alfa - 90 + 20)*3.14159265/180)*180;
    RadX2=centrX-x;
    RadY2=centrY-y;

    x=sin((alfa - 90 - 20)*3.14159265/180)*180;
    y=cos((alfa - 90 - 20)*3.14159265/180)*180;
    RadX1=centrX-x;
    RadY1=centrY-y;

    mnemo_fon->Canvas->Pen->Color=(TColor)RGB(70,83,72);  //����  RGB
    mnemo_fon->Canvas->Pen->Width=8;      //������ �����
    mnemo_fon->Canvas->Arc(KrugX1,KrugY1,KrugX2,KrugY2,RadX1,RadY1,RadX2,RadY2);
    */
}
//---------------------------------------------------------------------------
//--����� �������� ����������--//
//---------------------------------------------------------------------------
// ��������� ������� ������ 1
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
// ��������� ������� ������ 2
void __fastcall TForm1::BtnMagn2ResClick(TObject *Sender)
{
    // ���������� ��������� ������
    int Bnum = StrToInt(((TButton*)Sender)->Hint);

    if(Bnum == 5) // ������
    {
        Pnl_ResM2Vvod -> Visible = false;
        flagSBres = 0;
    }
    else if(Bnum == 4) // ����
    {
        {

            magnRes2 = StrToFloat(EditRESm2Vvod->Text);
            MemoStat -> Lines -> Add(Label_Time -> Caption + " | ������ ���������� 2 ������� ��: " + EditRESm2Vvod->Text);
        }


        Pnl_ResM2Vvod->Visible=false;
    }
    else
    {
        // ������ �� ��������� �������
        flagSBres = Bnum;
        EditRESm2Vvod->Text = "0";
        Pnl_ResM2Vvod -> Visible = true;
    }
}
// ��������� ������� ���
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnVCHMResClick(TObject *Sender)
{
  // ���������� ��������� ������
    int Bnum = StrToInt(((TButton*)Sender)->Hint);

    if(Bnum == 5) // ������
    {
        Pnl_ResMVCHMvod -> Visible = false;
        flagSBres = 0;
    }
    else if(Bnum == 4) // ����
    {
        {

            magnRes3 = StrToFloat(EditRESmVCHMvod->Text);
            MemoStat -> Lines -> Add(Label_Time -> Caption + " | ������ ���������� ��� ������� ��: " + EditRESmVCHMvod->Text);
        }


        Pnl_ResMVCHMvod->Visible=false;
    }
    else
    {
        // ������ �� ��������� �������
        flagSBres = Bnum;
        EditRESmVCHMvod->Text = "0";
        Pnl_ResMVCHMvod -> Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::M1_nameClick(TObject *Sender)
{
    // �������������� �������� ������
    if(PCMain -> ActivePage == TSNalad) // ��������� ����� ������ � ������� ���� �����
    {
        GK_n = StrToInt(((TLabel*)Sender)->Hint);
        Pnl_GK -> Top = ((TLabel*)Sender) -> Top + 30;
        Pnl_GK -> Left = ((TLabel*)Sender) -> Left - 65;
        Edit_GK -> Text = ((TLabel*)Sender)->Caption;
        Pnl_GK -> Visible = true;
    }
    else
    {
        Pnl_GK -> Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnGKOnClick(TObject *Sender)
{
    // ������������� ��� ������ �����
    if((bool)StrToInt(((TButton*)Sender)->Hint))
    {
        AnsiString temp_str = Edit_GK->Text;
        for(int i=0;i<8;i++) { iniGAS.M_names[GK_n][i] = 0; }
        for(int i=1;i<=temp_str.Length();i++) { iniGAS.M_names[GK_n][i-1] = temp_str[i]; }
        //iniGAS.M_names[GK_n] = Edit_GK->Text;

        
        SaveGasNames();
        //LoadGasNames();
        RenameGases();
    }

    Pnl_GK -> Visible = false;
}
//---------------------------------------------------------------------------
void TForm1::SaveGasNames()
{
    if(!DirectoryExists("Modules")) { CreateDir("Modules"); }
    if(!FileExists(loc_gas_udb))
    {
        int fileID = FileCreate(loc_gas_udb);
        FileClose(fileID);
    }
    if(FileExists(loc_gas_udb))
    {
        int SizeOfIniFile=(int)sizeof(iniGAS);

        FILE *im0;
        im0=fopen(loc_gas_udb,"wb");
        if(im0)       { fwrite(&iniGAS,SizeOfIniFile,1,im0); fclose(im0); }
        else if(!im0) { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
    }
    else { MessageBox(NULL, "���������� �������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::LoadGasNames()
{
    if(FileExists(loc_gas_udb))
    {
        int SizeOfIniFile=(int)sizeof(iniGAS);

        FILE *im0;
        im0=fopen(loc_gas_udb,"rb");
        if(im0)       { fread(&iniGAS,SizeOfIniFile,1,im0); fclose(im0); }
        else if(!im0) { MessageBox(NULL, "���������� ��������� ������", "������", MB_OK | MB_ICONSTOP); }
    }
    else { MessageBox(NULL, "���������� ��������� ������", "������", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::RenameGases()
{
    AnsiString temp_str = "";
    if(iniGAS.M_names[1][0])
    {
        for(int i=0;i<8;i++) { temp_str = temp_str + iniGAS.M_names[1][i]; }
        M1_name->Caption = temp_str;
    }
    else M1_name->Caption = "---";

    temp_str = "";
    if(iniGAS.M_names[2][0])
    {
        for(int i=0;i<8;i++) { temp_str = temp_str + iniGAS.M_names[2][i]; }
        VCHM_name->Caption = temp_str;
    }
    else VCHM_name->Caption = "---";

    temp_str = "";
    if(iniGAS.M_names[3][0])
    {
        for(int i=0;i<8;i++) { temp_str = temp_str + iniGAS.M_names[3][i]; }
        M2_name->Caption = temp_str;
    }
    else M2_name->Caption = "---";
}
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
    if((BU_IVE[0]->RCom)&&(!(Comport[0]->DevState))) Comport[0]->PortTask |= 0x100;
    if((BU_IVE[1]->RCom)&&(!(Comport[0]->DevState))) Comport[0]->PortTask |= 0x200;
    if(!(Comport[0]->PortTask)&&!(Comport[0]->Pr_nal))
    {
        if(nasmod[9])
            Comport[0]->PortTask |= 0x01;
        else
            diagnS[BU_IVE[0]->diagnS_byte] &= ~(BU_IVE[0]->diagnS_mask);

        if(nasmod[10])
            Comport[0]->PortTask |= 0x02;
        else
            diagnS[BU_IVE[1]->diagnS_byte] &= ~(BU_IVE[1]->diagnS_mask);
    }

    if((Comport[0]->PortTask) & 0x100)
    {
        Comport[0]->DevErr = BU_IVE[0]->BU_IVE_Manage(Comport[0]->DevState,1);
        if((Comport[0]->DevState) > 1)
        {
            (Comport[0]->DevErr) ? diagnS[BU_IVE[0]->diagnS_byte] |= BU_IVE[0]->diagnS_mask : diagnS[BU_IVE[0]->diagnS_byte] &= (~BU_IVE[0]->diagnS_mask);
            (Comport[0]->PortTask) &= (~0x100);
            BU_IVE[0]->RCom = 0;
            Comport[0]->DevState = 0;
        }
        return;
    }
    if((Comport[0]->PortTask) & 0x200)
    {
        Comport[0]->DevErr = BU_IVE[1]->BU_IVE_Manage(Comport[0]->DevState,1);
        if((Comport[0]->DevState) > 1)
        {
            (Comport[0]->DevErr) ? diagnS[BU_IVE[1]->diagnS_byte] |= BU_IVE[1]->diagnS_mask : diagnS[BU_IVE[1]->diagnS_byte] &= (~BU_IVE[1]->diagnS_mask);
            (Comport[0]->PortTask) &= (~0x200);
            BU_IVE[1]->RCom = 0;
            Comport[0]->DevState = 0;
        }
        return;
    }
    else if((Comport[0]->PortTask)&0x01)
    {
        Comport[0]->DevErr = BU_IVE[0]->BU_IVE_Manage(Comport[0]->DevState,0);
        if((Comport[0]->DevState)>1)
        {
            (Comport[0]->DevErr) ? diagnS[BU_IVE[0]->diagnS_byte] |= BU_IVE[0]->diagnS_mask : diagnS[BU_IVE[0]->diagnS_byte] &= (~BU_IVE[0]->diagnS_mask);
            (Comport[0]->PortTask) &= (~0x01);
            Comport[0]->DevState = 0;
        }
        return;
    }
    else if((Comport[0]->PortTask)&0x02)
    {
        Comport[0]->DevErr = BU_IVE[1]->BU_IVE_Manage(Comport[0]->DevState,0);
        if((Comport[0]->DevState)>1)
        {
            (Comport[0]->DevErr) ? diagnS[BU_IVE[1]->diagnS_byte] |= BU_IVE[1]->diagnS_mask : diagnS[BU_IVE[1]->diagnS_byte] &= (~BU_IVE[1]->diagnS_mask);
            (Comport[0]->PortTask) &= (~0x02);
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

    if(!(Comport[1]->PortTask)&&!(Comport[1]->Pr_nal))
    {
        if(nasmod[8])
            Comport[1]->PortTask |= 0x01;
        else
            diagnS[TRMD[0]->diagnS_byte] &= ~(TRMD[0]->diagnS_mask);

        Comport[1]->PortTask |= 0x06;
    }

    if(Comport[1]->PortTask & 0x01)
	{
		Comport[1]->DevErr = TRMD[0]->TRMD_Manage(Comport[1]->DevState,0);
		if(Comport[1]->DevState > 1)
		{
			Comport[1]->DevErr ? diagnS[TRMD[0]->diagnS_byte] |= TRMD[0]->diagnS_mask : diagnS[TRMD[0]->diagnS_byte] &= (~TRMD[0]->diagnS_mask);
			Comport[1]->PortTask &= (~0x01);
			Comport[1]->DevState = 0;
		}
		return;
	}
    else if(Comport[1]->PortTask & 0x02)
	{
		Comport[1]->DevErr = Dat_MTM9D[0]->DatMTM9D_Manage(Comport[1]->DevState,0);
		if(Comport[1]->DevState > 1)
		{
			Comport[1]->DevErr ? diagnS[Dat_MTM9D[0]->diagnS_byte] |= Dat_MTM9D[0]->diagnS_mask : diagnS[Dat_MTM9D[0]->diagnS_byte] &= (~Dat_MTM9D[0]->diagnS_mask);
			Comport[1]->PortTask &= (~0x02);
			Comport[1]->DevState = 0;
		}
		return;
	}
    else if(Comport[1]->PortTask & 0x04)
	{
		Comport[1]->DevErr = Dat_MTP4D[0]->DatMTP4D_Manage(Comport[1]->DevState,0);
		if(Comport[1]->DevState > 1)
		{
			Comport[1]->DevErr ? diagnS[Dat_MTP4D[0]->diagnS_byte] |= Dat_MTP4D[0]->diagnS_mask : diagnS[Dat_MTP4D[0]->diagnS_byte] &= (~Dat_MTP4D[0]->diagnS_mask);
			Comport[1]->PortTask &= (~0x04);
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

    if(!(Comport[2]->PortTask)&&!(Comport[2]->Pr_nal)) Comport[2]->PortTask |= 0x01;
	
    if(Comport[2]->PortTask & 0x01)
	{
		Comport[2]->DevErr = AZ_drive[0]->AZ_manage(Comport[2]->DevState);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[AZ_drive[0]->diagnS_byte] |= AZ_drive[0]->diagnS_mask : diagnS[AZ_drive[0]->diagnS_byte] &= (~AZ_drive[0]->diagnS_mask);
			Comport[2]->PortTask &= (~0x01);
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
void __fastcall SComport::Com_Timer(TObject *Sender)
{
    if(((TTimer*)Sender)->Name == "ComTimer1") Timer_Com1();
    if(((TTimer*)Sender)->Name == "ComTimer2") Timer_Com2();
    if(((TTimer*)Sender)->Name == "ComTimer3") Timer_Com3();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//-------------------  ������ �� ��������� �������  -------------------------
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
