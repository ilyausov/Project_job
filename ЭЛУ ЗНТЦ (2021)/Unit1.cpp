//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <ctype.h>
#include <math.hpp>
#pragma hdrstop

#include "Unit1.h"
#include "Modules\Com\Com.cpp"
#include "Logic.cpp"
#include "Header.h"

//#include "Modules\Com\Com.cpp"
#include "Modules\DatMPT200\DatMPT200.cpp"
#include "Modules\DatPPT200\DatPPT200.cpp"
#include "Modules\TRMD\TRMD.cpp"
#include "Names.h"
#include "External.cpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "CSPIN"
#pragma resource "*.dfm"

TForm1 *Form1;

// ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ ЭЛУ И ИТ
// -------------------------------------------------------------------------------
int tigelVPos = 0;
//запрос толщины пленки по номеру канала
char strTOLSH[25]={0};

//---------------------------------------------------------------------------
//--Класс: Параллельный поток с логикой--//
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
//--Стандартные функции члены класса--//
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
    FreeOnTerminate = true; // освободить память после исполнения
    // определение частоты процессора
    QueryPerformanceFrequency(&TimeFreq);
    TimeFreq.QuadPart /= 1000;
    // определение текущего времени
    QueryPerformanceCounter(&TimeNew);
    while ( !Terminated )
    {
        TimeOld = TimeNew;
        // определение текущего времени
        QueryPerformanceCounter(&TimeNew);
        // главный цикл логики
        Synchronize(LM);
    }
};
//---------------------------------------------------------------------------
//--Класс: Параллельный поток с точным таймером--//
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
//--Стандартные функции члены класса--//
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
    FreeOnTerminate = true; // освободить память после исполнения
    LARGE_INTEGER
        TimeNew, TimeOld, TimeFreq;
    // определение частоты процессора
    QueryPerformanceFrequency(&TimeFreq);
    TimeFreq.QuadPart /= 1000;
    // определение текущего времени
    QueryPerformanceCounter(&TimeOld);
    unsigned long
        timeOld = GetTickCount(),
        timeNew;
    // определение текущего времени
    while ( !Terminated )
    {
        // определение нового текущего времени
        timeNew = GetTickCount();
        // сравнение нового времени с предыдущим на разницу в 1 мс
        if ( timeNew != timeOld )
        {
            for ( unsigned long i = 0 ; i < ( timeNew - timeOld ) ; i++ )
                // запустили диспетчер порта
                Synchronize(Timer1ms);
            // обновили "старое" время
            timeOld = timeNew;
        }
        // определение нового текущего времени
        QueryPerformanceCounter(&TimeNew);
        // сравнение нового времени с предыдущим на разницу в 1 мс
        if (((TimeNew.QuadPart - TimeOld.QuadPart)/TimeFreq.QuadPart)>=1)
        {
            // запустили диспетчер внутренних плат
            Synchronize(EM);
            // обновили "старое" время
            TimeOld = TimeNew;
        }
    }
}

//---------------------------------------------------------------------------
//--Диспетчер обмена со встроенными платами--//
//---------------------------------------------------------------------------
void TForm1::ExternalManager()
{

  	// если признак отладки обходим
	if(pr_otl) return;

    // чтение PEX-P32C32(1)
    if ( externalTask & 0x01 )
    {
        // опросили
        externalError = PISO_P32C32U_1( 0 , zin );
        // анализ ответа
        switch ( externalError )
        {
            // ошибок нет
            case 0:
            {
                // сбросить диагностику
                diagnS[1] &= (~0x01);
                // снять задачу
                externalTask &= (~0x01);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x01;
                // снять задачу
                externalTask &= (~0x01);
            }; break;
        }
    }
    // чтение PEX-P32C32(2)
    if ( externalTask & 0x02 )
    {
        // опросили
        externalError = PISO_P32C32U_2( 0 , zin );
        // анализ ответа
        switch ( externalError )
        {
            // ошибок нет
            case 0:
            {
                // сбросить диагностику
                diagnS[1] &= (~0x02);
                // снять задачу
                externalTask &= (~0x02);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x02;
                // снять задачу
                externalTask &= (~0x02);
            }; break;
        }
    }
    // чтение ISO-P32C32
    if ( externalTask & 0x04 )
    {
        // опросили
        externalError = ISO_P32C32_1( 0 , zin );
        // анализ ответа
        switch ( externalError )
        {
            // ошибок нет
            case 0:
            {
                // сбросить диагностику
                diagnS[1] &= (~0x04);
                // снять задачу
                externalTask &= (~0x04);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x04;
                // снять задачу
                externalTask &= (~0x04);
            }; break;
        }
    }
    // чтение ACL-7250
    else if( externalTask & 0x08 )
    {
        // опросили
        externalError = ACL7250(0,zin);
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x08);
                // снять задачу
                externalTask &=(~0x08);
            }; break;
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x08;
                // снять задачу
                externalTask &= (~0x08);
            }; break;
        }
    }

    // чтение аналоговых входных сигналов с PISO-813
    else if(externalTask & 0x10)
    {
        // опросили
        externalError = PISO_813U(aik , AIK_COUNT * 8);
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x10);
                // снять задачу
                externalTask &= (~0x10);
            }; break;
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x10;
                // снять задачу
                externalTask &= (~0x10);
            }; break;
        }
    }

    // записать в PEX-P32C32(1)
    else if(externalTask & 0x20)
    {
         // опросили
         externalError = PISO_P32C32U_1( 1 , out );
        // анализ команды
        switch ( externalError )
        {
            // ошибок нет
            case 0:
            {
                // сбросить диагностику
                diagnS[1] &= (~0x01);
                // если нет ошибок связи снять задачу
                externalTask &= (~0x20);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x01;
                // снять задачу
                externalTask &= (~0x20);
            }; break;
        }
    }
    // записать в PEX-P32C32(2)
    else if(externalTask & 0x40)
    {
         // опросили
         externalError = PISO_P32C32U_2( 1 , out );
        // анализ команды
        switch ( externalError )
        {
            // ошибок нет
            case 0:
            {
                // сбросить диагностику
                diagnS[1] &= (~0x02);
                // если нет ошибок связи снять задачу
                externalTask &= (~0x40);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x02;
                // снять задачу
                externalTask &= (~0x40);
            }; break;
        }
    }
    // записать в ACL7250
    else if(externalTask & 0x80 )
    {
        // записали
        externalError = ACL7250( 1 , out );
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x08);
                // снять задачу
                externalTask &= (~0x80);
            }; break;
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x08;
                // снять задачу
                externalTask &= (~0x80);
            }; break;
        }
    }
    // запись в ISO-DA16
    else if( externalTask & 0x100 )
    {
        for( int i = 0 ; i < AOUT_COUNT * 4 ; i++ )
        {
            externalError = ISO_DA16 ( 0 , aout[i] , 0 , i );
            // анализ ответа
            switch ( externalError )
            {
                case 0:
                {
                    // снять диагностику
                    diagnS[1] &= (~0x20);
                }; break;
                default:
                {
                    // выставить диагностику
                    diagnS[1] |= 0x20;
                    // снять задачу
                    externalTask &= (~0x100);
                }; break;
            }
        }
        externalTask &= (~0x100);
    }
    else
    {
        externalTask = 0x1FF;
    }    
}
//---------------------------------------------------------------------------
//--Визуализация страницы формата--//
void TForm1::VisualFormat()
{    // маска
   unsigned int mask;
    unsigned char i=0,j=0;
    // визуализация дискретных входов (ISO P32C32)
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

        // визуализация дискретных выходов (ISO P32C32)
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

    // визуализация аналоговых входов
    for(i=0;i<AIK_COUNT;i++)
    { for(j=0;j<8;j++)
      {
          Dec_Ain[i][j] -> Text = IntToStr(aik[i*8+j]);
          CG_Ain[i][j] -> Progress = aik[i*8+j];
          UV_Ain[i][j] -> Text = FloatToStrF( float(aik[i*8+j])/4095.0 * 10.0,  ffFixed, 5, 3);
      }
    }
    // визуализация аналоговых выходов
    for(i=0;i<AOUT_COUNT;i++)
    { for(j=0;j<4;j++)
      {
        if(i*4+j==7){if(aoutKon[i*4+j] < 0)  { aoutKon[i*4+j] = 0;  }}
        else{ if(aoutKon[i*4+j] < 8192)  { aoutKon[i*4+j] = 8192;  }}
          if(aoutKon[i*4+j] > 16384) { aoutKon[i*4+j] = 16384; }
      if(i*4+j==7){if(aout[i*4+j] < 0)     { aout[i*4+j] = 0;     }}
      else{if(aout[i*4+j] < 8192)     { aout[i*4+j] = 8192;     }}
          if(aout[i*4+j] > 16383)    { aout[i*4+j] = 16383;    }

          Dec_Aout[i][j] -> Text = IntToStr( aoutKon[i*4+j] );
    if(i*4+j==7)
          UV_Aout[i][j] -> Text = FloatToStrF((float(aout[i*4+j])-8192.0) / 8191.0 * 10.0, ffFixed, 5, 2);
    else
        UV_Aout[i][j] -> Text = FloatToStrF( float(aout[i*4+j]-8192) / 8191.0 * 10.0, ffFixed, 5, 3);

          CG_Aout[i][j] -> Progress = aout[i*4+j];
      }
    }
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

    // инициализация массива отображения текущих графиков
    serTemp[0] = Series1;
    serTemp[1] = Series2;
    serTemp[2] = Series3;

    // инициализация массива отображения архивных графиков
    serArh[0] = Series11;
    serArh[1] = Series12;
    serArh[2] = Series13;

	// инициализировать объекты РРГ
    InitObjectsRRG();   //ПОДКЛЮЧИТЬ RRG.cpp
    InitObjectsKl();
    
    // увеличить память под отображение картинок
    TSWork -> DoubleBuffered = true;
    EditSHRName -> DoubleBuffered = true;
    Pnl_Work -> DoubleBuffered = true;
    LBError -> DoubleBuffered = true;
	PanDIAGvak -> DoubleBuffered = true;

    // вывод архивных файлов
    int
        fileCount,
        rezult;
    TSearchRec SR;

    // библиотеки
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

    // статистика и диагностика
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

    // графики
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
    if(MessageDlg("Выйти из программы?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
    Close();
}
//---------------------------------------------------------------------------
//--Визуализация мнемосхемы--//
//---------------------------------------------------------------------------
void TForm1::VisualMnemo()
{
    if(!ust_ready) return;

    VisualColorElement();                   //  визуализация элементов мнемосхемы
    VisualVoda();                           //  прорисовать таблицу воды
    VisualParam();                          //  визуалиазция параметров мнемосхемы
    VisualDiagn();                          //  отображение диагностик на мнемосхему
    VisualButtons();                        //  отображение клавиш
    VisualZagol();                          //  визуализация шапки программы
    VisualOperatorDlg();                    //  визуализация диалога оператора
}

//----Параметры / Мнемосхема
void __fastcall TForm1::PCMainChange(TObject *Sender)
{

  if(PCMain -> ActivePage == TSNalad)
  {
        Pnl_Work -> Visible = false;
        Pnl_Work -> Parent = TabSheet1;
        Pnl_Work -> Top = -4;
        Pnl_Work -> Left = +4;




        PnlMnemoParam -> Height = 58; //уменьшаем размер таблицы
        Pnl_GK->BringToFront();

        

        State     -> Visible = false;
        Panel317->BringToFront();

        Panel2->Visible=false;
        Panel96 ->Visible=false;
        Panel37 ->Visible=false;
        Panel234->Visible=false;
        Panel230->Visible=false;

        Panel207->Visible=false;
        Panel97 ->Visible=false;
        Panel237->Visible=false;
        Panel227->Visible=false;
        Panel67->Visible=false;

        EdtZadA01->Visible=false;
        EdtZadA02->Visible=false;
        EdtZadA03->Visible=false;
        Edit13   ->Visible=false;
        EdtZadA05->Visible=false;

        EdtTekA01->Visible=false;
        EdtTekA02->Visible=false;
        EdtTekA03->Visible=false;
        EdtTekA04->Visible=false;
        EdtTekA05->Visible=false;


        VisualMnemo();
        Pnl_Work -> Visible = true;
  }
  else if(PCMain -> ActivePage == TSWork)
  {     Pnl_Work -> Visible = false;
        Pnl_Work -> Parent = TSWork;

       
        Pnl_Work -> Top = 32;
        Pnl_Work -> Left = 8;

        PnlMnemoParam -> Height = 188; //Увеличиваем размер таблицы

        

        State     -> Visible = true;

        Panel2->Visible=true;
        Panel96 ->Visible=true;
        Panel37 ->Visible=true;
        Panel234->Visible=true;
        Panel230->Visible=true;

        Panel207->Visible=true;
        Panel97 ->Visible=true;
        Panel237->Visible=true;
        Panel227->Visible=true;
        Panel67->Visible=true;

        EdtZadA01->Visible=true;
        EdtZadA02->Visible=true;
        EdtZadA03->Visible=true;
        Edit13   ->Visible=true;
        EdtZadA05->Visible=true;

        EdtTekA01->Visible=true;
        EdtTekA02->Visible=true;
        EdtTekA03->Visible=true;
        EdtTekA04->Visible=true;
        EdtTekA05->Visible=true;

        VisualMnemo();
        Pnl_Work -> Visible = true;
  }
}
//---------------------------------------
/////// ГЕНЕРАЦИЯ ЭЛЕМЕНТОВ ФОРМАТА
//---------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{

//выставляем панели оператора по центру станицы
APanel -> Left = 600;
APanel -> Top = 340;
PnlDiagm ->Left=655;
PnlDiagm ->Top=70;
PnlCondition->Left=650;
PnlCondition->Top=180;

//обновляем название газов
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
     Form1->GB_zin4_2,
     Form1->GB_zin5_1,
     Form1->GB_zin5_2
   };

 /////////////////////////////////////////////////////////////////////////////
   for(i=0;i<ZIN_COUNT;i++)
   { // цикл по кол-ву бит в контейнере
     for(j=0;j<16;j++)
     {  //  создание экземпляра названия ZIN
        Title_Zin[i][j] = new TLabel(this);
        // расположение, размеры, название
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
        // создание экземпляра
        Check_Zin[i][j] = new TImage(this);
        // расположение, размера
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
     Form1->GB_out3_2,
     Form1->GB_out4_1,
     Form1->GB_out4_2,
     Form1->GB_out5_1,
     Form1->GB_out5_2
   };
   /////////////////////////////////////////////////////////////////////////////
   // цикл по количеству контейнеров для отображения
   for(i=0;i<OUT_COUNT;i++)
   { // цикл по количеству бит в контейнере
     for(j=0;j<16;j++)
     {  // создание экземпляра названия
        Title_Out[i][j] = new TLabel(this);
        // расположение, размеры, название
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
        // создание экземпляра
        Check_Out[i][j] = new TImage(this);
        // расположение, размера
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
   // цикл по количеству контейнеров для отображения
   for(i=0;i<AIK_COUNT;i++)
   { // цикл по количеству бит в контейнере
     for(j=0;j<8;j++)
     {  // создание экземпляра названия
        Title_Ain[i][j] = new TLabel(this);
        // расположение, размеры, название
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
        // создание экземпляра
        Dec_Ain[i][j] = new TEdit(this);
        // расположение, размеры
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
        // создание экземпляра
        UV_Ain[i][j] = new TEdit(this);
        // расположение, размеры
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
        // создание экземпляра
        CG_Ain[i][j] = new TCGauge(this);
        // расположение, размеры
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
   { Form1->GB_aout0
   };
   /////////////////////////////////////////////////////////////////////////////
   // цикл по количеству контейнеров для отображения
   for(i=0;i<AOUT_COUNT;i++)
   { // цикл по количеству бит в контейнере
     for(j=0;j<4;j++)
     {  // создание экземпляра названия
        Title_Aout[i][j] = new TLabel(this);
        // расположение, размеры, название
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
        // создание экземпляра
        Dec_Aout[i][j] = new TEdit(this);
        // расположение, размеры
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
        // создание экземпляра
        UV_Aout[i][j] = new TEdit(this);
        // расположение, размеры
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
        // создание экземпляра
        Dec_Aout_zad[i][j] = new TEdit(this);
        // расположение, размеры
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
        // создание экземпляра
        UV_Aout_zad[i][j] = new TEdit(this);
        // расположение, размеры
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
        // создание экземпляра
        CG_Aout[i][j] = new TCGauge(this);
        // расположение, размеры
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
        if(i*4+j==7)CG_Aout[i][j] -> MinValue = 0;
        else        CG_Aout[i][j] -> MinValue = 8193;
        ////////////////////////////////////////////////////////////////////////
        // создание экземпляра
        CG_Aout_zad[i][j] = new TCGauge(this);
        // расположение, размеры
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
        if(i*4+j==7)CG_Aout_zad[i][j] -> MinValue = 0;
        else        CG_Aout_zad[i][j] -> MinValue = 8193;
        ////////////////////////////////////////////////////////////////////////
        // создание экземпляра
        Zero_Aout[i][j] = new TButton(this);
        // расположение, размеры
        Zero_Aout[i][j] -> Parent = AoutParents[i];
        Zero_Aout[i][j] -> Left = 16;
        Zero_Aout[i][j] -> Top = 70 + 72 * j;
        Zero_Aout[i][j] -> Width = 25;
        Zero_Aout[i][j] -> Height = 25;
        Zero_Aout[i][j] -> Caption = "0";
        Zero_Aout[i][j] -> Hint = IntToStr(i*4 + j);
        Zero_Aout[i][j] -> OnClick = ZeroAoutClick;
        ////////////////////////////////////////////////////////////////////////
        // создание экземпляра
        Zad_Aout[i][j] = new TScrollBar(this);
        // расположение, размеры
        Zad_Aout[i][j] -> Parent = AoutParents[i];
        Zad_Aout[i][j] -> Left = 42;
        Zad_Aout[i][j] -> Ctl3D = true;
        Zad_Aout[i][j] -> Top = 70 + 72 * j;
        Zad_Aout[i][j] -> Width = 390;
        Zad_Aout[i][j] -> Height = 25;
        Zad_Aout[i][j] -> Max = 16383;
        if(i*4+j==7)Zad_Aout[i][j] -> Min = 0;
        else        Zad_Aout[i][j] -> Min = 8193;
        Zad_Aout[i][j] -> Hint = IntToStr(i*4 + j);
        Zad_Aout[i][j] -> ShowHint = false;
        Zad_Aout[i][j] -> OnChange = SetAoutChange;
        ////////////////////////////////////////////////////////////////////////
        // создание экземпляра
        Ent_Aout[i][j] = new TButton(this);
        // расположение, размеры
        Ent_Aout[i][j] -> Parent = AoutParents[i];
        Ent_Aout[i][j] -> Left = 433;
        Ent_Aout[i][j] -> Top = 70 + 72 * j;
        Ent_Aout[i][j] -> Width = 25;
        Ent_Aout[i][j] -> Height = 25;
        Ent_Aout[i][j] -> Caption = "У";
        Ent_Aout[i][j] -> Hint = IntToStr(i*4 + j);
        Ent_Aout[i][j] -> OnClick = EntAoutClick;
  }
 }

    // запуск логического кольца в потоке
    // создали поток
    LogicThread = new TLogicThread(true);
    // поток самоудалится после завершения
    LogicThread -> FreeOnTerminate = true;
    // приоритет потока низкий
    LogicThread -> Priority = tpLower;
    // запустили поток
    LogicThread -> Resume();

    // запуск высокоточного таймера в потоке
    // создали поток
    TimerExist = new TTimerExist(true);
    // поток самоудалится после завершения
    TimerExist -> FreeOnTerminate = true;
    // приоритет потока низкий
    TimerExist -> Priority = tpLower;
    // запустили поток
    TimerExist -> Resume();

	// активация драйверов плат
    OpenPISO_DIO(); // попытка связаться с драйвером PISO-P32C32/P16R16
    OpenISO_P32C32(); // попытка связаться с драйвером ISO-P32C32
    OpenACL_7250();   // попытка связаться с драйвером ACL-7250
    OpenPISO_813 ();    // попытка связаться с драйвером PISO-813
    OpenISO_DA16();    // попытка связаться с драйвером ISO-DA16

    PCMain  -> DoubleBuffered = true;
    Form1-> DoubleBuffered = true;
    Pnl_Title -> DoubleBuffered = true;

    Label_Time -> Caption = FormatDateTime("hh:mm:ss",Time());
    Label_Date -> Caption = FormatDateTime("dd.mm.yyyy",Date());

    // загрузить значения настроечного массива
    MemoNasmod -> Lines -> LoadFromFile("Nasmod\\Nasmod.txt");

    EditNastrTo0  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](0));
    EditNastrTo1  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](1));
    EditNastrTo2  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](2));
    EditNastrTo3  -> Text = MemoNasmod -> Lines -> operator [](3);
    EditNastrTo4  -> Text = MemoNasmod -> Lines -> operator [](4);
    EditNastrTo5  -> Text = MemoNasmod -> Lines -> operator [](5);
    EditNastrTo6  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](6));
    EditNastrTo7  -> Text = MemoNasmod -> Lines -> operator [](7);
    EditNastrTo8  -> Text = MemoNasmod -> Lines -> operator [](8);
    EditNastrTo9  -> Text = MemoNasmod -> Lines -> operator [](9);
    EditNastrTo10  -> Text = MemoNasmod -> Lines -> operator [](10);
    EditNastrTo11  -> Text = MemoNasmod -> Lines -> operator [](11);
    EditNastrTo12  -> Text = MemoNasmod -> Lines -> operator [](12);
    EditNastrTo13  -> Text = MemoNasmod -> Lines -> operator [](13);
    EditNastrTo14  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](14));
    EditNastrTo15  -> Text = MemoNasmod -> Lines -> operator [](15);
    EditNastrTo16  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](16));

    MemoNasmod -> Lines -> Clear();

    // загрузить значения T
    MemoT -> Lines -> LoadFromFile("Nasmod\\Mex.txt");
    EdtTRed1  -> Text = MemoT -> Lines -> operator [](0);
    EdtTRed2  -> Text = MemoT -> Lines -> operator [](1);
    EdtTRed3  -> Text = MemoT -> Lines -> operator [](2);
    EdtTRed4  -> Text = MemoT -> Lines -> operator [](3);
    EdtTRed5  -> Text = MemoT -> Lines -> operator [](4);
    EdtTRed6  -> Text = MemoT -> Lines -> operator [](5);
    EdtTRed7  -> Text = MemoT -> Lines -> operator [](6);
    EdtTRed8  -> Text = MemoT -> Lines -> operator [](7);
    MemoT -> Lines -> Clear();

    Init_SComport();
	Comport[0]->Reser_Port(Comport[0]->BTN_reset);  // включение порта
    Comport[1]->Reser_Port(Comport[1]->BTN_reset);  // включение порта
    Comport[2]->Reser_Port(Comport[2]->BTN_reset);  // включение порта
    Comport[3]->Reser_Port(Comport[3]->BTN_reset);  // включение порта

    Init_DatMPT200();
    Init_DatPPT200();
    Init_TRMD();
    Init_SAZ_drive();
    AZdrive_Load();	// загрузка параметров драйверов

    SetOut(1,2,0x800);				// выставить Стоп механизмов

    BtnNastrDaClick(BtnNastrDa);
    BtnAutoDaClick(BtnAutoDa);

    BtnTDaClick(BtnTDa);
    BtnRDaClick(BtnRDa);
    ust_ready = 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Визуализация элементов мнемосхемы--//
