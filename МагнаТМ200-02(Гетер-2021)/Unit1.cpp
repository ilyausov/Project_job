//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "stdio.h"
#include "ctype.h"

#include "Logic.cpp"
#include "Header.h"

#include "Modules\DatPPT200\DatPPT200.cpp"
#include "Modules\DatMPT200\DatMPT200.cpp"
#include "Modules\DZaslVAT\DZaslVAT.cpp"
#include "Modules\TRMD\TRMD.cpp"
#include "Modules\KNOmsk\KNOmsk.cpp"
#include "Modules\IVE\IVE.cpp"

#include "Names.h"
#include "External.cpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "CSPIN"
#pragma resource "*.dfm"

TForm1 *Form1;

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

    // чтение PISO-P32C32(1)
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
     // чтение PISO-P32C32(2)
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
    // чтение ACL-7250
    if( externalTask & 0x04 )
    {
        // опросили
        externalError = ACL7250(0,zin);
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x10);
                // снять задачу
                externalTask &=(~0x04);
            }; break;
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x10;
                // снять задачу
                externalTask &= (~0x04);
            }; break;
        }
    }

    // чтение аналоговых входных сигналов с PISO-813
    if(externalTask & 0x08)
    {
        // опросили
        externalError = PISO_813U(aik , AIK_COUNT * 8);
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x04);
                // снять задачу
                externalTask &= (~0x08);
            }; break;
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x04;
                // снять задачу
                externalTask &= (~0x08);
            }; break;
        }
    }

    // записать в PISO-P32С32(1)
    if(externalTask & 0x10)
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
                externalTask &= (~0x10);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x01;
                // снять задачу
                externalTask &= (~0x10);
            }; break;
        }
    }
    // записать в PISO-P32С32(2)
    if(externalTask & 0x20)
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
                externalTask &= (~0x20);
            }; break;
            // есть ошибки связи
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x02;
                // снять задачу
                externalTask &= (~0x20);
            }; break;
        }
    }
    // записать в ACL7250
    if(externalTask & 0x40)
    {
        // записали
        externalError = ACL7250( 1 , out );
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x10);
                // снять задачу
                externalTask &= (~0x40);
            }; break;
            default:
            {
                // выставить диагностику
                diagnS[1] |= 0x10;
                // снять задачу
                externalTask &= (~0x40);
            }; break;
        }
    }
    // запись в ISO-DA16
    if( externalTask & 0x80 )
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
                    diagnS[1] &= (~0x08);
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
        externalTask &= (~0x80);
    }
    // запись в ACL-7225
    if( externalTask & 0x100 )
    {
        // записали
        externalError = ACL7225_1( 1 , out );
        // анализ ответа
        switch ( externalError )
        {
            case 0:
            {
                // снять диагностику
                diagnS[1] &= (~0x20);
                // снять задачу
                externalTask &= (~0x100);
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
    serTemp[3] = Series4;
    serTemp[4] = Series5;
    serTemp[5] = Series6;
    serTemp[6] = Series7;
    serTemp[7] = Series8;
    serTemp[8] = Series9;
    serTemp[9] = Series10;
    serTemp[10] = Series19;

    // инициализация массива отображения архивных графиков
    serArh[0] = Series11;
    serArh[1] = Series12;
    serArh[2] = Series13;
    serArh[3] = Series14;
    serArh[4] = Series15;
    serArh[5] = Series16;
    serArh[6] = Series17;
    serArh[7] = Series18;
    serArh[8] = Series20;
    serArh[9] = Series21;
    serArh[10] = Series22;

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




        PnlMnemoParam -> Height = 396; //уменьшаем размер таблицы
        Pnl_GK->BringToFront();

        //Скрываем время процесса
        Panel4 -> Visible = false;
        Panel6 -> Visible = false;
        EdtZadA14 -> Visible = false;
        EdtTekA14 -> Visible = false;
        State     -> Visible = false;
        Panel317->BringToFront();


        VisualMnemo();
        Pnl_Work -> Visible = true;
  }
  else if(PCMain -> ActivePage == TSWork)
  {     Pnl_Work -> Visible = false;
        Pnl_Work -> Parent = TSWork;

       
        Pnl_Work -> Top = 32;
        Pnl_Work -> Left = 8;

        PnlMnemoParam -> Height = 422; //Увеличиваем размер таблицы

        //Добавляем время процесса
        Panel4 -> Visible = true;
        Panel6 -> Visible = true;
        EdtZadA14 -> Visible = true;
        EdtTekA14 -> Visible = true;
        State     -> Visible = true;



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
     Form1->GB_zin4_2
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
     Form1->GB_ain1
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
   { Form1->GB_aout0,
     Form1->GB_aout1,
     Form1->GB_aout2
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
    OpenPISO_P32C32(); // попытка связаться с драйвером ISO-P32C32
    OpenACL_7250();   // попытка связаться с драйвером ACL-7250
    OpenPISO_813 ();    // попытка связаться с драйвером ISO-813
    OpenISO_DA16();    // попытка связаться с драйвером ISO-DA16
    OpenACL_7225_1();

    PCMain  -> DoubleBuffered = true;
    Form1-> DoubleBuffered = true;
    Pnl_Title -> DoubleBuffered = true;
    
    Label_Time -> Caption = TimeToStr(Time());
    Label_Date -> Caption = DateToStr(Date());

    // загрузить значения настроечного массива
    MemoNasmod -> Lines -> LoadFromFile("Nasmod\\Nasmod.txt");
    EditNastrTo0  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](0));
    EditNastrTo1  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](1));
    EditNastrTo2  -> Text = MemoNasmod -> Lines -> operator [](2);
    EditNastrTo3  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](3));
    EditNastrTo4  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](4));
    EditNastrTo5  -> Text = MemoNasmod -> Lines -> operator [](5);
    EditNastrTo6  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](6));
    EditNastrTo7  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](7));
    EditNastrTo8  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](8));
    EditNastrTo9  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](9));
    EditNastrTo10  -> Text = MemoNasmod -> Lines -> operator [](10);
    EditNastrTo11  -> Text = MemoNasmod -> Lines -> operator [](11);
    EditNastrTo12  -> Text = MemoNasmod -> Lines -> operator [](12);
    EditNastrTo13  -> Text = MemoNasmod -> Lines -> operator [](13);
    EditNastrTo14  -> Text = MemoNasmod -> Lines -> operator [](14);
    EditNastrTo15  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](15));
    EditNastrTo16  -> Text = MemoNasmod -> Lines -> operator [](16);
    EditNastrTo17  -> Text = MemoNasmod -> Lines -> operator [](17);
    EditNastrTo18  -> Text = MemoNasmod -> Lines -> operator [](18);
    EditNastrTo19  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](19));
    EditNastrTo20  -> Text = MemoNasmod -> Lines -> operator [](20);
    EditNastrTo21  -> Text = MemoNasmod -> Lines -> operator [](21);
    EditNastrTo22  -> ItemIndex = StrToInt(MemoNasmod -> Lines -> operator [](22));
    MemoNasmod -> Lines -> Clear();


    // загрузить значения T
    MemoT -> Lines -> LoadFromFile("Nasmod\\Mex.txt");
    EdtTRed1  -> Text = MemoT -> Lines -> operator [](0);
    EdtTRed2  -> Text = MemoT -> Lines -> operator [](1);
    EdtTRed3  -> Text = MemoT -> Lines -> operator [](2);
    EdtTRed4  -> Text = MemoT -> Lines -> operator [](3);
    EdtTRed5  -> Text = MemoT -> Lines -> operator [](4);
    MemoT -> Lines -> Clear();

    BtnNastrDaClick(BtnNastrDa);
    BtnAutoDaClick(BtnAutoDa);
    BtnTrDaClick(BtnTrDa);
    BtnTDaClick(BtnTDa);
    BtnRDaClick(BtnRDa);

    // Инициализация RS-страниц
    Init_SComport();
	Comport[0]->Reser_Port(Comport[0]->BTN_reset);  // включение порта
    Comport[1]->Reser_Port(Comport[1]->BTN_reset);  // включение порта
    Comport[2]->Reser_Port(Comport[2]->BTN_reset);  // включение порта
    Comport[3]->Reser_Port(Comport[3]->BTN_reset);  // включение порта
    Comport[4]->Reser_Port(Comport[4]->BTN_reset);  // включение порта
    Comport[5]->Reser_Port(Comport[5]->BTN_reset);  // включение порта

    Init_DZaslVAT();
    Init_DatPPT200();
    Init_DatMPT200();
    Init_TRMD();
    Init_BU_IVE();
    Init_KNOmsk();
    Init_SAZ_drive();
    AZdrive_Load();	// загрузка параметров драйверов

   /*
    // организация доступа
    LoadData2();

    pas_str = "";
    for(int i=0;iniPAS.pass[i]!=0;i++)
    pas_str = pas_str + iniPAS.pass[i];
    Edit_Acc_UserPas -> Text = pas_str;

    // скрываем вкладки
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

    // если мнемосхема заблокирована, переключаемся на доступ
    if(!PCNalad->Pages[0]->TabVisible)
		PCNalad->ActivePage = TSNaladAcc;
    */

    // инициализировать объекты РРГ и клапанов
    InitObjectsRRG();   //ПОДКЛЮЧИТЬ RRG.cpp
    InitObjectsKl();

    SetOut(1,3,0x4000);				// выставить Стоп механизмов

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
    if((D_D1>1)&&(D_D1<=10000))  { d_d1->Caption=FloatToStrF(pow(10,(float)D_D1/1000.0-3.5),ffExponent,3,8); } //РРТ200
    else d_d1->Caption = "вне диап.";
    if((D_D2>1)&&(D_D2<=10000))  { d_d2->Caption=FloatToStrF(pow(10,(float)D_D2/1000.0-3.5),ffExponent,3,8); } //РРТ200
    else d_d2->Caption = "вне диап.";
    if((D_D3>1)&&(D_D3<=10000))  { d_d3->Caption=FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffExponent,3,8); } //РРТ200
    else d_d3->Caption = "вне диап.";
    if((D_D4>0)&&(D_D4<=10000))  { d_d4->Caption=FloatToStrF(pow(10,(float)D_D4/1000.0*1.667-9.333),ffExponent,3,8); } //МРТ200
    if((D_D5>1)&&(D_D5<=10000))  { d_d5->Caption=FloatToStrF(pow(10,(float)D_D5/1000.0-3.5),ffExponent,3,8); } //РРТ200
    else d_d5->Caption = "вне диап.";
    if((D_D6>1)&&(D_D6<=10000))  { d_d6->Caption=FloatToStrF(pow(10,(float)D_D6/1000.0-3.5),ffExponent,3,8); } //РРТ200
    else d_d6->Caption = "вне диап.";

    //Коэффициент согласования текущий
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(shr[28]) 
    {
        if(N_TEK_GIS!=0) { coef_pd_tek->Caption = FloatToStrF(1000.0/(float)N_TEK_GIS,ffFixed,5,0); } // тек. коэфф. согл
        else             { coef_pd_tek->Caption = 0;}
    }
    else coef_pd_tek->Caption = 0;
    //Коэффициент согласования за.данный
    coef_pd_zad->Caption = FloatToStrF(1000.0/(float)nasmod[14],ffFixed,5,0); // заданный коэфф. согл п/д
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Номер стадии
    if(shr[3]||shr[4]||shr[2])
    switch (N_ST)
    {
        case 1: {EditNST -> Caption = "Нагрев";             break;}
        case 2: {EditNST -> Caption = "ВЧ-очистка";         break;}
        case 3: {EditNST -> Caption = "Ионная очистка";     break;}
        case 4:
        {
            if(PR_OTP)
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Отпыл М1";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Отпыл М2";
                else
                    EditNST -> Caption = "Отпыл М1 и М2";
            }
            else
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Напыление М1 (слой 1)";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Напыление М2 (слой 1)";
                else
                    EditNST -> Caption = "Напыление М1 и М2 (слой 1)";
            }
            break;
        }


        case 5: {
            if(PR_OTP)
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Отпыл М1";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Отпыл М2";
                else
                    EditNST -> Caption = "Отпыл М1 и М2";
            }
            else
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Напыление М1 (слой 2)";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Напыление М2 (слой 2)";
                else
                    EditNST -> Caption = "Напыление М1 и М2 (слой 2)";
            }
            break;
        }
        case 6: {
            if(PR_OTP)
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Отпыл М1";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Отпыл М2";
                else
                    EditNST -> Caption = "Отпыл М1 и М2";
            }
            else
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Напыление М1 (слой 3)";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Напыление М2 (слой 3)";
                else
                    EditNST -> Caption = "Напыление М1 и М2 (слой 3)";
            }
            break;
        }
        case 7: {
            if(PR_OTP)
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Отпыл М1";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Отпыл М2";
                else
                    EditNST -> Caption = "Отпыл М1 и М2";
            }
            else
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Напыление М1 (слой 4)";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Напыление М2 (слой 4)";
                else
                    EditNST -> Caption = "Напыление М1 и М2 (слой 4)";
            }
            break;
        }
        case 8: {
            if(PR_OTP)
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Отпыл М1";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Отпыл М2";
                else
                    EditNST -> Caption = "Отпыл М1 и М2";
            }
            else
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Напыление М1 (слой 5)";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Напыление М2 (слой 5)";
                else
                    EditNST -> Caption = "Напыление М1 и М2 (слой 5)";
            }
            break;
        }
        case 9: {
            if(PR_OTP)
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Отпыл М1";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Отпыл М2";
                else
                    EditNST -> Caption = "Отпыл М1 и М2";
            }
            else
            {
                if((par[N_ST][6])&&(!(par[N_ST][7])))
                    EditNST -> Caption = "Напыление М1 (слой 6)";
                else if((par[N_ST][7])&&(!(par[N_ST][6])))
                    EditNST -> Caption = "Напыление М2 (слой 6)";
                else
                    EditNST -> Caption = "Напыление М1 и М2 (слой 6)";
            }
            break;
        }
        case 10: {EditNST -> Caption = "Тренировка М1";     break;}
        case 11: {EditNST -> Caption = "Тренировка М2";     break;}
        case 12: {EditNST -> Caption = "Тренировка М1 и М2";break;}
        default: {EditNST -> Caption = "";                  break;}
    }
    else
    EditNST -> Caption = "";
    //--------------------------------------------------------------------------
    //Температуры
    Temp_PD -> Caption = FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1)+ "°С";//п/д

    
    Temp_K  -> Caption = FloatToStrF((float(TEK_TEMP1)/10.0),ffFixed,4,1)+ "°С";//камера
    //--------------------------------------------------------------------------
    //ДЗ
    poz_DZ  -> Caption = FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,0) + "%";
    //--------------------------------------------------------------------------
    // температура КН
    Temp_KN -> Caption = FloatToStrF(float(OTVET_KN_M[3])/10.0,ffFixed,5,1) + "°K";
    //--------------------------------------------------------------------------
    // скорость ТМН
    if(aik[15]>20)
        V_TMN-> Caption = FloatToStrF(float(aik[15])*50000.0/4095,ffFixed,5,0);
    else
        V_TMN-> Caption ="0";
    if(PCMain -> ActivePage == TSNalad) // Наладка
    {
    //**************РРГ*****Задания************************************************************
    if(shr[24]) { rrg1_zad -> Caption = FloatToStrF((float)U_PUN/4095.0*RRG1_MAX,    ffFixed,5,2); }
		else if(shr[20]) { rrg1_zad -> Caption = FloatToStrF((float)par[0][0]/4095.0*RRG1_MAX,ffFixed,5,2); }
        else { rrg1_zad -> Caption = "0,00"; }

    rrg2_zad-> Caption =EdtRKon0_1-> Text;  // расход РРГ2
    rrg3_zad-> Caption =EdtRKon0_2-> Text;  // расход РРГ3

    if(shr[4])
        rrg4_zad-> Caption =FloatToStrF((float)nasmod[2]/4095.0*RRG4_MAX,ffFixed,5,1); //расход РРГ4
    else
        rrg4_zad-> Caption =FloatToStrF((float)par[0][3]/4095.0*RRG4_MAX,ffFixed,5,1);
    //*****************РРГ*Текущие***************************************************************
    rrg1_tek-> Caption = FloatToStrF((float)aik[6]*RRG1_MAX/4095.0,  ffFixed, 6, 2); // расход РРГ1

    rrg2_tek-> Caption = FloatToStrF((float)aik[7]*RRG2_MAX/4095.0,  ffFixed, 6, 1); // расход РРГ2

    rrg3_tek-> Caption = FloatToStrF((float)aik[8]*RRG3_MAX/4095.0,  ffFixed, 6, 1); // расход РРГ3

    rrg4_tek-> Caption = FloatToStrF((float)aik[9]*RRG4_MAX/4095.0,  ffFixed, 6, 1); // расход РРГ4

    //----------------Таблица-------Задания---------------------------------------------------------------
    EdtZadA00-> Text =EdtRKon0_15-> Text;//Температура п/д
    EdtZadA01-> Text =EdtRKon0_5-> Text;//Ток ИИ
    EdtZadA03-> Text =EdtRKon0_6-> Text;//Мощность М1
    EdtZadA06-> Text =EdtRKon0_7-> Text;//Мощность М2
    EdtZadA09-> Text =EdtRKon0_8-> Text;//Падающая мощность ВЧГ п/д
    EdtZadA12-> Text =EdtRKon0_9-> Text;//Процент открытия ДЗ
    EdtZadA13-> Text =EdtRKon0_4-> Text;//Давление
    //****************таблица******Текущие*******************************************************
    EdtTekA00-> Text =FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1);         //Температура п/д
    //******************ИИ**********************************************************************

        EdtTekA01-> Text =FloatToStrF((float)OTVET_II[5]*512.0/1023, ffFixed, 6, 1); //Ток ИИ
        EdtTekA02-> Text =FloatToStrF((float)OTVET_II[4]*3072/1024, ffFixed, 6, 0); //Напряжение ИИ

    //***********М1**********************************************************************************

        EdtTekA03-> Text =FloatToStrF((float)OTVET_BM1[6]*3072.0/1023.0,ffFixed,6,0);//Мощность М1
        EdtTekA04-> Text =FloatToStrF((float)OTVET_BM1[4]*665.6/1023.0,ffFixed,6,0); //напряжение М1
        EdtTekA05-> Text =FloatToStrF((float)OTVET_BM1[5]*10.24/1023.0,ffFixed,8,1); //Ток М1

    //***********М2**********************************************************************************
        EdtTekA06-> Text =FloatToStrF((float)OTVET_BM2[6]*3072.0/1023.0,ffFixed,6,0);//Мощность М2
        EdtTekA07-> Text =FloatToStrF((float)OTVET_BM2[4]*665.6/1023.0,ffFixed,6,0); //напряжение М2
        EdtTekA08-> Text =FloatToStrF((float)OTVET_BM2[5]*10.24/1023.0,ffFixed,8,1); //Ток М2

    //***********ВЧГ п/д**********************************************************************************
    EdtTekA09-> Text =FloatToStrF((float)aik[10]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // падающая мощность ВЧГ ПД
    EdtTekA10-> Text =FloatToStrF((float)aik[11]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // отраженная мощность ВЧГ ПД
    EdtTekA11-> Text =FloatToStrF((float)aik[14]*SMESH_MAX_USER/4095.0, ffFixed, 6, 0); // смещение ВЧГ ПД
    //****************************************************************************************************
    EdtTekA12-> Text =FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,1) ;//процент открытия ДЗ
    EdtTekA13-> Text =FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffFixed,8,2);//Давление по РРТ200

    }
    else // Мнемосхема
    {

    //*************РРГ задания*************************************************************
        if(((shr[3]&&!PR_NALADKA)||shr[2])&&shr[24]) { rrg1_zad -> Caption = FloatToStrF((float)U_PUN/4095.0*RRG1_MAX,    ffFixed,5,2); }
        else if((shr[3]&&!PR_NALADKA)&&shr[20]) { rrg1_zad -> Caption = FloatToStrF((float)par[N_ST][0]/4095.0*RRG1_MAX,    ffFixed,5,2); }
		else rrg1_zad -> Caption = "0,00";
        //РРГ2
        if((shr[3]&&!PR_NALADKA)&&shr[21])rrg2_zad -> Caption =FloatToStrF((float)par[N_ST][1]/4095.0*RRG2_MAX,        ffFixed,5,1);
        else rrg2_zad -> Caption = "0,0";   //Расход РРГ2
        //РРГ3
        if((shr[3]&&!PR_NALADKA)&&shr[22])rrg3_zad -> Caption =FloatToStrF((float)par[N_ST][2]/4095.0*RRG3_MAX,        ffFixed,5,1);
        else rrg3_zad -> Caption = "0,0";   //Расход РРГ3
        //РРГ4
        if(shr[23])
        {
        if(shr[4])
            rrg4_zad -> Caption =FloatToStrF((float)nasmod[2]/4095.0*RRG4_MAX,        ffFixed,5,1);   //Расход РРГ2
        else
            rrg4_zad -> Caption =FloatToStrF((float)par[N_ST][3]/4095.0*RRG4_MAX,        ffFixed,5,1);   //Расход РРГ2
        }
        else rrg4_zad -> Caption = "0,0";
        //температура п/д
        if(shr[3]&&!PR_NALADKA )
        {
            if((nasmod[4]==0)||(nasmod[6]==0))EdtZadA00 -> Text ="0,0";                               //температура п/д
            else if(nasmod[6]==1)             EdtZadA00 -> Text =FloatToStrF((float)PAR_TEMP2/10.0, ffFixed, 5, 1);
            else                              EdtZadA00 -> Text =FloatToStrF((float)PAR_OHL/81.92, ffFixed, 5, 1);
        }
        else
        {
            EdtZadA00 -> Text ="0,0";
        }
        //Ток ИИ
        if(shr[3]&&N_ST==3)
            EdtZadA01-> Text =FloatToStrF((float)par[3][5]/ 4095.0 * 500.0, ffFixed, 5, 1);
        else
            EdtZadA01-> Text ="0,0";
        //Мощность М1
        if(((shr[3])&&(N_ST>3)&&(N_ST<10))||((shr[2])&&((N_ST==10)||(N_ST==12))))
            EdtZadA03-> Text =FloatToStrF((float)par[N_ST][6]/4095.0*3600 , ffFixed, 5, 0);
        else EdtZadA03-> Text =0;
        //Мощность М2
        if(((shr[3])&&(N_ST>3)&&(N_ST<10))||((shr[2])&&((N_ST==11)||(N_ST==12))))
            EdtZadA06-> Text =FloatToStrF((float)par[N_ST][7]/4095.0*3600 , ffFixed, 5, 0);
        else EdtZadA06-> Text =0;

        //Падающая мощность ВЧГ п/д
        if(shr[3]&&((N_ST==2)||N_ST>3&&N_ST<10))
            EdtZadA09-> Text =FloatToStrF((float)par[N_ST][8]/ 4095.0* CESAR_MAX_PD, ffFixed, 5, 0);
        else
            EdtZadA09-> Text ="0";
        //процент открытия ДЗ
        if((shr[3]||shr[2])&&N_ST>1)
        {
            if(PR_OTP)
                EdtZadA12-> Text=FloatToStrF((float)par[N_ST][13]/10000.0*100.0, ffFixed, 5, 1);
            else
                EdtZadA12-> Text=FloatToStrF((float)par[N_ST][9]/10000.0*100.0, ffFixed, 5, 1);
        }
        else
            EdtZadA12-> Text="0,0";
        //Давление
        if(((shr[3])&&((N_ST==2)||(N_ST>3&&N_ST<10)))||
            (shr[2])&&((N_ST>9)&&(N_ST<13)))
            EdtZadA13-> Text =FloatToStrF(pow(10,(float)par[N_ST][4]/1000.0-3.5),ffFixed, 8, 2);  //давление РРТ200
        else
            EdtZadA13-> Text ="0,0";
        // время процесса
        if(shr[2]||shr[3])
            if(PR_OTP)
                EdtZadA14-> Text =FloatToStrF((float)par[N_ST][14],ffFixed,5,0);   //Время процесса
            else
                EdtZadA14-> Text =FloatToStrF((float)par[N_ST][12],ffFixed,5,0);   //Время процесса
        else
             EdtZadA14-> Text ="0";
        //РРГ текущий
        //РРГ1
        if(shr[24]||shr[20]&&(shr[3]&&!PR_NALADKA)) { rrg1_tek -> Caption = FloatToStrF((float)aik[6]*RRG1_MAX/4095.0,ffFixed,5,2); }
        else        { rrg1_tek -> Caption = "0,00"; }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //РРГ2
        if(shr[21]&&(shr[3]&&!PR_NALADKA)) { rrg2_tek -> Caption = FloatToStrF((float)aik[7]*RRG2_MAX/4095.0,ffFixed,5,1); }
        else        { rrg2_tek -> Caption = "0,0"; }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //РРГ3
        if(shr[22]&&(shr[3]&&!PR_NALADKA)) { rrg3_tek -> Caption = FloatToStrF((float)aik[8]*RRG3_MAX/4095.0,ffFixed,5,1); }
        else        { rrg3_tek -> Caption = "0,0"; }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //РРГ4
        if((shr[23]&&(shr[3]&&!PR_NALADKA))||(shr[23]&&shr[4])) { rrg4_tek -> Caption = FloatToStrF((float)aik[9]*RRG4_MAX/4095.0,ffFixed,5,1); }
        else        { rrg4_tek -> Caption = "0,0"; }
        //****************таблица******Текущие*******************************************************
        if(shr[31]||shr[38])EdtTekA00-> Text =FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1);         //Температура п/д
        else                EdtTekA00-> Text ="0,0";
    //******************ИИ**********************************************************************
        if(shr[33]&&(shr[3]&&!PR_NALADKA))
        {
            EdtTekA01-> Text =FloatToStrF((float)OTVET_II[5]*512.0/1023, ffFixed, 6, 1); //Ток ИИ
            EdtTekA02-> Text =FloatToStrF((float)OTVET_II[4]*3072/1024, ffFixed, 6, 0); //Напряжение ИИ
        }
        else
        {
            EdtTekA01-> Text ="0,0"; //Ток ИИ
            EdtTekA02-> Text =0; //Напряжение ИИ
        }
    //***********М1**********************************************************************************
        if(shr[35]&&((shr[3]&&!PR_NALADKA)||shr[2]))
        {
            EdtTekA03-> Text =FloatToStrF((float)OTVET_BM1[6]*3072.0/1023.0,ffFixed,6,0);//Мощность М1
            EdtTekA04-> Text =FloatToStrF((float)OTVET_BM1[4]*665.6/1023.0,ffFixed,6,0); //напряжение М1
            EdtTekA05-> Text =FloatToStrF((float)OTVET_BM1[5]*10.24/1023.0,ffFixed,8,1); //Ток М1
        }
        else
        {
            EdtTekA03-> Text =0;
            EdtTekA04-> Text =0;
            EdtTekA05-> Text =0;
        }
    //***********М2**********************************************************************************
        if(shr[36]&&((shr[3]&&!PR_NALADKA)||shr[2]))
        {
            EdtTekA06-> Text =FloatToStrF((float)OTVET_BM2[6]*3072.0/1023.0,ffFixed,6,0);//Мощность М2
            EdtTekA07-> Text =FloatToStrF((float)OTVET_BM2[4]*665.6/1023.0,ffFixed,6,0); //напряжение М2
            EdtTekA08-> Text =FloatToStrF((float)OTVET_BM2[5]*10.24/1023.0,ffFixed,8,1); //Ток М2
        }
        else
        {
            EdtTekA06-> Text = 0;
            EdtTekA07-> Text =0;
            EdtTekA08-> Text =0;
        }
    //***********ВЧГ п/д**********************************************************************************
        if(shr[28])
        {
            EdtTekA09-> Text =FloatToStrF((float)aik[10]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // падающая мощность ВЧГ ПД
            EdtTekA10-> Text =FloatToStrF((float)aik[11]*CESAR_MAX_PD/4095.0, ffFixed, 6, 0); // отраженная мощность ВЧГ ПД
            EdtTekA11-> Text =FloatToStrF((float)aik[14]*SMESH_MAX_USER/4095.0, ffFixed, 6, 0); // смещение ВЧГ ПД
        }
        else
        {
            EdtTekA09-> Text ="0"; // падающая мощность ВЧГ ПД
            EdtTekA10-> Text ="0"; // отраженная мощность ВЧГ ПД
            EdtTekA11-> Text ="0"; // смещение ВЧГ ПД
        }
    //****************************************************************************************************

        if(shr[25]||shr[26]||shr[27])EdtTekA12-> Text =FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,1);//процент открытия ДЗ
        else                         EdtTekA12-> Text ="0,0";

        if((shr[3]||shr[2])&&shr[24])
            EdtTekA13-> Text =FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffFixed,8,2);//Давление по РРТ200
        else
            EdtTekA13-> Text ="0,0";

        if(((shr[4]||shr[2])&&!PR_NALADKA))EdtTekA14 -> Text = IntToStr(T_PROC);
        else                     EdtTekA14 -> Text ="0";
    }




}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Отображение клавиш--//
void TForm1::VisualButtons()
{

    // Откачка камеры
    OtkKam    -> Font -> Color = SetPopeColor(shr[1]);
    // Рабочий цикл
    PnlPVS    -> Font -> Color = SetPopeColor(shr[3]);
    // Сброс РЦ
    PnlRC    -> Font -> Color = SetPopeColor(shr[5]);
    // Отключение установки
    PnlUstOff   -> Font -> Color = SetPopeColor(shr[7]);
    // Сбор пластин
    PnlSbor -> Font -> Color = SetPopeColor(shr[6]);
    //Тренировка
    PnlTR  -> Font -> Color = SetPopeColor(shr[2]);
    //чиллер
    Pnl_Chil_On_A   -> Font -> Color = SetPopeColor(zin[2]&0x20);
    Pnl_Chil_Off_A  -> Font -> Color = SetPopeColor(!(zin[2]&0x20));

    ///////////////////////////////////////////////////////////////////////////
    // РРГ1 (Вкл.)
    Pnl_RRG1_On     -> Font -> Color = SetPopeColor(shr[20]);
    // РРГ2 (Вкл.)
    Pnl_RRG2_On     -> Font -> Color = SetPopeColor(shr[21]);
    // РРГ3 (Вкл.)
    Pnl_RRG3_On     -> Font -> Color = SetPopeColor(shr[22]);
    // РРГ4 кам(Вкл.)
    Pnl_RRG4_Kam    -> Font -> Color = SetPopeColor(shr[23]&&!PR_RG4);
    // РРГ4 нас(Вкл.)
    Pnl_RRG4_Nas    -> Font -> Color = SetPopeColor(shr[23]&&PR_RG4);
    // УУН (Вкл.)
    Pnl_Uun_On      -> Font -> Color = SetPopeColor(shr[24]);
    // БПИИ (Вкл.)
    Pnl_Bpii_On     -> Font -> Color = SetPopeColor(shr[33]);
    // БПИИ (Откл.)
    Pnl_Bpii_Off    -> Font -> Color = SetPopeColor(shr[34]);
    // БПМ1 (Вкл.)
    Pnl_Bpm1_On     -> Font -> Color = SetPopeColor(shr[35]);
    // БПМ1 (Откл.)
    Pnl_Bpm1_Off    -> Font -> Color = SetPopeColor(shr[37]);
    // БПМ2 (Вкл.)
    Pnl_Bpm2_On     -> Font -> Color = SetPopeColor(shr[36]);
    // БПМ2 (Откл.)
    Pnl_Bpm2_Off    -> Font -> Color = SetPopeColor(shr[37]);
    
    // Нагрев кам (вкл.)
    Pnl_Nagr_Kam_On -> Font -> Color = SetPopeColor(shr[29]);
    // Нагрев кам (Откл.)
    Pnl_Nagr_Kam_Off-> Font -> Color = SetPopeColor(shr[30]);
    // Нагрев п/д (вкл.)
    Pnl_Nagr_Pd_On  -> Font -> Color = SetPopeColor(shr[31]);
    // Нагрев п/д (Откл.)
    Pnl_Nagr_Pd_Off -> Font -> Color = SetPopeColor(shr[32]);
    // Манипулятор в HOME
    Pnl_Man_Home    -> Font -> Color = SetPopeColor(shr[12]);
    // Манипулятор в камеру
    Pnl_Man_Kam     -> Font -> Color = SetPopeColor(shr[13]);
    // Транспортный тест вкл.
    Pnl_Transp_On   -> Font -> Color = SetPopeColor(shr[9]&&!PR_TRTEST);
    // Транспортный тест выкл.
    Pnl_Transp_Off   -> Font -> Color = SetPopeColor(shr[9]&&PR_TRTEST);
    //Чиллер вкл
    Pnl_Chil_On_R   -> Font -> Color = SetPopeColor(zin[2]&0x20);
    //Чиллер откл
    Pnl_Chil_Off_R  -> Font -> Color = SetPopeColor(!(zin[2]&0x20));
    //Чиллер п/д вкл
    Pnl_Chil_Pd_On  -> Font -> Color = SetPopeColor(shr[38]);
    //Чиллер п/д откл
    Pnl_Chil_Pd_Off -> Font -> Color = SetPopeColor(shr[39]);
    //КН вкл
    Pnl_Kn_On       -> Font -> Color = SetPopeColor(shr[40]);
    //КН откл
    Pnl_Kn_Off      -> Font -> Color = SetPopeColor(shr[41]);
    //Вращение вкл
    Pnl_Vrash_On    -> Font -> Color = SetPopeColor(shr[16]);
    //Вращение откл
    Pnl_Vrash_Off   -> Font -> Color = SetPopeColor(shr[17]);
    //Подъёмник п/д в HOME
    Pnl_Pod_Home    -> Font -> Color = SetPopeColor(shr[14]);
    Pnl_PerH    -> Font -> Color = SetPopeColor(shr[14]);
    //Подъёмник п/д старт
    Pnl_PerS    -> Font -> Color = SetPopeColor(shr[15]);
    //Подъёмник п/д в положение напыления
    Pnl_Pod_Nap     -> Font -> Color = SetPopeColor(shr[18]);
    //Подъёмник п/д в положение очистки
    Pnl_Pod_Och     -> Font -> Color = SetPopeColor(shr[42]);
    // ВЧГ п/д вкл
    Pnl_Vchg_On     -> Font -> Color = SetPopeColor(shr[28]);
    //Щелевой затвор открыть
    Pnl_ShZatv_On   -> Font -> Color = SetPopeColor(shr[10]);
    //Щелевой затвор закрыть
    Pnl_ShZatv_Off  -> Font -> Color = SetPopeColor(shr[11]);
    //ДЗ открыть
    PnlDZOn         -> Font -> Color = SetPopeColor(shr[25]);
    //ДЗ закрыть
    PnlDZOff        -> Font -> Color = SetPopeColor(shr[26]);
    //ДЗ на угол
    PnlDZUgol       -> Font -> Color = SetPopeColor(shr[27]);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--Визуализация отображения воды--//
void TForm1::VisualVoda()
{


 //Есть охлаждение ИИ
if(zin[0]&0x01)
PnlKan01->Color=0x00EAD999;
else
PnlKan01->Color=0x003030FF;

 //Есть охлаждение М1
if(zin[0]&0x02)
PnlKan02->Color=0x00EAD999;
else
PnlKan02->Color=0x003030FF;

//Есть охлаждение М2
if(zin[0]&0x04)
PnlKan03->Color=0x00EAD999;
else
PnlKan03->Color=0x003030FF;

//Есть охлаждение КН
if(zin[0]&0x08)
PnlKan04->Color=0x00EAD999;
else
PnlKan04->Color=0x003030FF;

//Есть охлаждение ТМН и форнасоса
if(zin[0]&0x10)
PnlKan05->Color=0x00EAD999;
else
PnlKan05->Color=0x003030FF;

//Есть охлаждение резерв
if(zin[0]&0x20)
PnlKan06->Color=0x00EAD999;
else
PnlKan06->Color=0x003030FF;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Температура воды охлаждения ИИ
    PnlKan01 -> Caption = FloatToStrF((((float)aik[0]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[0]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan01 -> Caption = "0,0°C"; }
    // Температура воды охлаждения М1
    PnlKan02 -> Caption = FloatToStrF((((float)aik[1]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[1]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan02 -> Caption = "0,0°C"; }
    // Температура воды охлаждения М2
    PnlKan03 -> Caption = FloatToStrF((((float)aik[2]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[2]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan03 -> Caption = "0,0°C"; }
    // Температура воды охлаждения КН
    PnlKan04 -> Caption = FloatToStrF((((float)aik[3]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[3]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan04 -> Caption = "0,0°C"; }
    // Температура воды охлаждения ТМН и форнасоса
    PnlKan05 -> Caption = FloatToStrF((((float)aik[4]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[4]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan05 -> Caption = "0,0°C"; }
    // Температура воды охлаждения резерв
    PnlKan06 -> Caption = FloatToStrF((((float)aik[5]*10.0/4095.0)-1.0)/0.04, ffFixed, 5, 1) + "°C";
    if(((((float)aik[5]*10.0/4095.0)-1.0)/0.04)<0) { PnlKan06 -> Caption = "0,0°C"; }

}
//---------------------------------------------------------------------------
//--Визуализация элементов мнемосхемы--//
//---------------------------------------------------------------------------
void TForm1::VisualColorElement()
{

        anim_fase = !anim_fase;

            // красный
    if((diagn[14])|| (diagn[28]))
	{
		SetOut(1,2,0x40); //
	}
    else
	{
		SetOut(0,2,0x40); //
	}

    // желтый
    if((pr_yel)||(!shr[1]&&!shr[2]&&!shr[3]&&!shr[4]&&!shr[5]&&!shr[6]&&!shr[7]&&!shr[9]))
	{
		SetOut(1,2,0x20); //
	}
    else
	{
		SetOut(0,2,0x20); //
	}

    // зеленый
    if(shr[1]||shr[2]||shr[3]||shr[4]||shr[5]||shr[6]||shr[7]||shr[9])
	{
		SetOut(1,2,0x10); //
	}
    else
	{
		SetOut(0,2,0x10); //

	}

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[20]||shr[24]) { rrg1 -> Visible = true;   }  // РРГ1
  else        { rrg1 -> Visible = false;  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[21]) { rrg2 -> Visible = true;   }  // РРГ2
  else        { rrg2 -> Visible = false;  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[22]) { rrg3 -> Visible = true;   }  // РРГ3
  else        { rrg3 -> Visible = false;  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(shr[23]) { rrg4 -> Visible = true;   }  // РРГ4
  else        { rrg4 -> Visible = false;  }
//------клапана---------------------------------------------------------------//
//кл1
    if(out[2]&0x100)
        Kl1-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl1-> Picture->Bitmap = 0;
//кл2
    if(out[2]&0x200)
        Kl2-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl2-> Picture->Bitmap = 0;
//кл3
    if(out[2]&0x400)
        Kl3-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl3-> Picture->Bitmap = 0;
//кл4
    if(out[2]&0x1000)
        Kl4-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
    else
        Kl4-> Picture->Bitmap = 0;
//кл5
    if(out[2]&0x2000)
        Kl5-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl5-> Picture->Bitmap = 0;
//кл6
    if(out[3]&0x02)
        Kl6-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl6-> Picture->Bitmap = 0;
//кл7
    if(out[2]&0x800)
        Kl7-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl7-> Picture->Bitmap = 0;
//кл8
    if(out[3]&0x04)
        Kl8-> Picture->Bitmap = e_klg_o->Picture->Bitmap;
    else
        Kl8-> Picture->Bitmap = 0;
//Фк-Кам
    switch(zin[0]&0x300)
    {
        case 0x000:{Fk_Kam-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
        case 0x100:{Fk_Kam-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; }break;
        case 0x200:{Fk_Kam-> Picture->Bitmap = 0;                        }break;
        case 0x300:{Fk_Kam-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; }break;
    }
//Фк-шл
    switch(zin[0]&0xC000)
    {
        case 0x0000:{Fk_Shl-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break;}
        case 0x4000:{Fk_Shl-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; break;}
        case 0x8000:{Fk_Shl-> Picture->Bitmap = 0;                        break;}
        case 0xC000:{Fk_Shl-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break;}
    }
//Фк-шл мягк
    if(out[0]&0x200)
        Fk_Shl_M-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap;
    else
        Fk_Shl_M-> Picture->Bitmap =0;
//Фк-КН
    switch(zin[0]&0x3000)
    {
        case 0x0000:{Fk_Kn-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break;}
        case 0x1000:{Fk_Kn-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; break;}
        case 0x2000:{Fk_Kn-> Picture->Bitmap = 0;                        break;}
        case 0x3000:{Fk_Kn-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break;}
    }
//Фк-ТМН
    switch(zin[0]&0xC00)
    {
        case 0x000:{Fk_Tmn-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break;}
        case 0x400:{Fk_Tmn-> Picture->Bitmap = e_klg_o_vert->Picture->Bitmap; break;}
        case 0x800:{Fk_Tmn-> Picture->Bitmap = 0;                        break;}
        case 0xC00:{Fk_Tmn-> Picture->Bitmap = e_klg_n_vert->Picture->Bitmap; break;}
    }
//Кл-НАП1
    if(out[4]&0x8000)
        Kl_Nap1-> Picture->Bitmap =e_klg_o->Picture->Bitmap;
    else
        Kl_Nap1-> Picture->Bitmap =0;
//Кл-НАП2
    if(out[3]&0x01)
        Kl_Nap2-> Picture->Bitmap =e_klg_o_vert->Picture->Bitmap;
    else
        Kl_Nap2-> Picture->Bitmap =0;
//Затвор
    switch(zin[2]&0xC00)
    {
        case 0x000:{Zatv-> Picture->Bitmap = e_klg_n->Picture->Bitmap; break;}
        case 0x400:{Zatv-> Picture->Bitmap = e_klg_o->Picture->Bitmap; break;}
        case 0x800:{Zatv-> Picture->Bitmap = 0;                        break;}
        case 0xC00:{Zatv-> Picture->Bitmap = e_klg_n->Picture->Bitmap; break;}
    }
//---механизмы----------------------------------------------------------------//
    //дверь шлюза
    if(zin[2]&0x8000)
        door->Visible=true;
    else
        door->Visible=false;
    //щелевой затвор
    switch(zin[2]&0x3000)
    {
        case 0x0000:{ShZatv-> Picture->Bitmap =ShZatvNo-> Picture->Bitmap;  break; }
        case 0x1000:{ShZatv-> Picture->Bitmap =0;                           break; }
        case 0x2000:{ShZatv-> Picture->Bitmap =ShZatvCl-> Picture->Bitmap;  break; }
        case 0x3000:{ShZatv-> Picture->Bitmap =ShZatvNo-> Picture->Bitmap;  break; }
    }
    //заслонка п/д

    //заслонка ии
    switch(zin[3]&0x24)
    {
        case 0x00:{ZaslII-> Picture->Bitmap =ii_n-> Picture->Bitmap;break;}
        case 0x04:{ZaslII-> Picture->Bitmap =ii_o-> Picture->Bitmap;break;}
        case 0x20:{ZaslII-> Picture->Bitmap =0;break;}
        case 0x24:{ZaslII-> Picture->Bitmap =ii_n-> Picture->Bitmap;break;}
    }
    //заслонка м1
    switch(zin[3]&0x09)
    {
        case 0x00:{ZaslM1-> Picture->Bitmap =m1_n-> Picture->Bitmap;break;}
        case 0x01:{ZaslM1-> Picture->Bitmap =m1_o-> Picture->Bitmap;break;}
        case 0x08:{ZaslM1-> Picture->Bitmap =0;break;}
        case 0x09:{ZaslM1-> Picture->Bitmap =m1_n-> Picture->Bitmap;break;}
    }
    //заслонка м2
    switch(zin[3]&0x12)
    {
        case 0x00:{ZaslM2-> Picture->Bitmap =m2_n-> Picture->Bitmap;break;}
        case 0x02:{ZaslM2-> Picture->Bitmap =m2_o-> Picture->Bitmap;break;}
        case 0x10:{ZaslM2-> Picture->Bitmap =0;break;}
        case 0x12:{ZaslM2-> Picture->Bitmap =m2_n-> Picture->Bitmap;break;}
    }
    //манипулятор
    if(zin[3]&0x200)
        man->Color=clWhite;
    else
        man->Color=0x0064F046;
    switch (POL_PER)
    {
        case 0:
        {
            man->Width=80;break;


        }
        case 1:
        {

            man->Width=190;
            break;
        }
        case 2:
        {

            man->Width=190;
            break;
        }
        case 3:
        {
            man->Width=301;break;
           
        }
    }
    //подъёмник п/д
    if(zin[3]&0x1000)
        pd-> Picture->Bitmap =pd_home-> Picture->Bitmap;
    else
        pd-> Picture->Bitmap =pd_N-> Picture->Bitmap;
    //вчг анимация
    if(shr[28])
    {
        if(anim_fase)
            anim->Visible=true;
        else
            anim->Visible=false;
    }
    else
        anim->Visible=false;
    //нагрев п/д
    if(shr[31])
        p_pd-> Picture->Bitmap =p_pd_home-> Picture->Bitmap;
    else
        p_pd-> Picture->Bitmap =p_pd_N-> Picture->Bitmap;
    //вращение п/д
    if(zin[3]&0x80)
        VR_home->Visible=true;
    else
        VR_home->Visible=false;

    //негрев
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
    //вращение п/д
    if(out[4]&0x10)
        Vrash->Visible=(bool)anim_fase;
    else
        Vrash->Visible=false;
    //ИИ
    if(shr[33])
        II->Visible=true;
    else
        II->Visible=false;
    //анимация ИИ
    if(shr[33]&&VRII)
    {
        anim_II->Visible=true;
        if(anim_fase)
            anim_II->Picture->Bitmap=anim_II_1->Picture->Bitmap;
        else
            anim_II->Picture->Bitmap=anim_II_2->Picture->Bitmap;
    }
    else
        anim_II->Visible=false;
    //М1
    if(shr[35])
        M1->Visible=true;
    else
        M1->Visible=false;
    //анимация М1
    if(shr[35]&&VRBM1)
    {
        anim_m1->Visible=true;
        if(anim_fase)
            anim_m1->Picture->Bitmap=anim_M1_1->Picture->Bitmap;
        else
            anim_m1->Picture->Bitmap=anim_M1_2->Picture->Bitmap;
    }
    else
        anim_m1->Visible=false;
    //М2
    if(shr[36])
        M2->Visible=true;
    else
        M2->Visible=false;
    //анимация М2
    if(shr[36]&&VRBM2)
    {
        anim_m2->Visible=true;
        if(anim_fase)
            anim_m2->Picture->Bitmap=anim_M2_1->Picture->Bitmap;
        else
            anim_m2->Picture->Bitmap=anim_M2_2->Picture->Bitmap;
    }
    else
        anim_m2->Visible=false;
//---насосы-------------------------------------------------------------------//
//ТМН


    if(diagn[14]&0x08)
    {
        Tmn-> Picture->Bitmap =tmn_red-> Picture->Bitmap;
        str_up->Visible=false;
        str_down->Visible=false;
    } //авария
    else if((out[4]&0x40)&&(!(zin[4]&0x20)))//разгон
    {
        str_down->Visible=false;
        str_up->Visible=true;
        if(anim_fase)Tmn-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
        else         Tmn-> Picture->Bitmap =0;
    }
    else if((!(out[4]&0x40))&&(aik[15]>=POROG_OST_TMN))//торможение
    {
        str_down->Visible=true;
        str_up->Visible=false;
        if(anim_fase)Tmn-> Picture->Bitmap =tmn_white-> Picture->Bitmap;
        else         Tmn-> Picture->Bitmap =0;
    }
    else if((out[4]&0x40)&&(zin[4]&0x20))//норма
    {
        str_down->Visible=false;
        str_up->Visible=false;
        Tmn-> Picture->Bitmap =tmn_white-> Picture->Bitmap;

    }

    else
    {
        Tmn-> Picture->Bitmap =0;//выкл
        str_down->Visible=false;
        str_up->Visible=false;
    }

//ФВН шлюза
    if(zin[1]&0x01)     Fvn_Shl-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
    else                Fvn_Shl-> Picture->Bitmap =0;
//ФВН камеры
    if(!(zin[1]&0x08))     Fvn_Kam-> Picture->Bitmap =FN_red-> Picture->Bitmap;         //авария
    else if(!(zin[1]&0x04))Fvn_Kam-> Picture->Bitmap =FN_yellow-> Picture->Bitmap;         //предупреждение
    else if(zin[1]&0x02)   Fvn_Kam-> Picture->Bitmap =FN_SHL_o-> Picture->Bitmap;
    else                   Fvn_Kam-> Picture->Bitmap =0;
//КН
    if (OTVET_KN_M[0]&0x800)
        kn->Picture->Bitmap = kn_o->Picture->Bitmap;
    else
        kn->Picture->Bitmap =0;
    //ДЗ
    if(shr[27]) Dz-> Picture->Bitmap= dross-> Picture->Bitmap;
    else{
    switch(zin[2]&0x300)
    {
        case 0x000:{Dz-> Picture->Bitmap= zasl_grey-> Picture->Bitmap;break;}
        case 0x100:{Dz-> Picture->Bitmap= zasl_white-> Picture->Bitmap; break;}
        case 0x200:{Dz-> Picture->Bitmap= 0; break;}
        case 0x300:{Dz-> Picture->Bitmap= zasl_grey-> Picture->Bitmap; break;}
    }    }
    //заслонка п/д
    switch(zin[1]&0x300)
    {
        case 0x000:{ZaslPD-> Picture->Bitmap= ZaslPD_None-> Picture->Bitmap;ZaslPD->Left=492;break;}
        case 0x100:{ZaslPD-> Picture->Bitmap= ZaslPD_norm-> Picture->Bitmap;ZaslPD->Left=597; break;}
        case 0x200:{ZaslPD-> Picture->Bitmap= ZaslPD_home-> Picture->Bitmap;ZaslPD->Left=492; break;}
        case 0x300:{ZaslPD-> Picture->Bitmap= ZaslPD_None-> Picture->Bitmap;ZaslPD->Left=492;break;}
    }

//---трубы--------------------------------------------------------------------//
//1
    if((zin[1]&0x01)||((((zin[0]&0xC000)==0x4000)||(out[0]&0x200))&&(tube_2->Visible==true)))
        tube_1->Visible=true;
    else
        tube_1->Visible=false;
//2
    if((out[4]&0x8000)||((((zin[0]&0xC000)==0x4000)||(out[0]&0x200))&&(tube_1->Visible==true)))
        tube_2->Visible=true;
    else
        tube_2->Visible=false;
//3
    if(out[3]&0x01)
        tube_3->Visible=true;
    else
        tube_3->Visible=false;
//4
    if(out[3]&0x02)
        tube_4->Visible=true;
    else
        tube_4->Visible=false;
//5
    if((shr[23])||((out[2]&0x2000)&&(tube_6->Visible==true)))
        tube_5->Visible=true;
    else
        tube_5->Visible=false;
//6
    if((zin[1]&0x02)||((out[2]&0x2000)&&(tube_5->Visible==true))||(((zin[0]&0x3000)==0x1000)&&(tube_14->Visible==true)))
        tube_6->Visible=true;
    else
        tube_6->Visible=false;
//7
    if((out[2]&0x1000)&&(tube_5->Visible==true))
        tube_7->Visible=true;
    else
        tube_7->Visible=false;
    if(tube_7->Visible==true)
        tube_up-> Picture->Bitmap=tube_up_open-> Picture->Bitmap;
    else
        tube_up-> Picture->Bitmap=tube_up_close-> Picture->Bitmap;
//9
    if(((zin[0]&0x300)==0x100)&&(tube_6->Visible==true))
        tube_9->Visible=true;
    else
        tube_9->Visible=false;
//10
    if(((zin[0]&0xC00)==0x400)&&(tube_6->Visible==true))
        tube_10->Visible=true;
    else
        tube_10->Visible=false;
//11
    if((out[4]&0x40)&&(tube_10->Visible==true))
        tube_11->Visible=true;
    else
        tube_11->Visible=false;
//12
    if((zin[2]&0x100)&&(tube_11->Visible==true))
        tube_12->Visible=true;
    else
        tube_12->Visible=false;
//13
    if(((zin[2]&0xC00)==0x400)&&(tube_14->Visible==true))
        tube_13->Visible=true;
    else
        tube_13->Visible=false;
//14
    if((OTVET_KN_M[0]&0x800)||(((zin[0]&0x3000)==0x1000)&&(tube_6->Visible==true)))
        tube_14->Visible=true;
    else
        tube_14->Visible=false;
//15
    if((out[3]&0x04)&&(tube_6->Visible==true))
        tube_15->Visible=true;
    else
        tube_15->Visible=false;
//16
    if((out[2]&0x800)&&(tube_17->Visible==true))
        tube_16->Visible=true;
    else
        tube_16->Visible=false;
//17
    if(((out[2]&0x100)&&(tube_18->Visible==true))
    ||((out[2]&0x200)&&(tube_19->Visible==true))
    ||((out[2]&0x400)&&(tube_20->Visible==true)))
        tube_17->Visible=true;
    else
        tube_17->Visible=false;
//18
    if(((out[3]&0x100)&&(tube_17->Visible==true))||(shr[20]||shr[24]))
        tube_18->Visible=true;
    else
        tube_18->Visible=false;
//19
    if(((out[3]&0x200)&&(tube_17->Visible==true))||(shr[21]))
        tube_19->Visible=true;
    else
        tube_19->Visible=false;
//20
    if(((out[3]&0x400)&&(tube_17->Visible==true))||(shr[22]))
        tube_20->Visible=true;
    else
        tube_20->Visible=false;
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void __fastcall TForm1::Timer_250Timer(TObject *Sender)
{
   VisualFormat();
   VisualDebug();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//--Визуализация параметров автомата--//                                                         FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffExponent,3,8);
//---------------------------------------------------------------------------
void TForm1::VisualParA()
{
//N_ST=1**********************************************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon1_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon1_15 -> Text =FloatToStrF((float)par[1][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon1_15 -> Text =FloatToStrF((float)par[1][15]/81.92, ffFixed, 5, 1);
    EdtAKon1_12 -> Text =FloatToStrF((float)par[1][12], ffFixed, 5, 0);                     //время процесса
//N_ST=2**********************************************************************************************************
    EdtAKon2_4  -> Text =FloatToStrF(pow(10,(float)par[2][4]/1000.0-3.5),ffFixed, 5, 1);                     //давление РРТ200
    EdtAKon2_8  -> Text =FloatToStrF((float)par[2][8]/ 4095.0* CESAR_MAX_PD, ffFixed, 5, 0);//Мощность ВЧГ
    EdtAKon2_9  -> Text =FloatToStrF((float)par[2][9]/10000.0*100.0, ffFixed, 5, 1);                 //Процент открытия ДЗ
    EdtAKon2_12 -> Text =FloatToStrF((float)par[2][12], ffFixed, 5, 0);                     //Время процесса
//N_ST=3**********************************************************************************************************
    EdtAKon3_0  -> Text =FloatToStrF((float)par[3][0]/4095.0*RRG1_MAX, ffFixed, 5, 2);      //Расход РРГ1
    EdtAKon3_5  -> Text =FloatToStrF((float)par[3][5]/ 4095.0 * 500.0, ffFixed, 5, 1);      //Ток ИИ
    EdtAKon3_9  -> Text =FloatToStrF((float)par[3][9]/10000.0*100.0, ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon3_12 -> Text =FloatToStrF((float)par[3][12], ffFixed, 5, 0);                     //Время процесса
//N_ST=4**********************************************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon4_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon4_15 -> Text =FloatToStrF((float)par[4][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon4_15 -> Text =FloatToStrF((float)par[4][15]/81.92, ffFixed, 5, 1);
    EdtAKon4_1  -> Text =FloatToStrF((float)par[4][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //Расход РРГ2
    EdtAKon4_2  -> Text =FloatToStrF((float)par[4][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //Расход РРГ3
    EdtAKon4_4  -> Text =FloatToStrF(pow(10,(float)par[4][4]/1000.0-3.5),ffFixed, 5, 2);                     //давление РРТ200
    EdtAKon4_6  -> Text =FloatToStrF((float)par[4][6]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М1
    EdtAKon4_7  -> Text =FloatToStrF((float)par[4][7]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М2
    EdtAKon4_8  -> Text =FloatToStrF((float)par[4][8]/4095.0*CESAR_MAX_PD,ffFixed, 5, 0);   //Мощность ВЧГ
    EdtAKon4_13 -> Text =FloatToStrF((float)par[4][13]/10000.0*100.0 , ffFixed, 5, 1);               //Процент открытия ДЗ
    EdtAKon4_9  -> Text =FloatToStrF((float)par[4][9]/10000.0*100.0 , ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon4_14 -> Text =FloatToStrF((float)par[4][14] , ffFixed, 5, 0);                    //Время отпыла
    EdtAKon4_12 -> Text =FloatToStrF((float)par[4][12] , ffFixed, 5, 0);                    //Время процесса
//N_ST=5************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon5_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon5_15 -> Text =FloatToStrF((float)par[5][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon5_15 -> Text =FloatToStrF((float)par[5][15]/81.92, ffFixed, 5, 1);
    EdtAKon5_1  -> Text =FloatToStrF((float)par[5][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //Расход РРГ2
    EdtAKon5_2  -> Text =FloatToStrF((float)par[5][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //Расход РРГ3
    EdtAKon5_4  -> Text =FloatToStrF(pow(10,(float)par[5][4]/1000.0-3.5),ffFixed, 5, 2);                     //давление РРТ200
    EdtAKon5_6  -> Text =FloatToStrF((float)par[5][6]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М1
    EdtAKon5_7  -> Text =FloatToStrF((float)par[5][7]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М2
    EdtAKon5_8  -> Text =FloatToStrF((float)par[5][8]/4095.0*CESAR_MAX_PD,ffFixed, 5, 0);   //Мощность ВЧГ
    EdtAKon5_13 -> Text =FloatToStrF((float)par[5][13]/10000.0*100.0 , ffFixed, 5, 1);               //Процент открытия ДЗ
    EdtAKon5_9  -> Text =FloatToStrF((float)par[5][9]/10000.0*100.0 , ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon5_14 -> Text =FloatToStrF((float)par[5][14] , ffFixed, 5, 0);                    //Время отпыла
    EdtAKon5_12 -> Text =FloatToStrF((float)par[5][12] , ffFixed, 5, 0);                    //Время процесса
//N_ST=6************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon6_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon6_15 -> Text =FloatToStrF((float)par[6][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon6_15 -> Text =FloatToStrF((float)par[6][15]/81.92, ffFixed, 5, 1);
    EdtAKon6_1  -> Text =FloatToStrF((float)par[6][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //Расход РРГ2
    EdtAKon6_2  -> Text =FloatToStrF((float)par[6][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //Расход РРГ3
    EdtAKon6_4  -> Text =FloatToStrF(pow(10,(float)par[6][4]/1000.0-3.5),ffFixed, 5, 2);                     //давление РРТ200
    EdtAKon6_6  -> Text =FloatToStrF((float)par[6][6]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М1
    EdtAKon6_7  -> Text =FloatToStrF((float)par[6][7]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М2
    EdtAKon6_8  -> Text =FloatToStrF((float)par[6][8]/4095.0*CESAR_MAX_PD,ffFixed, 5, 0);   //Мощность ВЧГ
    EdtAKon6_13 -> Text =FloatToStrF((float)par[6][13]/10000.0*100.0 , ffFixed, 5, 1);               //Процент открытия ДЗ
    EdtAKon6_9  -> Text =FloatToStrF((float)par[6][9]/10000.0*100.0 , ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon6_14 -> Text =FloatToStrF((float)par[6][14] , ffFixed, 5, 0);                    //Время отпыла
    EdtAKon6_12 -> Text =FloatToStrF((float)par[6][12] , ffFixed, 5, 0);                    //Время процесса
//N_ST=7************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon7_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon7_15 -> Text =FloatToStrF((float)par[7][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon7_15 -> Text =FloatToStrF((float)par[7][15]/81.92, ffFixed, 5, 1);
    EdtAKon7_1  -> Text =FloatToStrF((float)par[7][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //Расход РРГ2
    EdtAKon7_2  -> Text =FloatToStrF((float)par[7][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //Расход РРГ3
    EdtAKon7_4  -> Text =FloatToStrF(pow(10,(float)par[7][4]/1000.0-3.5),ffFixed, 5, 2);                     //давление РРТ200
    EdtAKon7_6  -> Text =FloatToStrF((float)par[7][6]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М1
    EdtAKon7_7  -> Text =FloatToStrF((float)par[7][7]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М2
    EdtAKon7_8  -> Text =FloatToStrF((float)par[7][8]/4095.0*CESAR_MAX_PD,ffFixed, 5, 0);   //Мощность ВЧГ
    EdtAKon7_13 -> Text =FloatToStrF((float)par[7][13]/10000.0*100.0 , ffFixed, 5, 1);               //Процент открытия ДЗ
    EdtAKon7_9  -> Text =FloatToStrF((float)par[7][9]/10000.0*100.0 , ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon7_14 -> Text =FloatToStrF((float)par[7][14] , ffFixed, 5, 0);                    //Время отпыла
    EdtAKon7_12 -> Text =FloatToStrF((float)par[7][12] , ffFixed, 5, 0);                    //Время процесса
//N_ST=8************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon8_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon8_15 -> Text =FloatToStrF((float)par[8][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon8_15 -> Text =FloatToStrF((float)par[8][15]/81.92, ffFixed, 5, 1);
    EdtAKon8_1  -> Text =FloatToStrF((float)par[8][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //Расход РРГ2
    EdtAKon8_2  -> Text =FloatToStrF((float)par[8][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //Расход РРГ3
    EdtAKon8_4  -> Text =FloatToStrF(pow(10,(float)par[8][4]/1000.0-3.5),ffFixed, 5, 2);                     //давление РРТ200
    EdtAKon8_6  -> Text =FloatToStrF((float)par[8][6]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М1
    EdtAKon8_7  -> Text =FloatToStrF((float)par[8][7]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М2
    EdtAKon8_8  -> Text =FloatToStrF((float)par[8][8]/4095.0*CESAR_MAX_PD,ffFixed, 5, 0);   //Мощность ВЧГ
    EdtAKon8_13 -> Text =FloatToStrF((float)par[8][13]/10000.0*100.0 , ffFixed, 5, 1);               //Процент открытия ДЗ
    EdtAKon8_9  -> Text =FloatToStrF((float)par[8][9]/10000.0*100.0 , ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon8_14 -> Text =FloatToStrF((float)par[8][14] , ffFixed, 5, 0);                    //Время отпыла
    EdtAKon8_12 -> Text =FloatToStrF((float)par[8][12] , ffFixed, 5, 0);                    //Время процесса
//N_ST=9************************************************************************
    if((nasmod[4]==0)||(nasmod[6]==0))EdtAKon9_15 -> Text =0;                               //температура п/д
    else if(nasmod[6]==1)             EdtAKon9_15 -> Text =FloatToStrF((float)par[9][15]/10.0, ffFixed, 5, 1);
    else                              EdtAKon9_15 -> Text =FloatToStrF((float)par[9][15]/81.92, ffFixed, 5, 1);
    EdtAKon9_1  -> Text =FloatToStrF((float)par[9][1]/4095.0*RRG2_MAX, ffFixed, 5, 1);      //Расход РРГ2
    EdtAKon9_2  -> Text =FloatToStrF((float)par[9][2]/4095.0*RRG3_MAX, ffFixed, 5, 1);      //Расход РРГ3
    EdtAKon9_4  -> Text =FloatToStrF(pow(10,(float)par[9][4]/1000.0-3.5),ffFixed, 5, 2);                    //давление РРТ200
    EdtAKon9_6  -> Text =FloatToStrF((float)par[9][6]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М1
    EdtAKon9_7  -> Text =FloatToStrF((float)par[9][7]/4095.0*3600 , ffFixed, 5, 0);         //Мощность М2
    EdtAKon9_8  -> Text =FloatToStrF((float)par[9][8]/4095.0*CESAR_MAX_PD,ffFixed, 5, 0);   //Мощность ВЧГ
    EdtAKon9_13 -> Text =FloatToStrF((float)par[9][13]/10000.0*100.0 , ffFixed, 5, 1);               //Процент открытия ДЗ
    EdtAKon9_9  -> Text =FloatToStrF((float)par[9][9]/10000.0*100.0 , ffFixed, 5, 1);                //Процент открытия ДЗ
    EdtAKon9_14 -> Text =FloatToStrF((float)par[9][14] , ffFixed, 5, 0);                    //Время отпыла
    EdtAKon9_12 -> Text =FloatToStrF((float)par[9][12] , ffFixed, 5, 0);                    //Время процесса
//N_ST=10***********************************************************************
    EdtAKon10_4  -> Text =FloatToStrF(pow(10,(float)par[10][4]/1000.0-3.5),ffFixed, 5, 2);                   //давление РРТ200
    EdtAKon10_6  -> Text =FloatToStrF((float)par[10][6]/4095.0*3600 , ffFixed, 5, 0);       //Мощность М1
    EdtAKon10_9  -> Text =FloatToStrF((float)par[10][9]/10000.0*100.0 , ffFixed, 5, 1);              //Процент открытия ДЗ
    EdtAKon10_12 -> Text =FloatToStrF((float)par[10][12] , ffFixed, 5, 0);                  //Время процесса
//N_ST=11***********************************************************************
    EdtAKon11_4  -> Text =FloatToStrF(pow(10,(float)par[11][4]/1000.0-3.5),ffFixed, 5, 2);                   //давление РРТ200
    EdtAKon11_7  -> Text =FloatToStrF((float)par[11][7]/4095.0*3600 , ffFixed, 5, 0);       //Мощность М2
    EdtAKon11_9  -> Text =FloatToStrF((float)par[11][9]/10000.0*100.0 , ffFixed, 5, 0);              //Процент открытия ДЗ
    EdtAKon11_12 -> Text =FloatToStrF((float)par[11][12] , ffFixed, 5, 0);                  //Время процесса
//N_ST=12***********************************************************************
    EdtAKon12_4  -> Text =FloatToStrF(pow(10,(float)par[12][4]/1000.0-3.5),ffFixed, 5, 2);                   //давление РРТ200
    EdtAKon12_6  -> Text =FloatToStrF((float)par[12][6]/4095.0*3600 , ffFixed, 5, 0);       //Мощность М1
    EdtAKon12_7  -> Text =FloatToStrF((float)par[12][7]/4095.0*3600 , ffFixed, 5, 0);       //Мощность М2
    EdtAKon12_9  -> Text =FloatToStrF((float)par[12][9]/10000.0*100.0 , ffFixed, 5, 1);              //Процент открытия ДЗ
    EdtAKon12_12 -> Text =FloatToStrF((float)par[12][12] , ffFixed, 5, 0);                  //Время процесса
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
    //N_ST=1 Нагрев------------------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[1][15]=0;
    else if(nasmod[6]==1)   par[1][15]=StrToFloat  ( EdtARed1_15->Text ) * 10.0;
    else                    par[1][15]=StrToFloat  ( EdtARed1_15->Text ) * 81.92;
    par[1][12]= StrToInt    ( EdtARed1_12->Text );                          //Время процесса
    //N_ST=2 ВЧ-Очистка--------------------------------------------------------------------------
    par[2][4]=(3.5+log10(StrToFloat(EdtARed2_4->Text)))*1000.0;//давление по РРТ
    par[2][8]=  StrToFloat  ( EdtARed2_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[2][9]=  StrToFloat  ( EdtARed2_9->Text )*10000.0/100.0;                      //процент открытия дз
    par[2][12]= StrToInt    ( EdtARed2_12->Text );                          //премя процесса
    //N_ST=3 Ионная очистка----------------------------------------------------------------------
    par[3][0]=  StrToFloat  ( EdtARed3_0->Text ) / RRG1_MAX * 4095.0;       //РРГ1
    par[3][5]=  StrToFloat  ( EdtARed3_5->Text ) * 4095.0 / 500.0;          //Ток ИИ
    par[3][9]=  StrToFloat  ( EdtARed3_9->Text )*10000.0/100.0;                      //процент открытия дз
    par[3][12]= StrToInt    ( EdtARed3_12->Text );                          //Время процесса
    //N_ST=4 Напыление слой 1--------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[4][15]=0;
    else if(nasmod[6]==1)   par[4][15]=StrToFloat  ( EdtARed4_15->Text ) * 10.0;
    else                    par[4][15]=StrToFloat  ( EdtARed4_15->Text ) * 81.92;
    par[4][1]=  StrToFloat  ( EdtARed4_1->Text ) / RRG2_MAX * 4095.0;       //РРГ2
    par[4][2]=  StrToFloat  ( EdtARed4_2->Text ) / RRG3_MAX * 4095.0;       //РРГ3
    par[4][4]=(3.5+log10(StrToFloat(EdtARed4_4->Text)))*1000.0;//давление по РРТ
    par[4][6]=  StrToFloat  ( EdtARed4_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[4][7]=  StrToFloat  ( EdtARed4_7-> Text) * 4095.0 / 3600;           //мощность М2
    par[4][8]=  StrToFloat  ( EdtARed4_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[4][13]= StrToFloat  ( EdtARed4_13->Text ) *10000.0/100.0;                    //процент открытия дз
    par[4][9]=  StrToFloat  ( EdtARed4_9->Text ) *10000.0/100.0;                     //процент открытия дз
    par[4][14]= StrToInt    ( EdtARed4_14->Text );                          //время отпыла
    par[4][12]= StrToInt    ( EdtARed4_12->Text );                          //Время процесса
    //N_ST=5 Напыление слой 2--------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[5][15]=0;
    else if(nasmod[6]==1)   par[5][15]=StrToFloat  ( EdtARed5_15->Text ) * 10.0;
    else                    par[5][15]=StrToFloat  ( EdtARed5_15->Text ) * 81.92;
    par[5][1]=  StrToFloat  ( EdtARed5_1->Text ) / RRG2_MAX * 4095.0;       //РРГ2
    par[5][2]=  StrToFloat  ( EdtARed5_2->Text ) / RRG3_MAX * 4095.0;       //РРГ3
    par[5][4]=(3.5+log10(StrToFloat(EdtARed5_4->Text)))*1000.0;//давление по РРТ
    par[5][6]=  StrToFloat  ( EdtARed5_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[5][7]=  StrToFloat  ( EdtARed5_7-> Text) * 4095.0 / 3600;           //мощность М2
    par[5][8]=  StrToFloat  ( EdtARed5_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[5][13]= StrToFloat  ( EdtARed5_13->Text ) *10000.0/100.0;                    //процент открытия дз
    par[5][9]=  StrToFloat  ( EdtARed5_9->Text ) *10000.0/100.0;                     //процент открытия дз
    par[5][14]= StrToInt    ( EdtARed5_14->Text );                          //время отпыла
    par[5][12]= StrToInt    ( EdtARed5_12->Text );                          //Время процесса
    //N_ST=6 Напыление слой 3--------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[6][15]=0;
    else if(nasmod[6]==1)   par[6][15]=StrToFloat  ( EdtARed6_15->Text ) * 10.0;
    else                    par[6][15]=StrToFloat  ( EdtARed6_15->Text ) * 81.92;
    par[6][1]=  StrToFloat  ( EdtARed6_1->Text ) / RRG2_MAX * 4095.0;       //РРГ2
    par[6][2]=  StrToFloat  ( EdtARed6_2->Text ) / RRG3_MAX * 4095.0;       //РРГ3
    par[6][4]=(3.5+log10(StrToFloat(EdtARed6_4->Text)))*1000.0;//давление по РРТ
    par[6][6]=  StrToFloat  ( EdtARed6_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[6][7]=  StrToFloat  ( EdtARed6_7-> Text) * 4095.0 / 3600;           //мощность М2
    par[6][8]=  StrToFloat  ( EdtARed6_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[6][13]= StrToFloat  ( EdtARed6_13->Text ) *10000.0/100.0;                    //процент открытия дз
    par[6][9]=  StrToFloat  ( EdtARed6_9->Text ) *10000.0/100.0;                     //процент открытия дз
    par[6][14]= StrToInt    ( EdtARed6_14->Text );                          //время отпыла
    par[6][12]= StrToInt    ( EdtARed6_12->Text );                          //Время процесса
    //N_ST=7 Напыление слой 4--------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[7][15]=0;
    else if(nasmod[6]==1)   par[7][15]=StrToFloat  ( EdtARed7_15->Text ) * 10.0;
    else                    par[7][15]=StrToFloat  ( EdtARed7_15->Text ) * 81.92;
    par[7][1]=  StrToFloat  ( EdtARed7_1->Text ) / RRG2_MAX * 4095.0;       //РРГ2
    par[7][2]=  StrToFloat  ( EdtARed7_2->Text ) / RRG3_MAX * 4095.0;       //РРГ3
    par[7][4]=(3.5+log10(StrToFloat(EdtARed7_4->Text)))*1000.0;//давление по РРТ
    par[7][6]=  StrToFloat  ( EdtARed7_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[7][7]=  StrToFloat  ( EdtARed7_7-> Text) * 4095.0 / 3600;           //мощность М2
    par[7][8]=  StrToFloat  ( EdtARed7_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[7][13]= StrToFloat  ( EdtARed7_13->Text ) *10000.0/100.0;                    //процент открытия дз
    par[7][9]=  StrToFloat  ( EdtARed7_9->Text ) *10000.0/100.0;                     //процент открытия дз
    par[7][14]= StrToInt    ( EdtARed7_14->Text );                          //время отпыла
    par[7][12]= StrToInt    ( EdtARed7_12->Text );                          //Время процесса
    //N_ST=8 Напыление слой 5--------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[8][15]=0;
    else if(nasmod[6]==1)   par[8][15]=StrToFloat  ( EdtARed8_15->Text ) * 10.0;
    else                    par[8][15]=StrToFloat  ( EdtARed8_15->Text ) * 81.92;
    par[8][1]=  StrToFloat  ( EdtARed8_1->Text ) / RRG2_MAX * 4095.0;       //РРГ2
    par[8][2]=  StrToFloat  ( EdtARed8_2->Text ) / RRG3_MAX * 4095.0;       //РРГ3
    par[8][4]=  (3.5+log10(StrToFloat(EdtARed8_4->Text)))*1000.0;//давление по РРТ
    par[8][6]=  StrToFloat  ( EdtARed8_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[8][7]=  StrToFloat  ( EdtARed8_7-> Text) * 4095.0 / 3600;           //мощность М2
    par[8][8]=  StrToFloat  ( EdtARed8_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[8][13]= StrToFloat  ( EdtARed8_13->Text ) *10000.0/100.0;                    //процент открытия дз
    par[8][9]=  StrToFloat  ( EdtARed8_9->Text ) *10000.0/100.0;                     //процент открытия дз
    par[8][14]= StrToInt    ( EdtARed8_14->Text );                          //время отпыла
    par[8][12]= StrToInt    ( EdtARed8_12->Text );                          //Время процесса
    //N_ST=9 Напыление слой 6--------------------------------------------------------------------
    if((nasmod[4]==0)||(nasmod[6]==0))par[9][15]=0;
    else if(nasmod[6]==1)   par[9][15]=StrToFloat  ( EdtARed9_15->Text ) * 10.0;
    else                    par[9][15]=StrToFloat  ( EdtARed9_15->Text ) * 81.92;
    par[9][1]=  StrToFloat  ( EdtARed9_1->Text ) / RRG2_MAX * 4095.0;       //РРГ2
    par[9][2]=  StrToFloat  ( EdtARed9_2->Text ) / RRG3_MAX * 4095.0;       //РРГ3
    par[9][4]=(3.5+log10(StrToFloat(EdtARed9_4->Text)))*1000.0;//давление по РРТ
    par[9][6]=  StrToFloat  ( EdtARed9_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[9][7]=  StrToFloat  ( EdtARed9_7-> Text) * 4095.0 / 3600;           //мощность М2
    par[9][8]=  StrToFloat  ( EdtARed9_8->Text ) * 4095.0 / CESAR_MAX_PD;   //ВЧГ
    par[9][13]= StrToFloat  ( EdtARed9_13->Text ) *10000.0/100.0;                    //процент открытия дз
    par[9][9]=  StrToFloat  ( EdtARed9_9->Text ) *10000.0/100.0;                     //процент открытия дз
    par[9][14]= StrToInt    ( EdtARed9_14->Text );                          //время отпыла
    par[9][12]= StrToInt    ( EdtARed9_12->Text );                          //Время процесса





    MemoStat -> Lines -> Add(Label_Time -> Caption + "Переданы параметры автоматической работы:");

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Нагрев:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //1 стадия
    if ( EdtAKon1_15 -> Text != EdtARed1_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon1_15 -> Text + " -> " + EdtARed1_15 -> Text );
    if ( EdtAKon1_12 -> Text != EdtARed1_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon1_12 -> Text + " -> " + EdtARed1_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("ВЧ-очистка:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //2 стадия
    if ( EdtAKon2_4 -> Text != EdtARed2_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon2_4 -> Text + " -> " + EdtARed2_4 -> Text );
    if ( EdtAKon2_8 -> Text != EdtARed2_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon2_8 -> Text + " -> " + EdtARed2_8 -> Text );
    if ( EdtAKon2_9 -> Text != EdtARed2_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ: " + EdtAKon2_9 -> Text + " -> " + EdtARed2_9 -> Text );
    if ( EdtAKon2_12 -> Text != EdtARed2_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon2_12 -> Text + " -> " + EdtARed2_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Ионная очистка:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //3 стадия
    if ( EdtAKon3_0 -> Text != EdtARed3_0 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ1: " + EdtAKon3_0 -> Text + " -> " + EdtARed3_0 -> Text );
    if ( EdtAKon3_5 -> Text != EdtARed3_5 -> Text )
        MemoStat -> Lines -> Add("Ток ИИ: " + EdtAKon3_5 -> Text + " -> " + EdtARed3_5 -> Text );
    if ( EdtAKon3_9 -> Text != EdtARed3_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ: " + EdtAKon3_9 -> Text + " -> " + EdtARed3_9 -> Text );
    if ( EdtAKon3_12 -> Text != EdtARed3_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon3_12 -> Text + " -> " + EdtARed3_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Напыление (слой 1):");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //4 стадия
    if ( EdtAKon4_15 -> Text != EdtARed4_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon4_15 -> Text + " -> " + EdtARed4_15 -> Text );
    if ( EdtAKon4_1 -> Text != EdtARed4_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtAKon4_1 -> Text + " -> " + EdtARed4_1 -> Text );
    if ( EdtAKon4_2 -> Text != EdtARed4_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtAKon4_2 -> Text + " -> " + EdtARed4_2 -> Text );
    if ( EdtAKon4_4 -> Text != EdtARed4_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon4_4 -> Text + " -> " + EdtARed4_4 -> Text );
    if ( EdtAKon4_6 -> Text != EdtARed4_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon4_6 -> Text + " -> " + EdtARed4_6 -> Text );
    if ( EdtAKon4_7 -> Text != EdtARed4_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon4_7 -> Text + " -> " + EdtARed4_7 -> Text );
    if ( EdtAKon4_8 -> Text != EdtARed4_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon4_8 -> Text + " -> " + EdtARed4_8 -> Text );
    if ( EdtAKon4_13 -> Text != EdtARed4_13 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при отпыле: " + EdtAKon4_13 -> Text + " -> " + EdtARed4_13 -> Text );
    if ( EdtAKon4_9 -> Text != EdtARed4_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при напылении: " + EdtAKon4_9 -> Text + " -> " + EdtARed4_9 -> Text );
    if ( EdtAKon4_14 -> Text != EdtARed4_14 -> Text )
        MemoStat -> Lines -> Add("Время отпыла: " + EdtAKon4_14 -> Text + " -> " + EdtARed4_14 -> Text );
    if ( EdtAKon4_12 -> Text != EdtARed4_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon4_12 -> Text + " -> " + EdtARed4_12 -> Text );


    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Напыление (слой 2):");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //5 стадия
    if ( EdtAKon5_15 -> Text != EdtARed5_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon5_15 -> Text + " -> " + EdtARed5_15 -> Text );
    if ( EdtAKon5_1 -> Text != EdtARed5_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtAKon5_1 -> Text + " -> " + EdtARed5_1 -> Text );
    if ( EdtAKon5_2 -> Text != EdtARed5_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtAKon5_2 -> Text + " -> " + EdtARed5_2 -> Text );
    if ( EdtAKon5_4 -> Text != EdtARed5_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon5_4 -> Text + " -> " + EdtARed5_4 -> Text );
    if ( EdtAKon5_6 -> Text != EdtARed5_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon5_6 -> Text + " -> " + EdtARed5_6 -> Text );
    if ( EdtAKon5_7 -> Text != EdtARed5_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon5_7 -> Text + " -> " + EdtARed5_7 -> Text );
    if ( EdtAKon5_8 -> Text != EdtARed5_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon5_8 -> Text + " -> " + EdtARed5_8 -> Text );
    if ( EdtAKon5_13 -> Text != EdtARed5_13 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при отпыле: " + EdtAKon5_13 -> Text + " -> " + EdtARed5_13 -> Text );
    if ( EdtAKon5_9 -> Text != EdtARed5_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при напылении: " + EdtAKon5_9 -> Text + " -> " + EdtARed5_9 -> Text );
    if ( EdtAKon5_14 -> Text != EdtARed5_14 -> Text )
        MemoStat -> Lines -> Add("Время отпыла: " + EdtAKon5_14 -> Text + " -> " + EdtARed5_14 -> Text );
    if ( EdtAKon5_12 -> Text != EdtARed5_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon5_12 -> Text + " -> " + EdtARed5_12 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Напыление (слой 3):");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //6 стадия
    if ( EdtAKon6_15 -> Text != EdtARed6_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon6_15 -> Text + " -> " + EdtARed6_15 -> Text );
    if ( EdtAKon6_1 -> Text != EdtARed6_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtAKon6_1 -> Text + " -> " + EdtARed6_1 -> Text );
    if ( EdtAKon6_2 -> Text != EdtARed6_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtAKon6_2 -> Text + " -> " + EdtARed6_2 -> Text );
    if ( EdtAKon6_4 -> Text != EdtARed6_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon6_4 -> Text + " -> " + EdtARed6_4 -> Text );
    if ( EdtAKon6_6 -> Text != EdtARed6_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon6_6 -> Text + " -> " + EdtARed6_6 -> Text );
    if ( EdtAKon6_7 -> Text != EdtARed6_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon6_7 -> Text + " -> " + EdtARed6_7 -> Text );
    if ( EdtAKon6_8 -> Text != EdtARed6_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon6_8 -> Text + " -> " + EdtARed6_8 -> Text );
    if ( EdtAKon6_13 -> Text != EdtARed6_13 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при отпыле: " + EdtAKon6_13 -> Text + " -> " + EdtARed6_13 -> Text );
    if ( EdtAKon6_9 -> Text != EdtARed6_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при напылении: " + EdtAKon6_9 -> Text + " -> " + EdtARed6_9 -> Text );
    if ( EdtAKon6_14 -> Text != EdtARed6_14 -> Text )
        MemoStat -> Lines -> Add("Время отпыла: " + EdtAKon6_14 -> Text + " -> " + EdtARed6_14 -> Text );
    if ( EdtAKon6_12 -> Text != EdtARed6_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon6_12 -> Text + " -> " + EdtARed6_12 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Напыление (слой 4):");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //7 стадия
    if ( EdtAKon7_15 -> Text != EdtARed7_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon7_15 -> Text + " -> " + EdtARed7_15 -> Text );
    if ( EdtAKon7_1 -> Text != EdtARed7_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtAKon7_1 -> Text + " -> " + EdtARed7_1 -> Text );
    if ( EdtAKon7_2 -> Text != EdtARed7_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtAKon7_2 -> Text + " -> " + EdtARed7_2 -> Text );
    if ( EdtAKon7_4 -> Text != EdtARed7_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon7_4 -> Text + " -> " + EdtARed7_4 -> Text );
    if ( EdtAKon7_6 -> Text != EdtARed7_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon7_6 -> Text + " -> " + EdtARed7_6 -> Text );
    if ( EdtAKon7_7 -> Text != EdtARed7_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon7_7 -> Text + " -> " + EdtARed7_7 -> Text );
    if ( EdtAKon7_8 -> Text != EdtARed7_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon7_8 -> Text + " -> " + EdtARed7_8 -> Text );
    if ( EdtAKon7_13 -> Text != EdtARed7_13 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при отпыле: " + EdtAKon7_13 -> Text + " -> " + EdtARed7_13 -> Text );
    if ( EdtAKon7_9 -> Text != EdtARed7_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при напылении: " + EdtAKon7_9 -> Text + " -> " + EdtARed7_9 -> Text );
    if ( EdtAKon7_14 -> Text != EdtARed7_14 -> Text )
        MemoStat -> Lines -> Add("Время отпыла: " + EdtAKon7_14 -> Text + " -> " + EdtARed7_14 -> Text );
    if ( EdtAKon7_12 -> Text != EdtARed7_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon7_12 -> Text + " -> " + EdtARed7_12 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Напыление (слой 5):");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //8 стадия
    if ( EdtAKon8_15 -> Text != EdtARed8_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon8_15 -> Text + " -> " + EdtARed8_15 -> Text );
    if ( EdtAKon8_1 -> Text != EdtARed8_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtAKon8_1 -> Text + " -> " + EdtARed8_1 -> Text );
    if ( EdtAKon8_2 -> Text != EdtARed8_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtAKon8_2 -> Text + " -> " + EdtARed8_2 -> Text );
    if ( EdtAKon8_4 -> Text != EdtARed8_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon8_4 -> Text + " -> " + EdtARed8_4 -> Text );
    if ( EdtAKon8_6 -> Text != EdtARed8_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon8_6 -> Text + " -> " + EdtARed8_6 -> Text );
    if ( EdtAKon8_7 -> Text != EdtARed8_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon8_7 -> Text + " -> " + EdtARed8_7 -> Text );
    if ( EdtAKon8_8 -> Text != EdtARed8_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon8_8 -> Text + " -> " + EdtARed8_8 -> Text );
    if ( EdtAKon8_13 -> Text != EdtARed8_13 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при отпыле: " + EdtAKon8_13 -> Text + " -> " + EdtARed8_13 -> Text );
    if ( EdtAKon8_9 -> Text != EdtARed8_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при напылении: " + EdtAKon8_9 -> Text + " -> " + EdtARed8_9 -> Text );
    if ( EdtAKon8_14 -> Text != EdtARed8_14 -> Text )
        MemoStat -> Lines -> Add("Время отпыла: " + EdtAKon8_14 -> Text + " -> " + EdtARed8_14 -> Text );
    if ( EdtAKon8_12 -> Text != EdtARed8_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon8_12 -> Text + " -> " + EdtARed8_12 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Напыление (слой 6):");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //9 стадия
    if ( EdtAKon9_15 -> Text != EdtARed9_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtAKon9_15 -> Text + " -> " + EdtARed9_15 -> Text );
    if ( EdtAKon9_1 -> Text != EdtARed9_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtAKon9_1 -> Text + " -> " + EdtARed9_1 -> Text );
    if ( EdtAKon9_2 -> Text != EdtARed9_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtAKon9_2 -> Text + " -> " + EdtARed9_2 -> Text );
    if ( EdtAKon9_4 -> Text != EdtARed9_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon9_4 -> Text + " -> " + EdtARed9_4 -> Text );
    if ( EdtAKon9_6 -> Text != EdtARed9_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon9_6 -> Text + " -> " + EdtARed9_6 -> Text );
    if ( EdtAKon9_7 -> Text != EdtARed9_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon9_7 -> Text + " -> " + EdtARed9_7 -> Text );
    if ( EdtAKon9_8 -> Text != EdtARed9_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtAKon9_8 -> Text + " -> " + EdtARed9_8 -> Text );
    if ( EdtAKon9_13 -> Text != EdtARed9_13 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при отпыле: " + EdtAKon9_13 -> Text + " -> " + EdtARed9_13 -> Text );
    if ( EdtAKon9_9 -> Text != EdtARed9_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ при напылении: " + EdtAKon9_9 -> Text + " -> " + EdtARed9_9 -> Text );
    if ( EdtAKon9_14 -> Text != EdtARed9_14 -> Text )
        MemoStat -> Lines -> Add("Время отпыла: " + EdtAKon9_14 -> Text + " -> " + EdtARed9_14 -> Text );
    if ( EdtAKon9_12 -> Text != EdtARed9_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon9_12 -> Text + " -> " + EdtARed9_12 -> Text );


    // перекрасить переданные параметры
    EdtARed1_15 -> Color = clWhite;
    EdtARed1_12 -> Color = clWhite;

    EdtARed2_4 -> Color = clWhite;
    EdtARed2_8 -> Color = clWhite;
    EdtARed2_9 -> Color = clWhite;
    EdtARed2_12 -> Color = clWhite;

    EdtARed3_0 -> Color = clWhite;
    EdtARed3_5 -> Color = clWhite;
    EdtARed3_9 -> Color = clWhite;
    EdtARed3_12 -> Color = clWhite;

    EdtARed4_15 -> Color = clWhite;
    EdtARed4_1 -> Color = clWhite;
    EdtARed4_2 -> Color = clWhite;
    EdtARed4_4 -> Color = clWhite;
    EdtARed4_6 -> Color = clWhite;
    EdtARed4_7 -> Color = clWhite;
    EdtARed4_8 -> Color = clWhite;
    EdtARed4_13 -> Color = clWhite;
    EdtARed4_9 -> Color = clWhite;
    EdtARed4_14 -> Color = clWhite;
    EdtARed4_12 -> Color = clWhite;

    EdtARed5_15 -> Color = clWhite;
    EdtARed5_1 -> Color = clWhite;
    EdtARed5_2 -> Color = clWhite;
    EdtARed5_4 -> Color = clWhite;
    EdtARed5_6 -> Color = clWhite;
    EdtARed5_7 -> Color = clWhite;
    EdtARed5_8 -> Color = clWhite;
    EdtARed5_13 -> Color = clWhite;
    EdtARed5_9 -> Color = clWhite;
    EdtARed5_14 -> Color = clWhite;
    EdtARed5_12 -> Color = clWhite;

    EdtARed6_15 -> Color = clWhite;
    EdtARed6_1 -> Color = clWhite;
    EdtARed6_2 -> Color = clWhite;
    EdtARed6_4 -> Color = clWhite;
    EdtARed6_6 -> Color = clWhite;
    EdtARed6_7 -> Color = clWhite;
    EdtARed6_8 -> Color = clWhite;
    EdtARed6_13 -> Color = clWhite;
    EdtARed6_9 -> Color = clWhite;
    EdtARed6_14 -> Color = clWhite;
    EdtARed6_12 -> Color = clWhite;

    EdtARed7_15 -> Color = clWhite;
    EdtARed7_1 -> Color = clWhite;
    EdtARed7_2 -> Color = clWhite;
    EdtARed7_4 -> Color = clWhite;
    EdtARed7_6 -> Color = clWhite;
    EdtARed7_7 -> Color = clWhite;
    EdtARed7_8 -> Color = clWhite;
    EdtARed7_13 -> Color = clWhite;
    EdtARed7_9 -> Color = clWhite;
    EdtARed7_14 -> Color = clWhite;
    EdtARed7_12 -> Color = clWhite;

    EdtARed8_15 -> Color = clWhite;
    EdtARed8_1 -> Color = clWhite;
    EdtARed8_2 -> Color = clWhite;
    EdtARed8_4 -> Color = clWhite;
    EdtARed8_6 -> Color = clWhite;
    EdtARed8_7 -> Color = clWhite;
    EdtARed8_8 -> Color = clWhite;
    EdtARed8_13 -> Color = clWhite;
    EdtARed8_9 -> Color = clWhite;
    EdtARed8_14 -> Color = clWhite;
    EdtARed8_12 -> Color = clWhite;

    EdtARed9_15 -> Color = clWhite;
    EdtARed9_1 -> Color = clWhite;
    EdtARed9_2 -> Color = clWhite;
    EdtARed9_4 -> Color = clWhite;
    EdtARed9_6 -> Color = clWhite;
    EdtARed9_7 -> Color = clWhite;
    EdtARed9_8 -> Color = clWhite;
    EdtARed9_13 -> Color = clWhite;
    EdtARed9_9 -> Color = clWhite;
    EdtARed9_14 -> Color = clWhite;
    EdtARed9_12 -> Color = clWhite;
    // обновить страницу
    VisualParA();
   
 /* int i=0,j=0,k=0,n=0,n0=0,n1=0,n2=0,n3=0,iIzmeneniya=0,ii=0;
    // перформатирование параметров РРГ1 - РРГ3 в массив par_V[][]
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

if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_10" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        // проверили на минимум
        if (valueText < -100000.0)
        {
            valueText = -100000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 100000.0)
        {
            valueText = 100000.0;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
// Расход РРГ1
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_0" )||
            (((TEdit*)Sender)->Name == "EdtARed3_0" )
        )
    {
        // кол-во знаков после запятой 1
        format = 2;
        // проверили на минимум
        if (valueText < RRG1_MIN)
        {
            valueText = RRG1_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > RRG1_MAX)
        {
            valueText = RRG1_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // Расход РРГ2
    if  (
            (((TEdit*)Sender)->Name == "EdtARed9_1" )||
            (((TEdit*)Sender)->Name == "EdtARed8_1" )||
            (((TEdit*)Sender)->Name == "EdtARed7_1" )||
            (((TEdit*)Sender)->Name == "EdtARed6_1" )||
            (((TEdit*)Sender)->Name == "EdtARed5_1" )||
            (((TEdit*)Sender)->Name == "EdtARed4_1" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_1" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
        // проверили на минимум
        if (valueText < RRG2_MIN)
        {
            valueText = RRG2_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > RRG2_MAX)
        {
            valueText = RRG2_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // Расход РРГ3
    if  (
            (((TEdit*)Sender)->Name == "EdtARed9_2" )||
            (((TEdit*)Sender)->Name == "EdtARed8_2" )||
            (((TEdit*)Sender)->Name == "EdtARed7_2" )||
            (((TEdit*)Sender)->Name == "EdtARed6_2" )||
            (((TEdit*)Sender)->Name == "EdtARed5_2" )||
            (((TEdit*)Sender)->Name == "EdtARed4_2" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_2" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
        // проверили на минимум
        if (valueText < RRG3_MIN)
        {
            valueText = RRG3_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > RRG3_MAX)
        {
            valueText = RRG3_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }

    // Расход РРГ4
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_3" )||
            (((TEdit*)Sender)->Name == "EditNastrTo2" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
        // проверили на минимум
        if (valueText < RRG4_MIN)
        {
            valueText = RRG4_MIN;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > RRG4_MAX)
        {
            valueText = RRG4_MAX;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }


    // Давление вч очистка
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_4" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
        // проверили на минимум
        if (valueText < 0.5)
        {
            valueText = 0.5;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 4)
        {
            valueText = 4;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // Давление напыление
    if  (
            (((TEdit*)Sender)->Name == "EdtARed9_4" )||
            (((TEdit*)Sender)->Name == "EdtARed8_4" )||
            (((TEdit*)Sender)->Name == "EdtARed7_4" )||
            (((TEdit*)Sender)->Name == "EdtARed6_4" )||
            (((TEdit*)Sender)->Name == "EdtARed5_4" )||
            (((TEdit*)Sender)->Name == "EdtARed4_4" )
        )
    {
        // кол-во знаков после запятой 2
        format = 2;
        // проверили на минимум
        if (valueText < 0.01)
        {
            valueText = 0.01;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 50)
        {
            valueText = 50;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // Давление остальное
    if  (
            (((TEdit*)Sender)->Name == "EdtARed10_4" )||
            (((TEdit*)Sender)->Name == "EdtARed11_4" )||
            (((TEdit*)Sender)->Name == "EdtARed12_4" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_4" )
        )
    {
        // кол-во знаков после запятой 2
        format = 2;
        // проверили на минимум
        if (valueText < 0.01)
        {
            valueText = 0.01;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 50)
        {
            valueText = 50;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // Мощность М1
    if  (
            (((TEdit*)Sender)->Name == "EdtARed9_6" )||
            (((TEdit*)Sender)->Name == "EdtARed8_6" )||
            (((TEdit*)Sender)->Name == "EdtARed7_6" )||
            (((TEdit*)Sender)->Name == "EdtARed6_6" )||
            (((TEdit*)Sender)->Name == "EdtARed5_6" )||
            (((TEdit*)Sender)->Name == "EdtARed4_6" )||
            (((TEdit*)Sender)->Name == "EdtARed10_6" )||
            (((TEdit*)Sender)->Name == "EdtARed12_6" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_6" )

        )
    {       // кол-во знаков после запятой 0
            format = 0;
        if(nasmod[8])
        {

            // проверили на минимум
            if (valueText < 0)
            {
                valueText = 0;
                ((TEdit*)Sender)->Color = clYellow;
            }
            // проверили на максимум
            else if (valueText > 3000)
            {
                valueText = 3000;
                ((TEdit*)Sender)->Color = clYellow;
            }
        }
        else
            valueText = 0;
    }
    // Мощность М2
    if  (
            (((TEdit*)Sender)->Name == "EdtARed9_7" )||
            (((TEdit*)Sender)->Name == "EdtARed8_7" )||
            (((TEdit*)Sender)->Name == "EdtARed7_7" )||
            (((TEdit*)Sender)->Name == "EdtARed6_7" )||
            (((TEdit*)Sender)->Name == "EdtARed5_7" )||
            (((TEdit*)Sender)->Name == "EdtARed4_7" )||
            (((TEdit*)Sender)->Name == "EdtARed11_7" )||
            (((TEdit*)Sender)->Name == "EdtARed12_7" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_7" )

        )
    {   // кол-во знаков после запятой 0
            format = 0;
        if(nasmod[9])
        {

            // проверили на минимум
            if (valueText < 0)
            {
                valueText = 0;
                ((TEdit*)Sender)->Color = clYellow;
            }
            // проверили на максимум
            else if (valueText > 3000)
            {
                valueText = 3000;
                ((TEdit*)Sender)->Color = clYellow;
            }
        }
        else
            valueText = 0;
    }
    // ток ИИ
    if  (
            (((TEdit*)Sender)->Name == "EdtRRed0_5" )||
            (((TEdit*)Sender)->Name == "EdtARed3_5" )

        )
    {
        // кол-во знаков после запятой 0
        format = 0;
        if(nasmod[7])
        {
            // проверили на минимум
            if (valueText < 0)
            {
                valueText = 0;
                ((TEdit*)Sender)->Color = clYellow;
            }
            // проверили на максимум
            else if (valueText > 200)
            {
                valueText = 200;
                ((TEdit*)Sender)->Color = clYellow;
            }
        }
        else
            valueText = 0;
    }
    // мощность ВЧГ
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_8" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_8" )||
            (((TEdit*)Sender)->Name == "EdtARed9_8" )||
            (((TEdit*)Sender)->Name == "EdtARed8_8" )||
            (((TEdit*)Sender)->Name == "EdtARed7_8" )||
            (((TEdit*)Sender)->Name == "EdtARed6_8" )||
            (((TEdit*)Sender)->Name == "EdtARed5_8" )||
            (((TEdit*)Sender)->Name == "EdtARed4_8" )
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
        else if (valueText > 600)
        {
            valueText = 600;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // процент открытия ДЗ
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_9" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_9" )||
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
            (((TEdit*)Sender)->Name == "EdtARed4_13" )||
            (((TEdit*)Sender)->Name == "EdtARed5_13" )||
            (((TEdit*)Sender)->Name == "EdtARed6_13" )||
            (((TEdit*)Sender)->Name == "EdtARed7_13" )||
            (((TEdit*)Sender)->Name == "EdtARed8_13" )||
            (((TEdit*)Sender)->Name == "EdtARed9_13" )

        )
    {
        // кол-во знаков после запятой 1
        format = 1;
        // проверили на минимум
        if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // время процесса мал
    if  (
            (((TEdit*)Sender)->Name == "EdtARed2_12" )||
            (((TEdit*)Sender)->Name == "EdtARed3_12" )
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
        else if (valueText > 1800)
        {
            valueText = 1800;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // время процесса бол
    if  (
            (((TEdit*)Sender)->Name == "EdtARed12_12" )||
            (((TEdit*)Sender)->Name == "EdtARed11_12" )||
            (((TEdit*)Sender)->Name == "EdtARed10_12" )||
            (((TEdit*)Sender)->Name == "EdtARed9_12" )||
            (((TEdit*)Sender)->Name == "EdtARed8_12" )||
            (((TEdit*)Sender)->Name == "EdtARed7_12" )||
            (((TEdit*)Sender)->Name == "EdtARed6_12" )||
            (((TEdit*)Sender)->Name == "EdtARed5_12" )||
            (((TEdit*)Sender)->Name == "EdtARed4_12" )||
            (((TEdit*)Sender)->Name == "EdtARed1_12" )
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
        else if (valueText > 3600)
        {
            valueText = 3600;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // время отпыла
    if  (

            (((TEdit*)Sender)->Name == "EdtARed9_14" )||
            (((TEdit*)Sender)->Name == "EdtARed8_14" )||
            (((TEdit*)Sender)->Name == "EdtARed7_14" )||
            (((TEdit*)Sender)->Name == "EdtARed6_14" )||
            (((TEdit*)Sender)->Name == "EdtARed5_14" )||
            (((TEdit*)Sender)->Name == "EdtARed4_14" )
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
        else if (valueText > 600)
        {
            valueText = 600;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // температура нагрева п/д
    if  (

            (((TEdit*)Sender)->Name == "EdtARed9_15" )||
            (((TEdit*)Sender)->Name == "EdtARed8_15" )||
            (((TEdit*)Sender)->Name == "EdtARed7_15" )||
            (((TEdit*)Sender)->Name == "EdtARed6_15" )||
            (((TEdit*)Sender)->Name == "EdtARed5_15" )||
            (((TEdit*)Sender)->Name == "EdtARed4_15" )||
            (((TEdit*)Sender)->Name == "EdtARed1_15" )||
            (((TEdit*)Sender)->Name == "EdtRRed0_15" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
        if(nasmod[6]==1)
        {
            // проверили на минимум
            if (valueText < 80)
            {
                valueText = 80;
                ((TEdit*)Sender)->Color = clYellow;
            }
            // проверили на максимум
            else if (valueText > 400)
            {
                valueText = 400;
                ((TEdit*)Sender)->Color = clYellow;
            }
        }
        else
        {
            // проверили на минимум
            if (valueText < -20)
            {
                valueText = -20;
                ((TEdit*)Sender)->Color = clYellow;
            }
            // проверили на максимум
            else if (valueText > 90)
            {
                valueText = 90;
                ((TEdit*)Sender)->Color = clYellow;
            }
        }
    }
    //настройка
    // нагрев камеры
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo5" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
       // проверили на минимум
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
        // проверили на максимум
        else if (valueText > 150)
        {
            valueText = 150;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // напряжение БПМ
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo10" )||
            (((TEdit*)Sender)->Name == "EditNastrTo11" )
        )
    {
        // кол-во знаков после запятой 1
        format = 0;
       // проверили на минимум
        if (valueText < 100)
        {
            valueText = 100;
            ((TEdit*)Sender)->Color = clYellow;
        }

        // проверили на максимум
        else if (valueText > 650)
        {
            valueText = 650;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // частота комутации
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo12" )||
            (((TEdit*)Sender)->Name == "EditNastrTo13" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
       if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }
        else if (valueText > 0&&valueText < 4)
        {
            valueText = 4;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 40)
        {
            valueText = 40;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // коэф согласования
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo14" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
       if (valueText < 4)
        {
            valueText = 4;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 20)
        {
            valueText = 20;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // скорость вращения
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo17" )||
            (((TEdit*)Sender)->Name == "EditNastrTo18" )
        )
    {
        // кол-во знаков после запятой 0
        format = 0;
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
    // температура КН
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo20" )
        )
    {
        // кол-во знаков после запятой 0
        format = 1;
       if (valueText < 14)
        {
            valueText = 14;
            ((TEdit*)Sender)->Color = clYellow;
        }
        // проверили на максимум
        else if (valueText > 18)
        {
            valueText = 18;
            ((TEdit*)Sender)->Color = clYellow;
        }
    }
    // путь подъёма
    if  (

            (((TEdit*)Sender)->Name == "EditNastrTo16" )||
            (((TEdit*)Sender)->Name == "EditNastrTo21" )
        )
    {
        // кол-во знаков после запятой 1
        format = 1;
       if (valueText < 0)
        {
            valueText = 0;
            ((TEdit*)Sender)->Color = clYellow;
        }

    }

    // проверка на изменение
    if (((TEdit*)Sender)->Color!=clYellow) ((TEdit*)Sender)->Color = clSilver;
    ((TEdit*)Sender)->Text = FloatToStrF(valueText, ffFixed, 8, format);

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
    //температура п/д
    if((nasmod[4]==0)||(nasmod[6]==0))par[0][15]=0;
    else if(nasmod[6]==1)   par[0][15]=StrToFloat  ( EdtRRed0_15->Text ) * 10.0;
    else                    par[0][15]=StrToFloat  ( EdtRRed0_15->Text ) * 81.92;
    //Расход РРГ1
    par[0][0] =StrToFloat( EdtRRed0_0 -> Text )/ RRG1_MAX * 4095.0 ;
    //Расход РРГ2
    par[0][1] =StrToFloat( EdtRRed0_1 -> Text )/ RRG2_MAX * 4095.0 ;
    //Расход РРГ3
    par[0][2] =StrToFloat( EdtRRed0_2 -> Text )/ RRG3_MAX * 4095.0 ;
    //Расход РРГ4
    par[0][3] =StrToFloat( EdtRRed0_3 -> Text )/ RRG4_MAX * 4095.0 ;
    //Давление РРТ200
    par[0][4] =(3.5+log10(StrToFloat(EdtRRed0_4->Text)))*1000.0;
    //ток ИИ
    par[0][5] =StrToFloat( EdtRRed0_5->Text ) * 4095.0 / 500.0;
    //мощность М1
    par[0][6] =StrToFloat  ( EdtRRed0_6-> Text) * 4095.0 / 3600;
    //Мощность М2
    par[0][7] =StrToFloat  ( EdtRRed0_7-> Text) * 4095.0 / 3600;
    //Мощность ВЧГ
    par[0][8] =StrToFloat  ( EdtRRed0_8->Text ) * 4095.0 / CESAR_MAX_PD;
    //Процент открытия ДЗ
    par[0][9] =StrToFloat  ( EdtRRed0_9->Text ) *10000.0/100.0;
    //Подъём подложкодержателя
    par[0][10] =StrToInt  ( EdtRRed0_10->Text );
    //скорость подъёма
    if(EdtRRed0_11 -> ItemIndex == 0)       { par[0][11]=0; }
    else if(EdtRRed0_11 -> ItemIndex == 1)  { par[0][11]=1; }
    else if(EdtRRed0_11 -> ItemIndex == 2)  { par[0][11]=2; }




    MemoStat -> Lines -> Add("");
    MemoStat -> Lines -> Add(Label_Time -> Caption + " Переданы наладочные параметры:");
    MemoStat -> Lines -> Add("");

    if ( EdtRKon0_15 -> Text != EdtRRed0_15 -> Text )
        MemoStat -> Lines -> Add("Температура п/д: " + EdtRKon0_15 -> Text + " -> " + EdtRRed0_15 -> Text );
    if ( EdtRKon0_0 -> Text != EdtRRed0_0 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ1: " + EdtRKon0_0 -> Text + " -> " + EdtRRed0_0 -> Text );
    if ( EdtRKon0_1 -> Text != EdtRRed0_1 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ2: " + EdtRKon0_1 -> Text + " -> " + EdtRRed0_1 -> Text );
    if ( EdtRKon0_2 -> Text != EdtRRed0_2 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ3: " + EdtRKon0_2 -> Text + " -> " + EdtRRed0_2 -> Text );
    if ( EdtRKon0_3 -> Text != EdtRRed0_3 -> Text )
        MemoStat -> Lines -> Add("Расход РРГ4: " + EdtRKon0_3 -> Text + " -> " + EdtRRed0_3 -> Text );
    if ( EdtRKon0_4 -> Text != EdtRRed0_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtRKon0_4 -> Text + " -> " + EdtRRed0_4 -> Text );
    if ( EdtRKon0_5 -> Text != EdtRRed0_5 -> Text )
        MemoStat -> Lines -> Add("Ток ИИ: " + EdtRKon0_5 -> Text + " -> " + EdtRRed0_5 -> Text );
    if ( EdtRKon0_6 -> Text != EdtRRed0_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtRKon0_6 -> Text + " -> " + EdtRRed0_6 -> Text );
    if ( EdtRKon0_7 -> Text != EdtRRed0_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtRKon0_7 -> Text + " -> " + EdtRRed0_7 -> Text );
    if ( EdtRKon0_8 -> Text != EdtRRed0_8 -> Text )
        MemoStat -> Lines -> Add("Мощность ВЧГ: " + EdtRKon0_8 -> Text + " -> " + EdtRRed0_8 -> Text );
    if ( EdtRKon0_9 -> Text != EdtRRed0_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ: " + EdtRKon0_9 -> Text + " -> " + EdtRRed0_9 -> Text );
    if ( EdtRKon0_10 -> Text != EdtRRed0_10 -> Text )
        MemoStat -> Lines -> Add("Подъём подложкодержателя: " + EdtRKon0_10 -> Text + " -> " + EdtRRed0_10 -> Text );
    if ( EdtRKon0_11 -> Text != EdtRRed0_11 -> Text )
        MemoStat -> Lines -> Add("Скорость подъёма: " + EdtRKon0_11 -> Text + " -> " + EdtRRed0_11 -> Text );


    // перекрасить переданные параметры
    EdtRRed0_15 -> Color = clWhite;
    EdtRRed0_0 -> Color = clWhite;
    EdtRRed0_1 -> Color = clWhite;
    EdtRRed0_2 -> Color = clWhite;
    EdtRRed0_3 -> Color = clWhite;
    EdtRRed0_4 -> Color = clWhite;
    EdtRRed0_5 -> Color = clWhite;
    EdtRRed0_6 -> Color = clWhite;
    EdtRRed0_7 -> Color = clWhite;
    EdtRRed0_8 -> Color = clWhite;
    EdtRRed0_9 -> Color = clWhite;
    EdtRRed0_10 -> Color = clWhite;
    EdtRRed0_11 -> Color = clWhite;

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

//N_ST=1
EdtALib1_15 -> Text = MemoLib -> Lines -> operator [](0);
EdtALib1_12 -> Text = MemoLib -> Lines -> operator [](1);
//N_ST=2
EdtALib2_4 -> Text = MemoLib -> Lines -> operator [](2);
EdtALib2_8 -> Text = MemoLib -> Lines -> operator [](3);
EdtALib2_9 -> Text = MemoLib -> Lines -> operator [](4);
EdtALib2_12 -> Text = MemoLib -> Lines -> operator [](5);
//N_ST=3
EdtALib3_0 -> Text = MemoLib -> Lines -> operator [](6);
EdtALib3_5 -> Text = MemoLib -> Lines -> operator [](7);
EdtALib3_9 -> Text = MemoLib -> Lines -> operator [](8);
EdtALib3_12 -> Text = MemoLib -> Lines -> operator [](9);
//N_ST=4
EdtALib4_15 -> Text = MemoLib -> Lines -> operator [](10);
EdtALib4_1 -> Text = MemoLib -> Lines -> operator [](11);
EdtALib4_2 -> Text = MemoLib -> Lines -> operator [](12);
EdtALib4_4 -> Text = MemoLib -> Lines -> operator [](13);
EdtALib4_6 -> Text = MemoLib -> Lines -> operator [](14);
EdtALib4_7 -> Text = MemoLib -> Lines -> operator [](15);
EdtALib4_8 -> Text = MemoLib -> Lines -> operator [](16);
EdtALib4_13 -> Text = MemoLib -> Lines -> operator [](17);
EdtALib4_9 -> Text = MemoLib -> Lines -> operator [](18);
EdtALib4_14 -> Text = MemoLib -> Lines -> operator [](19);
EdtALib4_12 -> Text = MemoLib -> Lines -> operator [](20);
//N_ST=5
EdtALib5_15 -> Text = MemoLib -> Lines -> operator [](21);
EdtALib5_1 -> Text = MemoLib -> Lines -> operator [](22);
EdtALib5_2 -> Text = MemoLib -> Lines -> operator [](23);
EdtALib5_4 -> Text = MemoLib -> Lines -> operator [](24);
EdtALib5_6 -> Text = MemoLib -> Lines -> operator [](25);
EdtALib5_7 -> Text = MemoLib -> Lines -> operator [](26);
EdtALib5_8 -> Text = MemoLib -> Lines -> operator [](27);
EdtALib5_13 -> Text = MemoLib -> Lines -> operator [](28);
EdtALib5_9 -> Text = MemoLib -> Lines -> operator [](29);
EdtALib5_14 -> Text = MemoLib -> Lines -> operator [](30);
EdtALib5_12 -> Text = MemoLib -> Lines -> operator [](31);
//N_ST=6
EdtALib6_15 -> Text = MemoLib -> Lines -> operator [](32);
EdtALib6_1 -> Text = MemoLib -> Lines -> operator [](33);
EdtALib6_2 -> Text = MemoLib -> Lines -> operator [](34);
EdtALib6_4 -> Text = MemoLib -> Lines -> operator [](35);
EdtALib6_6 -> Text = MemoLib -> Lines -> operator [](36);
EdtALib6_7 -> Text = MemoLib -> Lines -> operator [](37);
EdtALib6_8 -> Text = MemoLib -> Lines -> operator [](38);
EdtALib6_13 -> Text = MemoLib -> Lines -> operator [](39);
EdtALib6_9 -> Text = MemoLib -> Lines -> operator [](40);
EdtALib6_14 -> Text = MemoLib -> Lines -> operator [](41);
EdtALib6_12 -> Text = MemoLib -> Lines -> operator [](42);
//N_ST=7
EdtALib7_15 -> Text = MemoLib -> Lines -> operator [](43);
EdtALib7_1 -> Text = MemoLib -> Lines -> operator [](44);
EdtALib7_2 -> Text = MemoLib -> Lines -> operator [](45);
EdtALib7_4 -> Text = MemoLib -> Lines -> operator [](46);
EdtALib7_6 -> Text = MemoLib -> Lines -> operator [](47);
EdtALib7_7 -> Text = MemoLib -> Lines -> operator [](48);
EdtALib7_8 -> Text = MemoLib -> Lines -> operator [](49);
EdtALib7_13 -> Text = MemoLib -> Lines -> operator [](50);
EdtALib7_9 -> Text = MemoLib -> Lines -> operator [](51);
EdtALib7_14 -> Text = MemoLib -> Lines -> operator [](52);
EdtALib7_12 -> Text = MemoLib -> Lines -> operator [](53);
//N_ST=8
EdtALib8_15 -> Text = MemoLib -> Lines -> operator [](54);
EdtALib8_1 -> Text = MemoLib -> Lines -> operator [](55);
EdtALib8_2 -> Text = MemoLib -> Lines -> operator [](56);
EdtALib8_4 -> Text = MemoLib -> Lines -> operator [](57);
EdtALib8_6 -> Text = MemoLib -> Lines -> operator [](58);
EdtALib8_7 -> Text = MemoLib -> Lines -> operator [](59);
EdtALib8_8 -> Text = MemoLib -> Lines -> operator [](60);
EdtALib8_13 -> Text = MemoLib -> Lines -> operator [](61);
EdtALib8_9 -> Text = MemoLib -> Lines -> operator [](62);
EdtALib8_14 -> Text = MemoLib -> Lines -> operator [](63);
EdtALib8_12 -> Text = MemoLib -> Lines -> operator [](64);
//N_ST=9
EdtALib9_15 -> Text = MemoLib -> Lines -> operator [](65);
EdtALib9_1 -> Text = MemoLib -> Lines -> operator [](66);
EdtALib9_2 -> Text = MemoLib -> Lines -> operator [](67);
EdtALib9_4 -> Text = MemoLib -> Lines -> operator [](68);
EdtALib9_6 -> Text = MemoLib -> Lines -> operator [](69);
EdtALib9_7 -> Text = MemoLib -> Lines -> operator [](70);
EdtALib9_8 -> Text = MemoLib -> Lines -> operator [](71);
EdtALib9_13 -> Text = MemoLib -> Lines -> operator [](72);
EdtALib9_9 -> Text = MemoLib -> Lines -> operator [](73);
EdtALib9_14 -> Text = MemoLib -> Lines -> operator [](74);
EdtALib9_12 -> Text = MemoLib -> Lines -> operator [](75);






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


// очистить массив библиотечных параметров
EdtALib1_15 -> Text = "";
EdtALib1_12 -> Text = "";

EdtALib2_4 -> Text = "";
EdtALib2_8 -> Text = "";
EdtALib2_9 -> Text = "";
EdtALib2_12 -> Text = "";

EdtALib3_0 -> Text = "";
EdtALib3_5 -> Text = "";
EdtALib3_9 -> Text = "";
EdtALib3_12 -> Text = "";

EdtALib4_15 -> Text = "";
EdtALib4_1 -> Text = "";
EdtALib4_2 -> Text = "";
EdtALib4_4 -> Text = "";
EdtALib4_6 -> Text = "";
EdtALib4_7 -> Text = "";
EdtALib4_8 -> Text = "";
EdtALib4_13 -> Text = "";
EdtALib4_9 -> Text = "";
EdtALib4_14 -> Text = "";
EdtALib4_12 -> Text = "";

EdtALib5_15 -> Text = "";
EdtALib5_1 -> Text = "";
EdtALib5_2 -> Text = "";
EdtALib5_4 -> Text = "";
EdtALib5_6 -> Text = "";
EdtALib5_7 -> Text = "";
EdtALib5_8 -> Text = "";
EdtALib5_13 -> Text = "";
EdtALib5_9 -> Text = "";
EdtALib5_14 -> Text = "";
EdtALib5_12 -> Text = "";

EdtALib6_15 -> Text = "";
EdtALib6_1 -> Text = "";
EdtALib6_2 -> Text = "";
EdtALib6_4 -> Text = "";
EdtALib6_6 -> Text = "";
EdtALib6_7 -> Text = "";
EdtALib6_8 -> Text = "";
EdtALib6_13 -> Text = "";
EdtALib6_9 -> Text = "";
EdtALib6_14 -> Text = "";
EdtALib6_12 -> Text = "";

EdtALib7_15 -> Text = "";
EdtALib7_1 -> Text = "";
EdtALib7_2 -> Text = "";
EdtALib7_4 -> Text = "";
EdtALib7_6 -> Text = "";
EdtALib7_7 -> Text = "";
EdtALib7_8 -> Text = "";
EdtALib7_13 -> Text = "";
EdtALib7_9 -> Text = "";
EdtALib7_14 -> Text = "";
EdtALib7_12 -> Text = "";

EdtALib8_15 -> Text = "";
EdtALib8_1 -> Text = "";
EdtALib8_2 -> Text = "";
EdtALib8_4 -> Text = "";
EdtALib8_6 -> Text = "";
EdtALib8_7 -> Text = "";
EdtALib8_8 -> Text = "";
EdtALib8_13 -> Text = "";
EdtALib8_9 -> Text = "";
EdtALib8_14 -> Text = "";
EdtALib8_12 -> Text = "";

EdtALib9_15 -> Text = "";
EdtALib9_1 -> Text = "";
EdtALib9_2 -> Text = "";
EdtALib9_4 -> Text = "";
EdtALib9_6 -> Text = "";
EdtALib9_7 -> Text = "";
EdtALib9_8 -> Text = "";
EdtALib9_13 -> Text = "";
EdtALib9_9 -> Text = "";
EdtALib9_14 -> Text = "";
EdtALib9_12 -> Text = "";



}

}
//---------------------------------------------------------------------------
//--Запись в библиотеку--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnSaveClick(TObject *Sender)
{

MemoLib -> Lines -> Clear();
//N_ST=1
    MemoLib -> Lines -> Add ( EdtARed1_15 -> Text );
    
    MemoLib -> Lines -> Add ( EdtARed1_12 -> Text );
//N_ST=2
    MemoLib -> Lines -> Add ( EdtARed2_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed2_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed2_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed2_12 -> Text );
//N_ST=3
    MemoLib -> Lines -> Add ( EdtARed3_0 -> Text );
    MemoLib -> Lines -> Add ( EdtARed3_5 -> Text );
    MemoLib -> Lines -> Add ( EdtARed3_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed3_12 -> Text );
//N_ST=4
    MemoLib -> Lines -> Add ( EdtARed4_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_2 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_13 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_14 -> Text );
    MemoLib -> Lines -> Add ( EdtARed4_12 -> Text );
//N_ST=5
    MemoLib -> Lines -> Add ( EdtARed5_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_2 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_13 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_14 -> Text );
    MemoLib -> Lines -> Add ( EdtARed5_12 -> Text );
//N_ST=6
    MemoLib -> Lines -> Add ( EdtARed6_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_2 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_13 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_14 -> Text );
    MemoLib -> Lines -> Add ( EdtARed6_12 -> Text );
//N_ST=7
    MemoLib -> Lines -> Add ( EdtARed7_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_2 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_13 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_14 -> Text );
    MemoLib -> Lines -> Add ( EdtARed7_12 -> Text );
//N_ST=8
    MemoLib -> Lines -> Add ( EdtARed8_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_2 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_13 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_14 -> Text );
    MemoLib -> Lines -> Add ( EdtARed8_12 -> Text );
//N_ST=9
    MemoLib -> Lines -> Add ( EdtARed9_15 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_1 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_2 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_4 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_6 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_7 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_8 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_13 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_9 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_14 -> Text );
    MemoLib -> Lines -> Add ( EdtARed9_12 -> Text );

// отображение диалогового окна
GBSaveDialog -> Visible = true;

}
//---------------------------------------------------------------------------
//--Чтение из библиотеки--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BitBtnLoadClick(TObject *Sender)
{
//N_ST=1
EdtARed1_15 -> Text = EdtALib1_15 -> Text;
EdtARed1_15 -> Color = clSilver;
EdtARed1Exit(EdtARed1_15);
EdtARed1_12 -> Text = EdtALib1_12 -> Text;
EdtARed1_12 -> Color = clSilver;
//N_ST=2
EdtARed2_4 -> Text = EdtALib2_4 -> Text;
EdtARed2_4 -> Color = clSilver;
EdtARed2_8 -> Text = EdtALib2_8 -> Text;
EdtARed2_8 -> Color = clSilver;
EdtARed2_9 -> Text = EdtALib2_9 -> Text;
EdtARed2_9 -> Color = clSilver;
EdtARed2_12 -> Text = EdtALib2_12 -> Text;
EdtARed2_12 -> Color = clSilver;
//N_ST=3
EdtARed3_0 -> Text = EdtALib3_0 -> Text;
EdtARed3_0 -> Color = clSilver;
if(nasmod[7])EdtARed3_5 -> Text = EdtALib3_5 -> Text;
EdtARed3_5 -> Color = clSilver;
EdtARed3_9 -> Text = EdtALib3_9 -> Text;
EdtARed3_9 -> Color = clSilver;
EdtARed3_12 -> Text = EdtALib3_12 -> Text;
EdtARed3_12 -> Color = clSilver;
//N_ST=4
EdtARed4_15 -> Text = EdtALib4_15 -> Text;
EdtARed4_15 -> Color = clSilver;
EdtARed1Exit(EdtARed4_15);
EdtARed4_1 -> Text = EdtALib4_1 -> Text;
EdtARed4_1 -> Color = clSilver;
EdtARed4_2 -> Text = EdtALib4_2 -> Text;
EdtARed4_2 -> Color = clSilver;
EdtARed4_4 -> Text = EdtALib4_4 -> Text;
EdtARed4_4 -> Color = clSilver;
if(nasmod[8])EdtARed4_6 -> Text = EdtALib4_6 -> Text;
EdtARed4_6 -> Color = clSilver;
if(nasmod[9])EdtARed4_7 -> Text = EdtALib4_7 -> Text;
EdtARed4_7 -> Color = clSilver;
EdtARed4_8 -> Text = EdtALib4_8 -> Text;
EdtARed4_8 -> Color = clSilver;
EdtARed4_13 -> Text = EdtALib4_13 -> Text;
EdtARed4_13 -> Color = clSilver;
EdtARed4_9 -> Text = EdtALib4_9 -> Text;
EdtARed4_9 -> Color = clSilver;
EdtARed4_14 -> Text = EdtALib4_14 -> Text;
EdtARed4_14 -> Color = clSilver;
EdtARed4_12 -> Text = EdtALib4_12 -> Text;
EdtARed4_12 -> Color = clSilver;
//N_ST=5
EdtARed5_15 -> Text = EdtALib5_15 -> Text;
EdtARed5_15 -> Color = clSilver;
EdtARed1Exit(EdtARed5_15);
EdtARed5_1 -> Text = EdtALib5_1 -> Text;
EdtARed5_1 -> Color = clSilver;
EdtARed5_2 -> Text = EdtALib5_2 -> Text;
EdtARed5_2 -> Color = clSilver;
EdtARed5_4 -> Text = EdtALib5_4 -> Text;
EdtARed5_4 -> Color = clSilver;
if(nasmod[8])EdtARed5_6 -> Text = EdtALib5_6 -> Text;
EdtARed5_6 -> Color = clSilver;
if(nasmod[9])EdtARed5_7 -> Text = EdtALib5_7 -> Text;
EdtARed5_7 -> Color = clSilver;
EdtARed5_8 -> Text = EdtALib5_8 -> Text;
EdtARed5_8 -> Color = clSilver;
EdtARed5_13 -> Text = EdtALib5_13 -> Text;
EdtARed5_13 -> Color = clSilver;
EdtARed5_9 -> Text = EdtALib5_9 -> Text;
EdtARed5_9 -> Color = clSilver;
EdtARed5_14 -> Text = EdtALib5_14 -> Text;
EdtARed5_14 -> Color = clSilver;
EdtARed5_12 -> Text = EdtALib5_12 -> Text;
EdtARed5_12 -> Color = clSilver;
//N_ST=6
EdtARed6_15 -> Text = EdtALib6_15 -> Text;
EdtARed6_15 -> Color = clSilver;
EdtARed1Exit(EdtARed6_15);
EdtARed6_1 -> Text = EdtALib6_1 -> Text;
EdtARed6_1 -> Color = clSilver;
EdtARed6_2 -> Text = EdtALib6_2 -> Text;
EdtARed6_2 -> Color = clSilver;
EdtARed6_4 -> Text = EdtALib6_4 -> Text;
EdtARed6_4 -> Color = clSilver;
if(nasmod[8])EdtARed6_6 -> Text = EdtALib6_6 -> Text;
EdtARed6_6 -> Color = clSilver;
if(nasmod[9])EdtARed6_7 -> Text = EdtALib6_7 -> Text;
EdtARed6_7 -> Color = clSilver;
EdtARed6_8 -> Text = EdtALib6_8 -> Text;
EdtARed6_8 -> Color = clSilver;
EdtARed6_13 -> Text = EdtALib6_13 -> Text;
EdtARed6_13 -> Color = clSilver;
EdtARed6_9 -> Text = EdtALib6_9 -> Text;
EdtARed6_9 -> Color = clSilver;
EdtARed6_14 -> Text = EdtALib6_14 -> Text;
EdtARed6_14 -> Color = clSilver;
EdtARed6_12 -> Text = EdtALib6_12 -> Text;
EdtARed6_12 -> Color = clSilver;
//N_ST=7
EdtARed7_15 -> Text = EdtALib7_15 -> Text;
EdtARed7_15 -> Color = clSilver;
EdtARed1Exit(EdtARed7_15);
EdtARed7_1 -> Text = EdtALib7_1 -> Text;
EdtARed7_1 -> Color = clSilver;
EdtARed7_2 -> Text = EdtALib7_2 -> Text;
EdtARed7_2 -> Color = clSilver;
EdtARed7_4 -> Text = EdtALib7_4 -> Text;
EdtARed7_4 -> Color = clSilver;
if(nasmod[8])EdtARed7_6 -> Text = EdtALib7_6 -> Text;
EdtARed7_6 -> Color = clSilver;
if(nasmod[9])EdtARed7_7 -> Text = EdtALib7_7 -> Text;
EdtARed7_7 -> Color = clSilver;
EdtARed7_8 -> Text = EdtALib7_8 -> Text;
EdtARed7_8 -> Color = clSilver;
EdtARed7_13 -> Text = EdtALib7_13 -> Text;
EdtARed7_13 -> Color = clSilver;
EdtARed7_9 -> Text = EdtALib7_9 -> Text;
EdtARed7_9 -> Color = clSilver;
EdtARed7_14 -> Text = EdtALib7_14 -> Text;
EdtARed7_14 -> Color = clSilver;
EdtARed7_12 -> Text = EdtALib7_12 -> Text;
EdtARed7_12 -> Color = clSilver;
//N_ST=8
EdtARed8_15 -> Text = EdtALib8_15 -> Text;
EdtARed8_15 -> Color = clSilver;
EdtARed1Exit(EdtARed8_15);
EdtARed8_1 -> Text = EdtALib8_1 -> Text;
EdtARed8_1 -> Color = clSilver;
EdtARed8_2 -> Text = EdtALib8_2 -> Text;
EdtARed8_2 -> Color = clSilver;
EdtARed8_4 -> Text = EdtALib8_4 -> Text;
EdtARed8_4 -> Color = clSilver;
if(nasmod[8])EdtARed8_6 -> Text = EdtALib8_6 -> Text;
EdtARed8_6 -> Color = clSilver;
if(nasmod[9])EdtARed8_7 -> Text = EdtALib8_7 -> Text;
EdtARed8_7 -> Color = clSilver;
EdtARed8_8 -> Text = EdtALib8_8 -> Text;
EdtARed8_8 -> Color = clSilver;
EdtARed8_13 -> Text = EdtALib8_13 -> Text;
EdtARed8_13 -> Color = clSilver;
EdtARed8_9 -> Text = EdtALib8_9 -> Text;
EdtARed8_9 -> Color = clSilver;
EdtARed8_14 -> Text = EdtALib8_14 -> Text;
EdtARed8_14 -> Color = clSilver;
EdtARed8_12 -> Text = EdtALib8_12 -> Text;
EdtARed8_12 -> Color = clSilver;
//N_ST=9
EdtARed9_15 -> Text = EdtALib9_15 -> Text;
EdtARed9_15 -> Color = clSilver;
EdtARed1Exit(EdtARed9_15);
EdtARed9_1 -> Text = EdtALib9_1 -> Text;
EdtARed9_1 -> Color = clSilver;
EdtARed9_2 -> Text = EdtALib9_2 -> Text;
EdtARed9_2 -> Color = clSilver;
EdtARed9_4 -> Text = EdtALib9_4 -> Text;
EdtARed9_4 -> Color = clSilver;
if(nasmod[8])EdtARed9_6 -> Text = EdtALib9_6 -> Text;
EdtARed9_6 -> Color = clSilver;
if(nasmod[9])EdtARed9_7 -> Text = EdtALib9_7 -> Text;
EdtARed9_7 -> Color = clSilver;
EdtARed9_8 -> Text = EdtALib9_8 -> Text;
EdtARed9_8 -> Color = clSilver;
EdtARed9_13 -> Text = EdtALib9_13 -> Text;
EdtARed9_13 -> Color = clSilver;
EdtARed9_9 -> Text = EdtALib9_9 -> Text;
EdtARed9_9 -> Color = clSilver;
EdtARed9_14 -> Text = EdtALib9_14 -> Text;
EdtARed9_14 -> Color = clSilver;
EdtARed9_12 -> Text = EdtALib9_12 -> Text;
EdtARed9_12 -> Color = clSilver;




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
    if((nasmod[4]==0)||(nasmod[6]==0))EdtRKon0_15 -> Text =0;
    else if(nasmod[6]==1)   EdtRKon0_15 -> Text =FloatToStrF((float)par[0][15]/ 10.0, ffFixed, 5, 1);
    else                    EdtRKon0_15 -> Text =FloatToStrF((float)par[0][15]/ 81.92, ffFixed, 5, 1); //температура нагрева п/д
    EdtRKon0_0 -> Text =FloatToStrF((float)par[0][0]* RRG1_MAX / 4095.0, ffFixed, 5, 2);//Расход РРГ1
    EdtRKon0_1 -> Text =FloatToStrF((float)par[0][1]* RRG2_MAX / 4095.0, ffFixed, 5, 1);//Расход РРГ2
    EdtRKon0_2 -> Text =FloatToStrF((float)par[0][2]* RRG3_MAX / 4095.0, ffFixed, 5, 1);//Расход РРГ3
    EdtRKon0_3 -> Text =FloatToStrF((float)par[0][3]* RRG4_MAX / 4095.0, ffFixed, 5, 1);//Расход РРГ4
    EdtRKon0_4 -> Text =FloatToStrF(pow(10,(float)par[0][4]/1000.0-3.5), ffFixed, 5, 2);//Давление РРТ200
    EdtRKon0_5 -> Text =FloatToStrF((float)par[0][5]/ 4095.0 * 500.0, ffFixed, 5, 0);//Ток ИИ
    EdtRKon0_6 -> Text =FloatToStrF((float)par[0][6]/ 4095.0 * 3600, ffFixed, 5, 0);//М1
    EdtRKon0_7 -> Text =FloatToStrF((float)par[0][7]/ 4095.0 * 3600, ffFixed, 5, 0);//М2
    EdtRKon0_8 -> Text =FloatToStrF((float)par[0][8]/ 4095.0 * CESAR_MAX_PD, ffFixed, 5, 0);//ВЧГ
    EdtRKon0_9 -> Text =FloatToStrF((float)par[0][9]/10000.0*100.0, ffFixed, 5, 1);//Процент ДЗ
    EdtRKon0_10 -> Text =FloatToStrF((float)par[0][10], ffFixed, 8, 0);  //Подъём подложкодержателя
    if(par[0][11]==0)       { EdtRKon0_11 -> Text ="Большая"; }
    else if(par[0][11]==1)  { EdtRKon0_11 -> Text ="Малая"; }
    else if(par[0][11]==2)  { EdtRKon0_11 -> Text ="Ползущая";}




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

AnsiString valuesNames[] =
    {
    
    	//0 страница
        "sh_",
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
        "",
        //1 страница
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
        //2 страница
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
        "",
        "zshr",
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
        "diagn[29]",
        //4 страница
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
        " ",

        //6 страница
        "aout[0]",
        "aout[1]",
        "aout[2]",
        "aout[3]",
        "aout[4]",
        "aout[5]",
        "aout[6]",
        "aout[7]",
        "aout[8]",
        "",
        "D_D1",
        "D_D2",
        "D_D3",
        "D_D4",
        "D_D5",
        "D_D6",
        "",
        "UVAK_KN",
        "UVAKN_TMN",
        "UVAKV_TMN",
        "UVAK_KAM",
        "UATM_D1",
        "UVAK_SHL",
        "UATM_D4",
        "UVAK_SHL_MO",
        "POROG_DAVL",
        "UVAK_ZTMN",
        "",
        "",
        "",

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
        "nasmod[17]",
        "nasmod[18]",
        "nasmod[19]",
        "nasmod[20]",
        "nasmod[21]",
        "",
        "",
        "par_t[0]",
        "par_t[1]",
        "par_t[2]",
        "par_t[3]",
        "par_t[4]",
        "par_t[5]",

        //8 страница
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

        //10 страница
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

        //11 страница
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

        //12 страница
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

        //13 страница
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

        //14 страница
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
        "par[6][14]",
        "par[6][15]",
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

        //15 страница
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
        "par[7][15]",
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

        //16 страница
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
        "par[8][14]",
        "par[8][15]",
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

        //17 страница
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
        "par[9][15]",
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

        //18 страница
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
        "par[10][14]",
        "par[10][15]",
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

        //19 страница
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
        "",
        "",

        //20 страница
        "par[12][0]",
        "par[12][1]",
        "par[12][2]",
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
        "par[12][15]",
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

        //21 страница
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
        "CT_19",
        "CT_24",
        "CT24K1",
        "CT_27",
        "CT_28",
        "CT28K1",
        "CT_29",
        "CT29K1",
        "CT_31",
        "CT31K1",
        "CT_33",
        "CT33K1",
        "CT_35",
        "CT35K1",
        "CT_36",
        "CT36K1",
        "CT_38",
        "CT_39",
        "CT_40",
        "CT_41",


        //22 страница
        "CT_KN",
        "CT_VRUN",
        "CT_PR_UN",
        "CT_REQUN",
        "CT_II",
        "CT_VODA_BM1",
        "CT_VODA_BM2",
        "CT_VODA_II",
        "CT_KZ1",
        "CT_KZ2",
        "ctPderjDvij",
        "CT_PER",
        "CT_POD",
        "CT_DZASL",
        "CT_TEMP1",
        "CT_TEMP2",
        "CT_VHG",
        "CT_IST",
        "CT_BM1",
        "CT_BM2",
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

        //23 страница
        "T_K_KN",
        "T_KTMN",
        "T_KTMN_RAZGON",
        "T_KKAM_V",
        "T_OTK_KN",
        "T_PROC",
        "T_KNAP",
        "T_NAPUSK",
        "T_SBROSHE",
        "T_DVIJ",
        "T_KSHL_MO",
        "T_KSHL",
        "T_VPRB_UN",
        "T_VREJ_UN",
        "T_VRUN",
        "T_KUN",
        "T_REQUN",
        "",
        "T_VRGIS",
        "T_KGIS",
        "T_VKL_BPN",
        "T_VRTEMP",
        "T_KTEMP",
        "T_VRII",
        "T_KII",
        "T_VRBM",
        "T_KBM",
        "",
        "",
        "T_K_KAM",

        //24 страница
        "PR_TRTEST",
        "PR_RG4",
        "PR_OTP",
        "PR_NALADKA",
        "PR_TREN",
        "PR_PER",
        "PR_POD",
        "N_ST",
        "otvet",
        "N_ST_MAX",
        "DOPUSK_VENT",
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

        //25 страница
        "PR_DZASL",
        "OTVET_DZASL",
        "",
        "",
        "DAVL_DZASL",
        "DATA_DZASL",
        "",
        "",
        "X_TDZASL",
        "VRDZASL",
        "E_TDZASL",
        "DELDZASL",
        "LIM1DZASL",
        "LIM2DZASL",
        "DOPDZASL",
        "",
        "KOM_DZASL",
        "",
        "",
        "",
        "",
        "CT_DZASL",
        "T_KDZASL",
        "T_VRDZASL",
        "",
        "PAR_DZASL",
        "ZPAR_DZASL",
        "",
        "TEK_DAVL_DZASL",
        "TEK_POZ_DZASL",

        //26 страница
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
        
        //27 страница
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

        //28 страница
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
        "",
        "",
        "",
        "KOM_POD",
        "OTVET_POD",
        "V_POD",
        "TYPE_POD",
        "",
        "PR_POD",
        "HOME_POD",
        "",
        "PUT_POD",
        "TEK_ABS_POD",
        "TEK_OTN_POD",

        //29 страница
        "VRPD",
        "",
        "prMVPvRabPol",
        "pderjCounter",
        "pderjInIsh",
        "",
        "PDVmin",
        "PDVmax",
        "",
        "tkPderjIsh",
        "tkPderjDvij",
        "tkPderjRazgon",
        "ctPderjDvij",
        "ctPderjCheck_0",
        "ctPderjCheck_1",
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

        //30 страница
        "VRUN",
        "PAR_UN",
        "X_TUN",
        "E_TUN",
        "DELUN",
        "E_PUN",
        "K_PUN",
        "K_IUN",
        "U_PUN",
        "A_VIH",
        "",
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
        "T_VRUN",
        "T_KUN",
        "T_VREJ_UN",
        "T_VPRB_UN",
        "T_REQUN",
        "CT_VRUN",
        "CT_PR_UN",
        "CT_REQUN",

        //31 страница
        "VRBM1",
        "PR_SV_BM1",
        "PR_NAP1",
        "UST_BM1",
        "X_TBM1",
        "E_TBM1",
        "DELBM1",
        "DOPBM1",
        "PAR_BM1",
        "LIM1BM1",
        "LIM2BM1",
        "T_VRBM",
        "T_KBM",
        "VRBM2",
        "PR_SV_BM2",
        "PR_NAP2",
        "UST_BM2",
        "X_TBM2",
        "E_TBM2",
        "DELBM2",
        "DOPBM2",
        "PAR_BM2",
        "LIM1BM2",
        "LIM2BM2",
        "T_KOTS_PROB",
        "PR_KZ1",
        "N_KZ1",
        "PR_KZ2",
        "N_KZ2",
        "N_PROB",

        //32 страница
        "VRII",
        "",
        "PR_SV_II",
        "X_TII",
        "E_TII",
        "DELII",
        "DOPII",
        "PAR_II",
        "LIM1II",
        "LIM2II",
        "T_VRII",
        "CT_II",
        "TK_OJ_OTV",
        "T_KII",
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

        //33 страница
        "OTVET_BM1[0]",
        "OTVET_BM1[1]",
        "OTVET_BM1[2]",
        "OTVET_BM1[3]",
        "OTVET_BM1[4]",
        "OTVET_BM1[5]",
        "OTVET_BM1[6]",
        "OTVET_BM1[7]",
        "OTVET_BM1[8]",
        "OTVET_BM1[9]",
        "OTVET_BM2[0]",
        "OTVET_BM2[1]",
        "OTVET_BM2[2]",
        "OTVET_BM2[3]",
        "OTVET_BM2[4]",
        "OTVET_BM2[5]",
        "OTVET_BM2[6]",
        "OTVET_BM2[7]",
        "OTVET_BM2[8]",
        "OTVET_BM2[9]",
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

        //34 страница
        "KOM_BM1[0]",
        "KOM_BM1[1]",
        "KOM_BM1[2]",
        "KOM_BM1[3]",
        "KOM_BM1[4]",
        "",
        "KOM_BM2[0]",
        "KOM_BM2[1]",
        "KOM_BM2[2]",
        "KOM_BM2[3]",
        "KOM_BM2[4]",
        "",
        "KOM_II[0]",
        "KOM_II[1]",
        "KOM_II[2]",
        "KOM_II[3]",
        "KOM_II[4]",
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

        //35 страница
        "PR_KN",
        "PR_PER0_KN",
        "PR_SV_KN",
        "",
        "",
        "KOM_KN",
        "OTVET_KN",
        "",
        "CT_KN",
        "",
        "OTVET_KN_M[0]",
        "OTVET_KN_M[1]",
        "OTVET_KN_M[2]",
        "OTVET_KN_M[3]",
        "OTVET_KN_M[4]",
        "OTVET_KN_M[5]",
        "OTVET_KN_M[6]",
        "OTVET_KN_M[7]",
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
		EditOTLtek2->Text = IntToStr(sh[1]);
		EditOTLtek3->Text = IntToStr(shr[1]);
		EditOTLtek4->Text = IntToStr(sh[2]);
		EditOTLtek5->Text = IntToStr(shr[2]);
		EditOTLtek6->Text = IntToStr(sh[3]);
		EditOTLtek7->Text = IntToStr(shr[3]);
		EditOTLtek8->Text = IntToStr(sh[4]);
		EditOTLtek9->Text = IntToStr(shr[4]);
		EditOTLtek10->Text = IntToStr(sh[5]);
		EditOTLtek11->Text = IntToStr(shr[5]);
		EditOTLtek12->Text = IntToStr(sh[6]);
		EditOTLtek13->Text = IntToStr(shr[6]);
		EditOTLtek14->Text = IntToStr(sh[7]);
		EditOTLtek15->Text = IntToStr(shr[7]);
		EditOTLtek16->Text = IntToStr(sh[8]);
		EditOTLtek17->Text = IntToStr(shr[8]);
		EditOTLtek18->Text = IntToStr(sh[9]);
		EditOTLtek19->Text = IntToStr(shr[9]);
		EditOTLtek20->Text = IntToStr(sh[10]);
		EditOTLtek21->Text = IntToStr(shr[10]);
		EditOTLtek22->Text = IntToStr(sh[11]);
		EditOTLtek23->Text = IntToStr(shr[11]);
		EditOTLtek24->Text = IntToStr(sh[12]);
		EditOTLtek25->Text = IntToStr(shr[12]);
		EditOTLtek26->Text = IntToStr(sh[13]);
		EditOTLtek27->Text = IntToStr(shr[13]);
		EditOTLtek28->Text = IntToStr(sh[14]);
		EditOTLtek29->Text = IntToStr(shr[14]);
		EditOTLtek30->Text = IntToStr(0);
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
		EditOTLtek30->Text = IntToStr(diagn[29]);
}; break;
case 4:
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
	   //	EditOTLtek30->Text = IntToStr(aik[16]);
}; break;
case 6:
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
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(D_D1);
		EditOTLtek12->Text = IntToStr(D_D2);
		EditOTLtek13->Text = IntToStr(D_D3);
		EditOTLtek14->Text = IntToStr(D_D4);
		EditOTLtek15->Text = IntToStr(D_D5);
		EditOTLtek16->Text = IntToStr(D_D6);
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(UVAK_KN);
		EditOTLtek19->Text = IntToStr(UVAKN_TMN);
		EditOTLtek20->Text = IntToStr(UVAKV_TMN);
		EditOTLtek21->Text = IntToStr(UVAK_KAM);
		EditOTLtek22->Text = IntToStr(UATM_D1);
		EditOTLtek23->Text = IntToStr(UVAK_SHL);
		EditOTLtek24->Text = IntToStr(UATM_D4);
		EditOTLtek25->Text = IntToStr(UVAK_SHL_MO);
		EditOTLtek26->Text = IntToStr(POROG_DAVL);
		EditOTLtek27->Text = IntToStr(UVAK_ZTMN);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(0);
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
		EditOTLtek18->Text = IntToStr(nasmod[17]);
		EditOTLtek19->Text = IntToStr(nasmod[18]);
		EditOTLtek20->Text = IntToStr(nasmod[19]);
		EditOTLtek21->Text = IntToStr(nasmod[20]);
		EditOTLtek22->Text = IntToStr(nasmod[21]);
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
case 10:
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
case 11:
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
case 12:
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
case 13:
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
case 14:
{
		EditOTLtek1->Text = IntToStr(par[6][0]);
		EditOTLtek2->Text = IntToStr(par[6][1]);
		EditOTLtek3->Text = IntToStr(par[6][2]);
		EditOTLtek4->Text = IntToStr(par[6][3]);
		EditOTLtek5->Text = IntToStr(par[6][4]);
		EditOTLtek6->Text = IntToStr(par[6][5]);
		EditOTLtek7->Text = IntToStr(par[6][6]);
		EditOTLtek8->Text = IntToStr(par[6][7]);
		EditOTLtek9->Text = IntToStr(par[6][8]);
		EditOTLtek10->Text = IntToStr(par[6][9]);
		EditOTLtek11->Text = IntToStr(par[6][10]);
		EditOTLtek12->Text = IntToStr(par[6][11]);
		EditOTLtek13->Text = IntToStr(par[6][12]);
		EditOTLtek14->Text = IntToStr(par[6][13]);
		EditOTLtek15->Text = IntToStr(par[6][14]);
		EditOTLtek16->Text = IntToStr(par[6][15]);
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
case 15:
{
		EditOTLtek1->Text = IntToStr(par[7][0]);
		EditOTLtek2->Text = IntToStr(par[7][1]);
		EditOTLtek3->Text = IntToStr(par[7][2]);
		EditOTLtek4->Text = IntToStr(par[7][3]);
		EditOTLtek5->Text = IntToStr(par[7][4]);
		EditOTLtek6->Text = IntToStr(par[7][5]);
		EditOTLtek7->Text = IntToStr(par[7][6]);
		EditOTLtek8->Text = IntToStr(par[7][7]);
		EditOTLtek9->Text = IntToStr(par[7][8]);
		EditOTLtek10->Text = IntToStr(par[7][9]);
		EditOTLtek11->Text = IntToStr(par[7][10]);
		EditOTLtek12->Text = IntToStr(par[7][11]);
		EditOTLtek13->Text = IntToStr(par[7][12]);
		EditOTLtek14->Text = IntToStr(par[7][13]);
		EditOTLtek15->Text = IntToStr(par[7][14]);
		EditOTLtek16->Text = IntToStr(par[7][15]);
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
case 16:
{
		EditOTLtek1->Text = IntToStr(par[8][0]);
		EditOTLtek2->Text = IntToStr(par[8][1]);
		EditOTLtek3->Text = IntToStr(par[8][2]);
		EditOTLtek4->Text = IntToStr(par[8][3]);
		EditOTLtek5->Text = IntToStr(par[8][4]);
		EditOTLtek6->Text = IntToStr(par[8][5]);
		EditOTLtek7->Text = IntToStr(par[8][6]);
		EditOTLtek8->Text = IntToStr(par[8][7]);
		EditOTLtek9->Text = IntToStr(par[8][8]);
		EditOTLtek10->Text = IntToStr(par[8][9]);
		EditOTLtek11->Text = IntToStr(par[8][10]);
		EditOTLtek12->Text = IntToStr(par[8][11]);
		EditOTLtek13->Text = IntToStr(par[8][12]);
		EditOTLtek14->Text = IntToStr(par[8][13]);
		EditOTLtek15->Text = IntToStr(par[8][14]);
		EditOTLtek16->Text = IntToStr(par[8][15]);
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
		EditOTLtek1->Text = IntToStr(par[9][0]);
		EditOTLtek2->Text = IntToStr(par[9][1]);
		EditOTLtek3->Text = IntToStr(par[9][2]);
		EditOTLtek4->Text = IntToStr(par[9][3]);
		EditOTLtek5->Text = IntToStr(par[9][4]);
		EditOTLtek6->Text = IntToStr(par[9][5]);
		EditOTLtek7->Text = IntToStr(par[9][6]);
		EditOTLtek8->Text = IntToStr(par[9][7]);
		EditOTLtek9->Text = IntToStr(par[9][8]);
		EditOTLtek10->Text = IntToStr(par[9][9]);
		EditOTLtek11->Text = IntToStr(par[9][10]);
		EditOTLtek12->Text = IntToStr(par[9][11]);
		EditOTLtek13->Text = IntToStr(par[9][12]);
		EditOTLtek14->Text = IntToStr(par[9][13]);
		EditOTLtek15->Text = IntToStr(par[9][14]);
		EditOTLtek16->Text = IntToStr(par[9][15]);
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
case 18:
{
		EditOTLtek1->Text = IntToStr(par[10][0]);
		EditOTLtek2->Text = IntToStr(par[10][1]);
		EditOTLtek3->Text = IntToStr(par[10][2]);
		EditOTLtek4->Text = IntToStr(par[10][3]);
		EditOTLtek5->Text = IntToStr(par[10][4]);
		EditOTLtek6->Text = IntToStr(par[10][5]);
		EditOTLtek7->Text = IntToStr(par[10][6]);
		EditOTLtek8->Text = IntToStr(par[10][7]);
		EditOTLtek9->Text = IntToStr(par[10][8]);
		EditOTLtek10->Text = IntToStr(par[10][9]);
		EditOTLtek11->Text = IntToStr(par[10][10]);
		EditOTLtek12->Text = IntToStr(par[10][11]);
		EditOTLtek13->Text = IntToStr(par[10][12]);
		EditOTLtek14->Text = IntToStr(par[10][13]);
		EditOTLtek15->Text = IntToStr(par[10][14]);
		EditOTLtek16->Text = IntToStr(par[10][15]);
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
		EditOTLtek1->Text = IntToStr(par[11][0]);
		EditOTLtek2->Text = IntToStr(par[11][1]);
		EditOTLtek3->Text = IntToStr(par[11][2]);
		EditOTLtek4->Text = IntToStr(par[11][3]);
		EditOTLtek5->Text = IntToStr(par[11][4]);
		EditOTLtek6->Text = IntToStr(par[11][5]);
		EditOTLtek7->Text = IntToStr(par[11][6]);
		EditOTLtek8->Text = IntToStr(par[11][7]);
		EditOTLtek9->Text = IntToStr(par[11][8]);
		EditOTLtek10->Text = IntToStr(par[11][9]);
		EditOTLtek11->Text = IntToStr(par[11][10]);
		EditOTLtek12->Text = IntToStr(par[11][11]);
		EditOTLtek13->Text = IntToStr(par[11][12]);
		EditOTLtek14->Text = IntToStr(par[11][13]);
		EditOTLtek15->Text = IntToStr(par[11][14]);
		EditOTLtek16->Text = IntToStr(par[11][15]);
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
case 20:
{
		EditOTLtek1->Text = IntToStr(par[12][0]);
		EditOTLtek2->Text = IntToStr(par[12][1]);
		EditOTLtek3->Text = IntToStr(par[12][2]);
		EditOTLtek4->Text = IntToStr(par[12][3]);
		EditOTLtek5->Text = IntToStr(par[12][4]);
		EditOTLtek6->Text = IntToStr(par[12][5]);
		EditOTLtek7->Text = IntToStr(par[12][6]);
		EditOTLtek8->Text = IntToStr(par[12][7]);
		EditOTLtek9->Text = IntToStr(par[12][8]);
		EditOTLtek10->Text = IntToStr(par[12][9]);
		EditOTLtek11->Text = IntToStr(par[12][10]);
		EditOTLtek12->Text = IntToStr(par[12][11]);
		EditOTLtek13->Text = IntToStr(par[12][12]);
		EditOTLtek14->Text = IntToStr(par[12][13]);
		EditOTLtek15->Text = IntToStr(par[12][14]);
		EditOTLtek16->Text = IntToStr(par[12][15]);
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
case 21:
{
		EditOTLtek1->Text = IntToStr(CT_T1);
		EditOTLtek2->Text = IntToStr(CT_T20);
		EditOTLtek3->Text = IntToStr(CT_1);
		EditOTLtek4->Text = IntToStr(CT_2);
		EditOTLtek5->Text = IntToStr(CT_3);
		EditOTLtek6->Text = IntToStr(CT_4);
		EditOTLtek7->Text = IntToStr(CT_5);
		EditOTLtek8->Text = IntToStr(CT_6);
		EditOTLtek9->Text = IntToStr(CT_7);
		EditOTLtek10->Text = IntToStr(CT_9);
		EditOTLtek11->Text = IntToStr(CT_19);
		EditOTLtek12->Text = IntToStr(CT_24);
		EditOTLtek13->Text = IntToStr(CT24K1);
		EditOTLtek14->Text = IntToStr(CT_27);
		EditOTLtek15->Text = IntToStr(CT_28);
		EditOTLtek16->Text = IntToStr(CT28K1);
		EditOTLtek17->Text = IntToStr(CT_29);
		EditOTLtek18->Text = IntToStr(CT29K1);
		EditOTLtek19->Text = IntToStr(CT_31);
		EditOTLtek20->Text = IntToStr(CT31K1);
		EditOTLtek21->Text = IntToStr(CT_33);
		EditOTLtek22->Text = IntToStr(CT33K1);
		EditOTLtek23->Text = IntToStr(CT_35);
		EditOTLtek24->Text = IntToStr(CT35K1);
		EditOTLtek25->Text = IntToStr(CT_36);
		EditOTLtek26->Text = IntToStr(CT36K1);
		EditOTLtek27->Text = IntToStr(CT_38);
		EditOTLtek28->Text = IntToStr(CT_39);
		EditOTLtek29->Text = IntToStr(CT_40);
		EditOTLtek30->Text = IntToStr(CT_41);
}; break;
case 22:
{
		EditOTLtek1->Text = IntToStr(CT_KN);
		EditOTLtek2->Text = IntToStr(CT_VRUN);
		EditOTLtek3->Text = IntToStr(CT_PR_UN);
		EditOTLtek4->Text = IntToStr(CT_REQUN);
		EditOTLtek5->Text = IntToStr(CT_II);
		EditOTLtek6->Text = IntToStr(CT_VODA_BM1);
		EditOTLtek7->Text = IntToStr(CT_VODA_BM2);
		EditOTLtek8->Text = IntToStr(CT_VODA_II);
		EditOTLtek9->Text = IntToStr(CT_KZ1);
		EditOTLtek10->Text = IntToStr(CT_KZ2);
		EditOTLtek11->Text = IntToStr(ctPderjDvij);
		EditOTLtek12->Text = IntToStr(CT_PER);
		EditOTLtek13->Text = IntToStr(CT_POD);
		EditOTLtek14->Text = IntToStr(CT_DZASL);
		EditOTLtek15->Text = IntToStr(CT_TEMP1);
		EditOTLtek16->Text = IntToStr(CT_TEMP2);
		EditOTLtek17->Text = IntToStr(CT_VHG);
		EditOTLtek18->Text = IntToStr(CT_IST);
		EditOTLtek19->Text = IntToStr(CT_BM1);
		EditOTLtek20->Text = IntToStr(CT_BM2);
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
case 23:
{
		EditOTLtek1->Text = IntToStr(T_K_KN);
		EditOTLtek2->Text = IntToStr(T_KTMN);
		EditOTLtek3->Text = IntToStr(T_KTMN_RAZGON);
		EditOTLtek4->Text = IntToStr(T_KKAM_V);
		EditOTLtek5->Text = IntToStr(T_OTK_KN);
		EditOTLtek6->Text = IntToStr(T_PROC);
		EditOTLtek7->Text = IntToStr(T_KNAP);
		EditOTLtek8->Text = IntToStr(T_NAPUSK);
		EditOTLtek9->Text = IntToStr(T_SBROSHE);
		EditOTLtek10->Text = IntToStr(T_DVIJ);
		EditOTLtek11->Text = IntToStr(T_KSHL_MO);
		EditOTLtek12->Text = IntToStr(T_KSHL);
		EditOTLtek13->Text = IntToStr(T_VPRB_UN);
		EditOTLtek14->Text = IntToStr(T_VREJ_UN);
		EditOTLtek15->Text = IntToStr(T_VRUN);
		EditOTLtek16->Text = IntToStr(T_KUN);
		EditOTLtek17->Text = IntToStr(T_REQUN);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(T_VRGIS);
		EditOTLtek20->Text = IntToStr(T_KGIS);
		EditOTLtek21->Text = IntToStr(T_VKL_BPN);
		EditOTLtek22->Text = IntToStr(T_VRTEMP);
		EditOTLtek23->Text = IntToStr(T_KTEMP);
		EditOTLtek24->Text = IntToStr(T_VRII);
		EditOTLtek25->Text = IntToStr(T_KII);
		EditOTLtek26->Text = IntToStr(T_VRBM);
		EditOTLtek27->Text = IntToStr(T_KBM);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(0);
		EditOTLtek30->Text = IntToStr(T_K_KAM);
}; break;
case 24:
{
		EditOTLtek1->Text = IntToStr(PR_TRTEST);
		EditOTLtek2->Text = IntToStr(PR_RG4);
		EditOTLtek3->Text = IntToStr(PR_OTP);
		EditOTLtek4->Text = IntToStr(PR_NALADKA);
		EditOTLtek5->Text = IntToStr(PR_TREN);
		EditOTLtek6->Text = IntToStr(PR_PER);
		EditOTLtek7->Text = IntToStr(PR_POD);
		EditOTLtek8->Text = IntToStr(N_ST);
		EditOTLtek9->Text = IntToStr(otvet);
		EditOTLtek10->Text = IntToStr(N_ST_MAX);
		EditOTLtek11->Text = IntToStr(DOPUSK_VENT);
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
case 25:
{
		EditOTLtek1->Text = IntToStr(PR_DZASL);
		EditOTLtek2->Text = IntToStr(OTVET_DZASL);
		EditOTLtek3->Text = IntToStr(0);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(DAVL_DZASL);
		EditOTLtek6->Text = IntToStr(DATA_DZASL);
		EditOTLtek7->Text = IntToStr(0);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(X_TDZASL);
		EditOTLtek10->Text = IntToStr(VRDZASL);
		EditOTLtek11->Text = IntToStr(E_TDZASL);
		EditOTLtek12->Text = IntToStr(DELDZASL);
		EditOTLtek13->Text = IntToStr(LIM1DZASL);
		EditOTLtek14->Text = IntToStr(LIM2DZASL);
		EditOTLtek15->Text = IntToStr(DOPDZASL);
		EditOTLtek16->Text = IntToStr(0);
		EditOTLtek17->Text = IntToStr(KOM_DZASL);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(0);
		EditOTLtek21->Text = IntToStr(0);
		EditOTLtek22->Text = IntToStr(CT_DZASL);
		EditOTLtek23->Text = IntToStr(T_KDZASL);
		EditOTLtek24->Text = IntToStr(T_VRDZASL);
		EditOTLtek25->Text = IntToStr(0);
		EditOTLtek26->Text = IntToStr(PAR_DZASL);
		EditOTLtek27->Text = IntToStr(ZPAR_DZASL);
		EditOTLtek28->Text = IntToStr(0);
		EditOTLtek29->Text = IntToStr(TEK_DAVL_DZASL);
		EditOTLtek30->Text = IntToStr(TEK_POZ_DZASL);
}; break;
case 26:
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
case 27:
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


case 28:
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
		EditOTLtek17->Text = IntToStr(0);
		EditOTLtek18->Text = IntToStr(0);
		EditOTLtek19->Text = IntToStr(0);
		EditOTLtek20->Text = IntToStr(KOM_POD);
		EditOTLtek21->Text = IntToStr(OTVET_POD);
		EditOTLtek22->Text = IntToStr(V_POD);
		EditOTLtek23->Text = IntToStr(TYPE_POD);
		EditOTLtek24->Text = IntToStr(0);
		EditOTLtek25->Text = IntToStr(PR_POD);
		EditOTLtek26->Text = IntToStr(HOME_POD);
		EditOTLtek27->Text = IntToStr(0);
		EditOTLtek28->Text = IntToStr(PUT_POD);
		EditOTLtek29->Text = IntToStr(TEK_ABS_POD);
		EditOTLtek30->Text = IntToStr(TEK_OTN_POD);
}; break;
case 29:
{
		EditOTLtek1->Text = IntToStr(VRPD);
		EditOTLtek2->Text = IntToStr(0);
		EditOTLtek3->Text = IntToStr(prMVPvRabPol);
		EditOTLtek4->Text = IntToStr(pderjCounter);
		EditOTLtek5->Text = IntToStr(pderjInIsh);
		EditOTLtek6->Text = IntToStr(0);
		EditOTLtek7->Text = IntToStr(PDVmin);
		EditOTLtek8->Text = IntToStr(PDVmax);
		EditOTLtek9->Text = IntToStr(0);
		EditOTLtek10->Text = IntToStr(tkPderjIsh);
		EditOTLtek11->Text = IntToStr(tkPderjDvij);
		EditOTLtek12->Text = IntToStr(tkPderjRazgon);
		EditOTLtek13->Text = IntToStr(ctPderjDvij);
		EditOTLtek14->Text = IntToStr(ctPderjCheck_0);
		EditOTLtek15->Text = IntToStr(ctPderjCheck_1);
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
case 30:
{
		EditOTLtek1->Text = IntToStr(VRUN);
		EditOTLtek2->Text = IntToStr(PAR_UN);
		EditOTLtek3->Text = IntToStr(X_TUN);
		EditOTLtek4->Text = IntToStr(E_TUN);
		EditOTLtek5->Text = IntToStr(DELUN);
		EditOTLtek6->Text = IntToStr(E_PUN);
		EditOTLtek7->Text = IntToStr(K_PUN);
		EditOTLtek8->Text = IntToStr(K_IUN);
		EditOTLtek9->Text = IntToStr(U_PUN);
		EditOTLtek10->Text = IntToStr(A_VIH);
		EditOTLtek11->Text = IntToStr(0);
		EditOTLtek12->Text = IntToStr(LIMPUN);
		EditOTLtek13->Text = IntToStr(LIMIUN);
		EditOTLtek14->Text = IntToStr(LIM1UN);
		EditOTLtek15->Text = IntToStr(LIM2UN);
		EditOTLtek16->Text = IntToStr(LIMUUN);
		EditOTLtek17->Text = IntToStr(LIMU_UN);
		EditOTLtek18->Text = IntToStr(LIMUPR_UN);
		EditOTLtek19->Text = IntToStr(PORCNV_UN);
		EditOTLtek20->Text = IntToStr(PORCPR_UN);
		EditOTLtek21->Text = IntToStr(PROBUN);
		EditOTLtek22->Text = IntToStr(0);
		EditOTLtek23->Text = IntToStr(T_VRUN);
		EditOTLtek24->Text = IntToStr(T_KUN);
		EditOTLtek25->Text = IntToStr(T_VREJ_UN);
		EditOTLtek26->Text = IntToStr(T_VPRB_UN);
		EditOTLtek27->Text = IntToStr(T_REQUN);
		EditOTLtek28->Text = IntToStr(CT_VRUN);
		EditOTLtek29->Text = IntToStr(CT_PR_UN);
		EditOTLtek30->Text = IntToStr(CT_REQUN);
}; break;
case 31:
{
		EditOTLtek1->Text = IntToStr(VRBM1);
		EditOTLtek2->Text = IntToStr(PR_SV_BM1);
		EditOTLtek3->Text = IntToStr(PR_NAP1);
		EditOTLtek4->Text = IntToStr(UST_BM1);
		EditOTLtek5->Text = IntToStr(X_TBM1);
		EditOTLtek6->Text = IntToStr(E_TBM1);
		EditOTLtek7->Text = IntToStr(DELBM1);
		EditOTLtek8->Text = IntToStr(DOPBM1);
		EditOTLtek9->Text = IntToStr(PAR_BM1);
		EditOTLtek10->Text = IntToStr(LIM1BM1);
		EditOTLtek11->Text = IntToStr(LIM2BM1);
		EditOTLtek12->Text = IntToStr(T_VRBM);
		EditOTLtek13->Text = IntToStr(T_KBM);
		EditOTLtek14->Text = IntToStr(VRBM2);
		EditOTLtek15->Text = IntToStr(PR_SV_BM2);
		EditOTLtek16->Text = IntToStr(PR_NAP2);
		EditOTLtek17->Text = IntToStr(UST_BM2);
		EditOTLtek18->Text = IntToStr(X_TBM2);
		EditOTLtek19->Text = IntToStr(E_TBM2);
		EditOTLtek20->Text = IntToStr(DELBM2);
		EditOTLtek21->Text = IntToStr(DOPBM2);
		EditOTLtek22->Text = IntToStr(PAR_BM2);
		EditOTLtek23->Text = IntToStr(LIM1BM2);
		EditOTLtek24->Text = IntToStr(LIM2BM2);
		EditOTLtek25->Text = IntToStr(T_KOTS_PROB);
		EditOTLtek26->Text = IntToStr(PR_KZ1);
		EditOTLtek27->Text = IntToStr(N_KZ1);
		EditOTLtek28->Text = IntToStr(PR_KZ2);
		EditOTLtek29->Text = IntToStr(N_KZ2);
		EditOTLtek30->Text = IntToStr(N_PROB);
}; break;
case 32:
{
		EditOTLtek1->Text = IntToStr(VRII);
		EditOTLtek2->Text = IntToStr(0);
		EditOTLtek3->Text = IntToStr(PR_SV_II);
		EditOTLtek4->Text = IntToStr(X_TII);
		EditOTLtek5->Text = IntToStr(E_TII);
		EditOTLtek6->Text = IntToStr(DELII);
		EditOTLtek7->Text = IntToStr(DOPII);
		EditOTLtek8->Text = IntToStr(PAR_II);
		EditOTLtek9->Text = IntToStr(LIM1II);
		EditOTLtek10->Text = IntToStr(LIM2II);
		EditOTLtek11->Text = IntToStr(T_VRII);
		EditOTLtek12->Text = IntToStr(CT_II);
		EditOTLtek13->Text = IntToStr(TK_OJ_OTV);
		EditOTLtek14->Text = IntToStr(T_KII);
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
case 33:
{
		EditOTLtek1->Text = IntToStr(OTVET_BM1[0]);
		EditOTLtek2->Text = IntToStr(OTVET_BM1[1]);
		EditOTLtek3->Text = IntToStr(OTVET_BM1[2]);
		EditOTLtek4->Text = IntToStr(OTVET_BM1[3]);
		EditOTLtek5->Text = IntToStr(OTVET_BM1[4]);
		EditOTLtek6->Text = IntToStr(OTVET_BM1[5]);
		EditOTLtek7->Text = IntToStr(OTVET_BM1[6]);
		EditOTLtek8->Text = IntToStr(OTVET_BM1[7]);
		EditOTLtek9->Text = IntToStr(OTVET_BM1[8]);
		EditOTLtek10->Text = IntToStr(OTVET_BM1[9]);
		EditOTLtek11->Text = IntToStr(OTVET_BM2[0]);
		EditOTLtek12->Text = IntToStr(OTVET_BM2[1]);
		EditOTLtek13->Text = IntToStr(OTVET_BM2[2]);
		EditOTLtek14->Text = IntToStr(OTVET_BM2[3]);
		EditOTLtek15->Text = IntToStr(OTVET_BM2[4]);
		EditOTLtek16->Text = IntToStr(OTVET_BM2[5]);
		EditOTLtek17->Text = IntToStr(OTVET_BM2[6]);
		EditOTLtek18->Text = IntToStr(OTVET_BM2[7]);
		EditOTLtek19->Text = IntToStr(OTVET_BM2[8]);
		EditOTLtek20->Text = IntToStr(OTVET_BM2[9]);
		EditOTLtek21->Text = IntToStr(OTVET_II[0]);
		EditOTLtek22->Text = IntToStr(OTVET_II[1]);
		EditOTLtek23->Text = IntToStr(OTVET_II[2]);
		EditOTLtek24->Text = IntToStr(OTVET_II[3]);
		EditOTLtek25->Text = IntToStr(OTVET_II[4]);
		EditOTLtek26->Text = IntToStr(OTVET_II[5]);
		EditOTLtek27->Text = IntToStr(OTVET_II[6]);
		EditOTLtek28->Text = IntToStr(OTVET_II[7]);
		EditOTLtek29->Text = IntToStr(OTVET_II[8]);
		EditOTLtek30->Text = IntToStr(OTVET_II[9]);
}; break;
case 34:
{
		EditOTLtek1->Text = IntToStr(KOM_BM1[0]);
		EditOTLtek2->Text = IntToStr(KOM_BM1[1]);
		EditOTLtek3->Text = IntToStr(KOM_BM1[2]);
		EditOTLtek4->Text = IntToStr(KOM_BM1[3]);
		EditOTLtek5->Text = IntToStr(KOM_BM1[4]);
		EditOTLtek6->Text = IntToStr(0);
		EditOTLtek7->Text = IntToStr(KOM_BM2[0]);
		EditOTLtek8->Text = IntToStr(KOM_BM2[1]);
		EditOTLtek9->Text = IntToStr(KOM_BM2[2]);
		EditOTLtek10->Text = IntToStr(KOM_BM2[3]);
		EditOTLtek11->Text = IntToStr(KOM_BM2[4]);
		EditOTLtek12->Text = IntToStr(0);
		EditOTLtek13->Text = IntToStr(KOM_II[0]);
		EditOTLtek14->Text = IntToStr(KOM_II[1]);
		EditOTLtek15->Text = IntToStr(KOM_II[2]);
		EditOTLtek16->Text = IntToStr(KOM_II[3]);
		EditOTLtek17->Text = IntToStr(KOM_II[4]);
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
case 35:
{
		EditOTLtek1->Text = IntToStr(PR_KN);
		EditOTLtek2->Text = IntToStr(PR_PER0_KN);
		EditOTLtek3->Text = IntToStr(PR_SV_KN);
		EditOTLtek4->Text = IntToStr(0);
		EditOTLtek5->Text = IntToStr(0);
		EditOTLtek6->Text = IntToStr(KOM_KN);
		EditOTLtek7->Text = IntToStr(OTVET_KN);
		EditOTLtek8->Text = IntToStr(0);
		EditOTLtek9->Text = IntToStr(CT_KN);
		EditOTLtek10->Text = IntToStr(0);
		EditOTLtek11->Text = IntToStr(OTVET_KN_M[0]);
		EditOTLtek12->Text = IntToStr(OTVET_KN_M[1]);
		EditOTLtek13->Text = IntToStr(OTVET_KN_M[2]);
		EditOTLtek14->Text = IntToStr(OTVET_KN_M[3]);
		EditOTLtek15->Text = IntToStr(OTVET_KN_M[4]);
		EditOTLtek16->Text = IntToStr(OTVET_KN_M[5]);
		EditOTLtek17->Text = IntToStr(OTVET_KN_M[6]);
		EditOTLtek18->Text = IntToStr(OTVET_KN_M[7]);
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
    EditNastrIn2 -> Text =FloatToStrF((float)nasmod[2]/4095.0*RRG5_MAX,ffFixed,5,1);
    EditNastrIn3 -> Text =( nasmod[3] ? "Да" : "Нет" );
    EditNastrIn4 -> Text =( nasmod[4] ? "Да" : "Нет" );
    EditNastrIn5 -> Text =FloatToStrF((float)nasmod[5]/10.0,ffFixed,5,1);
    switch(nasmod[6])
    {
        case 0:{EditNastrIn6 -> Text="Нет";break;}
        case 1:{EditNastrIn6 -> Text="Резист.";break;}
        case 2:{EditNastrIn6 -> Text="Чиллер";break;}
    }
    EditNastrIn7 -> Text =( nasmod[7] ? "Да" : "Нет" );
    EditNastrIn8 -> Text =( nasmod[8] ? "Да" : "Нет" );
    EditNastrIn9 -> Text =( nasmod[9] ? "Да" : "Нет" );
    EditNastrIn10 -> Text =FloatToStrF((float)nasmod[10]/4095.0*650.0,ffFixed,5,0);
    EditNastrIn11 -> Text =FloatToStrF((float)nasmod[11]/4095.0*650.0,ffFixed,5,0);
    EditNastrIn12 -> Text =FloatToStrF((float)nasmod[12]/255.0*40.0,ffFixed,5,1);
    EditNastrIn13 -> Text =FloatToStrF((float)nasmod[13]/255.0*40.0,ffFixed,5,1);
    EditNastrIn14 -> Text =FloatToStrF( 1000.0 / float(nasmod[14]), ffFixed, 6, 0 );
    EditNastrIn15 -> Text =( nasmod[15] ? "Да" : "Нет" );
    EditNastrIn16 -> Text =FloatToStrF(((float)nasmod[16]/200.0), ffFixed, 5, 1 );
    EditNastrIn17 -> Text =FloatToStrF(((float)nasmod[17]/102.4), ffFixed, 5, 0 );
    EditNastrIn18 -> Text =FloatToStrF(((float)nasmod[18]/102.4), ffFixed, 5, 0 );
    EditNastrIn19-> Text =( nasmod[19] ? "Да" : "Нет" );
    EditNastrIn20 -> Text =FloatToStrF((float)nasmod[20]/10.0,ffFixed,5,1);
    EditNastrIn21 -> Text =FloatToStrF(((float)nasmod[21]/200.0), ffFixed, 5, 1 );
    EditNastrIn22-> Text =( nasmod[22] ? "Да" : "Нет" );






}

//---------------------------------------------------------------------------
//--Подтверждение передачи настроечных параметров--//
//---------------------------------------------------------------------------
void __fastcall TForm1::BtnNastrDaClick(TObject *Sender)
{

    // Скрыть панель
    PanelParNastr -> Visible = false;
    //Предельный уровень высоковакуумной откачки камеры мрт200
    nasmod[0]=int(((log10(StrToFloat(EditNastrTo0->Text)))*0.6+5.6)*1000.0);
    //Уровень высоковакуумной откачки камеры ТМН  мрт200
    nasmod[1]=int(((log10(StrToFloat(EditNastrTo1->Text)))*0.6+5.6)*1000.0);
    //Расход РРГ4 (подача Ar под пластину)
    nasmod[2]=StrToFloat(EditNastrTo2->Text)/RRG4_MAX*4095.0;
    //Рабочий цикл с отключением технологического процесса
    EditNastrTo3 -> Text == "Да" ? nasmod[3] = 1 : nasmod[3] = 0;
    //Включить нагрев
    EditNastrTo4 -> Text == "Да" ? nasmod[4] = 1 : nasmod[4] = 0;
    //Температура нагрева технологической камеры
    nasmod[5]=StrToFloat(EditNastrTo5->Text)*10;
    //Стабилизация температуры п/д
    nasmod[6]=EditNastrTo6 ->ItemIndex;
    //Включить БПИИ
    EditNastrTo7 -> Text == "Да" ? nasmod[7] = 1 : nasmod[7] = 0;
    //Включить БПМ1
    EditNastrTo8 -> Text == "Да" ? nasmod[8] = 1 : nasmod[8] = 0;
    //Включить БПМ2
    EditNastrTo9 -> Text == "Да" ? nasmod[9] = 1 : nasmod[9] = 0;
    //Максимальное напряжние на БПМ1
    nasmod[10]=int(StrToFloat(EditNastrTo10->Text)*4095.0/650.0);
    //Максимальное напряжение на БПМ2
    nasmod[11]=int(StrToFloat(EditNastrTo11->Text)*4095.0/650.0);
    //Частота комутации М1
    nasmod[12]= int(StrToFloat(EditNastrTo12->Text)*255.0/40.0);
    //Частота комутации М2
    nasmod[13]= int(StrToFloat(EditNastrTo13->Text)*255.0/40.0);
    //Допустимый коэффициент согласования ВЧГ
    nasmod[14]=(unsigned int)(1000.0/StrToFloat(EditNastrTo14->Text));
    //Транспортный тес с пластиной
    EditNastrTo15 -> Text == "Да" ? nasmod[15] = 1 : nasmod[15] = 0;
    //Путь подложкодержателя для напыления
    nasmod[16]=StrToFloat(EditNastrTo16->Text)*200;
    //минимальная скорость вращения подложкодержателя
    nasmod[17]=(unsigned int)(102.4*StrToInt(EditNastrTo17->Text));
    //рабочая скорость вращения подложкодержателя
    nasmod[18]=(unsigned int)(102.4*StrToInt(EditNastrTo18->Text));
    //Работа с КН
    EditNastrTo19 -> Text == "Да" ? nasmod[19] = 1 : nasmod[19] = 0;
    //Температура КН
    nasmod[20]=StrToFloat(EditNastrTo20->Text)*10;
    // Путь подъёма п/д для ионной очистки
    nasmod[21]=StrToFloat(EditNastrTo21->Text)*200;
    //Отпыл на заслонке магнетронов ?
    EditNastrTo22 -> Text == "Да" ? nasmod[22] = 1 : nasmod[22] = 0;




    // запомнили действие в журнал
    MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time -> Caption + " : Изменены значения настроечного массива : ");
    if ( EditNastrTo0 -> Text != EditNastrIn0 -> Text )
        MemoStat -> Lines -> Add( "Предельный уровень высоковакуумной откачки камеры : " + EditNastrIn0 -> Text + " -> " + EditNastrTo0 -> Text );
    if ( EditNastrTo1 -> Text != EditNastrIn1 -> Text )
        MemoStat -> Lines -> Add( "Уровень высоковакуумной откачки камеры ТМН : " + EditNastrIn1 -> Text + " -> " + EditNastrTo1 -> Text );
    if ( EditNastrTo2 -> Text != EditNastrIn2 -> Text )
        MemoStat -> Lines -> Add( "Расход РРГ4 (подача Ar : " + EditNastrIn2 -> Text + " -> " + EditNastrTo2 -> Text );
    if ( EditNastrTo3 -> Text != EditNastrIn3 -> Text )
        MemoStat -> Lines -> Add( "Рабочий цикл с отключением технологического процесса? : " + EditNastrIn3 -> Text + " -> " + EditNastrTo3 -> Text );
    if ( EditNastrTo4 -> Text != EditNastrIn4 -> Text )
        MemoStat -> Lines -> Add( "Включить нагрев? : " + EditNastrIn4 -> Text + " -> " + EditNastrTo4 -> Text );
    if ( EditNastrTo5 -> Text != EditNastrIn5 -> Text )
        MemoStat -> Lines -> Add( "Температура нагрева технологической камеры : " + EditNastrIn5 -> Text + " -> " + EditNastrTo5 -> Text );
    if ( EditNastrTo6 -> Text != EditNastrIn6 -> Text )
        MemoStat -> Lines -> Add( "Стабилизация температуры п/д? : " + EditNastrIn6 -> Text + " -> " + EditNastrTo6 -> Text );
    if ( EditNastrTo7 -> Text != EditNastrIn7 -> Text )
        MemoStat -> Lines -> Add( "Включать БПИИ? : " + EditNastrIn7 -> Text + " -> " + EditNastrTo7 -> Text );
    if ( EditNastrTo8 -> Text != EditNastrIn8 -> Text )
        MemoStat -> Lines -> Add( "Включать БПМ1? : " + EditNastrIn8 -> Text + " -> " + EditNastrTo8 -> Text );
    if ( EditNastrTo9 -> Text != EditNastrIn9 -> Text )
        MemoStat -> Lines -> Add( "Включать БПМ2? : " + EditNastrIn9 -> Text + " -> " + EditNastrTo9 -> Text );
    if ( EditNastrTo10 -> Text != EditNastrIn10 -> Text )
        MemoStat -> Lines -> Add( "Максимальное напряжение на БПМ1 : " + EditNastrIn10 -> Text + " -> " + EditNastrTo10 -> Text );
    if ( EditNastrTo11 -> Text != EditNastrIn11 -> Text )
        MemoStat -> Lines -> Add( "Максимальное напряжение на БПМ2 : " + EditNastrIn11 -> Text + " -> " + EditNastrTo11 -> Text );
    if ( EditNastrTo12 -> Text != EditNastrIn12 -> Text )
        MemoStat -> Lines -> Add( "Частота коммутации М1 : " + EditNastrIn12 -> Text + " -> " + EditNastrTo12 -> Text );
    if ( EditNastrTo13 -> Text != EditNastrIn13 -> Text )
        MemoStat -> Lines -> Add( "Частота коммутации М2 : " + EditNastrIn13 -> Text + " -> " + EditNastrTo13 -> Text );
    if ( EditNastrTo14 -> Text != EditNastrIn14 -> Text )
        MemoStat -> Lines -> Add( "Допустимый коэффициент согласования ВЧГ : " + EditNastrIn14 -> Text + " -> " + EditNastrTo14 -> Text );
    if ( EditNastrTo15 -> Text != EditNastrIn15 -> Text )
        MemoStat -> Lines -> Add( "Транспортный тест с пластиной? : " + EditNastrIn15 -> Text + " -> " + EditNastrTo15 -> Text );
    if ( EditNastrTo16 -> Text != EditNastrIn16 -> Text )
        MemoStat -> Lines -> Add( "Путь подъёма подложкодержателя для напыления : " + EditNastrIn16 -> Text + " -> " + EditNastrTo16 -> Text );
    if ( EditNastrTo17 -> Text != EditNastrIn17 -> Text )
        MemoStat -> Lines -> Add( "Минимальная скорость вращения подложкодержателя : " + EditNastrIn17 -> Text + " -> " + EditNastrTo17 -> Text );
    if ( EditNastrTo18 -> Text != EditNastrIn18 -> Text )
        MemoStat -> Lines -> Add( "Рабочая скорость вращения подложкодержателя : " + EditNastrIn18 -> Text + " -> " + EditNastrTo18 -> Text );
    if ( EditNastrTo19 -> Text != EditNastrIn19 -> Text )
        MemoStat -> Lines -> Add( "Работа с КН? : " + EditNastrIn19 -> Text + " -> " + EditNastrTo19 -> Text );
    if ( EditNastrTo20 -> Text != EditNastrIn20 -> Text )
        MemoStat -> Lines -> Add( "Температура КН (выход на режим) : " + EditNastrIn20 -> Text + " -> " + EditNastrTo20 -> Text );
    if ( EditNastrTo21 -> Text != EditNastrIn21 -> Text )
        MemoStat -> Lines -> Add( "Путь подъёма подложкодержателя для ионной очистки : " + EditNastrIn21 -> Text + " -> " + EditNastrTo21 -> Text );
    if ( EditNastrTo22 -> Text != EditNastrIn22 -> Text )
        MemoStat -> Lines -> Add( "Отпыл на заслонки магнетронов ? : " + EditNastrIn22 -> Text + " -> " + EditNastrTo22 -> Text );


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
    EditNastrTo17 -> Color = clWhite;
    EditNastrTo18 -> Color = clWhite;
    EditNastrTo19 -> Color = clWhite;
    EditNastrTo20 -> Color = clWhite;
    EditNastrTo21 -> Color = clWhite;
    EditNastrTo22 -> Color = clWhite;





    // сохранить значения настроечного массива
    MemoNasmod -> Lines -> Clear();

    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo0->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo1->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo2->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo3->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo4->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo5->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo6->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo7->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo8->ItemIndex));
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo9->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo10->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo11->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo12->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo13->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo14->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo15->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo16->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo17->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo18->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo19->ItemIndex));
    MemoNasmod -> Lines -> Add(EditNastrTo20->Text);
    MemoNasmod -> Lines -> Add(EditNastrTo21->Text);
    MemoNasmod -> Lines -> Add(IntToStr(EditNastrTo22->ItemIndex));

    MemoNasmod -> Lines -> SaveToFile("Nasmod\\Nasmod.txt");


    if((nasmod[4]==0)||(nasmod[6]==0))
    {
        Panel16             ->Caption="-";
        Panel111            ->Caption="-";
        Panel244            ->Caption="-";
        EdtARed1_15->Enabled=false;
        EdtARed4_15->Enabled=false;
        EdtARed5_15->Enabled=false;
        EdtARed6_15->Enabled=false;
        EdtARed7_15->Enabled=false;
        EdtARed8_15->Enabled=false;
        EdtARed9_15->Enabled=false;
        EdtRRed0_15->Enabled=false;
        par[0][15]=0;
        par[1][15]=0;
        par[4][15]=0;
        par[5][15]=0;
        par[6][15]=0;
        par[7][15]=0;
        par[8][15]=0;
        par[9][15]=0;

    }
    else if(nasmod[6]==1)
    {
        Panel16             ->Caption="80 - 400";
        Panel111            ->Caption="80 - 400";
        Panel244            ->Caption="80 - 400";
        EdtARed1_15->Enabled=true;
        EdtARed4_15->Enabled=true;
        EdtARed5_15->Enabled=true;
        EdtARed6_15->Enabled=true;
        EdtARed7_15->Enabled=true;
        EdtARed8_15->Enabled=true;
        EdtARed9_15->Enabled=true;
        EdtRRed0_15->Enabled=true;
        par[0][15]=800;
        par[1][15]=800;
        par[4][15]=800;
        par[5][15]=800;
        par[6][15]=800;
        par[7][15]=800;
        par[8][15]=800;
        par[9][15]=800;
    }
    else
    {
        Panel16             ->Caption="-20 - 90";
        Panel111            ->Caption="-20 - 90";
        Panel244            ->Caption="-20 - 90";
        EdtARed1_15->Enabled=true;
        EdtARed4_15->Enabled=true;
        EdtARed5_15->Enabled=true;
        EdtARed6_15->Enabled=true;
        EdtARed7_15->Enabled=true;
        EdtARed8_15->Enabled=true;
        EdtARed9_15->Enabled=true;
        EdtRRed0_15->Enabled=true;
        par[0][15]=0;
        par[1][15]=0;
        par[4][15]=0;
        par[5][15]=0;
        par[6][15]=0;
        par[7][15]=0;
        par[8][15]=0;
        par[9][15]=0;
    }
    EdtARed1Exit(EdtRRed0_15);
    EdtARed1Exit(EdtARed1_15);
    EdtARed1Exit(EdtARed4_15);
    EdtARed1Exit(EdtARed5_15);
    EdtARed1Exit(EdtARed6_15);
    EdtARed1Exit(EdtARed7_15);
    EdtARed1Exit(EdtARed8_15);
    EdtARed1Exit(EdtARed9_15);
    if(nasmod[7])
    {
        EdtARed3_5->Enabled=true;
        EdtRRed0_5->Enabled=true;

    }
    else
    {
        EdtARed3_5->Enabled=false;
        EdtRRed0_5->Enabled=false;
        par[3][5]=0;
        par[0][5]=0;

    }
    if(nasmod[8])
    {
        EdtRRed0_6->Enabled=true;
        EdtARed10_6->Enabled=true;
        EdtARed12_6->Enabled=true;
        EdtARed4_6->Enabled=true;
        EdtARed5_6->Enabled=true;
        EdtARed6_6->Enabled=true;
        EdtARed7_6->Enabled=true;
        EdtARed8_6->Enabled=true;
        EdtARed9_6->Enabled=true;

    }
    else
    {
        EdtRRed0_6->Enabled=false;
        EdtARed10_6->Enabled=false;
        EdtARed12_6->Enabled=false;
        EdtARed4_6->Enabled=false;
        EdtARed5_6->Enabled=false;
        EdtARed6_6->Enabled=false;
        EdtARed7_6->Enabled=false;
        EdtARed8_6->Enabled=false;
        EdtARed9_6->Enabled=false;
        par[0][6]=0;
        par[10][6]=0;
        par[12][6]=0;
        par[4][6]=0;
        par[5][6]=0;
        par[6][6]=0;
        par[7][6]=0;
        par[8][6]=0;
        par[9][6]=0;
    }
    if(nasmod[9])
    {
        EdtRRed0_7->Enabled=true;
        EdtARed11_7->Enabled=true;
        EdtARed12_7->Enabled=true;
        EdtARed4_7->Enabled=true;
        EdtARed5_7->Enabled=true;
        EdtARed6_7->Enabled=true;
        EdtARed7_7->Enabled=true;
        EdtARed8_7->Enabled=true;
        EdtARed9_7->Enabled=true;

    }
    else
    {
        EdtRRed0_7->Enabled=false;
        EdtARed11_7->Enabled=false;
        EdtARed12_7->Enabled=false;
        EdtARed4_7->Enabled=false;
        EdtARed5_7->Enabled=false;
        EdtARed6_7->Enabled=false;
        EdtARed7_7->Enabled=false;
        EdtARed8_7->Enabled=false;
        EdtARed9_7->Enabled=false;
        par[0][7]=0;
        par[11][7]=0;
        par[12][7]=0;
        par[4][7]=0;
        par[5][7]=0;
        par[6][7]=0;
        par[7][7]=0;
        par[8][7]=0;
        par[9][7]=0;

    }
    EdtARed1Exit(EdtRRed0_6);
    EdtARed1Exit(EdtARed10_6);
    EdtARed1Exit(EdtARed12_6);
    EdtARed1Exit(EdtARed4_6);
    EdtARed1Exit(EdtARed5_6);
    EdtARed1Exit(EdtARed6_6);
    EdtARed1Exit(EdtARed7_6);
    EdtARed1Exit(EdtARed8_6);
    EdtARed1Exit(EdtARed9_6);

    EdtARed1Exit(EdtRRed0_7);
    EdtARed1Exit(EdtARed11_7);
    EdtARed1Exit(EdtARed12_7);
    EdtARed1Exit(EdtARed4_7);
    EdtARed1Exit(EdtARed5_7);
    EdtARed1Exit(EdtARed6_7);
    EdtARed1Exit(EdtARed7_7);
    EdtARed1Exit(EdtARed8_7);
    EdtARed1Exit(EdtARed9_7);

    if(nasmod[7])
    {
        EdtARed3_5->Enabled=true;
        EdtRRed0_5->Enabled=true;
    }
    else
    {
        EdtARed3_5->Enabled=false;
        EdtRRed0_5->Enabled=false;

        par[3][5]=0;
        par[0][5]=0;


    }
    EdtARed1Exit(EdtRRed0_5);
    EdtARed1Exit(EdtARed3_5);



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
    Comport[4]->Dev_Timer++;
    Comport[5]->Dev_Timer++;
}
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

    // Температура нагрева п/д
    if(shr[31]||shr[38])
    {
        graphTemp = graphTemp + FloatToStrF((float(TEK_TEMP2)/10.0),ffFixed,4,1) + ";";
        serTemp[0] -> AddY(float(TEK_TEMP2)/10.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[0] -> AddY(0.0,Label_Time -> Caption);
    }

    // Ток ИИ                               
    if(shr[33])
    {
        graphTemp = graphTemp + FloatToStrF((float)OTVET_II[5]*512.0/1023, ffFixed, 6, 0) + ";";
        serTemp[1] -> AddY((float)OTVET_II[5]*512.0/1023, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[1] -> AddY(0.0,Label_Time -> Caption);
    }

    // М1
    if(shr[35])
    {
        graphTemp = graphTemp + FloatToStrF((float)OTVET_BM1[6]*3072.0/1023.0,ffFixed,6,0) + ";";
        serTemp[2] -> AddY((float)OTVET_BM1[6]*3072.0/1023.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[2] -> AddY(0.0,Label_Time -> Caption);
    }

    // М2
    if(shr[36])
    {
        graphTemp = graphTemp + FloatToStrF((float)OTVET_BM2[6]*3072.0/1023.0,ffFixed,6,0) + ";";
        serTemp[3] -> AddY((float)OTVET_BM2[6]*3072.0/1023.0, Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[3] -> AddY(0.0,Label_Time -> Caption);
    }


    // Пад. мощность ВЧГ ИП
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
    // Отр. мощность ВЧГ ИП
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
   /* // процент ДЗ
    if(shr[25]||shr[27])
    {

        graphTemp = graphTemp + FloatToStrF((float(TEK_POZ_DZASL)/10000.0*100.0),ffFixed,3,0) + ";";
        serTemp[6] -> AddY(float(TEK_POZ_DZASL)/10000.0*100.0, Label_Time->Caption );
    }
    else
    {    */
        graphTemp = graphTemp + "0,0;";
        serTemp[6] -> AddY(0.0,Label_Time -> Caption);
   // }
    // давл
    if(shr[24])
    {

        graphTemp = graphTemp + FloatToStrF(pow(10,(float)D_D3/1000.0-3.5),ffFixed,5,1) + ";";
        serTemp[7] -> AddY(pow(10,(float)D_D3/1000.0-3.5), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[7] -> AddY(0.0,Label_Time -> Caption);
    }
    // РРГ1
    if(shr[20])
    {

        graphTemp = graphTemp + FloatToStrF((float)aik[6]*RRG1_MAX/4095.0,  ffFixed, 6, 2) + ";";
        serTemp[8] -> AddY(((float)aik[6]*RRG1_MAX/4095.0), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[8] -> AddY(0.0,Label_Time -> Caption);
    }
    // РРГ2
    if(shr[21])
    {

        graphTemp = graphTemp + FloatToStrF((float)aik[7]*RRG2_MAX/4095.0,  ffFixed, 6, 1) + ";";
        serTemp[9] -> AddY(((float)aik[7]*RRG2_MAX/4095.0  ), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[9] -> AddY(0.0,Label_Time -> Caption);
    }
    // РРГ3
    if(shr[22])
    {

        graphTemp = graphTemp + FloatToStrF((float)aik[8]*RRG3_MAX/4095.0,  ffFixed, 6, 1) + ";";
        serTemp[10] -> AddY(((float)aik[8]*RRG3_MAX/4095.0), Label_Time->Caption );
    }
    else
    {
        graphTemp = graphTemp + "0,0;";
        serTemp[10] -> AddY(0.0,Label_Time -> Caption);
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
                (((TImage*)Sender)->Name) == "Tmn"
        )
    {
        BtnDeviceOn -> Caption = "Вкл.";
        BtnDeviceOff -> Caption = "Откл.";
    }

    else    if  (
                (((TImage*)Sender)->Name) == "pp"
                )
    {
        BtnDeviceOn -> Caption = "Вверх";
        BtnDeviceOff -> Caption = "Вниз";
    }
    else
    {
        BtnDeviceOn -> Caption = "Откр.";
        BtnDeviceOff -> Caption = "Закр.";
    }
    // отобразить панель управления
    LblDeviceName -> Caption = ((TImage*)Sender) -> Hint;
    PnlDevice -> Hint = ((TImage*)Sender) -> Name;

    if(((((TImage*)Sender)->Name) == "Kl1")||((((TImage*)Sender)->Name) == "Kl2")
    ||((((TImage*)Sender)->Name) == "Kl7")||((((TImage*)Sender)->Name) == "Kl8")
    ||((((TImage*)Sender)->Name) == "ZaslII")||((((TImage*)Sender)->Name) == "ZaslM1")
    ||((((TImage*)Sender)->Name) == "ZaslM2")
    )

        PnlDevice -> Top = ((TImage*)Sender)->Top - 20;

    else
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
    if ( ((TButton*)Sender) -> Parent -> Hint == "Kl1" )       SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x100);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x200);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl3" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x400);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl4" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x1000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl5" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x2000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl6" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl7" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 2, 0x800);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl8" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x04);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl_Nap1" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x8000);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Kl_Nap2" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 3, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Tmn" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x02);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Kn" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x04);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x01);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x300);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fk_Shl_M" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 0, 0x200);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fvn_Shl" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x08);
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Fvn_Kam" )  SetOut(StrToInt(((TButton*)Sender)->Hint), 4, 0x04);

    else if ( ((TButton*)Sender) -> Parent -> Hint == "pp" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 1, 0x08);
            SetOut(1, 1, 0x04);
        }
        else
        {
            SetOut(1, 1, 0x08);
            SetOut(0, 1, 0x04);
        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Zatv" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 0, 0x40);
            SetOut(1, 0, 0x20);
        }
        else
        {
            SetOut(1, 0, 0x40);
            SetOut(0, 0, 0x20);
        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "ZaslII" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 1, 0x200);
            SetOut(1, 1, 0x100);
        }
        else
        {
            SetOut(1, 1, 0x200);
            SetOut(0, 1, 0x100);
        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "ZaslM1" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 1, 0x20);
            SetOut(1, 1, 0x10);
        }
        else
        {
            SetOut(1, 1, 0x20);
            SetOut(0, 1, 0x10);
        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "ZaslM2" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 1, 0x80);
            SetOut(1, 1, 0x40);
        }
        else
        {
            SetOut(1, 1, 0x80);
            SetOut(0, 1, 0x40);
        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "ZaslPD" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(0, 1, 0x02);
            SetOut(1, 1, 0x01);
        }
        else
        {
            SetOut(1, 1, 0x02);
            SetOut(0, 1, 0x01);
        }
    }
    else if ( ((TButton*)Sender) -> Parent -> Hint == "Tmn" )
    {
        if ( StrToInt(((TButton*)Sender)->Hint) )
        {
            SetOut(1, 4, 0x60);

        }
        else
        {

            SetOut(0, 4, 0x40);
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
    // норма
    EditNormName -> Text = NormNames[norma];
    // количество режимов, чьи шаги надо отображать
    #define SHR_VALUE_COUNT 11
    // порядок следования важности шагов (веса шагов)
    unsigned char SHRValue[SHR_VALUE_COUNT] = {8,5,3,2,7,1,4,6,10,19,9};
    // анализируем активность режимов в порядке убывания значимости
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
            // в режиме 8 шаг 7
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==7))
            {
                if(shr[5]==24)
                    Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
                else if(shr[5]==32)
                    Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
                else
                    Form1 -> EditSHRName -> Text = SHR5Names[shr[5]];
            }
            //в режиме 8 шаг 6 пишем шаги режима 7
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==6))
                Form1 -> EditSHRName -> Text = SHR7Names[shr[7]];
            //в режиме 8 шаг 6 пишем шаги режима 7
            if((SHRValue[i]==10)&&(shr[SHRValue[i]]==10))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            //в режиме 8 шаг 6 пишем шаги режима 7
            if((SHRValue[i]==8)&&(shr[SHRValue[i]]==6))
                Form1 -> EditSHRName -> Text = SHR7Names[shr[7]];
            // в режиме 6 шаг 14
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==14))
            {
                if(shr[10]==10)
                    Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];

                else
                    Form1 -> EditSHRName -> Text = SHR10Names[shr[10]];
            }
            //в режиме 6 шаг 11 пишем шаги режима 19
            if((SHRValue[i]==6)&&(shr[SHRValue[i]]==11))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            //в режиме 5 шаг 32 пишем шаги режима 19
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==32))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            //в режиме 5 шаг 24 пишем шаги режима 19
            if((SHRValue[i]==5)&&(shr[SHRValue[i]]==24))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            //в режиме 3 шаг 31 пишем шаги режима 4
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==31))
                Form1 -> EditSHRName -> Text = SHR4Names[shr[4]];
            //в режиме 3 шаг 40 пишем шаги режима 19
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==40))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            //в режиме 3 шаг 8 пишем шаги режима 19
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==8))
                Form1 -> EditSHRName -> Text = SHR19Names[shr[19]];
            //в режиме 3 шаг 2 пишем шаги режима 1
            if((SHRValue[i]==3)&&(shr[SHRValue[i]]==2))
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];
            //в режиме 2 шаг 2 пишем шаги режима 1
            if((SHRValue[i]==2)&&(shr[SHRValue[i]]==2))
                Form1 -> EditSHRName -> Text = SHR1Names[shr[1]];

            // в режиме 2 шаг 10 считать время
            if(shr[2]==10)
            {
                TempStr = SHR2Names[shr[2]]+IntToStr(par[N_ST][12]-CT_2);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 2 шаг 19 считать время
            if(shr[2]==19)
            {
                TempStr = SHR2Names[shr[2]]+IntToStr(par[N_ST][12]-CT_2);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 2 шаг 29 считать время
            if(shr[2]==29)
            {
                TempStr = SHR2Names[shr[2]]+IntToStr(par[N_ST][12]-CT_2);
                Form1 -> EditSHRName -> Text = TempStr;
            }

            // в режиме 3 шаг 5 считать время
            if(shr[3]==5)
            {
                TempStr = SHR3Names[shr[3]]+IntToStr(T_NAPUSK-CT_3);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 5 считать время
            if(shr[4]==5)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(T_SBROSHE-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 6 считать время
            if(shr[4]==6)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 11 считать время
            if(shr[4]==11)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 20 считать время
            if(shr[4]==20)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 38 считать время
            if(shr[4]==38)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][14]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 58 считать время
            if(shr[4]==58)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][12]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 74 считать время
            if(shr[4]==74)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(T_SBROSHE-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 4 шаг 86 считать время
            if(shr[4]==86)
            {
                TempStr = SHR4Names[shr[4]]+IntToStr(par[N_ST][14]-CT_4);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 5 шаг 29 считать время
            if(shr[5]==29)
            {
                TempStr = SHR5Names[shr[5]]+IntToStr(T_NAPUSK-CT_5);
                Form1 -> EditSHRName -> Text = TempStr;
            }
            // в режиме 6 шаг 8 считать время
            if(shr[6]==8)
            {
                TempStr = SHR6Names[shr[6]]+IntToStr(T_NAPUSK-CT_6);
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
                                                Form1->MemoDiag->Lines->Add(">>"+Form1->Label_Time->Caption+" : выставлено : "+DiagnSNames[i*8+j]);
                                        }
                                        else
                                        {
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

    if(shr[3]==6)
    {
        APanel_String1 -> Caption = "Напуск завершен";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Загрузка-Выгрузка";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Caption = "разрешена";
        APanel_String3 -> Visible = true;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="ДА";
        APanel_NetBut -> Caption="НЕТ";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[3]==7)
    {
        APanel_String1 -> Caption = "Начать откачку ?";
        APanel_String1 -> Visible = true;
         APanel_String2 -> Caption = "Шлюз с пластиной ?";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[5]==15)
    {
        APanel_String1 -> Caption = "Шлюз с пластиной?";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Visible = false;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = true;
        APanel_NetBut -> Visible = true;
        APanel_DaBut -> Caption="ДА";
        APanel_NetBut -> Caption="НЕТ";
        APanel -> Visible = true;
        pr_yel = 1;
    }

    else if(shr[5]==30)
    {
        APanel_String1 -> Caption = "Напуск завершён";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Выгрузите пластину";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[5]==31)
    {
        APanel_String1 -> Caption = "Начать откачку? ";
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
    else if(shr[6]==4)
    {
        APanel_String1 -> Caption = "Есть пластина в шлюзе?";
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
    else if(shr[6]==9)
    {
        APanel_String1 -> Caption = "Напуск завершён";
        APanel_String1 -> Visible = true;
        APanel_String2 -> Caption = "Выгрузите пластину?";
        APanel_String2 -> Visible = true;
        APanel_String3 -> Visible = false;
        APanel_DaBut -> Visible = false;
        APanel_NetBut -> Visible = false;
        APanel_DaBut -> Caption="Да";
        APanel_NetBut -> Caption="Нет";
        APanel -> Visible = true;
        pr_yel = 1;
    }
    else if(shr[6]==10)
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
             LblRejim -> Caption = "Откачка камеры";
		    // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
            // есть охлаждение КН по zin
		    if(!(zin[0]&0x08)&&(nasmod[19])) { ListBoxCondition -> Items -> Add("Нет охлаждения КН"); }
            // есть охлаждение ТМН zin
		    if(!(zin[0]&0x10)) { ListBoxCondition -> Items -> Add("Нет охлаждения ТМН"); }
            // есть связь с Д1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("Нет связи с Д1"); }
		    // есть связь с Д2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с Д2"); }
            // есть связь с Д3
		    //if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("Нет связи с Д3"); }
		    // есть связь с Д4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("Нет связи с Д4"); }
            // есть связь с Д5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Д5"); }
		    // есть связь с Д6
		    //if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("Нет связи с Д6"); }
            // есть связь с термодат
		    if((nasmod[4])&&(diagnS[2]&0x10))   { ListBoxCondition -> Items -> Add("Нет связи с Термодат"); }
            // есть связь с БПИИ
		    if((nasmod[7])&&(diagnS[2]&0x20))   { ListBoxCondition -> Items -> Add("Нет связи с БПИИ"); }
            // есть связь с БПМ1
		    if((nasmod[8])&&(diagnS[2]&0x04))   { ListBoxCondition -> Items -> Add("Нет связи с БПМ1"); }
            // есть связь с БПМ2
		    if((nasmod[9])&&(diagnS[2]&0x08))   { ListBoxCondition -> Items -> Add("Нет связи с БПМ2"); }
            // есть связь с КН
		    if((nasmod[19])&&(diagnS[2]&0x40))   { ListBoxCondition -> Items -> Add("Нет связи с КН"); }
            // не запущен ни один режим
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((i!=38)&&(shr[i])) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);


        }; break;
        case 2:
        {
             LblRejim -> Caption = "Тренировка";
		    // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
            // есть охлаждение КН по zin
		    if(!(zin[0]&0x08)&&(nasmod[19])) { ListBoxCondition -> Items -> Add("Нет охлаждения КН"); }
            // есть охлаждение ТМН zin
		    if(!(zin[0]&0x10)) { ListBoxCondition -> Items -> Add("Нет охлаждения ТМН"); }
            // есть охлаждение М1 zin
		    if(!(zin[0]&0x02)) { ListBoxCondition -> Items -> Add("Нет охлаждения М1"); }
            // есть охлаждение М2 zin
		    if(!(zin[0]&0x04)) { ListBoxCondition -> Items -> Add("Нет охлаждения М2"); }
            // есть связь с Д1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("Нет связи с Д1"); }
		    // есть связь с Д2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с Д2"); }
            // есть связь с Д3
		    //if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("Нет связи с Д3"); }
		    // есть связь с Д4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("Нет связи с Д4"); }
            // есть связь с Д5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Д5"); }
		    // есть связь с Д6
		    //if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("Нет связи с Д6"); }
            // есть связь с ДЗ
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("Нет связи с Дросельной заслонкой"); }
            // есть связь с БПМ1
		    if((nasmod[8])&&(diagnS[2]&0x04))   { ListBoxCondition -> Items -> Add("Нет связи с БПМ1"); }
            // есть связь с БПМ2
		    if((nasmod[9])&&(diagnS[2]&0x08))   { ListBoxCondition -> Items -> Add("Нет связи с БПМ2"); }
            // есть связь с КН
		    if((nasmod[19])&&(diagnS[2]&0x40))   { ListBoxCondition -> Items -> Add("Нет связи с КН"); }
            // не запущен ни один режим
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((i!=38)&&(shr[i])) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
        }; break;

		case 3:
		{   LblRejim -> Caption = "Рабочий цикл (РЦ)";
            // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
            // есть охлаждение КН по zin
		    if(!(zin[0]&0x08)&&(nasmod[19])) { ListBoxCondition -> Items -> Add("Нет охлаждения КН"); }
            // есть охлаждение ТМН zin
		    if(!(zin[0]&0x10)) { ListBoxCondition -> Items -> Add("Нет охлаждения ТМН"); }
            // есть связь с Д1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("Нет связи с Д1"); }
		    // есть связь с Д2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с Д2"); }
            // есть связь с Д3
		    //if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("Нет связи с Д3"); }
		    // есть связь с Д4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("Нет связи с Д4"); }
            // есть связь с Д5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Д5"); }
		    // есть связь с Д6
		    //if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("Нет связи с Д6"); }
            // есть связь с термодат
		    if((nasmod[4])&&(diagnS[2]&0x10))   { ListBoxCondition -> Items -> Add("Нет связи с Термодат"); }
            // есть связь с БПИИ
		    if((nasmod[7])&&(diagnS[2]&0x20))   { ListBoxCondition -> Items -> Add("Нет связи с БПИИ"); }
            // есть связь с БПМ1
		    if((nasmod[8])&&(diagnS[2]&0x04))   { ListBoxCondition -> Items -> Add("Нет связи с БПМ1"); }
            // есть связь с БПМ2
		    if((nasmod[9])&&(diagnS[2]&0x08))   { ListBoxCondition -> Items -> Add("Нет связи с БПМ2"); }
            // есть связь с КН
		    if((nasmod[19])&&(diagnS[2]&0x40))   { ListBoxCondition -> Items -> Add("Нет связи с КН"); }
            // не запущен ни один режим
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if((i!=38)&&(shr[i])) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
		}; break;
        case 5:  
        {
             LblRejim -> Caption = "Сброс РЦ";
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if(!shr[2]&&!shr[3])  { ListBoxCondition -> Items -> Add("Не запущен Техпроцесс"); }
		    // не запущен режим 5
		    if(shr[5])  { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
        }; break;
        case 6:   
		{   LblRejim -> Caption = "Сбор пластин";
            // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
            // есть связь с Д1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("Нет связи с Д1"); }
		    // есть связь с Д2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с Д2"); }
            // есть связь с Д3
		    //if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("Нет связи с Д3"); }
		    // есть связь с Д4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("Нет связи с Д4"); }
            // есть связь с Д5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Д5"); }
		    // есть связь с Д6
		    //if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("Нет связи с Д6"); }
            // есть связь с ДЗ
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("Нет связи с Дросельной заслонкой"); }
            // есть связь с с подъёмом п/д
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с контроллером подъёма п/д"); }
            // не запущен ни один режим
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if(shr[i]) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
		}; break;
       case 7:
		{   LblRejim -> Caption = "Отключение установки";
            // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
		    // не запущен ни один режим
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++) // не анализируются R_16(); R_29();R_31();R_38();
                if((i!=16)&&(i!=29)&&(i!=31)&&(i!=38))
                    if(shr[i]) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
		}; break;
         case 9:
		{   LblRejim -> Caption = "Включить транспортный тест";
            // ЩЗ открыт
		    if((zin[2]&0x3000)!=0x1000) { ListBoxCondition -> Items -> Add("Щелевой затвор не открыт"); }
            // Ман в HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("Манипулятор не в HOME"); }
            // подъём п/д в HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("Привод подъёма п/д не в HOME"); }
            // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
            // есть связь с с подъёмом п/д
		    if(diagnS[2]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с контроллером подъёма п/д"); }
            // не запущен ни один режим
		    for(unsigned char i=1;i<(SHR_NAMES_COUNT+1);i++)
                if(shr[i]) ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[i]);
		}; break;
        case 109:
		{   LblRejim -> Caption = "Отключить транспортный тест";
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		    // запущен режим  9
		    if(!shr[9]) { ListBoxCondition -> Items -> Add("Не запущен режим " + SHRNames[9]); }
		}; break;
       	case 10:
		{   LblRejim -> Caption = "Открыть щелевой затвор";
		    // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
            // есть связь с Д1
		    if(diagnS[0]&0x01)   { ListBoxCondition -> Items -> Add("Нет связи с Д1"); }
		    // есть связь с Д2
		    if(diagnS[0]&0x02)   { ListBoxCondition -> Items -> Add("Нет связи с Д2"); }
            // есть связь с Д3
		    //if(diagnS[0]&0x04)   { ListBoxCondition -> Items -> Add("Нет связи с Д3"); }
		    // есть связь с Д4
		    if(diagnS[0]&0x08)   { ListBoxCondition -> Items -> Add("Нет связи с Д4"); }
		    // есть связь с Д5
		    if(diagnS[0]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Д5"); }
            // есть связь с Д6
		    //if(diagnS[0]&0x20)   { ListBoxCondition -> Items -> Add("Нет связи с Д6"); }
       		    // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
		    // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
       		    // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
       		    // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
       		    // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
       		    // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
       		    // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
       		    // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
       		    // не запущен режим 10
		    if(shr[10]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[10]); }
		}; break;    
        case 11:
		{   LblRejim -> Caption = "Закрыть щелевой затвор";
            // Ман в HOME
		    if(!(zin[3]&0x200)) { ListBoxCondition -> Items -> Add("Манипулятор не в HOME"); }
            // есть давление в пневмосети по zin
		    if(!(zin[1]&0x10)) { ListBoxCondition -> Items -> Add("Нет давления в пневмосети"); }
                // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
		    // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
       		    // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
       		    // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
       		    // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
       		    // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
       		    // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
       		    // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
       		    // не запущен режим 11
		    if(shr[11]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[11]); }
		}; break;   
        case 12:
        {   LblRejim -> Caption = "Манипулятор в HOME";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
		    // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
       		    // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
       		    // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
       		    // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
       		    // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
       		    // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
       		    // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 11
		    if(shr[11]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[11]); }
            // не запущен режим 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]); }
            // не запущен режим 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
            // подъём п/д в HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("Привод подъёма п/д не в HOME"); }
        }; break;

        case 13:
        {   LblRejim -> Caption = "Манипулятор в камеру";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
		    // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
       		    // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
       		    // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
       		    // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
       		    // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
       		    // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
       		    // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 11
		    if(shr[11]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[11]); }
            // не запущен режим 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]); }
            // не запущен режим 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
            // подъём п/д в HOME
		    if(!(zin[3]&0x1000)) { ListBoxCondition -> Items -> Add("Привод подъёма п/д не в HOME"); }
            // ЩЗ открыт
		    if((zin[2]&0x3000)!=0x1000) { ListBoxCondition -> Items -> Add("Щелевой затвор не открыт"); }
        }; break;

		case 101:
		{   LblRejim -> Caption = "Стоп механизмов";  // STOP механизмов -- длинная красная кнопка
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////
       		    // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
		    // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
       		    // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
       		    // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
       		    // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
       		    // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
		    // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
        }; break;
        case 14:
        {   LblRejim -> Caption = "Подъём п/д в HOME";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]); }
            // не запущен режим 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]); }
            // не запущен режим 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
            // заслонка п/д не открыта
            if((zin[1]&0x300) != 0x100) ListBoxCondition -> Items -> Add("Заслонка п/д не открыта");
        }; break;
        case 15:
        {   LblRejim -> Caption = "Подъём п/д в вверх";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]); }
            // не запущен режим 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]); }
            // не запущен режим 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
            // заслонка п/д не открыта
            if((zin[1]&0x300) != 0x100) ListBoxCondition -> Items -> Add("Заслонка п/д не открыта");
        }; break;
        case 16:
        {   LblRejim -> Caption = "Вращение п/д вкл.";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]); }
            // не запущен режим 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
        }; break;
        case 17:
        {   LblRejim -> Caption = "Вращение п/д откл.";
            // запущен режим 16
		    if(!(shr[16])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[16]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

        }; break;
        case 18:
        {   LblRejim -> Caption = "Подъём п/д в рабочее положение (напыление)";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]); }
            // не запущен режим 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]); }
            // не запущен режим 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
            // заслонка п/д не открыта
            if((zin[1]&0x300) != 0x100) ListBoxCondition -> Items -> Add("Заслонка п/д не открыта");

        }; break;
        case 42:
        {   LblRejim -> Caption = "Подъём п/д в рабочее положение (ионная очистка)";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 12
		    if(shr[12]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[12]); }
            // не запущен режим 13
		    if(shr[13]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[13]); }
            // не запущен режим 14
		    if(shr[14]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[14]); }
            // не запущен режим 15
		    if(shr[15]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[15]); }
            // не запущен режим 16
		    if(shr[16]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[16]); }
            // не запущен режим 17
		    if(shr[17]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[17]); }
            // не запущен режим 18
		    if(shr[18]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[18]); }
            // не запущен режим 42
		    if(shr[42]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[42]); }
            // заслонка п/д не открыта
            if((zin[1]&0x300) != 0x100) ListBoxCondition -> Items -> Add("Заслонка п/д не открыта");

        }; break;
        case 20:
        {   LblRejim -> Caption = "РРГ1 вкл.";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 20
		    if(shr[20]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]); }
            // не запущен режим 24
		    if(shr[24]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]); }
        }; break;
        case 120:
        {   LblRejim -> Caption = "РРГ1 откл.";
            // запущен режим 20
		    if(!(shr[20])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[20]); }

            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

        }; break;
        case 21:
        {   LblRejim -> Caption = "РРГ2 вкл.";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 21
		    if(shr[21]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[21]); }
        }; break;
        case 121:
        {   LblRejim -> Caption = "РРГ2 откл.";
            // запущен режим 21
		    if(!(shr[21])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[21]); }

            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

        }; break;
        case 22:
        {   LblRejim -> Caption = "РРГ3 вкл.";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 22
		    if(shr[22]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[22]); }
        }; break;
        case 122:
        {   LblRejim -> Caption = "РРГ3 откл.";
            // запущен режим 22
		    if(!(shr[22])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[22]); }

            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

        }; break;
        case 23:
        {   LblRejim -> Caption = "РРГ4 вкл. в камеру";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
        }; break;
        case 223:
        {   LblRejim -> Caption = "РРГ4 вкл. в насос";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
        }; break;
        case 123:
        {   LblRejim -> Caption = "РРГ4 откл.";
            // запущен режим 23
		    if(!(shr[23])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[23]); }

            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

        }; break;
        case 24:
        {   LblRejim -> Caption = "УУН вкл.";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 20
		    if(shr[20]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[20]); }
            // не запущен режим 24
		    if(shr[24]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[24]); }
        }; break;
        case 124:
        {   LblRejim -> Caption = "УУН откл.";
            // запущен режим 22
		    if(!(shr[24])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[24]); }

            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

        }; break;
        case 25:
        {   LblRejim -> Caption = "Открыть ДЗ";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 25
		    if(shr[25]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[25]); }
        }; break;
        case 26:
        {   LblRejim -> Caption = "Закрыть ДЗ";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 26
		    if(shr[26]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[26]); }
        }; break;
        case 27:
        {   LblRejim -> Caption = "ДЗ на угол";
            // есть связь с ДЗ
		    if(diagnS[0]&0x40)   { ListBoxCondition -> Items -> Add("Нет связи с Дросельной заслонкой"); }
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 27
		    if(shr[27]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[27]); }
        }; break;
        case 28:
        {   LblRejim -> Caption = "ВЧГ п/д вкл.";

            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 28
		    if(shr[28]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[28]); }
        }; break;
        case 128:
        {   LblRejim -> Caption = "ВЧГ п/д откл.";


            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }

            // запущен режим 28
		    if(!(shr[28])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[28]); }
        }; break;
        case 29:
        {   LblRejim -> Caption = "Нагрев камеры вкл.";
            // не запущен режим 29
		    if(shr[29]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[29]); }
            // есть связь с термодат
		    if(diagnS[2]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Термодат"); }
            //есть разрешение
            if(!(nasmod[4])){ ListBoxCondition -> Items -> Add("Нет разрешения нагрева" ); }

        }; break;
        case 30:
        {   LblRejim -> Caption = "Нагрев камеры откл.";
            // запущен режим 29
		    if(!(shr[29])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[29]); }

        }; break;
        case 31:
        {   LblRejim -> Caption = "Нагрев п/д вкл.";
            // запущен режим 31
		    if(shr[31]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[31]); }
            //есть разрешение нагрева п/д
            if(nasmod[6]!=1){ ListBoxCondition -> Items -> Add("Нет разрешения нагрева п/д");  }
            // есть связь с термодат
		    if(diagnS[2]&0x10)   { ListBoxCondition -> Items -> Add("Нет связи с Термодат"); }
            //есть разрешение
            if(!(nasmod[4])){ ListBoxCondition -> Items -> Add("Нет разрешения нагрева" ); }
        }; break;
        case 32:
        {   LblRejim -> Caption = "Нагрев п/д откл.";
            // не запущен режим 31
		    if(!(shr[31])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[31]); }
        }; break;
        case 33:
        {   LblRejim -> Caption = "БПИИ вкл.";
            // есть связь с БПИИ
		    if(diagnS[2]&0x20)   { ListBoxCondition -> Items -> Add("Нет связи с БПИИ"); }
            //есть разрешение
            if(!(nasmod[7]))     { ListBoxCondition -> Items -> Add("Нет разрешения для работы с БПИИ"); }
            //есть охлаждение ИИ
		    if(!(zin[0]&0x01)) { ListBoxCondition -> Items -> Add("Нет охлаждения ИИ"); }
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 33
		    if(shr[33]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[33]); }
        }; break;
        case 34:
        {   LblRejim -> Caption = "БПИИ откл.";

            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // запущен режим 33
		    if(!(shr[33])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[33]); }
        }; break;
        case 35:
        {   LblRejim -> Caption = "БПМ1 вкл.";
            // есть связь с БПМ1
		    if(diagnS[2]&0x04)   { ListBoxCondition -> Items -> Add("Нет связи с БПМ1"); }
            //есть разрешение
            if(!(nasmod[8]))     { ListBoxCondition -> Items -> Add("Нет разрешения для работы с БПМ1"); }
            //есть охлаждение М1
		    if(!(zin[0]&0x02)) { ListBoxCondition -> Items -> Add("Нет охлаждения М1"); }
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 35
		    if(shr[35]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[35]); }
            // не запущен режим 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[37]); }
        }; break;
        case 36:
        {   LblRejim -> Caption = "БПМ2 вкл.";
            // есть связь с БПМ2
		    if(diagnS[2]&0x08)   { ListBoxCondition -> Items -> Add("Нет связи с БПМ2"); }
            //есть разрешение
            if(!(nasmod[9]))     { ListBoxCondition -> Items -> Add("Нет разрешения для работы с БПМ2"); }
            //есть охлаждение М1
		    if(!(zin[0]&0x04)) { ListBoxCondition -> Items -> Add("Нет охлаждения М2"); }
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 36
		    if(shr[36]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[36]); }
            // не запущен режим 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[37]); }
        }; break;
        case 37:
        {   LblRejim -> Caption = "Сброс БПМов";

            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3 или запущен в наладке
		    if((shr[3])&&(!PR_NALADKA)) { ListBoxCondition -> Items -> Add("Наладочный режим запрещен"); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 37
		    if(shr[37]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[37]); }
            // запущен хотя бы один из режимов бпм
            if((!(shr[35]))&&(!(shr[36]))) { ListBoxCondition -> Items -> Add("Не запущен ни один из режимов БПМ " ); }
        }; break;
        case 38:
        {   LblRejim -> Caption = "Чиллер п/д вкл.";

            // не запущен режим 38
		    if(shr[38]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[38]); }
            //есть разрешение нагрева п/д
            if(nasmod[6]!=2){ ListBoxCondition -> Items -> Add("Нет разрешения включения чиллера п/д");  }

        }; break;
        case 39:
        {   LblRejim -> Caption = "Чиллер п/д откл.";

            // запущен режим 38
		    if(!(shr[38])) { ListBoxCondition -> Items -> Add("Не запущен режим: " + SHRNames[38]); }


        }; break;
        case 110:
        {   LblRejim -> Caption = "Чиллер вкл.";
            if(!(zin[2]&0x40)) { ListBoxCondition -> Items -> Add("Нет удалённого доступа чиллером"); }
        }; break;
        case 111:
        {   LblRejim -> Caption = "Чиллер откл.";
            if(!(zin[2]&0x40)) { ListBoxCondition -> Items -> Add("Нет удалённого доступа чиллером"); }
        }; break;
        case 40:
        {   LblRejim -> Caption = "Пуск КН";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[40]); }
        }; break;
        case 41:
        {   LblRejim -> Caption = "Стоп КН";
            // не запущен режим 1
		    if(shr[1]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[1]); }
            // не запущен режим 2
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 3
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }
            // не запущен режим 4
		    if(shr[4]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[4]); }
            // не запущен режим 5
		    if(shr[5]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[5]); }
            // не запущен режим 6
		    if(shr[6]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[6]); }
            // не запущен режим 7
		    if(shr[7]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[7]); }
            // не запущен режим 8
		    if(shr[8]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[8]); }
            // не запущен режим 9
		    if(shr[9]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[9]); }
            // не запущен режим 40
		    if(shr[40]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[40]); }
        }; break;
        case 213:
        {   LblRejim -> Caption = "Сброс аварии механизмов";

        }; break;
        
        case 100:
        {   LblRejim -> Caption = "Общий сброс";
            // не запущен режим 1
		    if(shr[2]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[2]); }
            // не запущен режим 1
		    if(shr[3]) { ListBoxCondition -> Items -> Add("Запущен режим: " + SHRNames[3]); }

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
        MemoStat -> Lines -> Add( Label_Date -> Caption + " " + Label_Time->Caption + " : Запущен режим: <" + LblRejim -> Caption + ">" );
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
//-- Работа с COM-портами --//
//---------------------------------------------------------------------------
void Timer_Com1()
{
   // return; /

    try
    {
        if(Comport[0]->port_err)
        {
            if(Comport[0]->port_ct > 10)
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
    //return;

    try
    {
        if(Comport[1]->port_err)
        {
            if(Comport[1]->port_ct > 10)
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

        // Есть ручное задание
        if(!(Comport[1]->PortTask))
        {
        if(nasmod[8]) // включать БПМ1
        {
	        if((BU_IVE[0]->RCom)&&(!(Comport[1]->DevState))) Comport[1]->PortTask |= 0x100;
	        if(!(Comport[1]->Pr_nal)) Comport[1]->PortTask |= 0x01; // Обновляем автоматическое задание
        }
        else
        {
            Comport[1]->PortTask &= (~0x101);
            diagnS[BU_IVE[0]->diagnS_byte] &= (~BU_IVE[0]->diagnS_mask);
        }
        if(nasmod[9]) // включать БПМ2
        {
	        if((BU_IVE[1]->RCom)&&(!(Comport[1]->DevState))) Comport[1]->PortTask |= 0x200;
	        if(!(Comport[1]->Pr_nal)) Comport[1]->PortTask |= 0x02; // Обновляем автоматическое задание
        }
        else
        {
            Comport[1]->PortTask &= (~0x202);
            diagnS[BU_IVE[1]->diagnS_byte] &= (~BU_IVE[1]->diagnS_mask);
        }
        if(nasmod[7]) // включать БПИИ1
        {
	        if((BU_IVE[2]->RCom)&&(!(Comport[1]->DevState))) Comport[1]->PortTask |= 0x400;
	        if(!(Comport[1]->Pr_nal)) Comport[1]->PortTask |= 0x04; // Обновляем автоматическое задание
        }
        else
        {
            Comport[1]->PortTask &= (~0x404);
            diagnS[BU_IVE[2]->diagnS_byte] &= (~BU_IVE[2]->diagnS_mask);
        }
        }

        if((Comport[1]->PortTask) & 0x100)
	    {
		    Comport[1]->DevErr = BU_IVE[0]->BU_IVE_Manage(Comport[1]->DevState,1);
		    if((Comport[1]->DevState) > 1)
		    {
			    (Comport[1]->DevErr) ? diagnS[BU_IVE[0]->diagnS_byte] |= BU_IVE[0]->diagnS_mask : diagnS[BU_IVE[0]->diagnS_byte] &= (~BU_IVE[0]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x100);
			    BU_IVE[0]->RCom = 0;
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
	    else if((Comport[1]->PortTask)&0x01)
	    {
		    Comport[1]->DevErr = BU_IVE[0]->BU_IVE_Manage(Comport[1]->DevState,0);
		    if((Comport[1]->DevState)>1)
		    {
			    (Comport[1]->DevErr) ? diagnS[BU_IVE[0]->diagnS_byte] |= BU_IVE[0]->diagnS_mask : diagnS[BU_IVE[0]->diagnS_byte] &= (~BU_IVE[0]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x01);
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
        else if((Comport[1]->PortTask) & 0x200)
	    {
		    Comport[1]->DevErr = BU_IVE[1]->BU_IVE_Manage(Comport[1]->DevState,1);
		    if((Comport[1]->DevState) > 1)
		    {
			    (Comport[1]->DevErr) ? diagnS[BU_IVE[1]->diagnS_byte] |= BU_IVE[1]->diagnS_mask : diagnS[BU_IVE[1]->diagnS_byte] &= (~BU_IVE[1]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x200);
			    BU_IVE[1]->RCom = 0;
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
	    else if((Comport[1]->PortTask)&0x02)
	    {
		    Comport[1]->DevErr = BU_IVE[1]->BU_IVE_Manage(Comport[1]->DevState,0);
		    if((Comport[1]->DevState)>1)
		    {
			    (Comport[1]->DevErr) ? diagnS[BU_IVE[1]->diagnS_byte] |= BU_IVE[1]->diagnS_mask : diagnS[BU_IVE[1]->diagnS_byte] &= (~BU_IVE[1]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x02);
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
        else if((Comport[1]->PortTask) & 0x400)
	    {
		    Comport[1]->DevErr = BU_IVE[2]->BU_IVE_Manage(Comport[1]->DevState,1);
		    if((Comport[1]->DevState) > 1)
		    {
			    (Comport[1]->DevErr) ? diagnS[BU_IVE[2]->diagnS_byte] |= BU_IVE[2]->diagnS_mask : diagnS[BU_IVE[2]->diagnS_byte] &= (~BU_IVE[2]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x400);
			    BU_IVE[2]->RCom = 0;
			    Comport[1]->DevState = 0;
		    }
		    return;
	    }
	    else if((Comport[1]->PortTask)&0x04)
	    {
		    Comport[1]->DevErr = BU_IVE[2]->BU_IVE_Manage(Comport[1]->DevState,0);
		    if((Comport[1]->DevState)>1)
		    {
			    (Comport[1]->DevErr) ? diagnS[BU_IVE[2]->diagnS_byte] |= BU_IVE[2]->diagnS_mask : diagnS[BU_IVE[2]->diagnS_byte] &= (~BU_IVE[2]->diagnS_mask);
			    (Comport[1]->PortTask) &= (~0x04);
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
            if(Comport[2]->port_ct > 10)
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

	    if(!(Comport[2]->PortTask)&&!(Comport[2]->Pr_nal)) Comport[2]->PortTask |= 0x1f; // Обновляем автоматическое задание

        if((Comport[2]->PortTask)&0x01)
	    {
		    Comport[2]->DevErr = Dat_PPT200[0]->DatPPT200_Manage(Comport[2]->DevState,0);
		    if((Comport[2]->DevState) > 1)
		    {
			    (Comport[2]->DevErr) ? diagnS[Dat_PPT200[0]->diagnS_byte] |= Dat_PPT200[0]->diagnS_mask : diagnS[Dat_PPT200[0]->diagnS_byte] &= (~Dat_PPT200[0]->diagnS_mask);
			    (Comport[2]->PortTask) &= (~0x01);
			    Comport[2]->DevState = 0;
		    }
		    return;
	    }
        else if((Comport[2]->PortTask)&0x02)
	    {
		    Comport[2]->DevErr = Dat_PPT200[1]->DatPPT200_Manage(Comport[2]->DevState,0);
		    if((Comport[2]->DevState) > 1)
		    {
			    (Comport[2]->DevErr) ? diagnS[Dat_PPT200[1]->diagnS_byte] |= Dat_PPT200[1]->diagnS_mask : diagnS[Dat_PPT200[1]->diagnS_byte] &= (~Dat_PPT200[1]->diagnS_mask);
			    (Comport[2]->PortTask) &= (~0x02);
			    Comport[2]->DevState = 0;
		    }
		    return;
	    }
        else if((Comport[2]->PortTask)&0x04)
	    {
		    Comport[2]->DevErr = Dat_PPT200[2]->DatPPT200_Manage(Comport[2]->DevState,0);
		    if((Comport[2]->DevState) > 1)
		    {
			    (Comport[2]->DevErr) ? diagnS[Dat_PPT200[2]->diagnS_byte] |= Dat_PPT200[2]->diagnS_mask : diagnS[Dat_PPT200[2]->diagnS_byte] &= (~Dat_PPT200[2]->diagnS_mask);
			    (Comport[2]->PortTask) &= (~0x04);
			    Comport[2]->DevState = 0;
		    }
		    return;
	    }
	    else if((Comport[2]->PortTask)&0x08)
	    {
		    Comport[2]->DevErr = Dat_PPT200[3]->DatPPT200_Manage(Comport[2]->DevState,0);
		    if((Comport[2]->DevState)>1)
		    {
			    (Comport[2]->DevErr) ? diagnS[Dat_PPT200[3]->diagnS_byte] |= Dat_PPT200[3]->diagnS_mask : diagnS[Dat_PPT200[3]->diagnS_byte] &= (~Dat_PPT200[3]->diagnS_mask);
			    (Comport[2]->PortTask) &= (~0x08);
			    Comport[2]->DevState = 0;
		    }
		    return;
	    }
        else if((Comport[2]->PortTask)&0x10)
	    {
		    Comport[2]->DevErr = Dat_MPT200[0]->DatMPT200_Manage(Comport[2]->DevState,0);
		    if((Comport[2]->DevState)>1)
		    {
			    (Comport[2]->DevErr) ? diagnS[Dat_MPT200[0]->diagnS_byte] |= Dat_MPT200[0]->diagnS_mask : diagnS[Dat_MPT200[0]->diagnS_byte] &= (~Dat_MPT200[0]->diagnS_mask);
			    (Comport[2]->PortTask) &= (~0x10);
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
            if(Comport[3]->port_ct > 10)
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

        if(!(Comport[3]->PortTask)&&!(Comport[3]->Pr_nal)) Comport[3]->PortTask |= 0x03; // Обновляем автоматическое задание

        if((Comport[3]->PortTask)&0x01)
	    {
		    Comport[3]->DevErr = Dat_PPT200[4]->DatPPT200_Manage(Comport[3]->DevState,0);
		    if((Comport[3]->DevState) > 1)
		    {
			    (Comport[3]->DevErr) ? diagnS[Dat_PPT200[4]->diagnS_byte] |= Dat_PPT200[4]->diagnS_mask : diagnS[Dat_PPT200[4]->diagnS_byte] &= (~Dat_PPT200[4]->diagnS_mask);
			    (Comport[3]->PortTask) &= (~0x01);
			    Comport[3]->DevState = 0;
		    }
		    return;
	    }
        else if((Comport[3]->PortTask)&0x02)
	    {
		    Comport[3]->DevErr = TRMD[0]->TRMD_Manage(Comport[3]->DevState,0);
		    if((Comport[3]->DevState) > 1)
		    {
			    (Comport[3]->DevErr) ? diagnS[TRMD[0]->diagnS_byte] |= TRMD[0]->diagnS_mask : diagnS[TRMD[0]->diagnS_byte] &= (~TRMD[0]->diagnS_mask);
			    (Comport[3]->PortTask) &= (~0x02);
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
            // ShowMessage("Обнаружена ошибка. Com4 отключен!");
        }
        return;
    }
}
//---------------------------------------------------------------------------
void Timer_Com5()
{
    //return;

    try
    {
        if(Comport[4]->port_err)
        {
            if(Comport[4]->port_ct > 10)
            {
                if(Comport[4]->Port.Open(Comport[4]->PortName.c_str(),Comport[4]->B_Rate,Data8Bit,Comport[4]->P_Rate,OneStopBit))
                {
                    Comport[4]->State = 1;
                    Comport[4]->BTN_reset->Caption = "Стоп порта";
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

	    // Установка загружена и порт включен
	    if(!(Comport[4]->State)||!ust_ready) return;

	    // Отображение приема/передачи
	    Comport[4]->RB_prd->Checked = !(Comport[4]->DevState);
	    Comport[4]->RB_prm->Checked = Comport[4]->DevState;

	    if(!(Comport[4]->PortTask)&&!(Comport[4]->Pr_nal)) Comport[4]->PortTask |= 0x01; // Обновляем автоматическое задание

	    if((Comport[4]->PortTask)&0x01)
	    {
		    Comport[4]->DevErr = AZ_drive[0]->AZ_manage(Comport[4]->DevState);
		    if((Comport[4]->DevState) > 1)
            {
			    (Comport[4]->DevErr) ? diagnS[AZ_drive[0]->diagnS_byte] |= AZ_drive[0]->diagnS_mask : diagnS[AZ_drive[0]->diagnS_byte] &= (~AZ_drive[0]->diagnS_mask);
			    (Comport[4]->PortTask) &= (~0x01);
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
			            Comport[4]->BTN_reset->Caption = "Пуск порта";
                        Comport[4]->port_ct = 0;
                }
                // ShowMessage("Обнаружена ошибка. Com5 отключен!");
        }
        return;
    }
}
//---------------------------------------------------------------------------
void Timer_Com6()
{
    //return;

    try
    {
        if(Comport[5]->port_err)
        {
            if(Comport[5]->port_ct > 10)
            {
                if(Comport[5]->Port.Open(Comport[5]->PortName.c_str(),Comport[5]->B_Rate,Data8Bit,Comport[5]->P_Rate,OneStopBit))
                {
                    Comport[5]->State = 1;
                    Comport[5]->BTN_reset->Caption = "Стоп порта";
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

	    // Установка загружена и порт включен
	    if(!(Comport[5]->State)||!ust_ready) return;

	    // Отображение приема/передачи
	    Comport[5]->RB_prd->Checked = !(Comport[5]->DevState);
	    Comport[5]->RB_prm->Checked = Comport[5]->DevState;

        // Есть ручное задание
	    if((KNOmsk[0]->RCom)&&(!Comport[5]->DevState)) Comport[5]->PortTask |= 0x100; //
	    if((!Comport[5]->PortTask) && (!Comport[5]->Pr_nal)) Comport[5]->PortTask |= 0x01; // Обновляем автоматическое задание

	    if(Comport[5]->PortTask & 0x100)
	    {
		    Comport[5]->DevErr = KNOmsk[0]->KNOmsk_Manage(Comport[5]->DevState,1);
		    if(Comport[5]->DevState > 1)
		    {
			    Comport[5]->DevErr ? diagnS[KNOmsk[0]->diagnS_byte] |= KNOmsk[0]->diagnS_mask : diagnS[KNOmsk[0]->diagnS_byte] &= (~KNOmsk[0]->diagnS_mask);
			    Comport[5]->PortTask &= (~0x100);
			    KNOmsk[0]->RCom = 0;
		    	Comport[5]->DevState = 0;
		    }
		    return;
	    }
	    else if(Comport[5]->PortTask & 0x01)
	    {
		    Comport[5]->DevErr = KNOmsk[0]->KNOmsk_Manage(Comport[5]->DevState,0);
		    if(Comport[5]->DevState > 1)
		    {
			    Comport[5]->DevErr ? diagnS[KNOmsk[0]->diagnS_byte] |= KNOmsk[0]->diagnS_mask : diagnS[KNOmsk[0]->diagnS_byte] &= (~KNOmsk[0]->diagnS_mask);
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
                Comport[5]->BTN_reset->Caption = "Пуск порта";
                Comport[5]->port_ct = 0;
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
    if(((TTimer*)Sender)->Name == "ComTimer5") Timer_Com5();
    if(((TTimer*)Sender)->Name == "ComTimer6") Timer_Com6();
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
		case 2 :sh[1]       = StrToInt(EditOTLzad2->Text) ; break;
		case 3 :shr[1]      = StrToInt(EditOTLzad3->Text) ; break;
		case 4 :sh[2]       = StrToInt(EditOTLzad4->Text) ; break;
		case 5 :shr[2]      = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :sh[3]       = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :shr[3]      = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :sh[4]       = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :shr[4]      = StrToInt(EditOTLzad9->Text) ; break;
		case 10:sh[5]       = StrToInt(EditOTLzad10->Text); break;
		case 11:shr[5]      = StrToInt(EditOTLzad11->Text); break;
		case 12:sh[6]       = StrToInt(EditOTLzad12->Text); break;
		case 13:shr[6]      = StrToInt(EditOTLzad13->Text); break;
		case 14:sh[7]       = StrToInt(EditOTLzad14->Text); break;
		case 15:shr[7]      = StrToInt(EditOTLzad15->Text); break;
		case 16:sh[8]       = StrToInt(EditOTLzad16->Text); break;
		case 17:shr[8]      = StrToInt(EditOTLzad17->Text); break;
		case 18:sh[9]       = StrToInt(EditOTLzad18->Text); break;
		case 19:shr[9]      = StrToInt(EditOTLzad19->Text); break;
		case 20:sh[10]      = StrToInt(EditOTLzad20->Text); break;
		case 21:shr[10]     = StrToInt(EditOTLzad21->Text); break;
		case 22:sh[11]      = StrToInt(EditOTLzad22->Text); break;
		case 23:shr[11]     = StrToInt(EditOTLzad23->Text); break;
		case 24:sh[12]      = StrToInt(EditOTLzad24->Text); break;
		case 25:shr[12]     = StrToInt(EditOTLzad25->Text); break;
		case 26:sh[13]      = StrToInt(EditOTLzad26->Text); break;
		case 27:shr[13]     = StrToInt(EditOTLzad27->Text); break;
		case 28:sh[14]      = StrToInt(EditOTLzad28->Text); break;
		case 29:shr[14]     = StrToInt(EditOTLzad29->Text); break;
//		case 30:            = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //1 страница
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
 //2 страница
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
		case 30:diagn[29]   = StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //4 страница
case 4:
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
//		case 13:= StrToInt(EditOTLzad13->Text); break;
		case 14:aik[0]      = StrToInt(EditOTLzad14->Text); break;
		case 15:aik[1]      = StrToInt(EditOTLzad15->Text); break;
		case 16:aik[2]      = StrToInt(EditOTLzad16->Text); break;
		case 17:aik[3]      = StrToInt(EditOTLzad17->Text); break;
		case 18:aik[4]      = StrToInt(EditOTLzad18->Text); break;
		case 19:aik[5]      = StrToInt(EditOTLzad19->Text); break;
		case 20:aik[6]      = StrToInt(EditOTLzad20->Text); break;
		case 21:aik[7]      = StrToInt(EditOTLzad21->Text); break;
		case 22:aik[8]      = StrToInt(EditOTLzad22->Text); break;
		case 23:aik[9]      = StrToInt(EditOTLzad23->Text); break;
		case 24:aik[10]     = StrToInt(EditOTLzad24->Text); break;
		case 25:aik[11]     = StrToInt(EditOTLzad25->Text); break;
		case 26:aik[12]     = StrToInt(EditOTLzad26->Text); break;
		case 27:aik[13]     = StrToInt(EditOTLzad27->Text); break;
		case 28:aik[14]     = StrToInt(EditOTLzad28->Text); break;
		case 29:aik[15]     = StrToInt(EditOTLzad29->Text); break;
	   //	case 30:aik[16]     = StrToInt(EditOTLzad30->Text); break;
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
		case 5 :aout[4]     = StrToInt(EditOTLzad5->Text) ; break;
		case 6 :aout[5]     = StrToInt(EditOTLzad6->Text) ; break;
		case 7 :aout[6]     = StrToInt(EditOTLzad7->Text) ; break;
		case 8 :aout[7]     = StrToInt(EditOTLzad8->Text) ; break;
		case 9 :aout[8]     = StrToInt(EditOTLzad9->Text) ; break;
//		case 10:= StrToInt(EditOTLzad10->Text); break;
		case 11:D_D1        = StrToInt(EditOTLzad11->Text); break;
		case 12:D_D2        = StrToInt(EditOTLzad12->Text); break;
		case 13:D_D3        = StrToInt(EditOTLzad13->Text); break;
		case 14:D_D4        = StrToInt(EditOTLzad14->Text); break;
		case 15:D_D5        = StrToInt(EditOTLzad15->Text); break;
		case 16:D_D6        = StrToInt(EditOTLzad16->Text); break;
//		case 17:= StrToInt(EditOTLzad17->Text); break;
		case 18:UVAK_KN     = StrToInt(EditOTLzad18->Text); break;
		case 19:UVAKN_TMN   = StrToInt(EditOTLzad19->Text); break;
		case 20:UVAKV_TMN   = StrToInt(EditOTLzad20->Text); break;
		case 21:UVAK_KAM    = StrToInt(EditOTLzad21->Text); break;
		case 22:UATM_D1     = StrToInt(EditOTLzad22->Text); break;
		case 23:UVAK_SHL    = StrToInt(EditOTLzad23->Text); break;
		case 24:UATM_D4     = StrToInt(EditOTLzad24->Text); break;
		case 25:UVAK_SHL_MO = StrToInt(EditOTLzad25->Text); break;
		case 26:POROG_DAVL  = StrToInt(EditOTLzad26->Text); break;
		case 27:UVAK_ZTMN   = StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
//		case 30:= StrToInt(EditOTLzad30->Text); break;
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
		case 18:nasmod[17]  = StrToInt(EditOTLzad18->Text); break;
		case 19:nasmod[18]  = StrToInt(EditOTLzad19->Text); break;
		case 20:nasmod[19]  = StrToInt(EditOTLzad20->Text); break;
		case 21:nasmod[20]  = StrToInt(EditOTLzad21->Text); break;
		case 22:nasmod[21]  = StrToInt(EditOTLzad22->Text); break;
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
     //10 страница
case 10:
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
     //11 страница
case 11:
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
     //12 страница
case 12:
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
     //13 страница
case 13:
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
     //14 страница
case 14:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[6][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[6][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[6][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[6][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[6][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[6][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[6][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[6][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[6][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[6][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[6][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[6][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[6][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[6][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[6][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[6][15]= StrToInt(EditOTLzad16->Text); break;
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
     //15 страница
case 15:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[7][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[7][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[7][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[7][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[7][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[7][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[7][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[7][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[7][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[7][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[7][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[7][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[7][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[7][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[7][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[7][15]= StrToInt(EditOTLzad16->Text); break;
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
     //16 страница
case 16:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[8][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[8][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[8][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[8][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[8][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[8][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[8][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[8][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[8][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[8][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[8][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[8][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[8][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[8][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[8][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[8][15]= StrToInt(EditOTLzad16->Text); break;
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
     //17 страница
case 17:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[9][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[9][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[9][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[9][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[9][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[9][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[9][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[9][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[9][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[9][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[9][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[9][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[9][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[9][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[9][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[9][15]= StrToInt(EditOTLzad16->Text); break;
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
     //18 страница
case 18:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[10][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[10][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[10][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[10][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[10][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[10][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[10][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[10][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[10][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[10][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[10][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[10][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[10][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[10][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[10][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[10][15]= StrToInt(EditOTLzad16->Text); break;
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
     //19 страница
case 19:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[11][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[11][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[11][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[11][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[11][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[11][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[11][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[11][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[11][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[11][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[11][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[11][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[11][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[11][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[11][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[11][15]= StrToInt(EditOTLzad16->Text); break;
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
     //20 страница
case 20:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :par[12][0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :par[12][1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :par[12][2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :par[12][3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :par[12][4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :par[12][5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :par[12][6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :par[12][7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :par[12][8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:par[12][9]= StrToInt(EditOTLzad10->Text); break;
		case 11:par[12][10]= StrToInt(EditOTLzad11->Text); break;
		case 12:par[12][11]= StrToInt(EditOTLzad12->Text); break;
		case 13:par[12][12]= StrToInt(EditOTLzad13->Text); break;
		case 14:par[12][13]= StrToInt(EditOTLzad14->Text); break;
		case 15:par[12][14]= StrToInt(EditOTLzad15->Text); break;
		case 16:par[12][15]= StrToInt(EditOTLzad16->Text); break;
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
     //21 страница
case 21:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :CT_T1= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :CT_T20= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :CT_1= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :CT_2= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :CT_3= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :CT_4= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :CT_5= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :CT_6= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :CT_7= StrToInt(EditOTLzad9->Text) ; break;
		case 10:CT_9= StrToInt(EditOTLzad10->Text); break;
		case 11:CT_19= StrToInt(EditOTLzad11->Text); break;
		case 12:CT_24= StrToInt(EditOTLzad12->Text); break;
		case 13:CT24K1= StrToInt(EditOTLzad13->Text); break;
		case 14:CT_27= StrToInt(EditOTLzad14->Text); break;
		case 15:CT_28= StrToInt(EditOTLzad15->Text); break;
		case 16:CT28K1= StrToInt(EditOTLzad16->Text); break;
		case 17:CT_29= StrToInt(EditOTLzad17->Text); break;
		case 18:CT29K1= StrToInt(EditOTLzad18->Text); break;
		case 19:CT_31= StrToInt(EditOTLzad19->Text); break;
		case 20:CT31K1= StrToInt(EditOTLzad20->Text); break;
		case 21:CT_33= StrToInt(EditOTLzad21->Text); break;
		case 22:CT33K1= StrToInt(EditOTLzad22->Text); break;
		case 23:CT_35= StrToInt(EditOTLzad23->Text); break;
		case 24:CT35K1= StrToInt(EditOTLzad24->Text); break;
		case 25:CT_36= StrToInt(EditOTLzad25->Text); break;
		case 26:CT36K1= StrToInt(EditOTLzad26->Text); break;
		case 27:CT_38= StrToInt(EditOTLzad27->Text); break;
		case 28:CT_39= StrToInt(EditOTLzad28->Text); break;
		case 29:CT_40= StrToInt(EditOTLzad29->Text); break;
		case 30:CT_41= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //22 страница
case 22:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :CT_KN= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :CT_VRUN= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :CT_PR_UN= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :CT_REQUN= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :CT_II= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :CT_VODA_BM1= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :CT_VODA_BM2= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :CT_VODA_II= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :CT_KZ1= StrToInt(EditOTLzad9->Text) ; break;
		case 10:CT_KZ2= StrToInt(EditOTLzad10->Text); break;
		case 11:ctPderjDvij= StrToInt(EditOTLzad11->Text); break;
		case 12:CT_PER= StrToInt(EditOTLzad12->Text); break;
		case 13:CT_POD= StrToInt(EditOTLzad13->Text); break;
		case 14:CT_DZASL= StrToInt(EditOTLzad14->Text); break;
		case 15:CT_TEMP1= StrToInt(EditOTLzad15->Text); break;
		case 16:CT_TEMP2= StrToInt(EditOTLzad16->Text); break;
		case 17:CT_VHG= StrToInt(EditOTLzad17->Text); break;
		case 18:CT_IST= StrToInt(EditOTLzad18->Text); break;
		case 19:CT_BM1= StrToInt(EditOTLzad19->Text); break;
		case 20:CT_BM2= StrToInt(EditOTLzad20->Text); break;
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
     //23 страница
case 23:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :T_K_KN= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :T_KTMN= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :T_KTMN_RAZGON= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :T_KKAM_V= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :T_OTK_KN= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :T_PROC= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :T_KNAP= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :T_NAPUSK= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :T_SBROSHE= StrToInt(EditOTLzad9->Text) ; break;
		case 10:T_DVIJ= StrToInt(EditOTLzad10->Text); break;
		case 11:T_KSHL_MO= StrToInt(EditOTLzad11->Text); break;
		case 12:T_KSHL= StrToInt(EditOTLzad12->Text); break;
		case 13:T_VPRB_UN= StrToInt(EditOTLzad13->Text); break;
		case 14:T_VREJ_UN= StrToInt(EditOTLzad14->Text); break;
		case 15:T_VRUN= StrToInt(EditOTLzad15->Text); break;
		case 16:T_KUN= StrToInt(EditOTLzad16->Text); break;
		case 17:T_REQUN= StrToInt(EditOTLzad17->Text); break;
	 //	case 18:= StrToInt(EditOTLzad18->Text); break;
		case 19:T_VRGIS= StrToInt(EditOTLzad19->Text); break;
		case 20:T_KGIS= StrToInt(EditOTLzad20->Text); break;
		case 21:T_VKL_BPN= StrToInt(EditOTLzad21->Text); break;
		case 22:T_VRTEMP= StrToInt(EditOTLzad22->Text); break;
		case 23:T_KTEMP= StrToInt(EditOTLzad23->Text); break;
		case 24:T_VRII= StrToInt(EditOTLzad24->Text); break;
		case 25:T_KII= StrToInt(EditOTLzad25->Text); break;
		case 26:T_VRBM= StrToInt(EditOTLzad26->Text); break;
		case 27:T_KBM= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
//		case 29:= StrToInt(EditOTLzad29->Text); break;
		case 30:T_K_KAM= StrToInt(EditOTLzad30->Text); break;
	}
}; break;

     //24 страница
case 24:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_TRTEST= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :PR_RG4= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_OTP= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :PR_NALADKA= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :PR_TREN= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :PR_PER= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :PR_POD= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :N_ST= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :otvet= StrToInt(EditOTLzad9->Text) ; break;
		case 10:N_ST_MAX= StrToInt(EditOTLzad10->Text); break;
		case 11:DOPUSK_VENT= StrToInt(EditOTLzad11->Text); break;
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
     //25 страница
case 25:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_DZASL= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_DZASL= StrToInt(EditOTLzad2->Text) ; break;
//		case 3 := StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
		case 5 :DAVL_DZASL= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :DATA_DZASL= StrToInt(EditOTLzad6->Text) ; break;
//		case 7 := StrToInt(EditOTLzad7->Text) ; break;
//		case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :X_TDZASL= StrToInt(EditOTLzad9->Text) ; break;
		case 10:VRDZASL= StrToInt(EditOTLzad10->Text); break;
		case 11:E_TDZASL= StrToInt(EditOTLzad11->Text); break;
		case 12:DELDZASL= StrToInt(EditOTLzad12->Text); break;
		case 13:LIM1DZASL= StrToInt(EditOTLzad13->Text); break;
		case 14:LIM2DZASL= StrToInt(EditOTLzad14->Text); break;
		case 15:DOPDZASL= StrToInt(EditOTLzad15->Text); break;
//		case 16:= StrToInt(EditOTLzad16->Text); break;
		case 17:KOM_DZASL= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
//		case 20:= StrToInt(EditOTLzad20->Text); break;
//		case 21:= StrToInt(EditOTLzad21->Text); break;
		case 22:CT_DZASL= StrToInt(EditOTLzad22->Text); break;
		case 23:T_KDZASL= StrToInt(EditOTLzad23->Text); break;
		case 24:T_VRDZASL= StrToInt(EditOTLzad24->Text); break;
//		case 25:= StrToInt(EditOTLzad25->Text); break;
		case 26:PAR_DZASL= StrToInt(EditOTLzad26->Text); break;
		case 27:ZPAR_DZASL= StrToInt(EditOTLzad27->Text); break;
//		case 28:= StrToInt(EditOTLzad28->Text); break;
		case 29:TEK_DAVL_DZASL= StrToInt(EditOTLzad29->Text); break;
		case 30:TEK_POZ_DZASL= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //26 страница
case 26:
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
     //27 страница
case 27:
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
     //28 страница

case 28:
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
//		case 17:= StrToInt(EditOTLzad17->Text); break;
//		case 18:= StrToInt(EditOTLzad18->Text); break;
//		case 19:= StrToInt(EditOTLzad19->Text); break;
		case 20:KOM_POD= StrToInt(EditOTLzad20->Text); break;
		case 21:OTVET_POD= StrToInt(EditOTLzad21->Text); break;
		case 22:V_POD= StrToInt(EditOTLzad22->Text); break;
		case 23:TYPE_POD= StrToInt(EditOTLzad23->Text); break;
//		case 24:= StrToInt(EditOTLzad24->Text); break;
		case 25:PR_POD= StrToInt(EditOTLzad25->Text); break;
		case 26:HOME_POD= StrToInt(EditOTLzad26->Text); break;
//		case 27:= StrToInt(EditOTLzad27->Text); break;
		case 28:PUT_POD= StrToInt(EditOTLzad28->Text); break;
		case 29:TEK_ABS_POD= StrToInt(EditOTLzad29->Text); break;
		case 30:TEK_OTN_POD= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //29 страница
case 29:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :VRPD= StrToInt(EditOTLzad1->Text) ; break;
//		case 2 := StrToInt(EditOTLzad2->Text) ; break;
		case 3 :prMVPvRabPol= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :pderjCounter= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :pderjInIsh= StrToInt(EditOTLzad5->Text) ; break;
//		case 6 := StrToInt(EditOTLzad6->Text) ; break;
		case 7 :PDVmin= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :PDVmax= StrToInt(EditOTLzad8->Text) ; break;
//		case 9 := StrToInt(EditOTLzad9->Text) ; break;
		case 10:tkPderjIsh= StrToInt(EditOTLzad10->Text); break;
		case 11:tkPderjDvij= StrToInt(EditOTLzad11->Text); break;
		case 12:tkPderjRazgon= StrToInt(EditOTLzad12->Text); break;
		case 13:ctPderjDvij= StrToInt(EditOTLzad13->Text); break;
		case 14:ctPderjCheck_0= StrToInt(EditOTLzad14->Text); break;
		case 15:ctPderjCheck_1= StrToInt(EditOTLzad15->Text); break;
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
     //30 страница
case 30:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :VRUN= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :PAR_UN= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :X_TUN= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :E_TUN= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :DELUN= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :E_PUN= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :K_PUN= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :K_IUN= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :U_PUN= StrToInt(EditOTLzad9->Text) ; break;
		case 10:A_VIH= StrToInt(EditOTLzad10->Text); break;
//		case 11:= StrToInt(EditOTLzad11->Text); break;
		case 12:LIMPUN= StrToInt(EditOTLzad12->Text); break;
		case 13:LIMIUN= StrToInt(EditOTLzad13->Text); break;
		case 14:LIM1UN= StrToInt(EditOTLzad14->Text); break;
		case 15:LIM2UN= StrToInt(EditOTLzad15->Text); break;
		case 16:LIMUUN= StrToInt(EditOTLzad16->Text); break;
		case 17:LIMU_UN= StrToInt(EditOTLzad17->Text); break;
		case 18:LIMUPR_UN= StrToInt(EditOTLzad18->Text); break;
		case 19:PORCNV_UN= StrToInt(EditOTLzad19->Text); break;
		case 20:PORCPR_UN= StrToInt(EditOTLzad20->Text); break;
		case 21:PROBUN= StrToInt(EditOTLzad21->Text); break;
//		case 22:= StrToInt(EditOTLzad22->Text); break;
		case 23:T_VRUN= StrToInt(EditOTLzad23->Text); break;
		case 24:T_KUN= StrToInt(EditOTLzad24->Text); break;
		case 25:T_VREJ_UN= StrToInt(EditOTLzad25->Text); break;
		case 26:T_VPRB_UN= StrToInt(EditOTLzad26->Text); break;
		case 27:T_REQUN= StrToInt(EditOTLzad27->Text); break;
		case 28:CT_VRUN= StrToInt(EditOTLzad28->Text); break;
		case 29:CT_PR_UN= StrToInt(EditOTLzad29->Text); break;
		case 30:CT_REQUN= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //31 страница
case 31:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :VRBM1= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :PR_SV_BM1= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_NAP1= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :UST_BM1= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :X_TBM1= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :E_TBM1= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :DELBM1= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :DOPBM1= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :PAR_BM1= StrToInt(EditOTLzad9->Text) ; break;
		case 10:LIM1BM1= StrToInt(EditOTLzad10->Text); break;
		case 11:LIM2BM1= StrToInt(EditOTLzad11->Text); break;
		case 12:T_VRBM= StrToInt(EditOTLzad12->Text); break;
		case 13:T_KBM= StrToInt(EditOTLzad13->Text); break;
		case 14:VRBM2= StrToInt(EditOTLzad14->Text); break;
		case 15:PR_SV_BM2= StrToInt(EditOTLzad15->Text); break;
		case 16:PR_NAP2= StrToInt(EditOTLzad16->Text); break;
		case 17:UST_BM2= StrToInt(EditOTLzad17->Text); break;
		case 18:X_TBM2= StrToInt(EditOTLzad18->Text); break;
		case 19:E_TBM2= StrToInt(EditOTLzad19->Text); break;
		case 20:DELBM2= StrToInt(EditOTLzad20->Text); break;
		case 21:DOPBM2= StrToInt(EditOTLzad21->Text); break;
		case 22:PAR_BM2= StrToInt(EditOTLzad22->Text); break;
		case 23:LIM1BM2= StrToInt(EditOTLzad23->Text); break;
		case 24:LIM2BM2= StrToInt(EditOTLzad24->Text); break;
		case 25:T_KOTS_PROB= StrToInt(EditOTLzad25->Text); break;
		case 26:PR_KZ1= StrToInt(EditOTLzad26->Text); break;
		case 27:N_KZ1= StrToInt(EditOTLzad27->Text); break;
		case 28:PR_KZ2= StrToInt(EditOTLzad28->Text); break;
		case 29:N_KZ2= StrToInt(EditOTLzad29->Text); break;
		case 30:N_PROB= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //32 страница
case 32:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :VRII= StrToInt(EditOTLzad1->Text) ; break;
//		case 2 := StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_SV_II= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :X_TII= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :E_TII= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :DELII= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :DOPII= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :PAR_II= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :LIM1II= StrToInt(EditOTLzad9->Text) ; break;
		case 10:LIM2II= StrToInt(EditOTLzad10->Text); break;
		case 11:T_VRII= StrToInt(EditOTLzad11->Text); break;
		case 12:CT_II= StrToInt(EditOTLzad12->Text); break;
		case 13:TK_OJ_OTV= StrToInt(EditOTLzad13->Text); break;
		case 14:T_KII= StrToInt(EditOTLzad14->Text); break;
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
     //33 страница
case 33:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :OTVET_BM1[0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :OTVET_BM1[1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :OTVET_BM1[2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :OTVET_BM1[3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :OTVET_BM1[4]= StrToInt(EditOTLzad5->Text) ; break;
		case 6 :OTVET_BM1[5]= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :OTVET_BM1[6]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :OTVET_BM1[7]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :OTVET_BM1[8]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:OTVET_BM1[9]= StrToInt(EditOTLzad10->Text); break;
		case 11:OTVET_BM2[0]= StrToInt(EditOTLzad11->Text); break;
		case 12:OTVET_BM2[1]= StrToInt(EditOTLzad12->Text); break;
		case 13:OTVET_BM2[2]= StrToInt(EditOTLzad13->Text); break;
		case 14:OTVET_BM2[3]= StrToInt(EditOTLzad14->Text); break;
		case 15:OTVET_BM2[4]= StrToInt(EditOTLzad15->Text); break;
		case 16:OTVET_BM2[5]= StrToInt(EditOTLzad16->Text); break;
		case 17:OTVET_BM2[6]= StrToInt(EditOTLzad17->Text); break;
		case 18:OTVET_BM2[7]= StrToInt(EditOTLzad18->Text); break;
		case 19:OTVET_BM2[8]= StrToInt(EditOTLzad19->Text); break;
		case 20:OTVET_BM2[9]= StrToInt(EditOTLzad20->Text); break;
		case 21:OTVET_II[0]= StrToInt(EditOTLzad21->Text); break;
		case 22:OTVET_II[1]= StrToInt(EditOTLzad22->Text); break;
		case 23:OTVET_II[2]= StrToInt(EditOTLzad23->Text); break;
		case 24:OTVET_II[3]= StrToInt(EditOTLzad24->Text); break;
		case 25:OTVET_II[4]= StrToInt(EditOTLzad25->Text); break;
		case 26:OTVET_II[5]= StrToInt(EditOTLzad26->Text); break;
		case 27:OTVET_II[6]= StrToInt(EditOTLzad27->Text); break;
		case 28:OTVET_II[7]= StrToInt(EditOTLzad28->Text); break;
		case 29:OTVET_II[8]= StrToInt(EditOTLzad29->Text); break;
		case 30:OTVET_II[9]= StrToInt(EditOTLzad30->Text); break;
	}
}; break;
     //34 страница
case 34:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :KOM_BM1[0]= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :KOM_BM1[1]= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :KOM_BM1[2]= StrToInt(EditOTLzad3->Text) ; break;
		case 4 :KOM_BM1[3]= StrToInt(EditOTLzad4->Text) ; break;
		case 5 :KOM_BM1[4]= StrToInt(EditOTLzad5->Text) ; break;
//		case 6 := StrToInt(EditOTLzad6->Text) ; break;
		case 7 :KOM_BM2[0]= StrToInt(EditOTLzad7->Text) ; break;
		case 8 :KOM_BM2[1]= StrToInt(EditOTLzad8->Text) ; break;
		case 9 :KOM_BM2[2]= StrToInt(EditOTLzad9->Text) ; break;
		case 10:KOM_BM2[3]= StrToInt(EditOTLzad10->Text); break;
		case 11:KOM_BM2[4]= StrToInt(EditOTLzad11->Text); break;
//		case 12:= StrToInt(EditOTLzad12->Text); break;
		case 13:KOM_II[0]= StrToInt(EditOTLzad13->Text); break;
		case 14:KOM_II[1]= StrToInt(EditOTLzad14->Text); break;
		case 15:KOM_II[2]= StrToInt(EditOTLzad15->Text); break;
		case 16:KOM_II[3]= StrToInt(EditOTLzad16->Text); break;
		case 17:KOM_II[4]= StrToInt(EditOTLzad17->Text); break;
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
     //35 страница
case 35:
{
switch (StrToInt(((TButton*)Sender)->Hint))
	{
		case 1 :PR_KN= StrToInt(EditOTLzad1->Text) ; break;
		case 2 :PR_PER0_KN= StrToInt(EditOTLzad2->Text) ; break;
		case 3 :PR_SV_KN= StrToInt(EditOTLzad3->Text) ; break;
//		case 4 := StrToInt(EditOTLzad4->Text) ; break;
	  //	case 5 := StrToInt(EditOTLzad5->Text) ; break;
		case 6 :KOM_KN= StrToInt(EditOTLzad6->Text) ; break;
		case 7 :OTVET_KN= StrToInt(EditOTLzad7->Text) ; break;
	 //	case 8 := StrToInt(EditOTLzad8->Text) ; break;
		case 9 :CT_KN= StrToInt(EditOTLzad9->Text) ; break;
	//	case 10:= StrToInt(EditOTLzad10->Text); break;
		case 11:OTVET_KN_M[0]= StrToInt(EditOTLzad11->Text); break;
		case 12:OTVET_KN_M[1]= StrToInt(EditOTLzad12->Text); break;
		case 13:OTVET_KN_M[2]= StrToInt(EditOTLzad13->Text); break;
		case 14:OTVET_KN_M[3]= StrToInt(EditOTLzad14->Text); break;
		case 15:OTVET_KN_M[4]= StrToInt(EditOTLzad15->Text); break;
		case 16:OTVET_KN_M[5]= StrToInt(EditOTLzad16->Text); break;
		case 17:OTVET_KN_M[6]= StrToInt(EditOTLzad17->Text); break;
		case 18:OTVET_KN_M[7]= StrToInt(EditOTLzad18->Text); break;
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
        // визуализация даты-времени

        Label_Time -> Caption = TimeToStr(Time());
        Label_Date -> Caption = DateToStr(Date());

        // время контура логики
        Form1 -> EditTLogic -> Text = FloatToStrF(logic_time,ffFixed,6,3);

        // если время меньше 10:00 добавим спереди 0 для красоты
        if ( Label_Time -> Caption.Length() == 7 )
        {
            Label_Time -> Caption = "0" + Label_Time -> Caption;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1secTimer(TObject *Sender)
{
    ComTimers();   // инкремент счётчиков Com при ошибках

    VisualGraph();  //  визуализаия графиков

    // рассчет ресурсов магнетронов
    // визуализация и расчёт ресурса магнетрона 1 поз 3
    if ( shr[35] && ( (OTVET_BM1[6] * 3072.0 / 1023.0 ) > 100 ))
        magnRes1 += StrToFloat((float)OTVET_BM1[6]/1023.0*3072.0/3600.0/1000.0);
    EditRESm13->Text= FloatToStrF(magnRes1,ffFixed,6,3);
    // визуализация и расчёт ресурса магнетрона 2 поз 3
    if ( shr[36] && ( (OTVET_BM2[6] * 3072.0 / 1023.0 ) > 100 ))
        magnRes2 += StrToFloat((float)OTVET_BM2[6]/1023.0*3072.0/3600.0/1000.0);
    EditRESm23->Text= FloatToStrF(magnRes2,ffFixed,6,3);
}
//---------------------------------------------------------------------------
//--Визуализация параметров механизмов--//
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

    par_t[1]  = StrToInt(EdtTRed1->Text);
    par_t[2]  = StrToInt(EdtTRed2->Text);
    par_t[3]  = StrToInt(EdtTRed3->Text);
    par_t[4]  = StrToInt(EdtTRed4->Text);
    par_t[5]  = StrToInt(EdtTRed5->Text);

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add(Label_Time -> Caption + "  Переданы параметры механизмов:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    if ( EdtTKon1 -> Text != EdtTRed1 -> Text )
        MemoStat -> Lines -> Add("Путь манипулятора от исходного датчика (HOME) до переукладки пластины в кассете: " + EdtTKon1 -> Text + " -> " + EdtTRed1 -> Text );
    if ( EdtTKon2 -> Text != EdtTRed2 -> Text )
        MemoStat -> Lines -> Add("Путь манипулятора от исходного датчика (HOME) до переукладки пластины в камере: " + EdtTKon2 -> Text + " -> " + EdtTRed2 -> Text );
    if ( EdtTKon3 -> Text != EdtTRed3 -> Text )
        MemoStat -> Lines -> Add("Угол поворота манипулятора от исходного датчика (HOME) до переукладки пластины в кассете: " + EdtTKon3 -> Text + " -> " + EdtTRed3 -> Text );
    if ( EdtTKon4 -> Text != EdtTRed4 -> Text )
        MemoStat -> Lines -> Add("Шаг кассеты: " + EdtTKon4 -> Text + " -> " + EdtTRed4 -> Text );
    if ( EdtTKon5 -> Text != EdtTRed5 -> Text )
        MemoStat -> Lines -> Add("Путь кассеты для переукладки пластины: " + EdtTKon5 -> Text + " -> " + EdtTRed5 -> Text );

    // перекрасить переданные параметры
    EdtTRed1 -> Color = clWhite;
    EdtTRed2 -> Color = clWhite;
	EdtTRed3 -> Color = clWhite;
	EdtTRed4 -> Color = clWhite;
	EdtTRed5 -> Color = clWhite;

    // обновить страницу
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
{   // визуализация механизмов и их путей
    // перемещение
    if(!ust_ready) return;

    //Подъём п/д
    if(TEK_ABS_POD<0)
        p_pd->Top = 246;
    else if(TEK_ABS_POD>nasmod[16])
        p_pd->Top = 246-50;
    else
        p_pd->Top = 246 - int(50.0*float(TEK_ABS_POD)/float(nasmod[16]));

    datch_up->Top =p_pd->Top+20;
    datch_up->Height=271-datch_up->Top;

    tube_up->Top =p_pd->Top;
    tube_up->Height=293-tube_up->Top;

    pd->Top = p_pd->Top+2;

    anim->Top =p_pd->Top-23;
    //подъёмник
    switch(zin[1]&0xC0)
    {
        case 0x00:{ pp->Top=p_pd->Top;pp-> Picture->Bitmap =pp_n-> Picture->Bitmap;break;}
        case 0x40:{ pp->Top=p_pd->Top-9;pp-> Picture->Bitmap =pp_home-> Picture->Bitmap;break;}
        case 0x80:{ pp->Top=p_pd->Top+2;pp-> Picture->Bitmap =pp_home-> Picture->Bitmap;break;}
        case 0xC0:{ pp->Top=p_pd->Top;pp-> Picture->Bitmap =pp_n-> Picture->Bitmap;break;}
    }

    // Заданные пути
    Edt_AZ_1_1mn -> Text = IntToStr(par[0][10]);
    // Абсолютные пути
    Edt_AZ_1_2mn -> Text = IntToStr(TEK_ABS_POD);
    // Относительные пути
    Edt_AZ_1_3mn -> Text = IntToStr(TEK_OTN_POD);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_OptClick(TObject *Sender)
{
  // При сохранении параметров
  // скрываем вкладки
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

  // сохраням данные и выходим, очищая поля на всякий
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
  // Ввод текущего пароля для смены
  if((Key == 8) && (Vtekpas_str.Length() > 0)) // бэкспэйс
  {
       Vtekpas_str[Vtekpas_str.Length()] = 0;     // стираем последний символ в скрытой строке
       Vtekpas_str.SetLength(Vtekpas_str.Length() - 1);
  }
  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vtekpas_str += Key; // добавляем символ с скрытую строку
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit_Acc_NewPas1KeyPress(TObject *Sender,
      char &Key)
{
  // Ввод нового пароля 1 для смены
  if((Key == 8) && (Vnew1pas_str.Length() > 0)) // бэкспэйс
  {
       Vnew1pas_str[Vnew1pas_str.Length()] = 0;     // стираем последний символ в скрытой строке
       Vnew1pas_str.SetLength(Vnew1pas_str.Length() - 1);
  }
  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vnew1pas_str += Key; // добавляем символ с скрытую строку
  }    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit_Acc_NewPas2KeyPress(TObject *Sender,
      char &Key)
{
  // Ввод нового пароля 2 для смены
  if((Key == 8) && (Vnew2pas_str.Length() > 0)) // бэкспэйс
  {
       Vnew2pas_str[Vnew2pas_str.Length()] = 0;     // стираем последний символ в скрытой строке
       Vnew2pas_str.SetLength(Vnew2pas_str.Length() - 1);
  }
  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vnew2pas_str += Key; // добавляем символ с скрытую строку
  }    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_NewPasClick(TObject *Sender)
{
   // Обрабатываем кнопку сохранения пароля
   if((Vtekpas_str != Edit_Acc_DefPas->Text)&&((Vtekpas_str != Edit_Acc_UserPas->Text)||(Edit_Acc_UserPas->Text == "")))
   {
        Vtekpas_str = "";
        Vnew1pas_str = "";
        Vnew2pas_str = "";
        // Высвечиваем окно "Текущий пароль введен не верно"
        MessageBox(NULL, "Текущий пароль введен не верно", "Ошибка" , MB_OK | MB_ICONERROR);
        return;
   }
   if(Vnew1pas_str != Vnew2pas_str)
   {
        Vtekpas_str = "";
        Vnew1pas_str = "";
        Vnew2pas_str = "";
        // Высвечиваем окно "Введенные пароли не совпадают"
        MessageBox(NULL, "Введенные пароли не совпадают", "Ошибка" , MB_OK | MB_ICONERROR);
        return;
   }
   if((Vnew1pas_str.Length()<5)||(Vnew1pas_str.Length()>20))
   {
        Vtekpas_str = "";
        Vnew1pas_str = "";
        Vnew2pas_str = "";
         // Высвечиваем окно "Длина пароля не удовлетворяет требованиям"
        MessageBox(NULL, "Длина пароля не удовлетворяет требованиям", "Ошибка" , MB_OK | MB_ICONERROR);
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

   // Высвечиваем окно "Новый пароль установлен"
   MessageBox(NULL, "Новый пароль установлен", "Подтверждение" , MB_OK );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Edit_Acc_VPasKeyPress(TObject *Sender,
      char &Key)
{
  // Ввод пароля для входа
  if((Key == 8) && (Vpas_str.Length() > 0)) // бэкспэйс
  {
       Vpas_str[Vpas_str.Length()] = 0;     // стираем последний символ в скрытой строке
       Vpas_str.SetLength(Vpas_str.Length() - 1);
  }

  else if(Key == 13) // ввод
  {
        // запускаем обработку
        But_Acc_VPasClick(But_Acc_VPas);
  }

  else if(
        ((Key >= 48)&&(Key <= 57)) ||
        ((Key >= 65)&&(Key <= 90)) ||
        ((Key >= 97)&&(Key <= 122)) ||
        (((Key&0xFF) >= 192)&&((Key&0xFF) <= 255))
          )
  {
     Vpas_str += Key; // добавляем символ с скрытую строку
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::But_Acc_VPasClick(TObject *Sender)
{
        // Проверяем пароль
        // сначала наш
  if((Edit_Acc_DefPas->Text == Vpas_str)||((Edit_Acc_UserPas->Text != "")&&(Edit_Acc_UserPas->Text == Vpas_str)))
        {
                Vpas_str = "";
                Pnl_Acc_Pas -> Visible = false;
                Pnl_Acc_Opt -> Visible = true;
                Pnl_Acc_SetPas -> Visible = true;
        }
  else
  {
        // Высвечиваем окно "Пароль введен не верно"
        MessageBox(NULL, "Пароль введен не верно!", "Ошибка" , MB_OK | MB_ICONERROR);
                Vpas_str = "";
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer_AccTimer(TObject *Sender)
{
 // Таймер для вывода звездочек в поля ввода паролей
    // закрытие звездочками вводимого пароля
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
    // закрытие звездочками текущего пароля
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
    // закрытие звездочками нового пароля 1
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
    // закрытие звездочками нового пароля 2
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
	else if(!im0) { MessageBox(NULL, "Невозможно записать данные", "Ошибка", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
void TForm1::LoadData2()
{
	int SizeOfIniFile=(int)sizeof(iniPAS);

	if(!DirectoryExists("Data")) { CreateDir("Data"); }
	FILE *im0;
	im0=fopen(loc_acc_udb,"rb");
	if(im0)       { fread(&iniPAS,SizeOfIniFile,1,im0); fclose(im0); }
	else if(!im0) { MessageBox(NULL, "Невозможно загрузить данные", "Ошибка", MB_OK | MB_ICONSTOP); }
}
//---------------------------------------------------------------------------
























void __fastcall TForm1::CB_StChangeChange(TObject *Sender)
{
    switch(((TComboBox*)Sender)->ItemIndex)
    {
        case 0:
        {
            Pnl_St4->Visible=true;
            Pnl_St5->Visible=false;
            Pnl_St6->Visible=false;
            Pnl_St7->Visible=false;
            Pnl_St8->Visible=false;
            Pnl_St9->Visible=false;
        }break;
        case 1:
        {
            Pnl_St4->Visible=false;
            Pnl_St5->Visible=true;
            Pnl_St6->Visible=false;
            Pnl_St7->Visible=false;
            Pnl_St8->Visible=false;
            Pnl_St9->Visible=false;
        }break;
        case 2:
        {
            Pnl_St4->Visible=false;
            Pnl_St5->Visible=false;
            Pnl_St6->Visible=true;
            Pnl_St7->Visible=false;
            Pnl_St8->Visible=false;
            Pnl_St9->Visible=false;
        }break;
        case 3:
        {
            Pnl_St4->Visible=false;
            Pnl_St5->Visible=false;
            Pnl_St6->Visible=false;
            Pnl_St7->Visible=true;
            Pnl_St8->Visible=false;
            Pnl_St9->Visible=false;
        }break;
        case 4:
        {
            Pnl_St4->Visible=false;
            Pnl_St5->Visible=false;
            Pnl_St6->Visible=false;
            Pnl_St7->Visible=false;
            Pnl_St8->Visible=true;
            Pnl_St9->Visible=false;
        }break;
        case 5:
        {
            Pnl_St4->Visible=false;
            Pnl_St5->Visible=false;
            Pnl_St6->Visible=false;
            Pnl_St7->Visible=false;
            Pnl_St8->Visible=false;
            Pnl_St9->Visible=true;
        }break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnTrDaClick(TObject *Sender)
{
     // панель подтверждения отправки убрать
    PanelParTr -> Visible = false;
    //N_ST=10 ------------------------------------------------------------------------------

    par[10][4]=(3.5+log10(StrToFloat(EdtARed10_4->Text)))*1000.0;//давление по РРТ
    par[10][6]=StrToFloat  ( EdtARed10_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[10][9]=StrToFloat  ( EdtARed10_9->Text )*10000.0/100.0;                      //процент открытия дз
    par[10][12]=StrToInt   ( EdtARed10_12->Text );                          //премя процесса
    //N_ST=11
    par[11][4]=(3.5+log10(StrToFloat(EdtARed11_4->Text)))*1000.0;//давление по РРТ
    par[11][7]=StrToFloat  ( EdtARed11_7-> Text) * 4095.0 / 3600;           //Мощность М2
    par[11][9]=StrToFloat  ( EdtARed11_9->Text )*10000.0/100.0;                      //процент открытия дз
    par[11][12]=StrToInt    ( EdtARed11_12->Text );                          //премя процесса
    //N_ST=12
    par[12][4]=(3.5+log10(StrToFloat(EdtARed12_4->Text)))*1000.0;//давление по РРТ
    par[12][6]=StrToFloat  ( EdtARed12_6-> Text) * 4095.0 / 3600;           //Мощность М1
    par[12][7]=StrToFloat  ( EdtARed12_7-> Text) * 4095.0 / 3600;           //Мощность М2
    par[12][9]=StrToFloat  ( EdtARed12_9->Text )*10000.0/100.0;                      //процент открытия дз
    par[12][12]=StrToInt   ( EdtARed12_12->Text );                          //премя процесса





    MemoStat -> Lines -> Add(Label_Time -> Caption + "Переданы параметры тренировки:");

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Тренировка М1:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //10 стадия
    if ( EdtAKon10_4 -> Text != EdtARed10_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon10_4 -> Text + " -> " + EdtARed10_4 -> Text );
    if ( EdtAKon10_6 -> Text != EdtARed10_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon10_6 -> Text + " -> " + EdtARed10_6 -> Text );
    if ( EdtAKon10_9 -> Text != EdtARed10_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ: " + EdtAKon10_9 -> Text + " -> " + EdtARed10_9 -> Text );
    if ( EdtAKon10_12 -> Text != EdtARed10_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon10_12 -> Text + " -> " + EdtARed10_12 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Тренировка М2:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //11 стадия
    if ( EdtAKon11_4 -> Text != EdtARed11_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon11_4 -> Text + " -> " + EdtARed11_4 -> Text );
    if ( EdtAKon11_7 -> Text != EdtARed11_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon11_7 -> Text + " -> " + EdtARed11_7 -> Text );
    if ( EdtAKon11_9 -> Text != EdtARed11_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ: " + EdtAKon11_9 -> Text + " -> " + EdtARed11_9 -> Text );
    if ( EdtAKon11_12 -> Text != EdtARed11_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon11_12 -> Text + " -> " + EdtARed11_12 -> Text );

    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");
    MemoStat -> Lines -> Add("Тренировка М1 и М2:");
    MemoStat -> Lines -> Add("------------------------------------------------------------------------------------------------------------------------------------");

    //12 стадия
    if ( EdtAKon12_4 -> Text != EdtARed12_4 -> Text )
        MemoStat -> Lines -> Add("Давление: " + EdtAKon12_4 -> Text + " -> " + EdtARed12_4 -> Text );
    if ( EdtAKon12_6 -> Text != EdtARed12_6 -> Text )
        MemoStat -> Lines -> Add("Мощность М1: " + EdtAKon12_6 -> Text + " -> " + EdtARed12_6 -> Text );
    if ( EdtAKon12_7 -> Text != EdtARed12_7 -> Text )
        MemoStat -> Lines -> Add("Мощность М2: " + EdtAKon12_7 -> Text + " -> " + EdtARed12_7 -> Text );
    if ( EdtAKon12_9 -> Text != EdtARed12_9 -> Text )
        MemoStat -> Lines -> Add("Процент открытия ДЗ: " + EdtAKon12_9 -> Text + " -> " + EdtARed12_9 -> Text );
    if ( EdtAKon12_12 -> Text != EdtARed12_12 -> Text )
        MemoStat -> Lines -> Add("Время процесса: " + EdtAKon12_12 -> Text + " -> " + EdtARed12_12 -> Text );

    // перекрасить переданные параметры
    EdtARed10_4 -> Color = clWhite;
    EdtARed10_6 -> Color = clWhite;
    EdtARed10_9 -> Color = clWhite;
    EdtARed10_12 -> Color = clWhite;

    EdtARed11_4 -> Color = clWhite;
    EdtARed11_7 -> Color = clWhite;
    EdtARed11_9 -> Color = clWhite;
    EdtARed11_12 -> Color = clWhite;

    EdtARed12_4 -> Color = clWhite;
    EdtARed12_6 -> Color = clWhite;
    EdtARed12_7 -> Color = clWhite;
    EdtARed12_9 -> Color = clWhite;
    EdtARed12_12 -> Color = clWhite;
    // обновить страницу
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
    // общий обработчик для двух клавиш диалога,
    // "ДА" имеет hint = Сбросить
    if ( ((TButton*)Sender)->Hint == "Сбросить" )
        if ( ((TButton*)Sender)->Name == "BtnYesMagn13Res" )
            magnRes1 = 0;
        else if ( ((TButton*)Sender)->Name == "BtnYesMagn23Res" )
            magnRes2 = 0;

    // в обоих случаях панель диалога закрываем
    ((TButton*)Sender) -> Parent -> Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BtnSbrosMagn13ResClick(TObject *Sender)
{
    // сброс ресурса магнетронов
   if ( ((TButton*)Sender)->Hint == "1" )
        PnlRES13danet -> Visible = true;
   else if ( ((TButton*)Sender)->Hint == "2" )
        PnlRES23danet -> Visible = true;

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