TColor TForm1::SetPopeColor(bool value)
{
    if(value) return clWhite;
    else      return 0x0064F046;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Отображение текущих значений мнемосхемы--//
void TForm1::VisualParam()
{       
   //датчики
    if((D_D1>=0)&&(D_D1<=10000))  { d_d1->Caption=FloatToStrF(pow(10,(float)D_D1/1000.0*1.667-9.333),ffExponent,3,8); }   //МРТ200
    if((D_D2>=0)&&(D_D2<=10000))  { d_d2->Caption=FloatToStrF(pow(10,(float)D_D2/1000.0-3.5),ffExponent,3,8); }           //РРТ200
    if((D_D3>=0)&&(D_D3<=10000))  { d_d3->Caption=FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffExponent,3,8); }           //РРТ200
    if((D_D4>=0)&&(D_D4<=10000))  { d_d4->Caption=FloatToStrF(pow(10,(float)D_D4/1000.0*1.667-9.333),ffExponent,3,8); }   //МРТ200
    if((D_D5>=0)&&(D_D5<=10000))  { d_d5->Caption=FloatToStrF(pow(10,(float)D_D5/1000.0*1.667-9.333),ffExponent,3,8); }   //МРТ200
    if((D_D6>=0)&&(D_D6<=10000))  { d_d6->Caption=FloatToStrF(pow(10,(float)D_D6/1000.0-3.5),ffExponent,3,8); }           //РРТ200
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Номер стадии
   if((shr[3]||shr[4]||shr[2])&&(N_ST))
    {
        if(N_ST==1)EditNST -> Caption = "Прогрев п/д";
        else
        {
            int i=N_ST-1;
            EditNST -> Caption = "Напыление слоя: ";
           EditNST -> Caption =  EditNST -> Caption+i;
        }

    }
    else
        EditNST -> Caption = "";
    //--------------------------------------------------------------------------
    //Температуры

    Temp_K  -> Caption = FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1)+ "°С";//шлюз
    Temp_PD  -> Caption = FloatToStrF((float(TEK_TEMP3)/10.0),ffFixed,4,1)+ "°С";//п/д
    //--------------------------------------------------------------------------

    if(PCMain -> ActivePage == TSNalad) // Наладка
    {

            //Номер тигля
            Num_Tig -> Caption = "";
            EdtTekA01->Text="0";
            EdtZadA00->Text=FloatToStrF((((((float)par[0][1])/100)-4)/1.6), ffFixed, 5, 1);
            //Ток нагрева
            if(shr[29])EdtTekA00->Text=FloatToStrF((((float)TEK_TEMP1/100.0)-4.0)/1.6, ffFixed, 4, 1);
            else       EdtTekA00->Text="0,0";
    }
    else // Мнемосхема
    {
//задания Таблица***************************************************************
        if((shr[3])&&(N_ST))
        {
        //Ток нагрева
            if(N_ST==1)
                EdtZadA00->Text=FloatToStrF((((((float)par[N_ST][1])/100)-4)/1.6), ffFixed, 5, 1);
            else
                EdtZadA00->Text="0,0";
        //Номер тигля
            if((N_ST>1)&&(N_ST<102))
            {
                EdtZadA01->Text=FloatToStrF((float)par[N_ST][4],ffFixed, 5, 0);
            }
            else
                EdtZadA01->Text="0";
        //Номер плёнки
            if((N_ST>1)&&(N_ST<102))
            {
                EdtZadA02->Text=FloatToStrF((float)par[N_ST][5],ffFixed, 5, 0);
            }
            else
                EdtZadA02->Text="0";
        //Ток эмиссии
            if((N_ST>1)&&(N_ST<102))
            {
                EdtZadA03->Text=FloatToStrF((float)PAR_ELU/10,ffFixed, 5, 1);
            }
            else
                EdtZadA03->Text="0,0";

        //Время процесса
            EdtZadA05->Text=FloatToStrF((float)par[N_ST][2],ffFixed, 5, 0);
        }
        else
        {
            EdtZadA00->Text="0,0";
            EdtZadA01->Text="0";
            EdtZadA02->Text="0";
            EdtZadA03->Text="0,0";
            EdtZadA05->Text="0";
        }
        //Текущие
        if((shr[3])&&(N_ST))
        {
            //Ток нагрева
            if(shr[29])
                EdtTekA00->Text=FloatToStrF((((float)TEK_TEMP1/100.0)-4.0)/1.6, ffFixed, 4, 1);
            else       EdtTekA00->Text="0,0";

            // номер тигля
          if ( tigelVPos )
          {
                if ( N_TIGEL )
                {
                    Num_Tig -> Caption = IntToStr( N_TIGEL );
                    EdtTekA01->Text=    IntToStr( N_TIGEL );
                }
                else
                {
                    Num_Tig -> Caption = "";
                    EdtTekA01->Text="0";
                }
          }
          else
          {
                Num_Tig -> Caption = "";
                EdtTekA01->Text="0";
          }

                                         
            //Номер плёнки
          //  EdtTekA02->Text== IntToStr(par[N_ST][5]);
            //Ток эмиссии
            EdtTekA03->Text= IntToStr(TEK_ELU/10);      //aik
            //Толщина     //aik
            EdtTekA04->Text= strTOLSH;
            //Время процесса
            EdtTekA05->Text=IntToStr(T_PROC);
        }
        else
        {
        EdtTekA00->Text="0,0";
        EdtTekA01->Text="0";
       // EdtTekA02->Text="0";
        EdtTekA03->Text="0,0";
        EdtTekA04->Text="0";
        EdtTekA05->Text="0";
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Отображение клавиш--//
void TForm1::VisualButtons()
{

    // Откачка камеры
    Pan_Otk      -> Font -> Color = SetPopeColor(shr[1]);
    Pan_Rc       -> Font -> Color = SetPopeColor(shr[3]);
    Pan_SbrRc    -> Font -> Color = SetPopeColor(shr[5]);
    Pan_Zagr     -> Font -> Color = SetPopeColor(shr[26]);
    Pan_Vgr      -> Font -> Color = SetPopeColor(shr[6]);
    Pan_Otkl     -> Font -> Color = SetPopeColor(shr[7]);
    Pan_Chil_On  -> Font -> Color = SetPopeColor(out[3]&0x80);
    Pan_Chil_Off -> Font -> Color = SetPopeColor(!(out[3]&0x80));


    if(shr[9])
    {   if(PR_TRTEST)
        {   Pan_Transp_On  -> Font -> Color = SetPopeColor(0);
            Pan_Transp_Off -> Font -> Color = SetPopeColor(1);
        }
        else
        {   Pan_Transp_On  -> Font -> Color = SetPopeColor(1);
            Pan_Transp_Off -> Font -> Color = SetPopeColor(0);
        }
    }
    else
    {   Pan_Transp_On  -> Font -> Color = SetPopeColor(0);
        Pan_Transp_Off -> Font -> Color = SetPopeColor(0);
    }
    Pan_SZ_Shl_Op-> Font        -> Color = SetPopeColor(shr[10]);
    Pan_SZ_Shl_Cl-> Font        -> Color = SetPopeColor(shr[11]);
    Pan_Za_Isp_Op-> Font        -> Color = SetPopeColor(shr[12]);
    Pan_Za_Isp_Off-> Font       -> Color = SetPopeColor(shr[13]);
    Pan_Nagr_Shl_On-> Font      -> Color = SetPopeColor(shr[27]);
    Pan_Nagr_Shl_Off-> Font     -> Color = SetPopeColor(shr[28]);
    Pan_Nagr_Pd_On-> Font       -> Color = SetPopeColor(shr[29]);
    Pan_Nagr_Pd_Off-> Font      -> Color = SetPopeColor(shr[30]);
    Pan_Chill_Ohl_On-> Font     -> Color = SetPopeColor(zin[1]&0x100);
    Pan_Chill_Ohl_Off-> Font    -> Color = SetPopeColor(!(zin[1]&0x100));
    Pan_Chill_Nagr_On-> Font    -> Color = SetPopeColor(out[3]&0x40);
    Pan_Chill_Nagr_Off-> Font   -> Color = SetPopeColor(!(out[3]&0x40));

    Pan_Vrash_Besk-> Font       -> Color = SetPopeColor(shr[24]);
    Pan_Vrash_Stop-> Font       -> Color = SetPopeColor(shr[31]);


    Pnl_Per_Gor_Home-> Font            -> Color = SetPopeColor(shr[16]);
    Pnl_Per_Gor_Start-> Font           -> Color = SetPopeColor(shr[17]);
    Pnl_Per_Vert_Home-> Font           -> Color = SetPopeColor(shr[18]);
    Pnl_Per_Vert_Start-> Font          -> Color = SetPopeColor(shr[19]);
    Pnl_Vr_Home-> Font                 -> Color = SetPopeColor(shr[20]);
    Pnl_Vr_Start-> Font                -> Color = SetPopeColor(shr[21]);
    Pnl_Kas_Home-> Font                -> Color = SetPopeColor(shr[14]);
    Pnl_Kas_Start-> Font               -> Color = SetPopeColor(shr[15]);
    Pnl_Pov_Home-> Font                -> Color = SetPopeColor(shr[22]);
    Pnl_Pov_Start-> Font               -> Color = SetPopeColor(shr[23]);

    Pan_Dat_1-> Font               -> Color = SetPopeColor(shr[32]&&(PR_PPD_D==1));
    Pan_Dat_2-> Font               -> Color = SetPopeColor(shr[32]&&(PR_PPD_D==2));
    Pan_Dat_3-> Font               -> Color = SetPopeColor(shr[32]&&(PR_PPD_D==3));
    Pan_Dat_4-> Font               -> Color = SetPopeColor(shr[32]&&(PR_PPD_D==4));
    Pan_Dat_5-> Font               -> Color = SetPopeColor(shr[32]&&(PR_PPD_D==5));
    Pan_Dat_6-> Font               -> Color = SetPopeColor(shr[32]&&(PR_PPD_D==6));

    Pan_Kam_Rasp-> Font               -> Color = SetPopeColor(shr[43]);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Визуализация отображения воды--//
void TForm1::VisualVoda()
{


 //Есть охлаждение камеры
if(zin[0]&0x01)
PnlKan01->Color=0x00EAD999;
else
PnlKan01->Color=0x003030FF;

 //Есть охлаждение шлюза
if(zin[0]&0x02)
PnlKan02->Color=0x00EAD999;
else
PnlKan02->Color=0x003030FF;

//Есть охлаждение Испарит
if(zin[0]&0x04)
PnlKan03->Color=0x00EAD999;
else
PnlKan03->Color=0x003030FF;

//Есть охлаждение Кв датч
if(zin[0]&0x08)
PnlKan04->Color=0x00EAD999;
else
PnlKan04->Color=0x003030FF;

//Есть охлаждение ТМН кам
if(zin[0]&0x10)
PnlKan05->Color=0x00EAD999;
else
PnlKan05->Color=0x003030FF;

//Есть охлаждение ТМН Шл
if(zin[0]&0x20)
PnlKan06->Color=0x00EAD999;
else
PnlKan06->Color=0x003030FF;

//Есть охлаждение ТМН Исп
if(zin[0]&0x40)
PnlKan07->Color=0x00EAD999;
else
PnlKan07->Color=0x003030FF;

//Есть охлаждение ФВН шл
if(zin[0]&0x80)
PnlKan08->Color=0x00EAD999;
else
PnlKan08->Color=0x003030FF;

//Есть охлаждение ФВН Исп
if(zin[0]&0x100)
PnlKan09->Color=0x00EAD999;
else
PnlKan09->Color=0x003030FF;

//Есть охлаждение резерв 1
if(zin[0]&0x200)
PnlKan010->Color=0x00EAD999;
else
PnlKan010->Color=0x003030FF;

//Есть охлаждение резерв 2
if(zin[0]&0x400)
PnlKan011->Color=0x00EAD999;
else
PnlKan011->Color=0x003030FF;

//Есть охлаждение резерв 3
if(zin[0]&0x800)
PnlKan012->Color=0x00EAD999;
else
PnlKan012->Color=0x003030FF;






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Температура воды охлаждения камеры
    PnlKan01 -> Caption = FloatToStrF((((float)aik[0]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[0]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan01 -> Caption = "0,0°C"; }
    // Температура воды охлаждения Шлюза
    PnlKan02 -> Caption = FloatToStrF((((float)aik[1]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[1]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan02 -> Caption = "0,0°C"; }
    // Температура воды охлаждения Испарит
    PnlKan03 -> Caption = FloatToStrF((((float)aik[2]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[2]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan03 -> Caption = "0,0°C"; }
    // Температура воды охлаждения Кв датч
    PnlKan04 -> Caption = FloatToStrF((((float)aik[3]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[3]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan04 -> Caption = "0,0°C"; }
    // Температура воды охлаждения ТМН кам
    PnlKan05 -> Caption = FloatToStrF((((float)aik[4]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[4]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan05 -> Caption = "0,0°C"; }
    // Температура воды охлаждения ТМН шл
    PnlKan06 -> Caption = FloatToStrF((((float)aik[5]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[5]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan06 -> Caption = "0,0°C"; }
    // Температура воды охлаждения ТМН исп
    PnlKan07 -> Caption = FloatToStrF((((float)aik[6]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[6]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan07 -> Caption = "0,0°C"; }
    // Температура воды охлаждения Фвн шл
    PnlKan08 -> Caption = FloatToStrF((((float)aik[7]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[7]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan08 -> Caption = "0,0°C"; }
    // Температура воды охлаждения Фвн исп
    PnlKan09 -> Caption = FloatToStrF((((float)aik[8]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[8]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan09 -> Caption = "0,0°C"; }
    // Температура воды охлаждения резерв 1
    PnlKan010 -> Caption = FloatToStrF((((float)aik[9]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[9]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan010 -> Caption = "0,0°C"; }
    // Температура воды охлаждения резерв 2
    PnlKan011 -> Caption = FloatToStrF((((float)aik[10]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[10]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan011 -> Caption = "0,0°C"; }
    // Температура воды охлаждения резерв 3
    PnlKan012 -> Caption = FloatToStrF((((float)aik[11]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[11]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan012 -> Caption = "0,0°C"; }

}
//---------------------------------------------------------------------------
//--Визуализация элементов мнемосхемы--//
//---------------------------------------------------------------------------
void TForm1::VisualColorElement()
{

        anim_fase = !anim_fase;
                // красный
    if((diagn[14])|| (diagn[24])|| (diagn[28]))
	{
		SetOut(1,2,0x4000); //
        SetOut(1,4,0x800);
	}
    else
	{
		SetOut(0,2,0x4000); //
        SetOut(0,4,0x800);
	}

    // желтый
    if((pr_yel)||(!shr[1]&&!shr[2]&&!shr[3]&&!shr[4]&&!shr[5]&&!shr[6]&&!shr[7]&&!shr[9]&&!shr[26]))
	{
		SetOut(1,2,0x2000); //
        SetOut(1,4,0x400);
	}
    else
	{
		SetOut(0,2,0x2000); //
        SetOut(0,4,0x400);
	}

    // зеленый
    if(shr[1]||shr[2]||shr[3]||shr[4]||shr[5]||shr[6]||shr[7]||shr[9]||shr[26])
	{
		SetOut(1,2,0x1000); //
        SetOut(1,4,0x200);
	}
    else
	{
		SetOut(0,2,0x1000); //
        SetOut(0,4,0x200);

	}
         if(shr[27])
            Nagr_shl->Visible=true;
         else
            Nagr_shl->Visible=false;
        //Клапана///////////////////////////////////////////////////////////////
        //ФК-Шл
        switch(zin[2]&0x03)
        {
            case 0x00:{Fk_Shl->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неопределено
            case 0x01:{Fk_Shl->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;   break;}//открыто
            case 0x02:{Fk_Shl->Picture->Bitmap=0;                               break;}//закрыто
            case 0x03:{Fk_Shl->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неоднозначно
        }
        //ФК-Шл (Мягк.)
        if(out[0]&0x02)Fk_Shl_M->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;
        else           Fk_Shl_M->Picture->Bitmap=0;
        //ФК-ТМН Шл
        switch(zin[2]&0x0C)
        {
            case 0x00:{Fk_Tmn_Shl->Picture->Bitmap=e_klg_n->Picture->Bitmap;   break;}//неопределено
            case 0x04:{Fk_Tmn_Shl->Picture->Bitmap=e_klg_o->Picture->Bitmap;   break;}//открыто
            case 0x08:{Fk_Tmn_Shl->Picture->Bitmap=0;                               break;}//закрыто
            case 0x0C:{Fk_Tmn_Shl->Picture->Bitmap=e_klg_n->Picture->Bitmap;   break;}//неоднозначно
        }
        //Затвор ТМН Шл
        switch(zin[2]&0x30)
        {
            case 0x00:{Zatv_Tmn_Shl->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неопределено
            case 0x10:{Zatv_Tmn_Shl->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;   break;}//открыто
            case 0x20:{Zatv_Tmn_Shl->Picture->Bitmap=0;                               break;}//закрыто
            case 0x30:{Zatv_Tmn_Shl->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неоднозначно
        }
        //ФК Исп
        switch(zin[2]&0x300)
        {
            case 0x000:{Fk_Isp->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неопределено
            case 0x100:{Fk_Isp->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;   break;}//открыто
            case 0x200:{Fk_Isp->Picture->Bitmap=0;                               break;}//закрыто
            case 0x300:{Fk_Isp->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неоднозначно
        }
        //ФК Исп (Мягк.)
        if(out[0]&0x200)Fk_Isp_M->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;
        else            Fk_Isp_M->Picture->Bitmap=0;
        //ФК-ТМН Исп
        switch(zin[2]&0xC00)
        {
            case 0x000:{Fk_Tmn_Isp->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неопределено
            case 0x400:{Fk_Tmn_Isp->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;   break;}//открыто
            case 0x800:{Fk_Tmn_Isp->Picture->Bitmap=0;                               break;}//закрыто
            case 0xC00:{Fk_Tmn_Isp->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неоднозначно
        }
        //Затвор ТМН Исп
        switch(zin[2]&0x3000)
        {
            case 0x0000:{Zatv_Tmn_Isp->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неопределено
            case 0x1000:{Zatv_Tmn_Isp->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;   break;}//открыто
            case 0x2000:{Zatv_Tmn_Isp->Picture->Bitmap=0;                               break;}//закрыто
            case 0x3000:{Zatv_Tmn_Isp->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неоднозначно
        }
        //ФК-ТМН кам
        switch(zin[3]&0x03)
        {
            case 0x00:{Fk_Tmn_Kam->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неопределено
            case 0x01:{Fk_Tmn_Kam->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;   break;}//открыто
            case 0x02:{Fk_Tmn_Kam->Picture->Bitmap=0;                               break;}//закрыто
            case 0x03:{Fk_Tmn_Kam->Picture->Bitmap=e_klg_n_vert->Picture->Bitmap;   break;}//неоднозначно
        }
        //ФК кам
        switch(zin[3]&0x0C)
        {
            case 0x00:{Fk_Kam->Picture->Bitmap=e_klg_n->Picture->Bitmap;   break;}//неопределено
            case 0x04:{Fk_Kam->Picture->Bitmap=e_klg_o->Picture->Bitmap;   break;}//открыто
            case 0x08:{Fk_Kam->Picture->Bitmap=0;                          break;}//закрыто
            case 0x0C:{Fk_Kam->Picture->Bitmap=e_klg_n->Picture->Bitmap;   break;}//неоднозначно
        }
        //ФК кам (Мягк.)
        if(out[3]&0x02)Fk_Kam_M->Picture->Bitmap=e_klg_o->Picture->Bitmap;
        else           Fk_Kam_M->Picture->Bitmap=0;
        //Кл-НАП1
        if(out[1]&0x01)Kl_Nap1->Picture->Bitmap=e_klg_o->Picture->Bitmap;
        else           Kl_Nap1->Picture->Bitmap=0;
        //Кл-НАП2
        if(out[1]&0x04)Kl_Nap2->Picture->Bitmap=e_klg_o->Picture->Bitmap;
        else           Kl_Nap2->Picture->Bitmap=0;
        //Кл-НАП3
        if(out[1]&0x02)Kl_Nap3->Picture->Bitmap=e_klg_o->Picture->Bitmap;
        else           Kl_Nap3->Picture->Bitmap=0;
        //Кл подачи газа
        if(out[4]&0x2000)Image13->Picture->Bitmap=e_klg_o_vert->Picture->Bitmap;
        else           Image13->Picture->Bitmap=0;
        //Трубы/////////////////////////////////////////////////////////////////
        //1
        if((out[1]&0x01)||((t_2->Visible)&&((zin[2]&0x03)==0x01))||((t_2->Visible)&&(out[0]&0x02)))
            t_1->Visible=true;
        else
            t_1->Visible=false;
        //2
        if(((t_1->Visible)&&((zin[2]&0x03)==0x01))||((t_1->Visible)&&(out[0]&0x02))||(zin[1]&0x01))
            t_2->Visible=true;
        else
            t_2->Visible=false;
        //3
        if(((zin[2]&0x30)==0x10)&&(t_4->Visible))
            t_3->Visible=true;
        else
            t_3->Visible=false;
        //4
        if((out[5]&0x02)/*&&(t_5->Visible)*/)
            t_4->Visible=true;
        else
            t_4->Visible=false;
        //5
        if(((zin[2]&0x0C)==0x04)&&(t_2->Visible))
            t_5->Visible=true;
        else
            t_5->Visible=false;
        //6
        if(((zin[2]&0x3000)==0x1000)&&(t_7->Visible))
            t_6->Visible=true;
        else
            t_6->Visible=false;
        //7
        if((out[5]&0x08)/*&&(t_8->Visible)*/)
            t_7->Visible=true;
        else
            t_7->Visible=false;
        //8
        if(((zin[2]&0xC00)==0x400)&&(t_14->Visible))
            t_8->Visible=true;
        else
            t_8->Visible=false;
        //9
        if((out[1]&0x04)||((out[0]&0x0200)&&(t_14->Visible))||(((zin[2]&0x300)==0x100)&&(t_14->Visible)))
            t_9->Visible=true;
        else
            t_9->Visible=false;
        //10
        if(out[4]&0x2000)
            t_10->Visible=true;
        else
            t_10->Visible=false;
        //11
        if((zin[5]&0x07)/*&&((t_10->Visible)||(t_12->Visible))*/)
            t_11->Visible=true;
        else
            t_11->Visible=false;
        //12
        if(((zin[3]&0x03)==0x01)&&(t_14->Visible))
            t_12->Visible=true;
        else
            t_12->Visible=false;
        //13
        if((out[1]&0x02)||(((zin[3]&0xC0)==0x40)&&(t_14->Visible))||((out[3]&0x02)&&(t_14->Visible)))
            t_13->Visible=true;
        else
            t_13->Visible=false;
        //14
        if(
        (zin[1]&0x08)||
        ((out[3]&0x02)&&(t_13->Visible))||
        (((zin[3]&0xC0)==0x40)&&(t_13->Visible))||
        (((zin[2]&0x300)==0x100)&&(t_9->Visible))||
        ((out[0]&0x0200)&&(t_9->Visible))
        )
            t_14->Visible=true;
        else
            t_14->Visible=false;
        //Механизмы/////////////////////////////////////////////////////////////
        
        if((zin[3]&0x300)==0x100)
            kr_gor->Visible=true;
        else
            kr_gor->Visible=false;
        if(zin[4]&0x10)
            kr_gor->Picture->Bitmap=Image26->Picture->Bitmap;
        else
            kr_gor->Picture->Bitmap=Image25->Picture->Bitmap;
        //привод горизонтального перемещения
        if(zin[4]&0x10)
            Meh_Gor->Color=clWhite;
        else
            Meh_Gor->Color=0x0064F046;
        
        if(zin[4]&0x400)
            kr_vert->Picture->Bitmap=Image28->Picture->Bitmap;
        else
            kr_vert->Picture->Bitmap=Image27->Picture->Bitmap;
        //Привод вертикального перемещения
        if(zin[4]&0x400)
            Meh_Vert->Color=clWhite;
        else
            Meh_Vert->Color=0x0064F046;
        //Кассета
        if(zin[4]&0x80)
            Kas->Picture->Bitmap=Image21->Picture->Bitmap;
        else
            Kas->Picture->Bitmap=Image20->Picture->Bitmap;
        //Дверь шлюза
        if(!(zin[0]&0x2000))
            door_shl->Visible=true;
        else
            door_shl->Visible=false;
        //ЩЗ камеры - шлюза
        switch (zin[3]&0x30)
        {
            case 0x00:{ShZatvKam->Picture->Bitmap=Image34->Picture->Bitmap; break;}//неопределено
            case 0x10:{ShZatvKam->Picture->Bitmap=0;                        break;}//открыто
            case 0x20:{ShZatvKam->Picture->Bitmap=Image33->Picture->Bitmap; break;}//закрыто
            case 0x30:{ShZatvKam->Picture->Bitmap=Image34->Picture->Bitmap; break;}//неоднозначно
        }
        //ЩЗ испарителя
        switch (zin[3]&0xC0)
        {
            case 0x00:{ShZatvWork->Picture->Bitmap=Image37->Picture->Bitmap; break;}//неопределено
            case 0x40:{ShZatvWork->Picture->Bitmap=0;                        break;}//открыто
            case 0x80:{ShZatvWork->Picture->Bitmap=Image36->Picture->Bitmap; break;}//закрыто
            case 0xC0:{ShZatvWork->Picture->Bitmap=Image37->Picture->Bitmap; break;}//неоднозначно
        }
        //заслонка камеры
        switch (zin[2]&0xC0)
        {
            case 0x00:{ZaslKam->Picture->Bitmap=Image1->Picture->Bitmap; break;}//неопределено
            case 0x40:{ZaslKam->Picture->Bitmap=0;                        break;}//открыто
            case 0x80:{ZaslKam->Picture->Bitmap=Image4->Picture->Bitmap; break;}//закрыто
            case 0xC0:{ZaslKam->Picture->Bitmap=Image1->Picture->Bitmap; break;}//неоднозначно
        }
        //Экран затвора испарителя
        switch (zin[2]&0xC000)
        {
            case 0x0000:{pp->Top=391;pp->Picture->Bitmap=Image8->Picture->Bitmap; break;}//неопределено
            case 0x4000:{pp->Top=351;pp->Picture->Bitmap=Image9->Picture->Bitmap; break;}//открыто
            case 0x8000:{pp->Top=391;pp->Picture->Bitmap=Image9->Picture->Bitmap; break;}//закрыто
            case 0xC000:{pp->Top=391;pp->Picture->Bitmap=Image8->Picture->Bitmap; break;}//неоднозначно
        }
        //Заслонка испарителя
        switch (zin[1]&0xC0)
        {
            case 0x00:{Zasl_Isp->Left=794;Zasl_Isp->Picture->Bitmap=Image5->Picture->Bitmap; break;}//неопределено
            case 0x40:{Zasl_Isp->Left=824;Zasl_Isp->Picture->Bitmap=Image7->Picture->Bitmap;  break;}//открыто
            case 0x80:{Zasl_Isp->Left=784;Zasl_Isp->Picture->Bitmap=Image6->Picture->Bitmap;  break;}//закрыто
            case 0xC0:{Zasl_Isp->Left=794;Zasl_Isp->Picture->Bitmap=Image5->Picture->Bitmap;  break;}//неоднозначно
        }
        //Номер датчика
        if((TEK_ABS_PPD>=(-5))&&(TEK_ABS_PPD<=(5)))
            Num_Dat->Caption="1";
        else if ((TEK_ABS_PPD>=((IMP60*(2-1))-5))&&(TEK_ABS_PPD<=((IMP60*(2-1))+5)))
            Num_Dat->Caption="2";
        else if ((TEK_ABS_PPD>=((IMP60*(3-1))-5))&&(TEK_ABS_PPD<=((IMP60*(3-1))+5)))
            Num_Dat->Caption="3";
        else if ((TEK_ABS_PPD>=((IMP60*(4-1))-5))&&(TEK_ABS_PPD<=((IMP60*(4-1))+5)))
            Num_Dat->Caption="4";
        else if ((TEK_ABS_PPD>=((IMP60*(5-1))-5))&&(TEK_ABS_PPD<=((IMP60*(5-1))+5)))
            Num_Dat->Caption="5";
        else if ((TEK_ABS_PPD>=((IMP60*(6-1))-5))&&(TEK_ABS_PPD<=((IMP60*(6-1))+5)))
            Num_Dat->Caption="6";
        else
            Num_Dat->Caption="-";
        //Вращение испарителя
        //Вращение п/д
        if(PR_VR)
            Vr_Pd->Visible=(bool)anim_fase;
        else
            Vr_Pd->Visible=false;
        if(zin[4]&0x02)
            home_vr->Visible=true;
        else
            home_vr->Visible=false;
        //поворот датчиков
        if(out[2]&0x100)
            Vr_Dat->Visible=(bool)anim_fase;
        else
            Vr_Dat->Visible=false;
        //лампа нагрева
        if(shr[29])
            lamp1->Visible=true;
        else
            lamp1->Visible=false;
        //ТМН кам
        if(!(zin[5]&0x08))//авария
        {
            Tmn_Kam-> Picture->Bitmap =tmn_red-> Picture->Bitmap;
            str_up->Visible=false;
            str_down->Visible=false;
        }
        else if(!(zin[5]&0x10))//предупреждение
        {
            Tmn_Kam-> Picture->Bitmap =tmn_yellow-> Picture->Bitmap;
            str_up->Visible=false;
            str_down->Visible=false;
        }
        else if(out[5]&0x10)//включён
        {
            if(zin[5]&0x04)//норма
            {
                str_down->Visible=false;
                str_up->Visible=false;
                Tmn_Kam-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
            }
            else //разгон
            {
                str_down->Visible=false;
                str_up->Visible=true;
                if(anim_fase)Tmn_Kam-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
                else         Tmn_Kam-> Picture->Bitmap =0;
            }

        }
        else if(zin[5]&0x02)//торможение
            {
                str_down->Visible=true;
                str_up->Visible=false;
                if(anim_fase)Tmn_Kam-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
                else         Tmn_Kam-> Picture->Bitmap =0;
            }
        else
        {
            Tmn_Kam-> Picture->Bitmap =0;
            str_up->Visible=false;
            str_down->Visible=false;
        }
        //ТМН Шл
        if(!(zin[5]&0x200))      //авария
        {
            Tmn_Shl-> Picture->Bitmap =tmn_red-> Picture->Bitmap;
            str_up_shl      ->Visible=false;
            str_down_shl    ->Visible=false;
        }
        else if (out[5]&0x02) //работа
        {

            if(zin[5]&0x100)//норма
            {
                Tmn_Shl-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
                str_up_shl      ->Visible=false;
                str_down_shl    ->Visible=false;
            }
            else
            {
                if(anim_fase)
                    Tmn_Shl-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
                else
                    Tmn_Shl-> Picture->Bitmap =0;
                if(PR_OST_TMN_ISP)
                {
                    str_up_shl      ->Visible=false;
                    str_down_shl    ->Visible=true;
                }
                else
                {
                    str_up_shl      ->Visible=true;
                    str_down_shl    ->Visible=false;
                }
            }
        }
        else
        {
            Tmn_Shl-> Picture->Bitmap =0;
            str_up_shl      ->Visible=false;
            str_down_shl    ->Visible=false;
        }
        //ТМН исп
        if(!(zin[5]&0x800))      //авария
        {
            Tmn_Isp-> Picture->Bitmap =tmn_red-> Picture->Bitmap;
            str_up_isp      ->Visible=false;
            str_down_isp    ->Visible=false;
        }
        else if (out[5]&0x08) //работа
        {

            if(zin[5]&0x400)//норма
            {
                Tmn_Isp-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
                str_up_isp      ->Visible=false;
                str_down_isp    ->Visible=false;
            }
            else
            {
                if(anim_fase)
                    Tmn_Isp-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
                else
                    Tmn_Isp-> Picture->Bitmap =0;
                if(PR_OST_TMN_ISP)
                {
                    str_up_isp      ->Visible=false;
                    str_down_isp    ->Visible=true;
                }
                else
                {
                    str_up_isp      ->Visible=true;
                    str_down_isp    ->Visible=false;
                }
            }

        }
        else
        {
            Tmn_Isp-> Picture->Bitmap =0;
            str_up_isp      ->Visible=false;
            str_down_isp    ->Visible=false;
        }

        //ФВН камеры
        if(!(zin[1]&0x20))//авария
            Fvn_Kam-> Picture->Bitmap =FN_red-> Picture->Bitmap;
        else if(!(zin[1]&0x10))//предупреждение
            Fvn_Kam-> Picture->Bitmap =FN_yellow-> Picture->Bitmap;
        else if(zin[1]&0x08)//работает
            Fvn_Kam-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
        else
            Fvn_Kam-> Picture->Bitmap =0;
        //ФВН шлюза
        if(!(zin[1]&0x04))//авария
            Fvn_Shl-> Picture->Bitmap =FN_red-> Picture->Bitmap;
        else if(!(zin[1]&0x02))//предупреждение
            Fvn_Shl-> Picture->Bitmap =FN_yellow-> Picture->Bitmap;
        else if(zin[1]&0x01)//работает
            Fvn_Shl-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
        else
            Fvn_Shl-> Picture->Bitmap =0;
        //испаритель
        if(zin[3]&0x400)
        {
            Door_Isp->Visible=true;
            Isp_Work->Visible=true;
            Num_Tig ->Visible=true;
         //   Vr_Isp  ->Visible=true;
        }
        else
        {
            Door_Isp->Visible=false;
            Isp_Work->Visible=false;
            Num_Tig ->Visible=false;
          //  Vr_Isp  ->Visible=false;
        }
        if(VRELU&&(zin[1]&0x40))
            anim->Visible=(bool)anim_fase;
        else
            anim->Visible=false;

}

//---------------------------------------------------------------------------
//--Управление дискретным сигналом--//
//---------------------------------------------------------------------------
void SetOut(bool value, unsigned char outNmb, unsigned int outMask)
{   // установить значение группы дискретов
    value ? out[outNmb] |= outMask : out[outNmb] &= (~outMask);
}
//---------------------------------------------------------------------------
//--Задание аналогового выхода--//
//------------------------------+---------------------------------------------
void A_OUT(unsigned int Nmb,unsigned int Value)
{ // изменить выходные дискретные сигналы
  aout[Nmb] = Value;
  aoutKon[Nmb] = Value;
}
//---------------------------------------------------------------------------
void SetZin(bool value, unsigned char outNmb, unsigned int outMask)
{   // установить значение группы дискретов
    value ? zin[outNmb] |= outMask : zin[outNmb] &= (~outMask);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Отработка кнопок формата--//
void __fastcall TForm1::SetOutClick(TObject *Sender)
{   // Установка OUTов
    unsigned char bitNmb  = PCFormatOut -> TabIndex;
    unsigned int  bitMask = StrToInt(((TButton*)Sender)->Hint);
    // Выставить сигнал
    out[bitNmb] & bitMask ? SetOut(0, bitNmb, bitMask) : SetOut(1, bitNmb, bitMask);
    // Перерисовать формат
    //VisualFormat();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::ZeroAoutClick(TObject *Sender)
{ // Установка в ноль аналогового выхода
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
        // Установка OUTов
        unsigned char bitNmb = PCFormatZin -> TabIndex;
        unsigned int  bitMask = StrToInt(((TButton*)Sender)->Hint);
        // Выставить сигнал
        zin[bitNmb] & bitMask ? SetZin(0, bitNmb, bitMask) : SetZin(1, bitNmb, bitMask);
        // Перерисовать формат
        VisualFormat();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::SetAoutChange(TObject *Sender)
{ // Изменение установки аналогового выхода
  unsigned int
   bitNmb = StrToInt(((TImage*)Sender)->Hint),
   i = bitNmb / 4,
   j = bitNmb % 4;
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   Dec_Aout_zad[i][j] -> Text = FloatToStrF(float(((TScrollBar*)Sender)->Position), ffFixed, 5, 0);
   // Пересчитать текущее значение аналоговой установки
   if(i*4+j==7)
        UV_Aout_zad[i][j] -> Text = FloatToStrF(float(((TScrollBar*)Sender)->Position-8192) * 10.0 / 8191.0, ffFixed, 5, 2);
   else
        UV_Aout_zad[i][j] -> Text = FloatToStrF(float(((TScrollBar*)Sender)->Position-8192) * 10.0 / 8191.0, ffFixed, 5, 3);
   // Перерисовать визуальное отображение
   CG_Aout_zad[i][j] -> Progress = Zad_Aout[i][j] -> Position;
   // Перерисовать формат
   //VisualFormat();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __fastcall TForm1::EntAoutClick(TObject *Sender)
{ // Установка аналогового входа
  unsigned int
   bitNmb = StrToInt(((TImage*)Sender)->Hint),
   i = bitNmb / 4,
   j = bitNmb % 4;
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   A_OUT(bitNmb, Zad_Aout[i][j] -> Position);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_250Timer(TObject *Sender)
{
    if(!ust_ready) return;
   VisualFormat();
   VisualDebug();
   ParamTransCheck();
}
//---------------------------------------------------------------------------
//--Визуализация параметров автомата--//                                                         FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffExponent,3,8);
//---------------------------------------------------------------------------
void TForm1::VisualParA()
{
//N_ST=1**********************************************************************************************************
    EdtAKon1_1 -> Text =FloatToStrF((((((float)par[1][1])/100)-4)/1.6), ffFixed, 5, 1);       //Ток нагрева
    EdtAKon1_2 -> Text =FloatToStrF((float)par[1][2], ffFixed, 5, 0);                     //время процесса
//N_ST=2:101**********************************************************************************************************
    EdtAKonN_3 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][3], ffFixed, 5, 0);                     //номер процесса
    EdtAKonN_4 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][4], ffFixed, 5, 0);                     //номер тигля
    EdtAKonN_8 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][8], ffFixed, 5, 0);                     //номер датчика толщины
    EdtAKonN_5 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][5], ffFixed, 5, 0);                     //номер плёнки
    EdtAKonN_7 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][7], ffFixed, 5, 0);                     //время нарастания тока эмиссии
    EdtAKonN_6 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][6]/10.0, ffFixed, 5, 1);                     //Ток эмиссии
    EdtAKonN_2 -> Text =FloatToStrF((float)par[CB_StChange->ItemIndex+2][2], ffFixed, 5, 0);                     //время напыления

}
//---------------------------------------------------------------------------
//--Передача параметров автомата--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParAClick(TObject *Sender)
{
       PanelParA -> Visible = true;
}
//---------------------------------------------------------------------------
//--Отказ от передачи параметров ручных--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnAutoNetClick(TObject *Sender)
{
    PanelParA -> Visible = false;
}
//---------------------------------------------------------------------------
//--Подтверждение передачи параметров автомата--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnAutoDaClick(TObject *Sender)
{
    // панель подтверждения отправки убрать
    PanelParA -> Visible = false;
    //N_ST=1
    par[1][1]= int(((StrToFloat(EdtARed1_1->Text)*1.6) + 4.0)*100.0);
    par[1][2]= StrToInt    ( EdtARed1_2->Text );                         //Время процесса
    //N_ST=2:101
    for(int i =0;i<100;i++)
    {
        par[i+2][3]=StrToInt(MemoDin->Lines->Strings[i*7+0]);
        par[i+2][4]=StrToInt(MemoDin->Lines->Strings[i*7+1]);
        par[i+2][8]=StrToInt(MemoDin->Lines->Strings[i*7+2]);
        par[i+2][5]=StrToInt(MemoDin->Lines->Strings[i*7+3]);
        par[i+2][7]=StrToInt(MemoDin->Lines->Strings[i*7+4]);
        par[i+2][6]=StrToFloat(MemoDin->Lines->Strings[i*7+5])*10;
        par[i+2][2]=StrToInt(MemoDin->Lines->Strings[i*7+6]);
    }

    // обновить страницу
    VisualParA();

    ParamTrans();
}

//---------------------------------------------------------------------------
//--Изменение параметров автомата--//
//---------------------------------------------------------------------------
void __fastcall TForm1::EdtARed1Exit(TObject *Sender)
{

// заменить точку на запятую
    AnsiString
        text = ((TEdit*)Sender)->Text;
    for ( unsigned char i = 1 ; i < text.Length(); i++)
        if (text[i] == '.') text[i] = ',';
    unsigned char
        format; // кол-во знаков после запятой
    float
        valueText = StrToFloat(text);
    // окрасить ячейку
    ((TEdit*)Sender)->Color = clSilver;

//Ток нагрева
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_1" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_1" )
        )
    {
        // кол-во знаков после запятой 0
        format = 1;
        // проверили на минимум
        if (valueText < 1)
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 7)
        {
            valueText = 7;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Время нагрева
    if  (
            (((TEdit*)Sender)->Name == "EdtARed1_2" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 1200)
        {
            valueText = 1200;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Номер процесса
    if  (
            (((TEdit*)Sender)->Name == "EdtARedN_3" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 1)
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 20)
        {
            valueText = 20;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Номер тигля
    if  (
           ( ((TEdit*)Sender)->Name == "EdtARedN_4" )||
           ( ((TEdit*)Sender)->Name == "EdtARedN_8" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 1)
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 6)
        {
            valueText = 6;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Номер плёнки
    if  (
            (((TEdit*)Sender)->Name == "EdtARedN_5" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if ((valueText > 0)&&(valueText < 1))
        {
            valueText = 1;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > 99)
        {
            valueText = 99;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Время нарастания тока эмиссии
    if  (
            (((TEdit*)Sender)->Name == "EdtARedN_7" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 5)
        {
            valueText = 5;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 300)
        {
            valueText = 300;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Ток эмиссии
    if  (
            (((TEdit*)Sender)->Name == "EdtARedN_6" )
        )
    {
        // кол-во знаков после запятой 0
        format = 1;
        // проверили на минимум
        if (valueText < 3)
        {
            valueText = 3;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 1000)
        {
            valueText = 1000;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Время напыления
    if  (
            (((TEdit*)Sender)->Name == "EdtARedN_2" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }

    }
////Настройка///////////////////////////////////////////////////////////////////
//количество циклов промывки
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo3" )||
            (((TEdit*)Sender)->Name == "EditNastrTo4" )||
            (((TEdit*)Sender)->Name == "EditNastrTo5" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 5)
        {
            valueText = 5;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//Температура прогрева шлюза
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo7" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if ((valueText > 0)&&(valueText < 30))
        {
            valueText = 30;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
//задержка на окрытие заслонки тигеля
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo8" )||
            (((TEdit*)Sender)->Name == "EditNastrTo9" )||
            (((TEdit*)Sender)->Name == "EditNastrTo10" )||
            (((TEdit*)Sender)->Name == "EditNastrTo11" )||
            (((TEdit*)Sender)->Name == "EditNastrTo12" )||
            (((TEdit*)Sender)->Name == "EditNastrTo13" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 30)
        {
            valueText = 30;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    //Время открытия заслонки смотрового окна
    if  (
            (((TEdit*)Sender)->Name == "EditNastrTo15" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < 5)
        {
            valueText = 5;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 300)
        {
            valueText = 300;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
	// Пути манипулятора
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_9" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_10" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_11" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_12" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_13" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < -4000000.0)
        {
            valueText = -4000000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 4000000.0)
        {
            valueText = 4000000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // Пути манипулятора
    if  (
            (((TEdit*)Sender)->Name == "EdtTRed1" )||
            (((TEdit*)Sender)->Name == "EdtTRed2" )||
            (((TEdit*)Sender)->Name == "EdtTRed3" )||
            (((TEdit*)Sender)->Name == "EdtTRed4" )||
            (((TEdit*)Sender)->Name == "EdtTRed5" )||
            (((TEdit*)Sender)->Name == "EdtTRed6" )||
            (((TEdit*)Sender)->Name == "EdtTRed7" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < -2000000.0)
        {
            valueText = -2000000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 2000000.0)
        {
            valueText = 2000000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // проверка на изменение
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 8, format);

//------------------------------------------------------------------------------
    //обработка цветов массива параметров
    if  (
            (((TEdit*)Sender)->Name == "EdtARedN_3" )||
            (((TEdit*)Sender)->Name == "EdtARedN_4" )||
            (((TEdit*)Sender)->Name == "EdtARedN_8" )||
            (((TEdit*)Sender)->Name == "EdtARedN_5" )||
            (((TEdit*)Sender)->Name == "EdtARedN_7" )||
            (((TEdit*)Sender)->Name == "EdtARedN_6" )||
            (((TEdit*)Sender)->Name == "EdtARedN_2" )
        )
    {

        MemoDin->Lines->Strings[CB_StChange->ItemIndex*7+StrToInt(((TEdit*)Sender)->Hint)]=((TEdit*)Sender)->Text;

        if(((TEdit*)Sender)->Color==clYellow)
        {
            MemoCol->Lines->Strings[CB_StChange->ItemIndex*7+StrToInt(((TEdit*)Sender)->Hint)]=1;
        }
        else if(((TEdit*)Sender)->Color==clSilver)
        {
            MemoCol->Lines->Strings[CB_StChange->ItemIndex*7+StrToInt(((TEdit*)Sender)->Hint)]=2;
        }
        else
        {
            MemoCol->Lines->Strings[CB_StChange->ItemIndex*7+StrToInt(((TEdit*)Sender)->Hint)]=0;
        }
    }
//------------------------------------------------------------------------------

    VisualParA();
    

}

//---------------------------------------------------------------------------
//--Передача параметров ручного режима--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParRClick(TObject *Sender)
{
    PanelParR -> Visible = true;
}
//---------------------------------------------------------------------------
//--Отказ от передачи параметров ручных--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnRNetClick(TObject *Sender)
{
    // панель подтверждения отправки убрать
    PanelParR -> Visible = false;
}
//---------------------------------------------------------------------------
//--Подтверждение передачи параметров ручного--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnRDaClick(TObject *Sender)
{

    // панель подтверждения отправки убрать
    PanelParR -> Visible = false;
    //  Ток нагрева п/д
    par[0][1] =int(((StrToFloat(EdtRRed0_1->Text)*1.6) + 4.0)*100.0);
    //  Привод горизонтального перемещения
    par[0][9] =StrToInt  ( EdtRRed0_9->Text );
    //  Привод вертикального перемещения
    par[0][10] =StrToInt  ( EdtRRed0_10->Text );
    //  Вращение подложкодержателя
    par[0][11] =StrToInt  ( EdtRRed0_11->Text );
    //  Кассета
    par[0][12] =StrToInt  ( EdtRRed0_12->Text );
    //  Поворот датчика толщины
    par[0][13] =StrToInt  ( EdtRRed0_13->Text );
    //  Скорость
    if(EdtRRed0_14 -> ItemIndex == 0)       { par[0][14]=0; }
    else if(EdtRRed0_14 -> ItemIndex == 1)  { par[0][14]=1; }
    else if(EdtRRed0_14 -> ItemIndex == 2)  { par[0][14]=2; }




    MemoStat -> Lines -> Add("");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "Переданы наладочные параметры:");
    MemoStat -> Lines -> Add("");

    if ( EdtRKon0_1 -> Text != EdtRRed0_1 -> Text )
        MemoStat -> Lines -> Add("Ток нагрева п/д: " + EdtRKon0_1 -> Text + " -> " + EdtRRed0_1 -> Text );
    if ( EdtRKon0_9 -> Text != EdtRRed0_9 -> Text )
        MemoStat -> Lines -> Add("Привод горизонтального перемещения: " + EdtRKon0_9 -> Text + " -> " + EdtRRed0_9 -> Text );
    if ( EdtRKon0_10 -> Text != EdtRRed0_10 -> Text )
        MemoStat -> Lines -> Add("Привод вертикального перемещения: " + EdtRKon0_10 -> Text + " -> " + EdtRRed0_10 -> Text );
    if ( EdtRKon0_11 -> Text != EdtRRed0_11 -> Text )
        MemoStat -> Lines -> Add("Вращение подложкодержателя: " + EdtRKon0_11 -> Text + " -> " + EdtRRed0_11 -> Text );
    if ( EdtRKon0_12 -> Text != EdtRRed0_12 -> Text )
        MemoStat -> Lines -> Add("Кассета: " + EdtRKon0_12 -> Text + " -> " + EdtRRed0_12 -> Text );
    if ( EdtRKon0_13 -> Text != EdtRRed0_13 -> Text )
        MemoStat -> Lines -> Add("Поворот датчика толщины: " + EdtRKon0_13 -> Text + " -> " + EdtRRed0_13 -> Text );
    if ( EdtRKon0_14 -> Text != EdtRRed0_14 -> Text )
        MemoStat -> Lines -> Add("Скорость: " + EdtRKon0_14 -> Text + " -> " + EdtRRed0_14 -> Text );



    // перекрасить переданные параметры

    EdtRRed0_1 -> Color = clWhite;
    EdtRRed0_9 -> Color = clWhite;
    EdtRRed0_10 -> Color = clWhite;
    EdtRRed0_11 -> Color = clWhite;
    EdtRRed0_12 -> Color = clWhite;
    EdtRRed0_13 -> Color = clWhite;
    EdtRRed0_14 -> Color = clWhite;

    // обновить страницу
    VisualParR();

}
//---------------------------------------------------------------------------
//--Выбор файла из архива параметров--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryClick(TObject *Sender)
{

libNmb = ListBoxLibrary -> ItemIndex;
// если выбрали существующую программу
if ( libNmb != -1 )
{
AnsiString fName = "LIB\\" + ListBoxLibrary->Items->operator[](libNmb) + ".txt";
if ( FileExists ( fName ) )
{
MemoLib -> Lines -> LoadFromFile( fName );
LblPresentName -> Caption = "Предыдущее имя: " + ListBoxLibrary->Items->operator[](libNmb);
// поместили в параметры редакции библиотечный массив для стадий
pr_lib=1;
//N_ST=1
EdtALib1_1 -> Text = MemoLib -> Lines -> operator [](0);
EdtALib1_2 -> Text = MemoLib -> Lines -> operator [](1);
//N_ST=2
EdtALibN_3-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+0+2);
EdtALibN_4-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+1+2);
EdtALibN_8-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+2+2);
EdtALibN_5-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+3+2);
EdtALibN_7-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+4+2);
EdtALibN_6-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+5+2);
EdtALibN_2-> Text =MemoLib -> Lines -> operator [](CB_StChange->ItemIndex*7+6+2);





BitBtnLoad -> Enabled = true;
}
else
{
// поместили в параметры редакции библиотечный массив для стадий
BitBtnLoad -> Enabled = false;
LblPresentName -> Caption = "Предыдущее имя: ";
}
}

}
//---------------------------------------------------------------------------
//--Выбрали несуществующую программу--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryMouseDown(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
libNmb = -1;
}
//---------------------------------------------------------------------------
//--Выбор библиотеки из списка--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxLibraryMouseUp(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{

// если выбрали несуществующую программу
if ( libNmb == -1 )
{
// клавиша чтения из библиотеки недоступна
BitBtnLoad -> Enabled = false;
// предыдушего наименования соответсвенно не выбрано
LblPresentName -> Caption = "Предыдущее имя: ";
// убрать фокус с предыдущей записи программы
ListBoxLibrary -> ItemIndex = -1;
                   
 pr_lib=0;
// очистить массив библиотечных параметров
EdtALib1_1 -> Text = "";
EdtALib1_2 -> Text = "";

EdtALibN_3 -> Text = "";
EdtALibN_4 -> Text = "";
EdtALibN_5 -> Text = "";
EdtALibN_7 -> Text = "";
EdtALibN_6 -> Text = "";
EdtALibN_2 -> Text = "";



}

}
//---------------------------------------------------------------------------
//--Запись в библиотеку--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnSaveClick(TObject *Sender)
{

MemoLib -> Lines -> Clear();
//N_ST=1
    MemoLib -> Lines -> Add ( EdtARed1_1 -> Text );

    MemoLib -> Lines -> Add ( EdtARed1_2 -> Text );
//N_ST=2:101
    for(int i=0;i<700;i++)
    {
        MemoLib -> Lines -> Add (MemoDin->Lines->Strings[i]);
    }



// отображение диалогового окна
GBSaveDialog -> Visible = true;

}
//---------------------------------------------------------------------------
//--Чтение из библиотеки--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnLoadClick(TObject *Sender)
{             
//N_ST=1
EdtARed1_1 -> Text = EdtALib1_1 -> Text;
EdtARed1_1 -> Color = clSilver;
EdtARed1_2 -> Text = EdtALib1_2 -> Text;
EdtARed1_2 -> Color = clSilver;       
//N_ST=2:101
MemoDin->Lines=MemoLib->Lines;
MemoDin->Lines->Delete(0);
MemoDin->Lines->Delete(0);
MemoCol->Lines=MemoColReserv->Lines;
CB_StChangeChange(CB_StChange);



}
//---------------------------------------------------------------------------
//--Отменить сохранение параметров в библиотеку--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnSaveNoClick(TObject *Sender)
{
EdtNewName -> Text = "";
GBSaveDialog -> Visible = false;
}
//---------------------------------------------------------------------------
//--Подтверждение сохранения параметров в библиотеку--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnSaveYesClick(TObject *Sender)
{
// Проверка на существование директорий
if ( ! DirectoryExists("LIB") ) CreateDir("LIB");
AnsiString fileName = "LIB\\" + EdtNewName -> Text + ".txt";
if ( ! FileExists( fileName ) )
{
// подтверждать удаление предыдущего файла
if ( MessageDlg("Подтверждаете запись программы?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
// если вводится программа взамен существующей
if ( libNmb != -1 )
DeleteFileA( "LIB\\" + ListBoxLibrary -> Items -> operator[](libNmb) + ".txt" );
int fileID = FileCreate ( fileName );
FileClose( fileID );
}
else
{
if ( MessageDlg("Файл с заданным названием уже существует, перезаписать?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrNo ) return;
}
MemoLib -> Lines -> SaveToFile( "LIB\\" + EdtNewName -> Text + ".txt" );
GBSaveDialog -> Visible = false;

// вывод архивных файлов
int
fileCount = 0,
rezult;
TSearchRec SR;
// библиотеки
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
//--Визуализация параметров ручного--//
//---------------------------------------------------------------------------
void TForm1::VisualParR()
{
    EdtRKon0_1 -> Text  =FloatToStrF((((((float)par[0][1])/100)-4)/1.6), ffFixed, 5, 1); //  Ток нагрева п/д
    EdtRKon0_9 -> Text  =FloatToStrF((float)par[0][9], ffFixed, 8, 0);                   //  Привод горизонтального перемещения
    EdtRKon0_10 -> Text =FloatToStrF((float)par[0][10], ffFixed, 8, 0);                  //  Привод вертикального перемещения
    EdtRKon0_11 -> Text =FloatToStrF((float)par[0][11], ffFixed, 8, 0);                  //  Вращение подложкодержателя
    EdtRKon0_12 -> Text =FloatToStrF((float)par[0][12], ffFixed, 8, 0);                  //  Кассета
    EdtRKon0_13 -> Text =FloatToStrF((float)par[0][13], ffFixed, 8, 0);                  //  Поворот датчика толщины
    if(par[0][14]==0)       { EdtRKon0_14 -> Text ="Большая"; }                          //  Скорость
    else if(par[0][14]==1)  { EdtRKon0_14 -> Text ="Малая"; }
    else if(par[0][14]==2)  { EdtRKon0_14 -> Text ="Ползущая";}




}
//---------------------------------------------------------------------------
//--Визуализация страницы отладки--//
//---------------------------------------------------------------------------
void __fastcall TForm1::PCPeremChange(TObject *Sender)
{

    // тут же перерисовать саму панель
    VisualDebug();
    // сменить родителя панели отображения переменных
    PnlDebugValues -> Parent = PCPerem -> ActivePage;
    // очистить значения предыдущих изменений
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
    /*          //0 страница
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
        "",
        "",
        "",
        "",
        "",*/
AnsiString valuesNames[] =
    {

    	//0 страница
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
        "",
        //1 страница
        "shr[15]",
        "sh[15]",
        "shr[16]",
        "sh[16]",
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
        //2 страница
        "shr[30]",
        "sh[30]",
        "shr[31]",
        "sh[31]",
        "shr[32]",
        "sh[32]",
        "shr[36]",
        "sh[36]",
        "shr[37]",
        "sh[37]",
        "shr[38]",
        "sh[38]",
        "shr[43]",
        "sh[43]",
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
        "zshr3",
        "norma",
        "qkk",
        //3 страница
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
        "",
        //4 страница
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
        "",
        "",

        //5 страница
        "out[0]",
        "out[1]",
        "out[2]",
        "out[3]",
        "out[4]",
        "out[5]",
        "",
        "zin[0]",
        "zin[1]",
        "zin[2]",
        "zin[3]",
        "zin[4]",
        "zin[5]",
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

        //6 страница
        "aout[0]",
        "aout[1]",
        "aout[2]",
        "aout[3]",
        "",
        "",
        "",
        "",
        "",
        "",
        "D_D1",
        "D_D2",
        "D_D3",
        "D_D4",
        "D_D5",
        "D_D6",
        "",
        "UVAK_KAM",
        "UVAKN_TMN",
        "UVAKV_TMN",
        "UVAK_SHL",
        "UVAKV_SHL",
        "UVAKN_SHL",
        "UVAK_SHL_MO",
        "UVAK_KAM_MO",
        "UVVAK_KAM",
        "UVAKN_ISP",
        "UVAKV_ISP",
        "UVAK_ISP_MO",
        "UATM",

        //7 страница
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
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "par_t[0]",
        "par_t[1]",
        "par_t[2]",
        "par_t[3]",
        "par_t[4]",
        "par_t[5]",

        //8 страница
        "par[0][1]",
        "par[0][9]",
        "par[0][10]",
        "par[0][11]",
        "par[0][12]",
        "par[0][13]",
        "par[0][14]",
        "",
        "par[1][1]",
        "par[1][2]",
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



        //9 страница
        "par[2][2]",
        "par[2][3]",
        "par[2][4]",
        "par[2][5]",
        "par[2][6]",
        "par[2][7]",
        "par[2][8]",
        "",
        "par[3][2]",
        "par[3][3]",
        "par[3][4]",
        "par[3][5]",
        "par[3][6]",
        "par[3][7]",
        "par[3][8]",
        "",
        "par[4][2]",
        "par[4][3]",
        "par[4][4]",
        "par[4][5]",
        "par[4][6]",
        "par[4][7]",
        "par[4][8]",
        "",
        "",
        "",
        "",
        "",
        "",
        "",

        //10 страница
        "par[5][2]",
        "par[5][3]",
        "par[5][4]",
        "par[5][5]",
        "par[5][6]",
        "par[5][7]",
        "par[5][8]",
        "",
        "par[6][2]",
        "par[6][3]",
        "par[6][4]",
        "par[6][5]",
        "par[6][6]",
        "par[6][7]",
        "par[6][8]",
        "",
        "par[7][2]",
        "par[7][3]",
        "par[7][4]",
        "par[7][5]",
        "par[7][6]",
        "par[7][7]",
        "par[7][8]",
        "",
        "",
        "",
        "",
        "",
        "",
        "",

        //11 страница
        "par[8][2]",
        "par[8][3]",
        "par[8][4]",
        "par[8][5]",
        "par[8][6]",
        "par[8][7]",
        "par[8][8]",
        "",
        "par[9][2]",
        "par[9][3]",
        "par[9][4]",
        "par[9][5]",
        "par[9][6]",
        "par[9][7]",
        "par[9][8]",
        "",
        "par[10][2]",
        "par[10][3]",
        "par[10][4]",
        "par[10][5]",
        "par[10][6]",
        "par[10][7]",
        "par[10][8]",
        "",
        "",
        "",
        "",
        "",
        "",
        "",


        //12 страница
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
        "CT_25",
        "CT_26",
        "CT_27",
        "CT27K1",
        "CT_29",
        "CT29K1",
        "CT_36",
        "CT36K1",
        "CT_38",
        "",
        "CT_RASPLAV",
        "CT_ELU",
        "CT_TEMP1",
        "CT_TEMP2",
        "CT_TMN",
        "CT_IST",
        "CT_VODA_NG",
        "CT_TOLSH",
        "",


        //13 страница
        "T_PROC",
        "T_KTMN_SHL_RAZGON",
        "T_KTMN_ISP_RAZGON",
        "T_KTMN_KAM_RAZGON",
        "T_VKL_BPN",
        "T_DVIJ",
        "T_KKAM",
        "T_KTMN",
        "T_KSHL",
        "T_KNAP",
        "T_NAPUSK",
        "T_KSHL_V",
        "T_KKAM_MO",
        "T_KVVAK_KAM",
        "T_KISP_MO",
        "T_KISP",
        "T_KISP_V",
        "T_KUST_ELU",
        "T_TMN",
        "T_VODA",
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



        //14 страница
        "PR_TRTEST",
        "KOM_TOLSH",
        "PR_TOLSH",
        "ZAD_N_PL",
        "PR_NALADKA",
        "SOST_V",
        "SOST_N",
        "RAB_NIJN",
        "PR_NAL_PD",
        "N_ST_MAX",
        "N_ST",
        "N_ZIKL_PROM",
        "N_ZIKL_PROM_KAM",
        "N_ZIKL_PROM_ISP",
        "N_ZIKL_PROM_SHL",
        "IMP60",
        "",
        "tigelVPos",
        "",
        "otvet",
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
        //15 страница
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


         //16 страница
        "PR_ELU",
        "KOM_ELU",
        "PAR_ELU",
        "ZPAR_ELU",
        "X_ELU",
        "VRELU",
        "E_ELU",
        "UST_ELU",
        "DELELU",
        "LIM1ELU",
        "LIM2ELU",
        "T_VRELU",
        "T_KELU",
        "DOPELU",
        "TEK_ELU",
        "N_PROCESS_ELU",
        "N_TIGEL",
        "POCKET_SET",
        "EMISSION_RELEASE_INTERVAL",
        "REMP_ELU",
        "pri_elu[0]",
        "pri_elu[1]",
        "pri_elu[2]",
        "pri_elu[3]",
        "pri_elu[4]",
        "pri_elu[5]",
        "",
        "",
        "",
        "",


        //17 страница
        "KOM_VR",
        "OTVET_VR",
        "TYPE_VR",
        "",
        "PR_VR",
        "HOME_VR",
        "",
        "PUT_VR",
        "V_VR",
        "TEK_ABS_VR",
        "TEK_OTN_VR",
        "",
        "CT_VR",
        "",
        "",
        "",
        "",
        "KOM_PER",
        "OTVET_PER",
        "TYPE_PER",
        "",
        "PR_PER",
        "HOME_PER",
        "",
        "PUT_PER",
        "V_PER",
        "TEK_ABS_PER",
        "TEK_OTN_PER",
        "",
        "CT_PER",


        

        //18 страница
        "KOM_POD",
        "OTVET_POD",
        "TYPE_POD",
        "",
        "PR_POD",
        "HOME_POD",
        "",
        "PUT_POD",
        "V_POD",
        "TEK_ABS_POD",
        "TEK_OTN_POD",
        "",
        "CT_POD",
        "",
        "",
        "",
        "",
        "KOM_KAS",
        "OTVET_KAS",
        "TYPE_KAS",
        "",
        "PR_KAS",
        "HOME_KAS",
        "",
        "PUT_KAS",
        "V_KAS",
        "TEK_ABS_KAS",
        "TEK_OTN_KAS",
        "",
        "CT_KAS",

        //19 страница
        "KOM_PPD",
        "OTVET_PPD",
        "TYPE_PPD",
        "",
        "PR_PPD",
        "HOME_PPD",
        "",
        "PUT_PPD",
        "V_PPD",
        "TEK_ABS_PPD",
        "TEK_OTN_PPD",
        "",
        "CT_PPD",
        "",
        "PR_PPD_D",
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


    };
	// считали номер активной страницы
    unsigned char pageCount = StrToInt ( PCPerem -> ActivePage -> Hint );
    // выставили имена переменных на странице
    for ( unsigned int i = 30 * pageCount; i < ( 30 * ( pageCount + 1  ) ); i++)
        EdtDebugValues[i%30] -> Text = valuesNames[i];

switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
{


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
		EditOTLtek30->Text = IntToStr(0);
}; break;
case 1:
{
		EditOTLtek1->Text = IntToStr(shr[15]);
		EditOTLtek2->Text = IntToStr(sh[15]);
		EditOTLtek3->Text = IntToStr(shr[16]);
		EditOTLtek4->Text = IntToStr(sh[16]);
		EditOTLtek5->Text = IntToStr(shr[17]);
		EditOTLtek6->Text = IntToStr(sh[17]);
		EditOTLtek7->Text = IntToStr(shr[18]);
		EditOTLtek8->Text = IntToStr(sh[18]);
		EditOTLtek9->Text = IntToStr(shr[19]);
		EditOTLtek10->Text = IntToStr(sh[19]);
		EditOTLtek11->Text = IntToStr(shr[20]);
		EditOTLtek12->Text = IntToStr(sh[20]);
		EditOTLtek13->Text = IntToStr(shr[21]);
		EditOTLtek14->Text = IntToStr(sh[21]);
		EditOTLtek15->Text = IntToStr(shr[22]);
		EditOTLtek16->Text = IntToStr(sh[22]);
		EditOTLtek17->Text = IntToStr(shr[23]);
		EditOTLtek18->Text = IntToStr(sh[23]);
		EditOTLtek19->Text = IntToStr(shr[24]);
		EditOTLtek20->Text = IntToStr(sh[24]);
		EditOTLtek21->Text = IntToStr(shr[25]);
		EditOTLtek22->Text = IntToStr(sh[25]);
		EditOTLtek23->Text = IntToStr(shr[26]);
		EditOTLtek24->Text = IntToStr(sh[26]);
		EditOTLtek25->Text = IntToStr(shr[27]);
		EditOTLtek26->Text = IntToStr(sh[27]);
		EditOTLtek27->Text = IntToStr(shr[28]);
		EditOTLtek28->Text = IntToStr(sh[28]);
		EditOTLtek29->Text = IntToStr(shr[29]);
		EditOTLtek30->Text = IntToStr(sh[29]);
}; break;
case 2:
{
		EditOTLtek1->Text = IntToStr(shr[30]);
		EditOTLtek2->Text = IntToStr(sh[30]);
		EditOTLtek3->Text = IntToStr(shr[31]);
		EditOTLtek4->Text = IntToStr(sh[31]);
		EditOTLtek5->Text = IntToStr(shr[32]);
		EditOTLtek6->Text = IntToStr(sh[32]);
		EditOTLtek7->Text = IntToStr(shr[36]);
		EditOTLtek8->Text = IntToStr(sh[36]);
		EditOTLtek9->Text = IntToStr(shr[37]);
		EditOTLtek10->Text = IntToStr(sh[37]);
		EditOTLtek11->Text = IntToStr(shr[38]);
		EditOTLtek12->Text = IntToStr(sh[38]);
		EditOTLtek13->Text = IntToStr(shr[43]);
		EditOTLtek14->Text = IntToStr(sh[43]);
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
		EditOTLtek28->Text = IntToStr(zshr3);
		EditOTLtek29->Text = IntToStr(norma);
		EditOTLtek30->Text = IntToStr(qkk);
}; break;
case 3:
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
		EditOTLtek30->Text = IntToStr(0);
}; break;
case 4:
{
		EditOTLtek1->Text = IntToStr(diagnS[0]);
		EditOTLtek2->Text = IntToStr(diagnS[1]);
		EditOTLtek3->Text = IntToStr(diagnS[2]);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(0);
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
case 5:
{
		EditOTLtek1->Text = IntToStr(out[0]);
		EditOTLtek2->Text = IntToStr(out[1]);
		EditOTLtek3->Text = IntToStr(out[2]);
		EditOTLtek4->Text = IntToStr(out[3]);
		EditOTLtek5->Text = IntToStr(out[4]);
		EditOTLtek6->Text = IntToStr(out[5]);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(zin[0]);
		EditOTLtek9->Text = IntToStr(zin[1]);
		EditOTLtek10->Text = IntToStr(zin[2]);
		EditOTLtek11->Text = IntToStr(zin[3]);
		EditOTLtek12->Text = IntToStr(zin[4]);
		EditOTLtek13->Text = IntToStr(zin[5]);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(aik[0]);
		EditOTLtek16->Text = IntToStr(aik[1]);
		EditOTLtek17->Text = IntToStr(aik[2]);
		EditOTLtek18->Text = IntToStr(aik[3]);
		EditOTLtek19->Text = IntToStr(aik[4]);
		EditOTLtek20->Text = IntToStr(aik[5]);
		EditOTLtek21->Text = IntToStr(aik[6]);
		EditOTLtek22->Text = IntToStr(aik[7]);
		EditOTLtek23->Text = IntToStr(aik[8]);
		EditOTLtek24->Text = IntToStr(aik[9]);
		EditOTLtek25->Text = IntToStr(aik[10]);
		EditOTLtek26->Text = IntToStr(aik[11]);
		EditOTLtek27->Text = IntToStr(aik[12]);
		EditOTLtek28->Text = IntToStr(aik[13]);
		EditOTLtek29->Text = IntToStr(aik[14]);
	   	EditOTLtek30->Text = IntToStr(aik[15]);
}; break;
case 6:
{
		EditOTLtek1->Text = IntToStr(aout[0]);
		EditOTLtek2->Text = IntToStr(aout[1]);
		EditOTLtek3->Text = IntToStr(aout[2]);
		EditOTLtek4->Text = IntToStr(aout[3]);
		EditOTLtek5->Text = IntToStr(0);
		EditOTLtek6->Text = IntToStr(0);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(0);
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(D_D1);
		EditOTLtek12->Text = IntToStr(D_D2);
		EditOTLtek13->Text = IntToStr(D_D3);
		EditOTLtek14->Text = IntToStr(D_D4);
		EditOTLtek15->Text = IntToStr(D_D5);
		EditOTLtek16->Text = IntToStr(D_D6);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(UVAK_KAM);
		EditOTLtek19->Text = IntToStr(UVAKN_TMN);
		EditOTLtek20->Text = IntToStr(UVAKV_TMN);
		EditOTLtek21->Text = IntToStr(UVAK_SHL);
		EditOTLtek22->Text = IntToStr(UVAKV_SHL);
		EditOTLtek23->Text = IntToStr(UVAKN_SHL);
		EditOTLtek24->Text = IntToStr(UVAK_SHL_MO);
		EditOTLtek25->Text = IntToStr(UVAK_KAM_MO);
		EditOTLtek26->Text = IntToStr(UVVAK_KAM);
		EditOTLtek27->Text = IntToStr(UVAKN_ISP);
		EditOTLtek28->Text = IntToStr(UVAKV_ISP);
		EditOTLtek29->Text = IntToStr(UVAK_ISP_MO);
		EditOTLtek30->Text = IntToStr(UATM);
}; break;
case 7:
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
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(0);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(0);
		EditOTLtek23->Text = IntToStr(0);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(par_t[0]);
		EditOTLtek26->Text = IntToStr(par_t[1]);
		EditOTLtek27->Text = IntToStr(par_t[2]);
		EditOTLtek28->Text = IntToStr(par_t[3]);
		EditOTLtek29->Text = IntToStr(par_t[4]);
		EditOTLtek30->Text = IntToStr(par_t[5]);
}; break;
case 8:
{
		EditOTLtek1->Text = IntToStr(par[0][1]);
		EditOTLtek2->Text = IntToStr(par[0][9]);
		EditOTLtek3->Text = IntToStr(par[0][10]);
		EditOTLtek4->Text = IntToStr(par[0][11]);
		EditOTLtek5->Text = IntToStr(par[0][12]);
		EditOTLtek6->Text = IntToStr(par[0][13]);
		EditOTLtek7->Text = IntToStr(par[0][14]);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(par[1][1]);
		EditOTLtek10->Text = IntToStr(par[1][2]);
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
case 9:
{
		EditOTLtek1->Text = IntToStr(par[2][2]);
		EditOTLtek2->Text = IntToStr(par[2][3]);
		EditOTLtek3->Text = IntToStr(par[2][4]);
		EditOTLtek4->Text = IntToStr(par[2][5]);
		EditOTLtek5->Text = IntToStr(par[2][6]);
		EditOTLtek6->Text = IntToStr(par[2][7]);
		EditOTLtek7->Text = IntToStr(par[2][8]);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(par[3][2]);
		EditOTLtek10->Text = IntToStr(par[3][3]);
		EditOTLtek11->Text = IntToStr(par[3][4]);
		EditOTLtek12->Text = IntToStr(par[3][5]);
		EditOTLtek13->Text = IntToStr(par[3][6]);
		EditOTLtek14->Text = IntToStr(par[3][7]);
		EditOTLtek15->Text = IntToStr(par[3][8]);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(par[4][2]);
		EditOTLtek18->Text = IntToStr(par[4][3]);
		EditOTLtek19->Text = IntToStr(par[4][4]);
		EditOTLtek20->Text = IntToStr(par[4][5]);
		EditOTLtek21->Text = IntToStr(par[4][6]);
		EditOTLtek22->Text = IntToStr(par[4][7]);
		EditOTLtek23->Text = IntToStr(par[4][8]);
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
		EditOTLtek1->Text = IntToStr(par[5][2]);
		EditOTLtek2->Text = IntToStr(par[5][3]);
		EditOTLtek3->Text = IntToStr(par[5][4]);
		EditOTLtek4->Text = IntToStr(par[5][5]);
		EditOTLtek5->Text = IntToStr(par[5][6]);
		EditOTLtek6->Text = IntToStr(par[5][7]);
		EditOTLtek7->Text = IntToStr(par[5][8]);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(par[6][2]);
		EditOTLtek10->Text = IntToStr(par[6][3]);
		EditOTLtek11->Text = IntToStr(par[6][4]);
		EditOTLtek12->Text = IntToStr(par[6][5]);
		EditOTLtek13->Text = IntToStr(par[6][6]);
		EditOTLtek14->Text = IntToStr(par[6][7]);
		EditOTLtek15->Text = IntToStr(par[6][8]);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(par[7][2]);
		EditOTLtek18->Text = IntToStr(par[7][3]);
		EditOTLtek19->Text = IntToStr(par[7][4]);
		EditOTLtek20->Text = IntToStr(par[7][5]);
		EditOTLtek21->Text = IntToStr(par[7][6]);
		EditOTLtek22->Text = IntToStr(par[7][7]);
		EditOTLtek23->Text = IntToStr(par[7][8]);
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
		EditOTLtek1->Text = IntToStr(par[8][2]);
		EditOTLtek2->Text = IntToStr(par[8][3]);
		EditOTLtek3->Text = IntToStr(par[8][4]);
		EditOTLtek4->Text = IntToStr(par[8][5]);
		EditOTLtek5->Text = IntToStr(par[8][6]);
		EditOTLtek6->Text = IntToStr(par[8][7]);
		EditOTLtek7->Text = IntToStr(par[8][8]);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(par[9][2]);
		EditOTLtek10->Text = IntToStr(par[9][3]);
		EditOTLtek11->Text = IntToStr(par[9][4]);
		EditOTLtek12->Text = IntToStr(par[9][5]);
		EditOTLtek13->Text = IntToStr(par[9][6]);
		EditOTLtek14->Text = IntToStr(par[9][7]);
		EditOTLtek15->Text = IntToStr(par[9][8]);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(par[10][2]);
		EditOTLtek18->Text = IntToStr(par[10][3]);
		EditOTLtek19->Text = IntToStr(par[10][4]);
		EditOTLtek20->Text = IntToStr(par[10][5]);
		EditOTLtek21->Text = IntToStr(par[10][6]);
		EditOTLtek22->Text = IntToStr(par[10][7]);
		EditOTLtek23->Text = IntToStr(par[10][8]);
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
		EditOTLtek12->Text = IntToStr(CT_25);
		EditOTLtek13->Text = IntToStr(CT_26);
		EditOTLtek14->Text = IntToStr(CT_27);
		EditOTLtek15->Text = IntToStr(CT27K1);
		EditOTLtek16->Text = IntToStr(CT_29);
		EditOTLtek17->Text = IntToStr(CT29K1);
		EditOTLtek18->Text = IntToStr(CT_36);
		EditOTLtek19->Text = IntToStr(CT36K1);
		EditOTLtek20->Text = IntToStr(CT_38);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(CT_RASPLAV);
		EditOTLtek23->Text = IntToStr(CT_ELU);
		EditOTLtek24->Text = IntToStr(CT_TEMP1);
		EditOTLtek25->Text = IntToStr(CT_TEMP2);
		EditOTLtek26->Text = IntToStr(CT_TMN);
		EditOTLtek27->Text = IntToStr(CT_IST);
		EditOTLtek28->Text = IntToStr(CT_VODA_NG);
		EditOTLtek29->Text = IntToStr(CT_TOLSH);
		EditOTLtek30->Text = IntToStr(0);
}; break;
case 13:
{
		EditOTLtek1->Text = IntToStr(T_PROC);
		EditOTLtek2->Text = IntToStr(T_KTMN_SHL_RAZGON);
		EditOTLtek3->Text = IntToStr(T_KTMN_ISP_RAZGON);
		EditOTLtek4->Text = IntToStr(T_KTMN_KAM_RAZGON);
		EditOTLtek5->Text = IntToStr(T_VKL_BPN);
		EditOTLtek6->Text = IntToStr(T_DVIJ);
		EditOTLtek7->Text = IntToStr(T_KKAM);
		EditOTLtek8->Text = IntToStr(T_KTMN);
		EditOTLtek9->Text = IntToStr(T_KSHL);
		EditOTLtek10->Text = IntToStr(T_KNAP);
		EditOTLtek11->Text = IntToStr(T_NAPUSK);
		EditOTLtek12->Text = IntToStr(T_KSHL_V);
		EditOTLtek13->Text = IntToStr(T_KKAM_MO);
		EditOTLtek14->Text = IntToStr(T_KVVAK_KAM);
		EditOTLtek15->Text = IntToStr(T_KISP_MO);
		EditOTLtek16->Text = IntToStr(T_KISP);
		EditOTLtek17->Text = IntToStr(T_KISP_V);
		EditOTLtek18->Text = IntToStr(T_KUST_ELU);
		EditOTLtek19->Text = IntToStr(T_TMN);
		EditOTLtek20->Text = IntToStr(T_VODA);
		EditOTLtek21->Text = IntToStr(T_KSHL_MO);
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
		EditOTLtek1->Text = IntToStr(PR_TRTEST);
		EditOTLtek2->Text = IntToStr(KOM_TOLSH);
		EditOTLtek3->Text = IntToStr(PR_TOLSH);
		EditOTLtek4->Text = IntToStr(ZAD_N_PL);
		EditOTLtek5->Text = IntToStr(PR_NALADKA);
		EditOTLtek6->Text = IntToStr(SOST_V);
		EditOTLtek7->Text = IntToStr(SOST_N);
		EditOTLtek8->Text = IntToStr(RAB_NIJN);
		EditOTLtek9->Text = IntToStr(PR_NAL_PD);
		EditOTLtek10->Text = IntToStr(N_ST_MAX);
		EditOTLtek11->Text = IntToStr(N_ST);
		EditOTLtek12->Text = IntToStr(N_ZIKL_PROM);
		EditOTLtek13->Text = IntToStr(N_ZIKL_PROM_KAM);
		EditOTLtek14->Text = IntToStr(N_ZIKL_PROM_ISP);
		EditOTLtek15->Text = IntToStr(N_ZIKL_PROM_SHL);
		EditOTLtek16->Text = FloatToStr(IMP60);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(tigelVPos);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(otvet);
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
case 16:
{
		EditOTLtek1->Text = IntToStr(PR_ELU);
		EditOTLtek2->Text = IntToStr(KOM_ELU);
		EditOTLtek3->Text = IntToStr(PAR_ELU);
		EditOTLtek4->Text = IntToStr(ZPAR_ELU);
		EditOTLtek5->Text = IntToStr(X_ELU);
		EditOTLtek6->Text = IntToStr(VRELU);
		EditOTLtek7->Text = IntToStr(E_ELU);
		EditOTLtek8->Text = IntToStr(UST_ELU);
		EditOTLtek9->Text = IntToStr(DELELU);
		EditOTLtek10->Text = IntToStr(LIM1ELU);
		EditOTLtek11->Text = IntToStr(LIM2ELU);
		EditOTLtek12->Text = IntToStr(T_VRELU);
		EditOTLtek13->Text = IntToStr(T_KELU);
		EditOTLtek14->Text = IntToStr(DOPELU);
		EditOTLtek15->Text = IntToStr(TEK_ELU);
		EditOTLtek16->Text = IntToStr(N_PROCESS_ELU);
		EditOTLtek17->Text = IntToStr(N_TIGEL);
		EditOTLtek18->Text = IntToStr(POCKET_SET);
		EditOTLtek19->Text = IntToStr(EMISSION_RELEASE_INTERVAL);
		EditOTLtek20->Text = IntToStr(REMP_ELU);
		EditOTLtek21->Text = IntToStr(pri_elu[0]);
		EditOTLtek22->Text = IntToStr(pri_elu[1]);
		EditOTLtek23->Text = IntToStr(pri_elu[2]);
		EditOTLtek24->Text = IntToStr(pri_elu[3]);
		EditOTLtek25->Text = IntToStr(pri_elu[4]);
		EditOTLtek26->Text = IntToStr(pri_elu[5]);
		EditOTLtek27->Text = IntToStr(0);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(0);
}; break;
case 17:
{
		EditOTLtek1->Text = IntToStr(KOM_VR);
		EditOTLtek2->Text = IntToStr(OTVET_VR);
		EditOTLtek3->Text = IntToStr(TYPE_VR);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(PR_VR);
		EditOTLtek6->Text = IntToStr(HOME_VR);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(PUT_VR);
		EditOTLtek9->Text = IntToStr(V_VR);
		EditOTLtek10->Text = IntToStr(TEK_ABS_VR);
		EditOTLtek11->Text = IntToStr(TEK_OTN_VR);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(CT_VR);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(KOM_PER);
		EditOTLtek19->Text = IntToStr(OTVET_PER);
		EditOTLtek20->Text = IntToStr(TYPE_PER);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(PR_PER);
		EditOTLtek23->Text = IntToStr(HOME_PER);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(PUT_PER);
		EditOTLtek26->Text = IntToStr(V_PER);
		EditOTLtek27->Text = IntToStr(TEK_ABS_PER);
		EditOTLtek28->Text = IntToStr(TEK_OTN_PER);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(CT_PER);
}; break;
case 18:
{
		EditOTLtek1->Text = IntToStr(KOM_POD);
		EditOTLtek2->Text = IntToStr(OTVET_POD);
		EditOTLtek3->Text = IntToStr(TYPE_POD);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(PR_POD);
		EditOTLtek6->Text = IntToStr(HOME_POD);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(PUT_POD);
		EditOTLtek9->Text = IntToStr(V_POD);
		EditOTLtek10->Text = IntToStr(TEK_ABS_POD);
		EditOTLtek11->Text = IntToStr(TEK_OTN_POD);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(CT_POD);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(0);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(KOM_KAS);
		EditOTLtek19->Text = IntToStr(OTVET_KAS);
		EditOTLtek20->Text = IntToStr(TYPE_KAS);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(PR_KAS);
		EditOTLtek23->Text = IntToStr(HOME_KAS);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(PUT_KAS);
		EditOTLtek26->Text = IntToStr(V_KAS);
		EditOTLtek27->Text = IntToStr(TEK_ABS_KAS);
		EditOTLtek28->Text = IntToStr(TEK_OTN_KAS);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(CT_KAS);
}; break;
case 19:
{
		EditOTLtek1->Text = IntToStr(KOM_PPD);
		EditOTLtek2->Text = IntToStr(OTVET_PPD);
		EditOTLtek3->Text = IntToStr(TYPE_PPD);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(PR_PPD);
		EditOTLtek6->Text = IntToStr(HOME_PPD);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(PUT_PPD);
		EditOTLtek9->Text = IntToStr(V_PPD);
		EditOTLtek10->Text = IntToStr(TEK_ABS_PPD);
		EditOTLtek11->Text = IntToStr(TEK_OTN_PPD);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(CT_PPD);
		EditOTLtek14->Text = IntToStr(0);
		EditOTLtek15->Text = IntToStr(PR_PPD_D);
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


}

}
//---------------------------------------------------------------------------

//ОТЛАДОЧНАЯ СТРАНИЦА
//---------------------------------------------------------------------------
//--Изменение давления Д1--//
void __fastcall TForm1::SBD1DebugChange(TObject *Sender)
{   // изменить код давления
    EdtD1Code -> Text = IntToStr(SBD1Debug->Position);
    // пересчитать значение давления
    EdtD1Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD1Debug->Position)/1000.0-6.8)/0.6),ffExponent,3,8);
}
//---------------------------------------------------------------------------
//--Изменение давления Д1--//
void __fastcall TForm1::EdtD1CodeChange(TObject *Sender)
{   // изменить код давления
    SBD1Debug -> Position = StrToInt(EdtD1Code->Text);
    // пересчитать значение давления
   EdtD1Davl -> Text = FloatToStrF(100.0*pow(10.0,(float(SBD1Debug->Position)/1000.0-6.8)/0.6),ffExponent,3,8);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Изменение давления Д2--//
void __fastcall TForm1::SBD2DebugChange(TObject *Sender)
{   // изменить код давления
    EdtD2Code -> Text = IntToStr(SBD2Debug->Position);
    // пересчитать значение давления
    EdtD2Davl -> Text = FloatToStrF(133.3*pow(10,(float)SBD2Debug->Position/1000.0-6.0),ffExponent,3,8);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Изменение давления Д2--//
void __fastcall TForm1::EdtD2CodeChange(TObject *Sender)
{   // изменить код давления
    SBD2Debug -> Position = StrToInt(EdtD2Code->Text);
    // пересчитать значение давления
    EdtD2Davl -> Text = FloatToStrF(133.3*pow(10,(float)SBD2Debug->Position/1000.0-6.0),ffExponent,3,8);
}

//---------------------------------------------------------------------------
//--Передача настроечных параметров--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNasModClick(TObject *Sender)
{
  PanelParNastr -> Visible = true;
}
//---------------------------------------------------------------------------
//--Визуализация настроечных параметров--//
//---------------------------------------------------------------------------
void TForm1::VisualNasmod()
{

    EditNastrIn0 -> Text =FloatToStrF(pow(10,(float)nasmod[0]/1000.0*1.667-9.333),ffExponent,3,8);
    EditNastrIn1 -> Text =FloatToStrF(pow(10,(float)nasmod[1]/1000.0*1.667-9.333),ffExponent,3,8);
    EditNastrIn2 -> Text =FloatToStrF(pow(10,(float)nasmod[2]/1000.0*1.667-9.333),ffExponent,3,8);
    EditNastrIn3 -> Text =FloatToStrF((float)nasmod[3],ffFixed,5,0);
    EditNastrIn4 -> Text =FloatToStrF((float)nasmod[4],ffFixed,5,0);
    EditNastrIn5 -> Text =FloatToStrF((float)nasmod[5],ffFixed,5,0);

    EditNastrIn7 -> Text =FloatToStrF((float)nasmod[7]/10.0,ffFixed,5,0);
    EditNastrIn8 -> Text =FloatToStrF((float)nasmod[8],ffFixed,5,0);
    EditNastrIn9 -> Text =FloatToStrF((float)nasmod[9],ffFixed,5,0);
    EditNastrIn10 -> Text =FloatToStrF((float)nasmod[10],ffFixed,5,0);
    EditNastrIn11 -> Text =FloatToStrF((float)nasmod[11],ffFixed,5,0);
    EditNastrIn12 -> Text =FloatToStrF((float)nasmod[12],ffFixed,5,0);
    EditNastrIn13 -> Text =FloatToStrF((float)nasmod[13],ffFixed,5,0);
    EditNastrIn14 -> Text =( nasmod[14] ? "Да" : "Нет" );
    EditNastrIn15 -> Text =FloatToStrF((float)nasmod[15],ffFixed,5,0);
    EditNastrIn16 -> Text =( nasmod[16] ? "Да" : "Нет" );

}

//---------------------------------------------------------------------------
//--Подтверждение передачи настроечных параметров--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrDaClick(TObject *Sender)
{

    // Скрыть панель
    PanelParNastr -> Visible = false;
    //предельный уровень высоковакуумной откачки камеры мрт200
    nasmod[0]=int(((log10(StrToFloat(EditNastrTo0->Text)))*0.6+5.6)*1000.0);
    //предельный уровень высоковакуумной откачки испарителя мрт200
    nasmod[1]=int(((log10(StrToFloat(EditNastrTo1->Text)))*0.6+5.6)*1000.0);
    //предельный уровень высоковакуумной откачки шлюза мрт200
    nasmod[2]=int(((log10(StrToFloat(EditNastrTo2->Text)))*0.6+5.6)*1000.0);
    //количество циклов промывки при откачке камеры
    nasmod[3]=StrToInt(EditNastrTo3->Text);
    //количество циклов промывки при откачке испарителя
    nasmod[4]=StrToInt(EditNastrTo4->Text);
    //количество циклов при откачке шлюза
    nasmod[5]=StrToInt(EditNastrTo5->Text);

    //температура прогрева шлюза
    nasmod[7]=StrToFloat(EditNastrTo7->Text)*10;
    //Задержка на открытие заслонки 1 тигеля
    nasmod[8]=StrToInt(EditNastrTo8->Text);
    //Задержка на открытие заслонки 2 тигеля
    nasmod[9]=StrToInt(EditNastrTo9->Text);
    //Задержка на открытие заслонки 3 тигеля
    nasmod[10]=StrToInt(EditNastrTo10->Text);
    //Задержка на открытие заслонки 4 тигеля
    nasmod[11]=StrToInt(EditNastrTo11->Text);
    //Задержка на открытие заслонки 5 тигеля
    nasmod[12]=StrToInt(EditNastrTo12->Text);
    //Задержка на открытие заслонки 6 тигеля
    nasmod[13]=StrToInt(EditNastrTo13->Text);
    //Рабочий цикл с отключением техпроцесса
    EditNastrTo14 -> Text == "Да" ? nasmod[14] = 1 : nasmod[14] = 0;
    //Время открытия заслонки смотрового окна
    nasmod[15]=StrToInt(EditNastrTo15->Text);
    //Работа без опроса датчика наличия п/д на ПГП
    EditNastrTo16 -> Text == "Да" ? nasmod[16] = 1 : nasmod[16] = 0;

    // запомнили действие в журнал
    MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time -> Caption + " : Изменены значения настроечного массива : ");
    if ( EditNastrTo0 -> Text != EditNastrIn0 -> Text )
        MemoStat -> Lines -> Add( "Предельный уровень высоковакуумной откачки камеры : " + EditNastrIn0 -> Text + " -> " + EditNastrTo0 -> Text );
    if ( EditNastrTo1 -> Text != EditNastrIn1 -> Text )
        MemoStat -> Lines -> Add( "предельный уровень высоковакуумной откачки испарителя : " + EditNastrIn1 -> Text + " -> " + EditNastrTo1 -> Text );
    if ( EditNastrTo2 -> Text != EditNastrIn2 -> Text )
        MemoStat -> Lines -> Add( "предельный уровень высоковакуумной откачки шлюза : " + EditNastrIn2 -> Text + " -> " + EditNastrTo2 -> Text );
    if ( EditNastrTo3 -> Text != EditNastrIn3 -> Text )
        MemoStat -> Lines -> Add( "количество циклов промывки при откачке камеры : " + EditNastrIn3 -> Text + " -> " + EditNastrTo3 -> Text );
    if ( EditNastrTo4 -> Text != EditNastrIn4 -> Text )
        MemoStat -> Lines -> Add( "количество циклов промывки при откачке испарителя : " + EditNastrIn4 -> Text + " -> " + EditNastrTo4 -> Text );
    if ( EditNastrTo5 -> Text != EditNastrIn5 -> Text )
        MemoStat -> Lines -> Add( "количество циклов при откачке шлюза : " + EditNastrIn5 -> Text + " -> " + EditNastrTo5 -> Text );
       if ( EditNastrTo7 -> Text != EditNastrIn7 -> Text )
        MemoStat -> Lines -> Add( "температура прогрева шлюза : " + EditNastrIn7 -> Text + " -> " + EditNastrTo7 -> Text );
    if ( EditNastrTo8 -> Text != EditNastrIn8 -> Text )
        MemoStat -> Lines -> Add( "Задержка на открытие заслонки 1 тигеля : " + EditNastrIn8 -> Text + " -> " + EditNastrTo8 -> Text );
    if ( EditNastrTo9 -> Text != EditNastrIn9 -> Text )
        MemoStat -> Lines -> Add( "Задержка на открытие заслонки 2 тигеля : " + EditNastrIn9 -> Text + " -> " + EditNastrTo9 -> Text );
    if ( EditNastrTo10 -> Text != EditNastrIn10 -> Text )
        MemoStat -> Lines -> Add( "Задержка на открытие заслонки 2 тигеля : " + EditNastrIn10 -> Text + " -> " + EditNastrTo10 -> Text );
    if ( EditNastrTo11 -> Text != EditNastrIn11 -> Text )
        MemoStat -> Lines -> Add( "Задержка на открытие заслонки 2 тигеля : " + EditNastrIn11 -> Text + " -> " + EditNastrTo11 -> Text );
    if ( EditNastrTo12 -> Text != EditNastrIn12 -> Text )
        MemoStat -> Lines -> Add( "Задержка на открытие заслонки 2 тигеля : " + EditNastrIn12 -> Text + " -> " + EditNastrTo12 -> Text );
    if ( EditNastrTo13 -> Text != EditNastrIn13 -> Text )
        MemoStat -> Lines -> Add( "Задержка на открытие заслонки 2 тигеля : " + EditNastrIn13 -> Text + " -> " + EditNastrTo13 -> Text );
    if ( EditNastrTo14 -> Text != EditNastrIn14 -> Text )
        MemoStat -> Lines -> Add( "Рабочий цикл с отключением техпроцесса : " + EditNastrIn14 -> Text + " -> " + EditNastrTo14 -> Text );
    if ( EditNastrTo15 -> Text != EditNastrIn15 -> Text )
        MemoStat -> Lines -> Add( "Время открытия заслонки смотрового окна : " + EditNastrIn15 -> Text + " -> " + EditNastrTo15 -> Text );
    if ( EditNastrTo16 -> Text != EditNastrIn16 -> Text )
        MemoStat -> Lines -> Add( "Работа без опроса датчика наличия п/д на ПГП : " + EditNastrIn16 -> Text + " -> " + EditNastrTo16 -> Text );

    // закрашивание белым цветом переданных ячек

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





    // сохранить значения настроечного массива
    MemoNasmod -> Lines -> Clear();

    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo0->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo1->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo2->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo3->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo4->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo5->Text);

    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo6->ItemIndex));
    
    MemoNasmod -> Lines -> Add(EditNastrTo7->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo8->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo9->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo10->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo11->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo12->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo13->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo14->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo15->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo16->ItemIndex));
    MemoNasmod -> Lines -> SaveToFile("Nasmod\\Nasmod.txt");




    VisualNasmod();
    VisualParA();

    VisualParR();
    // визуализация настроечных параметров
}
//---------------------------------------------------------------------------
//--Отказ от передачи настроечных параметров--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrNetClick(TObject *Sender)
{
  PanelParNastr -> Visible = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--Функция срабатывания таймера логики--//
//---------------------------------------------------------------------------
void __fastcall TLogicThread::LM()
{
    // главный цикл логики
    LogicMain();

    logic_time = (float)(TimeNew.QuadPart-TimeOld.QuadPart)/(float)(TimeFreq.QuadPart);
    logicPerSecond++;
}
//---------------------------------------------------------------------------
//--Функция срабатывания точного таймера 1 мс--//
//---------------------------------------------------------------------------
void __fastcall TTimerExist::EM()
{
    // диспетчер внешних плат
    Form1 -> ExternalManager();
}
//---------------------------------------------------------------------------
//--Функция срабатывания точного таймера 1 мс--//
//---------------------------------------------------------------------------
void __fastcall TTimerExist::Timer1ms()
{  
    if(!ust_ready) return;
    // времена логики
    TIME();

    Comport[0]->Dev_Timer++;
    Comport[1]->Dev_Timer++;
    Comport[2]->Dev_Timer++;
    Comport[3]->Dev_Timer++;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//--Изменение кол-ва графиков--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ChBoxGraphTemp1Click(TObject *Sender)
{
 // анализируем принадлежность объекта текущим графикам
    if(StrToInt(((TCheckBox*)Sender)->Hint)<SERIES_COUNT)
    { serTemp[StrToInt(((TCheckBox*)Sender)->Hint)] -> Active = ((TCheckBox*)Sender) -> Checked; }
    // архивные графики
    else
    { serArh[StrToInt(((TCheckBox*)Sender)->Hint)-SERIES_COUNT]->Active=((TCheckBox*)Sender)->Checked; }
}
//---------------------------------------------------------------------------
//--Разбор строки архива графиков--//
//---------------------------------------------------------------------------
void ArhToGraph (AnsiString graphStr)
{
    // первое значение - дата, следующие - данные
    AnsiString str[SERIES_COUNT+1];
    // очистка массива
    for ( int i = 0 ; i < SERIES_COUNT+1 ; i++ ) str[i] = "";
    int byteNmb = 1;
    // разбиение строки по значениям
    for ( int i = 0 ; i < SERIES_COUNT+1 ; i++ )
    {
        while (graphStr[byteNmb]!=';')
        {
            str[i] += AnsiString(graphStr[byteNmb]);
            byteNmb++;
        }
        byteNmb++;
    }
    // заполнение графиков значениями
    for ( int i = 1 ; i < SERIES_COUNT+1 ; i++ )
        serArh[i-1] -> AddY(StrToFloat(str[i]),str[0]);
}
//---------------------------------------------------------------------------
//--Загрузить графики--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxGraphArhClick(TObject *Sender)
{
    // очистка графиков
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
//--Отображаем диагностики и статистику--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ListBoxStatArhClick(TObject *Sender)
{
    // открываем статистику
    int itemNmb = ListBoxStatArh -> ItemIndex;
    AnsiString fName = "Stat\\" + ListBoxStatArh->Items->operator[](itemNmb) + ".txt";
    MemoStatArh -> Lines -> LoadFromFile (fName);
    fName = "Diag\\" + ListBoxStatArh->Items->operator[](itemNmb) + ".txt";
    // открывали по статистике, диагностик может не быть
    if ( FileExists ( fName ) )
        MemoDiagArh -> Lines -> LoadFromFile (fName);
}
//---------------------------------------------------------------------------
//--Визуализация графиков--//
//---------------------------------------------------------------------------
void TForm1::VisualGraph()
{

    AnsiString graphTemp = "";

    if((shr[3]==0&&shr[4]==0)||PR_NALADKA) return;    // не писать

    // время
    graphTemp = Label_Time -> Caption + ";";

    // Температура нагрева п/д                                 /100.0)
    if((shr[4])&&(shr[29]))
    {
        graphTemp = graphTemp + FloatToStrF((((float(TEK_TEMP1)/100.0)-4.0)/1.6),ffFixed,4,1) + ";";
        serTemp[0] -> AddY((((float(TEK_TEMP1)/100.0)-4.0)/1.6), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[0] -> AddY(0.0,Label_Time -> Caption);
    }

    // Ток эмиссии
    if((shr[4])&&(shr[36])&&(shr[38]))
    {
        graphTemp = graphTemp + FloatToStrF((float)TEK_ELU/10.0, ffFixed, 6, 1) + ";";
        serTemp[2] -> AddY((float)TEK_ELU/10.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[2] -> AddY(0.0,Label_Time -> Caption);
    }
    // Температура нагрева п/д
    if((shr[4])&&(shr[29]))
    {
        graphTemp = graphTemp + FloatToStrF((float(TEK_TEMP3)/10.0),ffFixed,4,1) + ";";
        serTemp[1] -> AddY(float(TEK_TEMP3)/10.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[1] -> AddY(0.0,Label_Time -> Caption);
    }



    // добавили строку
    MemoGraph -> Lines -> Add ( graphTemp );
}
//---------------------------------------------------------------------------
//--Отладка: вкл/откл--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDebugClick(TObject *Sender)
{
    LabelOTLreg -> Visible = (bool) StrToInt( ((TButton*)Sender) -> Hint );
    LabelOTLreg -> Visible = pr_otl = (bool)StrToInt(((TButton*)Sender)->Hint);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//--Управление активными элементами--//
//---------------------------------------------------------------------------
void __fastcall TForm1::ImgFkKamClick(TObject *Sender)
{
    // анализируем с какой страницы пришел вызов, если с автоматической то выход
    if ( PCMain -> ActivePage == TSWork ) return;
    // наименования клавиш
    if  (
                (((TImage*)Sender)->Name) == "Fvn_Shl" ||
                (((TImage*)Sender)->Name) == "Fvn_Kam" ||
                (((TImage*)Sender)->Name) == "Tmn_Shl" ||
                (((TImage*)Sender)->Name) == "Tmn_Kam" ||
                (((TImage*)Sender)->Name) == "Tmn_Isp"
        )
    {
        BtnDeviceOn -> Caption = "Вкл.";
        BtnDeviceOff -> Caption = "Откл.";
    }


    else
    {
        BtnDeviceOn -> Caption = "Откр.";
        BtnDeviceOff -> Caption = "Закр.";
    }
    // отобразить панель управления
    LblDeviceName -> Caption = ((TImage*)Sender) -> Hint;
    PnlDevice -> Hint = ((TImage*)Sender) -> Name;


    PnlDevice -> Top = ((TImage*)Sender)->Top - 90;
    PnlDevice -> Left = ((TImage*)Sender)->Left - 90;
    PnlDevice -> Visible = true;
    PnlDevice -> BringToFront();
}
//---------------------------------------------------------------------------
//--Управление устройствами--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDeviceOnClick(TObject *Sender)
{
    // скрыть диалоговую панель управления элементами мнемосхемы
    ((TButton*)Sender) -> Parent -> Visible = false;
    // если нажата клавиша выход, то выйти
    if (((TButton*)Sender) -> Name == "BtnDeviceExit" ) return;
    if ( ((TButton*)Sender) -> Parent -> Hint == "Kl_Nap1" )       SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl_Nap2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x04);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl_Nap3" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 1, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Tmn_Shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x10);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Tmn_Isp" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x1000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Tmn_Kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x04);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Shl_M" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Isp_M" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x200);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Kam_M" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x02);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fvn_Shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x08);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fvn_Kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x10);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Tmn_Shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 5, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Tmn_Isp" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 5, 0x08);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Zatv_Tmn_Shl" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 0, 0x04);
            SetOut(0, 0, 0x08);
        }
        else
        {
            SetOut(0, 0, 0x04);
            SetOut(1, 0, 0x08);

        }
    }

    else if ( ((TButton*)Sender) -> Parent -> Hint == "pp" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 0, 0x40);
            SetOut(0, 0, 0x80);
        }
        else
        {
            SetOut(0, 0, 0x40);
            SetOut(1, 0, 0x80);

        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Zasl_Isp" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 1, 0x1000);
            SetOut(0, 1, 0x2000);
        }
        else
        {
            SetOut(0, 1, 0x1000);
            SetOut(1, 1, 0x2000);

        }
    }

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Zatv_Tmn_Isp" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 0, 0x400);
            SetOut(0, 0, 0x800);
        }
        else
        {
            SetOut(0, 0, 0x400);
            SetOut(1, 0, 0x800);

        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Shl" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 0, 0x01);
        }
        else
        {
            SetOut(0, 0, 0x03);

        }
    }


    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Isp" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 0, 0x100);
        }
        else
        {


            SetOut(0, 0, 0x300);
        }
    }


    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Kam" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {

            SetOut(1, 3, 0x01);
        }
        else
        {

            SetOut(0, 3, 0x03);
        }
    }


    else if ( ((TButton*)Sender) -> Parent -> Hint == "Tmn_Kam" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(1, 5, 0x30);

        }
        else
        {

            SetOut(0, 5, 0x10);
        }
    }


    // запомнили действие в журнал
    MemoStat -> Lines -> Add( DateToStr(Date()) + " " + TimeToStr(Time()) + " : Управление элементом : " + LblDeviceName -> Caption + " : " + ((TButton*)Sender)->Caption);
}
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Визуализация заголовка--//
void TForm1::VisualZagol()
{
    int i=0;
    AnsiString TempStr = "";
    if(shr[1])
    {
        Form1 -> EditSHRName2->Visible=true;
        Form1 -> EditSHRName2   ->  Text    = SHR2Names[shr[2]];
    }
    else
        Form1 -> EditSHRName2->Visible=false;
    // норма
    EditNormName -> Text = NormNames[norma];
    // количество режимов, чьи шаги надо отображать
    #define SHR_VALUE_COUNT 13
    // порядок следования важности шагов (веса шагов)
    unsigned char SHRValue[SHR_VALUE_COUNT] = {8,5,3/*,2*/,7,1,4,6,10,9,26,12,25};
    // анализируем активность режимов в порядке убывания значимости
    for(i=0; i<SHR_VALUE_COUNT; i++)
        if(shr[SHRValue[i]])
        {   Form1 -> EditRName -> Text = SHRNames[SHRValue[i]];
            switch(SHRValue[i])
            {   case 1: Form1 -> EditSHRName -> Text = SHR1Names[shr[SHRValue[i]]]; break;
               // case 2: Form1 -> EditSHRName -> Text = SHR2Names[shr[SHRValue[i]]]; break;
                case 3: Form1 -> EditSHRName -> Text = SHR3Names[shr[SHRValue[i]]]; break;
                case 4: Form1 -> EditSHRName -> Text = SHR4Names[shr[SHRValue[i]]]; break;
                case 5: Form1 -> EditSHRName -> Text = SHR5Names[shr[SHRValue[i]]]; break;
                case 6: Form1 -> EditSHRName -> Text = SHR6Names[shr[SHRValue[i]]]; break;
                case 7: Form1 -> EditSHRName -> Text = SHR7Names[shr[SHRValue[i]]]; break;
                case 8: Form1 -> EditSHRName -> Text = SHR8Names[shr[SHRValue[i]]]; break;
                case 9: Form1 -> EditSHRName -> Text = SHR9Names[shr[SHRValue[i]]]; break;
                case 10: Form1 -> EditSHRName -> Text = SHR10Names[shr[SHRValue[i]]]; break;
                case 12: Form1 -> EditSHRName -> Text = SHR12Names[shr[SHRValue[i]]]; break;
                case 25: Form1 -> EditSHRName -> Text = SHR25Names[shr[SHRValue[i]]]; break;
                case 26: Form1 -> EditSHRName -> Text = SHR26Names[shr[SHRValue[i]]]; break;

            }
            // в режиме 8 шаг 4
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==4))
            {
                if(shr[5]==16)
                    Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
                else if(shr[5]==31)
                    Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
                else
                    Form1 -> EditSHRName -> Text = SHR5Names[shr[5]];
            }
            // в режиме 8 шаг 5
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==5))
                Form1 -> EditSHRName -> Text = SHR7Names[shr[7]];
            // в режиме 5 шаг 16
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==16))
                Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
            // в режиме 5 шаг 31
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==31))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // в режиме 1 шаг 50
            if((SHRValue[i]==1)&&(shr[SHRValue[i]]==50))
                Form1 -> EditSHRName -> Text = SHR25Names[shr[25]];
            // в режиме 3 шаг 2
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==2))
            {
                if(shr[1]==50)
                    Form1 -> EditSHRName -> Text = SHR25Names[shr[25]];
                else
                    Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            }

            // в режиме 3 шаг 11
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==11))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // в режиме 3 шаг 25
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==25))
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            // в режиме 3 шаг 45
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==45))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // в режиме 26 шаг 9
            if((SHRValue[i]==26)&&(shr[SHRValue[i]]==9))
                Form1 -> EditSHRName -> Text = SHR25Names[shr[25]];
            // в режиме 6 шаг 15
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==15))
                Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
            // в режиме 6 шаг 31
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==31))
                Form1 -> EditSHRName -> Text = SHR2Names[shr[2]];
            // в режиме 6 шаг 32
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==32))
                Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];

            // в режиме 4 шаг 4 считать время
            if(shr[4]==4)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][2]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 16 считать время
            if(shr[4]==16)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[8]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 17 считать время
            if(shr[4]==17)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[9]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 18 считать время
            if(shr[4]==18)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[10]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 19 считать время
            if(shr[4]==19)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[11]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 20 считать время
            if(shr[4]==20)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 21 считать время
            if(shr[4]==21)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(nasmod[13]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 24 считать время
            if(shr[4]==24)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][2]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 6 шаг 28 считать время
            if(shr[4]==24)
            {
                TempStr = SHR6Names[shr[6]]+IntToStr(T_NAPUSK-CT_6);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 7 шаг 34 считать время
            if(shr[7]==34)
            {
                if(PR_OST_TMN_ISP&&PR_OST_TMN_SHL)
                    TempStr = SHR7Names[shr[7]]+IntToStr(600-CT_7);
                else if (PR_OST_TMN_ISP)
                    TempStr = "Остановка ТМНисп: "+IntToStr(600-CT_7);
                else
                    TempStr = "Остановка ТМНшл: "+IntToStr(600-CT_7);
                Form1 -> EditSHRName -> Text = TempStr;
            }





            return;
        }

    // нет активных режимов очистить поля
    Form1 -> EditRName   -> Text = "";
    Form1 -> EditSHRName -> Text = "";

}
//---------------------------------------------------------------------------
//--ОТОБРАЖЕНИя диагностики на мнемосхему--//
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

        // выявление появления или исчезновения диагностики
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
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : выставлено : "+DiagnSNames[i*8+j]);
                                        }
                                        else
                                        {
                                                Form1->MemoDiag->Font->Color = clLime;
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : снято : "+DiagnSNames[i*8+j]);
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
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : выставлено : "+DiagnNames[i*8+j]);
                                        else
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : снято : "+DiagnNames[i*8+j]);
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
//--Визуализация диалога оператора--//
void TForm1::VisualOperatorDlg()
{

    if(shr[3]==8)
    {
        APanel_String1 -> Caption = "Напуск завершен";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Загрузите кассету";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="ДА";
        APanel_NetBut -> Caption="НЕТ";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[3]==9)
    {
        APanel_String1 -> Caption = "Кассета загружена ?";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[3]==43)
    {
        APanel_String1 -> Caption = "Нет п/д в кассете";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Начать откачку шлюза";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="ДА";
        APanel_NetBut -> Caption="НЕТ";
        APanel -> Visible = true;
        pr_yel = 1;
    }

    else if(shr[5]==28)
    {
        APanel_String1 -> Caption = "Напуск завершён";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Выгрузите кассету";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[5]==29)
    {
        APanel_String1 -> Caption = "Начать откачку шлюза? ";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[26]==8)
    {
        APanel_String1 -> Caption = "Напуск в испарителе завершён";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Загрузите материал";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Caption = "Начать откачку?";
        APanel_String3 -> Visible = true;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[6]==29)
    {
        APanel_String1 -> Caption = "Напуск завершён";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Выгрузите кассету";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[6]==30)
    {
        APanel_String1 -> Caption = "Откачать шлюз?";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
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
//--Отображение всего перечня диагностик--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBdVallClick(TObject *Sender)
{
    PnlDiagm -> Visible = true;
}
//---------------------------------------------------------------------------
//--Скрытие всего перечня диагностик--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnDiagmClick(TObject *Sender)
{
    ((TButton*)Sender) -> Parent -> Visible = false;
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Запуск режимов--//
void __fastcall TForm1::PanelRCClick(TObject *Sender)
{  // очистить условия запуска
    ListBoxCondition -> Items -> Clear();
    // анализ условий запуска
    switch (StrToInt(((TPanel*)Sender)->Hint))
    {
        case 1:
        {
             LblRejim -> Caption = "Откачка камеры и испарителя";
             //Нет п/д на МГП
             if (  ( zin[5] & 0x1000 ) ) ListBoxCondition -> Items -> Add("П/д на механизме горизонтального перемещения");
             //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");
             //Испаритель зафиксирован
             if ( ! ( zin[3] & 0x400 ) ) ListBoxCondition -> Items -> Add("Испаритель не зафиксирован");
             //Есть охлаждение камеры
             if ( ! ( zin[0] & 0x01 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения камеры");
             //Есть охлаждение камеры испарителя
             if ( ! ( zin[0] & 0x02 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения камеры испарителя");
             //Есть охлаждения испарителя ЭЛ
             if ( ! ( zin[0] & 0x04 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения испарителя ЭЛ");
             //Есть охлаждение кварцевого датчика
             if ( ! ( zin[0] & 0x08 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения кварцевого датчика");
             //Есть охлаждение ТМН камеры
             if ( ! ( zin[0] & 0x10 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ТМН камеры");
             //Есть охлаждение ТМН шлюза
             if ( ! ( zin[0] & 0x20 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ТМН шлюза");
             //Есть охлаждение ТМН камеры испарителя
             if ( ! ( zin[0] & 0x40 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ТМН камеры испарителя");
             //Есть охлаждение ФВН шлюза
             if ( ! ( zin[0] & 0x80 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ФВН шлюза");
             //Есть охлаждение ФВН камеры и камеры испарителя
             if ( ! ( zin[0] & 0x100 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ФВН камеры и камеры испарителя");
             //Есть охлаждение верхней крышки камеры
             if ( ! ( zin[0] & 0x200 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения верхней крышки камеры");
             //Есть охлаждения корпуса нагревателя п/д
             if ( ! ( zin[0] & 0x400 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения корпуса нагревателя п/д");
             //Есть связь с Д1
             if ( diagnS[0]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д1");
             //Есть связь с Д2
             if ( diagnS[0]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д2");
             //Есть связь с Д3
             if ( diagnS[0]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д3");
             //Есть связь с Д4
             if ( diagnS[0]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д4");
             //Есть связь с Д5
             if ( diagnS[0]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д5");
             //Есть связь с Д6
             if ( diagnS[0]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д6");
             //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Есть связь с контроллером привода кассеты
             if ( diagnS[2]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода кассеты");
             //Есть связь с контроллером привода вертик. перем.
             if ( diagnS[2]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вертик. перем.");
             //Есть связь с контроллером привода гориз. перем.
             if ( diagnS[2]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода гориз. перем.");
             //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");
             //Есть связь с Термодат
             if ( diagnS[2]&0x80 ) ListBoxCondition -> Items -> Add("Нет связи с Термодат");
             // не запущен ни один режим
            for ( unsigned char i = 1 ; i < SHR_COUNT ; i++ )
                if(i!=27)           //кроме 27
                    if ( shr[i] ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
        }; break;


		case 3:
		{   LblRejim -> Caption = "Рабочий цикл";
            //Нет п/д на МГП
             if (  ( zin[5] & 0x1000 ) ) ListBoxCondition -> Items -> Add("П/д на механизме горизонтального перемещения");
             //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");
             //Испаритель зафиксирован
             if ( ! ( zin[3] & 0x400 ) ) ListBoxCondition -> Items -> Add("Испаритель не зафиксирован");
             //Есть охлаждение камеры
             if ( ! ( zin[0] & 0x01 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения камеры");
             //Есть охлаждение камеры испарителя
             if ( ! ( zin[0] & 0x02 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения камеры испарителя");
             //Есть охлаждения испарителя ЭЛ
             if ( ! ( zin[0] & 0x04 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения испарителя ЭЛ");
             //Есть охлаждение кварцевого датчика
             if ( ! ( zin[0] & 0x08 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения кварцевого датчика");
             //Есть охлаждение ТМН камеры
             if ( ! ( zin[0] & 0x10 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ТМН камеры");
             //Есть охлаждение ТМН шлюза
             if ( ! ( zin[0] & 0x20 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ТМН шлюза");
             //Есть охлаждение ТМН камеры испарителя
             if ( ! ( zin[0] & 0x40 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ТМН камеры испарителя");
             //Есть охлаждение ФВН шлюза
             if ( ! ( zin[0] & 0x80 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ФВН шлюза");
             //Есть охлаждение ФВН камеры и камеры испарителя
             if ( ! ( zin[0] & 0x100 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения ФВН камеры и камеры испарителя");
             //Есть охлаждение верхней крышки камеры
             if ( ! ( zin[0] & 0x200 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения верхней крышки камеры");
             //Есть охлаждения корпуса нагревателя п/д
             if ( ! ( zin[0] & 0x400 ) ) ListBoxCondition -> Items -> Add("Нет охлаждения корпуса нагревателя п/д");
             //Есть связь с Д1
             if ( diagnS[0]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д1");
             //Есть связь с Д2
             if ( diagnS[0]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д2");
             //Есть связь с Д3
             if ( diagnS[0]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д3");
             //Есть связь с Д4
             if ( diagnS[0]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д4");
             //Есть связь с Д5
             if ( diagnS[0]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д5");
             //Есть связь с Д6
             if ( diagnS[0]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д6");
             //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Есть связь с контроллером привода кассеты
             if ( diagnS[2]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода кассеты");
             //Есть связь с контроллером привода вертик. перем.
             if ( diagnS[2]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вертик. перем.");
             //Есть связь с контроллером привода гориз. перем.
             if ( diagnS[2]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода гориз. перем.");
             //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");
             //Есть связь с изм.толщины
             if ( diagnS[2]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с изм.толщины");
             //Есть связь с контроллером ЭЛУ
             if ( diagnS[2]&0x40 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером ЭЛУ");
             //Есть связь с Термодат
             if ( diagnS[2]&0x80 ) ListBoxCondition -> Items -> Add("Нет связи с Термодат");
             // задания не записаны
            if(!PR_PERPAR) { ListBoxCondition -> Items -> Add("Задание вращения стола не передано"); }
             // не запущен ни один режим
            for ( unsigned char i = 1 ; i < SHR_COUNT ; i++ )
                if(i!=27)           //кроме 27
                    if ( shr[i] ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
		}; break;
        case 5:
        {
             LblRejim -> Caption = "Сброс РЦ";
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Запущен режим 3
            if ( !(shr[3]) ) ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[3]);
        }; break;
        case 6:
		{   LblRejim -> Caption = "Выгрузка п/д";

             //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");

             //Есть связь с Д1
             if ( diagnS[0]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д1");
             //Есть связь с Д2
             if ( diagnS[0]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д2");
             //Есть связь с Д3
             if ( diagnS[0]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д3");
             //Есть связь с Д4
             if ( diagnS[0]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д4");
             //Есть связь с Д5
             if ( diagnS[0]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д5");
             //Есть связь с Д6
             if ( diagnS[0]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д6");
             //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Есть связь с контроллером привода кассеты
             if ( diagnS[2]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода кассеты");
             //Есть связь с контроллером привода вертик. перем.
             if ( diagnS[2]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вертик. перем.");
             //Есть связь с контроллером привода гориз. перем.
             if ( diagnS[2]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода гориз. перем.");
             //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             // не запущен ни один режим
            for ( unsigned char i = 1 ; i < SHR_COUNT ; i++ )
                if(i!=27)           //кроме 27
                    if ( shr[i] ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
        }; break;
       case 7:
		{   LblRejim -> Caption = "Отключение установки";
            //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");


             // не запущен ни один режим
            for ( unsigned char i = 1 ; i < SHR_COUNT ; i++ )
                if((i!=27)&&(i!=1))           //кроме 27  и 1
                    if ( shr[i] ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
		}; break;
         case 9:
		{   LblRejim -> Caption = "Включить транспортный тест";
            //ЩЗ открыт
             if (  ( zin[3] & 0x30 )!=0x10 ) ListBoxCondition -> Items -> Add("ЩЗ не открыт");
             //ПГП в HOME
             if ( ! ( zin[4] & 0x10 ) ) ListBoxCondition -> Items -> Add("ПГП не в HOME");
             //ПВП в HOME
             if ( ! ( zin[4] & 0x400 ) ) ListBoxCondition -> Items -> Add("ПВП не в HOME");
             //Вращ в HOME
             if ( ! ( zin[4] & 0x02 ) ) ListBoxCondition -> Items -> Add("Привод вращения не в HOME");
            //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Есть связь с контроллером привода кассеты
             if ( diagnS[2]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода кассеты");
             //Есть связь с контроллером привода вертик. перем.
             if ( diagnS[2]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вертик. перем.");
             //Есть связь с контроллером привода гориз. перем.
             if ( diagnS[2]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода гориз. перем.");
             //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");
		}; break;
        case 109:
		{   LblRejim -> Caption = "Отключить транспортный тест";
		}; break;
       	case 100:
		{   LblRejim -> Caption = "Сброс";
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
		}; break;
        case 10:
		{   LblRejim -> Caption = "Открыть ЩЗ Шлюза";
            //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");

             //Есть связь с Д1
             if ( diagnS[0]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д1");
             //Есть связь с Д2
             if ( diagnS[0]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д2");
             //Есть связь с Д3
             if ( diagnS[0]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д3");
             //Есть связь с Д4
             if ( diagnS[0]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д4");
             //Есть связь с Д5
             if ( diagnS[0]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д5");
             //Есть связь с Д6
             if ( diagnS[0]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д6");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 10
            if ( (shr[10]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[10]);

		}; break;
        case 11:
		{   LblRejim -> Caption = "Закрыть ЩЗ Шлюза";
            //ПГП в HOME
             if ( ! ( zin[4] & 0x10 ) ) ListBoxCondition -> Items -> Add("ПГП не в HOME");

             //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");

            //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 11
            if ( (shr[11]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[11]);


		}; break;
        case 12:
        {   LblRejim -> Caption = "Открыть затвор Испарителя";
            //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");

             //Есть связь с Д1
             if ( diagnS[0]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д1");
             //Есть связь с Д2
             if ( diagnS[0]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д2");
             //Есть связь с Д3
             if ( diagnS[0]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д3");
             //Есть связь с Д4
             if ( diagnS[0]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д4");
             //Есть связь с Д5
             if ( diagnS[0]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д5");
             //Есть связь с Д6
             if ( diagnS[0]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д6");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 12
            if ( (shr[12]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]);
        }; break;


        case 13:
        {   LblRejim -> Caption = "Закрыть затвор Испарителя";
            //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 13
            if ( (shr[13]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]);
        }; break;

        case 14:
        {   LblRejim -> Caption = "Кассета в HOME";
            //Есть связь с контроллером привода кассеты
             if ( diagnS[2]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода кассеты");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;

        case 15:
        {   LblRejim -> Caption = "Кассета вверх/вниз";
            //Есть связь с контроллером привода кассеты
             if ( diagnS[2]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода кассеты");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;

        case 16:
        {   LblRejim -> Caption = "Привод горизонтального перемещения в HOME";
            //Есть связь с контроллером привода гориз. перем.
             if ( diagnS[2]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода гориз. перем.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;

        case 17:
        {   LblRejim -> Caption = "Привод горизонтального перемещения вперёд/назад";
            //Есть связь с контроллером привода гориз. перем.
             if ( diagnS[2]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода гориз. перем.");

             //ЩЗ открыт
             if (  ( zin[3] & 0x30 )!=0x10 ) ListBoxCondition -> Items -> Add("ЩЗ не открыт");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);

            //Не запущен режим 11
            if ( (shr[11]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[11]);
        }; break;

        case 18:
        {   LblRejim -> Caption = "Привод вертикального перемещения в HOME";
            //Есть связь с контроллером привода вертик. перем.
             if ( diagnS[2]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вертик. перем.");

             //ПГП в HOME
             //if ( ! ( zin[4] & 0x10 ) ) ListBoxCondition -> Items -> Add("ПГП не в HOME");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;

        case 19:
        {   LblRejim -> Caption = "Привод вертикального перемещения вверх/вниз";
            //Есть связь с контроллером привода вертик. перем.
             if ( diagnS[2]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вертик. перем.");

             //ПГП в HOME
             //if ( ! ( zin[4] & 0x10 ) ) ListBoxCondition -> Items -> Add("ПГП не в HOME");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);


        }; break;

        case 20:
        {   LblRejim -> Caption = "Привод вращения п/д ?";
            //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;

        case 21:
        {   LblRejim -> Caption = "Привод вращения п/д вперёд";
            //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;

        case 22:
        {   LblRejim -> Caption = "Привод поворота датчиков в HOME";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);


        }; break;

        case 23:
        {   LblRejim -> Caption = "Привод поворота датчиков вперёд/назад";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);



            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);

        }; break;

        case 24:
        {   LblRejim -> Caption = "Привод вращения п/д (бесконечное)";

            //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
           /* //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);*/
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
            // задания не записаны
            if(!PR_PERPAR) { ListBoxCondition -> Items -> Add("Задание вращения стола не передано"); }
        }; break;

		case 101:
		{   LblRejim -> Caption = "Стоп механизмов";  // STOP механизмов -- длинная красная кнопка
        }; break;

        case 26:
        {   LblRejim -> Caption = "Загрузка материала";
            //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");

             //Есть связь с Д1
             if ( diagnS[0]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д1");
             //Есть связь с Д2
             if ( diagnS[0]&0x02 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д2");
             //Есть связь с Д3
             if ( diagnS[0]&0x04 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д3");
             //Есть связь с Д4
             if ( diagnS[0]&0x08 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д4");
             //Есть связь с Д5
             if ( diagnS[0]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д5");
             //Есть связь с Д6
             if ( diagnS[0]&0x20 ) ListBoxCondition -> Items -> Add("Нет связи с вакуумным датчиком Д6");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);

        }; break;

        case 27:
        {   LblRejim -> Caption = "Нагрев Шлюза (Вкл.)";
            //Есть связь с Термодат
             if ( diagnS[2]&0x80 ) ListBoxCondition -> Items -> Add("Нет связи с Термодат");
             //Не запущен режим 27
            if ( (shr[27]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[27]);
        }; break;

        case 28:
        {   LblRejim -> Caption = "Нагрев Шлюза (Откл.)";
            //Есть связь с Термодат
             if ( diagnS[2]&0x80 ) ListBoxCondition -> Items -> Add("Нет связи с Термодат");
             //Запущен режим 27
            if (! (shr[27]) ) ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[27]);
        }; break;

        case 29:
        {   LblRejim -> Caption = "Нагрев п/д (Вкл.)";
            //Есть связь с Термодат
             if ( diagnS[2]&0x80 ) ListBoxCondition -> Items -> Add("Нет связи с Термодат");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим 29
            if ( (shr[29]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[29]);
        }; break;

        case 30:
        {   LblRejim -> Caption = "Нагрев п/д (Откл.)";
            //Есть связь с Термодат
             if ( diagnS[2]&0x80 ) ListBoxCondition -> Items -> Add("Нет связи с Термодат");
             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Запущен режим 29
            if ( !(shr[29]) ) ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[29]);
        }; break;

        case 31:
        {   LblRejim -> Caption = "Привод вращения п/д (откл.)";
            //Есть связь с контроллером привода вращения
             if ( diagnS[2]&0x01 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода вращения");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
          /*  //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); */
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 14
            if ( (shr[14]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]);
            //Не запущен режим 15
            if ( (shr[15]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]);
            //Не запущен режим 16
            if ( (shr[16]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]);
            //Не запущен режим 17
            if ( (shr[17]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]);
            //Не запущен режим 18
            if ( (shr[18]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]);
            //Не запущен режим 19
            if ( (shr[19]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[19]);
            //Не запущен режим 20
            if ( (shr[20]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]);
            //Не запущен режим 21
            if ( (shr[21]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]);
            //Не запущен режим 24
            if ( (shr[24]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]);
            //Не запущен режим 31
            if ( (shr[31]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]);
        }; break;
        case 32:
        {   LblRejim -> Caption = "Выбор датчика толщины 1";
        //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
        /*    //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);   */
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);
        }; break;
        case 33:
        {   LblRejim -> Caption = "Выбор датчика толщины 2";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
        /*    //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);     */
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);
        }; break;
        case 34:
        {   LblRejim -> Caption = "Выбор датчика толщины 3";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
        /*    //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);*/
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);
        }; break;
        case 35:
        {   LblRejim -> Caption = "Выбор датчика толщины 4";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
          /*  //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); */
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);
        }; break;
        case 36:
        {   LblRejim -> Caption = "Выбор датчика толщины 5";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
          /*  //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);  */
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);
        }; break;
        case 37:
        {   LblRejim -> Caption = "Выбор датчика толщины 6";
            //Есть связь с контроллером привода поворота датч.
             if ( diagnS[2]&0x10 ) ListBoxCondition -> Items -> Add("Нет связи с контроллером привода поворота датч.");

             //Не запущен режим 1
            if ( (shr[1]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]);
            //Не запущен режим 2
            if ( (shr[2]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]);
          /*  //Не запущен режим 3
            if ( (shr[3]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]);*/
            //Не запущен режим 4
            if ( (shr[4]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]);
            //Не запущен режим 5
            if ( (shr[5]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]);
            //Не запущен режим 6
            if ( (shr[6]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]);
            //Не запущен режим 7
            if ( (shr[7]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]);
            //Не запущен режим 8
            if ( (shr[8]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]);
            //Не запущен режим 9
            if ( (shr[9]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]);
            //Не запущен режим 25
            if ( (shr[25]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]);
            //Не запущен режим 26
            if ( (shr[26]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]);
            //Не запущен режим
            if ( (shr[3])&&(!PR_NALADKA) ) ListBoxCondition -> Items -> Add("Наладочный режим запрещен");
            //Не запущен режим 22
            if ( (shr[22]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]);
            //Не запущен режим 23
            if ( (shr[23]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[23]);
            //Не запущен режим 32
            if ( (shr[32]) ) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[32]);
        }; break;

        case 39:
        {   LblRejim -> Caption = "Чиллер охлаждения (Вкл.)";
            //Есть удалённое управление
             if ( ! ( zin[1] & 0x200 ) ) ListBoxCondition -> Items -> Add("Нет удалённого управления чиллера охлаждения");
        }; break;

        case 40:
        {   LblRejim -> Caption = "Чиллер охлаждения (Откл.)";
            //Есть удалённое управление
             if ( ! ( zin[1] & 0x200 ) ) ListBoxCondition -> Items -> Add("Нет удалённого управления чиллера охлаждения");
        }; break;

        case 41:
        {   LblRejim -> Caption = "Чиллер нагрева (Вкл.)";
        }; break;

        case 42:
        {   LblRejim -> Caption = "Чиллер нагрева (Откл.)";
        }; break;

        case 43:
        {   LblRejim -> Caption = "Подключение камеры слежения за расплавом";
            //Есть давление в пневмосети
             if ( ! ( zin[0] & 0x8000 ) ) ListBoxCondition -> Items -> Add("Нет давления в пневмосети");
        }; break;

        case 213:
        {   LblRejim -> Caption = "Сброс аварии механизмов";
        }; break;

        default: return;     // невозможная команда
        }
    // подтверждать удаление предыдущего файла
    if ( StrToInt(((TPanel*)Sender)->Hint) != 140 )
        if ( MessageDlg("Подтверждаете запуск режима: " + LblRejim -> Caption + "?", mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) != mrYes ) return;
    // если не выполнены условия запуска
    if ( ListBoxCondition -> Items -> Count )
        PnlCondition -> Visible = true;
    // если все условия выполнены передать команду
    else
    {
        // передать код команды
        qkk = StrToInt(((TPanel*)Sender)->Hint);
        // запомнили действие в журнал
        MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time->Caption + ": Запущен режим: <" + LblRejim -> Caption + ">" );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Действия, выполняемые при закрытии приложения--//
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	// остановить и выключить параллельные потоки
	TimerExist  -> Terminate();
	LogicThread -> Terminate();

	// сохранение архивов
	Save_Stat();
    SaveGasData();

	// отключение плат
    PISODIO_DriverClose();
    ISO_DriverClose();
    PISO813_DriverClose();
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
// сохранение статистики, привязанной к дате
void TForm1::Save_Stat()
{
    // сохранили архив
    AnsiString fileName;

    // диагностики возникшие в процессе работы
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

    // запомнили действие в журнал
    MemoStat -> Lines -> Add( "<<< " + Label_Time -> Caption + " | Программа завершена >>>");

    // действия пользователя в процессе работы
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

    // графики текущих переменных процесса
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
//--Отлавливаем управление ВЧГ кнопками--//
//---------------------------------------------------------------------------
#define kodKlL 37
#define kodKlV 38
#define kodKlP 39
#define kodKlN 40
//---------------------------------------------------------------------------
//--Отлавливаем управление ВЧГ кнопками--//
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
                Comport[0]->BTN_reset->Caption = "Стоп порта";
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

	// Установка загружена и порт включен
	if(!(Comport[0]->State)||!ust_ready) return;

	// Отображение приема/передачи
	Comport[0]->RB_prd->Checked = !(Comport[0]->DevState);
	Comport[0]->RB_prm->Checked = Comport[0]->DevState;

    /*
    // Есть ручное задание
	    if((DZaslVAT[0]->RCom)&&(!(Comport[0]->DevState))) Comport[0]->PortTask |= 0x100;
	    if(!(Comport[0]->PortTask)&&!(Comport[0]->Pr_nal)) Comport[0]->PortTask |= 0x01; // Обновляем автоматическое задание


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
	    }  */
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
            Comport[0]->BTN_reset->Caption = "Пуск порта";
            Comport[0]->port_ct = 0;
            }
            // ShowMessage("Обнаружена ошибка. Com1 отключен!");
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
                Comport[1]->BTN_reset->Caption = "Стоп порта";
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

	// Установка загружена и порт включен
	if(!(Comport[1]->State)||!ust_ready) return;

	// Отображение приема/передачи
	Comport[1]->RB_prd->Checked = !(Comport[1]->DevState);
	Comport[1]->RB_prm->Checked = Comport[1]->DevState;

    /*
    // Есть ручное задание
	    if((DZaslVAT[1]->RCom)&&(!(Comport[1]->DevState))) Comport[1]->PortTask |= 0x100;
	    if(!(Comport[1]->PortTask)&&!(Comport[1]->Pr_nal)) Comport[1]->PortTask |= 0x01; // Обновляем автоматическое задание


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
	    }        */
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
			            Comport[1]->BTN_reset->Caption = "Пуск порта";
                        Comport[1]->port_ct = 0;
                }
                // ShowMessage("Обнаружена ошибка. Com1 отключен!");
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
                Comport[2]->BTN_reset->Caption = "Стоп порта";
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

	// Установка загружена и порт включен
	if(!(Comport[2]->State)||!ust_ready) return;

	// Отображение приема/передачи
	Comport[2]->RB_prd->Checked = !(Comport[2]->DevState);
	Comport[2]->RB_prm->Checked = Comport[2]->DevState;

	if(!(Comport[2]->PortTask)&&!(Comport[2]->Pr_nal)) Comport[2]->PortTask |= 0x7f; // Обновляем автоматическое задание

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
		Comport[2]->DevErr = Dat_PPT200[2]->DatPPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_PPT200[2]->diagnS_byte] |= Dat_PPT200[2]->diagnS_mask : diagnS[Dat_PPT200[2]->diagnS_byte] &= (~Dat_PPT200[2]->diagnS_mask);
			Comport[2]->PortTask &= (~0x04);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x08)
	{
		Comport[2]->DevErr = Dat_MPT200[0]->DatMPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_MPT200[0]->diagnS_byte] |= Dat_MPT200[0]->diagnS_mask : diagnS[Dat_MPT200[0]->diagnS_byte] &= (~Dat_MPT200[0]->diagnS_mask);
			Comport[2]->PortTask &= (~0x08);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x10)
	{
		Comport[2]->DevErr = Dat_MPT200[1]->DatMPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_MPT200[1]->diagnS_byte] |= Dat_MPT200[1]->diagnS_mask : diagnS[Dat_MPT200[1]->diagnS_byte] &= (~Dat_MPT200[1]->diagnS_mask);
			Comport[2]->PortTask &= (~0x10);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x20)
	{
		Comport[2]->DevErr = Dat_MPT200[2]->DatMPT200_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[Dat_MPT200[2]->diagnS_byte] |= Dat_MPT200[2]->diagnS_mask : diagnS[Dat_MPT200[2]->diagnS_byte] &= (~Dat_MPT200[2]->diagnS_mask);
			Comport[2]->PortTask &= (~0x20);
			Comport[2]->DevState = 0;
		}
		return;
	}
    else if(Comport[2]->PortTask & 0x40)
	{
		Comport[2]->DevErr = TRMD[0]->TRMD_Manage(Comport[2]->DevState,0);
		if(Comport[2]->DevState > 1)
		{
			Comport[2]->DevErr ? diagnS[TRMD[0]->diagnS_byte] |= TRMD[0]->diagnS_mask : diagnS[TRMD[0]->diagnS_byte] &= (~TRMD[0]->diagnS_mask);
			Comport[2]->PortTask &= (~0x40);
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
			            Comport[2]->BTN_reset->Caption = "Пуск порта";
                        Comport[2]->port_ct = 0;
                }
                // ShowMessage("Обнаружена ошибка. Com1 отключен!");
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
                Comport[3]->BTN_reset->Caption = "Стоп порта";
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

	// Установка загружена и порт включен
	if(!(Comport[3]->State)||!ust_ready) return;

	// Отображение приема/передачи
	Comport[3]->RB_prd->Checked = !(Comport[3]->DevState);
	Comport[3]->RB_prm->Checked = Comport[3]->DevState;

    if(!(Comport[3]->Pr_nal) && !(Comport[3]->DevState))
    {
        if(*AZ_drive[0]->Pr_AZ ||
        *AZ_drive[1]->Pr_AZ ||
        *AZ_drive[2]->Pr_AZ ||
        *AZ_drive[3]->Pr_AZ ||
        *AZ_drive[4]->Pr_AZ)
        {
            if(!(*AZ_drive[0]->Pr_AZ)) Comport[3]->PortTask &= (~0x01);
            if(!(*AZ_drive[1]->Pr_AZ)) Comport[3]->PortTask &= (~0x02);
            if(!(*AZ_drive[2]->Pr_AZ)) Comport[3]->PortTask &= (~0x04);
            if(!(*AZ_drive[3]->Pr_AZ)) Comport[3]->PortTask &= (~0x08);
            if(!(*AZ_drive[4]->Pr_AZ)) Comport[3]->PortTask &= (~0x10);
            if(Comport[3]->PortTask == 0)
            {
                if(*AZ_drive[0]->Pr_AZ) Comport[3]->PortTask |= 0x01;
                if(*AZ_drive[1]->Pr_AZ) Comport[3]->PortTask |= 0x02;
                if(*AZ_drive[2]->Pr_AZ) Comport[3]->PortTask |= 0x04;
                if(*AZ_drive[3]->Pr_AZ) Comport[3]->PortTask |= 0x08;
                if(*AZ_drive[4]->Pr_AZ) Comport[3]->PortTask |= 0x10;
            }
        }
        else
        {
            if(!(Comport[3]->PortTask)) Comport[3]->PortTask |= 0x1f;	// 5 устройства
        }
    }

    if(Comport[3]->PortTask & 0x01)
	{
		Comport[3]->DevErr = AZ_drive[0]->AZ_manage(Comport[3]->DevState);
		if(Comport[3]->DevState > 1)
		{
			Comport[3]->DevErr ? diagnS[AZ_drive[0]->diagnS_byte] |= AZ_drive[0]->diagnS_mask : diagnS[AZ_drive[0]->diagnS_byte] &= (~AZ_drive[0]->diagnS_mask);
			Comport[3]->PortTask &= (~0x01);
			Comport[3]->DevState = 0;
		}
		return;
	}
    else if(Comport[3]->PortTask & 0x02)
	{
		Comport[3]->DevErr = AZ_drive[1]->AZ_manage(Comport[3]->DevState);
		if(Comport[3]->DevState > 1)
		{
			Comport[3]->DevErr ? diagnS[AZ_drive[1]->diagnS_byte] |= AZ_drive[1]->diagnS_mask : diagnS[AZ_drive[1]->diagnS_byte] &= (~AZ_drive[1]->diagnS_mask);
			Comport[3]->PortTask &= (~0x02);
			Comport[3]->DevState = 0;
		}
		return;
	}
    else if(Comport[3]->PortTask & 0x04)
	{
		Comport[3]->DevErr = AZ_drive[2]->AZ_manage(Comport[3]->DevState);
		if(Comport[3]->DevState > 1)
		{
			Comport[3]->DevErr ? diagnS[AZ_drive[2]->diagnS_byte] |= AZ_drive[2]->diagnS_mask : diagnS[AZ_drive[2]->diagnS_byte] &= (~AZ_drive[2]->diagnS_mask);
			Comport[3]->PortTask &= (~0x04);
			Comport[3]->DevState = 0;
		}
		return;
	}
    else if(Comport[3]->PortTask & 0x08)
	{
		Comport[3]->DevErr = AZ_drive[3]->AZ_manage(Comport[3]->DevState);
		if(Comport[3]->DevState > 1)
		{
			Comport[3]->DevErr ? diagnS[AZ_drive[3]->diagnS_byte] |= AZ_drive[3]->diagnS_mask : diagnS[AZ_drive[3]->diagnS_byte] &= (~AZ_drive[3]->diagnS_mask);
			Comport[3]->PortTask &= (~0x08);
			Comport[3]->DevState = 0;
		}
		return;
	}
    else if(Comport[3]->PortTask & 0x10)
	{
		Comport[3]->DevErr = AZ_drive[4]->AZ_manage(Comport[3]->DevState);
		if(Comport[3]->DevState > 1)
		{
			Comport[3]->DevErr ? diagnS[AZ_drive[4]->diagnS_byte] |= AZ_drive[4]->diagnS_mask : diagnS[AZ_drive[4]->diagnS_byte] &= (~AZ_drive[4]->diagnS_mask);
			Comport[3]->PortTask &= (~0x10);
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
			            Comport[3]->BTN_reset->Caption = "Пуск порта";
                        Comport[3]->port_ct = 0;
                }
                // ShowMessage("Обнаружена ошибка. Com1 отключен!");
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
}
//---------------------------------------------------------------------------
// ввод значения переменной
void __fastcall TForm1::BtnOtl1Click(TObject *Sender)
{       
	// выставили значения переменных на странице
	switch ( StrToInt ( PCPerem -> ActivePage -> Hint ) )
	{
     //0 страница
case 0:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :sh_         = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :shr[1]       = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :sh[1]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :shr[2]       = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :sh[2]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :shr[3]       = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :sh[3]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :shr[4]       = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :sh[4]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:shr[5]       = StrToInt(EditOTLzad10->Text); break;
		case 11:sh[5]      = StrToInt(EditOTLzad11->Text); break;
		case 12:shr[6]       = StrToInt(EditOTLzad12->Text); break;
		case 13:sh[6]      = StrToInt(EditOTLzad13->Text); break;
		case 14:shr[7]       = StrToInt(EditOTLzad14->Text); break;
		case 15:sh[7]      = StrToInt(EditOTLzad15->Text); break;
		case 16:shr[8]       = StrToInt(EditOTLzad16->Text); break;
		case 17:sh[8]      = StrToInt(EditOTLzad17->Text); break;
		case 18:shr[9]       = StrToInt(EditOTLzad18->Text); break;
		case 19:sh[9]      = StrToInt(EditOTLzad19->Text); break;
		case 20:shr[10]      = StrToInt(EditOTLzad20->Text); break;
		case 21:sh[10]     = StrToInt(EditOTLzad21->Text); break;
		case 22:shr[11]      = StrToInt(EditOTLzad22->Text); break;
		case 23:shr[11]     = StrToInt(EditOTLzad23->Text); break;
		case 24:shr[12]      = StrToInt(EditOTLzad24->Text); break;
		case 25:sh[12]     = StrToInt(EditOTLzad25->Text); break;
		case 26:shr[13]      = StrToInt(EditOTLzad26->Text); break;
		case 27:sh[13]     = StrToInt(EditOTLzad27->Text); break;
		case 28:shr[14]      = StrToInt(EditOTLzad28->Text); break;
		case 29:sh[14]     = StrToInt(EditOTLzad29->Text); break;
//		case 30:            = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //1 страница
case 1:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :shr[15]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :sh[15]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :shr[16]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :sh[16]     = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :shr[17]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :sh[17]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :shr[18]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :sh[18]     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :shr[19]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:sh[19]     = StrToInt(EditOTLzad10->Text); break;
		case 11:shr[20]      = StrToInt(EditOTLzad11->Text); break;
		case 12:sh[20]     = StrToInt(EditOTLzad12->Text); break;
		case 13:shr[21]      = StrToInt(EditOTLzad13->Text); break;
		case 14:sh[21]     = StrToInt(EditOTLzad14->Text); break;
		case 15:shr[22]      = StrToInt(EditOTLzad15->Text); break;
		case 16:sh[22]     = StrToInt(EditOTLzad16->Text); break;
		case 17:shr[23]      = StrToInt(EditOTLzad17->Text); break;
		case 18:sh[23]     = StrToInt(EditOTLzad18->Text); break;
		case 19:shr[24]      = StrToInt(EditOTLzad19->Text); break;
		case 20:sh[24]     = StrToInt(EditOTLzad20->Text); break;
		case 21:shr[25]      = StrToInt(EditOTLzad21->Text); break;
		case 22:sh[25]     = StrToInt(EditOTLzad22->Text); break;
		case 23:shr[26]      = StrToInt(EditOTLzad23->Text); break;
		case 24:sh[26]     = StrToInt(EditOTLzad24->Text); break;
		case 25:shr[27]      = StrToInt(EditOTLzad25->Text); break;
		case 26:sh[27]     = StrToInt(EditOTLzad26->Text); break;
		case 27:shr[28]      = StrToInt(EditOTLzad27->Text); break;
		case 28:sh[28]     = StrToInt(EditOTLzad28->Text); break;
		case 29:shr[29]      = StrToInt(EditOTLzad29->Text); break;
		case 30:sh[29]     = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
 //2 страница
case 2:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :shr[30]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :sh[30]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :shr[31]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :sh[31]     = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :shr[32]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :sh[32]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :shr[36]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :sh[36]     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :shr[37]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:sh[37]     = StrToInt(EditOTLzad10->Text); break;
		case 11:shr[38]      = StrToInt(EditOTLzad11->Text); break;
		case 12:sh[38]     = StrToInt(EditOTLzad12->Text); break;
		case 13:shr[43]      = StrToInt(EditOTLzad13->Text); break;
		case 14:sh[43]     = StrToInt(EditOTLzad14->Text); break;
//		case 15:      = StrToInt(EditOTLzad15->Text); break;
//		case 16:     = StrToInt(EditOTLzad16->Text); break;
//		case 17:      = StrToInt(EditOTLzad17->Text); break;
//		case 18:     = StrToInt(EditOTLzad18->Text); break;
//		case 19:     = StrToInt(EditOTLzad19->Text); break;
//		case 20:     = StrToInt(EditOTLzad20->Text); break;
//		case 21:      = StrToInt(EditOTLzad21->Text); break;
//		case 22:    = StrToInt(EditOTLzad22->Text); break;
//		case 23:      = StrToInt(EditOTLzad23->Text); break;
//		case 24:     = StrToInt(EditOTLzad24->Text); break;
//		case 25:      = StrToInt(EditOTLzad25->Text); break;
//		case 26:     = StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
		case 28:zshr3        = StrToInt(EditOTLzad28->Text); break;
		case 29:norma       = StrToInt(EditOTLzad29->Text); break;
		case 30:qkk         = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //3 страница
case 3:
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
//		case 30:   = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //4 страница
case 4:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :diagnS[0]  = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :diagnS[1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :diagnS[2]   = StrToInt(EditOTLzad3->Text) ; break;
//		case 4 :   = StrToInt(EditOTLzad4->Text) ; break;
//		case 5 :   = StrToInt(EditOTLzad5->Text) ; break;
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
     //5 страница
case 5:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :out[0]      = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :out[1]      = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :out[2]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :out[3]      = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :out[4]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :out[5]      = StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
		case 8 :zin[0]      = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :zin[1]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:zin[2]      = StrToInt(EditOTLzad10->Text); break;
		case 11:zin[3]      = StrToInt(EditOTLzad11->Text); break;
		case 12:zin[4]      = StrToInt(EditOTLzad12->Text); break;
		case 13:zin[5]= StrToInt(EditOTLzad13->Text); break;
//		case 14:     = StrToInt(EditOTLzad14->Text); break;
		case 15:aik[0]      = StrToInt(EditOTLzad15->Text); break;
		case 16:aik[1]      = StrToInt(EditOTLzad16->Text); break;
		case 17:aik[2]      = StrToInt(EditOTLzad17->Text); break;
		case 18:aik[3]      = StrToInt(EditOTLzad18->Text); break;
		case 19:aik[4]      = StrToInt(EditOTLzad19->Text); break;
		case 20:aik[5]      = StrToInt(EditOTLzad20->Text); break;
		case 21:aik[6]      = StrToInt(EditOTLzad21->Text); break;
		case 22:aik[7]      = StrToInt(EditOTLzad22->Text); break;
		case 23:aik[8]      = StrToInt(EditOTLzad23->Text); break;
		case 24:aik[9]     = StrToInt(EditOTLzad24->Text); break;
		case 25:aik[10]     = StrToInt(EditOTLzad25->Text); break;
		case 26:aik[11]     = StrToInt(EditOTLzad26->Text); break;
		case 27:aik[12]     = StrToInt(EditOTLzad27->Text); break;
		case 28:aik[13]     = StrToInt(EditOTLzad28->Text); break;
		case 29:aik[14]     = StrToInt(EditOTLzad29->Text); break;
	   	case 30:aik[15]     = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //6 страница
case 6:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :aout[0]     = StrToInt(EditOTLzad1->Text) ; break;
		case 2 :aout[1]     = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :aout[2]     = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :aout[3]     = StrToInt(EditOTLzad4->Text) ; break;
//		case 5 :     = StrToInt(EditOTLzad5->Text) ; break;
//		case 6 :     = StrToInt(EditOTLzad6->Text) ; break;
//		case 7 :     = StrToInt(EditOTLzad7->Text) ; break;
//		case 8 :     = StrToInt(EditOTLzad8->Text) ; break;
//		case 9 :     = StrToInt(EditOTLzad9->Text) ; break;
//		case 10:= StrToInt(EditOTLzad10->Text); break;
		case 11:D_D1        = StrToInt(EditOTLzad11->Text); break;
		case 12:D_D2        = StrToInt(EditOTLzad12->Text); break;
		case 13:D_D3        = StrToInt(EditOTLzad13->Text); break;
		case 14:D_D4        = StrToInt(EditOTLzad14->Text); break;
		case 15:D_D5        = StrToInt(EditOTLzad15->Text); break;
		case 16:D_D6        = StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
		case 18:UVAK_KAM     = StrToInt(EditOTLzad18->Text); break;
		case 19:UVAKN_TMN   = StrToInt(EditOTLzad19->Text); break;
		case 20:UVAKV_TMN   = StrToInt(EditOTLzad20->Text); break;
		case 21:UVAK_SHL    = StrToInt(EditOTLzad21->Text); break;
		case 22:UVAKV_SHL     = StrToInt(EditOTLzad22->Text); break;
		case 23:UVAKN_SHL    = StrToInt(EditOTLzad23->Text); break;
		case 24:UVAK_SHL_MO     = StrToInt(EditOTLzad24->Text); break;
		case 25:UVAK_KAM_MO = StrToInt(EditOTLzad25->Text); break;
		case 26:UVVAK_KAM  = StrToInt(EditOTLzad26->Text); break;
		case 27:UVAKN_ISP   = StrToInt(EditOTLzad27->Text); break;
		case 28:UVAKV_ISP= StrToInt(EditOTLzad28->Text); break;
		case 29:UVAK_ISP_MO= StrToInt(EditOTLzad29->Text); break;
		case 30:UATM= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //7 страница
case 7:
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
//		case 18:  = StrToInt(EditOTLzad18->Text); break;
//		case 19:  = StrToInt(EditOTLzad19->Text); break;
//		case 20:  = StrToInt(EditOTLzad20->Text); break;
//		case 21:  = StrToInt(EditOTLzad21->Text); break;
//		case 22:  = StrToInt(EditOTLzad22->Text); break;
//		case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:par_t[0]    = StrToInt(EditOTLzad25->Text); break;
		case 26:par_t[1]    = StrToInt(EditOTLzad26->Text); break;
		case 27:par_t[2]    = StrToInt(EditOTLzad27->Text); break;
		case 28:par_t[3]    = StrToInt(EditOTLzad28->Text); break;
		case 29:par_t[4]    = StrToInt(EditOTLzad29->Text); break;
		case 30:par_t[5]    = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //8 страница
case 8:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[0][1]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[0][9]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[0][10]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[0][11]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[0][12]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[0][13]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[0][14]= StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[1][1]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[1][2]= StrToInt(EditOTLzad10->Text); break;
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
     //9 страница
case 9:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[2][2]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[2][3]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[2][4]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[2][5]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[2][6]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[2][7]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[2][8]= StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[3][2]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[3][3]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[3][4]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[3][5]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[3][6]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[3][7]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[3][8]= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:par[4][2]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[4][3]= StrToInt(EditOTLzad18->Text); break;
		case 19:par[4][4]= StrToInt(EditOTLzad19->Text); break;
		case 20:par[4][5]= StrToInt(EditOTLzad20->Text); break;
		case 21:par[4][6]= StrToInt(EditOTLzad21->Text); break;
		case 22:par[4][7]= StrToInt(EditOTLzad22->Text); break;
		case 23:par[4][8]= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //10 страница
case 10:
{
switch (StrToInt(((TButton*)Sender)->Hint))
		{
		case 1 :par[5][2]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[5][3]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[5][4]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[5][5]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[5][6]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[5][7]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[5][8]= StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[6][2]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[6][3]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[6][4]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[6][5]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[6][6]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[6][7]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[6][8]= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:par[7][2]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[7][3]= StrToInt(EditOTLzad18->Text); break;
		case 19:par[7][4]= StrToInt(EditOTLzad19->Text); break;
		case 20:par[7][5]= StrToInt(EditOTLzad20->Text); break;
		case 21:par[7][6]= StrToInt(EditOTLzad21->Text); break;
		case 22:par[7][7]= StrToInt(EditOTLzad22->Text); break;
		case 23:par[7][8]= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //11 страница
case 11:
{
switch (StrToInt(((TButton*)Sender)->Hint))
		{
		case 1 :par[8][2]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[8][3]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[8][4]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[8][5]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[8][6]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[8][7]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[8][8]= StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[9][2]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[9][3]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[9][4]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[9][5]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[9][6]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[9][7]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[9][8]= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:par[10][2]= StrToInt(EditOTLzad17->Text); break;
		case 18:par[10][3]= StrToInt(EditOTLzad18->Text); break;
		case 19:par[10][4]= StrToInt(EditOTLzad19->Text); break;
		case 20:par[10][5]= StrToInt(EditOTLzad20->Text); break;
		case 21:par[10][6]= StrToInt(EditOTLzad21->Text); break;
		case 22:par[10][7]= StrToInt(EditOTLzad22->Text); break;
		case 23:par[10][8]= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //12 страница
case 12:
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
		case 12:CT_25= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_26= StrToInt(EditOTLzad13->Text); break;
		case 14:CT_27= StrToInt(EditOTLzad14->Text); break;
		case 15:CT27K1= StrToInt(EditOTLzad15->Text); break;
		case 16:CT_29= StrToInt(EditOTLzad16->Text); break;
		case 17:CT29K1= StrToInt(EditOTLzad17->Text); break;
		case 18:CT_36= StrToInt(EditOTLzad18->Text); break;
		case 19:CT36K1= StrToInt(EditOTLzad19->Text); break;
		case 20:CT_38= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:CT_RASPLAV= StrToInt(EditOTLzad22->Text); break;
	    case 23:CT_ELU= StrToInt(EditOTLzad23->Text); break;
		case 24:CT_TEMP1= StrToInt(EditOTLzad24->Text); break;
		case 25:CT_TEMP2= StrToInt(EditOTLzad25->Text); break;
		case 26:CT_TMN= StrToInt(EditOTLzad26->Text); break;
		case 27:CT_IST= StrToInt(EditOTLzad27->Text); break;
		case 28:CT_VODA_NG= StrToInt(EditOTLzad28->Text); break;
//		case 29:CT_TOLSH= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //13 страница
case 13:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :T_PROC= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :T_KTMN_SHL_RAZGON= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :T_KTMN_ISP_RAZGON= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :T_KTMN_KAM_RAZGON= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :T_VKL_BPN= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :T_DVIJ= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :T_KKAM= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :T_KTMN= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :T_KSHL= StrToInt(EditOTLzad9->Text) ; break;
		case 10:T_KNAP= StrToInt(EditOTLzad10->Text); break;
		case 11:T_NAPUSK= StrToInt(EditOTLzad11->Text); break;
		case 12:T_KSHL_V= StrToInt(EditOTLzad12->Text); break;
		case 13:T_KKAM_MO= StrToInt(EditOTLzad13->Text); break;
		case 14:T_KVVAK_KAM= StrToInt(EditOTLzad14->Text); break;
		case 15:T_KISP_MO= StrToInt(EditOTLzad15->Text); break;
		case 16:T_KISP= StrToInt(EditOTLzad16->Text); break;
		case 17:T_KISP_V= StrToInt(EditOTLzad17->Text); break;
		case 18:T_KUST_ELU= StrToInt(EditOTLzad18->Text); break;
		case 19:T_TMN= StrToInt(EditOTLzad19->Text); break;
		case 20:T_VODA= StrToInt(EditOTLzad20->Text); break;
		case 21:T_KSHL_MO= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//	    case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //14 страница
case 14:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_TRTEST= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :KOM_TOLSH= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_TOLSH= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :ZAD_N_PL= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :PR_NALADKA= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :SOST_V= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :SOST_N= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :RAB_NIJN= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :PR_NAL_PD= StrToInt(EditOTLzad9->Text) ; break;
		case 10:N_ST_MAX= StrToInt(EditOTLzad10->Text); break;
		case 11:N_ST= StrToInt(EditOTLzad11->Text); break;
		case 12:N_ZIKL_PROM= StrToInt(EditOTLzad12->Text); break;
		case 13:N_ZIKL_PROM_KAM= StrToInt(EditOTLzad13->Text); break;
		case 14:N_ZIKL_PROM_ISP= StrToInt(EditOTLzad14->Text); break;
		case 15:N_ZIKL_PROM_SHL= StrToInt(EditOTLzad15->Text); break;
		case 16:IMP60= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
		case 18:tigelVPos= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
		case 20:otvet= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//	    case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //15 страница
case 15:
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
     //16 страница
case 16:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_ELU= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :KOM_ELU= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PAR_ELU= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :ZPAR_ELU= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :X_ELU= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :VRELU= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :E_ELU= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :UST_ELU= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :DELELU= StrToInt(EditOTLzad9->Text) ; break;
		case 10:LIM1ELU= StrToInt(EditOTLzad10->Text); break;
		case 11:LIM2ELU= StrToInt(EditOTLzad11->Text); break;
		case 12:T_VRELU= StrToInt(EditOTLzad12->Text); break;
		case 13:T_KELU= StrToInt(EditOTLzad13->Text); break;
		case 14:DOPELU= StrToInt(EditOTLzad14->Text); break;
		case 15:TEK_ELU= StrToInt(EditOTLzad15->Text); break;
		case 16:N_PROCESS_ELU= StrToInt(EditOTLzad16->Text); break;
		case 17:N_TIGEL= StrToInt(EditOTLzad17->Text); break;
		case 18:POCKET_SET= StrToInt(EditOTLzad18->Text); break;
		case 19:EMISSION_RELEASE_INTERVAL= StrToInt(EditOTLzad19->Text); break;
		case 20:REMP_ELU= StrToInt(EditOTLzad20->Text); break;
		case 21:pri_elu[0]= StrToInt(EditOTLzad21->Text); break;
		case 22:pri_elu[1]= StrToInt(EditOTLzad22->Text); break;
	    case 23:pri_elu[2]= StrToInt(EditOTLzad23->Text); break;
		case 24:pri_elu[3]= StrToInt(EditOTLzad24->Text); break;
		case 25:pri_elu[4]= StrToInt(EditOTLzad25->Text); break;
		case 26:pri_elu[5]= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //17 страница
case 17:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :KOM_VR= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_VR= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :TYPE_VR= StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
		case 5 :PR_VR= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :HOME_VR= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
		case 8 :PUT_VR= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :V_VR= StrToInt(EditOTLzad9->Text) ; break;
		case 10:TEK_ABS_VR= StrToInt(EditOTLzad10->Text); break;
		case 11:TEK_OTN_VR= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_VR= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
		case 18:KOM_PER= StrToInt(EditOTLzad18->Text); break;
		case 19:OTVET_PER= StrToInt(EditOTLzad19->Text); break;
		case 20:TYPE_PER= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:PR_PER= StrToInt(EditOTLzad22->Text); break;
	    case 23:HOME_PER= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:PUT_PER= StrToInt(EditOTLzad25->Text); break;
		case 26:V_PER= StrToInt(EditOTLzad26->Text); break;
		case 27:TEK_ABS_PER= StrToInt(EditOTLzad27->Text); break;
		case 28:TEK_OTN_PER= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
		case 30:CT_PER= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //18 страница
case 18:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :KOM_POD= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_POD= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :TYPE_POD= StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
		case 5 :PR_POD= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :HOME_POD= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
		case 8 :PUT_POD= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :V_POD= StrToInt(EditOTLzad9->Text) ; break;
		case 10:TEK_ABS_POD= StrToInt(EditOTLzad10->Text); break;
		case 11:TEK_OTN_POD= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_POD= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
//		case 15:= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
		case 18:KOM_KAS= StrToInt(EditOTLzad18->Text); break;
		case 19:OTVET_KAS= StrToInt(EditOTLzad19->Text); break;
		case 20:TYPE_KAS= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:PR_KAS= StrToInt(EditOTLzad22->Text); break;
	    case 23:HOME_KAS= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:PUT_KAS= StrToInt(EditOTLzad25->Text); break;
		case 26:V_KAS= StrToInt(EditOTLzad26->Text); break;
		case 27:TEK_ABS_KAS= StrToInt(EditOTLzad27->Text); break;
		case 28:TEK_OTN_KAS= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
		case 30:CT_KAS= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //19 страница
case 19:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :KOM_PPD= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_PPD= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :TYPE_PPD= StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
		case 5 :PR_PPD= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :HOME_PPD= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
		case 8 :PUT_PPD= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :V_PPD= StrToInt(EditOTLzad9->Text) ; break;
		case 10:TEK_ABS_PPD= StrToInt(EditOTLzad10->Text); break;
		case 11:TEK_OTN_PPD= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_PPD= StrToInt(EditOTLzad13->Text); break;
//		case 14:= StrToInt(EditOTLzad14->Text); break;
		case 15:PR_PPD_D= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
//	    case 23:= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
//		case 26:= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
	}
}; break;



    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------- Изменение названия газов ------------------------------------------
void __fastcall TForm1::Gas_Name0Click(TObject *Sender)
{
	int i=0;
	AnsiString temp_str = "";

	if(PCMain -> ActivePage == TSNalad) // сохраняем номер канала и выводим окно ввода
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
	// Подтверждение или отмена ввода
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
	else if(!im0) { MessageBox(NULL, "Невозможно загрузить данные", "Ошибка", MB_OK | MB_ICONSTOP); }
}
//----------------------------------------------------------------------------------------------
void TForm1::SaveGasData()
{
	int SizeOfIniFile=(int)sizeof(GasNames);

	if(!DirectoryExists("Data")) { CreateDir("Data"); }
	FILE *im0;
	im0=fopen(loc_udb,"wb");
	if(im0)       { fwrite(&GasNames,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "Невозможно записать данные", "Ошибка", MB_OK | MB_ICONSTOP); }
}
//----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TForm1::RenameGases()   // обновить названия газов
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
    // отображение мнемосхемы
    VisualMnemo();
            
    Label_Time -> Caption = FormatDateTime("hh:mm:ss",Time());
    Label_Date -> Caption = FormatDateTime("dd.mm.yyyy",Date());

    // время контура логики
    Form1 -> EditTLogic -> Text = FloatToStrF(logic_time,ffFixed,6,3);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1secTimer(TObject *Sender)
{
    VisualGraph();                          //  визуализаия графиков

}
//---------------------------------------------------------------------------
//--Визуализация параметров механизмов--//
//---------------------------------------------------------------------------
void TForm1::VisualParT()
{
EdtTKon1 -> Text = FloatToStrF((float)par_t[0], ffFixed, 8, 0);
EdtTKon2 -> Text = FloatToStrF((float)par_t[1], ffFixed, 8, 0);
EdtTKon3 -> Text = FloatToStrF((float)par_t[2], ffFixed, 8, 0);
EdtTKon4 -> Text = FloatToStrF((float)par_t[3], ffFixed, 8, 0);
EdtTKon5 -> Text = FloatToStrF((float)par_t[4], ffFixed, 8, 0);
EdtTKon6 -> Text = FloatToStrF((float)par_t[5], ffFixed, 8, 0);
EdtTKon7 -> Text = FloatToStrF((float)par_t[6], ffFixed, 8, 0);
EdtTKon8 -> Text = FloatToStrF((float)par_t[7], ffFixed, 8, 0);
}
//---------------------------------------------------------------------------
//--Передача параметров автомата--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnParTClick(TObject *Sender)
{
       PanelParT -> Visible = true;
}
//---------------------------------------------------------------------------
//--Отказ от передачи параметров ручных--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnTNetClick(TObject *Sender)
{
    PanelParT -> Visible = false;
}
//---------------------------------------------------------------------------
//--Подтверждение передачи параметров автомата--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnTDaClick(TObject *Sender)
{
    // панель подтверждения отправки убрать
    PanelParT -> Visible = false;

    par_t[0]  = StrToInt(EdtTRed1->Text);
    par_t[1]  = StrToInt(EdtTRed2->Text);
    par_t[2]  = StrToInt(EdtTRed3->Text);
    par_t[3]  = StrToInt(EdtTRed4->Text);
    par_t[4]  = StrToInt(EdtTRed5->Text);
    par_t[5]  = StrToInt(EdtTRed6->Text);
    par_t[6]  = StrToInt(EdtTRed7->Text);
    par_t[7]  = StrToInt(EdtTRed8->Text);

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "  Переданы параметры механизмов:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    if ( EdtTKon1 -> Text != EdtTRed1 -> Text )
        MemoStat -> Lines -> Add("h - шаг кассеты: " + EdtTKon1 -> Text + " -> " + EdtTRed1 -> Text );
    if ( EdtTKon2 -> Text != EdtTRed2 -> Text )
        MemoStat -> Lines -> Add("h1 - пусть кассеты для переукладки п/д: " + EdtTKon2 -> Text + " -> " + EdtTRed2 -> Text );
    if ( EdtTKon3 -> Text != EdtTRed3 -> Text )
        MemoStat -> Lines -> Add("Координата ПГП от HOME до позиции переукладки в касете: " + EdtTKon3 -> Text + " -> " + EdtTRed3 -> Text );
    if ( EdtTKon4 -> Text != EdtTRed4 -> Text )
        MemoStat -> Lines -> Add("Координата ПГП от HOME до положения ПГП при движении кассеты для смены п/д: " + EdtTKon4 -> Text + " -> " + EdtTRed4 -> Text );
    if ( EdtTKon5 -> Text != EdtTRed5 -> Text )
        MemoStat -> Lines -> Add("Координата ПГП от HOME до положения ПГП во время вращения ПВП: " + EdtTKon5 -> Text + " -> " + EdtTRed5 -> Text );
    if ( EdtTKon6 -> Text != EdtTRed6 -> Text )
        MemoStat -> Lines -> Add("Координата ПВП от HOME до позиции переукладки п/д: " + EdtTKon6 -> Text + " -> " + EdtTRed6 -> Text );
    if ( EdtTKon7 -> Text != EdtTRed7 -> Text )
        MemoStat -> Lines -> Add("Координата ПГП для режима 'Выгрузка п/д': " + EdtTKon7 -> Text + " -> " + EdtTRed7 -> Text );
    if ( EdtTKon8 -> Text != EdtTRed8 -> Text )
        MemoStat -> Lines -> Add("Координата кассеты от HOME до положения определения п/д в пазах кассеты: " + EdtTKon7 -> Text + " -> " + EdtTRed7 -> Text );
    // перекрасить переданные параметры
    EdtTRed1 -> Color = clWhite;
    EdtTRed2 -> Color = clWhite;
	EdtTRed3 -> Color = clWhite;
	EdtTRed4 -> Color = clWhite;
	EdtTRed5 -> Color = clWhite;
    EdtTRed6 -> Color = clWhite;
    EdtTRed7 -> Color = clWhite;
    EdtTRed8 -> Color = clWhite;
    // обновить страницу
    VisualParT();
    MemoT -> Lines -> Clear();
    MemoT -> Lines -> Add(EdtTKon1->Text);
    MemoT -> Lines -> Add(EdtTKon2->Text);
    MemoT -> Lines -> Add(EdtTKon3->Text);
    MemoT -> Lines -> Add(EdtTKon4->Text);
    MemoT -> Lines -> Add(EdtTKon5->Text);
    MemoT -> Lines -> Add(EdtTKon6->Text);
    MemoT -> Lines -> Add(EdtTKon7->Text);
    MemoT -> Lines -> Add(EdtTKon8->Text);
    MemoT -> Lines -> SaveToFile("Nasmod\\Mex.txt");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_MechTimer(TObject *Sender)
{   // визуализация механизмов и их путей
    // перемещение
    if(!ust_ready) return;
    //Кассета
    if(TEK_ABS_KAS<=-par_t[1])
        Kas->Top = 159;
    else if(TEK_ABS_KAS>par_t[0])
        Kas->Top = 156-45;
    else
        Kas->Top = 156 + int(45.0*float(-TEK_ABS_KAS)/(float)par_t[0]);
    //ПВП
    if(TEK_ABS_POD>=0)
        Meh_Vert->Height = 57;
    else if(TEK_ABS_POD<par_t[5])
        Meh_Vert->Height = 57-10;
    else
        Meh_Vert->Height = 57 - int(10.0*float(TEK_ABS_POD)/(float)par_t[5]);
    //крюк вертикальный
        kr_vert->Top=Meh_Vert->Top+Meh_Vert->Height-1;
    //ПГП
    if(TEK_ABS_PER>=par_t[2])
        Meh_Gor->Width=529;
    else if((TEK_ABS_PER<par_t[2])&&(TEK_ABS_PER>=par_t[3]))
        Meh_Gor->Width= 344 + int(185.0*float(TEK_ABS_PER-par_t[3])/(float)(par_t[2]-par_t[3]));
    else if((TEK_ABS_PER<par_t[3])&&(TEK_ABS_PER>=0))
        Meh_Gor->Width= 171 + int(173.0*float(TEK_ABS_PER)/(float)(par_t[3]));
    else if((TEK_ABS_PER<0)&&(TEK_ABS_PER>=par_t[4]))
        Meh_Gor->Width= 171 + int(54.0*float(-TEK_ABS_PER)/(float)(par_t[4]));
    else
        Meh_Gor->Width=117;
    Meh_Gor->Left=965-Meh_Gor->Width;
    //крюк горизонтальный
        kr_gor->Left=Meh_Gor->Left+17;
    // Заданные пути
    Edt_AZ_1_1mn -> Text = IntToStr(par[0][9]);
    Edt_AZ_2_1mn -> Text = IntToStr(par[0][10]);
    Edt_AZ_3_1mn -> Text = IntToStr(par[0][11]);
    Edt_AZ_4_1mn -> Text = IntToStr(par[0][12]);
    Edt_AZ_5_1mn -> Text = IntToStr(par[0][13]);
    // Абсолютные пути
    Edt_AZ_1_2mn -> Text = IntToStr(TEK_ABS_PER);
    Edt_AZ_2_2mn -> Text = IntToStr(TEK_ABS_POD);
    Edt_AZ_3_2mn -> Text = IntToStr(TEK_ABS_VR);
    Edt_AZ_4_2mn -> Text = IntToStr(TEK_ABS_KAS);
    Edt_AZ_5_2mn -> Text = IntToStr(TEK_ABS_PPD);
    // Относительные пути
    Edt_AZ_1_3mn -> Text = IntToStr(TEK_OTN_PER);
    Edt_AZ_2_3mn -> Text = IntToStr(TEK_OTN_POD);
    Edt_AZ_3_3mn -> Text = IntToStr(TEK_OTN_VR);
    Edt_AZ_4_3mn -> Text = IntToStr(TEK_OTN_KAS);
    Edt_AZ_5_3mn -> Text = IntToStr(TEK_OTN_PPD);



    //Верхний
    if(SOST_V==1)
    {
        PD_V->Top=Kas->Top;
        PD_V->Visible=true;
        PD_V->Left=367;
    }
    else if(SOST_V==2)
    {
        PD_V->Top=156;
        PD_V->Visible=true;
        PD_V->Left=Meh_Gor->Left-69;
    }
    else if(SOST_V==3)
    {
        PD_V->Top=kr_vert->Top+4;
        PD_V->Visible=true;
        PD_V->Left=kr_vert->Left-69;
    }
    else
    {
        PD_V->Visible=false;
    }

    //Нижний
    if(SOST_N==1)
    {
        PD_N->Top=Kas->Top+45;
        PD_N->Visible=true;
        PD_N->Left=367;
    }
    else if(SOST_N==2)
    {
        PD_N->Top=156;
        PD_N->Visible=true;
        PD_N->Left=Meh_Gor->Left-69;
    }
    else if(SOST_N==3)
    {
        PD_N->Top=kr_vert->Top+4;
        PD_N->Visible=true;
        PD_N->Left=kr_vert->Left-69;
    }
    else
    {
        PD_N->Visible=false;
    }
    //П/Д
    if((shr[9])&&(PR_NAL_PD==0))
    {
        PD_V->Visible=false;
        PD_N->Visible=false;
    }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::CB_StChangeChange(TObject *Sender)
{
    //контроль
    EdtAKonN_3->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][3] , ffFixed, 5, 0);
    EdtAKonN_4->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][4] , ffFixed, 5, 0);
    EdtAKonN_8->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][8] , ffFixed, 5, 0);
    EdtAKonN_5->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][5] , ffFixed, 5, 0);
    EdtAKonN_7->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][7] , ffFixed, 5, 0);
    EdtAKonN_6->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][6]/10.0 , ffFixed, 5, 1);
    EdtAKonN_2->Text=FloatToStrF((float)par[StrToInt(((TComboBox*)Sender)->ItemIndex)+2][2] , ffFixed, 5, 0);
    //задание
    EdtARedN_3->Text=MemoDin->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+0];
    if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+0])=="1")
        EdtARedN_3->Color=clYellow;
    else if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+0])=="2")
        EdtARedN_3->Color=clSilver;
    else    EdtARedN_3->Color=clWhite;

    EdtARedN_4->Text=MemoDin->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+1];
    if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+1])=="1")
        EdtARedN_4->Color=clYellow;
    else if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+1])=="2")
        EdtARedN_4->Color=clSilver;
    else    EdtARedN_4->Color=clWhite;

    EdtARedN_8->Text=MemoDin->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+2];
    if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+2])=="1")
        EdtARedN_8->Color=clYellow;
    else if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+2])=="2")
        EdtARedN_8->Color=clSilver;
    else    EdtARedN_8->Color=clWhite;

    EdtARedN_5->Text=MemoDin->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+3];
    if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+3])=="1")
        EdtARedN_5->Color=clYellow;
    else if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+3])=="2")
        EdtARedN_5->Color=clSilver;
    else    EdtARedN_5->Color=clWhite;

    EdtARedN_7->Text=MemoDin->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+4];
    if((MemoCol->Lines->Strings[StrToInt(((TComboBox*)Sender)->ItemIndex)*7+4])=="1")
        EdtARedN_7->Color=clYellow;
    else if((MemoCol->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+4])=="2")
        EdtARedN_7->Color=clSilver;
    else    EdtARedN_7->Color=clWhite;

    EdtARedN_6->Text=MemoDin->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+5];
    if((MemoCol->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+5])=="1")
        EdtARedN_6->Color=clYellow;
    else if((MemoCol->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+5])=="2")
        EdtARedN_6->Color=clSilver;
    else    EdtARedN_6->Color=clWhite;

    EdtARedN_2->Text=MemoDin->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+6];
    if((MemoCol->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+6])=="1")
        EdtARedN_2->Color=clYellow;
    else if((MemoCol->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+6])=="2")
        EdtARedN_2->Color=clSilver;
    else    EdtARedN_2->Color=clWhite;

    //библиотека
    if(pr_lib)
    {
        EdtALibN_3->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+0+2];
        EdtALibN_4->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+1+2];
        EdtALibN_8->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+2+2];
        EdtALibN_5->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+3+2];
        EdtALibN_7->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+4+2];
        EdtALibN_6->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+5+2];
        EdtALibN_2->Text=MemoLib->Lines->Strings[((TComboBox*)Sender)->ItemIndex*7+6+2];
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EdtD3CodeChange(TObject *Sender)
{
   // изменить код давления
    SBD3Debug -> Position = StrToInt(EdtD3Code->Text);
    // пересчитать значение давления
    EdtD3Davl -> Text = FloatToStrF(pow(10,(float)SBD3Debug->Position/1000.0-3.5),ffExponent,3,8);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SBD3DebugChange(TObject *Sender)
{
    // изменить код давления
    EdtD3Code -> Text = IntToStr(SBD3Debug->Position);
    // пересчитать значение давления
    EdtD3Davl -> Text = FloatToStrF(pow(10,(float)SBD3Debug->Position/1000.0-3.5),ffExponent,3,8);
    //(3.5+log10(StrToFloat(EditNastrTo13->Text)))*1000.0
    //FloatToStrF(pow(10,(float)D_D1/1000.0-3.5),ffExponent,3,8);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SBD4DebugChange(TObject *Sender)
{
    // изменить код давления
    EdtD4Code -> Text = IntToStr(SBD4Debug->Position);
    // пересчитать значение давления
    EdtD4Davl -> Text = FloatToStrF(pow(10,(float)SBD4Debug->Position/1000.0*1.667-9.333),ffExponent,3,8);
    //FloatToStrF(pow(10,(float)D_D4/1000.0*1.667-9.333),ffExponent,3,8);
    //int(((log10(StrToFloat(EditNastrTo21->Text)))*0.6+5.6)*1000.0);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EdtD4CodeChange(TObject *Sender)
{
    // изменить код давления
    SBD4Debug -> Position = StrToInt(EdtD4Code->Text);
    // пересчитать значение давления
    EdtD4Davl -> Text = FloatToStrF(pow(10,(float)SBD4Debug->Position/1000.0*1.667-9.333),ffExponent,3,8);
}
//---------------------------------------------------------------------------
void TForm1::ParamTrans() // запись параметров движений в контроллеры
{
    // рассчет и формирование таблицы
    // вращение стола
    // #1
    // AZ_drive[1]->Zad_Op_data[1][0] = 7;		// тип движения
    // AZ_drive[1]->Zad_Op_data[1][1] = 0;		// путь
    // AZ_drive[1]->Zad_Op_data[1][2] = AZ_drive[i]->data_mech.v_mech[0];	// скорость
    // AZ_drive[1]->Zad_Op_data[1][3] = AZ_drive[1]->data_mech.set_mech[0];	// ускорение
    // AZ_drive[1]->Zad_Op_data[1][4] = AZ_drive[1]->data_mech.set_mech[1];	// торможение
    // AZ_drive[1]->Zad_Op_data[1][5] = 1000;	// масштаб
    // AZ_drive[1]->Zad_Op_data[1][6] = 0;		// 
	// AZ_drive[1]->Zad_Op_data[1][7] = 3;		// тип последовательности
    // AZ_drive[1]->Zad_Op_data[1][8] = -1;		// следующий шаг последоват.
	
	//#1 бьесконечное вращение
	AZ_drive[3]->Zad_Op_data[1][0] = 7;
	AZ_drive[3]->Zad_Op_data[1][1] = 0;
	AZ_drive[3]->Zad_Op_data[1][2] = AZ_drive[3]->data_mech.v_mech[0];
	AZ_drive[3]->Zad_Op_data[1][3] = AZ_drive[3]->data_mech.set_mech[0];
	AZ_drive[3]->Zad_Op_data[1][4] = AZ_drive[3]->data_mech.set_mech[1];
	AZ_drive[3]->Zad_Op_data[1][5] = 1000;
	AZ_drive[3]->Zad_Op_data[1][6] = 0;
	AZ_drive[3]->Zad_Op_data[1][7] = 3;
	AZ_drive[3]->Zad_Op_data[1][8] = -1;
	
	AZ_drive[3]->Zad_Op_data[2][0] = 10;
	AZ_drive[3]->Zad_Op_data[2][1] = 0;
	AZ_drive[3]->Zad_Op_data[2][2] = AZ_drive[3]->data_mech.v_mech[0];
	AZ_drive[3]->Zad_Op_data[2][3] = AZ_drive[3]->data_mech.set_mech[0];
	AZ_drive[3]->Zad_Op_data[2][4] = AZ_drive[3]->data_mech.set_mech[1];
	AZ_drive[3]->Zad_Op_data[2][5] = 1000;
	AZ_drive[3]->Zad_Op_data[2][6] = 0;
	AZ_drive[3]->Zad_Op_data[2][7] = 0;
	AZ_drive[3]->Zad_Op_data[2][8] = -256;

    Visual_AZdata();

    for(int i=0;i<DRIVE_COUNT;i++)
        for(int j=1;j<=AZ_drive[i]->Max_Op_Data;j++)
        {
            AZ_drive[i]->Data_err[j] = 1;
        }

	PR_PERPAR = 0;
}
//---------------------------------------------------------------------------
void TForm1::ParamTransCheck() // слежение за записью параметров
{
    if(PR_PERPAR)
    {
        PB_ParA_trans->Visible = false;
        Lbl_ParA_trans->Caption = "Параметры записаны";
        PB_ParA_trans->Position = 100;
    }
    else
    {
        PB_ParA_trans->Visible = true;
        Lbl_ParA_trans->Caption = "Параметры не записаны";

        // считаем количество заданий для записи
        int summ_data = 0, done_data = 0;

        for(int i=0;i<DRIVE_COUNT;i++)
            for(int j=1;j<=AZ_drive[i]->Max_Op_Data;j++)
			{
				summ_data++;
				if(!(AZ_drive[i]->Data_err[j])) done_data++;
			}

		PB_ParA_trans->Position = RoundTo(float(done_data)*100.0/float(summ_data),0);

		if(summ_data == done_data) PR_PERPAR = 1;
    }
}
//---------------------------------------------------------------------------









